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
GCHART_GTK3_CPPFLAGS = @GTK_CFLAGS@ @GLIBMM_CFLAGS@ @CAIROMM_CFLAGS@ @GTKMM_CFLAGS@ @SIGC_CFLAGS@
GCHART_GTK3_LIBS = @M_LIBS@ @GTK_LIBS@ @GLIBMM_LIBS@ @CAIROMM_LIBS@ @GTKMM_LIBS@ @SIGC_LIBS@
GCHART_GTK4_CPPFLAGS = @GTK_CFLAGS@ @GLIBMM_CFLAGS@ @CAIROMM_CFLAGS@ @GTKMM_CFLAGS@ @SIGC_CFLAGS@
GCHART_GTK4_LIBS = @M_LIBS@ @GTK_LIBS@ @GLIBMM_LIBS@ @CAIROMM_LIBS@ @GTKMM_LIBS@ @SIGC_LIBS@

if ENABLE_GTK3
lib_LTLIBRARIES += libgchart-gtk3.la
if ENABLE_GLADE
gladedir = ${libdir}/glade/modules
glade_LTLIBRARIES = libgladegchart.la
libgladegchart_la_CPPFLAGS = @GTK_CFLAGS@ @GTKMM_CFLAGS@
libgladegchart_la_LDFLAGS = -avoid-version
libgladegchart_la_LIBADD = libgchart-gtk3.la @GLIBMM_LIBS@
libgladegchart_la_SOURCES = \
	gchart_glade.h      \
	gchart_glade.cpp
endif
endif

if ENABLE_GTK4
lib_LTLIBRARIES += libgchart-gtk4.la
endif

## Shared library
libgchart_gtk3_la_CPPFLAGS = \
    $(AM_CPPFLAGS)          \
    $(WARN_CPPFLAGS)        \
    $(DISABLE_DEPRECATED)   \
    $(GCHART_GTK3_CPPFLAGS) \
    -DG_LOG_DOMAIN=\"Gchart\"

libgchart_gtk3_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

libgchart_gtk3_la_LIBADD = \
    $(GCHART_GTK3_LIBS) \
    -lm

libgchart_gtk3_la_SOURCES = \
	$(sources_public_h)  \
	$(sources_private_h) \
	$(sources_c)

libgchart_gtk4_la_CPPFLAGS = \
    $(AM_CPPFLAGS)          \
    $(WARN_CPPFLAGS)        \
    $(DISABLE_DEPRECATED)   \
    $(GCHART_GTK4_CPPFLAGS) \
    -DG_LOG_DOMAIN=\"Gchart\"

libgchart_gtk4_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

libgchart_gtk4_la_LIBADD = \
    $(GCHART_GTK4_LIBS) \
    -lm

libgchart_gtk4_la_SOURCES = \
	$(sources_public_h)  \
	$(sources_private_h) \
	$(sources_c)

libgchart_includedir = $(includedir)/gchart
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

Gchart.gir: libgchart.la
Gchart_gir_INCLUDES = GObject-2.0 Gtk-3.0
Gchart_gir_CFLAGS = $(GCHART_CFLAGS) -I$(srcdir) -I$(builddir)
Gchart_gir_LIBS = libgchart.la
Gchart_gir_FILES = $(introspection_sources)
Gchart_gir_SCANNERFLAGS = --identifier-prefix=Gchart --c-include "gchart.hpp"
INTROSPECTION_GIRS += Gchart.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif