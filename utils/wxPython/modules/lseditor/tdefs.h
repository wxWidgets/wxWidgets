/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     03/04/1999
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:   	GNU General Public License 
/////////////////////////////////////////////////////////////////////////////
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
/////////////////////////////////////////////////////////////////////////////



#ifndef __TDEFS_G__
#define __TDEFS_G__

// should be compiled with wxSTL-v.1.2 (or higher)

#include "wxstldefs.h"

#if defined( wxUSE_TEMPLATE_STL )
	#include <vector>
	#include <list>
#else
	#include "wxstlvec.h"
	#include "wxstllst.h"
#endif

#include "wx/window.h"
#include "wx/scrolbar.h"

#include "sourcepainter.h"

#define NPOS ((size_t)(-1))

class wxTextEditorModel;
class wxTextEditorView;

/*
 * class represents column-row position in the source text,
 * may refere to the column past the end-of-line,
 * but should not point past the last-line in the text
 */

class TPosition
{
public:
	size_t mRow;
	size_t mCol;

	inline TPosition() : mRow(0), mCol(0) {}

	inline TPosition( size_t row, size_t col )

		: mRow( row ), mCol( col ) {}

	inline bool operator>( const TPosition& rhs ) const
	{ 
		if ( mRow == rhs.mRow ) return mCol > rhs.mCol;
		else
			return mRow > rhs.mRow;
	}

	inline bool operator<( const TPosition& rhs ) const
	{ 
		if ( mRow == rhs.mRow ) return mCol < rhs.mCol;
		else
			return mRow < rhs.mRow;
	}
};

class TRange
{
public:
	TPosition mFrom;
	TPosition mTill;

	TRange() {}
	TRange( const TPosition& from, const TPosition& till )
		: mFrom( from ), mTill( till )
	{}
};

inline bool operator==( const TPosition& lhs, const TPosition& rhs )
{
	return lhs.mRow == rhs.mRow && lhs.mCol == rhs.mCol;
}

// FOR NOW:: bigger ones...

#define MAX_BLOCK_LEN      (1024*164)
#define BALANCED_BLOCK_LEN (1024*152)		 
#define FILLED_BLOCK_LEN   (1024*148)

// FOR NOW::
#define T_ASSERT( x ) if ( !(x) ) throw;

// to speed up debug v. :

#define is_eol_char( ch )     ( ch == (char)10 )
#define is_DOS_eol_char( ch ) ( ch == (char)13 )

// the target-platfrom eol-marking is selected when
// new text document is created or auto-detection
// failed to determine the text-format (e.g. no EOLs found at all)

#if defined(__WINDOWS__) || defined(__WXMSW__)

	  #define IS_UNIX_TEXT_BY_DEFAULT FALSE
#else
	  #define IS_UNIX_TEXT_BY_DEFAULT TRUE
#endif

//inline bool is_eol_char( char ch ) { return ch == 10 && ch == 13 ; }

/*
 * Class contains single fragment of the source text, which
 * may grow or shrink in the process of editing. Blocks always
 * start at the begining of the line and end at eol, i.e. lines
 * are not broken among blocks
 */

class TBlock
{
public:
	char mBuf[MAX_BLOCK_LEN];
	size_t mTextLen;
	size_t mRowCount;

	TBlock() : mTextLen(0), mRowCount(0) { mBuf[0] = '\0'; }

	void RecalcBlockProperties();

	bool operator==( const TBlock& blk ) const { return this == &blk; }

        bool operator!=( const TBlock& blk ) const { return this != &blk; }

	bool operator<( const TBlock& blk ) const { return TRUE; }

        bool operator>( const TBlock& blk ) const { return FALSE; }
};

/*
 * captures info about mutable command 
 */

class TCommand
{
public:

	TCommand() : mType(-1) {}
	TCommand( int type ) : mType( type ) {}
	~TCommand() {}

	int       mType;
	char*     mData;
	size_t    mDataLen;

	TRange    mRange;

	// positions of cursor before and after executions of this command
	TPosition mPrePos;
	TPosition mPostPos;

};

enum TEXT_EDITOR_COMMAND
{
	TCMD_INSERT,
	TCMD_DELETE
};

