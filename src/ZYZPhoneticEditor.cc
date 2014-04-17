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

#include "ZYZPhoneticEditor.h"
#include <assert.h>
#include "ZYConfig.h"
#include "ZYZhuyinProperties.h"
#include "ZYZPhoneticSection.h"
#include "ZYZBuiltinSymbolSection.h"

namespace ZY {

/**
 * Implementation Notes:
 * 1. support input editing;
 * 2. support phonetic candidates;
 * 3. support built-in symbols candidates;
 * 4. support list all user symbols;
 * 5. support show user symbols;
 * 6. support easy symbols input;
 */

/* init static members */
PhoneticEditor::PhoneticEditor (ZhuyinProperties & props, Config & config)
    : EnhancedEditor (props, config),
      m_lookup_table (m_config.pageSize ())
{
    /* TODO: alloc one instance here. */
    m_instance = NULL;
    assert (NULL != m_instance);

    /* init symbols sections here. */
    m_symbol_sections[STATE_BUILTIN_SYMBOL_SHOWN].reset
        (new BuiltinSymbolSection (*this));

    m_phonetic_section.reset
        (new PhoneticSection (*this));
}

PhoneticEditor::~PhoneticEditor (void)
{
}

gboolean
PhoneticEditor::processEnter (guint keyval, guint keycode,
                              guint modifiers)
{
    if (!m_text)
        return FALSE;
    if (cmshm_filter (modifiers) != 0)
        return TRUE;

    commit ();
    return TRUE;
}

gboolean
PhoneticEditor::processFunctionKey (guint keyval, guint keycode,
                                    guint modifiers)
{
    if (m_text.empty ())
        return FALSE;

    /* ignore numlock */
    modifiers = cmshm_filter (modifiers);

    if (modifiers != 0)
        return TRUE;

    /* process some cursor control keys */
    if (modifiers == 0) { /* no modifiers. */
        switch (keyval) {
        case IBUS_BackSpace:
            removeCharBefore ();
            return TRUE;

        case IBUS_Delete:
        case IBUS_KP_Delete:
            removeCharAfter ();
            return TRUE;

        case IBUS_Left:
        case IBUS_KP_Left:
            moveCursorLeft ();
            return TRUE;

        case IBUS_Right:
        case IBUS_KP_Right:
            moveCursorRight ();
            return TRUE;

        case IBUS_Home:
        case IBUS_KP_Home:
            moveCursorToBegin ();
            return TRUE;

        case IBUS_End:
        case IBUS_KP_Enter:
            moveCursorToEnd ();
            return TRUE;

        case IBUS_Escape:
            reset ();
            return TRUE;

        default:
            return TRUE;
        }
    }

    return TRUE;
}

gboolean
PhoneticEditor::processCandidateKey (guint keyval, guint keycode,
                                     guint modifiers)
{
    if (!m_lookup_table.size ())
        return FALSE;

    /* ignore numlock */
    modifiers = cmshm_filter (modifiers);

    if (modifiers != 0)
        return TRUE;

    /* process some cursor control keys */
    if (modifiers == 0) { /* no modifiers. */
        switch (keyval) {
        case IBUS_Up:
        case IBUS_KP_Up:
            cursorUp ();
            return TRUE;

        case IBUS_Down:
        case IBUS_KP_Down:
            cursorDown ();
            return TRUE;

        case IBUS_Page_Up:
        case IBUS_KP_Page_Up:
            pageUp ();
            return TRUE;

        case IBUS_Page_Down:
        case IBUS_KP_Page_Down:
            pageDown ();
            return TRUE;

        default:
            return TRUE;
        }
    }

    return TRUE;
}

};
