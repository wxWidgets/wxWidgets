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

SString::size_type SString::npos = -1;

inline unsigned int HashString(const char *s) {
    unsigned int ret = 0;
    while (*s) {
        ret <<= 4;
        ret ^= *s;
        s++;
    }
    return ret;
}

// Get a line of input. If end of line escaped with '\\' then continue reading.
static bool GetFullLine(const char *&fpc, int &lenData, char *s, int len) {
	bool continuation = true;
	s[0] = '\0';
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
				*s = '\0';
				return true;
			}
		} else if ((ch == '\\') && (lenData > 0) && ((*fpc == '\r') || (*fpc == '\n'))) {
			continuation = true;
		} else {
			continuation = false;
			*s++ = ch;
			*s = '\0';
			len--;
		}
	}
	return false;
}

PropSet::PropSet() {
	superPS = 0;
    for (int root=0; root < hashRoots; root++)
        props[root] = 0;
}

PropSet::~PropSet() {
	superPS = 0;
	Clear();
}

void PropSet::Set(const char *key, const char *val) {
    unsigned int hash = HashString(key);
	for (Property *p=props[hash % hashRoots]; p; p=p->next) {
		if ((hash == p->hash) && (0 == strcmp(p->key, key))) {
			// Replace current value
			delete [](p->val);
			p->val = StringDup(val);
			return;
		}
	}
	// Not found
    Property *pNew = new Property;
    if (pNew) {
        pNew->hash = HashString(key);
	    pNew->key = StringDup(key);
	    pNew->val = StringDup(val);
	    pNew->next = props[hash % hashRoots];
        props[hash % hashRoots] = pNew;
    }
}

void PropSet::Set(char *keyval) {
    while (isspace(*keyval))
        keyval++;
	char *eqat = strchr(keyval, '=');
	if (eqat) {
		*eqat = '\0';
		Set(keyval, eqat + 1);
		*eqat = '=';
	}
}

SString PropSet::Get(const char *key) {
    unsigned int hash = HashString(key);
	for (Property *p=props[hash % hashRoots]; p; p=p->next) {
		if ((hash == p->hash) && (0 == strcmp(p->key, key))) {
            return p->val;
        }
    }
	if (superPS) {
		// Failed here, so try in base property set
		return superPS->Get(key);
	} else {
		return "";
	}
}

SString PropSet::GetExpanded(const char *key) {
    SString val = Get(key);
    return Expand(val.c_str());
}

