# vim:set et ts=4:
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

setup_libzhuyin_PYTHON = \
        main.py \
        config.py \
        ibus-libzhuyin-preferences.ui \
        $(NULL)

setup_libzhuyindir = $(datadir)/ibus-libzhuyin/setup

CLEANFILES = \
        *.pyc \
        $(NULL)

EXTRA_DIST = \
        config.py.in \
        $(NULL)

test-libzhuyin:
        $(ENV) DBUS_DEBUG=true \
                   IBUS_LOCALEDIR=@localedir@ \
                   IBUS_PKGDATADIR=$(pkgdatadir) \
                   PYTHONPATH=$(abs_top_srcdir):$(pyexecdir) \
                   $(PYTHON) $(srcdir)/main.py
