/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2012-2016 Symless Ltd.
 * Copyright (C) 2012 Nick Bolton
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

// TODO: split this class into windows and unix to get rid
// of all the #ifdefs!

#include "synergy/DaemonApp.h"

#include "synergy/App.h"
#include "synergy/ArgParser.h"
#include "synergy/ServerArgs.h"
#include "synergy/ClientArgs.h"
#include "ipc/IpcClientProxy.h"
#include "ipc/IpcMessage.h"
#include "ipc/IpcLogOutputter.h"
#include "net/SocketMultiplexer.h"
#include "arch/XArch.h"
#include "base/Log.h"
#include "base/TMethodJob.h"
#include "base/TMethodEventJob.h"
#include "base/EventQueue.h"
#include "base/log_outputters.h"
#include "base/Log.h"

#if SYSAPI_WIN32

#include "arch/win32/ArchMiscWindows.h"
#include "arch/win32/XArchWindows.h"
#include "synergy/Screen.h"
#include "platform/MSWindowsScreen.h"
#include "platform/MSWindowsDebugOutputter.h"
#include "platform/MSWindowsWatchdog.h"
#include "platform/MSWindowsEventQueueBuffer.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

DaemonApp* DaemonApp::s_instance = NULL;

int
mainLoopStatic()
{
	DaemonApp::s_instance->mainLoop(true);
	return kExitSuccess;
}

int
unixMainLoopStatic(int, const nchar**)
{
	return mainLoopStatic();
}

#if SYSAPI_WIN32
int
winMainLoopStatic(int, const nchar**)
{
	return ArchMiscWindows::runDaemon(mainLoopStatic);
}
#endif

DaemonApp::DaemonApp() :
	m_ipcServer(nullptr),
	m_ipcLogOutputter(nullptr),
	#if SYSAPI_WIN32
	m_watchdog(nullptr),
	#endif
	m_events(nullptr),
	m_fileLogOutputter(nullptr)
{
	s_instance = this;
}

DaemonApp::~DaemonApp()
{
}

int
DaemonApp::run(int argc, nchar** argv)
{
#if SYSAPI_WIN32
	// win32 instance needed for threading, etc.
	ArchMiscWindows::setInstanceWin32(GetModuleHandle(NULL));
#endif

	Arch arch;
	arch.init();

	Log log;
	EventQueue events;
	m_events = &events;

	bool uninstall = false;
	try
	{
#if SYSAPI_WIN32
		// sends debug messages to visual studio console window.
		log.insert(new MSWindowsDebugOutputter());
#endif

		// default log level to system setting.
		string logLevel = arch.setting(_N("LogLevel"));
		if (!logLevel.empty())
			log.setFilter(logLevel.c_str());

		bool foreground = false;

		for (int i = 1; i < argc; ++i) {
			string arg(argv[i]);

			if (arg == _N("/f") || arg == _N("-f")) {
				foreground = true;
			}
#if SYSAPI_WIN32
			else if (arg == _N("/install")) {
				uninstall = true;
				arch.installDaemon();
				return kExitSuccess;
			}
			else if (arg == _N("/uninstall")) {
				arch.uninstallDaemon();
				return kExitSuccess;
			}
#endif
			else {
				nstringstream ss;
				ss << _N("Unrecognized argument: ") << arg;
				foregroundError(ss.str().c_str());
				return kExitArgs;
			}
		}

		if (foreground) {
			// run process in foreground instead of daemonizing.
			// useful for debugging.
			mainLoop(false);
		}
		else {
#if SYSAPI_WIN32
			arch.daemonize(_N("Synergy"), winMainLoopStatic);
#elif SYSAPI_UNIX
			arch.daemonize(_N("Synergy"), unixMainLoopStatic);
#endif
		}

		return kExitSuccess;
	}
	catch (XArch& e) {
		nstring message = e.what();
		if (uninstall && (message.find(_N("The service has not been started")) != nstring::npos)) {
			// TODO: if we're keeping this use error code instead (what is it?!).
			// HACK: this message happens intermittently, not sure where from but
			// it's quite misleading for the user. they thing something has gone
			// horribly wrong, but it's just the service manager reporting a false
			// positive (the service has actually shut down in most cases).
		}
		else {
			foregroundError(message.c_str());
		}
		return kExitFailed;
	}
	catch (std::exception& e) {
		foregroundError(e.what());
		return kExitFailed;
	}
	catch (...) {
		foregroundError(_N("Unrecognized error."));
		return kExitFailed;
	}
}

void
DaemonApp::mainLoop(bool logToFile)
{
	try
	{
		DAEMON_RUNNING(true);

		if (logToFile) {
			m_fileLogOutputter = new FileLogOutputter(logFilename().c_str());
			CLOG->insert(m_fileLogOutputter);
		}

		// create socket multiplexer.  this must happen after daemonization
		// on unix because threads evaporate across a fork().
		SocketMultiplexer multiplexer;

		// uses event queue, must be created here.
		m_ipcServer = new IpcServer(m_events, &multiplexer);

		// send logging to gui via ipc, log system adopts outputter.
		m_ipcLogOutputter = new IpcLogOutputter(*m_ipcServer, kIpcClientGui, true);
		CLOG->insert(m_ipcLogOutputter);

#if SYSAPI_WIN32
		m_watchdog = new MSWindowsWatchdog(false, *m_ipcServer, *m_ipcLogOutputter);
		m_watchdog->setFileLogOutputter(m_fileLogOutputter);
#endif

		m_events->adoptHandler(
			m_events->forIpcServer().messageReceived(), m_ipcServer,
			new TMethodEventJob<DaemonApp>(this, &DaemonApp::handleIpcMessage));

		m_ipcServer->listen();

#if SYSAPI_WIN32

		// install the platform event queue to handle service stop events.
		m_events->adoptBuffer(new MSWindowsEventQueueBuffer(m_events));

		nstring command = ARCH->setting(L"Command");
		bool elevate = ARCH->setting(L"Elevate" == L"1";
		if (command != L"") {
			LOG((CLOG_INFO L"using last known command: %ls"), command.c_str()));
			m_watchdog->setCommand(command, elevate);
		}

		m_watchdog->startAsync();
#endif
		m_events->loop();

#if SYSAPI_WIN32
		m_watchdog->stop();
		delete m_watchdog;
#endif

		m_events->removeHandler(
			m_events->forIpcServer().messageReceived(), m_ipcServer);

		CLOG->remove(m_ipcLogOutputter);
		delete m_ipcLogOutputter;
		delete m_ipcServer;

		DAEMON_RUNNING(false);
	}
	catch (std::exception& e) {
		LOG((CLOG_CRIT _N("An error occurred: %" _NF), e.what()));
	}
	catch (...) {
		LOG((CLOG_CRIT _N("An unknown error occurred.\n")));
	}
}

