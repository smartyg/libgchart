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

typedef const std::shared_ptr<GchartPoint<T::const_iterator>> (*GchartFilter) (const std::shared_ptr<T> &chart, float &x, GchartMap::const_iterator &it);
Typedef float (*GchartInterpolator) (const std::shared_ptr<GchartPoint>&, const std::shared_ptr<GchartPoint>&, const float&);

Class GchartChartBase {
Private:
Const std::shared_ptr<void> _ptr;
const int _identifier;
	const GColor _color;
        const GchartInterpolator _interpolate;
Protected:
GchartChartBase (const std::shared_ptr<void> ptr, std::type_index type) : _ptr(ptr), _type(type) { }
virtual ~GchartChartBase (void) = 0;
const std::shared_ptr<void> getPtr (void) const {
Return this->_ptr;
}
Public:
enum Type {
                NONE = 0,
		LINEAR = 1,
		CURVE_2,
		CURVE_3,
		CURVE_4,
		CURVE_5,
		CUSTOM
	};
int getIdentifier (void) const;
const GchartColor getColor (void) const;
size_t size (void) const noexcept;

static float linear (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved2 (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved3 (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved4 (const GchartMap &map, float &x, GchartMap::const_iterator &it);
	static float curved5 (const GchartMap &map, float &x, GchartMap::const_iterator &it);


virtual const std::shared_ptr<GchartPoint> getPoint (const float&) const = 0;
vitrual const std::shared_ptr<GchartPoint> getNextPoint (const std::shared_ptr<GchartPoint>&, const float&) const = 0;
}

Template<Iteratable T>
class GchartChart : public GchartChartBase {
private:
	const GchartFilter _filter;

public:
	

        // disable copy and move constructor and assignment
        GchartChart (void) = delete;

	// For none inerpolation
        GchartChart (const int identifier, const GchartColor &color, const std::shared_ptr<T> map, std::function<const std::shared_ptr<GchartPoint>(const std::shared_ptr<T>&, const T::const_iterator&)> filter) : GChartChart(NONE, identifier, color, map) { }
	// for curved chart
	GchartChart (const GchartChart::Type &t, const int identifier, const GchartColor &color, const std::shared_ptr<T> data, const GchartFilter filter) : _identifier(identifier), _color(color) {
GchartMap<T> map = new GchartMap<T>(data, use_filter);
This->_map = static_cast<GchartMapBase>(map);
// assign interpolator
}
	~GchartChart (void);

	//const float& operator[] (std::size_t idx) const;
	//float getValue (const float &x) const;

//private:
	//float getValue (float &x, GchartMap::const_iterator &it) const;

public:
        const std::shared_ptr<T> getData (void) {
Const std::shared_ptr<T> map = static_pointer_cast<T>(this->getPtr ());
Return map;
}
        const std::shared_ptr<GchartPointBase> getPoint (const float &x) const {
const std::shared_ptr<T> map = static_pointer_cast<T>(this->getPtr ());
For (T::const_iterator it = map->begin (), it != map->end (), ++it) {
Std::shared_ptr<GchartPoint<T::const_iterator>> prev, next;
Std::pair<GchartMatchType, std::shared_ptr<GchartPoint<T>>> res;
If (this->_filter != nullptr) {
res = this->_filter (map, x, it);
} else {
res = GchartChart<T>::createPoint (it, x);
}
switch (std::get<0>(res)) {
  Case MATCH_EXACT:
Return static_pointer_cast<GchartPointBase>(std::get<1>(res));
case MATCH_SMALLER:
prev = std::get<1>(res);
Break;
case MATCH_BIGGER:
next = std::get<1>(res);
Break;
case MATCH_NONE:
default:
Break;
}
}
If (!next) return prev;
If _interpolate == nullptr {
Const float y = GchartChartBase::linear(...);
Return static_pointer_cast<GchartPointBase>(GchartPoint<T::const_iterator>::create (x, y, it_prev));
} else {
const float y = this->_interpolate (...)
Return static_pointer_cast<GchartPointBase>(GchartPoint<T::const_iterator>::create (x, y, it_prev));
}
}
	const std::shared_ptr<GchartPoint> getNextPoint (const std::shared_ptr<GchartPoint> &prev, const float &x_hint) const;
	//const GchartMap::const_iterator end (void) const noexcept;
	//const GchartMap::const_iterator begin (void) const noexcept;
	//const GchartMap::const_iterator last (void) const;
};

#endif /* __GCHART_CHART_HPP__ */
