/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindow class
// Author:      David Webster
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#ifdef __GNUG__
#pragma interface "window.h"
#endif

#define INCL_DOS
#define INCL_PM
#include <os2.h>

#define wxKEY_SHIFT     1
#define wxKEY_CTRL      2

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxButton;

#if wxUSE_DRAG_AND_DROP
class WXDLLEXPORT wxDropTarget;
#endif

#if wxUSE_WX_RESOURCES
class WXDLLEXPORT wxResourceTable;
class WXDLLEXPORT wxItemResource;
#endif

WXDLLEXPORT_DATA(extern const char*) wxPanelNameStr;

WXDLLEXPORT_DATA(extern const wxSize) wxDefaultSize;
WXDLLEXPORT_DATA(extern const wxPoint) wxDefaultPosition;

class WXDLLEXPORT wxWindow: public wxWindowBase
{
    DECLARE_ABSTRACT_CLASS(wxWindow)

    friend class wxDC;
    friend class wxPaintDC;

public:
    wxWindow() { Init(); };
    inline wxWindow( wxWindow*       parent
                    ,wxWindowID      id
                    ,const wxPoint&  pos = wxDefaultPosition
                    ,const wxSize&   size = wxDefaultSize
                    ,long            style = 0
                    ,const wxString& name = wxPanelNameStr
                   )
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindow();

    bool Create( wxWindow*       parent
                ,wxWindowID      id
                ,const wxPoint&  pos = wxDefaultPosition
                ,const wxSize&   size = wxDefaultSize
                ,long            style = 0
                ,const wxString& name = wxPanelNameStr
               );

// ---------------------------------------------------------------------------
// wxWindowBase pure virtual implementations
// ---------------------------------------------------------------------------

    // Z order
    virtual void Raise();
    virtual void Lower();
    // Set the focus to this window
    virtual void SetFocus();
    // Warp the pointer the given position
    virtual void WarpPointer(int x_pos, int y_pos) ;
    // Capture/release mouse
    virtual void CaptureMouse();
    virtual void ReleaseMouse();
    // Send the window a refresh event
    virtual void Refresh(bool eraseBack = TRUE, const wxRect *rect = NULL);
    // Clear the window
    virtual void Clear();
    // Set window's font
    virtual bool SetFont(const wxFont& f);
    // Get character size
    virtual int  GetCharHeight() const;
    virtual int  GetCharWidth() const;
    virtual void GetTextExtent( const wxString& string
                               ,int*            x
                               ,int*            y
                               ,int*            descent = NULL
                               ,int*            externalLeading = NULL
                               ,const wxFont*   theFont = NULL
                              ) const;

    // Configure the window's scrollbar
    virtual void SetScrollbar( int  orient
                              ,int  pos
                              ,int  thumbVisible
                              ,int  range
                              ,bool refresh = TRUE
                             );
    virtual void SetScrollPos( int  orient
                              ,int  pos
                              ,bool refresh = TRUE
                             );
    virtual int  GetScrollPos(int orient) const;
    virtual int  GetScrollRange(int orient) const;
    virtual int  GetScrollThumb(int orient) const;
    virtual void ScrollWindow( int           dx
                              ,int           dy
                              ,const wxRect* rect = NULL
                             );
#if wxUSE_DRAG_AND_DROP
    void         SetDropTarget(wxDropTarget *pDropTarget);
#endif
    // coordinates translation
    virtual void DoClientToScreen( int *x, int *y ) const;
    virtual void DoScreenToClient( int *x, int *y ) const;

    // retrieve the position/size of the window
    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetClientSize( int *width, int *height ) const;

    // this is the virtual function to be overriden in any derived class which
    // wants to change how SetSize() or Move() works - it is called by all
    // versions of these functions in the base class
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    // same as DoSetSize() for the client size
    virtual void DoSetClientSize(int width, int height);
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );
    virtual WXWidget GetHandle() const;

// ---------------------------------------------------------------------------
// wxWindowBase virtual implementations that need to be overriden
// ---------------------------------------------------------------------------

    // Fit the window around the items
    virtual void            Fit();
    // Show or hide the window
    virtual bool            Show(bool show); // check if base implementation is OK
    // Enable or disable the window
    virtual bool            Enable(bool enable); // check if base implementation is OK
    // Set the cursor
    virtual bool            SetCursor(const wxCursor& cursor); // check if base implementation is OK

