/////////////////////////////////////////////////////////////////////////////
// Name:        common/window.cpp
// Purpose:     common (to all ports) wxWindow functions
// Author:      Julian Smart, Vadim Zeitlin
// Modified by:
// Created:     13/07/98
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "windowbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
    #include "wx/defs.h"
    #include "wx/window.h"
    #include "wx/checkbox.h"
    #include "wx/radiobut.h"
    #include "wx/settings.h"
    #include "wx/dialog.h"
#endif //WX_PRECOMP

#if wxUSE_CONSTRAINTS
    #include "wx/layout.h"
#endif // wxUSE_CONSTRAINTS

#if wxUSE_DRAG_AND_DROP
    #include "wx/dnd.h"
#endif // wxUSE_DRAG_AND_DROP

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif // wxUSE_TOOLTIPS

#if wxUSE_CARET
    #include "wx/caret.h"
#endif // wxUSE_CARET

// ----------------------------------------------------------------------------
// static data
// ----------------------------------------------------------------------------

int wxWindowBase::ms_lastControlId = -200;

IMPLEMENT_ABSTRACT_CLASS(wxWindowBase, wxEvtHandler)

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxWindowBase, wxEvtHandler)
    EVT_SYS_COLOUR_CHANGED(wxWindowBase::OnSysColourChanged)
    EVT_INIT_DIALOG(wxWindowBase::OnInitDialog)
END_EVENT_TABLE()

// ============================================================================
// implementation of the common functionality of the wxWindow class
// ============================================================================

// ----------------------------------------------------------------------------
// initialization
// ----------------------------------------------------------------------------

// the default initialization
void wxWindowBase::InitBase()
{
    // no window yet, no parent nor children
    m_parent = (wxWindow *)NULL;
    m_windowId = -1;
    m_children.DeleteContents( FALSE ); // don't auto delete node data

    // no constraints on the minimal window size
    m_minWidth =
    m_minHeight =
    m_maxWidth =
    m_maxHeight = -1;

    // window is created enabled but it's not visible yet
    m_isShown = FALSE;
    m_isEnabled = TRUE;

    // no client data
    m_clientObject = (wxClientData *)NULL;
    m_clientData = NULL;

    // the default event handler is just this window
    m_eventHandler = this;

#if wxUSE_VALIDATORS
    // no validator
    m_windowValidator = (wxValidator *) NULL;
#endif // wxUSE_VALIDATORS

    // use the system default colours
    wxSystemSettings settings;

    m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_BTNFACE);
    m_foregroundColour = *wxBLACK;  // TODO take this from sys settings too?
    m_font = *wxSWISS_FONT;         //      and this?

    // no style bits
    m_windowStyle = 0;

    // an optimization for the event processing: checking this flag is much
    // faster than using IsKindOf(CLASSINFO(wxWindow))
    m_isWindow = TRUE;

#if wxUSE_CONSTRAINTS
    // no constraints whatsoever
    m_constraints = (wxLayoutConstraints *) NULL;
    m_constraintsInvolvedIn = (wxWindowList *) NULL;
    m_windowSizer = (wxSizer *) NULL;
    m_sizerParent = (wxWindowBase *) NULL;
    m_autoLayout = FALSE;
#endif // wxUSE_CONSTRAINTS

#if wxUSE_DRAG_AND_DROP
    m_dropTarget = (wxDropTarget *)NULL;
#endif // wxUSE_DRAG_AND_DROP

#if wxUSE_TOOLTIPS
    m_tooltip = (wxToolTip *)NULL;
#endif // wxUSE_TOOLTIPS

#if wxUSE_CARET
    m_caret = (wxCaret *)NULL;
#endif // wxUSE_CARET
}

// common part of window creation process
bool wxWindowBase::CreateBase(wxWindowBase *parent,
                              wxWindowID id,
                              const wxPoint& WXUNUSED(pos),
                              const wxSize& WXUNUSED(size),
                              long style,
                              const wxString& name)
{
    // m_isWindow is set to TRUE in wxWindowBase::Init() as well as many other
    // member variables - check that it has been called (will catch the case
    // when a new ctor is added which doesn't call InitWindow)
    wxASSERT_MSG( m_isWindow, _T("Init() must have been called before!") );

    // generate a new id if the user doesn't care about it
    m_windowId = id == -1 ? NewControlId() : id;

    SetName(name);
    SetWindowStyleFlag(style);
    SetParent(parent);

    return TRUE;
}

