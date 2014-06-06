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

#include "ZYZZhuyinEngine.h"
#include <string>
#include "ZYConfig.h"
#include "ZYZConfig.h"
#include "ZYRawEditor.h"
#include "ZYZZhuyinEditor.h"


using namespace ZY;

/* constructor */
ZhuyinEngine::ZhuyinEngine (IBusEngine *engine)
    : Engine (engine),
      m_props (ZhuyinConfig::instance ()),
      m_prev_pressed_key (IBUS_VoidSymbol),
      m_input_mode (MODE_INIT)
{
    gint i;

    m_zhuyin_scheme = ZhuyinConfig::instance ().keyboardLayout ();

    switch (m_zhuyin_scheme) {
    case CHEWING_STANDARD ... CHEWING_DACHEN_CP26:
        m_editors[MODE_INIT].reset
            (new ZhuyinEditor (m_props, ZhuyinConfig::instance ()));
        break;
    case FULL_PINYIN_HANYU ... FULL_PINYIN_SECONDARY_BOPOMOFO:
        assert (FALSE);
#if 0
        m_editors[MODE_INIT].reset
            (new PinyinEditor (m_props, ZhuyinConfig::instance ()));
#endif
        break;
    default:
        assert (FALSE);
    }

    m_editors[MODE_RAW].reset
        (new RawEditor (m_props, PinyinConfig::instance ()));

    m_props.signalUpdateProperty ().connect
        (std::bind (&ZhuyinEngine::updateProperty, this, _1));

    for (i = MODE_INIT; i < MODE_LAST; i ++) {
        connectEditorSignals (m_editors[i]);
    }
}

/* destructor */
ZhuyinEngine::~ZhuyinEngine (void)
{
}

gboolean
ZhuyinEngine::processKeyEvent (guint keyval, guint keycode, guint modifiers)
{
    gboolean retval = FALSE;

    if (contentIsPassword ())
        return retval;

    /* check Shift or Ctrl + Release hotkey,
     * and then ignore other Release key event */
    if (modifiers & IBUS_RELEASE_MASK) {
        /* press and release keyval are same,
         * and no other key event between the press and release key event */
        gboolean triggered = FALSE;

        if (m_prev_pressed_key == keyval) {
            if (keyval == IBUS_Shift_L || keyval == IBUS_Shift_R)
                triggered = TRUE;
        }

        if (triggered && m_editors[m_input_mode]->text ().empty ()) {
            /* switch input editor. */
            m_props.toggleModeChinese ();

            switch (m_input_mode) {
            case MODE_INIT:
                m_input_mode = MODE_RAW;
                break;
            case MODE_RAW:
                m_input_mode = MODE_INIT;
                break;
            }

            return TRUE;
        }
    }

    retval = m_editors[m_input_mode]->processKeyEvent (keyval, keycode, modifiers);

    /* store ignored key event by editors */
    m_prev_pressed_key = retval ? IBUS_VoidSymbol : keyval;

    return retval;
}
