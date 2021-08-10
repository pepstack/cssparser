# cssparser (katana-parser) - A pure-C CSS parser.

[katana-parser](https://gitee.com/mirrors/katana-parser)


CssParser is an implementation of the [CSS][] \(Cascading Style Sheet\) parsing algorithm implemented as a pure C library with no outside dependencies. It's designed to serve as a building block for other tools and libraries such as linters, validators, templating languages, and refactoring and analysis tools.

CssParser is inspired by [Gumbo][], so it has some goals and features same as Gumbo.

Goals & features:

* Simple API that can be easily wrapped by other languages.
* Relatively lightweight, with no outside dependencies.
* Support for fragment parsing.

Non-goals:

* Mutability. CssParser is intentionally designed to turn a style sheet into a parse tree, and free that parse tree all at once. It's not designed to persistently store nodes or subtrees outside of the parse tree, or to perform arbitrary style mutations within your program. If you need this functionality, we recommend translating the CssParser parse tree into a mutable style representation more suited for the particular needs of your program before operating on it.

Wishlist:

* Fully conformant with the [CSS-syntax][].
* Hackable dump or print.
* Robust and resilient to bad input.
* Full-featured error reporting.
* Additional performance improvements.
* Tested on [Official W3C Test Suites][].

Installation
============

To build and install the library, issue the standard UNIX incantation from the root of the distribution:

```bash
$ ./autogen.sh
$ ./configure CFLAGS="-std=c99"
$ make
$ sudo make install
```

CssParser comes with full pkg-config support, so you can use the pkg-config to print the flags needed to link your program against it:

```bash
$ pkg-config --cflags cssparser         # print compiler flags
$ pkg-config --libs cssparser           # print linker flags
$ pkg-config --cflags --libs cssparser  # print both
```

For example:

```bash
$ gcc examples/dump_stylesheet.c `pkg-config --cflags --libs cssparser` -o dump
```

If package cssparser was **not found in the pkg-config** search path, perhaps you should add the directory containing `cssparser.pc` to the `PKG_CONFIG_PATH` environment variable as following:

```bash
$ export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

See the pkg-config man page for more info.

Basic Usage
===========

Within your program, you need to include "cssparser.h" and then issue a call to `css_parse_string`:

```C
#include "cssparser.h"

int main() {
  const char* css = "selector { property: value }";
  CssParserOutput* output = css_parse_string(css, strlen(css), CssParserParserModeStylesheet);
  // Do stuff with output, eg. print the input style
  css_dump_output(output);
  css_destroy_output(output);
}
```

See the API documentation and sample programs for more details.

Contributing
===========
Bug reports are very much welcome.  Please use GitHub's issue-tracking feature, as it makes it easier to keep track of bugs and makes it possible for other project watchers to view the existing issues.

Patches and pull requests are also welcome.

If you're unwilling to do this, it would be most helpful if you could file bug reports that include detailed prose about where in the code the error is and how to fix it, but leave out exact source code.



[CSS]: http://www.w3.org/Style/CSS/current-work
[CSS-syntax]: http://www.w3.org/TR/css3-syntax
[Gumbo]: https://github.com/google/gumbo-parser
[reworkcss/css]: https://github.com/reworkcss/css
[Official W3C Test Suites]: http://www.w3.org/Style/CSS/Test/
[semantic versioning]: http://semver.org/
