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
    gboolean retval = m_user_symbols.loadFromFile (filename);
    return retval;
}

};
