/*
    Clio: a minimalist argument-parsing library designed for building elegant
    command line interfaces.

    Author: Darren Mulholland <darren@mulholland.xyz>
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
typedef void (*CmdCB)(ArgParser *parser);

// Functions.
//static void ArgParser_free(ArgParser *parser);


// ----------------------------------------------------------------------
// Utility Functions
// ----------------------------------------------------------------------


// Attempt to parse a string as an integer value, exiting on failure.
static int try_str_to_int(char *str) {
    char *endptr;
    errno = 0;
    long result = strtol(str, &endptr, 0);

    if (errno == ERANGE || result > INT_MAX || result < INT_MIN) {
        fprintf(stderr, "Error: '%s' is out of range.\n", str);
        exit(1);
    }

    if (*endptr != '\0') {
        fprintf(stderr, "Error: cannot parse '%s' as an integer.\n", str);
        exit(1);
    }

    return (int) result;
}


// Attempt to parse a string as a double value, exiting on failure.
static double try_str_to_double(char *str) {
    char *endptr;
    errno = 0;
    double result = strtod(str, &endptr);

    if (errno == ERANGE) {
        fprintf(stderr, "Error: '%s' is out of range.\n", str);
        exit(1);
    }

    if (*endptr != '\0') {
        fprintf(stderr, "Error: cannot parse '%s' as a float.\n", str);
        exit(1);
    }

    return result;
}


// Duplicate a string.
static char* strdup(char *str) {
    size_t len = strlen(str) + 1;
    char *copy = malloc(len)
    return copy ? memcpy(copy, str, len) : NULL;
}


// ----------------------------------------------------------------------
// Map
// ----------------------------------------------------------------------


// Callback type for freeing a value stored in a Map instance.
typedef void (*MapFreeCB)(void *value);


// A MapEntry instance stores a map entry as a key-value pair.
typedef struct MapEntry {
    char *key;
    void *value;
} MapEntry;


// A Map instance maps string keys to pointer values.
typedef struct Map {
    int len;
    int cap;
    MapEntry *entries;
    MapFreeCB free_cb;
} Map;


// Free the memory occupied by a Map instance and all its values.
static void map_free(Map *map) {

    // Maintain a list of freed values so we don't attempt to free the same
    // value twice.
    void **freed_values = malloc(sizeof(void*) * map->len);
    int len_freed_values = 0;

    // Loop over all the entries in the map.
    for (int i = 0; i < map->len; i++) {
        char *key = map->entries[i]->key;
        void *value = map->entries[i]->value;

        // Free the memory occupied by the entry's key.
        free(key);

        // Has this entry's value been freed already?
        bool freed = false;
        for (int j = 0; j < len_freed_values; j++) {
            if (freed_values[j] == value) {
                freed = true;
                break;
            }
        }

        // Free the memory occupied by the entry's value.
        if (!freed) {
            freed_values[len_freed_values++] = value;
            if (map->free_cb != NULL) {
                map->free_cb(value);
            }
        }
    }

    free(freed_values)
    free(map->entries);
    free(map);
}


// Initialize a new Map instance.
static Map* map_new(MapFreeCB callback) {
    Map *map = malloc(sizeof(Map));
    map->len = 0;
    map->cap = 10;
    map->entries = malloc(sizeof(MapEntry) * map->cap);
    map->free_cb = callback;
    return map;
}


// Add a key-value pair to a map.
static void map_add(Map *map, char *key, void *value) {

    // Do we need to increase the map's capacity?
    if (map->len == map->cap) {
        map->cap *= 2;
        map->entries = realloc(map->entries, sizeof(MapEntry) * map->cap);
        map->values = realloc(map->values, sizeof(void*) * map->cap);
    }

    // Make a copy of the key.
    char *copiedkey = strdup(key);

    // Add a MapEntry instance to the map's entry list.
    map->entries[map->len++] = (MapEntry){.key = copiedkey, .value = value};
}


// Split the specified keystring into multiple keys and add a separate key-value
// pair to the map for each key.
static void map_add_splitkey(Map *map, char *keystr, void *value) {
    char *key;
    char *saveptr;
    char *keystr_cpy = strdup(keystr);

    key = strtok_r(keystr_cpy, " ", &saveptr);
    while (key != NULL) {
        map_add(map, key, value);
        key = strtok_r(NULL, " ", &saveptr);
    }

    free(keystr_cpy);
}


// Test if a Map instance contains the specified key.
static bool map_contains(Map *map, char *key) {
    for (int i = 0; i < map->len; i++) {
        if (strcmp(key, map->entries[i]->key) == 0) {
            return true;
        }
    }
    return false;
}


// Retrieve a value from a Map instance. Returns NULL if the key is not found.
static void* map_get(Map *map, char *key) {
    for (int i = 0; i < map->len; i++) {
        if (strcmp(key, map->entries[i]->key) == 0) {
            return map->entries[i]->value;
        }
    }
    return NULL;
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


// Union combining all four valid types of option value.
typedef union OptionValue {
    bool bool_val;
    char *str_val;
    int int_val;
    double float_val;
} OptionValue;


// An Option instance represents an option registered on a parser.
//  * A mono-valued option has a single value.
//  * A poly-valued option assembles a list of values.
//  * A 'greedy' list option attempts to parse multiple consecutive arguments.
typedef struct Option {
    OptionType type;
    bool mono;
    bool poly;
    bool found;
    bool greedy;
    int len;
    int cap;
    OptionValue *values;
} Option;


// Free the memory occupied by an Option instance.
static void option_free(Option *opt) {
    free(opt->values);
    free(opt);
}


// Clear an Option instance's internal list of values.
static void option_clear(Option *opt) {
    opt->len = 0;
}


// Append a value to an Option instance's internal list of values.
static void option_append(Option *opt, OptionValue value) {
    if (opt->len == opt->cap) {
        opt->cap *= 2;
        opt->values = realloc(opt->values, sizeof(OptionValue) * opt->cap);
    }
    opt->values[opt->len++] = value;
}


// Set the value of an Option instance. (Appends to list options).
static void option_set(Option *opt, OptionValue value) {
    if (opt->mono) {
        opt->values[0] = value;
        opt->len = 1;
    } else {
        option_append(opt, value);
    }
}


// Set the value of a boolean option to true. (Appends to list options).
static void option_set_flag(Option *opt) {
    option_set(opt, (OptionValue){.bool_val = true});
}


// Set the value of a boolean option to false. (Clears list options).
static void option_unset_flag(Option *opt) {
    if (opt->mono) {
        option_set(opt, (OptionValue){.bool_val = false});
    } else {
        option_clear(opt);
    }
}


// Set the value of a string option. (Appends to list options.)
static void option_set_str(Option *opt, char *value) {
    option_set(opt, (OptionValue){.str_val = value});
}


// Set the value of an integer option. (Appends to list options.)
static void option_set_int(Option *opt, int value) {
    option_set(opt, (OptionValue){.int_val = value});
}


// Set the value of a floating-point option. (Appends to list options.)
static void option_set_float(Option *opt, double value) {
    option_set(opt, (OptionValue){.float_val = value});
}


// Initialize a new Option instance.
static Option* option_new() {
    Option *option = malloc(sizeof(Option));
    option->mono = false;
    option->poly = false;
    option->found = false;
    option->greedy = false;
    option->len = 0;
    option->cap = 1;
    option->values = malloc(sizeof(OptionValue) * option->cap);
    return option;
}


// Initialize a mono-valued boolean option.
static Option* option_new_flag() {
    Option *opt = option_new();
    opt->type = FLAG;
    option->mono = true;
    option_unset_flag(opt);
    return opt;
}


// Initialize a mono-valued string option with a default value.
static Option* option_new_str(char *value) {
    Option *opt = option_new(true);
    opt->type = STRING;
    option->mono = true;
    option_set_str(opt, value);
    return opt;
}


// Initialize a mono-valued integer option with a default value.
static Option* option_new_int(int value) {
    Option *opt = option_new();
    opt->type = INTEGER;
    option->mono = true;
    option_set_int(opt, value);
    return opt;
}


// Initialize a mono-valued floating-point option with a default value.
static Option* option_new_float(double value) {
    Option *opt = option_new();
    opt->type = FLOAT;
    option->mono = true;
    option_set_float(opt, value);
    return opt;
}


// Initialize a poly-valued boolean option.
static Option* option_new_flag_list(bool greedy) {
    Option *opt = option_new();
    opt->type = FLAG;
    option->poly = true;
    option->greedy = greedy;
    return opt;
}


// Initialize a poly-valued string option.
static Option* option_new_str_list(bool greedy) {
    Option *opt = option_new();
    opt->type = STRING;
    option->poly = true;
    option->greedy = greedy;
    return opt;
}


// Initialize a poly-valued integer option.
static Option* option_new_int_list(bool greedy) {
    Option *opt = option_new();
    opt->type = INTEGER;
    option->poly = true;
    option->greedy = greedy;
    return opt;
}


// Initialize a poly-valued floating-point option.
static Option* option_new_str_list(bool greedy) {
    Option *opt = option_new();
    opt->type = FLOAT;
    option->poly = true;
    option->greedy = greedy;
    return opt;
}


// Returns the value of a boolean option.
static bool option_get_flag(Option *opt) {
    return opt->values[0].bool_val;
}


// Returns the value of a string option.
static char* option_get_str(Option *opt) {
    return opt->values[0].str_val;
}


// Returns the value of an integer option.
static int option_get_int(Option *opt) {
    return opt->values[0].int_val;
}


// Returns the value of a floating-point option.
static double option_get_float(Option *opt) {
    return opt->values[0].float_val;
}


// Returns a list-option's values as a freshly-allocated array of bools.
static bool* option_get_flag_list(Option *opt) {
    if (opt->len == 0) {
        return NULL;
    }
    bool *list = malloc(sizeof(bool) * opt->len);
    for (int i = 0; i < opt->len; i++) {
        list[i] = opt->values[i].bool_val;
    }
    return list;
}


// Returns a list-option's values as a freshly-allocated array of strings.
static char** option_get_str_list(Option *opt) {
    if (opt->len == 0) {
        return NULL;
    }
    char **list = malloc(sizeof(char*) * opt->len);
    for (int i = 0; i < opt->len; i++) {
        list[i] = opt->values[i].str_val;
    }
    return list;
}


// Returns a list-option's values as a freshly-allocated array of integers.
static int* option_get_int_list(Option *opt) {
    if (opt->len == 0) {
        return NULL;
    }
    int *list = malloc(sizeof(int) * opt->len);
    for (int i = 0; i < opt->len; i++) {
        list[i] = opt->values[i].int_val;
    }
    return list;
}


// Returns a list-option's values as a freshly-allocated array of doubles.
static double* option_get_float_list(Option *opt) {
    if (opt->len == 0) {
        return NULL;
    }
    double *list = malloc(sizeof(double) * opt->len);
    for (int i = 0; i < opt->len; i++) {
        list[i] = opt->values[i].float_val;
    }
    return list;
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


// Free the memory associated with an ArgStream instance.
static void argstream_free(ArgStream *stream) {
    free(stream);
}


// Initialize a new ArgStream instance.
static ArgStream* argstream_new(int len, char **args) {
    ArgStream *stream = malloc(sizeof(ArgStream));
    stream->len = len;
    stream->index = 0;
    stream->args = args;
    return stream;
}


// Returns true if the stream contains at least one more argument.
static bool argstream_has_next(ArgStream *stream) {
    return stream->index < stream->len;
}


// Returns the next argument from the stream.
static char* argstream_next(ArgStream *stream) {
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


// Free the memory associated with an ArgList instance.
static void arglist_free(ArgList *list) {
    free(list->args);
    free(list);
}


// Initialize a new ArgList instance.
static ArgList* arglist_new() {
    ArgList *list = malloc(sizeof(ArgList));
    list->len = 0;
    list->cap = 10;
    list->args = malloc(sizeof(char*) * list->cap);
    return list;
}


// Append an argument to the list.
static void arglist_append(ArgList *list, char *arg) {
    if (list->len == list->cap) {
        list->cap *= 2;
        list->args = realloc(list->args, sizeof(char*) * list->cap);
    }
    list->args[list->len] = arg;
    list->len++;
}


// Print the ArgList instance to stdout.
static void arglist_print(ArgList *list) {
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


// An ArgParser instance is responsible for storing registered options and
// commands. Note that every registered command recursively receives an
// ArgParser instance of its own. In theory commands can be stacked to any
// depth, although in practice even two levels is confusing for users.
typedef struct ArgParser {
    char *helptext;
    char *version;
    Map *options;
    Map *commands;
    Map *callbacks;
    ArgList *arguments;
    char *cmd_name;
    ArgParser *cmd_parser;
} ArgParser;


// Free the memory associated with an ArgParser instance.
static void argparser_free(ArgParser *parser) {
    map_free(parser->options);
    map_free(parser->commands);
    map_free(parser->callbacks);
    arglist_free(parser->arguments);
    free(parser);
}


// Initialize a new ArgParser instance. Supplying help text activates the
// automatic --help flag, supplying a version string activates the automatic
// --version flag. NULL can be passed for either parameter.
static ArgParser* argparser_new(char *helptext, char *version) {
    ArgParser *parser = malloc(sizeof(ArgParser));
    parser->helptext = helptext;
    parser->version = version;
    parser->options = map_new(option_free);
    parser->commands = map_new(argparser_free);
    parser->callbacks = map_new(NULL);
    parser->arguments = arglist_new();
    parser->cmd_name = NULL;
    parser->cmd_parser = NULL;
    return parser;
}


// Register a boolean option.
static void argparser_add_flag(ArgParser *parser, char *name) {
    Option *opt = option_new_flag();
    map_add_splitkey(parser->options, name, opt);
}


// Register a string option with a default value.
static void argparser_add_str(ArgParser *parser, char *name, char* value) {
    Option *opt = option_new_str(value);
    map_add_splitkey(parser->options, name, opt);
}


// Register an integer option with a default value.
static void argparser_add_int(ArgParser *parser, char *name, int value) {
    Option *opt = option_new_int(value);
    map_add_splitkey(parser->options, name, opt);
}


// Register a float option with a default value.
static void argparser_add_float(ArgParser *parser, char *name, double value) {
    Option *opt = option_new_float(value);
    map_add_splitkey(parser->options, name, opt);
}


// Register a boolean list option.
static void argparser_add_flag_list(ArgParser *parser, char *name) {
    Option *opt = option_new_flag_list();
    map_add_splitkey(parser->options, name, opt);
}


// Register a string list option.
static void argparser_add_str_list(ArgParser *parser, char *name) {
    Option *opt = option_new_str_list();
    map_add_splitkey(parser->options, name, opt);
}


// Register an integer list option.
static void argparser_add_int_list(ArgParser *parser, char *name) {
    Option *opt = option_new_int_list();
    map_add_splitkey(parser->options, name, opt);
}


// Register a floating-point list option.
static void argparser_add_float_list(ArgParser *parser, char *name) {
    Option *opt = option_new_float_list();
    map_add_splitkey(parser->options, name, opt);
}


// Register a command and its associated callback.
static ArgParser* argparser_add_cmd(
    ArgParser *parser, char *name, CmdCB callback, char *helptext
) {
    ArgParser *cmd_parser = argparser_new(helptext, NULL);
    map_add_splitkey(parser->commands, name, cmd_parser);
    map_add_splitkey(parser->callbacks, name, callback);
    return cmd_parser;
}


// Retrieve a registered Option instance.
static Option* argparser_get_opt(ArgParser *parser, char *name) {
    Option *opt = map_get(parser->options, name);
    if (opt == NULL) {
        fprintf(stderr, "Abort: '%s' is not a registered option.\n", name);
        exit(1);
    }
    return opt;
}


// Returns the value of the specified boolean option.
static bool argparser_get_flag(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_flag(opt);
}


// Returns the value of the specified string option.
static char* argparser_get_str(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_str(opt);
}


// Returns the value of the specified integer option.
static int argparser_get_int(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_int(opt);
}


// Returns the value of the specified floating-point option.
static double argparser_get_float(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_float(opt);
}


// Returns an option's values as a freshly-allocated array of booleans. The
// array's memory is not affected by calls to argparser_free().
static bool* argparser_get_flag_list(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_flag_list(opt);
}


// Returns an option's values as a freshly-allocated array of string pointers.
// The array's memory is not affected by calls to argparser_free().
static char** argparser_get_str_list(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_str_list(opt);
}


// Returns an option's values as a freshly-allocated array of integers. The
// array's memory is not affected by calls to argparser_free().
static int* argparser_get_int_list(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_int_list(opt);
}


// Returns an option's values as a freshly-allocated array of doubles. The
// array's memory is not affected by calls to argparser_free().
static double* argparser_get_float_list(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_get_float_list(opt);
}


// Returns the length of the specified option's internal list of values.
static int argparser_len_list(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return opt->len;
}


// Clear the specified option's internal list of values.
static void argparser_clear_list(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    return option_clear(opt);
}


// Set the specified boolean option to true. (Appends to list options.)
static void argparser_set_flag(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    option_set_flag(opt);
}


// Set the specified boolean option to false. (Clears list options.)
static void argparser_unset_flag(ArgParser *parser, char *name) {
    Option *opt = argparser_get_opt(parser, name);
    option_unset_flag(opt);
}


// Set the value of the specified string option. (Appends to list options.)
static void argparser_set_str(ArgParser *parser, char *name, char *value) {
    Option *opt = argparser_get_opt(parser, name);
    option_set_str(opt, value);
}


// Set the value of the specified integer option. (Appends to list options.)
static void argparser_set_int(ArgParser *parser, char *name, int value) {
    Option *opt = argparser_get_opt(parser, name);
    option_set_int(opt, value);
}


// Set the value of the specified float option. (Appends to list options.)
static void argparser_set_float(ArgParser *parser, char *name, double value) {
    Option *opt = argparser_get_opt(parser, name);
    option_set_float(opt, value);
}


// Returns true if the parser has found one or more positional arguments.
static bool argparser_has_args(ArgParser *parser) {
    return parser->arguments->len > 0;
}


// Returns the number of positional arguments.
static int argparser_len_args(ArgParser *parser) {
    return parser->arguments->len;
}


// Returns the positional arguments as a freshly-allocated array of string
// pointers. The memory occupied by the returned array is not affected by
// calls to argparser_free().
static char** argparser_get_args(ArgParser *parser) {
    char **args = malloc(sizeof(char*) * parser->arguments->len);
    memcpy(args, parser->arguments->args, sizeof(char*) * parser->arguments->len);
    return args;
}


// Attempts to parse and return the positional arguments as a freshly-allocated
// array of integers. Exits with an error message on failure. The memory
// occupied by the returned array is not affected by calls to argparser_free().
static int* argparser_get_args_as_ints(ArgParser *parser) {
    int *args = malloc(sizeof(int) * parser->arguments->len);
    for (int i = 0; i < parser->arguments->len; i++) {
        *(args + i) = try_str_to_int(parser->arguments->args[i]);
    }
    return args;
}


// Attempts to parse and return the positional arguments as a freshly-allocated
// array of doubles. Exits with an error message on failure. The memory
// occupied by the returned array is not affected by calls to argparser_free().
static double* argparser_get_args_as_floats(ArgParser *parser) {
    double *args = malloc(sizeof(double) * parser->arguments->len);
    for (int i = 0; i < parser->arguments->len; i++) {
        *(args + i) = try_str_to_double(parser->arguments->args[i]);
    }
    return args;
}


// Returns true if the parser has found a command.
static bool argparser_has_cmd(ArgParser *parser) {
    return parser->cmd_name != NULL;
}


// Returns the command name, if the parser has found a command.
static char* argparser_get_cmd_name(ArgParser *parser) {
    return parser->cmd_name;
}


// Returns the command's parser instance, if the parser has found a command.
static ArgParser* argparser_get_cmd_parser(ArgParser *parser) {
    return parser->cmd_parser;
}










// Parse an option of the form --name=value or -n=value.
static void argparser_parse_name_equals_value(
    ArgParser *parser, char *prefix, char *arg
) {
    char *name;
    char *value;
    char *saveptr;
    char *arg_cpy = strdup(arg);

    name = strtok_r(arg_cpy, "=", &saveptr);
    value = strtok_r(NULL, "=", &saveptr);



    free(arg_cpy);





    char *key;
    char *saveptr;
    char *keystr_cpy = strdup(keystr);

    key = strtok_r(keystr_cpy, " ", &saveptr);
    while (key != NULL) {
        map_add(map, key, value);
        key = strtok_r(NULL, " ", &saveptr);
    }

    free(keystr_cpy);

}


// Parse a long-form option, i.e. an option beginning with a double dash.
static void argparser_parse_longform(
    ArgParser *parser, char *arg, ArgStream *stream
) {
    // Do we have an option of the form --name=value?
    if (strstr(arg, "=") != NULL) {
        argparser_parse_name_equals_value(parser, "--", arg);
    }

    // Is the argument a registered option name?
    else if (map_contains(parser->options, arg)) {
        Option *opt = map_get(parser->options, arg);

        // Do we have multiple instances of a mono-valued option?
        if (opt->mono && opt->found) {
            fprintf(stderr, "Error: option --%s can be set only once.\n", arg);
            exit(1);
        }





    }

    // Is the argument the automatic --help flag?
    else if (strcmp(arg, "help") == 0 && parser->helptext != NULL) {
        puts(parser->helptext);
        exit(0);
    }

    // Is the argument the automatic --version flag?
    else if (strcmp(arg, "version") == 0 && parser->version != NULL) {
        puts(parser->version);
        exit(0);
    }

    // The argument is not a registered or automatic option name.
    // Print an error message and exit.
    else {
        fprintf(stderr, "Error: --%s is not a recognised option.\n", arg);
        exit(1);
    }
}


// Parse a stream of string arguments.
static void argparser_parse_stream(ArgParser *parser, ArgStream *stream) {

    // Switch to turn off option parsing if we encounter a double dash, '--'.
    // Everything following the '--' will be treated as a positional argument.
    bool parsing = true;

    // Loop while we have arguments to process.
    while (argstream_has_next(stream)) {

        // Fetch the next argument from the stream.
        char *arg = argstream_next(stream);

        // If parsing has been turned off, simply add the argument to the list
        // of positionals.
        if (!parsing) {
            arglist_append(parser->arguments, arg);
            continue;
        }

        // If we encounter a '--' argument, turn off option-parsing.
        if (strcmp(arg, "--") == 0) {
            parsing = false;
            continue;
        }

        // Is the argument a long-form option or flag?
        else if (strncmp(arg, "--", 2) == 0) {
            argparser_parse_longform(parser, arg + 2, stream);
        }

        // Is the argument a short-form option or flag? If the argument
        // consists of a single dash or a dash followed by a digit, we treat
        // it as a positional argument.
        else if (arg[0] == '-') {
            if (strlen(arg) == 1 || isdigit(arg[1])) {
                arglist_append(parser->arguments, arg);
            } else {
                argparser_parse_shortform(parser, arg + 1, stream);
            }
        }

        // Is the argument a registered command?
        else if (map_contains(parser->commands, arg)) {
            ArgParser *cmd_parser = map_get(parser->commands, arg);
            CmdCB cmd_callback = map_get(parser->callbacks, arg);
            argparser_parse_stream(cmd_parser, stream);
            if (cmd_callback != NULL) {
                cmd_callback(cmd_parser);
            }
            parser->cmd_name = arg;
            parser->cmd_parser = cmd_parser;
        }

        // Is the argument the automatic 'help' command? The commands
        //     $ app cmd --help
        // and
        //     $ app help cmd
        // are functionally equivalent. Both will print the help text
        // associated with the command.
        else if (strcmp(arg, "help") == 0) {
            if (argstream_has_next(stream)) {
                char *name = argstream_next(stream);
                if (map_contains(parser->commands, name)) {
                    ArgParser *cmd_parser = map_get(parser->commands, name);
                    puts(cmd_parser->helptext);
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

        // Otherwise add the argument to our list of positional arguments.
        else {
            arglist_append(parser->arguments, arg);
        }

////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////
    }
}


// Parse an array of string arguments.
static void argparser_parse(ArgParser *parser, int len, char *args[]) {

    // Convert the input array into a stream.
    ArgStream *stream = argstream_new(len, args);

    // Parse the stream.
    argparser_parse_stream(parser, stream);

    // Free the memory associated with the stream.
    argstream_free(stream);
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
