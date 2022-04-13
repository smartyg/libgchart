dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(libgchart, 0.1.0, goedhart.martijn@gmail.com)
AC_CONFIG_SRCDIR([src/Gchart.cpp])
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

# Check for language support
AC_LANG([C])
AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_CXX_C_O

# Check for c++ language versions
AX_CXX_COMPILE_STDCXX(11, ext, mandatory)
AX_CXX_COMPILE_STDCXX(14, ext, optional)
AX_CXX_COMPILE_STDCXX(17, ext, optional)
# AX_CXX_COMPILE_STDCXX(20, ext, optional)

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
# Let user decide which GTK(mm) version to compile for (gtk3 or gtk4)
AC_ARG_WITH([gtk-version],
            [AS_HELP_STRING([--with-gtk-version],
              [set gtk version to compile against (gtk3 or gtk4 (default))])],
            [],
            [with_gtk_version=gtk4])

# use debuging
AC_ARG_ENABLE([debug],
	[AS_HELP_STRING([--enable-debug], [Turn on debugging @<:@default=disabled@:>@])],
	[case "${enableval}" in
		yes) debug=true ;;
		no)  debug=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
	esac],
	[debug=false])

# release flag (remove all debug code from compilation)
AC_ARG_ENABLE([release],
	[AS_HELP_STRING([--enable-release], [Compile for release (remove debug code) @<:@default=disabled@:>@])],
	[case "${enableval}" in
		yes) release=true; debug=no ;;
		no)  release=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --enable-release]) ;;
	esac],
	[release=false])

#compile documentation (doxygen)
AC_ARG_ENABLE([docs],
	[AS_HELP_STRING([--disable-docs], [generate documentation @<:@default=enabled@:>@])],
	[case "${enableval}" in
		yes) docs=true ;;
		no)  docs=false ;;
		*) AC_MSG_ERROR([bad value ${enableval} for --disable-docs]) ;;
	esac],
	[docs=true])

	AS_IF([test "x$with_gtk_version" = "xgtk3"], [
PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.24.0])
PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.24.0 gdkmm-3.0 >= 3.24.0])
PKG_CHECK_MODULES([GLIBMM], [glibmm-2.4 > 2.66.0])
PKG_CHECK_MODULES([CAIROMM], [cairomm-1.0])
],[
PKG_CHECK_MODULES([GTK], [gtk4 >= 4.6.0])
PKG_CHECK_MODULES([GTKMM], [gtkmm-4.0 >= 4.6.0])
PKG_CHECK_MODULES([GLIBMM], [glibmm-2.68])
PKG_CHECK_MODULES([CAIROMM], [cairomm-1.16])
])

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

if test "x$release" == xtrue; then
	AC_DEFINE([_RELEASE], [], [release flag])
	AM_SILENT_RULES([yes])
elif test "x$debug" == xtrue; then
	AC_DEFINE([_DEBUG], [], [debug flag])
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

AS_IF([test "x$with_gtk_version" = "xgtk3"], [
AC_DEFINE([_GTK3], [], [gtk3 flag])
],[
AC_DEFINE([_GTK4], [], [gtk4 flag])
])

AM_CONDITIONAL([GTK3], [test x$with_gtk_version = xgtk3])
AM_CONDITIONAL([GTK4], [test x$with_gtk_version = xgtk4])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])
AM_CONDITIONAL([RELEASE], [test x$release = xtrue])
AM_CONDITIONAL([DOCS], [test x$docs = xtrue])
AM_CONDITIONAL([HAVE_GIT], [test -n "$GIT"])
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_CONDITIONAL([HAVE_DOT], [test -n "$DOT"])

AC_CONFIG_FILES([Makefile src/Makefile examples/Makefile docs/Doxyfile docs/Makefile libgchart.pc])
AC_OUTPUT
