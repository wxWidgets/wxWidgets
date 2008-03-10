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



// ============================================================================
// Global functions/macros
// ============================================================================

/**
    Gets the printer command used to print a file. The default is @c lpr.
*/
wxString wxGetPrinterCommand();

/**
    Sets the printer command used to print a file. The default is @c lpr.
*/
void wxSetPrinterCommand(const wxString& command);

/**
    Gets the orientation (PS_PORTRAIT or PS_LANDSCAPE). The default is PS_PORTRAIT.
*/
int wxGetPrinterOrientation();

/**
    Sets the additional options for the print command (e.g. specific printer). The
    default is nothing.
*/
void wxSetPrinterOptions(const wxString& options);

/**
    Gets the translation (from the top left corner) for PostScript output. The
    default is 0.0, 0.0.
*/
void wxGetPrinterTranslation(float* x, float* y);

/**
    Sets the scaling factor for PostScript output. The default is 1.0, 1.0.
*/
void wxSetPrinterScaling(float x, float y);

/**
    Sets the orientation (PS_PORTRAIT or PS_LANDSCAPE). The default is PS_PORTRAIT.
*/
void wxSetPrinterOrientation(int orientation);

/**
    Sets the printing mode controlling where output is sent (PS_PREVIEW, PS_FILE or
    PS_PRINTER).
    The default is PS_PREVIEW.
*/
void wxSetPrinterMode(int mode);

/**
    Sets the PostScript output filename.
*/
void wxSetPrinterFile(const wxString& filename);

/**
    Gets the PostScript output filename.
*/
wxString wxGetPrinterFile();

/**
    Gets the additional options for the print command (e.g. specific printer). The
    default is nothing.
*/
wxString wxGetPrinterOptions();

/**
    Gets the command used to view a PostScript file. The default depends on the
    platform.
*/
wxString wxGetPrinterPreviewCommand();

/**
    Gets the printing mode controlling where output is sent (PS_PREVIEW, PS_FILE or
    PS_PRINTER).
    The default is PS_PREVIEW.
*/
int wxGetPrinterMode();

/**
    Gets the scaling factor for PostScript output. The default is 1.0, 1.0.
*/
void wxGetPrinterScaling(float* x, float* y);

/**
    Sets the command used to view a PostScript file. The default depends on the
    platform.
*/
void wxSetPrinterPreviewCommand(const wxString& command);

