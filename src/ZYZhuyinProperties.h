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
#ifndef __ZY_ZHUYIN_PROPERTIES_H_
#define __ZY_ZHUYIN_PROPERTIES_H_

#include "ZYSignal.h"
#include "ZYProperty.h"

namespace ZY {

class Config;

class ZhuyinProperties {
public:
    ZhuyinProperties (Config & config);

    void toggleModeChinese   (void);
    void toggleModeFull      (void);
    void toggleModeTrad      (void);

    void reset (void);

    gboolean modeChinese (void) const   { return m_mode_chinese; }
    gboolean modeFull (void) const      { return m_mode_full; }
    gboolean modeTrad (void) const      { return m_mode_trad; }

    PropList & properties (void)        { return m_props; }

    gboolean propertyActivate (const gchar *prop_name, guint prop_state);

    signal <void (Property &)> & signalUpdateProperty (void)
    {
        return m_signal_update_property;
    }

private:
    void updateProperty (Property & prop) const
    {
        m_signal_update_property (prop);
    }

    signal <void (Property &)> m_signal_update_property;

private:
    Config    & m_config;
    gboolean    m_mode_chinese;
    gboolean    m_mode_full;
    gboolean    m_mode_trad;

    /* properties */
    Property    m_prop_chinese;
    Property    m_prop_full;
    Property    m_prop_trad;
    Property    m_prop_setup;
    PropList    m_props;
};

};

#endif
