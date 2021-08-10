# CssParser (libcssparser) - A pure-C CSS parser.

08/2021

CssParser originally forked from [katana-parser](https://gitee.com/mirrors/katana-parser), and I make some changes.

CssParser is an implementation of the [CSS][] \(Cascading Style Sheet\) parsing algorithm implemented as a pure C library with no outside dependencies. It's designed to serve as a building block for other tools and libraries such as linters, validators, templating languages, and refactoring and analysis tools.

CssParser is inspired by [Google Gumbo](https://github.com/google/gumbo-parser), so it has some goals and features same as Gumbo.

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

To build and install the library on Linux or MingW (for win32), enter the root of distribution in bash shell:

**libtool** is required for building project.

```bash
$ ./autogen.sh

$ ./configure
or
$ ./configure CFLAGS="-std=c99"

$ make
$ sudo make install
```

Alternatively, you can build it with Visual Studio for Windows by open the solution file (CssParser.sln) in msvc folder.

Basic Usage
===========

Within your program, you only need to include "CssParser.h":


```C
#include "CssParser.h"

int main()
{
  const char* css = "selector { property: value }";
  CssOutput* output = css_parse_string(css, strlen(css), CssParserParserModeStylesheet);

  // Do stuff with output, eg. print the input style
  css_dump_output(output);
  css_destroy_output(output);
}
```

See the API documentation and examples for more details.

Contributing
============

Bug reports are very much welcome. Please use GitHub's issue-tracking feature, as it makes it easier to keep track of bugs and makes it possible for other project watchers to view the existing issues.

Patches and pull requests are also welcome.

If you're unwilling to do this, it would be most helpful if you could file bug reports that include detailed prose about where in the code the error is and how to fix it, but leave out exact source code.


[CSS]: http://www.w3.org/Style/CSS/current-work
[CSS-syntax]: http://www.w3.org/TR/css3-syntax
[Gumbo]: https://github.com/google/gumbo-parser
[reworkcss/css]: https://github.com/reworkcss/css
[Official W3C Test Suites]: http://www.w3.org/Style/CSS/Test/
[semantic versioning]: http://semver.org/
