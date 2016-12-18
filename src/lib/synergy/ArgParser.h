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

#pragma once

#include "base/String.h"
#include "common/stdvector.h"

class ServerArgs;
class ClientArgs;
class ToolArgs;
class ArgsBase;
class App;

class ArgParser {

public:
	ArgParser(App* app);

	bool				parseServerArgs(ServerArgs& args, int argc, const nchar* const* argv);
	bool				parseClientArgs(ClientArgs& args, int argc, const nchar* const* argv);
	bool				parsePlatformArg(ArgsBase& argsBase, const int& argc, const nchar* const* argv, int& i);
	bool				parseToolArgs(ToolArgs& args, int argc, const nchar* const* argv);
	bool				parseGenericArgs(int argc, const nchar* const* argv, int& i);
	bool				parseDeprecatedArgs(int argc, const nchar* const* argv, int& i);
	void				setArgsBase(ArgsBase& argsBase) { m_argsBase = &argsBase; }

	static	bool		isArg(int argi, int argc, const nchar* const* argv,
							const nchar* name1, const nchar* name2,
							int minRequiredParameters = 0);
	static void			splitCommandString(nstring& command, std::vector<nstring>& argv);
	static bool			searchDoubleQuotes(nstring& command, size_t& left,
							size_t& right, size_t startPos = 0);
	static void			removeDoubleQuotes(nstring& arg);
	static const nchar** getArgv(std::vector<nstring>& argsArray);
	static std::string	assembleCommand(std::vector<nstring>& argsArray,
							nstring ignoreArg = _N(""), int parametersRequired = 0);

private:
	void				updateCommonArgs(const nchar* const* argv);
	bool				checkUnexpectedArgs();

	static ArgsBase&	argsBase() { return *m_argsBase; }

private:
	App*				m_app;

	static ArgsBase*	m_argsBase;
};
