/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
#include "config.h"

#include "g_chart.h"

#include <math.h>
#include <libintl.h>
#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define PADDING (5)
#define BORDER_OFFSET (PADDING)
#define DOT_RADIUS 2.0

#define UNUSED(x) (void)(x) ///< Used to indicate that a function does not use a certain parameter.

typedef enum _text_mode text_mode_t;
typedef struct _GChartPrivate GChartPrivate;

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

struct _GChartPrivate
{
	float x_scale, y1_scale, y2_scale, offset_left, offset_right, offset_top, offset_bottom, info_box_width;
	float x_min, x_max, y1_min, y1_max, y2_min, y2_max, x_info_value;
	gchar *title, *x_label, *y1_label, *y2_label, *x_unit, *y1_unit, *y2_unit;
	gboolean plot_lines, plot_dots, enable_y2;
	chart_value_to_info_string_t x_info_cb, y1_info_cb, y2_info_cb;
	gconstpointer user_data;

	/* callbacks for point plotting */
	chart_get_value_t x_cb, y1_cb, y2_cb;

	/* callbacks for lineplotting */
	chart_range_value_t x_min_value_cb, x_max_value_cb, y1_min_value_cb, y1_max_value_cb, y2_min_value_cb, y2_max_value_cb;
	guint n_steps;
	float step_size;
};

/* Declaration of the callback functions. */
static gboolean _chart_cursor_move_cb(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);
static gboolean _chart_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data);

static void g_chart_dispose(GObject *object);
static void g_chart_finalize(GObject *object);
static void g_chart_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec);
static void g_chart_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec);
inline static void _chart_update_offsets(cairo_t *cr, GChartPrivate *priv);
inline static void _draw_info(GtkWidget *widget, cairo_t *cr, const GChartPrivate *priv);
inline static void _chart_draw(GtkWidget *widget, cairo_t *cr, GChartPrivate *priv);
inline static void _draw_raster(cairo_t *cr, const GChartPrivate *priv, const guint width, const guint height);
inline static void _draw_sub_line(cairo_t *cr, const double x1, const double y1, const double x2, const double y2);
inline static void _draw_sub_line_vertical(cairo_t *cr, const double value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data, const double x1, const double y1, const double y2);
inline static void _draw_sub_line_horizontal(cairo_t *cr, const double value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data, const double x1, const double y1, const double x2);
inline static void _set_line_atributes(cairo_t *cr, const double width, const cairo_line_join_t line_join, const cairo_line_cap_t line_cap);
static void _print_text(cairo_t *cr, const char *text, const float x, const float y, const text_mode_t m, const float padding);
static gchar *get_value_as_text(const float value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data);
static void _print_value_with_unit(cairo_t *cr, const float value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data, const float x, const float y, const text_mode_t m, const float padding);

G_DEFINE_TYPE_WITH_PRIVATE(GChart, g_chart, GTK_TYPE_DRAWING_AREA);