// ----------------------------------------------------------------------------
// destruction
// ----------------------------------------------------------------------------

// common clean up
wxWindowBase::~wxWindowBase()
{
    // FIXME if these 2 cases result from programming errors in the user code
    //       we should probably assert here instead of silently fixing them

    // Just in case the window has been Closed, but we're then deleting
    // immediately: don't leave dangling pointers.
    wxPendingDelete.DeleteObject(this);

    // Just in case we've loaded a top-level window via LoadNativeDialog but
    // we weren't a dialog class
    wxTopLevelWindows.DeleteObject(this);

    wxASSERT_MSG( GetChildren().GetCount() == 0, _T("children not destroyed") );

    // make sure that there are no dangling pointers left pointing to us
    wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    if ( panel )
    {
        if ( panel->GetLastFocus() == this )
        {
            panel->SetLastFocus((wxWindow *)NULL);
        }
    }

#if wxUSE_CARET
    if ( m_caret )
        delete m_caret;
#endif // wxUSE_CARET

#if wxUSE_VALIDATORS
    if ( m_windowValidator )
        delete m_windowValidator;
#endif // wxUSE_VALIDATORS

    if ( m_clientObject )
        delete m_clientObject;

#if wxUSE_CONSTRAINTS
    // Have to delete constraints/sizer FIRST otherwise sizers may try to look
    // at deleted windows as they delete themselves.
    DeleteRelatedConstraints();

    if ( m_constraints )
    {
        // This removes any dangling pointers to this window in other windows'
        // constraintsInvolvedIn lists.
        UnsetConstraints(m_constraints);
        delete m_constraints;
        m_constraints = NULL;
    }

    if ( m_windowSizer )
        delete m_windowSizer;

    // If this is a child of a sizer, remove self from parent
    if ( m_sizerParent )
        m_sizerParent->RemoveChild(this);
#endif // wxUSE_CONSTRAINTS

#if wxUSE_DRAG_AND_DROP
    if ( m_dropTarget )
        delete m_dropTarget;
#endif // wxUSE_DRAG_AND_DROP

#if wxUSE_TOOLTIPS
    if ( m_tooltip )
        delete m_tooltip;
#endif // wxUSE_TOOLTIPS
}

bool wxWindowBase::Destroy()
{
    delete this;

    return TRUE;
}

bool wxWindowBase::Close(bool force)
{
    wxCloseEvent event(wxEVT_CLOSE_WINDOW, m_windowId);
    event.SetEventObject(this);
#if WXWIN_COMPATIBILITY
    event.SetForce(force);
#endif // WXWIN_COMPATIBILITY
    event.SetCanVeto(!force);

    // return FALSE if window wasn't closed because the application vetoed the
    // close event
    return GetEventHandler()->ProcessEvent(event) && !event.GetVeto();
}

