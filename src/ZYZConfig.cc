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
#include <string.h>

#define USE_G_SETTINGS_LIST_KEYS 0

namespace ZY {

const gchar * const CONFIG_FUZZY_ZHUYIN              = "fuzzy-zhuyin";
const gchar * const CONFIG_PAGE_SIZE                 = "candidate-num";

const gchar * const CONFIG_INIT_CHINESE              = "chinese-mode";
const gchar * const CONFIG_INIT_FULL_WIDTH           = "full-half-width";
const gchar * const CONFIG_INIT_TRAD_CHINESE         = "traditional-chinese";
const gchar * const CONFIG_ALWAYS_INPUT_NUMBERS      = "always-input-num";
const gchar * const CONFIG_SPACE_SHOW_CANDIDATES     = "space-show-candidates";
const gchar * const CONFIG_CANDIDATES_AFTER_CURSOR   = "candidates-after-cursor";

const gchar * const CONFIG_KEYBOARD_LAYOUT           = "keyboard-layout";
const gchar * const CONFIG_CANDIDATE_KEYS            = "candidate-keys";

const gchar * const CONFIG_EASY_SYMBOL               = "easy-symbol";
const gchar * const CONFIG_USER_SYMBOL               = "user-symbol";

const gchar * const CONFIG_IMPORT_DICTIONARY         = "import-dictionary";
const gchar * const CONFIG_CLEAR_USER_DATA           = "clear-user-data";

const zhuyin_option_t ZHUYIN_DEFAULT_OPTION =
    USE_TONE           |
    FORCE_TONE         |
    ZHUYIN_CORRECT_ALL |
    0;

std::unique_ptr<ZhuyinConfig> ZhuyinConfig::m_instance;

ZhuyinConfig::ZhuyinConfig ()
    : Config ("com.github.libzhuyin.ibus-libzhuyin")
{
    m_settings = g_settings_new (m_schema_id.c_str ());
    initDefaultValues ();
    g_signal_connect (m_settings,
                      "changed",
                      G_CALLBACK (valueChangedCallback),
                      this);

}

ZhuyinConfig::~ZhuyinConfig (void)
{
    g_object_unref (m_settings);
    m_settings = NULL;
}

void
ZhuyinConfig::init ()
{
    if (m_instance.get () == NULL) {
        m_instance.reset (new ZhuyinConfig ());
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

    m_is_zhuyin = TRUE;
    m_zhuyin_scheme = ZHUYIN_DEFAULT;
    m_pinyin_scheme = FULL_PINYIN_DEFAULT;

    m_init_chinese = TRUE;
    m_init_full_width = FALSE;
    m_init_trad_chinese = TRUE;

    m_always_input_numbers = FALSE;
    m_space_show_candidates = FALSE;
    m_candidates_after_cursor = TRUE;

    m_candidate_keys = "1234567890";

    m_easy_symbol = TRUE;
    m_user_symbol = TRUE;
}

/* Here are the zhuyin keyboard layout mapping table. */
static const struct {
    gint layout;
    ZhuyinScheme scheme;
} zhuyin_schemes [] = {
    {0, ZHUYIN_STANDARD},
    {1, ZHUYIN_HSU},
    {2, ZHUYIN_IBM},
    {3, ZHUYIN_GINYIEH},
    {4, ZHUYIN_ETEN},
    {5, ZHUYIN_ETEN26},
    {6, ZHUYIN_STANDARD_DVORAK},
    {7, ZHUYIN_HSU_DVORAK},
    {8, ZHUYIN_DACHEN_CP26},
};

static const struct {
    gint layout;
    FullPinyinScheme scheme;
} pinyin_schemes [] = {
    {9, FULL_PINYIN_HANYU},
    {10, FULL_PINYIN_LUOMA},
    {11, FULL_PINYIN_SECONDARY_ZHUYIN},
};

static const struct {
    const gchar * const name;
    guint option;
} fuzzy_zhuyin_options [] = {
    /* fuzzy pinyin */
    { "fuzzy-zhuyin-c-ch",       ZHUYIN_AMB_C_CH      },
    { "fuzzy-zhuyin-z-zh",       ZHUYIN_AMB_Z_ZH      },
    { "fuzzy-zhuyin-s-sh",       ZHUYIN_AMB_S_SH      },
    { "fuzzy-zhuyin-l-n",        ZHUYIN_AMB_L_N       },
    { "fuzzy-zhuyin-f-h",        ZHUYIN_AMB_F_H       },
    { "fuzzy-zhuyin-l-r",        ZHUYIN_AMB_L_R       },
    { "fuzzy-zhuyin-g-k",        ZHUYIN_AMB_G_K       },
    { "fuzzy-zhuyin-an-ang",     ZHUYIN_AMB_AN_ANG    },
    { "fuzzy-zhuyin-en-eng",     ZHUYIN_AMB_EN_ENG    },
    { "fuzzy-zhuyin-in-ing",     ZHUYIN_AMB_IN_ING    },
};

void
ZhuyinConfig::readDefaultValues (void)
{
#if USE_G_SETTINGS_LIST_KEYS
    /* read all values together */
    initDefaultValues ();
    gchar **keys = g_settings_list_keys (m_settings);
    g_return_if_fail (keys != NULL);

    for (gchar **iter = keys; *iter != NULL; ++iter) {
        gchar *name = *iter;

        /* skip signals here. */
        if (0 == strcmp(CONFIG_IMPORT_DICTIONARY, name))
            continue;

        if (0 == strcmp(CONFIG_CLEAR_USER_DATA, name))
            continue;

        GVariant *value = g_settings_get_value (m_settings, name);
        valueChanged (m_schema_id, name, value);
        g_variant_unref (value);
    }

    g_strfreev (keys);
#else
    /* others */
    m_page_size = read (CONFIG_PAGE_SIZE, 10);
    if (m_page_size > 10) {
        m_page_size = 10;
        g_warn_if_reached ();
    }

    /* init states */
    m_init_chinese = read (CONFIG_INIT_CHINESE, true);
    m_init_full_width = read (CONFIG_INIT_FULL_WIDTH, false);
    m_init_trad_chinese = read (CONFIG_INIT_TRAD_CHINESE, true);

    m_always_input_numbers = read (CONFIG_ALWAYS_INPUT_NUMBERS, false);
    m_space_show_candidates = read (CONFIG_SPACE_SHOW_CANDIDATES, false);
    m_candidates_after_cursor = read (CONFIG_CANDIDATES_AFTER_CURSOR, true);

    gint layout = read (CONFIG_KEYBOARD_LAYOUT, 0);

    m_is_zhuyin = TRUE;
    m_zhuyin_scheme = ZHUYIN_DEFAULT;
    m_pinyin_scheme = FULL_PINYIN_DEFAULT;

    for (guint i = 0; i < G_N_ELEMENTS (pinyin_schemes); ++i) {
        if (pinyin_schemes[i].layout == layout) {
            m_is_zhuyin = FALSE;
            m_pinyin_scheme = pinyin_schemes[i].scheme;
        }
    }

    for (guint i = 0; i < G_N_ELEMENTS (zhuyin_schemes); ++i) {
        if (zhuyin_schemes[i].layout == layout) {
            m_is_zhuyin = TRUE;
            m_zhuyin_scheme = zhuyin_schemes[i].scheme;
        }
    }

    m_candidate_keys = read (CONFIG_CANDIDATE_KEYS, std::string ("1234567890"));

    m_easy_symbol = read (CONFIG_EASY_SYMBOL, true);
    m_user_symbol = read (CONFIG_USER_SYMBOL, true);

    /* fuzzy zhuyin */
    if (read (CONFIG_FUZZY_ZHUYIN, false))
        m_option_mask |= ZHUYIN_AMB_ALL;
    else
        m_option_mask &= ~ZHUYIN_AMB_ALL;

    /* read values */
    for (guint i = 0; i < G_N_ELEMENTS (fuzzy_zhuyin_options); i++) {
        if (read (fuzzy_zhuyin_options[i].name,
                  (fuzzy_zhuyin_options[i].option & ZHUYIN_DEFAULT_OPTION) != 0)) {
            m_option |= fuzzy_zhuyin_options[i].option;
        }
        else {
            m_option &= ~fuzzy_zhuyin_options[i].option;
        }
    }
#endif
}

gboolean
ZhuyinConfig::valueChanged (const std::string &schema_id,
                            const std::string &name,
                            GVariant          *value)
{
    if (m_schema_id != schema_id)
        return FALSE;

    if (Config::valueChanged (schema_id, name, value))
        return TRUE;

    /* init states */
    if (CONFIG_INIT_CHINESE == name)
        m_init_chinese = normalizeGVariant (value, true);
    else if (CONFIG_INIT_FULL_WIDTH == name)
        m_init_full_width = normalizeGVariant (value, false);
    else if (CONFIG_INIT_TRAD_CHINESE == name)
        m_init_trad_chinese = normalizeGVariant (value, true);
    else if (CONFIG_ALWAYS_INPUT_NUMBERS == name)
        m_always_input_numbers = normalizeGVariant (value, false);
    else if (CONFIG_SPACE_SHOW_CANDIDATES == name)
        m_space_show_candidates = normalizeGVariant (value, false);
    else if (CONFIG_CANDIDATES_AFTER_CURSOR == name)
        m_candidates_after_cursor = normalizeGVariant (value, true);
    else if (CONFIG_KEYBOARD_LAYOUT == name) {
        gint layout = normalizeGVariant (value, 0);

        m_is_zhuyin = TRUE;
        m_zhuyin_scheme = ZHUYIN_DEFAULT;
        m_pinyin_scheme = FULL_PINYIN_DEFAULT;

        for (guint i = 0; i < G_N_ELEMENTS (pinyin_schemes); ++i) {
            if (pinyin_schemes[i].layout == layout) {
                m_is_zhuyin = FALSE;
                m_pinyin_scheme = pinyin_schemes[i].scheme;
            }
        }

        for (guint i = 0; i < G_N_ELEMENTS (zhuyin_schemes); ++i) {
            if (zhuyin_schemes[i].layout == layout) {
                m_is_zhuyin = TRUE;
                m_zhuyin_scheme = zhuyin_schemes[i].scheme;
            }
        }
    } else if (CONFIG_CANDIDATE_KEYS == name) {
        m_candidate_keys = normalizeGVariant (value, std::string ("1234567890"));
    } /* lookup table page size */
    else if (CONFIG_PAGE_SIZE == name) {
        m_page_size = normalizeGVariant (value, 10);
        if (m_page_size > 10) {
            m_page_size = 10;
            g_warn_if_reached ();
        }
    }
    else if (CONFIG_EASY_SYMBOL == name) {
        m_easy_symbol = normalizeGVariant (value, true);
    }
    else if (CONFIG_USER_SYMBOL == name) {
        m_user_symbol = normalizeGVariant (value, true);
    }
    else if (CONFIG_IMPORT_DICTIONARY == name) {
        std::string filename = normalizeGVariant (value, std::string(""));
        LibZhuyinBackEnd::instance ().importZhuyinDictionary(filename.c_str ());
    }
    else if (CONFIG_CLEAR_USER_DATA == name) {
        std::string target = normalizeGVariant (value, std::string(""));
        LibZhuyinBackEnd::instance ().clearZhuyinUserData(target.c_str ());
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
ZhuyinConfig::valueChangedCallback (GSettings    *settings,
                                    const gchar  *name,
                                    ZhuyinConfig *self)
{
    gchar * property = NULL;
    g_object_get (settings, "schema-id", &property, NULL);
    std::string schema_id (property);
    g_free (property);

    if (self->m_schema_id != schema_id)
        return;

    GVariant * value = g_settings_get_value (settings, name);
    self->valueChanged (self->m_schema_id, name, value);
    g_variant_unref (value);

    if (self->m_schema_id == "com.github.libzhuyin.ibus-libzhuyin")
        LibZhuyinBackEnd::instance ().setZhuyinOptions (self);
}

};
