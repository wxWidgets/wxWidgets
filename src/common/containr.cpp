///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/containr.cpp
// Purpose:     implementation of wxControlContainer
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.01
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/containr.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/scrolbar.h"
    #include "wx/radiobut.h"
#endif //WX_PRECOMP

// trace mask for focus messages
#define TRACE_FOCUS wxT("focus")

#if (defined(__WXMSW__) || defined(__WXMAC__)) && wxUSE_RADIOBTN
#define USE_RADIOBTN_NAV
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxControlContainerBase
// ----------------------------------------------------------------------------

void wxControlContainerBase::UpdateParentCanFocus(bool acceptsFocusChildren)
{
    // In the ports where it does something non trivial, the parent window
    // should only be focusable if it doesn't have any focusable children
    // (e.g. native focus handling in wxGTK totally breaks down otherwise).
    m_winParent->SetCanFocus(m_acceptsFocusSelf && !acceptsFocusChildren);
}

bool wxControlContainerBase::UpdateCanFocusChildren()
{
    const bool acceptsFocusChildren = HasAnyFocusableChildren();

    UpdateParentCanFocus(acceptsFocusChildren);

    return acceptsFocusChildren;
}

bool wxControlContainerBase::HasAnyFocusableChildren() const
{
    const wxWindowList& children = m_winParent->GetChildren();
    for ( wxWindowList::const_iterator i = children.begin(),
                                     end = children.end();
          i != end;
          ++i )
    {
        const wxWindow * const child = *i;

        if ( !m_winParent->IsClientAreaChild(child) )
            continue;

        // Here we check whether the child can accept the focus at all, as we
        // want to try focusing it later even if it can't accept it right now.
        if ( child->AcceptsFocusRecursively() )
            return true;
    }

    return false;
}

bool wxControlContainerBase::HasAnyChildrenAcceptingFocus() const
{
    const wxWindowList& children = m_winParent->GetChildren();
    for ( wxWindowList::const_iterator i = children.begin(),
                                     end = children.end();
          i != end;
          ++i )
    {
        const wxWindow * const child = *i;

        if ( !m_winParent->IsClientAreaChild(child) )
            continue;

        // Here we check if the child accepts focus right now as we need to
        // know if we can give the focus to it or not.
        if ( child->CanAcceptFocus() )
            return true;
    }

    return false;
}

bool wxControlContainerBase::DoSetFocus()
{
    wxLogTrace(TRACE_FOCUS, wxT("SetFocus on wxPanel 0x%p."),
               m_winParent->GetHandle());

    if (m_inSetFocus)
        return true;

    // when the panel gets the focus we move the focus to either the last
    // window that had the focus or the first one that can get it unless the
    // focus had been already set to some other child

    wxWindow *win = wxWindow::FindFocus();
    while ( win )
    {
        if ( win == m_winParent )
        {
            // our child already has focus, don't take it away from it
            return true;
        }

        if ( win->IsTopLevel() )
        {
            // don't look beyond the first top level parent - useless and
            // unnecessary
            break;
        }

        win = win->GetParent();
    }

    // protect against infinite recursion:
    m_inSetFocus = true;

    bool ret = SetFocusToChild();

    m_inSetFocus = false;

    return ret;
}

bool wxControlContainerBase::AcceptsFocus() const
{
    return m_acceptsFocusSelf && m_winParent->CanBeFocused();
}

bool wxControlContainerBase::SetFocusToChild()
{
    return wxSetFocusToChild(m_winParent, &m_winLastFocused);
}

#ifdef __WXMSW__

bool wxControlContainerBase::HasTransparentBackground() const
{
    for ( wxWindow *win = m_winParent->GetParent(); win; win = win->GetParent() )
    {
        if ( win->MSWHasInheritableBackground() )
            return true;

        if ( win->IsTopLevel() )
            break;
    }

    return false;
}

#endif // __WXMSW__

#ifndef wxHAS_NATIVE_TAB_TRAVERSAL

// ----------------------------------------------------------------------------
// generic wxControlContainer
// ----------------------------------------------------------------------------

wxControlContainer::wxControlContainer()
{
    m_winLastFocused = nullptr;
}

void wxControlContainer::SetLastFocus(wxWindow *win)
{
    // the panel itself should never get the focus at all but if it does happen
    // temporarily (as it seems to do under wxGTK), at the very least don't
    // forget our previous m_winLastFocused
    if ( win != m_winParent )
    {
        // if we're setting the focus
        if ( win )
        {
            // find the last _immediate_ child which got focus
            wxWindow *winParent = win;
            while ( winParent != m_winParent )
            {
                win = winParent;
                winParent = win->GetParent();

                // Yes, this can happen, though in a totally pathological case.
                // like when detaching a menubar from a frame with a child
                // which has pushed itself as an event handler for the menubar.
                // (under wxGTK)

                wxASSERT_MSG( winParent,
                              wxT("Setting last focus for a window that is not our child?") );
            }
        }

        m_winLastFocused = win;

        if ( win )
        {
            wxLogTrace(TRACE_FOCUS, wxT("Set last focus to %s(%s)"),
                       win->GetClassInfo()->GetClassName(),
                       win->GetLabel().c_str());
        }
        else
        {
            wxLogTrace(TRACE_FOCUS, wxT("No more last focus"));
        }
    }
}

