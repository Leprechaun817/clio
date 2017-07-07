package main


import (
	"fmt"
	"github.com/dmulholland/clio/go/clio"
)


// This sample application will parse its own command-line arguments.
func main() {

	// We instantiate an argument parser, optionally supplying help text and
	// a version string. Supplying help text activates the automatic --help
	// flag, supplying a version string activates the automatic --version
	// flag. Empty strings "" can be passed to avoid activating either.
	parser := clio.NewParser("Help!", "Version 1.2.3")

	// Register a flag, --bool, with a single-character alias, -b. A flag is a
    // boolean option - it's either present (true) or absent (false).
    parser.AddFlag("bool b")

	// Register a string option, --string <arg>, with a single-character
    // alias, -s <arg>. A string argument requires a default value, here
    // 'default'.
    parser.AddStr("string s", "default")

	// Register an integer option, --int <arg>. An integer option requires a
    // default value, here 123.
	parser.AddInt("int", 123)

	// Register a float option, --float <arg>. A float option requires a
	// default value, here 1.0.
	parser.AddFloat("float", 1.0)

	// Register an integer list, --intlist <arg>, with a single-character
    // alias, -i <arg>. A list option accepts multiple values. The final
    // parameter specifies that the option is not 'greedy'.
	parser.AddIntList("intlist i", false)

	// Register a 'greedy' float list, --floatlist <args>, with a single-
    // character alias, -f <args>. A list option accepts multiple values; a
    // 'greedy' list attempts to parse multiple consecutive arguments.
	parser.AddFloatList("floatlist f", true)

	// Register a command 'foo', with an alias 'bar'. We need to supply the
    // command's help text and callback method.
	cmdParser := parser.AddCmd("foo bar", "Command!", callback)

	// Registering a command returns a new ArgParser instance dedicated to
    // parsing the command's arguments. We can register as many flags and
    // options as we like on this sub-parser. Note that the sub-parser can
    // reuse the parent's option names without interference.
	cmdParser.AddFlag("bool b")
	cmdParser.AddInt("int i", 123)

	// Once all our options and commands have been registered we can call the
	// parser's Parse() method to parse the application's command line
	// arguments. Only the root parser's Parse() method should be called -
	// command arguments will be parsed automatically.
	parser.Parse()

    // We can now retrieve our option and argument values from the parser
    // instance. Here we simply dump the parser to stdout.
    fmt.Println(parser)
}


// Callback function for the 'foo' command. The function receives an ArgParser
// instance containing the command's parsed arguments. Here we simply dump it
// to stdout.
func callback(parser *clio.ArgParser) {
	fmt.Println("---------- callback ----------")
	fmt.Println(parser)
	fmt.Println("------------------------------\n")
}
