/////////////////////////////////////////////////////////////////////////////
// Name:        paper.cpp
// Purpose:     Paper size classes
// Author:      Julian Smart 
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "paper.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_PRINTING_ARCHITECTURE


#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/settings.h"
#include "wx/intl.h"
#endif

#include "wx/paper.h"
#include "wx/module.h"

#include <stdlib.h>
#include <string.h>

#ifdef __WXMSW__
#include <windows.h>
#include <commdlg.h>

#ifndef __WIN32__
#include <print.h>
#endif

#endif
 // End __WXMSW__

IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperType, wxObject)
IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperDatabase, wxList)

/*
 * Paper size database for all platforms
 */

wxPrintPaperType::wxPrintPaperType()
{
    m_paperId = wxPAPER_NONE;
    m_platformId = 0;
    m_paperName = "";
    m_width = 0;
    m_height = 0;
}

wxPrintPaperType::wxPrintPaperType(wxPaperSize paperId, int platformId, const wxString& name, int w, int h)
{
    m_paperId = paperId;
    m_platformId = platformId;
    m_paperName = name;
    m_width = w;
    m_height = h;
}

// Get width and height in points (1/72th of an inch)
wxSize wxPrintPaperType::GetSizeDeviceUnits() const
{
    return wxSize( (int) ((m_width / 10.0) / (25.4 / 72.0)), (int) ((m_height / 10.0) / (25.4 / 72.0)) );
}

/*
 * Print paper database for PostScript
 */

wxPrintPaperDatabase* wxThePrintPaperDatabase = (wxPrintPaperDatabase*) NULL;

wxPrintPaperDatabase::wxPrintPaperDatabase():wxList(wxKEY_STRING)
{
    DeleteContents(TRUE);
}

