// -------------------------------------------------------------------------
// Clio: a minimalist argument-parsing library designed for building elegant
// command line interfaces.
//
// Author: Darren Mulholland <darren@mulholland.xyz>
// License: Public Domain
// Version: 2.0.0
// -------------------------------------------------------------------------

import java.util.Map;
import java.util.HashMap;
import java.util.TreeMap;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.function.Consumer;


// ArgParser is the sole class exported by the library. An ArgParser instance
// is responsible for registering options and commands and parsing the input
// array of raw arguments. Note that every registered command recursively
// receives an ArgParser instance of its own.
class ArgParser {


    // Internal enum for classifying option types. We use 'flag' as a synonym
    // for boolean options, i.e. options that are either present (true) or
    // absent (false). All other option types require an argument.
    private enum OptionType {
        Flag, String, Int, Float
    }


    // Internal class for storing option data.
    private static class Option {
        OptionType type;
        boolean found;
        boolean greedy;
        List<Object> values = new ArrayList<Object>();

        Option(OptionType type) {
            this.type = type;
        }

        // Returns the last value from the option's internal list.
        Object get() {
            return values.get(values.size() - 1);
        }

        // Append a value to the option's internal list.
        void set(Object value) {
            values.add(value);
        }

        // Clear the option's internal list of values.
        void clear() {
            values.clear();
        }

        // Returns a string representation of the option's values.
        public String toString() {
            return values.toString();
        }
    }


    // Internal class for making a list of arguments available as a stream.
    private static class ArgStream {
        List<String> args;
        int length;
        int index;

        ArgStream(List<String> args) {
            this.args = args;
            this.length = args.size();
            this.index = 0;
        }

        ArgStream(String[] args) {
            this(Arrays.asList(args));
        }

        // Returns the next argument from the stream.
        String next() {
            return args.get(index++);
        }

        // Returns the next argument from the stream without consuming it.
        String peek() {
            return args.get(index);
        }

        // Returns true if the stream contains at least one more element.
        boolean hasNext() {
            return index < length;
        }

        // Returns true if the stream contains at least one more element and
        // that element has the form of an option value.
        boolean hasNextValue() {
            if (hasNext()) {
                String arg = peek();
                if (arg.startsWith("-")) {
                    if (arg.equals("-") || Character.isDigit(arg.charAt(1))) {
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return true;
                }
            }
            return false;
        }
    }


    // Help text for the application or command.
    private String helptext;


    // Application version number.
    private String version;


    // Stores options indexed by name.
    private Map<String, Option> options;


    // Stores command parsers indexed by command name.
    private Map<String, ArgParser> commands;


    // Stores command callbacks indexed by command name.
    private Map<String, Consumer<ArgParser>> callbacks;


    // Stores positional arguments.
    private List<String> arguments;


    // Stores the command name, if a command was found while parsing.
    private String cmdName;


    // Stores the command's parser instance, if a command was found.
    private ArgParser cmdParser;


    // Stores a reference to a command parser's parent parser instance.
    private ArgParser parent;


    // Initialize a parser with no automatic --help or --version flags.
    ArgParser() {
        this(null, null);
    }


    // Initialize a parser with automatic --help and --version flags.
    ArgParser(String helptext, String version) {
        this.helptext = (helptext == null) ? null : helptext.trim();
        this.version = (version == null) ? null : version.trim();
        this.options = new HashMap<String, Option>();
        this.commands = new HashMap<String, ArgParser>();
        this.callbacks = new HashMap<String, Consumer<ArgParser>>();
        this.arguments = new ArrayList<String>();
    }


    // ---------------------------------------------------------------------
    // Register options.
    // ---------------------------------------------------------------------


