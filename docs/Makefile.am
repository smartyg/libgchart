if DOCS
if HAVE_DOXYGEN
directory = $(top_srcdir)/docs/man/man3

dist_man_MANS = $(directory)/g_chart.h.3
$(directory)/g_chart.h.3: doxyfile.stamp

doxyfile.stamp:
	$(DOXYGEN) Doxyfile
	echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: doxyfile.stamp
clean-local:
	rm -rf $(top_srcdir)/docs/man $(top_srcdir)/docs/html
endif
endif
