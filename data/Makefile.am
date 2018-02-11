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


APPDATA_XML = libzhuyin.appdata.xml

gsettings_SCHEMAS = com.github.libzhuyin.ibus-libzhuyin.gschema.xml

@GSETTINGS_RULES@

SUBDIRS = \
        icons \
        model \
        $(NULL)

appdatadir = @datadir@/appdata

appdata_DATA = $(APPDATA_XML)

desktop_in_in_files = \
        ibus-setup-libzhuyin.desktop.in.in \
        $(NULL)

desktop_in_files = $(desktop_in_in_files:.in.in=.in)

$(desktop_in_files): %.desktop.in: %.desktop.in.in Makefile
	$(AM_V_GEN) sed \
		-e "s|\@libexecdir\@|$(libexecdir)|" \
		-e "s|\@pkgdatadir\@|$(pkgdatadir)|" \
		$< > $@.tmp && mv $@.tmp $@

desktopdir=$(datadir)/applications
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

symbols_DATA = \
        easysymbol.txt \
        usersymbol.txt \
        $(NULL)

symbolsdir = $(pkgdatadir)

EXTRA_DIST = \
        $(desktop_in_in_files) \
        $(symbols_DATA) \
        $(APPDATA_XML) \
        $(gsettings_SCHEMAS) \
        $(NULL)

DISTCLEANFILES = \
        $(desktop_in_files) \
        $(desktop_DATA) \
        $(NULL)
