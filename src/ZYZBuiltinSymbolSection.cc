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

#include "ZYZBuiltinSymbolSection.h"
#include <assert.h>
#include "ZYSymbols.h"

namespace ZY {

BuiltinSymbolSection::BuiltinSymbolSection (PhoneticEditor & editor,
                                            ZhuyinProperties & props) :
    SymbolSection (editor, props)
{
    m_type = BUILTIN_SYMBOL_TYPE;
}

BuiltinSymbolSection::~BuiltinSymbolSection ()
{
}

bool
BuiltinSymbolSection::initCandidates (zhuyin_instance_t * instance,
                                      const String & lookup)
{
    assert (1 == lookup.length ());
    m_lookup = lookup;

    /* cache the choices. */
    gchar ** choices = NULL;
    size_t num = get_choice_list (m_lookup[0], choices);
    assert (num > 0);
    for (size_t i = 0; i < num; ++i) {
        m_candidates.push_back (choices[i]);
    }

    return true;
}

bool
BuiltinSymbolSection::fillLookupTableByPage ()
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
BuiltinSymbolSection::selectCandidate (guint index)
{
    m_choice = m_candidates[index];
    return g_utf8_strlen (m_choice, -1);
}

};
