/*
    Clio: A toolkit for creating elegant command line interfaces.

      * Author: Darren Mulholland <dmulholland@outlook.ie>
      * License: Public Domain
*/
package clio


import (
    "fmt"
    "os"
    "strings"
    "strconv"
    "unicode"
    "sort"
)


// Library version number.
const Version = "0.1.0"


// Enum for classifying option types.
// We use 'flag' as a synonym for boolean options, i.e. options that are either
// present (true) or absent (false). All other option types require an argument.
const (
    FlagOption = iota
    StringOption
    IntegerOption
    FloatOption
)


// An option can have a boolean, string, integer, or floating point value.
type Option struct {
    optionType int
    boolValue bool
    strValue string
    intValue int
    floatValue float64
}


// Returns a string representation of an option element's value.
func (option *Option) String() string {
    var str string
    switch option.optionType {
    case FlagOption:
        str = fmt.Sprintf("%v", option.boolValue)
    case StringOption:
        str = option.strValue
    case IntegerOption:
        str = fmt.Sprintf("%v", option.intValue)
    case FloatOption:
        str = fmt.Sprintf("%v", option.floatValue)
    }
    return str
}


// Callback type for processing commands.
type Callback func(*ArgParser)


// Makes a slice of string arguments available as a stream.
type ArgStream struct {
    args []string
    index int
    length int
}


// Initializes a new ArgStream instance.
func newStream(args []string) *ArgStream {
    return &ArgStream{
        args: args,
        index: 0,
        length: len(args),
    }
}


// Returns true if the stream contains at least one more argument.
func (stream *ArgStream) hasNext() bool {
    return stream.index < stream.length
}


// Returns the next argument from the stream.
func (stream *ArgStream) next() string {
    stream.index += 1
    return stream.args[stream.index - 1]
}


// Returns the next argument from the stream without consuming it.
func (stream *ArgStream) peek() string {
    return stream.args[stream.index]
}


// Returns a slice containing all the remaining arguments from the stream.
func (stream *ArgStream) remainder() []string {
    return stream.args[stream.index:]
}


// ArgParser is the workhorse of the toolkit. An ArgParser instance is responsible
// for registering options and parsing the input array of raw arguments.
// Note that every registered command recursively receives an ArgParser instance of
// its own. In theory commands can be stacked to any depth, although in practice even
// two levels is confusing for users and best avoided.
type ArgParser struct {

    // Help text for the application or command.
    helptext string

    // Application version number.
    version string

    // Stores option objects indexed by option name.
    options map[string]*Option

    // Stores option objects indexed by single-letter shortcut.
    shortcuts map[rune]*Option

    // Stores command sub-parser instances indexed by command.
    commands map[string]*ArgParser

    // Stores command callbacks indexed by command.
    callbacks map[string]Callback

    // Stores positional arguments parsed from the input array.
    arguments []string

    // Stores the command string, if a command is identified.
    command string

    // Stores the command's parser instance, if a command is identified.
    commandParser *ArgParser
}


// Initialize a new ArgParser instance.
func NewParser(helptext string, version string) *ArgParser {
    return &ArgParser {
        helptext: strings.TrimSpace(helptext),
        version: strings.TrimSpace(version),
        options: make(map[string]*Option),
        shortcuts: make(map[rune]*Option),
        commands: make(map[string]*ArgParser),
        callbacks: make(map[string]Callback),
        arguments: make([]string, 0, 10),
    }
}


// Register a flag, optionally specifying a single-letter shortcut alias.
func (parser *ArgParser) AddFlag(name string, shortcuts ...rune) {
    option := Option{
        optionType: FlagOption,
        boolValue: false,
    }
    parser.options[name] = &option
    for _, shortcut := range shortcuts {
        parser.shortcuts[shortcut] = &option
    }
}


// Register a string option, optionally specifying a single-letter shortcut alias.
func (parser *ArgParser) AddStringOption(name string, defValue string, shortcuts ...rune) {
    option := Option{
        optionType: StringOption,
        strValue: defValue,
    }
    parser.options[name] = &option
    for _, shortcut := range shortcuts {
        parser.shortcuts[shortcut] = &option
    }
}


// Register an integer option, optionally specifying a single-letter shortcut alias.
func (parser *ArgParser) AddIntOption(name string, defValue int, shortcuts ...rune) {
    option := Option{
        optionType: IntegerOption,
        intValue: defValue,
    }
    parser.options[name] = &option
    for _, shortcut := range shortcuts {
        parser.shortcuts[shortcut] = &option
    }
}


// Register a float option, optionally specifying a single-letter shortcut alias.
func (parser *ArgParser) AddFloatOption(name string, defValue float64, shortcuts ...rune) {
    option := Option{
        optionType: FloatOption,
        floatValue: defValue,
    }
    parser.options[name] = &option
    for _, shortcut := range shortcuts {
        parser.shortcuts[shortcut] = &option
    }
}


