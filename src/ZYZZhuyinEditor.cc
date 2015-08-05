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

#include "ZYZZhuyinEditor.h"
#include <assert.h>
#include "ZYConfig.h"
#include "ZYLibZhuyin.h"
#include "ZYZhuyinProperties.h"
#include "ZYTradSimpConverter.h"
#include "ZYEnhancedText.h"
#include "ZYSymbols.h"


using namespace ZY;

ZhuyinEditor::ZhuyinEditor (ZhuyinProperties & props, Config & config)
    : PhoneticEditor (props, config)
{
    m_instance = LibZhuyinBackEnd::instance ().allocZhuyinInstance ();
}

ZhuyinEditor::~ZhuyinEditor (void)
{
    reset ();

    LibZhuyinBackEnd::instance ().freeZhuyinInstance (m_instance);
    m_instance = NULL;
}

void
ZhuyinEditor::commit (void)
{
    if (G_UNLIKELY (m_preedit_text.empty ()))
        return;

    for (size_t i = 0; i < m_instances.size (); ++i) {
        zhuyin_train (m_instances[i]);
    }
    LibZhuyinBackEnd::instance ().modified ();
    PhoneticEditor::commit (m_preedit_text.c_str ());
    reset ();
}

void
ZhuyinEditor::reset (void)
{
    m_preedit_text = "";

    PhoneticEditor::reset ();
}

void
ZhuyinEditor::updateZhuyin (void)
{
    static const char * tones[] = {" ", "ˊ", "ˇ", "ˋ", "˙", NULL};

    const String & enhanced_text = m_text;
    String new_text;
    size_t append_offset = 0;

    resizeInstances ();

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    while (end_pos != enhanced_text.size ()) {
        section_t type = probe_section_quick (enhanced_text, start_pos);

        if (PHONETIC_SECTION == type) {
            String section;
            get_phonetic_section (enhanced_text, start_pos, end_pos, section);

            zhuyin_instance_t * instance = m_instances[index];
            size_t len = zhuyin_parse_more_chewings
                (instance, section.c_str ());
            bool valid = zhuyin_valid_zhuyin_keys (instance);
            if (!valid) {
                size_t valid_len = zhuyin_get_parsed_input_length (instance);

                const gchar * str = NULL;
                zhuyin_get_raw_user_input (instance, &str);
                section = str;

                /* hack here: usually only one zhuyin key will have problem,
                   so it is safe to just move the cursor. */
                m_cursor -= len - valid_len;
            }
            zhuyin_guess_sentence (instance);

            new_text += section;
            append_offset += section.length ();

            /* check whether the last character is tone,
               if not part of parsed chewing input,
               turn the tone into symbol. */
            if (end_pos == m_text.size ()) {

                /* check tone symbol. */
                const char tone = section[section.size () - 1];
                gchar ** symbols = NULL;
                zhuyin_in_chewing_keyboard (instance, tone, &symbols);

                gboolean is_tone = FALSE;
                /* TODO: use g_strv_contains in future. */
                if (1 == g_strv_length (symbols)) {
                    for (const char ** strs = tones; *strs != NULL; ++strs) {
                        if (g_str_equal (*strs, symbols[0])) {
                            is_tone = TRUE;
                            break;
                        }
                    }
                }

                if (is_tone && section.size () > len) {
                    erase_input_sequence (new_text, append_offset - 1, 1);
                    insert_symbol (new_text, append_offset - 1,
                                   BUILTIN_SYMBOL_TYPE,
                                   "", symbols[0]);
                    /* as we changed the last space character,
                       reached the end of user input, exit the loop. */
                    g_strfreev (symbols);
                    break;
                }
                g_strfreev (symbols);
            }

            ++index;
        }

        if (SYMBOL_SECTION == type) {
            String type, lookup, choice;
            get_symbol_section (enhanced_text, start_pos, end_pos,
                                type, lookup, choice);

            insert_symbol (new_text, append_offset, type, lookup, choice);
            append_offset ++;
        }

        start_pos = end_pos;
    }

    m_text = new_text;
    return;
}

void
ZhuyinEditor::updateAuxiliaryText (void)
{
    /* libchewing doesn't use the auxiliary text, always hide. */
    return;
}

