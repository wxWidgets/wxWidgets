/////////////////////////////////////////////////////////////////////////////
// Name:        cmndata.h
// Purpose:     interface of wxFontData
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxFontData
    @wxheader{cmndata.h}

    @ref overview_wxfontdialogoverview "wxFontDialog overview"

    This class holds a variety of information related to font dialogs.

    @library{wxcore}
    @category{FIXME}

    @see Overview(), wxFont, wxFontDialog
*/
class wxFontData : public wxObject
{
public:
    /**
        Constructor. Initializes @e fontColour to black, @e showHelp to black,
        @e allowSymbols to @true, @e enableEffects to @true,
        @e minSize to 0 and @e maxSize to 0.
    */
    wxFontData();

    /**
        Enables or disables 'effects' under MS Windows or generic only. This refers to
        the
        controls for manipulating colour, strikeout and underline properties.
        The default value is @true.
    */
    void EnableEffects(bool enable);

    /**
        Under MS Windows, returns a flag determining whether symbol fonts can be
        selected. Has no
        effect on other platforms.
        The default value is @true.
    */
    bool GetAllowSymbols();

    /**
        Gets the font chosen by the user if the user pressed OK
        (wxFontDialog::ShowModal returned wxID_OK).
    */
    wxFont GetChosenFont();

    /**
        Gets the colour associated with the font dialog.
        The default value is black.
    */
    wxColour GetColour();

    /**
        Determines whether 'effects' are enabled under Windows. This refers to the
        controls for manipulating colour, strikeout and underline properties.
        The default value is @true.
    */
    bool GetEnableEffects();

    /**
        Gets the font that will be initially used by the font dialog. This should have
        previously been set by the application.
    */
    wxFont GetInitialFont();

    /**
        Returns @true if the Help button will be shown (Windows only).
        The default value is @false.
    */
    bool GetShowHelp();

    /**
        Under MS Windows, determines whether symbol fonts can be selected. Has no
        effect on other platforms.
        The default value is @true.
    */
    void SetAllowSymbols(bool allowSymbols);

    /**
        Sets the font that will be returned to the user (for internal use only).
    */
    void SetChosenFont(const wxFont& font);

    /**
        Sets the colour that will be used for the font foreground colour.
        The default colour is black.
    */
    void SetColour(const wxColour& colour);

    /**
        Sets the font that will be initially used by the font dialog.
    */
    void SetInitialFont(const wxFont& font);

    /**
        Sets the valid range for the font point size (Windows only).
        The default is 0, 0 (unrestricted range).
    */
    void SetRange(int min, int max);

    /**
        Determines whether the Help button will be displayed in the font dialog
        (Windows only).
        The default value is @false.
    */
    void SetShowHelp(bool showHelp);

    /**
        Assignment operator for the font data.
    */
    void operator =(const wxFontData& data);
};



/**
    @class wxPageSetupDialogData
    @wxheader{cmndata.h}

    This class holds a variety of information related to wxPageSetupDialog.

    It contains a wxPrintData member which is used to hold basic printer
    configuration data (as opposed to the
    user-interface configuration settings stored by wxPageSetupDialogData).

    @library{wxcore}
    @category{printing}

    @see @ref overview_printingoverview "Printing framework overview",
    wxPageSetupDialog
*/
class wxPageSetupDialogData : public wxObject
{
public:
    //@{
    /**
        Construct an object from a print data object.
    */
    wxPageSetupDialogData();
    wxPageSetupDialogData(wxPageSetupDialogData& data);
    wxPageSetupDialogData(wxPrintData& printData);
    //@}

    /**
        Destructor.
    */
    ~wxPageSetupDialogData();

    /**
        Enables or disables the 'Help' button (Windows only).
    */
    void EnableHelp(bool flag);

    /**
        Enables or disables the margin controls (Windows only).
    */
    void EnableMargins(bool flag);

    /**
        Enables or disables the orientation control (Windows only).
    */
    void EnableOrientation(bool flag);

    /**
        Enables or disables the paper size control (Windows only).
    */
    void EnablePaper(bool flag);

    /**
        Enables or disables the @b Printer button, which invokes a printer setup dialog.
    */
    void EnablePrinter(bool flag);

    /**
        Returns @true if the dialog will simply return default printer information (such
        as orientation)
        instead of showing a dialog. Windows only.
    */
    bool GetDefaultInfo() const;

    /**
        Returns @true if the page setup dialog will take its minimum margin values from
        the currently
        selected printer properties. Windows only.
    */
    bool GetDefaultMinMargins() const;

