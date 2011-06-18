/////////////////////////////////////////////////////////////////////////////
// Name:        colourdata.h
// Purpose:     interface of wxColourData
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxColourData

    This class holds a variety of information related to colour dialogs.

    @library{wxcore}
    @category{cmndlg,data}

    @see wxColour, wxColourDialog, @ref overview_cmndlg_colour
*/
class wxColourData : public wxObject
{
public:
    /**
        Constructor. Initializes the custom colours to @c wxNullColour, the
        @e data colour setting to black, and the @e choose full setting to
        @true.
    */
    wxColourData();

    /**
        Destructor.
    */
    virtual ~wxColourData();

    /**
        Under Windows, determines whether the Windows colour dialog will
        display the full dialog with custom colour selection controls. Under
        PalmOS, determines whether colour dialog will display full rgb colour
        picker or only available palette indexer. Has no meaning under other
        platforms.

        The default value is @true.
    */
    bool GetChooseFull() const;

    /**
        Gets the current colour associated with the colour dialog.

        The default colour is black.
    */
    wxColour& GetColour();

    /**
        Returns custom colours associated with the colour dialog.

        @param i
            An integer between 0 and 15, being any of the 15 custom colours
            that the user has saved. The default custom colours are invalid
            colours.
    */
    wxColour GetCustomColour(int i) const;

    /**
        Under Windows, tells the Windows colour dialog to display the full
        dialog with custom colour selection controls. Under other platforms,
        has no effect.

        The default value is @true.
    */
    void SetChooseFull(bool flag);

    /**
        Sets the default colour for the colour dialog.

        The default colour is black.
    */
    void SetColour(const wxColour& colour);

    /**
        Sets custom colours for the colour dialog.

        @param i
            An integer between 0 and 15 for whatever custom colour you want to
            set. The default custom colours are invalid colours.
        @param colour
            The colour to set
    */
    void SetCustomColour(int i, const wxColour& colour);

    /**
        Converts the colours saved in this class in a string form, separating
        the various colours with a comma.
    */
    wxString ToString() const;

    /**
        Decodes the given string, which should be in the same format returned
        by ToString(), and sets the internal colours.
    */
    bool FromString(const wxString& str);

    /**
        Assignment operator for the colour data.
    */
    wxColourData& operator =(const wxColourData& data);
};
