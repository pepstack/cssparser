ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libcssparser.la
libcssparser_la_CFLAGS = -Wall
libcssparser_la_LDFLAGS = -version-info 1:0:0 -no-undefined
libcssparser_la_SOURCES = \
                src/foundation.c \
                src/foundation.h \
                src/cssparser.h \
                src/cssparser_lex.c \
                src/cssparser_lex.h \
                src/cssparser_tab.c \
                src/cssparser_tab.h \
                src/cssparser.c \
                src/cssparser_i.h \
                src/selector.c \
                src/selector.h \
                src/tokenizer.c

include_HEADERS = src/cssparser.h
# make a dir
# includedir=$(libdir)/../include/cssparser
# nobase_include_HEADERS

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cssparser.pc

noinst_PROGRAMS = dump_stylesheet fragment
LDADD = libcssparser.la
AM_CPPFLAGS = -I"$(srcdir)/src"

dump_stylesheet_SOURCES = examples/dump_stylesheet.c 
fragment_SOURCES = examples/fragment.c

# Deletes all the files generated by autogen.sh.
MAINTAINERCLEANFILES =   \
  aclocal.m4             \
  config.guess           \
  config.sub             \
  configure              \
  depcomp                \
  install-sh             \
  ltmain.sh              \
  Makefile.in            \
  missing                \
  mkinstalldirs          \
  config.h.in            \
  stamp.h.in             \
  m4/ltsugar.m4          \
  m4/libtool.m4          \
  m4/ltversion.m4        \
  m4/lt~obsolete.m4      \
  m4/ltoptions.m4
