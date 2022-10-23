// Scintilla source code edit control
/** @file UniqueString.cxx
 ** Define an allocator for UniqueString.
 **/
// Copyright 2017 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstring>
#include <vector>
#include <algorithm>
#include <memory>

#include "UniqueString.h"

namespace Scintilla {

/// Equivalent to strdup but produces a std::unique_ptr<const char[]> allocation to go
/// into collections.
UniqueString UniqueStringCopy(const char *text) {
	if (!text) {
		return UniqueString();
	}
	const size_t len = strlen(text);
	std::unique_ptr<char[]> upcNew = Sci::make_unique<char[]>(len + 1);
	memcpy(upcNew.get(), text, len + 1);
	return UniqueString(upcNew.release());
}

// A set of strings that always returns the same pointer for each string.

UniqueStringSet::UniqueStringSet() = default;

UniqueStringSet::~UniqueStringSet() {
	strings.clear();
}

void UniqueStringSet::Clear() noexcept {
	strings.clear();
}

const char *UniqueStringSet::Save(const char *text) {
	if (!text)
		return nullptr;

	for (const UniqueString &us : strings) {
		if (text == us.get()) {
			return us.get();
		}
	}

	strings.push_back(UniqueStringCopy(text));
	return strings.back().get();
}

}
