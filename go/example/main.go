/*
    A simple application demonstrating Clio in action.

      * Author: Darren Mulholland <dmulholland@outlook.ie>
      * License: Public Domain
*/
package main


import "fmt"
import "github.com/dmulholland/clio/go/clio"


func main() {

    // We instantiate an argument parser, optionally supplying help text
    // and a version string. Supplying help text activates the automatic
    // --help flag, supplying a version string activates the automatic
    // --version flag. Empty strings "" can be passed to avoid activating either.
    parser := clio.NewParser("Usage: example...", "1.0.0")

    // Register two flags, --bool1 and --bool2.
    // The second flag has a single-character alias, -b.
    // A flag is a boolean option - it is either present (true) or
    // absent (false).
    parser.AddFlag("bool1");
    parser.AddFlag("bool2", 'b');

    // Register two string options, --str1 <arg> and --str2 <arg>.
    // The second option has a single-character alias, -s <arg>.
    // Options require default values, here 'alice' and 'bob'.
    parser.AddStringOption("str1", "alice");
    parser.AddStringOption("str2", "bob", 's');

    // Register two integer options, --int1 <arg> and --int2 <arg>.
    // The second option has a single-character alias, -i <arg>.
    // Options require default values, here 123 and 456.
    parser.AddIntOption("int1", 123);
    parser.AddIntOption("int2", 456, 'i');

    // Register two floating point options, --float1 <arg> and --float2 <arg>.
    // The second option has a single-character alias, -f <arg>.
    // Options require default values, here 1.0 and 2.0.
    parser.AddFloatOption("float1", 1.0);
    parser.AddFloatOption("float2", 2.0, 'f');

    // Register a command, 'cmd'. We need to specify the command's help text and callback method.
    cmdParser := parser.AddCommand("cmd", cmdCallback, "Usage: example cmd...");

    // Registering a command returns a new ArgParser instance dedicated to parsing the command's
    // arguments. We can register as many flags and options as we like on this sub-parser.
    cmdParser.AddFlag("foo");

    // The command parser can reuse the parent parser's option names without interference.
    cmdParser.AddStringOption("str1", "ciara");
    cmdParser.AddStringOption("str2", "dave", 's');

    // Once all our options and commands have been registered we can call the parser's
    // Parse() method to parse the application's command line arguments. Only the root
    // parser's Parse() method should be called - command arguments will be parsed
    // automatically.
    parser.Parse()

    // We can now retrieve our option and argument values from the parser instance.
    // Here we simply dump it to stdout.
    fmt.Println(parser)
}


// Callback method for the 'cmd' command.
// This method will be called if the 'cmd' command is identified.
// The method receives an ArgParser instance containing the command's
// parsed arguments. Here we simply dump it to stdout.
func cmdCallback(parser *clio.ArgParser) {
    fmt.Println("---------- cmdCallback() ----------");
    fmt.Println(parser);
    fmt.Println("...................................\n");
}
