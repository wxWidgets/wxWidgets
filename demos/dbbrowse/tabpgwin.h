/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by: 19990908 : mj
//              19990909 :
//              - rename to tabpgwin
//              - restruction of Variable declaration
//              - to prevent Warnings under MingGW32
// Modified by: 19990909 : mj
//              - mNoVertScroll TRUE = no / FALSE = Original Code
//                the Original Code Paints a Vertical Scroll in wxPagedWindow
//                which is not needed in this Version. Use TRUE for this.
// Created:     07/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CONTROLAREA_G__
#define __CONTROLAREA_G__

#ifdef __GNUG__
#pragma interface "controlarea.h"
#endif

#include "wx/defs.h"
#include "wx/window.h"
#include "wx/string.h"

#define WXCONTROLAREA_VERSION      1.0

// layout types for title bars of the tabs
// (are selected up by evaluating the available free space )

class twTabInfo; // forward decl.

#define wxTITLE_IMG_AND_TEXT 0
#define wxTITLE_IMG_ONLY     1
#define wxTITLE_BORDER_ONLY  2

/*
 * class manages and decorates contained "tab"-windows.
 * Draws decorations similar to those in "Project Workplace"
 * of Microsoft Developer Studio 4.xx
 */

class wxTabbedWindow : public wxPanel
{
 DECLARE_DYNAMIC_CLASS( wxTabbedWindow )

public:

 friend class wxTabbedWindowSerializer;

 wxList mTabs;
 void HideInactiveTabs( bool andRepaint );

 // overrride,to provide different font for tab-labels

 virtual wxFont GetLabelingFont();

 // FOR NOW:: scrollbars are actually related to wxPagedWindow

 wxScrollBar* mpTabScroll;
 wxScrollBar* mpHorizScroll;
 wxScrollBar* mpVertScroll;

public:

 // public properties (invoke ReclaclLayout(TRUE) to apply changes)

 int    mVertGap;         // default: 3
 int    mHorizGap;        // default: 5
 int    mTitleVertGap;    // default: 3
 int    mTitleHorizGap;   // default: 4
 int    mImageTextGap;    // default: 2
 int    mFirstTitleGap;   // default: 11
 int    mBorderOnlyWidth; // default: 8

 wxPen  mWhitePen;        // default: RGB(255,255,255)
 wxPen  mGrayPen;         // default: RGB(192,192,192)
 wxPen  mDarkPen;         // default: RGB(128,128,128)
 wxPen  mBlackPen;        // default: RGB(  0,  0,  0)

 int    mActiveTab;
 int    mTitleHeight;
 int    mLayoutType;

 // notifications (can be handled by derivatives)

 virtual void OnTabAdded( twTabInfo* pInfo ) {}

 virtual void SizeTabs(int x,int y, int width, int height, bool repant);

public:
 wxTabbedWindow();
 virtual ~wxTabbedWindow();

 // tabs can be also added when the window is
 // already displayed - "on the fly"

 virtual void AddTab( wxWindow* pContent,           // contained window
                      wxString  tabText,            // tab label
                      wxString  imageFileName = "", // if "", only text label is displayed
                      long      imageType     = wxBITMAP_TYPE_BMP );

 // NOTE:: if this AddTab(..) overload is called, the
 //        image bitmap will not be serialized (if performed),
 //        use the above method instead, so that images could
 //        be restored using the given file names

 virtual void AddTab( wxWindow* pContent,
                      wxString  tabText,
                      wxBitmap* pImage = NULL );

 virtual void RemoveTab( int tabNo );

 /* misc accessors */

 virtual int GetTabCount();
 virtual wxWindow* GetTab( int tabNo );
 virtual wxWindow* GetActiveTab();
 virtual void SetActiveTab( int tabNo );

 void DrawShadedRect( int x, int y, int width, int height,
                                          wxPen& upperPen, wxPen& lowerPen, wxDC& dc );

