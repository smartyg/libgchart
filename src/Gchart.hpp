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

#include <memory>
#include <string>
#include <gtkmm.h>
#include <cairomm/cairomm.h>

#include "GchartColor.hpp"
#include "GchartLabel.hpp"
#include "GchartPoint.hpp"
#include "GchartChart.hpp"
#include "GchartProvider.hpp"

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
	Glib::RefPtr<Gtk::EventControllerScroll> m_scroll;
	Glib::RefPtr<Gtk::EventControllerMotion> m_move;
	Glib::RefPtr<Gtk::EventControllerKey> m_button;

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

	bool onZoom (double dx, double dy);
	void onMouseMove (double x, double y);
	bool onKeyPressed (guint keyval, guint keycode, Gdk::ModifierType state);

	void onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
	void calulateOffsets (const int &width, const int &height);
	void drawInfo (Cairo::RefPtr<Cairo::Context> layer, const int &width, const int &height, const float &x_info_value, const float &y1_info_value, const float &y2_info_value);
	void drawBuffer (Cairo::RefPtr<Cairo::Surface> surface);
	void drawChart (Cairo::RefPtr<Cairo::Context> layer, const std::shared_ptr<GchartProvider> &y, const int &height, const float &x_hint) const;
	void drawPoint (Cairo::RefPtr<Cairo::Context> layer, const std::shared_ptr<GchartProvider> &y, const std::shared_ptr<GchartPoint> &point, const int &height) const;

	double getXCoord (const float &x) const;
	double getYCoord (const float &y, const std::shared_ptr<GchartProvider> &y_provider) const;
	void calculateMinMaxValues (const int &width, const int &height);
	void drawRaster (Cairo::RefPtr<Cairo::Context> layer, const int &width, const int &height, int &x_lines);

	static void setLineAtributes (Cairo::RefPtr<Cairo::Context> layer, const double &width, const Cairo::Context::LineJoin &line_join, const Cairo::Context::LineCap &line_cap);
	static void drawSubLine (Cairo::RefPtr<Cairo::Context> layer, const double &x1, const double &y1, const double &x2, const double &y2);
	static void verticalSubLine (Cairo::RefPtr<Cairo::Context> layer, const double &value, const std::shared_ptr<GchartLabel> &label, const double &x1, const double &y1, const double &y2);
	static void horizontalSubLine (Cairo::RefPtr<Cairo::Context> layer, const double &value, const std::shared_ptr<GchartLabel> &label, const double &x1, const double &y1, const double &x2);
	static void printText2 (Cairo::RefPtr<Cairo::Context> layer, const float &value, const std::shared_ptr<GchartLabel> &label, const float &x, const float &y, const Gchart::AllignMode &m, const float &padding);
	static void printText (Cairo::RefPtr<Cairo::Context> layer, const std::string &text, const float &x, const float &y, const Gchart::AllignMode &m, const float &padding);
};

#endif /* __GCHART_HPP__ */