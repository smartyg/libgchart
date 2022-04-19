/* kate: indent-mode cstyle; tab-width 4; indent-width 4; */
/*
 * helper.hpp
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

#ifndef __G_CHARTHELPER_HPP__
#define __G_CHARTHELPER_HPP__

#include "config.h"

#ifdef HAVE_CXX20
	#include <format>
	#define string_format std::format
#else
	#include <memory>
	#include <string>
	#include <stdexcept>

template<typename ... Args>
std::string string_format (const std::string& format, Args ... args)
{
	int size_s = std::snprintf (nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	if (size_s <= 0) {
		throw std::runtime_error( "Error during formatting.");
	}
	auto size = static_cast<size_t>(size_s);
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf (buf.get(), size, format.c_str(), args ...);
	return std::string (buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
#endif

#endif /* __G_CHARTHELPER_HPP__ */
