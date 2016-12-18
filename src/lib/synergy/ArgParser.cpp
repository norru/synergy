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

#include "synergy/StreamChunker.h"
#include "synergy/App.h"
#include "synergy/ServerArgs.h"
#include "synergy/ClientArgs.h"
#include "synergy/ToolArgs.h"
#include "synergy/ArgsBase.h"
#include "base/Log.h"
#include "base/String.h"

#ifdef WINAPI_MSWINDOWS
#include <VersionHelpers.h>
#endif

ArgsBase* ArgParser::m_argsBase = NULL;

ArgParser::ArgParser(App* app) :
	m_app(app)
{
}

bool
ArgParser::parseServerArgs(ServerArgs& args, int argc, const nchar* const* argv)
{
	setArgsBase(args);
	updateCommonArgs(argv);

	for (int i = 1; i < argc; ++i) {
		if (parsePlatformArg(args, argc, argv, i)) {
			continue;
		}
		else if (parseGenericArgs(argc, argv, i)) {
			continue;
		}
		else if (parseDeprecatedArgs(argc, argv, i)) {
			continue;
		}
		else if (isArg(i, argc, argv, _N("-a"), _N("--address"), 1)) {
			// save listen address
			args.m_synergyAddress = argv[++i];
		}
		else if (isArg(i, argc, argv, _N("-c"), _N("--config"), 1)) {
			// save configuration file path
			args.m_configFile = argv[++i];
		}
		else {
			LOG((CLOG_PRINT _N("%" _NF ": unrecognized option `%" _NF "'" BYE), args.m_pname, argv[i], args.m_pname));
			return false;
		}
	}

	if (checkUnexpectedArgs()) {
		return false;
	}

	return true;
}

bool
ArgParser::parseClientArgs(ClientArgs& args, int argc, const nchar* const* argv)
{
	setArgsBase(args);
	updateCommonArgs(argv);

	int i;
	for (i = 1; i < argc; ++i) {
		if (parsePlatformArg(args, argc, argv, i)) {
			continue;
		}
		else if (parseGenericArgs(argc, argv, i)) {
			continue;
		}
		else if (parseDeprecatedArgs(argc, argv, i)) {
			continue;
		}
		else if (isArg(i, argc, argv, NULL, _N("--camp"))) {
			// ignore -- included for backwards compatibility
		}
		else if (isArg(i, argc, argv, NULL, _N("--no-camp"))) {
			// ignore -- included for backwards compatibility
		}
		else if (isArg(i, argc, argv, NULL, _N("--yscroll"), 1)) {
			// define scroll
			args.m_yscroll = atoi(argv[++i]);
		}
		else {
			if (i + 1 == argc) {
				args.m_synergyAddress = argv[i];
				return true;
			}

			LOG((CLOG_PRINT _N("%" _NF ": unrecognized option `%" _NF "'" BYE), args.m_pname, argv[i], args.m_pname));
			return false;
		}
	}

	// exactly one non-option argument (server-address)
	if (i == argc) {
		LOG((CLOG_PRINT _N("%" _NF ": a server address or name is required" BYE),
			args.m_pname, args.m_pname));
		return false;
	}

	if (checkUnexpectedArgs()) {
		return false;
	}

	return true;
}

bool
ArgParser::parsePlatformArg(ArgsBase& argsBase, const int& argc, const nchar* const* argv, int& i)
{
#if WINAPI_MSWINDOWS
	if (isArg(i, argc, argv, NULL, _N("--service"))) {
		LOG((CLOG_WARN _N("obsolete argument --service, use synergyd instead.")));
		argsBase.m_shouldExit = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--exit-pause"))) {
		argsBase.m_pauseOnExit = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--stop-on-desk-switch"))) {
		argsBase.m_stopOnDeskSwitch = true;
	}
	else {
		// option not supported here
		return false;
	}

	return true;
#elif WINAPI_XWINDOWS
	if (isArg(i, argc, argv, _N("-display"), _N("--display"), 1)) {
		// use alternative display
		argsBase.m_display = argv[++i];
	}

	else if (isArg(i, argc, argv, NULL, _N("--no-xinitthreads"))) {
		argsBase.m_disableXInitThreads = true;
	}

	else {
		// option not supported here
		return false;
	}

	return true;
#elif WINAPI_CARBON
	// no options for carbon
	return false;
#endif
}

