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
    #include "wx/dcmemory.h"
    #include "wx/region.h"
#endif // WX_PRECOMP

#include "wx/nonownedwnd.h"
#include "wx/graphics.h"
#include "wx/qt/private/converter.h"
#include "wx/qt/private/utils.h"

#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtWidgets/QWidget>

// ============================================================================
// wxNonOwnedWindow implementation
// ============================================================================

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
bool wxNonOwnedWindow::DoSetPathShape(const wxGraphicsPath& path)
{
    // Convert the path to wxRegion by rendering the path on a window-sized
    // bitmap, creating a mask from it and finally creating the region from
    // this mask.
    wxBitmap bmp(GetSize());

    {
        wxMemoryDC dc(bmp);
        dc.SetBackground(*wxBLACK_BRUSH);
        dc.Clear();

        std::unique_ptr<wxGraphicsContext> context(wxGraphicsContext::Create(dc));
        context->SetBrush(*wxWHITE_BRUSH);
        context->SetAntialiasMode(wxANTIALIAS_NONE);
        context->FillPath(path);
    }

    bmp.SetMask(new wxMask(bmp, *wxBLACK));

    return DoSetRegionShape(wxRegion(bmp));
}
#endif

