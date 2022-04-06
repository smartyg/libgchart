## Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgchart-@LIBRARY_API_VERSION@.la

GCHART_CFLAGS = @GLIB_CFLAGS@ @GTK_CFLAGS@

GCHART_LIBS = @M_LIBS@ @GLIB_LIBS@ @GTK_LIBS@

## Shared library
libgchart_@LIBRARY_API_VERSION@_la_CFLAGS = \
    $(AM_CFLAGS)          \
    $(WARN_CFLAGS)        \
    $(DISABLE_DEPRECATED) \
    $(GCHART_CFLAGS)      \
    -DG_LOG_DOMAIN=\"Gchart\"

libgchart_@LIBRARY_API_VERSION@_la_LDFLAGS = -version-info @LIBRARY_CURRENT_VERSION@:@LIBRARY_REVISION_VERSION@:@LIBRARY_AGE_VERSION@

libgchart_@LIBRARY_API_VERSION@_la_LIBADD = \
    $(GCHART_LIBS) \
    -lm

sources_private_h =       \
	gchart-internal.h \
	gchart-draw.h     \
	gchart-text.h

sources_public_h = gchart.h

sources_c =               \
	gchart.c          \
	gchart-draw.c     \
	gchart-text.c

libgchart_@LIBRARY_API_VERSION@_la_SOURCES = \
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
Gchart_@LIBRARY_API_VERSION@_gir_SCANNERFLAGS = --identifier-prefix=Gchart --c-include "gchart.h"
INTROSPECTION_GIRS += Gchart-@LIBRARY_API_VERSION@.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif