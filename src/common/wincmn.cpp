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
    #include "wx/control.h"
    #include "wx/checkbox.h"
    #include "wx/radiobut.h"
    #include "wx/textctrl.h"
    #include "wx/settings.h"
    #include "wx/dialog.h"
    #include "wx/msgdlg.h"
    #include "wx/statusbr.h"
#endif //WX_PRECOMP

#if wxUSE_CONSTRAINTS
    #include "wx/layout.h"
    #include "wx/sizer.h"
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
    EVT_MIDDLE_DOWN(wxWindowBase::OnMiddleClick)
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

    // no client data (yet)
    m_clientData = NULL;
    m_clientDataType = ClientData_None;

    // the default event handler is just this window
    m_eventHandler = this;

#if wxUSE_VALIDATORS
    // no validator
    m_windowValidator = (wxValidator *) NULL;
#endif // wxUSE_VALIDATORS

    // use the system default colours
    wxSystemSettings settings;

    m_backgroundColour = settings.GetSystemColour(wxSYS_COLOUR_BTNFACE);
    // m_foregroundColour = *wxBLACK;  // TODO take this from sys settings too?
    m_foregroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOWTEXT);

    // GRG, changed Mar/2000
    m_font = settings.GetSystemFont(wxSYS_DEFAULT_GUI_FONT);
    // no style bits
    m_exStyle =
    m_windowStyle = 0;

    // an optimization for the event processing: checking this flag is much
    // faster than using IsKindOf(CLASSINFO(wxWindow))
    m_isWindow = TRUE;