void wxPrintPaperDatabase::CreateDatabase()
{
/*
  AddPaperType(wxTRANSLATE("A4 210 x 297 mm"), 210, 297);
  AddPaperType(wxTRANSLATE("A3 297 x 420 mm"), 297, 420);
  AddPaperType(wxTRANSLATE("Letter 8 1/2 x 11 in"), 216, 279);
  AddPaperType(wxTRANSLATE("Legal 8 1/2 x 14 in"), 216, 356);
*/

    WXADDPAPER(wxPAPER_LETTER,             DMPAPER_LETTER,             wxTRANSLATE("Letter, 8 1/2 x 11 in"), 2159, 2794);
    WXADDPAPER(wxPAPER_LEGAL,              DMPAPER_LEGAL,              wxTRANSLATE("Legal, 8 1/2 x 14 in"), 2159, 3556);
    WXADDPAPER(wxPAPER_A4,                 DMPAPER_A4,                 wxTRANSLATE("A4 sheet, 210 x 297 mm"), 2100, 2970);
    WXADDPAPER(wxPAPER_CSHEET,             DMPAPER_CSHEET,             wxTRANSLATE("C sheet, 17 x 22 in"), 4318, 5588);
    WXADDPAPER(wxPAPER_DSHEET,             DMPAPER_DSHEET,             wxTRANSLATE("D sheet, 22 x 34 in"), 5588, 8636);
    WXADDPAPER(wxPAPER_ESHEET,             DMPAPER_ESHEET,             wxTRANSLATE("E sheet, 34 x 44 in"), 8636, 11176);
    WXADDPAPER(wxPAPER_LETTERSMALL,        DMPAPER_LETTERSMALL,        wxTRANSLATE("Letter Small, 8 1/2 x 11 in"), 2159, 2794);
    WXADDPAPER(wxPAPER_TABLOID,            DMPAPER_TABLOID,            wxTRANSLATE("Tabloid, 11 x 17 in"), 2794, 4318);
    WXADDPAPER(wxPAPER_LEDGER,             DMPAPER_LEDGER,             wxTRANSLATE("Ledger, 17 x 11 in"), 4318, 2794);
    WXADDPAPER(wxPAPER_STATEMENT,          DMPAPER_STATEMENT,          wxTRANSLATE("Statement, 5 1/2 x 8 1/2 in"), 1397, 2159);
    WXADDPAPER(wxPAPER_EXECUTIVE,          DMPAPER_EXECUTIVE,          wxTRANSLATE("Executive, 7 1/4 x 10 1/2 in"), 1842, 2667);
    WXADDPAPER(wxPAPER_A3,                 DMPAPER_A3,                 wxTRANSLATE("A3 sheet, 297 x 420 mm"), 2970, 4200);
    WXADDPAPER(wxPAPER_A4SMALL,            DMPAPER_A4SMALL,            wxTRANSLATE("A4 small sheet, 210 x 297 mm"), 2100, 2970);
    WXADDPAPER(wxPAPER_A5,                 DMPAPER_A5,                 wxTRANSLATE("A5 sheet, 148 x 210 mm"), 1480, 2100);
    WXADDPAPER(wxPAPER_B4,                 DMPAPER_B4,                 wxTRANSLATE("B4 sheet, 250 x 354 mm"), 2500, 3540);
    WXADDPAPER(wxPAPER_B5,                 DMPAPER_B5,                 wxTRANSLATE("B5 sheet, 182 x 257 millimeter"), 1820, 2570);
    WXADDPAPER(wxPAPER_FOLIO,              DMPAPER_FOLIO,              wxTRANSLATE("Folio, 8 1/2 x 13 in"), 2159, 3302);
    WXADDPAPER(wxPAPER_QUARTO,             DMPAPER_QUARTO,             wxTRANSLATE("Quarto, 215 x 275 mm"), 2150, 2750);
    WXADDPAPER(wxPAPER_10X14,              DMPAPER_10X14,              wxTRANSLATE("10 x 14 in"), 2540, 3556);
    WXADDPAPER(wxPAPER_11X17,              DMPAPER_11X17,              wxTRANSLATE("11 x 17 in"), 2794, 4318);
    WXADDPAPER(wxPAPER_NOTE,               DMPAPER_NOTE,               wxTRANSLATE("Note, 8 1/2 x 11 in"), 2159, 2794);
    WXADDPAPER(wxPAPER_ENV_9,              DMPAPER_ENV_9,              wxTRANSLATE("#9 Envelope, 3 7/8 x 8 7/8 in"), 984, 2254);
    WXADDPAPER(wxPAPER_ENV_10,             DMPAPER_ENV_10,             wxTRANSLATE("#10 Envelope, 4 1/8 x 9 1/2 in"), 1048, 2413);
    WXADDPAPER(wxPAPER_ENV_11,             DMPAPER_ENV_11,             wxTRANSLATE("#11 Envelope, 4 1/2 x 10 3/8 in"), 1143, 2635);
    WXADDPAPER(wxPAPER_ENV_12,             DMPAPER_ENV_12,             wxTRANSLATE("#12 Envelope, 4 3/4 x 11 in"), 1206, 2794);
    WXADDPAPER(wxPAPER_ENV_14,             DMPAPER_ENV_14,             wxTRANSLATE("#14 Envelope, 5 x 11 1/2 in"), 1270, 2921);
    WXADDPAPER(wxPAPER_ENV_DL,             DMPAPER_ENV_DL,             wxTRANSLATE("DL Envelope, 110 x 220 mm"), 1100, 2200);
    WXADDPAPER(wxPAPER_ENV_C5,             DMPAPER_ENV_C5,             wxTRANSLATE("C5 Envelope, 162 x 229 mm"), 1620, 2290);
    WXADDPAPER(wxPAPER_ENV_C3,             DMPAPER_ENV_C3,             wxTRANSLATE("C3 Envelope, 324 x 458 mm"), 3240, 4580);
    WXADDPAPER(wxPAPER_ENV_C4,             DMPAPER_ENV_C4,             wxTRANSLATE("C4 Envelope, 229 x 324 mm"), 2290, 3240);
    WXADDPAPER(wxPAPER_ENV_C6,             DMPAPER_ENV_C6,             wxTRANSLATE("C6 Envelope, 114 x 162 mm"), 1140, 1620);
    WXADDPAPER(wxPAPER_ENV_C65,            DMPAPER_ENV_C65,            wxTRANSLATE("C65 Envelope, 114 x 229 mm"), 1140, 2290);
    WXADDPAPER(wxPAPER_ENV_B4,             DMPAPER_ENV_B4,             wxTRANSLATE("B4 Envelope, 250 x 353 mm"), 2500, 3530);
    WXADDPAPER(wxPAPER_ENV_B5,             DMPAPER_ENV_B5,             wxTRANSLATE("B5 Envelope, 176 x 250 mm"), 1760, 2500);
    WXADDPAPER(wxPAPER_ENV_B6,             DMPAPER_ENV_B6,             wxTRANSLATE("B6 Envelope, 176 x 125 mm"), 1760, 1250);
    WXADDPAPER(wxPAPER_ENV_ITALY,          DMPAPER_ENV_ITALY,          wxTRANSLATE("Italy Envelope, 110 x 230 mm"), 1100, 2300);
    WXADDPAPER(wxPAPER_ENV_MONARCH,        DMPAPER_ENV_MONARCH,        wxTRANSLATE("Monarch Envelope, 3 7/8 x 7 1/2 in"), 984, 1905);
    WXADDPAPER(wxPAPER_ENV_PERSONAL,       DMPAPER_ENV_PERSONAL,       wxTRANSLATE("6 3/4 Envelope, 3 5/8 x 6 1/2 in"), 921, 1651);
    WXADDPAPER(wxPAPER_FANFOLD_US,         DMPAPER_FANFOLD_US,         wxTRANSLATE("US Std Fanfold, 14 7/8 x 11 in"), 3778, 2794);
    WXADDPAPER(wxPAPER_FANFOLD_STD_GERMAN, DMPAPER_FANFOLD_STD_GERMAN, wxTRANSLATE("German Std Fanfold, 8 1/2 x 12 in"), 2159, 3048);
    WXADDPAPER(wxPAPER_FANFOLD_LGL_GERMAN, DMPAPER_FANFOLD_LGL_GERMAN, wxTRANSLATE("German Legal Fanfold, 8 1/2 x 13 in"), 2159, 3302);

/* Another time!
#ifdef __WIN95__
    WXADDPAPER(wxPAPER_ISO_B4,             DMPAPER_ISO_B4,             "B4 (ISO) 250 x 353 mm",
    WXADDPAPER(wxPAPER_JAPANESE_POSTCARD,  DMPAPER_JAPANESE_POSTCARD,  "Japanese Postcard 100 x 148 mm",
    WXADDPAPER(wxPAPER_9X11,               DMPAPER_9X11,               "9 x 11 in",
    WXADDPAPER(wxPAPER_10X11,              DMPAPER_10X11,              "10 x 11 in",
    WXADDPAPER(wxPAPER_15X11,              DMPAPER_15X11,              "15 x 11 in",
    WXADDPAPER(wxPAPER_ENV_INVITE,         DMPAPER_ENV_INVITE,         "Envelope Invite 220 x 220 mm",
    // What does \275 mean?
//    WXADDPAPER(wxPAPER_LETTER_EXTRA,       DMPAPER_LETTER_EXTRA,       "Letter Extra 9 \275 x 12 in",
//    WXADDPAPER(wxPAPER_LEGAL_EXTRA,        DMPAPER_LEGAL_EXTRA,        "Legal Extra 9 \275 x 15 in",
    WXADDPAPER(wxPAPER_TABLOID_EXTRA,      DMPAPER_TABLOID_EXTRA,      "Tabloid Extra 11.69 x 18 in",
    WXADDPAPER(wxPAPER_A4_EXTRA,           DMPAPER_A4_EXTRA,           "A4 Extra 9.27 x 12.69 in",
//    WXADDPAPER(wxPAPER_LETTER_TRANSVERSE,  DMPAPER_LETTER_TRANSVERSE,  "Letter Transverse 8 \275 x 11 in",
    WXADDPAPER(wxPAPER_A4_TRANSVERSE,      DMPAPER_A4_TRANSVERSE,      "A4 Transverse 210 x 297 mm",
//    WXADDPAPER(wxPAPER_LETTER_EXTRA_TRANSVERSE, DMPAPER_LETTER_EXTRA_TRANSVERSE, "Letter Extra Transverse 9\275 x 12 in",
    WXADDPAPER(wxPAPER_A_PLUS,             DMPAPER_A_PLUS,             "SuperA/SuperA/A4 227 x 356 mm",
    WXADDPAPER(wxPAPER_B_PLUS,             DMPAPER_B_PLUS,             "SuperB/SuperB/A3 305 x 487 mm",
    WXADDPAPER(wxPAPER_LETTER_PLUS,        DMPAPER_LETTER_PLUS,        "Letter Plus 8.5 x 12.69 in",
    WXADDPAPER(wxPAPER_A4_PLUS,            DMPAPER_A4_PLUS,            "A4 Plus 210 x 330 mm",
    WXADDPAPER(wxPAPER_A5_TRANSVERSE,      DMPAPER_A5_TRANSVERSE,      "A5 Transverse 148 x 210 mm",
    WXADDPAPER(wxPAPER_B5_TRANSVERSE,      DMPAPER_B5_TRANSVERSE,      "B5 (JIS) Transverse 182 x 257 mm",
    WXADDPAPER(wxPAPER_A3_EXTRA,           DMPAPER_A3_EXTRA,           "A3 Extra 322 x 445 mm",
    WXADDPAPER(wxPAPER_A5_EXTRA,           DMPAPER_A5_EXTRA,           "A5 Extra 174 x 235 mm",
    WXADDPAPER(wxPAPER_B5_EXTRA,           DMPAPER_B5_EXTRA,           "B5 (ISO) Extra 201 x 276 mm",
    WXADDPAPER(wxPAPER_A2,                 DMPAPER_A2,                 "A2 420 x 594 mm",
    WXADDPAPER(wxPAPER_A3_TRANSVERSE,      DMPAPER_A3_TRANSVERSE,      "A3 Transverse 297 x 420 mm",
    WXADDPAPER(wxPAPER_A3_EXTRA_TRANSVERSE DMPAPER_A3_EXTRA_TRANSVERSE "A3 Extra Transverse 322 x 445 mm",
#endif
*/
}

