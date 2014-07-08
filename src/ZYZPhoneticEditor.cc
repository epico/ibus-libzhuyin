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
#include "ZYZBopomofoSymbolSection.h"
#include "ZYEnhancedText.h"
#include "ZYLibZhuyin.h"
#include "ZYSymbols.h"


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
    /* alloc one instance. */
    m_instance = LibZhuyinBackEnd::instance ().allocZhuyinInstance ();
    assert (NULL != m_instance);

    /* init symbols sections here. */
    m_symbol_sections[STATE_BUILTIN_SYMBOL_SHOWN].reset
        (new BuiltinSymbolSection (*this, props));

    m_symbol_sections[STATE_BOPOMOFO_SYMBOL_SHOWN].reset
        (new BopomofoSymbolSection (*this, props));

    m_phonetic_section.reset
        (new PhoneticSection (*this, props));
}

PhoneticEditor::~PhoneticEditor (void)
{
    m_text = "";
    m_cursor = 0;

    /* free m_instances */
    resizeInstances ();

    LibZhuyinBackEnd::instance ().freeZhuyinInstance (m_instance);
}

gboolean
PhoneticEditor::processEnter (guint keyval, guint keycode,
                              guint modifiers)
{
    if (IBUS_Return != keyval && IBUS_KP_Enter != keyval)
        return FALSE;

    if (cmshm_filter (modifiers) != 0)
        return TRUE;

    if (!m_text) {
        Text text ("\n");
        commitText (text);
        return TRUE;
    }

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
            return FALSE;
        }
    }

    return TRUE;
}

gboolean
PhoneticEditor::processShowCandidateKey (guint keyval, guint keycode,
                                         guint modifiers)
{
    if (m_text.empty ())
        return FALSE;

    /* ignore numlock */
    modifiers = cmshm_filter (modifiers);

    if (modifiers != 0)
        return TRUE;

    /* process some show candidate keys */
    if (modifiers == 0) { /* no modifiers. */
        switch (keyval) {
        case IBUS_Down:
        case IBUS_KP_Down:
            /* check phonetic or symbol section here */
            prepareCandidates ();
            break;

        case IBUS_Up:
        case IBUS_KP_Up:
            m_lookup_table.clear ();
            m_input_state = STATE_INPUT;
            break;

        default:
            return FALSE;
        }
    }

    update ();
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
            break;
        }

        /* process candidate keys */
        std::string keys = m_config.candidateKeys ();
        std::size_t found = keys.find (keyval);
        if (found != std::string::npos) { /* found. */
            selectCandidateInPage (found);
            m_input_state = STATE_INPUT;
            return TRUE;
        }
    }

    return FALSE;
}

gboolean
PhoneticEditor::processKeyEvent (guint keyval, guint keycode,
                                 guint modifiers)
{
    return FALSE;
}

void
PhoneticEditor::updateLookupTableFast (void)
{
    Editor::updateLookupTableFast (m_lookup_table, TRUE);
}

void
PhoneticEditor::updateLookupTable (void)
{
    m_lookup_table.clear ();

    fillLookupTableByPage ();
    if (m_lookup_table.size ()) {
        Editor::updateLookupTable (m_lookup_table, TRUE);
    } else {
        hideLookupTable ();
    }
}

gboolean
PhoneticEditor::fillLookupTableByPage (void)
{
    if (STATE_CANDIDATE_SHOWN == m_input_state)
        return m_phonetic_section->fillLookupTableByPage ();

    if (STATE_BUILTIN_SYMBOL_SHOWN == m_input_state ||
        STATE_BOPOMOFO_SYMBOL_SHOWN == m_input_state /* ||
        STATE_USER_SYMBOL_LIST_ALL == m_input_state ||
        STATE_USER_SYMBOL_SHOWN == m_input_state */) {
        return m_symbol_sections[m_input_state]->
            fillLookupTableByPage ();
    }

    return FALSE;
}

void
PhoneticEditor::pageUp (void)
{
    if (G_LIKELY (m_lookup_table.pageUp ())) {
        updateLookupTableFast ();
        updatePreeditText ();
        updateAuxiliaryText ();
    }
}

