// Scintilla source code edit control
/** @file AutoComplete.cxx
 ** Defines the auto completion list box.
 **/
// Copyright 1998-2002 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Platform.h"

#include "PropSet.h"
#include "AutoComplete.h"

AutoComplete::AutoComplete() : 
	active(false),
	separator(' '),
	ignoreCase(false),
	chooseSingle(false),
	posStart(0),
	startLen(0),
	cancelAtStartPos(true),
	autoHide(true),
	dropRestOfWord(false)	{
	stopChars[0] = '\0';
	fillUpChars[0] = '\0';
}

AutoComplete::~AutoComplete() {
	lb.Destroy();
}

bool AutoComplete::Active() {
	return active;
}

void AutoComplete::Start(Window &parent, int ctrlID, int position, int startLen_) {
	if (!lb.Created()) {
		lb.Create(parent, ctrlID);
	}
	lb.Clear();
	active = true;
	startLen = startLen_;
	posStart = position;
}

void AutoComplete::SetStopChars(const char *stopChars_) {
	strncpy(stopChars, stopChars_, sizeof(stopChars));
	stopChars[sizeof(stopChars) - 1] = '\0';
}

bool AutoComplete::IsStopChar(char ch) {
	return ch && strchr(stopChars, ch);
}

void AutoComplete::SetFillUpChars(const char *fillUpChars_) {
	strncpy(fillUpChars, fillUpChars_, sizeof(fillUpChars));
	fillUpChars[sizeof(fillUpChars) - 1] = '\0';
}

bool AutoComplete::IsFillUpChar(char ch) {
	return ch && strchr(fillUpChars, ch);
}
 
void AutoComplete::SetSeparator(char separator_) {
	separator = separator_;
}

char AutoComplete::GetSeparator() {
	return separator;
}

void AutoComplete::SetList(const char *list) {
	lb.Clear();
	char *words = new char[strlen(list) + 1];
	if (words) {
		strcpy(words, list);
		char *startword = words;
		int i = 0;
		for (; words && words[i]; i++) {
			if (words[i] == separator) {
				words[i] = '\0';
				lb.Append(startword);
				startword = words + i + 1;
			}
		}
		if (startword) {
			lb.Append(startword);
		}
		delete []words;
	}
}

void AutoComplete::Show() {
	lb.Show();
	lb.Select(0);
}

void AutoComplete::Cancel() {
	if (lb.Created()) {
		lb.Destroy();
		active = false;
	}
}


void AutoComplete::Move(int delta) {
	int count = lb.Length();
	int current = lb.GetSelection();
	current += delta;
	if (current >= count)
		current = count - 1;
	if (current < 0)
		current = 0;
	lb.Select(current);
}

void AutoComplete::Select(const char *word) {
	size_t lenWord = strlen(word);
	int location = -1;
	const int maxItemLen=1000;
	char item[maxItemLen];
	int start = 0; // lower bound of the api array block to search
	int end = lb.Length() - 1; // upper bound of the api array block to search
	while ((start <= end) && (location == -1)) { // Binary searching loop
		int pivot = (start + end) / 2;
		lb.GetValue(pivot, item, maxItemLen);
		int cond;
		if (ignoreCase)
			cond = CompareNCaseInsensitive(word, item, lenWord);
		else
			cond = strncmp(word, item, lenWord);
		if (!cond) {
			// Find first match
			while (pivot > start) {
				lb.GetValue(pivot-1, item, maxItemLen);
				if (ignoreCase)
					cond = CompareNCaseInsensitive(word, item, lenWord);
				else
					cond = strncmp(word, item, lenWord);
				if (0 != cond)
					break;
				--pivot;
			}
			location = pivot;
		} else if (cond < 0) {
			end = pivot - 1;
		} else if (cond > 0) {
			start = pivot + 1;
		}
	}
	if (location == -1 && autoHide)
		Cancel();
	else
		lb.Select(location);
}

