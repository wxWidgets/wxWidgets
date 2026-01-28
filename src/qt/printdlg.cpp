/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/printdlg.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/display.h"
#include "wx/modalhook.h"
#include "wx/paper.h"
#include "wx/qt/dcprint.h"
#include "wx/qt/printdlg.h"
#include "wx/qt/private/converter.h"

#include <QPrinter>
#include <QPrinterInfo>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPageSize>


namespace // anonymous
{
QPrinter::DuplexMode ConvertDuplexMode(wxDuplexMode wxmode)
{
    switch (wxmode)
    {
    case wxDUPLEX_SIMPLEX:      return QPrinter::DuplexNone;
    case wxDUPLEX_HORIZONTAL:   return QPrinter::DuplexLongSide;
    case wxDUPLEX_VERTICAL:     return QPrinter::DuplexShortSide;
    default:
        wxFAIL_MSG("Unknown wxDuplexMode wxmode");
    }

    return QPrinter::DuplexNone;
}

wxDuplexMode ConvertDuplexMode(QPrinter::DuplexMode qtmode)
{
    switch (qtmode)
    {
    case QPrinter::DuplexNone:      return wxDUPLEX_SIMPLEX;
    case QPrinter::DuplexLongSide:  return wxDUPLEX_HORIZONTAL;
    case QPrinter::DuplexShortSide: return wxDUPLEX_VERTICAL;
    default:
        wxFAIL_MSG("Unknown QPrinter::DuplexMode qtmode");
    }

    return wxDUPLEX_SIMPLEX;
}

QPrinter::PaperSource ConvertBin(wxPrintBin wxsource)
{
    switch (wxsource)
    {
    case wxPRINTBIN_ONLYONE:        return QPrinter::OnlyOne;
    case wxPRINTBIN_LOWER:          return QPrinter::Lower;
    case wxPRINTBIN_MIDDLE:         return QPrinter::Middle;
    case wxPRINTBIN_MANUAL:         return QPrinter::Manual;
    case wxPRINTBIN_ENVELOPE:       return QPrinter::Envelope;
    case wxPRINTBIN_ENVMANUAL:      return QPrinter::EnvelopeManual;
    case wxPRINTBIN_AUTO:           return QPrinter::Auto;
    case wxPRINTBIN_TRACTOR:        return QPrinter::Tractor;
    case wxPRINTBIN_SMALLFMT:       return QPrinter::SmallFormat;
    case wxPRINTBIN_LARGEFMT:       return QPrinter::LargeFormat;
    case wxPRINTBIN_LARGECAPACITY:  return QPrinter::LargeCapacity;
    case wxPRINTBIN_CASSETTE:       return QPrinter::Cassette;
    case wxPRINTBIN_FORMSOURCE:     return QPrinter::FormSource;
    case wxPRINTBIN_DEFAULT:        wxFALLTHROUGH;
    case wxPRINTBIN_USER:           return QPrinter::CustomSource;
    default:
        wxFAIL_MSG("Unknown wxPrintBin wxsource");
    }

    return QPrinter::CustomSource;
}

wxPrintBin ConvertBin(QPrinter::PaperSource qtsource)
{
    switch (qtsource)
    {
    case QPrinter::OnlyOne:        return wxPRINTBIN_ONLYONE;
    case QPrinter::Lower:          return wxPRINTBIN_LOWER;
    case QPrinter::Middle:         return wxPRINTBIN_MIDDLE;
    case QPrinter::Manual:         return wxPRINTBIN_MANUAL;
    case QPrinter::Envelope:       return wxPRINTBIN_ENVELOPE;
    case QPrinter::EnvelopeManual: return wxPRINTBIN_ENVMANUAL;
    case QPrinter::Auto:           return wxPRINTBIN_AUTO;
    case QPrinter::Tractor:        return wxPRINTBIN_TRACTOR;
    case QPrinter::SmallFormat:    return wxPRINTBIN_SMALLFMT;
    case QPrinter::LargeFormat:    return wxPRINTBIN_LARGEFMT;
    case QPrinter::LargeCapacity:  return wxPRINTBIN_LARGECAPACITY;
    case QPrinter::Cassette:       return wxPRINTBIN_CASSETTE;
    case QPrinter::FormSource:     return wxPRINTBIN_FORMSOURCE;
    case QPrinter::CustomSource:   return wxPRINTBIN_USER;
    default:
        wxFAIL_MSG("Unknown QPrinter::PaperSource qtsource");
    }

    return wxPRINTBIN_DEFAULT;
}

void ConvertPageSize(const wxPrintData& data, QPrinter* qtPrinter)
{
    QPageSize qtPageSize;

    if ( data.GetPaperId() != wxPAPER_NONE )
    {
        const wxSize size = wxThePrintPaperDatabase->GetSize(data.GetPaperId()) / 10;

        const auto pageSizeId = QPageSize::id(wxQtConvertSize(size), QPageSize::Millimeter);

        qtPageSize = QPageSize(pageSizeId);
    }
    else // custom paper size
    {
        const wxSize size = data.GetPaperSize() / 10;

        if ( size.GetWidth() > 0 && size.GetHeight() > 0 )
        {
            qtPageSize = QPageSize(wxQtConvertSize(size), QPageSize::Millimeter);
        }
    }

    if ( !qtPageSize.isValid() )
    {
        QPrinterInfo printerInfo(*qtPrinter);

        qtPageSize = printerInfo.defaultPageSize();
    }

    qtPrinter->setPageSize(qtPageSize);
}

void ConvertPageSize(const QPrinter* qtPrinter, wxPrintData& data)
{
    const auto qtPageSize = qtPrinter->pageLayout().pageSize();
    const auto qtSize = qtPageSize.size(QPageSize::Millimeter).toSize() * 10;
    const wxPaperSize id = wxThePrintPaperDatabase->GetSize(wxQtConvertSize(qtSize));

    data.SetPaperId(id);
    data.SetPaperSize(wxQtConvertSize(qtSize));
}
} // anonymous namespace