 virtual void DrawDecorations( wxDC& dc );

 // return -1, if non of the title bars was hitted,
 // otherwise the index of the hitted tab title bar

 virtual int HitTest( const wxPoint& pos );

 // should be invoked to redisplay window with changed properties

 virtual void RecalcLayout( bool andRepaint = TRUE );

 // event handlers

 void OnPaint( wxPaintEvent& event );
 void OnSize ( wxSizeEvent& event );

 void OnBkErase( wxEraseEvent& event );
 void OnLButtonDown( wxMouseEvent& event );

 DECLARE_EVENT_TABLE()
};

/*
 * class manages and decorates contained "sheets" (or pages).
 * Draws decorations similar to those in "Output window"
 * of Microsoft Developer Studio 4.xx
 */

class wxPagedWindow : public wxTabbedWindow
{
 DECLARE_DYNAMIC_CLASS( wxPagedWindow )
 // the protected: public: changes prevents Warnings in gcc
protected:
 bool         mScrollEventInProgress;
public:
 int          mTabTrianGap;
 wxBrush      mWhiteBrush;
 wxBrush      mGrayBrush;
 int          mCurentRowOfs;
 int          mAdjustableTitleRowLen; // setup by dragging mini-sash
                                      // with the mosue pointer
protected:
 // drag&drop state variables
 bool         mIsDragged;
 int          mDagOrigin;
 bool         mCursorChanged;
 wxCursor     mResizeCursor;
 wxCursor     mNormalCursor;
 int          mOriginalTitleRowLen;

public:
 int          mTitleRowStart;
 int          mResizeNailGap;
 int          mTitleRowLen;               // actual title row length
 int          mNoVertScroll;              // No Vertical Scroll  TRUE/FALSE

 void DrawPaperBar( twTabInfo& tab, int x, int y,
                                    wxBrush& brush, wxPen& pen, wxDC& dc );

 int GetWholeTabRowLen();

 // adjusts scorllbars to fit around tabs

 virtual void OnTabAdded( twTabInfo* pInfo );

 // sets smaller font for page-labels

 virtual wxFont GetLabelingFont();

public:


public:
 wxPagedWindow();
 ~wxPagedWindow();

 // NOTE:: use public methods of the base class
 //        to add "pages" to this window

 /* misc accessors */

 // below two methods should be called after
 // the tabs were added (AddTab(..)). Set up
 // these scrollbars to match the needs of the
 // tabs added into this area

 wxScrollBar& GetVerticalScrollBar();
 wxScrollBar& GetHorizontalScrollBar();

 virtual void DrawDecorations( wxDC& dc );

 // return -1, if non of the title bars was hitted,
 // otherwise the index of the hitted tab title bar

 virtual int HitTest( const wxPoint& pos );

 virtual void RecalcLayout( bool andRepaint = TRUE );

 // event handlers

 void OnPaint( wxPaintEvent& event );
 void OnSize ( wxSizeEvent& event );
 void OnLButtonDown( wxMouseEvent& event );
 void OnLButtonUp  ( wxMouseEvent& event );
 void OnMouseMove  ( wxMouseEvent& event );
 void OnScroll     ( wxScrollEvent& event );

 DECLARE_EVENT_TABLE()
};

// helper structure of wxTabbedWindow

class twTabInfo : public wxObject
{
 DECLARE_DYNAMIC_CLASS( twTabInfo )
public:
 twTabInfo();
 ~twTabInfo();

 int ImgWidth();
 int ImgHeight();
 int ImageToTxtGap( int prefGap );

 bool HasImg();
 wxBitmap& GetImg();
 // bool HasText();
 unsigned int HasText();
 wxString& GetText();
 wxWindow& GetContent();

public:
 wxWindow* mpContent;
 wxBitmap  mBitMap;

 wxString  mText;
 wxSize    mDims;

 // used for serialization
 wxString  mImageFile;
 long      mImageType;

};

#endif
