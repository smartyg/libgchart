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

#include "gchart.h"

#include <features.h>
#include <math.h>
#include <glib.h>
#include <glib-object.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "gchart-internal.h"
#include "gchart-draw.h"

enum
{
	PROP_0,
	PROP_TITLE,
	PROP_PLOT_LINES,
	PROP_PLOT_DOTS,
	PROP_ENABLE_Y2,
	PROP_N_STEPS,
	PROP_STEP_SIZE,
	N_PROPERTIES
};

static void _gchart_dispose(GObject *object);
static void _gchart_finalize(GObject *object);
static void _gchart_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void _gchart_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);

inline static float _gchart_get_x_value_mouse(GtkWidget *widget, const GdkEventMotion *event);
static gboolean _gchart_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data);
static gboolean _gchart_cursor_move_cb(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
static gboolean _gchart_cursor_click_cb(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);

G_DEFINE_TYPE_WITH_PRIVATE(Gchart, gchart, GTK_TYPE_DRAWING_AREA);

static void gchart_class_init(GchartClass *klass)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = _gchart_dispose;
	gobject_class->finalize = _gchart_finalize;
	gobject_class->set_property = _gchart_set_property;
	gobject_class->get_property = _gchart_get_property;

	g_object_class_install_property(gobject_class, PROP_TITLE, g_param_spec_string("title",
																				   "title",
																				   "Title of this chart.",
																				   NULL,
																				   G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_PLOT_LINES, g_param_spec_boolean("plot-lines",
																						 "plot lines between points",
																						 "Plot lines between points.",
																						 TRUE,
																						 G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_PLOT_DOTS, g_param_spec_boolean("plot-dots",
																						"plot dots at points",
																						"Plot dots at points.",
																						FALSE,
																						G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_ENABLE_Y2, g_param_spec_boolean("enable-y2",
																						"enable y2 axis",
																						"Enable the left axis.",
																						 FALSE,
																						 G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_N_STEPS, g_param_spec_uint("n-steps",
																				   "number of steps",
																				   "Number of steps.",
																				   0,
																				   2048,
																				   0,
																				   G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_STEP_SIZE, g_param_spec_float("step-size",
																					  "size of a step",
																					  "Size of a step.",
																					  0.0,
																					  FLT_MAX,
																					  0.0,
																					  G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void gchart_init(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv = gchart_get_instance_private(self);

	priv->x_scale = priv->y1_scale = priv->y2_scale = priv->x_info_value = 0;
	priv->offset_left = priv->offset_top = priv->offset_bottom = priv->offset_right = FLT_MIN;

	priv->x_label = priv->y1_label = priv->y2_label = priv->x_unit = priv->y1_unit = priv->y2_unit = NULL;
	priv->plot_lines = TRUE;
	priv->plot_dots = FALSE;
	priv->enable_y2 = FALSE;
	priv->x_info_cb = priv->y1_info_cb = priv->y2_info_cb = NULL;
	priv->user_data = NULL;

	/* callbacks for point plotting */
	priv->x_cb = priv->y1_cb = priv->y2_cb = NULL;

	/* callbacks for lineplotting */
	priv->x_min_value_cb = priv->x_max_value_cb = priv->y1_min_value_cb = priv->y1_max_value_cb = priv->y2_min_value_cb = priv->y2_max_value_cb = NULL;
	priv->n_steps = 100;
	priv->step_size = 0.0;

	priv->zoom = 0;
	priv->x_center = NAN;

	priv->on_mouse_over_cb = NULL;
	priv->on_mouse_click_cb = NULL;

	/* Connect the chart_t struct to this widget. */
	g_object_set_data(G_OBJECT(self), "chart-data", NULL);

	/* Keep track of these events. */
	gtk_widget_add_events(GTK_WIDGET(self), GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

	/* Connect the draw and motion signals. */
	g_signal_connect(G_OBJECT(self), "draw", G_CALLBACK(_gchart_draw_cb), NULL);
	g_signal_connect(G_OBJECT(self), "motion-notify-event", G_CALLBACK(_gchart_cursor_move_cb), NULL);
	g_signal_connect(G_OBJECT(self), "button-press-event", G_CALLBACK(_gchart_cursor_click_cb), NULL);
}

static void _gchart_dispose(GObject *object)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	G_OBJECT_CLASS(gchart_parent_class)->dispose(object);
}

static void _gchart_finalize(GObject *object)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	Gchart *self = GCHART_CAST(object);
	GchartPrivate *priv = gchart_get_instance_private(self);

	g_free(priv->x_label);
	g_free(priv->y1_label);
	g_free(priv->y2_label);
	g_free(priv->x_unit);
	g_free(priv->y1_unit);
	g_free(priv->y2_unit);
	g_free(priv->title);
	priv->x_label = priv->y1_label = priv->y2_label = priv->x_unit = priv->y1_unit = priv->y2_unit = NULL;

	/*
	 * Always chain up to the parent class; as with dispose(), finalize()
	 * is guaranteed to exist on the parent's class virtual function table
	 */
	G_OBJECT_CLASS(gchart_parent_class)->finalize(object);
}

static void _gchart_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	Gchart *self = GCHART_CAST(object);
	GchartPrivate *priv = gchart_get_instance_private(self);

	switch(property_id)
	{
		case PROP_TITLE:
			if(value->g_type == G_TYPE_STRING)
			{
				g_free(priv->title);
				priv->title = g_value_dup_string(value);
			}
			break;
		case PROP_PLOT_LINES:
			if(value->g_type == G_TYPE_BOOLEAN)
				priv->plot_lines = g_value_get_boolean(value);
			break;
		case PROP_PLOT_DOTS:
			if(value->g_type == G_TYPE_BOOLEAN)
				priv->plot_dots = g_value_get_boolean(value);
			break;
		case PROP_ENABLE_Y2:
			if(value->g_type == G_TYPE_BOOLEAN)
				priv->enable_y2 = g_value_get_boolean(value);
			break;
		case PROP_N_STEPS:
			if(value->g_type == G_TYPE_UINT)
				priv->n_steps = g_value_get_uint(value);
			break;
		case PROP_STEP_SIZE:
			if(value->g_type == G_TYPE_FLOAT)
				priv->step_size = g_value_get_float(value);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void _gchart_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	Gchart *self = GCHART_CAST(object);
	GchartPrivate *priv = gchart_get_instance_private(self);

	switch(property_id)
	{
		case PROP_TITLE:
			g_value_set_string(value, priv->title);
			break;
		case PROP_PLOT_LINES:
			g_value_set_boolean(value, priv->plot_lines);
			break;
		case PROP_PLOT_DOTS:
			g_value_set_boolean(value, priv->plot_dots);
			break;
		case PROP_ENABLE_Y2:
			g_value_set_boolean(value, priv->enable_y2);
			break;
		case PROP_N_STEPS:
			g_value_set_uint(value, priv->n_steps);
			break;
		case PROP_STEP_SIZE:
			g_value_set_float(value, priv->step_size);
			break;
		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

Gchart *gchart_new(void)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	return g_object_new(GCHART_TYPE, NULL);
}

void gchart_free(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	g_object_unref(self);
}

gboolean gchart_set_from_definition(Gchart *self, const GchartDef *cd, gconstpointer user_data)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;

	g_return_val_if_fail(GCHART_IS_CHART(self), FALSE);

	if(cd == NULL || cd->y1_value_cb == NULL || (cd->n_y_axis == 2 && cd->y2_value_cb == NULL))
		return FALSE;

	priv = gchart_get_instance_private(self);

	g_free(priv->title);
	priv->title = g_strdup(cd->title);

	priv->plot_lines = cd->plot_lines;
	priv->plot_dots = cd->plot_dots;

	g_free(priv->x_label);
	priv->x_label = g_strdup(cd->x_label);
	g_free(priv->y1_label);
	priv->y1_label = g_strdup(cd->y1_label);
	g_free(priv->x_unit);
	priv->x_unit = g_strdup(cd->x_unit);
	g_free(priv->y1_unit);
	priv->y1_unit = g_strdup(cd->y1_unit);
	priv->user_data = user_data;
	priv->x_cb = cd->x_value_cb;
	priv->y1_cb = cd->y1_value_cb;
	priv->x_info_cb = cd->x_info_cb;
	priv->y1_info_cb = cd->y1_info_cb;

	priv->x_min_value_cb = cd->x_min_value_cb;
	priv->x_max_value_cb = cd->x_max_value_cb;
	priv->y1_min_value_cb = cd->y1_min_value_cb;
	priv->y1_max_value_cb = cd->y1_max_value_cb;
	priv->n_steps = cd->n_steps;
	priv->step_size = cd->step_size;

	if(cd->n_y_axis == 2)
		priv->enable_y2 = TRUE;
	else
		priv->enable_y2 = FALSE;

	g_free(priv->y2_label);
	priv->y2_label = g_strdup(cd->y2_label);
	g_free(priv->y2_unit);
	priv->y2_unit = g_strdup(cd->y2_unit);
	priv->y2_cb = cd->y2_value_cb;
	priv->y2_info_cb = cd->y2_info_cb;
	priv->y2_min_value_cb = cd->y2_min_value_cb;
	priv->y2_max_value_cb = cd->y2_max_value_cb;

	return TRUE;
}

void gchart_enable_y2_axis(Gchart *self, gboolean enable)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->enable_y2 = enable;
}

void gchart_plot_lines(Gchart *self, gboolean lines)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->plot_lines = lines;
}

void gchart_plot_dots(Gchart *self, gboolean dots)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->plot_dots = dots;
}

gconstpointer gchart_get_user_data(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_val_if_fail(GCHART_IS_CHART(self), NULL);
	priv = gchart_get_instance_private(self);
	return priv->user_data;
}

void gchart_set_user_data(Gchart *self, gconstpointer user_data)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->user_data = user_data;
}

void gchart_set_title(Gchart *self, const gchar *title)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	g_free(priv->title);
	priv->title = g_strdup(title);
}

void gchart_set_x_functions(Gchart *self, const gchar *x_label, const gchar *x_unit, GchartValueToInfoString x_info_cb, GchartGetValue x_value_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	g_free(priv->x_label);
	priv->x_label = g_strdup(x_label);
	g_free(priv->x_unit);
	priv->x_unit = g_strdup(x_unit);
	priv->x_info_cb = x_info_cb;
	priv->x_cb = x_value_cb;
}

void gchart_set_y1_functions(Gchart *self, const gchar *y1_label, const gchar *y1_unit, GchartValueToInfoString y1_info_cb, GchartGetValue y1_value_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	g_free(priv->y1_label);
	priv->y1_label = g_strdup(y1_label);
	g_free(priv->y1_unit);
	priv->y1_unit = g_strdup(y1_unit);
	priv->y1_info_cb = y1_info_cb;
	priv->y1_cb = y1_value_cb;
}

void gchart_set_y2_functions(Gchart *self, const gchar *y2_label, const gchar *y2_unit, GchartValueToInfoString y2_info_cb, GchartGetValue y2_value_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	g_free(priv->y2_label);
	priv->y2_label = g_strdup(y2_label);
	g_free(priv->y2_unit);
	priv->y2_unit = g_strdup(y2_unit);
	priv->y2_info_cb = y2_info_cb;
	priv->y2_cb = y2_value_cb;
}

void gchart_set_x_limits(Gchart *self, const float x_min, const float x_max)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->x_min = x_min;
	priv->x_max = x_max;
}

void gchart_set_y1_limits(Gchart *self, const float y1_min, const float y1_max)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->y1_min = y1_min;
	priv->y1_max = y1_max;
}

