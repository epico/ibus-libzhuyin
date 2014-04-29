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

namespace ZY {

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
    if (NULL == m_zhuyin_context)
        return FALSE;

    ZhuyinScheme scheme = config->keyboardLayout ();
    switch (scheme) {
    case CHEWING_STANDARD ... CHEWING_DACHEN_CP26:
        zhuyin_set_chewing_scheme (m_zhuyin_context, scheme);
        break;
    case FULL_PINYIN_HANYU ... FULL_PINYIN_SECONDARY_BOPOMOFO:
        zhuyin_set_full_pinyin_scheme (m_zhuyin_context, scheme);
        break;
    }

    zhuyin_option_t options = config->option ();
    zhuyin_set_options (m_zhuyin_context, options);
    return TRUE;
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

void
LibZhuyinBackEnd::modified (void)
{
    /* Restart the timer */
    g_timer_start (m_timer);

    if (m_timeout_id != 0)
        return;

    m_timeout_id = g_timeout_add_seconds (LIBZHUYIN_SAVE_TIMEOUT,
                                          LibZhuyinBackEnd::timeoutCallback,
                                          static_cast<gpointer> (this));
}

gboolean
LibZhuyinBackEnd::timeoutCallback (gpointer data)
{
    LibZhuyinBackEnd *self = static_cast<LibZhuyinBackEnd *>(data);

    /* Get the elapsed time since last modification of database. */
    guint elapsed = (guint)g_timer_elapsed (self->m_timer, NULL);

    if (elapsed >= LIBZHUYIN_SAVE_TIMEOUT &&
        self->saveUserDB ()) {
        self->m_timeout_id = 0;
        return FALSE;
    }

    return TRUE;
}

};
