// -------------------------------------------------------------------------
// Clio: a minimalist argument-parsing library designed for building elegant
// command line interfaces.
// -------------------------------------------------------------------------

#include "clio.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

using namespace std;

using clio::callback;
using clio::ArgStream;
using clio::OptionType;
using clio::Option;
using clio::ArgParser;


// -------------------------------------------------------------------------
// String parsing utilities.
// -------------------------------------------------------------------------


static int tryStringToInt(string arg) {
    try {
        return stoi(arg);
    } catch (invalid_argument) {
        cerr << "Error: cannot parse '" << arg << "' as an integer." << endl;
        exit(1);
    } catch (out_of_range) {
        cerr << "Error: " << arg << " is out of range." << endl;
        exit(1);
    }
}


static double tryStringToDouble(string arg) {
    try {
        return stod(arg);
    } catch (invalid_argument) {
        cerr << "Error: cannot parse '" << arg << "' as a float." << endl;
        exit(1);
    } catch (out_of_range) {
        cerr << "Error: " << arg << " is out of range." << endl;
        exit(1);
    }
}


// -------------------------------------------------------------------------
// Option.
// -------------------------------------------------------------------------


void Option::trySetValue(string value) {
    switch (this->type) {
        case OptionType::Flag:
            cerr << "Option::trySetValue: invalid code path." << endl;
            exit(1);
        case OptionType::String:
            this->strings.push_back(value);
            break;
        case OptionType::Int:
            this->ints.push_back(tryStringToInt(value));
            break;
        case OptionType::Float:
            this->doubles.push_back(tryStringToDouble(value));
            break;
    }
}


// -------------------------------------------------------------------------
// ArgStream.
// -------------------------------------------------------------------------


void ArgStream::append(string arg) {
    args.push_back(arg);
}


string ArgStream::next() {
    string arg = args.front();
    args.pop_front();
    return arg;
}


string ArgStream::peek() {
    return args.front();
}


bool ArgStream::hasNext() {
    return args.size() > 0;
}