bool
ArgParser::parseToolArgs(ToolArgs& args, int argc, const nchar* const* argv)
{
	for (int i = 1; i < argc; ++i) {
		if (isArg(i, argc, argv, NULL, _N("--get-active-desktop"), 0)) {
			args.m_printActiveDesktopName = true;
			return true;
		}
		else if (isArg(i, argc, argv, NULL, _N("--get-installed-dir"), 0)) {
			args.m_getInstalledDir = true;
			return true;
		}
		else if (isArg(i, argc, argv, NULL, _N("--get-profile-dir"), 0)) {
			args.m_getProfileDir = true;
			return true;
		}
		else if (isArg(i, argc, argv, NULL, _N("--get-arch"), 0)) {
			args.m_getArch = true;
			return true;
		}
		else {
			return false;
		}
	}

	return false;
}

bool
ArgParser::parseGenericArgs(int argc, const nchar* const* argv, int& i)
{
	if (isArg(i, argc, argv, _N("-d"), _N("--debug"), 1)) {
		// change logging level
		argsBase().m_logFilter = argv[++i];
	}
	else if (isArg(i, argc, argv, _N("-l"), _N("--log"), 1)) {
		argsBase().m_logFile = argv[++i];
	}
	else if (isArg(i, argc, argv, _N("-f"), _N("--no-daemon"))) {
		// not a daemon
		argsBase().m_daemon = false;
	}
	else if (isArg(i, argc, argv, NULL, _N("--daemon"))) {
		// daemonize
		argsBase().m_daemon = true;
	}
	else if (isArg(i, argc, argv, _N("-n"), _N("--name"), 1)) {
		// save screen name
		argsBase().m_name = argv[++i];
	}
	else if (isArg(i, argc, argv, _N("-1"), _N("--no-restart"))) {
		// don't try to restart
		argsBase().m_restartable = false;
	}
	else if (isArg(i, argc, argv, NULL, _N("--restart"))) {
		// try to restart
		argsBase().m_restartable = true;
	}
	else if (isArg(i, argc, argv, _N("-z"), NULL)) {
		argsBase().m_backend = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--no-hooks"))) {
		argsBase().m_noHooks = true;
	}
	else if (isArg(i, argc, argv, _N("-h"), _N("--help"))) {
		if (m_app) {
			m_app->help();
		}
		argsBase().m_shouldExit = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--version"))) {
		if (m_app) {
			m_app->version();
		}
		argsBase().m_shouldExit = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--no-tray"))) {
		argsBase().m_disableTray = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--ipc"))) {
		argsBase().m_enableIpc = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--server"))) {
		// HACK: stop error happening when using portable (synergyp)
	}
	else if (isArg(i, argc, argv, NULL, _N("--client"))) {
		// HACK: stop error happening when using portable (synergyp)
	}
	else if (isArg(i, argc, argv, NULL, _N("--enable-drag-drop"))) {
		bool useDragDrop = true;

#ifdef WINAPI_XWINDOWS

		useDragDrop = false;
		LOG((CLOG_INFO _N("ignoring --enable-drag-drop, not supported on linux.")));

#endif

#ifdef WINAPI_MSWINDOWS

		if (!IsWindowsVistaOrGreater()) {
			useDragDrop = false;
			LOG((CLOG_INFO _N("ignoring --enable-drag-drop, not supported below vista.")));
		}
#endif

		if (useDragDrop) {
			argsBase().m_enableDragDrop = true;
		}
	}
	else if (isArg(i, argc, argv, NULL, _N("--enable-crypto"))) {
		argsBase().m_enableCrypto = true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--profile-dir"), 1)) {
		argsBase().m_profileDirectory = argv[++i];
	}
	else if (isArg(i, argc, argv, NULL, _N("--plugin-dir"), 1)) {
		argsBase().m_pluginDirectory = argv[++i];
	}
	else {
		// option not supported here
		return false;
	}

	return true;
}

bool
ArgParser::parseDeprecatedArgs(int argc, const nchar* const* argv, int& i)
{
	if (isArg(i, argc, argv, NULL, _N("--crypto-pass"))) {
		LOG((CLOG_NOTE _N("--crypto-pass is deprecated")));
		i++;
		return true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--res-w"))) {
		LOG((CLOG_NOTE _N("--res-w is deprecated")));
		i++;
		return true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--res-h"))) {
		LOG((CLOG_NOTE _N("--res-h is deprecated")));
		i++;
		return true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--prm-wc"))) {
		LOG((CLOG_NOTE _N("--prm-wc is deprecated")));
		i++;
		return true;
	}
	else if (isArg(i, argc, argv, NULL, _N("--prm-hc"))) {
		LOG((CLOG_NOTE _N("--prm-hc is deprecated")));
		i++;
		return true;
	}

	return false;
}

