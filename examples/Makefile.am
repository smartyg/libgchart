## Process this file with automake to produce Makefile.in

CHART_CPPFLAGS = @GTK_CFLAGS@ @GLIBMM_CFLAGS@ @CAIROMM_CFLAGS@ @GTKMM_CFLAGS@

CHART_LIBS = @M_LIBS@ @GTK_LIBS@ @GLIBMM_LIBS@ @CAIROMM_LIBS@ @GTKMM_LIBS@

## Demo Application
noinst_PROGRAMS = chart

chart_SOURCES = chart.cpp

chart_CPPFLAGS =          \
    -I$(top_srcdir)/src   \
    $(AM_CPPFLAGS)        \
    $(WARN_CPPFLAGS)      \
    $(DISABLE_DEPRECATED) \
    $(CHART_CPPFLAGS)     \
    $(GTHREAD_CPPFLAGS)

chart_LDADD =       \
    $(CHART_LIBS)   \
    $(GTHREAD_LIBS) \
    $(top_builddir)/src/libgchart-@LIBRARY_API_VERSION@.la


## Misc
#EXTRA_DIST = poi.png mapviewer.ui mapviewer.js README

