/////////////////////////////////////////////////////////////////////////////
// Name:        dcps.h
// Purpose:     interface of wxPostScriptDC
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPostScriptDC
    @wxheader{dcps.h}

    This defines the wxWidgets Encapsulated PostScript device context,
    which can write PostScript files on any platform. See wxDC for
    descriptions of the member functions.

    @library{wxbase}
    @category{dc}
*/
class wxPostScriptDC : public wxDC
{
public:
    //@{
    /**
        Constructor. @a output is an optional file for printing to, and if
        @a interactive is @true a dialog box will be displayed for adjusting
        various parameters. @a parent is the parent of the printer dialog box.
        Use the @e Ok member to test whether the constructor was successful
        in creating a usable device context.
        See @ref overview_printersettings "Printer settings" for functions to set and
        get PostScript printing settings.
        This constructor and the global printer settings are now deprecated;
        use the wxPrintData constructor instead.
    */
    wxPostScriptDC(const wxPrintData& printData);
    wxPostScriptDC(const wxString& output,
                   bool interactive = true,
                   wxWindow* parent);
    //@}

    /**
        Return resolution used in PostScript output. See
        SetResolution().
    */
    static int GetResolution();

    /**
        Set resolution (in pixels per inch) that will be used in PostScript
        output. Default is 720ppi.
    */
    static void SetResolution(int ppi);
};

