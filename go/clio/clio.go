/*
    Package clio is a minimalist argument-parsing library designed for
    building elegant command-line interfaces.
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


// Package version number.
const Version = "2.0.0.beta"


// Print a message to stderr and exit with an error code.
func exit(msg string) {
    fmt.Fprintf(os.Stderr, "Error: %v.\n", msg)
    os.Exit(1)
}


// -------------------------------------------------------------------------
// Options
// -------------------------------------------------------------------------


// Enum for classifying option types. We use 'flag' as a synonym for boolean
// options, i.e. options that are either present (true) or absent (false). All
// other option types require an argument.
const (
    flagOpt = iota
    strOpt
    intOpt
    floatOpt
)


// Union combining all four valid types of option value.
type optionValue struct {
    boolVal bool
    strVal string
    intVal int
    floatVal float64
}


// Internal type for storing option data.
type option struct {
    optType int
    found bool
    greedy bool
    values []optionValue
}


// Clear an option's internal list of values.
func (opt *option) clear() {
    opt.values = nil
}


// Append a value to a boolean option's internal list.
func (opt *option) setFlag(value bool) {
    opt.values = append(opt.values, optionValue{boolVal: value})
}


// Append a value to a string option's internal list.
func (opt *option) setStr(value string) {
    opt.values = append(opt.values, optionValue{strVal: value})
}


// Append a value to an integer option's internal list.
func (opt *option) setInt(value int) {
    opt.values = append(opt.values, optionValue{intVal: value})
}


// Append a value to a floating-point option's internal list.
func (opt *option) setFloat(value float64) {
    opt.values = append(opt.values, optionValue{floatVal: value})
}


// Try setting an option by parsing the value of a string argument. Exit
// with an error message on failure.
func (opt *option) trySet(arg string) {
    switch opt.optType {

    case strOpt:
        opt.setStr(arg)

    case intOpt:
        intVal, err := strconv.ParseInt(arg, 0, 0)
        if err != nil {
            exit(fmt.Sprintf("cannot parse '%v' as an integer", arg))
        }
        opt.setInt(int(intVal))

    case floatOpt:
        floatVal, err := strconv.ParseFloat(arg, 64)
        if err != nil {
            exit(fmt.Sprintf("cannot parse '%v' as a float", arg))
        }
        opt.setFloat(floatVal)
    }
}


// Initialize a boolean option with a default value.
func newFlag(value bool) *option {
    opt := &option{
        optType: flagOpt,
    }
    opt.setFlag(value)
    return opt
}


// Initialize a string option with a default value.
func newStr(value string) *option {
    opt := &option{
        optType: strOpt,
    }
    opt.setStr(value)
    return opt
}


// Initialize an integer option with a default value.
func newInt(value int) *option {
    opt := &option{
        optType: intOpt,
    }
    opt.setInt(value)
    return opt
}


// Initialize a floating-point option with a default value.
func newFloat(value float64) *option {
    opt := &option{
        optType: floatOpt,
    }
    opt.setFloat(value)
    return opt
}


// Initialize a boolean list option.
func newFlagList() *option {
    opt := &option{
        optType: flagOpt,
    }
    return opt
}


// Initialize a string list option.
func newStrList(greedy bool) *option {
    opt := &option{
        optType: strOpt,
    }
    opt.greedy = greedy
    return opt
}


// Initialize an integer list option.
func newIntList(greedy bool) *option {
    opt := &option{
        optType: intOpt,
    }
    opt.greedy = greedy
    return opt
}


// Initialize a floating-point list option.
func newFloatList(greedy bool) *option {
    opt := &option{
        optType: floatOpt,
    }
    opt.greedy = greedy
    return opt
}


// Returns the value of a boolean option.
func (opt *option) getFlag() bool {
    return opt.values[len(opt.values) - 1].boolVal
}


// Returns the value of a string option.
func (opt *option) getStr() string {
    return opt.values[len(opt.values) - 1].strVal
}


// Returns the value of an integer option.
func (opt *option) getInt() int {
    return opt.values[len(opt.values) - 1].intVal
}


// Returns the value of a floating-point option.
func (opt *option) getFloat() float64 {
    return opt.values[len(opt.values) - 1].floatVal
}


// Returns a list option's values as a slice of booleans.
func (opt *option) getFlagList() []bool {
    values := make([]bool, 0, len(opt.values))
    for _, optVal := range opt.values {
        values = append(values, optVal.boolVal)
    }
    return values
}


// Returns a list option's values as a slice of strings.
func (opt *option) getStrList() []string {
    values := make([]string, 0, len(opt.values))
    for _, optVal := range opt.values {
        values = append(values, optVal.strVal)
    }
    return values
}


// Returns a list option's values as a slice of integers.
func (opt *option) getIntList() []int {
    values := make([]int, 0, len(opt.values))
    for _, optVal := range opt.values {
        values = append(values, optVal.intVal)
    }
    return values
}


// Returns a list option's values as a slice of floats.
func (opt *option) getFloatList() []float64 {
    values := make([]float64, 0, len(opt.values))
    for _, optVal := range opt.values {
        values = append(values, optVal.floatVal)
    }
    return values
}


// -------------------------------------------------------------------------
// ArgStream
// -------------------------------------------------------------------------


// Makes a slice of string arguments available as a stream.
type argStream struct {
    args []string
    index int
    length int
}


// Initialize a new argStream instance.
func newArgStream(args []string) *argStream {
    return &argStream{
        args: args,
        index: 0,
        length: len(args),
    }
}


// Returns the next argument from the stream.
func (stream *argStream) next() string {
    stream.index += 1
    return stream.args[stream.index - 1]
}


// Returns the next argument from the stream without consuming it.
func (stream *argStream) peek() string {
    return stream.args[stream.index]
}


// Returns true if the stream contains at least one more element.
func (stream *argStream) hasNext() bool {
    return stream.index < stream.length
}


// Returns true if the stream contains at least one more element and that
// element has the form of an option value.
func (stream *argStream) hasNextValue() bool {
    if stream.hasNext() {
        next := stream.peek()
        if strings.HasPrefix(next, "-") {
            if next == "-" || unicode.IsDigit([]rune(next)[1]) {
                return true
            } else {
                return false
            }
        } else {
            return true
        }
    }
    return false
}


// -------------------------------------------------------------------------
// ArgParser
// -------------------------------------------------------------------------


// Command callback.
type cmdCallback func(*ArgParser)


// An ArgParser instance is responsible for storing registered options and
// commands. Note that every registered command recursively receives an
// ArgParser instance of its own. In theory commands can be stacked to any
// depth, although in practice even two levels is confusing for users.
type ArgParser struct {

    // Help text for the application or command.
    helptext string

    // Application version number.
    version string

    // Stores option objects indexed by option name.
    options map[string]*option

    // Stores command sub-parser instances indexed by command name.
    commands map[string]*ArgParser

    // Stores command callbacks indexed by command.
    callbacks map[string]cmdCallback

    // Stores positional arguments parsed from the input array.
    arguments []string

    // Stores the command name, if a command is found while parsing.
    cmdName string

    // Stores the command's parser instance, if a command is found.
    cmdParser *ArgParser
}


// NewParser initializes a new ArgParser instance.
func NewParser(helptext string, version string) *ArgParser {
    return &ArgParser {
        helptext: strings.TrimSpace(helptext),
        version: strings.TrimSpace(version),
        options: make(map[string]*option),
        commands: make(map[string]*ArgParser),
        callbacks: make(map[string]cmdCallback),
        arguments: make([]string, 0),
    }
}


// -------------------------------------------------------------------------
// ArgParser: registering options.
// -------------------------------------------------------------------------


// AddFlag registers a boolean option.
func (parser *ArgParser) AddFlag(name string) {
    opt := newFlag(false)
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// AddStr registers a string option with a default value.
func (parser *ArgParser) AddStr(name string, value string) {
    opt := newStr(value)
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// AddInt registers an integer option with a default value.
func (parser *ArgParser) AddInt(name string, value int) {
    opt := newInt(value)
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// AddFloat registers a floating-point option with a default value.
func (parser *ArgParser) AddFloat(name string, value float64) {
    opt := newFloat(value)
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// AddFlagList registers a boolean list option.
func (parser *ArgParser) AddFlagList(name string) {
    opt := newFlagList()
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// AddStrList registers a string list option.
func (parser *ArgParser) AddStrList(name string, greedy bool) {
    opt := newStrList(greedy)
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// AddIntList registers an integer list option.
func (parser *ArgParser) AddIntList(name string, greedy bool) {
    opt := newIntList(greedy)
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// AddFloatList registers a floating-point list option.
func (parser *ArgParser) AddFloatList(name string, greedy bool) {
    opt := newFloatList(greedy)
    for _, element := range strings.Split(name, " ") {
        parser.options[element] = opt
    }
}


// -------------------------------------------------------------------------
// ArgParser: retrieving options.
// -------------------------------------------------------------------------


// Found returns true if the specified option was found while parsing.
func (parser *ArgParser) Found(name string) bool {
    return parser.options[name].found
}


// GetFlag returns the value of the specified boolean option.
func (parser *ArgParser) GetFlag(name string) bool {
    return parser.options[name].getFlag()
}


// GetStr returns the value of the specified string option.
func (parser *ArgParser) GetStr(name string) string {
    return parser.options[name].getStr()
}


// GetInt returns the value of the specified integer option.
func (parser *ArgParser) GetInt(name string) int {
    return parser.options[name].getInt()
}


// GetFloat returns the value of the specified floating-point option.
func (parser *ArgParser) GetFloat(name string) float64 {
    return parser.options[name].getFloat()
}


// LenList returns the length of the named option's internal list of values.
func (parser *ArgParser) LenList(name string) int {
    return len(parser.options[name].values)
}


// GetFlagList returns the named option's values as a slice of booleans.
func (parser *ArgParser) GetFlagList(name string) []bool {
    return parser.options[name].getFlagList()
}


// GetStrList returns the named option's values as a slice of strings.
func (parser *ArgParser) GetStrList(name string) []string {
    return parser.options[name].getStrList()
}


// GetIntList returns the named option's values as a slice of integers
func (parser *ArgParser) GetIntList(name string) []int {
    return parser.options[name].getIntList()
}


// GetFloatList returns the named option's values as a slice of floats.
func (parser *ArgParser) GetFloatList(name string) []float64 {
    return parser.options[name].getFloatList()
}


// -------------------------------------------------------------------------
// ArgParser: setting options.
// -------------------------------------------------------------------------


// Clear clears the named option's internal list of values.
func (parser *ArgParser) ClearList(name string) {
    parser.options[name].clear()
}


// SetFlag appends a value to a boolean option's internal list.
func (parser *ArgParser) SetFlag(name string, value bool) {
    parser.options[name].setFlag(value)
}


// SetStr appends a value to a string option's internal list.
func (parser *ArgParser) SetStr(name string, value string) {
    parser.options[name].setStr(value)
}


// SetInt appends a value to an integer option's internal list.
func (parser *ArgParser) SetInt(name string, value int) {
    parser.options[name].setInt(value)
}


// SetFloat appends a value to a floating-point option's internal list.
func (parser *ArgParser) SetFloat(name string, value float64) {
    parser.options[name].setFloat(value)
}


// -------------------------------------------------------------------------
// ArgParser: positional arguments.
// -------------------------------------------------------------------------


// HasArgs returns true if the parser has found one or more positional
// arguments.
func (parser *ArgParser) HasArgs() bool {
    return len(parser.arguments) > 0
}


// LenArgs returns the number of positional arguments.
func (parser *ArgParser) LenArgs() int {
    return len(parser.arguments)
}


// GetArg returns the positional argument at the specified index.
func (parser *ArgParser) GetArg(index int) string {
    return parser.arguments[index]
}


// GetArgs returns the positional arguments as a slice of strings.
func (parser *ArgParser) GetArgs() []string {
    return parser.arguments
}


// GetArgsAsInts attempts to parse and return the positional arguments as a
// slice of integers. The application will exit with an error message if any
// of the arguments cannot be parsed as an integer.
func (parser *ArgParser) GetArgsAsInts() []int {
    ints := make([]int, 0)
    for _, strArg := range parser.arguments {
        intArg, err := strconv.ParseInt(strArg, 0, 0)
        if err != nil {
            exit(fmt.Sprintf("cannot parse '%v' as an integer", strArg))
        }
        ints = append(ints, int(intArg))
    }
    return ints
}


// GetArgsAsFloats attempts to parse and return the positional arguments as a
// slice of floats. The application will exit with an error message if any
// of the arguments cannot be parsed as a float.
func (parser *ArgParser) GetArgsAsFloats() []float64 {
    floats := make([]float64, 0)
    for _, strArg := range parser.arguments {
        floatArg, err := strconv.ParseFloat(strArg, 64)
        if err != nil {
            exit(fmt.Sprintf("cannot parse '%v' as a float", strArg))
        }
        floats = append(floats, floatArg)
    }
    return floats
}


// -------------------------------------------------------------------------
// ArgParser: commands.
// -------------------------------------------------------------------------


// AddCmd registers a command, its help text, and its associated callback.
func (parser *ArgParser) AddCmd(name, helptext string, callback func(*ArgParser)) *ArgParser {
    cmdParser := NewParser(helptext, "")
    for _, element := range strings.Split(name, " ") {
        parser.commands[element] = cmdParser
        parser.callbacks[element] = callback
    }
    return cmdParser
}


// HasCmd returns true if the parser has found a command.
func (parser *ArgParser) HasCmd() bool {
    return parser.cmdName != ""
}


// GetCmd returns the command name, if the parser has found a command.
func (parser *ArgParser) GetCmdName() string {
    return parser.cmdName
}


// GetCmdParser returns the command's parser instance, if a command was found.
func (parser *ArgParser) GetCmdParser() *ArgParser {
    return parser.cmdParser
}


// -------------------------------------------------------------------------
// ArgParser: parsing arguments.
// -------------------------------------------------------------------------


// Parses a stream of string arguments.
func (parser *ArgParser) parseStream(stream *argStream) {

    // Switch to turn off option parsing if we encounter a double dash.
    // Everything following the '--' will be treated as a positional
    // argument.
    parsing := true

    // Loop while we have arguments to process.
    for stream.hasNext() {

        // Fetch the next argument from the stream.
        arg := stream.next()

        // If parsing has been turned off, simply add the argument to the
        // list of positionals.
        if !parsing {
            parser.arguments = append(parser.arguments, arg)
            continue
        }

        // If we encounter a -- argument, turn off option-parsing.
        if arg == "--" {
            parsing = false
            continue
        }

        // Is the argument a long-form option or flag?
        if strings.HasPrefix(arg, "--") {
            parser.parseLongOption(arg[2:], stream)
            continue
        }

        // Is the argument a short-form option or flag? If the argument
        // consists of a single dash or a dash followed by a digit, we treat
        // it as a positional argument.
        if strings.HasPrefix(arg, "-") {
            if arg == "-" || unicode.IsDigit([]rune(arg)[1]) {
                parser.arguments = append(parser.arguments, arg)
            } else {
                parser.parseShortOption(arg[1:], stream)
            }
            continue
        }

        // Is the argument a registered command?
        if cmdParser, ok := parser.commands[arg]; ok {
            cmdParser.parseStream(stream)
            parser.callbacks[arg](cmdParser)
            parser.cmdName = arg
            parser.cmdParser = cmdParser
            continue
        }

        // Is the argument the automatic 'help' command?
        if arg == "help"{
            if stream.hasNext() {
                name := stream.next()
                if cmdParser, ok := parser.commands[name]; ok {
                    fmt.Println(cmdParser.helptext)
                    os.Exit(0)
                } else {
                    exit(fmt.Sprintf("'%v' is not a recognised command", name))
                }
            } else {
                exit("the help command requires an argument")
            }
        }

        // If we get here, we have a positional argument.
        parser.arguments = append(parser.arguments, arg)
    }
}


// ParseArgs parses a slice of string arguments.
func (parser *ArgParser) ParseArgs(args []string) {
    parser.parseStream(newArgStream(args))
}


// Parse parses the application's command line arguments.
func (parser *ArgParser) Parse() {
    parser.ParseArgs(os.Args[1:])
}


// Parse a long-form option, i.e. an option beginning with a double dash.
func (parser *ArgParser) parseLongOption(arg string, stream *argStream) {

    // Do we have an option of the form --name=value?
    if strings.Contains(arg, "=") {
        parser.parseEqualsOption("--", arg)
        return
    }

    // Is the argument a registered option name?
    if opt, ok := parser.options[arg]; ok {
        opt.found = true

        // If the option is a flag, store the boolean true.
        if opt.optType == flagOpt {
            opt.setFlag(true)
            return
        }

        // Not a flag, so check for a following option value.
        if !stream.hasNextValue() {
            exit(fmt.Sprintf("missing argument for the --%v option", arg))
        }

        // Try to parse the argument as a value of the appropriate type.
        opt.trySet(stream.next())

        // If the option is a greedy list, keep trying to parse values
        // until we run out of arguments.
        if opt.greedy {
            for stream.hasNextValue() {
                opt.trySet(stream.next())
            }
        }
        return
    }

    // Is the argument the automatic --help flag?
    if arg == "help" && parser.helptext != "" {
        fmt.Println(parser.helptext)
        os.Exit(0)
    }

    // Is the argument the automatic --version flag?
    if arg == "version" && parser.version != "" {
        fmt.Println(parser.version)
        os.Exit(0)
    }

    // The argument is not a registered or automatic option name.
    // Print an error message and exit.
    exit(fmt.Sprintf("--%v is not a recognised option", arg))
}


// Parse a short-form option, i.e. an option beginning with a single dash.
func (parser *ArgParser) parseShortOption(arg string, stream *argStream) {

    // Do we have an option of the form -n=value?
    if strings.Contains(arg, "=") {
        parser.parseEqualsOption("-", arg)
        return
    }

    // We handle each character individually to support condensed options:
    //    -abc foo bar
    // is equivalent to:
    //    -a foo -b bar -c
    for _, char := range arg {
        name := string(char)

        // Do we have the name of a registered option?
        if opt, ok := parser.options[name]; ok {
            opt.found = true

            // If the option is a flag, store the boolean true.
            if opt.optType == flagOpt {
                opt.setFlag(true)
                continue
            }

            // Not a flag, so check for a following option value.
            if !stream.hasNextValue() {
                exit(fmt.Sprintf("missing argument for the -%v option", name))
            }

            // Try to parse the argument as a value of the appropriate type.
            opt.trySet(stream.next())

            // If the option is a greedy list, keep trying to parse values
            // until we run out of arguments.
            if opt.greedy {
                for stream.hasNextValue() {
                    opt.trySet(stream.next())
                }
            }

        // Not a registered option. Print a error message and exit.
        } else {
            exit(fmt.Sprintf("-%v is not a recognised option", name))
        }
    }
}


// Parse an option of the form --name=value or -n=value.
func (parser *ArgParser) parseEqualsOption(prefix string, arg string) {
    split := strings.SplitN(arg, "=", 2)
    name := split[0]
    value := split[1]

    // Do we have the name of a registered option?
    opt, ok := parser.options[name]
    if !ok {
        exit(fmt.Sprintf("%s%s is not a recognised option", prefix, name))
    }
    opt.found = true

    // Boolean flags should never contain an equals sign.
    if opt.optType == flagOpt {
        exit(fmt.Sprintf("invalid format for boolean flag %s%s", prefix, name))
    }

    // Check that a value has been supplied.
    if value == "" {
        exit(fmt.Sprintf("missing argument for the %s%s option", prefix, name))
    }

    // Try to parse the argument as a value of the appropriate type.
    opt.trySet(value)
}


// -------------------------------------------------------------------------
// ArgParser: utilities.
// -------------------------------------------------------------------------


// Help prints the parser's help text, then exits.
func (parser *ArgParser) Help() {
    fmt.Println(parser.helptext)
    os.Exit(0)
}


// String returns a string representation of the parser instance.
func (parser *ArgParser) String() string {
    lines := make([]string, 0)

    lines = append(lines, "Options:")
    if len(parser.options) > 0 {
        names := make([]string, 0, len(parser.options))
        for name := range parser.options {
            names = append(names, name)
        }
        sort.Strings(names)
        for _, name := range names {
            var valstr string
            opt := parser.options[name]

            switch opt.optType {
            case flagOpt:
                valstr = fmt.Sprintf("%v", opt.getFlagList())
            case strOpt:
                valstr = fmt.Sprintf("%v", opt.getStrList())
            case intOpt:
                valstr = fmt.Sprintf("%v", opt.getIntList())
            case floatOpt:
                valstr = fmt.Sprintf("%v", opt.getFloatList())
            }

            lines = append(lines, fmt.Sprintf("  %v: %v", name, valstr))
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
    if parser.HasCmd() {
        lines = append(lines, fmt.Sprintf("  %v", parser.GetCmdName()))
    } else {
        lines = append(lines, "  [none]")
    }

    return strings.Join(lines, "\n")
}
