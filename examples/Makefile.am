## Process this file with automake to produce Makefile.in

CHART_GTK3_CPPFLAGS =
CHART_GTK3_LIBS =
CHART_GTK4_CPPFLAGS =
CHART_GTK4_LIBS =

if ENABLE_GTK3
CHART_GTK3_CPPFLAGS += @GTK3_CFLAGS@ @GLIBMM3_CFLAGS@ @CAIROMM3_CFLAGS@ @GTKMM3_CFLAGS@
CHART_GTK3_LIBS += @GTK3_LIBS@ @GLIBMM3_LIBS@ @CAIROMM3_LIBS@ @GTKMM3_LIBS@
endif
if ENABLE_GTK4
CHART_GTK4_CPPFLAGS += @GTK4_CFLAGS@ @GLIBMM4_CFLAGS@ @CAIROMM4_CFLAGS@ @GTKMM4_CFLAGS@
CHART_GTK4_LIBS += @GTK4_LIBS@ @GLIBMM4_LIBS@ @CAIROMM4_LIBS@ @GTKMM4_LIBS@
endif

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

