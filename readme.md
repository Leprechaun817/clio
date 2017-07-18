
# Clio

Clio is an argument-parsing library designed for building elegant command-line interfaces. Argument-parsing libraries tend to be fiddly, complicated, and ugly. Clio aims for simplicity and for consistency across a range of programming languages.

Clio is implemented in:

* [C](http://mulholland.xyz/docs/clio/api/c.html)
* [C++](http://mulholland.xyz/docs/clio/api/cpp.html)
* [Go](http://mulholland.xyz/docs/clio/api/go.html)
* [Java](http://mulholland.xyz/docs/clio/api/java.html)
* [Python](http://mulholland.xyz/docs/clio/api/python.html)

All implementations support the same core feature set:

* Long-form boolean flags with single-character shortcuts: `--flag`, `-f`.

* Long-form string, integer, and floating-point options with
  single-character shortcuts: `--option <arg>`, `-o <arg>`.

* Condensed short-form options: `-abc <arg> <arg>`.

* Automatic `--help` and `--version` flags.

* Support for list options with multiple values.

* Support for git-style command interfaces with arbitrarily-nested commands.

See the [documentation][docs] for details.

[docs]: http://mulholland.xyz/docs/clio/
