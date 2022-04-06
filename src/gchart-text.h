/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * gchart-text.h
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

#ifndef __G_CHART_TEXT_H__
#define __G_CHART_TEXT_H__

#include <features.h>
#include <cairo.h>

#include "gchart-internal.h"

typedef enum _text_mode text_mode_t;

enum _text_mode
{
	MIDDLE_TOP,
	MIDDLE_BOTTOM,
	MIDDLE_MIDDLE,
	LEFT_TOP,
	LEFT_MIDDLE,
	LEFT_BOTTOM,
	RIGHT_TOP,
	RIGHT_MIDDLE,
	RIGHT_BOTTOM
};

void _gchart_print_text(cairo_t *cr, const char *text, const float x, const float y, const text_mode_t m, const float padding);
void _gchart_print_value_with_unit(cairo_t *cr, const float value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data, const float x, const float y, const text_mode_t m, const float padding);

#endif /* __G_CHART_TEXT_H__ */
