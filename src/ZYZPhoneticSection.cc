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

#include "ZYZPhoneticSection.h"

namespace ZY {

PhoneticSection::PhoneticSection (PhoneticEditor & editor) :
    m_editor (editor)
{
}

PhoneticSection::~PhoneticSection ()
{
}

bool
PhoneticSection::initCandidates (zhuyin_instance_t * instance,
                                 int offset)
{
    m_instance = instance;
    m_offset = offset;

    zhuyin_guess_candidates (m_instance, offset);

    return true;
}

bool
PhoneticSection::populateCandidates ()
{
    /* clear lookup table. */
    LookupTable & lookup_table = getLookupTable ();
    lookup_table.clear ();

    guint num = 0;
    zhuyin_get_n_candidate (m_instance, &num);

    for (size_t i = 0; i < num; ++i) {
        lookup_candidate_t * candidate = NULL;
        zhuyin_get_candidate (m_instance, i, &candidate);

        const gchar * str = NULL;
        zhuyin_get_candidate_string (m_instance, candidate, &str);

        lookup_table.appendCandidate (Text (str));
    }

    return true;
}

int
PhoneticSection::selectCandidate (guint index)
{
    lookup_candidate_t * candidate = NULL;
    zhuyin_get_candidate (m_instance, index, &candidate);

    int retval = zhuyin_choose_candidate (m_instance, m_offset, candidate);

    return retval - m_offset;
}

};
