/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * gchart-provider.h
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

#ifndef __G_CHART_PROVIDER_H__
#define __G_CHART_PROVIDER_H__

#include <features.h>
/*
#include <glib.h>
#include <cairo.h>
#include <gtk/gtk.h>

#include "gchart.h"
#include "gchart-internal.h"
*/
#include <memory>
#include <string>
#include <stdexcept>
#include <forward_list>
#include <iostream>

#include "Gchart.hpp"
#include "helper.hpp"

//#include "gchart.h"

#define FLAG_FREE (1 << 0)
#define FLAG_INTERPOLATE (1 << 1)

#define IS_FLAG_SET(flags, f) (((flags) & (f)) == (f))

struct GchartColor {
	const double _red;
	const double _green;
	const double _blue;
	const double _alpha;

	GchartColor (const double red, const double blue, const double green) : _red(red), _green(green), _blue(blue), _alpha(1.0) {};
	GchartColor (const double red, const double blue, const double green, const double alpha) : _red(red), _green(green), _blue(blue), _alpha(alpha) {};
};

class GchartPoint {
private:
	const float _x;
	const float _y;
	const int _flags;
	GchartMap::const_iterator _it;

public:
	GchartPoint (const float x, const float y, GchartMap::const_iterator it) : _x(x), _y(y), _flags(0), _it(it) {};
	//GchartPoint (const float x, const float y, const int flags) : _x(x), _y(y), _flags(flags) {};
	GchartPoint (const float x, const float y, const int flags, GchartMap::const_iterator it) : _x(x), _y(y), _flags(flags), _it(it) {};
	~GchartPoint (void) {}

	const float& getX (void) const {
		return this->_x;
	}

	const float& getY (void) const {
		return this->_y;
	}

	const int& getFlags (void) const {
		return this->_flags;
	}

	GchartMap::const_iterator getIterator (void) const {
		return this->_it;
	}
};

class GchartLabel {
private:
	const std::string _label;
	const std::string _unit;
	GchartValuePrint _unit_value_cb;

public:
	GchartLabel (const std::string label, const std::string unit, GchartValuePrint unit_value_cb = &GchartLabel::defaultPrint) : _label(label), _unit(unit), _unit_value_cb(unit_value_cb) {};
	~GchartLabel (void) {};

	const std::string& getLabel (void) const {
		return this->_label;
	}

	const std::string& getUnit (void) const {
		return this->_unit;
	}

	const std::string getValueUnitText (const float& value) const {
		return this->_unit_value_cb (this, value);
	}

	static const std::string defaultPrint (const GchartLabel *self, const float &value) {
		//char *str = asprintf ("%0.2f %s", value, self.getUnit ());
		return string_format ("%0.2f %s", value, self->getUnit ().c_str());
	}
};

class GchartChart {
private:
	GchartGetValue _get_value;
	const GchartMap _map;
	void *_user_data;
	const int _identifier;
	const GchartColor _color;

public:
	enum Type {
		LINEAR = 1,
		CURVE_2,
		CURVE_3,
		CURVE_4,
		CURVE_5,
		CUSTOM
	};
	//typedef GchartChartType Type;

	// For linear inerpolation
	GchartChart (const int identifier, const GchartColor &color, const GchartMap map) : _identifier(identifier), _color(color), _map(map), _get_value(&GchartChart::linear), _user_data(nullptr) {};

	// for curved chart
	GchartChart (const GchartChart::Type &t, const int identifier, const GchartColor &color, const GchartMap map, GchartGetValue cb = nullptr, void *user_data = nullptr) : _identifier(identifier), _color(color), _map(map) {
		switch (t) {
			case Type::LINEAR:
				this->_get_value = &GchartChart::linear;
				this->_user_data = nullptr;
				break;/*
			case Type::CURVE_2:
				this->_get_value = &GchartChart::curved2;
				this->_user_data = nullptr;
				break;
			case Type::CURVE_3:
				this->_get_value = &GchartChart::curved2;
				this->_user_data = nullptr;
				break;
			case Type::CURVE_4:
				this->_get_value = &GchartChart::curved2;
				this->_user_data = nullptr;
				break;
			case Type::CURVE_5:
				this->_get_value = &GchartChart::curved2;
				this->_user_data = nullptr;
				break;*/
			case Type::CUSTOM:
			default:
				this->_get_value = cb;
				this->_user_data = user_data;
		}
	}

	~GchartChart (void) {}

	const float& operator[] (std::size_t idx) const {
		auto it = this->_map.begin ();
		for (int i = 0; i < idx; i++) {
			++it;
		}
		return (*it).second;
	}

	const float getValue (const float &x) const {
		auto it = this->_map.end ();
		float x_hint = x;
		float y = this->_get_value (this->_map, x_hint, it);
		if (x_hint == x) return y;
		return NAN;
	}
private:
	const float getValue (float &x, GchartMap::const_iterator &it) const {
		return this->_get_value (this->_map, x, it);
	}

public:
/*
	const GchartPoint *getPoint (const float &x) const {
		const float y = this->getValue (x);
		GchartPoint *p = new GchartPoint (x, y, FLAG_FREE);
		return p;
	}
*/
	const std::shared_ptr<GchartPoint> getPoint (const float &x) const {
		const float y = this->getValue (x);
		return std::make_shared<GchartPoint>(x, y, this->_map.end ());
	}

	const int getIdentifier (void) const {
		return this->_identifier;
	}

