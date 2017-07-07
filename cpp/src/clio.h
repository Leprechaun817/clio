// -------------------------------------------------------------------------
// Clio: a minimalist argument-parsing library designed for building elegant
// command line interfaces.
//
// Clio is written in portable C++11.
//
// Author: Darren Mulholland <darren@mulholland.xyz>
// License: Public Domain
// Version: 2.0.0a1
// -------------------------------------------------------------------------

#pragma once

#include <deque>
#include <map>
#include <string>
#include <vector>

namespace clio {

    // Callback type for registering commands.
    class ArgParser;
    typedef void (*callback)(ArgParser& parser);

    // Internal use.
    class ArgStream {
        public:
            void append(std::string arg);
            std::string next();
            std::string peek();
            bool hasNext();
            bool hasNextValue();
        private:
            std::deque<std::string> args;
    };

    // Internal use.
    enum class OptionType {
        Flag, String, Int, Float
    };

    // Internal use.
    class Option {
        public:
            OptionType type;
            bool greedy = false;
            bool found = false;
            std::vector<bool> bools;
            std::vector<std::string> strings;
            std::vector<int> ints;
            std::vector<double> doubles;

            Option(OptionType type)
                : type(type) {}
            Option(OptionType type, bool greedy)
                : type(type), greedy(greedy) {}

            void trySetValue(std::string value);
    };

    // ArgParser provides the external interface to the library.
    class ArgParser {

        public:

            ArgParser() {}
            ArgParser(std::string helptext, std::string version)
                : helptext(helptext), version(version) {}
            ArgParser(std::string helptext, ArgParser* parent)
                : helptext(helptext), parent(parent) {}

            ~ArgParser();

            // Register single-valued options.
            void addFlag(std::string name);
            void addStr(std::string name, std::string value);
            void addInt(std::string name, int value);
            void addFloat(std::string name, double value);

            // Register list options.
            void addFlagList(std::string name);
            void addStrList(std::string name, bool greedy);
            void addIntList(std::string name, bool greedy);
            void addFloatList(std::string name, bool greedy);

            // Parse command line arguments.
            void parse(int argc, char **argv);
            void parse(ArgStream& args);

            // Returns true if the named option was found while parsing.
            bool found(std::string name);

            // Fetch option values.
            bool getFlag(std::string name);
            std::string getStr(std::string name);
            int getInt(std::string name);
            double getFloat(std::string name);

            // Returns the length of the named option's list of values.
            int lenList(std::string name);

            // Fetch list option values.
            std::vector<bool> getFlagList(std::string name);
            std::vector<std::string> getStrList(std::string name);
            std::vector<int> getIntList(std::string name);
            std::vector<double> getFloatList(std::string name);

            // Clear the named option's list of values.
            void clearList(std::string name);

            // Set option values.
            void setFlag(std::string name, bool value);
            void setStr(std::string name, std::string value);
            void setInt(std::string name, int value);
            void setFloat(std::string name, double value);

            // Fetch positional arguments.
            bool hasArgs();
            int lenArgs();
            std::string getArg(int index);
            std::vector<std::string> getArgs();
            std::vector<int> getArgsAsInts();
            std::vector<double> getArgsAsFloats();

            // Set positional arguments.
            void clearArgs();
            void appendArg(std::string arg);

            // Register a command.
            ArgParser& addCmd(
                std::string name,
                std::string help,
                callback cb
            );

            // Utilities for handling commands manually.
            bool hasCmd();
            std::string getCmdName();
            ArgParser& getCmdParser();
            ArgParser& getParent();
            bool hasParent();

            // Print a parser instance to stdout.
            void print();

        private:

            std::string helptext;
            std::string version;
            std::map<std::string, Option*> options;
            std::map<std::string, ArgParser*> commands;
            std::map<std::string, callback> callbacks;
            std::vector<std::string> arguments;
            std::string cmdName;
            ArgParser *cmdParser = nullptr;
            ArgParser *parent = nullptr;

            void parseEqualsOption(
                std::string prefix,
                std::string name,
                std::string value
            );
            void parseLongOption(std::string arg, ArgStream& stream);
            void parseShortOption(std::string arg, ArgStream& stream);
    };
}
