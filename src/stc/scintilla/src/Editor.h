// Scintilla source code edit control
/** @file Editor.h
 ** Defines the main editor class.
 **/
// Copyright 1998-2003 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef EDITOR_H
#define EDITOR_H

/**
 */
class Caret {
public:
	bool active;
	bool on;
	int period;

	Caret();
};

/**
 */
class Timer {
public:
	bool ticking;
	int ticksToWait;
	enum {tickSize = 100};
	TickerID tickerID;

	Timer();
};

/**
 */
class LineLayout {
private:
	friend class LineLayoutCache;
	int *lineStarts;
	int lenLineStarts;
	/// Drawing is only performed for @a maxLineLength characters on each line.
	int lineNumber;
	bool inCache;
public:
	enum { wrapWidthInfinite = 0x7ffffff };
	int maxLineLength;
	int numCharsInLine;
	enum validLevel { llInvalid, llCheckTextAndStyle, llPositions, llLines } validity;
	int xHighlightGuide;
	bool highlightColumn;
	int selStart;
	int selEnd;
	bool containsCaret;
	int edgeColumn;
	char *chars;
	char *styles;
	char *indicators;
	int *positions;
	char bracePreviousStyles[2];

	// Wrapped line support
	int widthLine;
	int lines;

	LineLayout(int maxLineLength_);
	virtual ~LineLayout();
	void Resize(int maxLineLength_);
	void Free();
	void Invalidate(validLevel validity_);
	int LineStart(int line) {
		if (line <= 0) {
			return 0;
		} else if ((line >= lines) || !lineStarts) {
			return numCharsInLine;
		} else {
			return lineStarts[line];
		}
	}
	void SetLineStart(int line, int start);
	void SetBracesHighlight(Range rangeLine, Position braces[],
		char bracesMatchStyle, int xHighlight);
	void RestoreBracesHighlight(Range rangeLine, Position braces[]);
};

/**
 */
class LineLayoutCache {
	int level;
	int length;
	int size;
	LineLayout **cache;
	bool allInvalidated;
	int styleClock;
	void Allocate(int length_);
	void AllocateForLevel(int linesOnScreen, int linesInDoc);
public:
	LineLayoutCache();
	virtual ~LineLayoutCache();
	void Deallocate();
	enum {
		llcNone=SC_CACHE_NONE,
		llcCaret=SC_CACHE_CARET,
		llcPage=SC_CACHE_PAGE,
		llcDocument=SC_CACHE_DOCUMENT
	};
	void Invalidate(LineLayout::validLevel validity_);
	void SetLevel(int level_);
	int GetLevel() { return level; }
	LineLayout *Retrieve(int lineNumber, int lineCaret, int maxChars, int styleClock_,
		int linesOnScreen, int linesInDoc);
	void Dispose(LineLayout *ll);
};

class SelectionText {
public:
	char *s;
	int len;
	bool rectangular;
	SelectionText() : s(0), len(0), rectangular(false) {}
	~SelectionText() {
		Set(0, 0);
	}
	void Set(char *s_, int len_, bool rectangular_=false) {
		delete []s;
		s = s_;
		if (s)
			len = len_;
		else
			len = 0;
		rectangular = rectangular_;
	}
};

/**
 * A smart pointer class to ensure Surfaces are set up and deleted correctly.
 */
class AutoSurface {
private:
	Surface *surf;
public:
	AutoSurface(int codePage) {
		surf = Surface::Allocate();
		if (surf) {
			surf->Init();
			surf->SetUnicodeMode(SC_CP_UTF8 == codePage);
			surf->SetDBCSMode(codePage);
		}
	}
	AutoSurface(SurfaceID sid, int codePage) {
		surf = Surface::Allocate();
		if (surf) {
			surf->Init(sid);
			surf->SetUnicodeMode(SC_CP_UTF8 == codePage);
			surf->SetDBCSMode(codePage);
		}
	}
	~AutoSurface() {
		delete surf;
	}
	Surface *operator->() const {
		return surf;
	}
	operator Surface *() const {
		return surf;
	}
};