// ---------------------------------------------------------------------------
// additional functions
// ---------------------------------------------------------------------------

    // Dialog support: override these and call
    // base class members to add functionality
    // that can't be done using validators.
    void SetClientSize( int width, int height )
        { DoSetClientSize(width, height); }

    void SetClientSize( const wxSize& size )
        { DoSetClientSize(size.x, size.y); }

    void SetClientSize(const wxRect& rect)
        { SetClientSize( rect.width, rect.height ); }

    // Validate controls. If returns FALSE,
    // validation failed: don't quit
    virtual bool Validate();

    // Return code for dialogs
    inline void SetReturnCode(int retCode);
    inline int GetReturnCode();


    // Get the window with the focus
    static wxWindow *FindFocus();

    // Accept files for dragging
    virtual void DragAcceptFiles(bool accept);

    // Most windows have the concept of a label; for frames, this is the
    // title; for items, this is the label or button text.
    inline virtual wxString GetLabel() const { return GetTitle(); }

#if wxUSE_CARET && WXWIN_COMPATIBILITY
    // Caret manipulation
    virtual void CreateCaret(int w, int h);
    virtual void CreateCaret(const wxBitmap *bitmap);
    virtual void DestroyCaret();
    virtual void ShowCaret(bool show);
    virtual void SetCaretPos(int x, int y);
    virtual void GetCaretPos(int *x, int *y) const;
#endif

    // Handle a control command
    virtual void OnCommand(wxWindow& win, wxCommandEvent& event);

    // Get the default button, if there is one
    inline virtual wxButton *GetDefaultItem() const;
    inline virtual void SetDefaultItem(wxButton *but);

    // Override to define new behaviour for default action (e.g. double clicking
    // on a listbox)
    virtual void OnDefaultAction(wxControl *initiatingItem);

    // EventHandlers
    void OnEraseBackground(wxEraseEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnIdle(wxIdleEvent& event);


public:
// ---------------------------------------------------------------------------
// IMPLEMENTATION
// ---------------------------------------------------------------------------

    // For implementation purposes - sometimes decorations make the client area
    // smaller
    virtual wxPoint GetClientAreaOrigin() const;

    // Makes an adjustment to the window position (for example, a frame that has
    // a toolbar that it manages itself).
    virtual void AdjustForParentClientOrigin(int& x, int& y, int sizeFlags);

    // Executes the default message
    virtual long Default();

/* TODO: you may need something like this
  // Determine whether 3D effects are wanted
  virtual WXDWORD Determine3DEffects(WXDWORD defaultBorderStyle, bool *want3D);
*/

    // Constraint implementation
    void UnsetConstraints(wxLayoutConstraints *c);
    // Back-pointer to other windows we're involved with, so if we delete
    // this window, we must delete any constraints we're involved with.

    wxObject *GetChild(int number) const ;

    // Transfers data to any child controls
    void OnInitDialog(wxInitDialogEvent& event);

protected:

    // Caret data
    int                   m_caretWidth;
    int                   m_caretHeight;
    bool                  m_caretEnabled;
    bool                  m_caretShown;

    wxButton *            m_defaultItem;

public:
    int                   m_returnCode;
    bool                  m_isBeingDeleted;
    bool                  m_isShown;
    bool                  m_winCaptured;
    bool                  m_mouseInWindow;
    bool                  m_backgroundTransparent;
    // handles
    WXHWND                m_hWnd;
    WXHMENU               m_hMenu; // Menu, if any

    // the size of one page for scrolling
    int                   m_xThumbSize;
    int                   m_yThumbSize;
    long                  m_lDlgCode;

DECLARE_EVENT_TABLE()
private:
    void   Init();
    void   PMDetachWindowMenu();
    WXHWND GetHwnd() const { return m_hWnd; }
    void   SetHwnd(WXHWND hWnd) { m_hWnd = hWnd; }
};

////////////////////////////////////////////////////////////////////////
//// INLINES

inline wxButton *wxWindow::GetDefaultItem() const { return m_defaultItem; }
inline void wxWindow::SetDefaultItem(wxButton *but) { m_defaultItem = but; }
inline void wxWindow::SetReturnCode(int retCode) { m_returnCode = retCode; }
inline int wxWindow::GetReturnCode() { return m_returnCode; }

// Get the active window.
wxWindow* WXDLLEXPORT wxGetActiveWindow();

#endif
    // _WX_WINDOW_H_
