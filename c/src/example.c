/*
    A simple application demonstrating Clio in action.

    Author: Darren Mulholland <dmulholland@outlook.ie>
    License: Public Domain
*/

#include <stdlib.h>
#include <stdio.h>
#include "clio.h"


// Callback function for the 'cmd' command. The function receives a pointer
// to an ArgParser instance containing the command's parsed arguments. Here
// we simply dump the parser to stdout.
void callback(ArgParser *parser) {
    puts("---------- callback() ----------");
    clio_print(parser);
    puts("...................................\n");
}


// Application entry point.
int main(int argc, char **argv) {

    // We instantiate an argument parser, optionally supplying help text
    // and a version string. Supplying help text activates the automatic
    // --help flag, supplying a version string activates the automatic
    // --version flag.
    ArgParser *parser = clio("Usage: example...", "1.0.0");

    // Register two flags, --bool1 and --bool2.
    // The second flag has a single-character alias, -b.
    // A flag is a boolean option - it is either present (true) or
    // absent (false).
    clio_add_flag(parser, "bool1", NULL);
    clio_add_flag(parser, "bool2", "b");

    // Register two string options, --str1 <arg> and --str2 <arg>.
    // The second option has a single-character alias, -s <arg>.
    // Options require default values, here 'alice' and 'bob'.
    clio_add_str(parser, "str1", "alice", NULL);
    clio_add_str(parser, "str2", "bob", "s");

    // Register two integer options, --int1 <arg> and --int2 <arg>.
    // The second option has a single-character alias, -i <arg>.
    // Options require default values, here 123 and 456.
    clio_add_int(parser, "int1", 123, NULL);
    clio_add_int(parser, "int2", 456, "i");

    // Register two floating point options, --float1 <arg> and --float2 <arg>.
    // The second option has a single-character alias, -f <arg>.
    // Options require default values, here 1.0 and 2.0.
    clio_add_float(parser, "float1", 1.0, NULL);
    clio_add_float(parser, "float2", 2.0, "f");

    // Register a command, 'cmd'.
    // We need to specify the command's help text and callback method.
    ArgParser *cmd_parser = clio_add_cmd(parser, "cmd", callback, "Usage: example cmd...");

    // Registering a command returns a new ArgParser instance dedicated
    // to parsing the command's arguments. We can register as many flags
    // and options as we like on this sub-parser.
    clio_add_flag(cmd_parser, "foo", NULL);

    // The command parser can safely reuse the parent parser's option names.
    clio_add_str(cmd_parser, "str1", "ciara", NULL);
    clio_add_str(cmd_parser, "str2", "dave", NULL);

    // Once all our options and commands have been registered we call
    // clio_parse() on the root parser.
    clio_parse(parser, argc, argv);

    // We can now retrieve our option and argument values from the parser instance.
    // Here we simply dump it to stdout.
    clio_print(parser);

    // Calling clio_free() frees the memory associated with the parser and any
    // associated command parsers.
    clio_free(parser);
}
