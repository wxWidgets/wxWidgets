///////////////////////////////////////////////////////////////////////////////
// Name:        wx/containr.h
// Purpose:     wxControlContainer and wxNavigationEnabled declarations
// Author:      Vadim Zeitlin
// Created:     06.08.01
// Copyright:   (c) 2001, 2011 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTAINR_H_
#define _WX_CONTAINR_H_

#include "wx/defs.h"

#ifndef wxHAS_NATIVE_TAB_TRAVERSAL
    // We need wxEVT_XXX declarations in this case.
    #include "wx/event.h"
#endif

class WXDLLIMPEXP_FWD_CORE wxWindow;
class WXDLLIMPEXP_FWD_CORE wxWindowBase;

/*
    This header declares wxControlContainer class however it's not a real
    container of controls but rather just a helper used to implement TAB
    navigation among the window children. You should rarely need to use it
    directly, derive from the documented public wxNavigationEnabled<> class to
    implement TAB navigation in a custom composite window.
 */

// ----------------------------------------------------------------------------
// wxControlContainerBase: common part used in both native and generic cases
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxControlContainerBase
{
public:
    // default ctor, SetContainerWindow() must be called later
    wxControlContainerBase()
    {
        m_winParent = nullptr;

        // By default, we accept focus ourselves.
        m_acceptsFocusSelf = true;

        m_inSetFocus = false;
        m_winLastFocused = nullptr;
    }
    virtual ~wxControlContainerBase() {}

    void SetContainerWindow(wxWindow *winParent)
    {
        wxASSERT_MSG( !m_winParent, wxT("shouldn't be called twice") );

        m_winParent = winParent;
    }

    // This can be called by the window to indicate that it never wants to have
    // the focus for itself.
    void DisableSelfFocus()
        { m_acceptsFocusSelf = false; UpdateParentCanFocus(); }

    // This can be called to undo the effect of a previous DisableSelfFocus()
    // (otherwise calling it is not necessary as the window does accept focus
    // by default).
    void EnableSelfFocus()
        { m_acceptsFocusSelf = true; UpdateParentCanFocus(); }

    // should be called from SetFocus(), returns false if we did nothing with
    // the focus and the default processing should take place
    bool DoSetFocus();

    // returns whether we should accept focus ourselves or not
    bool AcceptsFocus() const;

    // Returns whether we or one of our children accepts focus.
    bool AcceptsFocusRecursively() const
        { return AcceptsFocus() || HasAnyChildrenAcceptingFocus(); }

    // We accept focus from keyboard if we accept it at all.
    bool AcceptsFocusFromKeyboard() const { return AcceptsFocusRecursively(); }

    // Call this when the number of children of the window changes.
    //
    // Returns true if we have any focusable children, false otherwise.
    bool UpdateCanFocusChildren();

#ifdef __WXMSW__
    // This is not strictly related to navigation, but all windows containing
    // more than one children controls need to return from this method if any
    // of their parents has an inheritable background, so do this automatically
    // for all of them (another alternative could be to do it in wxWindow
    // itself but this would be potentially more backwards incompatible and
    // could conceivably break some custom windows).
    bool HasTransparentBackground() const;
#endif // __WXMSW__

protected:
    // set the focus to the child which had it the last time
    virtual bool SetFocusToChild();

    // return true if we have any children accepting focus
    bool HasAnyFocusableChildren() const;

    // return true if we have any children that do accept focus right now
    bool HasAnyChildrenAcceptingFocus() const;


    // the parent window we manage the children for
    wxWindow *m_winParent;

    // the child which had the focus last time this panel was activated
    wxWindow *m_winLastFocused;

private:
    // Update the window status to reflect whether it is getting focus or not.
    void UpdateParentCanFocus(bool acceptsFocusChildren);
    void UpdateParentCanFocus()
    {
        UpdateParentCanFocus(HasAnyFocusableChildren());
    }

    // Indicates whether the associated window can ever have focus itself.
    //
    // Usually this is the case, e.g. a wxPanel can be used either as a
    // container for its children or just as a normal window which can be
    // focused. But sometimes, e.g. for wxStaticBox, we can never have focus
    // ourselves and can only get it if we have any focusable children.
    bool m_acceptsFocusSelf;

    // a guard against infinite recursion
    bool m_inSetFocus;
};

#ifdef wxHAS_NATIVE_TAB_TRAVERSAL

// ----------------------------------------------------------------------------
// wxControlContainer for native TAB navigation
// ----------------------------------------------------------------------------

// this must be a real class as we forward-declare it elsewhere
class WXDLLIMPEXP_CORE wxControlContainer : public wxControlContainerBase
{
protected:
    // set the focus to the child which had it the last time
    virtual bool SetFocusToChild() override;
};

#else // !wxHAS_NATIVE_TAB_TRAVERSAL

