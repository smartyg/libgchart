/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * gchart-draw.cpp
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

#include "Gchart.hpp"

#include <algorithm>
#include <glibmm.h>
#include <gtkmm.h>
#include <cairomm/cairomm.h>

#include "GchartProvider.hpp"

#define PADDING (5)
#define BORDER_OFFSET (PADDING)
#define DOT_RADIUS 2.0

#if _ENABLE_GTK == 4
#define LINECAP_ROUND ROUND
#define LINECAP_BUTT BUTT
#define LINEJOIN_ROUND ROUND
#define LINEJOIN_MITER MITER
#define ARGB32 ARGB32
#elif _ENABLE_GTK == 3
#define LINECAP_ROUND LINE_CAP_ROUND
#define LINECAP_BUTT LINE_CAP_BUTT
#define LINEJOIN_ROUND LINE_JOIN_ROUND
#define LINEJOIN_MITER LINE_JOIN_MITER
#define ARGB32 FORMAT_ARGB32
#endif

Gchart::Gchart (void) : Glib::ObjectBase ("gchart") {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
	this->init = false;
	this->update_buffer = false;
	this->x_mouse_pointer = NAN;
	this->plot_lines = true;
	this->plot_dots = true;

#if _ENABLE_GTK == 4
	m_scroll = Gtk::EventControllerScroll::create ();
	m_move = Gtk::EventControllerMotion::create ();
	m_button = Gtk::EventControllerKey::create ();

	m_scroll->set_flags (Gtk::EventControllerScroll::Flags::HORIZONTAL | Gtk::EventControllerScroll::Flags::VERTICAL);

	this->add_controller(m_scroll);
	this->add_controller(m_move);
	this->add_controller(m_button);

	this->set_draw_func (sigc::mem_fun (*this, &Gchart::onDraw));

	m_scroll->signal_scroll ().connect (sigc::mem_fun (*this, &Gchart::onZoom), false);
	m_move->signal_motion ().connect (sigc::mem_fun (*this, &Gchart::onMouseMove), false);
	m_button->signal_key_pressed ().connect (sigc::mem_fun (*this, &Gchart::onKeyPressed), false);
#endif
#if _ENABLE_GTK == 3
	this->set_events (Gdk::EventMask::POINTER_MOTION_MASK | Gdk::EventMask::BUTTON_PRESS_MASK | Gdk::EventMask::SCROLL_MASK);
	this->signal_draw ().connect (sigc::mem_fun (*this, &Gchart::onDraw_gtk3), false);
	this->signal_scroll_event ().connect (sigc::mem_fun (*this, &Gchart::onZoom_gtk3), false);
	this->signal_motion_notify_event ().connect (sigc::mem_fun (*this, &Gchart::onMouseMove_gtk3), false);
	this->signal_button_press_event ().connect (sigc::mem_fun (*this, &Gchart::onKeyPressed_gtk3), false);
#endif
}

Gchart::~Gchart (void) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
}

sigc::signal<void(const float&)> Gchart::signal_mouse_move (void) {
	return this->_signal_mouse_move;
}

void Gchart::setLabels (const std::string &x_label, const std::string &x_unit, GchartValuePrint x_print, const std::string &y1_label, const std::string &y1_unit, GchartValuePrint y1_print) {
	g_debug("%s:%d %s (%s, %s, %p, %s, %s, %p)", __FILE__, __LINE__, __func__, x_label.c_str(), x_unit.c_str(), x_print, y1_label.c_str(), y1_unit.c_str(), y1_print);
	this->reset (true);
	if (x_print == nullptr) x_print = &GchartLabel::defaultPrint;
	if (y1_print == nullptr) y1_print = &GchartLabel::defaultPrint;
	this->label = std::make_shared<GchartLabel> (x_label, x_unit, x_print);
	this->y1 = std::make_shared<GchartProvider> (y1_label, y1_unit, y1_print);
}

