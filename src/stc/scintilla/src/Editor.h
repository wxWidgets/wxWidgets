// Scintilla source code edit control
// Editor.h - defines the main editor class
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef EDITOR_H
#define EDITOR_H

class Caret {
public:
	bool active;
	bool on;
	int period;
		
	Caret();
};

class Timer {

public:
	bool ticking;
	int ticksToWait;
	enum {tickSize = 100};
	int tickerID;
		
	Timer();
};

class LineLayout {
public:
	// Drawing is only performed for maxLineLength characters on each line.
	enum {maxLineLength = 4000};
	int numCharsInLine;
	int xHighlightGuide;
	bool highlightColumn;
	int selStart;
	int selEnd;
	int edgeColumn;
	char chars[maxLineLength+1];
	char styles[maxLineLength+1];
	char indicators[maxLineLength+1];
	int positions[maxLineLength+1];
};

class Editor : public DocWatcher {
	// Private so Editor objects can not be copied
	Editor(const Editor &) : DocWatcher() {}
	Editor &operator=(const Editor &) { return *this; }
protected:	// ScintillaBase subclass needs access to much of Editor

	// On GTK+, Scintilla is a container widget holding two scroll bars and a drawing area
	// whereas on Windows there is just one window with both scroll bars turned on.
	// Therefore, on GTK+ the following are separate windows but only one window on Windows.
	Window wMain;	// The Scintilla parent window
	Window wDraw;	// The text drawing area

	// Style resources may be expensive to allocate so are cached between uses.
	// When a style attribute is changed, this cache is flushed.
	bool stylesValid;	
	ViewStyle vs;
	Palette palette;
	int printMagnification;
	int printColourMode;
	
	bool hideSelection;
	bool inOverstrike;

	// In bufferedDraw mode, graphics operations are drawn to a pixmap and then copied to 
	// the screen. This avoids flashing but is about 30% slower.
	bool bufferedDraw;

	int xOffset;				// Horizontal scrolled amount in pixels
	int xCaretMargin;	// Ensure this many pixels visible on both sides of caret
	bool horizontalScrollBarVisible;
	
	Surface pixmapLine;
	Surface pixmapSelMargin;
	Surface pixmapSelPattern;
	Surface pixmapIndentGuide;
	Surface pixmapIndentGuideHighlight;
	// Intellimouse support - currently only implemented for Windows
	unsigned int ucWheelScrollLines;
	int cWheelDelta; //wheel delta from roll

	KeyMap kmap;

	Caret caret;
	Timer timer;

	Point lastClick;
	unsigned int lastClickTime;
	enum { selChar, selWord, selLine } selectionType;
	Point ptMouseLast;
	bool firstExpose;
	bool inDragDrop;
	bool dropWentOutside;
	int posDrag;
	int posDrop;
	int lastXChosen;
	int lineAnchor;
	int originalAnchorPos;
	int currentPos;
	int anchor;
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
	
	char *dragChars;
	int lenDrag;
	bool dragIsRectangle;
	enum { selStream, selRectangle, selRectangleFixed } selType;
	int xStartSelect;
	int xEndSelect;
	bool primarySelection;
	
	int caretPolicy;
	int caretSlop;

	int searchAnchor;

	int displayPopupMenu;

#ifdef MACRO_SUPPORT
	int recordingMacro;
#endif

	int foldFlags;
	ContractionState cs;

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

	PRectangle GetClientRectangle();
	PRectangle GetTextRectangle();
	
	int LinesOnScreen();
	int LinesToScroll();
	int MaxScrollPos();
	Point LocationFromPosition(unsigned int pos);
	int XFromPosition(unsigned int pos);
	int PositionFromLocation(Point pt);
	int PositionFromLineX(int line, int x);
	int LineFromLocation(Point pt);
	void SetTopLine(int topLineNew);
	
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
	int MovePositionOutsideChar(int pos, int moveDir, bool checkLineEnd=true);
	int MovePositionTo(int newPos, bool extend = false);
	int MovePositionSoVisible(int pos, int moveDir);
	void SetLastXChosen();

	void ScrollTo(int line);
	virtual void ScrollText(int linesToMove);
	void HorizontalScrollTo(int xPos);
	void MoveCaretInsideView();
	void EnsureCaretVisible(bool useMargin=true);
	void ShowCaretAtCurrentPosition();
	void DropCaret();
	void InvalidateCaret();