bool wxWindowBase::DestroyChildren()
{
    wxWindowList::Node *node;
    for ( ;; )
    {
        // we iterate until the list becomes empty
        node = GetChildren().GetFirst();
        if ( !node )
            break;

        wxWindow *child = node->GetData();

        wxASSERT_MSG( child, _T("children list contains empty nodes") );

        delete child;

        wxASSERT_MSG( !GetChildren().Find(child),
                      _T("child didn't remove itself using RemoveChild()") );
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// centre/fit the window
// ----------------------------------------------------------------------------

// centre the window with respect to its parent in either (or both) directions
void wxWindowBase::Centre(int direction)
{
    int widthParent, heightParent;

    wxWindow *parent = GetParent();
    if ( parent )
    {
        parent->GetClientSize(&widthParent, &heightParent);
    }
    else
    {
        // centre with respect to the whole screen
        wxDisplaySize(&widthParent, &heightParent);
    }

    int width, height;
    GetSize(&width, &height);

    int xNew = -1,
        yNew = -1;

    if ( direction & wxHORIZONTAL )
        xNew = (widthParent - width)/2;

    if ( direction & wxVERTICAL )
        yNew = (heightParent - height)/2;

    // controls are always centered on their parent because it doesn't make
    // sense to centre them on the screen
    if ( (direction & wxCENTER_FRAME) || wxDynamicCast(this, wxControl) )
    {
        // adjust to the parents client area origin
        wxPoint posParent = parent->ClientToScreen(wxPoint(0, 0));

        xNew += posParent.x;
        yNew += posParent.y;
    }

    Move(xNew, yNew);
}

// fits the window around the children
void wxWindowBase::Fit()
{
    int maxX = 0,
        maxY = 0;

    for ( wxWindowList::Node *node = GetChildren().GetFirst();
          node;
          node = node->GetNext() )
    {
        wxWindow *win = node->GetData();
        if ( win->IsTopLevel() )
        {
            // dialogs and frames lie in different top level windows - don't
            // deal with them here
            continue;
        }

        int wx, wy, ww, wh;
        win->GetPosition(&wx, &wy);
        win->GetSize(&ww, &wh);
        if ( wx + ww > maxX )
            maxX = wx + ww;
        if ( wy + wh > maxY )
            maxY = wy + wh;
    }

    // leave a margin
    SetClientSize(maxX + 7, maxY + 14);
}

// set the min/max size of the window

void wxWindowBase::SetSizeHints(int minW, int minH,
                                int maxW, int maxH,
                                int WXUNUSED(incW), int WXUNUSED(incH))
{
    m_minWidth = minW;
    m_maxWidth = maxW;
    m_minHeight = minH;
    m_maxHeight = maxH;
}

// ----------------------------------------------------------------------------
// show/hide/enable/disable the window
// ----------------------------------------------------------------------------

bool wxWindowBase::Show(bool show)
{
    if ( show != m_isShown )
    {
        m_isShown = show;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool wxWindowBase::Enable(bool enable)
{
    if ( enable != m_isEnabled )
    {
        m_isEnabled = enable;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
// ----------------------------------------------------------------------------
// RTTI
// ----------------------------------------------------------------------------

bool wxWindowBase::IsTopLevel() const
{
    return wxDynamicCast(this, wxFrame) || wxDynamicCast(this, wxDialog);
}

// ----------------------------------------------------------------------------
// reparenting the window
// ----------------------------------------------------------------------------

void wxWindowBase::AddChild(wxWindowBase *child)
{
    wxCHECK_RET( child, _T("can't add a NULL child") );

    GetChildren().Append(child);
    child->SetParent(this);
}

void wxWindowBase::RemoveChild(wxWindowBase *child)
{
    wxCHECK_RET( child, _T("can't remove a NULL child") );

    GetChildren().DeleteObject(child);
    child->SetParent((wxWindow *)NULL);
}

bool wxWindowBase::Reparent(wxWindowBase *newParent)
{
    wxWindow *oldParent = GetParent();
    if ( newParent == oldParent )
    {
        // nothing done
        return FALSE;
    }

    // unlink this window from the existing parent.
    if ( oldParent )
    {
        oldParent->RemoveChild(this);
    }
    else
    {
        wxTopLevelWindows.DeleteObject(this);
    }

    // add it to the new one
    if ( newParent )
    {
        newParent->AddChild(this);
    }
    else
    {
        wxTopLevelWindows.Append(this);
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// event handler stuff
// ----------------------------------------------------------------------------

void wxWindowBase::PushEventHandler(wxEvtHandler *handler)
{
    handler->SetNextHandler(GetEventHandler());
    SetEventHandler(handler);
}

wxEvtHandler *wxWindowBase::PopEventHandler(bool deleteHandler)
{
    wxEvtHandler *handlerA = GetEventHandler();
    if ( handlerA )
    {
        wxEvtHandler *handlerB = handlerA->GetNextHandler();
        handlerA->SetNextHandler((wxEvtHandler *)NULL);
        SetEventHandler(handlerB);
        if ( deleteHandler )
        {
            delete handlerA;
            handlerA = (wxEvtHandler *)NULL;
        }
    }

    return handlerA;
}

// ----------------------------------------------------------------------------
// cursors, fonts &c
// ----------------------------------------------------------------------------

bool wxWindowBase::SetBackgroundColour( const wxColour &colour )
{
    if ( !colour.Ok() || (colour == m_backgroundColour) )
        return FALSE;

    m_backgroundColour = colour;

    return TRUE;
}

bool wxWindowBase::SetForegroundColour( const wxColour &colour )
{
    if ( !colour.Ok() || (colour == m_foregroundColour) )
        return FALSE;

    m_foregroundColour = colour;

    return TRUE;
}

bool wxWindowBase::SetCursor(const wxCursor& cursor)
{
    // don't try to set invalid cursor, always fall back to the default
    const wxCursor& cursorOk = cursor.Ok() ? cursor : *wxSTANDARD_CURSOR;

    if ( cursorOk == m_cursor )
    {
        // no change
        return FALSE;
    }

    m_cursor = cursorOk;

    return TRUE;
}

bool wxWindowBase::SetFont(const wxFont& font)
{
    // don't try to set invalid font, always fall back to the default
    const wxFont& fontOk = font.Ok() ? font : *wxSWISS_FONT;

    if ( fontOk == m_font )
    {
        // no change
        return FALSE;
    }

    m_font = fontOk;

    return TRUE;
}

#if wxUSE_CARET
void wxWindowBase::SetCaret(wxCaret *caret)
{
    if ( m_caret )
    {
        delete m_caret;
    }

    m_caret = caret;

    if ( m_caret )
    {
        wxASSERT_MSG( m_caret->GetWindow() == this,
                      _T("caret should be created associated to this window") );
    }
}
#endif // wxUSE_CARET

#if wxUSE_VALIDATORS
// ----------------------------------------------------------------------------
// validators
// ----------------------------------------------------------------------------

void wxWindowBase::SetValidator(const wxValidator& validator)
{
    if ( m_windowValidator )
        delete m_windowValidator;

    m_windowValidator = (wxValidator *)validator.Clone();

    if ( m_windowValidator )
        m_windowValidator->SetWindow(this) ;
}
#endif // wxUSE_VALIDATORS

// ----------------------------------------------------------------------------
// update region testing
// ----------------------------------------------------------------------------

bool wxWindowBase::IsExposed(int x, int y) const
{
    return m_updateRegion.Contains(x, y) != wxOutRegion;
}

bool wxWindowBase::IsExposed(int x, int y, int w, int h) const
{
    return m_updateRegion.Contains(x, y, w, h) != wxOutRegion;
}

// ----------------------------------------------------------------------------
// find window by id or name
// ----------------------------------------------------------------------------

wxWindow *wxWindowBase::FindWindow( long id )
{
    if ( id == m_windowId )
        return (wxWindow *)this;

    wxWindowBase *res = (wxWindow *)NULL;
    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node && !res; node = node->GetNext() )
    {
        wxWindowBase *child = node->GetData();
        res = child->FindWindow( id );
    }

    return (wxWindow *)res;
}

wxWindow *wxWindowBase::FindWindow( const wxString& name )
{
    if ( name == m_windowName )
        return (wxWindow *)this;

    wxWindowBase *res = (wxWindow *)NULL;
    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node && !res; node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        res = child->FindWindow(name);
    }

    return (wxWindow *)res;
}

// ----------------------------------------------------------------------------
// dialog oriented functions
// ----------------------------------------------------------------------------

void wxWindowBase::MakeModal(bool modal)
{
    // Disable all other windows
    if ( IsTopLevel() )
    {
        wxWindowList::Node *node = wxTopLevelWindows.GetFirst();
        while (node)
        {
            wxWindow *win = node->GetData();
            if (win != this)
                win->Enable(!modal);

            node = node->GetNext();
        }
    }
}

bool wxWindowBase::Validate()
{
#if wxUSE_VALIDATORS
    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node; node = node->GetNext() )
    {
        wxWindowBase *child = node->GetData();
        wxValidator *validator = child->GetValidator();
        if ( validator && !validator->Validate((wxWindow *)this) )
        {
            return FALSE;
        }
    }
#endif // wxUSE_VALIDATORS

    return TRUE;
}

bool wxWindowBase::TransferDataToWindow()
{
#if wxUSE_VALIDATORS
    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node; node = node->GetNext() )
    {
        wxWindowBase *child = node->GetData();
        wxValidator *validator = child->GetValidator();
        if ( validator && !validator->TransferToWindow() )
        {
            wxLog *log = wxLog::GetActiveTarget();
            if ( log )
            {
                wxLogWarning(_("Could not transfer data to window"));
                log->Flush();
            }

            return FALSE;
        }
    }
#endif // wxUSE_VALIDATORS

    return TRUE;
}

bool wxWindowBase::TransferDataFromWindow()
{
#if wxUSE_VALIDATORS
    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node; node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        if ( child->GetValidator() &&
             !child->GetValidator()->TransferFromWindow() )
        {
            return FALSE;
        }
    }
#endif // wxUSE_VALIDATORS

    return TRUE;
}

void wxWindowBase::InitDialog()
{
    wxInitDialogEvent event(GetId());
    event.SetEventObject( this );
    GetEventHandler()->ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// tooltips
// ----------------------------------------------------------------------------

#if wxUSE_TOOLTIPS

void wxWindowBase::SetToolTip( const wxString &tip )
{
    // don't create the new tooltip if we already have one
    if ( m_tooltip )
    {
        m_tooltip->SetTip( tip );
    }
    else
    {
        SetToolTip( new wxToolTip( tip ) );
    }

    // setting empty tooltip text does not remove the tooltip any more - use
    // SetToolTip((wxToolTip *)NULL) for this
}

void wxWindowBase::DoSetToolTip(wxToolTip *tooltip)
{
    if ( m_tooltip )
        delete m_tooltip;

    m_tooltip = tooltip;
}

#endif // wxUSE_TOOLTIPS

// ----------------------------------------------------------------------------
// constraints and sizers
// ----------------------------------------------------------------------------

#if wxUSE_CONSTRAINTS

void wxWindowBase::SetConstraints( wxLayoutConstraints *constraints )
{
    if ( m_constraints )
    {
        UnsetConstraints(m_constraints);
        delete m_constraints;
    }
    m_constraints = constraints;
    if ( m_constraints )
    {
        // Make sure other windows know they're part of a 'meaningful relationship'
        if ( m_constraints->left.GetOtherWindow() && (m_constraints->left.GetOtherWindow() != this) )
            m_constraints->left.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->top.GetOtherWindow() && (m_constraints->top.GetOtherWindow() != this) )
            m_constraints->top.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->right.GetOtherWindow() && (m_constraints->right.GetOtherWindow() != this) )
            m_constraints->right.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->bottom.GetOtherWindow() && (m_constraints->bottom.GetOtherWindow() != this) )
            m_constraints->bottom.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->width.GetOtherWindow() && (m_constraints->width.GetOtherWindow() != this) )
            m_constraints->width.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->height.GetOtherWindow() && (m_constraints->height.GetOtherWindow() != this) )
            m_constraints->height.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->centreX.GetOtherWindow() && (m_constraints->centreX.GetOtherWindow() != this) )
            m_constraints->centreX.GetOtherWindow()->AddConstraintReference(this);
        if ( m_constraints->centreY.GetOtherWindow() && (m_constraints->centreY.GetOtherWindow() != this) )
            m_constraints->centreY.GetOtherWindow()->AddConstraintReference(this);
    }
}

