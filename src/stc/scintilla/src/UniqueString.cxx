// Scintilla source code edit control
/** @file UniqueString.cxx
 ** Define an allocator for UniqueString.
 **/
// Copyright 2017 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

#include "UniqueString.h"
#include "Compat.h"

namespace Scintilla {

/// Equivalent to strdup but produces a std::unique_ptr<const char[]> allocation to go
/// into collections.
UniqueString UniqueStringCopy(const char *text) {
	if (!text) {
		return UniqueString();
	}
	const std::string sv(text);
	std::unique_ptr<char[]> upcNew = Sci::make_unique<char[]>(sv.length() + 1);
	sv.copy(upcNew.get(), sv.length());
	return UniqueString(upcNew.release());
}

// A set of strings that always returns the same pointer for each string.

UniqueStringSet::UniqueStringSet() noexcept = default;

UniqueStringSet::~UniqueStringSet() {
	strings.clear();
}

void UniqueStringSet::Clear() noexcept {
	strings.clear();
}

const char *UniqueStringSet::Save(const char *text) {
	if (!text)
		return nullptr;

	const std::string sv(text);
	for (const UniqueString &us : strings) {
		if (sv == us.get()) {
			return us.get();
		}
	}

	strings.push_back(UniqueStringCopy(text));
	return strings.back().get();
}

}
