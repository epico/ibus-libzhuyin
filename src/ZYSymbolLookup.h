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

#ifndef __ZY_SYMBOL_LOOKUP_H_
#define __ZY_SYMBOL_LOOKUP_H_

#include "ZYString.h"
#include <vector>
#include <glib.h>

namespace ZY {

class SymbolLookup {
private:
    typedef std::vector<String> symbol_vec_t;

public:
    SymbolLookup () {}
    virtual ~SymbolLookup () {}

    gboolean loadFromFile (const char * filename);

    String find(const String index);
    const std::vector<String> & getIndexes() { return m_indexes; }

    void dumpInfo ();
protected:
    symbol_vec_t m_indexes;
    symbol_vec_t m_symbols;
};

};

#endif
