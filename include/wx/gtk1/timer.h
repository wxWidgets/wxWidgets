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
public:
    wxTimer();
    ~wxTimer();

    int Interval() const { return m_time; }

    bool OneShot() const { return m_oneShot; }

    virtual bool Start( int millisecs = -1, bool oneShot = FALSE );
    virtual void Stop();

    virtual void Notify() = 0;  // override this

protected:
    int  m_tag;
    int  m_time;
    bool m_oneShot;

private:
    DECLARE_ABSTRACT_CLASS(wxTimer)
};

#endif // __GTKTIMERH__