// --------------------------------------------------------------------
// The following functions is used by wxSetFocusToChild()
// --------------------------------------------------------------------

#if defined(USE_RADIOBTN_NAV)

namespace
{

wxRadioButton* wxGetSelectedButtonInGroup(const wxRadioButton *btn)
{
    // Find currently selected button
    if (btn->GetValue())
        return const_cast<wxRadioButton*>(btn);

    if (btn->HasFlag(wxRB_SINGLE))
        return nullptr;

    wxRadioButton *selBtn;

    // First check all previous buttons
    for (selBtn = btn->GetPreviousInGroup(); selBtn; selBtn = selBtn->GetPreviousInGroup())
        if (selBtn->GetValue())
            return selBtn;

    // Now all following buttons
    for (selBtn = btn->GetNextInGroup(); selBtn; selBtn = selBtn->GetNextInGroup())
        if (selBtn->GetValue())
            return selBtn;

    return nullptr;
}

} // anonymous namespace

#endif // USE_RADIOBTN_NAV

// ----------------------------------------------------------------------------
// Keyboard handling - this is the place where the TAB traversal logic is
// implemented. As this code is common to all ports, this ensures consistent
// behaviour even if we don't specify how exactly the wxNavigationKeyEvent are
// generated and this is done in platform specific code which also ensures that
// we can follow the given platform standards.
// ----------------------------------------------------------------------------

