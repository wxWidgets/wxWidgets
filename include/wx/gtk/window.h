/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
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
public:
  wxWindow();
  wxWindow( wxWindow *parent, wxWindowID id,
    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
    long style = 0, const wxString &name = wxPanelNameStr );
  bool Create( wxWindow *parent, wxWindowID id,
    const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
    long style = 0, const wxString &name = wxPanelNameStr );
  virtual ~wxWindow();
  bool Close( bool force = FALSE );
  virtual bool Destroy();
  virtual bool DestroyChildren();

  virtual void PrepareDC( wxDC &dc );

  virtual void SetSize( int x, int y, int width, int height,
    int sizeFlags = wxSIZE_AUTO );
  virtual void SetSize( int width, int height );
  virtual void Move( int x, int y );
  virtual void GetSize( int *width, int *height ) const;
  virtual void SetClientSize( int const width, int const height );
  virtual void GetClientSize( int *width, int *height ) const;
  virtual void GetPosition( int *x, int *y ) const;
  virtual void Centre( int direction = wxHORIZONTAL );
  virtual void Fit();

  void OnSize( wxSizeEvent &event );
  void OnIdle( wxIdleEvent& event );

  virtual bool Show( bool show );
  virtual void Enable( bool enable );
  virtual void MakeModal( bool modal );
  virtual bool IsEnabled() const { return m_isEnabled; };
  virtual void SetFocus();
  virtual bool OnClose();

  virtual void AddChild( wxWindow *child );
  wxList *GetChildren();
  virtual void RemoveChild( wxWindow *child );
  void SetReturnCode( int retCode );
  int GetReturnCode();
  wxWindow *GetParent();

  wxEvtHandler *GetEventHandler();
  void SetEventhandler( wxEvtHandler *handler );

  virtual wxValidator *GetValidator();
  virtual void SetValidator( wxValidator *validator );

  bool IsBeingDeleted();

  void SetId( wxWindowID id );
  wxWindowID GetId();

  void SetCursor( const wxCursor &cursor );

  virtual void Refresh( bool eraseBackground = TRUE, const wxRect *rect = NULL );
  virtual void Clear();
  virtual bool IsExposed( long x, long y );
  virtual bool IsExposed( long x, long y, long width, long height );

  virtual wxColour GetBackgroundColour() const;
  virtual void SetBackgroundColour( const wxColour &colour );

  virtual void SetDefaultBackgroundColour( const wxColour& col )
    { m_defaultBackgroundColour = col; };
  virtual wxColour GetDefaultBackgroundColour() const
    { return m_defaultBackgroundColour; };
  virtual void SetDefaultForegroundColour( const wxColour& col )
    { m_defaultForegroundColour = col; };
  virtual wxColour GetDefaultForegroundColour() const
    { return m_defaultForegroundColour; };

  virtual void SetFont( const wxFont &font );
  virtual wxFont *GetFont();
  // For backward compatibility
  inline virtual void SetButtonFont(const wxFont& font) { SetFont(font); }
  inline virtual void SetLabelFont(const wxFont& font) { SetFont(font); }
  inline virtual wxFont *GetLabelFont() { return GetFont(); };
  inline virtual wxFont *GetButtonFont() { return GetFont(); };
  virtual void SetWindowStyleFlag( long flag );
  virtual long GetWindowStyleFlag() const;
  virtual void CaptureMouse();
  virtual void ReleaseMouse();
  virtual void SetTitle( const wxString &title );
  virtual wxString GetTitle() const;
  virtual void SetName( const wxString &name );
  virtual wxString GetName() const;
  virtual wxString GetLabel() const;

  void OnSysColourChanged( wxSysColourChangedEvent &WXUNUSED(event) ) {};

  virtual bool IsShown() const;
  virtual bool IsRetained();
  virtual wxWindow *FindWindow( long id );
  virtual wxWindow *FindWindow( const wxString& name );
  void AllowDoubleClick( bool WXUNUSED(allow) ) {};
  void SetDoubleClick( bool WXUNUSED(allow) ) {};
  virtual void ClientToScreen( int *x, int *y );
  virtual void ScreenToClient( int *x, int *y );

  virtual bool Validate();
  virtual bool TransferDataToWindow();
  virtual bool TransferDataFromWindow();
  void OnInitDialog( wxInitDialogEvent &event );
  virtual void InitDialog();
  
  virtual bool PopupMenu( wxMenu *menu, int x, int y );

  virtual void SetDropTarget( wxDropTarget *dropTarget );
  virtual wxDropTarget *GetDropTarget() const;
  
private:
  virtual GtkWidget* GetConnectWidget(void);
  
public:
  virtual void SetScrollbar( int orient, int pos, int thumbVisible,
    int range, bool refresh = TRUE );
  virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE );
  virtual int GetScrollPos( int orient ) const;
  virtual int GetScrollThumb( int orient ) const;
  virtual int GetScrollRange( int orient ) const;
  virtual void ScrollWindow( int dx, int dy, const wxRect* rect = NULL );

  // return FALSE from here if the window doesn't want the focus
  virtual bool AcceptsFocus() const;

  // update the UI state (called from OnIdle)
  void UpdateWindowUI();

  
public:         // cannot get private going yet

  void PreCreation( wxWindow *parent, wxWindowID id, const wxPoint &pos,
    const wxSize &size, long style, const wxString &name );
  void PostCreation();
  bool HasVMT();
  virtual void ImplementSetSize();
  virtual void ImplementSetPosition();
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
  bool            m_resizing;

public:  // Layout section

  wxLayoutConstraints * m_constraints;
  wxList *              m_constraintsInvolvedIn;
  wxSizer *             m_windowSizer;
  wxWindow *            m_sizerParent;
  bool                  m_autoLayout;

  wxLayoutConstraints *GetConstraints() const;
  void SetConstraints( wxLayoutConstraints *constraints );
  void SetAutoLayout( bool autoLayout );
  bool GetAutoLayout() const;
  bool Layout();
  void SetSizer( wxSizer *sizer );
  wxSizer *GetSizer() const;
  void SetSizerParent( wxWindow *win );
  wxWindow *GetSizerParent() const;
  void UnsetConstraints(wxLayoutConstraints *c);
  inline wxList *GetConstraintsInvolvedIn() const ;
  void AddConstraintReference(wxWindow *otherWin);
  void RemoveConstraintReference(wxWindow *otherWin);
  void DeleteRelatedConstraints();
  virtual void ResetConstraints();
  virtual void SetConstraintSizes(bool recurse = TRUE);
  virtual bool LayoutPhase1(int *noChanges);
  virtual bool LayoutPhase2(int *noChanges);
  virtual bool DoPhase(int);
  virtual void TransformSizerToActual(int *x, int *y) const ;
  virtual void SizerSetSize(int x, int y, int w, int h);
  virtual void SizerMove(int x, int y);
  virtual void SetSizeConstraint(int x, int y, int w, int h);
  virtual void MoveConstraint(int x, int y);
  virtual void GetSizeConstraint(int *w, int *h) const ;
  virtual void GetClientSizeConstraint(int *w, int *h) const ;
  virtual void GetPositionConstraint(int *x, int *y) const ;

  DECLARE_DYNAMIC_CLASS(wxWindow)
  DECLARE_EVENT_TABLE()
};

#endif // __GTKWINDOWH__
