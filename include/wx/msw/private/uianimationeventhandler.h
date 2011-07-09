///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/uianimationeventhander.h
// Purpose:     Declaration/implementation of the internal MSW animation event handlers
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-06-16
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UI_ANIMATION_EVENT_HANDLER_H_
#define _WX_UI_ANIMATION_EVENT_HANDLER_H_

#include <UIAnimation.h>

#include "wx/msw/ole/oleutils.h"

class wxUIAnimationStoryboardBase;

// Re-implementaion of IMPLEMENT/DECLARE_IUNKNOWN_METHODS. Reasons are explained bellow.
template <class T> class wxMSWAnimationEventHandlerBase : public T
{
public:
    wxMSWAnimationEventHandlerBase()
    {
        m_referenceCount = 1;
    }

    HRESULT QueryInterfaceCallback(REFIID requestedInterfaceId,
        REFIID requestedInterfaceIdCallback,
        void** requestedInterfacePointer)
    {
        if (requestedInterfacePointer == NULL)
        {
            return E_POINTER;
        }
        if ((requestedInterfaceId == IID_IUnknown) || (requestedInterfaceId == requestedInterfaceIdCallback))
        {
            *requestedInterfacePointer = static_cast<T*>(this);
            AddRef();
            return S_OK;
        }
        *requestedInterfacePointer = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP QueryInterface(REFIID requestedInterfaceId,
        void** requestedInterfacePointer)
    {
        T** animationInterface = reinterpret_cast<T**>(requestedInterfacePointer);   
        return QueryInterfaceCallback(requestedInterfaceId, __uuidof(**animationInterface), reinterpret_cast<void**>(animationInterface));
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        ++m_referenceCount;
        return m_referenceCount;
    }

    STDMETHODIMP_(ULONG) Release()
    {
        m_referenceCount -= 1;
        if (m_referenceCount == 0)
        {
            delete this;
            return 0;
        }
        return m_referenceCount;
    }

protected:
    DWORD m_referenceCount;
};

// MSW event handler class. Events fire when the IUIAnimationManager status changes.
// It implements auto-repeat and event handling for storyboards.
class UIAnimationManagerEventHandlerBase : public wxMSWAnimationEventHandlerBase<IUIAnimationManagerEventHandler>
{
public:
    UIAnimationManagerEventHandlerBase(wxUIAnimationStoryboardMSW* storyboard);

    // IUIAnimationManagerEventHandler
    STDMETHODIMP OnManagerStatusChanged(UI_ANIMATION_MANAGER_STATUS newStatus, 
        UI_ANIMATION_MANAGER_STATUS previousStatus);
private:
    wxUIAnimationStoryboardMSW* m_storyboard;
};

// MSW event handler class. Events fire when the values of the animation variables update.
class UIAnimationTimerEventHandlerBase : public wxMSWAnimationEventHandlerBase<IUIAnimationTimerEventHandler>
{
public:
    UIAnimationTimerEventHandlerBase(wxUIAnimationStoryboardMSW* storyboard);

    // IUIAnimationTimerEventHandler
    STDMETHODIMP OnPreUpdate();

    STDMETHODIMP OnPostUpdate();

    STDMETHODIMP OnRenderingTooSlow(UINT32 framesPerSecond);

private:
    wxUIAnimationStoryboardMSW* m_storyboard;
};

#if 0 // This will cause an access violation when the storyboard is scheduled to play.
class UIAnimationManagerEventHandlerBase : public IUIAnimationManagerEventHandler
{
public:
    UIAnimationManagerEventHandlerBase()
    {
    }

    DECLARE_IUNKNOWN_METHODS;
public:
    // IUIAnimationManagerEventHandler
    STDMETHODIMP OnManagerStatusChanged(UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus);

};

class UIAnimationTimerEventHandlerBase : public IUIAnimationTimerEventHandler
{
public:
    UIAnimationTimerEventHandlerBase(wxUIAnimationStoryboardBase* storyboard);

    DECLARE_IUNKNOWN_METHODS;
public:
    // IUIAnimationTimerEventHandler
    STDMETHODIMP OnPreUpdate();
    STDMETHODIMP OnPostUpdate();
    STDMETHODIMP OnRenderingTooSlow(UINT32 framesPerSecond);


private :
    wxUIAnimationStoryboardBase* m_storyboard;
};
#endif
#endif