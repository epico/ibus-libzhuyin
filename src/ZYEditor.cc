/* vim:set et ts=4 sts=4:
 *
 * ibus-libzhuyin - New Zhuyin engine based on libzhuyin for IBus
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "ZYText.h"
#include "ZYEditor.h"

namespace ZY {

Editor::Editor (ZhuyinProperties & props, Config & config)
    : m_cursor (0),
      m_props (props),
      m_config (config)
{
}

Editor::~Editor (void)
{
}

gboolean
Editor::processKeyEvent (guint keyval, guint keycode, guint modifiers)
{
    if (modifiers & IBUS_MOD4_MASK)
        return FALSE;

    modifiers &= (IBUS_CONTROL_MASK |
                  IBUS_MOD1_MASK |
                  IBUS_SUPER_MASK |
                  IBUS_HYPER_MASK |
                  IBUS_META_MASK);
    /* ignore key events with some masks */
    if (modifiers != 0)
        return FALSE;

    if (keyval >= IBUS_exclam && keyval <= IBUS_asciitilde) {
        /* char key */
        m_text.insert (m_cursor++, keyval);
        update ();
        return TRUE;
    }
    else {
        /* control key */
        if (!m_text)
            return FALSE;
    }

    switch (keyval) {
    case IBUS_BackSpace:
        if (m_cursor > 0) {
            m_text.erase (--m_cursor, 1);
            update ();
        }
        return TRUE;
    case IBUS_Delete:
    case IBUS_KP_Delete:
        if (m_cursor < m_text.length ()) {
            m_text.erase (m_cursor, 1);
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
    case IBUS_space:
    case IBUS_Return:
    case IBUS_KP_Enter:
        {
            StaticText text (m_text);
            commitText (text);
            reset ();
        }
        return TRUE;
    case IBUS_Escape:
        if (m_text.empty ())
            return FALSE;

        reset ();
        return TRUE;
    default:
        return TRUE;
    }
}

void
Editor::reset (void)
{
    gboolean need_update = (m_cursor != 0 || !m_text.empty ());
    m_cursor = 0;
    m_text = "";
    if (need_update)
        update ();
}

void
Editor::pageUp (void)
{
}

void
Editor::pageDown (void)
{
}

void
Editor::cursorUp (void)
{
}

void
Editor::cursorDown (void)
{
}

void
Editor::candidateClicked (guint index, guint button, guint state)
{
}

void
Editor::update (void)
{
    if (m_text) {
        StaticText text (m_text);
        text.appendAttribute (IBUS_ATTR_TYPE_UNDERLINE, IBUS_ATTR_UNDERLINE_SINGLE, 0, -1);
        updatePreeditText (text, m_cursor, TRUE);
    }
    else {
        hidePreeditText ();
    }
}

};

