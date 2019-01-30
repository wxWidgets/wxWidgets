/////////////////////////////////////////////////////////////////////////////
// Name:        dcgraph.h
// Purpose:     interface of wxGCDC
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGCDC

    wxGCDC is a device context that draws on a wxGraphicsContext.

    @remarks
    If Direct2D is a renderer of underlying graphics context, only wxFont objects
    representing TrueType fonts can be used in the font-related functions.

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
    wxGCDC( const wxWindowDC& windowDC );

    /**
       Constructs a wxGCDC from a wxMemoryDC.
    */
    wxGCDC( const wxMemoryDC& memoryDC );

    /**
       Constructs a wxGCDC from a wxPrinterDC.
    */
    wxGCDC( const wxPrinterDC& printerDC );

    /**
       Construct a wxGCDC from an existing graphics context.

       Note that this object takes ownership of @a context and will delete it
       when it is destroyed or when SetGraphicsContext() is called with a
       different context object.
    */
    wxGCDC(wxGraphicsContext* context);

    /**
       Constructs a wxGCDC from a wxEnhMetaFileDC.

       This constructor is only available in wxMSW port and when @c
       wxUSE_ENH_METAFILE build option is enabled, i.e. when wxEnhMetaFileDC
       class itself is available.

       @since 2.9.3
    */
    wxGCDC( const wxEnhMetaFileDC& emfDC );

    wxGCDC();
    virtual ~wxGCDC();

    /**
       Retrieves associated wxGraphicsContext
    */
    wxGraphicsContext* GetGraphicsContext() const;

    /**
       Set the graphics context to be used for this wxGCDC.

       Note that this object takes ownership of @a context and will delete it when
       it is destroyed or when SetGraphicsContext() is called again.
    */
    void SetGraphicsContext(wxGraphicsContext* context);

};

