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

BuiltinSymbolSection::BuiltinSymbolSection (PhoneticEditor & editor) :
    SymbolSection (editor)
{
    m_type = "builtin";
}

BuiltinSymbolSection::~BuiltinSymbolSection ()
{
}

bool
BuiltinSymbolSection::initCandidates (const String & lookup)
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
BuiltinSymbolSection::populateCandidates ()
{
    /* clear lookup table. */
    LookupTable & lookup_table = getLookupTable ();
    lookup_table.clear ();

    iterator_t iter = m_candidates.begin ();
    for (; iter != m_candidates.end (); ++iter) {
        lookup_table.appendCandidate (Text (*iter));
    }

    return true;
}

bool
BuiltinSymbolSection::selectCandidate (guint index)
{
    m_choice = m_candidates[index];
    return true;
}

};