void Gchart::setLabels (const std::string &x_label, const std::string &x_unit, GchartValuePrint x_print, const std::string &y1_label, const std::string &y1_unit, GchartValuePrint y1_print, const std::string &y2_label, const std::string &y2_unit, GchartValuePrint y2_print) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
	this->reset (true);
	if (x_print == nullptr) x_print = &GchartLabel::defaultPrint;
	if (y1_print == nullptr) y1_print = &GchartLabel::defaultPrint;
	if (y2_print == nullptr) y2_print = &GchartLabel::defaultPrint;
	this->label = std::make_shared<GchartLabel> (x_label, x_unit, x_print);
	this->y1 = std::make_shared<GchartProvider> (y1_label, y1_unit, y1_print);
	this->y2 = std::make_shared<GchartProvider> (y2_label, y2_unit, y2_print);
}

bool Gchart::addY1Chart (const GchartChart::Type &t, const int &identifier, const GchartColor &color, const GchartMap chart, GchartGetValue get_value) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
	bool ret = false;
	if (this->y1) {
		ret = this->y1->addChart (t, identifier, color, chart, get_value);
		this->init = true;
	}
	return ret;
}

bool Gchart::addY2Chart (const GchartChart::Type &t, const int &identifier, const GchartColor &color, const GchartMap chart, GchartGetValue get_value) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
	if (this->y2)
		return this->y2->addChart (t, identifier, color, chart, get_value);
	return false;
}

bool Gchart::removeY1Chart (const int &n) {
	g_debug("%s:%d %s (%d)", __FILE__, __LINE__, __func__, n);
	return this->y1->removeChart (n);
}

bool Gchart::removeY2Chart (const int &n) {
	g_debug("%s:%d %s (%d)", __FILE__, __LINE__, __func__, n);
	return this->y2->removeChart (n);
}

bool Gchart::reset (const bool confirm) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
	if (confirm) {
		this->init = false;
		this->zoom = 1.0;
		this->x_center = NAN;
		if (this->y1)
			this->y1->reset (confirm);
		if (this->y2)
			this->y2->reset (confirm);
		return true;
	}
	return false;
}

#if _ENABLE_GTK == 3
bool Gchart::onZoom_gtk3 (const GdkEventScroll *e) {
	return this->onZoom (e->delta_x, e->delta_y);
}
#endif

bool Gchart::onZoom (double dx, double dy) {
	g_debug("%s:%d %s (%lf, %lf)", __FILE__, __LINE__, __func__, dx, dy);
	if (!std::isfinite (this->x_mouse_pointer)) return true;
	if (dy == 0) {
		this->x_center += dx * (this->x_max - this->x_min);
	} else {
		this->zoom += -dy;
		this->x_center = this->x_mouse_pointer;
	}
	this->update_buffer = true;
	this->queue_draw ();
	return true;
}

#if _ENABLE_GTK == 3
bool Gchart::onMouseMove_gtk3 (const GdkEventMotion *e) {
	this->onMouseMove (e->x, e->y);
	return true;
}
#endif

void Gchart::onMouseMove (const double &x_coord, const double &y_coord) {
	g_debug("%s:%d %s (%lf, %lf)", __FILE__, __LINE__, __func__, x_coord, y_coord);
	float x;
	if (this->inDrawingBox (x_coord, y_coord)) {
		x = this->x_min + ((x_coord - this->offset_left) / this->x_scale);
		if (x != this->x_mouse_pointer) {
			this->x_mouse_pointer = x;
			this->queue_draw ();
			this->_signal_mouse_move.emit(x);
		}
	}
	return;
}

#if _ENABLE_GTK == 3
bool Gchart::onKeyPressed_gtk3 (const GdkEventButton *e) {
	(void)e;
	return true;
}
#endif

bool Gchart::inDrawingBox (const double &x, const double &y) const {
	if (x >= this->offset_left && x <= (this->get_allocated_width () - this->offset_right) && y >= this->offset_top && y <= (this->get_allocated_height () - this->offset_bottom)) return true;
	else return false;
}

bool Gchart::onKeyPressed (guint keyval, guint keycode, Gdk::ModifierType state) {
	g_debug("%s:%d %s (%d, %d, %d)", __FILE__, __LINE__, __func__, keyval, keycode, static_cast<int>(state));
	return true;
}

