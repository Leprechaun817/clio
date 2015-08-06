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


// We use the Clio class as a namespace wrapper so we can squeeze
// the entire toolkit into a single file.
public class Clio {


    // Library version number.
    public String clioVersion = "0.1.0";


    // Internal enum for classifying option types.
    // We use 'flag' as a synonym for boolean options.
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


    // ArgParser is the workhorse class of the toolkit. An ArgParser instance is responsible
    // for registering options and parsing the input array of raw arguments.
    // Note that every registered command recursively receives an ArgParser instance of
    // its own. In theory commands can be stacked to any depth, although in practice even
    // two levels is confusing for users and best avoided.
    public static class ArgParser {

        // Help text for the application or command.
        private String helptext;

        // Application version number.
        private String version;

        // Stores option objects indexed by option name.
        private Map<String, Option> optionsByName = new HashMap<String, Option>();

        // Stores option objects indexed by single-letter shortcut.
        private Map<String, Option> optionsByShortcut = new HashMap<String, Option>();

        // Stores command sub-parser instances indexed by command.
        private Map<String, ArgParser> commandParsers = new HashMap<String, ArgParser>();

        // Stores command callbacks indexed by command.
        private Map<String, Consumer<ArgSet>> commandCallbacks = new HashMap<String, Consumer<ArgSet>>();

        // Stores free arguments parsed from the input array - i.e. arguments not associated with an option.
        private List<String> freeArgs = new ArrayList<String>();


        // Specifying a string of help-text activates the automatic --help flag.
        ArgParser(String helptext) {
            this(helptext, null);
        }


        // Specifying a version string activates the automatic --version flag.
        ArgParser(String helptext, String version) {
            this.helptext = helptext;
            this.version = version;
        }


        // Register a flag, i.e. a boolean option without an argument.
        void addFlag(String name) {
            optionsByName.put(name, new Option(OptionType.Flag, false));
        }


        // Register a flag, additionally specifying a single-letter shortcut alias.
        void addFlag(String name, String shortcut) {
            Option option = new Option(OptionType.Flag, false);
            optionsByName.put(name, option);
            optionsByShortcut.put(shortcut, option);
        }


        // Register an option with a string argument.
        void addStringOption(String name, String defaultValue) {
            optionsByName.put(name, new Option(OptionType.String, defaultValue));
        }


        // Register a string option, additionally specifying a single-letter shortcut alias.
        void addStringOption(String name, String shortcut, String defaultValue) {
            Option option = new Option(OptionType.String, defaultValue);
            optionsByName.put(name, option);
            optionsByShortcut.put(shortcut, option);
        }


        // Register an option with an integer argument.
        void addIntOption(String name, int defaultValue) {
            optionsByName.put(name, new Option(OptionType.Int, defaultValue));
        }


        // Register an integer option, additionally specifying a single-letter shortcut alias.
        void addIntOption(String name, String shortcut, int defaultValue) {
            Option option = new Option(OptionType.Int, defaultValue);
            optionsByName.put(name, option);
            optionsByShortcut.put(shortcut, option);
        }


        // Register an option with a floating point argument.
        void addFloatOption(String name, double defaultValue) {
            optionsByName.put(name, new Option(OptionType.Float, defaultValue));
        }


        // Register a float option, additionally specifying a single-letter shortcut alias.
        void addFloatOption(String name, String shortcut, double defaultValue) {
            Option option = new Option(OptionType.Float, defaultValue);
            optionsByName.put(name, option);
            optionsByShortcut.put(shortcut, option);
        }


        // Register a command and its associated callback.
        ArgParser addCommand(String command, String helptext, Consumer<ArgSet> callback) {
            ArgParser commandParser = new ArgParser(helptext);
            commandParsers.put(command, commandParser);
            commandCallbacks.put(command, callback);
            return commandParser;
        }


        // Parse an array of arguments.
        ArgSet parse(String[] argArray) {
            return parse(Arrays.asList(argArray));
        }


