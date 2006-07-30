/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     ??/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNTBAR_G__
#define __DYNTBAR_G__

#include "wx/tbarbase.h"
#include "wx/dynarray.h"

// layout item

class wxToolLayoutItem : public wxObject
{
public:
	wxRect    mRect;
	bool      mIsSeparator;
};

class wxDynToolInfo;

typedef wxToolLayoutItem* wxToolLayoutItemPtrT;
typedef wxDynToolInfo*    wxDynToolInfoPtrT;


WX_DEFINE_ARRAY( wxToolLayoutItemPtrT, wxLayoutItemArrayT  );
WX_DEFINE_ARRAY( wxDynToolInfoPtrT,    wxDynToolInfoArrayT );

// base class for layouting algorithm implementations

class LayoutManagerBase
{
public:
	virtual void Layout( const wxSize&       parentDim, 
						 wxSize&	         resultingDim,
						 wxLayoutItemArrayT& items,
						 int		         horizGap,
 		                 int		         vertGap   ) = 0;

	virtual ~LayoutManagerBase() {}
};

// layouts items in left-to-right order from
// top towards bottom

class BagLayout : public LayoutManagerBase
{
public:
	virtual void Layout( const wxSize&       parentDim, 
						 wxSize&	         resultingDim,
						 wxLayoutItemArrayT& items,
						 int		         horizGap,
 		                 int		         vertGap   );
};

class wxDynToolInfo : public wxToolLayoutItem
{
	DECLARE_DYNAMIC_CLASS(wxDynToolInfo)

public:
	wxWindow* mpToolWnd;
	int       mIndex;
	wxSize    mRealSize;
};

// layouting orientations for tools

#define LO_HORIZONTAL    0
#define LO_VERTICAL      1
#define LO_FIT_TO_WINDOW 2

// class manages containment and layouting of tool-windows

class wxDynamicToolBar : public wxToolBarBase
{
	DECLARE_DYNAMIC_CLASS(wxDynamicToolBar)
protected:

	friend class wxDynamicToolBarSerializer;

	wxDynToolInfoArrayT mTools;
	LayoutManagerBase*  mpLayoutMan;

protected:
	virtual void SizeToolWindows();

public: /* public properties */

	int                mSepartorSize; // default: 8
	int                mVertGap;      // default: 0
	int                mHorizGap;	  // default: 0
	
public:
	wxDynamicToolBar();

	wxDynamicToolBar(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
	             const long style = wxNO_BORDER, const int orientation = wxVERTICAL,
		         const int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr);

	~wxDynamicToolBar(void);

	bool Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
				const long style = wxNO_BORDER, const int orientation = wxVERTICAL, const int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr);

	// overridables

	virtual void AddTool( int toolIndex, 
						  wxWindow* pToolWindow, 
						  const wxSize& size = wxDefaultSize );

	virtual void AddTool( int toolIndex, 
						  const wxString& imageFileName,
						  int imageFileType = wxBITMAP_TYPE_BMP,
						  const wxString& labelText = "", bool alignTextRight = FALSE,
						  bool isFlat = TRUE );
	virtual void AddTool( int toolIndex, wxBitmap labelBmp,
						  const wxString& labelText = "", bool alignTextRight = FALSE,
						  bool isFlat = TRUE );

	// method from wxToolBarBase (for compatibility), only
	// first two arguments are valid

  virtual wxToolBarTool *AddTool(const int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
               const bool toggle = FALSE, const long xPos = -1, const long yPos = -1, wxObject *clientData = NULL,
               const wxString& helpString1 = "", const wxString& helpString2 = "");

	virtual void AddSeparator( wxWindow* pSepartorWnd = NULL );

	wxDynToolInfo* GetToolInfo( int toolIndex );

	void RemveTool( int toolIndex );

	// the default implementation draws shaded line
	virtual void DrawSeparator( wxDynToolInfo& info, wxDC& dc );

	// see definitions of orientation types
	virtual bool Layout();

	virtual void GetPreferredDim( const wxSize& givenDim, wxSize& prefDim );

	virtual LayoutManagerBase* CreateDefaulLayout() { return new BagLayout(); }

	virtual void SetLayout( LayoutManagerBase* pLayout );

	virtual void EnableTool(const int toolIndex, const bool enable = TRUE);

	// event handlers

	void OnSize( wxSizeEvent& event );
	void OnPaint( wxPaintEvent& event );
	void OnEraseBackground( wxEraseEvent& event );

	// overriden from wxToolBarBase

	virtual bool Realize(void);

	DECLARE_EVENT_TABLE()
};

#endif
