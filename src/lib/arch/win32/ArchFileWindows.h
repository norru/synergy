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

#pragma once

#include "arch/IArchFile.h"

#define ARCH_FILE ArchFileWindows

//! Win32 implementation of IArchFile
class ArchFileWindows : public IArchFile {
public:
	ArchFileWindows();
	virtual ~ArchFileWindows();

	// IArchFile overrides
	virtual const wchar_t*	getBasename(const wchar_t* pathname);
	virtual std::wstring getUserDirectory();
	virtual std::wstring getSystemDirectory();
	virtual std::wstring getInstalledDirectory();
	virtual std::wstring getLogDirectory();
	virtual std::wstring getPluginDirectory();
	virtual std::wstring getProfileDirectory();
	virtual std::wstring concatPath(const std::wstring& prefix,
							const std::wstring& suffix);
	virtual void		setProfileDirectory(const std::wstring& s);
	virtual void		setPluginDirectory(const std::wstring& s);

private:
	std::wstring		m_profileDirectory;
	std::wstring		m_pluginDirectory;
};
