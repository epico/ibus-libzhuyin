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
#include "ZYZhuyinProperties.h"
#include <libintl.h>
#include "ZYText.h"
#include "ZYConfig.h"

namespace ZY {

#define _(text) (dgettext (GETTEXT_PACKAGE, text))
#define N_(text) text

ZhuyinProperties::ZhuyinProperties (Config & config)
    : m_config (config),
      m_mode_chinese (m_config.initChinese ()),
      m_mode_full_width (m_config.initFullWidth ()),
      m_mode_trad (m_config.initTradChinese ()),
      m_prop_chinese ("InputMode",
                PROP_TYPE_NORMAL,
                StaticText (m_mode_chinese ?
                            _("Chinese") :
                            _("English")),
                m_mode_chinese ?
                    PKGDATADIR"/icons/chinese.svg" :
                    PKGDATADIR"/icons/english.svg",
                StaticText (m_mode_chinese ?
                            _("Switch to English Mode") :
                            _("Switch to Chinese Mode"))),
      m_prop_full_width ("mode.full",
                PROP_TYPE_NORMAL,
                StaticText (m_mode_full_width ?
                            _("Full Width") :
                            _("Half Width")),
                m_mode_full_width ?
                    PKGDATADIR"/icons/full.svg" :
                    PKGDATADIR"/icons/half.svg",
                StaticText (m_mode_full_width ?
                            _("Switch to Half Width Mode"):
                            _("Switch to Full Width Mode"))),
      m_prop_trad ( "mode.trad",
                PROP_TYPE_NORMAL,
                StaticText (m_mode_trad ?
                            _("Traditional Chinese") :
                            _("Simplified Chinese")),
                m_mode_trad ?
                    PKGDATADIR"/icons/trad-chinese.svg" :
                    PKGDATADIR"/icons/simp-chinese.svg",
                StaticText (m_mode_trad ?
                            _("Switch to Simplfied Chinese Mode"):
                            _("Switch to Traditional Chinese Mode"))),
      m_prop_setup ("setup",
                PROP_TYPE_NORMAL,
                StaticText (_("Preferences")),
                "ibus-setup",
                StaticText (_("Preferences")))
{
    if (m_mode_chinese)
        m_prop_chinese.setSymbol(N_("中"));
    else
        m_prop_chinese.setSymbol(N_("英"));

    m_props.append (m_prop_chinese);
    m_props.append (m_prop_full_width);
    m_props.append (m_prop_trad);
    m_props.append (m_prop_setup);

}

void
ZhuyinProperties::toggleModeChinese (void)
{
    m_mode_chinese = ! m_mode_chinese;
    m_prop_chinese.setLabel (m_mode_chinese ?
                             _("Chinese") :
                             _("English"));

    if (m_mode_chinese)
        m_prop_chinese.setSymbol(N_("中"));
    else
        m_prop_chinese.setSymbol(N_("英"));

    m_prop_chinese.setIcon (m_mode_chinese ?
                            PKGDATADIR"/icons/chinese.svg" :
                            PKGDATADIR"/icons/english.svg");
    m_prop_chinese.setTooltip (m_mode_chinese ?
                               _("Switch to English Mode") :
                               _("Switch to Chinese Mode"));
    updateProperty (m_prop_chinese);
}

void
ZhuyinProperties::toggleModeFullWidth (void)
{
    m_mode_full_width = !m_mode_full_width;
    m_prop_full_width.setLabel (m_mode_full_width ?
                                  _("Full Width") :
                                  _("Half Width"));
    m_prop_full_width.setIcon (m_mode_full_width ?
                                 PKGDATADIR"/icons/full.svg" :
                                 PKGDATADIR"/icons/half.svg");
    m_prop_full_width.setTooltip (m_mode_full_width ?
                                 _("Switch to Half Width Mode"):
                                 _("Switch to Full Width Mode"));
    updateProperty (m_prop_full_width);
}

void
ZhuyinProperties::toggleModeTrad (void)
{
    m_mode_trad = ! m_mode_trad;
    m_prop_trad.setLabel (m_mode_trad ?
                          _("Traditional Chinese") :
                          _("Simplified Chinese"));
    m_prop_trad.setIcon (m_mode_trad ?
                            PKGDATADIR"/icons/trad-chinese.svg" :
                            PKGDATADIR"/icons/simp-chinese.svg");
    m_prop_trad.setTooltip(m_mode_trad ?
                           _("Switch to Simplfied Chinese Mode"):
                           _("Switch to Traditional Chinese Mode"));
    updateProperty (m_prop_trad);
}

void
ZhuyinProperties::reset (void)
{
    if (modeChinese () != m_config.initChinese ()) {
        toggleModeChinese ();
    }
    if (modeFullWidth () != m_config.initFullWidth ()) {
        toggleModeFullWidth ();
    }
    if (modeTrad () != m_config.initTradChinese ()) {
        toggleModeTrad ();
    }
}

gboolean
ZhuyinProperties::propertyActivate (const gchar *prop_name, guint prop_state) {
    const static std::string mode_chinese ("InputMode");
    const static std::string mode_full ("mode.full");
    const static std::string mode_trad ("mode.trad");

    if (mode_chinese == prop_name) {
        toggleModeChinese ();
        return TRUE;
    }
    else if (mode_full == prop_name) {
        toggleModeFullWidth ();
        return TRUE;
    }
    else if (mode_trad == prop_name) {
        toggleModeTrad ();
        return TRUE;
    }
    return FALSE;
}


};
