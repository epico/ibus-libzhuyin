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

#include <cstring>
#include "ZYEngine.h"
#include "ZYZZhuyinEngine.h"

namespace ZY {
/* code of engine class of GObject */
#define IBUS_ZHUYIN_ENGINE(obj)             \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), IBUS_TYPE_ZHUYIN_ENGINE, IBusZhuyinEngine))
#define IBUS_ZHUYIN_ENGINE_CLASS(klass)     \
    (G_TYPE_CHECK_CLASS_CAST ((klass), IBUS_TYPE_ZHUYIN_ENGINE, IBusZhuyinEngineClass))
#define IBUS_IS_ZHUYIN_ENGINE(obj)          \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), IBUS_TYPE_ZHUYIN_ENGINE))
#define IBUS_IS_ZHUYIN_ENGINE_CLASS(klass)  \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), IBUS_TYPE_ZHUYIN_ENGINE))
#define IBUS_ZHUYIN_ENGINE_GET_CLASS(obj)   \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), IBUS_TYPE_ZHUYIN_ENGINE, IBusZhuyinEngineClass))


typedef struct _IBusZhuyinEngine IBusZhuyinEngine;
typedef struct _IBusZhuyinEngineClass IBusZhuyinEngineClass;

struct _IBusZhuyinEngine {
    IBusEngine parent;

