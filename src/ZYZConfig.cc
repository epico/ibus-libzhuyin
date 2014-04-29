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
#include "ZYLibZhuyin.h"

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
ZhuyinConfig::init (Bus & bus)
{
    if (m_instance.get () == NULL) {
        m_instance.reset (new ZhuyinConfig (bus));
        m_instance->readDefaultValues ();
    }
}

void
ZhuyinConfig::initDefaultValues (void)
{
    m_option = ZHUYIN_DEFAULT_OPTION;
    m_option_mask = USE_TONE | FORCE_TONE | ZHUYIN_CORRECT_ALL;

    m_orientation = IBUS_ORIENTATION_VERTICAL;
    m_page_size = 10;

    m_keyboard_layout = CHEWING_DEFAULT;

    m_init_chinese = TRUE;
    m_init_full_english = FALSE;
    m_init_full_punct = TRUE;
    m_init_trad_chinese = TRUE;

    m_candidate_keys = "1234567890";
}

/* Here are the zhuyin keyboard layout mapping table. */
static const struct {
    gint layout;
    ZhuyinScheme scheme;
} zhuyin_schemes [] = {
    {0, CHEWING_STANDARD},
    {1, CHEWING_HSU},
    {2, CHEWING_IBM},
    {3, CHEWING_GINYIEH},
    {4, CHEWING_ETEN},
    {5, CHEWING_ETEN26},
    {6, CHEWING_STANDARD_DVORAK},
    {7, CHEWING_HSU_DVORAK},
    {8, CHEWING_DACHEN_CP26},
    {9, FULL_PINYIN_HANYU},
    {10, FULL_PINYIN_LUOMA},
    {11, FULL_PINYIN_SECONDARY_BOPOMOFO},
};