void
PhoneticEditor::pageDown (void)
{
    if (G_LIKELY ((m_lookup_table.pageDown ()) ||
                  (fillLookupTableByPage () && m_lookup_table.pageDown ()))) {
        updateLookupTableFast ();
        updatePreeditText ();
        updateAuxiliaryText ();
    }
}

void
PhoneticEditor::cursorUp (void)
{
    if (G_LIKELY (m_lookup_table.cursorUp ())) {
        updateLookupTableFast ();
        updatePreeditText ();
        updateAuxiliaryText ();
    }
}

void
PhoneticEditor::cursorDown (void)
{
    if (G_LIKELY ((m_lookup_table.cursorPos () == m_lookup_table.size() - 1) &&
                  (fillLookupTableByPage () == FALSE))) {
        return;
    }

    if (G_LIKELY (m_lookup_table.cursorDown ())) {
        updateLookupTableFast ();
        updatePreeditText ();
        updateAuxiliaryText ();
    }
}

void
PhoneticEditor::candidateClicked (guint index, guint button,
                                  guint state)
{
    selectCandidateInPage (index);
}

void
PhoneticEditor::reset (void)
{
    m_input_state = STATE_INPUT;

    m_lookup_table.clear ();
    m_buffer = "";

    zhuyin_reset (m_instance);

    m_text = "";
    resizeInstances ();

    EnhancedEditor::reset ();
}

void
PhoneticEditor::update (void)
{
    updateLookupTable ();
    updatePreeditText ();
    updateAuxiliaryText ();
}

void
PhoneticEditor::commit (const gchar *str)
{
    StaticText text(str);
    commitText (text);
}

gboolean
PhoneticEditor::selectCandidate (guint index)
{
    if (STATE_CANDIDATE_SHOWN == m_input_state) {
        int retval = m_phonetic_section->selectCandidate (index);

        m_input_state = STATE_INPUT;

        updateZhuyin ();
        update ();
        return retval;
    }

    if (STATE_BUILTIN_SYMBOL_SHOWN == m_input_state ||
        STATE_BOPOMOFO_SYMBOL_SHOWN == m_input_state /* ||
        STATE_USER_SYMBOL_LIST_ALL == m_input_state ||
        STATE_USER_SYMBOL_SHOWN == m_input_state */) {
        SymbolSectionPtr symbols = m_symbol_sections[m_input_state];

        int retval = symbols->selectCandidate (index);

        erase_input_sequence (m_text, m_cursor, 1);
        insert_symbol (m_text, m_cursor, symbols->m_type,
                       symbols->m_lookup, symbols->m_choice);

        m_input_state = STATE_INPUT;

        update ();
        return retval;
    }

    return FALSE;
}

gboolean
PhoneticEditor::selectCandidateInPage (guint index)
{
    guint page_size = m_lookup_table.pageSize ();
    guint cursor_pos = m_lookup_table.cursorPos ();

    if (G_UNLIKELY (index >= page_size))
        return FALSE;
    index += (cursor_pos / page_size) * page_size;

    return selectCandidate (index);
}

gboolean
PhoneticEditor::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor --;
    erase_input_sequence (m_text, m_cursor, 1);

    updateZhuyin ();
    update ();

    return TRUE;
}

gboolean
PhoneticEditor::removeCharAfter (void)
{
    if (G_UNLIKELY (m_cursor ==
                    get_enhanced_text_length (m_text)))
        return FALSE;

    erase_input_sequence (m_text, m_cursor, 1);

    updateZhuyin ();
    update ();

    return TRUE;
}

gboolean
PhoneticEditor::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor --;
    update ();
    return TRUE;
}

gboolean
PhoneticEditor::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor ==
                    get_enhanced_text_length (m_text)))
        return FALSE;

    m_cursor ++;
    update ();
    return TRUE;
}

gboolean
PhoneticEditor::moveCursorToBegin (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor = 0;
    update ();
    return TRUE;
}

