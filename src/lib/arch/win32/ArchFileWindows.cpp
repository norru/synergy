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

#include "arch/win32/ArchFileWindows.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shlobj.h>
#include <tchar.h>
#include <string.h>

//
// ArchFileWindows
//

ArchFileWindows::ArchFileWindows()
{
	// do nothing
}

ArchFileWindows::~ArchFileWindows()
{
	// do nothing
}

const wchar_t*
ArchFileWindows::getBasename(const wchar_t* pathname)
{
	if (pathname == NULL) {
		return NULL;
	}

	// check for last slash
	const wchar_t* basename = strrchr(pathname, L'/');
	if (basename != NULL) {
		++basename;
	}
	else {
		basename = pathname;
	}

	// check for last backslash
	const wchar_t* basename2 = strrchr(pathname, L'\\');
	if (basename2 != NULL && basename2 > basename) {
		basename = basename2 + 1;
	}

	return basename;
}

std::wstring
ArchFileWindows::getUserDirectory()
{
	// try %HOMEPATH%
	wchar_t dir[MAX_PATH];
	DWORD result = GetEnvironmentVariableW(L"HOMEPATH", dir, MAX_PATH);
	if (result != 0 && result < MAX_PATH) {
		// sanity check -- if dir doesn't appear to start with a
		// drive letter and isn't a UNC name then don't use it
		// FIXME -- allow UNC names
		if (dir[0] && (dir[1] == L':' ||
			((dir[0] == L'\\' || dir[0] == L'/') &&
			(dir[1] == L'\\' || dir[1] == L'/')))) {
			return dir;
		}
	}

	// get the location of the personal files.  that's as close to
	// a home directory as we're likely to find.
	ITEMIDLIST* idl;
	if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &idl))) {
		wchar_t* path = NULL;
		if (SHGetPathFromIDListW(idl, dir)) {
			DWORD attr = GetFileAttributes(dir);
			if (attr != 0xffffffff && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0)
				path = dir;
		}

		IMalloc* shalloc;
		if (SUCCEEDED(SHGetMalloc(&shalloc))) {
			shalloc->Free(idl);
			shalloc->Release();
		}

		if (path != NULL) {
			return path;
		}
	}

	// use root of C drive as a default
	return L"C:";
}

std::wstring
ArchFileWindows::getSystemDirectory()
{
	// get windows directory
	wchar_t dir[MAX_PATH];
	if (GetWindowsDirectoryW(dir, MAX_PATH) != 0) {
		return dir;
	}
	else {
		// can't get it.  use C:\ as a default.
		return L"C:";
	}
}

std::wstring
ArchFileWindows::getInstalledDirectory()
{
	wchar_t fileNameBuffer[MAX_PATH];
	GetModuleFileNameW(NULL, fileNameBuffer, MAX_PATH);
	std::wstring fileName(fileNameBuffer);
	size_t lastSlash = fileName.find_last_of(L"\\");
	fileName = fileName.substr(0, lastSlash);

	return fileName;
}

std::wstring
ArchFileWindows::getLogDirectory()
{
	return getInstalledDirectory();
}

std::wstring
ArchFileWindows::getPluginDirectory()
{
	if (!m_pluginDirectory.empty()) {
		return m_pluginDirectory;
	}

	std::wstring dir = getProfileDirectory();
	dir.append(L"\\Plugins");
	return dir;
}

std::wstring
ArchFileWindows::getProfileDirectory()
{
	std::wstring dir;
	if (!m_profileDirectory.empty()) {
		dir = m_profileDirectory;
	}
	else {
		wchar_t result[MAX_PATH];
		if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, result))) {
			dir = result;
		}
		else {
			dir = getUserDirectory();
		}
	}

	// HACK: append program name, this seems wrong.
	dir.append(L"\\Synergy");

	return dir;
}

std::wstring
ArchFileWindows::concatPath(const std::wstring& prefix,
				const std::wstring& suffix)
{
	std::wstring path;
	path.reserve(prefix.size() + 1 + suffix.size());
	path += prefix;
	if (path.size() == 0 ||
		(path[path.size() - 1] != L'\\' &&
		path[path.size() - 1] != L'/')) {
		path += L'\\';
	}
	path += suffix;
	return path;
}

void
ArchFileWindows::setProfileDirectory(const std::wstring& s)
{
	m_profileDirectory = s;
}

void
ArchFileWindows::setPluginDirectory(const std::wstring& s)
{
	m_pluginDirectory = s;
}