/**
 */
class Editor : public DocWatcher {
	// Private so Editor objects can not be copied
	Editor(const Editor &) : DocWatcher() {}
	Editor &operator=(const Editor &) { return *this; }

protected:	// ScintillaBase subclass needs access to much of Editor

	/** On GTK+, Scintilla is a container widget holding two scroll bars
	 * whereas on Windows there is just one window with both scroll bars turned on. */
	Window wMain;	///< The Scintilla parent window

	/** Style resources may be expensive to allocate so are cached between uses.
	 * When a style attribute is changed, this cache is flushed. */
	bool stylesValid;
	ViewStyle vs;
	Palette palette;

	int printMagnification;
	int printColourMode;
	int printWrapState;
	int cursorMode;
	int controlCharSymbol;

	bool hasFocus;
	bool hideSelection;
	bool inOverstrike;
	int errorStatus;
	bool mouseDownCaptures;

	/** In bufferedDraw mode, graphics operations are drawn to a pixmap and then copied to
	 * the screen. This avoids flashing but is about 30% slower. */
	bool bufferedDraw;
	/** In twoPhaseDraw mode, drawing is performed in two phases, first the background
	* and then the foreground. This avoids chopping off characters that overlap the next run. */
	bool twoPhaseDraw;

	int xOffset;		///< Horizontal scrolled amount in pixels
	int xCaretMargin;	///< Ensure this many pixels visible on both sides of caret
	bool horizontalScrollBarVisible;
	int scrollWidth;
	bool verticalScrollBarVisible;
	bool endAtLastLine;

	Surface *pixmapLine;
	Surface *pixmapSelMargin;
	Surface *pixmapSelPattern;
	Surface *pixmapIndentGuide;
	Surface *pixmapIndentGuideHighlight;

	LineLayoutCache llc;

	KeyMap kmap;

	Caret caret;
	Timer timer;
	Timer autoScrollTimer;
	enum { autoScrollDelay = 200 };

	Point lastClick;
	unsigned int lastClickTime;
	int dwellDelay;
	int ticksToDwell;
	bool dwelling;
	enum { selChar, selWord, selLine } selectionType;
	Point ptMouseLast;
	bool inDragDrop;
	bool dropWentOutside;
	int posDrag;
	int posDrop;
	int lastXChosen;
	int lineAnchor;
	int originalAnchorPos;
	int currentPos;
	int anchor;
	int targetStart;
	int targetEnd;
	int searchFlags;
	int topLine;
	int posTopLine;

	bool needUpdateUI;
	Position braces[2];
	int bracesMatchStyle;
	int highlightGuideColumn;

	int theEdge;

	enum { notPainting, painting, paintAbandoned } paintState;
	PRectangle rcPaint;
	bool paintingAllText;

	int modEventMask;

	SelectionText drag;
	enum { selStream, selRectangle, selRectangleFixed } selType;
	int xStartSelect;
	int xEndSelect;
	bool primarySelection;

	int caretXPolicy;
	int caretXSlop;	///< Ensure this many pixels visible on both sides of caret

	int caretYPolicy;
	int caretYSlop;	///< Ensure this many lines visible on both sides of caret

	int visiblePolicy;
	int visibleSlop;

	int searchAnchor;

	bool recordingMacro;

	int foldFlags;
	ContractionState cs;

	// Wrapping support
	enum { eWrapNone, eWrapWord } wrapState;
	int wrapWidth;
	int docLineLastWrapped;

	Document *pdoc;

	Editor();
	virtual ~Editor();
	virtual void Initialise() = 0;
	virtual void Finalise();

	void InvalidateStyleData();
	void InvalidateStyleRedraw();
	virtual void RefreshColourPalette(Palette &pal, bool want);
	void RefreshStyleData();
	void DropGraphics();

	virtual PRectangle GetClientRectangle();
	PRectangle GetTextRectangle();

