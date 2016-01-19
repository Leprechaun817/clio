/*
    Clio: A toolkit for creating elegant command line interfaces.

    Author: Darren Mulholland <dmulholland@outlook.ie>
    License: Public Domain
*/

import java.util.Map;
import java.util.HashMap;
import java.util.TreeMap;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.function.Consumer;


// ArgParser is the sole class exported by the library. An ArgParser instance is
// responsible for registering options and parsing the input array of raw arguments.
//
// Note that every registered command recursively receives an ArgParser instance of
// its own. In theory commands can be stacked to any depth, although in practice even
// two levels is confusing for users and best avoided.
class ArgParser {


    // Library version number.
    String libVersion = "1.0.0.dev";


    // Internal enum for classifying option types.
    // We use 'flag' as a synonym for boolean options, i.e. options that are either
    // present (true) or absent (false). All other option types require an argument.
    private enum OptionType {
        Flag, String, Int, Float
    }


    // Internal class for storing option data.
    private static class Option {
        OptionType type;
        Object value;

        Option(OptionType type, Object value) {
            this.type = type;
            this.value = value;
        }
    }


    // Internal class for making a list of arguments available as a stream.
    private static class ArgStream {
        List<String> arguments;
        int length;
        int index;

        ArgStream(List<String> arguments) {
            this.arguments = arguments;
            this.length = arguments.size();
            this.index = 0;
        }

        // Returns true if the stream contains another argument.
        boolean hasNext() {
            return index < length;
        }

        // Returns the next argument from the stream.
        String next() {
            return arguments.get(index++);
        }

        // Returns the next argument from the stream without consuming it.
        String peek() {
            return arguments.get(index);
        }

        // Returns a list containing all the remaining arguments from the stream.
        List<String> remainder() {
            return arguments.subList(index, length);
        }
    }


    // Help text for the application or command.
    private String helptext;

    // Application version number.
    private String version;

    // Stores option objects indexed by option name.
    private Map<String, Option> options = new HashMap<String, Option>();

    // Stores option objects indexed by single-letter alias.
    private Map<Character, Option> shortcuts = new HashMap<Character, Option>();

    // Stores command sub-parser instances indexed by command.
    private Map<String, ArgParser> commands = new HashMap<String, ArgParser>();

    // Stores command callbacks indexed by command.
    private Map<String, Consumer<ArgParser>> callbacks = new HashMap<String, Consumer<ArgParser>>();

    // Stores positional arguments parsed from the input array - i.e. arguments not associated with an option.
    private List<String> arguments = new ArrayList<String>();

    // Stores the command string, if a command is found while parsing.
    private String command;

    // Stores the command's parser instance, if a command is found.
    private ArgParser commandParser;


    // No automatic --help or --version flags.
    ArgParser() {
        this(null, null);
    }


    // Supplying help text activates the automatic --help flag.
    ArgParser(String helptext) {
        this(helptext, null);
    }


    // Supplying a version string activates the automatic --version flag.
    ArgParser(String helptext, String version) {
        if (helptext != null) {
            helptext = helptext.trim();
        }
        if (version != null) {
            version = version.trim();
        }
        this.helptext = helptext;
        this.version = version;
    }


    // Register a flag, i.e. a boolean option without an argument.
    void addFlag(String name) {
        options.put(name, new Option(OptionType.Flag, false));
    }


    // Register a flag, additionally specifying a single-letter alias.
    void addFlag(String name, char shortcut) {
        Option option = new Option(OptionType.Flag, false);
        options.put(name, option);
        shortcuts.put(shortcut, option);
    }


    // Register an option with a string argument.
    void addStrOpt(String name, String defaultValue) {
        options.put(name, new Option(OptionType.String, defaultValue));
    }


    // Register a string option, additionally specifying a single-letter alias.
    void addStrOpt(String name, String defaultValue, char shortcut) {
        Option option = new Option(OptionType.String, defaultValue);
        options.put(name, option);
        shortcuts.put(shortcut, option);
    }


    // Register an option with an integer argument.
    void addIntOpt(String name, int defaultValue) {
        options.put(name, new Option(OptionType.Int, defaultValue));
    }


    // Register an integer option, additionally specifying a single-letter alias.
    void addIntOpt(String name, int defaultValue, char shortcut) {
        Option option = new Option(OptionType.Int, defaultValue);
        options.put(name, option);
        shortcuts.put(shortcut, option);
    }


    // Register an option with a floating point argument.
    void addFloatOpt(String name, double defaultValue) {
        options.put(name, new Option(OptionType.Float, defaultValue));
    }


    // Register a float option, additionally specifying a single-letter alias.
    void addFloatOpt(String name, double defaultValue, char shortcut) {
        Option option = new Option(OptionType.Float, defaultValue);
        options.put(name, option);
        shortcuts.put(shortcut, option);
    }