bool ArgStream::hasNextValue() {
    if (args.size() > 0) {
        auto const & next = args.front();
        if (next == "") {
            return true;
        }
        if (next[0] == '-') {
            if (next.size() == 1 || isdigit(next[1])) {
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


// -------------------------------------------------------------------------
// ArgParser: registering options.
// -------------------------------------------------------------------------


void ArgParser::addFlag(string name) {
    Option* option = new Option(OptionType::Flag);
    option->bools.push_back(false);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


void ArgParser::addStr(string name, string value) {
    Option* option = new Option(OptionType::String);
    option->strings.push_back(value);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


void ArgParser::addInt(string name, int value) {
    Option* option = new Option(OptionType::Int);
    option->ints.push_back(value);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


void ArgParser::addFloat(string name, double value) {
    Option* option = new Option(OptionType::Float);
    option->doubles.push_back(value);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


void ArgParser::addFlagList(string name) {
    Option* option = new Option(OptionType::Flag);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


void ArgParser::addStrList(string name, bool greedy) {
    Option* option = new Option(OptionType::String, greedy);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


void ArgParser::addIntList(string name, bool greedy) {
    Option* option = new Option(OptionType::Int, greedy);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


void ArgParser::addFloatList(string name, bool greedy) {
    Option* option = new Option(OptionType::Float, greedy);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        options[word] = option;
    }
}


// -------------------------------------------------------------------------
// ArgParser: fetching option values.
// -------------------------------------------------------------------------


bool ArgParser::found(string name) {
    return options[name]->found;
}


bool ArgParser::getFlag(string name) {
    return options[name]->bools.back();
}


string ArgParser::getStr(string name) {
    return options[name]->strings.back();
}


int ArgParser::getInt(string name) {
    return options[name]->ints.back();
}


double ArgParser::getFloat(string name) {
    return options[name]->doubles.back();
}


int ArgParser::lenList(string name) {
    switch (options[name]->type) {
        case OptionType::Flag:
            return options[name]->bools.size();
        case OptionType::String:
            return options[name]->strings.size();
        case OptionType::Int:
            return options[name]->ints.size();
        case OptionType::Float:
            return options[name]->doubles.size();
    }
}


vector<bool> ArgParser::getFlagList(string name) {
    return options[name]->bools;
}


vector<string> ArgParser::getStrList(string name) {
    return options[name]->strings;
}


vector<int> ArgParser::getIntList(string name) {
    return options[name]->ints;
}


vector<double> ArgParser::getFloatList(string name) {
    return options[name]->doubles;
}


// -------------------------------------------------------------------------
// ArgParser: setting option values.
// -------------------------------------------------------------------------


void ArgParser::clearList(string name) {
    switch (options[name]->type) {
        case OptionType::Flag:
            options[name]->bools.clear();
            break;
        case OptionType::String:
            options[name]->strings.clear();
            break;
        case OptionType::Int:
            options[name]->ints.clear();
            break;
        case OptionType::Float:
            options[name]->doubles.clear();
            break;
    }
}


void ArgParser::setFlag(string name, bool value) {
    options[name]->bools.push_back(value);
}


void ArgParser::setStr(string name, string value) {
    options[name]->strings.push_back(value);
}


void ArgParser::setInt(string name, int value) {
    options[name]->ints.push_back(value);
}


void ArgParser::setFloat(string name, double value) {
    options[name]->doubles.push_back(value);
}


// -------------------------------------------------------------------------
// ArgParser: fetching positional arguments.
// -------------------------------------------------------------------------


bool ArgParser::hasArgs() {
    return arguments.size() > 0;
}


int ArgParser::lenArgs() {
    return arguments.size();
}


string ArgParser::getArg(int index) {
    return arguments[index];
}


vector<string> ArgParser::getArgs() {
    return arguments;
}


vector<int> ArgParser::getArgsAsInts() {
    vector<int> values;
    for (auto const & arg: arguments) {
        values.push_back(tryStringToInt(arg));
    }
    return values;
}


vector<double> ArgParser::getArgsAsFloats() {
    vector<double> values;
    for (auto const & arg: arguments) {
        values.push_back(tryStringToDouble(arg));
    }
    return values;
}


// -------------------------------------------------------------------------
// ArgParser: setting positional arguments.
// -------------------------------------------------------------------------


void ArgParser::clearArgs() {
    arguments.clear();
}


void ArgParser::appendArg(string arg) {
    arguments.push_back(arg);
}


// -------------------------------------------------------------------------
// ArgParser: registering and handling commands.
// -------------------------------------------------------------------------


ArgParser& ArgParser::addCmd(string name, string helptext, callback cb) {
    ArgParser *parser = new ArgParser(helptext, this);

    stringstream stream(name);
    string word;

    while (stream >> word) {
        commands[word] = parser;
        callbacks[word] = cb;
    }

    return *parser;
}


bool ArgParser::hasCmd() {
    return cmdName != "";
}


string ArgParser::getCmdName() {
    return cmdName;
}


ArgParser& ArgParser::getCmdParser() {
    return *cmdParser;
}


ArgParser& ArgParser::getParent() {
    return *parent;
}


bool ArgParser::hasParent() {
    return parent != nullptr;
}


// -------------------------------------------------------------------------
// ArgParser: parsing arguments.
// -------------------------------------------------------------------------


// Parse an option of the form --name=value or -n=value.
void ArgParser::parseEqualsOption(string prefix, string name, string value) {

    // Do we have a registered option?
    auto const & element = options.find(name);
    if (element == options.end()) {
        cerr << "Error: " << prefix << name << " is not a recognised option.";
        cerr << endl;
        exit(1);
    }
    Option *opt = element->second;
    opt->found = true;

    // Boolean flags should never contain an equals sign.
    if (opt->type == OptionType::Flag) {
        cerr << "Error: invalid format for boolean flag ";
        cerr << prefix << name << "." << endl;
        exit(1);
    }

    // Make sure we have a value.
    if (value.size() == 0) {
        cerr << "Error: missing value for ";
        cerr << prefix << name << "." << endl;
        exit(1);
    }

    opt->trySetValue(value);
}


// Parse a long-form option, i.e. an option beginning with a double dash.
void ArgParser::parseLongOption(string arg, ArgStream& stream) {

    // Do we have an option of the form --name=value?
    size_t pos = arg.find("=");
    if (pos != string::npos) {
        this->parseEqualsOption(
            "--",
            arg.substr(0, pos),
            arg.substr(pos + 1)
        );
        return;
    }

    // Is the argument a registered option name?
    auto const & element = options.find(arg);
    if (element != options.end()) {
        Option *opt = element->second;
        opt->found = true;

        // If the option is a flag, store the boolean true and return.
        if (opt->type == OptionType::Flag) {
            opt->bools.push_back(true);
            return;
        }

        // Not a flag so check for a following value.
        if (!stream.hasNextValue()) {
            cerr << "Error: missing argument for --" << arg << "." << endl;
            exit(1);
        }

        // Try to parse the argument as a value of the appropriate type.
        opt->trySetValue(stream.next());

        // If the option is a greedy list, keep trying to parse values
        // until we run out of arguments.
        if (opt->greedy) {
            while (stream.hasNextValue()) {
                opt->trySetValue(stream.next());
            }
        }
        return;
    }

    // Is the argument the automatic --help flag?
    if (arg == "help" && this->helptext != "") {
        cout << this->helptext << endl;
        exit(0);
    }

    // Is the argument the automatic --version flag?
    if (arg == "version" && this->version != "") {
        cout << this->version << endl;
        exit(0);
    }

    // The argument is not a registered or automatic option name.
    // Print an error message and exit.
    cerr << "Error: --" << arg << " is not a recognised option." << endl;
    exit(1);
}


// Parse a short-form option, i.e. an option beginning with a single dash.
void ArgParser::parseShortOption(string arg, ArgStream& stream) {

    // Do we have an option of the form -n=value?
    size_t pos = arg.find("=");
    if (pos != string::npos) {
        this->parseEqualsOption(
            "-",
            arg.substr(0, pos),
            arg.substr(pos + 1)
        );
        return;
    }

    // We handle each character individually to support condensed options:
    //    -abc foo bar
    // is equivalent to:
    //    -a foo -b bar -c
    for (char& c: arg) {

        // Do we have the name of a registered option?
        auto const & element = options.find(arg);
        if (element == options.end()) {
            cerr << "Error: -" << c << " is not a recognised option.";
            cerr << endl;
            exit(1);
        }
        Option *opt = element->second;
        opt->found = true;

        // If the option is a flag, store the boolean true.
        if (opt->type == OptionType::Flag) {
            opt->bools.push_back(true);
        }

        // Not a flag so check for a following option value.
        else if (stream.hasNextValue()) {

            // Try to parse the argument as a value of the appropriate type.
            opt->trySetValue(stream.next());

            // If the option is a greedy list, keep trying to parse values
            // until we run out of arguments.
            if (opt->greedy) {
                while (stream.hasNextValue()) {
                    opt->trySetValue(stream.next());
                }
            }
        }

        // We're missing an option value.
        else {
            cerr << "Error: missing argument for -" << c << endl;
            exit(1);
        }
    }
}


// Parse a stream of string arguments.
void ArgParser::parse(ArgStream& stream) {

    // Switch to turn off parsing if we encounter a double dash, '--'.
    // Everything following the -- will be treated as a positional argument.
    bool parsing = true;

    // Loop while we have arguments to process.
    while (stream.hasNext()) {

        // Fetch the next argument from the stream.
        string arg = stream.next();

        // If parsing has been turned off, simply add the argument to the
        // list of positionals.
        if (!parsing) {
            arguments.push_back(arg);
            continue;
        }

        // If we enounter a '--', turn off option parsing.
        if (arg == "--") {
            parsing = false;
            continue;
        }

        // Is the argument a long-form option or flag?
        if (arg.compare(0, 2, "--") == 0) {
            this->parseLongOption(arg.substr(2), stream);
            continue;
        }

        // Is the argument a short-form option or flag? If the argument
        // consists of a single dash or a dash followed by a digit, we treat
        // it as a positional argument.
        if (arg[0] == '-') {
            if (arg.size() == 1 || isdigit(arg[1])) {
                arguments.push_back(arg);
            } else {
                this->parseShortOption(arg.substr(1), stream);
            }
            continue;
        }

        // Is the argument a registered command?
        auto element = commands.find(arg);
        if (element != commands.end()) {
            ArgParser *parser = element->second;
            this->cmdName = arg;
            this->cmdParser = parser;
            parser->parse(stream);
            this->callbacks[arg](*parser);
            continue;
        }

        // Is the argument the automatic 'help' command?
        if (arg == "help") {
            if (stream.hasNext()) {
                string name = stream.next();
                if (commands.find(name) == commands.end()) {
                    cerr << "Error: '" << name;
                    cerr << "' is not a recognised command" << endl;
                    exit(1);
                } else {
                    cout << commands[name]->helptext << endl;
                    exit(0);
                }
            } else {
                cerr << "Error: the help command requires an argument.";
                cerr << endl;
                exit(1);
            }
        }

        // Otherwise add the argument to our list of positional arguments.
        this->arguments.push_back(arg);
    }
}


// Parse an array of string arguments. We assume that argc and argv are the
// original parameters passed to main() and skip the first element.
void ArgParser::parse(int argc, char **argv) {
    ArgStream stream;
    for (int i = 1; i < argc; i++) {
        stream.append(argv[i]);
    }
    this->parse(stream);
}


// -------------------------------------------------------------------------
// ArgParser: utilities.
// -------------------------------------------------------------------------


// Override the << stream insertion operator to support vectors. This will
// allow us to cout our lists of option values in the print() method.
template<typename T>
static ostream& operator<<(ostream& stream, const vector<T>& vec) {
    stream << "[";
    for(size_t i = 0; i < vec.size(); ++i) {
        if (i) cout << ", ";
        stream << vec[i];
    }
    stream << "]";
    return stream;
}


// Print the parser instance to standard out.
void ArgParser::print() {
    cout << "Options:" << endl;
    if (options.size() > 0) {
        for (auto element: options) {
            cout << "  " << element.first << ": ";
            Option *option = element.second;
            switch (option->type) {
                case OptionType::Flag:
                    cout << option->bools;
                    break;
                case OptionType::String:
                    cout << option->strings;
                    break;
                case OptionType::Int:
                    cout << option->ints;
                    break;
                case OptionType::Float:
                    cout << option->doubles;
                    break;
            }
            cout << endl;
        }
    } else {
        cout << "  [none]" << endl;
    }

    cout << endl << "Arguments:" << endl;
    if (arguments.size() > 0) {
        for (auto arg: arguments) {
            cout << "  " << arg << endl;
        }
    } else {
        cout << "  [none]" << endl;
    }

    cout << endl << "Command:" << endl;
    if (this->hasCmd()) {
        cout << "  " << cmdName << endl;
    } else {
        cout << "  [none]" << endl;
    }
}


// -------------------------------------------------------------------------
// ArgParser: cleanup.
// -------------------------------------------------------------------------


ArgParser::~ArgParser() {

    // Free the memory occupied by registered Option instances. Each
    // instance can be registered under multiple names.
    vector<Option*> odel;
    for (auto element: options) {
        Option *optionptr = element.second;
        if (find(odel.begin(), odel.end(), optionptr) == odel.end()) {
            delete optionptr;
            odel.push_back(optionptr);
        }
    }

    // Free the memory occupied by registered command parsers. Each parser
    // instance can be registered under multiple names.
    vector<ArgParser*> pdel;
    for (auto element: commands) {
        ArgParser *parserptr = element.second;
        if (find(pdel.begin(), pdel.end(), parserptr) == pdel.end()) {
            delete parserptr;
            pdel.push_back(parserptr);
        }
    }
}