wxIMPLEMENT_CLASS(wxQtPrintNativeData, wxPrintNativeDataBase);

wxQtPrintNativeData::wxQtPrintNativeData() = default;
wxQtPrintNativeData::~wxQtPrintNativeData() = default;

bool wxQtPrintNativeData::TransferTo(wxPrintData& data)
{
    if ( !m_qtPrinter )
    {
        m_qtPrinter.reset(new QPrinter());
    }

    data.SetPrinterName(wxQtConvertString(m_qtPrinter->printerName()));
    data.SetFilename(wxQtConvertString(m_qtPrinter->outputFileName()));
    data.SetColour(m_qtPrinter->colorMode() == QPrinter::Color);
    data.SetDuplex(ConvertDuplexMode(m_qtPrinter->duplex()));
    data.SetNoCopies(m_qtPrinter->copyCount());
    data.SetCollate(m_qtPrinter->collateCopies());
    data.SetBin(ConvertBin(m_qtPrinter->paperSource()));

    const wxPrintOrientation orientation =
        m_qtPrinter->pageLayout().orientation() == QPageLayout::Portrait
                                                 ? wxPORTRAIT : wxLANDSCAPE;
    data.SetOrientation(orientation);

    ConvertPageSize(GetQtPrinter(), data);

    return true;
}

bool wxQtPrintNativeData::TransferFrom(const wxPrintData& data)
{
    const QPrinter::PrinterMode mode = data.GetQuality() == wxPRINT_QUALITY_HIGH
                                     ? QPrinter::HighResolution
                                     : QPrinter::ScreenResolution;

    m_qtPrinter.reset(new QPrinter(mode));

    m_qtPrinter->setPrinterName(wxQtConvertString(data.GetPrinterName()));
    m_qtPrinter->setOutputFileName(wxQtConvertString(data.GetFilename()));
    m_qtPrinter->setColorMode(data.GetColour() ? QPrinter::Color : QPrinter::GrayScale);
    m_qtPrinter->setDuplex(ConvertDuplexMode(data.GetDuplex()));
    m_qtPrinter->setCopyCount(data.GetNoCopies());
    m_qtPrinter->setCollateCopies(data.GetCollate());
    m_qtPrinter->setPaperSource(ConvertBin(data.GetBin()));
    m_qtPrinter->setResolution(wxDisplay::GetStdPPIValue());

    const QPageLayout::Orientation orientation =
        data.GetOrientation() == wxPORTRAIT
                               ? QPageLayout::Portrait
                               : QPageLayout::Landscape;

    m_qtPrinter->setPageOrientation(orientation);

    ConvertPageSize(data, GetQtPrinter());

    return true;
}

