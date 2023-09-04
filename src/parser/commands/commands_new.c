#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../../status.h"
#include "../../utils/memory/p_memory.h"
#include "../../utils/common/customstring.h"

struct commands_colle *cmds;

static struct inprogress_parsing
{
    struct command *command;
    char last_chunk[FILE_BUF_SIZE_LOCAL];
};

static struct parser_tokens
{
    char *tokens;
};

// struct which defines type of node , value in commands for better debugging of syntex and as well as
// for better parsing.

struct parsed_list
{
    struct parsed_list *head;
    int length;
};

struct parsed_node
{
    unsigned short type;
    char *value;
    struct parsed_node *next;
    struct parsed_node *prev;
};

static struct command *generate_new_command()
{
    struct command *command = pmalloc(sizeof(struct command));
    return command;
}

int check_token_insymbol_table(struct parser_tokens *token_table, char s)
{
    for (size_t i = 0; i < MAX_TOKENS; i++)
    {
        if (token_table[i] == s)
        {
            return 1;
        }
    };
    return 0;
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

static struct parser_tokens *initiate_tokens_structs()
{
    char *tokens = pmalloc(MAX_TOKENS);
    if (!tokens)
    {
        printf("Error while allocating memory for token table\n");
        return -EINMEM;
    }
    tokens = "{},:|";
    // Memory allocated for tokens table.
    struct parser_tokens *tokens_table = pmalloc(sizeof(struct parser_tokens));
    if (!tokens_table)
    {
        printf("Error while allocating memory for token table\n");
        return -EINMEM;
    }

    tokens_table->tokens = tokens;
    return tokens_table;
}

static struct parsed_list *initiate_parsedlist_struct()
{
    // Memory allocated for parsing linked list which represents all tokens.
    struct parsed_list *parsed_list = pmalloc(sizeof(struct parsed_list));
    if (!parsed_list)
    {
        printf("Error while allocating memory for parsing list.\n");
        return -EINMEM;
    }

    parsed_list->head = 0x00;
    parsed_list->length = 0;
    return parsed_list;
}

int commands_parser()
{

    // File operations
    int cmd_fd = open(CMD_CONFIG, O_RDONLY);

    // Exit process if we failed to read CMD config file
    if (cmd_fd < 0)
    {
        perror("Error opening file");
        return -CMD_FILE;
    }

    // Initiating required parsers.
    struct parser_tokens *parser_tokens = initiate_tokens_structs();
    struct parsed_list *parsing_list = initiate_parsedlist_struct();
    struct inprogress_parsing *track_parsing = pmalloc(sizeof(struct inprogress_parsing));
    if (!track_parsing)
    {
        return -EINMEM;
    }

    /*
        Read data from the file with buff size of FILE_BUF_SIZE
        Looping through it for reading in and parsing it in chunks.
    */

    // initiate_global_commands_collection();

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
