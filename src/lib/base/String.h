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

#include "common/common.h"
#include "common/stdstring.h"

#include <stdarg.h>
#include <vector>

// Native strings. Because Windows uses UTF-16 natively, not UTF-8.
// That means using wchar_t and std::wstring instead of char and std::string
#ifdef SYSAPI_WIN32
	#include <wchar.h>
	#include <cwctype>
	// Wide char string specifier
	#define N L
	// Native [string] format specifier
	#define NF L"%ls"
	// Native [string] format specifier and [N] connecting
	#define NFC L"%ls" L
	#define nchar wchar_t
	#define nstring std::wstring
	#define nstrlen wcslen
	#define nstrcpy wcscpy
	#define nstrcmp wcscmp
	#define nstrstr wcsstr
	#define nstrchr wcschr
	#define nstringstream std::wstringstream
	#define nistringstream std::wistringstream
	#define tonupper towupper
	#define tonlower towlower
	#define nsprintf wsprintf
	#define nprintf wprintf
	#define nifstream std::wifstream
	#define nfstream std::wfstream
	#define ncerr std::wcerr
	#define ncout std::wcout
#else
	#include <cctype>
	#define N
	// Native [string] format specifier
	#define NF "%s"
	#define NFC "%s"
	#define nchar char
	#define nstring std::string
	#define nstrlen strlen
	#define nstrcpy strcpy
	#define nstrcmp strcmp
	#define nstrstr strstr
	#define nstrchr strchr
	#define nstringstream std::stringstream
	#define nistringstream std::istringstream
	#define tonupper toupper
	#define tonlower tolower
	#define nsprintf sprintf
	#define nprintf printf
	#define nifstream std::ifstream
	#define nfstream std::fstream
	#define ncerr std::cerr
	#define ncout std::cout
#endif

namespace synergy {

//! String utilities
/*!
Provides functions for string manipulation.
*/
namespace string {

//! Format positional arguments
/*!
Format a string using positional arguments.  fmt has literal
characters and conversion specifications introduced by `\%':
- \%\%  -- literal `\%'
- \%{n} -- positional element n, n a positive integer, {} are literal

All arguments in the variable list are const nchar*.  Positional
elements are indexed from 1.
*/
nstring format(const nchar* fmt, ...);

//! Format positional arguments
/*!
Same as format() except takes va_list.
*/
nstring vformat(const nchar* fmt, va_list);

//! Print a string using sprintf-style formatting
/*!
Equivalent to sprintf() except the result is returned as a nstring.
*/
nstring sprintf(const nchar* fmt, ...);

//! Find and replace all
/*!
Finds \c find inside \c subject and replaces it with \c replace
*/
void findReplaceAll(nstring& subject, const nstring& find,
	const nstring& replace);

//! Remove file extension
/*!
Finds the last dot and remove all characters from the dot to the end
*/
void removeFileExt(nstring& filename);

#ifdef Q_OS_WIN
//! Convert into hexdecimal
/*!
Convert each character in \c subject into hexdecimal form with \c width
*/
void toHex(std::string& subject, int width, const nchar fill = '0'));
#endif

//! Convert into hexdecimal
/*!
Convert each character in \c subject into hexdecimal form with \c width
*/
void toHex(nstring& subject, int width, const nchar fill ='0'));

#if Q_OS_WIN
//! Convert to all uppercase
/*!
Convert each character in \c subject to uppercase
*/
void uppercase(std::string& subject);
#endif

//! Convert to all uppercase
/*!
Convert each character in \c subject to uppercase
*/
void uppercase(nstring& subject);

//! Remove all specific char in suject
/*!
Remove all specific \c c in \c suject
*/
void removeChar(nstring& subject, const char c);

//! Convert a size type to a string
/*!
Convert an size type to a string
*/
nstring sizeTypeToString(size_t n);

//! Convert a string to a size type
/*!
Convert an a \c string to an size type
*/
size_t stringToSizeType(nstring string);

//! Split a string into substrings
/*!
Split a \c string that separated by a \c c into substrings
*/
std::vector<nstring> splitString(nstring string, const char c);

nstring toNative(std::string s);

nstring toNative(const char* s);

std::string nativeToUtf8(const nchar* s);

std::string nativeToUtf8(nstring s);

//! Case-insensitive comparisons
/*!
This class provides case-insensitve comparison functions.
*/
class CaselessCmp {
	public:
	//! Same as less()
	bool			operator()(const nstring& a, const nstring& b) const;

	//! Returns true iff \c a is lexicographically less than \c b
	static bool		less(const nstring& a, const nstring& b);

	//! Returns true iff \c a is lexicographically equal to \c b
	static bool		equal(const nstring& a, const nstring& b);

	//! Returns true iff \c a is lexicographically less than \c b
	static bool		cmpLess(const nstring::value_type& a,
						const nstring::value_type& b);

	//! Returns true iff \c a is lexicographically equal to \c b
	static bool		cmpEqual(const nstring::value_type& a,
						const nstring::value_type& b);
};

}
}
