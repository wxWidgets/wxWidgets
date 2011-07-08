///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uianimationstoryboard.h
// Purpose:     Declaration of the storyboard base classe
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-05-11
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UI_ANIMATION_STORYBOARD_H_BASE_
#define _WX_UI_ANIMATION_STORYBOARD_H_BASE_

class WXDLLIMPEXP_ANIMATION wxUIAnimationStoryboardBase
{
public:
    wxUIAnimationStoryboardBase()
    {
    }

    virtual ~wxUIAnimationStoryboardBase()
    {
    }

    void SetAnimationTarget(wxControl* targetControl)
    {
        m_targetControl = targetControl;
    }

    virtual void Start() = 0;

    virtual void Stop() = 0;
protected:
    wxControl* m_targetControl;
};

// ----------------------------------------------------------------------------
// Platform-specific storyboard classes.
// ----------------------------------------------------------------------------
#if defined(__WXMSW__)
    #define wxUIAnimationStoryboardMSW wxUIAnimationStoryboard
    #include "wx/msw/uianimationstoryboard.h"
#elif defined(__WXCOCOA__)
    #define wxUIAnimationStoryboardCocoa wxUIAnimationStoryboard
    #include "wx/cocoa/uianimationstoryboard.h"
#endif

#endif//_WX_UI_ANIMATION_STORYBOARD_H_BASE_