    /**
        Returns @true if the printer setup button is enabled.
    */
    bool GetEnableHelp() const;

    /**
        Returns @true if the margin controls are enabled (Windows only).
    */
    bool GetEnableMargins() const;

    /**
        Returns @true if the orientation control is enabled (Windows only).
    */
    bool GetEnableOrientation() const;

    /**
        Returns @true if the paper size control is enabled (Windows only).
    */
    bool GetEnablePaper() const;

    /**
        Returns @true if the printer setup button is enabled.
    */
    bool GetEnablePrinter() const;

    /**
        Returns the right (x) and bottom (y) margins in millimetres.
    */
    wxPoint GetMarginBottomRight() const;

    /**
        Returns the left (x) and top (y) margins in millimetres.
    */
    wxPoint GetMarginTopLeft() const;

    /**
        Returns the right (x) and bottom (y) minimum margins the user can enter
        (Windows only). Units
        are in millimetres
    */
    wxPoint GetMinMarginBottomRight() const;

    /**
        Returns the left (x) and top (y) minimum margins the user can enter (Windows
        only). Units
        are in millimetres
    */
    wxPoint GetMinMarginTopLeft() const;

    /**
        Returns the paper id (stored in the internal wxPrintData object).
        For further information, see wxPrintData::SetPaperId.
    */
    wxPaperSize GetPaperId() const;

    /**
        Returns the paper size in millimetres.
    */
    wxSize GetPaperSize() const;

    /**
        Returns a reference to the @ref overview_wxprintdata "print data" associated
        with this object.
    */
    wxPrintData GetPrintData();

    /**
        Returns @true if the print data associated with the dialog data is valid.
        This can return @false on Windows if the current printer is not set, for example.
        On all other platforms, it returns @true.
    */
    bool IsOk() const;

    /**
        Pass @true if the dialog will simply return default printer information (such as
        orientation)
        instead of showing a dialog. Windows only.
    */
    void SetDefaultInfo(bool flag);

    /**
        Pass @true if the page setup dialog will take its minimum margin values from the
        currently
        selected printer properties. Windows only. Units are in millimetres
    */
    void SetDefaultMinMargins(bool flag);

    /**
        Sets the right (x) and bottom (y) margins in millimetres.
    */
    void SetMarginBottomRight(const wxPoint& pt);

    /**
        Sets the left (x) and top (y) margins in millimetres.
    */
    void SetMarginTopLeft(const wxPoint& pt);

    /**
        Sets the right (x) and bottom (y) minimum margins the user can enter (Windows
        only). Units are
        in millimetres.
    */
    void SetMinMarginBottomRight(const wxPoint& pt);

    /**
        Sets the left (x) and top (y) minimum margins the user can enter (Windows
        only). Units are
        in millimetres.
    */
    void SetMinMarginTopLeft(const wxPoint& pt);

    /**
        Sets the paper size id. For further information, see wxPrintData::SetPaperId.
        Calling this function overrides the explicit paper dimensions passed in
        SetPaperSize().
    */
    void SetPaperId(wxPaperSize& id);

    /**
        Sets the paper size in millimetres. If a corresponding paper id is found, it
        will be set in the
        internal wxPrintData object, otherwise the paper size overrides the paper id.
    */
    void SetPaperSize(const wxSize& size);

    /**
        Sets the @ref overview_wxprintdata "print data" associated with this object.
    */
    void SetPrintData(const wxPrintData& printData);

    //@{
    /**
        Assigns page setup data to this object.
    */
    void operator =(const wxPrintData& data);
    void operator =(const wxPageSetupDialogData& data);
    //@}
};



/**
    @class wxColourData
    @wxheader{cmndata.h}

    This class holds a variety of information related to colour dialogs.

    @library{wxcore}
    @category{FIXME}

    @see wxColour, wxColourDialog, @ref overview_wxcolourdialogoverview
    "wxColourDialog overview"
*/
class wxColourData : public wxObject
{
public:
    /**
        Constructor. Initializes the custom colours to @c wxNullColour,
        the @e data colour setting
        to black, and the @e choose full setting to @true.
    */
    wxColourData();

    /**
        Destructor.
    */
    ~wxColourData();

    /**
        Under Windows, determines whether the Windows colour dialog will display the
        full dialog
        with custom colour selection controls. Under PalmOS, determines whether colour
        dialog
        will display full rgb colour picker or only available palette indexer.
        Has no meaning under other platforms.
        The default value is @true.
    */
    bool GetChooseFull() const;

