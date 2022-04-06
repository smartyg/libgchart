/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * g_chart.h
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

#ifndef __G_CHART_H__
#define __G_CHART_H__

#include <features.h>
#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GCHART_TYPE                  (gchart_get_type())
#define GCHART_CAST(obj)             (G_TYPE_CHECK_INSTANCE_CAST((obj), GCHART_TYPE, Gchart))
#define GCHART_IS_CHART(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GCHART_TYPE))
#define GCHART_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST((klass), GCHART_TYPE, GchartClass))
#define GCHART_IS_CHART_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GCHART_TYPE))
#define GCHART_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS((obj), GCHART_TYPE, GchartClass))

/**
 * GchartGetValue:
 * @x: x value of the y value of.
 * @user_data: pointer to arbitrairy user data.
 *
 * Callback prototype to get a value.
 *
 * Returns: Y value corresponds to the requested x value.
 */
typedef float (*GchartGetValue)(float x, gconstpointer user_data);

/**
 * GchartRangeValue:
 * @user_data: pointer to arbitrairy user data.
 *
 * Callback prototype to get the minimum and maximum x and y values.
 *
 * Returns: value.
 */
typedef float (*GchartRangeValue)(gconstpointer user_data);

/**
 * GchartValueToInfoString:
 * @v: floating point number to be converted to a string.
 * @string: (inout): #GValue type to fill the string into.
 * @user_data: pointer to arbitrairy user data.
 *
 * Callback prototype to format a value to a GValue G_TYPE_STRING type.
 *
 * Returns: a GValue G_TYPE_STRING type containing the text representation of v.
 */
typedef gboolean (*GchartValueToInfoString)(float v, GValue *string, gconstpointer user_data);

/**
 * GchartAction:
 * @x_value: X value on the mouse pointer.
 * @user_data: pointer to arbitrairy user data.
 *
 * Callback prototype to get on mouse over events.
 */
typedef void (*GchartAction)(float x_value, gconstpointer user_data);

/**
 * Gchart: (skip)
 */
typedef struct _Gchart Gchart;

/**
 * GchartClass: (skip)
 */
typedef struct _GchartClass GchartClass;

/**
 * GchartDef: (skip)
 * Type to define properties for a chart.
 * This type can be passed into `g_chart_set_from_definition` to set all the
 * chart properties in one call.
 */
typedef struct _gchart_def GchartDef;

struct _gchart_def
{
	const gchar *title; ///< Title of this chart.
	gboolean plot_lines; ///< TRUE when a line must been drawn between the points.
	gboolean plot_dots; ///< TRUE when a dot must been drawn to represent a point.
	guint n_y_axis; ///< Number of y-axis (1 or 2). When this is `1`, all the y2_* fields are ingored.
	const gchar *x_label; ///< Name of the quantity on the x-axis.
	const gchar *y1_label; ///< Name of the quantity on the y-axis.
	const gchar *y2_label; ///< Name of the quantity on the y-axis.
	const gchar *x_unit; ///< Units on the x-axis.
	const gchar *y1_unit; ///< Units on the y-axis.
	const gchar *y2_unit; ///< Units on the y-axis.
	GchartValueToInfoString x_info_cb; ///< Callback function to convert a value to human readable text (when x_unit is not NULL, it will be appended).
	GchartValueToInfoString y1_info_cb; ///< Callback function to convert a value to human readable text (when y_unit is not NULL, it will be appended).
	GchartValueToInfoString y2_info_cb; ///< Callback function to convert a value to human readable text (when y_unit is not NULL, it will be appended).
	GchartRangeValue x_min_value_cb; ///< Function that returns the minimum value on the X axis. When this is NULL, 0 is used.
	GchartRangeValue x_max_value_cb; ///< Funciton that returns the maximum value on the X axis. When this is NULL, the value will be `n_steps` * `steps_size` and the x minimum is set to 0..
	GchartRangeValue y1_min_value_cb; ///< Function that returns the minimum value on the Y1 axis. When this is NULL, the y1-value which corresponds to the minumum x-value is used.
	GchartRangeValue y1_max_value_cb; ///< Function that returns the maximum value on the Y1 axis. When this is NULL, the y1-value which corresponds to the maximum x-value is used.
	GchartRangeValue y2_min_value_cb; ///< Function that returns the minimum value on the Y2 axis. When this is NULL, the y2-value which corresponds to the minumum x-value is used.
	GchartRangeValue y2_max_value_cb; ///< Function that returns the maximum value on the Y2 axis. When this is NULL, the y2-value which corresponds to the maximum x-value is used.
	guint n_steps; ///< Number of steps to use for the plotting.
	float step_size; ///< Size of each step, if `x_max_value_cb` is given, this value is ignored.
	GchartGetValue x_value_cb; ///< Function to get the real value on the X axis. When this is NULL, the pseudo x value is used.
	GchartGetValue y1_value_cb; ///< Function to get the real value on the Y1 axis.
	GchartGetValue y2_value_cb; ///< Function to get the real value on the Y2 axis.
};

