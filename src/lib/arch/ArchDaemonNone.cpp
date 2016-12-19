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

#include "arch/ArchDaemonNone.h"

//
// ArchDaemonNone
//

ArchDaemonNone::ArchDaemonNone()
{
	// do nothing
}

ArchDaemonNone::~ArchDaemonNone()
{
	// do nothing
}

void
ArchDaemonNone::installDaemon(const nchar*,
				const nchar*,
				const nchar*,
				const nchar*,
				const nchar*)
{
	// do nothing
}

void
ArchDaemonNone::uninstallDaemon(const nchar*)
{
	// do nothing
}

int
ArchDaemonNone::daemonize(const nchar* name, DaemonFunc func)
{
	// simply forward the call to func.  obviously, this doesn't
	// do any daemonizing.
	return func(1, &name);
}

bool
ArchDaemonNone::canInstallDaemon(const nchar*)
{
	return false;
}

bool
ArchDaemonNone::isDaemonInstalled(const nchar*)
{
	return false;
}

void
ArchDaemonNone::installDaemon()
{
}

void
ArchDaemonNone::uninstallDaemon()
{
}

nstring
ArchDaemonNone::commandLine() const
{
	return N"";
}
