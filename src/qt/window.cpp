/////////////////////////////////////////////////////////////////////////////
// Name:        window.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#include "wx/defs.h"
#include "wx/window.h"
#include "wx/dc.h"
#include "wx/frame.h"
#include "wx/app.h"
#include "wx/layout.h"
#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/msgdlg.h"
#include "wx/dcclient.h"
#include "wx/dnd.h"
#include "wx/mdi.h"
#include "wx/notebook.h"
#include "wx/statusbr.h"
#include <math.h>

//-----------------------------------------------------------------------------
// data
//-----------------------------------------------------------------------------

extern wxList wxPendingDelete;
extern wxList wxTopLevelWindows;

//-----------------------------------------------------------------------------
// wxWindow implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxWindow,wxEvtHandler)

BEGIN_EVENT_TABLE(wxWindow, wxEvtHandler)
  EVT_SIZE(wxWindow::OnSize)
  EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindow::OnInitDialog)
  EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

wxWindow::wxWindow()
{
  m_parent = NULL;
  m_children.DeleteContents( FALSE );
  m_x = 0;
  m_y = 0;
  m_width = 0;
  m_height = 0;
  m_retCode = 0;
  m_eventHandler = this;
  m_windowValidator = NULL;
  m_windowId = -1;
  m_cursor = new wxCursor( wxCURSOR_ARROW );
  m_font = *wxSWISS_FONT;
  m_windowStyle = 0;
  m_windowName = "noname";
  m_constraints = NULL;
  m_constraintsInvolvedIn = NULL;
  m_windowSizer = NULL;
  m_sizerParent = NULL;
  m_autoLayout = FALSE;
  m_pDropTarget = NULL;
};

wxWindow::wxWindow( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  Create( parent, id, pos, size, style, name );
};

bool wxWindow::Create( wxWindow *parent, wxWindowID id,
      const wxPoint &pos, const wxSize &size,
      long style, const wxString &name )
{
  return TRUE;  
};

wxWindow::~wxWindow(void)
{
  DestroyChildren();
  
//  delete m_cursor;

  DeleteRelatedConstraints();
  if (m_constraints)
  {
    // This removes any dangling pointers to this window
    // in other windows' constraintsInvolvedIn lists.
    UnsetConstraints(m_constraints);
    delete m_constraints;
    m_constraints = NULL;
  }
  if (m_windowSizer)
  {
    delete m_windowSizer;
    m_windowSizer = NULL;
  }
  // If this is a child of a sizer, remove self from parent
  if (m_sizerParent)
    m_sizerParent->RemoveChild((wxWindow *)this);

  // Just in case the window has been Closed, but
  // we're then deleting immediately: don't leave
  // dangling pointers.
  wxPendingDelete.DeleteObject(this);

  // Just in case we've loaded a top-level window via
  // wxWindow::LoadNativeDialog but we weren't a dialog
  // class
  wxTopLevelWindows.DeleteObject(this);
    
};

bool wxWindow::Close( bool force )
{
  wxCloseEvent event(wxEVT_CLOSE_WINDOW, m_windowId);
  event.SetEventObject(this);
  event.SetForce(force);

  return GetEventHandler()->ProcessEvent(event);
};

bool wxWindow::Destroy(void)
{
  m_hasVMT = FALSE;
  delete this;
  return TRUE;
};

bool wxWindow::DestroyChildren(void)
{
  if (GetChildren()) 
  {
    wxNode *node;
    while ((node = GetChildren()->First()) != (wxNode *)NULL) 
    {
      wxWindow *child;
      if ((child = (wxWindow *)node->Data()) != (wxWindow *)NULL) 
      {
        delete child;
  if (GetChildren()->Member(child)) delete node;
      };
    };
  };
  return TRUE;
};

void wxWindow::PrepareDC( wxDC &WXUNUSED(dc) )
{
  // are we to set fonts here ?
};

