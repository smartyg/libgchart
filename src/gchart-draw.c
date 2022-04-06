/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * gchart-draw.c
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

#include "gchart-draw.h"

#include <features.h>
#include <math.h>
#include <glib.h>
#include <cairo.h>
#include <gtk/gtk.h>

#include "gchart-internal.h"
#include "gchart-text.h"

inline static void _gchart_draw_raster(cairo_t *cr, const GchartPrivate *priv, const guint width, const guint height);
inline static void _gchart_draw_sub_line(cairo_t *cr, const double x1, const double y1, const double x2, const double y2);
inline static void _gchart_draw_sub_line_vertical(cairo_t *cr, const double value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data, const double x1, const double y1, const double y2);
inline static void _gchart_draw_sub_line_horizontal(cairo_t *cr, const double value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data, const double x1, const double y1, const double x2);
inline static void _gchart_set_line_atributes(cairo_t *cr, const double width, const cairo_line_join_t line_join, const cairo_line_cap_t line_cap);

void _gchart_draw(GtkWidget *widget, cairo_t *cr, GchartPrivate *priv)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	float step_size, x_value, x_min_real, x_max_real;
	guint width, height;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	/* Get the real minimum and maximum values of the x axis. */
	if(priv->x_min_value_cb != NULL)
		x_min_real = priv->x_min_value_cb(priv->user_data);
	else
		x_min_real = 0;

	if(priv->x_max_value_cb != NULL)
		x_max_real = priv->x_max_value_cb(priv->user_data);
	else
	{
		x_min_real = 0;
		x_max_real = priv->step_size * priv->n_steps;
	}

	/* If zoom is bigger than 1.0 then adjust the minimum and maximum x value to it. */
	if(isfinite(priv->zoom) && priv->zoom > 1.0f)
	{
		float x_span = (x_max_real - x_min_real) / ( 2 * priv->zoom);
		/* If x_center is not set, set it to the middle of the real window. */
		if(!isfinite(priv->x_center))
		{
			priv->x_center = (x_max_real + x_min_real) /  2;
		}
		else
		{
			/* If x_center was set, check if it is within the minimum and maximum. */
			priv->x_center = MAX(x_min_real + x_span, priv->x_center);
			priv->x_center = MIN(x_max_real - x_span, priv->x_center);
		}
		priv->x_max = priv->x_center + x_span;
		priv->x_min = priv->x_center - x_span;
	}
	else
	{
		priv->zoom = 1;
		priv->x_max = x_max_real;
		priv->x_min = x_min_real;
		priv->x_center = (x_max_real + x_min_real) /  2;
	}

	/* Calculate the step size. */
	step_size = (priv->x_max - priv->x_min) / priv->n_steps;

	/* Get the minimum and maximum values for the Y1 axis */
	if((priv->y1_min_value_cb == NULL || priv->y1_max_value_cb == NULL) && priv->x_min != priv->x_max)
	{
		float f;
		for(f = priv->x_min; f <= priv->x_max; f += step_size)
		{
			float y;
			y = priv->y1_cb(f, priv->user_data);
			if(priv->y1_min_value_cb == NULL)
				priv->y1_min = MIN(priv->y1_min, y);
			if(priv->y1_max_value_cb == NULL)
				priv->y1_max = MAX(priv->y1_max, y);
		}
	}

	if(priv->y1_min_value_cb != NULL)
		priv->y1_min = priv->y1_min_value_cb(priv->user_data);

	if(priv->y1_max_value_cb != NULL)
		priv->y1_max = priv->y1_max_value_cb(priv->user_data);

	if(priv->enable_y2)
	{
		/* Get the minimum and maximum values for the Y2 axis */
		if((priv->y2_min_value_cb == NULL || priv->y2_max_value_cb == NULL) && priv->x_min != priv->x_max)
		{
			float f;
			for(f = priv->x_min; f <= priv->x_max; f += step_size)
			{
				float y;
				y = priv->y2_cb(f, priv->user_data);
				if(priv->y2_min_value_cb == NULL)
					priv->y2_min = MIN(priv->y2_min, y);
				if(priv->y2_max_value_cb == NULL)
					priv->y2_max = MAX(priv->y2_max, y);
			}
		}

		if(priv->y2_min_value_cb != NULL)
			priv->y2_min = priv->y2_min_value_cb(priv->user_data);

		if(priv->y2_max_value_cb != NULL)
			priv->y2_max = priv->y2_max_value_cb(priv->user_data);
	}

	priv->x_scale = (width - priv->offset_left - priv->offset_right) / (priv->x_max - priv->x_min);
	priv->y1_scale = (height - priv->offset_top - priv->offset_bottom) / (priv->y1_max - priv->y1_min);
	if(priv->enable_y2)
		priv->y2_scale = (height - priv->offset_top - priv->offset_bottom) / (priv->y2_max - priv->y2_min);

	_gchart_draw_raster(cr, priv, width, height);

	if(priv->x_min == priv->x_max) return;

	cairo_set_source_rgba(cr, 1, 0, 0, 1);
	cairo_set_dash(cr, NULL, 0, 0);
	_gchart_set_line_atributes(cr, 0.5, CAIRO_LINE_JOIN_ROUND, CAIRO_LINE_CAP_ROUND);

	for(x_value = priv->x_min; x_value <= priv->x_max; x_value += step_size)
	{
		double x = priv->offset_left + ((x_value - priv->x_min) * priv->x_scale);
		double y1 = height - priv->offset_bottom - ((priv->y1_cb(x_value, priv->user_data) - priv->y1_min) * priv->y1_scale);
		if(isnan(x) || isnan(y1)) continue;
		/* If this is the first point, starts with a move. */
		if(x_value == priv->x_min)
			cairo_move_to(cr, priv->offset_left, height - priv->offset_bottom);
		else
		{
			if(priv->plot_lines == TRUE)
			{
				cairo_line_to(cr, x, y1);
				cairo_stroke(cr);
			}
			else
				cairo_move_to(cr, x, y1);
			if(priv->plot_dots == TRUE)
			{
				cairo_arc(cr, x, y1, DOT_RADIUS, 0, 2 * M_PI);
				cairo_fill(cr);
				cairo_stroke(cr);
			}
		}
		cairo_move_to(cr, x, y1);
	}

	if(priv->enable_y2)
	{
		cairo_set_source_rgba(cr, 0, 0.6, 0, 1);

		for(x_value = priv->x_min; x_value <= priv->x_max; x_value += step_size)
		{
			double x = priv->offset_left + ((x_value - priv->x_min) * priv->x_scale);
			double y2 = height - priv->offset_bottom - ((priv->y2_cb(x_value, priv->user_data) - priv->y2_min) * priv->y2_scale);
			if(isnan(x) || isnan(y2)) continue;
			/* If this is the first point, starts with a move. */
			if(x_value == priv->x_min)
				cairo_move_to(cr, priv->offset_left, height - priv->offset_bottom);
			else
			{
				if(priv->plot_lines == TRUE)
				{
					cairo_line_to(cr, x, y2);
					cairo_stroke(cr);
				}
				else
					cairo_move_to(cr, x, y2);
				if(priv->plot_dots == TRUE)
				{
					cairo_arc(cr, x, y2, DOT_RADIUS, 0, 2 * M_PI);
					cairo_fill(cr);
					cairo_stroke(cr);
				}
			}
			cairo_move_to(cr, x, y2);
		}
	}

	return;
}

