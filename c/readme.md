
# C Version

To use Clio in your application add the `clio.c` and `clio.h` files to your source folder and include the header file:

    #include "clio.h"

The header file exports an `ArgParser` type and a selection of `clio_*` prefixed functions.


## Usage

Initialize an argument parser, optionally specifying help text and a version
string:

    ArgParser * clio_new(char *helptext, char *version)

Supplying help text activates the automatic `--help` flag; supplying a version string activates the automatic `--version` flag. A `NULL` pointer can be passed
for either parameter.

You can now register your application's options and commands on the parser instance as explained below.

Once the required options and commands have been registered, call the `clio_parse()` method to process the application's command line arguments.

    void clio_parse(ArgParser *parser, int argc, char **argv)

The arguments are assumed to be `argc` and `argv` as supplied to `main()`, i.e., the first element of the array is assumed to be the application name and will be ignored.

Parsed option values can be retrieved from the parser instance itself.


### Options

Clio supports long-form options (`--foo`) with single-character aliases (`-f`). Note that when registering an option you should omit the leading dashes, i.e. you should register the option name as `"foo"` rather than `"--foo"`.

Registering options:

*   `void clio_add_flag(ArgParser *parser, char *name, char *alias)`

    Register a flag, optionally specifying a single-character alias. A flag is
    a boolean option - it takes no argument but is either present (true) or
    absent (false). A `NULL` pointer can be passed for the alias parameter.

*   `void clio_add_str(ArgParser *parser, char *name, char* def_value, char *alias)`

    Register a string option and its default value, optionally specifying a
    single-character alias. A `NULL` pointer can be passed for the alias parameter.

*   `void clio_add_int(ArgParser *parser, char *name, int def_value, char *alias)`

    Register an integer option and its default value, optionally specifying a
    single-character alias. A `NULL` pointer can be passed for the alias parameter.

*   `void clio_add_float(ArgParser *parser, char *name, double def_value, char *alias)`

    Register a floating-point option and its default value, optionally specifying a
    single-character alias. A `NULL` pointer can be passed for the alias parameter.

Retrieving values:

*   `bool clio_get_flag(ArgParser *parser, char *name)`

*   `char * clio_get_str(ArgParser *parser, char *name)`

*   `int clio_get_int(ArgParser *parser, char *name)`

*   `double clio_get_float(ArgParser *parser, char *name)`

All options have default values which are used when the option is omitted from the command line arguments.

Note that Clio supports the standard `--` option-parsing switch. All command line arguments following a `--` will be treated as positional arguments rather than options, even if they begin with a single or double dash.


### Positional Arguments

The following methods provide access to positional arguments:

*   `bool clio_has_args(ArgParser *parser)`

    Returns true if at least one positional argument has been found.

*   `int clio_num_args(ArgParser *parser)`

    Returns the number of positional arguments.

*   `char ** clio_get_args(ArgParser *parser)`

    Returns the positional arguments as an array of string pointers.
    The memory occupied by the array is not affected by calls to `clio_free()`.

*   `int * clio_get_args_as_ints(ArgParser *parser)`

    Attempts to parse and return the positional arguments as an array of
    integers. Exits with an error message on failure.
    The memory occupied by the array is not affected by calls to `clio_free()`.

*   `double * clio_get_args_as_floats(ArgParser *parser)`

    Attempts to parse and return the positional arguments as an array of
    doubles. Exits with an error message on failure.
    The memory occupied by the array is not affected by calls to `clio_free()`.


### Commands

Clio supports git-style command interfaces with arbitrarily-nested commands. Register a command on a parser using the `clio_add_cmd()` method:

    ArgParser * clio_add_cmd(ArgParser *parser, char *command, void (*callback)(ArgParser *parser), char *helptext)

This method returns the `ArgParser` instance associated with the new command. You can register flags and options on this sub-parser using the methods listed above. (Note that you do not need to call `clio_parse()` on the command parser instance - calling `clio_parse()` on the root parser is sufficient.)

Commands support an automatic `--help` flag and an automatic `help <cmd>` command.

The command's callback function should accept a pointer to an `ArgParser` instance and return `void`. This function will be called if the command is found.

Other command-related methods are:

*   `bool clio_has_cmd(ArgParser *parser)`

    Returns true if the parser has identified a command.

*   `char * clio_get_cmd(ArgParser *parser)`

    Returns the command name, if a command was identified.

*   `ArgParser * clio_get_cmd_parser(ArgParser *parser)`

    Returns the command parser, if a command was identified.


### Memory Management

Call the `clio_free()` function to free the memory associated with an individual parser instance and any associated command sub-parsers:

    void clio_free(ArgParser *parser)