#if _ENABLE_GTK == 3
bool Gchart::onDraw_gtk3 (const Cairo::RefPtr<Cairo::Context>& cr) {
	this->onDraw (cr, 0, 0);
	return true;
}
#endif

void Gchart::onDraw (const Cairo::RefPtr<Cairo::Context>& cr, int width, int height) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
	if (!this->init) return;

	width = this->get_allocated_width ();
	height = this->get_allocated_height ();

	if (this->update_buffer || this->buffered_width != width || this->buffered_height != height) {
		Cairo::RefPtr<Cairo::Surface> ref_surface = cr->get_target ();
		this->buffer = Cairo::Surface::create (ref_surface, ref_surface->get_content (), width, height);
		this->drawBuffer (this->buffer);
		this->update_buffer = false;
		this->buffered_width = width;
		this->buffered_height = height;
	}
	cr->set_source (this->buffer, 0, 0);
	cr->paint ();

	if (std::isfinite (this->x_mouse_pointer)) {
		cr->set_source_rgba (0.3, 0.3, 0.3, 0.4);
		Gchart::setLineAtributes (cr, 1.0, CAIRO_ENUM_NS_CONTEXT::LineJoin::LINEJOIN_MITER, CAIRO_ENUM_NS_CONTEXT::LineCap::LINECAP_BUTT);
		cr->move_to (this->getXCoord (this->x_mouse_pointer), this->offset_top);
		cr->line_to (this->getXCoord (this->x_mouse_pointer), height - this->offset_bottom);
		cr->stroke ();
		this->drawInfo (cr, width, height, this->x_mouse_pointer);
	}
	return;
}

void Gchart::calulateOffsets (const int &width, const int &height) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	/* Get expected text width for the top and left. This is based on the labels on the y-axis */
	Cairo::TextExtents extents, extents2;
	std::string text;
	float info_box_width;

	// Create a temporary surface to calculate the text sizes.
	auto surface = Cairo::ImageSurface::create (CAIRO_ENUM_NS_SURFACE::Format::ARGB32, width, height);
	auto layer = Cairo::Context::create (surface);

	text = this->y1->getLabel ()->getValueUnitText (0.0);
	layer->get_text_extents (text, extents);
	text.clear();
	extents.width += PADDING;
	extents.height += PADDING;
	this->offset_left = extents.width + BORDER_OFFSET;
	this->offset_top = extents.height / 2 + BORDER_OFFSET;
	info_box_width = extents.width;

	/* Get expected text width for the info box */
	layer->get_text_extents (this->label->getLabel (), extents);
	extents.width += PADDING;
	info_box_width = MAX(extents.width, info_box_width);

	layer->get_text_extents (this->y1->getLabel ()->getLabel (), extents);
	extents.width += PADDING;
	info_box_width = MAX(extents.width, info_box_width);

	if(this->y2) {
		layer->get_text_extents (this->y2->getLabel ()->getLabel (), extents);
		extents.width += PADDING;
		info_box_width = MAX(extents.width, info_box_width);
	}

	/* Get expected text width for bottom and right. This is based on the labels on the x-axis */
	text = this->label->getValueUnitText (0.0);

	layer->get_text_extents (text, extents);
	text.clear ();
	if(this->y2) {
		text = this->y2->getLabel ()->getValueUnitText (0.0);
		layer->get_text_extents (text, extents2);
		text.clear ();
		extents2.width += PADDING;
		info_box_width = MAX(extents2.width, info_box_width);
	} else
		extents2.width = 0;
	extents.width += PADDING;
	extents.height += PADDING;
	info_box_width = MAX(extents.width, info_box_width);
	this->offset_bottom = extents.height + BORDER_OFFSET;
	this->infobox_width = info_box_width + 10 * BORDER_OFFSET;
	this->offset_right = extents.width / 2 + extents2.width + this->infobox_width + BORDER_OFFSET;
}

