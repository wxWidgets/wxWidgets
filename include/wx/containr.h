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

#include "wx/defs.h"

/*
   Implementation note: wxControlContainer is not a real mix-in but rather
   a class meant to be aggregated with (and not inherited from). Although
   logically it should be a mix-in, doing it like this has no advantage from
   the point of view of the existing code but does have some problems (we'd
   need to play tricks with event handlers which may be difficult to do
   safely). The price we pay for this simplicity is the ugly macros below.
 */

// ----------------------------------------------------------------------------
// wxControlContainerBase: common part used in both native and generic cases
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlContainerBase
{
public:
    // default ctor, SetContainerWindow() must be called later
    wxControlContainerBase()
    {
        m_winParent = NULL;

        // do accept focus initially, we'll stop doing it if/when any children
        // are added
        m_acceptsFocus = true;
    }

    void SetContainerWindow(wxWindow *winParent)
    {
        wxASSERT_MSG( !m_winParent, _T("shouldn't be called twice") );

        m_winParent = winParent;
    }

    // should be called when we decide that we should [stop] accepting focus
    void SetCanFocus(bool acceptsFocus);

    // returns whether we should accept focus ourselves or not
    bool AcceptsFocus() const { return m_acceptsFocus; }

    // call this when the number of children of the window changes
    void UpdateCanFocus() { SetCanFocus(ShouldAcceptFocus()); }

protected:
    // return true if we should be focusable
    bool ShouldAcceptFocus() const;

private:
    // the parent window we manage the children for
    wxWindow *m_winParent;

    // value returned by AcceptsFocus(), should be changed using SetCanFocus()
    // only
    bool m_acceptsFocus;
};

// common part of WX_DECLARE_CONTROL_CONTAINER in the native and generic cases,
// it should be used in the wxWindow-derived class declaration
#define WX_DECLARE_CONTROL_CONTAINER_BASE()                                   \
public:                                                                       \
    virtual bool AcceptsFocus() const;                                        \
    virtual void AddChild(wxWindowBase *child);                               \
    virtual void RemoveChild(wxWindowBase *child);                            \
    void SetFocusIgnoringChildren();                                          \
    void AcceptFocus(bool acceptFocus)                                        \
    {                                                                         \
        m_container.SetCanFocus(acceptFocus);                                 \
    }                                                                         \
                                                                              \
protected:                                                                    \
    wxControlContainer m_container

// this macro must be used in the derived class ctor
#define WX_INIT_CONTROL_CONTAINER() \
    m_container.SetContainerWindow(this)

// common part of WX_DELEGATE_TO_CONTROL_CONTAINER in the native and generic
// cases, must be used in the wxWindow-derived class implementation
#define WX_DELEGATE_TO_CONTROL_CONTAINER_BASE(classname, basename)            \
    void classname::AddChild(wxWindowBase *child)                             \
    {                                                                         \
        basename::AddChild(child);                                            \
                                                                              \
        m_container.UpdateCanFocus();                                         \
    }                                                                         \
                                                                              \
    bool classname::AcceptsFocus() const                                      \
    {                                                                         \
        return m_container.AcceptsFocus();                                    \
    }


#ifdef wxHAS_NATIVE_TAB_TRAVERSAL

// ----------------------------------------------------------------------------
// wxControlContainer for native TAB navigation
// ----------------------------------------------------------------------------

// this must be a real class as we forward-declare it elsewhere
class WXDLLEXPORT wxControlContainer : public wxControlContainerBase
{
};

#define WX_EVENT_TABLE_CONTROL_CONTAINER(classname)

#define WX_DECLARE_CONTROL_CONTAINER WX_DECLARE_CONTROL_CONTAINER_BASE

#define WX_DELEGATE_TO_CONTROL_CONTAINER(classname, basename)                 \
    WX_DELEGATE_TO_CONTROL_CONTAINER_BASE(classname, basename)                \
                                                                              \
    void classname::RemoveChild(wxWindowBase *child)                          \
    {                                                                         \
        basename::RemoveChild(child);                                         \
                                                                              \
        m_container.UpdateCanFocus();                                         \
    }                                                                         \
                                                                              \
    void classname::SetFocusIgnoringChildren()                                \
    {                                                                         \
        SetFocus();                                                           \
    }

#else // !wxHAS_NATIVE_TAB_TRAVERSAL

class WXDLLEXPORT wxFocusEvent;
class WXDLLEXPORT wxNavigationKeyEvent;
class WXDLLEXPORT wxWindow;
class WXDLLEXPORT wxWindowBase;

// ----------------------------------------------------------------------------
// wxControlContainer for TAB navigation implemented in wx itself
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlContainer : public wxControlContainerBase
{
public:
    // default ctor, SetContainerWindow() must be called later
    wxControlContainer();

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
    void SetFocusIgnoringChildren(); \
    virtual void OnChildFocus(wxChildFocusEvent& event); \
    virtual void SetFocus(); \
    virtual void RemoveChild(wxWindowBase *child); \
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
#define WX_DELEGATE_TO_CONTROL_CONTAINER(classname, basename)                 \
    WX_DELEGATE_TO_CONTROL_CONTAINER_BASE(classname, basename)                \
                                                                              \
    void classname::RemoveChild(wxWindowBase *child)                          \
    {                                                                         \
        m_container.HandleOnWindowDestroy(child);                             \
                                                                              \
        basename::RemoveChild(child);                                         \
                                                                              \
        m_container.UpdateCanFocus();                                         \
    }                                                                         \
                                                                              \
    void classname::OnNavigationKey( wxNavigationKeyEvent& event )            \
    {                                                                         \
        m_container.HandleOnNavigationKey(event);                             \
    }                                                                         \
                                                                              \
    void classname::SetFocus()                                                \
    {                                                                         \
        if ( !m_container.DoSetFocus() )                                      \
            basename::SetFocus();                                             \
    }                                                                         \
                                                                              \
    void classname::SetFocusIgnoringChildren()                                \
    {                                                                         \
        basename::SetFocus();                                                 \
    }                                                                         \
                                                                              \
    void classname::OnChildFocus(wxChildFocusEvent& event)                    \
    {                                                                         \
        m_container.SetLastFocus(event.GetWindow());                          \
    }                                                                         \
                                                                              \
    void classname::OnFocus(wxFocusEvent& event)                              \
    {                                                                         \
        m_container.HandleOnFocus(event);                                     \
    } 

#endif // wxHAS_NATIVE_TAB_TRAVERSAL/!wxHAS_NATIVE_TAB_TRAVERSAL

#endif // _WX_CONTAINR_H_
