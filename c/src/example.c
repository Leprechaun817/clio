// -------------------------------------------------------------------------
// A simple application demonstrating Clio in action.
// -------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include "clio.h"


// Callback function for the 'foo' command. The function receives a pointer
// to an ArgParser instance containing the command's parsed arguments. Here
// we simply dump the parser to stdout.
void callback(ArgParser *parser) {
    puts("-------------------- callback() --------------------");
    ap_print(parser);
    puts("....................................................\n");
}


// This sample application will parse its own command-line arguments.
int main(int argc, char **argv) {

    // We instantiate an argument parser, optionally supplying help text and a
    // version string. Supplying help text activates the automatic --help flag,
    // supplying a version string activates the automatic --version flag.
    // We can pass NULL for either parameter.
    ArgParser *parser = ap_new("Help!", "Version!");

    // Register a flag, --bopt, with a single-character alias, -b. A flag is a
    // boolean option - it's either present (true) or absent (false).
    ap_add_flag(parser, "bopt b");

    // Register a string option, --sopt <arg>, with a single-character alias,
    // -s <arg>. A string argument requires a default value, here 'defval'.
    ap_add_str(parser, "sopt s", "defval");

    // Register an integer option, --iopt <arg>. An integer option requires a
    // default value, here 123.
    ap_add_int(parser, "iopt", 123);

    // Register a float option, --fopt <arg>. A float option requires a default
    // value, here 1.0.
    ap_add_float(parser, "fopt", 1.0);

    // Register an integer list, --ilist <arg>, with a single-character alias,
    // -i <arg>. A list option accepts multiple values. The final parameter
    // specifies that the option is not 'greedy'.
    ap_add_int_list(parser, "ilist i", false);

    // Register a 'greedy' float list, --flist <args>, with a single-character
    // alias, -f <args>. A list option accepts multiple values; a 'greedy' list
    // attempts to parse multiple consecutive arguments.
    ap_add_float_list(parser, "flist f", true);

    // Register a command 'foo', with an alias 'bar'. We need to supply the
    // command's help text and callback method.
    ArgParser *cmd_parser = ap_add_cmd(parser, "foo bar", "Command!", callback);

    // Registering a command returns a new ArgParser instance dedicated to
    // parsing the command's arguments. We can register as many flags and
    // options as we like on this sub-parser. Note that the sub-parser can
    // reuse the parent's option names without interference.
    ap_add_flag(cmd_parser, "bopt b");
    ap_add_int(cmd_parser, "iopt i", 123);

    // Once all our options and commands have been registered we can call the
    // ap_parse() function with an array of argument strings. (Note that we
    // only need to call ap_parse() on the root parser - command arguments
    // are parsed automatically.)
    ap_parse(parser, argc, argv);

    // We can now retrieve our option and argument values from the parser
    // instance. Here we simply dump the parser to stdout.
    ap_print(parser);
}