bool wxQtPrintNativeData::IsOk() const
{
    return m_qtPrinter && m_qtPrinter->isValid();
}

//##############################################################################

namespace // anonymous
{
// ConvertToNative/ConvertFromNative for wxQtPrintDialog.
void ConvertToNative(wxPrintDialogData& printData, QPrintDialog* printDialog = nullptr)
{
    if ( !printDialog )
    {
        printData.GetPrintData().ConvertToNative();
    }
    else
    {
        QAbstractPrintDialog::PrintDialogOptions options;

        if ( printData.GetCollate() )
            options |= QAbstractPrintDialog::PrintCollateCopies;
        if ( printData.GetEnablePrintToFile() )
            options |= QAbstractPrintDialog::PrintToFile;
        if ( printData.GetEnableSelection() )
            options |= QAbstractPrintDialog::PrintSelection;
        if ( printData.GetEnableCurrentPage() )
            options |= QAbstractPrintDialog::PrintCurrentPage;
        if ( printData.GetEnablePageNumbers() )
            options |= QAbstractPrintDialog::PrintPageRange;
        // TODO: Add GetEnableShowPageSize() to wxPrintDialogData
        // if ( printData.GetEnableShowPageSize() )
        //    options |= QAbstractPrintDialog::PrintShowPageSize;

        printDialog->setOptions(options);

        printDialog->setMinMax(printData.GetMinPage(),
                               printData.GetMaxPage());
        printDialog->setFromTo(printData.GetFromPage(),
                               printData.GetToPage());

        QAbstractPrintDialog::PrintRange printRange = QAbstractPrintDialog::AllPages;

        if ( printData.GetSelection() )
            printRange = QAbstractPrintDialog::Selection;
        else if ( printData.GetCurrentPage() )
            printRange = QAbstractPrintDialog::CurrentPage;
        else if ( printData.GetSpecifiedPages() )
            printRange = QAbstractPrintDialog::PageRange;

        printDialog->setPrintRange(printRange);
    }

    return true;
}

bool ConvertFromNative(wxPrintDialogData& printData, QPrintDialog* printDialog = nullptr)
{
    printData.GetPrintData().ConvertFromNative();

    if ( printDialog )
    {
        printData.SetMinPage(printDialog->minPage());
        printData.SetMaxPage(printDialog->maxPage());
        printData.SetFromPage(printDialog->fromPage());
        printData.SetToPage(printDialog->toPage());

        auto printRange = printDialog->printRange();

        printData.SetAllPages(printRange == QAbstractPrintDialog::AllPages);
        printData.SetSelection(printRange == QAbstractPrintDialog::Selection);
        printData.SetCurrentPage(printRange == QAbstractPrintDialog::CurrentPage);
        // TODO: Add SetSpecifiedPages() to wxPrintDialogData
        // printData.SetSpecifiedPages(printRange == QAbstractPrintDialog::PageRange);
    }

    return true;
}

// ConvertToNative/ConvertFromNative for wxQtPageSetupDialog.
bool ConvertToNative(wxPageSetupDialogData& setupData)
{
    auto& data = setupData.GetPrintData();
    data.ConvertToNative();

    auto* nativeData = static_cast<wxQtPrintNativeData*>(data.GetNativeData());
    auto* qtPrinter = nativeData->GetQtPrinter();

    const wxRealPoint minTopLeft     = setupData.GetMinMarginTopLeft();
    const wxRealPoint minBottomRight = setupData.GetMinMarginBottomRight();
    const wxRealPoint topLeft        = setupData.GetMarginTopLeft();
    const wxRealPoint bottomRight    = setupData.GetMarginBottomRight();

    QPageLayout pageLayout = qtPrinter->pageLayout();
    pageLayout.setUnits(QPageLayout::Millimeter);
    pageLayout.setMargins(QMarginsF{topLeft.x, topLeft.y, bottomRight.x, bottomRight.y});
    pageLayout.setMinimumMargins(QMarginsF{minTopLeft.x, minTopLeft.y, minBottomRight.x, minBottomRight.y});

    return qtPrinter->setPageLayout(pageLayout);
}

bool ConvertFromNative(wxPageSetupDialogData& setupData)
{
    auto& data = setupData.GetPrintData();
    data.ConvertFromNative();

    auto* nativeData = static_cast<wxQtPrintNativeData*>(data.GetNativeData());
    auto* qtPrinter = nativeData->GetQtPrinter();

    QPageLayout pageLayout = qtPrinter->pageLayout();
    pageLayout.setUnits(QPageLayout::Millimeter);

    const auto margins = pageLayout.margins();
    const auto minMargins = pageLayout.minimumMargins();

    setupData.SetMarginTopLeft(wxPoint(margins.left(), margins.top()));
    setupData.SetMarginBottomRight(wxPoint(margins.right(), margins.bottom()));
    setupData.SetMinMarginTopLeft(wxPoint(minMargins.left(), minMargins.top()));
    setupData.SetMinMarginBottomRight(wxPoint(minMargins.right(), minMargins.bottom()));

    return true;
}
} // anonymous namespace

