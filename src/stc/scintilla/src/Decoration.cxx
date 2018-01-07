/** @file Decoration.cxx
 ** Visual elements added over text.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstdarg>

#include <stdexcept>
#include <vector>
#include <algorithm>
#include <memory>

#include "Platform.h"

#include "Scintilla.h"
#include "Position.h"
#include "SplitVector.h"
#include "Partitioning.h"
#include "RunStyles.h"
#include "Decoration.h"

#ifdef SCI_NAMESPACE
using namespace Scintilla;
#endif

Decoration::Decoration(int indicator_) : indicator(indicator_) {
}

Decoration::~Decoration() {
}

bool Decoration::Empty() const {
	return (rs.Runs() == 1) && (rs.AllSameAs(0));
}

DecorationList::DecorationList() : currentIndicator(0), currentValue(1), current(nullptr),
	lengthDocument(0), clickNotified(false) {
}

DecorationList::~DecorationList() {
	current = nullptr;
}

Decoration *DecorationList::DecorationFromIndicator(int indicator) {
	for (const std::unique_ptr<Decoration> &deco : decorationList) {
		if (deco->Indicator() == indicator) {
			return deco.get();
		}
	}
	return nullptr;
}

Decoration *DecorationList::Create(int indicator, int length) {
	currentIndicator = indicator;
	std::unique_ptr<Decoration> decoNew(new Decoration(indicator));
	decoNew->rs.InsertSpace(0, length);

	std::vector<std::unique_ptr<Decoration>>::iterator it = std::lower_bound(
		decorationList.begin(), decorationList.end(), decoNew, 
		[](const std::unique_ptr<Decoration> &a, const std::unique_ptr<Decoration> &b) {
		return a->Indicator() < b->Indicator();
	});
	std::vector<std::unique_ptr<Decoration>>::iterator itAdded = 
		decorationList.insert(it, std::move(decoNew));

	SetView();

	return itAdded->get();
}

void DecorationList::Delete(int indicator) {
	decorationList.erase(std::remove_if(decorationList.begin(), decorationList.end(),
		[=](const std::unique_ptr<Decoration> &deco) {
		return deco->Indicator() == indicator;
	}), decorationList.end());
	current = nullptr;
	SetView();
}

void DecorationList::SetCurrentIndicator(int indicator) {
	currentIndicator = indicator;
	current = DecorationFromIndicator(indicator);
	currentValue = 1;
}

void DecorationList::SetCurrentValue(int value) {
	currentValue = value ? value : 1;
}

bool DecorationList::FillRange(int &position, int value, int &fillLength) {
	if (!current) {
		current = DecorationFromIndicator(currentIndicator);
		if (!current) {
			current = Create(currentIndicator, lengthDocument);
		}
	}
	const bool changed = current->rs.FillRange(position, value, fillLength);
	if (current->Empty()) {
		Delete(currentIndicator);
	}
	return changed;
}

void DecorationList::InsertSpace(int position, int insertLength) {
	const bool atEnd = position == lengthDocument;
	lengthDocument += insertLength;
	for (const std::unique_ptr<Decoration> &deco : decorationList) {
		deco->rs.InsertSpace(position, insertLength);
		if (atEnd) {
			deco->rs.FillRange(position, 0, insertLength);
		}
	}
}

void DecorationList::DeleteRange(int position, int deleteLength) {
	lengthDocument -= deleteLength;
	for (const std::unique_ptr<Decoration> &deco : decorationList) {
		deco->rs.DeleteRange(position, deleteLength);
	}
	DeleteAnyEmpty();
	if (decorationList.size() != decorationView.size()) {
		// One or more empty decorations deleted so update view.
		current = nullptr;
		SetView();
	}
}

void DecorationList::DeleteLexerDecorations() {
	decorationList.erase(std::remove_if(decorationList.begin(), decorationList.end(),
		[=](const std::unique_ptr<Decoration> &deco) {
		return deco->Indicator() < INDIC_CONTAINER;
	}), decorationList.end());
	current = nullptr;
	SetView();
}

void DecorationList::DeleteAnyEmpty() {
	if (lengthDocument == 0) {
		decorationList.clear();
	} else {
		decorationList.erase(std::remove_if(decorationList.begin(), decorationList.end(),
			[=](const std::unique_ptr<Decoration> &deco) {
			return deco->Empty();
		}), decorationList.end());
	}
}

void DecorationList::SetView() {
	decorationView.clear();
	for (const std::unique_ptr<Decoration> &deco : decorationList) {
		decorationView.push_back(deco.get());
	}
}

int DecorationList::AllOnFor(int position) const {
	int mask = 0;
	for (const std::unique_ptr<Decoration> &deco : decorationList) {
		if (deco->rs.ValueAt(position)) {
			if (deco->Indicator() < INDIC_IME) {
				mask |= 1 << deco->Indicator();
			}
		}
	}
	return mask;
}

int DecorationList::ValueAt(int indicator, int position) {
	const Decoration *deco = DecorationFromIndicator(indicator);
	if (deco) {
		return deco->rs.ValueAt(position);
	}
	return 0;
}

int DecorationList::Start(int indicator, int position) {
	const Decoration *deco = DecorationFromIndicator(indicator);
	if (deco) {
		return deco->rs.StartRun(position);
	}
	return 0;
}

int DecorationList::End(int indicator, int position) {
	const Decoration *deco = DecorationFromIndicator(indicator);
	if (deco) {
		return deco->rs.EndRun(position);
	}
	return 0;
}
