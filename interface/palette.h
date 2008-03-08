/////////////////////////////////////////////////////////////////////////////
// Name:        palette.h
// Purpose:     documentation for wxPalette class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxPalette
    @wxheader{palette.h}
    
    A palette is a table that maps pixel values to RGB colours. It allows the
    colours of a low-depth bitmap, for example, to be mapped to the available
    colours in a display. The notion of palettes is becoming more and more
    obsolete nowadays and only the MSW port is still using a native palette.
    All other ports use generic code which is basically just an array of
    colours. 
    
    It is likely that in the future the only use for palettes within wxWidgets
    will be for representing colour indeces from images (such as GIF or PNG).
    The image handlers for these formats have been modified to create a palette
    if there is such information in the original image file (usually 256 or less
    colour images). See wxImage for more information.
    
    @library{wxcore}
    @category{gdi}
    
    @stdobjects
    Objects:
    wxNullPalette
    
    @seealso
    wxDC::SetPalette, wxBitmap
*/
class wxPalette : public wxGDIObject
{
public:
    //@{
    /**
        Creates a palette from arrays of size @e n, one for each
        red, blue or green component.
        
        @param palette 
        A pointer or reference to the palette to copy.
        
        @param n 
        The number of indices in the palette.
        
        @param red 
        An array of red values.
        
        @param green 
        An array of green values.
        
        @param blue 
        An array of blue values.
        
        @sa Create()
    */
    wxPalette();
        wxPalette(const wxPalette& palette);
        wxPalette(int n, const unsigned char* red,
                  const unsigned char* green,
                  const unsigned char* blue);
    //@}

    /**
        Destructor.
        See @ref overview_refcountdestruct "reference-counted object destruction" for
        more info.
    */
    ~wxPalette();

    /**
        Creates a palette from arrays of size @e n, one for each
        red, blue or green component.
        
        @param n 
        The number of indices in the palette.
        
        @param red 
        An array of red values.
        
        @param green 
        An array of green values.
        
        @param blue 
        An array of blue values.
        
        @returns @true if the creation was successful, @false otherwise.
        
        @sa wxPalette()
    */
    bool Create(int n, const unsigned char* red,
                const unsigned char* green,
                const unsigned char* blue);

    /**
        Returns number of entries in palette.
    */
    int GetColoursCount();

    /**
        Returns a pixel value (index into the palette) for the given RGB values.
        
        @param red 
        Red value.
        
        @param green 
        Green value.
        
        @param blue 
        Blue value.
        
        @returns The nearest palette index or wxNOT_FOUND for unexpected errors.
        
        @sa GetRGB()
    */
    int GetPixel(unsigned char red, unsigned char green,
                 unsigned char blue);

    /**
        Returns RGB values for a given palette index.
        
        @param pixel 
        The palette index.
        
        @param red 
        Receives the red value.
        
        @param green 
        Receives the green value.
        
        @param blue 
        Receives the blue value.
        
        @returns @true if the operation was successful.
        
        @sa GetPixel()
    */
#define bool GetRGB(int pixel, const unsigned char* red,
                const unsigned char* green,
                const unsigned char* blue)     /* implementation is private */

    /**
        Returns @true if palette data is present.
    */
#define bool IsOk()     /* implementation is private */

    /**
        Assignment operator, using @ref overview_trefcount "reference counting".
    */
    wxPalette operator =(const wxPalette& palette);
};
