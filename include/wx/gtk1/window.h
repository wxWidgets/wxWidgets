/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKWINDOWH__
#define __GTKWINDOWH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/event.h"
#include "wx/validate.h"
#include "wx/cursor.h"
#include "wx/font.h"
#include "wx/dc.h"
#include "wx/region.h"
#include "wx/dnd.h"

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxFrameNameStr;
extern wxList wxTopLevelWindows;

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxLayoutConstraints;
class wxSizer;

class wxWindow;
class wxCanvas;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxPanelNameStr;
extern const wxSize wxDefaultSize;
extern const wxPoint wxDefaultPosition;

//-----------------------------------------------------------------------------
// wxWindow
//-----------------------------------------------------------------------------

class wxWindow: public wxEvtHandler
{
  DECLARE_DYNAMIC_CLASS(wxWindow)

  public:
  
    wxWindow(void);
    wxWindow( wxWindow *parent, const wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = wxPanelNameStr );
    bool Create( wxWindow *parent, const wxWindowID id,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = 0, const wxString &name = wxPanelNameStr );
    virtual ~wxWindow(void);
    bool Close( const bool force = FALSE );
    virtual bool Destroy(void);
    virtual bool DestroyChildren(void);
    
    virtual void PrepareDC( wxDC &dc );
    
    virtual void SetSize( const int x, const int y, const int width, const int height, 
      const int sizeFlags = wxSIZE_AUTO );
    virtual void SetSize( const int width, const int height );
    virtual void Move( const int x, const int y );
    virtual void GetSize( int *width, int *height ) const;
    virtual void SetClientSize( int const width, int const height );
    virtual void GetClientSize( int *width, int *height ) const;
    virtual void GetPosition( int *x, int *y ) const;
    virtual void Centre( const int direction = wxHORIZONTAL );
    virtual void Fit(void);
    
    void OnSize( wxSizeEvent &event );
    
    virtual bool Show( const bool show );
    virtual void Enable( const bool enable );
    virtual void MakeModal( const bool modal );
    virtual bool IsEnabled(void) const { return m_isEnabled; };
    virtual void SetFocus(void);
    virtual bool OnClose(void);
    
    virtual void AddChild( wxWindow *child );
    wxList *GetChildren(void);
    virtual void RemoveChild( wxWindow *child );
    void SetReturnCode( int retCode );
    int GetReturnCode(void);
    wxWindow *GetParent(void);
    
    wxEvtHandler *GetEventHandler(void);
    void SetEventhandler( wxEvtHandler *handler );
    
    virtual wxValidator *GetValidator(void);
    virtual void SetValidator( wxValidator *validator );
    
    bool IsBeingDeleted(void);
    
    void SetId( wxWindowID id );
    wxWindowID GetId(void);
    
    void SetCursor( const wxCursor &cursor );
    
    virtual void Refresh( const bool eraseBackground = TRUE, const wxRect *rect = NULL );
    virtual void Clear(void);
    virtual bool IsExposed( const long x, const long y );
    virtual bool IsExposed( const long x, const long y, const long width, const long height );
    
    virtual wxColour GetBackgroundColour(void) const;
    virtual void SetBackgroundColour( const wxColour &colour );
    
    virtual void SetDefaultBackgroundColour( const wxColour& col )
      { m_defaultBackgroundColour = col; };
    virtual wxColour GetDefaultBackgroundColour(void) const
      { return m_defaultBackgroundColour; };
    virtual void SetDefaultForegroundColour( const wxColour& col )
      { m_defaultForegroundColour = col; };
    virtual wxColour GetDefaultForegroundColour(void) const
      { return m_defaultForegroundColour; };
    
    virtual void SetFont( const wxFont &font );
    virtual wxFont *GetFont(void);
    // For backward compatibility
    inline virtual void SetButtonFont(const wxFont& font) { SetFont(font); }
    inline virtual void SetLabelFont(const wxFont& font) { SetFont(font); }
    inline virtual wxFont *GetLabelFont(void) { return GetFont(); };
    inline virtual wxFont *GetButtonFont(void) { return GetFont(); };
    virtual void SetWindowStyleFlag( long flag );
    virtual long GetWindowStyleFlag(void) const;
    virtual void CaptureMouse(void);
    virtual void ReleaseMouse(void);
    virtual void SetTitle( const wxString &title );
    virtual wxString GetTitle(void) const;
    virtual void SetName( const wxString &name );
    virtual wxString GetName(void) const;
    virtual wxString GetLabel(void) const;
    
    void OnSysColourChanged( wxSysColourChangedEvent &WXUNUSED(event) ) {};
    
