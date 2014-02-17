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
#include "ZYUtil.h"
#include "ZYObject.h"

namespace ZY {

class Bus;

class Config : public Object {
protected:
    Config (Bus & bus, const std::string & name);
    virtual ~Config (void);

public:
    guint option (void) const                   { return m_option & m_option_mask; }
    guint pageSize (void) const                 { return m_page_size; }
    gint zhuyinSchema (void) const              { return m_zhuyin_schema; }
    gboolean initChinese (void) const           { return m_init_chinese; }
    gboolean initFull (void) const              { return m_init_full; }
    gboolean initFullPunct (void) const         { return m_init_full_punct; }
    gboolean initTradChinese (void) const       { return m_init_trad_chinese; }
    std::string selectKeys (void) const         { return m_select_keys; }

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
    guint m_option;
    guint m_option_mask;

    guint m_page_size;

    gint m_zhuyin_schema;

    gboolean m_init_chinese;
    gboolean m_init_full;
    gboolean m_init_full_punct;
    gboolean m_init_trad_chinese;

    std::string m_select_keys;
};

};
#endif
