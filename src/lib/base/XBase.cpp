/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
 * Copyright (C) 2002 Chris Schoeneman
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "base/XBase.h"
#include "base/String.h"

#include <cerrno>
#include <cstdarg>

//
// XBase
//

XBase::XBase() :
	std::runtime_error(N"")
{
	// do nothing
}

XBase::XBase(const nstring& msg) :
	std::runtime_error(msg)
{
	// do nothing
}

XBase::~XBase() _NOEXCEPT
{
	// do nothing
}

#if SYSAPI_WIN32
const std::string
XBase::what() const _NOEXCEPT
{
	const char* what = std::runtime_error::what();
	if (!what || !strlen(what)) {
		m_what = getWhat();
		return synergy::string::nativeToUtf8(m_what);
	}
	return what;
}
#endif

const nstring
XBase::what() const _NOEXCEPT
{
	nstring what = synergy::string::toNative(std::runtime_error::what());
	if (what.empty()) {
		m_what = getWhat();
		return m_what;
	}
	return what;
}

nstring
XBase::format(const nchar* /*id*/, const nchar* fmt, ...) const throw()
{
	// FIXME -- lookup message string using id as an index.  set
	// fmt to that string if it exists.

	// format
	nstring result;
	va_list args;
	va_start(args, fmt);
	try {
		result = synergy::string::vformat(fmt, args);
	}
	catch (...) {
		// ignore
	}
	va_end(args);

	return result;
}
