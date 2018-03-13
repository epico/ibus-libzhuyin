/* vim:set et ts=4 sts=4:
 *
 * ibus-libzhuyin - New Zhuyin engine based on libzhuyin for IBus
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "ZYConfig.h"

#include "ZYTypes.h"
#include "ZYBus.h"

namespace ZY {


Config::Config (const std::string & name)
    : m_schema_id (name)
{
    m_settings = NULL;
    initDefaultValues ();
}

Config::~Config (void)
{
}

void
Config::initDefaultValues (void)
{
    m_page_size = 5;

    m_is_zhuyin = TRUE;
    m_zhuyin_scheme = ZHUYIN_DEFAULT;
    m_pinyin_scheme = FULL_PINYIN_DEFAULT;

    m_init_chinese = TRUE;
    m_init_full_width = FALSE;
    m_init_trad_chinese = TRUE;

    m_always_input_numbers = FALSE;
    m_space_show_candidates = FALSE;
    m_candidates_after_cursor = TRUE;

    m_easy_symbol = TRUE;
    m_user_symbol = TRUE;
}


void
Config::readDefaultValues (void)
{
}

bool
Config::read (const gchar * name,
              bool          defval)
{
    GVariant *value = NULL;
    if ((value = g_settings_get_value (m_settings, name)) != NULL) {
        if (g_variant_classify (value) == G_VARIANT_CLASS_BOOLEAN)
            return g_variant_get_boolean (value);
    }

    g_warn_if_reached ();
    return defval;
}

gint
Config::read (const gchar * name,
              gint          defval)
{
    GVariant *value = NULL;
    if ((value = g_settings_get_value (m_settings, name)) != NULL) {
        if (g_variant_classify (value) == G_VARIANT_CLASS_INT32)
            return g_variant_get_int32 (value);
    }

    g_warn_if_reached ();
    return defval;
}

std::string
Config::read (const gchar * name,
              const gchar * defval)
{
    GVariant *value = NULL;
    if ((value = g_settings_get_value (m_settings, name)) != NULL) {
        if (g_variant_classify (value) == G_VARIANT_CLASS_STRING)
            return g_variant_get_string (value, NULL);
    }

    g_warn_if_reached ();
    return defval;
}

gboolean
Config::valueChanged (const std::string &schema_id,
                      const std::string &name,
                      GVariant          *value)
{
    return FALSE;
}

void
Config::valueChangedCallback (GSettings   *settings,
                              const gchar *name,
                              Config      *self)
{
    gchar * property = NULL;
    g_object_get (settings, "schema-id", &property, NULL);
    std::string schema_id (property);
    g_free (property);

    GVariant * value = g_settings_get_value (settings, name);
    self->valueChanged (schema_id, name, value);
    g_variant_unref (value);
}

};
