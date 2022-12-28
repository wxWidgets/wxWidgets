/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/cursor.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include <QtWidgets/QApplication>
#include <QtGui/QBitmap>

#ifndef WX_PRECOMP
    #include "wx/bitmap.h"
#endif // WX_PRECOMP

#include "wx/cursor.h"
#include "wx/qt/private/converter.h"

void wxSetCursor(const wxCursor& cursor)
{
    if (cursor.GetHandle().shape() == Qt::ArrowCursor)
        QApplication::restoreOverrideCursor();
    else
        QApplication::setOverrideCursor(cursor.GetHandle());
}

void wxBeginBusyCursor(const wxCursor *cursor)
{
    QApplication::setOverrideCursor(cursor->GetHandle());
}

bool wxIsBusy()
{
    return QApplication::overrideCursor() != nullptr;
}

void wxEndBusyCursor()
{
    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// wxCursorRefData
//-----------------------------------------------------------------------------

class wxCursorRefData: public wxGDIRefData
{
public:
    wxCursorRefData() {}
    wxCursorRefData( const wxCursorRefData& data ) : m_qtCursor(data.m_qtCursor) {}
    wxCursorRefData( QCursor &c ) : m_qtCursor(c) {}

    QCursor m_qtCursor;
};

wxIMPLEMENT_DYNAMIC_CLASS(wxCursor, wxGDIObject);


wxCursor::wxCursor(const wxString& cursor_file,
                   wxBitmapType type,
                   int hotSpotX, int hotSpotY)
{
#if wxUSE_IMAGE
    wxImage img;
    if (!img.LoadFile(cursor_file, type))
        return;

    // eventually set the hotspot:
    if (!img.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X))
        img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, hotSpotX);
    if (!img.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y))
        img.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, hotSpotY);

    InitFromImage(img);
#endif // wxUSE_IMAGE
}

#if wxUSE_IMAGE
wxCursor::wxCursor(const wxImage& img)
{
    InitFromImage(img);
}

wxCursor::wxCursor(const char* const* xpmData)
{
    InitFromImage(wxImage(xpmData));
}
#endif // wxUSE_IMAGE

wxPoint wxCursor::GetHotSpot() const
{
    return wxQtConvertPoint(GetHandle().hotSpot());
}

QCursor &wxCursor::GetHandle() const
{
    return static_cast<wxCursorRefData*>(m_refData)->m_qtCursor;
}

void wxCursor::InitFromStock( wxStockCursor cursorId )
{
    AllocExclusive();

    Qt::CursorShape qt_cur;
    switch (cursorId)
    {
    case wxCURSOR_BLANK:
    {
        GetHandle() = QBitmap();
        return;
    }
//    case wxCURSOR_ARROW:
    case wxCURSOR_DEFAULT:     qt_cur = Qt::ArrowCursor;    break;
//    case wxCURSOR_RIGHT_ARROW:
    case wxCURSOR_HAND:        qt_cur = Qt::OpenHandCursor; break;
    case wxCURSOR_CROSS:       qt_cur = Qt::CrossCursor;    break;
    case wxCURSOR_SIZEWE:      qt_cur = Qt::SizeHorCursor;  break;
    case wxCURSOR_SIZENS:      qt_cur = Qt::SizeVerCursor;  break;
    case wxCURSOR_ARROWWAIT:   qt_cur = Qt::BusyCursor;     break;
    case wxCURSOR_WATCH:
    case wxCURSOR_WAIT:        qt_cur = Qt::WaitCursor;     break;
    case wxCURSOR_SIZING:      qt_cur = Qt::SizeAllCursor;  break;
//    case wxCURSOR_SPRAYCAN:
    case wxCURSOR_IBEAM:       qt_cur = Qt::IBeamCursor;    break;
//    case wxCURSOR_PENCIL:
    case wxCURSOR_NO_ENTRY:    qt_cur = Qt::ForbiddenCursor; break;
    case wxCURSOR_SIZENWSE:    qt_cur = Qt::SizeFDiagCursor; break;
    case wxCURSOR_SIZENESW:    qt_cur = Qt::SizeBDiagCursor; break;
    case wxCURSOR_QUESTION_ARROW: qt_cur = Qt::WhatsThisCursor; break;
/*  case wxCURSOR_PAINT_BRUSH:
    case wxCURSOR_MAGNIFIER:
    case wxCURSOR_CHAR:
    case wxCURSOR_LEFT_BUTTON:
    case wxCURSOR_MIDDLE_BUTTON:
    case wxCURSOR_RIGHT_BUTTON:
    case wxCURSOR_BULLSEYE:
    case wxCURSOR_POINT_LEFT:
    case wxCURSOR_POINT_RIGHT:
    case wxCURSOR_DOUBLE_ARROW:
    case wxCURSOR_CROSS_REVERSE:
    case wxCURSOR_BASED_ARROW_DOWN:
    case wxCURSOR_BASED_ARROW_UP:   qt_cur = Qt::UpArrowCursor; break;
*/
    default:
        wxFAIL_MSG(wxT("unsupported cursor type"));
        qt_cur = Qt::ArrowCursor;
        break;
    }

    GetHandle().setShape(qt_cur);
}

#if wxUSE_IMAGE

void wxCursor::InitFromImage( const wxImage & image )
{
    AllocExclusive();
    GetHandle() = QCursor(*wxBitmap(image).GetHandle(),
                           image.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_X) ?
                           image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_X) : -1,
                           image.HasOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y) ?
                           image.GetOptionInt(wxIMAGE_OPTION_CUR_HOTSPOT_Y) : -1);
}

#endif // wxUSE_IMAGE

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    return new wxCursorRefData;
}

wxGDIRefData *wxCursor::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxCursorRefData(*(wxCursorRefData *)data);
}
