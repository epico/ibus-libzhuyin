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
#include <gio/gio.h>
#include <ibus.h>
#include <zhuyin.h>
#include "ZYUtil.h"
#include "ZYObject.h"

namespace ZY {

class Config {
protected:
    Config (const std::string & name);
    virtual ~Config (void);

public:
    zhuyin_option_t option (void) const         { return m_option & m_option_mask; }
    guint orientation (void) const              { return m_orientation; }
    guint pageSize (void) const                 { return m_page_size; }
    gboolean isZhuyin(void) const               { return m_is_zhuyin; }
    ZhuyinScheme zhuyinScheme (void) const      { return m_zhuyin_scheme; }
    FullPinyinScheme pinyinScheme (void) const  { return m_pinyin_scheme; }
    gboolean initChinese (void) const           { return m_init_chinese; }
    gboolean initFullWidth (void) const         { return m_init_full_width; }
    gboolean initTradChinese (void) const       { return m_init_trad_chinese; }
    gboolean alwaysInputNumbers (void) const    { return m_always_input_numbers; }
    gboolean spaceShowCandidates (void) const   { return m_space_show_candidates; }
    gboolean candidatesAfterCursor (void) const { return m_candidates_after_cursor; }
    std::string candidateKeys (void) const      { return m_candidate_keys; }

    gboolean easySymbol (void) const            { return m_easy_symbol; }
    gboolean userSymbol (void) const            { return m_user_symbol; }

protected:
    bool read (const gchar * name, bool defval);
    gint read (const gchar * name, gint defval);
    std::string read (const gchar * name, const std::string &defval);
    void initDefaultValues (void);

    virtual void readDefaultValues (void);
    virtual gboolean valueChanged (const std::string  &schema_id,
                                   const std::string  &name,
                                   GVariant           *value);
private:
    static void valueChangedCallback (GSettings      *settings,
                                      const gchar    *name,
                                      Config         *self);

protected:
    GSettings * m_settings;
    std::string m_schema_id;

    zhuyin_option_t m_option;
    zhuyin_option_t m_option_mask;

    gint m_orientation;
    guint m_page_size;

    gboolean m_is_zhuyin;
    ZhuyinScheme m_zhuyin_scheme;
    FullPinyinScheme m_pinyin_scheme;

    gboolean m_init_chinese;
    gboolean m_init_full_width;
    gboolean m_init_trad_chinese;

    gboolean m_always_input_numbers;
    gboolean m_space_show_candidates;
    gboolean m_candidates_after_cursor;

    std::string m_candidate_keys;

    gboolean m_easy_symbol;
    gboolean m_user_symbol;
};

static inline bool
normalizeGVariant (GVariant *value, bool defval)
{
    if (value == NULL ||
        g_variant_classify (value) != G_VARIANT_CLASS_BOOLEAN) {
        g_warn_if_reached ();
        return defval;
    }
    return g_variant_get_boolean (value);
}

static inline gint
normalizeGVariant (GVariant *value, gint defval)
{
    if (value == NULL ||
        g_variant_classify (value) != G_VARIANT_CLASS_INT32) {
        g_warn_if_reached ();
        return defval;
    }
    return g_variant_get_int32 (value);
}

static inline std::string
normalizeGVariant (GVariant *value, const std::string &defval)
{
    if (value == NULL ||
        g_variant_classify (value) != G_VARIANT_CLASS_STRING) {
        g_warn_if_reached ();
        return defval;
    }
    return g_variant_get_string (value, NULL);
}

};
#endif
