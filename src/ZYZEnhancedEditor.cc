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


#include "ZYZEnhancedEditor.h"
#include <assert.h>
#include "ZYEnhancedText.h"

namespace ZY {

EnhancedEditor::EnhancedEditor (ZhuyinProperties & props, Config & config)
    : Editor (props, config)
{
}

EnhancedEditor::~EnhancedEditor (void)
{
}

gboolean
EnhancedEditor::processEvent (guint keyval, guint keycode, guint modifiers)
{
    modifiers &= (IBUS_CONTROL_MASK |
                  IBUS_MOD1_MASK |
                  IBUS_SUPER_MASK |
                  IBUS_HYPER_MASK |
                  IBUS_META_MASK);
    /* ignore key events with some masks */
    if (modifiers != 0)
        return TRUE;

    if (isPhonetic (keyval)) {
        /* zhuyin or pinyin key */
        assert (insert_phonetic (m_text, m_cursor++, keyval));
        update ();
        return TRUE;
    } else {
        /* control key */
        if (!m_text)
            return FALSE;
    }

    switch (keyval) {
    case IBUS_BackSpace:
        if (m_cursor > 0) {
            erase_input_sequence (m_text, --m_cursor, 1);
            update ();
        }
        return TRUE;
    case IBUS_Delete:
    case IBUS_KP_Delete:
        if (m_cursor < get_enhanced_text_length (m_text)) {
            erase_input_sequence (m_text, m_cursor, 1);
            update ();
        }
        return TRUE;
    case IBUS_Left:
    case IBUS_KP_Left:
        if (!m_text)
            return FALSE;
        if (m_cursor > 0) {
            m_cursor --;
            update ();
        }
        return TRUE;
    case IBUS_Right:
    case IBUS_KP_Right:
        if (m_cursor < m_text.length ()) {
            m_cursor ++;
            update ();
        }
        return TRUE;
    case IBUS_Escape:
        reset ();
        return TRUE;
    default:
        return TRUE;
    }
}

};
