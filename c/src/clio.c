/*
    Clio: A toolkit for creating elegant command line interfaces.

    Author: Darren Mulholland <dmulholland@outlook.ie>
    License: Public Domain
*/

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>


// ----------------------------------------------------------------------
// Local Declarations
// ----------------------------------------------------------------------


// Types.
typedef struct ArgParser ArgParser;
typedef void (*CmdCallback)(ArgParser *parser);

// Functions.
static void ArgParser_free(ArgParser *parser);


// ----------------------------------------------------------------------
// Utility Functions
// ----------------------------------------------------------------------


// Attempts to parse a string as an integer value, exiting on failure.
static int try_str_to_int(char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);
    if (errno != 0 || *endptr != '\0' || result > INT_MAX || result < INT_MIN) {
        fprintf(stderr, "Error: cannot parse '%s' as an integer.\n", str);
        exit(1);
    }
    return (int) result;
}


// Attempts to parse a string as a double value, exiting on failure.
static double try_str_to_double(char *str) {
    char *endptr;
    errno = 0;
    double result = strtod(str, &endptr);
    if (errno != 0 || *endptr != '\0') {
        fprintf(stderr, "Error: cannot parse '%s' as a float.\n", str);
        exit(1);
    }
    return result;
}


// ----------------------------------------------------------------------
// Option Elements
// ----------------------------------------------------------------------


// We use 'flag' as a synonym for boolean options, i.e. options that are either
// present (true) or absent (false). All other option types require an argument.
typedef enum OptionType {
    FLAG,
    STRING,
    INTEGER,
    FLOAT,
} OptionType;


// An Option instance represents an option registered on a parser.
typedef struct Option {
    OptionType type;
    union {
        bool bool_val;
        char *str_val;
        int int_val;
        double float_val;
    };
    char *name;
    char alias;
} Option;


// Initializes a new flag (boolean option).
static Option * Option_new_flag(char *name, char alias, bool value) {
    Option *option = malloc(sizeof(Option));
    option->type = FLAG;
    option->name = name;
    option->alias = alias;
    option->bool_val = value;
    return option;
}


// Initializes a new string option.
static Option * Option_new_str(char *name, char alias, char  *value) {
    Option *option = malloc(sizeof(Option));
    option->type = STRING;
    option->name = name;
    option->alias = alias;
    option->str_val = value;
    return option;
}


// Initializes a new integer option.
static Option * Option_new_int(char *name, char alias, int value) {
    Option *option = malloc(sizeof(Option));
    option->type = INTEGER;
    option->name = name;
    option->alias = alias;
    option->int_val = value;
    return option;
}


// Initializes a new floating point option.
static Option * Option_new_float(char *name, char alias, double value) {
    Option *option = malloc(sizeof(Option));
    option->type = FLOAT;
    option->name = name;
    option->alias = alias;
    option->float_val = value;
    return option;
}


// Frees the memory occupied by an Option instance.
static void Option_free(Option *option) {
    free(option);
}


// Prints an Option instance to stdout for debugging.
static void Option_print(Option *option) {
    switch (option->type) {
        case FLAG:
            printf("  %s: %s\n", option->name, option->bool_val ? "true" : "false");
            break;
        case STRING:
            printf("  %s: %s\n", option->name, option->str_val);
            break;
        case INTEGER:
            printf("  %s: %i\n", option->name, option->int_val);
            break;
        case FLOAT:
            printf("  %s: %f\n", option->name, option->float_val);
            break;
    }
}


// ----------------------------------------------------------------------
// OptionMap
// ----------------------------------------------------------------------


// A collection of Option elements indexed by name and and by
// single-character shortcut alias.
typedef struct OptionMap {
    int len;
    int cap;
    Option **list;
} OptionMap;


// Initializes a new OptionMap instance.
static OptionMap * OptionMap_new() {
    OptionMap *map = malloc(sizeof(OptionMap));
    map->len = 0;
    map->cap = 10;
    map->list = malloc((sizeof(Option*)) * map->cap);
    return map;
}