	void PaintSelMargin(Surface *surface, PRectangle &rc);
        void LayoutLine(int line, Surface *surface, ViewStyle &vstyle, LineLayout &ll);
	void DrawLine(Surface *surface, ViewStyle &vsDraw, int line, int lineVisible, int xStart, 
		PRectangle rcLine, LineLayout &ll);
	void Paint(Surface *surfaceWindow, PRectangle rcArea);
	long FormatRange(bool draw, RangeToFormat *pfr);

	virtual void SetVerticalScrollPos() = 0;
	virtual void SetHorizontalScrollPos() = 0;
	virtual bool ModifyScrollBars(int nMax, int nPage) = 0;
	virtual void ReconfigureScrollBars();
	void SetScrollBarsTo(PRectangle rsClient);
	void SetScrollBars();

	void AddChar(char ch);
	virtual void AddCharUTF(char *s, unsigned int len);
	void ClearSelection();
	void ClearAll();
    	void ClearDocumentStyle();
	void Cut();
	void PasteRectangular(int pos, const char *ptr, int len);
	virtual void Copy() = 0;
	virtual void Paste() = 0;
	void Clear();
	void SelectAll();
	void Undo();
	void Redo();
	void DelChar();
	void DelCharBack();
	virtual void ClaimSelection() = 0;

	virtual void NotifyChange() = 0;
	virtual void NotifyFocus(bool focus);
	virtual void NotifyParent(SCNotification scn) = 0;
	virtual void NotifyStyleToNeeded(int endStyleNeeded);
	void NotifyChar(char ch);
	void NotifyMove(int position);
	void NotifySavePoint(bool isSavePoint);
	void NotifyModifyAttempt();
	virtual void NotifyDoubleClick(Point pt, bool shift);
	void NotifyUpdateUI();
	bool NotifyMarginClick(Point pt, bool shift, bool ctrl, bool alt);
	void NotifyNeedShown(int pos, int len);
	
	void NotifyModifyAttempt(Document *document, void *userData);
	void NotifySavePoint(Document *document, void *userData, bool atSavePoint);
	void NotifyModified(Document *document, DocModification mh, void *userData);
	void NotifyDeleted(Document *document, void *userData);
	void NotifyStyleNeeded(Document *doc, void *userData, int endPos);

	
#ifdef MACRO_SUPPORT
	void NotifyMacroRecord(unsigned int iMessage, unsigned long wParam, long lParam);
#endif

	void PageMove(int direction, bool extend=false);
	void ChangeCaseOfSelection(bool makeUpperCase);
	void LineTranspose();
    	virtual void CancelModes();
	virtual int KeyCommand(unsigned int iMessage);
	virtual int KeyDefault(int /* key */, int /*modifiers*/);
	int KeyDown(int key, bool shift, bool ctrl, bool alt);

	int GetWhitespaceVisible();
	void SetWhitespaceVisible(int view);

	void Indent(bool forwards);

	long FindText(unsigned int iMessage, unsigned long wParam, long lParam);
	void SearchAnchor();
	long SearchText(unsigned int iMessage, unsigned long wParam, long lParam);
	void GoToLine(int lineNo);

	char *CopyRange(int start, int end);
	int SelectionRangeLength();
	char *CopySelectionRange();
	void CopySelectionIntoDrag();
	void SetDragPosition(int newPos);
	virtual void StartDrag();
	void DropAt(int position, const char *value, bool moving, bool rectangular);
	// PositionInSelection returns 0 if position in selection, -1 if position before selection, and 1 if after.
	// Before means either before any line of selection or before selection on its line, with a similar meaning to after
	int PositionInSelection(int pos);
	bool PointInSelection(Point pt);
	bool PointInSelMargin(Point pt);
	virtual void ButtonDown(Point pt, unsigned int curTime, bool shift, bool ctrl, bool alt);
	void ButtonMove(Point pt);
	void ButtonUp(Point pt, unsigned int curTime, bool ctrl);

	void Tick();
	virtual void SetTicking(bool on) = 0;
	virtual void SetMouseCapture(bool on) = 0;
	virtual bool HaveMouseCapture() = 0;

	void CheckForChangeOutsidePaint(Range r);
	int BraceMatch(int position, int maxReStyle);
	void SetBraceHighlight(Position pos0, Position pos1, int matchStyle);
	
	void SetDocPointer(Document *document);
	
	void Expand(int &line, bool doExpand);
	void ToggleContraction(int line);
	void EnsureLineVisible(int line);

	virtual long DefWndProc(unsigned int iMessage, unsigned long wParam, long lParam) = 0;
	
public:
	// Public so scintilla_send_message can use it
	virtual long WndProc(unsigned int iMessage, unsigned long wParam, long lParam);
	// Public so scintilla_set_id can use it
	int ctrlID;	
};

#endif