void Gchart::drawInfo (const Cairo::RefPtr<Cairo::Context>& layer, const int &width, const int &height, const float &x_info_value) const {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	float h, offset, n;
	Cairo::TextExtents extents;

	layer->set_font_size (15);
	layer->get_text_extents (this->y1->getLabel ()->getLabel (), extents);
	offset = extents.height / 2 + PADDING;

	n = 1.5;
	double h_min_y1 = PADDING + extents.height + this->y1->size () * (extents.height + 13 + 2 * PADDING);

	if (this->y2)
	{
		double h_min_y2 = PADDING + extents.height + this->y2->size () * (extents.height + 13 + 2 * PADDING);
		h = std::max (static_cast<double>(height) / 3, h_min_y2);
		n = 2.5;

		/* Y2 info */
		layer->set_source_rgba (0, 0, 0, 1);
		float h_actual = 1.5 * h - (h_min_y2 / 2);
		Gchart::printText (layer, this->y2->getLabel ()->getLabel (), width - (this->infobox_width / 2), h_actual, MIDDLE_BOTTOM, PADDING / 2);
		h_actual += extents.height + PADDING;
		for (const GchartChart &c : *(this->y2.get ())) {
			const GchartColor& color = c.getColor ();
			float y_value = c.getValue (x_info_value);
			layer->set_source_rgba(0, 0, 0, 1);
			Gchart::printText2 (layer, y_value, this->y2->getLabel (), width - (this->infobox_width / 2), h_actual, MIDDLE_BOTTOM, PADDING / 2);
			layer->set_source_rgba (color._red, color._green, color._blue, color._alpha);
			layer->move_to (width - (this->infobox_width / 4) * 3, h_actual + 7);
			layer->line_to (width - (this->infobox_width / 4), h_actual + 7);
			layer->stroke ();
			h_actual += extents.height + 13 + 2 * PADDING;
		}
	} else
		h = std::max (static_cast<double>(height) / 2, h_min_y1);

	/* Y1 info */
	layer->set_source_rgba (0, 0, 0, 1);
	float h_actual = 0.5 * h - (h_min_y1 / 2);
	Gchart::printText (layer, this->y1->getLabel ()->getLabel (), width - (this->infobox_width / 2), h_actual, MIDDLE_BOTTOM, PADDING / 2);
	h_actual += extents.height + PADDING;
	for (const GchartChart &c : *(this->y1.get ())) {
		const GchartColor& color = c.getColor ();
		float y_value = c.getValue (x_info_value);
		layer->set_source_rgba(0, 0, 0, 1);
		Gchart::printText2 (layer, y_value, this->y1->getLabel (), width - (this->infobox_width / 2), h_actual, MIDDLE_BOTTOM, PADDING / 2);
		layer->set_source_rgba (color._red, color._green, color._blue, color._alpha);
		layer->move_to (width - (this->infobox_width / 4) * 3, h_actual + 7);
		layer->line_to (width - (this->infobox_width / 4), h_actual + 7);
		layer->stroke ();
		h_actual += extents.height + 13 + 2 * PADDING;
	}

	/* X info */
	layer->set_source_rgba (0, 0, 0, 1);
	Gchart::printText (layer, this->label->getLabel (), width - (this->infobox_width / 2), n * h - offset, MIDDLE_BOTTOM, PADDING / 2);
	Gchart::printText2 (layer, x_info_value, this->label, width - (this->infobox_width / 2), n * h + offset, MIDDLE_BOTTOM, PADDING / 2);

	layer->fill ();
}

void Gchart::drawBuffer (Cairo::RefPtr<Cairo::Surface> surface) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);
	int width, height;
	int x_lines;

	if (!this->init) return;

	width = this->get_allocated_width ();
	height = this->get_allocated_height ();

	this->calulateOffsets (width, height);

	this->calculateMinMaxValues (width, height);

	auto layer = Cairo::Context::create (surface);

	this->drawRaster (layer, width, height, x_lines);

	std::vector<double> dashes;
	layer->set_dash(dashes, 0);
	this->setLineAtributes (layer, 1.0, CAIRO_ENUM_NS_CONTEXT::LineJoin::LINEJOIN_ROUND, CAIRO_ENUM_NS_CONTEXT::LineCap::LINECAP_ROUND);
	//layer->set_source_rgba (1, 0, 0, 1);
	this->drawChart (layer, this->y1, height, (static_cast<float>(x_lines) / 10));
	if (this->y2) {
		//layer->set_source_rgba (0, 0.6, 0, 1);
		drawChart (layer, this->y2, height, (static_cast<float>(x_lines) / 10));
	}
}

