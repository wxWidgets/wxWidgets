/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.h
// Purpose:     wxGauge header, includes gauge class headers as appropriate
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GAUGE_H_
#define _WX_GAUGE_H_

#ifdef __GNUG__
#pragma interface "gauge.h"
#endif

#ifdef __WIN95__
#include "wx/msw/gauge95.h"
#define wxGauge wxGauge95
#define sm_classwxGauge sm_classwxGauge95
#else
#include "wx/msw/gaugemsw.h"
#define wxGauge wxGaugeMSW
#define sm_classwxGauge sm_classwxGaugeMSW
#endif

#endif
    // _WX_GAUGE_H_
