
# Clio: Python

Clio is a minimalist argument-parsing library implemented consistently across multiple programming languages. This is the Python implementation.

Clio supports long and short-form options with boolean, string, integer, and floating-point values. It also supports git-style command interfaces with arbitrarily-nested commands.


## Installation

Install:

    $ pip install pyclio

Import:

    >>> import clio

Clio requires Python 3.


## Usage

Initialize an argument parser, optionally specifying help text and a version
string:

    parser = clio.ArgParser("helptext", "version")

Supplying help text activates the automatic `--help` flag; supplying a version string activates the automatic `--version` flag.

You can now register your application's options and commands on the parser instance as explained below.

Once the required options and commands have been registered, call the parser's `parse()` method to process the application's command line arguments.

    parser.parse()

Parsed option values can be retrieved from the parser instance itself.


### Options

Clio supports long-form options (`--foo`) with single-character aliases (`-f`). Note that when registering an option you should omit the leading dashes, i.e. you should register the option name `"foo"` rather than `"--foo"`.

Option methods:

*   `parser.add_flag(name, alias=None)`

    Register a flag, optionally specifying a single-character alias. A flag is a boolean option - it takes no argument but is either present (true) or absent (false).

*   `parser.add_str_opt(name, default, alias=None)`

    Register a string option and its default value, optionally specifying a
    single-character alias.

*   `parser.add_int_opt(name, default, alias=None)`

    Register an integer option and its default value, optionally specifying a
    single-character alias.

*   `parser.add_float_opt(name, default, alias=None)`

    Register a float option and its default value, optionally specifying a
    single-character alias.

*   `parser.get_opt(name)`

    Retrieve an option value from a parser.

All options have default values which are used when the option is omitted from the command line arguments.

Note that Clio supports the standard `--` option-parsing switch. All command line arguments following a `--` will be treated as positional arguments rather than options, even if they begin with a single or double dash.


### Positional Arguments

The following methods provide access to positional arguments identified by the parser:

*   `parser.has_args()`

    Returns true if at least one positional argument has been found.

*   `parser.num_args()`

    Returns the number of positional arguments.

*   `parser.get_args()`

    Returns the positional arguments as a list of strings.

*   `parser.get_args_as_ints()`

    Attempts to parse and return the positional arguments as a list of integers.

*   `parser.get_args_as_floats()`

    Attempts to parse and return the positional arguments as a list of floats.


### Commands

Clio supports git-style command interfaces with arbitrarily-nested commands. Register a command on a parser using the `add_cmd()` method:

    cmd_parser = parser.add_cmd(name, callback, "helptext")

This method returns an `ArgParser` instance associated with the new command. You can register flags and options on this sub-parser using the methods listed above. (Note that you do not need to call `parse()` on the sub-parser instance - calling `parse()` on the application's root parser is sufficient.)

Commands support an automatic `--help` flag and an automatic `help <cmd>` command.

The supplied callback function will be called if the command is found by the parser. This function should accept the command's sub-parser instance as its sole argument.

Other command-related methods are:

*   `parser.has_cmd()`

    Returns true if the parser locates a command.

*   `parser.get_cmd()`

    Returns the command name, if a command was located.

*   `parser.get_cmd_parser()`

    Returns the command parser, if a command was located.


## License

This work has been placed in the public domain.