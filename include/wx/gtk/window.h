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
#include "wx/accel.h"

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
class wxResourceTable;
class wxItemResource;

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
  inline wxWindow(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxPanelNameStr)
  {
      Create(parent, id, pos, size, style, name);
  }
  bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           long style = 0,
           const wxString& name = wxPanelNameStr);
  virtual ~wxWindow();

  virtual bool LoadFromResource( wxWindow *parent, const wxString& resourceName, 
                                 const wxResourceTable *table = (const wxResourceTable *) NULL);
  virtual wxControl *CreateItem( const wxItemResource *childResource, 
                                 const wxResourceTable *table = (const wxResourceTable *) NULL);

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
  inline void Center(int direction = wxHORIZONTAL) { Centre(direction); }
  virtual void Fit();

  virtual void SetSizeHints( int minW, int minH, int maxW = -1, int maxH = -1, int incW = -1, int incH = -1 );

  void OnSize( wxSizeEvent &event );
  void OnIdle( wxIdleEvent& event );

  virtual bool Show( bool show );
  virtual void Enable( bool enable );
  virtual void MakeModal( bool modal );
  virtual bool IsEnabled() const { return m_isEnabled; }
  inline bool Enabled(void) const { return IsEnabled(); }
  virtual void SetFocus();
  virtual bool OnClose();

  virtual void AddChild( wxWindow *child );
  wxList *GetChildren();
  virtual void RemoveChild( wxWindow *child );
  void SetReturnCode( int retCode );
  int GetReturnCode();
  wxWindow *GetParent() const
    { return m_parent; }
  wxWindow *GetGrandParent(void) const
    { return (m_parent ? m_parent->m_parent : (wxWindow*)NULL); }
  void SetParent( wxWindow *p )
    { m_parent = p; }

  wxEvtHandler *GetEventHandler();
  void SetEventHandler( wxEvtHandler *handler );
  void PushEventHandler( wxEvtHandler *handler );
  wxEvtHandler *PopEventHandler( bool deleteHandler = FALSE );

  virtual wxValidator *GetValidator();
  virtual void SetValidator( const wxValidator &validator );

  virtual void SetAcceleratorTable( const wxAcceleratorTable& accel );
  virtual wxAcceleratorTable *GetAcceleratorTable() { return &m_acceleratorTable; }
  
  bool IsBeingDeleted();

  void SetId( wxWindowID id );
  wxWindowID GetId();

  void SetCursor( const wxCursor &cursor );

  virtual void Refresh( bool eraseBackground = TRUE, const wxRect *rect = (const wxRect *) NULL );
  virtual void Clear();
  
  virtual wxRegion GetUpdateRegion() const;
  virtual bool IsExposed(int x, int y) const;
  virtual bool IsExposed(int x, int y, int w, int h) const;
  virtual bool IsExposed(const wxPoint& pt) const;
  virtual bool IsExposed(const wxRect& rect) const;

  virtual wxColour GetBackgroundColour() const;
  virtual void SetBackgroundColour( const wxColour &colour );
  virtual wxColour GetForegroundColour() const;
  virtual void SetForegroundColour( const wxColour &colour );

  virtual int GetCharHeight(void) const;
  virtual int GetCharWidth(void) const;
  virtual void GetTextExtent( const wxString& string, int *x, int *y,
                             int *descent = (int *) NULL,
                             int *externalLeading = (int *) NULL,
                             const wxFont *theFont = (const wxFont *) NULL, bool use16 = FALSE) const;

  virtual void SetDefaultBackgroundColour( const wxColour& col )
    { m_defaultBackgroundColour = col; }
  virtual wxColour GetDefaultBackgroundColour() const
    { return m_defaultBackgroundColour; }
  virtual void SetDefaultForegroundColour( const wxColour& col )
    { m_defaultForegroundColour = col; }
  virtual wxColour GetDefaultForegroundColour() const
    { return m_defaultForegroundColour; }

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

  virtual void Raise(void);
  virtual void Lower(void);

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

  virtual void SetScrollbar( int orient, int pos, int thumbVisible,
    int range, bool refresh = TRUE );
  virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE );
  virtual int GetScrollPos( int orient ) const;
  virtual int GetScrollThumb( int orient ) const;
  virtual int GetScrollRange( int orient ) const;
  virtual void ScrollWindow( int dx, int dy, const wxRect* rect = (wxRect *) NULL );

  virtual bool AcceptsFocus() const;
  void UpdateWindowUI();

public:         // cannot get private going yet

  virtual GtkWidget* GetConnectWidget(void);
  virtual bool IsOwnGtkWindow( GdkWindow *window );

  void PreCreation( wxWindow *parent, wxWindowID id, const wxPoint &pos,
    const wxSize &size, long style, const wxString &name );
  void PostCreation();
  bool HasVMT();
  virtual void ImplementSetSize();
  virtual void ImplementSetPosition();

  wxWindow            *m_parent;
  wxList               m_children;
  int                  m_x,m_y;
  int                  m_width,m_height;
  int                  m_minWidth,m_minHeight;
  int                  m_maxWidth,m_maxHeight;
  int                  m_retCode;
  wxEvtHandler        *m_eventHandler;
  wxValidator         *m_windowValidator;
  wxDropTarget        *m_pDropTarget;
  wxWindowID           m_windowId;
  wxCursor            *m_cursor;
  wxFont               m_font;
  wxColour             m_backgroundColour;
  wxColour             m_defaultBackgroundColour;
  wxColour             m_foregroundColour ;
  wxColour             m_defaultForegroundColour;
  wxRegion             m_updateRegion;
  long                 m_windowStyle;
  bool                 m_isShown;
  bool                 m_isEnabled;
  wxString             m_windowName;
  wxAcceleratorTable   m_acceleratorTable;

  GtkWidget           *m_widget;
  GtkWidget           *m_wxwindow;
  GtkAdjustment       *m_hAdjust,*m_vAdjust;
  float                m_oldHorizontalPos;
  float                m_oldVerticalPos;
  bool                 m_needParent;
  bool                 m_hasScrolling;
  bool                 m_hasVMT;
  bool                 m_sizeSet;
  bool                 m_resizing;

public:

  wxLayoutConstraints *m_constraints;
  wxList              *m_constraintsInvolvedIn;
  wxSizer             *m_windowSizer;
  wxWindow            *m_sizerParent;
  bool                 m_autoLayout;

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
