/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/display.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/display.h"
#include "wx/display_impl.h"

class wxDisplayImplQt : public wxDisplayImpl
{
public:
    wxDisplayImplQt( unsigned n );

    virtual wxRect GetGeometry() const;
    virtual wxString GetName() const;

    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const;
    virtual wxVideoMode GetCurrentMode() const;
    virtual bool ChangeMode(const wxVideoMode& mode);
};

wxDisplayImplQt::wxDisplayImplQt( unsigned n )
    : wxDisplayImpl( n )
{
}

wxRect wxDisplayImplQt::GetGeometry() const
{
    return wxRect();
}

wxString wxDisplayImplQt::GetName() const
{
    return wxString();
}

wxArrayVideoModes wxDisplayImplQt::GetModes(const wxVideoMode& mode) const
{
    return wxArrayVideoModes();
}

wxVideoMode wxDisplayImplQt::GetCurrentMode() const
{
    return wxVideoMode();
}

bool wxDisplayImplQt::ChangeMode(const wxVideoMode& mode)
{
    return false;
}


//##############################################################################

class wxDisplayFactoryQt : public wxDisplayFactory
{
public:
    wxDisplayFactoryQt();

    virtual wxDisplayImpl *CreateDisplay(unsigned n);
    virtual unsigned GetCount();
    virtual int GetFromPoint(const wxPoint& pt);
};

    
wxDisplayFactoryQt::wxDisplayFactoryQt()
{
}

wxDisplayImpl *wxDisplayFactoryQt::CreateDisplay(unsigned n)
{
    return new wxDisplayImplQt( n );
}

unsigned wxDisplayFactoryQt::GetCount()
{
    return 0;
}

int wxDisplayFactoryQt::GetFromPoint(const wxPoint& pt)
{
    return 0;
}

//##############################################################################

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryQt;
}
