/////////////////////////////////////////////////////////////////////////////
// Name:        tbarbase.h
// Purpose:     Base class for toolbar classes
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TBARBASE_H_
#define _WX_TBARBASE_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "tbarbase.h"
#endif

#include "wx/defs.h"

#include "wx/bitmap.h"
#include "wx/list.h"
#include "wx/control.h"

class WXDLLEXPORT wxToolBar;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxToolBarNameStr;
WXDLLEXPORT_DATA(extern const wxSize) wxDefaultSize;
WXDLLEXPORT_DATA(extern const wxPoint) wxDefaultPosition;

enum
{
    wxTOOL_STYLE_BUTTON    = 1,
    wxTOOL_STYLE_SEPARATOR = 2,
    wxTOOL_STYLE_CONTROL
};

// ----------------------------------------------------------------------------
// wxToolBarTool is one button/separator/whatever in the toolbar
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarTool : public wxObject
{
public:
    // ctors & dtor
    // ------------

    wxToolBarTool() { }

#ifdef __WXGTK__
    wxToolBarTool(wxToolBar *owner,
                  int theIndex = 0,
                  const wxBitmap& bitmap1 = wxNullBitmap,
                  const wxBitmap& bitmap2 = wxNullBitmap,
                  bool toggle = FALSE,
                  wxObject *clientData = (wxObject *) NULL,
                  const wxString& shortHelpString = wxEmptyString,
                  const wxString& longHelpString = wxEmptyString,
                  GtkWidget *pixmap = (GtkWidget *) NULL );
#else // !GTK
    wxToolBarTool(int theIndex,
                  const wxBitmap& bitmap1 = wxNullBitmap,
                  const wxBitmap& bitmap2 = wxNullBitmap,
                  bool toggle = FALSE,
                  long xPos = -1,
                  long yPos = -1,
                  const wxString& shortHelpString = wxEmptyString,
                  const wxString& longHelpString = wxEmptyString);
#endif // GTK/!GTK

    wxToolBarTool(wxControl *control);

    ~wxToolBarTool();

    // accessors
    // ---------

    void SetSize( long w, long h ) { m_width = w; m_height = h; }
    long GetWidth() const { return m_width; }
    long GetHeight() const { return m_height; }

    wxControl *GetControl() const
    {
        wxASSERT_MSG( m_toolStyle == wxTOOL_STYLE_CONTROL,
                      _T("this toolbar tool is not a control") );

        return m_control;
    }

public:
    int                   m_toolStyle;
    int                   m_index;

    // as controls have their own client data, no need to waste memory
    union
    {
        wxObject         *m_clientData;
        wxControl        *m_control;
    };

    wxCoord               m_x;
    wxCoord               m_y;
    wxCoord               m_width;
    wxCoord               m_height;

    bool                  m_toggleState;
    bool                  m_isToggle;
    bool                  m_enabled;
    bool                  m_isMenuCommand;

    bool                  m_deleteSecondBitmap;
    wxBitmap              m_bitmap1;
    wxBitmap              m_bitmap2;

    wxString              m_shortHelpString;
    wxString              m_longHelpString;

#ifdef __WXGTK__
    wxToolBar            *m_owner;
    GtkWidget            *m_item;
    GtkWidget            *m_pixmap;
#endif // GTK

private:
    DECLARE_DYNAMIC_CLASS(wxToolBarTool)
};

