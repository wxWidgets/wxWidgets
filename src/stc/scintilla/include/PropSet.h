// Scintilla source code edit control
/** @file PropSet.h
 ** A Java style properties file module.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef PROPSET_H
#define PROPSET_H
#include "SString.h"

bool EqualCaseInsensitive(const char *a, const char *b);

bool isprefix(const char *target, const char *prefix);

struct Property {
	unsigned int hash;
	char *key;
	char *val;
	Property *next;
	Property() : hash(0), key(0), val(0), next(0) {}
};

/**
 */
class PropSet {
protected:
	enum { hashRoots=31 };
	Property *props[hashRoots];
	Property *enumnext;
	int enumhash;
	static unsigned int HashString(const char *s, size_t len) {
		unsigned int ret = 0;
		while (len--) {
			ret <<= 4;
			ret ^= *s;
			s++;
		}
		return ret;
	}
	static bool IncludesVar(const char *value, const char *key);
public:
	PropSet *superPS;
	PropSet();
	~PropSet();
	void Set(const char *key, const char *val, int lenKey=-1, int lenVal=-1);
	void Set(const char *keyVal);
	void SetMultiple(const char *s);
	SString Get(const char *key);
	SString GetExpanded(const char *key);
	SString Expand(const char *withVars, int maxExpands=100);
	int GetInt(const char *key, int defaultValue=0);
	SString GetWild(const char *keybase, const char *filename);
	SString GetNewExpand(const char *keybase, const char *filename="");
	void Clear();
	char *ToString();	// Caller must delete[] the return value
	bool GetFirst(char **key, char **val);
	bool GetNext(char **key, char **val);
};

/**
 */
class WordList {
public:
	// Each word contains at least one character - a empty word acts as sentinel at the end.
	char **words;
	char **wordsNoCase;
	char *list;
	int len;
	bool onlyLineEnds;	///< Delimited by any white space or only line ends
	bool sorted;
	int starts[256];
	WordList(bool onlyLineEnds_ = false) :
		words(0), wordsNoCase(0), list(0), len(0), onlyLineEnds(onlyLineEnds_), sorted(false) {}
	~WordList() { Clear(); }
	operator bool() { return len ? true : false; }
	char *operator[](int ind) { return words[ind]; }
	void Clear();
	void Set(const char *s);
	char *Allocate(int size);
	void SetFromAllocated();
	bool InList(const char *s);
	const char *GetNearestWord(const char *wordStart, int searchLen = -1,
		bool ignoreCase = false, SString wordCharacters="");
	char *GetNearestWords(const char *wordStart, int searchLen=-1,
		bool ignoreCase=false, char otherSeparator='\0');
};

inline bool IsAlphabetic(unsigned int ch) {
	return ((ch >= 'A') && (ch <= 'Z')) || ((ch >= 'a') && (ch <= 'z'));
}

#endif