enum TEXT_CHANGE_TYPE
{
	CT_MODIFIED,
	CT_DELETED,
	CT_INSERTED
};

class wxTextEditorView;

// STL-list is used for managing blocks, since it's alg. collects
// removed elements into a free-list, from which they
// can be reclaimed later, that way heap-fragmentation may be reduced

#if defined( wxUSE_TEMPLATE_STL )
	typedef list<TBlock>      TBlockListT;
	typedef vector<TCommand*> TCommandListT;
	typedef vector<wxTextEditorView*> TextViewListT;
#else
	typedef WXSTL_LIST( TBlock )  TBlockListT;

	typedef TCommand* TCommandPtrT;
	typedef WXSTL_VECTOR_SHALLOW_COPY( TCommandPtrT ) TCommandListT;

	typedef wxTextEditorView* TextViewPtrT;
	typedef WXSTL_VECTOR_SHALLOW_COPY( TextViewPtrT ) TextViewListT;
#endif

typedef TBlockListT::iterator TBlockIteratorT;


/*
 * class shields the higher-level operations from direct access
 * to blocks of fragmented in-memory buffers
 */

class TTextIterator
{
public:
	TBlockIteratorT   mBlockIter;
	TBlockIteratorT   mEndOfListIter;
	TPosition         mPos;

	size_t            mActualRow;
	size_t            mFirstRowInBlock;

	char*             mpCurRowStart;
	bool              mIsEof;

public:
	TTextIterator();

	char GetChar();
	bool IsEol();
	bool IsEof();
	bool IsLastLine();
	int  GetDistFromEol();

	void NextChar();
	void PreviousChar();
	void NextWord();
	void PreviousWord();
	void ToEndOfLine();
	void ToStartOfLine();

	bool IsInLastBlock();

	// accesors

	size_t    GetLineLen();
	TPosition GetPosition();

	char* GetClosestPos(); 
	char* GotoClosestPos();

	inline char* GetBlockStart() { return (*mBlockIter).mBuf; }
	inline char* GetBlockEnd()   { return (*mBlockIter).mBuf + (*mBlockIter).mTextLen; }

	bool DetectUnixText();

	// adjust this member to add specific separators, 
	// the default value is : ",.()[]\t\\+-*/|=<>:;\t\n~?!%"

	static string mSeparators;

	static bool IsSeparator( char ch );
};

class wxTextEditorModel;

class TTextChangeListenerBase
{
public:
	virtual void OnTextChanged( wxTextEditorModel* pModel, size_t atRow, size_t nRows, TEXT_CHANGE_TYPE ct ) = 0;
};

class TCppJavaHighlightListener : public TTextChangeListenerBase
{
protected:
	wxTextEditorModel* mpModel; // is set up temporarely

	enum { IN_COMMENT_STATE, OUT_OF_COMMENT_STATE };

public:
	virtual void OnTextChanged( wxTextEditorModel* pModel, size_t atRow, size_t nRows, TEXT_CHANGE_TYPE ct );
};


/*
 * Base class for user-defined "bookmarks" within the source-text, bookmarks
 * are automatically repositioned or deleted as the text is edited. Class
 * can be subclassed to add pin-specific data (e.g. breakpoint information)
 */

class TPinBase
{
public:
	int    mTypeCode;
	size_t mRow;

public:
	TPinBase() 
		: mTypeCode(-1), mRow(NPOS) {}

	TPinBase( int typeCode, size_t row )
		: mTypeCode( typeCode ), mRow( row ) {}

	size_t GetRow()      { return mRow; }
	int    GetTypeCode() { return mTypeCode; }

	virtual ~TPinBase() {}
};

// "recommened" type-code ranges for custom pins

#define HIHGLIGHTING_PINS_TC_STARRT 50
#define OTHER_PINS_TC_START         100

inline bool operator<( const TPinBase& lhs, TPinBase& rhs )

	{ return lhs.mRow < rhs.mRow; }

#if defined( wxUSE_TEMPLATE_STL )

	typedef vector<TPinBase*> PinListT;
	typedef vector<TTextChangeListenerBase*> ChangeListenerListT;