	int LinesOnScreen();
	int LinesToScroll();
	int MaxScrollPos();
	Point LocationFromPosition(int pos);
	int XFromPosition(int pos);
	int PositionFromLocation(Point pt);
	int PositionFromLocationClose(Point pt);
	int PositionFromLineX(int line, int x);
	int LineFromLocation(Point pt);
	void SetTopLine(int topLineNew);

	bool AbandonPaint();
	void RedrawRect(PRectangle rc);
	void Redraw();
	void RedrawSelMargin();
	PRectangle RectangleFromRange(int start, int end);
	void InvalidateRange(int start, int end);

	int CurrentPosition();
	bool SelectionEmpty();
	int SelectionStart(int line=-1);
	int SelectionEnd(int line=-1);
	void SetSelection(int currentPos_, int anchor_);
	void SetSelection(int currentPos_);
	void SetEmptySelection(int currentPos_);
	bool RangeContainsProtected(int start, int end) const;
	bool SelectionContainsProtected() const;
	int MovePositionOutsideChar(int pos, int moveDir, bool checkLineEnd=true);
	int MovePositionTo(int newPos, bool extend=false, bool ensureVisible=true);
	int MovePositionSoVisible(int pos, int moveDir);
	void SetLastXChosen();

	void ScrollTo(int line, bool moveThumb=true);
	virtual void ScrollText(int linesToMove);
	void HorizontalScrollTo(int xPos);
	void MoveCaretInsideView(bool ensureVisible=true);
	int DisplayFromPosition(int pos);
	void EnsureCaretVisible(bool useMargin=true, bool vert=true, bool horiz=true);
	void ShowCaretAtCurrentPosition();
	void DropCaret();
	void InvalidateCaret();

	void NeedWrapping(int docLineStartWrapping=0);
	bool WrapLines();
	void LinesJoin();
	void LinesSplit(int pixelWidth);

	int SubstituteMarkerIfEmpty(int markerCheck, int markerDefault);
	void PaintSelMargin(Surface *surface, PRectangle &rc);
	LineLayout *RetrieveLineLayout(int lineNumber);
	void LayoutLine(int line, Surface *surface, ViewStyle &vstyle, LineLayout *ll,
		int width=LineLayout::wrapWidthInfinite);
	ColourAllocated TextBackground(ViewStyle &vsDraw, bool overrideBackground, ColourAllocated background, bool inSelection, int styleMain, int i, LineLayout *ll);
	void DrawIndentGuide(Surface *surface, int lineVisible, int lineHeight, int start, PRectangle rcSegment, bool highlight);
	void DrawEOL(Surface *surface, ViewStyle &vsDraw, PRectangle rcLine, LineLayout *ll,
		int line, int lineEnd, int xStart, int subLine, int subLineStart,
		bool overrideBackground, ColourAllocated background);
	void DrawLine(Surface *surface, ViewStyle &vsDraw, int line, int lineVisible, int xStart,
		PRectangle rcLine, LineLayout *ll, int subLine=0);
    void RefreshPixMaps(Surface *surfaceWindow);
	void Paint(Surface *surfaceWindow, PRectangle rcArea);
	long FormatRange(bool draw, RangeToFormat *pfr);
	int TextWidth(int style, const char *text);

	virtual void SetVerticalScrollPos() = 0;
	virtual void SetHorizontalScrollPos() = 0;
	virtual bool ModifyScrollBars(int nMax, int nPage) = 0;
	virtual void ReconfigureScrollBars();
	void SetScrollBars();
	void ChangeSize();

	void AddChar(char ch);
	virtual void AddCharUTF(char *s, unsigned int len, bool treatAsDBCS=false);
	void ClearSelection();
	void ClearAll();
    	void ClearDocumentStyle();
	void Cut();
	void PasteRectangular(int pos, const char *ptr, int len);
	virtual void Copy() = 0;
	virtual bool CanPaste();
	virtual void Paste() = 0;
	void Clear();
	void SelectAll();
	void Undo();
	void Redo();
	void DelChar();
	void DelCharBack(bool allowLineStartDeletion);
	virtual void ClaimSelection() = 0;