void wxControlContainer::HandleOnNavigationKey( wxNavigationKeyEvent& event )
{
    // for a TLW we shouldn't involve the parent window, it has nothing to do
    // with keyboard navigation inside this TLW
    wxWindow *parent = m_winParent->IsTopLevel() ? nullptr
                                                 : m_winParent->GetParent();

    // the event is propagated downwards if the event emitter was our parent
    bool goingDown = event.GetEventObject() == parent;

    const wxWindowList& children = m_winParent->GetChildren();

    // if we have exactly one notebook-like child window (actually it could be
    // any window that returns true from its HasMultiplePages()), then
    // [Shift-]Ctrl-Tab and Ctrl-PageUp/Down keys should iterate over its pages
    // even if the focus is outside of the control because this is how the
    // standard MSW properties dialogs behave and we do it under other platforms
    // as well because it seems like a good idea -- but we can always put this
    // block inside "#ifdef __WXMSW__" if it's not suitable there
    if ( event.IsWindowChange() && !goingDown )
    {
        // check if we have a unique notebook-like child
        wxWindow *bookctrl = nullptr;
        for ( wxWindowList::const_iterator i = children.begin(),
                                         end = children.end();
              i != end;
              ++i )
        {
            wxWindow * const window = *i;
            if ( window->HasMultiplePages() )
            {
                if ( bookctrl )
                {
                    // this is the second book-like control already so don't do
                    // anything as we don't know which one should have its page
                    // changed
                    bookctrl = nullptr;
                    break;
                }

                bookctrl = window;
            }
        }

        if ( bookctrl )
        {
            // make sure that we don't bubble up the event again from the book
            // control resulting in infinite recursion
            wxNavigationKeyEvent eventCopy(event);
            eventCopy.SetEventObject(m_winParent);
            if ( bookctrl->GetEventHandler()->ProcessEvent(eventCopy) )
                return;
        }
    }

    // there is not much to do if we don't have children and we're not
    // interested in "notebook page change" events here
    if ( !children.GetCount() || event.IsWindowChange() )
    {
        // let the parent process it unless it already comes from our parent
        // of we don't have any
        if ( goingDown ||
             !parent || !parent->GetEventHandler()->ProcessEvent(event) )
        {
            event.Skip();
        }

        return;
    }

    // where are we going?
    const bool forward = event.GetDirection();

    // the node of the children list from which we should start looking for the
    // next acceptable child
    wxWindowList::compatibility_iterator node, start_node;

    // we should start from the first/last control and not from the one which
    // had focus the last time if we're propagating the event downwards because
    // for our parent we look like a single control
    if ( goingDown )
    {
        // just to be sure it's not used (normally this is not necessary, but
        // doesn't hurt either)
        m_winLastFocused = nullptr;

        // start from first or last depending on where we're going
        node = forward ? children.GetFirst() : children.GetLast();
    }
    else // going up
    {
        // try to find the child which has the focus currently

        // the event emitter might have done this for us
        wxWindow *winFocus = event.GetCurrentFocus();

        // but if not, we might know where the focus was ourselves
        if (!winFocus)
            winFocus = m_winLastFocused;

        // if still no luck, do it the hard way
        if (!winFocus)
            winFocus = wxWindow::FindFocus();

        if ( winFocus )
        {
#if defined(USE_RADIOBTN_NAV)
            // If we are in a radio button group, start from the first item in the
            // group
            if ( event.IsFromTab() && wxIsKindOf(winFocus, wxRadioButton ) )
                winFocus = static_cast<wxRadioButton*>(winFocus)->GetFirstInGroup();
#endif // USE_RADIOBTN_NAV
            // ok, we found the focus - now is it our child?
            start_node = children.Find( winFocus );
        }

        if ( !start_node && m_winLastFocused )
        {
            // window which has focus isn't our child, fall back to the one
            // which had the focus the last time
            start_node = children.Find( m_winLastFocused );
        }

        // if we still didn't find anything, we should start with the first one
        if ( !start_node )
        {
            start_node = children.GetFirst();
        }

        // and the first child which we can try setting focus to is the next or
        // the previous one
        node = forward ? start_node->GetNext() : start_node->GetPrevious();
    }

    // we want to cycle over all elements passing by nullptr
    for ( ;; )
    {
        // don't go into infinite loop
        if ( start_node && node && node == start_node )
            break;

        // Have we come to the last or first item on the panel?
        if ( !node )
        {
            if ( !start_node )
            {
                // exit now as otherwise we'd loop forever
                break;
            }

            if ( !goingDown )
            {
                // Check if our (maybe grand) parent is another panel: if this
                // is the case, they will know what to do with this navigation
                // key and so give them the chance to process it instead of
                // looping inside this panel (normally, the focus will go to
                // the next/previous item after this panel in the parent
                // panel).
                wxWindow *focusedParent = m_winParent;
                while ( parent )
                {
                    // We don't want to tab into a different dialog or frame or
                    // even an MDI child frame, so test for this explicitly
                    // (and in particular don't just use IsTopLevel() which
                    // would return false in the latter case).
                    if ( focusedParent->IsTopNavigationDomain(wxWindow::Navigation_Tab) )
                        break;

                    event.SetCurrentFocus( focusedParent );
                    if ( parent->GetEventHandler()->ProcessEvent( event ) )
                        return;

                    focusedParent = parent;

                    parent = parent->GetParent();
                }
            }
            //else: as the focus came from our parent, we definitely don't want
            //      to send it back to it!

            // no, we are not inside another panel so process this ourself
            node = forward ? children.GetFirst() : children.GetLast();

            continue;
        }

        wxWindow *child = node->GetData();

        // don't TAB to another TLW
        if ( child->IsTopLevel() )
        {
            node = forward ? node->GetNext() : node->GetPrevious();

            continue;
        }

#if defined(USE_RADIOBTN_NAV)
        if ( event.IsFromTab() )
        {
            if ( wxIsKindOf(child, wxRadioButton) )
            {
                // only radio buttons with either wxRB_GROUP or wxRB_SINGLE
                // can be tabbed to
                if ( child->HasFlag(wxRB_GROUP) )
                {
                    // need to tab into the active button within a group
                    wxRadioButton *rb = wxGetSelectedButtonInGroup((wxRadioButton*)child);
                    if ( rb )
                        child = rb;
                }
                else if ( !child->HasFlag(wxRB_SINGLE) )
                {
                    node = forward ? node->GetNext() : node->GetPrevious();
                    continue;
                }
            }
        }
        else if ( m_winLastFocused &&
                  wxIsKindOf(m_winLastFocused, wxRadioButton) &&
                  !m_winLastFocused->HasFlag(wxRB_SINGLE) )
        {
            wxRadioButton * const
                lastBtn = static_cast<wxRadioButton *>(m_winLastFocused);

            // cursor keys don't navigate out of a radio button group so
            // find the correct radio button to focus
            if ( forward )
            {
                child = lastBtn->GetNextInGroup();
                if ( !child )
                {
                    // no next button in group, set it to the first button
                    child = lastBtn->GetFirstInGroup();
                }
            }
            else
            {
                child = lastBtn->GetPreviousInGroup();
                if ( !child )
                {
                    // no previous button in group, set it to the last button
                    child = lastBtn->GetLastInGroup();
                }
            }

            if ( child == m_winLastFocused )
            {
                // must be a group consisting of only one button therefore
                // no need to send a navigation event
                event.Skip(false);
                return;
            }
        }
#endif // USE_RADIOBTN_NAV

        if ( child->CanAcceptFocusFromKeyboard() )
        {
            // if we're setting the focus to a child panel we should prevent it
            // from giving it to the child which had the focus the last time
            // and instead give it to the first/last child depending from which
            // direction we're coming
            event.SetEventObject(m_winParent);

            // disable propagation for this call as otherwise the event might
            // bounce back to us.
            wxPropagationDisabler disableProp(event);
            if ( !child->GetEventHandler()->ProcessEvent(event) )
            {
                // set it first in case SetFocusFromKbd() results in focus
                // change too
                m_winLastFocused = child;

                // everything is simple: just give focus to it
                child->SetFocusFromKbd();
            }
            //else: the child manages its focus itself

            event.Skip( false );

            return;
        }

        node = forward ? node->GetNext() : node->GetPrevious();
    }

    // we cycled through all of our children and none of them wanted to accept
    // focus
    event.Skip();
}

