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
#include "ZYZUserSymbolListAllSection.h"
#include "ZYZUserSymbolShownSection.h"


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

    m_symbol_sections[STATE_USER_SYMBOL_LIST_ALL].reset
        (new UserSymbolListAllSection (*this, props));

    m_symbol_sections[STATE_USER_SYMBOL_SHOWN].reset
        (new UserSymbolShownSection (*this, props));

    m_phonetic_section.reset
        (new PhoneticSection (*this, props));

    /* load easy symbols: easysymbol.txt */
    gchar * path = g_build_filename (g_get_user_config_dir (),
                                     "ibus", "libzhuyin",
                                     "easysymbol.txt", NULL);
    loadEasySymbolFile (".." G_DIR_SEPARATOR_S "data" G_DIR_SEPARATOR_S
                        "easysymbol.txt") ||
        loadEasySymbolFile (path) ||
        loadEasySymbolFile (PKGDATADIR G_DIR_SEPARATOR_S "easysymbol.txt");
    g_free(path);

    m_moved_left = FALSE;
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
PhoneticEditor::loadEasySymbolFile (const gchar * filename)
{
    gboolean retval = m_easy_symbols.loadFromFile (filename);
    return retval;
}

gboolean
PhoneticEditor::processEscape (guint keyval, guint keycode,
                               guint modifiers)
{
    if (IBUS_Escape != keyval)
        return FALSE;

    if (cmshm_filter (modifiers) != 0)
        return TRUE;

    if (STATE_INPUT == m_input_state) {
        reset ();
        update ();
        return TRUE;
    }

    if (STATE_CANDIDATE_SHOWN == m_input_state ||
        STATE_BUILTIN_SYMBOL_SHOWN == m_input_state ||
        STATE_BOPOMOFO_SYMBOL_SHOWN == m_input_state ||
        STATE_USER_SYMBOL_LIST_ALL == m_input_state ||
        STATE_USER_SYMBOL_SHOWN == m_input_state) {

        m_input_state = STATE_INPUT;

        if (m_moved_left) {
            moveCursorRight ();
            m_moved_left = FALSE;
        }

        update ();
        return TRUE;
    }

    return FALSE;
}

gboolean
PhoneticEditor::processSpace (guint keyval, guint keycode,
                              guint modifiers)
{
    if (IBUS_space != keyval && IBUS_KP_Space != keyval)
        return FALSE;

    if (STATE_INPUT == m_input_state) {
        if (cmshm_filter (modifiers) != 0)
            return FALSE;

        if (m_text.empty ()) {
            assert (is_special_symbol (' '));
            if (m_props.modeFullWidth ()) {
                String symbol;
                convert_special_symbol (keyval, symbol);
                commit (symbol);
            } else {
                String symbol = ' ';
                commit (symbol);
            }
            return TRUE;
        }

        if (!m_config.spaceShowCandidates ()) {
            return insert (keyval, keycode, modifiers);
        }
    }

    if (STATE_CANDIDATE_SHOWN == m_input_state ||
        STATE_BUILTIN_SYMBOL_SHOWN == m_input_state ||
        STATE_BOPOMOFO_SYMBOL_SHOWN == m_input_state ||
        STATE_USER_SYMBOL_LIST_ALL == m_input_state ||
        STATE_USER_SYMBOL_SHOWN == m_input_state) {
        if (cmshm_filter (modifiers) != 0)
            return TRUE;

        selectCandidate (m_lookup_table.cursorPos ());

        updateZhuyin ();
        update ();
        return TRUE;
    }

    return FALSE;
}

gboolean
PhoneticEditor::processCommit (guint keyval, guint keycode,
                               guint modifiers)
{
    if (IBUS_Return != keyval && IBUS_KP_Enter != keyval)
        return FALSE;

    if (cmshm_filter (modifiers) != 0)
        return TRUE;

    if (!m_text)
        return FALSE;

    commit ();
    return TRUE;
}