// Frees the memory occupied by an OptionMap instance and by all its
// associated Option instances.
static void OptionMap_free(OptionMap *map) {
    for (int i = 0; i < map->len; i++) {
        Option_free(map->list[i]);
    }
    free(map->list);
    free(map);
}


// Adds a new Option element to the map.
static void OptionMap_add(OptionMap *map, Option *option) {
    if (map->len == map->cap) {
        map->cap *= 2;
        map->list = realloc(map->list, (sizeof(Option*)) * map->cap);
    }
    map->list[map->len] = option;
    map->len++;
}


// Fetches an Option element by name. Returns NULL if the name is not found.
static Option * OptionMap_get_by_name(OptionMap *map, char *name) {
    for (int i = 0; i < map->len; i++) {
        Option *option = map->list[i];
        if (strcmp(name, option->name) == 0) {
            return option;
        }
    }
    return NULL;
}


// Fetches an Option element by its shortcut alias. Returns NULL if the alias
// is not found.
static Option * OptionMap_get_by_alias(OptionMap *map, char alias) {
    for (int i = 0; i < map->len; i++) {
        Option *option = map->list[i];
        if (option->alias == alias) {
            return option;
        }
    }
    return NULL;
}


// Prints an OptionMap instance to stdout for debugging.
static void OptionMap_print(OptionMap *map) {
    puts("Options:");
    if (map->len > 0) {
        for (int i = 0; i < map->len; i++) {
            Option_print(map->list[i]);
        }
    } else {
        puts("  [none]");
    }
}


// ----------------------------------------------------------------------
// CommandMap
// ----------------------------------------------------------------------


// Entry type for the CommandMap container.
typedef struct CommandMapEntry {
    char *command;
    ArgParser *parser;
    CmdCallback callback;
} CommandMapEntry;


// Container storing pointers to command sub-parsers and command callbacks
// indexed by command string.
typedef struct CommandMap {
    int len;
    int cap;
    CommandMapEntry *list;
} CommandMap;


// Initializes a new CommandMap instance.
static CommandMap * CommandMap_new() {
    CommandMap *map = malloc(sizeof(CommandMap));
    map->len = 0;
    map->cap = 10;
    map->list = malloc(sizeof(CommandMapEntry) * map->cap);
    return map;
}


// Frees the memory associated with a CommandMap instance, including the
// memory associated with each constituent command parser instance.
static void CommandMap_free(CommandMap *map) {
    for (int i = 0; i < map->len; i++) {
        ArgParser_free(map->list[i].parser);
    }
    free(map->list);
    free(map);
}


// Adds a new entry to a CommandMap instance.
static void CommandMap_add(CommandMap *map, char *command, ArgParser *parser, CmdCallback callback) {
    if (map->len == map->cap) {
        map->cap *= 2;
        map->list = realloc(map->list, sizeof(CommandMapEntry*) * map->cap);
    }
    map->list[map->len] = (CommandMapEntry) { command, parser, callback };
    map->len++;
}


// Returns a pointer to the CommandMapEntry corresponding to the specified
// command, or NULL if the command is not recognised.
static CommandMapEntry * CommandMap_get(CommandMap *map, char *command) {
    for (int i = 0; i < map->len; i++) {
        if (strcmp(map->list[i].command, command) == 0) {
            return &map->list[i];
        }
    }
    return NULL;
}


// ----------------------------------------------------------------------
// ArgStream
// ----------------------------------------------------------------------


// An ArgStream instance is a wrapper for an array of string pointers,
// allowing it to be accessed as a stream.
typedef struct ArgStream {
    int len;
    int index;
    char **args;
} ArgStream;


// Initializes a new ArgStream instance.
static ArgStream * ArgStream_new(int len, char **args) {
    ArgStream *stream = malloc(sizeof(ArgStream));
    stream->len = len;
    stream->index = 0;
    stream->args = args;
    return stream;
}


