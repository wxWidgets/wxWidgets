/////////////////////////////////////////////////////////////////////////////
// Name:        dcprint.h
// Purpose:     documentation for wxPrinterDC class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPrinterDC
    @wxheader{dcprint.h}

    A printer device context is specific to MSW and Mac, and allows access to any
    printer with a Windows or Macintosh driver. See wxDC for further
    information on device contexts, and wxDC::GetSize for
    advice on achieving the correct scaling for the page.

    @library{wxcore}
    @category{printing}

    @seealso
    @ref overview_printingoverview "Printing framework overview", wxDC
*/
class wxPrinterDC : public wxDC
{
public:
    //@{
    /**
        Constructor. With empty strings for the first three arguments, the default
        printer dialog is
        displayed. @e device indicates the type of printer and @e output
        is an optional file for printing to. The @e driver parameter is
        currently unused.  Use the @e Ok member to test whether the
        constructor was successful in creating a usable device context.
        
        This constructor is deprecated and retained only for backward compatibility.
    */
    wxPrinterDC(const wxPrintData& printData);
    wxPrinterDC(const wxString& driver, const wxString& device,
                const wxString& output,
                const bool interactive = @true,
                int orientation = wxPORTRAIT);
    //@}

    /**
        Return the rectangle in device coordinates that corresponds to the full paper
        area, including the nonprinting regions of the paper. The point (0,0) in device
        coordinates is the top left corner of the page rectangle, which is the printable
        area on MSW and Mac. The coordinates of the top left corner of the paper
        rectangle will therefore have small negative values, while the bottom right
        coordinates will be somewhat larger than the values returned by
        wxDC::GetSize.
    */
    wxRect wxPrinterDC::GetPaperRect();
};