gboolean
PhoneticEditor::processSelectCandidate (guint keyval, guint keycode,
                                        guint modifiers)
{

    switch (keyval) {
    case IBUS_Return:
    case IBUS_KP_Enter:
        break;

    default:
        return FALSE;
    }

    if (cmshm_filter (modifiers) != 0)
        return TRUE;

    selectCandidate (m_lookup_table.cursorPos ());

    updateZhuyin ();
    update ();
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
            if (m_config.candidatesAfterCursor () &&
                m_cursor == get_enhanced_text_length (m_text)) {
                moveCursorLeft ();
                m_moved_left = TRUE;
            }

            /* check phonetic or symbol section here */
            prepareCandidates ();
            break;

        case IBUS_Up:
        case IBUS_KP_Up:
            m_lookup_table.clear ();
            m_input_state = STATE_INPUT;

            if (m_moved_left) {
                moveCursorRight ();
                m_moved_left = FALSE;
            }

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
            return TRUE;
        }
    }

    return FALSE;
}

gboolean
PhoneticEditor::processEasySymbolKey (guint keyval, guint keycode,
                                  guint modifiers)
{
    if (!m_config.easySymbol ())
        return FALSE;

    if (! ('A' <= keyval && keyval <= 'Z'))
        return FALSE;

    String index = (gchar) keyval;
    String symbol = m_easy_symbols.find (index);

    if ("" == symbol)
        return FALSE;

    String lookup;
    int ch = find_lookup_key (symbol);
    if (ch != 0)
        lookup = (gchar) ch;

    insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                   lookup, symbol);

    update ();
    return TRUE;
}

gboolean
PhoneticEditor::processUserSymbolKey (guint keyval, guint keycode,
                                      guint modifiers)
{
    if (!m_config.userSymbol ())
        return FALSE;

    if ('`' != keyval)
        return FALSE;

    m_input_state = STATE_USER_SYMBOL_LIST_ALL;
    m_symbol_sections[m_input_state]->initCandidates
        (m_instance, "`");

    update ();
    return TRUE;
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
    updateLookupTableLabel ();
    if (m_lookup_table.size ()) {
        Editor::updateLookupTable (m_lookup_table, TRUE);
    } else {
        hideLookupTable ();
    }
}

void
PhoneticEditor::updateLookupTableLabel (void)
{
    String labels = m_config.candidateKeys ();

    size_t len = MIN (labels.length (), m_config.pageSize ());
    for (size_t i = 0; i < len; ++i) {
        String label = (gchar) labels[i];
        Text text (label);
        m_lookup_table.setLabel (i, text);
    }
}