gboolean
PhoneticEditor::moveCursorToEnd (void)
{
    if (G_UNLIKELY (m_cursor ==
                    get_enhanced_text_length (m_text)))
        return FALSE;

    m_cursor = get_enhanced_text_length (m_text);
    update ();
    return TRUE;
}

void
PhoneticEditor::resizeInstances (void)
{
    size_t num = get_number_of_phonetic_sections (m_text);

    /* re-allocate the zhuyin instances */
    if (num > m_instances.size ()) { /* need more instances. */
        for (size_t i = m_instances.size (); i < num; ++i) {
            /* allocate one instance */
            zhuyin_instance_t * instance = LibZhuyinBackEnd::instance ().
                allocZhuyinInstance ();
            assert (NULL != instance);
            m_instances.push_back (instance);
        }
    }

    if (num < m_instances.size ()) { /* free some instances. */
        for (size_t i = num; i < m_instances.size (); ++i) {
            LibZhuyinBackEnd::instance ().freeZhuyinInstance (m_instances[i]);
            m_instances[i] = NULL;
        }
        m_instances.resize (num);
    }
}

guint
PhoneticEditor::getZhuyinCursor (void)
{
    /* decrement the cursor variable to calculate the zhuyin cursor. */
    guint cursor = m_cursor;
    guint zhuyin_cursor = 0;

    const String & enhanced_text = m_text;

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    while (end_pos != enhanced_text.size ()) {
        if (0 == cursor)
            break;

        section_t type = probe_section_quick (enhanced_text, start_pos);

        if (PHONETIC_SECTION == type) {
            String section;
            get_phonetic_section (enhanced_text, start_pos, end_pos, section);
            size_t section_len = end_pos - start_pos;

            zhuyin_instance_t * instance = m_instances[index];
            size_t parsed_len = zhuyin_get_parsed_input_length (instance);

            assert (parsed_len <= section_len);

            if (cursor >= parsed_len) {
                cursor -= parsed_len;
                guint len = 0;
                zhuyin_get_n_zhuyin (instance, &len);
                zhuyin_cursor += len;
            } else {
                guint len = 0;
                zhuyin_get_n_zhuyin (instance, &len);

                guint inner_cursor = len;

                guint16 prev_end = 0, cur_end;
                for (size_t i = 0; i < len; ++i) {
                    ChewingKeyRest *pos = NULL;
                    zhuyin_get_zhuyin_key_rest (instance, i, &pos);
                    zhuyin_get_zhuyin_key_rest_positions
                        (instance, pos, NULL, &cur_end);

                    if (prev_end < cursor && cursor < cur_end)
                        inner_cursor = i;

                    prev_end = cur_end;
                }

                assert (inner_cursor >= 0);
                zhuyin_cursor += inner_cursor;
                cursor = 0;
            }

            if (0 == cursor)
                break;

            if (cursor >= section_len - parsed_len) {
                /* except for DaChen26 scheme here. */
                cursor -= section_len - parsed_len;
                zhuyin_cursor += section_len - parsed_len;
            } else {
                zhuyin_cursor += cursor;
                cursor = 0;
            }

            ++ index;
        }

        if (SYMBOL_SECTION == type) {
            String type, lookup, choice;
            get_symbol_section (enhanced_text, start_pos, end_pos,
                                type, lookup, choice);
            --cursor;
            zhuyin_cursor += g_utf8_strlen (choice, -1);
        }

        start_pos = end_pos;
    }

    return zhuyin_cursor;
}

gboolean
PhoneticEditor::insertPunct (gint ch)
{
    /* for punctuations. */
    if (is_half_punct (ch)) {
        if (m_props.modeFullPunct ()) {
            String choice;
            assert (half_punct_to_full_punct (ch, choice));

            String lookup;
            int ch = find_lookup_key (choice);
            if (ch != 0)
                lookup = ch;

            insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                           lookup, choice);
        } else {
            String choice = ch;
            insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                           "", choice);
        }

        return TRUE;
    }

    return FALSE;
}