// This removes any dangling pointers to this window in other windows'
// constraintsInvolvedIn lists.
void wxWindowBase::UnsetConstraints(wxLayoutConstraints *c)
{
    if ( c )
    {
        if ( c->left.GetOtherWindow() && (c->top.GetOtherWindow() != this) )
            c->left.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->top.GetOtherWindow() && (c->top.GetOtherWindow() != this) )
            c->top.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->right.GetOtherWindow() && (c->right.GetOtherWindow() != this) )
            c->right.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->bottom.GetOtherWindow() && (c->bottom.GetOtherWindow() != this) )
            c->bottom.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->width.GetOtherWindow() && (c->width.GetOtherWindow() != this) )
            c->width.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->height.GetOtherWindow() && (c->height.GetOtherWindow() != this) )
            c->height.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->centreX.GetOtherWindow() && (c->centreX.GetOtherWindow() != this) )
            c->centreX.GetOtherWindow()->RemoveConstraintReference(this);
        if ( c->centreY.GetOtherWindow() && (c->centreY.GetOtherWindow() != this) )
            c->centreY.GetOtherWindow()->RemoveConstraintReference(this);
    }
}

// Back-pointer to other windows we're involved with, so if we delete this
// window, we must delete any constraints we're involved with.
void wxWindowBase::AddConstraintReference(wxWindowBase *otherWin)
{
    if ( !m_constraintsInvolvedIn )
        m_constraintsInvolvedIn = new wxWindowList;
    if ( !m_constraintsInvolvedIn->Find(otherWin) )
        m_constraintsInvolvedIn->Append(otherWin);
}