void _gchart_draw_info(GtkWidget *widget, cairo_t *cr, const GchartPrivate *priv)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	guint width, height;
	float h, offset, x_value, n;
	cairo_text_extents_t extents;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget) - 2 * BORDER_OFFSET;

	cairo_set_font_size(cr, 15);

	cairo_text_extents(cr, priv->y1_label, &extents);
	offset = extents.height / 2 + PADDING;

	n = 1.5;

	if(priv->enable_y2)
	{
		h = MAX(height / 3, 3 * PADDING + 2 * extents.height + 13);
		n = 2.5;

		/* Y2 info */
		cairo_set_source_rgba(cr, 0, 0, 0, 1);
		_gchart_print_text(cr, priv->y2_label, width - (priv->info_box_width / 2), 1.5 * h - offset, MIDDLE_BOTTOM, PADDING / 2);
		_gchart_print_value_with_unit(cr, priv->y2_cb(priv->x_info_value, priv->user_data), priv->y2_unit, priv->y2_info_cb, priv->user_data, width - (priv->info_box_width / 2), 1.5 * h + offset, MIDDLE_BOTTOM, PADDING / 2);
		cairo_set_source_rgba(cr, 0, 0.6, 0, 1);
		cairo_move_to(cr, width - (priv->info_box_width / 4) * 3, 1.5 * h + offset + 7);
		cairo_line_to(cr, width - (priv->info_box_width / 4), 1.5 * h + offset + 7);
		cairo_stroke(cr);
	}
	else
		h = MAX(height / 2, 3 * PADDING + 2 * extents.height + 13);

	/* Y1 info */
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	_gchart_print_text(cr, priv->y1_label, width - (priv->info_box_width / 2), 0.5 * h - offset, MIDDLE_BOTTOM, PADDING / 2);
	_gchart_print_value_with_unit(cr, priv->y1_cb(priv->x_info_value, priv->user_data), priv->y1_unit, priv->y1_info_cb, priv->user_data, width - (priv->info_box_width / 2), 0.5 * h + offset, MIDDLE_BOTTOM, PADDING / 2);
	cairo_set_source_rgba(cr, 1, 0, 0, 1);
	cairo_move_to(cr, width - (priv->info_box_width / 4) * 3, 0.5 * h + offset + 7);
	cairo_line_to(cr, width - (priv->info_box_width / 4), 0.5 * h + offset + 7);
	cairo_stroke(cr);

	/* X info */
	if(priv->x_cb != NULL)
		x_value = priv->x_cb(priv->x_info_value, priv->user_data);
	else
		x_value = priv->x_info_value;
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	_gchart_print_text(cr, priv->x_label, width - (priv->info_box_width / 2), n * h - offset, MIDDLE_BOTTOM, PADDING / 2);
	_gchart_print_value_with_unit(cr, x_value, priv->x_unit, priv->x_info_cb, priv->user_data, width - (priv->info_box_width / 2), n * h + offset, MIDDLE_BOTTOM, PADDING / 2);

	cairo_fill(cr);
}

