
# Clio

A minimalist argument-parsing library for building elegant command-line interfaces.

Clio supports long and short-form options and arbitrarily-nested commands. It aims to provide a consistent interface across multiple programming languages, implemented wherever possible as a simple drop-in file.

* Supports long-form boolean flags with optional single-character shortcuts: `--flag`, `-f`.

* Supports long-form string, integer, and floating-point options with optional
  single-character shortcuts: `--option <arg>`, `-o <arg>`.

* Supports condensed short-form options: `-abc <arg> <arg>`.

* Supports automatic `--help` and `--version` flags.

* Supports git-style command interfaces with arbitrarily-nested commands.

Clio is currently an alpha-stage project. It lacks documentation and real-world testing and its interface remains subject to change.


## License

This work has been placed in the public domain.