struct _Gchart
{
	/* Parent instance structure */
	GtkDrawingArea parent_instance;
};

struct _GchartClass
{
	/* Parent class structure */
	GtkDrawingAreaClass parent_class;
};

/**
 * gchart_get_type:
 *
 * used by #GCHART_TYPE.
 *
 * Returns: Type value of the #GCHART type.
 */
GType gchart_get_type(void) G_GNUC_CONST;

/* gchart_new:
 *
 * Create a new empty chart.
 *
 * Returns: (transfer full): pointer to the new chart instance.
 */
Gchart *gchart_new(void);

/**
 * gchart_free:
 * @self: a #Gchart.
 *
 * Free and release the memory used by the chart.
 */
void gchart_free(Gchart *self);

/**
 * gchart_set_from_definition:
 * @self: a #Gchart.
 * @cd: (skip) (not nullable): Chart definition with a #GchartDef struct.
 * @user_data: User data to associate with this chart, this will be return with each callback function.
 *
 * Function to define a chart.
 *
 * Returns: TRUE if the definition was accepted and a chart has been defined.
 */
gboolean gchart_set_from_definition(Gchart *self, const GchartDef *cd, gconstpointer user_data);

/**
 * gchart_enable_y2_axis:
 * @self: a #Gchart.
 * @enable: TRUE to enable the use of the right y axis.
 *
 * Enable the right y axis.
 */
void gchart_enable_y2_axis(Gchart *self, gboolean enable);

/**
 * gchart_plot_lines:
 * @self: a #Gchart.
 * @lines: TRUE to enable plotting of raster lines in the chart.
 *
 * Control the plotting of raster lines. The spacing can be controlled by #gchart_set_n_steps.
 */
void gchart_plot_lines(Gchart *self, gboolean lines);

/**
 * gchart_plot_dots:
 * @self: a #Gchart.
 * @dots: TRUE to enable plotting of dots on point on chart.
 *
 * Control the plotting dots on each chart point.
 */
void gchart_plot_dots(Gchart *self, gboolean dots);

/**
 * gchart_get_user_data:
 * @self: a #Gchart.
 *
 * Get the associated user data with this chart instance.
 *
 * Returns: pointer the the associated user data.
 */
gconstpointer gchart_get_user_data(Gchart *self);

/**
 * gchart_set_user_data:
 * @self: a #Gchart.
 * @user_data: User data to associate with this chart instance.
 *
 * Set the associated user data with this chart instance.
 */
void gchart_set_user_data(Gchart *self, gconstpointer user_data);

/**
 * gchart_set_title:
 * @self: a #Gchart.
 * @title: Set the chart title.
 *
 * Set the chart title.
 */
void gchart_set_title(Gchart *self, const gchar *title);

/**
 * gchart_set_x_functions:
 * @self: a #Gchart.
 * @x_label: text to print along the x axis.
 * @x_unit: text to indicate the units on the x axis.
 * @x_info_cb: (scope call) (not nullable): callback to request text representation of the given x axis value.
 * @x_value_cb: (scope call) (nullable): callback to request x axis value, if NULL is given, the value of the x axis is used.
 *
 * This function defines the x axis.
 */
void gchart_set_x_functions(Gchart *self, const gchar *x_label, const gchar *x_unit, GchartValueToInfoString x_info_cb, GchartGetValue x_value_cb);

/**
 * gchart_set_y1_functions:
 * @self: a #Gchart.
 * @y1_label: text to print along the left y axis.
 * @y1_unit: text to indicate the units on the left y axis.
 * @y1_info_cb: (scope call) (not nullable): callback to request text representation of the given left y axis value.
 * @y1_value_cb: (scope call) (not nullable): callback to request left y axis value.
 *
 * This function defines the left y axis.
 */
void gchart_set_y1_functions(Gchart *self, const gchar *y1_label, const gchar *y1_unit, GchartValueToInfoString y1_info_cb, GchartGetValue y1_value_cb);

/**
 * gchart_set_y2_functions:
 * @self: a #Gchart.
 * @y2_label: text to print along the right y axis.
 * @y2_unit: text to indicate the units on the right y axis.
 * @y2_info_cb: (scope call) (not nullable): callback to request text representation of the given right y axis value.
 * @y2_value_cb: (scope call) (not nullable): callback to request right y axis value.
 *
 * This function defines the right y axis.
 */
void gchart_set_y2_functions(Gchart *self, const gchar *y2_label, const gchar *y2_unit, GchartValueToInfoString y2_info_cb, GchartGetValue y2_value_cb);

/**
 * gchart_set_x_limits:
 * @self: a #Gchart.
 * @x_min: minimum value on the x axis.
 * @x_max: maximum value on the x axis.
 *
 * This function defines the minimum and maximum value to print on the x axis.
 */
void gchart_set_x_limits(Gchart *self, const float x_min, const float x_max);

