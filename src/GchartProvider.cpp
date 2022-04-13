/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartProvider.cpp
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

#include "GchartProvider.hpp"

#include <cmath>
#include <memory>
#include <string>
#include <forward_list>
#include <stdexcept>

#include "GchartColor.hpp"
#include "GchartPoint.hpp"
#include "GchartLabel.hpp"
#include "GchartChart.hpp"

GchartProvider::GchartProvider (const std::string &label, const std::string &unit, GchartValuePrint print) {
	this->_label = std::make_shared<GchartLabel> (label, unit, print);
	this->y_min = NAN;
	this->y_max = NAN;
	this->y_scale = 1.0;
}

GchartProvider::~GchartProvider (void) {
	return;
}

bool GchartProvider::addChart (const GchartChart::Type &t, const int &identifier, const GchartColor &color, const GchartMap chart, GchartGetValue get_value) {
	this->_charts.emplace_front (t, identifier, color, chart, get_value);
	if (this->_charts.front ().getIdentifier () == identifier)
		return true;
	return false;
}

float GchartProvider::getYMax (void) const {
	float y_max = NAN;
	for (const auto &chart : this->_charts) {
		for (const auto &v : chart) {
			if (!std::isfinite (y_max))
				y_max = v.second;
			else
				y_max = std::max (v.second, y_max);
		}
	}
	return y_max;
}

float GchartProvider::getYMax (const float &x_min, const float &x_max) const {
	float y_max = NAN;
	for (const auto &chart : this->_charts) {
		for (const auto &v : chart) {
			if (v.first >= x_min && v.first <= x_max) {
				if (!std::isfinite (y_max))
					y_max = v.second;
				else
					y_max = std::max (v.second, y_max);
			}
		}
	}
	return y_max;
}

float GchartProvider::getYMin (void) const {
	float y_min = NAN;
	for (const auto &chart : this->_charts) {
		for (const auto &v : chart) {
			if (!std::isfinite (y_min))
				y_min = v.second;
			else
				y_min = std::min (v.second, y_min);
		}
	}
	return y_min;
}

float GchartProvider::getYMin (const float &x_min, const float &x_max) const {
	float y_min = NAN;
	for (const auto &chart : this->_charts) {
		for (const auto &v : chart) {
			if (v.first >= x_min && v.first <= x_max) {
				if (!std::isfinite (y_min))
					y_min = v.second;
				else
					y_min = std::min (v.second, y_min);
			}
		}
	}
	return y_min;
}

float GchartProvider::getXMax (void) const {
	float x_max = NAN;
	for (const auto &chart : this->_charts) {
		if (!std::isfinite (x_max))
			x_max = chart.last ()->first;
		else
			x_max = std::max (chart.last ()->first, x_max);
	}
	return x_max;
}

float GchartProvider::getXMin (void) const {
	float x_min = NAN;
	for (const auto &c : this->_charts) {
		if (!std::isfinite (x_min))
			x_min = c.begin ()->first;
		else
			x_min = std::min (c.begin ()->first, x_min);
	}
	return x_min;
}

const std::shared_ptr<GchartLabel>& GchartProvider::getLabel (void) const {
	return this->_label;
}

const GchartChart& GchartProvider::operator[] (const int &identifier) const {
	for (const GchartChart &c : this->_charts) {
		if (identifier == c.getIdentifier ())
			return c;
	}
	throw std::range_error ("Value is not present in list.");
}

std::forward_list<GchartChart>::const_iterator GchartProvider::end (void) const noexcept {
	return this->_charts.end ();
}

std::forward_list<GchartChart>::const_iterator GchartProvider::begin (void) const noexcept {
	return this->_charts.begin ();
}

void GchartProvider::reset (bool confirm) {
	if (confirm) {
		this->y_min = NAN;
		this->y_max = NAN;
		this->y_scale = 1.0;
		this->_charts.clear ();
	}
}
