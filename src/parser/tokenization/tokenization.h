#ifndef COMMANDS_H
#define COMMANDS_H

#define CMD_CONFIG "src/pie_static/command.txt"
#define FILE_BUF_SIZE 150
#define MAX_TOKENS 10
#define FILE_BUF_SIZE_LOCAL (FILE_BUF_SIZE * 10)

#define MAX_COMMAND_HASHTABLE_SIZE 10
#define CURRENT_COMMAND_COUNT_SIZE 5

#define NODE_TYPE_SPECIAL_TOKEN 0x0001
#define NODE_TYPE_VALUE 0X0002
#define NODE_TYPE_VALUE2 0x0011

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
    char *msg;
};

// struct which defines type of node , value in commands for better debugging of syntex and as well as
// for better parsing.

struct parsed_list
{
    struct parsed_node *head;
    struct parsed_node *tail;
    int length;
};

struct parsed_node
{
    unsigned short type;
    char *value;
    int pos;
    struct parsed_node *next;
    struct parsed_node *prev;
};

int commands_parser();

#endif