    /**
        Gets the current colour associated with the colour dialog.
        The default colour is black.
    */
    wxColour GetColour() const;

    /**
        Gets the @e ith custom colour associated with the colour dialog. @a i should
        be an integer between 0 and 15.
        The default custom colours are invalid colours.
    */
    wxColour GetCustomColour(int i) const;

    /**
        Under Windows, tells the Windows colour dialog to display the full dialog
        with custom colour selection controls. Under other platforms, has no effect.
        The default value is @true.
    */
    void SetChooseFull(const bool flag);

    /**
        Sets the default colour for the colour dialog.
        The default colour is black.
    */
    void SetColour(const wxColour& colour);

    /**
        Sets the @e ith custom colour for the colour dialog. @a i should
        be an integer between 0 and 15.
        The default custom colours are invalid colours.
    */
    void SetCustomColour(int i, const wxColour& colour);

    /**
        Assignment operator for the colour data.
    */
    void operator =(const wxColourData& data);
};



/**
    @class wxPrintData
    @wxheader{cmndata.h}

    This class holds a variety of information related to printers and
    printer device contexts. This class is used to create a wxPrinterDC
    and a wxPostScriptDC. It is also used as a data member of wxPrintDialogData
    and wxPageSetupDialogData, as part of the mechanism for transferring data
    between the print dialogs and the application.

    @library{wxcore}
    @category{printing}

    @see @ref overview_printingoverview "Printing framework overview",
    wxPrintDialog, wxPageSetupDialog, wxPrintDialogData, wxPageSetupDialogData, @ref overview_wxprintdialogoverview "wxPrintDialog Overview", wxPrinterDC, wxPostScriptDC
*/
class wxPrintData : public wxObject
{
public:
    //@{
    /**
        Copy constructor.
    */
    wxPrintData();
    wxPrintData(const wxPrintData& data);
    //@}

    /**
        Destructor.
    */
    ~wxPrintData();

    /**
        Returns the current bin (papersource). By default, the system is left to select
        the bin (@c wxPRINTBIN_DEFAULT is returned).
        See SetBin() for the full list of bin values.
    */
    wxPrintBin GetBin() const;

    /**
        Returns @true if collation is on.
    */
    bool GetCollate() const;

    /**
        Returns @true if colour printing is on.
    */
    bool GetColour() const;

    /**
        Returns the duplex mode. One of wxDUPLEX_SIMPLEX, wxDUPLEX_HORIZONTAL,
        wxDUPLEX_VERTICAL.
    */
    wxDuplexMode GetDuplex() const;

    /**
        Returns the number of copies requested by the user.
    */
    int GetNoCopies() const;

    /**
        Gets the orientation. This can be wxLANDSCAPE or wxPORTRAIT.
    */
    int GetOrientation() const;

    /**
        Returns the paper size id. For more information, see SetPaperId().
    */
    wxPaperSize GetPaperId() const;

    /**
        Returns the printer name. If the printer name is the empty string, it indicates
        that the default
        printer should be used.
    */
    const wxString GetPrinterName() const;

    /**
        Returns the current print quality. This can be a positive integer, denoting the
        number of dots per inch, or
        one of the following identifiers:
        
        On input you should pass one of these identifiers, but on return you may get
        back a positive integer
        indicating the current resolution setting.
    */
    wxPrintQuality GetQuality() const;

    /**
        Returns @true if the print data is valid for using in print dialogs.
        This can return @false on Windows if the current printer is not set, for example.
        On all other platforms, it returns @true.
    */
    bool IsOk() const;

    /**
        Sets the current bin. Possible values are:
    */
    void SetBin(wxPrintBin flag);

    /**
        Sets collation to on or off.
    */
    void SetCollate(bool flag);

    /**
        Sets colour printing on or off.
    */
    void SetColour(bool flag);

    /**
        Returns the duplex mode. One of wxDUPLEX_SIMPLEX, wxDUPLEX_HORIZONTAL,
        wxDUPLEX_VERTICAL.
    */
    void SetDuplex(wxDuplexMode mode);

    /**
        Sets the default number of copies to be printed out.
    */
    void SetNoCopies(int n);

    /**
        Sets the orientation. This can be wxLANDSCAPE or wxPORTRAIT.
    */
    void SetOrientation(int orientation);

    /**
        Sets the paper id. This indicates the type of paper to be used. For a mapping
        between
        paper id, paper size and string name, see wxPrintPaperDatabase in @c paper.h
        (not yet documented).
        @a paperId can be one of:
    */
    void SetPaperId(wxPaperSize paperId);

