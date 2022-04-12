/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * GchartLabel.hpp
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

#ifndef __GCHART_LABEL_HPP__
#define __GCHART_LABEL_HPP__

#include <string>

#include "helper.hpp"

class GchartLabel;

typedef const std::string (*GchartValuePrint) (const GchartLabel *self, const float &value);

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
		return string_format ("%0.2f %s", value, self->getUnit ().c_str());
	}
};

#endif /* __GCHART_LABEL_HPP__ */