        // Parse a list of arguments.
        ArgSet parse(List<String> argList) {

            // Index for looping through the argument list.
            int index = -1;

            // Switch to turn off option parsing if we encounter a -- argument.
            // Subsequent arguments beginning with a dash will be treated as free
            // arguments instead of options.
            boolean parsingOptions = true;

            // Loop over the argument list.
            while (++index < argList.size()) {

                // If we encounter a -- argument, turn off option parsing for subsequent arguments.
                if (parsingOptions && argList.get(index).equals("--")) {
                    parsingOptions = false;
                }

                // Is the current argument a long-form option or flag?
                else if (parsingOptions && argList.get(index).startsWith("--")) {
                    String argString = argList.get(index).substring(2);

                    // Is the argument a registered option?
                    if (optionsByName.containsKey(argString)) {
                        Option option = optionsByName.get(argString);

                        // If the option type is a flag, just store the boolean true.
                        if (option.type == OptionType.Flag) {
                            option.value = true;
                        }

                        // Otherwise, check for a following argument and parse it appropriately.
                        else if (++index < argList.size()) {
                            String nextarg = argList.get(index);
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

                        // No following argument, so print an error message and exit.
                        else {
                            System.err.format("Error: missing argument for the --%s option.\n", argString);
                            System.exit(1);
                        }
                    }

                    // Automatic --help flag if help-text has been specified.
                    else if (argString.equals("help") && helptext != null) {
                        System.out.println(helptext);
                        System.exit(0);
                    }

                    // Automatic --version flag is a version string has been specified.
                    else if (argString.equals("version") && version != null) {
                        System.out.println(version);
                        System.exit(0);
                    }

                    // Not a registered or automatic option. Print an error and exit.
                    else {
                        System.err.format("Error: --%s is not a recognised option.\n", argString);
                        System.exit(1);
                    }
                }

                // Is the current argument a short-form option or flag.
                else if (parsingOptions && argList.get(index).startsWith("-")) {

                    // Split the argument string into an array of single-character strings.
                    // This allows for condensed short-form arguments, i.e.
                    //     -a -b foo -c bar
                    // is equivalent to
                    //     -abc foo bar
                    for (String s: argList.get(index).substring(1).split("(?!^)")) {

                        // Is the character a registered shortcut?
                        if (optionsByShortcut.containsKey(s)) {
                            Option option = optionsByShortcut.get(s);

                            // If the option type is a flag, just store the boolean true.
                            if (option.type == OptionType.Flag) {
                                option.value = true;
                            }

                            // Otherwise, check for a following argument and parse it appropriately.
                            else if (++index < argList.size()) {
                                String nextarg = argList.get(index);
                                switch (option.type) {
                                    case String:
                                        option.value = argList.get(index);
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

                            // No following argument, so print an error message and exit.
                            else {
                                System.err.format("Error: missing argument for the -%s option.\n", s);
                                System.exit(1);
                            }
                        }

                        // Not a recognised shortcut. Print an error and exit.
                        else {
                            System.err.format("Error: -%s is not a recognised option.\n", s);
                            System.exit(1);
                        }
                    }
                }

                // Is the current argument a registered command? If so, we create a sub-parser
                // instance of the ArgParser class and use it to process the remaining arguments.
                // We then pass the resulting ArgSet to the callback associated with the command.
                else if (commandParsers.containsKey(argList.get(index))) {
                    ArgParser parser = commandParsers.get(argList.get(index));
                    Consumer<ArgSet> callback = commandCallbacks.get(argList.get(index));
                    ArgSet argSet = parser.parse(argList.subList(++index, argList.size()));
                    callback.accept(argSet);
                    break;
                }

                // Automatic 'help' command for registered commands. The commands
                //     app cmd --help
                // and
                //     app help cmd
                // are functionally equivalent. Both will print the help text
                // associated with the command.
                else if (argList.get(index).equals("help")) {
                    if (++index < argList.size()) {
                        String command = argList.get(index);
                        if (commandParsers.containsKey(command)) {
                            System.out.println(commandParsers.get(command).helptext);
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

                // Otherwise, argument to our list of free arguments.
                else {
                    freeArgs.add(argList.get(index));
                }
            }

            return new ArgSet(optionsByName, freeArgs);
        }
    }


    // An ArgSet instance represents a set of parsed arguments.
    public static class ArgSet {

        // Stores option objects indexed by option name.
        private Map<String, Option> options;

        // Stores the set's free arguments.
        private List<String> arguments;


        ArgSet(Map<String, Option> options, List<String> arguments) {
            this.options = options;
            this.arguments = arguments;
        }


        // Returns true if the flag was present in the arguments.
        boolean getFlag(String name) {
            return (boolean) options.get(name).value;
        }


        // Option accessors: the folowing methods return the option value
        // corresponding to the specified name.
        String getStringOption(String name) {
            return (String) options.get(name).value;
        }

        int getIntOption(String name) {
            return (int) options.get(name).value;
        }

        Double getFloatOption(String name) {
            return (Double) options.get(name).value;
        }


        // Returns true if the set contains one or more free arguments.
        boolean hasArgs() {
            return arguments.size() > 0;
        }


        // Returns the set's free arguments as a list of strings.
        List<String> getArgs() {
            return arguments;
        }


        // Convenience function: attempts to parse and return the set's free
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


        // Convenience function: attempts to parse and return the set's free
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


        // List all options and arguments for debugging.
        public String toString() {
            StringBuilder builder = new StringBuilder();

            builder.append("Options:\n");
            if (options.size() > 0) {
                for (String key: new TreeMap<String, Option>(options).keySet()) {
                    builder.append(String.format("  %s: %s\n", key, options.get(key).value));
                }
            } else {
                builder.append("  [none]");
            }

            builder.append("\nArguments:\n");
            if (arguments.size() > 0) {
                for (String arg: arguments) {
                    builder.append(String.format("  %s\n", arg));
                }
            } else {
                builder.append("  [none]");
            }

            return builder.toString().trim();
        }
    }
}
