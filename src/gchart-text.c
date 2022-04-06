/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * g_chart.c
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

#include "gchart-text.h"

#include <features.h>
#include <math.h>
#include <cairo.h>

#include "gchart-internal.h"

void _gchart_update_offsets(cairo_t *cr, GchartPrivate *priv);
static gchar *_gchart_get_value_as_text(const float value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data);

void _gchart_update_offsets(cairo_t *cr, GchartPrivate *priv)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	/* Get expected text width for the top and left. This is based on the labels on the y-axis */
	cairo_text_extents_t extents, extents2;
	gchar *text;
	float info_box_width;

	text = _gchart_get_value_as_text(0.0, priv->y1_unit, priv->y1_info_cb, priv->user_data);
	cairo_text_extents(cr, text, &extents);
	g_free(text);
	text = NULL;
	extents.width += PADDING;
	extents.height += PADDING;
	priv->offset_left = extents.width + BORDER_OFFSET;
	priv->offset_top = extents.height / 2 + BORDER_OFFSET;
	info_box_width = extents.width;

	/* Get expected text width for the info box */
	cairo_text_extents(cr, priv->x_label, &extents);
	extents.width += PADDING;
	info_box_width = MAX(extents.width, info_box_width);

	cairo_text_extents(cr, priv->y1_label, &extents);
	extents.width += PADDING;
	info_box_width = MAX(extents.width, info_box_width);

	if(priv->enable_y2)
	{
		cairo_text_extents(cr, priv->y2_label, &extents);
		extents.width += PADDING;
		info_box_width = MAX(extents.width, info_box_width);
	}

	/* Get expected text width for bottom and right. This is based on the labels on the y-axis */
	text = _gchart_get_value_as_text(0.0, priv->x_unit, priv->x_info_cb, priv->user_data);
	cairo_text_extents(cr, text, &extents);
	g_free(text);
	text = NULL;
	if(priv->enable_y2)
	{
		text = _gchart_get_value_as_text(0.0, priv->y2_unit, priv->y2_info_cb, priv->user_data);
		cairo_text_extents(cr, text, &extents2);
		g_free(text);
		text = NULL;
		extents2.width += PADDING;
		info_box_width = MAX(extents2.width, info_box_width);
	}
	else
		extents2.width = 0;
	extents.width += PADDING;
	extents.height += PADDING;
	info_box_width = MAX(extents.width, info_box_width);
	priv->offset_bottom = extents.height + BORDER_OFFSET;
	priv->info_box_width = info_box_width + 10 * BORDER_OFFSET;
	priv->offset_right = extents.width / 2 + extents2.width + priv->info_box_width + BORDER_OFFSET;
}

/**
 * _gchart_print_text:
 * @cr: Cairo drawing area.
 * @text: The text to be printed.
 * @x: X coordinate of the drawing position.
 * @y: Y coordinate of the drawing position.
 * @m: Position of the text with respect to the x and y coordinate, must be one of the nine positions, @see text_mode_t.
 * @padding: Reserve this amount of padding around the text.
 *
 * Prints text on the drawing surface.
 * The text position is
 */
void _gchart_print_text(cairo_t *cr, const char *text, const float x, const float y, const text_mode_t m, const float padding)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	float x_new, y_new;
	cairo_text_extents_t extents;

	cairo_text_extents(cr, text, &extents);
	extents.width += 2 * padding;
	extents.height += 2 * padding;
	x_new = x;
	y_new = y;
	
	switch(m)
	{
		case MIDDLE_TOP:
			x_new -= extents.width / 2;
			y_new += extents.height;
			break;
		case MIDDLE_MIDDLE:
			x_new -= extents.width / 2;
			y_new += extents.height / 2;
			break;
		case MIDDLE_BOTTOM:
			x_new -= extents.width / 2;
			break;
		case LEFT_TOP:
			y_new += extents.height;
			break;
		case LEFT_MIDDLE:
			y_new += extents.height / 2;
			break;
		case LEFT_BOTTOM:
			break;
		case RIGHT_TOP:
			x_new -= extents.width;
			y_new += extents.height;
			break;
		case RIGHT_MIDDLE:
			x_new -= extents.width;
			y_new += extents.height / 2;
			break;
		default:
		case RIGHT_BOTTOM:
			x_new -= extents.width / 2;
			break;
	}

	cairo_move_to(cr, x_new + padding, y_new - padding);
	cairo_show_text(cr, text);
}

static gchar *_gchart_get_value_as_text(const float value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	gchar *text;
	GValue v = G_VALUE_INIT;


	if(info_cb != NULL && unit != NULL) /* unit and info_cb are given */
	{
		if(info_cb(value, &v, user_data))
			text = g_strdup_printf("%s %s", g_value_get_string(&v), unit);
	}
	else if(info_cb != NULL) /* only info_cb is given */
	{
		if(info_cb(value, &v, user_data))
			text = g_value_dup_string(&v);
	}
	else if(unit != NULL) /* only unit is given */
	{
		text = g_strdup_printf("%.3G %s", value, unit);
	}
	else /* none are given */
	{
		text = g_strdup_printf("%.3G", value);
	}
	g_value_unset(&v);

	return text;
}

void _gchart_print_value_with_unit(cairo_t *cr, const float value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data, const float x, const float y, const text_mode_t m, const float padding)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	gchar *text;

	if(isnan(value)) return;
	text = _gchart_get_value_as_text(value, unit, info_cb, user_data);
	_gchart_print_text(cr, text, x, y, m, padding);

	g_free(text);
	text = NULL;
}
