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

#include "ZYFallbackEditor.h"
#include <assert.h>
#include "ZYSymbols.h"
#include "ZYZhuyinProperties.h"

namespace ZY {

inline gboolean
FallbackEditor::processSymbol (guint keyval, guint keycode, guint modifiers)
{
    guint cmshm_modifiers = cmshm_filter (modifiers);

    /* check ctrl, alt, hyper, supper masks */
    if (cmshm_modifiers != 0)
        return FALSE;

    /* English mode */
    if (!m_props.modeChinese ()) {

        /* Punctuation character */
        if (is_full_width_symbol (keyval)) {
            if(G_UNLIKELY (m_props.modeFullWidth ())) {
                String symbol;
                convert_full_width_symbol (keyval, symbol);
                commit (symbol);
            } else {
                commit (keyval);
            }
            return TRUE;
        }

    } else {
        /* Chinese mode, handled by ZhuyinEditor or PinyinEditor. */
        return TRUE;
    }

    return FALSE;
}

gboolean
FallbackEditor::processKeyEvent (guint keyval, guint keycode, guint modifiers)
{
    gboolean retval = FALSE;

    if (modifiers & IBUS_MOD4_MASK)
        return FALSE;

    modifiers &= (IBUS_CONTROL_MASK |
                  IBUS_MOD1_MASK |
                  IBUS_SUPER_MASK |
                  IBUS_HYPER_MASK |
                  IBUS_META_MASK);

    switch (keyval) {
        /* numbers */
        case IBUS_KP_0 ... IBUS_KP_9:
            keyval = keyval - IBUS_KP_0 + IBUS_0;
        case IBUS_0 ... IBUS_9:
        /* letters */
        case IBUS_a ... IBUS_z:
        case IBUS_A ... IBUS_Z:
            if (modifiers == 0) {
                retval = processSymbol (keyval, keycode, modifiers);
            }
            break;
        /* punct */
        case IBUS_exclam ... IBUS_slash:
        case IBUS_colon ... IBUS_at:
        case IBUS_bracketleft ... IBUS_quoteleft:
        case IBUS_braceleft ... IBUS_asciitilde:
            retval = processSymbol (keyval, keycode, modifiers);
            break;
        case IBUS_KP_Equal:
            retval = processSymbol ('=', keycode, modifiers);
            break;
        case IBUS_KP_Multiply:
            retval = processSymbol ('*', keycode, modifiers);
            break;
        case IBUS_KP_Add:
            retval = processSymbol ('+', keycode, modifiers);
            break;
        #if 0
        case IBUS_KP_Separator:
            retval = processSymbol (IBUS_separator, keycode, modifiers);
            break;
        #endif
        case IBUS_KP_Subtract:
            retval = processSymbol ('-', keycode, modifiers);
            break;
        case IBUS_KP_Decimal:
            retval = processSymbol ('.', keycode, modifiers);
            break;
        case IBUS_KP_Divide:
            retval = processSymbol ('/', keycode, modifiers);
            break;
        /* space */
        case IBUS_KP_Space:
            keyval = IBUS_space;
        case IBUS_space:
            retval = processSymbol (keyval, keycode, modifiers);
            break;
        /* others */
        default:
            break;
    }

    return retval;
}

void
FallbackEditor::reset (void) {
    m_quote = TRUE;
    m_double_quote = TRUE;
    m_prev_committed_char = 0;
}

};
