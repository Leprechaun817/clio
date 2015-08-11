/*
    A simple application demonstrating Clio in action.

    Author: Darren Mulholland <dmulholland@outlook.ie>
    License: Public Domain
*/

class Example {

    // Application entry point.
    public static void main(String[] args) {

        // We instantiate an argument parser, optionally supplying help text
        // and a version string. Supplying help text activates the automatic
        // --help flag, supplying a version string activates the automatic
        // --version flag.
        Clio.ArgParser parser = new Clio.ArgParser("Usage: example...", "1.0.0");

        // Register two flags, --bool1 and --bool2.
        // The second flag has a single-character alias, -b.
        // A flag is a boolean option - it is either present (true) or
        // absent (false).
        parser.addFlag("bool1");
        parser.addFlag("bool2", 'b');

        // Register two string options, --str1 <arg> and --str2 <arg>.
        // The second option has a single-character alias, -s <arg>.
        // Options require default values, here 'alice' and 'bob'.
        parser.addStringOption("str1", "alice");
        parser.addStringOption("str2", "bob", 's');

        // Register two integer options, --int1 <arg> and --int2 <arg>.
        // The second option has a single-character alias, -i <arg>.
        // Options require default values, here 123 and 456.
        parser.addIntOption("int1", 123);
        parser.addIntOption("int2", 456, 'i');

        // Register two floating point options, --float1 <arg> and --float2 <arg>.
        // The second option has a single-character alias, -f <arg>.
        // Options require default values, here 1.0 and 2.0.
        parser.addFloatOption("float1", 1.0);
        parser.addFloatOption("float2", 2.0, 'f');

        // Register a command, 'cmd'. We need to specify the command's help text and callback method.
        Clio.ArgParser cmdParser = parser.addCommand("cmd", Example::cmdCallback, "Usage: example cmd...");

        // Registering a command returns a new ArgParser instance dedicated to parsing the command's
        // arguments. We can register as many flags and options as we like on this sub-parser.
        cmdParser.addFlag("foo");

        // The command parser can reuse the parent parser's option names without interfering.
        cmdParser.addStringOption("str1", "ciara");
        cmdParser.addStringOption("str2", "dave", 's');

        // Once all our options and commands have been registered we can call the parser's
        // parse() method with a list or array of argument strings. Only the root parser's
        // parse() method should be called - command arguments will be parsed automatically.
        Clio.ArgSet argset = parser.parse(args);

        // The parse() method returns an ArgSet instance. Here we simply dump it to stdout.
        System.out.println(argset);
    }


    // Callback method for the 'cmd' command.
    // This method will be called if the 'cmd' command is present.
    // The method receives an ArgSet instance containing the command's
    // parsed arguments. Here we simply dump it to stdout.
    public static void cmdCallback(Clio.ArgSet argset) {
        System.out.println("---------- cmdCallback() ----------");
        System.out.println(argset);
        System.out.println("...................................\n");
    }
}
