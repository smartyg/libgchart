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

#ifndef __GCHART_CHART_HPP__
#define __GCHART_CHART_HPP__

#include <memory>
#include <iterator>
#include <map>

#include "GchartColor.hpp"
#include "GchartPoint.hpp"
#include "helper.hpp"

typedef float (*GchartGetValue) (const GchartMap &chart, float &x, GchartMap::const_iterator &it);

class GchartChart {
private:
	const int _identifier;
	const GchartColor _color;
	const GchartMap _map;
	GchartGetValue _get_value;
	void *_user_data;


public:
	enum Type {
		LINEAR = 1,
		CURVE_2,
		CURVE_3,
		CURVE_4,
		CURVE_5,
		CUSTOM
	};

	// For linear inerpolation
	GchartChart (const int identifier, const GchartColor &color, const GchartMap map);
	// for curved chart
	GchartChart (const GchartChart::Type &t, const int identifier, const GchartColor &color, const GchartMap map, GchartGetValue cb = nullptr, void *user_data = nullptr);
	~GchartChart (void);

	const float& operator[] (std::size_t idx) const;
	float getValue (const float &x) const;

private:
	float getValue (float &x, GchartMap::const_iterator &it) const;

public:
	const std::shared_ptr<GchartPoint> getPoint (const float &x) const;
	const int& getIdentifier (void) const;
	const std::shared_ptr<GchartPoint> getNextPoint (const std::shared_ptr<GchartPoint> &prev, const float &x_hint) const;
	size_t size (void) const noexcept;
	const GchartMap::const_iterator end (void) const noexcept;
	const GchartMap::const_iterator begin (void) const noexcept;
	const GchartMap::const_iterator last (void) const;
	const GchartColor& getColor (void) const;

	static float linear (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved2 (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved3 (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved4 (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved5 (const GchartMap &map, float &x, GchartMap::const_iterator &it);
};

#endif /* __GCHART_CHART_HPP__ */
