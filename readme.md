
# Clio

A minimalist argument-parsing library for building elegant command-line interfaces.

Clio supports long and short-form options and arbitrarily-nested commands. It aims to provide a consistent interface across multiple programming languages, implemented wherever possible as a simple drop-in file.


## Features

All implementations support the same core feature set:

* Long-form boolean flags with optional single-character shortcuts: `--flag`, `-f`.

* Long-form string, integer, and floating-point options with optional
  single-character shortcuts: `--option <arg>`, `-o <arg>`.

* Condensed short-form options: `-abc <arg> <arg>`.

* Automatic `--help` and `--version` flags.

* Git-style command interfaces with arbitrarily-nested commands.


## Disclaimer

Clio is currently an alpha-stage project. It lacks documentation and real-world testing and its interface remains subject to change.


## License

This work has been placed in the public domain.
