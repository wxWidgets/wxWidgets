/////////////////////////////////////////////////////////////////////////////
// Name:        timer.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKTIMERH__
#define __GTKTIMERH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"

//-----------------------------------------------------------------------------
// derived classes
//-----------------------------------------------------------------------------

class wxTimer;

//-----------------------------------------------------------------------------
// wxTimer
//-----------------------------------------------------------------------------

class wxTimer: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxTimer)

  public:

    wxTimer(void);
    ~wxTimer(void);
    int Interval(void);
    bool OneShot(void);
    virtual void Notify(void);
    void Start( int millisecs = -1, bool oneShot = FALSE );
    void Stop(void);
    
  private:
  
    int  m_time;
    bool m_oneShot;
};

#endif // __GTKTIMERH__
