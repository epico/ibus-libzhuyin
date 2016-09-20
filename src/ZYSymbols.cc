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

#include "ZYSymbols.h"
#include <assert.h>

namespace ZY {

/* The following tables are copied from libchewing code. */

static const char G_EASY_SYMBOL_KEY[] = {
#if 0
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
#endif
    /* only capital letters are allowed here. */
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
	'U', 'V', 'W', 'X', 'Y', 'Z'
};

bool
is_easy_symbol(const gint key)
{
    for (size_t i = 0; i < G_N_ELEMENTS (G_EASY_SYMBOL_KEY); ++i) {
        if (key == G_EASY_SYMBOL_KEY[i])
            return true;
    }
    return false;
}

static const char * const symbol_buf[][ 50 ] = {
	{ "0", "\xC3\xB8", 0 },
		/* "ø" */
	{ "[", "\xE3\x80\x8C", "\xE3\x80\x8E", "\xE3\x80\x8A", "\xE3\x80\x88",
		  "\xE3\x80\x90", "\xE3\x80\x94", 0 },
		/* "「", "『", "《", "〈", "【", "〔" */
	{ "]", "\xE3\x80\x8D", "\xE3\x80\x8F", "\xE3\x80\x8B", "\xE3\x80\x89",
		  "\xE3\x80\x91", "\xE3\x80\x95", 0 },
		/* "」", "』", "》", "〉", "】", "〕" */
	{ "{", "\xEF\xBD\x9B", 0 },
		/* "｛" */
	{ "}", "\xEF\xBD\x9D", 0 },
		/* "｝" */
	{ "<", "\xEF\xBC\x8C", "\xE2\x86\x90", 0 },
		/* "，", "←" */
	{ ">", "\xE3\x80\x82", "\xE2\x86\x92", "\xEF\xBC\x8E", 0 },
		/* "。", "→", "．" */
	{ "?", "\xEF\xBC\x9F", "\xC2\xBF", 0 },
		/* "？", "¿" */
	{ "!", "\xEF\xBC\x81", "\xE2\x85\xA0","\xC2\xA1", 0 },
		/* "！", "Ⅰ","¡" */
	{ "@", "\xEF\xBC\xA0", "\xE2\x85\xA1", "\xE2\x8A\x95", "\xE2\x8A\x99",
		  "\xE3\x8A\xA3", "\xEF\xB9\xAB", 0 },
		/* "＠", "Ⅱ", "⊕", "⊙", "㊣", "﹫" */
	{ "#", "\xEF\xBC\x83", "\xE2\x85\xA2", "\xEF\xB9\x9F", 0 },
		/* "＃", "Ⅲ", "﹟" */
	{ "$", "\xEF\xBC\x84", "\xE2\x85\xA3", "\xE2\x82\xAC", "\xEF\xB9\xA9",
		  "\xEF\xBF\xA0", "\xE2\x88\xAE","\xEF\xBF\xA1", "\xEF\xBF\xA5", 0 },
		/* "＄", "Ⅳ", "€", "﹩", "￠", "∮","￡", "￥" */
	{ "%", "\xEF\xBC\x85", "\xE2\x85\xA4", 0 },
		/* "％", "Ⅴ" */
	{ "^", "\xEF\xB8\xBF", "\xE2\x85\xA5", "\xEF\xB9\x80", "\xEF\xB8\xBD",
		  "\xEF\xB8\xBE", 0 },
		/* "︿", "Ⅵ", "﹀", "︽", "︾" */
	{ "&", "\xEF\xBC\x86", "\xE2\x85\xA6", "\xEF\xB9\xA0", 0 },
		/* "＆", "Ⅶ", "﹠" */
	{ "*", "\xEF\xBC\x8A", "\xE2\x85\xA7", "\xC3\x97", "\xE2\x80\xBB",
		  "\xE2\x95\xB3", "\xEF\xB9\xA1", "\xE2\x98\xAF", "\xE2\x98\x86",
		  "\xE2\x98\x85", 0 },
		/* "＊", "Ⅷ", "×", "※", "╳", "﹡", "☯", "☆", "★" */
	{ "(", "\xEF\xBC\x88", "\xE2\x85\xA8", 0 },
		/* "（", "Ⅸ" */
	{ ")", "\xEF\xBC\x89", "\xE2\x85\xA9", 0 },
		/* "）", "Ⅹ" */
	{ "_", "\xEF\xBC\xBF", "\xE2\x80\xA6", "\xE2\x80\xA5", "\xE2\x86\x90",
		  "\xE2\x86\x92", "\xEF\xB9\x8D", "\xEF\xB9\x89", "\xCB\x8D",
		  "\xEF\xBF\xA3", "\xE2\x80\x93", "\xE2\x80\x94", "\xC2\xAF",
		  "\xEF\xB9\x8A", "\xEF\xB9\x8E", "\xEF\xB9\x8F", "\xEF\xB9\xA3",
		  "\xEF\xBC\x8D", 0 },
		/* "＿", "…", "‥", "←", "→", "﹍", "﹉", "ˍ", "￣"
		 * "–", "—", "¯", "﹊", "﹎", "﹏", "﹣", "－" */
	{ "+", "\xEF\xBC\x8B", "\xC2\xB1", "\xEF\xB9\xA2", 0 },
		/* "＋", "±", "﹢" */
	{ "=", "\xEF\xBC\x9D", "\xE2\x89\x92", "\xE2\x89\xA0", "\xE2\x89\xA1",
		  "\xE2\x89\xA6", "\xE2\x89\xA7", "\xEF\xB9\xA6", 0 },
		/* "＝", "≒", "≠", "≡", "≦", "≧", "﹦" */
	{ "`", "\xE3\x80\x8F", "\xE3\x80\x8E", "\xE2\x80\xB2", "\xE2\x80\xB5", 0 },
		/* "』", "『", "′", "‵" */
	{ "~", "\xEF\xBD\x9E", 0 },
		/* "～" */
	{ ":", "\xEF\xBC\x9A", "\xEF\xBC\x9B", "\xEF\xB8\xB0", "\xEF\xB9\x95", 0 },
		/* "：", "；", "︰", "﹕" */
	{ "\"", "\xEF\xBC\x9B", 0 },
		/* "；" */
	{ "\'", "\xE3\x80\x81", "\xE2\x80\xA6", "\xE2\x80\xA5", 0 },
		/* "、", "…", "‥" */
	{ "\\", "\xEF\xBC\xBC", "\xE2\x86\x96", "\xE2\x86\x98", "\xEF\xB9\xA8", 0 },
		/* "＼", "↖", "↘", "﹨" */
	{ "-", "\xEF\xBC\x8D", "\xEF\xBC\xBF", "\xEF\xBF\xA3", "\xC2\xAF",
		  "\xCB\x8D", "\xE2\x80\x93", "\xE2\x80\x94", "\xE2\x80\xA5",
		  "\xE2\x80\xA6", "\xE2\x86\x90", "\xE2\x86\x92", "\xE2\x95\xB4",
		  "\xEF\xB9\x89", "\xEF\xB9\x8A", "\xEF\xB9\x8D", "\xEF\xB9\x8E",
		  "\xEF\xB9\x8F", "\xEF\xB9\xA3", 0 },
		/* "－", "＿", "￣", "¯", "ˍ", "–", "—", "‥", "…"
		 * "←", "→", "╴", "﹉", "﹊", "﹍", "﹎", "﹏", "﹣" */
	{ "/", "\xEF\xBC\x8F", "\xC3\xB7", "\xE2\x86\x97", "\xE2\x86\x99",
		  "\xE2\x88\x95", 0 },
		/* "／","÷","↗","↙","∕" */
	{ "|", "\xE2\x86\x91", "\xE2\x86\x93", "\xE2\x88\xA3", "\xE2\x88\xA5",
		  "\xEF\xB8\xB1", "\xEF\xB8\xB3", "\xEF\xB8\xB4" ,0 },
		/* "↑", "↓", "∣", "∥", "︱", "︳", "︴" */
	{ "A", "\xC3\x85","\xCE\x91", "\xCE\xB1", "\xE2\x94\x9C", "\xE2\x95\xA0",
		  "\xE2\x95\x9F", "\xE2\x95\x9E", 0 },
		/* "Å","Α", "α", "├", "╠", "╟", "╞" */
	{ "B", "\xCE\x92", "\xCE\xB2","\xE2\x88\xB5", 0 },
		/* "Β", "β","∵" */
	{ "C", "\xCE\xA7", "\xCF\x87", "\xE2\x94\x98", "\xE2\x95\xAF",
		  "\xE2\x95\x9D", "\xE2\x95\x9C", "\xE2\x95\x9B", "\xE3\x8F\x84",
		  "\xE2\x84\x83", "\xE3\x8E\x9D", "\xE2\x99\xA3", "\xC2\xA9", 0 },
		/* "Χ", "χ", "┘", "╯", "╝", "╜", "╛"
		 * "㏄", "℃", "㎝", "♣", "©" */
	{ "D", "\xCE\x94", "\xCE\xB4", "\xE2\x97\x87", "\xE2\x97\x86",
		  "\xE2\x94\xA4", "\xE2\x95\xA3", "\xE2\x95\xA2", "\xE2\x95\xA1",
		  "\xE2\x99\xA6", 0 },
		/* "Δ", "δ", "◇", "◆", "┤", "╣", "╢", "╡","♦" */
	{ "E", "\xCE\x95", "\xCE\xB5", "\xE2\x94\x90", "\xE2\x95\xAE",
		  "\xE2\x95\x97", "\xE2\x95\x93", "\xE2\x95\x95", 0 },
		/* "Ε", "ε", "┐", "╮", "╗", "╓", "╕" */
	{ "F", "\xCE\xA6", "\xCF\x88", "\xE2\x94\x82", "\xE2\x95\x91",
		  "\xE2\x99\x80", 0 },
		/* "Φ", "ψ", "│", "║", "♀" */
	{ "G", "\xCE\x93", "\xCE\xB3", 0 },
		/* "Γ", "γ" */
	{ "H", "\xCE\x97", "\xCE\xB7","\xE2\x99\xA5", 0 },
		/* "Η", "η","♥" */
	{ "I", "\xCE\x99", "\xCE\xB9", 0 },
		/* "Ι", "ι" */
	{ "J", "\xCF\x86", 0 },
		/* "φ" */
	{ "K", "\xCE\x9A", "\xCE\xBA","\xE3\x8E\x9E", "\xE3\x8F\x8E", 0 },
		/* "Κ", "κ","㎞", "㏎" */
	{ "L", "\xCE\x9B", "\xCE\xBB","\xE3\x8F\x92", "\xE3\x8F\x91", 0 },
		/* "Λ", "λ","㏒", "㏑" */
	{ "M", "\xCE\x9C", "\xCE\xBC", "\xE2\x99\x82", "\xE2\x84\x93",
		  "\xE3\x8E\x8E", "\xE3\x8F\x95", "\xE3\x8E\x9C","\xE3\x8E\xA1", 0 },
		/* "Μ", "μ", "♂", "ℓ", "㎎", "㏕", "㎜","㎡" */
	{ "N", "\xCE\x9D", "\xCE\xBD","\xE2\x84\x96", 0 },
		/* "Ν", "ν","№" */
	{ "O", "\xCE\x9F", "\xCE\xBF", 0 },
		/* "Ο", "ο" */
	{ "P", "\xCE\xA0", "\xCF\x80", 0 },
		/* "Π", "π" */
	{ "Q", "\xCE\x98", "\xCE\xB8","\xD0\x94","\xE2\x94\x8C", "\xE2\x95\xAD",
		  "\xE2\x95\x94", "\xE2\x95\x93", "\xE2\x95\x92", 0 },
		/* "Θ", "θ","Д","┌", "╭", "╔", "╓", "╒" */
	{ "R", "\xCE\xA1", "\xCF\x81", "\xE2\x94\x80", "\xE2\x95\x90" ,"\xC2\xAE" , 0 },
		/* "Ρ", "ρ", "─", "═" ,"®" */
	{ "S", "\xCE\xA3", "\xCF\x83", "\xE2\x88\xB4", "\xE2\x96\xA1",
		  "\xE2\x96\xA0", "\xE2\x94\xBC", "\xE2\x95\xAC", "\xE2\x95\xAA",
		  "\xE2\x95\xAB", "\xE2\x88\xAB", "\xC2\xA7", "\xE2\x99\xA0", 0 },
		/* "Σ", "σ", "∴", "□", "■", "┼", "╬", "╪", "╫"
		 * "∫", "§", "♠" */
	{ "T", "\xCE\xA4", "\xCF\x84", "\xCE\xB8", "\xE2\x96\xB3", "\xE2\x96\xB2",
		  "\xE2\x96\xBD", "\xE2\x96\xBC", "\xE2\x84\xA2", "\xE2\x8A\xBF",
		  "\xE2\x84\xA2", 0 },
		/* "Τ", "τ","θ","△","▲","▽","▼","™","⊿", "™" */
	{ "U", "\xCE\xA5", "\xCF\x85","\xCE\xBC","\xE2\x88\xAA", "\xE2\x88\xA9", 0 },
		/* "Υ", "υ","μ","∪", "∩" */
	{ "V", "\xCE\xBD", 0 },
	{ "W", "\xE2\x84\xA6", "\xCF\x89", "\xE2\x94\xAC", "\xE2\x95\xA6",
		  "\xE2\x95\xA4", "\xE2\x95\xA5", 0 },
		/* "Ω", "ω", "┬", "╦", "╤", "╥" */
	{ "X", "\xCE\x9E", "\xCE\xBE", "\xE2\x94\xB4", "\xE2\x95\xA9",
		  "\xE2\x95\xA7", "\xE2\x95\xA8", 0 },
		/* "Ξ", "ξ", "┴", "╩", "╧", "╨" */
	{ "Y", "\xCE\xA8", 0 },
		/* "Ψ" */
	{ "Z", "\xCE\x96", "\xCE\xB6", "\xE2\x94\x94", "\xE2\x95\xB0",
		  "\xE2\x95\x9A", "\xE2\x95\x99", "\xE2\x95\x98", 0 },
		/* "Ζ", "ζ", "└", "╰", "╚", "╙", "╘" */
};

int
find_lookup_key (const String & symbol)
{
    for (size_t i = 0; i < G_N_ELEMENTS (symbol_buf); ++i) {
        for (const char * const * buf = symbol_buf[i];
             *buf; ++buf) {
            if (symbol == *buf)
                return *symbol_buf[i][0];
        }
    }
    return 0;
}

int
get_choice_list (const gint key, gchar ** & choices)
{
    choices = NULL;
    GPtrArray * array = g_ptr_array_new ();

    for (size_t i = 0; i < G_N_ELEMENTS (symbol_buf); ++i) {
        const char * const * buf = symbol_buf[i];
        if (key == *buf[0]) {
            /* skip lookup char. */
            for (++buf; *buf; ++buf) {
                g_ptr_array_add (array, g_strdup (*buf));
            }
        }
    }

    const guint len = array->len;

    if (array->len) {
        g_ptr_array_add (array, NULL);
        /* must be freed by g_strfreev. */
        choices = (gchar **) g_ptr_array_free (array, FALSE);
        return len;
    }

    g_ptr_array_free (array, TRUE);
    return len;
}

bool
is_special_symbol (const gint key)
{
    String symbol;
    return convert_special_symbol (key, symbol);
}

bool
convert_special_symbol (const gint key, String & symbol)
{
    symbol = "";

    static const char keybuf[] = {
        '[', ']', '{', '}', '\'','<', ':', '\"', '>',
        '~', '!', '@', '#', '$', '%', '^', '&', '*',
        '(', ')', '_', '+', '=','\\', '|', '?',
        ',', '.', ';',
        ' ', '\"', '\'', '/', '<', '>', '`', '[',
        ']', '{', '}', '+',  '-'
    };

    static const char * const chibuf[] = {
        "\xE3\x80\x8C", "\xE3\x80\x8D", "\xE3\x80\x8E", "\xE3\x80\x8F",
        /* "「", "」", "『", "』" */
        "\xE3\x80\x81", "\xEF\xBC\x8C", "\xEF\xBC\x9A", "\xEF\xBC\x9B",
        /* "、", "，", "：", "；" */
        "\xE3\x80\x82", "\xEF\xBD\x9E", "\xEF\xBC\x81", "\xEF\xBC\xA0",
        /* "。", "～", "！", "＠" */
        "\xEF\xBC\x83", "\xEF\xBC\x84", "\xEF\xBC\x85", "\xEF\xB8\xBF",
        /* "＃", "＄", "％", "︿" */
        "\xEF\xBC\x86", "\xEF\xBC\x8A", "\xEF\xBC\x88", "\xEF\xBC\x89",
        /* "＆", "＊", "（", "）" */
        "\xEF\xB9\x8D", "\xEF\xBC\x8B", "\xEF\xBC\x9D", "\xEF\xBC\xBC",
        /* "﹍", "＋", "＝", "＼" */
        "\xEF\xBD\x9C", "\xEF\xBC\x9F", "\xEF\xBC\x8C", "\xE3\x80\x82",
        /* "｜", "？", "，", "。" */
        "\xEF\xBC\x9B",
        /* "；" */
        "\xE3\x80\x80","\xE2\x80\x9D",
        /* "　","”" */
        "\xE2\x80\x99","\xEF\xBC\x8F","\xEF\xBC\x9C","\xEF\xBC\x9E",
        /* "’","／","＜","＞" */
        "\xE2\x80\xB5","\xE3\x80\x94","\xE3\x80\x95","\xEF\xBD\x9B",
        /* "‵","〔""〕","｛" */
        "\xEF\xBD\x9D","\xEF\xBC\x8B","\xEF\xBC\x8D"
        /* "｝","＋","－" */
    };

    assert(G_N_ELEMENTS (keybuf) == G_N_ELEMENTS (chibuf));
    for (size_t i = 0; i < G_N_ELEMENTS (keybuf); ++i) {
        if (key == keybuf[i]) {
            symbol = chibuf[i];
            return true;
        }
    }

    return false;
}

bool
is_full_width_symbol (const gint key)
{
    String symbol;
    return convert_full_width_symbol (key, symbol);
}

bool
convert_full_width_symbol (const gint key, String & symbol)
{
    symbol = "";

    static char keybuf[] = {
        '0', '1', '2', '3',  '4',  '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd',  'e',  'f', 'g', 'h', 'i', 'j',
        'k', 'l', 'm', 'n',  'o',  'p', 'q', 'r', 's', 't',
        'u', 'v', 'w', 'x',  'y',  'z', 'A', 'B', 'C', 'D',
        'E', 'F', 'G', 'H',  'I',  'J', 'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R',  'S',  'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', ' ', '\"', '\'', '/', '<', '>', '`', '[',
        ']', '{', '}', '+',  '-'
    };

    static const char *chibuf[] = {
        "\xEF\xBC\x90","\xEF\xBC\x91","\xEF\xBC\x92","\xEF\xBC\x93",
        /* "０","１","２","３" */
        "\xEF\xBC\x94","\xEF\xBC\x95","\xEF\xBC\x96","\xEF\xBC\x97",
        /* "４","５","６","７" */
        "\xEF\xBC\x98","\xEF\xBC\x99","\xEF\xBD\x81","\xEF\xBD\x82",
        /* "８","９","ａ","ｂ" */
        "\xEF\xBD\x83","\xEF\xBD\x84","\xEF\xBD\x85","\xEF\xBD\x86",
        /* "ｃ","ｄ","ｅ","ｆ" */
        "\xEF\xBD\x87","\xEF\xBD\x88","\xEF\xBD\x89","\xEF\xBD\x8A",
        /* "ｇ","ｈ","ｉ","ｊ" */
        "\xEF\xBD\x8B","\xEF\xBD\x8C","\xEF\xBD\x8D","\xEF\xBD\x8E",
        /* "ｋ","ｌ","ｍ","ｎ" */
        "\xEF\xBD\x8F","\xEF\xBD\x90","\xEF\xBD\x91","\xEF\xBD\x92",
        /* "ｏ","ｐ","ｑ","ｒ" */
        "\xEF\xBD\x93","\xEF\xBD\x94","\xEF\xBD\x95","\xEF\xBD\x96",
        /* "ｓ","ｔ","ｕ","ｖ" */
        "\xEF\xBD\x97","\xEF\xBD\x98","\xEF\xBD\x99","\xEF\xBD\x9A",
        /* "ｗ","ｘ","ｙ","ｚ" */
        "\xEF\xBC\xA1","\xEF\xBC\xA2","\xEF\xBC\xA3","\xEF\xBC\xA4",
        /* "Ａ","Ｂ","Ｃ","Ｄ" */
        "\xEF\xBC\xA5","\xEF\xBC\xA6","\xEF\xBC\xA7","\xEF\xBC\xA8",
        /* "Ｅ","Ｆ","Ｇ","Ｈ" */
        "\xEF\xBC\xA9","\xEF\xBC\xAA","\xEF\xBC\xAB","\xEF\xBC\xAC",
        /* "Ｉ","Ｊ","Ｋ","Ｌ" */
        "\xEF\xBC\xAD","\xEF\xBC\xAE","\xEF\xBC\xAF","\xEF\xBC\xB0",
        /* "Ｍ","Ｎ","Ｏ","Ｐ" */
        "\xEF\xBC\xB1","\xEF\xBC\xB2","\xEF\xBC\xB3","\xEF\xBC\xB4",
        /* "Ｑ","Ｒ","Ｓ","Ｔ" */
        "\xEF\xBC\xB5","\xEF\xBC\xB6","\xEF\xBC\xB7","\xEF\xBC\xB8",
        /* "Ｕ","Ｖ","Ｗ","Ｘ" */
        "\xEF\xBC\xB9","\xEF\xBC\xBA","\xE3\x80\x80","\xE2\x80\x9D",
        /* "Ｙ","Ｚ","　","”" */
        "\xE2\x80\x99","\xEF\xBC\x8F","\xEF\xBC\x9C","\xEF\xBC\x9E",
        /* "’","／","＜","＞" */
        "\xE2\x80\xB5","\xE3\x80\x94","\xE3\x80\x95","\xEF\xBD\x9B",
        /* "‵","〔""〕","｛" */
        "\xEF\xBD\x9D","\xEF\xBC\x8B","\xEF\xBC\x8D"
        /* "｝","＋","－" */
    };

    assert(G_N_ELEMENTS (keybuf) == G_N_ELEMENTS (chibuf));
    for (size_t i = 0; i < G_N_ELEMENTS (keybuf); ++i) {
        if (key == keybuf[i]) {
            symbol = chibuf[i];
            return true;
        }
    }

    return convert_special_symbol (key, symbol);
}

};
