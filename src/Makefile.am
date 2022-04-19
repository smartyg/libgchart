## Process this file with automake to produce Makefile.in

sources_private_h =

sources_public_h =         \
	Gchart.hpp         \
	GchartProvider.hpp \
	GchartChart.hpp    \
	GchartPoint.hpp    \
	GchartLabel.hpp    \
	GchartColor.hpp    \
	helper.hpp

sources_c =                \
	Gchart.cpp         \
	GchartProvider.cpp \
	GchartChart.cpp

lib_LTLIBRARIES =
GCHART_GTK3_CPPFLAGS = @SIGC_CFLAGS@
GCHART_GTK3_LIBS = @M_LIBS@ @SIGC_LIBS@
GCHART_GTK4_CPPFLAGS = @SIGC_CFLAGS@
GCHART_GTK4_LIBS = @M_LIBS@ @SIGC_LIBS@

if ENABLE_GTK3
lib_LTLIBRARIES += libgchart-gtk3-@LIBRARY_API_VERSION@.la
GCHART_GTK3_CPPFLAGS += @GTK3_CFLAGS@ @GLIBMM3_CFLAGS@ @CAIROMM3_CFLAGS@ @GTKMM3_CFLAGS@
GCHART_GTK3_LIBS += @GTK3_LIBS@ @GLIBMM3_LIBS@ @CAIROMM3_LIBS@ @GTKMM3_LIBS@
if ENABLE_GLADE
gladedir = ${libdir}/glade/modules
glade_LTLIBRARIES = libgladegchart.la
libgladegchart_la_CPPFLAGS = @GTK3_CFLAGS@ @GTKMM3_CFLAGS@
libgladegchart_la_LDFLAGS = -avoid-version
libgladegchart_la_LIBADD = libgchart-gtk3-@LIBRARY_API_VERSION@.la @GLIBMM3_LIBS@
libgladegchart_la_SOURCES = \
	gchart_glade.h      \
	gchart_glade.cpp
endif
endif

if ENABLE_GTK4
lib_LTLIBRARIES += libgchart-gtk4-@LIBRARY_API_VERSION@.la
GCHART_GTK4_CPPFLAGS += @GTK4_CFLAGS@ @GLIBMM4_CFLAGS@ @CAIROMM4_CFLAGS@ @GTKMM4_CFLAGS@
GCHART_GTK4_LIBS += @GTK4_LIBS@ @GLIBMM4_LIBS@ @CAIROMM4_LIBS@ @GTKMM4_LIBS@
endif

## Shared library
libgchart_gtk3_@LIBRARY_API_VERSION@_la_CPPFLAGS = \
    $(AM_CPPFLAGS)          \
    $(WARN_CPPFLAGS)        \
    $(DISABLE_DEPRECATED)   \
    $(GCHART_GTK3_CPPFLAGS) \
    -DG_LOG_DOMAIN=\"Gchart\"

libgchart_gtk3_@LIBRARY_API_VERSION@_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

libgchart_gtk3_@LIBRARY_API_VERSION@_la_LIBADD = \
    $(GCHART_GTK3_LIBS) \
    -lm

libgchart_gtk3_@LIBRARY_API_VERSION@_la_SOURCES = \
	$(sources_public_h)  \
	$(sources_private_h) \
	$(sources_c)

libgchart_gtk4_@LIBRARY_API_VERSION@_la_CPPFLAGS = \
    $(AM_CPPFLAGS)          \
    $(WARN_CPPFLAGS)        \
    $(DISABLE_DEPRECATED)   \
    $(GCHART_GTK4_CPPFLAGS) \
    -DG_LOG_DOMAIN=\"Gchart\"

libgchart_gtk4_@LIBRARY_API_VERSION@_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

libgchart_gtk4_@LIBRARY_API_VERSION@_la_LIBADD = \
    $(GCHART_GTK4_LIBS) \
    -lm

libgchart_gtk4_@LIBRARY_API_VERSION@_la_SOURCES = \
	$(sources_public_h)  \
	$(sources_private_h) \
	$(sources_c)

libgchart_includedir = $(includedir)/gchart-0.1
nobase_libgchart_include_HEADERS = $(sources_public_h)

## Misc
CLEANFILES =

## GObject Introspection Support
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = $(INTROSPECTION_FLAGS) --add-include-path=$(srcdir) --add-include-path=$(builddir) --warn-all --warn-error
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir) --includedir=$(builddir)

if HAVE_INTROSPECTION
introspection_sources = $(sources_c) $(sources_public_h)

Gchart_@LIBRARY_API_VERSION@.gir: libgchart-@LIBRARY_API_VERSION@.la
Gchart_@LIBRARY_API_VERSION@_gir_INCLUDES = GObject-2.0 Gtk-3.0
Gchart_@LIBRARY_API_VERSION@_gir_CFLAGS = $(GCHART_CFLAGS) -I$(srcdir) -I$(builddir)
Gchart_@LIBRARY_API_VERSION@_gir_LIBS = libgchart-@LIBRARY_API_VERSION@.la
Gchart_@LIBRARY_API_VERSION@_gir_FILES = $(introspection_sources)
Gchart_@LIBRARY_API_VERSION@_gir_SCANNERFLAGS = --identifier-prefix=Gchart --c-include "gchart.hpp"
INTROSPECTION_GIRS += Gchart-@LIBRARY_API_VERSION@.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif