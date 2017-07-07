// -------------------------------------------------------------------------
// A simple application demonstrating Clio in action.
// -------------------------------------------------------------------------

#include <iostream>
#include "clio.h"

using namespace std;


// Callback function for the 'foo' command. The function receives a reference
// to an ArgParser instance containing the command's parsed arguments. Here
// we simply print the parser to stdout.
void callback(clio::ArgParser& parser) {
    cout << "---------- callback ----------\n";
    parser.print();
    cout << "------------------------------\n\n";
}


// This sample application will parse its own command-line arguments.
int main(int argc, char **argv) {

    // We instantiate an argument parser, optionally supplying help text and
    // a version string. Supplying help text activates the automatic --help
    // flag, supplying a version string activates the automatic --version
    // flag. We can pass an empty string for either parameter.
    clio::ArgParser parser("Help!", "Version 1.2.3");

    // Register a flag, --bool, with a single-character alias, -b. A flag is a
    // boolean option - it's either present (true) or absent (false).
    parser.addFlag("bool b");

    // Register a string option, --string <arg>, with a single-character
    // alias, -s <arg>. A string argument requires a default value, here
    // 'defval'.
    parser.addStr("string s", "defval");

    // Register an integer option, --int <arg>. An integer option requires a
    // default value, here 123.
    parser.addInt("int", 123);

    // Register a float option, --float <arg>. A float option requires a
    // default value, here 1.0.
    parser.addFloat("float", 1.0);

    // Register an integer list, --intlist <arg>, with a single-character
    // alias, -i <arg>. A list option accepts multiple values. The final
    // parameter specifies that the option is not 'greedy'.
    parser.addIntList("intlist i", false);

    // Register a 'greedy' float list, --floatlist <args>, with a single-
    // character alias, -f <args>. A list option accepts multiple values; a
    // 'greedy' list attempts to parse multiple consecutive arguments.
    parser.addFloatList("floatlist f", true);

    // Register a command 'foo', with an alias 'bar'. We need to supply the
    // command's help text and callback method.
    clio::ArgParser& cmd = parser.addCmd("foo bar", "Command!", callback);

    // Registering a command returns a new ArgParser instance dedicated to
    // parsing the command's arguments. We can register as many flags and
    // options as we like on this sub-parser. Note that the sub-parser can
    // reuse the parent's option names without interference.
    cmd.addFlag("bool b");
    cmd.addInt("int i", 123);

    // Once all our options and commands have been registered we can call the
    // parse() function with an array of argument strings. (Note that we
    // only need to call parse() on the root parser - command arguments
    // are parsed automatically.)
    parser.parse(argc, argv);

    // We can now retrieve our option and argument values from the parser
    // instance. Here we simply dump the parser to stdout.
    parser.print();
}
