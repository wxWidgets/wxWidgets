/////////////////////////////////////////////////////////////////////////////
// Name:        windows.cpp
// Purpose:     wxWindow
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "window.h"
#endif

#include "wx/setup.h"
#include "wx/menu.h"
#include "wx/dc.h"
#include "wx/dcclient.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/panel.h"
#include "wx/layout.h"
#include "wx/dialog.h"
#include "wx/listbox.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/msgdlg.h"
#include "wx/frame.h"

#include "wx/menuitem.h"
#include "wx/log.h"

#if  USE_DRAG_AND_DROP
#include "wx/dnd.h"
#endif

#include <string.h>

extern wxList wxPendingDelete;

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxWindow, wxEvtHandler)

BEGIN_EVENT_TABLE(wxWindow, wxEvtHandler)
  EVT_CHAR(wxWindow::OnChar)
  EVT_ERASE_BACKGROUND(wxWindow::OnEraseBackground)
  EVT_SYS_COLOUR_CHANGED(wxWindow::OnSysColourChanged)
  EVT_INIT_DIALOG(wxWindow::OnInitDialog)
  EVT_IDLE(wxWindow::OnIdle)
END_EVENT_TABLE()

#endif


// Constructor
wxWindow::wxWindow()
{
    // Generic
    m_windowId = 0;
    m_windowStyle = 0;
    m_windowParent = NULL;
    m_windowEventHandler = this;
    m_windowName = "";
    m_windowCursor = *wxSTANDARD_CURSOR;
    m_children = new wxList;
    m_constraints = NULL;
    m_constraintsInvolvedIn = NULL;
    m_windowSizer = NULL;
    m_sizerParent = NULL;
    m_autoLayout = FALSE;
    m_windowValidator = NULL;
    m_defaultItem = NULL;
    m_returnCode = 0;
    m_caretWidth = 0; m_caretHeight = 0;
    m_caretEnabled = FALSE;
    m_caretShown = FALSE;
    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
    m_foregroundColour = *wxBLACK;
    m_defaultForegroundColour = *wxBLACK ;
    m_defaultBackgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE) ;

#if  USE_DRAG_AND_DROP
    m_pDropTarget = NULL;
#endif
}

// Destructor
wxWindow::~wxWindow()
{
	// Have to delete constraints/sizer FIRST otherwise
	// sizers may try to look at deleted windows as they
	// delete themselves.
#if USE_CONSTRAINTS
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
#endif

    if (m_windowParent)
        m_windowParent->RemoveChild(this);

    DestroyChildren();

    // TODO: destroy the window

    delete m_children;
    m_children = NULL;

    // Just in case the window has been Closed, but
    // we're then deleting immediately: don't leave
    // dangling pointers.
    wxPendingDelete.DeleteObject(this);

    if ( m_windowValidator )
	    delete m_windowValidator;
}

// Destroy the window (delayed, if a managed window)
bool wxWindow::Destroy()
{
    delete this;
    return TRUE;
}

// Constructor
bool wxWindow::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos,
           const wxSize& size,
           long style,
           const wxString& name)
{
    // Generic
    m_windowId = 0;
    m_windowStyle = 0;
    m_windowParent = NULL;
    m_windowEventHandler = this;
    m_windowName = "";
    m_windowCursor = *wxSTANDARD_CURSOR;
    m_constraints = NULL;
    m_constraintsInvolvedIn = NULL;
    m_windowSizer = NULL;
    m_sizerParent = NULL;
    m_autoLayout = FALSE;
    m_windowValidator = NULL;

#if USE_DRAG_AND_DROP
    m_pDropTarget = NULL;
#endif

    m_caretWidth = 0; m_caretHeight = 0;
    m_caretEnabled = FALSE;
    m_caretShown = FALSE;
    m_minSizeX = -1;
    m_minSizeY = -1;
    m_maxSizeX = -1;
    m_maxSizeY = -1;
    m_defaultItem = NULL;
    m_windowParent = NULL;
    if (!parent)
        return FALSE;

    if (parent) parent->AddChild(this);

    m_returnCode = 0;

    SetName(name);

    if ( id == -1 )
    	m_windowId = (int)NewControlId();
    else
	m_windowId = id;

    m_backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW) ; ;
    m_foregroundColour = *wxBLACK;
    m_defaultForegroundColour = *wxBLACK ;
    m_defaultBackgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE) ;

    m_windowStyle = style;

    if ( id == -1 )
    	m_windowId = (int)NewControlId();
    else
	m_windowId = id;

    // TODO: create the window

    return TRUE;
}

