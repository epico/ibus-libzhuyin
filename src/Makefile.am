# vim:set noet ts=4:
#
# ibus-libzhuyin - New Zhuyin engine based on libzhuyin for IBus
#
# Copyright (c) 2014 Peng Wu <alexepico@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

libexec_PROGRAMS = ibus-engine-libzhuyin

#ibus_engine_libzhuyin_built_c_sources = \
#	$(NULL)
#ibus_engine_libzhuyin_built_h_sources = \
#	ZYPunctTable.h \
#	ZYSimpTradConverterTable.h \
#	$(NULL)

ibus_engine_libzhuyin_c_sources = \
	ZYConfig.cc \
	ZYEditor.cc \
	ZYLibZhuyin.cc \
	ZYZConfig.cc \
	ZYZhuyinProperties.cc \
	ZYTradSimpConverter.cc \
	ZYEnhancedText.cc \
	ZYZEnhancedEditor.cc \
	ZYZPhoneticEditor.cc \
	ZYZZhuyinEditor.cc \
	ZYZPinyinEditor.cc \
	ZYSymbols.cc \
	ZYSymbolLookup.cc \
	ZYZBuiltinSymbolSection.cc \
	ZYZPhoneticSection.cc \
	ZYZBopomofoSymbolSection.cc \
	ZYZUserSymbolListAllSection.cc \
	ZYZUserSymbolShownSection.cc \
	ZYFallbackEditor.cc \
	ZYEngine.cc \
	ZYZZhuyinEngine.cc \
	ZYMain.cc \
	$(NULL)

ibus_engine_libzhuyin_h_sources = \
	ZYBus.h \
	ZYConfig.h \
	ZYEditor.h \
	ZYLookupTable.h \
	ZYObject.h \
	ZYLibZhuyin.h \
	ZYZConfig.h \
	ZYZhuyinProperties.h \
	ZYTradSimpConverter.h \
	ZYEnhancedText.h \
	ZYZEnhancedEditor.h \
	ZYZPhoneticEditor.h \
	ZYZZhuyinEditor.h \
	ZYZPinyinEditor.h \
	ZYSymbols.h \
	ZYSymbolLookup.h \
	ZYZSymbolSection.h \
	ZYZBuiltinSymbolSection.h \
	ZYZPhoneticSection.h \
	ZYZBopomofoSymbolSection.h \
	ZYZUserSymbolListAllSection.h \
	ZYZUserSymbolShownSection.h \
	ZYPointer.h \
	ZYProperty.h \
	ZYRawEditor.h \
	ZYFallbackEditor.h \
	ZYSignal.h \
	ZYString.h \
	ZYText.h \
	ZYTypes.h \
	ZYUtil.h \
	ZYEngine.h \
	ZYZZhuyinEngine.h \
	ZYTradSimpConverterTable.h \
	$(NULL)


ibus_engine_libzhuyin_SOURCES = \
        $(ibus_engine_libzhuyin_c_sources) \
        $(ibus_engine_libzhuyin_h_sources) \
        $(NULL)

ibus_engine_libzhuyin_CXXFLAGS = \
        @IBUS_CFLAGS@ \
        @LIBZHUYIN_CFLAGS@ \
        @OPENCC_CFLAGS@ \
        -DGETTEXT_PACKAGE=\"@GETTEXT_PACKAGE@\" \
        -DPKGDATADIR=\"$(pkgdatadir)\" \
        -DLIBEXECDIR=\"$(libexecdir)\" \
        -DMODEL_DATADIR=\"$(libdir)\/ibus-libzhuyin\/data\" \
        $(NULL)

if HAVE_BOOST
ibus_engine_libzhuyin_CXXFLAGS += \
        @BOOST_CPPFLAGS@ \
        $(NULL)
else
ibus_engine_libzhuyin_CXXFLAGS += \
        -std=c++0x \
        $(NULL)
endif

ibus_engine_libzhuyin_LDADD = \
        @IBUS_LIBS@ \
        @LIBZHUYIN_LIBS@ \
        @OPENCC_LIBS@ \
        $(NULL)


component_DATA = \
        libzhuyin.xml \
        $(NULL)
componentdir = @datadir@/ibus/component

EXTRA_DIST = \
        libzhuyin.xml.in \
        $(NULL)

CLEANFILES = \
        libzhuyin.xml \
        $(NULL)

ZYTradSimpConverterTable.h:
	$(AM_V_GEN) \
	$(PYTHON) $(top_srcdir)/scripts/update-tradsimp-table.py > $@ || \
		( $(RM) $@; exit 1 )

update-tradsimp-table:
	$(RM) ZhConversion.php ZhConversion.py ZYTradSimpConverterTable.h
	$(MAKE) ZhConversion.php
	$(MAKE) ZhConversion.py
	$(MAKE) ZYTradSimpConverterTable.h

libzhuyin.xml: libzhuyin.xml.in
	$(AM_V_GEN) \
	( \
		libexecdir=${libexecdir}; \
		pkgdatadir=${pkgdatadir}; \
		s=`cat $<`; \
		eval "echo \"$${s}\""; \
	) > $@


ZhConversion.php:
	$(AM_V_GEN) \
	curl https://raw.githubusercontent.com/wikimedia/mediawiki/master/languages/data/ZhConversion.php > $@ || \
		( $(RM) $@; exit 1 )

ZhConversion.py: ZhConversion.php
	$(AM_V_GEN) \
	php $(top_srcdir)/scripts/php_2_py.php > $@ || \
		( $(RM) $@; exit 1 )
