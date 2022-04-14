/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * gchart.c
 * Copyright (C) Martijn Goedhart 2022 <goedhart.martijn@gmail.com>
 *
 * This is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <features.h>

#include "gchart_glade.h"

#include "Gchart.hpp"

#include <gtkmm/main.h>

extern "C" void gcharts_glade_init (void);

void gcharts_register (void) {
	Gchart::register_type ();
}

extern "C" void gcharts_glade_init (void) {
	Gtk::Main::init_gtkmm_internals ();
	gcharts_register ();
}