static void g_chart_class_init(GChartClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

	gobject_class->dispose = g_chart_dispose;
	gobject_class->finalize = g_chart_finalize;
	gobject_class->set_property = g_chart_set_property;
	gobject_class->get_property = g_chart_get_property;

	g_object_class_install_property(gobject_class, PROP_TITLE, g_param_spec_string("title",
		_("title"),
		_("Title of this chart."),
		NULL,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_PLOT_LINES, g_param_spec_boolean("plot-lines",
		_("plot lines between points"),
		_("Plot lines between points."),
		TRUE,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_PLOT_DOTS, g_param_spec_boolean("plot-dots",
		_("plot dots at points"),
		_("Plot dots at points."),
		FALSE,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_ENABLE_Y2, g_param_spec_boolean("enable-y2",
		_("enable y2 axis"),
		_("Enable the left axis."),
		FALSE,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_N_STEPS, g_param_spec_uint("n-steps",
		_("number of steps"),
		_("Number of steps."),
		0,
		2048,
		0,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	g_object_class_install_property(gobject_class, PROP_STEP_SIZE, g_param_spec_float("step-size",
		_("size of a step"),
		_("Size of a step."),
		0.0,
		FLT_MAX,
		0.0,
		G_PARAM_CONSTRUCT | G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
}

static void g_chart_init(GChart *self)
{
	GChartPrivate *priv = g_chart_get_instance_private(self);

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
	priv->n_steps = 0;
	priv->step_size = 0.0;

	/* Connect the chart_t struct to this widget. */
	g_object_set_data(G_OBJECT(self), "chart-data", NULL);

	/* Keep track of these events. */
	gtk_widget_add_events(GTK_WIDGET(self), GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

	/* Connect the draw and motion signals. */
	g_signal_connect(G_OBJECT(self), "draw", G_CALLBACK(_chart_draw_cb), NULL);
	g_signal_connect(G_OBJECT(self), "motion-notify-event", G_CALLBACK(_chart_cursor_move_cb), NULL);
}

static void g_chart_dispose(GObject *object)
{
	G_OBJECT_CLASS(g_chart_parent_class)->dispose(object);
}

static void g_chart_finalize(GObject *object)
{
	GChart *self = G_CHART(object);
	GChartPrivate *priv = g_chart_get_instance_private(self);

	g_free(priv->x_label);
	g_free(priv->y1_label);
	g_free(priv->y2_label);
	g_free(priv->x_unit);
	g_free(priv->y1_unit);
	g_free(priv->y2_unit);
	priv->x_label = priv->y1_label = priv->y2_label = priv->x_unit = priv->y1_unit = priv->y2_unit = NULL;

	/*
	 * Always chain up to the parent class; as with dispose(), finalize()
	 * is guaranteed to exist on the parent's class virtual function table
	 */
	G_OBJECT_CLASS(g_chart_parent_class)->finalize(object);
}

static void g_chart_set_property(GObject *object, guint property_id, const GValue *value, GParamSpec *pspec)
{
	GChart *self = G_CHART(object);
	GChartPrivate *priv = g_chart_get_instance_private(self);

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

GChart *g_chart_new(void)
{
	return g_object_new(G_TYPE_CHART, NULL);
}

void g_chart_free(GChart *self)
{
	g_object_unref(self);
}

static void g_chart_get_property(GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	GChart *self = G_CHART(object);
	GChartPrivate *priv = g_chart_get_instance_private(self);

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

gboolean g_chart_set_from_definition(GChart *self, const chart_def_t *cd, gconstpointer user_data)
{
	GChartPrivate *priv;

	g_return_val_if_fail(G_IS_CHART(self), FALSE);

	if(cd == NULL || cd->y1_value_cb == NULL || (cd->n_y_axis == 2 && cd->y2_value_cb == NULL))
		return FALSE;

	priv = g_chart_get_instance_private(self);

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

void g_chart_enable_y2_axis(GChart *self, gboolean enable)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->enable_y2 = enable;
}

void g_chart_plot_lines(GChart *self, gboolean lines)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->plot_lines = lines;
}

void g_chart_plot_dots(GChart *self, gboolean dots)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->plot_dots = dots;
}

gconstpointer g_chart_get_user_data(GChart *self)
{
	GChartPrivate *priv;
	g_return_val_if_fail(G_IS_CHART(self), NULL);
	priv = g_chart_get_instance_private(self);
	return priv->user_data;
}

void g_chart_set_user_data(GChart *self, gconstpointer user_data)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->user_data = user_data;
}

void g_chart_set_title(GChart *self, const gchar *title)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	g_free(priv->title);
	priv->title = g_strdup(title);
}

void g_chart_set_x_functions(GChart *self, const gchar *x_label, const gchar *x_unit, chart_value_to_info_string_t x_info_cb, chart_get_value_t x_value_cb)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	g_free(priv->x_label);
	priv->x_label = g_strdup(x_label);
	g_free(priv->x_unit);
	priv->x_unit = g_strdup(x_unit);
	priv->x_info_cb = x_info_cb;
	priv->x_cb = x_value_cb;
}

void g_chart_set_y1_functions(GChart *self, const gchar *y1_label, const gchar *y1_unit, chart_value_to_info_string_t y1_info_cb, chart_get_value_t y1_value_cb)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	g_free(priv->y1_label);
	priv->y1_label = g_strdup(y1_label);
	g_free(priv->y1_unit);
	priv->y1_unit = g_strdup(y1_unit);
	priv->y1_info_cb = y1_info_cb;
	priv->y1_cb = y1_value_cb;
}

void g_chart_set_y2_functions(GChart *self, const gchar *y2_label, const gchar *y2_unit, chart_value_to_info_string_t y2_info_cb, chart_get_value_t y2_value_cb)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	g_free(priv->y2_label);
	priv->y2_label = g_strdup(y2_label);
	g_free(priv->y2_unit);
	priv->y2_unit = g_strdup(y2_unit);
	priv->y2_info_cb = y2_info_cb;
	priv->y2_cb = y2_value_cb;
}

void g_chart_set_x_limits(GChart *self, const float x_min, const float x_max)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->x_min = x_min;
	priv->x_max = x_max;
}

void g_chart_set_y1_limits(GChart *self, const float y1_min, const float y1_max)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->y1_min = y1_min;
	priv->y1_max = y1_max;
}

void g_chart_set_y2_limits(GChart *self, const float y2_min, const float y2_max)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->y2_min = y2_min;
	priv->y2_max = y2_max;
}

void g_chart_set_x_limits_functions(GChart *self, chart_range_value_t x_min_value_cb, chart_range_value_t x_max_value_cb)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->x_min_value_cb = x_min_value_cb;
	priv->x_max_value_cb = x_max_value_cb;
}