    // Register a command and its associated callback.
    ArgParser addCmd(String command, Consumer<ArgParser> callback, String helptext) {
        ArgParser commandParser = new ArgParser(helptext);
        commands.put(command, commandParser);
        callbacks.put(command, callback);
        return commandParser;
    }


    // Print the parser's help text and exit.
    void help() {
        System.out.println(helptext);
        System.exit(0);
    }


    // Parse an array of arguments.
    void parse(String[] argArray) {
        parse(Arrays.asList(argArray));
    }


    // Parse a list of arguments.
    void parse(List<String> argList) {

        // Switch to turn off parsing if we encounter a -- argument.
        // Everything following the -- will be treated as a positional argument.
        boolean parsing = true;

        // Convert the input list into a stream.
        ArgStream stream = new ArgStream(argList);

        // Loop while we have arguments to process.
        while (stream.hasNext()) {

            // Fetch the next argument from the stream.
            String arg = stream.next();

            // If parsing has been turned off, simply add the argument to the list of positionals.
            if (!parsing) {
                arguments.add(arg);
                continue;
            }

            // If we encounter a -- argument, turn off parsing.
            if (arg.equals("--")) {
                parsing = false;
                continue;
            }

            // Is the current argument a long-form option or flag?
            if (arg.startsWith("--")) {

                // Strip the -- prefix.
                arg = arg.substring(2);

                // Is the argument a registered option name?
                if (options.containsKey(arg)) {
                    Option option = options.get(arg);

                    // If the option type is a flag, store the boolean value true.
                    if (option.type == OptionType.Flag) {
                        option.value = true;
                        continue;
                    }

                    // Not a flag, so check for a following argument.
                    if (!stream.hasNext()) {
                        System.err.format("Error: missing argument for the --%s option.\n", arg);
                        System.exit(1);
                    }

                    // Fetch the argument from the stream and attempt to parse it.
                    String nextarg = stream.next();

                    switch (option.type) {

                        case String:
                            option.value = nextarg;
                            break;

                        case Int:
                            try {
                                option.value = Integer.parseInt(nextarg);
                            }
                            catch (NumberFormatException e) {
                                System.err.format("Error: cannot parse '%s' as an integer.\n", nextarg);
                                System.exit(1);
                            }
                            break;

                        case Float:
                            try {
                                option.value = Double.parseDouble(nextarg);
                            }
                            catch (NumberFormatException e) {
                                System.err.format("Error: cannot parse '%s' as a float.\n", nextarg);
                                System.exit(1);
                            }
                            break;
                    }
                }

                // Automatic --help flag if help-text has been specified.
                else if (arg.equals("help") && helptext != null) {
                    System.out.println(helptext);
                    System.exit(0);
                }

                // Automatic --version flag is a version string has been specified.
                else if (arg.equals("version") && version != null) {
                    System.out.println(version);
                    System.exit(0);
                }

                // Not a registered or automatic option. Print an error and exit.
                else {
                    System.err.format("Error: --%s is not a recognised option.\n", arg);
                    System.exit(1);
                }
            }

            // Is the current argument a short-form option or flag.
            else if (arg.startsWith("-")) {

                // If the argument consists of a single dash or a dash followed by a digit,
                // treat it as a positional argument. (We don't support numerical shortcuts
                // as they can't be distinguished from negative integers.)
                if (arg.equals("-") || Character.isDigit(arg.charAt(1))) {
                    arguments.add(arg);
                    continue;
                }

                // Examine each character individually to allow for condensed
                // short-form arguments, i.e.
                //     -a -b foo -c bar
                // is equivalent to
                //     -abc foo bar
                for (char c: arg.substring(1).toCharArray()) {

                    // Is the character a registered shortcut?
                    if (shortcuts.containsKey(c)) {
                        Option option = shortcuts.get(c);

                        // If the option type is a flag, just store the boolean true.
                        if (option.type == OptionType.Flag) {
                            option.value = true;
                            continue;
                        }

                        // Not a flag, so check for a following argument.
                        if (!stream.hasNext()) {
                            System.err.format("Error: missing argument for the -%s option.\n", c);
                            System.exit(1);
                        }

                        // Fetch the argument from the stream and attempt to parse it.
                        String nextarg = stream.next();

                        switch (option.type) {

                            case String:
                                option.value = nextarg;
                                break;

                            case Int:
                                try {
                                    option.value = Integer.parseInt(nextarg);
                                }
                                catch (NumberFormatException e) {
                                    System.err.format("Error: cannot parse '%s' as an integer.\n", nextarg);
                                    System.exit(1);
                                }
                                break;

                            case Float:
                                try {
                                    option.value = Double.parseDouble(nextarg);
                                }
                                catch (NumberFormatException e) {
                                    System.err.format("Error: cannot parse '%s' as a float.\n", nextarg);
                                    System.exit(1);
                                }
                                break;
                        }
                    }

                    // Not a recognised shortcut. Print an error and exit.
                    else {
                        System.err.format("Error: -%s is not a recognised option.\n", c);
                        System.exit(1);
                    }
                }
            }

            // Is the current argument a registered command? If so, we create a sub-parser
            // instance of the ArgParser class and use it to process the remaining arguments.
            else if (commands.containsKey(arg)) {
                ArgParser cmdParser = commands.get(arg);
                Consumer<ArgParser> cmdCallback = callbacks.get(arg);
                cmdParser.parse(stream.remainder());
                cmdCallback.accept(cmdParser);
                this.command = arg;
                this.commandParser = cmdParser;
                break;
            }

            // Automatic 'help' command for registered commands. The commands
            //     $ app cmd --help
            // and
            //     $ app help cmd
            // are functionally equivalent. Both will print the help text
            // associated with the command.
            else if (arg.equals("help")) {
                if (stream.hasNext()) {
                    String command = stream.next();
                    if (commands.containsKey(command)) {
                        System.out.println(commands.get(command).helptext);
                        System.exit(0);
                    } else {
                        System.err.format("Error: '%s' is not a recognised command.\n", command);
                        System.exit(1);
                    }
                } else {
                    System.err.println("Error: the help command requires an argument.");
                    System.exit(1);
                }
            }

            // Otherwise, add the argument to our list of free arguments.
            else {
                arguments.add(arg);
            }
        }
    }


