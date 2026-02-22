/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/dcprint.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/dcprint.h"
#include "wx/qt/dcprint.h"
#include "wx/qt/printdlg.h"
#include "wx/qt/private/converter.h"

#include <QtGui/QPainter>

#include <QPrinter>

wxIMPLEMENT_ABSTRACT_CLASS(wxPrinterDCImpl, wxQtDCImpl)

wxPrinterDCImpl::wxPrinterDCImpl(wxPrinterDC* owner, const wxPrintData& data)
    : wxQtDCImpl(owner)
    , m_printData(data)
{
    if ( m_printData.IsOk() )
    {
        auto* nativeData =
            static_cast<wxQtPrintNativeData*>(m_printData.GetNativeData());

        m_qtPrinter = nativeData->GetQtPrinter();
        m_qtPainter = new QPainter();

        m_ok = m_qtPainter->begin(m_qtPrinter);
    }
}

wxPrinterDCImpl::~wxPrinterDCImpl() = default;

wxRect wxPrinterDCImpl::GetPaperRect() const
{
    if ( !IsOk() )
    {
        return wxRect{};
    }

    return wxQtConvertRect(m_qtPrinter->paperRect(QPrinter::DevicePixel).toRect());
}

int wxPrinterDCImpl::GetResolution() const
{
    if ( IsOk() )
    {
        return m_qtPrinter->resolution();
    }

    return wxQtDCImpl::GetResolution();
}

wxSize wxPrinterDCImpl::GetPPI() const
{
    if ( IsOk() )
    {
        return wxSize(m_qtPrinter->logicalDpiX(),
                      m_qtPrinter->logicalDpiY());
    }

    return wxQtDCImpl::GetPPI();
}

void wxPrinterDCImpl::DoGetSize(int* width, int* height) const
{
    const int w = m_qtPrinter ? m_qtPrinter->width() : 0;
    const int h = m_qtPrinter ? m_qtPrinter->height() : 0;

    if ( width )
        *width = w;
    if ( height )
        *height = h;
}

void wxPrinterDCImpl::DoGetSizeMM(int* width, int* height) const
{
    const int w = m_qtPrinter ? m_qtPrinter->widthMM() : 0;
    const int h = m_qtPrinter ? m_qtPrinter->heightMM() : 0;

    if ( width )
        *width = w;
    if ( height )
        *height = h;
}

bool wxPrinterDCImpl::StartDoc(const wxString& message)
{
    if ( m_ok )
    {
        // Commented out because Qt doesn't allow us to change the document name
        // after calling m_qtPainter->begin(m_qtPrinter) and logs this message:
        // 'QPrinter::setDocName: Cannot be changed while printer is active'

        // m_qtPrinter->setDocName(wxQtConvertString(message));

        wxUnusedVar(message);
    }

    return m_ok;
}

void wxPrinterDCImpl::EndDoc()
{
    if ( m_ok )
    {
        m_qtPainter->end();
    }
}

void wxPrinterDCImpl::StartPage()
{
}

void wxPrinterDCImpl::EndPage()
{
}

#endif // wxUSE_PRINTING_ARCHITECTURE