#else
	typedef TPinBase* TPinBasePtrT;
	typedef WXSTL_VECTOR_SHALLOW_COPY( TPinBasePtrT ) PinListT;

	typedef TTextChangeListenerBase* TTextChangeListenerBasePtrT;
	typedef WXSTL_VECTOR_SHALLOW_COPY( TTextChangeListenerBasePtrT ) ChangeListenerListT;
#endif

/* OLD STUFF::

struct TPinBaseCompareFunctor
{
	inline int operator()(const TPinBasePtrT* x, const TPinBasePtrT*& y ) const 
	{ 
		return x->mLine < y->mLine;
	}
};

typedef WXSTL_MULTIMAP( TPinBasePtrT, TPinBasePtrT, TPinBaseCompareFunctor ) PinMapT;
typedef PinMapT::iterator PinIteratorT;
*/

/* 
 * Class manages access and manpulation of in-memory text. Can
 * be accessed by multiple views, only one of which can be active
 * at a time.
 */

class wxTextEditorModel
{
protected:
	TBlockListT         mBlocks;
	
	TCommandListT       mCommands;
	size_t              mCurCommand;

	TextViewListT       mViews;
	wxTextEditorView*   mpActiveView;

	PinListT            mPins;
	bool                mIsUnixText;

	ChangeListenerListT mChangeListeners;

public:
	/*** public properties ***/

	bool          mTextChanged;
	size_t        mChangedFromRow;
	size_t        mChangedTillRow;

	bool          mWasChanged;			  // TRUE, if any content has been changed

	TPosition     mCursorPos;

	TPosition	  mPrevSelectionStart;
	TPosition     mPrevSelectionEnd;
	TPosition     mPrevCursorPos;

	TPosition     mSelectionStart;
	TPosition     mSelectionEnd;
	size_t        mRowsPerPage;

	bool          mIsReadOnly;           // default: FALSE
	bool          mIsModified;     
	bool          mInsertMode;			 // default: TRUE
	bool          mAutoIndentMode;       // default: TRUE
	bool          mSmartIndentMode;      // default: TRUE

	bool          mIsSelectionEditMode;  // default: TRUE
	size_t        mTabSize;              // default: 4

	StrListT      mSearchExprList;
	string        mLastFindExpr;

	bool          mCheckPointDestroyed;
	size_t        mCheckPointCmdNo;

protected:

	size_t GetLineCountInRange( char* from, char* till ); 

	// two lowest-level operations
	void DoInsertText ( const TPosition& pos, char* text, size_t len, TRange& actualRange );
	void DoDeleteRange( const TPosition& from, const TPosition& till, TRange& actualRange );

	void DoExecuteNewCommand( TCommand& cmd );

	void DoReexecuteCommand( TCommand& cmd );
	void DoUnexecuteCommand( TCommand& cmd );


	void ExecuteCommand( TCommand* pCmd );

	// to methods enabling grouping of undo-able commands
	bool CanPrependCommand( TCommand* pCmd );
	void PrependCommand( TCommand* pCmd );

	void SetPostPos( const TPosition& pos );

	void UndoImpl();
	void RedoImpl();

	void StartBatch();
	void FinishBatch();

	void CheckSelection();
	void TrackSelection();

	void NotifyView();
	void NotifyAllViews();

	void NotifyTextChanged( size_t atRow, size_t nRows, TEXT_CHANGE_TYPE ct );
	void NotifyTextChanged( TPosition from, TPosition till, TEXT_CHANGE_TYPE ct );

	void ArrangePositions( TPosition& upper, TPosition& lower );
	void ArrangePositions( size_t& upper, size_t& lower );

	void MergeChange( size_t fromRow, size_t nRows );

	void PrepreForCommand();

	size_t TextToScrColumn( const TPosition& pos );
	size_t ScrToTextColumn( TPosition pos );

	void DoMoveCursor( int rows, int cols );

public:
	wxTextEditorModel();
	virtual ~wxTextEditorModel();

	// utilities

	char* AllocCharacters( size_t n );
	char* AllocCharacters( size_t n, const char* srcBuf );
	void FreeCharacters( char* buf );