bool
ArgParser::isArg(
	int argi, int argc, const nchar* const* argv,
	const nchar* name1, const nchar* name2,
	int minRequiredParameters)
{
	if ((name1 != NULL && strcmp(argv[argi], name1) == 0) ||
		(name2 != NULL && strcmp(argv[argi], name2) == 0)) {
			// match.  check args left.
			if (argi + minRequiredParameters >= argc) {
				LOG((CLOG_PRINT _N("%" _NF ": missing arguments for `%" _NF "'" BYE),
					argsBase().m_pname, argv[argi], argsBase().m_pname));
				argsBase().m_shouldExit = true;
				return false;
			}
			return true;
	}

	// no match
	return false;
}

void
ArgParser::splitCommandString(nstring& command, std::vector<nstring>& argv)
{
	if (command.empty()) {
		return ;
	}

	size_t leftDoubleQuote = 0;
	size_t rightDoubleQuote = 0;
	searchDoubleQuotes(command, leftDoubleQuote, rightDoubleQuote);

	size_t startPos = 0;
	size_t space = command.find(_N(" "), startPos);

	while (space != nstring::npos) {
		bool ignoreThisSpace = false;

		// check if the space is between two double quotes
		if (space > leftDoubleQuote && space < rightDoubleQuote) {
			ignoreThisSpace = true;
		}
		else if (space > rightDoubleQuote){
			searchDoubleQuotes(command, leftDoubleQuote, rightDoubleQuote, rightDoubleQuote + 1);
		}

		if (!ignoreThisSpace) {
			nstring subString = command.substr(startPos, space - startPos);

			removeDoubleQuotes(subString);
			argv.push_back(subString);
		}

		// find next space
		if (ignoreThisSpace) {
			space = command.find(_N(" "), rightDoubleQuote + 1);
		}
		else {
			startPos = space + 1;
			space = command.find(_N(" "), startPos);
		}
	}

	nstring subString = command.substr(startPos, command.size());
	removeDoubleQuotes(subString);
	argv.push_back(subString);
}

bool
ArgParser::searchDoubleQuotes(nstring& command, size_t& left, size_t& right, size_t startPos)
{
	bool result = false;
	left = nstring::npos;
	right = nstring::npos;

	left = command.find(_N("\""), startPos);
	if (left != nstring::npos) {
		right = command.find(_N("\""), left + 1);
		if (right != nstring::npos) {
			result = true;
		}
	}

	if (!result) {
		left = 0;
		right = 0;
	}

	return result;
}

void
ArgParser::removeDoubleQuotes(nstring& arg)
{
	// if string is surrounded by double quotes, remove them
	if (arg[0] == _N('\"') &&
		arg[arg.size() - 1] == _N('\"')) {
		arg = arg.substr(1, arg.size() - 2);
	}
}

const char**
ArgParser::getArgv(std::vector<nstring>& argsArray)
{
	size_t argc = argsArray.size();

	// caller is responsible for deleting the outer array only
	// we use the c string pointers from argsArray and assign
	// them to the inner array. So caller only need to use
	// delete[] to delete the outer array
	const char** argv = new const char*[argc];

	for (size_t i = 0; i < argc; i++) {
		argv[i] = argsArray[i].c_str();
	}

	return argv;
}

nstring
ArgParser::assembleCommand(std::vector<nstring>& argsArray,  nstring ignoreArg, int parametersRequired)
{
	nstring result;

	for (std::vector<nstring>::iterator it = argsArray.begin(); it != argsArray.end(); ++it) {
		if (it->compare(ignoreArg) == 0) {
			it = it + parametersRequired;
			continue;
		}

		// if there is a space in this arg, use double quotes surround it
		if ((*it).find(" ") != nstring::npos) {
			(*it).insert(0, _N("\""));
			(*it).push_back(_N('\"'));
		}

		result.append(*it);
		// add space to saperate args
		result.append(" ");
	}

	if (!result.empty()) {
		// remove the tail space
		result = result.substr(0, result.size() - 1);
	}

	return result;
}

void
ArgParser::updateCommonArgs(const nchar* const* argv)
{
	argsBase().m_name = ARCH->getHostName();
	argsBase().m_pname = ARCH->getBasename(argv[0]);
}

bool
ArgParser::checkUnexpectedArgs()
{
#if SYSAPI_WIN32
	// suggest that user installs as a windows service. when launched as
	// service, process should automatically detect that it should run in
	// daemon mode.
	if (argsBase().m_daemon) {
		LOG((CLOG_ERR
			_N("the --daemon argument is not supported on windows. ")
			_N("instead, install %" _NF " as a service (--service install)"),
			argsBase().m_pname));
		return true;
	}
#endif

	return false;
}
