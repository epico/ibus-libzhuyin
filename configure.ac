# vim:set et ts=4:
#
# ibus-libzhuyin - New Zhuyin engine based on libzhuyin for IBus
#
# Copyright (c) 2014 Peng Wu <alexepico@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

# if not 1, append datestamp to the version number.
m4_define([ibus_released], [1])
m4_define([ibus_major_version], [1])
m4_define([ibus_minor_version], [10])
m4_define([ibus_micro_version], [3])
m4_define(ibus_maybe_datestamp,
    m4_esyscmd([if test x]ibus_released[ != x1; then date +.%Y%m%d | tr -d '\n\r'; fi]))

m4_define([ibus_version],
    ibus_major_version.ibus_minor_version.ibus_micro_version[]ibus_maybe_datestamp)

AC_INIT([ibus-libzhuyin], [ibus_version], [https://github.com/libzhuyin/ibus-libzhuyin/issues/new],[ibus-libzhuyin])
AM_INIT_AUTOMAKE([1.10])
AC_GNU_SOURCE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# define PACKAGE_VERSION_* variables
AS_VERSION
AS_NANO
AM_SANITY_CHECK
AM_MAINTAINER_MODE
AM_DISABLE_STATIC
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_ISC_POSIX
AC_HEADER_STDC
LT_INIT
AC_PROG_SED

# check ibus
PKG_CHECK_MODULES(IBUS, [
    ibus-1.0 >= 1.4.0
])

# check libzhuyin and database format
PKG_CHECK_MODULES(LIBZHUYIN, [
    libzhuyin >= 2.6.0
], [enable_libzhuyin=yes])
DATABASE_FORMAT=`$PKG_CONFIG --variable=database_format libzhuyin`
AC_SUBST(DATABASE_FORMAT)

# check if ibus_config_get_values, which is available in ibus-1.3.99+ (git master)
save_CFLAGS="$CFLAGS"
save_LIBS="$LIBS"
CFLAGS="$CFLAGS $IBUS_CFLAGS"
LIBS="$LIBS $IBUS_LIBS"
AC_CHECK_FUNCS([ibus_config_get_values])
CFLAGS="$save_CFLAGS"
LIBS="$save_LIBS"

# check env
AC_PATH_PROG(ENV, env)
AC_SUBST(ENV)

# check python
AC_ARG_WITH(python,
    AS_HELP_STRING([--with-python[=PATH]],
        [Select python2 or python3]),
    [PYTHON=$with_python], []
)
AM_PATH_PYTHON([2.5])

# --enable-boost
AC_ARG_ENABLE(boost,
    AC_HELP_STRING([--enable-boost],
        [Use boost to replace stdc++0x.]),
        [enable_boost=$enableval],
        [enable_boost=no]
)
if test x"$enable_boost" = x"yes"; then
    # check boost
    BOOST_REQUIRE([1.39])
    BOOST_FIND_HEADER([boost/bind.hpp])
    BOOST_FIND_HEADER([boost/signals2.hpp])
fi
AM_CONDITIONAL(HAVE_BOOST, test x"$enable_boost" = x"yes")

# --enable-opencc
AC_ARG_ENABLE(opencc,
    AC_HELP_STRING([--enable-opencc],
        [Use opencc for simplified and traditional Chinese conversion]),
        [enable_opencc=$enableval],
        [enable_opencc=no]
)
if test x"$enable_opencc" = x"yes"; then
    # check opencc
    PKG_CHECK_MODULES(OPENCC, [opencc >= 1.0.0], [
        AC_DEFINE(HAVE_OPENCC, 1, [Define if found opencc])
    ])
fi

# define GETTEXT_* variables
GETTEXT_PACKAGE=ibus-libzhuyin
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package])

AM_GNU_GETTEXT_VERSION([0.21])
AM_GNU_GETTEXT([external])

# Checks for GSettings.
PKG_CHECK_MODULES(GIO2, [gio-2.0 >= 2.26.0])

GLIB_GSETTINGS

# OUTPUT files
AC_CONFIG_FILES([ po/Makefile.in
Makefile
src/Makefile
src/libzhuyin.xml.in
setup/Makefile
setup/config.py
setup/ibus-setup-libzhuyin
ibus-libzhuyin.spec
data/Makefile
data/icons/Makefile
data/model/Makefile
data/model/table.conf
])

AC_OUTPUT

AC_MSG_RESULT([
Build options:
    Version                     $VERSION
    Install prefix              $prefix
    DBM                         $DATABASE_FORMAT
    Use boost                   $enable_boost
    Use opencc                  $enable_opencc
    Use libzhuyin               $enable_libzhuyin
])
