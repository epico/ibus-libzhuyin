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

#include "ZYLibZhuyin.h"

#include <string.h>
#include <assert.h>
#include <zhuyin.h>
#include "ZYZConfig.h"

#define LIBZHUYIN_SAVE_TIMEOUT   (5 * 60)

using namespace ZY;

std::unique_ptr<LibZhuyinBackEnd> LibZhuyinBackEnd::m_instance;

static LibZhuyinBackEnd libzhuyin_backend;

LibZhuyinBackEnd::LibZhuyinBackEnd () {
    m_timeout_id = 0;
    m_timer = g_timer_new ();
    m_zhuyin_context = NULL;
}

LibZhuyinBackEnd::~LibZhuyinBackEnd () {
    g_timer_destroy (m_timer);
    if (m_timeout_id != 0) {
        saveUserDB ();
        g_source_remove (m_timeout_id);
    }

    if (m_zhuyin_context)
        zhuyin_fini (m_zhuyin_context);
    m_zhuyin_context = NULL;
}

zhuyin_context_t *
LibZhuyinBackEnd::initZhuyinContext (Config *config)
{
    zhuyin_context_t * context = NULL;

    gchar * userdir = g_build_filename (g_get_user_cache_dir (),
                                        "ibus", "libzhuyin", NULL);
    int retval = g_mkdir_with_parents (userdir, 0700);
    if (retval) {
        g_free (userdir); userdir = NULL;
    }
    context = zhuyin_init (LIBZHUYIN_DATADIR, userdir);
    g_free (userdir);

    /* load user phrase library. */
    zhuyin_load_phrase_library (context, USER_DICTIONARY);

    return context;
}

gboolean
LibZhuyinBackEnd::setZhuyinOptions (Config *config)
{
    assert (FALSE);
}


zhuyin_instance_t *
LibZhuyinBackEnd::allocZhuyinInstance ()
{
    Config * config = &ZhuyinConfig::instance ();
    if (NULL == m_zhuyin_context) {
        m_zhuyin_context = initZhuyinContext (config);
    }

    setZhuyinOptions (config);
    return zhuyin_alloc_instance (m_zhuyin_context);
}

void
LibZhuyinBackEnd::freeZhuyinInstance (zhuyin_instance_t *instance)
{
    zhuyin_free_instance (instance);
}

void
LibZhuyinBackEnd::init (void) {
    g_assert (NULL == m_instance.get ());
    LibZhuyinBackEnd * backend = new LibZhuyinBackEnd;
    m_instance.reset (backend);
}

void
LibZhuyinBackEnd::finalize (void) {
    m_instance.reset ();
}

gboolean
LibZhuyinBackEnd::saveUserDB (void)
{
    if (m_zhuyin_context)
        zhuyin_save (m_zhuyin_context);
    return TRUE;
}
