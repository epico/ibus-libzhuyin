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
	ZYSymbols.cc \
	ZYZBuiltinSymbolSection.cc \
	ZYZPhoneticSection.cc \
	ZYEngine.cc \
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
	ZYSymbols.h \
	ZYZSymbolSection.h \
	ZYZBuiltinSymbolSection.h \
	ZYZPhoneticSection.h \
	ZYPointer.h \
	ZYProperty.h \
	ZYRawEditor.h \
	ZYSignal.h \
	ZYString.h \
	ZYText.h \
	ZYTypes.h \
	ZYUtil.h \
	ZYEngine.h \
	$(NULL)

#ibus_engine_libzhuyin_c_sources += \
#	ZYZConfig.cc \
#	ZYZPhoneticEditor.cc \
#	ZYZFullPinyinEditor.cc \
#	ZYZZhuyinEditor.cc \
#	ZYZPinyinEngine.cc \
#	ZYZZhuyinEngine.cc \
#	$(NULL)

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
        -DLIBZHUYIN_DATADIR=\"@LIBZHUYIN_DATADIR@\/data\" \
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