//##############################################################################
wxIMPLEMENT_CLASS(wxQtPrintDialog, wxPrintDialogBase);

wxQtPrintDialog::wxQtPrintDialog(wxWindow* parent, wxPrintDialogData* data)
    : m_dialogParent(parent), m_printDialogData(*data)
{
}

wxQtPrintDialog::wxQtPrintDialog(wxWindow* parent, wxPrintData* data)
    : m_dialogParent(parent), m_printDialogData(*data)
{
}

int wxQtPrintDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    ConvertToNative( m_printDialogData );

    auto& data = GetPrintData();
    auto* nativeData = static_cast<wxQtPrintNativeData*>(data.GetNativeData());
    auto* qtPrinter = nativeData->GetQtPrinter();

    QPrintDialog printDialog(qtPrinter, m_dialogParent->GetHandle());

    ConvertToNative( m_printDialogData, &printDialog );

    if ( printDialog.exec() == QDialog::Accepted )
    {
        ConvertFromNative( m_printDialogData, &printDialog );

        m_printerDC = new wxPrinterDC(data);

        return wxID_OK;
    }
    else
    {
        return wxID_CANCEL;
    }
}

wxDC* wxQtPrintDialog::GetPrintDC() { return m_printerDC; }

//##############################################################################
wxIMPLEMENT_CLASS(wxQtPageSetupDialog, wxPageSetupDialogBase);

wxQtPageSetupDialog::wxQtPageSetupDialog(wxWindow* parent, wxPageSetupDialogData* data)
    : m_dialogParent(parent), m_pageSetupData(*data)
{
}

int wxQtPageSetupDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    ConvertToNative( m_pageSetupData );

    auto& data = m_pageSetupData.GetPrintData();
    auto* nativeData = static_cast<wxQtPrintNativeData*>(data.GetNativeData());
    auto* qtPrinter = nativeData->GetQtPrinter();

    QPageSetupDialog pageSetupDialog(qtPrinter, m_dialogParent->GetHandle());

    if ( pageSetupDialog.exec() == QDialog::Accepted )
    {
        ConvertFromNative( m_pageSetupData );

        return wxID_OK;
    }
    else
    {
        return wxID_CANCEL;
    }
}

#endif // wxUSE_PRINTING_ARCHITECTURE