// ----------------------------------------------------------------------------
// wxControlContainer for TAB navigation implemented in wx itself
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxControlContainer : public wxControlContainerBase
{
public:
    // default ctor, SetContainerWindow() must be called later
    wxControlContainer();

    // the methods to be called from the window event handlers
    void HandleOnNavigationKey(wxNavigationKeyEvent& event);
    void HandleOnFocus(wxFocusEvent& event);
    void HandleOnWindowDestroy(wxWindowBase *child);

    // called from OnChildFocus() handler, i.e. when one of our (grand)
    // children gets the focus
    void SetLastFocus(wxWindow *win);

protected:

    wxDECLARE_NO_COPY_CLASS(wxControlContainer);
};

#endif // wxHAS_NATIVE_TAB_TRAVERSAL/!wxHAS_NATIVE_TAB_TRAVERSAL

// this function is for wxWidgets internal use only
extern WXDLLIMPEXP_CORE bool wxSetFocusToChild(wxWindow *win, wxWindow **child);

// ----------------------------------------------------------------------------
// wxNavigationEnabled: Derive from this class to support keyboard navigation
// among window children in a wxWindow-derived class. The details of this class
// don't matter, you just need to derive from it to make navigation work.
// ----------------------------------------------------------------------------

// The template parameter W must be a wxWindow-derived class.
template <class W>
class wxNavigationEnabled : public W
{
public:
    typedef W BaseWindowClass;

    wxNavigationEnabled()
    {
        m_container.SetContainerWindow(this);

#ifndef wxHAS_NATIVE_TAB_TRAVERSAL
        BaseWindowClass::Bind(wxEVT_NAVIGATION_KEY,
                              &wxNavigationEnabled::OnNavigationKey, this);

        BaseWindowClass::Bind(wxEVT_SET_FOCUS,
                              &wxNavigationEnabled::OnFocus, this);
        BaseWindowClass::Bind(wxEVT_CHILD_FOCUS,
                              &wxNavigationEnabled::OnChildFocus, this);
#endif // !wxHAS_NATIVE_TAB_TRAVERSAL
    }

    WXDLLIMPEXP_INLINE_CORE virtual bool AcceptsFocus() const override
    {
        return m_container.AcceptsFocus();
    }

    WXDLLIMPEXP_INLINE_CORE virtual bool AcceptsFocusRecursively() const override
    {
        return m_container.AcceptsFocusRecursively();
    }

    WXDLLIMPEXP_INLINE_CORE virtual bool AcceptsFocusFromKeyboard() const override
    {
        return m_container.AcceptsFocusFromKeyboard();
    }

    WXDLLIMPEXP_INLINE_CORE virtual void AddChild(wxWindowBase *child) override
    {
        BaseWindowClass::AddChild(child);

        if ( m_container.UpdateCanFocusChildren() )
        {
            // Under MSW we must have wxTAB_TRAVERSAL style for TAB navigation
            // to work.
            if ( !BaseWindowClass::HasFlag(wxTAB_TRAVERSAL) )
                BaseWindowClass::ToggleWindowStyle(wxTAB_TRAVERSAL);
        }
    }

    WXDLLIMPEXP_INLINE_CORE virtual void RemoveChild(wxWindowBase *child) override
    {
#ifndef wxHAS_NATIVE_TAB_TRAVERSAL
        m_container.HandleOnWindowDestroy(child);
#endif // !wxHAS_NATIVE_TAB_TRAVERSAL

        BaseWindowClass::RemoveChild(child);

        // We could reset wxTAB_TRAVERSAL here but it doesn't seem to do any
        // harm to keep it.
        m_container.UpdateCanFocusChildren();
    }

    WXDLLIMPEXP_INLINE_CORE virtual void SetFocus() override
    {
        if ( !m_container.DoSetFocus() )
            BaseWindowClass::SetFocus();
    }

    void SetFocusIgnoringChildren()
    {
        BaseWindowClass::SetFocus();
    }

#ifdef __WXMSW__
    WXDLLIMPEXP_INLINE_CORE virtual bool HasTransparentBackground() override
    {
        return m_container.HasTransparentBackground();
    }

    WXDLLIMPEXP_INLINE_CORE
    virtual void WXSetPendingFocus(wxWindow* win) override
    {
        return m_container.SetLastFocus(win);
    }
#endif // __WXMSW__

protected:
#ifndef wxHAS_NATIVE_TAB_TRAVERSAL
    void OnNavigationKey(wxNavigationKeyEvent& event)
    {
        m_container.HandleOnNavigationKey(event);
    }

    void OnFocus(wxFocusEvent& event)
    {
        m_container.HandleOnFocus(event);
    }

    void OnChildFocus(wxChildFocusEvent& event)
    {
        m_container.SetLastFocus(event.GetWindow());
        event.Skip();
    }
#endif // !wxHAS_NATIVE_TAB_TRAVERSAL

    wxControlContainer m_container;


    wxDECLARE_NO_COPY_TEMPLATE_CLASS(wxNavigationEnabled, W);
};

#endif // _WX_CONTAINR_H_
