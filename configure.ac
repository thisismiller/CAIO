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

AC_CONFIG_FILES(
  Makefile
  src/Makefile
  demo/Makefile
)
AC_CONFIG_SUBDIRS(
  coroutine
)

AC_OUTPUT
