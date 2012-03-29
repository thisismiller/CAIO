AC_INIT(CAIO, 1.0, millerdevel@gmail.com)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE
AM_SILENT_RULES([yes])
LT_INIT
AC_LANG([C++])

AC_PROG_CC
AC_PROG_CXX

TOPLEVEL=$(pwd)
CFLAGS="-O0 -g -Wall -std=c99 -isystem $TOPLEVEL/coroutine/source -I $TOPLEVEL/src"
CXXFLAGS="-O0 -g -Wall -std=gnu++0x -isystem $TOPLEVEL/coroutine/source -I $TOPLEVEL/src"

dnl libev {{{
AC_MSG_CHECKING([for libev])
OLD_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -lev"
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include <ev.h>],
                   [ev_io io])],
                   [HAVE_LIBEV="yes"], [HAVE_LIBEV="no"])
LDFLAGS=$OLD_LDFLAGS
AC_MSG_RESULT($HAVE_LIBEV)
if test "x$HAVE_LIBEV" = "xno"; then
  AC_MSG_ERROR(["You need to go install libev"])
fi
dnl }}}

AC_CONFIG_FILES(
  Makefile
  src/Makefile
  demo/Makefile
)
AC_CONFIG_SUBDIRS(
  coroutine
)

AC_OUTPUT
dnl vim: set ft=config foldmethod=marker foldmarker={{{,}}} :
