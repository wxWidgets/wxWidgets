///////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/uianimationstoryboard.mm
// Purpose:     Platform specific storyboard implementation
// Author:      Chiciu Bogdan Gabriel
// Created:     2011-07-06
// RCS-ID:      $Id$
// Copyright:   (c) 2011 wxWidgets Team
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

#include "wx/uianimationstoryboard.h"

wxUIAnimationStoryboardCocoa::wxUIAnimationStoryboardCocoa()
{
}

wxUIAnimationStoryboardCocoa::~wxUIAnimationStoryboardCocoa()
{
    m_animations.clear();
}

// Core Animations are started by adding them to the layer of our target.
void wxUIAnimationStoryboardCocoa::Start()
{
    wxVector<wxSharedPtr<CABasicAnimation>>::iterator animation_iter;
    for(animation_iter = m_animations.begin(); animation_iter != m_animations.end(); ++animation_iter)
	{
	    // TODO: we need to set a key for our animation so we can call removeAnimationForKey when we call ::Stop
		// TODO [check]: Is NSView a real NSView or a wrapper?
	    [[m_target->GetNSView() layer] addAnimation:(*iter) forKey:@""];
	}
}

void wxUIAnimationStoryboardCocoa::Stop()
{
    //TODO: use removeAnimationForKey once a key has been set
	wxFAIL_MSG("Method not implemented!");
	[[m_target->GetNSView() layer] removeAllAnimations];
}

static CAMediaTimingFunction wxUIAnimationStoryboardCocoa::GetCAMediaTimingFunctionFromAnimationCurve(wxAnimationCurve curve)
{
	switch(curve)
	{
	case wxANIMATION_CURVE_EASE_IN:
	    return [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];// TODO: check synatx
	case wxANIMATION_CURVE_EASE_OUT:
	    return [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseOut];// TODO: check synatx
	case wxANIMATION_CURVE_EASE_IN_OUT:
	    return [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut];// TODO: check synatx
	case wxANIMATION_CURVE_EASE_IN_OUT:
	    return [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionLinear];// TODO: check synatx
	}
}