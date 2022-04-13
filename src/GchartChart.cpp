/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartChart.hpp
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

#include "GchartChart.hpp"

#include <cmath>
#include <memory>
#include <iterator>
#include <map>
#include <string>

#include "GchartPoint.hpp"

// For linear inerpolation
GchartChart::GchartChart (const int identifier, const GchartColor &color, const GchartMap map) : _identifier(identifier), _color(color), _map(map), _get_value(&GchartChart::linear), _user_data(nullptr) {
	return;
}

// for curved chart
GchartChart::GchartChart (const GchartChart::Type &t, const int identifier, const GchartColor &color, const GchartMap map, GchartGetValue cb, void *user_data) : _identifier(identifier), _color(color), _map(map) {
	switch (t) {
		case Type::LINEAR:
			this->_get_value = &GchartChart::linear;
			this->_user_data = nullptr;
			break;
		case Type::CURVE_2:
			this->_get_value = &GchartChart::curved2;
			this->_user_data = nullptr;
			break;
		case Type::CURVE_3:
			this->_get_value = &GchartChart::curved3;
			this->_user_data = nullptr;
			break;
		case Type::CURVE_4:
			this->_get_value = &GchartChart::curved4;
			this->_user_data = nullptr;
			break;
		case Type::CURVE_5:
			this->_get_value = &GchartChart::curved5;
			this->_user_data = nullptr;
			break;
		case Type::CUSTOM:
		default:
			this->_get_value = cb;
			this->_user_data = user_data;
	}
}

GchartChart::~GchartChart (void) {
	return;
}

const float& GchartChart::operator[] (std::size_t idx) const {
	const auto it = std::next (this->_map.begin (), idx);
	return (*it).second;
}

float GchartChart::getValue (const float &x) const {
	auto it = this->_map.end ();
	float x_hint = x;
	float y = this->_get_value (this->_map, x_hint, it);
	if (x_hint == x) return y;
	return NAN;
}

float GchartChart::getValue (float &x, GchartMap::const_iterator &it) const {
	return this->_get_value (this->_map, x, it);
}

const std::shared_ptr<GchartPoint> GchartChart::getPoint (const float &x) const {
	const float y = this->getValue (x);
	return std::make_shared<GchartPoint>(x, y, this->_map.end ());
}

const int& GchartChart::getIdentifier (void) const {
	return this->_identifier;
}

const std::shared_ptr<GchartPoint> GchartChart::getNextPoint (const std::shared_ptr<GchartPoint> &prev, const float &x_hint) const {
	GchartMap::const_iterator it = prev->getIterator ();
	float x = x_hint;
	const float y = this->getValue (x, it);
	return std::make_shared<GchartPoint>(x, y, 0, it);
}

size_t GchartChart::size (void) const noexcept {
	return this->_map.size ();
}

const GchartMap::const_iterator GchartChart::end (void) const noexcept {
	return this->_map.end ();
}

const GchartMap::const_iterator GchartChart::begin (void) const noexcept {
	return this->_map.begin ();
}

const GchartMap::const_iterator GchartChart::last (void) const {
	GchartMap::const_iterator it = this->_map.end ();
	return --it;
}

const GchartColor& GchartChart::getColor (void) const {
	return this->_color;
}

float GchartChart::linear (const GchartMap &map, float &x, GchartMap::const_iterator &it) {
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
	return NAN;
}

float GchartChart::curved2 (const GchartMap &map, float &x, GchartMap::const_iterator &it) {
	return 0.0f;
}

float GchartChart::curved3 (const GchartMap &map, float &x, GchartMap::const_iterator &it) {
	return 0.0f;
}

float GchartChart::curved4 (const GchartMap &map, float &x, GchartMap::const_iterator &it) {
	return 0.0f;
}

float GchartChart::curved5 (const GchartMap &map, float &x, GchartMap::const_iterator &it) {
	return 0.0f;
}