void Gchart::drawChart (const Cairo::RefPtr<Cairo::Context>& layer, const std::shared_ptr<GchartProvider> &y, const int &height, const float &x_hint = NAN) const {
	g_debug("%s:%d %s (-, -, %d, %f)", __FILE__, __LINE__, __func__, height, x_hint);

	for (const GchartChart &c : *(y.get ())) {
		const GchartColor& color = c.getColor ();
		std::shared_ptr<GchartPoint> point, point_prev;

		layer->begin_new_path ();
		layer->set_source_rgba (color._red, color._green, color._blue, color._alpha);
		point = c.getPoint (this->x_min);
		point_prev = point;
		this->drawPoint (layer, y, point, height);

		while ((point = c.getNextPoint (point_prev, point_prev->getX () + x_hint)) != nullptr) {
			point_prev = point;

			if (point->getX () < this->x_min) break;
			if (point->getX () > this->x_max) break;
			if (!std::isfinite (point->getX ()) || !std::isfinite (point->getY ())) continue;

			this->drawPoint (layer, y, point, height);
		}

		point = c.getPoint (this->x_max);
		this->drawPoint (layer, y, point, height);
	}
}

void Gchart::drawPoint (const Cairo::RefPtr<Cairo::Context>& layer, const std::shared_ptr<GchartProvider> &y, const std::shared_ptr<GchartPoint> &point, const int &height) const {
	g_debug("%s:%d %s (%f, %f)", __FILE__, __LINE__, __func__, point->getX (), point->getY ());

	double x_coord = this->getXCoord (point->getX ());
	double y_coord = this->getYCoord (point->getY (), y);
	if (this->plot_lines) {
		g_debug("draw line to %f, %f", x_coord, height - y_coord);
		layer->line_to (x_coord, height - y_coord);
		layer->stroke();
	}
	layer->move_to (x_coord, height - y_coord);
	if (this->plot_dots) {
		layer->arc (x_coord, height - y_coord, DOT_RADIUS, 0, 2 * M_PI);
		layer->fill();
		layer->stroke ();
	}
	layer->move_to (x_coord, height - y_coord);
}

double Gchart::getXCoord (const float &x) const {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	if (!std::isfinite (x)) return this->offset_left;
	return this->offset_left + ((x - this->x_min) * this->x_scale);
}

double Gchart::getYCoord (const float &y, const std::shared_ptr<GchartProvider> &y_provider) const {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	if (!std::isfinite (y)) return this->offset_bottom;
	return this->offset_bottom + ((y - y_provider->_y_min) * y_provider->_y_scale);
}

void Gchart::calculateMinMaxValues (const int &width, const int &height) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	float x_min_data, x_max_data, x_span_zoom;
	// TODO: also check this->y2
	x_min_data = this->y1->getXMin ();
	x_max_data = this->y1->getXMax ();

	/* If zoom is bigger than 1.0 then adjust the minimum and maximum x value to it. */
	if (std::isfinite (this->zoom) && this->zoom > 1.0f) {
		x_span_zoom = (x_max_data - x_min_data) / ( 2 * this->zoom);
		/* If x_center is not set, set it to the middle of the real window. */
		if (!std::isfinite (this->x_center)) {
			this->x_center = (x_max_data + x_min_data) /  2;
		} else {
			/* If x_center was set, check if it is within the minimum and maximum. */
			this->x_center = MAX(x_min_data + x_span_zoom, this->x_center);
			this->x_center = MIN(x_max_data - x_span_zoom, this->x_center);
		}
		this->x_max = this->x_center + x_span_zoom;
		this->x_min = this->x_center - x_span_zoom;
	} else {
		this->zoom = 1.0;
		this->x_max = x_max_data;
		this->x_min = x_min_data;
	}

	this->x_scale = (width - this->offset_left - this->offset_right) / (this->x_max - this->x_min);

	this->y1->_y_min = this->y1->getYMin (this->x_min, this->x_max);
	this->y1->_y_max = this->y1->getYMax (this->x_min, this->x_max);
	this->y1->_y_scale = (height - this->offset_top - this->offset_bottom) / (this->y1->_y_max - this->y1->_y_min);

	if (this->y2) {
		this->y2->_y_min = this->y2->getYMin (this->x_min, this->x_max);
		this->y2->_y_max = this->y2->getYMax (this->x_min, this->x_max);
		this->y2->_y_scale = (height - this->offset_top - this->offset_bottom) / (this->y2->_y_max - this->y2->_y_min);
	}

	return;
}

