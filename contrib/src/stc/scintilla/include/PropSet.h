// SciTE - Scintilla based Text Editor
// PropSet.h - a java style properties file module
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef PROPSET_H
#define PROPSET_H

bool EqualCaseInsensitive(const char *a, const char *b);

#if PLAT_WIN
#define strcasecmp  stricmp
#define strncasecmp strnicmp
#endif

#ifdef __WXMSW__
#define strcasecmp  stricmp
#define strncasecmp strnicmp
#endif

// Define another string class.
// While it would be 'better' to use std::string, that doubles the executable size.

inline char *StringDup(const char *s, int len=-1) {
	if (!s)
		return 0;
	if (len == -1)
		len = strlen(s);
	char *sNew = new char[len + 1];
	if (sNew) {
		strncpy(sNew, s, len);
		sNew[len] = '\0';
	}
	return sNew;
}

class SString {
	char *s;
	int ssize;
public:
	typedef const char* const_iterator;
	typedef int size_type;
	static size_type npos;
	const char* begin(void) const {
		return s;
	}
	const char* end(void) const {
		return &s[ssize];
	}
	size_type size(void) const {
		if (s)
			return ssize;
		else
			return 0;
	}
	SString &assign(const char* sother, int size_ = -1) {
		char *t = s;
		s = StringDup(sother,size_);
		ssize = (s) ? strlen(s) : 0;
		delete []t;
		return *this;
	}
	SString &assign(const SString& sother, int size_ = -1) {
		return assign(sother.s,size_);
	}
	SString &assign(const_iterator ibeg, const_iterator iend) {
		return assign(ibeg,iend - ibeg);
	}
	SString() {
		s = 0;
		ssize = 0;
	}
	SString(const SString &source) {
		s = StringDup(source.s);
		ssize = (s) ? strlen(s) : 0;
	}
	SString(const char *s_) {
		s = StringDup(s_);
		ssize = (s) ? strlen(s) : 0;
	}
	SString(int i) {
		char number[100];
		sprintf(number, "%0d", i);
		s = StringDup(number);
		ssize = (s) ? strlen(s) : 0;
	}
	~SString() {
		delete []s;
		s = 0;
		ssize = 0;
	}
	SString &operator=(const SString &source) {
		if (this != &source) {
			delete []s;
			s = StringDup(source.s);
			ssize = (s) ? strlen(s) : 0;
		}
		return *this;
	}
	bool operator==(const SString &other) const {
		if ((s == 0) && (other.s == 0))
			return true;
		if ((s == 0) || (other.s == 0))
			return false;
		return strcmp(s, other.s) == 0;
	}
	bool operator!=(const SString &other) const {
		return !operator==(other);
	}
	bool operator==(const char *sother) const {
		if ((s == 0) && (sother == 0))
			return true;
		if ((s == 0) || (sother == 0))
			return false;
		return strcmp(s, sother) == 0;
	}
	bool operator!=(const char *sother) const {
		return !operator==(sother);
	}
	const char *c_str() const {
		if (s)
			return s;
		else
			return "";
	}
	int length() const {
		if (s)
			return strlen(s);
		else
			return 0;
	}
	char operator[](int i) const {
		if (s)
			return s[i];
		else
			return '\0';
	}
	SString &operator +=(const char *sother) {
		return append(sother,-1);
	}
	SString &operator +=(const SString &sother) {
		return append(sother.s,sother.ssize);
	}
	SString &operator +=(char ch) {
		return append(&ch,1);
	}
	SString &append(const char* sother, int lenOther) {
		int len = length();
		if(lenOther < 0)
			lenOther = strlen(sother);
		char *sNew = new char[len + lenOther + 1];
		if (sNew) {
			if (s)
				memcpy(sNew, s, len);
			strncpy(&sNew[len], sother, lenOther);
			sNew[len + lenOther] = '\0';
			delete []s;
			s = sNew;
			ssize = (s) ? strlen(s) : 0;
		}
		return *this;
	}
	int value() const {
		if (s)
			return atoi(s);
		else
			return 0;
	}
	void substitute(char find, char replace) {
		char *t = s;
		while (t) {
			t = strchr(t, find);
			if (t)
				*t = replace;
		}
	}
	// I don't think this really belongs here -- Neil
	void correctPath() {
#ifdef unix
		substitute('\\', '/');
#else
		substitute('/', '\\');
#endif
	}
};

struct Property {
	unsigned int hash;
	char *key;
	char *val;
	Property *next;
	Property() : hash(0), key(0), val(0), next(0) {}
};

class PropSet {
private:
	enum { hashRoots=31 };
	Property *props[hashRoots];
public:
	PropSet *superPS;
	PropSet();
	~PropSet();
	void Set(const char *key, const char *val);
	void Set(char *keyval);
	SString Get(const char *key);
	SString GetExpanded(const char *key);
	SString Expand(const char *withvars);
	int GetInt(const char *key, int defaultValue=0);
	SString GetWild(const char *keybase, const char *filename);
	SString GetNewExpand(const char *keybase, const char *filename);
	void Clear();
	void ReadFromMemory(const char *data, int len, const char *directoryForImports=0);
	void Read(const char *filename, const char *directoryForImports);
};

class WordList {
public:
	// Each word contains at least one character - a empty word acts as sentinal at the end.
	char **words;
	char **wordsNoCase;
	char *list;
	int len;
	bool onlyLineEnds;	// Delimited by any white space or only line ends
	bool sorted;
	int starts[256];
	WordList(bool onlyLineEnds_ = false) :
		words(0), wordsNoCase(0), list(0), len(0), onlyLineEnds(onlyLineEnds_), sorted(false) {}
	~WordList() { Clear(); }
	operator bool() { return words ? true : false; }
	const char *operator[](int ind) { return words[ind]; }
	void Clear();
	void Set(const char *s);
	char *Allocate(int size);
	void SetFromAllocated();
	bool InList(const char *s);
	const char *GetNearestWord(const char *wordStart, int searchLen = -1, bool ignoreCase = false);
	char *GetNearestWords(const char *wordStart, int searchLen = -1, bool ignoreCase = false);
};

inline bool nonFuncChar(char ch) {
	return strchr("\t\n\r !\"#$%&'()*+,-./:;<=>?@[\\]^`{|}~", ch) != NULL;
}

#endif
