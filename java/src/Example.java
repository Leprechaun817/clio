// -------------------------------------------------------------------------
// A simple application demonstrating Clio in action.
// -------------------------------------------------------------------------

class Example {

    // This sample application will parse its own command-line arguments.
    public static void main(String[] args) {

        // We instantiate an argument parser, optionally supplying help text
        // and a version string. Supplying help text activates the automatic
        // --help flag, supplying a version string activates the automatic
        // --version flag. We can pass null for either parameter.
        ArgParser parser = new ArgParser("Help!", "Version 1.2.3");

        // Register a flag, --bool, with a single-character alias, -b. A flag
        // is a boolean option - it's either present (true) or absent (false).
        parser.addFlag("bool b");

        // Register a string option, --string <arg>, with a single-character
        // alias, -s <arg>. A string argument requires a default value, here
        // 'default'.
        parser.addStr("string s", "default");

        // Register an integer option, --int <arg>. An integer option
        // requires a default value, here 123.
        parser.addInt("int", 123);

        // Register a float option, --float <arg>. A float option requires a
        // default value, here 1.0.
        parser.addFloat("float", 1.0);

        // Register an integer list, --intlist <arg>, with a single-character
        // alias,-i <arg>. A list option accepts multiple values.
        parser.addIntList("intlist i");

        // Register a 'greedy' float list, --floatlist <args>, with a single-
        // character alias, -f <args>. A list option accepts multiple values;
        // a 'greedy' list attempts to parse multiple consecutive arguments.
        parser.addFloatList("floatlist f", true);

        // Register a command, 'foo' with an alias 'bar'. We need to specify
        // the command's help text and callback method.
        ArgParser cmdParser = parser.addCmd(
            "foo bar", "Command!", Example::callback
        );

        // Registering a command returns a new ArgParser instance dedicated to
        // parsing the command's arguments. We can register as many flags and
        // options as we like on this sub-parser. Note that the sub-parser can
        // reuse the parent's option names without interference.
        cmdParser.addFlag("bool b");
        cmdParser.addInt("int i", 123);

        // Once all our options and commands have been registered we can call
        // the parse() method with an array of argument strings. (Note that we
        // only need to call parse() on the root parser - command arguments
        // are parsed automatically.)
        parser.parse(args);

        // We can now retrieve our option and argument values from the parser
        // instance. Here we simply dump it to stdout.
        System.out.println(parser);
    }

    // Callback method for the 'foo' command. This method will be called if
    // the command is found. The method receives an ArgParser instance
    // containing the command's parsed arguments. Here we simply dump it to
    // stdout.
    public static void callback(ArgParser parser) {
        System.out.println("---------- callback ----------");
        System.out.println(parser);
        System.out.println("------------------------------\n");
    }
}
