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

from gi import require_version as gi_require_version
gi_require_version('GLib', '2.0')
gi_require_version('Gio', '2.0')
gi_require_version('Gtk', '3.0')
gi_require_version('IBus', '1.0')

from gi.repository import GLib
from gi.repository import Gio

# set_prgname before importing other modules to show the name in warning
# messages when import modules are failed. E.g. Gtk.
GLib.set_prgname('ibus-setup-libzhuyin')

from gi.repository import Gtk
from gi.repository import IBus

import config

DOMAINNAME = 'ibus-libzhuyin'
locale.setlocale(locale.LC_ALL, "")
localedir = os.getenv("IBUS_LOCALEDIR")
pkgdatadir = os.getenv("IBUS_PKGDATADIR") or "."

# Python's locale module doesn't provide all methods on some
# operating systems like FreeBSD
try:
    locale.bindtextdomain(DOMAINNAME, localedir)
    locale.bind_textdomain_codeset(DOMAINNAME, 'UTF-8')
except AttributeError:
    pass

# Python's gettext module doesn't provide all methods in
# new Python version
try:
    gettext.bindtextdomain(DOMAINNAME, localedir)
    gettext.bind_textdomain_codeset(DOMAINNAME, 'UTF-8')
except AttributeError:
    pass

gettext.install(DOMAINNAME, localedir)

