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

#include "test/global/gtest.h"

TEST(ArgParserTests, isArg_abbreviationsArg_returnTrue)
{
	int i = 1;
	const int argc = 2;
	const char* argv[argc] = { _N("stub"), _N("-t") };
	bool result = ArgParser::isArg(i, argc, argv, _N("-t"), NULL);

	EXPECT_EQ(true, result);
}

TEST(ArgParserTests, isArg_fullArg_returnTrue)
{
	int i = 1;
	const int argc = 2;
	const char* argv[argc] = { _N("stub"), _N("--test") };
	bool result = ArgParser::isArg(i, argc, argv, NULL, _N("--test"));

	EXPECT_EQ(true, result);
}

TEST(ArgParserTests, isArg_missingArgs_returnFalse)
{
	int i = 1;
	const int argc = 2;
	const char* argv[argc] = { _N("stub"), _N("-t") };
	ArgParser argParser(NULL);
	ArgsBase argsBase;
	argParser.setArgsBase(argsBase);

	bool result = ArgParser::isArg(i, argc, argv, _N("-t"), NULL, 1);

	EXPECT_FALSE(result);
	EXPECT_EQ(true, argsBase.m_shouldExit);
}

TEST(ArgParserTests, searchDoubleQuotes_doubleQuotedArg_returnTrue)
{
	nstring command(_N("\"stub\""));
	size_t left = 0;
	size_t right = 0;

	bool result = ArgParser::searchDoubleQuotes(command, left, right);

	EXPECT_EQ(true, result);
	EXPECT_EQ(0, left);
	EXPECT_EQ(5, right);
}

TEST(ArgParserTests, searchDoubleQuotes_noDoubleQuotedArg_returnfalse)
{
	nstring command(_N("stub"));
	size_t left = 0;
	size_t right = 0;

	bool result = ArgParser::searchDoubleQuotes(command, left, right);

	EXPECT_FALSE(result);
	EXPECT_EQ(0, left);
	EXPECT_EQ(0, right);
}

TEST(ArgParserTests, searchDoubleQuotes_oneDoubleQuoteArg_returnfalse)
{
	nstring command(_N("\"stub"));
	size_t left = 0;
	size_t right = 0;

	bool result = ArgParser::searchDoubleQuotes(command, left, right);

	EXPECT_FALSE(result);
	EXPECT_EQ(0, left);
	EXPECT_EQ(0, right);
}

TEST(ArgParserTests, splitCommandString_oneArg_returnArgv)
{
	nstring command(_N("stub"));
	std::vector<nstring> argv;

	ArgParser::splitCommandString(command, argv);

	EXPECT_EQ(1, argv.size());
	EXPECT_EQ(_N("stub"), argv.at(0));
}

TEST(ArgParserTests, splitCommandString_twoArgs_returnArgv)
{
	nstring command(_N("stub1 stub2"));
	std::vector<nstring> argv;

	ArgParser::splitCommandString(command, argv);

	EXPECT_EQ(2, argv.size());
	EXPECT_EQ(_N("stub1"), argv.at(0));
	EXPECT_EQ(_N("stub2"), argv.at(1));
}

TEST(ArgParserTests, splitCommandString_doubleQuotedArgs_returnArgv)
{
	nstring command(_N("\"stub1\" stub2 \"stub3\""));
	std::vector<nstring> argv;

	ArgParser::splitCommandString(command, argv);

	EXPECT_EQ(3, argv.size());
	EXPECT_EQ(_N("stub1"), argv.at(0));
	EXPECT_EQ(_N("stub2"), argv.at(1));
	EXPECT_EQ(_N("stub3"), argv.at(2));
}

TEST(ArgParserTests, splitCommandString_spaceDoubleQuotedArgs_returnArgv)
{
	nstring command(_N("\"stub1\" stub2 \"stub3 space\""));
	std::vector<nstring> argv;

	ArgParser::splitCommandString(command, argv);

	EXPECT_EQ(3, argv.size());
	EXPECT_EQ(_N("stub1"), argv.at(0));
	EXPECT_EQ(_N("stub2"), argv.at(1));
	EXPECT_EQ(_N("stub3 space"), argv.at(2));
}

TEST(ArgParserTests, getArgv_stringArray_return2DArray)
{
	std::vector<nstring> argArray;
	argArray.push_back(_N("stub1"));
	argArray.push_back(_N("stub2"));
	argArray.push_back(_N("stub3 space"));
	const nchar** argv = ArgParser::getArgv(argArray);

	nstring row1(argv[0]);
	nstring row2(argv[1]);
	nstring row3(argv[2]);

	EXPECT_EQ(_N("stub1"), row1);
	EXPECT_EQ(_N("stub2"), row2);
	EXPECT_EQ(_N("stub3 space"), row3);

	delete[] argv;
}

TEST(ArgParserTests, assembleCommand_stringArray_returnCommand)
{
	std::vector<nstring> argArray;
	argArray.push_back(_N("stub1"));
	argArray.push_back(_N("stub2"));
	nstring command = ArgParser::assembleCommand(argArray);

	EXPECT_EQ(_N("stub1 stub2"), command);
}

TEST(ArgParserTests, assembleCommand_ignoreSecondArg_returnCommand)
{
	std::vector<nstring> argArray;
	argArray.push_back(_N("stub1"));
	argArray.push_back(_N("stub2"));
	nstring command = ArgParser::assembleCommand(argArray, _N("stub2"));

	EXPECT_EQ(_N("stub1"), command);
}

TEST(ArgParserTests, assembleCommand_ignoreSecondArgWithOneParameter_returnCommand)
{
	std::vector<nstring> argArray;
	argArray.push_back(_N("stub1"));
	argArray.push_back(_N("stub2"));
	argArray.push_back(_N("stub3"));
	argArray.push_back(_N("stub4"));
	nstring command = ArgParser::assembleCommand(argArray, _N("stub2"), 1);

	EXPECT_EQ(_N("stub1 stub4"), command);
}

TEST(ArgParserTests, assembleCommand_stringArrayWithSpace_returnCommand)
{
	std::vector<nstring> argArray;
	argArray.push_back(_N("stub1 space"));
	argArray.push_back(_N("stub2"));
	argArray.push_back(_N("stub3 space"));
	nstring command = ArgParser::assembleCommand(argArray);

	EXPECT_EQ(_N("\"stub1 space\" stub2 \"stub3 space\""), command);
}