void Gchart::drawRaster (const Cairo::RefPtr<Cairo::Context>& layer, const int &width, const int &height, int &x_lines) const {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	Cairo::TextExtents extents;
	int y_lines;

	/* fill the background */
	layer->set_source_rgb (1, 1, 1);
	layer->rectangle (0, 0, width, height);
	layer->fill ();

	/* x and y1 axis */
	layer->set_source_rgb (0, 0, 0);
	layer->move_to (this->offset_left, this->offset_top);
	/* y1-axis */
	layer->line_to (this->offset_left, height - this->offset_bottom);
	/* x-axis */
	layer->line_to (width - this->offset_right, height - this->offset_bottom);
	/* y2-axis */
	if (this->y2)
		layer->line_to(width - this->offset_right, this->offset_top);

	Gchart::setLineAtributes (layer, 1.0, CAIRO_ENUM_NS_CONTEXT::LineJoin::LINEJOIN_MITER, CAIRO_ENUM_NS_CONTEXT::LineCap::LINECAP_BUTT);
	layer->stroke ();

	layer->get_text_extents ("0", extents);
	x_lines = (width - this->offset_left - this->offset_right) / (extents.width * 14);
	y_lines = (height - this->offset_top - this->offset_bottom) / (extents.height * 4);

	for (int j = 1; j < x_lines; j++) {
		double x = j * ((width - this->offset_left - this->offset_right) / x_lines) + this->offset_left;
		double value = j * ((this->x_max - this->x_min) / x_lines) + this->x_min;
		verticalSubLine (layer, value, this->label, x, height - this->offset_bottom, this->offset_top);
	}

	for(int j = 1; j < y_lines; j++) {
		double y = height - this->offset_bottom - j * ((height - this->offset_top - this->offset_bottom) / y_lines);
		double value = j * ((this->y1->_y_max - this->y1->_y_min) / y_lines) + this->y1->_y_min;
		horizontalSubLine (layer, value, this->y1->getLabel (), this->offset_left, y, width - this->offset_right);
	}

	/* draw the first and last labels on the X axis */
	this->printText2 (layer, this->x_min, this->label, this->offset_left, height - this->offset_bottom, MIDDLE_TOP, 5);
	this->printText2 (layer, this->x_max, this->label, width - this->offset_right, height - this->offset_bottom, MIDDLE_TOP, 5);

	/* draw the first and last labels on the Y1 axis */
	this->printText2 (layer, this->y1->_y_min, this->y1->getLabel (), this->offset_left, height - this->offset_bottom, RIGHT_MIDDLE, 5);
	this->printText2 (layer, this->y1->_y_max, this->y1->getLabel (), this->offset_left, this->offset_top, RIGHT_MIDDLE, 5);

	if (this->y2) {
		/* draw the first and last labels on the Y2 axis */
		this->printText2 (layer, this->y2->_y_min, this->y2->getLabel (), width - this->offset_right, height - this->offset_bottom, LEFT_MIDDLE, 5);
		this->printText2 (layer, this->y2->_y_max, this->y2->getLabel (), width - this->offset_right, this->offset_top, LEFT_MIDDLE, 5);

		for(int j = 1; j < y_lines; j++) {
			double y = height - this->offset_bottom - j * ((height - this->offset_top - this->offset_bottom) / y_lines);
			double value = j * ((this->y2->_y_max - this->y2->_y_min) / y_lines) + this->y2->_y_min;
			this->printText2 (layer, value, this->y2->getLabel (), width - this->offset_right, y, LEFT_MIDDLE, 5);
		}
	}

	layer->set_source_rgba (1, 0, 0, 0.2);
	layer->fill ();
}