	virtual void NotifyChange() = 0;
	virtual void NotifyFocus(bool focus);
	virtual int GetCtrlID() { return ctrlID; }
	virtual void NotifyParent(SCNotification scn) = 0;
	virtual void NotifyStyleToNeeded(int endStyleNeeded);
	void NotifyChar(int ch);
	void NotifyMove(int position);
	void NotifySavePoint(bool isSavePoint);
	void NotifyModifyAttempt();
	virtual void NotifyDoubleClick(Point pt, bool shift);
	void NotifyUpdateUI();
	void NotifyPainted();
	bool NotifyMarginClick(Point pt, bool shift, bool ctrl, bool alt);
	void NotifyNeedShown(int pos, int len);
	void NotifyDwelling(Point pt, bool state);
	void NotifyZoom();

	void NotifyModifyAttempt(Document *document, void *userData);
	void NotifySavePoint(Document *document, void *userData, bool atSavePoint);
	void CheckModificationForWrap(DocModification mh);
	void NotifyModified(Document *document, DocModification mh, void *userData);
	void NotifyDeleted(Document *document, void *userData);
	void NotifyStyleNeeded(Document *doc, void *userData, int endPos);
	void NotifyMacroRecord(unsigned int iMessage, uptr_t wParam, sptr_t lParam);

	void PageMove(int direction, bool extend=false);
	void ChangeCaseOfSelection(bool makeUpperCase);
	void LineTranspose();
	void LineDuplicate();
	virtual void CancelModes();
	void NewLine();
	void CursorUpOrDown(int direction, bool extend=false);
	int StartEndDisplayLine(int pos, bool start);
	virtual int KeyCommand(unsigned int iMessage);
	virtual int KeyDefault(int /* key */, int /*modifiers*/);
	int KeyDown(int key, bool shift, bool ctrl, bool alt, bool *consumed=0);

	int GetWhitespaceVisible();
	void SetWhitespaceVisible(int view);

	void Indent(bool forwards);

	long FindText(uptr_t wParam, sptr_t lParam);
	void SearchAnchor();
	long SearchText(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
	long SearchInTarget(const char *text, int length);
	void GoToLine(int lineNo);

	char *CopyRange(int start, int end);
	void CopySelectionRange(SelectionText *ss);
	void SetDragPosition(int newPos);
	void DisplayCursor(Window::Cursor c);
	virtual void StartDrag();
	void DropAt(int position, const char *value, bool moving, bool rectangular);
	/** PositionInSelection returns 0 if position in selection, -1 if position before selection, and 1 if after.
	 * Before means either before any line of selection or before selection on its line, with a similar meaning to after. */
	int PositionInSelection(int pos);
	bool PointInSelection(Point pt);
	bool PointInSelMargin(Point pt);
	void LineSelection(int lineCurrent_, int lineAnchor_);
	void DwellEnd(bool mouseMoved);
	virtual void ButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
	void ButtonMove(Point pt);
	void ButtonUp(Point pt, unsigned int curTime, bool ctrl);

	void Tick();
	virtual void SetTicking(bool on) = 0;
	virtual void SetMouseCapture(bool on) = 0;
	virtual bool HaveMouseCapture() = 0;
	void SetFocusState(bool focusState);

	void CheckForChangeOutsidePaint(Range r);
	int BraceMatch(int position, int maxReStyle);
	void SetBraceHighlight(Position pos0, Position pos1, int matchStyle);

	void SetDocPointer(Document *document);

	void Expand(int &line, bool doExpand);
	void ToggleContraction(int line);
	void EnsureLineVisible(int lineDoc, bool enforcePolicy);
	int ReplaceTarget(bool replacePatterns, const char *text, int length=-1);

	int CodePage() const;

	virtual sptr_t DefWndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam) = 0;

public:
	// Public so the COM thunks can access it.
	bool IsUnicodeMode() const;
	// Public so scintilla_send_message can use it.
	virtual sptr_t WndProc(unsigned int iMessage, uptr_t wParam, sptr_t lParam);
	// Public so scintilla_set_id can use it.
	int ctrlID;
};

#endif
