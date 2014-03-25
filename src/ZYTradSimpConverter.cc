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
    static const int BUFFER_SIZE = 64;
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
        glong n_char;
        gunichar *in_ucs4 = g_utf8_to_ucs4_fast (in, -1, &n_char);

        ucs4_t *pinbuf = (ucs4_t *)in_ucs4;
        size_t inbuf_left = n_char;
        while (inbuf_left != 0) {
            ucs4_t *poutbuf = (ucs4_t *)m_buffer;
            size_t outbuf_left = BUFFER_SIZE;
            size_t retval = opencc_convert(m_cc, &pinbuf, &inbuf_left, &poutbuf, &outbuf_left);
            if (retval == (size_t) -1) {
                /* append left chars in pinbuf */
                g_warning ("opencc_convert return failed");
                out << (gunichar *) pinbuf;
                break;
            }
            *poutbuf = L'\0';
            out << m_buffer;
        }
        g_free (in_ucs4);
    }

private:
    opencc_t m_cc;
    gunichar m_buffer[BUFFER_SIZE + 1];
};

void
TradSimpConverter::tradToSimp (const gchar *in, String &out)
{
    static opencc cc;
    cc.convert (in, out);
}

};
