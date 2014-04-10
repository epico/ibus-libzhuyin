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
#include <zhuyin.h>

namespace ZY {

const gchar * const CONFIG_FUZZY_ZHUYIN              = "fuzzyzhuyin";
const gchar * const CONFIG_ORIENTATION               = "LookupTableOrientation";
const gchar * const CONFIG_PAGE_SIZE                 = "candidatenum";

const gchar * const CONFIG_KEYBOARD_LAYOUT           = "keyboardlayout";
const gchar * const CONFIG_INIT_CHINESE              = "chinesemode";
const gchar * const CONFIG_INIT_FULL_ENGLISH         = "fullhalfenglish";
const gchar * const CONFIG_INIT_FULL_PUNCT           = "fullhalfpunct";
const gchar * const CONFIG_INIT_TRAD_CHINESE         = "TraditionalChinese";
const gchar * const CONFIG_CANDIDATE_KEYS            = "candidatekeys";

const zhuyin_option_t ZHUYIN_DEFAULT_OPTION =
    USE_TONE           |
    FORCE_TONE         |
    ZHUYIN_CORRECT_ALL |
    0;

std::unique_ptr<ZhuyinConfig> ZhuyinConfig::m_instance;

ZhuyinConfig::ZhuyinConfig (Bus & bus)
    : Config (bus, "zhuyin")
{
    initDefaultValues ();
    g_signal_connect (get<IBusConfig> (),
                      "value-changed",
                      G_CALLBACK (valueChangedCallback),
                      this);

}

ZhuyinConfig::~ZhuyinConfig (void)
{
}

void
ZhuyinConfig::initDefaultValues (void)
{
    m_option = ZHUYIN_DEFAULT_OPTION;
    m_option_mask = USE_TONE | FORCE_TONE | ZHUYIN_CORRECT_ALL;

    m_orientation = IBUS_ORIENTATION_VERTICAL;
    m_page_size = 10;

    m_zhuyin_schema = 0;

    m_init_chinese = TRUE;
    m_init_full_english = FALSE;
    m_init_full_punct = TRUE;
    m_init_trad_chinese = TRUE;

    m_candidate_keys = "1234567890";
}

static const struct {
    const gchar * const name;
    guint option;
} options [] = {
    /* fuzzy pinyin */
    { "fuzzyzhuyin_c_ch",       ZHUYIN_AMB_C_CH      },
    { "fuzzyzhuyin_z_zh",       ZHUYIN_AMB_Z_ZH      },
    { "fuzzyzhuyin_s_sh",       ZHUYIN_AMB_S_SH      },
    { "fuzzyzhuyin_l_n",        ZHUYIN_AMB_L_N       },
    { "fuzzyzhuyin_f_h",        ZHUYIN_AMB_F_H       },
    { "fuzzyzhuyin_l_r",        ZHUYIN_AMB_L_R       },
    { "fuzzyzhuyin_g_k",        ZHUYIN_AMB_G_K       },
    { "fuzzyzhuyin_an_ang",     ZHUYIN_AMB_AN_ANG    },
    { "fuzzyzhuyin_en_eng",     ZHUYIN_AMB_EN_ENG    },
    { "fuzzyzhuyin_in_ing",     ZHUYIN_AMB_IN_ING    },
};


};
