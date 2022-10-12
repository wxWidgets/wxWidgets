/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/display.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/display.h"
#include "wx/private/display.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include "wx/qt/private/converter.h"

class wxDisplayImplQt : public wxDisplayImpl
{
public:
    wxDisplayImplQt( unsigned n );

    virtual wxRect GetGeometry() const override;
    virtual wxRect GetClientArea() const override;
    virtual int GetDepth() const override;

#if wxUSE_DISPLAY
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const override;
    virtual wxVideoMode GetCurrentMode() const override;
    virtual bool ChangeMode(const wxVideoMode& mode) override;
#endif // wxUSE_DISPLAY
};

wxDisplayImplQt::wxDisplayImplQt( unsigned n )
    : wxDisplayImpl( n )
{
}

wxRect wxDisplayImplQt::GetGeometry() const
{
    return wxQtConvertRect( QApplication::desktop()->screenGeometry( GetIndex() ));
}

wxRect wxDisplayImplQt::GetClientArea() const
{
    return wxQtConvertRect( QApplication::desktop()->availableGeometry( GetIndex() ));
}

int wxDisplayImplQt::GetDepth() const
{
    return IsPrimary() ? QApplication::desktop()->depth() : 0;
}

#if wxUSE_DISPLAY
wxArrayVideoModes wxDisplayImplQt::GetModes(const wxVideoMode& WXUNUSED(mode)) const
{
    return wxArrayVideoModes();
}

wxVideoMode wxDisplayImplQt::GetCurrentMode() const
{
    int width = QApplication::desktop()->width();
    int height = QApplication::desktop()->height();
    int depth = QApplication::desktop()->depth();

    return wxVideoMode( width, height, depth );
}

bool wxDisplayImplQt::ChangeMode(const wxVideoMode& WXUNUSED(mode))
{
    return false;
}
#endif // wxUSE_DISPLAY


//##############################################################################

#if wxUSE_DISPLAY

class wxDisplayFactoryQt : public wxDisplayFactory
{
public:
    virtual wxDisplayImpl *CreateDisplay(unsigned n) override;
    virtual unsigned GetCount() override;
    virtual int GetFromPoint(const wxPoint& pt) override;
};

wxDisplayImpl *wxDisplayFactoryQt::CreateDisplay(unsigned n)
{
    return new wxDisplayImplQt( n );
}

unsigned wxDisplayFactoryQt::GetCount()
{
    return QApplication::desktop()->screenCount();
}

int wxDisplayFactoryQt::GetFromPoint(const wxPoint& pt)
{
    return QApplication::desktop()->screenNumber( wxQtConvertPoint( pt ));
}

//##############################################################################

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryQt;
}

#else // wxUSE_DISPLAY

class wxDisplayFactorySingleQt : public wxDisplayFactorySingleQt
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay() override
    {
        return new wxDisplayImplQt(0);
    }
};

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactorySingleQt;
}

#endif // wxUSE_DISPLAY/!wxUSE_DISPLAY
