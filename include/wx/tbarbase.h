/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tbarbase.h
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

class WXDLLEXPORT wxToolBarBase;
class WXDLLEXPORT wxToolBarToolBase;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxToolBarNameStr;
WXDLLEXPORT_DATA(extern const wxSize) wxDefaultSize;
WXDLLEXPORT_DATA(extern const wxPoint) wxDefaultPosition;

enum wxToolBarToolStyle
{
    wxTOOL_STYLE_BUTTON    = 1,
    wxTOOL_STYLE_SEPARATOR = 2,
    wxTOOL_STYLE_CONTROL
};

// ----------------------------------------------------------------------------
// wxToolBarTool is a toolbar element.
//
// It has a unique id (except for the separators which always have id -1), the
// style (telling whether it is a normal button, separator or a control), the
// state (toggled or not, enabled or not) and short and long help strings. The
// default implementations use the short help string for the tooltip text which
// is popped up when the mouse pointer enters the tool and the long help string
// for the applications status bar.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarToolBase : public wxObject
{
public:
    // ctors & dtor
    // ------------

    wxToolBarToolBase(wxToolBarBase *tbar = (wxToolBarBase *)NULL,
                      int id = wxID_SEPARATOR,
                      const wxBitmap& bitmap1 = wxNullBitmap,
                      const wxBitmap& bitmap2 = wxNullBitmap,
                      bool toggle = FALSE,
                      wxObject *clientData = (wxObject *) NULL,
                      const wxString& shortHelpString = wxEmptyString,
                      const wxString& longHelpString = wxEmptyString)
        : m_shortHelpString(shortHelpString),
          m_longHelpString(longHelpString)
    {
        m_tbar = tbar;
        m_id = id;
        m_clientData = clientData;

        m_bitmap1 = bitmap1;
        m_bitmap2 = bitmap2;

        m_isToggle = toggle;
        m_enabled = TRUE;
        m_toggled = FALSE;

        m_toolStyle = id == wxID_SEPARATOR ? wxTOOL_STYLE_SEPARATOR
                                           : wxTOOL_STYLE_BUTTON;
    }

    wxToolBarToolBase(wxToolBarBase *tbar, wxControl *control)
    {
        m_tbar = tbar;
        m_control = control;
        m_id = control->GetId();

        m_isToggle = FALSE;
        m_enabled = TRUE;
        m_toggled = FALSE;

        m_toolStyle = wxTOOL_STYLE_CONTROL;
    }

    ~wxToolBarToolBase();

    // accessors
    // ---------

    // general
    int GetId() const { return m_id; }

    wxControl *GetControl() const
    {
        wxASSERT_MSG( IsControl(), _T("this toolbar tool is not a control") );

        return m_control;
    }

    wxToolBarBase *GetToolBar() const { return m_tbar; }

    // style
    int IsButton() const { return m_toolStyle == wxTOOL_STYLE_BUTTON; }
    int IsControl() const { return m_toolStyle == wxTOOL_STYLE_CONTROL; }
    int IsSeparator() const { return m_toolStyle == wxTOOL_STYLE_SEPARATOR; }
    int GetStyle() const { return m_toolStyle; }

    // state
    bool IsEnabled() const { return m_enabled; }
    bool IsToggled() const { return m_toggled; }
    bool CanBeToggled() const { return m_isToggle; }

    // attributes
    const wxBitmap& GetBitmap1() const { return m_bitmap1; }
    const wxBitmap& GetBitmap2() const { return m_bitmap2; }

    const wxBitmap& GetBitmap() const
        { return IsToggled() ? m_bitmap2 : m_bitmap1; }

    wxString GetShortHelp() const { return m_shortHelpString; }
    wxString GetLongHelp() const { return m_longHelpString; }

    wxObject *GetClientData() const
    {
        if ( m_toolStyle == wxTOOL_STYLE_CONTROL )
        {
            return (wxObject*)m_control->GetClientData();
        }
        else
        {
            return m_clientData;
        }
    }

    // modifiers: return TRUE if the state really changed
    bool Enable(bool enable);
    bool Toggle(bool toggle);
    bool SetToggle(bool toggle);
    bool SetShortHelp(const wxString& help);
    bool SetLongHelp(const wxString& help);

    void Toggle() { Toggle(!IsToggled()); }

    void SetBitmap1(const wxBitmap& bmp) { m_bitmap1 = bmp; }
    void SetBitmap2(const wxBitmap& bmp) { m_bitmap2 = bmp; }

    void SetClientData(wxObject *clientData)
    {
        if ( m_toolStyle == wxTOOL_STYLE_CONTROL )
        {
            m_control->SetClientData(clientData);
        }
        else
        {
            m_clientData = clientData;
        }
    }

    // add tool to/remove it from a toolbar
    virtual void Detach() { m_tbar = (wxToolBarBase *)NULL; }
    virtual void Attach(wxToolBarBase *tbar) { m_tbar = tbar; }

protected:
    wxToolBarBase *m_tbar;  // the toolbar to which we belong (may be NULL)

    int m_toolStyle; // see enum wxToolBarToolStyle
    int m_id;        // the tool id, wxID_SEPARATOR for separator

    // as controls have their own client data, no need to waste memory
    union
    {
        wxObject         *m_clientData;
        wxControl        *m_control;
    };

    // tool state
    bool m_toggled;
    bool m_isToggle;
    bool m_enabled;

    // normal and toggles bitmaps
    wxBitmap m_bitmap1;
    wxBitmap m_bitmap2;

    // short and long help strings
    wxString m_shortHelpString;
    wxString m_longHelpString;
};

