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

from __future__ import print_function

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
pkgdatadir = os.getenv("IBUS_PKGDATADIR") or "."
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
        self.__full_half_width = self.__builder.get_object("fullhalfwidth")
        self.__traditional_chinese = self.__builder.get_object("traditionalchinese")
        self.__always_input_num = self.__builder.get_object("alwaysinputnum")
        self.__space_show_candidates = self.__builder.get_object("spaceshowcandidates")
        self.__candidates_after_cursor = self.__builder.get_object("candidatesaftercursor")

        # read values
        self.__chinese_mode.set_active(self.__get_value("chinesemode", True))
        self.__full_half_width.set_active(self.__get_value("fullhalfwidth", False))
        self.__traditional_chinese.set_active(self.__get_value("traditionalchinese", True))
        self.__always_input_num.set_active(self.__get_value("alwaysinputnum", False))
        self.__space_show_candidates.set_active(self.__get_value("spaceshowcandidates", False))
        self.__candidates_after_cursor.set_active(self.__get_value("candidatesaftercursor", True))

        # connect signals
        self.__chinese_mode.connect("toggled", self.__toggled_cb, "chinesemode")
        self.__full_half_width.connect("toggled", self.__toggled_cb, "fullhalfwidth")
        self.__traditional_chinese.connect("toggled", self.__toggled_cb, "traditionalchinese")
        self.__always_input_num.connect("toggled", self.__toggled_cb, "alwaysinputnum")
        self.__space_show_candidates.connect("toggled", self.__toggled_cb, "spaceshowcandidates")
        self.__candidates_after_cursor.connect("toggled", self.__toggled_cb, "candidatesaftercursor")


    def __init_keyboard(self):
        # page Keyboard
        self.__page_keyboard.show()

        # init state
        self.__keyboard_layout = self.__builder.get_object("keyboardlayout")
        self.__candidate_keys = self.__builder.get_object("candidatekeys")
        self.__candidate_keys_entry = self.__candidate_keys.get_child()
        self.__candidate_num = self.__builder.get_object("candidatenum")

        # read value
        self.__keyboard_layout.set_active(self.__get_value("keyboardlayout", 0))
        self.__candidate_keys_entry.set_text(self.__get_value("candidatekeys", "1234567890"))
        self.__candidate_num.set_value(self.__get_value("candidatenum", 10))

        # connect signals
        self.__keyboard_layout.connect("changed", self.__keyboard_layout_cb, "keyboardlayout")
        self.__candidate_keys_entry.connect("changed", self.__candidate_keys_entry_cb, "candidatekeys")
        self.__candidate_num.connect("value-changed", self.__candidate_num_cb, "candidatenum")


    def __keyboard_layout_cb(self, widget, name):
        self.__set_value(name, widget.get_active())


    def __candidate_keys_entry_cb(self, widget, name):
        self.__set_value(name, widget.get_text())


    def __candidate_num_cb(self, widget, name):
        self.__set_value(name, int(widget.get_value()))


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
            for name, defval in self.__fuzzy_zhuyin_widgets:
                widget = self.__builder.get_object(name)
                widget.set_sensitive(val)

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
        # page User Phrases
        self.__page_user_phrases.show()

        # init state
        self.__box_user_symbol = self.__builder.get_object("boxUserSymbol")
        self.__user_symbol = self.__builder.get_object("usersymbol")
        self.__edit_user_symbol = self.__builder.get_object("editusersymbol")
        self.__box_easy_symbol = self.__builder.get_object("boxEasySymbol")
        self.__easy_symbol = self.__builder.get_object("easysymbol")
        self.__edit_easy_symbol = self.__builder.get_object("editeasysymbol")
        self.__import_dictionary = self.__builder.get_object("importdictionary")
        self.__clear_user_data = self.__builder.get_object("clearuserdata")
        self.__clear_all_data = self.__builder.get_object("clearalldata")

        # check file
        path = os.path.join(pkgdatadir, 'usersymbol.txt')
        if not os.access(path, os.R_OK):
            self.__box_user_symbol.hide()

        path = os.path.join(pkgdatadir, 'easysymbol.txt')
        if not os.access(path, os.R_OK):
            self.__box_easy_symbol.hide()

        # connect signals
        self.__user_symbol.connect("toggled", self.__enable_symbol_cb, "usersymbol", self.__edit_user_symbol)
        self.__edit_user_symbol.connect("clicked", self.__edit_symbol_cb, "usersymbol.txt")
        self.__easy_symbol.connect("toggled", self.__enable_symbol_cb, "easysymbol", self.__edit_easy_symbol)
        self.__edit_easy_symbol.connect("clicked", self.__edit_symbol_cb, "easysymbol.txt")
        self.__import_dictionary.connect("clicked", self.__import_dictionary_cb)
        self.__clear_user_data.connect("clicked", self.__clear_user_data_cb, "user")
        self.__clear_all_data.connect("clicked", self.__clear_user_data_cb, "all")

        # read value
        self.__user_symbol.set_active(self.__get_value("usersymbol", True))
        self.__easy_symbol.set_active(self.__get_value("easysymbol", True))


    def __enable_symbol_cb(self, widget, name, editbutton):
        editbutton.set_sensitive(widget.get_active ())
        self.__set_value(name, widget.get_active ())


    def __edit_symbol_cb(self, widget, filename):
        import shutil
        path = os.path.join(GLib.get_user_config_dir(), "ibus", "libzhuyin")
        os.path.exists(path) or os.makedirs(path)
        path = os.path.join(path, filename)
        if not os.path.exists(path):
            src = os.path.join(pkgdatadir, filename)
            shutil.copyfile(src, path)
        os.system("xdg-open %s" % path)


    def __import_dictionary_cb(self, widget):
        dialog = Gtk.FileChooserDialog \
            (_("Please choose a file"), None,
             Gtk.FileChooserAction.OPEN,
             (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
              Gtk.STOCK_OPEN, Gtk.ResponseType.OK))

        filter_text = Gtk.FileFilter()
        filter_text.set_name("Text files")
        filter_text.add_mime_type("text/plain")
        dialog.add_filter(filter_text)

        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            self.__set_value("importdictionary", dialog.get_filename())

        dialog.destroy()


    def __clear_user_data_cb(self, widget, name):
        self.__set_value("clearuserdata", name)


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
            print("val(%s) is not in support type." % repr(val), file=sys.stderr)
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
