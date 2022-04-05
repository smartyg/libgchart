/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * chart.c
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
#include <stdio.h>
#include <gtk/gtk.h>

#include "g_chart.h"

float _x_value_cb(float x, gconstpointer user_data);
float _y1_value_cb(float x, gconstpointer user_data);
gboolean _x_info_cb(float v, GValue *value, gconstpointer user_data);
gboolean _y1_info_cb(float v, GValue *value, gconstpointer user_data);
void _on_mouse_over_cb(float x_value, gconstpointer user_data);
void _on_mouse_click_cb(float x_value, gconstpointer user_data);

inline static char *ftostr (float f, int digits) {
	char *str = NULL;
	if (asprintf (&str, "%.*f", digits, f) < 0)
		return NULL;
	return str;
}

float _x_value_cb(float x, gconstpointer user_data) {
	g_debug("%s: %f, %p", __func__, x, user_data);
	return x;
}

float _y1_value_cb(float x, gconstpointer user_data) {
	g_debug("%s: %f, %p", __func__, x, user_data);
	return x * (40.0 / 180.0);
}

gboolean _x_info_cb(float v, GValue *value, gconstpointer user_data) {
	g_debug("%s: %f, %p", __func__, v, user_data);
	char *str = ftostr (v, 1);
	g_value_init (value, G_TYPE_STRING);
	g_value_set_string (value, str);
	free (str);
	return TRUE;
}

gboolean _y1_info_cb(float v, GValue *value, gconstpointer user_data) {
	g_debug("%s: %f, %p", __func__, v, user_data);
	char *str = ftostr (v, 1);
	g_value_init (value, G_TYPE_STRING);
	g_value_set_string (value, str);
	free (str);
	return TRUE;
}

void _on_mouse_over_cb(float x_value, gconstpointer user_data) {
	g_debug("%s: %f, %p", __func__, x_value, user_data);
}

void _on_mouse_click_cb(float x_value, gconstpointer user_data) {
	g_debug("%s: %f, %p", __func__, x_value, user_data);
}

int
main (int argc, char *argv[])
{
	Gchart *chart;
	GtkWidget *window;

	g_log_set_handler("Gchart", G_LOG_LEVEL_MASK, g_log_default_handler, NULL);

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Window");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	chart = g_object_new(GCHART_TYPE, NULL);
	gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(chart));

	gchart_enable_y2_axis(chart, FALSE);
	gchart_plot_lines(chart, TRUE);
	gchart_plot_dots(chart, TRUE);
	gchart_set_title(chart, "Test graph");

	//void gchart_set_user_data(chart, gconstpointer user_data);

	gchart_set_x_functions(chart, "time", NULL, _x_info_cb, _x_value_cb);
	gchart_set_y1_functions(chart, "speed", "km/h", _y1_info_cb, _y1_value_cb);
	gchart_set_x_limits(chart, 0, 180);
	gchart_set_y1_limits(chart, 0, 40);

	//void gchart_set_x_limits_functions(chart, GchartRangeValue x_min_value_cb, GchartRangeValue x_max_value_cb);
	//void gchart_set_y1_limits_functions(chart, GchartRangeValue y1_min_value_cb, GchartRangeValue y1_max_value_cb);

	gchart_set_n_steps(chart, 18);

	//void gchart_set_zoom(chart, 0.0, const float center);

	gchart_set_on_mouse_over_function(chart, _on_mouse_over_cb);
	gchart_set_on_mouse_click_function(chart, _on_mouse_click_cb);

	gtk_widget_show(GTK_WIDGET(chart));

	gtk_widget_show(window);

	gtk_main();

	return 0;
}