// Frees the memory associated with an ArgStream instance.
static void ArgStream_free(ArgStream *stream) {
    free(stream);
}


// Returns true if the stream contains at least one more argument.
static bool ArgStream_has_next(ArgStream *stream) {
    return stream->index < stream->len;
}


// Returns the next argument from the stream.
static char * ArgStream_next(ArgStream *stream) {
    return stream->args[stream->index++];
}


// ----------------------------------------------------------------------
// ArgList
// ----------------------------------------------------------------------


// Container for storing positional arguments parsed from the input stream.
typedef struct ArgList {
    int len;
    int cap;
    char **args;
} ArgList;


// Initializes a new ArgList instance.
static ArgList * ArgList_new() {
    ArgList *list = malloc(sizeof(ArgList));
    list->len = 0;
    list->cap = 10;
    list->args = malloc((sizeof(char*)) * list->cap);
    return list;
}


// Appends a new argument to the list.
static void ArgList_add(ArgList *list, char *arg) {
    if (list->len == list->cap) {
        list->cap *= 2;
        list->args = realloc(list->args, (sizeof(char*)) * list->cap);
    }
    list->args[list->len] = arg;
    list->len++;
}


// Frees the memory associated with the ArgList instance.
static void ArgList_free(ArgList *list) {
    free(list->args);
    free(list);
}


// Prints the ArgList instance to stdout for debugging.
static void ArgList_print(ArgList *list) {
    puts("Arguments:");
    if (list->len > 0) {
        for (int i = 0; i < list->len; i++) {
            printf("  %s\n", list->args[i]);
        }
    } else {
        puts("  [none]");
    }
}


// ----------------------------------------------------------------------
// ArgParser
// ----------------------------------------------------------------------


// An ArgParser instance is responsible for storing registered options.
// Note that every registered command recursively receives an ArgParser
// instance of its own. In theory commands can be stacked to any depth,
// although in practice even two levels is confusing for users.
struct ArgParser {
    char *helptext;
    char *version;
    OptionMap *options;
    ArgList *arguments;
    CommandMap *commands;
    char *command;
    ArgParser *command_parser;
};


// Initializes and returns a new ArgParser instance.
// Supplying help text activates the automatic --help flag, supplying a
// version string activates the automatic --version flag. A NULL pointer can
// be passed for either parameter.
static ArgParser * ArgParser_new(char *helptext, char *version) {
    ArgParser *parser = malloc(sizeof(ArgParser));
    parser->helptext = helptext;
    parser->version = version;
    parser->options = OptionMap_new();
    parser->arguments = ArgList_new();
    parser->commands = CommandMap_new();
    parser->command = NULL;
    parser->command_parser = NULL;
    return parser;
}


// Frees the memory associated with an ArgParser instance.
static void ArgParser_free(ArgParser *parser) {
    OptionMap_free(parser->options);
    ArgList_free(parser->arguments);
    CommandMap_free(parser->commands);
    free(parser);
}


// Registers a flag (boolean) option and its single-character shortcut alias.
static void ArgParser_add_flag(ArgParser *parser, char *name, char alias) {
    Option *option = Option_new_flag(name, alias, false);
    OptionMap_add(parser->options, option);
}


// Registers a string option and its single-character shortcut alias.
static void ArgParser_add_str_opt(ArgParser *parser, char *name, char* def_value, char alias) {
    Option *option = Option_new_str(name, alias, def_value);
    OptionMap_add(parser->options, option);
}


// Registers an integer option and its single-character shortcut alias.
static void ArgParser_add_int_opt(ArgParser *parser, char *name, int def_value, char alias) {
    Option *option = Option_new_int(name, alias, def_value);
    OptionMap_add(parser->options, option);
}


// Registers a float option and its single-character shortcut alias.
static void ArgParser_add_float_opt(ArgParser *parser, char *name, double def_value, char alias) {
    Option *option = Option_new_float(name, alias, def_value);
    OptionMap_add(parser->options, option);
}


