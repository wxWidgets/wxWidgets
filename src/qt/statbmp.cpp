/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/statbmp.cpp
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_STATBMP

#include "wx/statbmp.h"
#include "wx/qt/private/winevent.h"

#include <QtWidgets/QLabel>

class wxQtStaticBmp : public wxQtEventSignalHandler< QLabel, wxStaticBitmap >
{
public:
    wxQtStaticBmp( wxWindow *parent, wxStaticBitmap *handler ):
        wxQtEventSignalHandler< QLabel, wxStaticBitmap >( parent, handler )
    {
        // For compatibility with wxMSW and wxGTK3 ports.
        setAlignment( Qt::AlignCenter );
    }
};


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
    m_qtWindow = new wxQtStaticBmp( parent, this );

    SetBitmap( label );

    return wxStaticBitmapBase::Create( parent, id, pos, size, style, wxDefaultValidator, name );
}

QLabel* wxStaticBitmap::GetQLabel() const
{
    return static_cast<QLabel*>(m_qtWindow);
}

static void SetPixmap( QLabel *label, const QPixmap *pixMap )
{
    if ( pixMap != nullptr )
        label->setPixmap( *pixMap );
}

void wxStaticBitmap::SetBitmap(const wxBitmapBundle& bitmap)
{
    m_bitmapBundle = bitmap;

    SetPixmap( GetQLabel(), bitmap.GetBitmapFor(this).GetHandle() );

    InvalidateBestSize();
}

wxBitmap wxStaticBitmap::GetBitmap() const
{
    wxBitmap bitmap = m_bitmapBundle.GetBitmapFor(this);
    if ( !bitmap.IsOk() )
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
        QPixmap pix = GetQLabel()->pixmap(Qt::ReturnByValue);
        bitmap = wxBitmap( pix );
#else
        const QPixmap* pix = GetQLabel()->pixmap();
        if ( pix != nullptr )
            bitmap = wxBitmap( *pix );
#endif
    }

    return bitmap;
}

#endif // wxUSE_STATBMP
