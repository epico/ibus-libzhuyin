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

#ifndef __ZY_LIB_ZHUYIN_H_
#define __ZY_LIB_ZHUYIN_H_

#include <memory>
#include <glib.h>

typedef struct _zhuyin_context_t zhuyin_context_t;
typedef struct _zhuyin_instance_t zhuyin_instance_t;

namespace ZY {

class Config;

class LibZhuyinBackEnd {

public:
    LibZhuyinBackEnd ();
    virtual ~LibZhuyinBackEnd ();

    gboolean setZhuyinOptions (Config *config);

    zhuyin_context_t * initZhuyinContext (Config *config);

    zhuyin_instance_t *allocZhuyinInstance ();
    void freeZhuyinInstance (zhuyin_instance_t *instance);

    void modified (void);

#if 0
    gboolean importZhuyinDictionary (const char * filename);
    gboolean cleanZhuyinUserData (const char * target);
#endif

    /* use static initializer in C++. */
    static LibZhuyinBackEnd & instance (void) { return *m_instance; }

    static void init (void);
    static void finalize (void);

private:
    gboolean saveUserDB (void);
    static gboolean timeoutCallback (gpointer data);

private:
    /* libzhuyin context */
    zhuyin_context_t *m_zhuyin_context;

    guint m_timeout_id;
    GTimer *m_timer;

private:
    static std::unique_ptr<LibZhuyinBackEnd> m_instance;
};

};

#endif