gboolean
PhoneticEditor::insertEnglish (gint ch)
{
    /* for English. */
    if (is_half_english (ch)) {
        if (m_props.modeFullEnglish ()) {
            String choice;
            assert (half_english_to_full_english (ch, choice));

            String lookup;
            int ch = find_lookup_key (choice);
            if (ch != 0)
                lookup = ch;

            insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                           lookup, choice);
        } else {
            String choice = ch;
            insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                           "", choice);
        }

        return TRUE;
    }

    return FALSE;
}

gboolean
PhoneticEditor::prepareCandidates (void)
{
    /* decrement the cursor variable to calculate the zhuyin cursor. */
    guint cursor = m_cursor;

    const String & enhanced_text = m_text;

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    while (end_pos != enhanced_text.size ()) {
        if (0 == cursor)
            break;

        start_pos = end_pos;
        section_t type = probe_section_quick (enhanced_text, start_pos);

        if (PHONETIC_SECTION == type) {
            String section;
            get_phonetic_section (enhanced_text, start_pos, end_pos, section);

            size_t section_len = end_pos - start_pos;

            if (cursor < section_len)
                break;

            cursor -= section_len;
            ++index;
        }

        if (SYMBOL_SECTION == type) {
            String type, lookup, choice;
            get_symbol_section (enhanced_text, start_pos, end_pos,
                                type, lookup, choice);
            --cursor;
        }
    }

    /* deal with candidates */
    if (m_cursor < get_enhanced_text_length (enhanced_text)) {
        section_t type = probe_section_quick (enhanced_text, start_pos);

        if (PHONETIC_SECTION == type) {
            String section;
            get_phonetic_section (enhanced_text, start_pos, end_pos, section);
            size_t section_len = end_pos - start_pos;

            zhuyin_instance_t * instance = m_instances[index];
            size_t parsed_len = zhuyin_get_parsed_input_length (instance);

            assert (cursor < section_len);
            assert (parsed_len <= section_len);

            if (cursor >= parsed_len) {
                String lookup;
                lookup += section[cursor];
                m_input_state = STATE_BOPOMOFO_SYMBOL_SHOWN;
                m_symbol_sections[m_input_state]->initCandidates
                    (m_instance, lookup);

                update ();
                return TRUE;
            } else {
                guint len = 0;
                zhuyin_get_n_zhuyin (instance, &len);

                guint inner_cursor = len;

                guint16 prev_end = 0, cur_end;
                for (size_t i = 0; i < len; ++i) {
                    ChewingKeyRest *pos = NULL;
                    zhuyin_get_zhuyin_key_rest (instance, i, &pos);
                    zhuyin_get_zhuyin_key_rest_positions
                        (instance, pos, NULL, &cur_end);

                    if (prev_end < cursor && cursor < cur_end)
                        inner_cursor = i;

                    prev_end = cur_end;
                }

                assert (inner_cursor >= 0);
                m_input_state = STATE_CANDIDATE_SHOWN;
                m_phonetic_section->initCandidates (instance, inner_cursor);

                update ();
                return TRUE;
            }
        }

        if (SYMBOL_SECTION == type) {
            String type, lookup, choice;
            get_symbol_section (enhanced_text, start_pos, end_pos,
                                type, lookup, choice);

            if (BUILTIN_SYMBOL_TYPE == type) {
                m_input_state = STATE_BUILTIN_SYMBOL_SHOWN;
                bool retval = m_symbol_sections[m_input_state]->initCandidates
                    (m_instance, lookup);

                if (!retval) {
                    m_input_state = STATE_INPUT;
                    return FALSE;
                }
            } else if (BOPOMOFO_SYMBOL_TYPE == type) {
                m_input_state = STATE_BOPOMOFO_SYMBOL_SHOWN;
                bool retval = m_symbol_sections[m_input_state]->initCandidates
                    (m_instance, lookup);

                if (!retval) {
                    m_input_state = STATE_INPUT;
                    return FALSE;
                }
            } else
                assert (FALSE);

            update ();
            return TRUE;
        }
    }
}


};