#if wxUSE_CONSTRAINTS
    // no constraints whatsoever
    m_constraints = (wxLayoutConstraints *) NULL;
    m_constraintsInvolvedIn = (wxWindowList *) NULL;
    m_windowSizer = (wxSizer *) NULL;
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
                              const wxValidator& validator,
                              const wxString& name)
{
    // m_isWindow is set to TRUE in wxWindowBase::Init() as well as many other
    // member variables - check that it has been called (will catch the case
    // when a new ctor is added which doesn't call InitWindow)
    wxASSERT_MSG( m_isWindow, wxT("Init() must have been called before!") );

    // generate a new id if the user doesn't care about it
    m_windowId = id == -1 ? NewControlId() : id;

    SetName(name);
    SetWindowStyleFlag(style);
    SetParent(parent);

#if wxUSE_VALIDATORS
    SetValidator(validator);
#endif // wxUSE_VALIDATORS

    // if the parent window has wxWS_EX_VALIDATE_RECURSIVELY set, we want to
    // have it too - like this it's possible to set it only in the top level
    // dialog/frame and all children will inherit it by defult
    if ( parent && (parent->GetExtraStyle() & wxWS_EX_VALIDATE_RECURSIVELY) )
    {
        SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    }

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

    wxASSERT_MSG( GetChildren().GetCount() == 0, wxT("children not destroyed") );

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

    // we only delete object data, not untyped
    if ( m_clientDataType == ClientData_Object )
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

        wxASSERT_MSG( child, wxT("children list contains empty nodes") );

        delete child;

        wxASSERT_MSG( !GetChildren().Find(child),
                      wxT("child didn't remove itself using RemoveChild()") );
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// size/position related methods
// ----------------------------------------------------------------------------

// centre the window with respect to its parent in either (or both) directions
void wxWindowBase::Centre(int direction)
{
    // the position/size of the parent window or of the entire screen
    wxPoint posParent;
    int widthParent, heightParent;

    wxWindow *parent = NULL;

    if ( !(direction & wxCENTRE_ON_SCREEN) )
    {
        // find the parent to centre this window on: it should be the
        // immediate parent for the controls but the top level parent for the
        // top level windows (like dialogs)
        parent = GetParent();
        if ( IsTopLevel() )
        {
            while ( parent && !parent->IsTopLevel() )
            {
                parent = parent->GetParent();
            }
        }

        // did we find the parent?
        if ( !parent )
        {
            // no other choice
            direction |= wxCENTRE_ON_SCREEN;
        }
    }

    if ( direction & wxCENTRE_ON_SCREEN )
    {
        // centre with respect to the whole screen
        wxDisplaySize(&widthParent, &heightParent);
    }
    else
    {
        if ( IsTopLevel() )
        {
            // centre on the parent
            parent->GetSize(&widthParent, &heightParent);

            // adjust to the parents position
            posParent = parent->GetPosition();
        }
        else
        {
            // centre inside the parents client rectangle
            parent->GetClientSize(&widthParent, &heightParent);
        }
    }

    int width, height;
    GetSize(&width, &height);

    int xNew = -1,
        yNew = -1;

    if ( direction & wxHORIZONTAL )
        xNew = (widthParent - width)/2;

    if ( direction & wxVERTICAL )
        yNew = (heightParent - height)/2;

    xNew += posParent.x;
    yNew += posParent.y;

    // move the centre of this window to this position
    Move(xNew, yNew);
}

// fits the window around the children
void wxWindowBase::Fit()
{
    if ( GetChildren().GetCount() > 0 )
    {
        SetClientSize(DoGetBestSize());
    }
    //else: do nothing if we have no children
}

// return the size best suited for the current window
wxSize wxWindowBase::DoGetBestSize() const
{
    if ( GetChildren().GetCount() > 0 )
    {
        // our minimal acceptable size is such that all our windows fit inside
        int maxX = 0,
            maxY = 0;

        for ( wxWindowList::Node *node = GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            wxWindow *win = node->GetData();
            if ( win->IsTopLevel()
#if wxUSE_STATUSBAR
                    || wxDynamicCast(win, wxStatusBar)
#endif // wxUSE_STATUSBAR
               )
            {
                // dialogs and frames lie in different top level windows -
                // don't deal with them here; as for the status bars, they
                // don't lie in the client area at all
                continue;
            }

            int wx, wy, ww, wh;
            win->GetPosition(&wx, &wy);

            // if the window hadn't been positioned yet, assume that it is in
            // the origin
            if ( wx == -1 )
                wx = 0;
            if ( wy == -1 )
                wy = 0;

            win->GetSize(&ww, &wh);
            if ( wx + ww > maxX )
                maxX = wx + ww;
            if ( wy + wh > maxY )
                maxY = wy + wh;
        }

        // leave a margin
        return wxSize(maxX + 7, maxY + 14);
    }
    else
    {
        // for a generic window there is no natural best size - just use the
        // current one
        return GetSize();
    }
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
    return FALSE;
}

// ----------------------------------------------------------------------------
// reparenting the window
// ----------------------------------------------------------------------------

void wxWindowBase::AddChild(wxWindowBase *child)
{
    wxCHECK_RET( child, wxT("can't add a NULL child") );

    // this should never happen and it will lead to a crash later if it does
    // because RemoveChild() will remove only one node from the children list
    // and the other(s) one(s) will be left with dangling pointers in them
    wxASSERT_MSG( !GetChildren().Find(child), _T("AddChild() called twice") );

    GetChildren().Append(child);
    child->SetParent(this);
}

void wxWindowBase::RemoveChild(wxWindowBase *child)
{
    wxCHECK_RET( child, wxT("can't remove a NULL child") );

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
    // setting an invalid cursor is ok, it means that we don't have any special
    // cursor
    if ( m_cursor == cursor )
    {
        // no change
        return FALSE;
    }

    m_cursor = cursor;

    return TRUE;
}

bool wxWindowBase::SetFont(const wxFont& font)
{
    // don't try to set invalid font, always fall back to the default
    const wxFont& fontOk = font.Ok() ? font : *wxSWISS_FONT;

    if ( (wxFont&)fontOk == m_font )
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
                      wxT("caret should be created associated to this window") );
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
    bool recurse = (GetExtraStyle() & wxWS_EX_VALIDATE_RECURSIVELY) != 0;

    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node; node = node->GetNext() )
    {
        wxWindowBase *child = node->GetData();
        wxValidator *validator = child->GetValidator();
        if ( validator && !validator->Validate((wxWindow *)this) )
        {
            return FALSE;
        }

        if ( recurse && !child->Validate() )
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
    bool recurse = (GetExtraStyle() & wxWS_EX_VALIDATE_RECURSIVELY) != 0;

    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node; node = node->GetNext() )
    {
        wxWindowBase *child = node->GetData();
        wxValidator *validator = child->GetValidator();
        if ( validator && !validator->TransferToWindow() )
        {
            wxLogWarning(_("Could not transfer data to window"));
            wxLog::FlushActive();

            return FALSE;
        }

        if ( recurse )
        {
            if ( !child->TransferDataToWindow() )
            {
                // warning already given
                return FALSE;
            }
        }
    }
