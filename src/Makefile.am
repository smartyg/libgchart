## Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgchart-@LIBRARY_API_VERSION@.la libgchartsglade.la

GCHART_CPPFLAGS = @GTK_CFLAGS@ @GLIBMM_CFLAGS@ @CAIROMM_CFLAGS@ @GTKMM_CFLAGS@

GCHART_LIBS = @M_LIBS@ @GTK_LIBS@ @GLIBMM_LIBS@ @CAIROMM_LIBS@ @GTKMM_LIBS@

## Shared library
libgchart_@LIBRARY_API_VERSION@_la_CPPFLAGS = \
    $(AM_CPPFLAGS)          \
    $(WARN_CPPFLAGS)        \
    $(DISABLE_DEPRECATED) \
    $(GCHART_CPPFLAGS)      \
    -DG_LOG_DOMAIN=\"Gchart\"

libgchart_@LIBRARY_API_VERSION@_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

libgchart_@LIBRARY_API_VERSION@_la_LIBADD = \
    $(GCHART_LIBS) \
    -lm

sources_private_h =

sources_public_h =         \
	Gchart.hpp         \
	GchartProvider.hpp \
	GchartChart.hpp    \
	GchartPoint.hpp    \
	GchartLabel.hpp    \
	GchartColor.hpp    \
	helper.hpp         \
	gchart_glade.h

sources_c =                \
	Gchart.cpp         \
	GchartProvider.cpp \
	GchartChart.cpp

libgchart_@LIBRARY_API_VERSION@_la_SOURCES = \
	$(sources_public_h)  \
	$(sources_private_h) \
	$(sources_c)

libgchart_includedir = $(includedir)/gchart-0.1
nobase_libgchart_include_HEADERS = $(sources_public_h)

libgchartsglade_la_CPPFLAGS = \
    $(AM_CPPFLAGS)          \
    $(WARN_CPPFLAGS)        \
    $(DISABLE_DEPRECATED) \
    $(GCHART_CPPFLAGS)      \
    -DG_LOG_DOMAIN=\"Gchart\"

libgchartsglade_la_SOURCES = gchart_glade.cpp

libgchartsglade_la_LIBADD = \
    $(GCHART_LIBS) \
    -lm

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