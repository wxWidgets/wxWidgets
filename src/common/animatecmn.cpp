/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/animatecmn.cpp
// Purpose:     wxAnimation and wxAnimationCtrl
// Author:      Francesco Montorsi
// Modified By:
// Created:     24/09/2006
// Id:          $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_ANIMATIONCTRL

#include "wx/animate.h"
#include "wx/bitmap.h"

const wxChar wxAnimationCtrlNameStr[] = wxT("animationctrl");

// global object
wxAnimation wxNullAnimation;

IMPLEMENT_ABSTRACT_CLASS(wxAnimationBase, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxAnimationCtrlBase, wxControl)


#endif      // wxUSE_ANIMATIONCTRL