void wxWindow::SetSize( int x, int y, int width, int height, int sizeFlags )
{
  
  int newX = x;
  int newY = y;
  int newW = width;
  int newH = height;
  
  if ((sizeFlags & wxSIZE_USE_EXISTING) == wxSIZE_USE_EXISTING)
  {
    if (newX == -1) newX = m_x;
    if (newY == -1) newY = m_y;
    if (newW == -1) newW = m_width;
    if (newH == -1) newH = m_height;
  };
  
  if ((sizeFlags & wxSIZE_AUTO_WIDTH) == wxSIZE_AUTO_WIDTH)
  {
    if (newW == -1) newW = 80;
  };
  
  if ((sizeFlags & wxSIZE_AUTO_HEIGHT) == wxSIZE_AUTO_HEIGHT)
  {
    if (newH == -1) newH = 26;
  };
  
  if ((m_x != newX) || (m_y != newY) || (!m_sizeSet))
  {
    m_x = newX;
    m_y = newY;
    //
  };
  if ((m_width != newW) || (m_height != newH) || (!m_sizeSet))
  {
    m_width = newW;
    m_height = newH;
    //
  };
  
  wxSizeEvent event( wxSize(m_width,m_height), GetId() );
  event.SetEventObject( this );
  ProcessEvent( event );
};

void wxWindow::SetSize( int width, int height )
{
  SetSize( -1, -1, width, height, wxSIZE_USE_EXISTING );
};

void wxWindow::Move( int x, int y )
{
  SetSize( x, y, -1, -1, wxSIZE_USE_EXISTING );
};

void wxWindow::GetSize( int *width, int *height ) const
{
  if (width) (*width) = m_width;
  if (height) (*height) = m_height;
};

void wxWindow::SetClientSize( int width, int height )
{
};

void wxWindow::GetClientSize( int *width, int *height ) const
{
};

void wxWindow::GetPosition( int *x, int *y ) const
{
  if (x) (*x) = m_x;
  if (y) (*y) = m_y;
};

void wxWindow::ClientToScreen( int *x, int *y )
{
};

void wxWindow::ScreenToClient( int *x, int *y )
{
};

void wxWindow::Centre( int direction )
{
};

void wxWindow::Fit(void)
{
  int maxX = 0;
  int maxY = 0;
  wxNode *node = GetChildren()->First();
  while ( node )
  {
    wxWindow *win = (wxWindow *)node->Data();
    int wx, wy, ww, wh;
    win->GetPosition(&wx, &wy);
    win->GetSize(&ww, &wh);
    if ( wx + ww > maxX )
      maxX = wx + ww;
    if ( wy + wh > maxY )
      maxY = wy + wh;

    node = node->Next();
  }
  SetClientSize(maxX + 5, maxY + 10);
};

void wxWindow::OnSize( wxSizeEvent &WXUNUSED(event) )
{
  //if (GetAutoLayout()) Layout();
};

bool wxWindow::Show( bool show )
{
  return TRUE;
};

void wxWindow::Enable( bool enable )
{
  m_isEnabled = enable;
};

void wxWindow::MakeModal( bool modal )
{
}

void wxWindow::SetFocus(void)
{
};

bool wxWindow::OnClose(void)
{
  return TRUE;
};

void wxWindow::AddChild( wxWindow *child )
{
};

wxList *wxWindow::GetChildren(void)
{
  return (&m_children);
};

void wxWindow::RemoveChild( wxWindow *child )
{
  if (GetChildren())
 GetChildren()->DeleteObject( child );
  child->m_parent = NULL;
};

void wxWindow::SetReturnCode( int retCode )
{
  m_retCode = retCode;
};

int wxWindow::GetReturnCode(void)
{
  return m_retCode;
};

wxWindow *wxWindow::GetParent(void)
{
  return m_parent;
};

wxEvtHandler *wxWindow::GetEventHandler(void)
{
  return m_eventHandler;
};

void wxWindow::SetEventhandler( wxEvtHandler *handler )
{
  m_eventHandler = handler;
};

wxValidator *wxWindow::GetValidator(void)
{
  return m_windowValidator;
};

void wxWindow::SetValidator( wxValidator *validator )
{
  m_windowValidator = validator;
};

bool wxWindow::IsBeingDeleted(void)
{
  return FALSE;
};

void wxWindow::SetId( wxWindowID id )
{
  m_windowId = id;
};

wxWindowID wxWindow::GetId(void)
{
  return m_windowId;
};

void wxWindow::SetCursor( const wxCursor &cursor )
{
  if (*m_cursor == cursor) return;
  (*m_cursor) = cursor;
};

void wxWindow::Refresh( bool eraseBackground, const wxRect *rect )
{
};

bool wxWindow::IsExposed( long x, long y )
{
  return (m_updateRegion.Contains( x, y ) != wxOutRegion );
};

