/* vim:set et ts=4 sts=4:
 *
 * ibus-libzhuyin - New Zhuyin engine based on libzhuyin for IBus
 *
 * Copyright (c) 2014 Peng Wu <alexepico@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __ZY_SYMBOLS_H_
#define __ZY_SYMBOLS_H_

#include "ZYString.h"
#include <glib.h>

namespace ZY {

bool
is_easy_symbol(const char key);

int
find_lookup_key (const String & symbol);

int
get_choice_list (const char key, gchar ** & choices);

bool
is_half_punct (const char key);

bool
half_punct_to_full_punct (const char key, String & punct);

bool
is_half_english (const char key);

bool
half_english_to_full_english (const char key, String & english);

#define BUILTIN_SYMBOL_TYPE "builtin"
#define BOPOMOFO_SYMBOL_TYPE "bopomofo"


};

#endif
