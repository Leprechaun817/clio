
# Java Version

To use Clio in your Java application add the `ArgParser.java` file directly to your application's source folder. The library exports a single `ArgParser` class.


## Usage

Initialize an argument parser, optionally specifying help text and a version
string:

    ArgParser(String helptext, String version)

Supplying help text activates the automatic `--help` flag; supplying a version string activates the automatic `--version` flag.

You can now register your application's options and commands on the parser instance as explained below.

Once the required options and commands have been registered, call the parser's `parse()` method to process the application's command line arguments.

    void parse(String[] args)

Parsed option values can be retrieved from the parser instance itself.


### Options

Clio supports long-form options (`--foo`) with single-character aliases (`-f`). Note that when registering an option you should omit the leading dashes, i.e. you should register the option name as `"foo"` rather than `"--foo"`.

Registering options:

*   `void addFlag(String name, char alias)`

    Register a flag, optionally specifying a single-character alias. A flag is
    a boolean option - it takes no argument but is either present (true) or
    absent (false). The alias parameter may be omitted.

*   `void addStrOpt(String name, String defVal, char alias)`

    Register a string option and its default value, optionally specifying a
    single-character alias. The alias parameter may be omitted.

*   `void addIntOpt(String name, int defVal, char alias)`

    Register an integer option and its default value, optionally specifying a
    single-character alias. The alias parameter may be omitted.

*   `void addFloatOpt(String name, double defVal, char alias)`

    Register a floating-point option and its default value, optionally
    specifying a single-character alias. The alias parameter may be omitted.

Retrieving values:

*   `boolean getFlag(String name)`

*   `String getStrOpt(String name)`

*   `int getIntOpt(String name)`

*   `double getFloatOpt(String name)`

All options have default values which are used when the option is omitted from the command line arguments.

Note that Clio supports the standard `--` option-parsing switch. All command line arguments following a `--` will be treated as positional arguments rather than options, even if they begin with a single or double dash.


### Positional Arguments

The following methods provide access to positional arguments:

*   `boolean hasArgs()`

    Returns true if at least one positional argument has been found.

*   `int numArgs()`

    Returns the number of positional arguments.

*   `String getArg(int index)`

    Returns the positional argument at the specified index.

*   `List<String> getArgs()`

    Returns the positional arguments as a list of strings.

*   `List<Integer> getArgsAsInts()`

    Attempts to parse and return the positional arguments as a list of integers.
    Exits with an error message on failure.

*   `List<Double> getArgsAsFloats()`

    Attempts to parse and return the positional arguments as a list of floats.
    Exits with an error message on failure.


### Commands

Clio supports git-style command interfaces with arbitrarily-nested commands. Register a command on a parser using the `addCmd()` method:

    ArgParser addCmd(String command, Consumer<ArgParser> callback, String helptext)

This method returns the `ArgParser` instance associated with the new command. You can register flags and options on this sub-parser using the methods listed above. (Note that you do not need to call `parse()` on the command parser instance - calling `parse()` on the root parser is sufficient.)

Commands support an automatic `--help` flag and an automatic `help <cmd>` command.

The supplied callback function will be called if the command is found. This callback should accept the command's sub-parser instance as its sole argument.

Other command-related methods are:

*   `boolean hasCmd()`

    Returns true if the parser has identified a command.

*   `String getCmd()`

    Returns the command name, if a command was identified.

*   `ArgParser getCmdParser()`

    Returns the command parser, if a command was identified.
