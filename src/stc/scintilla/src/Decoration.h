/** @file Decoration.h
 ** Visual elements added over text.
 **/
// Copyright 1998-2007 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef DECORATION_H
#define DECORATION_H

#ifdef SCI_NAMESPACE
namespace Scintilla {
#endif

class Decoration {
	int indicator;
public:
	RunStyles rs;

	explicit Decoration(int indicator_);
	~Decoration();

	bool Empty() const;
	int Indicator() const {
		return indicator;
	}
};

class DecorationList {
	int currentIndicator;
	int currentValue;
	Decoration *current;	// Cached so FillRange doesn't have to search for each call.
	int lengthDocument;
	// Ordered by indicator
	std::vector<std::unique_ptr<Decoration>> decorationList;
	std::vector<const Decoration*> decorationView;	// Read-only view of decorationList
	bool clickNotified;

	Decoration *DecorationFromIndicator(int indicator);
	Decoration *Create(int indicator, int length);
	void Delete(int indicator);
	void DeleteAnyEmpty();
	void SetView();
public:

	DecorationList();
	~DecorationList();

	const std::vector<const Decoration*> &View() const { return decorationView; }

	void SetCurrentIndicator(int indicator);
	int GetCurrentIndicator() const { return currentIndicator; }

	void SetCurrentValue(int value);
	int GetCurrentValue() const { return currentValue; }

	// Returns true if some values may have changed
	bool FillRange(int &position, int value, int &fillLength);

	void InsertSpace(int position, int insertLength);
	void DeleteRange(int position, int deleteLength);

	void DeleteLexerDecorations();

	int AllOnFor(int position) const;
	int ValueAt(int indicator, int position);
	int Start(int indicator, int position);
	int End(int indicator, int position);

	bool ClickNotified() const {
		return clickNotified;
	}
	void SetClickNotified(bool notified) {
		clickNotified = notified;
	}
};

#ifdef SCI_NAMESPACE
}
#endif

#endif