// Registers a command and its associated callback.
static ArgParser * ArgParser_add_cmd(ArgParser *parser, char *command, CmdCallback callback, char *helptext) {
    ArgParser *cmd_parser = ArgParser_new(helptext, NULL);
    CommandMap_add(parser->commands, command, cmd_parser, callback);
    return cmd_parser;
}


// Parses the supplied stream of string arguments.
static void ArgParser_parse_stream(ArgParser *parser, ArgStream *stream) {

    // Switch to turn off parsing if we encounter a -- argument.
    // Everything following the -- will be treated as a positional argument.
    bool parsing = true;

    // Loop while we have arguments to process.
    while (ArgStream_has_next(stream)) {

        // Fetch the next argument from the stream.
        char *arg = ArgStream_next(stream);

        // If parsing has been turned off, add the argument to the list of positionals.
        if (!parsing) {
            ArgList_add(parser->arguments, arg);
            continue;
        }

        // If we encounter a -- argument, turn off parsing.
        if (strcmp(arg, "--") == 0) {
            parsing = false;
            continue;
        }

        // Is the argument a long-form option or flag?
        if (strncmp(arg, "--", 2) == 0) {

            // Strip the prefix.
            arg +=2;

            // Is the argument a registered option name?
            Option *option = OptionMap_get_by_name(parser->options, arg);
            if (option != NULL) {

                // If the option is a flag, store the boolean true.
                if (option->type == FLAG) {
                    option->bool_val = true;
                }

                // Otherwise, check for a following argument.
                else if (ArgStream_has_next(stream)) {

                    char *nextarg = ArgStream_next(stream);

                    if (option->type == STRING) {
                        option->str_val = nextarg;
                    }
                    else if (option->type == INTEGER) {
                        option->int_val = try_str_to_int(nextarg);
                    }
                    else if (option->type == FLOAT) {
                        option->float_val = try_str_to_double(nextarg);
                    }
                }

                // No following argument, so print an error and exit.
                else {
                    fprintf(stderr, "Error: missing argument for the --%s option.\n", arg);
                    exit(1);
                }
            }

            // Else, is the argument the automatic --help flag?
            else if (strcmp(arg, "help") == 0 && parser->helptext != NULL) {
                puts(parser->helptext);
                exit(0);
            }

            // Else, is the argument the automatic --version flag?
            else if (strcmp(arg, "version") == 0 && parser->version != NULL) {
                puts(parser->version);
                exit(0);
            }

            // The argument is not a registered or automatic option.
            // Print an error message and exit.
            else {
                fprintf(stderr, "Error: --%s is not a recognised option.\n", arg);
                exit(1);
            }
        }

        // Is the argument a short-form option or flag?
        else if (arg[0] == '-') {

            // If the argument consists of a sigle dash or a dash followed by a
            // digit, treat it as a positional argument.
            if (strlen(arg) == 1 || isdigit(arg[1])) {
                ArgList_add(parser->arguments, arg);
                continue;
            }

            // Examine each character individually to allow for condensed
            // short-form arguments, i.e.
            //     -a -b foo -c bar
            // is equivalent to
            //     -abc foo bar
            for (unsigned long i = 1; i < strlen(arg); i++) {

                // Is the character at index i a registered shortcut alias?
                Option *option = OptionMap_get_by_alias(parser->options, arg[i]);
                if (option == NULL) {
                    fprintf(stderr, "Error: -%c is not a recognised option.\n", arg[i]);
                    exit(1);
                }

                // If the option type is a flag, store the boolean true.
                if (option->type == FLAG) {
                    option->bool_val = true;
                }

                // Otherwise, check for a following argument.
                else if (ArgStream_has_next(stream)) {

                    char *nextarg = ArgStream_next(stream);

                    if (option->type == STRING) {
                        option->str_val = nextarg;
                    }
                    else if (option->type == INTEGER) {
                        option->int_val = try_str_to_int(nextarg);
                    }
                    else if (option->type == FLOAT) {
                        option->float_val = try_str_to_double(nextarg);
                    }
                }

                // No following argument. Print an error message and exit.
                else {
                    fprintf(stderr, "Error: missing argument for the -%c option.\n", arg[i]);
                    exit(1);
                }
            }
        }

        // Is the argument a registered command?
        else if (CommandMap_get(parser->commands, arg) != NULL) {
            CommandMapEntry *entry = CommandMap_get(parser->commands, arg);
            ArgParser_parse_stream(entry->parser, stream);
            entry->callback(entry->parser);
            parser->command = arg;
            parser->command_parser = entry->parser;
            break;
        }

        // Is the argument the automatic 'help' command? The commands
        //     $ app cmd --help
        // and
        //     $ app help cmd
        // are functionally equivalent. Both will print the help text
        // associated with the command.
        else if (strcmp(arg, "help") == 0) {
            if (ArgStream_has_next(stream)) {
                char *command = ArgStream_next(stream);
                if (CommandMap_get(parser->commands, command) != NULL) {
                    CommandMapEntry *entry = CommandMap_get(parser->commands, command);
                    puts(entry->parser->helptext);
                    exit(0);
                } else {
                    fprintf(stderr, "Error: '%s' is not a recognised command.\n", command);
                    exit(1);
                }
            } else {
                fprintf(stderr, "Error: the help command requires an argument.\n");
                exit(1);
            }
        }

        // Otherwise we have a positional argument.
        else {
            ArgList_add(parser->arguments, arg);
        }
    }
}


