/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartPoint.hpp
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

#ifndef __GCHART_POINT_HPP__
#define __GCHART_POINT_HPP__

#include <iterator>
#include <map>

#define FLAG_FREE (1 << 0)
#define FLAG_INTERPOLATE (1 << 1)
#define FLAG_POINT (1 << 2)

#define IS_FLAG_SET(flags, f) (((flags) & (f)) == (f))

//typedef std::map<const float, const float> GchartMap;

class GchartPointBase {
private:
	const float _x;
	const float _y;
	//const int _flags;
	//GchartMap::const_iterator _it;

public:
	GchartPoint (const float x, const float y) : _x(x), _y(y {};
	~GchartPoint (void) {}
/*
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
*/
};

Template<Iterator I>
class GchartPoint : public GchartPointBase {
I _it;

};

#endif /* __GCHART_POINT_HPP__ */
