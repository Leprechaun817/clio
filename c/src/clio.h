/*
    Clio: a minimalist argument-parsing library designed for building elegant
    command line interfaces.

    Author: Darren Mulholland <darren@mulholland.xyz>
    License: Public Domain
    Version: 2.0.0.dev
*/

#pragma once
#include <stdbool.h>

// An ArgParser instance stores registered options and commands.
typedef struct ArgParser ArgParser;

// Initialize a new ArgParser instance.
// Supplying help text activates the automatic --help flag, supplying a
// version string activates the automatic --version flag. A NULL pointer can
// be passed for either parameter.
ArgParser * clio_new(char *helptext, char *version);

// Registers a flag (a boolean option) and its single-character alias.
// A NULL pointer can be passed for the alias parameter.
void clio_add_flag(ArgParser *parser, char *name, char *alias);

// Registers a string option and its single-character alias.
// A NULL pointer can be passed for the alias parameter.
void clio_add_str(ArgParser *parser, char *name, char* def_value, char *alias);

// Registers an integer option and its single-character alias.
// A NULL pointer can be passed for the alias parameter.
void clio_add_int(ArgParser *parser, char *name, int def_value, char *alias);

// Registers a floating-point option and its single-character alias.
// A NULL pointer can be passed for the alias parameter.
void clio_add_float(ArgParser *parser, char *name, double def_value, char *alias);

// Registers a command and its associated callback function.
// The callback should accept a pointer to an ArgParser instance and return void.
ArgParser * clio_add_cmd(ArgParser *parser, char *command, void (*callback)(ArgParser *parser), char *helptext);

// Parses the supplied array of string pointers. The arguments are assumed to
// be argc and argv as supplied to main(), i.e., the first element of the
// array is assumed to be the application name and will be ignored.
void clio_parse(ArgParser *parser, int argc, char **argv);

// Returns the value of the named flag.
bool clio_get_flag(ArgParser *parser, char *name);

// Returns the value of the named string option.
char * clio_get_str(ArgParser *parser, char *name);

// Returns the value of the named integer option.
int clio_get_int(ArgParser *parser, char *name);

// Returns the value of the named floating-point option.
double clio_get_float(ArgParser *parser, char *name);

// Returns true if the parser has found one or more positional arguments.
bool clio_has_args(ArgParser *parser);

// Returns the number of positional arguments.
int clio_num_args(ArgParser *parser);

// Returns the positional arguments as an array of string pointers.
// The memory occupied by the array is not affected by calls to clio_free().
char ** clio_get_args(ArgParser *parser);

// Attempts to parse and return the positional arguments as an array of
// integers. Exits with an error message on failure.
// The memory occupied by the array is not affected by calls to clio_free().
int * clio_get_args_as_ints(ArgParser *parser);

// Attempts to parse and return the positional arguments as an array of
// doubles. Exits with an error message on failure.
// The memory occupied by the array is not affected by calls to clio_free().
double * clio_get_args_as_floats(ArgParser *parser);

// Returns true if the parser has identified a command.
bool clio_has_cmd(ArgParser *parser);

// Returns the command string, if the parser has identified a command.
char * clio_get_cmd(ArgParser *parser);

// Returns the command parser instance, if the parser has identified a command.
ArgParser * clio_get_cmd_parser(ArgParser *parser);

// Frees the memory associated with the specified ArgParser instance.
void clio_free(ArgParser *parser);

// Dumps an ArgParser instance to stdout for debugging.
void clio_dump(ArgParser *parser);
