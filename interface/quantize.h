/////////////////////////////////////////////////////////////////////////////
// Name:        quantize.h
// Purpose:     documentation for wxQuantize class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxQuantize
    @wxheader{quantize.h}

    Performs quantization, or colour reduction, on a wxImage.

    Functions in this class are static and so a wxQuantize object need not be
    created.

    @library{wxcore}
    @category{misc}
*/
class wxQuantize : public wxObject
{
public:
    /**
        Constructor. You do not need to construct a wxQuantize object since its
        functions are static.
    */
    wxQuantize();

    /**
        Converts input bitmap(s) into 8bit representation with custom palette.
        in_rows and out_rows are arrays [0..h-1] of pointer to rows
        (in_rows contains w * 3 bytes per row, out_rows w bytes per row).
        Fills out_rows with indexes into palette (which is also stored into palette
        variable).
    */
    void DoQuantize(unsigned w, unsigned h, unsigned char** in_rows,
                    unsigned char** out_rows,
                    unsigned char* palette,
                    int desiredNoColours);

    //@{
    /**
        This version sets a palette in the destination image so you don't
        have to manage it yourself.
    */
    bool Quantize(const wxImage& src, wxImage& dest,
                  wxPalette** pPalette,
                  int desiredNoColours = 236,
                  unsigned char** eightBitData = 0,
                  int flags = wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE|wxQUANTIZE_RETURN_8BIT_DATA);
    bool Quantize(const wxImage& src, wxImage& dest,
                  int desiredNoColours = 236,
                  unsigned char** eightBitData = 0,
                  int flags = wxQUANTIZE_INCLUDE_WINDOWS_COLOURS|wxQUANTIZE_FILL_DESTINATION_IMAGE|wxQUANTIZE_RETURN_8BIT_DATA);
    //@}
};