void g_chart_set_y1_limits_functions(GChart *self, chart_range_value_t y1_min_value_cb, chart_range_value_t y1_max_value_cb)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->y1_min_value_cb = y1_min_value_cb;
	priv->y1_max_value_cb = y1_max_value_cb;
}

void g_chart_set_y2_limits_functions(GChart *self, chart_range_value_t y2_min_value_cb, chart_range_value_t y2_max_value_cb)
{
	GChartPrivate *priv;
	g_return_if_fail(G_IS_CHART(self));
	priv = g_chart_get_instance_private(self);
	priv->y2_min_value_cb = y2_min_value_cb;
	priv->y2_max_value_cb = y2_max_value_cb;
}

static gboolean _chart_cursor_move_cb(GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	UNUSED(user_data);

	GdkWindow *window;
	GChartPrivate *priv;
	cairo_region_t *r;
	cairo_rectangle_int_t *rect;
	guint width, height;

	priv = g_chart_get_instance_private(G_CHART(widget));

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	if(event->x > priv->offset_left && event->x < width - priv->offset_right && event->y > priv->offset_top && event->y < height - priv->offset_bottom)
		priv->x_info_value = (event->x - priv->offset_left) / priv->x_scale + priv->x_min;
	else
		priv->x_info_value = NAN;

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

inline static void _chart_update_offsets(cairo_t *cr, GChartPrivate *priv)
{
	/* Get expected text width for the top and left. This is based on the labels on the y-axis */
	cairo_text_extents_t extents, extents2;
	gchar *text;
	float info_box_width;

	text = get_value_as_text(0.0, priv->y1_unit, priv->y1_info_cb, priv->user_data);
	cairo_text_extents(cr, text, &extents);
	g_free(text);
	text = NULL;
	extents.width += PADDING;
	extents.height += PADDING;
	priv->offset_left = extents.width + BORDER_OFFSET;
	priv->offset_top = extents.height / 2 + BORDER_OFFSET;
	info_box_width = extents.width;

	/* Get expected text width for the info box */
	cairo_text_extents(cr, _(priv->x_label), &extents);
	extents.width += PADDING;
	info_box_width = MAX(extents.width, info_box_width);

	cairo_text_extents(cr, _(priv->y1_label), &extents);
	extents.width += PADDING;
	info_box_width = MAX(extents.width, info_box_width);

	if(priv->enable_y2)
	{
		cairo_text_extents(cr, _(priv->y2_label), &extents);
		extents.width += PADDING;
		info_box_width = MAX(extents.width, info_box_width);
	}

	/* Get expected text width for bottom and right. This is based on the labels on the y-axis */
	text = get_value_as_text(0.0, priv->x_unit, priv->x_info_cb, priv->user_data);
	cairo_text_extents(cr, text, &extents);
	g_free(text);
	text = NULL;
	if(priv->enable_y2)
	{
		text = get_value_as_text(0.0, priv->y2_unit, priv->y2_info_cb, priv->user_data);
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

static gboolean _chart_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	UNUSED(user_data);

	GChartPrivate *priv = g_chart_get_instance_private(G_CHART(widget));
	if(priv->offset_left == FLT_MIN || priv->offset_right == FLT_MIN || priv->offset_top == FLT_MIN || priv->offset_bottom == FLT_MIN)
		_chart_update_offsets(cr, priv);

	_chart_draw(widget, cr, priv);

	if(!isnan(priv->x_info_value))
		_draw_info(widget, cr, priv);

	return FALSE;
}

inline static void _draw_info(GtkWidget *widget, cairo_t *cr, const GChartPrivate *priv)
{
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
		_print_text(cr, priv->y2_label, width - (priv->info_box_width / 2), 1.5 * h - offset, MIDDLE_BOTTOM, PADDING / 2);
		_print_value_with_unit(cr, priv->y2_cb(priv->x_info_value, priv->user_data), priv->y2_unit, priv->y2_info_cb, priv->user_data, width - (priv->info_box_width / 2), 1.5 * h + offset, MIDDLE_BOTTOM, PADDING / 2);
		cairo_set_source_rgba(cr, 0, 0.6, 0, 1);
		cairo_move_to(cr, width - (priv->info_box_width / 4) * 3, 1.5 * h + offset + 7);
		cairo_line_to(cr, width - (priv->info_box_width / 4), 1.5 * h + offset + 7);
		cairo_stroke(cr);
	}
	else
		h = MAX(height / 2, 3 * PADDING + 2 * extents.height + 13);

	/* Y1 info */
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	_print_text(cr, priv->y1_label, width - (priv->info_box_width / 2), 0.5 * h - offset, MIDDLE_BOTTOM, PADDING / 2);
	_print_value_with_unit(cr, priv->y1_cb(priv->x_info_value, priv->user_data), priv->y1_unit, priv->y1_info_cb, priv->user_data, width - (priv->info_box_width / 2), 0.5 * h + offset, MIDDLE_BOTTOM, PADDING / 2);
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
	_print_text(cr, priv->x_label, width - (priv->info_box_width / 2), n * h - offset, MIDDLE_BOTTOM, PADDING / 2);
	_print_value_with_unit(cr, x_value, priv->x_unit, priv->x_info_cb, priv->user_data, width - (priv->info_box_width / 2), n * h + offset, MIDDLE_BOTTOM, PADDING / 2);

	cairo_fill(cr);
}

inline static void _chart_draw(GtkWidget *widget, cairo_t *cr, GChartPrivate *priv)
{
	float step_size, x_value;
	guint width, height;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	if(priv->x_min_value_cb != NULL)
		priv->x_min = priv->x_min_value_cb(priv->user_data);
	else
		priv->x_min = 0;

	if(priv->x_max_value_cb != NULL)
	{
		priv->x_max = priv->x_max_value_cb(priv->user_data);
		step_size = (priv->x_max - priv->x_min) / priv->n_steps;
	}
	else
	{
		priv->x_min = 0;
		priv->x_max = priv->step_size * priv->n_steps;
		step_size = priv->step_size;
	}

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

	_draw_raster(cr, priv, width, height);

	if(priv->x_min == priv->x_max) return;

	cairo_set_source_rgba(cr, 1, 0, 0, 1);
	cairo_set_dash(cr, NULL, 0, 0);
	_set_line_atributes(cr, 0.5, CAIRO_LINE_JOIN_ROUND, CAIRO_LINE_CAP_ROUND);

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

inline static void _draw_raster(cairo_t *cr, const GChartPrivate *priv, const guint width, const guint height)
{
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

	_set_line_atributes(cr, 1, CAIRO_LINE_JOIN_MITER, CAIRO_LINE_CAP_BUTT);
	cairo_stroke(cr);

	cairo_text_extents(cr, "0", &extents);
	x_lines = (width - priv->offset_left - priv->offset_right) / (extents.width * 14);
	y_lines = (height - priv->offset_top - priv->offset_bottom) / (extents.height * 4);

	for(j = 1; j < x_lines; j++)
	{
		double x = j * ((width - priv->offset_left - priv->offset_right) / x_lines) + priv->offset_left;
		double value = j * ((priv->x_max - priv->x_min) / x_lines) + priv->x_min;
		_draw_sub_line_vertical(cr, value, priv->x_unit, priv->x_info_cb, priv->user_data, x, height - priv->offset_bottom, priv->offset_top);
	}

	for(j = 1; j < y_lines; j++)
	{
		double y = height - priv->offset_bottom - j * ((height - priv->offset_top - priv->offset_bottom) / y_lines);
		double value = j * ((priv->y1_max - priv->y1_min) / y_lines) + priv->y1_min;
		_draw_sub_line_horizontal(cr, value, priv->y1_unit, priv->y1_info_cb, priv->user_data, priv->offset_left, y, width - priv->offset_right);
	}

	/* draw the first and last labels on the X axis */
	_print_value_with_unit(cr, priv->x_min, priv->x_unit, priv->x_info_cb, priv->user_data, priv->offset_left, height - priv->offset_bottom, MIDDLE_TOP, 5);
	_print_value_with_unit(cr, priv->x_max, priv->x_unit, priv->x_info_cb, priv->user_data, width - priv->offset_right, height - priv->offset_bottom, MIDDLE_TOP, 5);

	/* draw the first and last labels on the Y1 axis */
	_print_value_with_unit(cr, priv->y1_min, priv->y1_unit, priv->y1_info_cb, priv->user_data, priv->offset_left, height - priv->offset_bottom, RIGHT_MIDDLE, 5);
	_print_value_with_unit(cr, priv->y1_max, priv->y1_unit, priv->y1_info_cb, priv->user_data, priv->offset_left, priv->offset_top, RIGHT_MIDDLE, 5);

	if(priv->enable_y2)
	{
		/* draw the first and last labels on the Y2 axis */
		_print_value_with_unit(cr, priv->y1_max, priv->y2_unit, priv->y2_info_cb, priv->user_data, width - priv->offset_right, height - priv->offset_bottom, LEFT_MIDDLE, 5);
		_print_value_with_unit(cr, priv->y2_max, priv->y2_unit, priv->y2_info_cb, priv->user_data, width - priv->offset_right, priv->offset_top, LEFT_MIDDLE, 5);
		for(j = 1; j < y_lines; j++)
		{
			double y = height - priv->offset_bottom - j * ((height - priv->offset_top - priv->offset_bottom) / y_lines);
			double value = j * ((priv->y2_max - priv->y2_min) / y_lines) + priv->y2_min;
			_print_value_with_unit(cr, value, priv->y2_unit, priv->y2_info_cb, priv->user_data, width - priv->offset_right, y, LEFT_MIDDLE, 5);
		}
	}

	cairo_set_source_rgba(cr, 1, 0, 0, 0.2);
	cairo_fill(cr);
}

inline static void _draw_sub_line(cairo_t *cr, const double x1, const double y1, const double x2, const double y2)
{
	const double dashes[] = {6.0};
	cairo_set_source_rgba(cr, 0, 0, 0, 0.8);
	_set_line_atributes(cr, 0.25, CAIRO_LINE_JOIN_MITER, CAIRO_LINE_CAP_BUTT);
	cairo_set_dash(cr, dashes, 1, 0);
	cairo_move_to(cr, x1, y1);
	cairo_line_to(cr, x2, y2);
}

inline static void _draw_sub_line_vertical(cairo_t *cr, const double value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data, const double x1, const double y1, const double y2)
{
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	_print_value_with_unit(cr, value, unit, info_cb, user_data, x1, y1, MIDDLE_TOP, 5);
	_draw_sub_line(cr, x1, y1, x1, y2);
	cairo_stroke(cr);
	cairo_fill(cr);
}

inline static void _draw_sub_line_horizontal(cairo_t *cr, const double value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data, const double x1, const double y1, const double x2)
{
	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	_print_value_with_unit(cr, value, unit, info_cb, user_data, x1, y1, RIGHT_MIDDLE, 5);
	_draw_sub_line(cr, x1, y1, x2, y1);
	cairo_stroke(cr);
	cairo_fill(cr);
}

inline static void _set_line_atributes(cairo_t *cr, const double width, const cairo_line_join_t line_join, const cairo_line_cap_t line_cap)
{
	cairo_set_line_width(cr, width);
	cairo_set_line_join(cr, line_join);
	cairo_set_line_cap(cr, line_cap);
}

/**
 * Prints text on the drawing surface.
 * The text position is
 * @param[in] cr
 * @param[in] text	The text to be printed.
 * @param[in] x		X coordinate of the drawing position.
 * @param[in] y		Y coordinate of the drawing position.
 * @param[in] m		Position of the text with respect to the x and y coordinate, must be one of the nine positions, @see text_mode_t.
 * @param[in] padding	Reserve this amount of padding around the text.
 */
static void _print_text(cairo_t *cr, const char *text, const float x, const float y, const text_mode_t m, const float padding)
{
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

static gchar *get_value_as_text(const float value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data)
{
	gchar *text;
	GValue *v;
	if(info_cb != NULL && unit != NULL) /* unit and info_cb are given */
	{
		v = info_cb(value, user_data);
		text = g_strdup_printf("%s %s", g_value_get_string(v), unit);
	}
	else if(info_cb != NULL) /* only info_cb is given */
	{
		v = info_cb(value, user_data);
		text = g_value_dup_string(v);
	}
	else if(unit != NULL) /* only unit is given */
	{
		text = g_strdup_printf("%.3G %s", value, unit);
	}
	else /* none are given */
	{
		text = g_strdup_printf("%.3G", value);
	}
	g_value_unset(v);
	g_slice_free(GValue, v);

	return text;
}

static void _print_value_with_unit(cairo_t *cr, const float value, const char *unit, const chart_value_to_info_string_t info_cb, gconstpointer user_data, const float x, const float y, const text_mode_t m, const float padding)
{
	gchar *text;

	if(isnan(value)) return;
	text = get_value_as_text(value, unit, info_cb, user_data);
	_print_text(cr, text, x, y, m, padding);

	g_free(text);
	text = NULL;
}
