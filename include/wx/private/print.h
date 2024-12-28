///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/print.h
// Purpose:     Private printing-related helpers.
// Author:      Vadim Zeitlin
// Created:     2024-12-28
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_PRINT_H_
#define _WX_PRIVATE_PRINT_H_

// ----------------------------------------------------------------------------
// RAII helper ensuring OnEndPrinting() is called on scope exit
// ----------------------------------------------------------------------------

class wxPrintingGuard
{
public:
    explicit wxPrintingGuard(wxPrintout *printout)
        : m_printout(printout)
    {
        m_printout->OnBeginPrinting();
    }

    ~wxPrintingGuard()
    {
        m_printout->OnEndPrinting();
    }

private:
    wxPrintout * const m_printout;

    wxDECLARE_NO_COPY_CLASS(wxPrintingGuard);
};

// ----------------------------------------------------------------------------
// Another helper ensuring EndPage() is called on scope exit
// ----------------------------------------------------------------------------

class wxPrintingPageGuard
{
public:
    explicit wxPrintingPageGuard(wxDC& dc)
        : m_dc(dc)
    {
        m_dc.StartPage();
    }

    ~wxPrintingPageGuard()
    {
        m_dc.EndPage();
    }

private:
    wxDC& m_dc;

    wxDECLARE_NO_COPY_CLASS(wxPrintingPageGuard);
};

#endif // _WX_PRIVATE_PRINT_H_
