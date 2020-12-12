/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/nonownedwnd.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) 2016 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/region.h"
    #include "wx/region.h"
#endif // WX_PRECOMP

#include "wx/nonownedwnd.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/utils.h"

#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtWidgets/QWidget>

// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

wxNonOwnedWindow::wxNonOwnedWindow()
{
}

bool wxNonOwnedWindow::DoClearShape()
{
    GetHandle()->setMask(QBitmap());
    return true;
}

bool wxNonOwnedWindow::DoSetRegionShape(const wxRegion& region)
{
    QPixmap pixmap(GetHandle()->size());
    QPainter painter(&pixmap);
    painter.fillRect(pixmap.rect(), Qt::white);
    painter.setClipRegion(region.GetHandle());
    painter.fillRect(pixmap.rect(), Qt::black);
    GetHandle()->setMask(pixmap.createMaskFromColor(Qt::white));
    return true;
}

#if wxUSE_GRAPHICS_CONTEXT
bool wxNonOwnedWindow::DoSetPathShape(const wxGraphicsPath& WXUNUSED(path))
{
    wxMISSING_IMPLEMENTATION( __FUNCTION__ );
    return true;
}
#endif

