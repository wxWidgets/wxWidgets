/////////////////////////////////////////////////////////////////////////////
// Name:        slider.h
// Purpose:     wxSlider header, includes slider class headers as appropriate
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _SLIDER_H_
#define _SLIDER_H_

#ifdef __GNUG__
#pragma interface "slider.h"
#endif

#ifdef __WIN95__
#include "wx/msw/slider95.h"
#define wxSlider wxSlider95
#define classwxSlider classwxSlider95
#else
#include "wx/msw/slidrmsw.h"
#define wxSlider wxSliderMSW
#define classwxSlider classwxSliderMSW
#endif

#endif
    // _SLIDER_H_
