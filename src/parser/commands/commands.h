#ifndef COMMANDS_H
#define COMMANDS_H

#define CMD_CONFIG "src/pie_static/command.txt"
#define FILE_BUF_SIZE 10 //  For adding null terminoator manually
#define MAX_COMMAND_HASHTABLE_SIZE 5

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