    /* members */
    Engine *engine;
};

struct _IBusZhuyinEngineClass {
    IBusEngineClass parent;
};

/* functions prototype */
static void     ibus_zhuyin_engine_class_init   (IBusZhuyinEngineClass  *klass);
static void     ibus_zhuyin_engine_init         (IBusZhuyinEngine       *zhuyin);
static GObject* ibus_zhuyin_engine_constructor  (GType                   type,
                                                 guint                   n_construct_params,
                                                 GObjectConstructParam  *construct_params);

static void     ibus_zhuyin_engine_destroy      (IBusZhuyinEngine       *zhuyin);
static gboolean ibus_zhuyin_engine_process_key_event
                                                (IBusEngine             *engine,
                                                 guint                   keyval,
                                                 guint                   keycode,
                                                 guint                   modifiers);
static void     ibus_zhuyin_engine_focus_in     (IBusEngine             *engine);
static void     ibus_zhuyin_engine_focus_out    (IBusEngine             *engine);
#if IBUS_CHECK_VERSION (1, 5, 4)
static void     ibus_zhuyin_engine_set_content_type
                                                (IBusEngine     *engine,
                                                 guint           purpose,
                                                 guint           hints);
#endif
static void     ibus_zhuyin_engine_reset        (IBusEngine             *engine);
static void     ibus_zhuyin_engine_enable       (IBusEngine             *engine);
static void     ibus_zhuyin_engine_disable      (IBusEngine             *engine);

#if 0
static void     ibus_engine_set_cursor_location (IBusEngine             *engine,
                                                 gint                    x,
                                                 gint                    y,
                                                 gint                    w,
                                                 gint                    h);
static void     ibus_zhuyin_engine_set_capabilities
                                                (IBusEngine             *engine,
                                                 guint                   caps);
#endif

static void     ibus_zhuyin_engine_page_up      (IBusEngine             *engine);
static void     ibus_zhuyin_engine_page_down    (IBusEngine             *engine);
static void     ibus_zhuyin_engine_cursor_up    (IBusEngine             *engine);
static void     ibus_zhuyin_engine_cursor_down  (IBusEngine             *engine);
static void     ibus_zhuyin_engine_property_activate
                                                (IBusEngine             *engine,
                                                 const gchar            *prop_name,
                                                 guint                   prop_state);
static void     ibus_zhuyin_engine_candidate_clicked
                                                (IBusEngine             *engine,
                                                 guint                   index,
                                                 guint                   button,
                                                 guint                   state);
#if 0
static void ibus_zhuyin_engine_property_show    (IBusEngine             *engine,
                                                 const gchar            *prop_name);
static void ibus_zhuyin_engine_property_hide    (IBusEngine             *engine,
                                                 const gchar            *prop_name);
#endif

G_DEFINE_TYPE (IBusZhuyinEngine, ibus_zhuyin_engine, IBUS_TYPE_ENGINE)

static void
ibus_zhuyin_engine_class_init (IBusZhuyinEngineClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS (klass);
    IBusEngineClass *engine_class = IBUS_ENGINE_CLASS (klass);

    object_class->constructor = ibus_zhuyin_engine_constructor;
    ibus_object_class->destroy = (IBusObjectDestroyFunc) ibus_zhuyin_engine_destroy;

    engine_class->process_key_event = ibus_zhuyin_engine_process_key_event;

    engine_class->reset = ibus_zhuyin_engine_reset;
    engine_class->enable = ibus_zhuyin_engine_enable;
    engine_class->disable = ibus_zhuyin_engine_disable;

    engine_class->focus_in = ibus_zhuyin_engine_focus_in;
    engine_class->focus_out = ibus_zhuyin_engine_focus_out;

#if IBUS_CHECK_VERSION (1, 5, 4)
    engine_class->set_content_type = ibus_zhuyin_engine_set_content_type;
#endif

    engine_class->page_up = ibus_zhuyin_engine_page_up;
    engine_class->page_down = ibus_zhuyin_engine_page_down;

    engine_class->cursor_up = ibus_zhuyin_engine_cursor_up;
    engine_class->cursor_down = ibus_zhuyin_engine_cursor_down;

    engine_class->property_activate = ibus_zhuyin_engine_property_activate;

    engine_class->candidate_clicked = ibus_zhuyin_engine_candidate_clicked;
}

static void
ibus_zhuyin_engine_init (IBusZhuyinEngine *zhuyin)
{
    if (g_object_is_floating (zhuyin))
        g_object_ref_sink (zhuyin);  // make engine sink
}

static GObject*
ibus_zhuyin_engine_constructor (GType                  type,
                                guint                  n_construct_params,
                                GObjectConstructParam *construct_params)
{
    IBusZhuyinEngine *engine;
    const gchar *name;

    engine = (IBusZhuyinEngine *) G_OBJECT_CLASS (ibus_zhuyin_engine_parent_class)->constructor (
                                                           type,
                                                           n_construct_params,
                                                           construct_params);
    name = ibus_engine_get_name ((IBusEngine *) engine);

    if (name) {
        if (std::strcmp (name, "libzhuyin") == 0 ||
            std::strcmp (name, "libzhuyin-debug") == 0) {
            engine->engine = new ZhuyinEngine (IBUS_ENGINE (engine));
        }
    } else {
        engine->engine = new ZhuyinEngine (IBUS_ENGINE (engine));
    }
    return (GObject *) engine;
}

static void
ibus_zhuyin_engine_destroy (IBusZhuyinEngine *zhuyin)
{
    delete zhuyin->engine;
    ((IBusObjectClass *) ibus_zhuyin_engine_parent_class)->destroy ((IBusObject *)zhuyin);
}

static gboolean
ibus_zhuyin_engine_process_key_event (IBusEngine     *engine,
                                      guint           keyval,
                                      guint           keycode,
                                      guint           modifiers)
{
    IBusZhuyinEngine *zhuyin = (IBusZhuyinEngine *) engine;
    return zhuyin->engine->processKeyEvent (keyval, keycode, modifiers);
}

#if IBUS_CHECK_VERSION (1, 5, 4)
static void
ibus_zhuyin_engine_set_content_type (IBusEngine     *engine,
                                     guint purpose,
                                     guint hints)
{
    IBusZhuyinEngine *zhuyin = (IBusZhuyinEngine *) engine;
    return zhuyin->engine->setContentType (purpose, hints);
}
#endif

static void
ibus_zhuyin_engine_property_activate (IBusEngine    *engine,
                                      const gchar   *prop_name,
                                      guint          prop_state)
{
    IBusZhuyinEngine *zhuyin = (IBusZhuyinEngine *) engine;
    zhuyin->engine->propertyActivate (prop_name, prop_state);
}
static void
ibus_zhuyin_engine_candidate_clicked (IBusEngine *engine,
                                      guint       index,
                                      guint       button,
                                      guint       state)
{
    IBusZhuyinEngine *zhuyin = (IBusZhuyinEngine *) engine;
    zhuyin->engine->candidateClicked (index, button, state);
}

#define FUNCTION(name, Name)                                        \
    static void                                                     \
    ibus_zhuyin_engine_##name (IBusEngine *engine)                  \
    {                                                               \
        IBusZhuyinEngine *zhuyin = (IBusZhuyinEngine *) engine;     \
        zhuyin->engine->Name ();                                    \
        ((IBusEngineClass *) ibus_zhuyin_engine_parent_class)       \
            ->name (engine);                                        \
    }
FUNCTION(focus_in,    focusIn)
FUNCTION(focus_out,   focusOut)
FUNCTION(reset,       reset)
FUNCTION(enable,      enable)
FUNCTION(disable,     disable)
FUNCTION(page_up,     pageUp)
FUNCTION(page_down,   pageDown)
FUNCTION(cursor_up,   cursorUp)
FUNCTION(cursor_down, cursorDown)
#undef FUNCTION

Engine::Engine (IBusEngine *engine) : m_engine (engine)
{
#if IBUS_CHECK_VERSION (1, 5, 4)
    m_input_purpose = IBUS_INPUT_PURPOSE_FREE_FORM;
#endif
}

gboolean
Engine::contentIsPassword()
{
#if IBUS_CHECK_VERSION (1, 5, 4)
   return IBUS_INPUT_PURPOSE_PASSWORD == m_input_purpose;
#else
   return FALSE;
#endif
}

void
Engine::focusOut (void)
{
#if IBUS_CHECK_VERSION (1, 5, 4)
    m_input_purpose = IBUS_INPUT_PURPOSE_FREE_FORM;
#endif
}

#if IBUS_CHECK_VERSION(1, 5, 4)
void
Engine::setContentType (guint purpose, guint hints)
{
    m_input_purpose = (IBusInputPurpose) purpose;
}
#endif

Engine::~Engine (void)
{
}

};

