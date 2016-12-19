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

#include "synergy/ArgParser.h"
#include "synergy/ArgsBase.h"
#include "test/mock/synergy/MockApp.h"

#include "test/global/gtest.h"

using namespace synergy;
using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;

bool g_helpShowed = false;
bool g_versionShowed = false;

void
showMockHelp()
{
	g_helpShowed = true;
}

void
showMockVersion()
{
	g_versionShowed = true;
}

TEST(GenericArgsParsingTests, parseGenericArgs_logLevelCmd_setLogLevel)
{
	int i = 1;
	const int argc = 3;
	const nchar* kLogLevelCmd[argc] = { N"stub", N"--debug", N"DEBUG" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kLogLevelCmd, i);

	nstring logFilter(argsBase.m_logFilter);

	EXPECT_EQ("DEBUG", logFilter);
	EXPECT_EQ(2, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_logFileCmd_saveLogFilename)
{
	int i = 1;
	const int argc = 3;
	const nchar* kLogFileCmd[argc] = { N"stub", N"--log", N"mock_filename" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kLogFileCmd, i);

	nstring logFile(argsBase.m_logFile);

	EXPECT_EQ("mock_filename", logFile);
	EXPECT_EQ(2, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_logFileCmdWithSpace_saveLogFilename)
{
	int i = 1;
	const int argc = 3;
	const nchar* kLogFileCmdWithSpace[argc] = { N"stub", N"--log", N"mo ck_filename" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kLogFileCmdWithSpace, i);

	nstring logFile(argsBase.m_logFile);

	EXPECT_EQ("mo ck_filename", logFile);
	EXPECT_EQ(2, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_noDeamonCmd_daemonFalse)
{
	int i = 1;
	const int argc = 2;
	const nchar* kNoDeamonCmd[argc] = { N"stub", N"-f" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kNoDeamonCmd, i);

	EXPECT_FALSE(argsBase.m_daemon);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_deamonCmd_daemonTrue)
{
	int i = 1;
	const int argc = 2;
	const nchar* kDeamonCmd[argc] = { N"stub", N"--daemon" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kDeamonCmd, i);

	EXPECT_EQ(true, argsBase.m_daemon);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_nameCmd_saveName)
{
	int i = 1;
	const int argc = 3;
	const nchar* kNameCmd[argc] = { N"stub", N"--name", N"mock" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kNameCmd, i);

	EXPECT_EQ("mock", argsBase.m_name);
	EXPECT_EQ(2, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_noRestartCmd_restartFalse)
{
	int i = 1;
	const int argc = 2;
	const nchar* kNoRestartCmd[argc] = { N"stub", N"--no-restart" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kNoRestartCmd, i);

	EXPECT_FALSE(argsBase.m_restartable);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_restartCmd_restartTrue)
{
	int i = 1;
	const int argc = 2;
	const nchar* kRestartCmd[argc] = { N"stub", N"--restart" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kRestartCmd, i);

	EXPECT_EQ(true, argsBase.m_restartable);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_backendCmd_backendTrue)
{
	int i = 1;
	const int argc = 2;
	const nchar* kBackendCmd[argc] = { N"stub", N"-z" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kBackendCmd, i);

	EXPECT_EQ(true, argsBase.m_backend);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_noHookCmd_noHookTrue)
{
	int i = 1;
	const int argc = 2;
	const nchar* kNoHookCmd[argc] = { N"stub", N"--no-hooks" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kNoHookCmd, i);

	EXPECT_EQ(true, argsBase.m_noHooks);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_helpCmd_showHelp)
{
	g_helpShowed = false;
	int i = 1;
	const int argc = 2;
	const nchar* kHelpCmd[argc] = { N"stub", N"--help" };

	NiceMock<MockApp> app;
	ArgParser argParser(&app);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);
	ON_CALL(app, help()).WillByDefault(Invoke(showMockHelp));

	argParser.parseGenericArgs(argc, kHelpCmd, i);

	EXPECT_EQ(true, g_helpShowed);
	EXPECT_EQ(1, i);
}


TEST(GenericArgsParsingTests, parseGenericArgs_versionCmd_showVersion)
{
	g_versionShowed = false;
	int i = 1;
	const int argc = 2;
	const nchar* kVersionCmd[argc] = { N"stub", N"--version" };

	NiceMock<MockApp> app;
	ArgParser argParser(&app);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);
	ON_CALL(app, version()).WillByDefault(Invoke(showMockVersion));

	argParser.parseGenericArgs(argc, kVersionCmd, i);

	EXPECT_EQ(true, g_versionShowed);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_noTrayCmd_disableTrayTrue)
{
	int i = 1;
	const int argc = 2;
	const nchar* kNoTrayCmd[argc] = { N"stub", N"--no-tray" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kNoTrayCmd, i);

	EXPECT_EQ(true, argsBase.m_disableTray);
	EXPECT_EQ(1, i);
}

TEST(GenericArgsParsingTests, parseGenericArgs_ipcCmd_enableIpcTrue)
{
	int i = 1;
	const int argc = 2;
	const nchar* kIpcCmd[argc] = { N"stub", N"--ipc" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kIpcCmd, i);

	EXPECT_EQ(true, argsBase.m_enableIpc);
	EXPECT_EQ(1, i);
}

#ifndef  WINAPI_XWINDOWS
TEST(GenericArgsParsingTests, parseGenericArgs_dragDropCmdOnNonLinux_enableDragDropTrue)
{
	int i = 1;
	const int argc = 2;
	const nchar* kDragDropCmd[argc] = { N"stub", N"--enable-drag-drop" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kDragDropCmd, i);

	EXPECT_EQ(true, argsBase.m_enableDragDrop);
	EXPECT_EQ(1, i);
}
#endif

#ifdef  WINAPI_XWINDOWS
TEST(GenericArgsParsingTests, parseGenericArgs_dragDropCmdOnLinux_enableDragDropFalse)
{
	int i = 1;
	const int argc = 2;
	const nchar* kDragDropCmd[argc] = { N"stub",N"--enable-drag-drop" };

	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	argParser.parseGenericArgs(argc, kDragDropCmd, i);

	EXPECT_FALSE(argsBase.m_enableDragDrop);
	EXPECT_EQ(1, i);
}
#endif
