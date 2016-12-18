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

#include "net/XSocket.h"
#include "base/String.h"

//
// XSocketAddress
//

XSocketAddress::XSocketAddress(EError error,
				const std::string& hostname, int port) _NOEXCEPT :
	m_error(error),
	m_hostname(hostname),
	m_port(port)
{
	// do nothing
}

XSocketAddress::EError
XSocketAddress::getError() const throw()
{
	return m_error;
}

std::string
XSocketAddress::getHostname() const throw()
{
	return m_hostname;
}

int
XSocketAddress::getPort() const throw()
{
	return m_port;
}

nstring
XSocketAddress::getWhat() const throw()
{
	static const nchar* s_errorID[] = {
		_N("XSocketAddressUnknown"),
		_N("XSocketAddressNotFound"),
		_N("XSocketAddressNoAddress"),
		_N("XSocketAddressUnsupported"),
		_N("XSocketAddressBadPort")
	};
	static const nchar* s_errorMsg[] = {
		_N("unknown error for: %{1}:%{2}"),
		_N("address not found for: %{1}"),
		_N("no address for: %{1}"),
		_N("unsupported address for: %{1}"),
		_N("invalid port")				// m_port may not be set to the bad port
	};

	const nchar* str;
#ifdef Q_OS_WIN
	std::wstring x = synergy::string::toNative(m_hostname);
	str = x.c_str();
#else
	str = m_hostname.c_str();
#endif
	return format(s_errorID[m_error], s_errorMsg[m_error], str,
		synergy::string::sprintf(_N("%d"), m_port).c_str());
}


//
// XSocketIOClose
//

nstring
XSocketIOClose::getWhat() const throw()
{
	return format(_N("XSocketIOClose"), _N("close: %{1}"), what());
}


//
// XSocketBind
//

nstring
XSocketBind::getWhat() const throw()
{
	return format(_N("XSocketBind"), _N("cannot bind address: %{1}"), what());
}


//
// XSocketConnect
//

nstring
XSocketConnect::getWhat() const throw()
{
	return format(_N("XSocketConnect"), _N("cannot connect socket: %{1}"), what());
}


//
// XSocketCreate
//

nstring
XSocketCreate::getWhat() const throw()
{
	return format(_N("XSocketCreate"), _N("cannot create socket: %{1}"), what());
}
