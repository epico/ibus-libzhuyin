# -*- coding: utf-8 -*-
#
# ibus-libzhuyin - New Zhuyin engine based on libzhuyin for IBus
#
# Copyright (c) 2013-2014 Peng Wu <alexepico@gmail.com>
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

import gettext

import locale
import os
import sys

from gi.repository import GLib
from gi.repository import Gtk
from gi.repository import IBus

import config

locale.setlocale(locale.LC_ALL, "")
localedir = os.getenv("IBUS_LOCALEDIR")
pkgdatadir = os.getenv("IBUS_PKGDATADIR")
gettext.install('ibus-libzhuyin', localedir)

class PreferencesWindow:
    def __init__(self, engine, parent=None):
        self.__bus = IBus.Bus()
        self.__config = self.__bus.get_config()
        self.__builder = Gtk.Builder()
        self.__builder.set_translation_domain("ibus-libzhuyin")
        self.__builder.add_from_file("ibus-libzhuyin-preferences.ui")
        self.__window = self.__builder.get_object("window")
        self.__init_pages()

        if engine == "zhuyin":
            self.__config_namespace = "engine/zhuyin"
            self.__values = dict(self.__config.get_values(self.__config_namespace))
            self.__init_inputting()
            self.__init_keyboard()
            self.__init_fuzzy_zhuyin()
            self.__init_user_phrases()
            self.__init_about()
            self.__update_fuzzy_zhuyin()
        else:
            print("Error: Unknown Engine")
            exit()

        self.__pages.set_current_page(0)
        self.__window.connect("delete-event", Gtk.main_quit)


    def __init_pages(self):
        self.__pages = self.__builder.get_object("pages")
        self.__page_inputting = self.__builder.get_object("pageInputting")
        self.__page_keyboard = self.__builder.get_object("pageKeyboard")
        self.__page_fuzzy_zhuyin = self.__builder.get_object("pageFuzzyZhuyin")
        self.__page_user_phrases = self.__builder.get_object("pageUserPhrases")
        self.__page_about = self.__builder.get_object("pageAbout")

        self.__page_inputting.hide()
        self.__page_keyboard.hide()
        self.__page_fuzzy_zhuyin.hide()
        self.__page_user_phrases.hide()
        self.__page_about.hide()


    def __init_inputting(self):
        # page Inputting
        self.__page_inputting.show()

        # init state
        self.__chinese_mode = self.__builder.get_object("chinesemode")
        self.__full_half_shape = self.__builder.get_object("fullhalfshape")
        self.__auto_shift_cursor = self.__builder.get_object("autoshiftcursor")
        self.__add_phrase_forward = self.__builder.get_object("addphraseforward")
        self.__clean_all_buffer = self.__builder.get_object("cleanallbuffer")
        self.__always_input_num = self.__builder.get_object("alwaysinputnum")
        self.__rearward_choice = self.__builder.get_object("rearwardchoice")
        self.__space_as_selection = self.__builder.get_object("spaceasselection")

        # read values
        self.__chinese_mode.set_active(self.__get_value("chinesemode", True))
        self.__full_half_shape.set_active(self.__get_value("fullhalfshape", False))
        self.__auto_shift_cursor.set_active(self.__get_value("autoshiftcursor", True))
        self.__add_phrase_forward.set_active(self.__get_value("addphraseforward", True))
        self.__clean_all_buffer.set_active(self.__get_value("cleanallbuffer", False))
        self.__always_input_num.set_active(self.__get_value("alwaysinputnum", True))
        self.__rearward_choice.set_active(self.__get_value("rearwardchoice", True))
        self.__space_as_selection.set_active(self.__get_value("spaceasselection", False))

        # connect signals
        self.__chinese_mode.connect("toggled", self.__toggled_cb, "chinesemode")
        self.__full_half_shape.connect("toggled", self.__toggled_cb, "fullhalfshape")
        self.__auto_shift_cursor.connect("toggled", self.__toggled_cb, "autoshiftcursor")
        self.__add_phrase_forward.connect("toggled", self.__toggled_cb, "addphraseforward")
        self.__clean_all_buffer.connect("toggled", self.__toggled_cb, "cleanallbuffer")
        self.__always_input_num.connect("toggled", self.__toggled_cb, "alwaysinputnum")
        self.__rearward_choice.connect("toggled", self.__toggled_cb, "rearwardchoice")
        self.__space_as_selection.connect("toggled", self.__toggled_cb, "spaceasselection")


    def __init_keyboard(self):
        pass


    def __init_fuzzy_zhuyin(self):
        # page Fuzzy Zhuyin
        self.__page_fuzzy_zhuyin.show()

        # fuzzy zhuyin
        self.__fuzzy_zhuyin = self.__builder.get_object("fuzzyzhuyin")
        self.__fuzzy_zhuyin_widgets = [
            ("fuzzyzhuyin_c_ch", True),
            ("fuzzyzhuyin_z_zh", True),
            ("fuzzyzhuyin_s_sh", True),
            ("fuzzyzhuyin_l_n", True),
            ("fuzzyzhuyin_f_h", True),
            ("fuzzyzhuyin_l_r", False),
            ("fuzzyzhuyin_g_k", False),
            ("fuzzyzhuyin_an_ang", True),
            ("fuzzyzhuyin_en_eng", True),
            ("fuzzyzhuyin_in_ing", True),
        ]

        def __fuzzy_zhuyin_toggled_cb(widget):
            val = widget.get_active()
            map(lambda w: self.__builder.get_object(w[0]).set_sensitive(val),
                self.__fuzzy_zhuyin_widgets)
        self.__fuzzy_zhuyin.connect("toggled", __fuzzy_zhuyin_toggled_cb)

        # init value
        self.__fuzzy_zhuyin.set_active(self.__get_value("fuzzyzhuyin", False))
        for name, defval in self.__fuzzy_zhuyin_widgets:
            widget = self.__builder.get_object(name)
            widget.set_active(self.__get_value(name, defval))

        self.__fuzzy_zhuyin.connect("toggled", self.__toggled_cb, "fuzzyzhuyin")
        for name, defval in self.__fuzzy_zhuyin_widgets:
            widget = self.__builder.get_object(name)
            widget.connect("toggled", self.__toggled_cb, name)


    def __init_user_phrases(self):
        pass


    def __init_about(self):
        # page About
        self.__page_about.show()

        self.__name_version = self.__builder.get_object("nameversion")
        self.__name_version.set_markup(_("<big><b>New Zhuyin %s</b></big>") % config.get_version())


    def __update_fuzzy_zhuyin(self):
        options = [
            ("fuzzyzhuyin_c_ch", "ㄘ <=> ㄔ"),
            ("fuzzyzhuyin_z_zh", "ㄗ <=> ㄓ"),
            ("fuzzyzhuyin_s_sh", "ㄙ <=> ㄕ"),
            ("fuzzyzhuyin_l_n", "ㄌ <=> ㄋ"),
            ("fuzzyzhuyin_f_h", "ㄈ <=> ㄏ"),
            ("fuzzyzhuyin_l_r", "ㄌ <=> ㄖ"),
            ("fuzzyzhuyin_g_k", "ㄍ <=> ㄎ"),
            ("fuzzyzhuyin_an_ang", "ㄢ <=> ㄤ"),
            ("fuzzyzhuyin_en_eng", "ㄣ <=> ㄥ"),
            ("fuzzyzhuyin_in_ing", "ㄧㄣ <=> ㄧㄥ"),
        ]

        for name, label in options:
            self.__builder.get_object(name).set_label(label)


    def __toggled_cb(self, widget, name):
        self.__set_value(name, widget.get_active ())

    def __get_value(self, name, defval):
        if name in self.__values:
            var = self.__values[name]
            if isinstance(defval, type(var)):
                return var
        self.__set_value(name, defval)
        return defval

    def __set_value(self, name, val):
        var = None
        if isinstance(val, bool):
            var = GLib.Variant.new_boolean(val)
        elif isinstance(val, int):
            var = GLib.Variant.new_int32(val)
        elif isinstance(val, str):
            var = GLib.Variant.new_string(val)
        else:
            print >> sys.stderr, "val(%s) is not in support type." % repr(val)
            return

        self.__values[name] = val
        self.__config.set_value(self.__config_namespace, name, var)


    def show(self):
        self.__window.show()


def main():
    name = "zhuyin"
    PreferencesWindow(name).show()
    Gtk.main()


if __name__ == "__main__":
    main()