    // Register a boolean option with a default value of false.
    void addFlag(String name) {
        Option opt = new Option(OptionType.Flag);
        opt.set(false);
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register a string option with a default value.
    void addStr(String name, String value) {
        Option opt = new Option(OptionType.String);
        opt.set(value);
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register an integer option with a default value.
    void addInt(String name, int value) {
        Option opt = new Option(OptionType.Int);
        opt.set(value);
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register a floating-point option with a default value.
    void addFloat(String name, double value) {
        Option opt = new Option(OptionType.Float);
        opt.set(value);
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register a boolean list option.
    void addFlagList(String name) {
        Option opt = new Option(OptionType.Flag);
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register a string list option.
    void addStrList(String name, boolean greedy) {
        Option opt = new Option(OptionType.String);
        opt.greedy = greedy;
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register a non-greedy string list option.
    void addStrList(String name) {
        addStrList(name, false);
    }


    // Register an integer list option.
    void addIntList(String name, boolean greedy) {
        Option opt = new Option(OptionType.Int);
        opt.greedy = greedy;
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register a non-greedy integer list option.
    void addIntList(String name) {
        addIntList(name, false);
    }


    // Register a floating-point list option.
    void addFloatList(String name, boolean greedy) {
        Option opt = new Option(OptionType.Float);
        opt.greedy = greedy;
        for (String alias: name.split("\\s+")) {
            options.put(alias, opt);
        }
    }


    // Register a non-greedy floating-point list option.
    void addFloatList(String name) {
        addFloatList(name, false);
    }


    // ---------------------------------------------------------------------
    // Retrieve option values.
    // ---------------------------------------------------------------------


    // Returns true if the specified option was found while parsing.
    boolean found(String name) {
        return options.get(name).found;
    }


    // Returns the value of the specified boolean option.
    boolean getFlag(String name) {
        return (boolean) options.get(name).get();
    }


    // Returns the value of the specified string option.
    String getStr(String name) {
        return (String) options.get(name).get();
    }


    // Returns the value of the specified integer option.
    int getInt(String name) {
        return (int) options.get(name).get();
    }


    // Returns the value of the specified floating-point option.
    double getFloat(String name) {
        return (double) options.get(name).get();
    }


    // Returns the length of the specified option's list of values.
    int lenList(String name) {
        return options.get(name).values.size();
    }


    // Returns the specified option's values as a list of booleans.
    List<Boolean> getFlagList(String name) {
        List<Boolean> list = new ArrayList<Boolean>();
        for (Object value: options.get(name).values) {
            list.add((Boolean) value);
        }
        return list;
    }


    // Returns the specified option's values as a list of strings.
    List<String> getStrList(String name) {
        List<String> list = new ArrayList<String>();
        for (Object value: options.get(name).values) {
            list.add((String) value);
        }
        return list;
    }


    // Returns the specified option's values as a list of integers.
    List<Integer> getIntList(String name) {
        List<Integer> list = new ArrayList<Integer>();
        for (Object value: options.get(name).values) {
            list.add((Integer) value);
        }
        return list;
    }


    // Returns the specified option's values as a list of floats.
    List<Double> getFloatList(String name) {
        List<Double> list = new ArrayList<Double>();
        for (Object value: options.get(name).values) {
            list.add((Double) value);
        }
        return list;
    }


    // ---------------------------------------------------------------------
    // Set option values.
    // ---------------------------------------------------------------------


    // Clear the specified option's internal list of values.
    void clearList(String name) {
        options.get(name).clear();
    }


    // Append a value to the specified option's internal list.
    void setFlag(String name, boolean value) {
        options.get(name).set(value);
    }


    // Append a value to the specified option's internal list.
    void setStr(String name, String value) {
        options.get(name).set(value);
    }


    // Append a value to the specified option's internal list.
    void setInt(String name, int value) {
        options.get(name).set(value);
    }


    // Append a value to the specified option's internal list.
    void setFloat(String name, double value) {
        options.get(name).set(value);
    }


    // ---------------------------------------------------------------------
    // Commands.
    // ---------------------------------------------------------------------


    // Register a command with its associated callback and help text.
    ArgParser addCmd(String name, String help, Consumer<ArgParser> callback) {
        ArgParser cmdParser = new ArgParser(help, null);
        cmdParser.parent = this;
        for (String alias: name.split("\\s+")) {
            commands.put(alias, cmdParser);
            callbacks.put(alias, callback);
        }
        return cmdParser;
    }


    // Returns true if the parser has found a command.
    boolean hasCmd() {
        return cmdName != null;
    }


    // Returns the command name, if a command was found.
    String getCmdName() {
        return cmdName;
    }


    // Returns the command's parser instance, if a command was found.
    ArgParser getCmdParser() {
        return cmdParser;
    }


    // Returns a command parser's parent parser.
    ArgParser getParent() {
        return parent;
    }


    // ---------------------------------------------------------------------
    // Positional arguments.
    // ---------------------------------------------------------------------


    // Returns true if the parser has found one or more positional arguments.
    boolean hasArgs() {
        return arguments.size() > 0;
    }


    // Returns the length of the list of positional arguments.
    int lenArgs() {
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
    // arguments as a list of integers. Exits with an error message on
    // failure.
    List<Integer> getArgsAsInts() {
        List<Integer> list = new ArrayList<Integer>();
        for (String arg: arguments) {
            try {
                list.add(Integer.parseInt(arg));
            }
            catch (NumberFormatException e) {
                System.err.format("Error: cannot parse '%s' as an integer.\n", arg);
                System.exit(1);
            }
        }
        return list;
    }


    // Convenience function: attempts to parse and return the positional
    // arguments as a list of doubles. Exits with an error message on
    // failure.
    List<Double> getArgsAsFloats() {
        List<Double> list = new ArrayList<Double>();
        for (String arg: arguments) {
            try {
                list.add(Double.parseDouble(arg));
            }
            catch (NumberFormatException e) {
                System.err.format("Error: cannot parse '%s' as a float.\n", arg);
                System.exit(1);
            }
        }
        return list;
    }


    // Clears the list of positional arguments.
    void clearArgs() {
        arguments.clear();
    }


    // Appends a string to the list of positional arguments.
    void appendArg(String arg) {
        arguments.add(arg);
    }


    // ---------------------------------------------------------------------
    // Parse arguments.
    // ---------------------------------------------------------------------


    // Parse an array of string arguments.
    void parse(String[] args) {
        parse(new ArgStream(args));
    }


    // Parse a list of string arguments.
    void parse(List<String> args) {
        parse(new ArgStream(args));
    }


    // Parse a stream of string arguments.
    private void parse(ArgStream stream) {

        // Switch to turn off option parsing if we encounter a double dash.
        // Everything following the '--' will be treated as a positional
        // argument.
        boolean parsing = true;

        // Loop while we have arguments to process.
        while (stream.hasNext()) {

            // Fetch the next argument from the stream.
            String arg = stream.next();

            // If parsing has been turned off, simply add the argument to the
            // list of positionals.
            if (!parsing) {
                arguments.add(arg);
                continue;
            }

            // If we encounter a -- argument, turn off option-parsing.
            if (arg.equals("--")) {
                parsing = false;
                continue;
            }

            // Is the argument a long-form option or flag?
            if (arg.startsWith("--")) {
                parseLongOption(arg.substring(2), stream);
            }

            // Is the argument a short-form option or flag? If the argument
            // consists of a single dash or a dash followed by a digit, we
            // treat it as a positional argument.
            else if (arg.startsWith("-")) {
                if (arg.equals("-") || Character.isDigit(arg.charAt(1))) {
                    arguments.add(arg);
                } else {
                    parseShortOption(arg.substring(1), stream);
                }
            }

            // Is the argument a registered command?
            else if (commands.containsKey(arg)) {
                ArgParser cmdParser = commands.get(arg);
                Consumer<ArgParser> cmdCallback = callbacks.get(arg);
                this.cmdName = arg;
                this.cmdParser = cmdParser;
                cmdParser.parse(stream);
                cmdCallback.accept(cmdParser);
            }

            // Is the argument the automatic 'help' command?
            else if (arg.equals("help")) {
                if (stream.hasNext()) {
                    String name = stream.next();
                    if (commands.containsKey(name)) {
                        System.out.println(commands.get(name).helptext);
                        System.exit(0);
                    } else {
                        System.err.format(
                            "Error: '%s' is not a recognised command.\n", name
                        );
                        System.exit(1);
                    }
                } else {
                    System.err.println(
                        "Error: the help command requires an argument."
                    );
                    System.exit(1);
                }
            }

            // Add the argument to our list of positional arguments.
            else {
                arguments.add(arg);
            }
        }
    }


    // Parse a long-form option, i.e. an option beginning with a double dash.
    private void parseLongOption(String arg, ArgStream stream) {

        // Do we have an option of the form --name=value?
        if (arg.contains("=")) {
            parseEqualsOption("--", arg);
        }

        // Is the argument a registered option name?
        else if (options.containsKey(arg)) {
            Option opt = options.get(arg);
            opt.found = true;

            // If the option is a flag, store the boolean true.
            if (opt.type == OptionType.Flag) {
                opt.set(true);
            }

            // Check for a following argument.
            else if (stream.hasNextValue()) {

                // Try to parse the argument according to the option type.
                opt.set(tryParseArg(opt.type, stream.next()));

                // If the option is a greedy list, keep trying to parse values
                // until we run out of arguments.
                if (opt.greedy) {
                    while (stream.hasNextValue()) {
                        opt.set(tryParseArg(opt.type, stream.next()));
                    }
                }
            }

            // We're missing a required option value.
            else {
                System.err.format(
                    "Error: missing argument for the --%s option.\n", arg
                );
                System.exit(1);
            }
        }

        // Is the argument the automatic --help flag?
        else if (arg.equals("help") && this.helptext != null) {
            System.out.println(helptext);
            System.exit(0);
        }

        // Is the argument the automatic --version flag?
        else if (arg.equals("version") && this.version != null) {
            System.out.println(version);
            System.exit(0);
        }

        // The argument is not a registered or automatic option name.
        else {
            System.err.format("Error: --%s is not a recognised option.\n", arg);
            System.exit(1);
        }
    }


    // Parse a short-form option, i.e. an option beginning with a single dash.
    private void parseShortOption(String arg, ArgStream stream) {

        // Do we have an option of the form -n=value?
        if (arg.contains("=")) {
            parseEqualsOption("-", arg);
            return;
        }

        // We handle each character individually to support condensed options:
        //    -abc foo bar
        // is equivalent to:
        //    -a foo -b bar -c
        for (char c: arg.toCharArray()) {

            // Check that we have the name of a registered option.
            String key = String.valueOf(c);
            if (!options.containsKey(key)) {
                System.err.format(
                    "Error: -%s is not a recognised option.\n", key
                );
                System.exit(1);
            }
            Option opt = options.get(key);
            opt.found = true;

            // If the option is a flag, store the boolean true.
            if (opt.type == OptionType.Flag) {
                opt.set(true);
            }

            // Check for a following argument.
            else if (stream.hasNextValue()) {

                // Try to parse the argument according to the option type.
                opt.set(tryParseArg(opt.type, stream.next()));

                // If the option is a greedy list, keep trying to parse values
                // until we run out of arguments.
                if (opt.greedy) {
                    while (stream.hasNextValue()) {
                        opt.set(tryParseArg(opt.type, stream.next()));
                    }
                }
            }

            // We're missing a required option value.
            else {
                System.err.format(
                    "Error: missing argument for the -%s option.\n", key
                );
                System.exit(1);
            }
        }
    }


    // Parse an option of the form --name=value or -n=value.
    private void parseEqualsOption(String prefix, String arg) {
        String[] split = arg.split("=", 2);
        String name = split[0];
        String value = split[1];

        // Do we have the name of a registered option?
        if (!options.containsKey(name)) {
            System.err.format(
                "Error: %s%s is not a recognised option.\n", prefix, name
            );
            System.exit(1);
        }
        Option opt = options.get(name);
        opt.found = true;

        // Boolean flags should never contain an equals sign.
        if (opt.type == OptionType.Flag) {
            System.err.format(
                "Error: invalid format for boolean flag %s%s.\n", prefix, name
            );
            System.exit(1);
        }

        // Make sure we have a value after the equals sign.
        if (value.equals("")) {
            System.err.format(
                "Error: missing argument for the %s%s option.\n", prefix, name
            );
            System.exit(1);
        }

        // Try to parse the argument according to the option type.
        opt.set(tryParseArg(opt.type, value));
    }


    // Attempt to parse a string argument as a value of the appropriate type.
    private Object tryParseArg(OptionType type, String arg) {
        Object value = null;

        switch (type) {

            case String:
                value = arg;
                break;

            case Int:
                try {
                    value = Integer.parseInt(arg);
                }
                catch (NumberFormatException e) {
                    System.err.format(
                        "Error: cannot parse '%s' as an integer.\n", arg
                    );
                    System.exit(1);
                }
                break;

            case Float:
                try {
                    value = Double.parseDouble(arg);
                }
                catch (NumberFormatException e) {
                    System.err.format(
                        "Error: cannot parse '%s' as a float.\n", arg
                    );
                    System.exit(1);
                }
                break;
        }

        return value;
    }


    // ---------------------------------------------------------------------
    // Utilities.
    // ---------------------------------------------------------------------


    // Print the parser's help text and exit.
    void help() {
        System.out.println(helptext);
        System.exit(0);
    }


    // Returns a string representation of the ArgParser instance.
    public String toString() {
        StringBuilder builder = new StringBuilder();

        builder.append("Options:\n");
        if (options.size() > 0) {
            for (String key: new TreeMap<String, Option>(options).keySet()) {
                builder.append(
                    String.format("  %s: %s\n", key, options.get(key).values)
                );
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
            builder.append(String.format("  %s\n", getCmdName()));
        } else {
            builder.append("  [none]\n");
        }

        return builder.toString().trim();
    }
}
