// SciTE - Scintilla based Text Editor
// PropSet.cxx - a java style properties file module
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

// Maintain a dictionary of properties

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "Platform.h"

#include "PropSet.h"

bool EqualCaseInsensitive(const char *a, const char *b) {
#if PLAT_GTK
	return 0 == strcasecmp(a, b);
#elif PLAT_WIN
	return 0 == stricmp(a, b);
#elif PLAT_WX
	return 0 == wxStricmp(a, b);
#endif
}

// Get a line of input. If end of line escaped with '\\' then continue reading.
static bool GetFullLine(const char *&fpc, int &lenData, char *s, int len) {
	bool continuation = true;
	while ((len > 1) && lenData > 0) {
		char ch = *fpc;
		fpc++;
		lenData--;
		if ((ch == '\r') || (ch == '\n')) {
			if (!continuation) {
				if ((lenData > 0) && (ch == '\r') && ((*fpc) == '\n')) {
					// munch the second half of a crlf
					fpc++;
					lenData--;
				}
				*s++ = '\0';
				return true;
			}
		} else if ((ch == '\\') && (lenData > 0) && ((*fpc == '\r') || (*fpc == '\n'))) {
			continuation = true;
		} else {
			continuation = false;
			*s++ = ch;
			len--;
		}
	}
	return false;
}

PropSet::PropSet() {
	superPS = 0;
	size = 10;
	used = 0;
	vals = new char * [size];
}

PropSet::~PropSet() {
	superPS = 0;
	Clear();
	delete []vals;
}

void PropSet::EnsureCanAddEntry() {
	if (used >= size - 2) {
		int newsize = size + 10;
		char **newvals = new char * [newsize];

		for (int i = 0; i < used; i++) {
			newvals[i] = vals[i];
		}
		delete []vals;
		vals = newvals;
		size = newsize;
	}
}

void PropSet::Set(const char *key, const char *val) {
	EnsureCanAddEntry();
	for (int i = 0; i < used; i += 2) {
		if (EqualCaseInsensitive(vals[i], key)) {
			// Replace current value
			delete [](vals[i + 1]);
			vals[i + 1] = StringDup(val);
			return;
		}
	}
	// Not found
	vals[used++] = StringDup(key);
	vals[used++] = StringDup(val);
}

void PropSet::Set(char *keyval) {
	char *eqat = strchr(keyval, '=');
	if (eqat) {
		*eqat = '\0';
		Set(keyval, eqat + 1);
		*eqat = '=';
	}
}

SString PropSet::Get(const char *key) {
	for (int i = 0; i < used; i += 2) {
		if (EqualCaseInsensitive(vals[i], key)) {
			return vals[i + 1];
		}
	}
	if (superPS) {
		// Failed here, so try in base property set
		return superPS->Get(key);
	} else {
		return "";
	}
}

int PropSet::GetInt(const char *key, int defaultValue) {
	SString val = Get(key);
	if (val.length())
		return Get(key).value();
	else
		return defaultValue;
}

bool isprefix(const char *target, const char *prefix) {
	while (*target && *prefix) {
		if (toupper(*target) != toupper(*prefix))
			return false;
		target++;
		prefix++;
	}
	if (*prefix)
		return false;
	else
		return true;
}

bool issuffix(const char *target, const char *suffix) {
	int lentarget = strlen(target);
	int lensuffix = strlen(suffix);
	if (lensuffix > lentarget)
		return false;
	for (int i = lensuffix - 1; i >= 0; i--) {
		if (toupper(target[i + lentarget - lensuffix]) != toupper(suffix[i]))
			return false;
	}
	return true;
}

SString PropSet::GetWild(const char *keybase, const char *filename) {
	for (int i = 0; i < used; i += 2) {
		if (isprefix(vals[i], keybase)) {
			char *orgkeyfile = vals[i] + strlen(keybase);
			char *keyfile = NULL;

			if (strstr(orgkeyfile, "$(") == orgkeyfile) {
				char *cpendvar = strchr(orgkeyfile, ')');
				if (cpendvar) {
					int lenvar = cpendvar - orgkeyfile - 2; 	// Subtract the $()
					char *var = static_cast<char *>(malloc(lenvar + 1));
					strncpy(var, orgkeyfile + 2, lenvar);
					var[lenvar] = '\0';
					SString s = Get(var);
					free(var);
					keyfile = strdup(s.c_str());
				}
			}
			char *keyptr = keyfile;

			if (keyfile == NULL)
				keyfile = orgkeyfile;

			for (; ; ) {
				char *del = strchr(keyfile, ';');
				if (del == NULL)
					del = keyfile + strlen(keyfile);
				char delchr = *del;
				*del = '\0';
				if (*keyfile == '*') {
					if (issuffix(filename, keyfile + 1)) {
						*del = delchr;
						free(keyptr);
						return vals[i + 1];
					}
				} else if (EqualCaseInsensitive(keyfile, filename)) {
					*del = delchr;
					free(keyptr);
					return vals[i + 1];
				}
				if (delchr == '\0')
					break;
				*del = delchr;
				keyfile = del + 1;
			}
			free(keyptr);

			if (EqualCaseInsensitive(vals[i], keybase)) {
				return vals[i + 1];
			}
		}
	}
	if (superPS) {
		// Failed here, so try in base property set
		return superPS->GetWild(keybase, filename);
	} else {
		return "";
	}
}