void gchart_set_y2_limits(Gchart *self, const float y2_min, const float y2_max)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->y2_min = y2_min;
	priv->y2_max = y2_max;
}

void gchart_set_x_limits_functions(Gchart *self, GchartRangeValue x_min_value_cb, GchartRangeValue x_max_value_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->x_min_value_cb = x_min_value_cb;
	priv->x_max_value_cb = x_max_value_cb;
}

void gchart_set_y1_limits_functions(Gchart *self, GchartRangeValue y1_min_value_cb, GchartRangeValue y1_max_value_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->y1_min_value_cb = y1_min_value_cb;
	priv->y1_max_value_cb = y1_max_value_cb;
}

void gchart_set_y2_limits_functions(Gchart *self, GchartRangeValue y2_min_value_cb, GchartRangeValue y2_max_value_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->y2_min_value_cb = y2_min_value_cb;
	priv->y2_max_value_cb = y2_max_value_cb;
}

void gchart_set_n_steps(Gchart *self, const unsigned int n_steps)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->n_steps = n_steps;
}

unsigned int gchart_get_n_steps(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_val_if_fail(GCHART_IS_CHART(self), 0);
	priv = gchart_get_instance_private(self);
	return priv->n_steps;
}

void gchart_set_zoom(Gchart *self, const float zoom, const float center)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	g_return_if_fail(zoom >= 1.0f);
	priv = gchart_get_instance_private(self);

	priv->zoom = zoom;
	if(isfinite(center))
		priv->x_center = center;
}

