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

#include "arch/Arch.h"
#include "base/String.h"
#include "common/common.h"
#include "common/stdvector.h"

//#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <stdio.h>
#include <cstdarg>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <locale>

namespace synergy {
namespace string {

nstring
format(const nchar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	nstring result = vformat(fmt, args);
	va_end(args);
	return result;
}

nstring
vformat(const nchar* fmt, va_list args)
{
	// find highest indexed substitution and the locations of substitutions
	std::vector<size_t> pos;
	std::vector<size_t> width;
	std::vector<int> index;
	int maxIndex = 0;
	for (const nchar* scan = fmt; *scan; ++scan) {
		if (*scan =='%')) {
			++scan;
			if (*scan == 0) {
				break;
			}
			else if (*scan =='%')) {
				// literal
				index.push_back(0);
				pos.push_back(static_cast<int>(scan - 1 - fmt));
				width.push_back(2);
			}
			else if (*scan == '{') {
				// get argument index
				nchar* end;
				int i = static_cast<int>(strtol(scan + 1, &end, 10));
				if (*end !='}')) {
					// invalid index -- ignore
					scan = end - 1;
				}
				else {
					index.push_back(i);
					pos.push_back(static_cast<int>(scan - 1 - fmt));
					width.push_back(static_cast<int>(end - scan + 2));
					if (i > maxIndex) {
						maxIndex = i;
					}
					scan = end;
				}
			}
			else {
				// improper escape -- ignore
			}
		}
	}

	// get args
	std::vector<const nchar*> value;
	std::vector<size_t> length;
	value.push_back(_N("%"));
	length.push_back(1);
	for (int i = 0; i < maxIndex; ++i) {
		const nchar* arg = va_arg(args, const nchar*);
		size_t len = strlen(arg);
		value.push_back(arg);
		length.push_back(len);
	}

	// compute final length
	size_t resultLength = nstrlen(fmt);
	const int n = static_cast<int>(pos.size());
	for (int i = 0; i < n; ++i) {
		resultLength -= width[i];
		resultLength += length[index[i]];
	}

	// substitute
	nstring result;
	result.reserve(resultLength);
	size_t src = 0;
	for (int i = 0; i < n; ++i) {
		result.append(fmt + src, pos[i] - src);
		result.append(value[index[i]]);
		src = pos[i] + width[i];
	}
	result.append(fmt + src);

	return result;
}

nstring
sprintf(const nchar* fmt, ...)
{
	nchar tmp[1024];
	nchar* buffer = tmp;
	int len = (int)(sizeof(tmp) / sizeof(tmp[0]));
	nstring result;
	while (buffer != NULL) {
		// try printing into the buffer
		va_list args;
		va_start(args, fmt);
		int n = ARCH->vsnprintf(buffer, len, fmt, args);
		va_end(args);

		// if the buffer wasn't big enough then make it bigger and try again
		if (n < 0 || n > len) {
			if (buffer != tmp) {
				delete[] buffer;
			}
			len *= 2;
			buffer = new nchar[len];
		}

		// if it was big enough then save the string and don't try again
		else {
			result = buffer;
			if (buffer != tmp) {
				delete[] buffer;
			}
			buffer = NULL;
		}
	}

	return result;
}

void
findReplaceAll(
	nstring& subject,
	const nstring& find,
	const nstring& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(find, pos)) != nstring::npos) {
		subject.replace(pos, find.length(), replace);
		pos += replace.length();
	}
}

nstring
removeFileExt(nstring filename)
{
	size_t dot = filename.find_last_of(_N('.'));

	if (dot == nstring::npos) {
		return filename;
	}

	return filename.substr(0, dot);
}

#ifdef SYSAPI_WIN32
void
toHex(std::string& subject, int width, const char fill)
{
	std::stringstream ss;
	ss << std::hex;
	for (unsigned int i = 0; i < subject.length(); i++) {
		ss << std::setw(width) << std::setfill(fill) << (int)(unsigned nchar)subject[i];
	}

	subject = ss.str();
}
#endif

void
toHex(nstring& subject, int width, const nchar fill)
{
	nstringstream ss;
	ss << std::hex;
	for (unsigned int i = 0; i < subject.length(); i++) {
		ss << std::setw(width) << std::setfill(fill) << (int)(unsigned nchar)subject[i];
	}

	subject = ss.str();
}

#ifdef SYSAPI_WIN32
void
uppercase(std::string& subject)
{
	std::transform(subject.begin(), subject.end(), subject.begin(), toupper);
}
#endif

void
uppercase(nstring& subject)
{
	std::transform(subject.begin(), subject.end(), subject.begin(), tonupper);
}

void
removeChar(nstring& subject, const nchar c)
{
	subject.erase(std::remove(subject.begin(), subject.end(), c), subject.end());
}

nstring
sizeTypeToString(size_t n)
{
	nstringstream ss;
	ss << n;
	return ss.str();
}

size_t
stringToSizeType(nstring string)
{
	nistringstream iss(string);
	size_t value;
	iss >> value;
	return value;
}

std::vector<nstring>
splitString(nstring string, const nchar c)
{
	std::vector<nstring> results;

	size_t head = 0;
	size_t separator = string.find(c);
	while (separator != nstring::npos) {
		if (head != separator) {
			results.push_back(string.substr(head, separator - head));
		}
		head = separator + 1;
		separator = string.find(c, head);
	}

	if (head < string.size()) {
		results.push_back(string.substr(head, string.size() - head));
	}

	return results;
}

nstring toNative(const char* s)
{
#ifdef SYSAPI_WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(s);
#else
	return std::string(s);
#endif
}

nstring toNative(std::string s)
{
#ifdef SYSAPI_WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(s);
#else
	return s;
#endif
}

std::string nativeToUtf8(const nchar* s)
{
#ifdef SYSAPI_WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(s);
#else
	return s;
#endif
}

std::string nativeToUtf8(nstring s)
{
#ifdef SYSAPI_WIN32
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(s);
#else
	return s;
#endif
}

//
// CaselessCmp
//

bool
CaselessCmp::cmpEqual(
	const nstring::value_type& a,
	const nstring::value_type& b)
{
	// should use std::tolower but not in all versions of libstdc++ have it
	return tonlower(a) == tonlower(b);
}

bool
CaselessCmp::cmpLess(
	const nstring::value_type& a,
	const nstring::value_type& b)
{
	// should use std::tolower but not in all versions of libstdc++ have it
	return tonlower(a) < tonlower(b);
}

bool
CaselessCmp::less(const nstring& a, const nstring& b)
{
	return std::lexicographical_compare(
		a.begin(), a.end(),
		b.begin(), b.end(),
		&synergy::string::CaselessCmp::cmpLess);
}

bool
CaselessCmp::equal(const nstring& a, const nstring& b)
{
	return !(less(a, b) || less(b, a));
}

bool
CaselessCmp::operator()(const nstring& a, const nstring& b) const
{
	return less(a, b);
}

}
}