// Register a command and its associated callback.
func (parser *ArgParser) AddCommand(command string, callback Callback, helptext string) *ArgParser {
    cmdParser := NewParser(helptext, "")
    parser.commands[command] = cmdParser
    parser.callbacks[command] = callback
    return cmdParser
}


// Print the parser's help text and exit.
func (parser *ArgParser) Help() {
    fmt.Println(parser.helptext)
    os.Exit(0)
}


// Parse a slice of string arguments.
func (parser *ArgParser) ParseArgs(args []string) {

    // Switch to turn off parsing if we encounter a -- argument.
    // Everything following the -- will be treated as a positional argument.
    parsing := true

    // Convert the input slice into a stream.
    stream := newStream(args)

    // Loop while we have arguments to process.
    for stream.hasNext() {

        // Fetch the next argument from the stream.
        arg := stream.next()

        // If parsing has been turned off, simply add the argument to the list of positionals.
        if !parsing {
            parser.arguments = append(parser.arguments, arg)
            continue
        }

        // If we encounter a -- argument, turn off option parsing.
        if arg == "--" {
            parsing = false
            continue
        }

        // Is the argument a long-form option or flag?
        if strings.HasPrefix(arg, "--") {
            arg = arg[2:]

            // Is the argument a registered option name?
            if option, ok := parser.options[arg]; ok {

                // If the option is a flag, store the boolean true.
                if option.optionType == FlagOption {
                    option.boolValue = true
                    continue
                }

                // Not a flag, so check for a following argument.
                if !stream.hasNext() {
                    fmt.Fprintf(os.Stderr, "Error: missing argument for the --%v option.\n", arg)
                    os.Exit(1)
                }

                // We have a following argument. Attempt to parse it.
                nextarg := stream.next()

                switch option.optionType {

                case StringOption:
                    option.strValue = nextarg

                case IntegerOption:
                    intValue, err := strconv.ParseInt(nextarg, 0, 0)
                    if err != nil {
                        fmt.Fprintf(os.Stderr, "Error: cannot parse '%v' as an integer.\n", nextarg)
                        os.Exit(1)
                    }
                    option.intValue = int(intValue)

                case FloatOption:
                    floatValue, err := strconv.ParseFloat(nextarg, 64)
                    if err != nil {
                        fmt.Fprintf(os.Stderr, "Error: cannot parse '%v' as a float.\n", nextarg)
                        os.Exit(1)
                    }
                    option.floatValue = floatValue
                }

                // We have successfully parsed a long-form option with an argument.
                // Move on to the next argument in the stream.
                continue
            }

            // Is the argument the automatic --help command?
            if arg == "help" && parser.helptext != "" {
                fmt.Println(parser.helptext)
                os.Exit(0)
            }

            // Is the argument the automatic --version command.
            if arg == "version" && parser.version != "" {
                fmt.Println(parser.version)
                os.Exit(0)
            }

            // The argument is not a registered or automatic option.
            // Print an error message and exit.
            fmt.Fprintf(os.Stderr, "Error: --%v is not a recognised option.\n", arg)
            os.Exit(1)
        }


        // Is the argument a short-form option or flag?
        if strings.HasPrefix(arg, "-"){

            // If the argument consists of a sigle dash or a dash followed by
            // a digit, treat it as a positional argument.
            if arg == "-" || unicode.IsDigit([]rune(arg)[1]) {
                parser.arguments = append(parser.arguments, arg)
                continue
            }

            // Examine each character individually to allow for condensed
            // short-form arguments, i.e.
            //     -a -b foo -c bar
            // is equivalent to:
            //     -abc foo bar
            for _, c := range arg[1:] {

                // Is the character a registered shortcut?
                if option, ok := parser.shortcuts[c]; ok {

                    // If the option is a flag, store the boolean true.
                    if option.optionType == FlagOption {
                        option.boolValue = true
                        continue
                    }

                    // Not a flag, so check for a following argument.
                    if !stream.hasNext() {
                        fmt.Fprintf(os.Stderr, "Error: missing argument for the -%c option.\n", c)
                        os.Exit(1)
                    }

                    // We have a following argument. Attempt to parse it.
                    nextarg := stream.next()

                    switch option.optionType {

                    case StringOption:
                        option.strValue = nextarg

                    case IntegerOption:
                        intValue, err := strconv.ParseInt(nextarg, 0, 0)
                        if err != nil {
                            fmt.Fprintf(os.Stderr, "Error: cannot parse '%v' as an integer.\n", nextarg)
                            os.Exit(1)
                        }
                        option.intValue = int(intValue)

                    case FloatOption:
                        floatValue, err := strconv.ParseFloat(nextarg, 64)
                        if err != nil {
                            fmt.Fprintf(os.Stderr, "Error: cannot parse '%v' as a float.\n", nextarg)
                            os.Exit(1)
                        }
                        option.floatValue = floatValue
                    }

                    // We have successfully parsed a single short-form option.
                    // Move on to the next short-form option in the block.
                    continue
                }

                // Not a registered shortcut. Print an error and exit.
                fmt.Fprintf(os.Stderr, "Error: -%c is not a recognised option.\n", c)
                os.Exit(1)
            }

            // We have successfully parsed a block of short-form options.
            // Move on to the next argument in the stream.
            continue
        }

        // Is the argument a registered command?
        if cmdParser, ok := parser.commands[arg]; ok {
            cmdParser.ParseArgs(stream.remainder())
            parser.callbacks[arg](cmdParser)
            parser.command = arg
            parser.commandParser = cmdParser
            break
        }

        // Is the argument the automatic 'help' command?
        if arg == "help"{
            if stream.hasNext() {
                command := stream.next()
                if cmdParser, ok := parser.commands[command]; ok {
                    fmt.Println(cmdParser.helptext)
                    os.Exit(0)
                } else {
                    fmt.Fprintf(os.Stderr, "Error: '%v' is not a recognised command.\n", command)
                    os.Exit(1)
                }
            } else {
                fmt.Fprintf(os.Stderr, "Error: the help command requires an argument.\n")
                os.Exit(1)
            }
        }

        // If we get here, we have a positional argument.
        parser.arguments = append(parser.arguments, arg)
    }
}


