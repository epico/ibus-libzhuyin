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

#include "ZYZBopomofoSymbolSection.h"
#include <assert.h>
#include <zhuyin.h>

namespace ZY {

BopomofoSymbolSection::BopomofoSymbolSection (PhoneticEditor & editor,
                                              ZhuyinProperties & props) :
    SymbolSection (editor, props)
{
    m_type = "bopomofo";
}

BopomofoSymbolSection::~BopomofoSymbolSection ()
{
}

bool
BopomofoSymbolSection::initCandidates (const String & lookup)
{
    assert (1 == lookup.length ());
    m_lookup = lookup;
    const char key = lookup[0];

    /* cache the choices. */
    gchar ** symbols = NULL;
    assert (zhuyin_in_chewing_keyboard (instance, key, &symbols));
    size_t num = g_strv_length (symbols);
    assert (num > 0);
    for (size_t i = 0; i < num; ++i) {
        m_candidates.push_back (symbols[i]);
    }
    g_strfreev (symbols);

    return true;
}

bool
BopomofoSymbolSection::fillLookupTableByPage ()
{
    LookupTable & lookup_table = getLookupTable ();

    guint len = m_candidates.size ();

    guint filled_nr = lookup_table.size ();
    guint page_size = lookup_table.pageSize ();

    /* fill lookup table by libzhuyin get candidates. */
    guint need_nr = MIN (page_size, len - filled_nr);
    g_assert (need_nr >=0);
    if (need_nr == 0)
        return FALSE;

    for (guint i = filled_nr; i < filled_nr + need_nr; i++) {
        if (i >= len)  /* no more candidates */
            break;

        Text text (m_candidates[i]);
        lookup_table.appendCandidate (text);
    }

    return TRUE;
}

int
BopomofoSymbolSection::selectCandidate (guint index)
{
    m_choice = m_candidates[index];
    return g_utf8_strlen (m_choice, -1);;
}

};
