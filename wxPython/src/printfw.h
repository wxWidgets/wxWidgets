/////////////////////////////////////////////////////////////////////////////
// Name:        printfw.h
// Purpose:     Exposing the class definition of wxPyPrintout so it can be
//              used by wxHtmlPrintout.  Must be included after helpers.h
//
// Author:      Robin Dunn
//
// Created:     29-Oct-1999
// RCS-ID:      $Id$
// Copyright:   (c) 1999 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////



class wxPyPrintout : public wxPrintout {
public:
    wxPyPrintout(const wxString& title) : wxPrintout(title) {}

    DEC_PYCALLBACK_BOOL_INTINT(OnBeginDocument);
    DEC_PYCALLBACK__(OnEndDocument);
    DEC_PYCALLBACK__(OnBeginPrinting);
    DEC_PYCALLBACK__(OnEndPrinting);
    DEC_PYCALLBACK__(OnPreparePrinting);
    DEC_PYCALLBACK_BOOL_INT_pure(OnPrintPage);
    DEC_PYCALLBACK_BOOL_INT(HasPage);

    // Since this one would be tough and ugly to do with the Macros...
    void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);
    void base_GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);

    PYPRIVATE;
};