void wxPrintPaperDatabase::ClearDatabase()
{
    Clear();
}

void wxPrintPaperDatabase::AddPaperType(wxPaperSize paperId, const wxString& name, int w, int h)
{
    Append(name, new wxPrintPaperType(paperId, 0, name, w, h));
}

void wxPrintPaperDatabase::AddPaperType(wxPaperSize paperId, int platformId, const wxString& name, int w, int h)
{
    Append(name, new wxPrintPaperType(paperId, platformId, name, w, h));
}

wxPrintPaperType *wxPrintPaperDatabase::FindPaperType(const wxString& name)
{
    wxNode *node = Find(name);
    if (node)
        return (wxPrintPaperType *)node->Data();
    else
        return (wxPrintPaperType *) NULL;
}

wxPrintPaperType *wxPrintPaperDatabase::FindPaperType(wxPaperSize id)
{
    wxNode *node = First();
    while (node)
    {
        wxPrintPaperType* paperType = (wxPrintPaperType*) node->Data();
        if (paperType->GetId() == id)
            return paperType;
        node = node->Next();
    }
    return (wxPrintPaperType *) NULL;
}

wxPrintPaperType *wxPrintPaperDatabase::FindPaperTypeByPlatformId(int id)
{
    wxNode *node = First();
    while (node)
    {
        wxPrintPaperType* paperType = (wxPrintPaperType*) node->Data();
        if (paperType->GetPlatformId() == id)
            return paperType;
        node = node->Next();
    }
    return (wxPrintPaperType *) NULL;
}