bool wxWindow::IsExposed( long x, long y, long width, long height )
{
  return (m_updateRegion.Contains( x, y, width, height ) != wxOutRegion );
};

void wxWindow::Clear(void)
{
};

wxColour wxWindow::GetBackgroundColour(void) const
{
  return m_backgroundColour;
};

void wxWindow::SetBackgroundColour( const wxColour &colour )
{
  m_backgroundColour = colour;
};

bool wxWindow::Validate(void)
{
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    if (child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->Validate(this)) 
      { return FALSE; }
    node = node->Next();
  };
  return TRUE;
};

bool wxWindow::TransferDataToWindow(void)
{
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    if (child->GetValidator() && /* child->GetValidator()->Ok() && */
  !child->GetValidator()->TransferToWindow() )
    {
      wxMessageBox( "Application Error", "Could not transfer data to window", wxOK|wxICON_EXCLAMATION );
      return FALSE;
    };
    node = node->Next();
  };
  return TRUE;
};

bool wxWindow::TransferDataFromWindow(void)
{
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *child = (wxWindow *)node->Data();
    if ( child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->TransferFromWindow() )
      { return FALSE; }
   node = node->Next();
  }
  return TRUE;
};

void wxWindow::OnInitDialog( wxInitDialogEvent &WXUNUSED(event) )
{
  TransferDataToWindow();
};

void wxWindow::InitDialog(void)
{
  wxInitDialogEvent event(GetId());
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
};

void wxWindow::SetDropTarget( wxDropTarget *dropTarget )
{
  if (m_pDropTarget)
  {
    m_pDropTarget->UnregisterWidget( dnd_widget );
    delete m_pDropTarget;
  };
  m_pDropTarget = dropTarget;
  if (m_pDropTarget)
  {
  };
};

wxDropTarget *wxWindow::GetDropTarget() const
{
  return m_pDropTarget;
};

void wxWindow::SetFont( const wxFont &font )
{
  m_font = font;
/*
  ...
*/
};

wxFont *wxWindow::GetFont(void)
{
  return &m_font;
};

void wxWindow::SetWindowStyleFlag( long flag )
{
  m_windowStyle = flag;
};

long wxWindow::GetWindowStyleFlag(void) const
{
  return m_windowStyle;
};

void wxWindow::CaptureMouse(void)
{
};

void wxWindow::ReleaseMouse(void)
{
};

void wxWindow::SetTitle( const wxString &WXUNUSED(title) )
{
};

wxString wxWindow::GetTitle(void) const
{
  return (wxString&)m_windowName;
};

wxString wxWindow::GetLabel(void) const
{
  return GetTitle();
};

void wxWindow::SetName( const wxString &name )
{
  m_windowName = name;
};

wxString wxWindow::GetName(void) const
{
  return (wxString&)m_windowName;
};

bool wxWindow::IsShown(void) const
{
  return m_isShown;
};

bool wxWindow::IsRetained(void)
{
  return FALSE;
};

wxWindow *wxWindow::FindWindow( long id )
{
  if (id == m_windowId) return this;
  wxNode *node = m_children.First();
  while (node)
  {
    wxWindow *child = (wxWindow*)node->Data();
    wxWindow *res = child->FindWindow( id );
    if (res) return res;
    node = node->Next();
  };
  return NULL;
};

wxWindow *wxWindow::FindWindow( const wxString& name )
{
  if (name == m_windowName) return this;
  wxNode *node = m_children.First();
  while (node)
  {
    wxWindow *child = (wxWindow*)node->Data();
    wxWindow *res = child->FindWindow( name );
    if (res) return res;
    node = node->Next();
  };
  return NULL;
};

void wxWindow::SetScrollbar( int orient, int pos, int thumbVisible,
      int range, bool WXUNUSED(refresh) )
{
};

void wxWindow::SetScrollPos( int orient, int pos, bool WXUNUSED(refresh) )
{
};

int wxWindow::GetScrollThumb( int orient ) const
{
};

int wxWindow::GetScrollPos( int orient ) const
{
};

int wxWindow::GetScrollRange( int orient ) const
{
};

void wxWindow::ScrollWindow( int dx, int dy, const wxRect* WXUNUSED(rect) )
{
};

//-------------------------------------------------------------------------------------
//          Layout
//-------------------------------------------------------------------------------------

