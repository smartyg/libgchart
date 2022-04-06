dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(libgchart, 0.1.0, goedhart.martijn@gmail.com)
AC_CONFIG_SRCDIR([src/gchart.c])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_BUILD
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign dist-xz])

# AX_DEFINE_SUBST(NAME, VALUE, DESCRIPTION)
# -----------------------------------------
AC_DEFUN([AX_DEFINE_SUBST], [
	AC_DEFINE([$1], ["$2"], [$3])
	AC_SUBST([$1], ['$2'])
])

# AX_CHECK_LIB(NAME, FUNC, NAME2)
# -----------------------------------------
AC_DEFUN([AX_CHECK_LIB], [
	AC_CHECK_LIB([$1], [$2], [AC_SUBST([$3_LIBS], ["-l$1"])
	AC_DEFINE([HAVE_LIB$3], [1], [Define if you have lib$1])])
])

# some other package constants
AX_DEFINE_SUBST([PROGRAM_NAME], [gchart], [program name])
AX_DEFINE_SUBST([PROGRAM_SUMMARY], [Library provides a simple chart widget for GTK+ 3.x], [program summary])
AX_DEFINE_SUBST([PROGRAM_DESCRIPTION], [], [program description])
AX_DEFINE_SUBST([LAST_CHANGE], [03-01-2015], [Date of last change])
AX_DEFINE_SUBST([AUTHOR], [Martijn Goedhart], [Author])
AX_DEFINE_SUBST([COPYRIGHT], [(c) 2014-2015 Martijn Goedhart], [Copyright])
AX_DEFINE_SUBST([LICENSE], [GNU General Public License], [License name])
AX_DEFINE_SUBST([LICENSE_SHORT], [GPLv2+], [License shorthand])
AX_DEFINE_SUBST([LICENSE_SHORT_RPM], [GPL-2.0+], [SPDX License code])
AX_DEFINE_SUBST([LICENSE_LONG], [GNU GPL version 2 or later <http://www.gnu.org/licenses/gpl-2.0.html>], [Long license description])
AX_DEFINE_SUBST([PACKAGE_HOMEPAGE], [http://www.martijn-goedhart.nl/libgchart/], [package home page])
AX_DEFINE_SUBST([LIBRARY_CURRENT_VERSION], [1], [version of this library])
AX_DEFINE_SUBST([LIBRARY_REVISION_VERSION], [0], [version of this library])
AX_DEFINE_SUBST([LIBRARY_AGE_VERSION], [1], [version of this library])
AX_DEFINE_SUBST([LIBRARY_API_VERSION], [0], [version of this library])

# Do not automatically add -g -O2 to the compiler
: ${CFLAGS=""}

# Checks for programs.
AC_LANG_C
AC_PROG_CC
# AC_PROG_CXX
AC_PROG_CC_C99
AC_C_INLINE
AC_C_CHAR_UNSIGNED
AC_C_STRINGIZE
AC_C_TYPEOF

dnl this allows us specify individual liking flags for each target
AM_PROG_CC_C_O

dnl Initialize Libtool
LT_INIT

dnl Check if Libtool is present
dnl Libtool is used for building share libraries 
AC_PROG_LIBTOOL

# Checks for header files.
AC_CHECK_HEADERS([libintl.h math.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

# Check which option are enabled and disabled
# use debuging
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [Turn on debugging @<:@default=disabled@:>@])],
	[case "${enableval}" in
		yes) debug=true ;;
		no)  debug=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	esac],
	[debug=false])

#compile documentation (doxygen)
AC_ARG_ENABLE([docs],
	[AS_HELP_STRING([--disable-docs], [generate documentation @<:@default=enabled@:>@])],
	[case "${enableval}" in
		yes) docs=true ;;
		no)  docs=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --disable-docs]) ;;
	esac],
	[docs=true])

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.32 gobject-2.0 >= 2.38])
PKG_CHECK_MODULES([GTK], [gtk+-3.0])

# Checks for libraries.
AX_CHECK_LIB([m], [floor], [M])

GOBJECT_INTROSPECTION_REQS=0.10.0
GOBJECT_INTROSPECTION_CHECK([$GOBJECT_INTROSPECTION_REQS])

GTK_DOC_CHECK([1.12])

AC_DEFINE([_GNU_SOURCE], [1], [use gnu extensions])

# Compute the major version of the library which is the last part of the library SONAME.
AC_SUBST([LIBRARY_OLD_INTERFACE], [$((LIBRARY_CURRENT_VERSION-LIBRARY_AGE_VERSION))])

# Test for programs
AC_CHECK_PROGS([GIT], [git])

if test "x$debug" == xtrue; then
	AC_DEFINE([DEBUG], [], [debug flag])
else
	AM_SILENT_RULES([yes])
fi

AC_SUBST(have_dot, "NO")
if test "x$docs" == xtrue
then
	AC_CHECK_PROGS([DOXYGEN], [doxygen])

	if test -z "$DOXYGEN"
	then
		AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
		docs=false
	else
		AC_CHECK_PROGS([DOT], [dot])
		if test -z "$DOT"
		then
			AC_MSG_WARN([Dot not found - continuing without Dot support])
		else
			AC_SUBST(have_dot, "YES")
		fi
	fi
fi

if test -f gchart.spec.in
then
	AC_CONFIG_FILES([gchart.spec])
	git_version=$VERSION
	if test -n "$GIT"
	then
		git_version=$($GIT describe --tags --match='v@<:@0-9@:>@*' --dirty=modified | $SED -E -e 's/^v(@<:@0-9@:>@@<:@.0-9@:>@*)(-(@<:@0-9@:>@+)-(@<:@0-9a-g@:>@+)(-(modified))?)?/\1+git\3.\4.\6/' -e 's/@<:@.@:>@{2,}/./' -e 's/(\+git)?@<:@.@:>@$//')
		if test $? -ne 0
		then
			git_version=$VERSION
		fi
	fi
	AC_SUBST([GIT_VERSION], [$git_version])
fi

if test -f README.in
then
	AC_CONFIG_FILES([README])
fi

AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AM_CONDITIONAL([DOCS], [test x$docs = xtrue])
AM_CONDITIONAL([HAVE_GIT], [test -n "$GIT"])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_CONDITIONAL([HAVE_DOT], [test -n "$DOT"])

AC_CONFIG_FILES([Makefile src/Makefile examples/Makefile docs/Doxyfile docs/Makefile libgchart.pc])
AC_OUTPUT
