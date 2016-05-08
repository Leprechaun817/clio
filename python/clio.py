# --------------------------------------------------------------------------
# Clio: a minimalist argument-parsing library for building elegant command
# line interfaces
#
# Author: Darren Mulholland <darren@mulholland.xyz>
# License: Public Domain
# --------------------------------------------------------------------------

import sys


# Library version number.
__version__ = "2.0.0.dev"


# Print a message to stderr and exit with a non-zero error code.
def err(msg):
    sys.exit("Error: %s." % msg)


# Internal class for storing option data. Type is one of 'bool', string',
# 'int', or 'float'. A mono-valued option takes a single value.
class Option:

    def __init__(self, type, mono=False):
        self.type = type
        self.mono = mono
        self.found = False
        self.values = []

    # Appends a value to the option's internal list.
    def append(self, value):
        self.values.append(value)

    # Clears the option's internal list of values.
    def clear(self):
        self.values.clear()


# Internal class for making a list of arguments available as a stream.
class ArgStream:

    def __init__(self, args):
        self.args = list(args)
        self.length = len(self.args)
        self.index = 0

    # Returns true if the stream contains another argument.
    def has_next(self):
        return self.index < self.length

    # Returns the next argument from the stream.
    def next(self):
        self.index += 1
        return self.args[self.index - 1]

    # Returns the next argument from the stream without consuming it.
    def peek(self):
        return self.args[self.index]

    # Returns a list containing all the remaining arguments from the stream.
    def remainder(self):
        return self.args[self.index:]


