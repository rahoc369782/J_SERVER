#ifndef GRAMMER_H
#define GRAMMER_H
#include "../../parser/tokenization/tokenization.h"

struct stack_item
{
    unsigned short type;
    struct stack_item *next;
};  

struct global_grammer_stack
{
    struct stack_item *head;
    struct stack_item *tail;
    int len;
};

struct parsed_node *init_grammer_validation(struct parsed_list *tokens_parsed);
struct global_grammer_stack *init_global_stack();
#endif