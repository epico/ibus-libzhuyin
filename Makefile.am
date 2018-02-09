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

SUBDIRS = src setup data po

ACLOCAL_AMFLAGS = -I m4

# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(docdir)

EXTRA_DIST = \
        autogen.sh \
        ibus-libzhuyin.spec.in \
        $(NULL)

noinst_DIST = \
        $(NULL)

DISTCLEANFILES = \
        po/stamp-it \
        $(NULL)
