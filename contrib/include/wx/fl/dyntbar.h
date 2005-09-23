/////////////////////////////////////////////////////////////////////////////
// Name:        dyntbar.h
// Purpose:     wxDynamicToolBar header
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     ??/10/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __DYNTBAR_G__
#define __DYNTBAR_G__

#include "wx/tbarbase.h"
#include "wx/dynarray.h"
#include "wx/fl/fldefs.h"

/*
Tool layout item.
*/

class WXDLLIMPEXP_FL wxToolLayoutItem : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxToolLayoutItem)

public:
    wxRect    mRect;
    bool      mIsSeparator;
};

class WXDLLIMPEXP_FL wxDynToolInfo;

typedef wxToolLayoutItem* wxToolLayoutItemPtrT;
typedef wxDynToolInfo*    wxDynToolInfoPtrT;

WXFL_DEFINE_ARRAY_PTR( wxToolLayoutItemPtrT, wxLayoutItemArrayT  );
WXFL_DEFINE_ARRAY_PTR( wxDynToolInfoPtrT,    wxDynToolInfoArrayT );

/*
This is a base class for layout algorithm implementations.
*/

class WXDLLIMPEXP_FL LayoutManagerBase
{
public:
        // Constructor.
    virtual void Layout( const wxSize&       parentDim,
                         wxSize&             resultingDim,
                         wxLayoutItemArrayT& items,
                         int                 horizGap,
                         int                 vertGap   ) = 0;

        // Destructor.
    virtual ~LayoutManagerBase() {}
};

/*
BagLayout lays out items in left-to-right order from
top to bottom.
*/

class WXDLLIMPEXP_FL BagLayout : public LayoutManagerBase
{
public:
        // Constructor.
    virtual void Layout( const wxSize&       parentDim, 
                         wxSize&             resultingDim,
                         wxLayoutItemArrayT& items,
                         int                 horizGap,
                         int                 vertGap   );
};

/*
This class holds dynamic toolbar item information.
*/

class WXDLLIMPEXP_FL wxDynToolInfo : public wxToolLayoutItem
{
    DECLARE_DYNAMIC_CLASS(wxDynToolInfo)

public:
    wxWindow* mpToolWnd;
    int       mIndex;
    wxSize    mRealSize;
};

// Layout orientations for tools

#define LO_HORIZONTAL    0
#define LO_VERTICAL      1
#define LO_FIT_TO_WINDOW 2

/*
wxDynamicToolBar manages containment and layout of tool windows.
*/

class WXDLLIMPEXP_FL wxDynamicToolBar : public wxToolBarBase
{
protected:
    friend class wxDynamicToolBarSerializer;

    wxDynToolInfoArrayT mTools;
    LayoutManagerBase*  mpLayoutMan;

protected:
        // Internal function for sizing tool windows.
    virtual void SizeToolWindows();

public: /* public properties */

    int                mSepartorSize; // default: 8
    int                mVertGap;      // default: 0
    int                mHorizGap;      // default: 0

public:
        // Default constructor.

    wxDynamicToolBar();

        // Constructor: see the documentation for wxToolBar for details.

    wxDynamicToolBar(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                 const long style = wxNO_BORDER, const int orientation = wxVERTICAL,
                 const int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr);

        // Destructor.

    ~wxDynamicToolBar(void);

        // Creation function: see the documentation for wxToolBar for details.

    bool Create(wxWindow *parent, const wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                const long style = wxNO_BORDER, const int orientation = wxVERTICAL, const int RowsOrColumns = 1, const wxString& name = wxToolBarNameStr);

        // Adds a tool. See the documentation for wxToolBar for details.

    virtual void AddTool( int toolIndex, 
                              wxWindow* pToolWindow,
                              const wxSize& size = wxDefaultSize );

        // Adds a tool. See the documentation for wxToolBar for details.

    virtual void AddTool( int toolIndex,
                              const wxString& imageFileName,
                              wxBitmapType imageFileType = wxBITMAP_TYPE_BMP,
                              const wxString& labelText = wxT(""), bool alignTextRight = false,
                              bool isFlat = true );
        // Adds a tool. See the documentation for wxToolBar for details.

    virtual void AddTool( int toolIndex, wxBitmap labelBmp,
                              const wxString& labelText = wxT(""), bool alignTextRight = false,
                              bool isFlat = true );

        // Unhide method from parent.

    virtual wxToolBarToolBase *AddTool (wxToolBarToolBase *tool)
                              { return wxToolBarBase::AddTool(tool); };

    // Method from wxToolBarBase (for compatibility), only
    // the first two arguments are valid.
    // See the documentation for wxToolBar for details.

    virtual wxToolBarToolBase *AddTool(const int toolIndex, const wxBitmap& bitmap, const wxBitmap& pushedBitmap = wxNullBitmap,
               const bool toggle = false, const long xPos = wxDefaultCoord, const long yPos = wxDefaultCoord, wxObject *clientData = NULL,
               const wxString& helpString1 = wxT(""), const wxString& helpString2 = wxT(""));

        // Adds a separator. See the documentation for wxToolBar for details.

    virtual void AddSeparator( wxWindow* pSepartorWnd );

        // Unhide method from parent.

    virtual wxToolBarToolBase *AddSeparator()
                              { return wxToolBarBase::AddSeparator(); };

        // Returns tool information for the given tool index.

    wxDynToolInfo* GetToolInfo( int toolIndex );

        // Removes the given tool. Misspelt in order not to clash with a similar function
        // in the base class.

    void RemveTool( int toolIndex );

        // Draws a separator. The default implementation draws a shaded line.

    virtual void DrawSeparator( wxDynToolInfo& info, wxDC& dc );

        // Performs layout. See definitions of orientation types.

    virtual bool Layout();

        // Returns the preferred dimension, taking the given dimension and a reference to the result.

    virtual void GetPreferredDim( const wxSize& givenDim, wxSize& prefDim );

        // Creates the default layout (BagLayout).

    virtual LayoutManagerBase* CreateDefaultLayout() { return new BagLayout(); }

        // Sets the layout for this toolbar.

    virtual void SetLayout( LayoutManagerBase* pLayout );

        // Enables or disables the given tool.

    virtual void EnableTool(int toolIndex, bool enable = true);

        // Responds to size events, calling Layout.

    void OnSize( wxSizeEvent& event );

        // Responds to paint events, drawing separators.

    void OnPaint( wxPaintEvent& event );

        // Responds to background erase events. Currently does nothing.

    void OnEraseBackground( wxEraseEvent& event );

        // Overriden from wxToolBarBase; does nothing.

    virtual bool Realize(void);

        // Finds a tool for the given position.

    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x,
                                                   wxCoord y) const;

        // Inserts a tool at the given position.

    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool);

        // Deletes a tool. The tool is still in m_tools list when this function is called, and it will
        // only be deleted from it if it succeeds.

    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool);

        // Called when the tools enabled flag changes.

    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable);

        // Called when the tool is toggled.

    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle);

        // Called when the tools 'can be toggled' flag changes.

    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle);

        // Creates a toolbar tool.

    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxString& label,
                                          const wxBitmap& bmpNormal,
                                          const wxBitmap& bmpDisabled,
                                          wxItemKind kind,
                                          wxObject *clientData,
                                          const wxString& shortHelp,
                                          const wxString& longHelp);

        // Creates a toolbar tool.

    virtual wxToolBarToolBase *CreateTool(wxControl *control);

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxDynamicToolBar)
};

#endif /* __DYNTBAR_G__ */