#endif // wxUSE_VALIDATORS

    return TRUE;
}

bool wxWindowBase::TransferDataFromWindow()
{
#if wxUSE_VALIDATORS
    bool recurse = (GetExtraStyle() & wxWS_EX_VALIDATE_RECURSIVELY) != 0;

    wxWindowList::Node *node;
    for ( node = m_children.GetFirst(); node; node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        wxValidator *validator = child->GetValidator();
        if ( validator && !validator->TransferFromWindow() )
        {
            // nop warning here because the application is supposed to give
            // one itself - we don't know here what might have gone wrongly

            return FALSE;
        }

        if ( recurse )
        {
            if ( !child->TransferDataFromWindow() )
            {
                // warning already given
                return FALSE;
            }
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
    if (m_windowSizer) delete m_windowSizer;

    m_windowSizer = sizer;
}

bool wxWindowBase::Layout()
{
    // If there is a sizer, use it instead of the constraints
    if ( GetSizer() )
    {
        int w, h;
        GetClientSize(&w, &h);

        GetSizer()->SetDimension( 0, 0, w, h );
    }
    else
    {
        wxLayoutConstraints *constr = GetConstraints();
        bool wasOk = constr && constr->AreSatisfied();

        ResetConstraints();   // Mark all constraints as unevaluated

        // if we're a top level panel (i.e. our parent is frame/dialog), our
        // own constraints will never be satisfied any more unless we do it
        // here
        if ( wasOk )
        {
            int noChanges = 1;
            while ( noChanges > 0 )
            {
                constr->SatisfyConstraints(this, &noChanges);
            }
        }

        DoPhase(1);           // Layout children
        DoPhase(2);           // Layout grand children
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
    if ( constr && constr->AreSatisfied() )
    {
        int x = constr->left.GetValue();
        int y = constr->top.GetValue();
        int w = constr->width.GetValue();
        int h = constr->height.GetValue();

        if ( (constr->width.GetRelationship() != wxAsIs ) ||
             (constr->height.GetRelationship() != wxAsIs) )
        {
            SetSize(x, y, w, h);
        }
        else
        {
            // If we don't want to resize this window, just move it...
            Move(x, y);
        }
    }
    else if ( constr )
    {
        wxLogDebug(wxT("Constraints not satisfied for %s named '%s'."),
                   GetClassInfo()->GetClassName(),
                   GetName().c_str());
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
#if wxUSE_CONTROLS
    wxUpdateUIEvent event(GetId());
    event.m_eventObject = this;

    if ( GetEventHandler()->ProcessEvent(event) )
    {
        if ( event.GetSetEnabled() )
            Enable(event.GetEnabled());

        if ( event.GetSetText() )
        {
            wxControl *control = wxDynamicCast(this, wxControl);
            if ( control )
            {
#if wxUSE_TEXTCTRL
                wxTextCtrl *text = wxDynamicCast(control, wxTextCtrl);
                if ( text )
                    text->SetValue(event.GetText());
                else
#endif // wxUSE_TEXTCTRL
                    control->SetLabel(event.GetText());
            }
        }

#if wxUSE_CHECKBOX
        wxCheckBox *checkbox = wxDynamicCast(this, wxCheckBox);
        if ( checkbox )
        {
            if ( event.GetSetChecked() )
                checkbox->SetValue(event.GetChecked());
        }
#endif // wxUSE_CHECKBOX

#if wxUSE_RADIOBTN
        wxRadioButton *radiobtn = wxDynamicCast(this, wxRadioButton);
        if ( radiobtn )
        {
            if ( event.GetSetChecked() )
                radiobtn->SetValue(event.GetChecked());
        }
#endif // wxUSE_RADIOBTN
    }
#endif // wxUSE_CONTROLS
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
// client data
// ----------------------------------------------------------------------------

void wxWindowBase::DoSetClientObject( wxClientData *data )
{
    wxASSERT_MSG( m_clientDataType != ClientData_Void,
                  wxT("can't have both object and void client data") );

    if ( m_clientObject )
        delete m_clientObject;

    m_clientObject = data;
    m_clientDataType = ClientData_Object;
}

wxClientData *wxWindowBase::DoGetClientObject() const
{
    // it's not an error to call GetClientObject() on a window which doesn't
    // have client data at all - NULL will be returned
    wxASSERT_MSG( m_clientDataType != ClientData_Void,
                  wxT("this window doesn't have object client data") );

    return m_clientObject;
}

void wxWindowBase::DoSetClientData( void *data )
{
    wxASSERT_MSG( m_clientDataType != ClientData_Object,
                  wxT("can't have both object and void client data") );

    m_clientData = data;
    m_clientDataType = ClientData_Void;
}

void *wxWindowBase::DoGetClientData() const
{
    // it's not an error to call GetClientData() on a window which doesn't have
    // client data at all - NULL will be returned
    wxASSERT_MSG( m_clientDataType != ClientData_Object,
                  wxT("this window doesn't have void client data") );

    return m_clientData;
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

// process Ctrl-Alt-mclick
void wxWindowBase::OnMiddleClick( wxMouseEvent& event )
{
#if wxUSE_MSGDLG
    if ( event.ControlDown() && event.AltDown() )
    {
        // don't translate these strings
        wxString port;
        switch ( wxGetOsVersion() )
        {
            case wxMOTIF_X:     port = _T("Motif"); break;
            case wxMACINTOSH:   port = _T("Mac"); break;
            case wxBEOS:        port = _T("BeOS"); break;
            case wxGTK:
            case wxGTK_WIN32:
            case wxGTK_OS2:
            case wxGTK_BEOS:    port = _T("GTK"); break;
            case wxWINDOWS:
            case wxPENWINDOWS:
            case wxWINDOWS_NT:
            case wxWIN32S:
            case wxWIN95:
            case wxWIN386:      port = _T("MS Windows"); break;
            case wxMGL_UNIX:
            case wxMGL_X:
            case wxMGL_WIN32:
            case wxMGL_OS2:     port = _T("MGL"); break;
            case wxWINDOWS_OS2:
            case wxOS2_PM:      port = _T("OS/2"); break;
            default:            port = _T("unknown"); break;
        }

        wxMessageBox(wxString::Format(
                                      _T(
                                        "       wxWindows Library (%s port)\nVersion %u.%u.%u, compiled at %s %s\n   Copyright (c) 1995-2000 wxWindows team"
                                        ),
                                      port.c_str(),
                                      wxMAJOR_VERSION,
                                      wxMINOR_VERSION,
                                      wxRELEASE_NUMBER,
                                      __DATE__,
                                      __TIME__
                                     ),
                     _T("wxWindows information"),
                     wxICON_INFORMATION | wxOK,
                     (wxWindow *)this);
    }
    else
#endif // wxUSE_MSGDLG
    {
        event.Skip();
    }
}

// ----------------------------------------------------------------------------
// list classes implementation
// ----------------------------------------------------------------------------

void wxWindowListNode::DeleteData()
{
    delete (wxWindow *)GetData();
}