wxLayoutConstraints *wxWindow::GetConstraints(void) const
{
  return m_constraints;
};

void wxWindow::SetConstraints( wxLayoutConstraints *constraints )
{
  if (m_constraints)
  {
    UnsetConstraints(m_constraints);
    delete m_constraints;
  }
  m_constraints = constraints;
  if (m_constraints)
  {
    // Make sure other windows know they're part of a 'meaningful relationship'
    if (m_constraints->left.GetOtherWindow() && (m_constraints->left.GetOtherWindow() != this))
      m_constraints->left.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->top.GetOtherWindow() && (m_constraints->top.GetOtherWindow() != this))
      m_constraints->top.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->right.GetOtherWindow() && (m_constraints->right.GetOtherWindow() != this))
      m_constraints->right.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->bottom.GetOtherWindow() && (m_constraints->bottom.GetOtherWindow() != this))
      m_constraints->bottom.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->width.GetOtherWindow() && (m_constraints->width.GetOtherWindow() != this))
      m_constraints->width.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->height.GetOtherWindow() && (m_constraints->height.GetOtherWindow() != this))
      m_constraints->height.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->centreX.GetOtherWindow() && (m_constraints->centreX.GetOtherWindow() != this))
      m_constraints->centreX.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
    if (m_constraints->centreY.GetOtherWindow() && (m_constraints->centreY.GetOtherWindow() != this))
      m_constraints->centreY.GetOtherWindow()->AddConstraintReference((wxWindow *)this);
  }
;
}
;

void wxWindow::SetAutoLayout( bool autoLayout )
{
  m_autoLayout = autoLayout;
};

bool wxWindow::GetAutoLayout(void) const
{
  return m_autoLayout;
};

wxSizer *wxWindow::GetSizer(void) const
{
  return m_windowSizer;
};

void wxWindow::SetSizerParent( wxWindow *win )
{
  m_sizerParent = win;
};

wxWindow *wxWindow::GetSizerParent(void) const
{
  return m_sizerParent;
};

// This removes any dangling pointers to this window
// in other windows' constraintsInvolvedIn lists.
void wxWindow::UnsetConstraints(wxLayoutConstraints *c)
{
  if (c)
  {
    if (c->left.GetOtherWindow() && (c->top.GetOtherWindow() != this))
      c->left.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->top.GetOtherWindow() && (c->top.GetOtherWindow() != this))
      c->top.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->right.GetOtherWindow() && (c->right.GetOtherWindow() != this))
      c->right.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->bottom.GetOtherWindow() && (c->bottom.GetOtherWindow() != this))
      c->bottom.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->width.GetOtherWindow() && (c->width.GetOtherWindow() != this))
      c->width.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->height.GetOtherWindow() && (c->height.GetOtherWindow() != this))
      c->height.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->centreX.GetOtherWindow() && (c->centreX.GetOtherWindow() != this))
      c->centreX.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
    if (c->centreY.GetOtherWindow() && (c->centreY.GetOtherWindow() != this))
      c->centreY.GetOtherWindow()->RemoveConstraintReference((wxWindow *)this);
  }
}

// Back-pointer to other windows we're involved with, so if we delete
// this window, we must delete any constraints we're involved with.
void wxWindow::AddConstraintReference(wxWindow *otherWin)
{
  if (!m_constraintsInvolvedIn)
    m_constraintsInvolvedIn = new wxList;
  if (!m_constraintsInvolvedIn->Member(otherWin))
    m_constraintsInvolvedIn->Append(otherWin);
}

// REMOVE back-pointer to other windows we're involved with.
void wxWindow::RemoveConstraintReference(wxWindow *otherWin)
{
  if (m_constraintsInvolvedIn)
    m_constraintsInvolvedIn->DeleteObject(otherWin);
}

// Reset any constraints that mention this window
void wxWindow::DeleteRelatedConstraints(void)
{
  if (m_constraintsInvolvedIn)
  {
    wxNode *node = m_constraintsInvolvedIn->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      wxNode *next = node->Next();
      wxLayoutConstraints *constr = win->GetConstraints();

      // Reset any constraints involving this window
      if (constr)
      {
        constr->left.ResetIfWin((wxWindow *)this);
        constr->top.ResetIfWin((wxWindow *)this);
        constr->right.ResetIfWin((wxWindow *)this);
        constr->bottom.ResetIfWin((wxWindow *)this);
        constr->width.ResetIfWin((wxWindow *)this);
        constr->height.ResetIfWin((wxWindow *)this);
        constr->centreX.ResetIfWin((wxWindow *)this);
        constr->centreY.ResetIfWin((wxWindow *)this);
      }
      delete node;
      node = next;
    }
    delete m_constraintsInvolvedIn;
    m_constraintsInvolvedIn = NULL;
  }
}

