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

#ifndef __ZY_LIB_ZHUYIN_BASE_EDITOR_H_
#define __ZY_LIB_ZHUYIN_BASE_EDITOR_H_

#include <zhuyin.h>
#include "ZYLookupTable.h"
#include "ZYZEnhancedEditor.h"
#include "ZYSymbolLookup.h"
#include <vector>

namespace ZY {

class ZhuyinEngine;

class PhoneticSection;
typedef std::shared_ptr<PhoneticSection> PhoneticSectionPtr;

class SymbolSection;
typedef std::shared_ptr<SymbolSection> SymbolSectionPtr;

class PhoneticEditor : public EnhancedEditor {
    friend class ZhuyinEngine;
    friend class SymbolSection;
    friend class PhoneticSection;

protected:
    typedef std::vector<zhuyin_instance_t *> zhuyin_instance_vec;

public:
    PhoneticEditor (ZhuyinProperties & props, Config & config);
    virtual ~PhoneticEditor (void);

public:
    /* virtual functions */
    virtual void pageUp (void);
    virtual void pageDown (void);
    virtual void cursorUp (void);
    virtual void cursorDown (void);
    virtual void update (void);
    virtual void reset (void);
    virtual void candidateClicked (guint index, guint button, guint state);
    virtual gboolean processKeyEvent (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processEscape (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processSpace (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processCommit (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processSelectCandidate (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processFunctionKey (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processShowCandidateKey (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processCandidateKey (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processEasySymbolKey (guint keyval, guint keycode, guint modifiers);
    virtual gboolean processUserSymbolKey (guint keyval, guint keycode, guint modifiers);
    virtual void updateLookupTable ();
    virtual void updateLookupTableFast ();
    virtual void updateLookupTableLabel ();
    virtual gboolean fillLookupTableByPage ();

    virtual gboolean insertPunct (guint ch);
    virtual gboolean insertEnglish (guint ch);
    virtual gboolean insertNumbers (guint ch);

protected:
    gboolean prepareCandidates (void);
    gboolean selectCandidate (guint i);
    gboolean selectCandidateInPage (guint i);
    guint getCursorLeft (void);
    guint getCursorRight (void);

    void commit (const gchar *str);

    /* pure virtual functions */
    virtual gboolean insert (guint keyval, guint keycode, guint modifiers) = 0;
    virtual gboolean removeCharBefore (void);
    virtual gboolean removeCharAfter (void);
    virtual gboolean moveCursorLeft (void);
    virtual gboolean moveCursorRight (void);
    virtual gboolean moveCursorToBegin (void);
    virtual gboolean moveCursorToEnd (void);
    virtual void commit (void) = 0;
    virtual void updateAuxiliaryText (void) = 0;
    virtual void updatePreeditText (void) = 0;
    virtual void updateZhuyin (void) = 0;

    /* for class DaChen26Editor to override this. */
    virtual guint getZhuyinCursor (void);

    void resizeInstances (void);

    gboolean loadEasySymbolFile (const gchar * filename);

    /* varibles */
    LookupTable                 m_lookup_table;
    String                      m_buffer;

    /* use libzhuyin here. */
    zhuyin_instance_t           *m_instance;

    enum InputState {
        STATE_INPUT = 0,               // input state
        STATE_CANDIDATE_SHOWN,         // candidates shown state
        STATE_BUILTIN_SYMBOL_SHOWN,    // built-in symbol shown state
        STATE_BOPOMOFO_SYMBOL_SHOWN,   // bopomofo symbol shown state
        STATE_USER_SYMBOL_LIST_ALL,    // user symbol input state
        STATE_USER_SYMBOL_SHOWN,       // user symbol shown state
        STATE_LAST,
    } m_input_state;

    SymbolSectionPtr m_symbol_sections[STATE_LAST];
    PhoneticSectionPtr m_phonetic_section;

    zhuyin_instance_vec m_instances;

    SymbolLookup m_easy_symbols;

    /* move left one character when cursor is at the end,
     * only in candidates after cursor option.
     */
    gboolean m_moved_left;
};

};

#endif