void wxWindow::SetFocus()
{
    // TODO
}

void wxWindow::Enable(bool enable)
{
    // TODO
}

void wxWindow::CaptureMouse()
{
    // TODO
}

void wxWindow::ReleaseMouse()
{
    // TODO
}

// Push/pop event handler (i.e. allow a chain of event handlers
// be searched)
void wxWindow::PushEventHandler(wxEvtHandler *handler)
{
	handler->SetNextHandler(GetEventHandler());
	SetEventHandler(handler);
}

wxEvtHandler *wxWindow::PopEventHandler(bool deleteHandler)
{
	if ( GetEventHandler() )
	{
		wxEvtHandler *handlerA = GetEventHandler();
		wxEvtHandler *handlerB = handlerA->GetNextHandler();
		handlerA->SetNextHandler(NULL);
		SetEventHandler(handlerB);
		if ( deleteHandler )
		{
			delete handlerA;
			return NULL;
		}
		else
			return handlerA;
	}
	else
		return NULL;
}

#if    USE_DRAG_AND_DROP

void wxWindow::SetDropTarget(wxDropTarget *pDropTarget)
{
  if ( m_pDropTarget != 0 ) {
    delete m_pDropTarget;
  }

  m_pDropTarget = pDropTarget;
  if ( m_pDropTarget != 0 )
  {
    // TODO
  }
}

#endif

// Old style file-manager drag&drop
void wxWindow::DragAcceptFiles(bool accept)
{
    // TODO
}

// Get total size
void wxWindow::GetSize(int *x, int *y) const
{
    // TODO
}

void wxWindow::GetPosition(int *x, int *y) const
{
    // TODO
}

void wxWindow::ScreenToClient(int *x, int *y) const
{
    // TODO
}

void wxWindow::ClientToScreen(int *x, int *y) const
{
    // TODO
}

void wxWindow::SetCursor(const wxCursor& cursor)
{
  m_windowCursor = cursor;
  if (m_windowCursor.Ok())
  {
    // TODO
  }
}


// Get size *available for subwindows* i.e. excluding menu bar etc.
void wxWindow::GetClientSize(int *x, int *y) const
{
    // TODO
}

void wxWindow::SetSize(int x, int y, int width, int height, int sizeFlags)
{
    // TODO
}

void wxWindow::SetClientSize(int width, int height)
{
    // TODO
}

// For implementation purposes - sometimes decorations make the client area
// smaller
wxPoint wxWindow::GetClientAreaOrigin() const
{
    return wxPoint(0, 0);
}

// Makes an adjustment to the window position (for example, a frame that has
// a toolbar that it manages itself).
void wxWindow::AdjustForParentClientOrigin(int& x, int& y, int sizeFlags)
{
    if (((sizeFlags & wxSIZE_NO_ADJUSTMENTS) == 0) && GetParent())
    {
        wxPoint pt(GetParent()->GetClientAreaOrigin());
        x += pt.x; y += pt.y;
    }
}

bool wxWindow::Show(bool show)
{
    // TODO
    return FALSE;
}

bool wxWindow::IsShown() const
{
    // TODO
    return FALSE;
}

int wxWindow::GetCharHeight() const
{
    // TODO
    return 0;
}

