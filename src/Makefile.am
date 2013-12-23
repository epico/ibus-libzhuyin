## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(pkgdatadir)"\" \
	$(IBUS_LIBZHUYIN_CFLAGS)

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = ibus_engine_libzhuyin

ibus_engine_libzhuyin_SOURCES = \
	main.vala config.vapi

ibus_engine_libzhuyin_VALAFLAGS =  \
	--pkg gtk+-3.0


ibus_engine_libzhuyin_LDFLAGS = \
	-Wl,--export-dynamic

ibus_engine_libzhuyin_LDADD = $(IBUS_LIBZHUYIN_LIBS)
