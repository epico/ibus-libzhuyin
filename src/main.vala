/*
 * main.vala
 * Copyright (C) 2013 Peng Wu <alexepico@gmail.com>
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

using GLib;
using Gtk;
using IBus;

public class Main : Object 
{


	static int main (string[] args) 
	{
		IBus.init ();
		Gtk.init (ref args);

		var app = new Main ();

		Gtk.main ();
		
		return 0;
	}
}