void wxControlContainer::HandleOnWindowDestroy(wxWindowBase *child)
{
    if ( child == m_winLastFocused )
        m_winLastFocused = nullptr;
}

// ----------------------------------------------------------------------------
// focus handling
// ----------------------------------------------------------------------------

void wxControlContainer::HandleOnFocus(wxFocusEvent& event)
{
    wxLogTrace(TRACE_FOCUS, wxT("OnFocus on wxPanel 0x%p, name: %s"),
               m_winParent->GetHandle(),
               m_winParent->GetName().c_str() );

    DoSetFocus();

    event.Skip();
}


#else
  // wxHAS_NATIVE_TAB_TRAVERSAL

bool wxControlContainer::SetFocusToChild()
{
    return wxSetFocusToChild(m_winParent, nullptr);
}


#endif // !wxHAS_NATIVE_TAB_TRAVERSAL

// ----------------------------------------------------------------------------
// SetFocusToChild(): this function is used by wxPanel but also by wxFrame in
// wxMSW, this is why it is outside of wxControlContainer class
// ----------------------------------------------------------------------------

bool wxSetFocusToChild(wxWindow *win, wxWindow **childLastFocused)
{
    wxCHECK_MSG( win, false, wxT("wxSetFocusToChild(): invalid window") );
    //    wxCHECK_MSG( childLastFocused, false,
    //             wxT("wxSetFocusToChild(): null child pointer") );

    if ( childLastFocused && *childLastFocused )
    {
        // It might happen that the window got reparented
        if ( (*childLastFocused)->GetParent() == win )
        {
            // And it also could have become hidden in the meanwhile
            // We want to focus on the deepest widget visible
            wxWindow *deepestVisibleWindow = nullptr;

            while ( *childLastFocused )
            {
                if ( (*childLastFocused)->IsShown() )
                {
                    if ( !deepestVisibleWindow )
                        deepestVisibleWindow = *childLastFocused;
                }
                else
                    deepestVisibleWindow = nullptr;

                // We shouldn't be looking for the child to focus beyond the
                // TLW boundary. And we use IsTopNavigationDomain() here
                // instead of IsTopLevel() because wxMDIChildFrame is also TLW
                // from this point of view.
                if ( (*childLastFocused)->
                        IsTopNavigationDomain(wxWindow::Navigation_Tab) )
                {
                    break;
                }

                *childLastFocused = (*childLastFocused)->GetParent();
            }

            if ( deepestVisibleWindow )
            {
                *childLastFocused = deepestVisibleWindow;

                wxLogTrace(TRACE_FOCUS,
                           wxT("SetFocusToChild() => last child (0x%p)."),
                           (*childLastFocused)->GetHandle());

                // not SetFocusFromKbd(): we're restoring focus back to the old
                // window and not setting it as the result of a kbd action
                (*childLastFocused)->SetFocus();
                return true;
            }
        }
        else
        {
            // it doesn't count as such any more
            *childLastFocused = nullptr;
        }
    }

    // set the focus to the first child who wants it
    wxWindowList::compatibility_iterator node = win->GetChildren().GetFirst();
    while ( node )
    {
        wxWindow *child = node->GetData();
        node = node->GetNext();

        // skip special windows:
        if ( !win->IsClientAreaChild(child) )
            continue;

        if ( child->CanAcceptFocusFromKeyboard() && !child->IsTopLevel() )
        {
#if defined(USE_RADIOBTN_NAV)
            // If a radiobutton is the first focusable child, search for the
            // selected radiobutton in the same group
            wxRadioButton* btn = wxDynamicCast(child, wxRadioButton);
            if (btn)
            {
                wxRadioButton* selected = wxGetSelectedButtonInGroup(btn);
                if (selected)
                    child = selected;
            }
#endif // USE_RADIOBTN_NAV

            wxLogTrace(TRACE_FOCUS,
                       wxT("SetFocusToChild() => first child (0x%p)."),
                       child->GetHandle());

            if (childLastFocused)
                *childLastFocused = child;
            child->SetFocusFromKbd();
            return true;
        }
    }

    return false;
}

