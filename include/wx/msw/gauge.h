/////////////////////////////////////////////////////////////////////////////
// Name:        gauge.h
// Purpose:     wxGauge header, includes gauge class headers as appropriate
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __GAUGEH__
#define __GAUGEH__

#ifdef __GNUG__
#pragma interface "gauge.h"
#endif

#ifdef __WIN95__
#include "wx/msw/gauge95.h"
#define wxGauge wxGauge95
#define classwxGauge classwxGauge95
#else
#include "wx/msw/gaugemsw.h
#define wxGauge wxGaugeMSW
#define classwxGauge classwxGaugeMSW
#endif

#endif
    // __GAUGEH__
