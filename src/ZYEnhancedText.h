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

#ifndef __ZY_ENHANCED_TEXT_H_
#define __ZY_ENHANCED_TEXT_H_

#include "ZYString.h"

namespace ZY{

typedef enum {
    PHONETIC_SECTION,
    SYMBOL_SECTION
} section_t;

typedef String::const_iterator iterator_t;

/* all enhanced text should loop from the start to get the correct position. */
size_t
get_enhanced_text_length (const String & enhanced_text);

/* offset is for the input offset,
   pos is for the actual position in enhanced text. */
section_t
probe_section (const String & enhanced_text, size_t offset, size_t & pos);

bool
get_phonetic_section (const String & enhanced_text,
                      size_t start_pos, size_t & end_pos, String & section);

/* only one input character here. */
bool
get_symbol_section (const String & enhanced_text,
                    size_t start_pos, size_t & end_pos,
                    String & type, String & lookup, String & choice);

bool
insert_phonetic(String & enhanced_text, size_t offset, const char key);

bool
insert_symbol(String & enhanced_text, size_t offset,
              const String & type, const String & lookup,
              const String & choice);

};

#endif