void
ZhuyinEditor::updatePreeditText (void)
{
    if (G_UNLIKELY (m_text.empty ())) {
        hidePreeditText ();
        return;
    }

    const String & enhanced_text = m_text;
    m_preedit_text.clear ();

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    while (end_pos != enhanced_text.size ()) {
        section_t type = probe_section_quick (enhanced_text, start_pos);

        if (PHONETIC_SECTION == type) {
            String section;
            get_phonetic_section (enhanced_text, start_pos, end_pos, section);
            zhuyin_instance_t * instance = m_instances[index];

            char * sentence = NULL;
            zhuyin_get_sentence (instance, &sentence);
            if (sentence)
                m_preedit_text += sentence;
            g_free (sentence);

            size_t len = zhuyin_get_parsed_input_length (instance);
            for (size_t i = len; i < section.size (); ++i) {
                char sym = section[i];
                gchar ** symbols = NULL;
                /* append bopomofo symbol except for DaChen26. */
                assert (zhuyin_in_chewing_keyboard (m_instance, sym, &symbols));
                assert (NULL != symbols[0]);
                m_preedit_text += symbols[0];
                g_strfreev (symbols);
            }

            ++index;
        }

        if (SYMBOL_SECTION == type) {
            String type, lookup, choice;
            get_symbol_section (enhanced_text, start_pos, end_pos,
                                type, lookup, choice);
            m_preedit_text += choice;
        }

        start_pos = end_pos;
    }

    m_buffer.clear ();
    if (m_props.modeTrad ()) {
        m_buffer = m_preedit_text;
    } else {
        TradSimpConverter::tradToSimp (m_preedit_text.c_str (), m_buffer);
    }
    m_preedit_text = m_buffer;

    StaticText preedit_text (m_preedit_text);
    /* underline */
    preedit_text.appendAttribute (IBUS_ATTR_TYPE_UNDERLINE, IBUS_ATTR_UNDERLINE_SINGLE, 0, -1);

    /* calcuate the cursor position. */
    size_t cursor = getZhuyinCursor ();
    Editor::updatePreeditText (preedit_text, cursor, TRUE);
    return;
}

gboolean
ZhuyinEditor::insert (guint keyval, guint keycode, guint modifiers)
{
    /* let client applications to handle shortcut key event */
    modifiers = cmshm_filter (modifiers);

    if (modifiers != 0 && m_text.empty ())
        return FALSE;

    gchar ** symbols = NULL;
    if (zhuyin_in_chewing_keyboard (m_instance, keyval, &symbols)) {
        g_strfreev (symbols);

        insert_phonetic (m_text, m_cursor++, keyval);

        updateZhuyin ();
        update ();
        return TRUE;
    }

    if (insertPunct (keyval)) {
        updateZhuyin ();
        update ();
        return TRUE;
    }

    if (insertEnglish (keyval)) {
        updateZhuyin ();
        update ();
        return TRUE;
    }

    if (insertNumbers (keyval)) {
        updateZhuyin ();
        update ();
        return TRUE;
    }

    return FALSE;
}

gboolean
ZhuyinEditor::processKeyEvent (guint keyval, guint keycode, guint modifiers)
{
    modifiers &= (IBUS_SHIFT_MASK |
                  IBUS_CONTROL_MASK |
                  IBUS_MOD1_MASK |
                  IBUS_SUPER_MASK |
                  IBUS_HYPER_MASK |
                  IBUS_META_MASK |
                  IBUS_LOCK_MASK);

    if (processEscape (keyval, keycode, modifiers))
        return TRUE;

    if (processSpace (keyval, keycode, modifiers))
        return TRUE;

    if (STATE_INPUT == m_input_state) {
        if (processEasySymbolKey (keyval, keycode, modifiers))
            return TRUE;

        if (processUserSymbolKey (keyval, keycode, modifiers))
            return TRUE;

        if (insert (keyval, keycode, modifiers))
            return TRUE;

        if (processCommit (keyval, keycode, modifiers))
            return TRUE;

        if (processFunctionKey (keyval, keycode, modifiers))
            return TRUE;

        if (processShowCandidateKey (keyval, keycode, modifiers))
            return TRUE;
    }

    if (STATE_CANDIDATE_SHOWN == m_input_state ||
        STATE_BUILTIN_SYMBOL_SHOWN == m_input_state ||
        STATE_BOPOMOFO_SYMBOL_SHOWN == m_input_state ||
        STATE_USER_SYMBOL_LIST_ALL == m_input_state ||
        STATE_USER_SYMBOL_SHOWN == m_input_state) {

        if (processSelectCandidate (keyval, keycode, modifiers))
            return TRUE;

        if (processCandidateKey (keyval, keycode, modifiers))
            return TRUE;
    }

    return FALSE;
}
