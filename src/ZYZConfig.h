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

#ifndef __ZY_LIB_ZHUYIN_ZHUYIN_CONFIG_H_
#define __ZY_LIB_ZHUYIN_ZHUYIN_CONFIG_H_

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <glib.h>
#include <ibus.h>
#include "ZYConfig.h"

namespace ZY {

class ZhuyinConfig : public Config {
public:
    static void init ();
    static ZhuyinConfig & instance (void) { return *m_instance; }

protected:
    ZhuyinConfig ();

public:
    virtual ~ZhuyinConfig (void);

protected:
    void initDefaultValues (void);

    virtual void readDefaultValues (void);
    virtual gboolean valueChanged (const std::string &schema_id,
                                   const std::string &name,
                                   GVariant          *value);
private:
    static void valueChangedCallback (GSettings      *settings,
                                      const gchar    *name,
                                      ZhuyinConfig   *self);

private:
    static std::unique_ptr<ZhuyinConfig> m_instance;
};

};

#endif