// Parses the supplied array of string arguments.
static void ArgParser_parse(ArgParser *parser, int argcount, char *args[]) {

    // Convert the input array into a stream.
    ArgStream *stream = ArgStream_new(argcount, args);

    // Parse the stream.
    ArgParser_parse_stream(parser, stream);

    // Free the memory associated with the stream.
    ArgStream_free(stream);
}


// Returns true if the named flag was found.
static bool ArgParser_get_flag(ArgParser *parser, char *name) {
    return OptionMap_get_by_name(parser->options, name)->bool_val;
}


// Returns the value of the named string option.
static char * ArgParser_get_str_opt(ArgParser *parser, char *name) {
    return OptionMap_get_by_name(parser->options, name)->str_val;
}


// Returns the value of the named integer option.
static int ArgParser_get_int_opt(ArgParser *parser, char *name) {
    return OptionMap_get_by_name(parser->options, name)->int_val;
}


// Returns the value of the named float option.
static double ArgParser_get_float_opt(ArgParser *parser, char *name) {
    return OptionMap_get_by_name(parser->options, name)->float_val;
}


// Returns true if the parser has found one or more positional arguments.
static bool ArgParser_has_args(ArgParser *parser) {
    return parser->arguments->len > 0;
}


// Returns the number of positional arguments.
static int ArgParser_num_args(ArgParser *parser) {
    return parser->arguments->len;
}


// Returns the positional arguments as an array of string pointers.
// The memory occupied by the returned array is not affected by calls to
// ArgParser_free().
static char ** ArgParser_get_args(ArgParser *parser) {
    char **args = malloc(sizeof(char*) * parser->arguments->len);
    memcpy(args, parser->arguments->args, sizeof(char*) * parser->arguments->len);
    return args;
}


// Attempts to parse and return the positional arguments as an array of
// integers. Exits with an error message on failure.
// The memory occupied by the returned array is not affected by calls to
// ArgParser_free().
static int * ArgParser_get_args_as_ints(ArgParser *parser) {
    int *args = malloc(sizeof(int) * parser->arguments->len);
    for (int i = 0; i < parser->arguments->len; i++) {
        *(args + i) = try_str_to_int(parser->arguments->args[i]);
    }
    return args;
}


// Attempts to parse and return the positional arguments as an array of
// doubles. Exits with an error message on failure.
// The memory occupied by the returned array is not affected by calls to
// ArgParser_free().
static double * ArgParser_get_args_as_floats(ArgParser *parser) {
    double *args = malloc(sizeof(double) * parser->arguments->len);
    for (int i = 0; i < parser->arguments->len; i++) {
        *(args + i) = try_str_to_double(parser->arguments->args[i]);
    }
    return args;
}