// REMOVE back-pointer to other windows we're involved with.
void wxWindowBase::RemoveConstraintReference(wxWindowBase *otherWin)
{
    if ( m_constraintsInvolvedIn )
        m_constraintsInvolvedIn->DeleteObject(otherWin);
}

// Reset any constraints that mention this window
void wxWindowBase::DeleteRelatedConstraints()
{
    if ( m_constraintsInvolvedIn )
    {
        wxWindowList::Node *node = m_constraintsInvolvedIn->GetFirst();
        while (node)
        {
            wxWindow *win = node->GetData();
            wxLayoutConstraints *constr = win->GetConstraints();

            // Reset any constraints involving this window
            if ( constr )
            {
                constr->left.ResetIfWin(this);
                constr->top.ResetIfWin(this);
                constr->right.ResetIfWin(this);
                constr->bottom.ResetIfWin(this);
                constr->width.ResetIfWin(this);
                constr->height.ResetIfWin(this);
                constr->centreX.ResetIfWin(this);
                constr->centreY.ResetIfWin(this);
            }

            wxWindowList::Node *next = node->GetNext();
            delete node;
            node = next;
        }

        delete m_constraintsInvolvedIn;
        m_constraintsInvolvedIn = (wxWindowList *) NULL;
    }
}

void wxWindowBase::SetSizer(wxSizer *sizer)
{
    m_windowSizer = sizer;
    if ( sizer )
        sizer->SetSizerParent(this);
}