// ----------------------------------------------------------------------------
// the base class for all toolbars
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarBase : public wxControl
{
public:
    wxToolBarBase();
    ~wxToolBarBase();

    // toolbar construction
    // --------------------

    // If pushedBitmap is NULL, a reversed version of bitmap is created and
    // used as the pushed/toggled image. If toggle is TRUE, the button toggles
    // between the two states.
    virtual wxToolBarTool *AddTool(int toolIndex,
                                   const wxBitmap& bitmap,
                                   const wxBitmap& pushedBitmap = wxNullBitmap,
                                   bool toggle = FALSE,
                                   wxCoord xPos = -1,
                                   wxCoord yPos = -1,
                                   wxObject *clientData = NULL,
                                   const wxString& helpString1 = wxEmptyString,
                                   const wxString& helpString2 = wxEmptyString);

    // add an arbitrary control to the toolbar at given index, return TRUE if
    // ok (notice that the control will be deleted by the toolbar and that it
    // will also adjust its position/size)
    //
    // NB: the control should have toolbar as its parent
    virtual bool AddControl(wxControl *control) { return FALSE; }

    virtual void AddSeparator();
    virtual void ClearTools();

    // must be called after all buttons have been created to finish toolbar
    // initialisation
    virtual bool Realize() = 0;

    // tools state
    // -----------

    virtual void EnableTool(int toolIndex, bool enable);

    // toggle is TRUE if toggled on
    virtual void ToggleTool(int toolIndex, bool toggle);

    // Set this to be togglable (or not)
    virtual void SetToggle(int toolIndex, bool toggle); 
    virtual wxObject *GetToolClientData(int index) const;

    virtual bool GetToolState(int toolIndex) const;
    virtual bool GetToolEnabled(int toolIndex) const;
    virtual wxToolBarTool *FindToolForPosition(long x, long y) const;

    virtual void SetToolShortHelp(int toolIndex, const wxString& helpString);
    virtual wxString GetToolShortHelp(int toolIndex) const;
    virtual void SetToolLongHelp(int toolIndex, const wxString& helpString);
    virtual wxString GetToolLongHelp(int toolIndex) const;

    // margins/packing/separation
    // --------------------------

    virtual void SetMargins(int x, int y);
    void SetMargins(const wxSize& size)
        { SetMargins((int) size.x, (int) size.y); }
    virtual void SetToolPacking(int packing);
    virtual void SetToolSeparation(int separation);

    virtual wxSize GetToolMargins() { return wxSize(m_xMargin, m_yMargin); }
    virtual int GetToolPacking() { return m_toolPacking; }
    virtual int GetToolSeparation() { return m_toolSeparation; }

    void SetMaxRowsCols(int rows, int cols)
        { m_maxRows = rows; m_maxCols = cols; }
    int GetMaxRows() const { return m_maxRows; }
    int GetMaxCols() const { return m_maxCols; }

    // tool(bar) size
    // -------------

    virtual void SetToolBitmapSize(const wxSize& size)
        { m_defaultWidth = size.x; m_defaultHeight = size.y; };
    virtual wxSize GetToolBitmapSize() const
        { return wxSize(m_defaultWidth, m_defaultHeight); }

    // After the toolbar has initialized, this is the size the tools take up
    virtual wxSize GetMaxSize() const;

    // The button size (in some implementations) is bigger than the bitmap size: this returns
    // the total button size.
    virtual wxSize GetToolSize() const
        { return wxSize(m_defaultWidth, m_defaultHeight); } ;

    // Handle wxToolBar events
    // -----------------------

    // NB: these functions are deprecated, use EVT_TOOL_XXX() instead!

    // Only allow toggle if returns TRUE. Call when left button up.
    virtual bool OnLeftClick(int toolIndex, bool toggleDown);

    // Call when right button down.
    virtual void OnRightClick(int toolIndex, long x, long y);

    // Called when the mouse cursor enters a tool bitmap.
    // Argument is -1 if mouse is exiting the toolbar.
    virtual void OnMouseEnter(int toolIndex);

    // more deprecated functions
    // -------------------------

#if WXWIN_COMPATIBILITY
    void SetDefaultSize(int w, int h) { SetDefaultSize(wxSize(w, h)); }
    long GetDefaultWidth() const { return m_defaultWidth; }
    long GetDefaultHeight() const { return m_defaultHeight; }
    int GetDefaultButtonWidth() const { return (int) GetDefaultButtonSize().x; };
    int GetDefaultButtonHeight() const { return (int) GetDefaultButtonSize().y; };
    virtual void SetDefaultSize(const wxSize& size) { SetToolBitmapSize(size); }
    virtual wxSize GetDefaultSize() const { return GetToolBitmapSize(); }
    virtual wxSize GetDefaultButtonSize() const { return GetToolSize(); }
    void GetMaxSize ( long * width, long * height ) const
    { wxSize maxSize(GetMaxSize()); *width = maxSize.x; *height = maxSize.y; }
#endif // WXWIN_COMPATIBILITY

    // implementation only from now on
    // -------------------------------

    wxList& GetTools() const { return (wxList&) m_tools; }

    // Lay the tools out
    virtual void LayoutTools();

    // Add all the buttons: required for Win95.
    virtual bool CreateTools() { return TRUE; }

    void Command(wxCommandEvent& event);

    // SCROLLING: this has to be copied from wxScrolledWindow since wxToolBarBase
    // inherits from wxControl. This could have been put into wxToolBarSimple,
    // but we might want any derived toolbar class to be scrollable.

    // Number of pixels per user unit (0 or -1 for no scrollbar)
    // Length of virtual canvas in user units
    virtual void SetScrollbars(int horizontal, int vertical,
            int x_length, int y_length,
            int x_pos = 0, int y_pos = 0);

    // Physically scroll the window
    virtual void Scroll(int x_pos, int y_pos);
    virtual void GetScrollPixelsPerUnit(int *x_unit, int *y_unit) const;
    virtual void EnableScrolling(bool x_scrolling, bool y_scrolling);
    virtual void AdjustScrollbars();

    // Prepare the DC by translating it according to the current scroll position
    virtual void PrepareDC(wxDC& dc);

    int GetScrollPageSize(int orient) const ;
    void SetScrollPageSize(int orient, int pageSize);

    // Get the view start
    virtual void ViewStart(int *x, int *y) const;

    // Actual size in pixels when scrolling is taken into account
    virtual void GetVirtualSize(int *x, int *y) const;

    // Do the toolbar button updates (check for EVT_UPDATE_UI handlers)
    virtual void DoToolbarUpdates();

    // event handlers
    void OnScroll(wxScrollEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnIdle(wxIdleEvent& event);

protected:
    wxList                m_tools;

    int                   m_maxRows;
    int                   m_maxCols;
    long                  m_maxWidth,
                          m_maxHeight;

    int                   m_currentTool; // Tool where mouse currently is
    int                   m_pressedTool; // Tool where mouse pressed

    int                   m_xMargin;
    int                   m_yMargin;
    int                   m_toolPacking;
    int                   m_toolSeparation;

    wxCoord               m_defaultWidth;
    wxCoord               m_defaultHeight;

public:
    ////////////////////////////////////////////////////////////////////////
    //// IMPLEMENTATION

    // Calculate scroll increment
    virtual int CalcScrollInc(wxScrollEvent& event);

    ////////////////////////////////////////////////////////////////////////
    //// PROTECTED DATA
protected:
    int                   m_xScrollPixelsPerLine;
    int                   m_yScrollPixelsPerLine;
    bool                  m_xScrollingEnabled;
    bool                  m_yScrollingEnabled;
    int                   m_xScrollPosition;
    int                   m_yScrollPosition;
    bool                  m_calcScrolledOffset; // If TRUE, wxCanvasDC uses scrolled offsets
    int                   m_xScrollLines;
    int                   m_yScrollLines;
    int                   m_xScrollLinesPerPage;
    int                   m_yScrollLinesPerPage;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_ABSTRACT_CLASS(wxToolBarBase)
};

#endif
    // _WX_TBARBASE_H_