// Returns true if the parser has found a command.
static bool ArgParser_has_cmd(ArgParser *parser) {
    return parser->command != NULL;
}


// Returns the command string, if the parser has found a command.
static char * ArgParser_get_cmd(ArgParser *parser) {
    return parser->command;
}


// Returns the command parser instance, if the parser has found a command.
static ArgParser * ArgParser_get_cmd_parser(ArgParser *parser) {
    return parser->command_parser;
}


// Prints an ArgParser instance to stdout for debugging.
static void ArgParser_print(ArgParser *parser) {
    OptionMap_print(parser->options);
    puts("");
    ArgList_print(parser->arguments);
    puts("\nComand:");
    if (ArgParser_has_cmd(parser)) {
        printf("  %s\n", ArgParser_get_cmd(parser));
    } else {
        puts("  [none]");
    }
}


// ----------------------------------------------------------------------
// Public Interface
// ----------------------------------------------------------------------

// The functions below constitute the public interface of the library.
// For the most part they are simple wrappers for the corresponding
// ArgParser functions defined above.


ArgParser * clio_new(char *helptext, char *version) {
    return ArgParser_new(helptext, version);
}


void clio_add_flag(ArgParser *parser, char *name, char *alias) {
    ArgParser_add_flag(parser, name, alias == NULL ? 0 : alias[0]);
}


void clio_add_str(ArgParser *parser, char *name, char* def_value, char *alias) {
    ArgParser_add_str_opt(parser, name, def_value, alias == NULL ? 0 : alias[0]);
}


void clio_add_int(ArgParser *parser, char *name, int def_value, char *alias) {
    ArgParser_add_int_opt(parser, name, def_value, alias == NULL ? 0 : alias[0]);
}


void clio_add_float(ArgParser *parser, char *name, double def_value, char *alias) {
    ArgParser_add_float_opt(parser, name, def_value, alias == NULL ? 0 : alias[0]);
}


ArgParser * clio_add_cmd(ArgParser *parser, char *command, CmdCallback callback, char *helptext) {
    return ArgParser_add_cmd(parser, command, callback, helptext);
}


void clio_parse(ArgParser *parser, int argc, char **argv) {
    ArgParser_parse(parser, argc - 1, argv + 1);
}


bool clio_get_flag(ArgParser *parser, char *name) {
    return ArgParser_get_flag(parser, name);
}


char * clio_get_str(ArgParser *parser, char *name) {
    return ArgParser_get_str_opt(parser, name);
}


int clio_get_int(ArgParser *parser, char *name) {
    return ArgParser_get_int_opt(parser, name);
}


double clio_get_float(ArgParser *parser, char *name) {
    return ArgParser_get_float_opt(parser, name);
}


bool clio_has_args(ArgParser *parser) {
    return ArgParser_has_args(parser);
}


int clio_num_args(ArgParser *parser) {
    return ArgParser_num_args(parser);
}


char ** clio_get_args(ArgParser *parser) {
    return ArgParser_get_args(parser);
}


int * clio_get_args_as_ints(ArgParser *parser) {
    return ArgParser_get_args_as_ints(parser);
}


double * clio_get_args_as_floats(ArgParser *parser) {
    return ArgParser_get_args_as_floats(parser);
}


bool clio_has_cmd(ArgParser *parser) {
    return ArgParser_has_cmd(parser);
}


char * clio_get_cmd(ArgParser *parser) {
    return ArgParser_get_cmd(parser);
}


ArgParser * clio_get_cmd_parser(ArgParser *parser) {
    return ArgParser_get_cmd_parser(parser);
}


void clio_free(ArgParser *parser) {
    ArgParser_free(parser);
}


void clio_dump(ArgParser *parser) {
    ArgParser_print(parser);
}
