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

#ifdef wxHAS_NATIVE_TAB_TRAVERSAL

#define WX_DECLARE_CONTROL_CONTAINER() \
    virtual bool AcceptsFocus() const { return false; } \
    void SetFocusIgnoringChildren() { SetFocus(); }

#define WX_INIT_CONTROL_CONTAINER()
#define WX_EVENT_TABLE_CONTROL_CONTAINER(classname)
#define WX_DELEGATE_TO_CONTROL_CONTAINER(classname, basename)

#else // !wxHAS_NATIVE_TAB_TRAVERSAL

class WXDLLEXPORT wxFocusEvent;
class WXDLLEXPORT wxNavigationKeyEvent;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxWindowBase;

/*
   Implementation note: wxControlContainer is not a real mix-in but rather
   a class meant to be aggregated with (and not inherited from). Although
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
    virtual void SetFocusIgnoringChildren(); \
    virtual void RemoveChild(wxWindowBase *child); \
    virtual bool AcceptsFocus() const; \
\
protected: \
    wxControlContainer m_container

// this macro must be used in the derived class ctor
#define WX_INIT_CONTROL_CONTAINER() \
    m_container.SetContainerWindow(this)

// implement the event table entries for wxControlContainer
#define WX_EVENT_TABLE_CONTROL_CONTAINER(classname) \
    EVT_SET_FOCUS(classname::OnFocus) \
    EVT_CHILD_FOCUS(classname::OnChildFocus) \
    EVT_NAVIGATION_KEY(classname::OnNavigationKey)

// implement the methods forwarding to the wxControlContainer
#define WX_DELEGATE_TO_CONTROL_CONTAINER(classname, basename)  \
void classname::OnNavigationKey( wxNavigationKeyEvent& event ) \
{ \
    m_container.HandleOnNavigationKey(event); \
} \
 \
void classname::RemoveChild(wxWindowBase *child) \
{ \
    m_container.HandleOnWindowDestroy(child); \
 \
    basename::RemoveChild(child); \
} \
 \
void classname::SetFocus() \
{ \
    if ( !m_container.DoSetFocus() ) \
        basename::SetFocus(); \
} \
 \
void classname::SetFocusIgnoringChildren() \
{ \
        basename::SetFocus(); \
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

#endif // wxHAS_NATIVE_TAB_TRAVERSAL/!wxHAS_NATIVE_TAB_TRAVERSAL

#endif // _WX_CONTAINR_H_
