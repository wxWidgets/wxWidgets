/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statbmp.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/statbmp.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QLabel>

class wxQtStaticBmp : public wxQtEventSignalHandler< QLabel, wxStaticBitmap >
{
public:
    wxQtStaticBmp( wxWindow *parent, wxStaticBitmap *handler ):
        wxQtEventSignalHandler< QLabel, wxStaticBitmap >( parent, handler ){}
};


wxStaticBitmap::wxStaticBitmap() :
    m_qtLabel(nullptr)
{
}

wxStaticBitmap::wxStaticBitmap( wxWindow *parent,
                wxWindowID id,
                const wxBitmapBundle& label,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
    Create( parent, id, label, pos, size, style, name );
}

bool wxStaticBitmap::Create( wxWindow *parent,
             wxWindowID id,
             const wxBitmapBundle& label,
             const wxPoint& pos,
             const wxSize& size,
             long style,
             const wxString& name)
{
    m_qtLabel = new wxQtStaticBmp( parent, this );
    SetBitmap( label );

    return QtCreateControl( parent, id, pos, size, style, wxDefaultValidator, name );
}

static void SetPixmap( QLabel *label, const QPixmap *pixMap )
{
    if ( pixMap != nullptr )
        label->setPixmap( *pixMap );
}

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
    SetPixmap( m_qtLabel, bitmap.GetBitmapFor(this).GetHandle() );
}

wxBitmap wxStaticBitmap::GetBitmap() const
{
    const QPixmap* pix = m_qtLabel->pixmap();
    if ( pix != nullptr )
        return wxBitmap( *pix );
    else
        return wxBitmap();
}

QWidget *wxStaticBitmap::GetHandle() const
{
    return m_qtLabel;
}