class PreferencesWindow:
    def __init__(self, engine, parent=None):
        self.__bus = IBus.Bus()
        self.__builder = Gtk.Builder()
        self.__builder.set_translation_domain(DOMAINNAME)
        self.__builder.add_from_file("ibus-libzhuyin-preferences.ui")
        self.__window = self.__builder.get_object("window")
        self.__init_pages()

        if engine == "zhuyin":
            self.__config_namespace = "com.github.libzhuyin.ibus-libzhuyin"
            self.__config = Gio.Settings.new(self.__config_namespace)
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
        self.__chinese_mode = self.__builder.get_object("chinese_mode")
        self.__full_half_width = self.__builder.get_object("full_half_width")
        self.__traditional_chinese = self.__builder.get_object("traditional_chinese")
        self.__always_input_num = self.__builder.get_object("always_input_num")
        self.__space_show_candidates = self.__builder.get_object("space_show_candidates")
        self.__candidates_after_cursor = self.__builder.get_object("candidates_after_cursor")

        # read values
        self.__chinese_mode.set_active(self.__get_value("chinese-mode"))
        self.__full_half_width.set_active(self.__get_value("full-half-width"))
        self.__traditional_chinese.set_active(self.__get_value("traditional-chinese"))
        self.__always_input_num.set_active(self.__get_value("always-input-num"))
        self.__space_show_candidates.set_active(self.__get_value("space-show-candidates"))
        self.__candidates_after_cursor.set_active(self.__get_value("candidates-after-cursor"))

        # connect signals
        self.__chinese_mode.connect("toggled", self.__toggled_cb, "chinese-mode")
        self.__full_half_width.connect("toggled", self.__toggled_cb, "full-half-width")
        self.__traditional_chinese.connect("toggled", self.__toggled_cb, "traditional-chinese")
        self.__always_input_num.connect("toggled", self.__toggled_cb, "always-input-num")
        self.__space_show_candidates.connect("toggled", self.__toggled_cb, "space-show-candidates")
        self.__candidates_after_cursor.connect("toggled", self.__toggled_cb, "candidates-after-cursor")


    def __init_keyboard(self):
        # page Keyboard
        self.__page_keyboard.show()

        # init state
        self.__keyboard_layout = self.__builder.get_object("keyboard_layout")
        self.__need_tone = self.__builder.get_object("need_tone")
        self.__candidate_keys = self.__builder.get_object("candidate_keys")
        self.__candidate_keys_entry = self.__candidate_keys.get_child()
        self.__candidate_num = self.__builder.get_object("candidate_num")

        # read value
        self.__keyboard_layout.set_active(self.__get_value("keyboard-layout"))
        self.__need_tone.set_active(self.__get_value("need-tone"))
        tree_iter = self.__keyboard_layout.get_active_iter()
        if tree_iter is not None:
            model = self.__keyboard_layout.get_model()
            self.__need_tone.set_sensitive(model[tree_iter][2])

        self.__candidate_keys_entry.set_text(self.__get_value("candidate-keys"))
        self.__candidate_num.set_value(self.__get_value("candidate-num"))

        # connect signals
        self.__keyboard_layout.connect("changed", self.__keyboard_layout_cb, "keyboard-layout")
        self.__need_tone.connect("toggled", self.__toggled_cb, "need-tone")
        self.__candidate_keys_entry.connect("changed", self.__candidate_keys_entry_cb, "candidate-keys")
        self.__candidate_num.connect("value-changed", self.__candidate_num_cb, "candidate-num")


    def __keyboard_layout_cb(self, widget, name):
        self.__set_value(name, widget.get_active())
        tree_iter = widget.get_active_iter()
        if tree_iter is not None:
            model = widget.get_model()
            self.__need_tone.set_active(model[tree_iter][1])
            self.__need_tone.set_sensitive(model[tree_iter][2])


    def __candidate_keys_entry_cb(self, widget, name):
        self.__set_value(name, widget.get_text())


    def __candidate_num_cb(self, widget, name):
        self.__set_value(name, int(widget.get_value()))


    def __init_fuzzy_zhuyin(self):
        # page Fuzzy Zhuyin
        self.__page_fuzzy_zhuyin.show()

        # fuzzy zhuyin
        self.__fuzzy_zhuyin = self.__builder.get_object("fuzzy_zhuyin")
        self.__fuzzy_zhuyin_widgets = [
            ('fuzzy_zhuyin_c_ch', 'fuzzy-zhuyin-c-ch'),
            ('fuzzy_zhuyin_z_zh', 'fuzzy-zhuyin-z-zh'),
            ('fuzzy_zhuyin_s_sh', 'fuzzy-zhuyin-s-sh'),
            ('fuzzy_zhuyin_l_n', 'fuzzy-zhuyin-l-n'),
            ('fuzzy_zhuyin_f_h', 'fuzzy-zhuyin-f-h'),
            ('fuzzy_zhuyin_l_r', 'fuzzy-zhuyin-l-r'),
            ('fuzzy_zhuyin_g_k', 'fuzzy-zhuyin-g-k'),
            ('fuzzy_zhuyin_an_ang', 'fuzzy-zhuyin-an-ang'),
            ('fuzzy_zhuyin_en_eng', 'fuzzy-zhuyin-en-eng'),
            ('fuzzy_zhuyin_in_ing', 'fuzzy-zhuyin-in-ing'),
        ]

        def __fuzzy_zhuyin_toggled_cb(widget):
            val = widget.get_active()
            for name, keyname in self.__fuzzy_zhuyin_widgets:
                widget = self.__builder.get_object(name)
                widget.set_sensitive(val)

        self.__fuzzy_zhuyin.connect("toggled", __fuzzy_zhuyin_toggled_cb)

        # init value
        self.__fuzzy_zhuyin.set_active(self.__get_value("fuzzy-zhuyin"))
        for name, keyname in self.__fuzzy_zhuyin_widgets:
            widget = self.__builder.get_object(name)
            widget.set_active(self.__get_value(keyname))

        self.__fuzzy_zhuyin.connect("toggled", self.__toggled_cb, "fuzzy-zhuyin")
        for name, keyname in self.__fuzzy_zhuyin_widgets:
            widget = self.__builder.get_object(name)
            widget.connect("toggled", self.__toggled_cb, keyname)


    def __init_user_phrases(self):
        # page User Phrases
        self.__page_user_phrases.show()

        # init state
        self.__box_user_symbol = self.__builder.get_object("boxUserSymbol")
        self.__user_symbol = self.__builder.get_object("user_symbol")
        self.__edit_user_symbol = self.__builder.get_object("edit_user_symbol")
        self.__box_easy_symbol = self.__builder.get_object("boxEasySymbol")
        self.__easy_symbol = self.__builder.get_object("easy_symbol")
        self.__edit_easy_symbol = self.__builder.get_object("edit_easy_symbol")
        self.__import_dictionary = self.__builder.get_object("import_dictionary")
        self.__clear_user_data = self.__builder.get_object("clear_user_data")
        self.__clear_all_data = self.__builder.get_object("clear_all_data")

        # check file
        path = os.path.join(pkgdatadir, 'usersymbol.txt')
        if not os.access(path, os.R_OK):
            self.__box_user_symbol.hide()

        path = os.path.join(pkgdatadir, 'easysymbol.txt')
        if not os.access(path, os.R_OK):
            self.__box_easy_symbol.hide()

        # connect signals
        self.__user_symbol.connect("toggled", self.__enable_symbol_cb, "user-symbol", self.__edit_user_symbol)
        self.__edit_user_symbol.connect("clicked", self.__edit_symbol_cb, "usersymbol.txt")
        self.__easy_symbol.connect("toggled", self.__enable_symbol_cb, "easy-symbol", self.__edit_easy_symbol)
        self.__edit_easy_symbol.connect("clicked", self.__edit_symbol_cb, "easysymbol.txt")
        self.__import_dictionary.connect("clicked", self.__import_dictionary_cb)
        self.__clear_user_data.connect("clicked", self.__clear_user_data_cb, "user")
        self.__clear_all_data.connect("clicked", self.__clear_user_data_cb, "all")

        # read value
        self.__user_symbol.set_active(self.__get_value("user-symbol"))
        self.__easy_symbol.set_active(self.__get_value("easy-symbol"))


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
            self.__set_value("import-dictionary", dialog.get_filename())

        dialog.destroy()


    def __clear_user_data_cb(self, widget, name):
        self.__set_value("clear-user-data", name)


    def __init_about(self):
        # page About
        self.__page_about.show()

        self.__name_version = self.__builder.get_object("name_version")
        self.__name_version.set_markup(_("<big><b>New Zhuyin %s</b></big>") % config.get_version())


    def __update_fuzzy_zhuyin(self):
        options = [
            ("fuzzy_zhuyin_c_ch", "ㄘ <=> ㄔ"),
            ("fuzzy_zhuyin_z_zh", "ㄗ <=> ㄓ"),
            ("fuzzy_zhuyin_s_sh", "ㄙ <=> ㄕ"),
            ("fuzzy_zhuyin_l_n", "ㄌ <=> ㄋ"),
            ("fuzzy_zhuyin_f_h", "ㄈ <=> ㄏ"),
            ("fuzzy_zhuyin_l_r", "ㄌ <=> ㄖ"),
            ("fuzzy_zhuyin_g_k", "ㄍ <=> ㄎ"),
            ("fuzzy_zhuyin_an_ang", "ㄢ <=> ㄤ"),
            ("fuzzy_zhuyin_en_eng", "ㄣ <=> ㄥ"),
            ("fuzzy_zhuyin_in_ing", "ㄧㄣ <=> ㄧㄥ"),
        ]

        for name, label in options:
            self.__builder.get_object(name).set_label(label)


    def __toggled_cb(self, widget, name):
        self.__set_value(name, widget.get_active ())

    def __get_value(self, name):
        var = self.__config.get_value(name)
        vartype = var.get_type_string()
        if vartype == 'b':
            return var.get_boolean()
        elif vartype == 'i':
            return var.get_int32()
        elif vartype == 's':
            return var.get_string()
        else:
            print("var(%s) is not in support type." % repr(var), file=sys.stderr)
            return None

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

        self.__config.set_value(name, var)


    def show(self):
        self.__window.show()


def main():
    name = "zhuyin"
    PreferencesWindow(name).show()
    Gtk.main()


if __name__ == "__main__":
    main()
