## Process this file with automake to produce Makefile.in

CHART_GTK3_CPPFLAGS = @GTK_CFLAGS@ @GLIBMM_CFLAGS@ @CAIROMM_CFLAGS@ @GTKMM_CFLAGS@
CHART_GTK3_LIBS = @GTK_LIBS@ @GLIBMM_LIBS@ @CAIROMM_LIBS@ @GTKMM_LIBS@
CHART_GTK4_CPPFLAGS = @GTK_CFLAGS@ @GLIBMM_CFLAGS@ @CAIROMM_CFLAGS@ @GTKMM_CFLAGS@
CHART_GTK4_LIBS = @GTK_LIBS@ @GLIBMM_LIBS@ @CAIROMM_LIBS@ @GTKMM_LIBS@

## Demo Application
noinst_PROGRAMS =
if ENABLE_GTK3
noinst_PROGRAMS += chart-gtk3
endif
if ENABLE_GTK4
noinst_PROGRAMS += chart-gtk4
endif


chart_gtk3_SOURCES = chart-gtk3.cpp
chart_gtk4_SOURCES = chart-gtk4.cpp

chart_gtk3_CPPFLAGS =          \
    -I$(top_srcdir)/src   \
    $(AM_CPPFLAGS)        \
    $(WARN_CPPFLAGS)      \
    $(DISABLE_DEPRECATED) \
    $(CHART_GTK3_CPPFLAGS)     \
    $(GTHREAD_CPPFLAGS)

chart_gtk3_LDADD =       \
    $(CHART_GTK3_LIBS)   \
    $(GTHREAD_LIBS) \
    $(top_builddir)/src/libgchart-gtk3-@LIBRARY_API_VERSION@.la

chart_gtk4_CPPFLAGS =          \
    -I$(top_srcdir)/src   \
    $(AM_CPPFLAGS)        \
    $(WARN_CPPFLAGS)      \
    $(DISABLE_DEPRECATED) \
    $(CHART_GTK4_CPPFLAGS)     \
    $(GTHREAD_CPPFLAGS)

chart_gtk4_LDADD =       \
    $(CHART_GTK4_LIBS)   \
    $(GTHREAD_LIBS) \
    $(top_builddir)/src/libgchart-gtk4-@LIBRARY_API_VERSION@.la

## Misc
#EXTRA_DIST = poi.png mapviewer.ui mapviewer.js README