	void DeleteSelection();
	TTextIterator CreateIterator( const TPosition& pos );

	void DeleteRange( const TPosition& from, const TPosition& till );
	void InsertText( const TPosition& pos, const char* text, size_t len );
	void GetTextFromRange( const TPosition& from, const TPosition& till, char** text, size_t& textLen );
	void LoadTextFromFile( const wxString& fname );
	void SaveTextToFile( const wxString& fname );
	void ResetSelection();
	void ClearUndoBuffer();


	void DeleteAllText();
	void GetAllText( char** text, size_t& textLen );

	void SetSelectionEditMode( bool editIsOn );

	/*** user-level commands ***/

	// mutable (undoable) commands

	void OnInsertChar( char ch );
	void OnDelete();
	void OnDeleteBack();
	void OnDeleteLine();

	void OnShiftSelectionIndent( bool left );

	// clipboard functions

	void OnCopy();
	void OnPaste();
	void OnCut();
	bool CanCopy();
	bool CanPaste();

	// undo-redo

	bool CanUndo();
	bool CanRedo();
	void OnUndo();
	void OnRedo();

	// imutable commands

	void OnMoveLeft();
	void OnMoveRight();
	void OnMoveUp();
	void OnMoveDown();

	void OnWordLeft();
	void OnWordRight();

	void OnMoveToPosition( const TPosition& pos );

	void OnEndOfLine();
	void OnStartOfLine();
	void OnPageUp();
	void OnPageDown();
	void OnSlideUp();
	void OnSlideDown();
	void OnStartOfText();
	void OnEndOfText();

	void OnSelectWord();
	void OnSelectAll();

	// bookmarks

	void OnToggleBookmark();
	void OnNextBookmark();
	void OnPreviousBookmark();

	// search

	bool OnFind();
	bool OnFindNext();
	bool OnFindPrevious();
	void OnGotoLine( int line, int col );
	void OnGotoLine();

	// status

	bool IsReadOnly();
	bool IsModified();
	bool IsInsertMode();

	// check-pointin

	void SetCheckpoint();
	bool CheckpointModified();

	// accessors

	TPosition GetStartOfSelection();
	TPosition GetEndOfSelection();
	TPosition GetCursor();

	size_t    GetTotalRowCount();
	bool	  SelectionIsEmpty();
	bool      IsLastLine( const TPosition& pos );

	bool      IsUnixText() { return mIsUnixText; }

	void GetSelection( char** text, size_t& textLen );

	void SetStartOfSelection( const TPosition& pos );
	void SetEndOfSelection( const TPosition& pos );
	void SetCursor( const TPosition& pos );

	void AddView( wxTextEditorView* pView );
	void RemoveView( wxTextEditorView* pView );
	void SetActiveView( wxTextEditorView* pView );
	wxTextEditorView* GetActiveView();

	void SetRowsPerPage( size_t n );

	void AddPin( TPinBase* pPin );
	PinListT& GetPins(); 

	// returns NPOS, if non
	size_t FindFirstPinInRange( size_t fromRow, size_t tillRow );
	size_t FindNextPinFrom( size_t fromRow );
	size_t FindPreviousPinFrom( size_t fromRow );

	size_t GetPinNoAt( size_t row, int pinTypeCode );
	TPinBase* GetPinAt( size_t row, int pinTypeCode );
	void RemovePinAt( size_t row, int pinTypeCode );

	void AddChangeListener( TTextChangeListenerBase* pListener );
};

class TCursorTimer;
class wxTextEditorView;

class TPinPainterBase : public wxObject
{
public:
	int    mPinTypeCode;

public:
	TPinPainterBase( int pinTc ) : mPinTypeCode( pinTc ) {}
	TPinPainterBase() : mPinTypeCode( -1 ) {}

	inline int GetPinTypeCode() { return mPinTypeCode; }

	virtual void DrawPin( TPinBase* pPin, wxTextEditorView& view, wxDC& dc, 
						  const wxPoint& pos, const wxSize& dim ) = 0;
};

/*
 * a couple very common ping objects/painters
 */