float gchart_get_zoom(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_val_if_fail(GCHART_IS_CHART(self), NAN);
	priv = gchart_get_instance_private(self);
	return priv->zoom;
}

float gchart_get_center(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_val_if_fail(GCHART_IS_CHART(self), NAN);
	priv = gchart_get_instance_private(self);
	return priv->x_center;
}

void gchart_set_on_mouse_over_function(Gchart *self, GchartAction on_mouse_over_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->on_mouse_over_cb = on_mouse_over_cb;
}

GchartAction gchart_get_on_mouse_over_function(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_val_if_fail(GCHART_IS_CHART(self), NULL);
	priv = gchart_get_instance_private(self);
	return priv->on_mouse_over_cb;
}

void gchart_set_on_mouse_click_function(Gchart *self, GchartAction on_mouse_click_cb)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_if_fail(GCHART_IS_CHART(self));
	priv = gchart_get_instance_private(self);
	priv->on_mouse_click_cb = on_mouse_click_cb;
}

GchartAction gchart_get_on_mouse_click_function(Gchart *self)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	GchartPrivate *priv;
	g_return_val_if_fail(GCHART_IS_CHART(self), NULL);
	priv = gchart_get_instance_private(self);
	return priv->on_mouse_click_cb;
}


static float _gchart_get_x_value_mouse(GtkWidget *widget, const GdkEventMotion *event)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	guint width, height;
	GchartPrivate *priv;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);
	priv = gchart_get_instance_private(GCHART_CAST(widget));

	if(event->x > priv->offset_left && event->x < width - priv->offset_right && event->y > priv->offset_top && event->y < height - priv->offset_bottom)
		return (event->x - priv->offset_left) / priv->x_scale + priv->x_min;
	return NAN;
}

