/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
#ifndef __G_CHART_H__
#define __G_CHART_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define G_TYPE_CHART                  (g_chart_get_type())
#define G_CHART(obj)                  (G_TYPE_CHECK_INSTANCE_CAST((obj), G_TYPE_CHART, GChart))
#define G_IS_CHART(obj)               (G_TYPE_CHECK_INSTANCE_TYPE((obj), G_TYPE_CHART))
#define G_CHART_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST((klass), G_TYPE_CHART, GChartClass))
#define G_IS_CHART_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_CHART))
#define G_CHART_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS((obj), G_TYPE_CHART, GChartClass))

/**
 * Callback prototype to get a value.
 */
typedef float (*chart_get_value_t)(float x, gconstpointer user_data);

/**
 * Callback prototype to get the minimum and maximum x and y values.
 */
typedef float (*chart_range_value_t)(gconstpointer user_data);

/**
 * Callback prototype to format a value to a GValue G_TYPE_STRING type.
 */
typedef GValue *(*chart_value_to_info_string_t)(float v, gconstpointer user_data);

/**
 * Callback prototype to get on mouse over events.
 */
typedef void (*chart_action_t)(float x_value, gconstpointer user_data);

typedef struct _GChart        GChart;
typedef struct _GChartClass   GChartClass;

/**
 * Type to define properties for a chart.
 * This type can be passed into `g_chart_set_from_definition` to set all the
 * chart properties in one call.
 */
typedef struct _chart_def chart_def_t;

struct _chart_def
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
	chart_value_to_info_string_t x_info_cb; ///< Callback function to convert a value to human readable text (when x_unit is not NULL, it will be appended).
	chart_value_to_info_string_t y1_info_cb; ///< Callback function to convert a value to human readable text (when y_unit is not NULL, it will be appended).
	chart_value_to_info_string_t y2_info_cb; ///< Callback function to convert a value to human readable text (when y_unit is not NULL, it will be appended).
	chart_range_value_t x_min_value_cb; ///< Function that returns the minimum value on the X axis. When this is NULL, 0 is used.
	chart_range_value_t x_max_value_cb; ///< Funciton that returns the maximum value on the X axis. When this is NULL, the value will be `n_steps` * `steps_size` and the x minimum is set to 0..
	chart_range_value_t y1_min_value_cb; ///< Function that returns the minimum value on the Y1 axis. When this is NULL, the y1-value which corresponds to the minumum x-value is used.
	chart_range_value_t y1_max_value_cb; ///< Function that returns the maximum value on the Y1 axis. When this is NULL, the y1-value which corresponds to the maximum x-value is used.
	chart_range_value_t y2_min_value_cb; ///< Function that returns the minimum value on the Y2 axis. When this is NULL, the y2-value which corresponds to the minumum x-value is used.
	chart_range_value_t y2_max_value_cb; ///< Function that returns the maximum value on the Y2 axis. When this is NULL, the y2-value which corresponds to the maximum x-value is used.
	guint n_steps; ///< Number of steps to use for the plotting.
	float step_size; ///< Size of each step, if `x_max_value_cb` is given, this value is ignored.
	chart_get_value_t x_value_cb; ///< Function to get the real value on the X axis. When this is NULL, the pseudo x value is used.
	chart_get_value_t y1_value_cb; ///< Function to get the real value on the Y1 axis.
	chart_get_value_t y2_value_cb; ///< Function to get the real value on the Y2 axis.
};

struct _GChart
{
	/* Parent instance structure */
	GtkDrawingArea parent_instance;
};

struct _GChartClass
{
	/* Parent class structure */
	GtkDrawingAreaClass parent_class;
};

/* used by G_TYPE_CHART */
GType g_chart_get_type(void) G_GNUC_CONST;

/**
 * Create a new chart with the given chart definition.
 */
GChart *g_chart_new(void);

/**
 * Free and release the memory used by the chart.
 */
void g_chart_free(GChart *self);

/*
 * Method definitions.
 */
gboolean g_chart_set_from_definition(GChart *self, const chart_def_t *cd, gconstpointer user_data);;
void g_chart_enable_y2_axis(GChart *self, gboolean enable);
void g_chart_plot_lines(GChart *self, gboolean lines);
void g_chart_plot_dots(GChart *self, gboolean dots);
gconstpointer g_chart_get_user_data(GChart *self);
void g_chart_set_user_data(GChart *self, gconstpointer user_data);
void g_chart_set_title(GChart *self, const gchar *title);
void g_chart_set_x_functions(GChart *self, const gchar *x_label, const gchar *x_unit, chart_value_to_info_string_t x_info_cb, chart_get_value_t x_value_cb);
void g_chart_set_y1_functions(GChart *self, const gchar *y1_label, const gchar *y1_unit, chart_value_to_info_string_t y1_info_cb, chart_get_value_t y1_value_cb);
void g_chart_set_y2_functions(GChart *self, const gchar *y2_label, const gchar *y2_unit, chart_value_to_info_string_t y2_info_cb, chart_get_value_t y2_value_cb);
void g_chart_set_x_limits(GChart *self, const float x_min, const float x_max);
void g_chart_set_y1_limits(GChart *self, const float y1_min, const float y1_max);
void g_chart_set_y2_limits(GChart *self, const float y2_min, const float y2_max);
void g_chart_set_x_limits_functions(GChart *self, chart_range_value_t x_min_value_cb, chart_range_value_t x_max_value_cb);
void g_chart_set_y1_limits_functions(GChart *self, chart_range_value_t y1_min_value_cb, chart_range_value_t y1_max_value_cb);
void g_chart_set_y2_limits_functions(GChart *self, chart_range_value_t y2_min_value_cb, chart_range_value_t y2_max_value_cb);
void g_chart_set_n_steps(GChart *self, const unsigned int n_steps);
unsigned int g_chart_get_n_steps(GChart *self);
void g_chart_set_zoom(GChart *self, const float zoom, const float center);
float g_chart_get_zoom(GChart *self);
float g_chart_get_center(GChart *self);
void g_chart_set_on_mouse_over_function(GChart *self, chart_action_t on_mouse_over_cb);
chart_action_t g_chart_get_on_mouse_over_function(GChart *self);
void g_chart_set_on_mouse_click_function(GChart *self, chart_action_t on_mouse_click_cb);
chart_action_t g_chart_get_on_mouse_click_function(GChart *self);

G_END_DECLS

#endif /* __G_CHART_H__ */