wxPrintPaperType *wxPrintPaperDatabase::FindPaperType(const wxSize& sz)
{
    wxNode *node = First();
    while (node)
    {
        wxPrintPaperType* paperType = (wxPrintPaperType*) node->Data();
        if (paperType->GetSize() == sz)
            return paperType;
        node = node->Next();
    }
    return (wxPrintPaperType *) NULL;
}

// Convert name to size id
wxPaperSize wxPrintPaperDatabase::ConvertNameToId(const wxString& name)
{
    wxPrintPaperType* type = FindPaperType(name);
    if (type)
        return type->GetId();
    else
        return wxPAPER_NONE;
}

// Convert size id to name
wxString wxPrintPaperDatabase::ConvertIdToName(wxPaperSize paperId)
{
    wxPrintPaperType* type = FindPaperType(paperId);
    if (type)
        return type->GetName();
    else
        return wxEmptyString;
}

// Get the paper size
wxSize wxPrintPaperDatabase::GetSize(wxPaperSize paperId)
{
    wxPrintPaperType* type = FindPaperType(paperId);
    if (type)
        return type->GetSize();
    else
        return wxSize(0, 0);
}

// Get the paper size
wxPaperSize wxPrintPaperDatabase::GetSize(const wxSize& size)
{
    wxPrintPaperType* type = FindPaperType(size);
    if (type)
        return type->GetId();
    else
        return wxPAPER_NONE;
}

// A module to allow initialization/cleanup of print paper
// things without calling these functions from app.cpp.

class WXDLLEXPORT wxPrintPaperModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxPrintPaperModule)
public:
    wxPrintPaperModule() {}
    bool OnInit();
    void OnExit();
};

IMPLEMENT_DYNAMIC_CLASS(wxPrintPaperModule, wxModule)

/*
 * Initialization/cleanup module
 */

bool wxPrintPaperModule::OnInit()
{
    wxThePrintPaperDatabase = new wxPrintPaperDatabase;
    wxThePrintPaperDatabase->CreateDatabase();

    return TRUE;
}

void wxPrintPaperModule::OnExit()
{
    delete wxThePrintPaperDatabase;
    wxThePrintPaperDatabase = NULL;
}

#endif // wxUSE_PRINTING_ARCHITECTURE