bool wxWindowBase::Layout()
{
    if ( GetConstraints() )
    {
        int w, h;
        GetClientSize(&w, &h);
        GetConstraints()->width.SetValue(w);
        GetConstraints()->height.SetValue(h);
    }

    // If top level (one sizer), evaluate the sizer's constraints.
    if ( GetSizer() )
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


// Do a phase of evaluating constraints: the default behaviour. wxSizers may
// do a similar thing, but also impose their own 'constraints' and order the
// evaluation differently.
bool wxWindowBase::LayoutPhase1(int *noChanges)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        return constr->SatisfyConstraints(this, noChanges);
    }
    else
        return TRUE;
}

bool wxWindowBase::LayoutPhase2(int *noChanges)
{
    *noChanges = 0;

    // Layout children
    DoPhase(1);
    DoPhase(2);
    return TRUE;
}

// Do a phase of evaluating child constraints
bool wxWindowBase::DoPhase(int phase)
{
    int noIterations = 0;
    int maxIterations = 500;
    int noChanges = 1;
    int noFailures = 0;
    wxWindowList succeeded;
    while ((noChanges > 0) && (noIterations < maxIterations))
    {
        noChanges = 0;
        noFailures = 0;
        wxWindowList::Node *node = GetChildren().GetFirst();
        while (node)
        {
            wxWindow *child = node->GetData();
            if ( !child->IsTopLevel() )
            {
                wxLayoutConstraints *constr = child->GetConstraints();
                if ( constr )
                {
                    if ( !succeeded.Find(child) )
                    {
                        int tempNoChanges = 0;
                        bool success = ( (phase == 1) ? child->LayoutPhase1(&tempNoChanges) : child->LayoutPhase2(&tempNoChanges) ) ;
                        noChanges += tempNoChanges;
                        if ( success )
                        {
                            succeeded.Append(child);
                        }
                    }
                }
            }
            node = node->GetNext();
        }

        noIterations++;
    }

    return TRUE;
}

