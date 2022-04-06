/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * gchart-draw.h
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

#ifndef __G_CHART_DRAW_H__
#define __G_CHART_DRAW_H__

#include <features.h>
#include <glib.h>
#include <cairo.h>
#include <gtk/gtk.h>

#include "gchart.h"
#include "gchart-internal.h"

void _gchart_update_offsets(cairo_t *cr, GchartPrivate *priv);
//void _gchart_draw_info(GtkWidget *widget, cairo_t *cr, const GchartPrivate *priv);
void _gchart_draw(GtkWidget *widget, cairo_t *cr, GchartPrivate *priv);
void _gchart_draw_info(GtkWidget *widget, cairo_t *cr, const GchartPrivate *priv);

#endif /* __G_CHART_DRAW_H__ */