int wxWindow::GetCharWidth() const
{
    // TODO
    return 0;
}

void wxWindow::GetTextExtent(const wxString& string, int *x, int *y,
                           int *descent, int *externalLeading, const wxFont *theFont, bool) const
{
  wxFont *fontToUse = (wxFont *)theFont;
  if (!fontToUse)
    fontToUse = (wxFont *) & m_windowFont;

    // TODO
}

void wxWindow::Refresh(bool eraseBack, const wxRectangle *rect)
{
    // TODO
}

// Responds to colour changes: passes event on to children.
void wxWindow::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    wxNode *node = GetChildren()->First();
    while ( node )
    {
        // Only propagate to non-top-level windows
        wxWindow *win = (wxWindow *)node->Data();
        if ( win->GetParent() )
        {
            wxSysColourChangedEvent event2;
            event.m_eventObject = win;
            win->GetEventHandler()->ProcessEvent(event2);
        }

        node = node->Next();
    }
}

// This can be called by the app (or wxWindows) to do default processing for the current
// event. Save message/event info in wxWindow so they can be used in this function.
long wxWindow::Default()
{
    // TODO
    return 0;
}

void wxWindow::InitDialog()
{
  wxInitDialogEvent event(GetId());
  event.SetEventObject( this );
  GetEventHandler()->ProcessEvent(event);
}

// Default init dialog behaviour is to transfer data to window
void wxWindow::OnInitDialog(wxInitDialogEvent& event)
{
  TransferDataToWindow();
}

// Caret manipulation
void wxWindow::CreateCaret(int w, int h)
{
  m_caretWidth = w;
  m_caretHeight = h;
  m_caretEnabled = TRUE;
}

void wxWindow::CreateCaret(const wxBitmap *WXUNUSED(bitmap))
{
    // TODO
}

void wxWindow::ShowCaret(bool show)
{
    // TODO
}

void wxWindow::DestroyCaret()
{
    // TODO
    m_caretEnabled = FALSE;
}

void wxWindow::SetCaretPos(int x, int y)
{
    // TODO
}

void wxWindow::GetCaretPos(int *x, int *y) const
{
    // TODO
}

wxWindow *wxGetActiveWindow()
{
    // TODO
    return NULL;
}

void wxWindow::SetSizeHints(int minW, int minH, int maxW, int maxH, int WXUNUSED(incW), int WXUNUSED(incH))
{
  m_minSizeX = minW;
  m_minSizeY = minH;
  m_maxSizeX = maxW;
  m_maxSizeY = maxH;
}

void wxWindow::Centre(int direction)
{
  int x, y, width, height, panel_width, panel_height, new_x, new_y;

  wxWindow *father = (wxWindow *)GetParent();
  if (!father)
    return;

  father->GetClientSize(&panel_width, &panel_height);
  GetSize(&width, &height);
  GetPosition(&x, &y);

  new_x = -1;
  new_y = -1;

  if (direction & wxHORIZONTAL)
    new_x = (int)((panel_width - width)/2);

  if (direction & wxVERTICAL)
    new_y = (int)((panel_height - height)/2);

  SetSize(new_x, new_y, -1, -1);

}

// Coordinates relative to the window
void wxWindow::WarpPointer (int x_pos, int y_pos)
{
    // TODO
}

void wxWindow::OnEraseBackground(wxEraseEvent& event)
{
    // TODO
    Default();
}

int wxWindow::GetScrollPos(int orient) const
{
    // TODO
    return 0;
}

// This now returns the whole range, not just the number
// of positions that we can scroll.
int wxWindow::GetScrollRange(int orient) const
{
    // TODO
    return 0;
}

int wxWindow::GetScrollThumb(int orient) const
{
    // TODO
    return 0;
}

void wxWindow::SetScrollPos(int orient, int pos, bool refresh)
{
    // TODO
    return;
}

