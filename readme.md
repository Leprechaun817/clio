
# Clio

Clio is a minimalist argument-parsing library designed for building elegant command-line interfaces.

Clio supports long and short-form options and arbitrarily-nested commands. It aims to provide a consistent interface across multiple programming languages, implemented wherever possible as a simple drop-in file.

Clio is implemented in [C][], [C++][], [Go][], [Java][], and [Python][].

[C]: http://mulholland.xyz/docs/clio/api/c.html
[C++]: http://mulholland.xyz/docs/clio/api/cpp.html
[Go]: http://mulholland.xyz/docs/clio/api/go.html
[Java]: http://mulholland.xyz/docs/clio/api/java.html
[Python]: http://mulholland.xyz/docs/clio/api/python.html


## Features

All implementations support the same core feature set:

* Long-form boolean flags with single-character shortcuts: `--flag`, `-f`.

* Long-form string, integer, and floating-point options with
  single-character shortcuts: `--opt <arg>`, `-o <arg>`.

* Condensed short-form options: `-abc <arg> <arg>`.

* Automatic `--help` and `--version` flags.

* Support for list options with multiple values.

* Support for git-style command interfaces with arbitrarily-nested commands.

See the [documentation][docs] for details.

[docs]: http://mulholland.xyz/docs/clio/


## License

This work has been placed in the public domain.
