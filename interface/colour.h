/////////////////////////////////////////////////////////////////////////////
// Name:        colour.h
// Purpose:     interface of wxColour
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxColour
    @wxheader{colour.h}

    A colour is an object representing a combination of Red, Green, and Blue (RGB)
    intensity values,
    and is used to determine drawing colours. See the
    entry for wxColourDatabase for how a pointer to a predefined,
    named colour may be returned instead of creating a new colour.

    Valid RGB values are in the range 0 to 255.

    You can retrieve the current system colour settings with wxSystemSettings.

    @library{wxcore}
    @category{gdi}

    @stdobjects
    ::Objects:, ::wxNullColour, ::Pointers:, ::wxBLACK, ::wxWHITE, ::wxRED,
    ::wxBLUE, ::wxGREEN, ::wxCYAN, ::wxLIGHT_GREY,

    @see wxColourDatabase, wxPen, wxBrush, wxColourDialog, wxSystemSettings
*/
class wxColour : public wxObject
{
public:
    //@{
    /**
        Copy constructor.
        
        @param red
            The red value.
        @param green
            The green value.
        @param blue
            The blue value.
        @param alpha
            The alpha value. Alpha values range from 0 (wxALPHA_TRANSPARENT) to 255
        (wxALPHA_OPAQUE).
        @param colourName
            The colour name.
        @param colour
            The colour to copy.
        
        @see wxColourDatabase
    */
    wxColour();
    wxColour(unsigned char red, unsigned char green,
             unsigned char blue,
             unsigned char alpha = wxALPHA_OPAQUE);
    wxColour(const wxString& colourNname);
    wxColour(const wxColour& colour);
    //@}

    /**
        Returns the alpha value, on platforms where alpha is not yet supported, this
        always returns wxALPHA_OPAQUE.
    */
    unsigned char Alpha() const;

    /**
        Returns the blue intensity.
    */
    unsigned char Blue() const;

    //@{
    /**
        is not
        specified in flags.
        
        @wxsince{2.7.0}
    */
    wxString GetAsString(long flags);
    const wxC2S_NAME, to obtain the colour name (e.g.
            wxColour(255,0,0) - "red"), wxC2S_CSS_SYNTAX, to obtain
    the colour in the "rgb(r,g,b)" or "rgba(r,g,b,a)" syntax
    (e.g. wxColour(255,0,0,85) - "rgba(255,0,0,0.333)"), and
    wxC2S_HTML_SYNTAX, to obtain the colour as  "#" followed
    by 6 hexadecimal digits (e.g. wxColour(255,0,0) - "#FF0000").
    This function never fails and always returns a non-empty string but asserts if
    the colour has alpha channel (i.e. is non opaque) but
        wxC2S_CSS_SYNTAX();
    //@}

    /**
        Returns a pixel value which is platform-dependent. On Windows, a COLORREF is
        returned.
        On X, an allocated pixel value is returned.
        -1 is returned if the pixel is invalid (on X, unallocated).
    */
    long GetPixel() const;

    /**
        Returns the green intensity.
    */
    unsigned char Green() const;

    /**
        Returns @true if the colour object is valid (the colour has been initialised
        with RGB values).
    */
    bool IsOk() const;

    /**
        Returns the red intensity.
    */
    unsigned char Red() const;

    //@{
    /**
        Sets the RGB intensity values using the given values (first overload),
        extracting them from the packed long (second overload), using the given string (third overloard).
        When using third form, Set() accepts: colour names (those listed in
        wxTheColourDatabase()), the CSS-like
        @c "rgb(r,g,b)" or @c "rgba(r,g,b,a)" syntax (case insensitive)
        and the HTML-like syntax (i.e. @c "#" followed by 6 hexadecimal digits
        for red, green, blue components).
        Returns @true if the conversion was successful, @false otherwise.
        
        @wxsince{2.7.0}
    */
    void Set(unsigned char red, unsigned char green,
             unsigned char blue,
             unsigned char alpha = wxALPHA_OPAQUE);
    void Set(unsigned long RGB);
    bool Set(const wxString& str);
    //@}

    /**
        Tests the inequality of two colours by comparing individual red, green, blue
        colours and alpha values.
    */
    bool operator !=(const wxColour& colour);

    //@{
    /**
        Assignment operator, using a colour name to be found in the colour database.
        
        @see wxColourDatabase
    */
    wxColour operator =(const wxColour& colour);
    wxColour operator =(const wxString& colourName);
    //@}

    /**
        Tests the equality of two colours by comparing individual red, green, blue
        colours and alpha values.
    */
    bool operator ==(const wxColour& colour);
};


/**
    FIXME
*/
wxColour Objects:
;

/**
    FIXME
*/
wxColour wxNullColour;

/**
    FIXME
*/
wxColour Pointers:
;

/**
    FIXME
*/
wxColour wxBLACK;

/**
    FIXME
*/
wxColour wxWHITE;

/**
    FIXME
*/
wxColour wxRED;

/**
    FIXME
*/
wxColour wxBLUE;

/**
    FIXME
*/
wxColour wxGREEN;

/**
    FIXME
*/
wxColour wxCYAN;

/**
    FIXME
*/
wxColour wxLIGHT_GREY;


