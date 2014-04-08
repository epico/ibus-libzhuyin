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

#include "ZYZConfig.h"

namespace ZY {

const gchar * const CONFIG_FUZZY_ZHUYIN              = "FuzzyZhuyin";
const gchar * const CONFIG_ORIENTATION               = "LookupTableOrientation";
const gchar * const CONFIG_PAGE_SIZE                 = "LookupTablePageSize";

const gchar * const CONFIG_KEYBOARD_LAYOUT           = "KeyboardLayout";
const gchar * const CONFIG_INIT_CHINESE              = "InitChinese";
const gchar * const CONFIG_INIT_FULL                 = "InitFull";
const gchar * const CONFIG_INIT_FULL_PUNCT           = "InitFullPunct";
const gchar * const CONFIG_INIT_TRAD_CHINESE         = "InitTraditionalChinese";
const gchar * const CONFIG_CANDIDATE_KEYS            = "CandidateKeys";

static const struct {
    const gchar * const name;
    guint option;
} options [] = {
    /* fuzzy pinyin */
    { "FuzzyZhuyin_C_CH",       ZHUYIN_AMB_C_CH      },
    { "FuzzyZhuyin_Z_ZH",       ZHUYIN_AMB_Z_ZH      },
    { "FuzzyZhuyin_S_SH",       ZHUYIN_AMB_S_SH      },
    { "FuzzyZhuyin_L_N",        ZHUYIN_AMB_L_N       },
    { "FuzzyZhuyin_F_H",        ZHUYIN_AMB_F_H       },
    { "FuzzyZhuyin_L_R",        ZHUYIN_AMB_L_R       },
    { "FuzzyZhuyin_G_K",        ZHUYIN_AMB_G_K       },
    { "FuzzyZhuyin_AN_ANG",     ZHUYIN_AMB_AN_ANG    },
    { "FuzzyZhuyin_EN_ENG",     ZHUYIN_AMB_EN_ENG    },
    { "FuzzyZhuyin_IN_ING",     ZHUYIN_AMB_IN_ING    },
};

ZhuyinConfig::ZhuyinConfig (Bus & bus)
    : Config (bus, "zhuyin")
{
}

};
