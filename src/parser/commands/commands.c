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

struct inprogress_parsing 
{
    struct command* command;
    char status;
};

static int generate_hash_for_commands_hash(char code)
{
    int partone = code & (code - 32);

    printf("hash %d is %d\n",partone,partone % MAX_COMMAND_HASHTABLE_SIZE);
    return partone % MAX_COMMAND_HASHTABLE_SIZE;
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

static void command_reader(char *f_chunk)
{
    /*
       Responsibility : Function is responsible for processing received chunks into valuable
       structures of PIE'commands.

       This read chunks and generate new structures for each command also store it in commands_colle
       structure.
   */
    
    /*
       Process bytes
    */
    int pos = 0;
    int l_pos = 0;
    
    char local_buf[FILE_BUF_SIZE];

    while (f_chunk > 0)
    {
        if (f_chunk[pos] == '/')
        {
            printf("first / encountered at %d = ", pos);
            printf("%s\n", local_buf);
            memset(local_buf, 0, sizeof(local_buf));
            pos++;
            l_pos = 0;  
            continue;
        }
        local_buf[l_pos] = f_chunk[pos];
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
    struct command *cmd = pmalloc(sizeof(struct command));
    cmd->code = 'S';
    struct command *cmd2 = pmalloc(sizeof(struct command));
    cmd2->code = 'A';
    struct command *cmd3 = pmalloc(sizeof(struct command));
    cmd3->code = 'F';
    struct command *cmd4 = pmalloc(sizeof(struct command));
    cmd4->code = 'D';
    set_commandin_collection(cmd4);
    set_commandin_collection(cmd3);
    set_commandin_collection(cmd);
    set_commandin_collection(cmd2);
    set_commandin_collection(cmd2);
    printf("set command is %c", cmds->commands[4]->code);
    ssize_t bytes; // Number of bytes read
    while (1)
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
        // command_reader(chunk_buf);
        printf("Read %zd bytes from the file:\n%s\n", bytes, chunk_buf);
    }
    // Close the file
    close(cmd_fd);
    return CMD_FILE;
}