void wxWindowBase::ResetConstraints()
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
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
    wxWindowList::Node *node = GetChildren().GetFirst();
    while (node)
    {
        wxWindow *win = node->GetData();
        if ( !win->IsTopLevel() )
            win->ResetConstraints();
        node = node->GetNext();
    }
}

// Need to distinguish between setting the 'fake' size for windows and sizers,
// and setting the real values.
void wxWindowBase::SetConstraintSizes(bool recurse)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr && constr->left.GetDone() && constr->right.GetDone( ) &&
            constr->width.GetDone() && constr->height.GetDone())
    {
        int x = constr->left.GetValue();
        int y = constr->top.GetValue();
        int w = constr->width.GetValue();
        int h = constr->height.GetValue();

        // If we don't want to resize this window, just move it...
        if ( (constr->width.GetRelationship() != wxAsIs ) ||
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
    else if ( constr )
    {
        wxChar *windowClass = GetClassInfo()->GetClassName();

        wxString winName;
        if ( GetName() == _T("") )
            winName = _T("unnamed");
        else
            winName = GetName();
        wxLogDebug( _T("Constraint(s) not satisfied for window of type %s, name %s:\n"),
                (const wxChar *)windowClass,
                (const wxChar *)winName);
        if ( !constr->left.GetDone()) wxLogDebug( _T("  unsatisfied 'left' constraint.\n")  );
        if ( !constr->right.GetDone()) wxLogDebug( _T("  unsatisfied 'right' constraint.\n")  );
        if ( !constr->width.GetDone()) wxLogDebug( _T("  unsatisfied 'width' constraint.\n")  );
        if ( !constr->height.GetDone())  wxLogDebug( _T("  unsatisfied 'height' constraint.\n")  );
        wxLogDebug( _T("Please check constraints: try adding AsIs() constraints.\n") );
    }

    if ( recurse )
    {
        wxWindowList::Node *node = GetChildren().GetFirst();
        while (node)
        {
            wxWindow *win = node->GetData();
            if ( !win->IsTopLevel() )
                win->SetConstraintSizes();
            node = node->GetNext();
        }
    }
}

// This assumes that all sizers are 'on' the same window, i.e. the parent of
// this window.
void wxWindowBase::TransformSizerToActual(int *x, int *y) const
{
    if ( !m_sizerParent || m_sizerParent->IsTopLevel() )
        return;

    int xp, yp;
    m_sizerParent->GetPosition(&xp, &yp);
    m_sizerParent->TransformSizerToActual(&xp, &yp);
    *x += xp;
    *y += yp;
}

void wxWindowBase::SizerSetSize(int x, int y, int w, int h)
{
    int xx = x;
    int yy = y;
    TransformSizerToActual(&xx, &yy);
    SetSize(xx, yy, w, h);
}

void wxWindowBase::SizerMove(int x, int y)
{
    int xx = x;
    int yy = y;
    TransformSizerToActual(&xx, &yy);
    Move(xx, yy);
}

// Only set the size/position of the constraint (if any)
void wxWindowBase::SetSizeConstraint(int x, int y, int w, int h)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        if ( x != -1 )
        {
            constr->left.SetValue(x);
            constr->left.SetDone(TRUE);
        }
        if ( y != -1 )
        {
            constr->top.SetValue(y);
            constr->top.SetDone(TRUE);
        }
        if ( w != -1 )
        {
            constr->width.SetValue(w);
            constr->width.SetDone(TRUE);
        }
        if ( h != -1 )
        {
            constr->height.SetValue(h);
            constr->height.SetDone(TRUE);
        }
    }
}

void wxWindowBase::MoveConstraint(int x, int y)
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        if ( x != -1 )
        {
            constr->left.SetValue(x);
            constr->left.SetDone(TRUE);
        }
        if ( y != -1 )
        {
            constr->top.SetValue(y);
            constr->top.SetDone(TRUE);
        }
    }
}

