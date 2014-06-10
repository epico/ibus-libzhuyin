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
#include <assert.h>
#include <string>
#include "ZYConfig.h"
#include "ZYZConfig.h"
#include "ZYRawEditor.h"
#include "ZYZZhuyinEditor.h"
#include "ZYZPinyinEditor.h"


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
        m_editors[MODE_INIT].reset
            (new PinyinEditor (m_props, ZhuyinConfig::instance ()));
        break;
    default:
        assert (FALSE);
    }

    m_editors[MODE_RAW].reset
        (new RawEditor (m_props, ZhuyinConfig::instance ()));

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
            default:
                assert (FALSE);
            }

            return TRUE;
        }
    }

    retval = m_editors[m_input_mode]->processKeyEvent (keyval, keycode, modifiers);

    /* store ignored key event by editors */
    m_prev_pressed_key = retval ? IBUS_VoidSymbol : keyval;

    return retval;
}

void
ZhuyinEngine::focusIn (void)
{
    /* TODO: check memory leak here.*/
    const ZhuyinScheme scheme = ZhuyinConfig::instance ().keyboardLayout ();
    if (scheme != m_zhuyin_scheme) {
        switch (scheme) {
        case CHEWING_STANDARD ... CHEWING_DACHEN_CP26:
            m_editors[MODE_INIT].reset
                (new ZhuyinEditor (m_props, ZhuyinConfig::instance ()));
            connectEditorSignals (m_editors[MODE_INIT]);
            break;
        case FULL_PINYIN_HANYU ... FULL_PINYIN_SECONDARY_BOPOMOFO:
            m_editors[MODE_INIT].reset
                (new PinyinEditor (m_props, ZhuyinConfig::instance ()));
            connectEditorSignals (m_editors[MODE_INIT]);
            break;
        default:
            assert (FALSE);
        }
        m_zhuyin_scheme = scheme;
    }

    registerProperties (m_props.properties ());
}

void
ZhuyinEngine::focusOut (void)
{
    Engine::focusOut ();

    reset ();
}

void
ZhuyinEngine::reset (void)
{
    m_prev_pressed_key = IBUS_VoidSymbol;
    m_input_mode = MODE_INIT;
    for (gint i = 0; i < MODE_LAST; i++) {
        m_editors[i]->reset ();
    }
}

void
ZhuyinEngine::enable (void)
{
    m_props.reset ();
}

void
ZhuyinEngine::disable (void)
{
}

void
ZhuyinEngine::pageUp (void)
{
    m_editors[m_input_mode]->pageUp ();
}

void
ZhuyinEngine::pageDown (void)
{
    m_editors[m_input_mode]->pageDown ();
}

void
ZhuyinEngine::cursorUp (void)
{
    m_editors[m_input_mode]->cursorUp ();
}

void
ZhuyinEngine::cursorDown (void)
{
    m_editors[m_input_mode]->cursorDown ();
}

inline void
ZhuyinEngine::showSetupDialog (void)
{
    g_spawn_command_line_async
        (LIBEXECDIR"/ibus-setup-libzhuyin zhuyin", NULL);
}

gboolean
ZhuyinEngine::propertyActivate (const char *prop_name, guint prop_state)
{
    const static String setup ("setup");
    if (m_props.propertyActivate (prop_name, prop_state)) {
        return TRUE;
    }
    else if (setup == prop_name) {
        showSetupDialog ();
        return TRUE;
    }
    return FALSE;
}

void
ZhuyinEngine::candidateClicked (guint index, guint button, guint state)
{
    m_editors[m_input_mode]->candidateClicked (index, button, state);
}

void
ZhuyinEngine::commitText (Text & text)
{
    Engine::commitText (text);
}

void
ZhuyinEngine::connectEditorSignals (EditorPtr editor)
{
    editor->signalCommitText ().connect (
        std::bind (&ZhuyinEngine::commitText, this, _1));

    editor->signalUpdatePreeditText ().connect (
        std::bind (&ZhuyinEngine::updatePreeditText, this, _1, _2, _3));
    editor->signalShowPreeditText ().connect (
        std::bind (&ZhuyinEngine::showPreeditText, this));
    editor->signalHidePreeditText ().connect (
        std::bind (&ZhuyinEngine::hidePreeditText, this));

    editor->signalUpdateAuxiliaryText ().connect (
        std::bind (&ZhuyinEngine::updateAuxiliaryText, this, _1, _2));
    editor->signalShowAuxiliaryText ().connect (
        std::bind (&ZhuyinEngine::showAuxiliaryText, this));
    editor->signalHideAuxiliaryText ().connect (
        std::bind (&ZhuyinEngine::hideAuxiliaryText, this));

    editor->signalUpdateLookupTable ().connect (
        std::bind (&ZhuyinEngine::updateLookupTable, this, _1, _2));
    editor->signalUpdateLookupTableFast ().connect (
        std::bind (&ZhuyinEngine::updateLookupTableFast, this, _1, _2));
    editor->signalShowLookupTable ().connect (
        std::bind (&ZhuyinEngine::showLookupTable, this));
    editor->signalHideLookupTable ().connect (
        std::bind (&ZhuyinEngine::hideLookupTable, this));
}
