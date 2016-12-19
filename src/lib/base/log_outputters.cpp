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

#include "base/log_outputters.h"
#include "base/TMethodJob.h"
#include "arch/Arch.h"

#include <fstream>

enum EFileLogOutputter {
	kFileSizeLimit = 1024 // kb
};

//
// StopLogOutputter
//

StopLogOutputter::StopLogOutputter()
{
	// do nothing
}

StopLogOutputter::~StopLogOutputter()
{
	// do nothing
}

void
StopLogOutputter::open(const nchar*)
{
	// do nothing
}

void
StopLogOutputter::close()
{
	// do nothing
}

void
StopLogOutputter::show(bool)
{
	// do nothing
}

bool
StopLogOutputter::write(ELevel, const nchar*)
{
	return false;
}


//
// ConsoleLogOutputter
//

ConsoleLogOutputter::ConsoleLogOutputter()
{
}

ConsoleLogOutputter::~ConsoleLogOutputter()
{
}

void
ConsoleLogOutputter::open(const nchar* title)
{
	ARCH->openConsole(title);
}

void
ConsoleLogOutputter::close()
{
	ARCH->closeConsole();
}

void
ConsoleLogOutputter::show(bool showIfEmpty)
{
	ARCH->showConsole(showIfEmpty);
}

bool
ConsoleLogOutputter::write(ELevel level, const nchar* msg)
{
	ARCH->writeConsole(level, msg);
	return true;
}

void
ConsoleLogOutputter::flush()
{

}


//
// SystemLogOutputter
//

SystemLogOutputter::SystemLogOutputter()
{
	// do nothing
}

SystemLogOutputter::~SystemLogOutputter()
{
	// do nothing
}

void
SystemLogOutputter::open(const nchar* title)
{
	ARCH->openLog(title);
}

void
SystemLogOutputter::close()
{
	ARCH->closeLog();
}

void
SystemLogOutputter::show(bool showIfEmpty)
{
	ARCH->showLog(showIfEmpty);
}

bool
SystemLogOutputter::write(ELevel level, const nchar* msg)
{
	ARCH->writeLog(level, msg);
	return true;
}

//
// SystemLogger
//

SystemLogger::SystemLogger(const nchar* title, bool blockConsole) :
	m_stop(NULL)
{
	// redirect log messages
	if (blockConsole) {
		m_stop = new StopLogOutputter;
		CLOG->insert(m_stop);
	}
	m_syslog = new SystemLogOutputter;
	m_syslog->open(title);
	CLOG->insert(m_syslog);
}

SystemLogger::~SystemLogger()
{
	CLOG->remove(m_syslog);
	delete m_syslog;
	if (m_stop != NULL) {
		CLOG->remove(m_stop);
		delete m_stop;
	}
}


//
// BufferedLogOutputter
//

BufferedLogOutputter::BufferedLogOutputter(UInt32 maxBufferSize) :
	m_maxBufferSize(maxBufferSize)
{
	// do nothing
}

BufferedLogOutputter::~BufferedLogOutputter()
{
	// do nothing
}

BufferedLogOutputter::const_iterator
BufferedLogOutputter::begin() const
{
	return m_buffer.begin();
}

BufferedLogOutputter::const_iterator
BufferedLogOutputter::end() const
{
	return m_buffer.end();
}

void
BufferedLogOutputter::open(const nchar*)
{
	// do nothing
}

void
BufferedLogOutputter::close()
{
	// remove all elements from the buffer
	m_buffer.clear();
}

void
BufferedLogOutputter::show(bool)
{
	// do nothing
}

bool
BufferedLogOutputter::write(ELevel, const nchar* message)
{
	while (m_buffer.size() >= m_maxBufferSize) {
		m_buffer.pop_front();
	}
	m_buffer.push_back(nstring(message));
	return true;
}


//
// FileLogOutputter
//

FileLogOutputter::FileLogOutputter(const nchar* logFile)
{
	setLogFilename(logFile);
}

FileLogOutputter::~FileLogOutputter()
{
}

void
FileLogOutputter::setLogFilename(const nchar* logFile)
{
	assert(logFile != NULL);
	m_fileName = logFile;
}

bool
FileLogOutputter::write(ELevel level, const nchar *message)
{
	bool moveFile = false;

	std::ofstream m_handle;
	m_handle.open(m_fileName.c_str(), std::fstream::app);
	if (m_handle.is_open() && m_handle.fail() != true) {
		m_handle << message << std::endl;

		// when file size exceeds limits, move to 'old log' filename.
		size_t p = m_handle.tellp();
		if (p > (kFileSizeLimit * 1024)) {
			moveFile = true;
		}
	}
	m_handle.close();

	if (moveFile) {
		nstring oldLogFilename = synergy::string::sprintf(N"%s.1", m_fileName.c_str());
		remove(oldLogFilename.c_str());
		rename(m_fileName.c_str(), oldLogFilename.c_str());
	}

	return true;
}

void
FileLogOutputter::open(const nchar *title) {}

void
FileLogOutputter::close() {}

void
FileLogOutputter::show(bool showIfEmpty) {}

//
// MesssageBoxLogOutputter
//

MesssageBoxLogOutputter::MesssageBoxLogOutputter()
{
	// do nothing
}

MesssageBoxLogOutputter::~MesssageBoxLogOutputter()
{
	// do nothing
}

void
MesssageBoxLogOutputter::open(const nchar* title)
{
	// do nothing
}

void
MesssageBoxLogOutputter::close()
{
	// do nothing
}

void
MesssageBoxLogOutputter::show(bool showIfEmpty)
{
	// do nothing
}

bool
MesssageBoxLogOutputter::write(ELevel level, const nchar* msg)
{
	// don't spam user with messages.
	if (level > kERROR) {
		return true;
	}

#if SYSAPI_WIN32
	MessageBox(NULL, msg, CLOG->getFilterName(level), MB_OK);
#endif

	return true;
}
