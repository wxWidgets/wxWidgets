/////////////////////////////////////////////////////////////////////////////
// Name:        timer.cpp
// Purpose:     wxTimer implementation
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "timer.h"
#endif

#include "wx/timer.h"

#if !USE_SHARED_LIBRARY
IMPLEMENT_ABSTRACT_CLASS(wxTimer, wxObject)
#endif

wxTimer::wxTimer()
{
    m_milli = 0 ;
    m_id = 0;
    m_oneShot = FALSE;
}

wxTimer::~wxTimer()
{
    Stop();
}

bool wxTimer::Start(int milliseconds,bool mode)
{
    m_oneShot = mode ;
    if (milliseconds <= 0)
        return FALSE;

    m_milli = milliseconds;

    // TODO: set the timer going.
    return FALSE;
}

void wxTimer::Stop()
{
    m_id = 0 ;
    m_milli = 0 ;
}


