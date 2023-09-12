#include "grammer.h"
#include <stdio.h>
#include <string.h>
#include "../../status.h"
#include "../../utils/memory/p_memory.h"
#include "../../parser/parser_config.h"
#include "../../parser/tokenization/tokenization.h"

static struct global_grammer_stack *global_stack;

static unsigned short stack_ops(const short ops, struct stack_item *item);
static struct stack_item *generate_stack_item(unsigned short type);
static struct parsed_node *pair_context_grammer(struct parsed_node *parsed_node);
static struct parsed_node *start_context_grammer(struct parsed_node *parsed_node);
static struct parsed_node *object_context_grammer(struct parsed_node *parsed_node);
struct parsed_node *init_grammer_validation(struct parsed_list *tokens_parsed);

// Main function which is responsible for starting grammers checking on parsed tokens.
struct parsed_node *init_grammer_validation(struct parsed_list *tokens_parsed)
{
    struct parsed_node *curr_node = tokens_parsed->head;
    // Init grammer is only responsible for calling start context i.e ([]). Anything encounter before start context will
    // considered as syntax error or unexpected token.

    // SO grammer for innt_grammer function is only check start context and pass control to it.

    switch (curr_node->type)
    {
    case LEFT_SQUARE_BRACKET:
    {
        struct stack_item *stack_item = generate_stack_item(LEFT_SQUARE_BRACKET);
        if (!stack_item)
        {
            return curr_node;
        }
        stack_ops(1, stack_item);
        return start_context_grammer(curr_node->next); // Exit with whatever start context returns.
    }
    case NEWLINE_CHARACTER:
    {
        return start_context_grammer(curr_node); // Exit with whatever start context returns.
    }
    default:
        return curr_node;
    }

    return NULL;
}

static struct parsed_node *start_context_grammer(struct parsed_node *parsed_node)
{
    /*
        Grammar rule for start context is already mentioned in parser.txt
        Grammar -> "\n {{Object}} \n"
        First check for a new line character as per grammar rule.
    */
    if (parsed_node == NULL)
    {
        return NULL;
    }

    if (parsed_node->type != NEWLINE_CHARACTER)
    {
        printf("encountersss error %d \n",parsed_node->type);
        return parsed_node;
    }

    // Switch to the next node for validating the next token. But keep track of the previous one.
    struct parsed_node *pre_node = parsed_node;
    parsed_node = parsed_node->next;

    /*
        Start context grammar's responsibility is to check for Object context i.e. ({}) and closing start context.
        Other tokens are considered as invalid tokens.
        As per grammar rule mentioned, if the object grammar encounters start context, it will pass control to the object grammar.
        It is object grammar's responsibility to check newline at its respective end so that start context can
        properly meet its grammar rules.
    */

    // Check for object grammar and closing of start context.
    switch (parsed_node->type)
    {
    case LEFT_CURLY_BRACE:
    {
        struct stack_item *_item = generate_stack_item((unsigned short)LEFT_CURLY_BRACE);
        if (!_item)
        {
            return parsed_node;
        }
        stack_ops(1, _item);
        return object_context_grammer(parsed_node->next);
    }

    case RIGHT_SQUARE_BRACKET:
    {
        unsigned short pop_ele = stack_ops(3, NULL); // check for opening square bracket.
        if (pop_ele == LEFT_SQUARE_BRACKET)
        {
            return NULL;
        }
        return parsed_node;
    }
    default:
        return parsed_node;
    }
}

static struct parsed_node *object_context_grammer(struct parsed_node *parsed_node)
{
    // printf("called object context\n");
    if (parsed_node->type != SPACE_CHARACTER)
    {
        printf("encounter error %d \n",parsed_node->type);
        return parsed_node;
    }

    // Switch to the next node for validating the next token. But keep track of the previous one.
    struct parsed_node *pre_node = parsed_node;
    parsed_node = parsed_node->next;

    // Check for object grammar and closing of object context.
    switch (parsed_node->type)
    {
    case TYPE_CHUNK_HEX:
    {
        struct stack_item *key_item = generate_stack_item((unsigned short)TYPE_CHUNK_HEX);
        if (!key_item)
        {
            return parsed_node;
        }
        stack_ops(1, key_item);
        // Adding encountered text node into
        printf("%s -> ", parsed_node->value);
        return pair_context_grammer(parsed_node->next);
    }
    case RIGHT_CURLY_BRACE:
    {
        // Check for closing bracket from global stack.
        unsigned short pop_ele = stack_ops(3, NULL); // check for opening square bracket.
        if (pop_ele == LEFT_CURLY_BRACE)
        {
            printf("p %p\n",parsed_node->next);
            return start_context_grammer(parsed_node->next);
        }
        printf("stakc err %d",pop_ele);
        return parsed_node;
    }
    default:
        return parsed_node;
    }
}

static struct parsed_node *pair_context_grammer(struct parsed_node *parsed_node)
{
    // printf("called pair context\n");
    if (parsed_node->type != COLON_CHARACTER)
    {
        return parsed_node;
    }

    // Switch to the next node for validating the next token. But keep track of the previous one.
    struct parsed_node *pre_node = parsed_node;
    parsed_node = parsed_node->next;

    // Check for object grammar and closing of start context.
    switch (parsed_node->type)
    {
    case TYPE_CHUNK_HEX:
    {
        // This is value of pair and last element of of global stack should be key of pair which is of
        // type TYPE_CHUNK_HEX.
        unsigned short pop_ele = stack_ops(3, NULL); // check for key of pair.
        if (pop_ele == TYPE_CHUNK_HEX)
        {
            printf("%s\n", parsed_node->value);
            return object_context_grammer(parsed_node->next);
        }
        return parsed_node;
    }
    default:
        return parsed_node;
    }
}

static struct stack_item *generate_stack_item(unsigned short type)
{
    struct stack_item *item = pmalloc(sizeof(struct stack_item));
    item->type = type;
    item->next = NULL;
    return item;
}

// Function which has responsibility of adding and deleting from stack inshort maintaining the global stack.
static unsigned short stack_ops(const short ops, struct stack_item *item)
{
    // 1 for adding
    // 2 for checking
    // 3 for poping

    if (ops == 1)
    {
        if (item == NULL)
        {
            return 0;
        }
        if (global_stack->head == NULL)
        {
            global_stack->head = item;
            global_stack->tail = item;
            global_stack->len += 1;
            return 1;
        }

        item->next = (struct stack_item *)global_stack->head;
        global_stack->head = (struct stack_item *)item;
        global_stack->len += 1;
        return 1;
    }

    if (ops == 2)
    {
        if (global_stack->head != NULL)
        {
            return global_stack->head->type;
        }
        return 0;
    }

    if (ops == 3)
    {
        if (global_stack->head != NULL)
        {
            struct stack_item *curr = global_stack->head;
            unsigned short pop_ele = curr->type;
            global_stack->head = global_stack->head->next;
            // remove memory
            free(curr);
            return pop_ele;
        }
        return 0;
    }
}

// Fcuntion which have responsibility to initiate global stack pointer to keep track of syntax token mismatch errors.
struct global_grammer_stack *init_global_stack()
{
    global_stack = pmalloc(sizeof(struct global_grammer_stack));
    if (!global_stack)
    {
        return NULL;
    }

    // Initiate global stack to default values.
    global_stack->head = NULL;
    global_stack->tail = NULL;
    global_stack->len = 0;
    return global_stack;
}