static gboolean _gchart_cursor_move_cb(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	UNUSED(user_data);

	GdkWindow *window;
	GchartPrivate *priv;
	cairo_region_t *r;
	cairo_rectangle_int_t *rect;
	guint width, height;
	float x_info_value_old;

	priv = gchart_get_instance_private(GCHART_CAST(widget));

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	/* Save the old info value, so it can be compared to the new one. */
	x_info_value_old = priv->x_info_value;
	priv->x_info_value = _gchart_get_x_value_mouse(widget, event);

	/* If the mouse changed on the x axis, call the on mouse over callback. */
	if(priv->on_mouse_over_cb != NULL && !isnan(priv->x_info_value) && (isnan(x_info_value_old) || priv->x_info_value != x_info_value_old))
		priv->on_mouse_over_cb(priv->x_info_value, priv->user_data);

	if((window = gtk_widget_get_window(widget)) != NULL)
	{
		rect = g_slice_new(cairo_rectangle_int_t);
		rect->x = width - priv->info_box_width;
		rect->y = 0;
		rect->width = priv->info_box_width;
		rect->height = height;
		r = cairo_region_create_rectangle(rect);
		gdk_window_invalidate_region(window, r, FALSE);
		cairo_region_destroy(r);
		g_slice_free(cairo_rectangle_int_t, rect);
	}

	return TRUE;
}

static gboolean _gchart_cursor_click_cb(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	UNUSED(user_data);

	float x_value;
	GchartPrivate *priv;

	priv = gchart_get_instance_private(GCHART_CAST(widget));
	if(priv->on_mouse_click_cb != NULL)
	{
		if(!isnan((x_value = _gchart_get_x_value_mouse(widget, event))))
			priv->on_mouse_click_cb(x_value, priv->user_data);
	}
	return TRUE;
}

static gboolean _gchart_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	UNUSED(user_data);

	GchartPrivate *priv = gchart_get_instance_private(GCHART_CAST(widget));
	if(priv->offset_left == FLT_MIN || priv->offset_right == FLT_MIN || priv->offset_top == FLT_MIN || priv->offset_bottom == FLT_MIN)
		_gchart_update_offsets(cr, priv);

	_gchart_draw(widget, cr, priv);

	if(!isnan(priv->x_info_value))
		_gchart_draw_info(widget, cr, priv);

	return FALSE;
}
