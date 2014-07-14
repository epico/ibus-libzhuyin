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

#include "ZYZPhoneticSection.h"
#include <assert.h>
#include "ZYZhuyinProperties.h"
#include "ZYTradSimpConverter.h"

namespace ZY {

PhoneticSection::PhoneticSection (PhoneticEditor & editor,
                                  ZhuyinProperties & props) :
    m_editor (editor), m_props (props)
{
}

PhoneticSection::~PhoneticSection ()
{
}

bool
PhoneticSection::initCandidates (zhuyin_instance_t * instance,
                                 int cursor)
{
    m_instance = instance;
    m_cursor = cursor;

    guint16 offset = 0;
    zhuyin_get_zhuyin_key_rest_offset
        (instance, cursor, &offset);

    zhuyin_guess_candidates (m_instance, offset);

    return true;
}

bool
PhoneticSection::fillLookupTableByPage ()
{
    LookupTable & lookup_table = getLookupTable ();

    guint len = 0;
    zhuyin_get_n_candidate (m_instance, &len);

    guint filled_nr = lookup_table.size ();
    guint page_size = lookup_table.pageSize ();

    /* fill lookup table by libzhuyin get candidates. */
    guint need_nr = MIN (page_size, len - filled_nr);
    g_assert (need_nr >=0);
    if (need_nr == 0)
        return FALSE;

    String word;
    for (guint i = filled_nr; i < filled_nr + need_nr; i++) {
        if (i >= len)  /* no more candidates */
            break;

        lookup_candidate_t * candidate = NULL;
        zhuyin_get_candidate (m_instance, i, &candidate);

        const gchar * phrase_string = NULL;
        zhuyin_get_candidate_string (m_instance, candidate, &phrase_string);

        /* show get candidates. */
        if (G_LIKELY (m_props.modeTrad ())) {
            word = phrase_string;
        } else { /* Simplified Chinese */
            word.truncate (0);
            TradSimpConverter::tradToSimp (phrase_string, word);
        }

        Text text (word);
        lookup_table.appendCandidate (text);
    }

    return TRUE;
}

int
PhoneticSection::selectCandidate (guint index)
{
    guint16 prev_pos = m_cursor, cur_pos = 0;
    ChewingKeyRest * key_rest = NULL;

    guint len = 0;
    zhuyin_get_n_candidate (m_instance, &len);

    if (index >= len)
        return 0;

    lookup_candidate_t * candidate = NULL;
    zhuyin_get_candidate (m_instance, index, &candidate);

    guint16 offset = 0;
    zhuyin_get_zhuyin_key_rest_offset
        (m_instance, m_cursor, &offset);

    offset = zhuyin_choose_candidate
        (m_instance, offset, candidate);

    zhuyin_get_n_zhuyin (m_instance, &len);

    if (offset < len) {
        zhuyin_get_zhuyin_key_rest (m_instance, offset, &key_rest);
        zhuyin_get_zhuyin_key_rest_positions
            (m_instance, key_rest, &cur_pos, NULL);
    } else {
        assert (offset == len);
        cur_pos = zhuyin_get_parsed_input_length (m_instance);
    }

    return cur_pos - prev_pos;
}

};