void wxWindow::SetSizer(wxSizer *sizer)
{
  m_windowSizer = sizer;
  if (sizer)
    sizer->SetSizerParent((wxWindow *)this);
}

/*
 * New version
 */

bool wxWindow::Layout(void)
{
  if (GetConstraints())
  {
    int w, h;
    GetClientSize(&w, &h);
    GetConstraints()->width.SetValue(w);
    GetConstraints()->height.SetValue(h);
  }
  
  // If top level (one sizer), evaluate the sizer's constraints.
  if (GetSizer())
  {
    int noChanges;
    GetSizer()->ResetConstraints();   // Mark all constraints as unevaluated
    GetSizer()->LayoutPhase1(&noChanges);
    GetSizer()->LayoutPhase2(&noChanges);
    GetSizer()->SetConstraintSizes(); // Recursively set the real window sizes
    return TRUE;
  }
  else
  {
    // Otherwise, evaluate child constraints
    ResetConstraints();   // Mark all constraints as unevaluated
    DoPhase(1);           // Just one phase need if no sizers involved
    DoPhase(2);
    SetConstraintSizes(); // Recursively set the real window sizes
  }
  return TRUE;
}


// Do a phase of evaluating constraints:
// the default behaviour. wxSizers may do a similar
// thing, but also impose their own 'constraints'
// and order the evaluation differently.
bool wxWindow::LayoutPhase1(int *noChanges)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    return constr->SatisfyConstraints((wxWindow *)this, noChanges);
  }
  else
    return TRUE;
}

bool wxWindow::LayoutPhase2(int *noChanges)
{
  *noChanges = 0;
  
  // Layout children
  DoPhase(1);
  DoPhase(2);
  return TRUE;
}

// Do a phase of evaluating child constraints
bool wxWindow::DoPhase(int phase)
{
  int noIterations = 0;
  int maxIterations = 500;
  int noChanges = 1;
  int noFailures = 0;
  wxList succeeded;
  while ((noChanges > 0) && (noIterations < maxIterations))
  {
    noChanges = 0;
    noFailures = 0;
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *child = (wxWindow *)node->Data();
      if (!child->IsKindOf(CLASSINFO(wxFrame)) && !child->IsKindOf(CLASSINFO(wxDialog)))
      {
        wxLayoutConstraints *constr = child->GetConstraints();
        if (constr)
        {
          if (succeeded.Member(child))
          {
          }
          else
          {
            int tempNoChanges = 0;
            bool success = ( (phase == 1) ? child->LayoutPhase1(&tempNoChanges) : child->LayoutPhase2(&tempNoChanges) ) ;
            noChanges += tempNoChanges;
            if (success)
            {
              succeeded.Append(child);
            }
          }
        }
      }
      node = node->Next();
    }
    noIterations ++;
  }
  return TRUE;
}

void wxWindow::ResetConstraints(void)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    constr->left.SetDone(FALSE);
    constr->top.SetDone(FALSE);
    constr->right.SetDone(FALSE);
    constr->bottom.SetDone(FALSE);
    constr->width.SetDone(FALSE);
    constr->height.SetDone(FALSE);
    constr->centreX.SetDone(FALSE);
    constr->centreY.SetDone(FALSE);
  }
  wxNode *node = GetChildren()->First();
  while (node)
  {
    wxWindow *win = (wxWindow *)node->Data();
    if (!win->IsKindOf(CLASSINFO(wxFrame)) && !win->IsKindOf(CLASSINFO(wxDialog)))
      win->ResetConstraints();
    node = node->Next();
  }
}

