// SciTE - Scintilla based Text Editor
// PropSet.h - a java style properties file module
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef PROPSET_H
#define PROPSET_H

bool EqualCaseInsensitive(const char *a, const char *b);

// Define another string class.
// While it would be 'better' to use std::string, that doubles the executable size.

inline char *StringDup(const char *s) {
	if (!s)
		return 0;
	char *sNew = new char[strlen(s) + 1];
	if (sNew)
		strcpy(sNew, s);
	return sNew;
}

class SString {
	char *s;
public:
	SString() {
		s = 0;
	}
	SString(const SString &source) {
		s = StringDup(source.s);
	}
	SString(const char *s_) {
		s = StringDup(s_);
	}
	SString(int i) {
		char number[100];
		sprintf(number, "%0d", i);
		//itoa(i, number, 10);
		s = StringDup(number);
	}
	~SString() {
		delete []s;
		s = 0;
	}
	SString &operator=(const SString &source) {
		if (this != &source) {
			delete []s;
			s = StringDup(source.s);
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
	bool operator==(const char *sother) const {
		if ((s == 0) && (sother == 0))
			return true;
		if ((s == 0) || (sother == 0))
			return false;
		return strcmp(s, sother) == 0;
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
		int len = length();
		int lenOther = strlen(sother);
		char *sNew = new char[len + lenOther + 1];
		if (sNew) {
			if (s)
				memcpy(sNew, s, len);
			memcpy(sNew + len, sother, lenOther);
			sNew[len + lenOther] = '\0';
			delete []s;
			s = sNew;
		}
		return *this;
	}
	int value() const {
		if (s)
			return atoi(s);
		else 
			return 0;
	}
};

class PropSet {
private:
	char **vals;
	int size;
	int used;
public:
	PropSet *superPS;
	PropSet();
	~PropSet();
	void EnsureCanAddEntry();
	void Set(const char *key, const char *val);
	void Set(char *keyval);
	SString Get(const char *key);
	int GetInt(const char *key, int defaultValue=0);
	SString GetWild(const char *keybase, const char *filename);
	SString GetNewExpand(const char *keybase, const char *filename);
	void Clear();
	void ReadFromMemory(const char *data, int len);
	void Read(const char *filename);
};

// This is a fixed length list of strings suitable for display  in combo boxes
// as a memory of user entries
template<int sz>
class EntryMemory {
	SString entries[sz];
public:
	void Insert(SString s) {
		for (int i=0;i<sz;i++) {
			if (entries[i] == s) {
				for (int j=i;j>0;j--) {
					entries[j] = entries[j-1];
				}
				entries[0] = s;
				return;
			}
		}
		for (int k=sz-1;k>0;k--) {
			entries[k] = entries[k-1];
		}
		entries[0] = s;
	}
	int Length() const {
		int len = 0;
		for (int i=0;i<sz;i++)
			if (entries[i].length())
				len++;
		return len;
	}
	SString At(int n) const {
		return entries[n];
	}
};

class WordList {
public:
	// Each word contains at least one character - a empty word acts as sentinal at the end.
	char **words;
	char *list;
	int len;
	bool onlyLineEnds;	// Delimited by any white space or only line ends
	int starts[256];
	WordList(bool onlyLineEnds_ = false) : 
		words(0), list(0), len(0), onlyLineEnds(onlyLineEnds_) {}
	~WordList() { Clear(); }
	operator bool() { return list ? true : false; }
	const char *operator[](int ind) { return words[ind]; }
	void Clear();
	void Set(const char *s);
	char *Allocate(int size);
	void SetFromAllocated();
	bool InList(const char *s);
};

#endif