    // Returns true if the named flag was found.
    boolean getFlag(String name) {
        return (boolean) options.get(name).value;
    }


    // Returns the value of the named option.
    String getStrOpt(String name) {
        return (String) options.get(name).value;
    }


    // Returns the value of the named option.
    int getIntOpt(String name) {
        return (int) options.get(name).value;
    }


    // Returns the value of the named option.
    double getFloatOpt(String name) {
        return (double) options.get(name).value;
    }


    // Returns true if the parser has found one or more positional arguments.
    boolean hasArgs() {
        return arguments.size() > 0;
    }


    // Returns the number of positional arguments identified by the parser.
    int numArgs() {
        return arguments.size();
    }


    // Returns the positional argument at the specified index.
    String getArg(int index) {
        return arguments.get(index);
    }


    // Returns the positional arguments as a list of strings.
    List<String> getArgs() {
        return arguments;
    }


    // Convenience function: attempts to parse and return the positional
    // arguments as a list of integers.
    List<Integer> getArgsAsInts() {
        List<Integer> intArgs = new ArrayList<Integer>();
        for (String arg: arguments) {
            try {
                intArgs.add(Integer.parseInt(arg));
            }
            catch (NumberFormatException e) {
                System.err.format("Error: cannot parse '%s' as an integer.\n", arg);
                System.exit(1);
            }
        }
        return intArgs;
    }


    // Convenience function: attempts to parse and return the positional
    // arguments as a list of floats.
    List<Double> getArgsAsFloats() {
        List<Double> floatArgs = new ArrayList<Double>();
        for (String arg: arguments) {
            try {
                floatArgs.add(Double.parseDouble(arg));
            }
            catch (NumberFormatException e) {
                System.err.format("Error: cannot parse '%s' as a float.\n", arg);
                System.exit(1);
            }
        }
        return floatArgs;
    }


    // Returns true if the parser has found a command.
    boolean hasCmd() {
        return command != null;
    }


    // Returns the command string, if a command was found.
    String getCmd() {
        return command;
    }


    // Returns the command's parser instance, if a command was found.
    ArgParser getCmdParser() {
        return commandParser;
    }


    // List all options and arguments for debugging.
    public String toString() {
        StringBuilder builder = new StringBuilder();

        builder.append("Options:\n");
        if (options.size() > 0) {
            for (String key: new TreeMap<String, Option>(options).keySet()) {
                builder.append(String.format("  %s: %s\n", key, options.get(key).value));
            }
        } else {
            builder.append("  [none]\n");
        }

        builder.append("\nArguments:\n");
        if (arguments.size() > 0) {
            for (String arg: arguments) {
                builder.append(String.format("  %s\n", arg));
            }
        } else {
            builder.append("  [none]\n");
        }

        builder.append("\nCommand:\n");
        if (hasCmd()) {
            builder.append(String.format("  %s\n", getCmd()));
        } else {
            builder.append("  [none]\n");
        }

        return builder.toString().trim();
    }
}
