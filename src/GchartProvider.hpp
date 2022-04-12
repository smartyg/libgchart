/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartProvider.hpp
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

#ifndef __GCHART_PROVIDER_HPP__
#define __GCHART_PROVIDER_HPP__

#include <features.h>

#include <memory>
#include <string>
#include <iterator>
#include <forward_list>

#include "GchartColor.hpp"
#include "GchartPoint.hpp"
#include "GchartLabel.hpp"
#include "GchartChart.hpp"

class GchartProvider {
private:
	typedef GchartChart* iterator;
	typedef const GchartChart* const_iterator;

	float y_min, y_max, y_scale;
	std::shared_ptr<GchartLabel> _label;
	std::forward_list<GchartChart> _charts;

	bool addChart (const GchartChart::Type &t, const int &identifier, const GchartColor &color, const GchartMap chart, GchartGetValue get_value);

public:
	GchartProvider (const std::string &label, const std::string &unit, GchartValuePrint print);
	~GchartProvider (void);

	/* If charts is changed, call this->drawing->reload(); */
	const float getYMax () const;
	const float getYMax (const float &x_min, const float &x_max) const;
	const float getYMin () const;
	const float getYMin (const float &x_min, const float &x_max) const;
	const float getXMax () const;
	const float getXMin () const;
	const std::shared_ptr<GchartLabel>& getLabel (void) const;
	const GchartChart& operator[] (const int &identifier) const;
	std::forward_list<GchartChart>::const_iterator end (void) const noexcept;
	std::forward_list<GchartChart>::const_iterator begin (void) const noexcept;
	void reset (bool confirm = false);

	friend class Gchart;
};

#endif /* __GCHART_PROVIDER_HPP__ */