void
DaemonApp::foregroundError(const nchar* message)
{
#if SYSAPI_WIN32
	MessageBoxW(NULL, message, L"Synergy Service", MB_OK | MB_ICONERROR);
#elif SYSAPI_UNIX
	cerr << message << endl;
#endif
}

std::string
DaemonApp::logFilename()
{
	string logFilename;
	logFilename = ARCH->setting(_N("LogFilename"));
	if (logFilename.empty()) {
		logFilename = ARCH->getLogDirectory();
		logFilename.append(_N("/"));
		logFilename.append(LOG_FILENAME);
	}

	return logFilename;
}

void
DaemonApp::handleIpcMessage(const Event& e, void*)
{
	IpcMessage* m = static_cast<IpcMessage*>(e.getDataObject());
	switch (m->type()) {
		case kIpcCommand: {
			IpcCommandMessage* cm = static_cast<IpcCommandMessage*>(m);
			nstring command = cm->command();

			// if empty quotes, clear.
			if (command == _N("\"\"")) {
				command.clear();
			}

			if (!command.empty()) {
				LOG((CLOG_DEBUG _N("new command, elevate=%d command=%" _NF), cm->elevate(), command.c_str()));

				std::vector<nstring> argsArray;
				ArgParser::splitCommandString(command, argsArray);
				ArgParser argParser(NULL);
				const nchar** argv = argParser.getArgv(argsArray);
				ServerArgs serverArgs;
				ClientArgs clientArgs;
				int argc = static_cast<int>(argsArray.size());
				bool server = argsArray[0].find(_N("synergys")) != nstring::npos ? true : false;
				ArgsBase* argBase = NULL;

				if (server) {
					argParser.parseServerArgs(serverArgs, argc, argv);
					argBase = &serverArgs;
				}
				else {
					argParser.parseClientArgs(clientArgs, argc, argv);
					argBase = &clientArgs;
				}

				delete[] argv;

				nstring logLevel(argBase->m_logFilter);
				if (!logLevel.empty()) {
					try {
						// change log level based on that in the command string
						// and change to that log level now.
						ARCH->setting(_N("LogLevel"), logLevel);
						CLOG->setFilter(logLevel.c_str());
					}
					catch (XArch& e) {
						LOG((CLOG_ERR _N("failed to save LogLevel setting, %" _NF), e.what()));
					}
				}

#if SYSAPI_WIN32
				std::wstring logFilename;
				if (argBase->m_logFile != NULL) {
					logFilename = std::string(argBase->m_logFile);
					ARCH->setting(L"LogFilename", logFilename);
					m_watchdog->setFileLogOutputter(m_fileLogOutputter);
					command = ArgParser::assembleCommand(argsArray, L"--log", 1);
					LOG((CLOG_DEBUG L"removed log file argument and filename %ls from command "), logFilename.c_str()));
					LOG((CLOG_DEBUG L"new command, elevate=%d command=%ls"), cm->elevate(), command.c_str()));
				}
				else {
					m_watchdog->setFileLogOutputter(NULL);
				}

				m_fileLogOutputter->setLogFilename(logFilename.c_str());
#endif
			}
			else {
				LOG((CLOG_DEBUG _N("empty command, elevate=%d"), cm->elevate()));
			}

			try {
				// store command in system settings. this is used when the daemon
				// next starts.
				ARCH->setting(_N("Command"), command);

				// TODO: it would be nice to store bools/ints...
				ARCH->setting(_N("Elevate"), nstring(cm->elevate() ? _N("1") : _N("0")));
			}
			catch (XArch& e) {
				LOG((CLOG_ERR _N("failed to save settings, %" _NF), e.what()));
			}

#if SYSAPI_WIN32
			// tell the relauncher about the new command. this causes the
			// relauncher to stop the existing command and start the new
			// command.
			m_watchdog->setCommand(command, cm->elevate());
#endif
			break;
		}

		case kIpcHello:
			IpcHelloMessage* hm = static_cast<IpcHelloMessage*>(m);
			nstring type;
			switch (hm->clientType()) {
				case kIpcClientGui: type = _N("gui"); break;
				case kIpcClientNode: type = _N("node"); break;
				default: type = _N("unknown"); break;
			}

			LOG((CLOG_DEBUG _N("ipc hello, type=%" _NF), type.c_str()));

#if SYSAPI_WIN32
			nstring watchdogStatus = m_watchdog->isProcessActive() ? _N("ok") : _N("error");
			LOG((CLOG_INFO _N("watchdog status: %" _NF), watchdogStatus.c_str()));
#endif

			m_ipcLogOutputter->notifyBuffer();
			break;
	}
}
