/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
 * Copyright (C) 2004 Chris Schoeneman
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

#include "synergy/IKeyState.h"
#include "base/EventQueue.h"

#include <cstring>
#include <cstdlib>

//
// IKeyState
//

IKeyState::IKeyState(IEventQueue* events)
{
}

//
// IKeyState::KeyInfo
//

IKeyState::KeyInfo*
IKeyState::KeyInfo::alloc(KeyID id,
				KeyModifierMask mask, KeyButton button, SInt32 count)
{
	KeyInfo* info           = (KeyInfo*)malloc(sizeof(KeyInfo));
	info->m_key              = id;
	info->m_mask             = mask;
	info->m_button           = button;
	info->m_count            = count;
	info->m_screens          = NULL;
	info->m_screensBuffer[0] = 0;
	return info;
}

IKeyState::KeyInfo*
IKeyState::KeyInfo::alloc(KeyID id,
				KeyModifierMask mask, KeyButton button, SInt32 count,
				const std::set<nstring>& destinations)
{
	nstring screens = join(destinations);

	// build structure
	KeyInfo* info  = (KeyInfo*)malloc(sizeof(KeyInfo) + screens.size() * sizeof(nchar));
	info->m_key     = id;
	info->m_mask    = mask;
	info->m_button  = button;
	info->m_count   = count;
	info->m_screens = info->m_screensBuffer;
	nstrcpy(info->m_screensBuffer, screens.c_str());
	return info;
}

IKeyState::KeyInfo*
IKeyState::KeyInfo::alloc(const KeyInfo& x)
{
	KeyInfo* info  = (KeyInfo*)malloc(sizeof(KeyInfo) +
										nstrlen(x.m_screensBuffer));
	info->m_key     = x.m_key;
	info->m_mask    = x.m_mask;
	info->m_button  = x.m_button;
	info->m_count   = x.m_count;
	info->m_screens = x.m_screens ? info->m_screensBuffer : NULL;
	nstrcpy(info->m_screensBuffer, x.m_screensBuffer);
	return info;
}

bool
IKeyState::KeyInfo::isDefault(const nchar* screens)
{
	return (screens == NULL || screens[0] == 0);
}

bool
IKeyState::KeyInfo::contains(const nchar* screens, const nstring& name)
{
	// special cases
	if (isDefault(screens)) {
		return false;
	}
	if (screens[0] == _N('*')) {
		return true;
	}

	const nchar* match = nstrstr(screens, name.c_str());
	if (match && match > screens) {
		return match[name.length()] == _N(':');
	}

	return false;
}

bool
IKeyState::KeyInfo::equal(const KeyInfo* a, const KeyInfo* b)
{
	return (a->m_key    == b->m_key &&
			a->m_mask   == b->m_mask &&
			a->m_button == b->m_button &&
			a->m_count  == b->m_count &&
			strcmp(a->m_screensBuffer, b->m_screensBuffer) == 0);
}

nstring
IKeyState::KeyInfo::join(const std::set<nstring>& destinations)
{
	// collect destinations into a string.  names are surrounded by ':'
	// which makes searching easy.  the string is empty if there are no
	// destinations and "*" means all destinations.
	nstring screens;
	for (std::set<nstring>::const_iterator i = destinations.begin();
								i != destinations.end(); ++i) {
		if (*i == _N("*")) {
			screens = _N("*");
			break;
		}
		else {
			if (screens.empty()) {
				screens = _N(":");
			}
			screens += *i;
			screens += _N(":");
		}
	}
	return screens;
}

void
IKeyState::KeyInfo::split(const nchar* screens, std::set<nstring>& dst)
{
	dst.clear();
	if (isDefault(screens)) {
		return;
	}
	if (screens[0] == _N('*')) {
		dst.insert(_N("*"));
		return;
	}

	const char* i = screens + 1;
	while (*i) {
		const nchar* j = strchr(i, _N(':'));
		dst.insert(nstring(i, j - i));
		i = j + 1;
	}
}
