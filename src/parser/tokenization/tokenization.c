#include "tokenization.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../../status.h"
#include "../../utils/memory/p_memory.h"
#include "../../utils/common/customstring.h"
#include "../../parser/grammer/grammer.h"
#include "../../parser/symboltable/symboltable.h"
#include "../../parser/parser_config.h"

struct commands_colle *cmds;

static struct inprogress_parsing
{
    struct command *command;
    char last_chunk[FILE_BUF_SIZE_LOCAL];
    int last_pos_in_chunk;
};

static struct parser_tokens
{
    char *tokens;
};

unsigned int hash(const char key)
{
    unsigned int hash_value = 0;
    char k[5] = {key, 0, 0, 0, 0};
    for (int i = 0; i < 5; i++)
    {
        k[i] = key + i;
    }
    char j;
    char m;
    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
        {
            j = k[i] << 3;
        }
        if (i == 3)
        {
            m = k[i] << 2;
        }
        hash_value = hash_value + k[i] + j; // Bit manipulation and addition
    }
    return hash_value % MAX_COMMAND_HASHTABLE_SIZE; // Modulus to fit within the table size
}

static struct command *generate_new_command()
{
    struct command *command = pmalloc(sizeof(struct command));
    return command;
}

void loop_the_parsed_list(struct parsed_list *parsed_list)
{
    struct parsed_node *curr_node = parsed_list->head;
    while (curr_node != NULL)
    {
        printf("type is %d and value is %ld\n", curr_node->type, sizeof(curr_node->value));
        curr_node = curr_node->next;
    }
}

int generate_node_for_tokens(char *token, unsigned short type, struct parsed_list *parsed_list)
{
    struct parsed_node *node = pmalloc(sizeof(struct parsed_node));
    if (!node)
    {
        return -EINMEM;
    }
    size_t len = strlen(token);

    if (len <= 0)
    {
        return 1;
    }

    // Allocate memory for the copy, including space for the null-terminator
    char *_token = (char *)pmalloc(len + 1);

    // Copy the string to the local variable
    strcpy(_token, token);

    // Generate node for given token.
    node->type = type;
    node->value = _token;
    node->next = 0x00;
    node->prev = 0x00;
    node->pos = parsed_list->length + 1;

    // Add node to last (tail) of parsed list for maintaning sequence of parsed tokens.
    // If it is first token or node in process.
    if (parsed_list->head == NULL)
    {
        parsed_list->head = node;
        parsed_list->tail = node;
        parsed_list->length = parsed_list->length + 1;
        return 1;
    }

    parsed_list->tail->next = node;
    node->prev = parsed_list->tail;
    parsed_list->tail = node;
    parsed_list->length = parsed_list->length + 1;
    return 1;
}

