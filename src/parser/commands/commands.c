#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
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
    struct command* command;
    char last_chunk[FILE_BUF_SIZE];
    short is_started;
};


static int generate_hash_for_commands_hash(char code)
{
    int partone = code & (code - 32);
    printf("hash %d is %d\n",partone,partone % MAX_COMMAND_HASHTABLE_SIZE);
    return partone % MAX_COMMAND_HASHTABLE_SIZE;
}

static void parse_pair(struct inprogress_parsing *track_parsing) {
    printf("Received key-value is %s\n",track_parsing->last_chunk);
}

static struct command* generate_new_command() {
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
    if(cmds == NULL) {
        res = -EIO;
        goto out;
    }
    struct command **commands_arr = pmalloc(MAX_COMMAND_HASHTABLE_SIZE * sizeof(struct command));
    if(commands_arr == NULL)
    {
        res = -EIO;
        goto out;
    }
    cmds->commands = commands_arr;
    memset(cmds->commands,0x00,MAX_COMMAND_HASHTABLE_SIZE * sizeof(cmds->commands));
    return res;
out:
    return res;
}

static int command_reader(char *f_chunk,struct inprogress_parsing* track_parsing)
{
    /*
       Responsibility : Function is responsible for processing received chunks into valuable
       structures of PIE'commands.

       This read chunks and generate new structures for each command also store it in commands_colle
       structure.
   */
    
    int pos = 0;
    int l_pos = 0;
    
    // char local_buf[FILE_BUF_SIZE];

    // Pass local buffer which is upto key value pair which we found till encounter '/'
    // Either it will pass bytes we have read.
    printf("received chunk is %s\n",f_chunk);
    while (f_chunk[pos] != '\0')
    {
        // Check for is new command parsing started with '{' character
        if (f_chunk[pos] == 123)
        {
            // Command is ended we need to switch previous tracking parsing command struct with new in progress command struct
            struct command *new_command = generate_new_command();
            if(!new_command)
            {
                return -EINMEM;
            }

            // Set last chunk to empty because we have completed current command parsing.
            memset(track_parsing->last_chunk, 0, sizeof(FILE_BUF_SIZE));
            track_parsing->command = new_command;
            pos++;
            continue;
        }

        // Check for is command parsing ended with '}' character
        if (f_chunk[pos] == 125)
        {
            continue;
        }

        // Check for key value pair which is ending with ','. If encounter ',' then simply pass to parse_pair()
        // for parsing and setting in WIP command struct.
        if (f_chunk[pos] == 44)
        {

            // Calling for parsing last_chunk key-value pair and setting in new generated command struct.
            parse_pair(track_parsing);

            // Set last chunk to empty because we have detected entire key value and completed its parsing.
            memset(track_parsing->last_chunk, 0, sizeof(FILE_BUF_SIZE));
            pos++;
            l_pos = 0;  
            continue;
        }
        track_parsing->last_chunk[l_pos] = f_chunk[pos];
        pos++;
        l_pos++;
    }
}

int commands_parser()
{

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
    if(!track_parsing)
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
        command_reader(chunk_buf,track_parsing);
    }
    // Close the file
    close(cmd_fd);
    return CMD_FILE;
}