SString PropSet::Expand(const char *withvars) {
	char *base = StringDup(withvars);
	char *cpvar = strstr(base, "$(");
	while (cpvar) {
		char *cpendvar = strchr(cpvar, ')');
		if (cpendvar) {
			int lenvar = cpendvar - cpvar - 2;  	// Subtract the $()
			char *var = StringDup(cpvar+2, lenvar);
			SString val = GetExpanded(var);
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

int PropSet::GetInt(const char *key, int defaultValue) {
	SString val = Get(key);
	if (val.length())
		return val.value();
	else
		return defaultValue;
}

inline bool isprefix(const char *target, const char *prefix) {
	while (*target && *prefix) {
		if (*target != *prefix)
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
		if (target[i + lentarget - lensuffix] != suffix[i])
			return false;
	}
	return true;
}

SString PropSet::GetWild(const char *keybase, const char *filename) {
    for (int root=0; root < hashRoots; root++) {
	    for (Property *p=props[root]; p; p=p->next) {
		    if (isprefix(p->key, keybase)) {
			    char *orgkeyfile = p->key + strlen(keybase);
			    char *keyfile = NULL;

			    if (strstr(orgkeyfile, "$(") == orgkeyfile) {
				    char *cpendvar = strchr(orgkeyfile, ')');
				    if (cpendvar) {
					    *cpendvar = '\0';
					    SString s = Get(orgkeyfile + 2);
					    *cpendvar= ')';
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
						    return p->val;
					    }
				    } else if (0 == strcmp(keyfile, filename)) {
					    *del = delchr;
					    free(keyptr);
					    return p->val;
				    }
				    if (delchr == '\0')
					    break;
				    *del = delchr;
				    keyfile = del + 1;
			    }
			    free(keyptr);

			    if (0 == strcmp(p->key, keybase)) {
				    return p->val;
			    }
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
			char *var = StringDup(cpvar+2, lenvar);
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
    for (int root=0; root < hashRoots; root++) {
        Property *p=props[root];
	    while (p) {
            Property *pNext=p->next;
		    p->hash = 0;
		    delete p->key;
		    p->key = 0;
		    delete p->val;
		    p->val = 0;
            delete p;
            p = pNext;
        }
        props[root] = 0;
    }
}

void PropSet::ReadFromMemory(const char *data, int len, const char *directoryForImports) {
	const char *pd = data;
	char linebuf[60000];
	bool ifIsTrue = true;
	while (len > 0) {
		GetFullLine(pd, len, linebuf, sizeof(linebuf));
		if (isalpha(linebuf[0]))    // If clause ends with first non-indented line
			ifIsTrue = true;
		if (isprefix(linebuf, "if ")) {
			const char *expr = linebuf + strlen("if") + 1;
			ifIsTrue = GetInt(expr);
		} else if (isprefix(linebuf, "import ") && directoryForImports) {
			char importPath[1024];
			strcpy(importPath, directoryForImports);
			strcat(importPath, linebuf + strlen("import") + 1);
			strcat(importPath, ".properties");
            		Read(importPath,directoryForImports);
		} else if (isalpha(linebuf[0])) {
			Set(linebuf);
		} else if (isspace(linebuf[0]) && ifIsTrue) {
			Set(linebuf);
		}
	}
}

void PropSet::Read(const char *filename, const char *directoryForImports) {
	char propsData[60000];
	FILE *rcfile = fopen(filename, "rb");
	if (rcfile) {
		int lenFile = fread(propsData, 1, sizeof(propsData), rcfile);
		fclose(rcfile);
		ReadFromMemory(propsData, lenFile, directoryForImports);
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
static char **ArrayFromWordList(char *wordlist, int *len, bool onlyLineEnds = false) {
#if 1
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
		int slen = strlen(wordlist);
		for (int k = 0; k < slen; k++) {
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
		keywords[words] = &wordlist[slen];
		*len = words;
	} else {
		*len = 0;
	}
#else
	int words = 0; // length of the returned buffer of pointers
	#undef APICHUNK // how many pointers will be pre-allocated (to avoid buffer reallocation on each new pointer)
	#define APICHUNK 256
	int size = APICHUNK; // real size of the returned buffer of pointers
	char **keywords; // buffer for the pointers returned
	int slen = strlen(wordlist); //length of the buffer with api file
	keywords = (char**) malloc((size + 1) * sizeof (*keywords));
	words = 0;
	for (int k = 0;;) {
		while (iswordsep(wordlist[k], onlyLineEnds))
			wordlist[k++] = '\0';
		if (k >= slen)
			break;
		if (words >= size) {
			do
				size += APICHUNK;
			while (size <= words);
			keywords = (char**) realloc(keywords, (size + 1) * sizeof (*keywords));
		}
		keywords[words++] = wordlist + k;
		do
			if (k < slen)
				k++;
			else
				goto out;
		while (!iswordsep(wordlist[k], onlyLineEnds));
	}
out:
	keywords[words] = wordlist + slen;
	*len = words;
#endif
	return keywords;
}

void WordList::Clear() {
	if (words) {
		delete []list;
#if 1
        delete []words;
#else
		free(words);
#endif
		free(wordsNoCase);
	}
	words = 0;
	wordsNoCase = 0;
	list = 0;
	len = 0;
	sorted = false;
}

void WordList::Set(const char *s) {
	list = StringDup(s);
	sorted = false;
	words = ArrayFromWordList(list, &len, onlyLineEnds);
	wordsNoCase = (char**) malloc ((len + 1) * sizeof (*wordsNoCase));
	memcpy(wordsNoCase, words, (len + 1) * sizeof (*words));
}

char *WordList::Allocate(int size) {
	list = new char[size + 1];
	list[size] = '\0';
	return list;
}

void WordList::SetFromAllocated() {
	sorted = false;
	words = ArrayFromWordList(list, &len, onlyLineEnds);
	wordsNoCase = (char**) malloc ((len + 1) * sizeof (*wordsNoCase));
	memcpy(wordsNoCase, words, (len + 1) * sizeof (*words));
}

int cmpString(const void *a1, const void *a2) {
    // Can't work out the correct incantation to use modern casts here
    return strcmp(*(char**)(a1), *(char**)(a2));
}

int cmpStringNoCase(const void *a1, const void *a2) {
    // Can't work out the correct incantation to use modern casts here
    return strcasecmp(*(char**)(a1), *(char**)(a2));
}

static void SortWordList(char **words, char **wordsNoCase, unsigned int len) {
	qsort(reinterpret_cast<void*>(words), len, sizeof(*words),
		cmpString);
	qsort(reinterpret_cast<void*>(wordsNoCase), len, sizeof(*wordsNoCase),
		cmpStringNoCase);
}
 
bool WordList::InList(const char *s) {
	if (0 == words)
		return false;
	if (!sorted) {
		sorted = true;
		SortWordList(words, wordsNoCase, len);
		for (unsigned int k = 0; k < (sizeof(starts) / sizeof(starts[0])); k++)
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

/**
 * Returns an element (complete) of the wordlist array which has the beginning
 * the same as the passed string. The length of the word to compare is passed
 * too. Letter case can be ignored or preserved (default).
 */
const char *WordList::GetNearestWord(const char *wordStart, int searchLen /*= -1*/, bool ignoreCase /*= false*/) {
	int start = 0; // lower bound of the api array block to search
	int end = len - 1; // upper bound of the api array block to search
	int pivot; // index of api array element just being compared
	int cond; // comparison result (in the sense of strcmp() result)
	const char *word; // api array element just being compared

	if (0 == words)
		return NULL;
	if (!sorted) {
		sorted = true;
		SortWordList(words, wordsNoCase, len);
	}
	if (ignoreCase)
		while (start <= end) { // binary searching loop
			pivot = (start + end) >> 1;
			word = wordsNoCase[pivot];
			cond = strncasecmp(wordStart, word, searchLen);
			if (!cond && nonFuncChar(word[searchLen])) // maybe there should be a "non-word character" test here?
				return word; // result must not be freed with free()
			else if (cond < 0)
				end = pivot - 1;
			else if (cond > 0)
				start = pivot + 1;
		}
	else // preserve the letter case
		while (start <= end) { // binary searching loop
			pivot = (start + end) >> 1;
			word = words[pivot];
			cond = strncmp(wordStart, word, searchLen);
			if (!cond && nonFuncChar(word[searchLen])) // maybe there should be a "non-word character" test here?
				return word; // result must not be freed with free()
			else if (cond >= 0)
				start = pivot + 1;
			else if (cond < 0)
				end = pivot - 1;
		}
	return NULL;
}
	
/**
 * Returns elements (first words of them) of the wordlist array which have
 * the beginning the same as the passed string. The length of the word to
 * compare is passed too. Letter case can be ignored or preserved (default).
 * If there are more words meeting the condition they are returned all of
 * them in the ascending order separated with spaces.
 *
 * NOTE: returned buffer has to be freed with a free() call.
 */
char *WordList::GetNearestWords(const char *wordStart, int searchLen /*= -1*/, bool ignoreCase /*= false*/) {
	int wordlen; // length of the word part (before the '(' brace) of the api array element
	int length = 0; // length of the returned buffer of words (string)
	int newlength; // length of the new buffer before the reallocating itself
	#undef WORDCHUNK // how many characters will be pre-allocated (to avoid buffer reallocation on each new word)
	#define WORDCHUNK 100
	int size = WORDCHUNK; // real size of the returned buffer of words
	char *buffer; // buffer for the words returned
	int start = 0; // lower bound of the api array block to search
	int end = len - 1; // upper bound of the api array block to search
	int pivot; // index of api array element just being compared
	int cond; // comparison result (in the sense of strcmp() result)
	int oldpivot; // pivot storage to be able to browse the api array upwards and then downwards
	const char *word; // api array element just being compared
	const char *brace; // position of the opening brace in the api array element just being compared

	if (0 == words)
		return NULL;
	if (!sorted) {
		sorted = true;
		SortWordList(words, wordsNoCase, len);
	}
	buffer = (char*) malloc(size);
	*buffer = '\0';
	if (ignoreCase)
		while (start <= end) { // binary searching loop
			pivot = (start + end) >> 1;
			word = wordsNoCase[pivot];
			cond = strncasecmp(wordStart, word, searchLen);
			if (!cond) {
				oldpivot = pivot;
				do { // browse sequentially the rest after the hit
					brace = strchr(word, '(');
					if (brace)
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					else {
						brace = word + strlen(word);
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					}
					wordlen = brace - word + 1;
					newlength = length + wordlen; // stretch the buffer
					if (length)
						newlength++;
					if (newlength >= size) {
						do
							size += WORDCHUNK;
						while (size <= newlength);
						buffer = (char*) realloc(buffer, size);
					}
					if (length) // append a new entry
						buffer[length++] = ' ';
					memcpy(buffer + length, word, wordlen);
					length = newlength;
					buffer[length] = '\0';
					if (++pivot > end)
						break;
					word = wordsNoCase[pivot];
				} while (!strncasecmp(wordStart, word, searchLen));

				pivot = oldpivot;
				for (;;) { // browse sequentially the rest before the hit
					if (--pivot < start)
						break;
					word = wordsNoCase[pivot];
					if (strncasecmp(wordStart, word, searchLen))
						break;                 
					brace = strchr(word, '(');
					if (brace)
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					else {
						brace = word + strlen(word);
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					}
					wordlen = brace - word + 1;
					newlength = length + wordlen; // stretch the buffer
					if (length)
						newlength++;
					if (newlength >= size)
					{
						do
							size += WORDCHUNK;
						while (size <= newlength);
						buffer = (char*) realloc(buffer, size);
					}
					if (length) // append a new entry
						buffer[length++] = ' ';
					memcpy(buffer + length, word, wordlen);
					length = newlength;
					buffer[length] = '\0';
				}
				return buffer; // result has to be freed with free()
			}
			else if (cond < 0)
				end = pivot - 1;
			else if (cond > 0)
				start = pivot + 1;
		}
	else // preserve the letter case
		while (start <= end) { // binary searching loop
			pivot = (start + end) >> 1;
			word = words[pivot];
			cond = strncmp(wordStart, word, searchLen);
			if (!cond) {
				oldpivot = pivot;
				do { // browse sequentially the rest after the hit
					brace = strchr(word, '(');
					if (brace)
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					else {
						brace = word + strlen(word);
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					}
					wordlen = brace - word + 1;
					newlength = length + wordlen; // stretch the buffer
					if (length)
						newlength++;
					if (newlength >= size)
					{
						do
							size += WORDCHUNK;
						while (size <= newlength);
						buffer = (char*) realloc(buffer, size);
					}
					if (length) // append a new entry
						buffer[length++] = ' ';
					memcpy(buffer + length, word, wordlen);
					length = newlength;
					buffer[length] = '\0';
					if (++pivot > end)
						break;
					word = words[pivot];
				} while (!strncmp(wordStart, word, searchLen));

				pivot = oldpivot;
				for (;;) { // browse sequentially the rest before the hit
					if (--pivot < start)
						break;
					word = words[pivot];
					if (strncmp(wordStart, word, searchLen))
						break;
					brace = strchr(word, '(');
					if (brace)
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					else {
						brace = word + strlen(word);
						do
							if (--brace < word)
								break;
						while (isspace(*brace));
					}
					wordlen = brace - word + 1;
					newlength = length + wordlen; // stretch the buffer
					if (length)
						newlength++;
					if (newlength >= size)
					{
						do
							size += WORDCHUNK;
						while (size <= newlength);
						buffer = (char*) realloc(buffer, size);
					}
					if (length) // append a new entry
						buffer[length++] = ' ';
					memcpy(buffer + length, word, wordlen);
					length = newlength;
					buffer[length] = '\0';
				}
				return buffer; // result has to be freed with free()
			}
			else if (cond < 0)
				end = pivot - 1;
			else if (cond > 0)
				start = pivot + 1;
		}
	free(buffer);
	return NULL;
}
