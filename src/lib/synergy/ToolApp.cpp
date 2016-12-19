/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2014-2016 Symless Ltd.
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

#include "synergy/ToolApp.h"

#include "synergy/ArgParser.h"
#include "arch/Arch.h"
#include "base/Log.h"
#include "base/String.h"

#include <iostream>
#include <sstream>

#if SYSAPI_WIN32
#include "platform/MSWindowsSession.h"
#endif

enum {
	kErrorOk,
	kErrorArgs,
	kErrorException,
	kErrorUnknown
};

UInt32
ToolApp::run(int argc, char** argv)
{
	if (argc <= 1) {
		std::ncerr << N"no args" << std::endl;
		return kErrorArgs;
	}

	try {
		ArgParser argParser(this);
		bool result = argParser.parseToolArgs(m_args, argc, argv);

		if (!result) {
			m_bye(kExitArgs);
		}

		if (m_args.m_printActiveDesktopName) {
#if SYSAPI_WIN32
			MSWindowsSession session;
			nstring name = session.getActiveDesktopName();
			if (name.empty()) {
				LOG((CLOG_CRIT L"failed to get active desktop name"));
				return kExitFailed;
			}
			else {
				LOG((CLOG_INFO L"activeDesktop: %ls", name.c_str()));
			}
#endif
		}
		else if (m_args.m_getInstalledDir) {
			std::ncout << ARCH->getInstalledDirectory() << std::endl;
		}
		else if (m_args.m_getProfileDir) {
			std::ncout << ARCH->getProfileDirectory() << std::endl;
		}
		else if (m_args.m_getArch) {
			std::ncout << ARCH->getPlatformName() << std::endl;
		}
		else {
			throw XSynergy(N"Nothing to do");
		}
	}
	catch (std::exception& e) {
		LOG((CLOG_CRIT "An error occurred: %s\n", e.what()));
		return kExitFailed;
	}
	catch (...) {
		LOG((CLOG_CRIT "An unknown error occurred.\n"));
		return kExitFailed;
	}

#if WINAPI_XWINDOWS
	// HACK: avoid sigsegv on linux
	m_bye(kErrorOk);
#endif

	return kErrorOk;
}

void
ToolApp::help()
{
}
