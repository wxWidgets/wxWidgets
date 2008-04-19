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

    This defines the wxWidgets Encapsulated PostScript device context, which
    can write PostScript files on any platform. See wxDC for descriptions of
    the member functions.

    @library{wxbase}
    @category{dc}
*/
class wxPostScriptDC : public wxDC
{
public:
    /**
        Constructs a PostScript printer device context from a wxPrintData
        object.
    */
    wxPostScriptDC(const wxPrintData& printData);
    /**
        Constructor. @a output is an optional file for printing to, and if
        @a interactive is @true a dialog box will be displayed for adjusting
        various parameters. @a parent is the parent of the printer dialog box.

        Use the wxDC::Ok() member to test whether the constructor was
        successful in creating a usable device context.

        See wxPrintData for various functions to set and get PostScript
        printing settings.

        @deprecated This constructor is deprecated.
    */
    wxPostScriptDC(const wxString& output,
                   bool interactive = true,
                   wxWindow* parent);

    /**
        Return resolution used in PostScript output.

        @see SetResolution()
    */
    static int GetResolution();

    /**
        Set resolution (in pixels per inch) that will be used in PostScript
        output. Default is 720ppi.
    */
    static void SetResolution(int ppi);
};