void _gchart_draw_raster(cairo_t *cr, const GchartPrivate *priv, const guint width, const guint height)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	cairo_text_extents_t extents;
	int x_lines, y_lines, j;

	/* fill the background */
	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	/* x and y1 axis */
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, priv->offset_left, priv->offset_top);
	/* y1-axis */
	cairo_line_to(cr, priv->offset_left, height - priv->offset_bottom);
	/* x-axis */
	cairo_line_to(cr, width - priv->offset_right, height - priv->offset_bottom);
	if(priv->enable_y2)
	{
		/* y2-axis */
		cairo_line_to(cr, width - priv->offset_right, priv->offset_top);
	}

	_gchart_set_line_atributes(cr, 1, CAIRO_LINE_JOIN_MITER, CAIRO_LINE_CAP_BUTT);
	cairo_stroke(cr);

	cairo_text_extents(cr, "0", &extents);
	x_lines = (width - priv->offset_left - priv->offset_right) / (extents.width * 14);
	y_lines = (height - priv->offset_top - priv->offset_bottom) / (extents.height * 4);

	for(j = 1; j < x_lines; j++)
	{
		double x = j * ((width - priv->offset_left - priv->offset_right) / x_lines) + priv->offset_left;
		double value = j * ((priv->x_max - priv->x_min) / x_lines) + priv->x_min;
		_gchart_draw_sub_line_vertical(cr, value, priv->x_unit, priv->x_info_cb, priv->user_data, x, height - priv->offset_bottom, priv->offset_top);
	}

	for(j = 1; j < y_lines; j++)
	{
		double y = height - priv->offset_bottom - j * ((height - priv->offset_top - priv->offset_bottom) / y_lines);
		double value = j * ((priv->y1_max - priv->y1_min) / y_lines) + priv->y1_min;
		_gchart_draw_sub_line_horizontal(cr, value, priv->y1_unit, priv->y1_info_cb, priv->user_data, priv->offset_left, y, width - priv->offset_right);
	}

	/* draw the first and last labels on the X axis */
	_gchart_print_value_with_unit(cr, priv->x_min, priv->x_unit, priv->x_info_cb, priv->user_data, priv->offset_left, height - priv->offset_bottom, MIDDLE_TOP, 5);
	_gchart_print_value_with_unit(cr, priv->x_max, priv->x_unit, priv->x_info_cb, priv->user_data, width - priv->offset_right, height - priv->offset_bottom, MIDDLE_TOP, 5);

	/* draw the first and last labels on the Y1 axis */
	_gchart_print_value_with_unit(cr, priv->y1_min, priv->y1_unit, priv->y1_info_cb, priv->user_data, priv->offset_left, height - priv->offset_bottom, RIGHT_MIDDLE, 5);
	_gchart_print_value_with_unit(cr, priv->y1_max, priv->y1_unit, priv->y1_info_cb, priv->user_data, priv->offset_left, priv->offset_top, RIGHT_MIDDLE, 5);

	if(priv->enable_y2)
	{
		/* draw the first and last labels on the Y2 axis */
		_gchart_print_value_with_unit(cr, priv->y1_max, priv->y2_unit, priv->y2_info_cb, priv->user_data, width - priv->offset_right, height - priv->offset_bottom, LEFT_MIDDLE, 5);
		_gchart_print_value_with_unit(cr, priv->y2_max, priv->y2_unit, priv->y2_info_cb, priv->user_data, width - priv->offset_right, priv->offset_top, LEFT_MIDDLE, 5);
		for(j = 1; j < y_lines; j++)
		{
			double y = height - priv->offset_bottom - j * ((height - priv->offset_top - priv->offset_bottom) / y_lines);
			double value = j * ((priv->y2_max - priv->y2_min) / y_lines) + priv->y2_min;
			_gchart_print_value_with_unit(cr, value, priv->y2_unit, priv->y2_info_cb, priv->user_data, width - priv->offset_right, y, LEFT_MIDDLE, 5);
		}
	}

	cairo_set_source_rgba(cr, 1, 0, 0, 0.2);
	cairo_fill(cr);
}