// a list of toolbar tools
WX_DECLARE_EXPORTED_LIST(wxToolBarToolBase, wxToolBarToolsList);

// ----------------------------------------------------------------------------
// the base class for all toolbars
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarBase : public wxControl
{
public:
    wxToolBarBase();
    virtual ~wxToolBarBase();

    // toolbar construction
    // --------------------

    // the most commonly used version of AddTool()
    wxToolBarToolBase *AddTool(int id,
                               const wxBitmap& bitmap,
                               const wxString& shortHelpString = wxEmptyString,
                               const wxString& longHelpString = wxEmptyString)
    {
        return AddTool(id, bitmap, wxNullBitmap, FALSE, NULL,
                       shortHelpString, longHelpString);
    }

    // If pushedBitmap is NULL, a reversed version of bitmap is created and
    // used as the pushed/toggled image. If toggle is TRUE, the button toggles
    // between the two states.
    wxToolBarToolBase *AddTool(int id,
                               const wxBitmap& bitmap,
                               const wxBitmap& pushedBitmap = wxNullBitmap,
                               bool toggle = FALSE,
                               wxObject *clientData = NULL,
                               const wxString& shortHelpString = wxEmptyString,
                               const wxString& longHelpString = wxEmptyString)
    {
        return AddTool(id, bitmap, pushedBitmap, toggle,
                       -1, -1, clientData, shortHelpString, longHelpString);
    }

    // the old version of AddTool() kept for compatibility
    virtual wxToolBarToolBase *AddTool
                               (
                                   int id,
                                   const wxBitmap& bitmap,
                                   const wxBitmap& pushedBitmap,
                                   bool toggle,
                                   wxCoord xPos,
                                   wxCoord yPos = -1,
                                   wxObject *clientData = NULL,
                                   const wxString& helpString1 = wxEmptyString,
                                   const wxString& helpString2 = wxEmptyString
                               );

    // insert the new tool at the given position, if pos == GetToolsCount(), it
    // is equivalent to AddTool()
    virtual wxToolBarToolBase *InsertTool
                               (
                                    size_t pos,
                                    int id,
                                    const wxBitmap& bitmap,
                                    const wxBitmap& pushedBitmap = wxNullBitmap,
                                    bool toggle = FALSE,
                                    wxObject *clientData = NULL,
                                    const wxString& help1 = wxEmptyString,
                                    const wxString& help2 = wxEmptyString
                               );

    // add an arbitrary control to the toolbar, return TRUE if ok (notice that
    // the control will be deleted by the toolbar and that it will also adjust
    // its position/size)
    //
    // NB: the control should have toolbar as its parent
    virtual wxToolBarToolBase *AddControl(wxControl *control);
    virtual wxToolBarToolBase *InsertControl(size_t pos, wxControl *control);

    // add a separator to the toolbar
    virtual wxToolBarToolBase *AddSeparator();
    virtual wxToolBarToolBase *InsertSeparator(size_t pos);

    // remove the tool from the toolbar: the caller is responsible for actually
    // deleting the pointer
    virtual wxToolBarToolBase *RemoveTool(int id);

    // delete tool either by index or by position
    virtual bool DeleteToolByPos(size_t pos);
    virtual bool DeleteTool(int id);

    // delete all tools
    virtual void ClearTools();

    // must be called after all buttons have been created to finish toolbar
    // initialisation
    virtual bool Realize();

    // tools state
    // -----------

    virtual void EnableTool(int id, bool enable);
    virtual void ToggleTool(int id, bool toggle);

    // Set this to be togglable (or not)
    virtual void SetToggle(int id, bool toggle);

    // set/get tools client data (not for controls)
    virtual wxObject *GetToolClientData(int id) const;
    virtual void SetToolClientData(int id, wxObject *clientData);

    // return TRUE if the tool is toggled
    virtual bool GetToolState(int id) const;

    virtual bool GetToolEnabled(int id) const;

    virtual void SetToolShortHelp(int id, const wxString& helpString);
    virtual wxString GetToolShortHelp(int id) const;
    virtual void SetToolLongHelp(int id, const wxString& helpString);
    virtual wxString GetToolLongHelp(int id) const;

    // margins/packing/separation
    // --------------------------

    virtual void SetMargins(int x, int y);
    void SetMargins(const wxSize& size)
        { SetMargins((int) size.x, (int) size.y); }
    virtual void SetToolPacking(int packing)
        { m_toolPacking = packing; }
    virtual void SetToolSeparation(int separation)
        { m_toolSeparation = separation; }

    virtual wxSize GetToolMargins() { return GetToolMargins(); }
    virtual int GetToolPacking() { return m_toolPacking; }
    virtual int GetToolSeparation() { return m_toolSeparation; }

    // for compatibility
    wxSize GetMargins() const { return wxSize(m_xMargin, m_yMargin); }

    // toolbar geometry
    // ----------------

    // set the number of toolbar rows
    virtual void SetRows(int nRows);

    // the toolbar can wrap - limit the number of columns or rows it may take
    void SetMaxRowsCols(int rows, int cols)
        { m_maxRows = rows; m_maxCols = cols; }
    int GetMaxRows() const { return m_maxRows; }
    int GetMaxCols() const { return m_maxCols; }

    // get/set the size of the bitmaps used by the toolbar: should be called
    // before adding any tools to the toolbar
    virtual void SetToolBitmapSize(const wxSize& size)
        { m_defaultWidth = size.x; m_defaultHeight = size.y; };
    virtual wxSize GetToolBitmapSize() const
        { return wxSize(m_defaultWidth, m_defaultHeight); }

    // the button size in some implementations is bigger than the bitmap size:
    // get the total button size (by default the same as bitmap size)
    virtual wxSize GetToolSize() const
        { return GetToolBitmapSize(); } ;

    // returns a (non separator) tool containing the point (x, y) or NULL if
    // there is no tool at this point (corrdinates are client)
    virtual wxToolBarToolBase *FindToolForPosition(wxCoord x,
                                                   wxCoord y) const = 0;

    // event handlers
    // --------------

    // NB: these functions are deprecated, use EVT_TOOL_XXX() instead!

    // Only allow toggle if returns TRUE. Call when left button up.
    virtual bool OnLeftClick(int id, bool toggleDown);

    // Call when right button down.
    virtual void OnRightClick(int id, long x, long y);

    // Called when the mouse cursor enters a tool bitmap.
    // Argument is -1 if mouse is exiting the toolbar.
    virtual void OnMouseEnter(int id);

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
#endif // WXWIN_COMPATIBILITY

    // implementation only from now on
    // -------------------------------

    size_t GetToolsCount() const { return m_tools.GetCount(); }

    void OnIdle(wxIdleEvent& event);

    // Do the toolbar button updates (check for EVT_UPDATE_UI handlers)
    virtual void DoToolbarUpdates();

    // Don't want toolbars to accept the focus
    virtual bool AcceptsFocus() const { return FALSE; }

protected:
    // to implement in derived classes
    // -------------------------------

    // the tool is not yet inserted into m_tools list when this function is
    // called and will only be added to it if this function succeeds
    virtual bool DoInsertTool(size_t pos, wxToolBarToolBase *tool) = 0;

    // the tool is still in m_tools list when this function is called, it will
    // only be deleted from it if it succeeds
    virtual bool DoDeleteTool(size_t pos, wxToolBarToolBase *tool) = 0;

    // called when the tools enabled flag changes
    virtual void DoEnableTool(wxToolBarToolBase *tool, bool enable) = 0;

    // called when the tool is toggled
    virtual void DoToggleTool(wxToolBarToolBase *tool, bool toggle) = 0;

    // called when the tools "can be toggled" flag changes
    virtual void DoSetToggle(wxToolBarToolBase *tool, bool toggle) = 0;

    // the functions to create toolbar tools
    virtual wxToolBarToolBase *CreateTool(int id,
                                          const wxBitmap& bitmap1,
                                          const wxBitmap& bitmap2,
                                          bool toggle,
                                          wxObject *clientData,
                                          const wxString& shortHelpString,
                                          const wxString& longHelpString) = 0;
    virtual wxToolBarToolBase *CreateTool(wxControl *control) = 0;

    // helper functions
    // ----------------

    // find the tool by id
    wxToolBarToolBase *FindById(int id) const;

    // the list of all our tools
    wxToolBarToolsList m_tools;

    // the offset of the first tool
    int m_xMargin;
    int m_yMargin;

    // the maximum number of toolbar rows/columns
    int m_maxRows;
    int m_maxCols;

    // the tool packing and separation
    int m_toolPacking,
        m_toolSeparation;

    // the size of the toolbar bitmaps
    wxCoord m_defaultWidth, m_defaultHeight;

private:
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TBARBASE_H_

