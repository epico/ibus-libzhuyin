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

#include "ZYZUserSymbolShownSection.h"
#include <assert.h>
#include "ZYSymbols.h"

namespace ZY {

UserSymbolShownSection::UserSymbolShownSection (PhoneticEditor & editor,
                                                ZhuyinProperties & props) :
    SymbolSection (editor, props)
{
    m_type = BUILTIN_SYMBOL_TYPE;
}

UserSymbolShownSection::~UserSymbolShownSection ()
{
}

bool
UserSymbolShownSection::initCandidates (zhuyin_instance_t * instance,
                                        const String & lookup)
{
    if (!lookup)
        return false;

    m_candidates.clear ();

    m_lookup = lookup;

    /* get the symbols from lookup string. */
    const gchar * str = m_lookup.c_str ();
    gboolean valid = g_utf8_validate (str, -1, NULL);
    g_return_val_if_fail (valid, false);

    while (*str) {
        gunichar unichar = g_utf8_get_char (str);
        String utf8ch;
        utf8ch.appendUnichar (unichar);
        m_candidates.push_back (utf8ch);
        str = g_utf8_next_char (str);
    }

    return true;
}

bool
UserSymbolShownSection::fillLookupTableByPage ()
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
UserSymbolShownSection::selectCandidate (guint index)
{
    if (index >= m_candidates.size ())
        return 0;

    m_choice = m_candidates[index];
    assert (1 == m_choice.utf8Length ());
    return 1;
}

};
