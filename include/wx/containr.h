///////////////////////////////////////////////////////////////////////////////
// Name:        wx/containr.h
// Purpose:     wxControlContainer class declration: a "mix-in" class which
//              implements the TAB navigation between the controls
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.01
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTAINR_H_
#define _WX_CONTAINR_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "containr.h"
#endif

class WXDLLEXPORT wxFocusEvent;
class WXDLLEXPORT wxNavigationKeyEvent;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxWindowBase;

/*
   Implementation note: wxControlContainer is not a real mix-in but rather
   a class meant to be agregated with (and not inherited from). Although
   logically it should be a mix-in, doing it like this has no advantage from
   the point of view of the existing code but does have some problems (we'd
   need to play tricks with event handlers which may be difficult to do
   safely). The price we pay for this simplicity is the ugly macros below.
 */

// ----------------------------------------------------------------------------
// wxControlContainer
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlContainer
{
public:
    // ctors and such
    wxControlContainer(wxWindow *winParent = NULL);
    void SetContainerWindow(wxWindow *winParent) { m_winParent = winParent; }

    // default item access: we have a permanent default item which is the one
    // set by the user code but we may also have a temporary default item which
    // would be chosen if the user pressed "Enter" now but the default action
    // reverts to the "permanent" default as soon as this temporary default
    // item lsoes focus

    // get the default item, temporary or permanent
    wxWindow *GetDefaultItem() const
        { return m_winTmpDefault ? m_winTmpDefault : m_winDefault; }

    // set the permanent default item, return its old value
    wxWindow *SetDefaultItem(wxWindow *win)
        { wxWindow *winOld = m_winDefault; m_winDefault = win; return winOld; }

    // set a temporary default item, SetTmpDefaultItem(NULL) should be called
    // soon after a call to SetTmpDefaultItem(window)
    void SetTmpDefaultItem(wxWindow *win) { m_winTmpDefault = win; }

    // the methods to be called from the window event handlers
    void HandleOnNavigationKey(wxNavigationKeyEvent& event);
    void HandleOnFocus(wxFocusEvent& event);
    void HandleOnWindowDestroy(wxWindowBase *child);

    // should be called from SetFocus(), returns false if we did nothing with
    // the focus and the default processing should take place
    bool DoSetFocus();

    // can our child get the focus?
    bool AcceptsFocus() const;

    // called from OnChildFocus() handler, i.e. when one of our (grand)
    // children gets the focus
    void SetLastFocus(wxWindow *win);

protected:
    // set the focus to the child which had it the last time
    bool SetFocusToChild();

    // the parent window we manage the children for
    wxWindow *m_winParent;

    // the child which had the focus last time this panel was activated
    wxWindow *m_winLastFocused;

    // a default window (usually a button) or NULL
    wxWindow *m_winDefault;

    // a temporary override of m_winDefault, use the latter if NULL
    wxWindow *m_winTmpDefault;

    // a guard against infinite recursion
    bool m_inSetFocus;

    DECLARE_NO_COPY_CLASS(wxControlContainer)
};

// this function is for wxWidgets internal use only
extern bool wxSetFocusToChild(wxWindow *win, wxWindow **child);

// ----------------------------------------------------------------------------
// macros which may be used by the classes wishing to implement TAB navigation
// among their children
// ----------------------------------------------------------------------------

// declare the methods to be forwarded
#define WX_DECLARE_CONTROL_CONTAINER() \
public: \
    void OnNavigationKey(wxNavigationKeyEvent& event); \
    void OnFocus(wxFocusEvent& event); \
    virtual void OnChildFocus(wxChildFocusEvent& event); \
    virtual void SetFocus(); \
    virtual void RemoveChild(wxWindowBase *child); \
    virtual wxWindow *GetDefaultItem() const; \
    virtual wxWindow *SetDefaultItem(wxWindow *child); \
    virtual void SetTmpDefaultItem(wxWindow *win); \
    virtual bool AcceptsFocus() const; \
\
protected: \
    wxControlContainer m_container

// implement the event table entries for wxControlContainer
#define WX_EVENT_TABLE_CONTROL_CONTAINER(classname) \
    EVT_SET_FOCUS(classname::OnFocus) \
    EVT_CHILD_FOCUS(classname::OnChildFocus) \
    EVT_NAVIGATION_KEY(classname::OnNavigationKey)

// implement the methods forwarding to the wxControlContainer
#define WX_DELEGATE_TO_CONTROL_CONTAINER(classname)  \
wxWindow *classname::SetDefaultItem(wxWindow *child) \
{ \
    return m_container.SetDefaultItem(child); \
} \
 \
void classname::SetTmpDefaultItem(wxWindow *child) \
{ \
    m_container.SetTmpDefaultItem(child); \
} \
 \
wxWindow *classname::GetDefaultItem() const \
{ \
    return m_container.GetDefaultItem(); \
} \
 \
void classname::OnNavigationKey( wxNavigationKeyEvent& event ) \
{ \
    m_container.HandleOnNavigationKey(event); \
} \
 \
void classname::RemoveChild(wxWindowBase *child) \
{ \
    m_container.HandleOnWindowDestroy(child); \
 \
    wxWindow::RemoveChild(child); \
} \
 \
void classname::SetFocus() \
{ \
    if ( !m_container.DoSetFocus() ) \
        wxWindow::SetFocus(); \
} \
 \
void classname::OnChildFocus(wxChildFocusEvent& event) \
{ \
    m_container.SetLastFocus(event.GetWindow()); \
} \
 \
void classname::OnFocus(wxFocusEvent& event) \
{ \
    m_container.HandleOnFocus(event); \
} \
bool classname::AcceptsFocus() const \
{ \
    return m_container.AcceptsFocus(); \
}


#endif // _WX_CONTAINR_H_
