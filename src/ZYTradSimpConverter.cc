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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "ZYTradSimpConverter.h"

#include <opencc.h>

#include "ZYTypes.h"
#include "ZYString.h"

namespace ZY {

class opencc {
public:
    opencc (void)
    {
        m_cc = opencc_open (OPENCC_DEFAULT_CONFIG_TRAD_TO_SIMP);
        g_assert (m_cc != NULL);
    }

    ~opencc (void)
    {
        opencc_close (m_cc);
    }

    void convert (const gchar *in, String &out)
    {
        char * converted = opencc_convert_utf8 (m_cc, in, -1);
        g_assert (converted != NULL);
        out = converted;
        opencc_convert_utf8_free (converted);
    }

private:
    opencc_t m_cc;
};

void
TradSimpConverter::tradToSimp (const gchar *in, String &out)
{
    static opencc cc;
    cc.convert (in, out);
}

};
