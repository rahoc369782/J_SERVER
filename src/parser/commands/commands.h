#ifndef COMMANDS_H
#define COMMANDS_H

#define CMD_CONFIG "src/pie_static/command.txt"
#define FILE_BUF_SIZE 1024
#define MAX_TOKENS 10
#define FILE_BUF_SIZE_LOCAL (FILE_BUF_SIZE * 10)

#define MAX_COMMAND_HASHTABLE_SIZE 5

#define NODE_TYPE_SPECIAL  0x0001
#define NODE_TYPE_KEY      0X0010
#define NODE_TYPE_VALUE    0x0011

struct command
{
    unsigned char code;
    char *name;
    char *desc;
    char *err;
    short errcode;
    char *succ;
    short succode;
    char secure;
    short fields;
};

struct commands_colle
{
    struct command **commands;
    int obj_counts;
};

int commands_parser();

#endif