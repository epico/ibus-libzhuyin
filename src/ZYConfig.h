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
#ifndef __ZY_CONFIG_H_
#define __ZY_CONFIG_H_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string>
#include <ibus.h>
#include <zhuyin.h>
#include "ZYUtil.h"
#include "ZYObject.h"

namespace ZY {

class Bus;

class Config : public Object {
protected:
    Config (Bus & bus, const std::string & name);
    virtual ~Config (void);

public:
    zhuyin_option_t option (void) const         { return m_option & m_option_mask; }
    guint orientation (void) const              { return m_orientation; }
    guint pageSize (void) const                 { return m_page_size; }
    ZhuyinScheme keyboardLayout (void) const    { return m_keyboard_layout; }
    gboolean initChinese (void) const           { return m_init_chinese; }
    gboolean initFullEnglish (void) const       { return m_init_full_english; }
    gboolean initFullPunct (void) const         { return m_init_full_punct; }
    gboolean initTradChinese (void) const       { return m_init_trad_chinese; }
    std::string candidateKeys (void) const      { return m_candidate_keys; }

protected:
    bool read (const gchar * name, bool defval);
    gint read (const gchar * name, gint defval);
    std::string read (const gchar * name, const gchar * defval);
    void initDefaultValues (void);

    virtual void readDefaultValues (void);
    virtual gboolean valueChanged (const std::string  &section,
                                   const std::string  &name,
                                   GVariant           *value);
private:
    static void valueChangedCallback (IBusConfig     *config,
                                      const gchar    *section,
                                      const gchar    *name,
                                      GVariant       *value,
                                      Config         *self);

protected:
    std::string m_section;
    zhuyin_option_t m_option;
    zhuyin_option_t m_option_mask;

    gint m_orientation;
    guint m_page_size;

    ZhuyinScheme m_keyboard_layout;

    gboolean m_init_chinese;
    gboolean m_init_full_english;
    gboolean m_init_full_punct;
    gboolean m_init_trad_chinese;

    std::string m_candidate_keys;
};

static inline bool
normalizeGVariant (GVariant *value, bool defval)
{
    if (value == NULL || g_variant_classify (value) != G_VARIANT_CLASS_BOOLEAN)
        return defval;
    return g_variant_get_boolean (value);
}

static inline gint
normalizeGVariant (GVariant *value, gint defval)
{
    if (value == NULL || g_variant_classify (value) != G_VARIANT_CLASS_INT32)
        return defval;
    return g_variant_get_int32 (value);
}

static inline std::string
normalizeGVariant (GVariant *value, const std::string &defval)
{
    if (value == NULL || g_variant_classify (value) != G_VARIANT_CLASS_STRING)
        return defval;
    return g_variant_get_string (value, NULL);
}

};
#endif
