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
#include "ZYConfig.h"
#include "ZYLibZhuyin.h"


using namespace ZY;

ZhuyinEditor::ZhuyinEditor (ZhuyinProperties & props, Config & config)
    : PhoneticEditor (props, config)
{
    m_instance = LibZhuyinBackEnd::instance ().allocZhuyinInstance ();
}

ZhuyinEditor::~ZhuyinEditor (void)
{
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
    PhoneticEditor::reset ();
}

void
ZhuyinEditor::updateZhuyin (void)
{
    String & enhanced_text = m_text;

    resizeInstances ();

    size_t index = 0;
    size_t start_pos = 0, end_pos = 0;

    while (end_pos != enhanced_text.size ()) {
        section_t type = probe_section_quick (enhanced_text, start_pos);

        if (PHONETIC_SECTION == type) {
            String section;
            get_phonetic_section (enhanced_text, start_pos, end_pos, section);
            zhuyin_instance_t * instance = m_instances[index];
            zhuyin_parse_more_chewings (instance, section.c_str ());
            zhuyin_guess_sentence (instance);
            ++index;
        }

        if (SYMBOL_SECTION == type) {
            String type, lookup, choice;
            get_symbol_section (enhanced_text, start_pos, end_pos,
                                type, lookup, choice);
        }

        start_pos = end_pos;
    }

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

    String & enhanced_text = m_text;

    assert (FALSE);
}

void
ZhuyinEditor::update (void)
{
    updateLookupTable ();
    updatePreeditText ();
    updateAuxiliaryText ();
}