/**
 * gchart_set_y1_limits:
 * @self: a #Gchart.
 * @y1_min: minimum value on the left y axis.
 * @y1_max: maximum value on the left y axis.
 *
 * This function defines the minimum and maximum value to print on the left y axis.
 */
void gchart_set_y1_limits(Gchart *self, const float y1_min, const float y1_max);

/**
 * gchart_set_y2_limits:
 * @self: a #Gchart.
 * @y2_min: minimum value on the right y axis.
 * @y2_max: maximum value on the right y axis.
 *
 * This function defines the minimum and maximum value to print on the right y axis.
 */
void gchart_set_y2_limits(Gchart *self, const float y2_min, const float y2_max);

/**
 * gchart_set_x_limits_functions:
 * @self: a #Gchart.
 * @x_min_value_cb: (scope call) (nullable): callback funtion to request minimum value of the x axis.
 * @x_max_value_cb: (scope call) (nullable): callback funtion to request maximum value of the x axis.
 *
 * Set the callback functions to dynamically request the range of the x axis.
 */
void gchart_set_x_limits_functions(Gchart *self, GchartRangeValue x_min_value_cb, GchartRangeValue x_max_value_cb);

/**
 * gchart_set_y1_limits_functions:
 * @self: a #Gchart.
 * @y1_min_value_cb: (scope call) (nullable): callback funtion to request minimum value of the left y axis.
 * @y1_max_value_cb: (scope call) (nullable): callback funtion to request maximum value of the left y axis.
 *
 * Set the callback functions to dynamically request the range of the left y axis.
 */
void gchart_set_y1_limits_functions(Gchart *self, GchartRangeValue y1_min_value_cb, GchartRangeValue y1_max_value_cb);

/**
 * gchart_set_y2_limits_functions:
 * @self: a #Gchart.
 * @y2_min_value_cb: (scope call) (nullable): callback funtion to request minimum value of the right y axis.
 * @y2_max_value_cb: (scope call) (nullable): callback funtion to request maximum value of the right y axis.
 *
 * Set the callback functions to dynamically request the range of the right y axis.
 */
void gchart_set_y2_limits_functions(Gchart *self, GchartRangeValue y2_min_value_cb, GchartRangeValue y2_max_value_cb);

/**
 * gchart_set_n_steps:
 * @self: a #Gchart.
 * @n_steps: Set the number steps on the axis.
 *
 * Function to set the number of steps on the axises.
 */
void gchart_set_n_steps(Gchart *self, const unsigned int n_steps);

/**
 * gchart_get_n_steps:
 * @self: a #Gchart.
 *
 * Function to get the number of steps on the axises.
 *
 * Returns: current number of steps.
 */
unsigned int gchart_get_n_steps(Gchart *self);

/**
 * gchart_set_zoom:
 * @self: a #Gchart.
 * @zoom: Zoom level between 0.0 and 1.0.
 * @center: X value to center around.
 *
 * Function to set the zoom level and center point on the x axis.
 */
void gchart_set_zoom(Gchart *self, const float zoom, const float center);

/**
 * gchart_get_zoom:
 * @self: a #Gchart.
 *
 * Function to get the current zoom level.
 *
 * Returns:  the current zoom level between 0.0 and 1.0.
 */
float gchart_get_zoom(Gchart *self);

/**
 * gchart_get_center:
 * @self: a #Gchart.
 *
 * Function to get the current x axis center point.
 *
 * Returns:  the current x axis center point.
 */
float gchart_get_center(Gchart *self);

/**
 * gchart_set_on_mouse_over_function:
 * @self: a #Gchart.
 * @on_mouse_over_cb: (scope call) (nullable): Callback function to call when the mouse moves over the chart.
 *
 * Set the callback function to call when the mouse moves over the chart (on-mouse-over event).
 */
void gchart_set_on_mouse_over_function(Gchart *self, GchartAction on_mouse_over_cb);

/**
 * gchart_get_on_mouse_over_function:
 * @self: a #Gchart.
 *
 * Get the current on-mouse-over callback function.
 *
 * Returns: (skip): pointer to the current callback function for on-mouse-over events.
 */
GchartAction gchart_get_on_mouse_over_function(Gchart *self);

/**
 * gchart_set_on_mouse_click_function:
 * @self: a #Gchart.
 * @on_mouse_click_cb: (scope call) (nullable): Callback function to call when the mouse is clicked on the chart.
 *
 * Set the callback function to call when the mouse button is clicked on the chart (on-mouse-click event).
 */
void gchart_set_on_mouse_click_function(Gchart *self, GchartAction on_mouse_click_cb);

/**
 * gchart_get_on_mouse_click_function:
 * @self: a #Gchart.
 *
 * Get the current on-mouse-click callback function.
 *
 * Returns: (skip): pointer to the current callback function for on-mouse-click events.
 */
GchartAction gchart_get_on_mouse_click_function(Gchart *self);

G_END_DECLS

#endif /* __G_CHART_H__ */