// Need to distinguish between setting the 'fake' size for
// windows and sizers, and setting the real values.
void wxWindow::SetConstraintSizes(bool recurse)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr && constr->left.GetDone() && constr->right.GetDone() &&
                constr->width.GetDone() && constr->height.GetDone())
  {
    int x = constr->left.GetValue();
    int y = constr->top.GetValue();
    int w = constr->width.GetValue();
    int h = constr->height.GetValue();

    // If we don't want to resize this window, just move it...
    if ((constr->width.GetRelationship() != wxAsIs) ||
        (constr->height.GetRelationship() != wxAsIs))
    {
      // Calls Layout() recursively. AAAGH. How can we stop that.
      // Simply take Layout() out of non-top level OnSizes.
      SizerSetSize(x, y, w, h);
    }
    else
    {
      SizerMove(x, y);
    }
  }
  else if (constr)
  {
    char *windowClass = this->GetClassInfo()->GetClassName();

    wxString winName;
  if (GetName() == "")
    winName = "unnamed";
  else
    winName = GetName();
    wxDebugMsg("Constraint(s) not satisfied for window of type %s, name %s:\n", (const char *)windowClass, (const char *)winName);
    if (!constr->left.GetDone())
      wxDebugMsg("  unsatisfied 'left' constraint.\n");
    if (!constr->right.GetDone())
      wxDebugMsg("  unsatisfied 'right' constraint.\n");
    if (!constr->width.GetDone())
      wxDebugMsg("  unsatisfied 'width' constraint.\n");
    if (!constr->height.GetDone())
      wxDebugMsg("  unsatisfied 'height' constraint.\n");
    wxDebugMsg("Please check constraints: try adding AsIs() constraints.\n");
  }

  if (recurse)
  {
    wxNode *node = GetChildren()->First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (!win->IsKindOf(CLASSINFO(wxFrame)) && !win->IsKindOf(CLASSINFO(wxDialog)))
        win->SetConstraintSizes();
      node = node->Next();
    }
  }
}

// This assumes that all sizers are 'on' the same
// window, i.e. the parent of this window.
void wxWindow::TransformSizerToActual(int *x, int *y) const
{
  if (!m_sizerParent || m_sizerParent->IsKindOf(CLASSINFO(wxDialog)) ||
         m_sizerParent->IsKindOf(CLASSINFO(wxFrame)) )
    return;
    
  int xp, yp;
  m_sizerParent->GetPosition(&xp, &yp);
  m_sizerParent->TransformSizerToActual(&xp, &yp);
  *x += xp;
  *y += yp;
}

void wxWindow::SizerSetSize(int x, int y, int w, int h)
{
  int xx = x;
  int yy = y;
  TransformSizerToActual(&xx, &yy);
  SetSize(xx, yy, w, h);
}

void wxWindow::SizerMove(int x, int y)
{
  int xx = x;
  int yy = y;
  TransformSizerToActual(&xx, &yy);
  Move(xx, yy);
}

// Only set the size/position of the constraint (if any)
void wxWindow::SetSizeConstraint(int x, int y, int w, int h)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    if (x != -1)
    {
      constr->left.SetValue(x);
      constr->left.SetDone(TRUE);
    }
    if (y != -1)
    {
      constr->top.SetValue(y);
      constr->top.SetDone(TRUE);
    }
    if (w != -1)
    {
      constr->width.SetValue(w);
      constr->width.SetDone(TRUE);
    }
    if (h != -1)
    {
      constr->height.SetValue(h);
      constr->height.SetDone(TRUE);
    }
  }
}

void wxWindow::MoveConstraint(int x, int y)
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    if (x != -1)
    {
      constr->left.SetValue(x);
      constr->left.SetDone(TRUE);
    }
    if (y != -1)
    {
      constr->top.SetValue(y);
      constr->top.SetDone(TRUE);
    }
  }
}

void wxWindow::GetSizeConstraint(int *w, int *h) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *w = constr->width.GetValue();
    *h = constr->height.GetValue();
  }
  else
    GetSize(w, h);
}

void wxWindow::GetClientSizeConstraint(int *w, int *h) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *w = constr->width.GetValue();
    *h = constr->height.GetValue();
  }
  else
    GetClientSize(w, h);
}

void wxWindow::GetPositionConstraint(int *x, int *y) const
{
  wxLayoutConstraints *constr = GetConstraints();
  if (constr)
  {
    *x = constr->left.GetValue();
    *y = constr->top.GetValue();
  }
  else
    GetPosition(x, y);
}

bool wxWindow::AcceptsFocus() const
{
  return IsEnabled() && IsShown();
}

void wxWindow::OnIdle(wxIdleEvent& WXUNUSED(event) )
{
  UpdateWindowUI();
}
