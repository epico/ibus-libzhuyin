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

#include "ZYZUserSymbolListAllSection.h"
#include <assert.h>
#include "ZYSymbols.h"

namespace ZY {

UserSymbolListAllSection::UserSymbolListAllSection (PhoneticEditor & editor,
                                                    ZhuyinProperties & props) :
    SymbolSection (editor, props)
{
    m_type = BUILTIN_SYMBOL_TYPE;

    /* load user symbol. */
    gchar * path = g_build_filename (g_get_user_config_dir (),
                                     "ibus", "libzhuyin",
                                     "usersymbol.txt", NULL);
    loadUserSymbolFile (".." G_DIR_SEPARATOR_S "data" G_DIR_SEPARATOR_S
                        "usersymbol.txt") ||
        loadUserSymbolFile (path) ||
        loadUserSymbolFile (PKGDATADIR G_DIR_SEPARATOR_S "usersymbol.txt");
    g_free(path);
}

UserSymbolListAllSection::~UserSymbolListAllSection ()
{
}

gboolean
UserSymbolListAllSection::loadUserSymbolFile (const gchar * filename)
{
    printf ("load %s.\n", filename);
    gboolean retval = m_user_symbols.loadFromFile (filename);
    return retval;
}

bool
UserSymbolListAllSection::initCandidates (zhuyin_instance_t * instance,
                                          const String & lookup)
{
    if (!lookup)
        return false;

    m_candidates.clear ();

    assert ("`" == lookup);
    m_lookup = lookup;

    /* cache the symbols. */
    const std::vector<String> & indexes = m_user_symbols.getIndexes ();
    for (size_t i = 0; i < indexes.size (); ++i) {
        m_candidates.push_back (indexes[i]);
    }

    return true;
}

bool
UserSymbolListAllSection::fillLookupTableByPage ()
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
UserSymbolListAllSection::selectCandidate (guint index)
{
    if (index >= m_candidates.size ())
        return 0;

    m_choice = m_candidates[index];
    return m_choice.utf8Length ();
}

};