void Gchart::setLineAtributes (const Cairo::RefPtr<Cairo::Context>& layer, const double &width, const CAIRO_ENUM_NS_CONTEXT::LineJoin &line_join, const CAIRO_ENUM_NS_CONTEXT::LineCap &line_cap) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	layer->set_line_width (width);
	layer->set_line_join (line_join);
	layer->set_line_cap (line_cap);
}

void Gchart::drawSubLine (const Cairo::RefPtr<Cairo::Context>& layer, const double &x1, const double &y1, const double &x2, const double &y2) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	std::vector<double> dashes = {6.0};
	layer->set_source_rgba (0, 0, 0, 0.8);
	Gchart::setLineAtributes (layer, 0.25, CAIRO_ENUM_NS_CONTEXT::LineJoin::LINEJOIN_MITER, CAIRO_ENUM_NS_CONTEXT::LineCap::LINECAP_BUTT);
	layer->set_dash(dashes, 0);
	layer->move_to(x1, y1);
	layer->line_to(x2, y2);
}

void Gchart::verticalSubLine (const Cairo::RefPtr<Cairo::Context>& layer, const double &value, const std::shared_ptr<GchartLabel> &label, const double &x1, const double &y1, const double &y2) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	layer->set_source_rgba (0, 0, 0, 1);
	Gchart::printText2 (layer, value, label, x1, y1, MIDDLE_TOP, 5);
	Gchart::drawSubLine (layer, x1, y1, x1, y2);
	layer->stroke ();
	layer->fill ();
}

void Gchart::horizontalSubLine (const Cairo::RefPtr<Cairo::Context>& layer, const double &value, const std::shared_ptr<GchartLabel> &label, const double &x1, const double &y1, const double &x2) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	layer->set_source_rgba (0, 0, 0, 1);
	Gchart::printText2 (layer, value, label, x1, y1, RIGHT_MIDDLE, 5);
	Gchart::drawSubLine (layer, x1, y1, x2, y1);
	layer->stroke ();
	layer->fill ();
}

void Gchart::printText2 (const Cairo::RefPtr<Cairo::Context>& layer, const float &value, const std::shared_ptr<GchartLabel> &label, const float &x, const float &y, const AllignMode &m, const float &padding) {
	Gchart::printText (layer, label->getValueUnitText (value), x, y, m, padding);
}

void Gchart::printText (const Cairo::RefPtr<Cairo::Context>& layer, const std::string &text, const float &x, const float &y, const AllignMode &m, const float &padding) {
	g_debug("%s:%d %s ()", __FILE__, __LINE__, __func__);

	float x_new, y_new;
	Cairo::TextExtents extents;

	layer->get_text_extents (text, extents);
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

	layer->move_to (x_new + padding, y_new - padding);
	layer->show_text (text);
}

#if _ENABLE_GTK == 3
// Glade code
GType Gchart::gtype = 0;

Gchart::Gchart (GtkDrawingArea *gobj) : Gtk::DrawingArea (gobj) {
}

Glib::ObjectBase *Gchart::wrap_new (GObject *o) {
	if (gtk_widget_is_toplevel (GTK_WIDGET (o))) {
		return new Gchart (GTK_DRAWING_AREA (o));
	} else {
		return Gtk::manage(new Gchart (GTK_DRAWING_AREA (o)));
	}
}

void Gchart::register_type (void) {
	if (gtype)
		return;

	Gchart dummy;

	GtkWidget *widget = GTK_WIDGET (dummy.gobj ());

	gtype = G_OBJECT_TYPE (widget);

	Glib::wrap_register (gtype, Gchart::wrap_new);
}
#endif