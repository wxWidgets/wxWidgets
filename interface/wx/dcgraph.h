/////////////////////////////////////////////////////////////////////////////
// Name:        dcgraph.h
// Purpose:     interface of wxGCDC
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGCDC

    wxGCDC is a device context that draws on a wxGraphicsContext.

    @library{wxcore}
    @category{dc}

    @see wxDC, wxGraphicsContext
*/

class wxGCDC: public wxDC
{
public:
    /**
       Constructs a wxGCDC from a wxWindowDC.
    */
    wxGCDC( const wxWindowDC& dc );

    /**
       Constructs a wxGCDC from a wxMemoryDC.
    */
    wxGCDC( const wxMemoryDC& dc );

    /**
       Constructs a wxGCDC from a wxPrinterDC.
    */
    wxGCDC( const wxPrinterDC& dc );

    /**
       Construct a wxGCDC from an existing grtaphics context.
    */
    wxGCDC(wxGraphicsContext* context);

    /**
       Constructs a wxGCDC from a wxEnhMetaFileDC.

       This constructor is only available in wxMSW port and when @c
       wxUSE_ENH_METAFILE build option is enabled, i.e. when wxEnhMetaFileDC
       class itself is available.

       @since 2.9.3
    */
    wxGCDC( const wxEnhMetaFileDC& dc );

    wxGCDC();
    virtual ~wxGCDC();
    /**
       Retrieves associated wxGraphicsContext
    */
    wxGraphicsContext* GetGraphicsContext() const;

    /**
       Set the grpahics context to be used for this wxGCDC.
    */
    void SetGraphicsContext( wxGraphicsContext* ctx );

};