    /**
        Sets the printer name. This can be the empty string to indicate that the default
        printer should be used.
    */
    void SetPrinterName(const wxString& printerName);

    /**
        Sets the desired print quality. This can be a positive integer, denoting the
        number of dots per inch, or
        one of the following identifiers:
        
        On input you should pass one of these identifiers, but on return you may get
        back a positive integer
        indicating the current resolution setting.
    */
    void SetQuality(wxPrintQuality quality);

    //@{
    /**
        Assigns print setup data to this object. wxPrintSetupData is deprecated,
        but retained for backward compatibility.
    */
    void operator =(const wxPrintData& data);
    void operator =(const wxPrintSetupData& data);
    //@}
};



/**
    @class wxPrintDialogData
    @wxheader{cmndata.h}

    This class holds information related to the visual characteristics of
    wxPrintDialog.
    It contains a wxPrintData object with underlying printing settings.

    @library{wxcore}
    @category{printing}

    @see @ref overview_printingoverview "Printing framework overview",
    wxPrintDialog, @ref overview_wxprintdialogoverview "wxPrintDialog Overview"
*/
class wxPrintDialogData : public wxObject
{
public:
    //@{
    /**
        Construct an object from a print dialog data object.
    */
    wxPrintDialogData();
    wxPrintDialogData(wxPrintDialogData& dialogData);
    wxPrintDialogData(wxPrintData& printData);
    //@}

    /**
        Destructor.
    */
    ~wxPrintDialogData();

    /**
        Enables or disables the 'Help' button.
    */
    void EnableHelp(bool flag);

    /**
        Enables or disables the 'Page numbers' controls.
    */
    void EnablePageNumbers(bool flag);

    /**
        Enables or disables the 'Print to file' checkbox.
    */
    void EnablePrintToFile(bool flag);

    /**
        Enables or disables the 'Selection' radio button.
    */
    void EnableSelection(bool flag);

    /**
        Returns @true if the user requested that all pages be printed.
    */
    bool GetAllPages() const;

    /**
        Returns @true if the user requested that the document(s) be collated.
    */
    bool GetCollate() const;

    /**
        Returns the @e from page number, as entered by the user.
    */
    int GetFromPage() const;

    /**
        Returns the @e maximum page number.
    */
    int GetMaxPage() const;

    /**
        Returns the @e minimum page number.
    */
    int GetMinPage() const;

    /**
        Returns the number of copies requested by the user.
    */
    int GetNoCopies() const;

    /**
        Returns a reference to the internal wxPrintData object.
    */
    wxPrintData GetPrintData();

    /**
        Returns @true if the user has selected printing to a file.
    */
    bool GetPrintToFile() const;

    /**
        Returns @true if the user requested that the selection be printed (where
        'selection' is
        a concept specific to the application).
    */
    bool GetSelection() const;

    /**
        Returns the @e to page number, as entered by the user.
    */
    int GetToPage() const;

    /**
        Returns @true if the print data is valid for using in print dialogs.
        This can return @false on Windows if the current printer is not set, for example.
        On all other platforms, it returns @true.
    */
    bool IsOk() const;

    /**
        Sets the 'Collate' checkbox to @true or @false.
    */
    void SetCollate(bool flag);

    /**
        Sets the @e from page number.
    */
    void SetFromPage(int page);

    /**
        Sets the @e maximum page number.
    */
    void SetMaxPage(int page);

    /**
        Sets the @e minimum page number.
    */
    void SetMinPage(int page);

    /**
        Sets the default number of copies the user has requested to be printed out.
    */
    void SetNoCopies(int n);

    /**
        Sets the internal wxPrintData.
    */
    void SetPrintData(const wxPrintData& printData);

    /**
        Sets the 'Print to file' checkbox to @true or @false.
    */
    void SetPrintToFile(bool flag);

    /**
        Selects the 'Selection' radio button. The effect of printing the selection
        depends on how the application
        implements this command, if at all.
    */
    void SetSelection(bool flag);

    /**
        Determines whether the dialog to be shown will be the Print dialog
        (pass @false) or Print Setup dialog (pass @true).
        This function has been deprecated since version 2.5.4.
    */
    void SetSetupDialog(bool flag);

    /**
        Sets the @e to page number.
    */
    void SetToPage(int page);

    //@{
    /**
        Assigns another print dialog data object to this object.
    */
    void operator =(const wxPrintData& data);
    void operator =(const wxPrintDialogData& data);
    //@}
};

