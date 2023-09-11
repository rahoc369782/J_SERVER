#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../../status.h"
#include "../../utils/memory/p_memory.h"
#include "../../utils/common/customstring.h"

/*
    Gloabl command collection lookup table pointer.
    Contains all PIE's commands in which is parsed from cmmands.txt.
*/

struct commands_colle *cmds;

static struct inprogress_parsing
{
    struct command *command;
    char last_chunk[FILE_BUF_SIZE_LOCAL];
    int last_pos_in_chunk;
    char last_char;
};

static struct parser_tokens
{
    char *tokens;
};

static void free_all_memory_notinused()
{
    // Free unecessary memory which work is done.
}
static int generate_hash_for_commands_hash(char code)
{
    int partone = 0;
    for (int i = 0; i < code; i++)
    {
        partone += i;
    }
    partone = partone >> 2;
    printf("hash %d is %d\n", partone, partone % MAX_TOKENS);
    return partone % MAX_TOKENS;
}

static void parse_pair(struct inprogress_parsing *track_parsing)
{
    printf("Received key-value is %s and struct is %p\n", track_parsing->last_chunk, track_parsing->command);
}

static struct command *generate_new_command()
{
    struct command *command = pmalloc(sizeof(struct command));
    return command;
}

static void set_commandin_collection(struct command *command)
{
    int position = generate_hash_for_commands_hash(command->code);
    cmds->commands[position] = command;
}

static int initiate_global_commands_collection()
{
    // Initialize global commands lookup table
    int res = EIO;
    cmds = 0;
    cmds = pmalloc(sizeof(struct commands_colle));
    if (cmds == NULL)
    {
        res = -EIO;
        goto out;
    }
    struct command **commands_arr = pmalloc(MAX_COMMAND_HASHTABLE_SIZE * sizeof(struct command));
    if (commands_arr == NULL)
    {
        res = -EIO;
        goto out;
    }
    cmds->commands = commands_arr;
    memset(cmds->commands, 0x00, MAX_COMMAND_HASHTABLE_SIZE * sizeof(cmds->commands));
    return res;
out:
    return res;
}

static int command_reader(char *f_chunk, struct inprogress_parsing *track_parsing)
{
    /*
       Responsibility : Function is responsible for processing received chunks into valuable
       structures of PIE'commands.

       This read chunks and generate new structures for each command also store it in commands_colle
       structure.
   */

    int pos = 0;

    // Pass local buffer which is upto key value pair which we found till encounter '/'
    // Either it will pass bytes we have read.
    // printf("received chunk is %s and previous is %s and p is %p\n",f_chunk , track_parsing->last_chunk, track_parsing->command);

    while (f_chunk[pos] != '\0')
    {
        // Check for is new command parsing started with '{' character
        if (f_chunk[pos] == '{')
        {
            // Command is ended we need to switch previous tracking parsing command struct with new in progress command struct
            struct command *new_command = generate_new_command();
            if (!new_command)
            {
                return -EINMEM;
            }

            // Set last chunk to empty because we have completed current command parsing.
            memset(track_parsing->last_chunk, 0, FILE_BUF_SIZE_LOCAL);
            track_parsing->command = new_command;
            track_parsing->last_pos_in_chunk = 0;
            pos++;
            continue;
        }

        // Check for is command parsing ended with '}' character
        if (f_chunk[pos] == '}')
        {
            pos++;
            continue;
        }

        // Check for empty space and line terminator if found then remove it.

        if (f_chunk[pos] == 10)
        {
            pos++;
            continue;
        }

        // Check for key value pair which is ending with ','. If encounter ',' then simply pass to parse_pair()
        // for parsing and setting in WIP command struct.
        if (f_chunk[pos] == ',')
        {
            // printf("sent chunk is %s\n",track_parsing->last_chunk);
            track_parsing->last_chunk[track_parsing->last_pos_in_chunk + 1] = 0;

            // Calling for parsing last_chunk key-value pair and setting in new generated command struct.
            parse_pair(track_parsing);

            // Set last chunk to empty because we have detected entire key value and completed its parsing.
            memset(track_parsing->last_chunk, 0, FILE_BUF_SIZE_LOCAL);
            pos++;
            track_parsing->last_pos_in_chunk = 0;
            continue;
        }
        track_parsing->last_chunk[track_parsing->last_pos_in_chunk] = f_chunk[pos];
        pos++;
        track_parsing->last_pos_in_chunk++;
    }
}

int commands_parser()
{
    char tokens[MAX_TOKENS] = {'{', '}', ':', ',', '|'};
    struct parser_tokens *tokens_table = pmalloc(sizeof(struct parser_tokens));
    tokens_table->tokens = tokens;
    int cmd_fd = open(CMD_CONFIG, O_RDONLY);

    // Exit process if we failed to read CMD config file
    if (cmd_fd < 0)
    {
        perror("Error opening file");
        return -CMD_FILE;
    }

    /*
        Read data from the file with buff size of FILE_BUF_SIZE
        Looping through it for reading in and parsing it in chunks.
    */
    initiate_global_commands_collection();
    // struct command *cmd = pmalloc(sizeof(struct command));
    // cmd->code = 'S';
    // set_commandin_collection(cmd);

    // Global declaration for tracking parsing propely
    struct inprogress_parsing *track_parsing;
    track_parsing = pmalloc(sizeof(struct inprogress_parsing));
    if (!track_parsing)
    {
        return -EINMEM;
    }
    ssize_t bytes = 1; // Number of bytes read
    while (bytes > 0)
    {
        char chunk_buf[FILE_BUF_SIZE]; // Buffer to store read data
        bytes = read(cmd_fd, chunk_buf, FILE_BUF_SIZE);

        /*
            Check if process is unable to read file.If not exit the process with vaalid message.
        */
        if (bytes < 0)
        {
            perror("Error while reading from cmd file");
            close(cmd_fd);
            return -CMD_FILE;
        }

        if (bytes == 0)
        {
            printf("CMD file completed");
            close(cmd_fd);
            break;
        }

        /*
            If data is available to process just process it with below function.
        */
        // chunk_buf[FILE_BUF_SIZE] = 0;
        command_reader(chunk_buf, track_parsing);
    }
    // Close the file
    close(cmd_fd);
    return CMD_FILE;
}