/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartColor.hpp
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

#ifndef __GCHART_COLOR_HPP__
#define __GCHART_COLOR_HPP__

struct GchartColor {
	const double _red;
	const double _green;
	const double _blue;
	const double _alpha;

	GchartColor (const double red, const double blue, const double green) : _red(red), _green(green), _blue(blue), _alpha(1.0) {};
	GchartColor (const double red, const double blue, const double green, const double alpha) : _red(red), _green(green), _blue(blue), _alpha(alpha) {};
	~GchartColor (void) {};
};

#endif /* __GCHART_COLOR_HPP__ */
