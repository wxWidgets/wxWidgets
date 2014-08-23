/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/cursor.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) Sean D'Epagnier 2014
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/cursor.h"

#include <QApplication>
#include <QBitmap>

void wxSetCursor(const wxCursor& cursor)
{
    if (cursor.m_qtCursor.shape() == Qt::ArrowCursor)
        QApplication::restoreOverrideCursor();
    else
        QApplication::setOverrideCursor(cursor.m_qtCursor);
}

void wxBeginBusyCursor(const wxCursor *cursor)
{
    QApplication::setOverrideCursor(cursor->m_qtCursor);
}

bool wxIsBusy()
{
    return QApplication::overrideCursor() != 0;
}

void wxEndBusyCursor()
{
    QApplication::restoreOverrideCursor();
}


IMPLEMENT_DYNAMIC_CLASS( wxCursor, wxGDIObject )

wxCursor::wxCursor( const wxCursor &cursor )
{
    m_qtCursor = cursor.m_qtCursor;
}

wxCursor::wxCursor(const wxString& filename,
                   wxBitmapType kind,
                   int hotSpotX,
                   int hotSpotY)
{
    switch ( kind )
    {
        case wxBITMAP_TYPE_ICO:
            m_qtCursor = QCursor(
                            *wxBitmap(filename, wxBITMAP_TYPE_ICO).GetHandle(),
                            hotSpotX, hotSpotY );
            break;
        case wxBITMAP_TYPE_BMP:
            m_qtCursor = QCursor(
                            *wxBitmap(filename, wxBITMAP_TYPE_ICO).GetHandle(),
                            hotSpotX, hotSpotY );
            break;
        default:
            wxLogError( wxT("unknown cursor resource type '%d'"), kind );
    }
}

void wxCursor::InitFromStock( wxStockCursor cursorId )
{
    Qt::CursorShape qt_cur;
    switch (cursorId)
    {
    case wxCURSOR_BLANK:
    {
        m_qtCursor = QBitmap();
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
        // will use the standard one
        break;
    }

    m_qtCursor.setShape(qt_cur);
}

#if wxUSE_IMAGE

void wxCursor::InitFromImage( const wxImage & image )
{
    m_qtCursor = *wxBitmap(image).GetHandle();
}

#endif // wxUSE_IMAGE

wxGDIRefData *wxCursor::CreateGDIRefData() const
{
    //return new wxCursorRefData;
    return NULL;
}

wxGDIRefData *
wxCursor::CloneGDIRefData(const wxGDIRefData * data) const
{
//    return new wxCursorRefData(data->bitmap());
    return NULL;

}