// New function that will replace some of the above.
void wxWindow::SetScrollbar(int orient, int pos, int thumbVisible,
    int range, bool refresh)
{
    // TODO
}

// Does a physical scroll
void wxWindow::ScrollWindow(int dx, int dy, const wxRectangle *rect)
{
    // TODO
    return;
}

void wxWindow::SetFont(const wxFont& font)
{
    m_windowFont = font;

    if (!m_windowFont.Ok())
	    return;
    // TODO
}

void wxWindow::OnChar(wxKeyEvent& event)
{
    if ( event.KeyCode() == WXK_TAB ) {
        // propagate the TABs to the parent - it's up to it to decide what
        // to do with it
        if ( GetParent() ) {
            if ( GetParent()->ProcessEvent(event) )
                return;
        }
    }
}

void wxWindow::OnPaint(wxPaintEvent& event)
{
	Default();
}

bool wxWindow::IsEnabled() const
{
    // TODO
    return FALSE;
}

// Dialog support: override these and call
// base class members to add functionality
// that can't be done using validators.
// NOTE: these functions assume that controls
// are direct children of this window, not grandchildren
// or other levels of descendant.

// Transfer values to controls. If returns FALSE,
// it's an application error (pops up a dialog)
bool wxWindow::TransferDataToWindow()
{
	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() &&
		     !child->GetValidator()->TransferToWindow() )
		{
			wxMessageBox("Application Error", "Could not transfer data to window", wxOK|wxICON_EXCLAMATION);
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

// Transfer values from controls. If returns FALSE,
// validation failed: don't quit
bool wxWindow::TransferDataFromWindow()
{
	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() && !child->GetValidator()->TransferFromWindow() )
		{
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

bool wxWindow::Validate()
{
	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		if ( child->GetValidator() && /* child->GetValidator()->Ok() && */ !child->GetValidator()->Validate(this) )
		{
			return FALSE;
		}

		node = node->Next();
	}
	return TRUE;
}

// Get the window with the focus
wxWindow *wxWindow::FindFocus()
{
    // TODO
    return NULL;
}

void wxWindow::AddChild(wxWindow *child)
{
    GetChildren()->Append(child);
    child->m_windowParent = this;
}

void wxWindow::RemoveChild(wxWindow *child)
{
    if (GetChildren())
        GetChildren()->DeleteObject(child);
    child->m_windowParent = NULL;
}

void wxWindow::DestroyChildren()
{
  if (GetChildren()) {
    wxNode *node;
    while ((node = GetChildren()->First()) != (wxNode *)NULL) {
      wxWindow *child;
      if ((child = (wxWindow *)node->Data()) != (wxWindow *)NULL) {
        delete child;
		if ( GetChildren()->Member(child) )
			delete node;
      }
    } /* while */
  }
}

void wxWindow::MakeModal(bool modal)
{
  // Disable all other windows
  if (this->IsKindOf(CLASSINFO(wxDialog)) || this->IsKindOf(CLASSINFO(wxFrame)))
  {
    wxNode *node = wxTopLevelWindows.First();
    while (node)
    {
      wxWindow *win = (wxWindow *)node->Data();
      if (win != this)
        win->Enable(!modal);

      node = node->Next();
    }
  }
}

// If nothing defined for this, try the parent.
// E.g. we may be a button loaded from a resource, with no callback function
// defined.
void wxWindow::OnCommand(wxWindow& win, wxCommandEvent& event)
{
  if (GetEventHandler()->ProcessEvent(event) )
    return;
  if (m_windowParent)
    m_windowParent->GetEventHandler()->OnCommand(win, event);
}

void wxWindow::SetConstraints(wxLayoutConstraints *c)
{
  if (m_constraints)
  {
    UnsetConstraints(m_constraints);
    delete m_constraints;
  }
  m_constraints = c;
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
}

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
void wxWindow::DeleteRelatedConstraints()
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

bool wxWindow::Layout()
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

void wxWindow::ResetConstraints()
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

bool wxWindow::Close(bool force)
{
  wxCloseEvent event(wxEVT_CLOSE_WINDOW, m_windowId);
  event.SetEventObject(this);
  event.SetForce(force);

  return GetEventHandler()->ProcessEvent(event);
}

wxObject* wxWindow::GetChild(int number) const
{
  // Return a pointer to the Nth object in the window
  if (!GetChildren())
    return(NULL) ;
  wxNode *node = GetChildren()->First();
  int n = number;
  while (node && n--)
    node = node->Next() ;
  if (node)
  {
    wxObject *obj = (wxObject *)node->Data();
    return(obj) ;
  }
  else
    return NULL ;
}

void wxWindow::OnDefaultAction(wxControl *initiatingItem)
{
    // Obsolete function
}

void wxWindow::Clear()
{
	wxClientDC dc(this);
    wxBrush brush(GetBackgroundColour(), wxSOLID);
    dc.SetBackground(brush);
    dc.Clear();
}

// Fits the panel around the items
void wxWindow::Fit()
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
	SetClientSize(maxX + 5, maxY + 5);
}