#define BOOKMARK_PIN_TC  (OTHER_PINS_TC_START)
#define BRKPOINT_PIN_TC  (BOOKMARK_PIN_TC + 1)

class TBookmarkPainter : public TPinPainterBase
{
protected:
	wxBrush mBkBrush;

public:
	TBookmarkPainter();

	virtual void DrawPin( TPinBase* pPin, wxTextEditorView& view, wxDC& dc, 
						  const wxPoint& pos, const wxSize& dim );
};

class TBookmarkPin : public TPinBase
{
public:
	TBookmarkPin( size_t row )
		: TPinBase( BOOKMARK_PIN_TC, row )
	{}

	static int GetPinTypeCode() { return BOOKMARK_PIN_TC; }
};

class TBreakpointPainter : public TPinPainterBase
{
protected:
	wxBrush mBkBrush;

public:
	TBreakpointPainter();

	virtual void DrawPin( TPinBase* pPin, wxTextEditorView& view, wxDC& dc, 
						  const wxPoint& pos, const wxSize& dim );
};

class TBreakpointPin : public TPinBase
{
public:
	TBreakpointPin( size_t row )
		: TPinBase( BRKPOINT_PIN_TC, row )
	{}

	static int GetPinTypeCode() { return BRKPOINT_PIN_TC; }
};


#if defined( wxUSE_TEMPLATE_STL )
	typedef vector<TPinPainterBase*> PinPainterListT;
#else
	typedef TPinPainterBase* TPinPainterBasePtrT;
	typedef WXSTL_VECTOR_SHALLOW_COPY( TPinPainterBasePtrT ) PinPainterListT;
#endif

/*
 * Class displays graphical view of data contained in wxTextModel
 */

class wxTextEditorView : public wxScrolledWindow
{
protected:

	wxTextEditorModel*  mpModel;
	TPosition           mSelectionStart;
	TPosition           mSelectionEnd;
	TPosition           mCursorPos;

	TPosition           mLastViewStart;
	size_t              mLastRowsTotal;

	size_t              mRowsPerPage;
	size_t              mColsPerPage;

	static char*        mpLineBuffer;
	static size_t       mpLineBufferLen;

	bool                mFullRefreshPending;
	bool                mAdjustScrollPending;

	wxFont              mFont;

	bool                mScrollingOn; // default: TRUE
	bool                mCursorOn;   // default: TRUE;

	bool                mLTMode; // line-tracking mode
								 // (when the whole line is coloured,
	                             //  instead of showing blinking cursor position)

	wxColour            mLTColour; // fill-colour for LT-mode

	bool                mDragStarted;
	char*               mpDraggedText;

	bool                mOwnsModel;

	wxString            mFragment; // reused heap-buffer 
								   // for coloured fragments
	SourcePainter*      mpPainter;
	PinPainterListT     mPinPainters;
	TTextIterator       mCashedIter;

	static TCursorTimer* mpTimer;

public: /*** public properties ***/

	int                mLeftMargin;		// default: 20
	int                mRightMargin;	// default: 0
	int                mTopMargin;		// default: 0
	int                mBottomMargin;	// default: 0
	int                mMaxColumns;     // default: 500

	TPosition          mPagePos;

	// color-scheme properties

	wxColour           mNormalTextCol;
	wxColour           mIndentifierTextCol;
	wxColour           mReservedWordTextCol;
	wxColour           mCommentTextCol;

	wxColour           mNormalBkCol;
	wxColour           mSelectionFgCol;
	wxColour           mSelectionBkCol;

	wxBrush            mNormalBkBrush;
	wxBrush            mSelectedBkBrush;

	// accessed by timer

	TPosition           mCursorScrPos;
	wxSize              mCharDim;

protected:

	char* GetLineBuffer( size_t len );

	virtual void PaintDecorations( size_t fromRow, size_t tillRow, wxDC& dc, TTextIterator& iter );
	virtual void PaintRows( size_t fromRow, size_t tillRow, wxDC& dc );

	void ObtainFontProperties();

	bool IsActiveView();
	void SetTextDefaults();
	void RecalcPagingInfo();