# ArgParser is the workhorse class of the toolkit. An ArgParser instance is
# responsible for registering options and parsing the input array of raw
# arguments. Note that every registered command recursively receives an
# ArgParser instance of its own. In theory commands can be stacked to any depth,
# although in practice even two levels is confusing for users and best avoided.
class ArgParser:

    # Specifying a string of help text activates the automatic --help flag.
    # Specifying a version string activates the automatic --version flag.
    def __init__(self, helptext=None, version=None):

        # Command line help text for the application or command.
        self.helptext = helptext.strip() if helptext else None

        # Application version number as a string.
        self.version = version.strip() if version else None

        # Stores options indexed by name and alias.
        self.options = {}

        # Stores command sub-parser instances indexed by command name.
        self.commands = {}

        # Stores command callbacks indexed by command.
        self.callbacks = {}

        # Stores positional arguments parsed from the input stream.
        self.arguments = []

        # Stores the command string, if a command is found while parsing.
        self.command = None

        # Stores the command's parser instance, if a command is found.
        self.command_parser = None

    # Enable dictionary/list-style access to options and arguments.
    def __getitem__(self, key):
        if isinstance(key, int):
            return self.arguments[key]
        else:
            option = self.options.get(key)
            if option:
                if option.mono:
                    return option.values[0]
                else:
                    return option.values
            else:
                err("'%s' is not a recognised option" % key)

    # List all options and arguments for debugging.
    def __str__(self):
        lines = []

        lines.append("Options:")
        if len(self.options):
            for name, option in sorted(self.options.items()):
                if option.mono:
                    lines.append("  %s: %s" % (name, self._get_mono_opt(name)))
                else:
                    lines.append("  %s: %s" % (name, self._get_poly_opt(name)))
        else:
            lines.append("  [none]")

        lines.append("\nArguments:")
        if len(self.arguments):
            for arg in self.arguments:
                lines.append("  %s" % arg)
        else:
            lines.append("  [none]")

        lines.append("\nCommand:")
        if self.has_cmd():
            lines.append("  %s" % self.get_cmd())
        else:
            lines.append("  [none]")

        return "\n".join(lines)

    # Register a mono-valued option.
    def _add_mono_opt(self, type, name, default):
        option = Option(type, mono=True)
        option.append(default)
        for element in name.split():
            self.options[element] = option

    # Register a boolean option.
    def add_flag(self, name):
        self._add_mono_opt("bool", name, False)

    # Register a string option.
    def add_str(self, name, default):
        self._add_mono_opt("string", name, default)

    # Register an integer option.
    def add_int(self, name, default):
        self._add_mono_opt("int", name, default)

    # Register a float option.
    def add_float(self, name, default):
        self._add_mono_opt("float", name, default)

    # Register a poly-valued option.
    def _add_poly_opt(self, type, name):
        option = Option(type)
        for element in name.split():
            self.options[element] = option

    # Register a boolean list option.
    def add_flag_list(self, name):
        self._add_poly_opt("bool", name)

    # Register a string list option.
    def add_str_list(self, name):
        self._add_poly_opt("string", name)

    # Register an integer list option.
    def add_int_list(self, name):
        self._add_poly_opt("int", name)

    # Register a float list option.
    def add_float_list(self, name):
        self._add_poly_opt("float", name)

    # Register a command and its associated callback.
    def add_cmd(self, command, callback, helptext):
        parser = ArgParser(helptext)
        for element in command.split():
            self.commands[element] = parser
            self.callbacks[element] = callback
        return parser

    # Print the parser's help text and exit.
    def help(self):
        sys.stdout.write(self.helptext + "\n")
        sys.exit()

    # Parse a list of string arguments.
    def parse(self, args=sys.argv[1:]):

        # Switch to turn off parsing if we encounter a -- argument. Everything
        # following the -- will be treated as a positional argument.
        parsing = True

        # Convert the input list into a stream.
        stream = ArgStream(args)

        # Loop while we have arguments to process.
        while stream.has_next():

            # Fetch the next argument from the stream.
            arg = stream.next()

            # If parsing has been turned off, simply add the argument to the
            # list of positionals.
            if not parsing:
                self.arguments.append(arg)
                continue

            # If we encounter a -- argument, turn off parsing.
            if arg == "--":
                parsing = False

            # Is the argument a long-form option?
            elif arg.startswith("--"):
                self._parse_longform_option(arg[2:], stream)

            # Is the argument a short-form option? If the argument consists of
            # a single dash or a dash followed by a digit, we treat it as a
            # positional argument.
            elif arg.startswith("-"):
                if arg == '-' or arg[1].isdigit():
                    self.arguments.append(arg)
                else:
                    self._parse_shortform_option(arg[1:], stream)

            # Is the argument a registered command?
            elif arg in self.commands:
                cmd_parser = self.commands[arg]
                cmd_callback = self.callbacks[arg]
                cmd_parser.parse(stream.remainder())
                cmd_callback(cmd_parser)
                self.command = arg
                self.command_parser = cmd_parser
                break

            # Is the argument the automatic 'help' command?
            elif arg == "help":
                if stream.has_next():
                    command = stream.next()
                    if command in self.commands:
                        sys.stdout.write(self.commands[command].helptext + "\n")
                        sys.exit()
                    else:
                        err("'%s' is not a recognised command" % command)
                else:
                    err("the help command requires an argument")

            # Otherwise, add the argument to our list of free arguments.
            else:
                self.arguments.append(arg)

    # Attempt to parse the specified argument as a string, integer, or float.
    # (Parsing as a string is a null operation.)
    def _try_parse_arg(self, argtype, arg):
        if argtype == "string":
            return arg
        elif argtype == "int":
            try:
                return int(arg)
            except ValueError:
                err("cannot parse '%s' as an integer" % arg)
        elif argtype == "float":
            try:
                return float(arg)
            except ValueError:
                err("cannot parse '%s' as a float" % arg)
        else:
            assert False, "invalid argument type '%s'" % argtype

    # Parse an option of the form --name=value or -n=value.
    def _parse_name_equals_value_option(self, prefix, arg):
        name, value = arg.split("=", maxsplit=1)

        option = self.options.get(name)
        if not option:
            err("%s%s is not a recognized option" % (prefix, name))

        if option.type == "bool":
            err("invalid format for boolean flag %s%s" % (prefix, name))

        if not value:
            err("missing argument for option %s%s" % (prefix, name))

        self._set_opt(name, self._try_parse_arg(option.type, value))

    # Parse a long-form option, i.e. an option beginning with a double dash.
    def _parse_longform_option(self, arg, stream):

        # Do we have an option of the form --name=value?
        if "=" in arg:
            self._parse_name_equals_value_option("--", arg)

        # Is the argument a registered option name?
        elif arg in self.options:
            option = self.options[arg]

            # If the option is a flag, store the boolean true.
            if option.type == "bool":
                self.set_flag(arg)

            # Check for a following argument.
            elif stream.has_next() and _is_arg(stream.peek()):

                # Try to parse the argument as a value of the appropriate type.
                value = self._try_parse_arg(option.type, stream.next())
                self._set_opt(arg, value)

                # If the option is a multi-value list, keep trying to parse
                # values until we hit the next option or the end of the stream.
                if not option.mono:
                    while stream.has_next() and _is_arg(stream.peek()):
                        value = self._try_parse_arg(option.type, stream.next())
                        self._set_opt(arg, value)

            # We're missing a required argument.
            else:
                err("missing argument for the --%s option" % arg)

        # Is the argument the automatic --help flag?
        elif arg == "help" and self.helptext is not None:
            sys.stdout.write(self.helptext + "\n")
            sys.exit()

        # Is the argument the automatic --version flag?
        elif arg == "version" and self.version is not None:
            sys.stdout.write(self.version + "\n")
            sys.exit()

        # The argument is not a registered or automatic option name.
        # Print an error message and exit.
        else:
            err("--%s is not a recognised option" % arg)

    # Parse a short-form option, i.e. an option beginning with a single dash.
    def _parse_shortform_option(self, arg, stream):

        # Do we have an option of the form -n=value?
        if "=" in arg:
            self._parse_name_equals_value_option("-", arg)
            return

        # We handle each character individually to support condensed options:
        #   -abc foo bar
        # is equivalent to:
        #   -a foo -b bar -c
        for char in arg:

            option = self.options.get(char)
            if not option:
                err("-%s is not a recognised option" % char)

            # If the option is a flag, store the boolean true.
            if option.type == "bool":
                self.set_flag(char)

            # Check for a following argument.
            elif stream.has_next() and _is_arg(stream.peek()):

                # Try to parse the argument as a value of the appropriate type.
                value = self._try_parse_arg(option.type, stream.next())
                self._set_opt(char, value)

                # If the option is a multi-value list, keep trying to parse
                # values until we hit the next option or the end of the stream.
                if not option.mono:
                    while stream.has_next() and _is_arg(stream.peek()):
                        value = self._try_parse_arg(option.type, stream.next())
                        self._set_opt(char, value)

            # We're missing a required argument.
            else:
                err("missing argument for the -%s option" % char)

    # Returns the value of the specified option.
    def _get_mono_opt(self, name):
        option = self.options.get(name)
        if option:
            return option.values[0]
        else:
            err("'%s' is not a recognized option" % name)

    # Returns the value of the specified option.
    def get_flag(self, name):
        return self._get_mono_opt(name)

    # Returns the value of the specified option.
    def get_str(self, name):
        return self._get_mono_opt(name)

    # Returns the value of the specified option.
    def get_int(self, name):
        return self._get_mono_opt(name)

    # Returns the value of the specified option.
    def get_float(self, name):
        return self._get_mono_opt(name)

    # Returns the length of the specified option list.
    def len_list(self, name):
        option = self.options.get(name)
        if option:
            return len(option.values)
        else:
            err("'%s' is not a recognized option" % name)

    # Clears the specified option's internal list of values.
    def clear_list(self, name):
        option = self.options.get(name)
        if option:
            option.clear()
        else:
            err("'%s' is not a recognized option" % name)

    # Returns the values of the specified poly-valued option.
    def _get_poly_opt(self, name):
        option = self.options.get(name)
        if option:
            return option.values
        else:
            err("'%s' is not a recognized option" % name)

    # Returns the values of the specified option list.
    def get_flag_list(self, name):
        return self._get_poly_opt(name)

    # Returns the values of the specified option list.
    def get_str_list(self, name):
        return self._get_poly_opt(name)

    # Returns the values of the specified option list.
    def get_int_list(self, name):
        return self._get_poly_opt(name)

    # Returns the values of the specified option list.
    def get_float_list(self, name):
        return self._get_poly_opt(name)

    # Sets the value of the specified option. (Appends to list options.)
    def _set_opt(self, name, value):
        option = self.options.get(name)
        if option:
            if option.mono:
                option.values[0] = value
            else:
                option.append(value)
        else:
            err("'%s' is not a recognized option" % name)

    # Sets the specified flag to true. (Appends to list options.)
    def set_flag(self, name):
        return self._set_opt(name, True)

    # Sets the specified flag to false. (Clears list options.)
    def unset_flag(self, name):
        option = self.options.get(name)
        if option:
            if option.mono:
                option.values[0] = False
            else:
                option.clear()
        else:
            err("'%s' is not a recognized option" % name)

    # Sets the value of the specified option. (Appends to list options.)
    def set_str(self, name, value):
        return self._set_opt(name, value)

    # Sets the value of the specified option. (Appends to list options.)
    def set_int(self, name, value):
        return self._set_opt(name, value)

    # Sets the value of the specified option. (Appends to list options.)
    def set_float(self, name, value):
        return self._set_opt(name, value)

    # Returns true if at least one positional argument has been found.
    def has_args(self):
        return len(self.arguments) > 0

    # Returns the length of the positional argument list.
    def len_args(self):
        return len(self.arguments)

    # Returns the positional argument at the specified index.
    def get_arg(self, index):
        return self.arguments[index]

    # Returns the positional arguments as a list of strings.
    def get_args(self):
        return self.arguments

    # Convenience function: attempts to parse and return the positional
    # arguments as a list of integers.
    def get_args_as_ints(self):
        args = []
        for arg in self.arguments:
            try:
                args.append(int(arg))
            except ValueError:
                err("cannot parse '%s' as an integer" % arg)
        return args

    # Convenience function: attempts to parse and return the positional
    # arguments as a list of floats.
    def get_args_as_floats(self):
        args = []
        for arg in self.arguments:
            try:
                args.append(float(arg))
            except ValueError:
                err("cannot parse '%s' as a float" % arg)
        return args

    # Returns true if the parser has found a registered command.
    def has_cmd(self):
        return self.command is not None

    # Returns the command string, if a command was found.
    def get_cmd(self):
        return self.command

    # Returns the command's parser instance, if a command was found.
    def get_cmd_parser(self):
        return self.command_parser


# Returns true if 'arg' looks like an argument rather than an option.
def _is_arg(arg):
    if arg.startswith('-'):
        if arg in ('-', '--') or arg[1].isdigit():
            return True
        else:
            return False
    else:
        return True
