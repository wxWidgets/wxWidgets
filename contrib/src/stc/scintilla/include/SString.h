// SciTE - Scintilla based Text Editor
/** @file SString.h
 ** A simple string class.
 **/
// Copyright 1998-2001 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SSTRING_H
#define SSTRING_H

// These functions are implemented because each platform calls them something different.
int CompareCaseInsensitive(const char *a, const char *b);
int CompareNCaseInsensitive(const char *a, const char *b, int len);
bool EqualCaseInsensitive(const char *a, const char *b);

// Define another string class.
// While it would be 'better' to use std::string, that doubles the executable size.
// An SString may contain embedded nul characters.

/**
 * @brief A simple string class.
 *
 * Hold the length of the string for quick operations,
 * can have a buffer bigger than the string to avoid too many memory allocations and copies.
 * May have embedded zeroes as a result of @a substitute, but rely too heavily on C string
 * functions to allow reliable manipulations of these strings.
 **/
class SString {
public:
	/** Type of string lengths (sizes) and positions (indexes). */
	typedef unsigned int lenpos_t;
	/** Out of bounds value indicating that the string argument should be measured. */
	enum { measure_length=0xffffffffU};

private:
	char *s;				///< The C string
	lenpos_t sSize;			///< The size of the buffer, less 1: ie. the maximum size of the string
	lenpos_t sLen;			///< The size of the string in s
	lenpos_t sizeGrowth;	///< Minimum growth size when appending strings
	enum { sizeGrowthDefault = 64 };
	bool grow(lenpos_t lenNew) {
		while (sizeGrowth * 6 < lenNew) {
			sizeGrowth *= 2;
		}
		char *sNew = new char[lenNew + sizeGrowth + 1];
		if (sNew) {
			if (s) {
				memcpy(sNew, s, sLen);
				delete []s;
			}
			s = sNew;
			s[sLen] = '\0';
			sSize = lenNew + sizeGrowth;
		}
		return sNew != 0;
	}

