#ifndef COMMANDS_H
#define COMMANDS_H

#define CMD_CONFIG "src/pie_static/command.txt"
#define FILE_BUF_SIZE 100
#define MAX_TOKENS 10
#define FILE_BUF_SIZE_LOCAL (FILE_BUF_SIZE * 10)

#define MAX_COMMAND_HASHTABLE_SIZE 10
#define CURRENT_COMMAND_COUNT_SIZE 5

#define NODE_TYPE_SPECIAL_TOKEN  0x0001
#define NODE_TYPE_VALUE          0X0002
#define NODE_TYPE_VALUE2         0x0011

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

struct syntax_status
{
    short code;
    char* msg;
};

int commands_parser();

#endif