    virtual bool IsShown(void);
    virtual bool IsRetained(void);
    virtual wxWindow *FindWindow( const long id );
    virtual wxWindow *FindWindow( const wxString& name );
    void AllowDoubleClick( bool WXUNUSED(allow) ) {};
    void SetDoubleClick( bool WXUNUSED(allow) ) {};
    virtual void ClientToScreen( int *x, int *y );
    virtual void ScreenToClient( int *x, int *y );
    
    virtual bool Validate(void);
    virtual bool TransferDataToWindow(void);
    virtual bool TransferDataFromWindow(void);
    void OnInitDialog( wxInitDialogEvent &event );
    virtual void InitDialog(void);
    
    virtual void SetDropTarget( wxDropTarget *dropTarget );
    virtual wxDropTarget *GetDropTarget() const;
    
    virtual void SetScrollbar( const int orient, const int pos, const int thumbVisible,
      const int range, const bool refresh = TRUE );
    virtual void SetScrollPos( const int orient, const int pos, const bool refresh = TRUE );
    virtual int GetScrollPos( const int orient ) const;
    virtual int GetScrollThumb( const int orient ) const;
    virtual int GetScrollRange( const int orient ) const;
    virtual void ScrollWindow( const int dx, const int dy, const wxRect* rect = NULL );
    
  public:         // cannot get private going yet
    
    void PreCreation( wxWindow *parent, const wxWindowID id, const wxPoint &pos, 
      const wxSize &size, const long style, const wxString &name );
    void PostCreation(void);
    bool HasVMT(void);
    virtual void ImplementSetSize(void);
    virtual void ImplementSetPosition(void);
    void GetDrawingOffset( long *x, long *y );
    
    wxWindow       *m_parent;
    wxList          m_children;
    int             m_x,m_y;
    int             m_width,m_height;
    int             m_retCode;
    wxEvtHandler   *m_eventHandler;
    wxValidator    *m_windowValidator;
    wxDropTarget   *m_pDropTarget;   
    wxWindowID      m_windowId;
    wxCursor       *m_cursor;
    wxFont          m_font;
    wxColour        m_backgroundColour;
    wxColour        m_defaultBackgroundColour;
    wxColour        m_foregroundColour ;
    wxColour        m_defaultForegroundColour;
    wxRegion        m_updateRegion;
    long            m_windowStyle;
    bool            m_isShown;
    bool            m_isEnabled;
    wxString        m_windowName;
    long            m_drawingOffsetX,m_drawingOffsetY;
    
    GtkWidget      *m_widget;
    GtkWidget      *m_wxwindow;
    GtkAdjustment  *m_hAdjust,*m_vAdjust;
    float           m_oldHorizontalPos;
    float           m_oldVerticalPos;
    bool            m_needParent;
    bool            m_hasScrolling;
    bool            m_hasVMT;
    bool            m_sizeSet;
    
  public:  // Layout section
  
    wxLayoutConstraints * m_constraints;  
    wxList *              m_constraintsInvolvedIn;
    wxSizer *             m_windowSizer;  
    wxWindow *            m_sizerParent;  
    bool                  m_autoLayout;                    

    wxLayoutConstraints *GetConstraints(void) const;
    void SetConstraints( wxLayoutConstraints *constraints );
    void SetAutoLayout( const bool autoLayout );
    bool GetAutoLayout(void) const;
    bool Layout(void);
    void SetSizer( wxSizer *sizer );
    wxSizer *GetSizer(void) const;
    void SetSizerParent( wxWindow *win );
    wxWindow *GetSizerParent(void) const;
    void UnsetConstraints(wxLayoutConstraints *c);
    inline wxList *GetConstraintsInvolvedIn(void) const ;
    void AddConstraintReference(wxWindow *otherWin);
    void RemoveConstraintReference(wxWindow *otherWin);
    void DeleteRelatedConstraints(void);
    virtual void ResetConstraints(void);
    virtual void SetConstraintSizes(const bool recurse = TRUE);
    virtual bool LayoutPhase1(int *noChanges);
    virtual bool LayoutPhase2(int *noChanges);
    virtual bool DoPhase(const int);
    virtual void TransformSizerToActual(int *x, int *y) const ;
    virtual void SizerSetSize(const int x, const int y, const int w, const int h);
    virtual void SizerMove(const int x, const int y);
    virtual void SetSizeConstraint(const int x, const int y, const int w, const int h);
    virtual void MoveConstraint(const int x, const int y);
    virtual void GetSizeConstraint(int *w, int *h) const ;
    virtual void GetClientSizeConstraint(int *w, int *h) const ;
    virtual void GetPositionConstraint(int *x, int *y) const ;
  
  DECLARE_EVENT_TABLE()
};

#endif // __GTKWINDOWH__