	SString &assign(const char *sOther, lenpos_t sSize_=measure_length) {
		if (!sOther) {
			sSize_ = 0;
		} else if (sSize_ == measure_length) {
			sSize_ = strlen(sOther);
		}
		if (sSize > 0 && sSize_ <= sSize) {	// Does not allocate new buffer if the current is big enough
			if (s && sSize_) {
				strncpy(s, sOther, sSize_);
			}
			s[sSize_] = '\0';
			sLen = sSize_;
		} else {
			delete []s;
			s = StringAllocate(sOther, sSize_);
			if (s) {
				sSize = sSize_;	// Allow buffer bigger than real string, thus providing space to grow
				sLen = strlen(s);
			} else {
				sSize = sLen = 0;
			}
		}
		return *this;
	}

public:
	SString() : s(0), sSize(0), sLen(0), sizeGrowth(sizeGrowthDefault) {
	}
	SString(const SString &source) : sizeGrowth(sizeGrowthDefault) {
		s = StringAllocate(source.s);
		sSize = sLen = (s) ? strlen(s) : 0;
	}
	SString(const char *s_) : sizeGrowth(sizeGrowthDefault) {
		s = StringAllocate(s_);
		sSize = sLen = (s) ? strlen(s) : 0;
	}
	SString(const char *s_, lenpos_t first, lenpos_t last) : sizeGrowth(sizeGrowthDefault) {
		s = StringAllocate(s_ + first, last - first);
		sSize = sLen = (s) ? strlen(s) : 0;
	}
	SString(int i) : sizeGrowth(sizeGrowthDefault) {
		char number[32];
		sprintf(number, "%0d", i);
		s = StringAllocate(number);
		sSize = sLen = (s) ? strlen(s) : 0;
	}
	SString(double d, int precision) : sizeGrowth(sizeGrowthDefault) {
		char number[32];
		sprintf(number, "%.*f", precision, d);
		s = StringAllocate(number);
		sSize = sLen = (s) ? strlen(s) : 0;
	}
	~SString() {
		delete []s;
		s = 0;
		sSize = 0;
		sLen = 0;
	}
	void clear() {
		if (s) {
			*s = '\0';
		}
		sLen = 0;
	}
	/** Size of buffer. */
	lenpos_t size() const {
		if (s)
			return sSize;
		else
			return 0;
	}
	/** Size of string in buffer. */
	lenpos_t length() const {
		return sLen;
	}
	SString &operator=(const char *source) {
		return assign(source);
	}
	SString &operator=(const SString &source) {
		if (this != &source) {
			assign(source.c_str());
		}
		return *this;
	}
	bool operator==(const SString &sOther) const {
		if ((s == 0) && (sOther.s == 0))
			return true;
		if ((s == 0) || (sOther.s == 0))
			return false;
		return strcmp(s, sOther.s) == 0;
	}
	bool operator!=(const SString &sOther) const {
		return !operator==(sOther);
	}
	bool operator==(const char *sOther) const {
		if ((s == 0) && (sOther == 0))
			return true;
		if ((s == 0) || (sOther == 0))
			return false;
		return strcmp(s, sOther) == 0;
	}
	bool operator!=(const char *sOther) const {
		return !operator==(sOther);
	}
	bool contains(char ch) {
		if (s && *s)
			return strchr(s, ch) != 0;
		else
			return false;
	}
	void setsizegrowth(lenpos_t sizeGrowth_) {
		sizeGrowth = sizeGrowth_;
	}
	const char *c_str() const {
		if (s)
			return s;
		else
			return "";
	}
	/** Give ownership of buffer to caller which must use delete[] to free buffer. */
	char *detach() {
		char *sRet = s;
		s = 0;
		sSize = 0;
		sLen = 0;
		return sRet;
	}
	char operator[](lenpos_t i) const {
		if (s && i < sSize)	// Or < sLen? Depends on the use, both are OK
			return s[i];
		else
			return '\0';
	}
	SString &append(const char *sOther, lenpos_t sLenOther=measure_length, char sep = '\0') {
		if (!sOther) {
			return *this;
		}
		if (sLenOther == measure_length) {
			sLenOther = strlen(sOther);
		}
		int lenSep = 0;
		if (sLen && sep) {	// Only add a separator if not empty
			lenSep = 1;
		}
		lenpos_t lenNew = sLen + sLenOther + lenSep;
		// Conservative about growing the buffer: don't do it, unless really needed
		if ((lenNew + 1 < sSize) || (grow(lenNew))) {
			if (lenSep) {
				s[sLen] = sep;
				sLen++;
			}
			strncpy(&s[sLen], sOther, sLenOther);
			sLen += sLenOther;
			s[sLen] = '\0';
		}
		return *this;
	}
	SString &operator+=(const char *sOther) {
		return append(sOther, static_cast<lenpos_t>(measure_length));
	}
	SString &operator+=(const SString &sOther) {
		return append(sOther.s, sOther.sSize);
	}
	SString &operator+=(char ch) {
		return append(&ch, 1);
	}
	SString &appendwithseparator(const char *sOther, char sep) {
		return append(sOther, strlen(sOther), sep);
	}
	SString &insert(lenpos_t pos, const char *sOther, lenpos_t sLenOther=measure_length) {
		if (!sOther) {
			return *this;
		}
		if (sLenOther == measure_length) {
			sLenOther = strlen(sOther);
		}
		lenpos_t lenNew = sLen + sLenOther;
		// Conservative about growing the buffer: don't do it, unless really needed
		if ((lenNew + 1 < sSize) || grow(lenNew)) {
			lenpos_t moveChars = sLen - pos + 1;
			for (lenpos_t i = moveChars; i > 0; i--) {
				s[pos + sLenOther + i - 1] = s[pos + i - 1];
			}
			memcpy(s + pos, sOther, sLenOther);
			sLen = lenNew;
		}
		return *this;
	}
	/** Remove @a len characters from the @a pos position, included.
	 * Characters at pos + len and beyond replace characters at pos.
	 * If @a len is 0, or greater than the length of the string
	 * starting at @a pos, the string is just truncated at @a pos.
	 */
	void remove(lenpos_t pos, lenpos_t len) {
		if (len < 1 || pos + len >= sLen) {
			s[pos] = '\0';
			sLen = pos;
		} else {
			for (lenpos_t i = pos; i < sLen - len + 1; i++) {
				s[i] = s[i+len];
			}
			sLen -= len;
		}
	}
	/** Read an integral numeric value from the string. */
	int value() const {
		if (s)
			return atoi(s);
		else
			return 0;
	}
	int search(const char *sFind, lenpos_t start=0) {
		if (start < sLen) {
			const char *sFound = strstr(s + start, sFind);
			if (sFound) {
				return sFound - s;
			}
		}
		return -1;
	}
	bool contains(const char *sFind) {
		return search(sFind) >= 0;
	}
	int substitute(char chFind, char chReplace) {
		int c = 0;
		char *t = s;
		while (t) {
			t = strchr(t, chFind);
			if (t) {
				*t = chReplace;
				t++;
				c++;
			}
		}
		return c;
	}
	int substitute(const char *sFind, const char *sReplace) {
		int c = 0;
		lenpos_t lenFind = strlen(sFind);
		lenpos_t lenReplace = strlen(sReplace);
		int posFound = search(sFind);
		while (posFound >= 0) {
			remove(posFound, lenFind);
			insert(posFound, sReplace, lenReplace);
			posFound = search(sFind, posFound + lenReplace);
			c++;
		}
		return c;
	}
	int remove(const char *sFind) {
		return substitute(sFind, "");
	}
	/**
	 * Duplicate a C string.
	 * Allocate memory of the given size, or big enough to fit the string if length isn't given;
	 * then copy the given string in the allocated memory.
	 * @return the pointer to the new string
	 */
	static char *StringAllocate(
		const char *s,			///< The string to duplicate
		lenpos_t len=measure_length)	///< The length of memory to allocate. Optional.
	{
		if (s == 0) {
			return 0;
		}
		if (len == measure_length) {
			len = strlen(s);
		}
		char *sNew = new char[len + 1];
		if (sNew) {
			strncpy(sNew, s, len);
			sNew[len] = '\0';
		}
		return sNew;
	}
};

/**
 * Duplicate a C string.
 * Allocate memory of the given size, or big enough to fit the string if length isn't given;
 * then copy the given string in the allocated memory.
 * @return the pointer to the new string
 */
inline char *StringDup(
	const char *s,			///< The string to duplicate
	SString::lenpos_t len=SString::measure_length)	///< The length of memory to allocate. Optional.
{
	return SString::StringAllocate(s, len);
}

#endif