int check_token_insymbol_table(struct parser_tokens *token_table, char s)
{
    // Symbols which we required for grammer validation.
    char symbolArray[] = {
        LEFT_SQUARE_BRACKET,
        LEFT_CURLY_BRACE,
        COMMA_CHARACTER,
        COLON_CHARACTER,
        SPACE_CHARACTER,
        NEWLINE_CHARACTER,
        RIGHT_CURLY_BRACE,
        RIGHT_SQUARE_BRACKET,
        TYPE_CHUNK_HEX,
        TYPE_VALUE_HEX};

    // for (size_t i = 0; i < MAX_TOKENS; i++)
    // {
    //     if (token_table->tokens[i] == s)
    //     {
    //         return token_table->tokens[i];
    //     }
    // };

    for (size_t i = 0; i < TOKENS_COUNT; i++)
    {
        if (symbolArray[i] == s)
        {
            return symbolArray[i];
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
/*
static struct parser_tokens *initiate_tokens_structs()
{
    char *tokens = pmalloc(MAX_TOKENS);
    if (!tokens)
    {
        printf("Error while allocating memory for token table\n");
        return -EIO;
    }
    // tokens = "[]{},:| ";

    tokens = {
        LEFT_SQUARE_BRACKET,
        LEFT_CURLY_BRACE,
        COMMA_CHARACTER,
        COLON_CHARACTER,
        SPACE_CHARACTER,
        NEWLINE_CHARACTER,
        RIGHT_CURLY_BRACE,
        RIGHT_SQUARE_BRACKET,
        TYPE_CHUNK_HEX,
        TYPE_VALUE_HEX};
    // Memory allocated for tokens table.
    struct parser_tokens *tokens_table = pmalloc(sizeof(struct parser_tokens));
    if (!tokens_table)
    {
        printf("Error while allocating memory for token table\n");
        return -EIO;
    }

    tokens_table->tokens = tokens;
    return tokens_table;
}
*/

static void destroy_parsedlist_struct(struct parsed_list *parsed_list)
{
    struct parsed_node *curr_node = parsed_list->head;
    while (curr_node != NULL)
    {
        struct parsed_node *next_node = curr_node->next; // Store the next node
        free(curr_node);                                 // Free the current node
        curr_node = next_node;                           // Move to the next node
    }
    // free(parsed_list->tail);  // You may or may not need to free the tail node
    parsed_list->head = NULL; // Make sure to update the list's head to NULL
    parsed_list->tail = NULL; // Make sure to update the list's tail to NULL
}

static struct parsed_list *initiate_parsedlist_struct()
{
    // Memory allocated for parsing linked list which represents all tokens.
    struct parsed_list *parsed_list = pmalloc(sizeof(struct parsed_list));
    if (!parsed_list)
    {
        printf("Error while allocating memory for parsing list.\n");
        return -EIO;
    }

    parsed_list->head = 0x00;
    parsed_list->tail = 0x00;
    parsed_list->length = 0;
    return parsed_list;
}

static struct syntax_status *check_syntax_validation(struct parsed_list *parsed_list)
{
    struct syntax_status *syntax = pmalloc(sizeof(struct syntax_status));
    if (syntax == NULL)
    {
        return -EIO;
    }

    // Start checking the parsed list of tokens
}

/*
    static int context_checker(char byte, struct token_stack *token_stack)
    {
        // Check last element and pass through below rules for checking context whether we need to consider byte param or not
        // int is_present = check_token_incontext_table(char type, char s);
        // return is_present;
    }

*/

static int command_reader(char *f_chunk, struct inprogress_parsing *track_parsing, struct parser_tokens *parser_tokens, struct parsed_list *parsed_list)
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

        // if (f_chunk[pos] == 9)
        // {
        //     pos++;
        //     continue;
        // }

        // Check if special token found then we have to break it down.
        int token = check_token_insymbol_table(parser_tokens, f_chunk[pos]);

        // 1 - It is special token
        // 0 - It is not special token
        if (token)
        {
            // Token encounter. Need to generate node for last chunk generated first before token encounter.

            int n_token_node_generated = generate_node_for_tokens(track_parsing->last_chunk, (unsigned short)TYPE_CHUNK_HEX, parsed_list);
            if (!n_token_node_generated)
            {
                printf("Failed to create node for parsed tokens\n");
                return -EINMEM;
            }

            // Generate token for special encountered token.
            char token_enc = f_chunk[pos];
            int s_token_node_generated = generate_node_for_tokens((char *)&token, (unsigned short)token, parsed_list);
            if (!s_token_node_generated)
            {
                printf("Failed to create node for parsed tokens\n");
                return -EINMEM;
            }

            // Clean last chunk generated for new chunk after token.
            memset(track_parsing->last_chunk, 0, FILE_BUF_SIZE_LOCAL);
            track_parsing->last_pos_in_chunk = 0;
            pos++;

            /*

                Check for is this token is '}' because we need to stop here and switch for next steps
                i.e Checking syntax for parsed list using Rules set , and if everything goes right we
                can generate command struct from it.

            */
            if (token == RIGHT_CURLY_BRACE)
            {
                // Stop here and go for syntax checking for parsed list.
                struct parsed_node *_validation = init_grammer_validation(parsed_list);
                if (_validation)
                {
                    printf("Unexpacted token at %d , %s\n", _validation->pos, _validation->value);
                    exit(1);
                }

                // If _validation returns NULL pointer means it is indicating that it does not encounter any grammer erros.
                // So that we can process next object. GO ahead*.

                // Destroy existing parsed list for new generating tokens. We have track of higher level char in used which we have
                // track through our global stack.
                // BUt don't assing 0 lenght to parsed list length because we want it to represent actual position of token in which
                // error happened.

                destroy_parsedlist_struct(parsed_list);
            }
            continue;
        }

        track_parsing->last_chunk[track_parsing->last_pos_in_chunk] = f_chunk[pos];
        pos++;
        track_parsing->last_pos_in_chunk++;
    }
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
    // init_pair_grammer();

    // init_object_grammer();
    struct parser_tokens *parser_tokens = 0x00;
    struct global_grammer_stack *global_stack = init_global_stack();
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
            perror("Error while reading from cmd file\n");
            close(cmd_fd);
            return -CMD_FILE;
        }

        if (bytes == 0)
        {
            printf("CMD file completed\n");
            close(cmd_fd);
            break;
        }

        /*
            If data is available to process just process it with below function.
        */
        // chunk_buf[FILE_BUF_SIZE] = 0;
        command_reader(chunk_buf, track_parsing, parser_tokens, parsing_list);
    }
    // loop_the_parsed_list(parsing_list);
    // printf("lenght of list is %d\n", parsing_list->length);
    // printf("traverse completed deleting node\n");
    // printf("lenght of list is %d\n", parsing_list->length);

    // Close the file
    close(cmd_fd);

    // Don't forget to clear all memory allocated.
    return CMD_FILE;
}
