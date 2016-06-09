
# Clio

Clio is a minimalist argument-parsing library designed for building elegant command-line interfaces.

Clio supports long and short-form options and arbitrarily-nested commands. It aims to provide a consistent interface across multiple programming languages, implemented wherever possible as a simple drop-in file.

Clio is currently implemented in [C][], [Go][], [Java][], and [Python][].

[C]: ./c/
[Go]: ./go/
[Java]: ./java/
[Python]: ./python/


## Features

All implementations support the same core feature set:

* Long-form boolean flags with single-character shortcuts: `--flag`, `-f`.

* Long-form string, integer, and floating-point options with
  single-character shortcuts: `--option <arg>`, `-o <arg>`.

* Condensed short-form options: `-abc <arg> <arg>`.

* Automatic `--help` and `--version` flags.

* Support for git-style command interfaces with arbitrarily-nested commands.


## License

This work has been placed in the public domain.