// Parse the application's command line arguments.
func (parser *ArgParser) Parse() {
    parser.ParseArgs(os.Args[1:])
}


// Returns true if the flag was present.
func (parser *ArgParser) GetFlag(name string) bool {
    return parser.options[name].boolValue
}


// Returns the value of a string option.
func (parser *ArgParser) GetStringOption(name string) string {
    return parser.options[name].strValue
}


// Returns the value of an integer option.
func (parser *ArgParser) GetIntOption(name string) int {
    return parser.options[name].intValue
}


// Returns the value of a float option.
func (parser *ArgParser) GetFloatOption(name string) float64 {
    return parser.options[name].floatValue
}


// Returns true if the parser has identified one or more positional arguments.
func (parser *ArgParser) HasArgs() bool {
    return len(parser.arguments) > 0
}


// Returns the positional arguments as a slice of strings.
func (parser *ArgParser) GetArgs() []string {
    return parser.arguments
}


// Convenience function: attempts to parse and return the positional
// arguments as a list of integers.
func (parser *ArgParser) GetArgsAsInts() []int {
    intList := make([]int, 0, 10)
    for _, strArg := range parser.arguments {
        intArg, err := strconv.ParseInt(strArg, 0, 0)
        if err != nil {
            fmt.Fprintf(os.Stderr, "Error: cannot parse '%v' as an integer.\n", strArg)
            os.Exit(1)
        }
        intList = append(intList, int(intArg))
    }
    return intList
}


// Convenience function: attempts to parse and return the positional
// arguments as a list of floats.
func (parser *ArgParser) GetArgsAsFloats() []float64 {
    floatList := make([]float64, 0, 10)
    for _, strArg := range parser.arguments {
        floatArg, err := strconv.ParseFloat(strArg, 64)
        if err != nil {
            fmt.Fprintf(os.Stderr, "Error: cannot parse '%v' as a float.\n", strArg)
            os.Exit(1)
        }
        floatList = append(floatList, floatArg)
    }
    return floatList
}


// Returns true if the parser has identified a registered command.
func (parser *ArgParser) HasCommand() bool {
    return parser.command != ""
}


// Returns the command string, if a command was found.
func (parser *ArgParser) GetCommand() string {
    return parser.command
}


// Returns the command's parser instance, if a command was found.
func (parser *ArgParser) GetCommandParser() *ArgParser {
    return parser.commandParser
}


// List all options and arguments for debugging.
func (parser *ArgParser) String() string {
    lines := make([]string, 0, 10)

    lines = append(lines, "Options:")
    if len(parser.options) > 0 {
        names := make([]string, 0, len(parser.options))
        for name := range parser.options {
            names = append(names, name)
        }
        sort.Strings(names)
        for _, name := range names {
            lines = append(lines, fmt.Sprintf("  %v: %v", name, parser.options[name]))
        }
    } else {
        lines = append(lines, "  [none]")
    }

    lines = append(lines, "\nArguments:")
    if len(parser.arguments) > 0 {
        for _, arg := range parser.arguments {
            lines = append(lines, fmt.Sprintf("  %v", arg))
        }
    } else {
        lines = append(lines, "  [none]")
    }

    lines = append(lines, "\nCommand:")
    if parser.HasCommand() {
        lines = append(lines, fmt.Sprintf("  %v", parser.GetCommand()))
    } else {
        lines = append(lines, "  [none]")
    }

    return strings.Join(lines, "\n")
}