inline static void _gchart_draw_sub_line(cairo_t *cr, const double x1, const double y1, const double x2, const double y2)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	const double dashes[] = {6.0};
	cairo_set_source_rgba(cr, 0, 0, 0, 0.8);
	_gchart_set_line_atributes(cr, 0.25, CAIRO_LINE_JOIN_MITER, CAIRO_LINE_CAP_BUTT);
	cairo_set_dash(cr, dashes, 1, 0);
	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);
}

inline static void _gchart_draw_sub_line_vertical(cairo_t *cr, const double value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data, const double x1, const double y1, const double y2)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	_gchart_print_value_with_unit(cr, value, unit, info_cb, user_data, x1, y1, MIDDLE_TOP, 5);
	_gchart_draw_sub_line(cr, x1, y1, x1, y2);
	cairo_stroke(cr);
	cairo_fill(cr);
}

inline static void _gchart_draw_sub_line_horizontal(cairo_t *cr, const double value, const char *unit, const GchartValueToInfoString info_cb, gconstpointer user_data, const double x1, const double y1, const double x2)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	_gchart_print_value_with_unit(cr, value, unit, info_cb, user_data, x1, y1, RIGHT_MIDDLE, 5);
	_gchart_draw_sub_line(cr, x1, y1, x2, y1);
	cairo_stroke(cr);
	cairo_fill(cr);
}

inline static void _gchart_set_line_atributes(cairo_t *cr, const double width, const cairo_line_join_t line_join, const cairo_line_cap_t line_cap)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	cairo_set_line_width(cr, width);
	cairo_set_line_join(cr, line_join);
	cairo_set_line_cap(cr, line_cap);
}