static const struct {
    const gchar * const name;
    guint option;
} fuzzy_zhuyin_options [] = {
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

void
ZhuyinConfig::readDefaultValues (void)
{
#if defined(HAVE_IBUS_CONFIG_GET_VALUES)
    /* read all values together */
    initDefaultValues ();
    GVariant *values =
            ibus_config_get_values (get<IBusConfig> (), m_section.c_str ());
    g_return_if_fail (values != NULL);

    GVariantIter iter;
    gchar *name;
    GVariant *value;
    g_variant_iter_init (&iter, values);
    while (g_variant_iter_next (&iter, "{sv}", &name, &value)) {
        valueChanged (m_section, name, value);
        g_free (name);
        g_variant_unref (value);
    }
    g_variant_unref (values);
#else
    /* others */
    m_orientation = read (CONFIG_ORIENTATION, IBUS_ORIENTATION_VERTICAL);
    if (m_orientation != IBUS_ORIENTATION_VERTICAL &&
        m_orientation != IBUS_ORIENTATION_HORIZONTAL) {
        m_orientation = IBUS_ORIENTATION_VERTICAL;
        g_warn_if_reached ();
    }
    m_page_size = read (CONFIG_PAGE_SIZE, 10);
    if (m_page_size > 10) {
        m_page_size = 10;
        g_warn_if_reached ();
    }

    gint layout = read (CONFIG_KEYBOARD_LAYOUT, 0);
    m_keyboard_layout = CHEWING_DEFAULT;

    for (guint i = 0; i < G_N_ELEMENTS (zhuyin_schemes); ++i) {
        if (zhuyin_schemes[i].layout == layout) {
            m_keyboard_layout = zhuyin_schemes[i].scheme;
        }
    }

    /* init states */
    m_init_chinese = read (CONFIG_INIT_CHINESE, true);
    m_init_full_english = read (CONFIG_INIT_FULL_ENGLISH, false);
    m_init_full_punct = read (CONFIG_INIT_FULL_PUNCT, true);
    m_init_trad_chinese = read (CONFIG_INIT_TRAD_CHINESE, true);

    m_candidate_keys = read (CONFIG_CANDIDATE_KEYS, "1234567890");

    /* fuzzy zhuyin */
    if (read (CONFIG_FUZZY_ZHUYIN, false))
        m_option_mask |= ZHUYIN_AMB_ALL;
    else
        m_option_mask &= ~ZHUYIN_AMB_ALL;

    /* read values */
    for (guint i = 0; i < G_N_ELEMENTS (fuzzy_zhuyin_options); i++) {
        if (read (fuzzy_zhuyin_options[i].name,
                  (fuzzy_zhuyin_options[i].option & PINYIN_DEFAULT_OPTION) != 0)) {
            m_option |= fuzzy_zhuyin_options[i].option;
        }
        else {
            m_option &= ~fuzzy_zhuyin_options[i].option;
        }
    }
#endif
}

gboolean
ZhuyinConfig::valueChanged (const std::string &section,
                            const std::string &name,
                            GVariant          *value)
{
    if (m_section != section)
        return FALSE;

    if (Config::valueChanged (section, name, value))
        return TRUE;

    /* init states */
    if (CONFIG_INIT_CHINESE == name)
        m_init_chinese = normalizeGVariant (value, true);
    else if (CONFIG_INIT_FULL_ENGLISH == name)
        m_init_full_english = normalizeGVariant (value, true);
    else if (CONFIG_INIT_FULL_PUNCT == name)
        m_init_full_punct = normalizeGVariant (value, true);
    else if (CONFIG_INIT_TRAD_CHINESE == name)
        m_init_trad_chinese = normalizeGVariant (value, false);
    else if (CONFIG_KEYBOARD_LAYOUT == name) {
        gint layout = normalizeGVariant (value, 0);
        m_keyboard_layout = CHEWING_DEFAULT;

        for (guint i = 0; i < G_N_ELEMENTS (zhuyin_schemes); ++i) {
            if (zhuyin_schemes[i].layout == layout) {
                m_keyboard_layout = zhuyin_schemes[i].scheme;
            }
        }
    } else if (CONFIG_CANDIDATE_KEYS == name) {
        m_candidate_keys = normalizeGVariant (value, "1234567890");
    } /* lookup table page size */
    else if (CONFIG_ORIENTATION == name) {
        m_orientation = normalizeGVariant (value, IBUS_ORIENTATION_VERTICAL);
        if (m_orientation != IBUS_ORIENTATION_VERTICAL &&
            m_orientation != IBUS_ORIENTATION_HORIZONTAL) {
            m_orientation = IBUS_ORIENTATION_VERTICAL;
            g_warn_if_reached ();
        }
    }
    else if (CONFIG_PAGE_SIZE == name) {
        m_page_size = normalizeGVariant (value, 10);
        if (m_page_size > 10) {
            m_page_size = 10;
            g_warn_if_reached ();
        }
    } /* fuzzy zhuyin */
    else if (CONFIG_FUZZY_ZHUYIN == name) {
        if (normalizeGVariant (value, false))
            m_option_mask |= ZHUYIN_AMB_ALL;
        else
            m_option_mask &= ~ZHUYIN_AMB_ALL;
    }
    else {
        for (guint i = 0; i < G_N_ELEMENTS (fuzzy_zhuyin_options); i++) {
            if (G_LIKELY (fuzzy_zhuyin_options[i].name != name))
                continue;
            if (normalizeGVariant (value,
                    (fuzzy_zhuyin_options[i].option & ZHUYIN_DEFAULT_OPTION) != 0))
                m_option |= fuzzy_zhuyin_options[i].option;
            else
                m_option &= ~fuzzy_zhuyin_options[i].option;
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;

}

void
ZhuyinConfig::valueChangedCallback (IBusConfig   *config,
                                    const gchar  *section,
                                    const gchar  *name,
                                    GVariant     *value,
                                    ZhuyinConfig *self)
{
    if (self->m_section != section)
        return;

    self->valueChanged (section, name, value);

    LibZhuyinBackEnd::instance ().setZhuyinOptions (self);
}

};
