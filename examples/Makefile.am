## Process this file with automake to produce Makefile.in

LIBGCHART_CFLAGS =          \
    $(GLIB_CFLAGS)          \
    $(GTK_CFLAGS)           \
    $(CAIRO_CFLAGS)

LIBGCHART_LIBS =            \
    $(GLIB_LIBS)            \
    $(GTK_LIBS)             \
    $(CAIRO_LIBS)

## Demo Application
noinst_PROGRAMS = chart

chart_SOURCES =         \
    chart.c

chart_CFLAGS =          \
    -I$(top_srcdir)/src     \
    $(WARN_CFLAGS)          \
    $(DISABLE_DEPRECATED)   \
    $(LIBGCHART_CFLAGS)     \
    $(GTHREAD_CFLAGS)

chart_LDADD =           \
    $(LIBGCHART_LIBS)       \
    $(GTHREAD_LIBS)         \
    $(top_builddir)/src/libgchart-@LIBRARY_API_VERSION@.la


## Misc
#EXTRA_DIST = poi.png mapviewer.ui mapviewer.js README