void wxWindow::SetValidator(const wxValidator& validator)
{
	if ( m_windowValidator )
		delete m_windowValidator;
	m_windowValidator = validator.Clone();

	if ( m_windowValidator )
		m_windowValidator->SetWindow(this) ;
}

void wxWindow::SetAcceleratorTable(const wxAcceleratorTable& accel)
{
    m_acceleratorTable = accel;
}

// Find a window by id or name
wxWindow *wxWindow::FindWindow(long id)
{
	if ( GetId() == id)
		return this;

	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		wxWindow *found = child->FindWindow(id);
		if ( found )
			return found;
		node = node->Next();
	}
	return NULL;
}

wxWindow *wxWindow::FindWindow(const wxString& name)
{
	if ( GetName() == name)
		return this;

	wxNode *node = GetChildren()->First();
	while ( node )
	{
		wxWindow *child = (wxWindow *)node->Data();
		wxWindow *found = child->FindWindow(name);
		if ( found )
			return found;
		node = node->Next();
	}
	return NULL;
}

void wxWindow::OnIdle(wxIdleEvent& event)
{
/* TODO: you may need to do something like this
 * if your GUI doesn't generate enter/leave events

    // Check if we need to send a LEAVE event
    if (m_mouseInWindow)
    {
        POINT pt;
        ::GetCursorPos(&pt);
        if (::WindowFromPoint(pt) != (HWND) GetHWND())
        {
            // Generate a LEAVE event
            m_mouseInWindow = FALSE;
            MSWOnMouseLeave(pt.x, pt.y, 0);
        }
    }
*/

    // This calls the UI-update mechanism (querying windows for
    // menu/toolbar/control state information)
	UpdateWindowUI();
}

// Raise the window to the top of the Z order
void wxWindow::Raise()
{
    // TODO
}

// Lower the window to the bottom of the Z order
void wxWindow::Lower()
{
    // TODO
}

bool wxWindow::AcceptsFocus() const
{
  return IsShown() && IsEnabled();
}

// Update region access
wxRegion wxWindow::GetUpdateRegion() const
{
    return m_updateRegion;
}

bool wxWindow::IsExposed(int x, int y, int w, int h) const
{
    return (m_updateRegion.Contains(x, y, w, h) != wxOutRegion);
}

bool wxWindow::IsExposed(const wxPoint& pt) const
{
    return (m_updateRegion.Contains(pt) != wxOutRegion);
}

bool wxWindow::IsExposed(const wxRect& rect) const
{
    return (m_updateRegion.Contains(rect) != wxOutRegion);
}

/*
 * Allocates control IDs
 */

int wxWindow::NewControlId()
{
    static int s_controlId = 0;
    s_controlId ++;
    return s_controlId;
}


