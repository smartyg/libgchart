/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * Gchart.hpp
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

#ifndef __GCHART_HPP__
#define __GCHART_HPP__

#include <gtkmm.h>
#include <cairomm/cairomm.h>

#include "GchartColor.hpp"
#include "GchartLabel.hpp"
#include "GchartPoint.hpp"
#include "GchartChart.hpp"
#include "GchartProvider.hpp"

#if _ENABLE_GTK == 4
#define CAIRO_ENUM_NS_CONTEXT Cairo::Context
#define CAIRO_ENUM_NS_SURFACE Cairo::Surface
#elif _ENABLE_GTK == 3
#define CAIRO_ENUM_NS_CONTEXT Cairo
#define CAIRO_ENUM_NS_SURFACE Cairo
#endif

class Gchart : public Gtk::DrawingArea {
private:
	std::shared_ptr<GchartProvider> y1, y2;

	float offset_left, offset_right, offset_top, offset_bottom, infobox_width;
	float x_max, x_min, x_scale;
	float zoom, x_center;
	float x_mouse_pointer;

	bool plot_lines, plot_dots;
	bool update_buffer, init;

	std::shared_ptr<GchartLabel> label;
	int buffered_width, buffered_height;

	Cairo::RefPtr<Cairo::Surface> buffer;

#if _ENABLE_GTK == 4
	Glib::RefPtr<Gtk::EventControllerScroll> m_scroll;
	Glib::RefPtr<Gtk::EventControllerMotion> m_move;
	Glib::RefPtr<Gtk::EventControllerKey> m_button;
#elif _ENABLE_GTK == 3
	// Code to make Glade work
	static GType gtype;
	Gchart (GtkDrawingArea *gobj);
	static Glib::ObjectBase *wrap_new (GObject* o);
#endif

	sigc::signal<void(const float&)> _signal_mouse_move;

public:
	Gchart (void);
	~Gchart (void);

	void setLabels (const std::string &x_label, const std::string &x_unit, GchartValuePrint x_print,
					const std::string &y1_label, const std::string &y1_unit, GchartValuePrint y1_print);
	void setLabels (const std::string &x_label, const std::string &x_unit, GchartValuePrint x_print,
					const std::string &y1_label, const std::string &y1_unit, GchartValuePrint y1_print,
					const std::string &y2_label, const std::string &y2_unit, GchartValuePrint y2_print);

	bool addY1Chart (const GchartChart::Type &t, const int &identifier, const GchartColor &color, const GchartMap chart, GchartGetValue get_value);
	bool addY2Chart (const GchartChart::Type &t, const int &identifier, const GchartColor &color, const GchartMap chart, GchartGetValue get_value);
	bool removeY1Chart (const int &n);
	bool removeY2Chart (const int &n);
	bool reset (const bool confirm = false);

	sigc::signal<void(const float&)> signal_mouse_move (void);

	static void register_type (void);

protected:
	enum AllignMode {
		RIGHT_BOTTOM = 0, // default
		MIDDLE_TOP = 1,
		MIDDLE_MIDDLE,
		MIDDLE_BOTTOM,
		LEFT_TOP,
		LEFT_MIDDLE,
		LEFT_BOTTOM,
		RIGHT_TOP,
		RIGHT_MIDDLE
	};

#if _ENABLE_GTK == 3
	bool onDraw_gtk3 (const Cairo::RefPtr<Cairo::Context>& cr);
	bool onZoom_gtk3 (const GdkEventScroll *e);
	bool onMouseMove_gtk3 (const GdkEventMotion *e);
	bool onKeyPressed_gtk3 (const GdkEventButton *e);
#endif

	bool onZoom (double dx, double dy);
	void onMouseMove (const double &x_coord, const double &y_coord);
	bool onKeyPressed (guint keyval, guint keycode, Gdk::ModifierType state);

	bool inDrawingBox (const double &x, const double &y) const;

	void onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
	void calulateOffsets (const int &width, const int &height);
	void drawInfo (const Cairo::RefPtr<Cairo::Context>& layer, const int &width, const int &height, const float &x_info_value) const;
	void drawBuffer (Cairo::RefPtr<Cairo::Surface> surface);
	void drawChart (const Cairo::RefPtr<Cairo::Context>& layer, const std::shared_ptr<GchartProvider> &y, const int &height, const float &x_hint) const;
	void drawPoint (const Cairo::RefPtr<Cairo::Context>& layer, const std::shared_ptr<GchartProvider> &y, const std::shared_ptr<GchartPoint> &point, const int &height) const;

	double getXCoord (const float &x) const;
	double getYCoord (const float &y, const std::shared_ptr<GchartProvider> &y_provider) const;
	void calculateMinMaxValues (const int &width, const int &height);
	void drawRaster (const Cairo::RefPtr<Cairo::Context>& layer, const int &width, const int &height, int &x_lines) const;

	static void setLineAtributes (const Cairo::RefPtr<Cairo::Context>& layer, const double &width, const CAIRO_ENUM_NS_CONTEXT::LineJoin &line_join, const CAIRO_ENUM_NS_CONTEXT::LineCap &line_cap);
	static void drawSubLine (const Cairo::RefPtr<Cairo::Context>& layer, const double &x1, const double &y1, const double &x2, const double &y2);
	static void verticalSubLine (const Cairo::RefPtr<Cairo::Context>& layer, const double &value, const std::shared_ptr<GchartLabel> &label, const double &x1, const double &y1, const double &y2);
	static void horizontalSubLine (const Cairo::RefPtr<Cairo::Context>& layer, const double &value, const std::shared_ptr<GchartLabel> &label, const double &x1, const double &y1, const double &x2);
	static void printText2 (const Cairo::RefPtr<Cairo::Context>& layer, const float &value, const std::shared_ptr<GchartLabel> &label, const float &x, const float &y, const Gchart::AllignMode &m, const float &padding);
	static void printText (const Cairo::RefPtr<Cairo::Context>& layer, const std::string &text, const float &x, const float &y, const Gchart::AllignMode &m, const float &padding);
};

#endif /* __GCHART_HPP__ */