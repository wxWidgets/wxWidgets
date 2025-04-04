/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/cmndata.cpp
// Purpose:     Common GDI data
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/cmndata.h"

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/gdicmn.h"
#endif

#include "wx/prntbase.h"
#include "wx/printdlg.h"
#include "wx/paper.h"


wxIMPLEMENT_DYNAMIC_CLASS(wxPrintData, wxObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxPrintDialogData, wxObject);
wxIMPLEMENT_DYNAMIC_CLASS(wxPageSetupDialogData, wxObject);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// Print data
// ----------------------------------------------------------------------------

wxPrintData::wxPrintData()
{
    m_nativeData = wxPrintFactory::GetFactory()->CreatePrintNativeData();
}

wxPrintData::wxPrintData(const wxPrintData&) = default;

void wxPrintData::SetPrivData( char *privData, int len )
{
    if (len > 0)
    {
        m_privData.resize(len);
        memcpy( &m_privData[0], privData, len );
    }
    else
    {
        m_privData.clear();
    }
}

wxPrintData::~wxPrintData() = default;

void wxPrintData::ConvertToNative()
{
    m_nativeData->TransferFrom( *this ) ;
}

void wxPrintData::ConvertFromNative()
{
    m_nativeData->TransferTo( *this ) ;
}

wxPrintData& wxPrintData::operator=(const wxPrintData&) = default;

// Is this data OK for showing the print dialog?
bool wxPrintData::IsOk() const
{
    m_nativeData->TransferFrom( *this );

    return m_nativeData->IsOk();
}

// ----------------------------------------------------------------------------
// Print dialog data
// ----------------------------------------------------------------------------

wxPrintDialogData::wxPrintDialogData()
{
    wxPrintFactory* factory = wxPrintFactory::GetFactory();
    if ( factory->HasOwnPrintToFile() )
        m_printEnablePrintToFile = false;
}

wxPrintDialogData::wxPrintDialogData(const wxPrintData& printData)
    : m_printData(printData)
{
    m_printMinPage = 1;
    m_printMaxPage = 9999;
}

wxPrintDialogData::~wxPrintDialogData()
{
}

void wxPrintDialogData::operator=(const wxPrintData& data)
{
    m_printData = data;
}

void wxPrintDialogData::DoSetWhat(Print what, bool flag)
{
    if ( flag )
        m_printWhat = what;
    else if ( m_printWhat == what )
        m_printWhat = Print::SpecifiedPages;
    //else: the current value is not the one we're clearing, don't do anything
}

void wxPrintDialogData::SetFromPage(int v)
{
    switch ( m_printPageRanges.size() )
    {
        case 0:
            m_printPageRanges.push_back(wxPrintPageRange());
            wxFALLTHROUGH;

        case 1:
            m_printPageRanges[0].fromPage = v;
            break;

        default:
            wxFAIL_MSG("Can't set \"from\" page when multiple ranges are used");
    }
}

void wxPrintDialogData::SetToPage(int v)
{
    switch ( m_printPageRanges.size() )
    {
        case 0:
            m_printPageRanges.push_back(wxPrintPageRange());
            wxFALLTHROUGH;

        case 1:
            m_printPageRanges[0].toPage = v;
            break;

        default:
            wxFAIL_MSG("Can't set \"to\" page when multiple ranges are used");
    }
}

int wxPrintDialogData::GetFromPage() const
{
    // Unlike in the setters above, don't assert even if we have multiple
    // ranges to avoid breaking existing code which had been written before
    // support for multiple ranges was added. Just return the value
    // corresponding to the start of the first range.
    return m_printPageRanges.empty() ? 0 : m_printPageRanges.front().fromPage;
}

int wxPrintDialogData::GetToPage() const
{
    // As above, try to gracefully degrade to the single range case by
    // returning the end of the last range.
    return m_printPageRanges.empty() ? 0 : m_printPageRanges.back().toPage;
}

// ----------------------------------------------------------------------------
// wxPageSetupDialogData
// ----------------------------------------------------------------------------

wxPageSetupDialogData::wxPageSetupDialogData()
{
    CalculatePaperSizeFromId();
}

wxPageSetupDialogData::wxPageSetupDialogData(const wxPrintData& printData)
    : m_printData(printData)
{
    // The wxPrintData paper size overrides these values, unless the size cannot
    // be found.
    CalculatePaperSizeFromId();
}

wxPageSetupDialogData& wxPageSetupDialogData::operator=(const wxPrintData& data)
{
    m_printData = data;
    CalculatePaperSizeFromId();

    return *this;
}

// If a corresponding paper type is found in the paper database, will set the m_printData
// paper size id member as well.
void wxPageSetupDialogData::SetPaperSize(const wxSize& sz)
{
    m_paperSize = sz;

    CalculateIdFromPaperSize();
}

// Sets the wxPrintData id, plus the paper width/height if found in the paper database.
void wxPageSetupDialogData::SetPaperSize(wxPaperSize id)
{
    m_printData.SetPaperId(id);

    CalculatePaperSizeFromId();
}

void wxPageSetupDialogData::SetPrintData(const wxPrintData& printData)
{
    m_printData = printData;
    CalculatePaperSizeFromId();
}

// Use paper size defined in this object to set the wxPrintData
// paper id
void wxPageSetupDialogData::CalculateIdFromPaperSize()
{
    wxASSERT_MSG( (wxThePrintPaperDatabase != nullptr),
                  wxT("wxThePrintPaperDatabase should not be null. Do not create global print dialog data objects.") );

    wxSize sz = GetPaperSize();

    wxPaperSize id = wxThePrintPaperDatabase->GetSize(wxSize(sz.x* 10, sz.y * 10));
    if (id != wxPAPER_NONE)
    {
        m_printData.SetPaperId(id);
    }
}

// Use paper id in wxPrintData to set this object's paper size
void wxPageSetupDialogData::CalculatePaperSizeFromId()
{
    wxASSERT_MSG( (wxThePrintPaperDatabase != nullptr),
                  wxT("wxThePrintPaperDatabase should not be null. Do not create global print dialog data objects.") );

    wxSize sz = wxThePrintPaperDatabase->GetSize(m_printData.GetPaperId());

    if (sz != wxSize(0, 0))
    {
        // sz is in 10ths of a mm, while paper size is in mm
        m_paperSize.x = sz.x / 10;
        m_paperSize.y = sz.y / 10;
    }
}

#endif // wxUSE_PRINTING_ARCHITECTURE