SString PropSet::GetNewExpand(const char *keybase, const char *filename) {
	char *base = StringDup(GetWild(keybase, filename).c_str());
	char *cpvar = strstr(base, "$(");
	while (cpvar) {
		char *cpendvar = strchr(cpvar, ')');
		if (cpendvar) {
			int lenvar = cpendvar - cpvar - 2;  	// Subtract the $()
			char *var = new char[lenvar + 1];
			strncpy(var, cpvar + 2, lenvar);
			var[lenvar] = '\0';
			SString val = GetWild(var, filename);
			int newlenbase = strlen(base) + val.length() - lenvar;
			char *newbase = new char[newlenbase];
			strncpy(newbase, base, cpvar - base);
			strcpy(newbase + (cpvar - base), val.c_str());
			strcpy(newbase + (cpvar - base) + val.length(), cpendvar + 1);
			delete []var;
			delete []base;
			base = newbase;
		}
		cpvar = strstr(base, "$(");
	}
	SString sret = base;
	delete []base;
	return sret;
}

void PropSet::Clear() {
	for (int i = 0; i < used; i++) {
		delete [](vals[i]);
		vals[i] = 0;
	}
	used = 0;
}

void PropSet::ReadFromMemory(const char *data, int len) {
	if (len > 0) {
		const char *pd = data;
		char linebuf[60000];
		while (GetFullLine(pd, len, linebuf, sizeof(linebuf))) {
			if (isalpha(linebuf[0]))
				Set(linebuf);
		}
	}
}

void PropSet::Read(const char *filename) {
	//printf("Opening properties <%s>\n", filename);
	Clear();
	char propsData[60000];
	FILE *rcfile = fopen(filename, "rb");
	if (rcfile) {
		int lenFile = fread(propsData, 1, sizeof(propsData), rcfile);
		fclose(rcfile);
		ReadFromMemory(propsData, lenFile);
	} else {
		//printf("Could not open <%s>\n", filename);
	}
}

static bool iswordsep(char ch, bool onlyLineEnds) {
	if (!isspace(ch))
		return false;
	if (!onlyLineEnds)
		return true;
	return ch == '\r' || ch == '\n';
}

// Creates an array that points into each word in the string and puts \0 terminators
// after each word.
static char **ArrayFromWordList(char *wordlist, bool onlyLineEnds = false) {
	char prev = '\n';
	int words = 0;
	for (int j = 0; wordlist[j]; j++) {
		if (!iswordsep(wordlist[j], onlyLineEnds) && iswordsep(prev, onlyLineEnds))
			words++;
		prev = wordlist[j];
	}
	char **keywords = new char * [words + 1];
	if (keywords) {
		words = 0;
		prev = '\0';
		int len = strlen(wordlist);
		for (int k = 0; k < len; k++) {
			if (!iswordsep(wordlist[k], onlyLineEnds)) {
				if (!prev) {
					keywords[words] = &wordlist[k];
					words++;
				}
			} else {
				wordlist[k] = '\0';
			}
			prev = wordlist[k];
		}
		keywords[words] = &wordlist[len];
	}
	return keywords;
}

void WordList::Clear() {
	if (words) {
		delete []words;
		delete []list;
	}
	words = 0;
	list = 0;
	len = 0;
}

void WordList::Set(const char *s) {
	len = 0;
	list = StringDup(s);
	words = ArrayFromWordList(list, onlyLineEnds);
}

char *WordList::Allocate(int size) {
	list = new char[size + 1];
	list[size] = '\0';
	return list;
}

void WordList::SetFromAllocated() {
	len = 0;
	words = ArrayFromWordList(list, onlyLineEnds);
}

// Shell sort based upon public domain C implementation by Raymond Gardner 1991
// Used here because of problems with mingw qsort.
static void SortWordList(char **words, unsigned int len) {
	unsigned int gap = len / 2;

	while (gap > 0) {
		unsigned int i = gap;
		while (i < len) {
			unsigned int j = i;
			char **a = words + j;
			do {
				j -= gap;
				char **b = a;
				a -= gap;
				if (strcmp(*a, *b) > 0) {
					char *tmp = *a;
					*a = *b;
					*b = tmp;
				} else {
					break;
				}
			} while (j >= gap);
			i++;
		}
		gap = gap / 2;
	}
}

bool WordList::InList(const char *s) {
	if (0 == words)
		return false;
	if (len == 0) {
		for (int i = 0; words[i][0]; i++)
			len++;
		SortWordList(words, len);
		for (int k = 0; k < (sizeof(starts) / sizeof(starts[0])); k++)
			starts[k] = -1;
		for (int l = len - 1; l >= 0; l--) {
			unsigned char indexChar = words[l][0];
			starts[indexChar] = l;
		}
	}
	unsigned char firstChar = s[0];
	int j = starts[firstChar];
	if (j >= 0) {
		while (words[j][0] == firstChar) {
			if (s[1] == words[j][1]) {
				const char *a = words[j] + 1;
				const char *b = s + 1;
				while (*a && *a == *b) {
					a++;
					b++;
				}
				if (!*a && !*b)
					return true;
			}
			j++;
		}
	}
	return false;
}