	TPinPainterBase* FindPainterForPin( TPinBase& pin );

public:
	wxTextEditorView( wxWindow* parent, wxWindowID id = -1, 
					  wxTextEditorModel* pModel = NULL, 
					  int wndStyle = wxSUNKEN_BORDER,
					  bool ownsModel = TRUE );
	~wxTextEditorView();

	/*** setup methods ***/

	void SetModel( wxTextEditorModel* pModel );

	// sets custom syntax-higlighting implementation
	void SetSourcePainter( SourcePainter* pPainter );
	void AddPinPainter( TPinPainterBase* pPainter );

	void SetDefaultFont( const wxFont& font );
	wxFont& GetDefaultFont(); 

	wxSize GetCharacterSize() { return mCharDim; }

	size_t GetRowsPerPage() { return mRowsPerPage; }
	void SetRowsPerPage( size_t n );
	void SetMaxColumns( size_t n );

	void SetLineTrackingMode( bool on, const wxColour& col = wxColour(255,255,0) );

	void EnableCursor( bool enable );
	void EnableScrollbars( bool enable );

	void SetColours( const wxColour& normalBkCol,
					 const wxColour& selectedBkCol,
					 const wxColour& selectedTextCol );

	void SetHeighlightingColours( const wxColour& normalTextCol,
								  const wxColour& identifierTextCol,
								  const wxColour& reservedWordTextCol,
								  const wxColour& commentTextCol );

	void SetMargins( int top, int left, int bottom, int right );

	// notifications from editor-model:

	void OnModelChanged();
	void ScrollView( int rows, int cols );

	// accessors

	void Activate();
	void Deactivate();

	// event handlers

#if (( wxVERSION_NUMBER < 2100 ) || (( wxVERSION_NUMBER == 2100 ) && (wxBETA_NUMBER <= 4)))
	void OnScroll( wxScrollEvent& event );
#else
        void OnScroll( wxScrollWinEvent& event );
#endif
	void OnPaint ( wxPaintEvent& event );
	void OnSize  ( wxSizeEvent& event );
	void OnEraseBackground( wxEraseEvent& event );

	void OnLButtonDown( wxMouseEvent& event );
	void OnLButtonUp  ( wxMouseEvent& event );
	void OnMotion     ( wxMouseEvent& event );
	void OnDblClick   ( wxMouseEvent& event );

	void OnSetFocus( wxFocusEvent& event );
	void OnKillFocus( wxFocusEvent& event );

	// requests editor to keep cursor blinking, even when
	// the window has lost it's focus

	void HoldCursor( bool hold );

	// FOR NOW:: hard-coded key-bindings

	void OnChar( wxKeyEvent& event );
	void OnKeyDown( wxKeyEvent& event );

	// utilities

	virtual void SyncViewPortPosition();
	virtual void SyncScrollbars();
	virtual void PositionCursor();

	void TextPosToScreenPos( const TPosition& txtPos, TPosition& scrPos );
	void ScreenPosToTextPos( const TPosition& scrPos, TPosition& txtPos );
	void ScreenPosToPixels ( const TPosition& scrPos, int& x, int& y );
	void PixelsToScrPos    ( int x, int y, int& scrRow, int& scrCol );
	void PixelsToTextPos   ( int x, int y, TPosition& textPos );

	bool IsClipboardCmd( wxKeyEvent& key );

	TPosition GetPagePos() { return mPagePos; }

	DECLARE_EVENT_TABLE()
};

// TODO:: mutex class should be used to avoid race on updates

class TCursorTimer : public wxTimer
{
protected:
	wxTextEditorView* mpView;
	volatile bool mIsLocked;
	volatile bool mIsShown;
	volatile bool mStarted;
	wxBrush       mBrush;
	bool          mMissOneTick;

	int           mBlinkInterval;  // default: 500mills
protected:

	void DrawCursor();

public:

	TCursorTimer();

	virtual void Notify();

	void SetView( wxTextEditorView* pView );
	wxTextEditorView* GetView();
	void HideCursor( bool forceHide = FALSE );
	void ShowCursor( bool forceShow = FALSE );

	void SetIsShown( bool isShown );
	void Lock();
	void Unlock();
};

#endif // __TDEFS_G__