void wxWindowBase::GetSizeConstraint(int *w, int *h) const
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        *w = constr->width.GetValue();
        *h = constr->height.GetValue();
    }
    else
        GetSize(w, h);
}

void wxWindowBase::GetClientSizeConstraint(int *w, int *h) const
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        *w = constr->width.GetValue();
        *h = constr->height.GetValue();
    }
    else
        GetClientSize(w, h);
}

void wxWindowBase::GetPositionConstraint(int *x, int *y) const
{
    wxLayoutConstraints *constr = GetConstraints();
    if ( constr )
    {
        *x = constr->left.GetValue();
        *y = constr->top.GetValue();
    }
    else
        GetPosition(x, y);
}

#endif // wxUSE_CONSTRAINTS

// ----------------------------------------------------------------------------
// do Update UI processing for child controls
// ----------------------------------------------------------------------------

// TODO: should this be implemented for the child window rather
// than the parent? Then you can override it e.g. for wxCheckBox
// to do the Right Thing rather than having to assume a fixed number
// of control classes.
void wxWindowBase::UpdateWindowUI()
{
    wxWindowID id = GetId();
    if ( id > 0 )
    {
        wxUpdateUIEvent event(id);
        event.m_eventObject = this;

        if ( GetEventHandler()->ProcessEvent(event) )
        {
            if ( event.GetSetEnabled() )
                Enable(event.GetEnabled());

            if ( event.GetSetText() )
            {
                wxControl *control = wxDynamicCast(this, wxControl);
                if ( control )
                    control->SetLabel(event.GetText());
            }

#if wxUSE_CHECKBOX
            wxCheckBox *checkbox = wxDynamicCast(this, wxCheckBox);
            if ( checkbox )
            {
                if ( event.GetSetChecked() )
                    checkbox->SetValue(event.GetChecked());
            }
#endif // wxUSE_CHECKBOX

#if wxUSE_RADIOBUTTON
            wxRadioButton *radiobtn = wxDynamicCast(this, wxRadioButton);
            if ( radiobtn )
            {
                if ( event.GetSetChecked() )
                    radiobtn->SetValue(event.GetChecked());
            }
#endif // wxUSE_RADIOBUTTON
        }
    }
}

// ----------------------------------------------------------------------------
// dialog units translations
// ----------------------------------------------------------------------------

wxPoint wxWindowBase::ConvertPixelsToDialog(const wxPoint& pt)
{
    int charWidth = GetCharWidth();
    int charHeight = GetCharHeight();
    wxPoint pt2(-1, -1);
    if (pt.x != -1)
        pt2.x = (int) ((pt.x * 4) / charWidth) ;
    if (pt.y != -1)
        pt2.y = (int) ((pt.y * 8) / charHeight) ;

    return pt2;
}

wxPoint wxWindowBase::ConvertDialogToPixels(const wxPoint& pt)
{
    int charWidth = GetCharWidth();
    int charHeight = GetCharHeight();
    wxPoint pt2(-1, -1);
    if (pt.x != -1)
        pt2.x = (int) ((pt.x * charWidth) / 4) ;
    if (pt.y != -1)
        pt2.y = (int) ((pt.y * charHeight) / 8) ;

    return pt2;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// propagate the colour change event to the subwindows
void wxWindowBase::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    wxWindowList::Node *node = GetChildren().GetFirst();
    while ( node )
    {
        // Only propagate to non-top-level windows
        wxWindow *win = node->GetData();
        if ( !win->IsTopLevel() )
        {
            wxSysColourChangedEvent event2;
            event.m_eventObject = win;
            win->GetEventHandler()->ProcessEvent(event2);
        }

        node = node->GetNext();
    }
}

// the default action is to populate dialog with data when it's created
void wxWindowBase::OnInitDialog( wxInitDialogEvent &WXUNUSED(event) )
{
    TransferDataToWindow();
}

// ----------------------------------------------------------------------------
// list classes implementation
// ----------------------------------------------------------------------------

void wxWindowListNode::DeleteData()
{
    delete (wxWindow *)GetData();
}