	const std::shared_ptr<GchartPoint> getNextPoint (const std::shared_ptr<GchartPoint> &prev, const float &x_hint) const {
		GchartMap::const_iterator it = prev->getIterator ();
		float x = x_hint;
		const float y = this->getValue (x, it);
		return std::make_shared<GchartPoint>(x, y, 0, it);
	}
/*
	const std::size count (void) const {
		return this->_map.count ();
	}
*/
	const auto end (void) const noexcept {
		return this->_map.end ();
	}

	const auto begin (void) const noexcept {
		return this->_map.begin ();
	}

	const auto last (void) const {
		auto it = this->_map.end ();
		return --it;
	}

	const GchartColor& getColor (void) const {
		return this->_color;
	}

	static const float linear (const GchartMap &map, float &x, GchartMap::const_iterator &it) {
		if (it != map.end ())
			g_debug("%s:%d %s (-, %f, {%f, %f})", __FILE__, __LINE__, __func__, x, (*it).first, (*it).second);
		else
			g_debug("%s:%d %s (-, %f, -)", __FILE__, __LINE__, __func__, x);
		float x1 = NAN;
		float y1 = NAN;
		if (it != map.end ()) {
			++it;
			if (it == map.end ()) {
				it = map.begin ();
			} else if ((*it).first < x) {
				x = (*it).first;
				return (*it).second;
			}
		} else
			it = map.begin ();
		//for (auto const& v : map) {
		for (; it != map.end (); ++it) {
			const auto &v = *it;
			if (v.first == x) {
				return v.second;
			} else if (v.first < x) {
				x1 = v.first;
				y1 = v.second;
			} else {
				--it;
				return (x - x1) * (v.second - y1) / (v.first - x1) + y1;
			}
		}
	}
};

class GchartProvider {
private:
	typedef GchartChart* iterator;
	typedef const GchartChart* const_iterator;

	float y_min, y_max, y_scale;
	std::shared_ptr<GchartLabel> _label;
	std::forward_list<GchartChart> _charts;

	bool addChart (const GchartChart::Type &t, const int &identifier, const GchartColor &color, const GchartMap chart, GchartGetValue get_value) {
		//this->_charts.push_front (new GchartChart (t, identifier, chart, get_value));
		this->_charts.emplace_front (t, identifier, color, chart, get_value);
		if (this->_charts.front ().getIdentifier () == identifier)
			return true;
		return false;
	}

public:
	GchartProvider (const std::string &label, const std::string &unit, GchartValuePrint print) {
		this->_label = std::make_shared<GchartLabel> (label, unit, print);
		this->y_min = NAN;
		this->y_max = NAN;
		this->y_scale = 1.0;
	}

	~GchartProvider (void) {}

	/* If charts is changed, call this->drawing->reload(); */
	const float getYMax () const {
		float y_max = NAN;
		for (const auto &chart : this->_charts) {
			for (const auto &v : chart) {
				if (!isfinite (y_max))
					y_max = v.second;
				else
					y_max = MAX (v.second, y_max);
			}
		}
		return y_max;
	}

	const float getYMax (const float &x_min, const float &x_max) const {
		float y_max = NAN;
		for (const auto &chart : this->_charts) {
			for (const auto &v : chart) {
				if (v.first >= x_min && v.first <= x_max) {
					if (!isfinite (y_max))
						y_max = v.second;
					else
						y_max = MAX (v.second, y_max);
				}
			}
		}
		return y_max;
	}

	const float getYMin () const {
		float y_min = NAN;
		for (const auto &chart : this->_charts) {
			for (const auto &v : chart) {
				if (!isfinite (y_min))
					y_min = v.second;
				else
					y_min = MIN (v.second, y_min);
			}
		}
		return y_min;
	}

	const float getYMin (const float &x_min, const float &x_max) const {
		float y_min = NAN;
		for (const auto &chart : this->_charts) {
			for (const auto &v : chart) {
				if (v.first >= x_min && v.first <= x_max) {
					if (!isfinite (y_min))
						y_min = v.second;
					else
						y_min = MIN (v.second, y_min);
				}
			}
		}
		return y_min;
	}

	const float getXMax () const {
		float x_max = NAN;
		for (const auto &chart : this->_charts) {
			if (!isfinite (x_max))
				x_max = chart.last ()->first;
			else
				x_max = MAX (chart.last ()->first, x_max);
		}
		return x_max;
	}

	const float getXMin () const {
		float x_min = NAN;
		for (const auto &c : this->_charts) {
			if (!isfinite (x_min))
				x_min = c.begin ()->first;
			else
				x_min = MIN (c.begin ()->first, x_min);
		}
		return x_min;
	}

	const std::shared_ptr<GchartLabel>& getLabel (void) const {
		return this->_label;
	}
/*
	const GchartChart& operator[] (const int &identifier) const {
		//return this->_charts[idx];
		for (const GchartChart &c : this->_charts) {
			if (identifier == c.getIdentifier ())
				return c;
		}
		return nullptr;
	}

	std::size_t count (void) const {
		this->_charts.size ();
	}
*/

	std::forward_list<GchartChart>::const_iterator end (void) const noexcept {
		return this->_charts.end ();
	}

	std::forward_list<GchartChart>::const_iterator begin (void) const noexcept {
		return this->_charts.begin ();
	}

	void reset (bool confirm = false) {
		if (confirm) {
			this->y_min = NAN;
			this->y_max = NAN;
			this->y_scale = 1.0;
			this->_charts.clear ();
		}
	}

	friend class Gchart;
};

#endif /* __G_CHART_PROVIDER_H__ */