gboolean
PhoneticEditor::fillLookupTableByPage (void)
{
    if (STATE_CANDIDATE_SHOWN == m_input_state)
        return m_phonetic_section->fillLookupTableByPage ();

    if (STATE_BUILTIN_SYMBOL_SHOWN == m_input_state ||
        STATE_BOPOMOFO_SYMBOL_SHOWN == m_input_state ||
        STATE_USER_SYMBOL_LIST_ALL == m_input_state ||
        STATE_USER_SYMBOL_SHOWN == m_input_state) {
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
        int offset = m_phonetic_section->selectCandidate (index);

#if 0
        if (0 == offset)
            return FALSE;
#endif

        m_cursor += offset;
        m_input_state = STATE_INPUT;

        updateZhuyin ();
        update ();
        return TRUE;
    }

    if (STATE_BUILTIN_SYMBOL_SHOWN == m_input_state ||
        STATE_BOPOMOFO_SYMBOL_SHOWN == m_input_state) {
        SymbolSectionPtr symbols = m_symbol_sections[m_input_state];

        int offset = symbols->selectCandidate (index);

        if (0 == offset)
            return FALSE;

        if (!m_config.candidatesAfterCursor ())
            m_cursor --;

        erase_input_sequence (m_text, m_cursor, 1);
        insert_symbol (m_text, m_cursor, symbols->getType (),
                       symbols->getLookup (), symbols->getChoice ());

        m_cursor += offset;
        m_input_state = STATE_INPUT;

        update ();
        return TRUE;
    }

    if (STATE_USER_SYMBOL_LIST_ALL == m_input_state ||
        STATE_USER_SYMBOL_SHOWN == m_input_state) {
        SymbolSectionPtr symbols = m_symbol_sections[m_input_state];

        int offset = symbols->selectCandidate (index);

        if (0 == offset)
            return FALSE;

        if (1 == offset) { /* direct commit. */
            String choice = symbols->getChoice ();
            String lookup;
            int ch = find_lookup_key (choice);
            if (ch != 0)
                lookup = (gchar) ch;

            assert (BUILTIN_SYMBOL_TYPE == symbols->getType ());
            erase_input_sequence (m_text, m_cursor, 1);
            insert_symbol (m_text, m_cursor, symbols->getType (),
                           lookup, choice);

            m_cursor += offset;
            m_input_state = STATE_INPUT;

            update ();
            return TRUE;
        }

        if (1 < offset) { /* show candidate. */
            String choice = symbols->getChoice ();

            m_input_state = STATE_USER_SYMBOL_SHOWN;
            m_symbol_sections[m_input_state]->initCandidates
                (m_instance, choice);

            update ();
            return TRUE;
        }
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

guint
PhoneticEditor::getCursorLeft (void)
{
    /* NOTE: adjust cursor when in parsed phonetic section. */
    const String & enhanced_text = m_text;
    guint cursor = m_cursor;

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    /* move cursor one character back. */
    probe_section_start (enhanced_text, m_cursor - 1,
                         cursor, index, start_pos);

    section_t type = probe_section_quick (enhanced_text, start_pos);

    /* only when in phonetic section, need adjustments.  */
    if (PHONETIC_SECTION == type) {
        String section;
        get_phonetic_section (enhanced_text, start_pos, end_pos, section);
        size_t section_len = end_pos - start_pos;

        zhuyin_instance_t * instance = m_instances[index];
        size_t parsed_len = zhuyin_get_parsed_input_length (instance);

        assert (cursor < section_len);
        assert (parsed_len <= section_len);

        /* only when in parsed phonetic section, need adjustments.  */
        if (cursor < parsed_len) {
            guint16 offset = 0;
            zhuyin_get_zhuyin_key_rest_offset (instance, cursor, &offset);

            /* move to the begin of current syllable. */
            ChewingKeyRest * key_rest = NULL;
            zhuyin_get_zhuyin_key_rest (instance, offset, &key_rest);

            guint16 begin = 0;
            zhuyin_get_zhuyin_key_rest_positions
                (instance, key_rest, &begin, NULL);

            /* align to the begin of chewing key. */
            /* restore cursor variable. */
            return m_cursor - (cursor + 1 - begin);
        }
    }

    return m_cursor - 1;
}

guint
PhoneticEditor::getCursorRight (void)
{
    /* NOTE: adjust cursor when in parsed phonetic section. */
    const String & enhanced_text = m_text;
    guint cursor = m_cursor;

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    probe_section_start (enhanced_text, m_cursor,
                         cursor, index, start_pos);

    section_t type = probe_section_quick (enhanced_text, start_pos);

    /* when in phonetic section, need adjustments.  */
    if (PHONETIC_SECTION == type) {
        String section;
        get_phonetic_section (enhanced_text, start_pos, end_pos, section);
        size_t section_len = end_pos - start_pos;

        zhuyin_instance_t * instance = m_instances[index];
        size_t parsed_len = zhuyin_get_parsed_input_length (instance);

        assert (cursor < section_len);
        assert (parsed_len <= section_len);

        /* when in parsed phonetic section, need adjustments.  */
        if (cursor < parsed_len) {
            guint16 offset = 0;
            zhuyin_get_zhuyin_key_rest_offset (instance, cursor, &offset);
            offset ++;

            guint len = 0;
            zhuyin_get_n_zhuyin (instance, &len);

            if (offset < len) {
                /* move to the begin of next syllable. */
                ChewingKeyRest * key_rest = NULL;
                zhuyin_get_zhuyin_key_rest (instance, offset, &key_rest);

                guint16 begin = 0;
                zhuyin_get_zhuyin_key_rest_positions
                (instance, key_rest, &begin, NULL);

                /* align to the begin of chewing key. */
                return m_cursor + (begin - cursor);
            } else {
                assert (offset == len);
                /* align to the end of parsed phonetic section. */
                return m_cursor + (parsed_len - cursor);
            }
        }
    }

    /* for symbol section. */
    return m_cursor + 1;
}


gboolean
PhoneticEditor::removeCharBefore (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    guint cursor = getCursorLeft ();
    erase_input_sequence (m_text, cursor, m_cursor - cursor);
    m_cursor = cursor;

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

    guint cursor = getCursorRight ();
    erase_input_sequence (m_text, m_cursor, cursor - m_cursor);

    updateZhuyin ();
    update ();

    return TRUE;
}

gboolean
PhoneticEditor::moveCursorLeft (void)
{
    if (G_UNLIKELY (m_cursor == 0))
        return FALSE;

    m_cursor = getCursorLeft ();
    update ();
    return TRUE;
}

gboolean
PhoneticEditor::moveCursorRight (void)
{
    if (G_UNLIKELY (m_cursor ==
                    get_enhanced_text_length (m_text)))
        return FALSE;

    m_cursor = getCursorRight ();
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
                guint16 inner_cursor = 0;
                zhuyin_get_zhuyin_key_rest_offset
                    (instance, cursor, &inner_cursor);

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
PhoneticEditor::insertSymbol (guint keyval, guint keycode, guint modifiers)
{
    /* for symbols. */
    if (is_full_width_symbol (keyval)) {
        if (m_props.modeFullWidth ()) {
            String choice;
            assert (convert_full_width_symbol (keyval, choice));

            String lookup;
            int ch = find_lookup_key (choice);
            if (ch != 0)
                lookup = (gchar) ch;

            insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                           lookup, choice);
        } else {
            String choice = (gchar) keyval;
            if (is_special_symbol (keyval))
                convert_special_symbol (keyval, choice);

            insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                           "", choice);
        }

        return TRUE;
    }

    return FALSE;
}

gboolean
PhoneticEditor::insertNumbers (guint keyval, guint keycode, guint modifiers)
{
    /* for input pad numbers. */
    static const guint keyvals[] = {IBUS_KP_Delete, IBUS_KP_Insert,
                                    IBUS_KP_End, IBUS_KP_Down,
                                    IBUS_KP_Next, IBUS_KP_Left,
                                    IBUS_KP_Begin, IBUS_KP_Right,
                                    IBUS_KP_Home, IBUS_KP_Up,
                                    IBUS_KP_Prior};

    static const char numbers[] = {'.', '0',
                                   '1', '2',
                                   '3', '4',
                                   '5', '6',
                                   '7', '8',
                                   '9'};

    assert (G_N_ELEMENTS (keyvals) == G_N_ELEMENTS (numbers));

    if (!m_config.alwaysInputNumbers ())
        return FALSE;

    for (size_t i = 0; i < G_N_ELEMENTS (keyvals); ++i) {
        if (keyvals[i] == keyval) {
            String choice;
            choice += numbers[i];

            insert_symbol (m_text, m_cursor++, BUILTIN_SYMBOL_TYPE,
                           "", choice);

            return TRUE;
        }
    }

    return FALSE;
}


gboolean
PhoneticEditor::prepareCandidates (void)
{
    const String & enhanced_text = m_text;
    guint cursor = m_cursor;

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    /* hack here: we use "m_cursor - 1" to determine the section type
       for candidates before cursor. */
    if (m_config.candidatesAfterCursor ())
        probe_section_start (enhanced_text, m_cursor,
                             cursor, index, start_pos);
    else
        probe_section_start (enhanced_text, m_cursor - 1,
                             cursor, index, start_pos);

    gboolean show_candidates = FALSE;
    if (m_config.candidatesAfterCursor ()) {
        show_candidates = m_cursor >= 0 &&
            m_cursor < get_enhanced_text_length (enhanced_text);
    } else {
        show_candidates = m_cursor > 0 &&
            m_cursor <= get_enhanced_text_length (enhanced_text);
    }

    /* deal with candidates */
    if (show_candidates) {
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
                m_input_state = STATE_CANDIDATE_SHOWN;
                if (m_config.candidatesAfterCursor ())
                    m_phonetic_section->initCandidates (instance, cursor);
                else
                    /* hack here: restore cursor. */
                    m_phonetic_section->initCandidates (instance, cursor + 1);

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

    return FALSE;
}


};
