/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * gchart-internal.h
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

#ifndef __G_CHART_INTERNAL_H__
#define __G_CHART_INTERNAL_H__

#include <features.h>
#include <glib.h>

#include "gchart.h"

#define PADDING (5)
#define BORDER_OFFSET (PADDING)
#define DOT_RADIUS 2.0

#define UNUSED(x) (void)(x) ///< Used to indicate that a function does not use a certain parameter.

typedef struct _GchartPrivate GchartPrivate;

struct _GchartPrivate
{
	float x_scale, y1_scale, y2_scale, offset_left, offset_right, offset_top, offset_bottom, info_box_width;
	float x_min, x_max, y1_min, y1_max, y2_min, y2_max, x_info_value;
	gchar *title, *x_label, *y1_label, *y2_label, *x_unit, *y1_unit, *y2_unit;
	gboolean plot_lines, plot_dots, enable_y2;
	GchartValueToInfoString x_info_cb, y1_info_cb, y2_info_cb;
	gconstpointer user_data;

	/* callbacks for point plotting */
	GchartGetValue x_cb, y1_cb, y2_cb;

	/* callbacks for lineplotting */
	GchartRangeValue x_min_value_cb, x_max_value_cb, y1_min_value_cb, y1_max_value_cb, y2_min_value_cb, y2_max_value_cb;

	guint n_steps;
	float step_size;
	float zoom, x_center;
	GchartAction on_mouse_over_cb;
	GchartAction on_mouse_click_cb;
};

#endif /* __G_CHART_INTERNAL_H__ */
