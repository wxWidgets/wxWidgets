/////////////////////////////////////////////////////////////////////////////
// Name:        gprint.cpp
// Author:      Robert Roebling
// Purpose:     Implement GNOME printing support
// Created:     09/20/04
// Copyright:   Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "gprint.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/gtk/gnome/gprint.h"

#if wxUSE_LIBGNOMEPRINT

#include "math.h"

#include "wx/fontutil.h"
#include "wx/printdlg.h"
#include "wx/gtk/private.h"

#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-pango.h>
#include <libgnomeprintui/gnome-print-dialog.h>

//----------------------------------------------------------------------------
// wxGnomePrintNativeData
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrintNativeData, wxPrintNativeDataBase)

wxGnomePrintNativeData::wxGnomePrintNativeData()
{
    m_config = gnome_print_config_default();
    m_job = gnome_print_job_new( m_config );
}

wxGnomePrintNativeData::~wxGnomePrintNativeData()
{
    g_object_unref (G_OBJECT (m_config));
    g_object_unref (G_OBJECT (m_job));
}

bool wxGnomePrintNativeData::TransferTo( wxPrintData &data )
{
    // TODO
    return true;
}

bool wxGnomePrintNativeData::TransferFrom( const wxPrintData &data )
{
    // TODO
    return true;
}
    
//----------------------------------------------------------------------------
// wxGnomePrintFactory
//----------------------------------------------------------------------------

wxPrinterBase* wxGnomePrintFactory::CreatePrinter( wxPrintDialogData *data )
{
    return new wxGnomePrinter( data );
}
        
wxPrintPreviewBase *wxGnomePrintFactory::CreatePrintPreview( wxPrintout *preview, 
                                                    wxPrintout *printout, 
                                                    wxPrintDialogData *data )
{
    return new wxPostScriptPrintPreview( preview, printout, data );
}

wxPrintPreviewBase *wxGnomePrintFactory::CreatePrintPreview( wxPrintout *preview, 
                                                    wxPrintout *printout, 
                                                    wxPrintData *data )
{
    return new wxPostScriptPrintPreview( preview, printout, data );
}

wxPrintDialogBase *wxGnomePrintFactory::CreatePrintDialog( wxWindow *parent, 
                                                  wxPrintDialogData *data )
{
    return new wxGnomePrintDialog( parent, data );
}

wxPrintDialogBase *wxGnomePrintFactory::CreatePrintDialog( wxWindow *parent, 
                                                  wxPrintData *data )
{
    return new wxGnomePrintDialog( parent, data );
}
                                                  
bool wxGnomePrintFactory::HasPrintSetupDialog()
{
    return false;
}

wxDialog *wxGnomePrintFactory::CreatePrintSetupDialog( wxWindow *parent, wxPrintData *data )
{
    return NULL;
}

bool wxGnomePrintFactory::HasOwnPrintToFile()
{
    return true;
}

bool wxGnomePrintFactory::HasPrinterLine()
{
    return true;
}

wxString wxGnomePrintFactory::CreatePrinterLine()
{
    // redundant now
    return wxEmptyString;
}

bool wxGnomePrintFactory::HasStatusLine()
{
    // redundant now
    return true;    
}

wxString wxGnomePrintFactory::CreateStatusLine()
{
    // redundant now
    return wxEmptyString;
}

wxPrintNativeDataBase *wxGnomePrintFactory::CreatePrintNativeData()
{
    return new wxGnomePrintNativeData;
}

//----------------------------------------------------------------------------
// wxGnomePrintSetupDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrintDialog, wxPrintDialogBase)

wxGnomePrintDialog::wxGnomePrintDialog( wxWindow *parent, wxPrintDialogData *data )
                    : wxPrintDialogBase(parent, wxID_ANY, _("Print"),
                               wxPoint(0, 0), wxSize(600, 600),
                               wxDEFAULT_DIALOG_STYLE |
                               wxTAB_TRAVERSAL)
{
    if (data)
        m_printDialogData = *data;
    
    Init();
}

wxGnomePrintDialog::wxGnomePrintDialog( wxWindow *parent, wxPrintData *data )
                    : wxPrintDialogBase(parent, wxID_ANY, _("Print"),
                               wxPoint(0, 0), wxSize(600, 600),
                               wxDEFAULT_DIALOG_STYLE |
                               wxTAB_TRAVERSAL)
{
    if (data)
        m_printDialogData = *data;

    Init();
}

void wxGnomePrintDialog::Init()
{
    wxPrintData data = m_printDialogData.GetPrintData();

    wxGnomePrintNativeData *native =
      (wxGnomePrintNativeData*) data.GetNativeData();
      
    m_widget = gnome_print_dialog_new( native->GetPrintJob(), 
                                       (guchar*)"Print",  
                                       GNOME_PRINT_DIALOG_RANGE|GNOME_PRINT_DIALOG_COPIES );
}

wxGnomePrintDialog::~wxGnomePrintDialog()
{
    m_widget = NULL;
}

int wxGnomePrintDialog::ShowModal()
{
    // Transfer data from m_printDalogData to dialog here

    int response = gtk_dialog_run (GTK_DIALOG (m_widget));
    gtk_widget_destroy(m_widget);
    m_widget = NULL;
    
	if (response == GNOME_PRINT_DIALOG_RESPONSE_CANCEL)
        return wxID_CANCEL;

    // Transfer data back here

    return wxID_OK;
}

wxDC *wxGnomePrintDialog::GetPrintDC()
{
    // Later
    return NULL;
}

bool wxGnomePrintDialog::Validate()
{
    return true;
}

bool wxGnomePrintDialog::TransferDataToWindow()
{
    return true;
}

bool wxGnomePrintDialog::TransferDataFromWindow()
{
    return true;
}

//----------------------------------------------------------------------------
// wxGnomePrinter
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrinter, wxPrinterBase)

wxGnomePrinter::wxGnomePrinter( wxPrintDialogData *data ) :
    wxPrinterBase( data )
{
    m_gpc = NULL;
}

wxGnomePrinter::~wxGnomePrinter()
{
}

bool wxGnomePrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt )
{
    if (!printout)
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    wxPrintData printdata = GetPrintDialogData().GetPrintData();
    wxGnomePrintNativeData *data = 
        (wxGnomePrintNativeData*) printdata.GetNativeData();

    // The GnomePrintJob is temporarily stored in the 
    // native print data as the native print dialog
    // needs to access it.
    GnomePrintJob *job = data->GetPrintJob();
    m_gpc = gnome_print_job_get_context (job);

    printout->SetIsPreview(false);

    if (m_printDialogData.GetMinPage() < 1)
        m_printDialogData.SetMinPage(1);
    if (m_printDialogData.GetMaxPage() < 1)
        m_printDialogData.SetMaxPage(9999);
    
    wxDC *dc;
    if (prompt)
        dc = PrintDialog( parent );
    else
        dc = new wxGnomePrintDC( this );
    
    if (!dc)
    {
        gnome_print_job_close( job );
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    wxSize ScreenPixels = wxGetDisplaySize();
    wxSize ScreenMM = wxGetDisplaySizeMM();

    printout->SetPPIScreen( (int) ((ScreenPixels.GetWidth() * 25.4) / ScreenMM.GetWidth()),
                            (int) ((ScreenPixels.GetHeight() * 25.4) / ScreenMM.GetHeight()) );
    printout->SetPPIPrinter( wxPostScriptDC::GetResolution(),
                             wxPostScriptDC::GetResolution() );
                             
    printout->SetDC(dc);

    int w, h;
    dc->GetSize(&w, &h);
    printout->SetPageSizePixels((int)w, (int)h);
    dc->GetSizeMM(&w, &h);
    printout->SetPageSizeMM((int)w, (int)h);
    
    printout->OnPreparePrinting();
    printout->OnBeginPrinting();

    if (!printout->OnBeginDocument(0, 0))
    {
        sm_lastError = wxPRINTER_ERROR;
    }
    else
    {
        int pn;
        for (pn = 1; pn <= 2; pn++)
        {
            dc->StartPage();
            printout->OnPrintPage(pn);
            dc->EndPage();
        }
    
        printout->OnEndDocument();
        printout->OnEndPrinting();
    }
    
    gnome_print_job_close( job );
    gnome_print_job_print( job );
    
    delete dc;
    
    return (sm_lastError == wxPRINTER_NO_ERROR);
}

wxDC* wxGnomePrinter::PrintDialog( wxWindow *parent )
{
    wxGnomePrintDialog dialog( parent, &m_printDialogData );
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        sm_lastError = wxPRINTER_ERROR;
        return NULL;
    }
    
    return new wxGnomePrintDC( this );
}

bool wxGnomePrinter::Setup( wxWindow *parent )
{
    return false;
}

//-----------------------------------------------------------------------------
// wxGnomePrintDC
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrintDC, wxDCBase)

wxGnomePrintDC::wxGnomePrintDC( wxGnomePrinter *printer )
{
    m_printer = printer;
    
    m_gpc = printer->GetPrintContext();
    
    m_layout = gnome_print_pango_create_layout( m_gpc );
    m_fontdesc = pango_font_description_from_string( "Sans 12" );
    
    m_currentRed = 0;
    m_currentBlue = 0;
    m_currentGreen = 0;
    
    m_signX =  1;  // default x-axis left to right
    m_signY = -1;  // default y-axis bottom up -> top down
}

wxGnomePrintDC::~wxGnomePrintDC()
{
}

bool wxGnomePrintDC::Ok() const
{
    return true;
}

bool wxGnomePrintDC::DoFloodFill(wxCoord x1, wxCoord y1, const wxColour &col, int style )
{
    return false;
}

bool wxGnomePrintDC::DoGetPixel(wxCoord x1, wxCoord y1, wxColour *col) const
{
    return false;
}

void wxGnomePrintDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    if  (m_pen.GetStyle() == wxTRANSPARENT) return;
    
    SetPen( m_pen );

	gnome_print_moveto ( m_gpc, XLOG2DEV(x1), YLOG2DEV(y1) );
	gnome_print_lineto ( m_gpc, XLOG2DEV(x2), YLOG2DEV(y2) );
	gnome_print_stroke ( m_gpc);
	
    CalcBoundingBox( x1, y1 );
    CalcBoundingBox( x2, y2 );
}

void wxGnomePrintDC::DoCrossHair(wxCoord x, wxCoord y)
{
}

void wxGnomePrintDC::DoDrawArc(wxCoord x1,wxCoord y1,wxCoord x2,wxCoord y2,wxCoord xc,wxCoord yc)
{
}

void wxGnomePrintDC::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
}

void wxGnomePrintDC::DoDrawPoint(wxCoord x, wxCoord y)
{
}

void wxGnomePrintDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
}

void wxGnomePrintDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
}

void wxGnomePrintDC::DoDrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
}

void wxGnomePrintDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );
        
        gnome_print_newpath( m_gpc );
        gnome_print_moveto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
        gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y) );
        gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y + height) );
        gnome_print_lineto( m_gpc, XLOG2DEV(x), YLOG2DEV(y + height) );
        gnome_print_closepath( m_gpc );
        gnome_print_fill( m_gpc );

        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);

        gnome_print_newpath( m_gpc );
        gnome_print_moveto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
        gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y) );
        gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y + height) );
        gnome_print_lineto( m_gpc, XLOG2DEV(x), YLOG2DEV(y + height) );
        gnome_print_closepath( m_gpc );
        gnome_print_stroke( m_gpc );
        
        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }
}

void wxGnomePrintDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
}

void wxGnomePrintDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxGnomePrintDC::DoDrawSpline(wxList *points)
{
}

bool wxGnomePrintDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
            wxDC *source, wxCoord xsrc, wxCoord ysrc, int rop, bool useMask,
            wxCoord xsrcMask, wxCoord ysrcMask)
{
    return false;
}

void wxGnomePrintDC::DoDrawIcon( const wxIcon& icon, wxCoord x, wxCoord y )
{
    DoDrawBitmap( icon, x, y, true );
}

void wxGnomePrintDC::DoDrawBitmap( const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask )
{
    if (!bitmap.Ok()) return;
    
    if (bitmap.HasPixbuf())
    {
        GdkPixbuf *pixbuf = bitmap.GetPixbuf();
	    guchar *raw_image = gdk_pixbuf_get_pixels( pixbuf );
	    bool has_alpha = gdk_pixbuf_get_has_alpha( pixbuf );
	    int rowstride = gdk_pixbuf_get_rowstride( pixbuf );
	    int height = gdk_pixbuf_get_height( pixbuf );
	    int width = gdk_pixbuf_get_width( pixbuf );
        
        gnome_print_gsave( m_gpc );        
        double matrix[6];
    	matrix[0] = XLOG2DEVREL(width);
    	matrix[1] = 0;
    	matrix[2] = 0;
    	matrix[3] = YLOG2DEVREL(height);
    	matrix[4] = XLOG2DEV(x);
        matrix[5] = YLOG2DEV(y+height);
    	gnome_print_concat( m_gpc, matrix );
	    gnome_print_moveto(  m_gpc, 0, 0 );
        if (has_alpha)
            gnome_print_rgbaimage( m_gpc, (guchar *)raw_image, width, height, rowstride );
        else
            gnome_print_rgbimage( m_gpc, (guchar *)raw_image, width, height, rowstride );
        gnome_print_grestore( m_gpc );
    }
    else
    {
        wxImage image = bitmap.ConvertToImage();

        if (!image.Ok()) return;

        gnome_print_gsave( m_gpc );        
        double matrix[6];
    	matrix[0] = XLOG2DEVREL(image.GetWidth());
    	matrix[1] = 0;
    	matrix[2] = 0;
    	matrix[3] = YLOG2DEVREL(image.GetHeight());
    	matrix[4] = XLOG2DEV(x);
        matrix[5] = YLOG2DEV(y+image.GetHeight());
    	gnome_print_concat( m_gpc, matrix );
	    gnome_print_moveto(  m_gpc, 0, 0 );
        gnome_print_rgbimage( m_gpc, (guchar*) image.GetData(), image.GetWidth(), image.GetHeight(), image.GetWidth()*3 );
        gnome_print_grestore( m_gpc );
    }
}

void wxGnomePrintDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y )
{
    x = XLOG2DEV(x);
    y = YLOG2DEV(y);
    
    bool underlined = m_font.Ok() && m_font.GetUnderlined();

#if wxUSE_UNICODE
    const wxCharBuffer data = wxConvUTF8.cWC2MB( text );
#else
    const wxWCharBuffer wdata = wxConvLocal.cMB2WC( text );
    if ( !wdata )
        return;
    const wxCharBuffer data = wxConvUTF8.cWC2MB( wdata );
#endif

    size_t datalen = strlen((const char*)data);
    pango_layout_set_text( m_layout, (const char*) data, datalen);
    
    if (underlined)
    {
        PangoAttrList *attrs = pango_attr_list_new();
        PangoAttribute *a = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
        a->start_index = 0;
        a->end_index = datalen;
        pango_attr_list_insert(attrs, a);
        pango_layout_set_attributes(m_layout, attrs);
        pango_attr_list_unref(attrs);
    }

    if (m_textForegroundColour.Ok())
    {
        unsigned char red = m_textForegroundColour.Red();
        unsigned char blue = m_textForegroundColour.Blue();
        unsigned char green = m_textForegroundColour.Green();

        // Set the equivalent PangoAttrStyle
    }

    int w,h;

    if (fabs(m_scaleY - 1.0) > 0.00001)
    {
        // If there is a user or actually any scale applied to
        // the device context, scale the font.
         
        // scale font description
        gint oldSize = pango_font_description_get_size( m_fontdesc );
        double size = oldSize;
        size = size * m_scaleY;
        pango_font_description_set_size( m_fontdesc, (gint)size );
         
        // actually apply scaled font
        pango_layout_set_font_description( m_layout, m_fontdesc );
        
        pango_layout_get_pixel_size( m_layout, &w, &h );
#if 0
        if ( m_backgroundMode == wxSOLID )
        {
            gdk_gc_set_foreground(m_textGC, m_textBackgroundColour.GetColor());
            gdk_draw_rectangle(m_window, m_textGC, TRUE, x, y, w, h);
            gdk_gc_set_foreground(m_textGC, m_textForegroundColour.GetColor());
        }
#endif         
        // Draw layout.
    	gnome_print_moveto (m_gpc, x, y);
	    gnome_print_pango_layout( m_gpc, m_layout );
         
        // reset unscaled size
        pango_font_description_set_size( m_fontdesc, oldSize );
         
        // actually apply unscaled font
        pango_layout_set_font_description( m_layout, m_fontdesc );
    }
    else
    {
        pango_layout_get_pixel_size( m_layout, &w, &h );
#if 0
        if ( m_backgroundMode == wxSOLID )
        {
            gdk_gc_set_foreground(m_textGC, m_textBackgroundColour.GetColor());
            gdk_draw_rectangle(m_window, m_textGC, TRUE, x, y, w, h);
            gdk_gc_set_foreground(m_textGC, m_textForegroundColour.GetColor());
        }
#endif        
        // Draw layout.
    	gnome_print_moveto (m_gpc, x, y);
	    gnome_print_pango_layout( m_gpc, m_layout );
    }



    if (underlined)
    {
        // undo underline attributes setting:
        pango_layout_set_attributes(m_layout, NULL);
    }
    
//    CalcBoundingBox (x + width, y + height);
    CalcBoundingBox (x, y);
}

void wxGnomePrintDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
{
}

void wxGnomePrintDC::Clear()
{
}

void wxGnomePrintDC::SetFont( const wxFont& font )
{
    m_font = font;
    
    if (m_font.Ok())
    {
        if (m_fontdesc)
            pango_font_description_free( m_fontdesc );
        
        m_fontdesc = pango_font_description_copy( m_font.GetNativeFontInfo()->description );
        
        pango_layout_set_font_description( m_layout, m_fontdesc );
    }
}

void wxGnomePrintDC::SetPen( const wxPen& pen )
{
    if (!pen.Ok()) return;

    // TODO: support for pen styles other than solid (use gnome_print_setdash)

    m_pen = pen;

    gnome_print_setlinewidth( m_gpc, XLOG2DEVREL( 1000 * m_pen.GetWidth() ) / 1000.0f );

    unsigned char red = m_pen.GetColour().Red();
    unsigned char blue = m_pen.GetColour().Blue();
    unsigned char green = m_pen.GetColour().Green();

    if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
    {
        double redPS = (double)(red) / 255.0;
        double bluePS = (double)(blue) / 255.0;
        double greenPS = (double)(green) / 255.0;

        gnome_print_setrgbcolor( m_gpc, redPS, bluePS, greenPS );

        m_currentRed = red;
        m_currentBlue = blue;
        m_currentGreen = green;
    }
}

void wxGnomePrintDC::SetBrush( const wxBrush& brush )
{
    if (!brush.Ok()) return;

    m_brush = brush;

    // Brush colour
    unsigned char red = m_brush.GetColour().Red();
    unsigned char blue = m_brush.GetColour().Blue();
    unsigned char green = m_brush.GetColour().Green();

    if (!m_colour)
    {
        // Anything not white is black
        if (! (red == (unsigned char) 255 &&
               blue == (unsigned char) 255 &&
               green == (unsigned char) 255) )
        {
            red = (unsigned char) 0;
            green = (unsigned char) 0;
            blue = (unsigned char) 0;
        }
        // setgray here ?
    }

    if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
    {
        double redPS = (double)(red) / 255.0;
        double bluePS = (double)(blue) / 255.0;
        double greenPS = (double)(green) / 255.0;

        gnome_print_setrgbcolor( m_gpc, redPS, bluePS, greenPS );

        m_currentRed = red;
        m_currentBlue = blue;
        m_currentGreen = green;
    }
}

void wxGnomePrintDC::SetLogicalFunction( int function )
{
}

void wxGnomePrintDC::SetBackground( const wxBrush& brush )
{
}

void wxGnomePrintDC::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
}

void wxGnomePrintDC::DestroyClippingRegion()
{
}

bool wxGnomePrintDC::StartDoc(const wxString& message)
{
    SetDeviceOrigin( 0,0 );
    
    return true;
}

void wxGnomePrintDC::EndDoc()
{
}

void wxGnomePrintDC::StartPage()
{
	gnome_print_beginpage( m_gpc, (const guchar*) "1" );
}

void wxGnomePrintDC::EndPage()
{
	gnome_print_showpage( m_gpc );
}

wxCoord wxGnomePrintDC::GetCharHeight() const
{
    return 0;
}

wxCoord wxGnomePrintDC::GetCharWidth() const
{
    return 0;
}

void wxGnomePrintDC::DoGetTextExtent(const wxString& string, wxCoord *width, wxCoord *height,
                     wxCoord *descent,
                     wxCoord *externalLeading,
                     wxFont *theFont ) const
{
    if ( width )
        *width = 0;
    if ( height )
        *height = 0;
    if ( descent )
        *descent = 0;
    if ( externalLeading )
        *externalLeading = 0;

    if (string.IsEmpty())
    {
        return;
    }
    
    // Set new font description
    if (theFont)
        pango_layout_set_font_description( m_layout, theFont->GetNativeFontInfo()->description );
        
    // Set layout's text
#if wxUSE_UNICODE
    const wxCharBuffer data = wxConvUTF8.cWC2MB( string );
    const char *dataUTF8 = (const char *)data;
#else
    const wxWCharBuffer wdata = wxConvLocal.cMB2WC( string );
    if ( !wdata )
    {
        if (width) (*width) = 0;
        if (height) (*height) = 0;
        return;
    }
    const wxCharBuffer data = wxConvUTF8.cWC2MB( wdata );
    const char *dataUTF8 = (const char *)data;
#endif

    if ( !dataUTF8 )
    {
        // hardly ideal, but what else can we do if conversion failed?
        return;
    }

    pango_layout_set_text( m_layout, dataUTF8, strlen(dataUTF8) );
 
    int w,h;
    pango_layout_get_pixel_size( m_layout, &w, &h );
    
    if (width)
        *width = (wxCoord) w; 
    if (height)
        *height = (wxCoord) h;
    if (descent)
    {
        PangoLayoutIter *iter = pango_layout_get_iter(m_layout);
        int baseline = pango_layout_iter_get_baseline(iter);
        pango_layout_iter_free(iter);
        *descent = h - PANGO_PIXELS(baseline);
    }
    
    // Reset old font description
    if (theFont)
        pango_layout_set_font_description( m_layout, m_fontdesc );
}

void wxGnomePrintDC::DoGetSize(int* width, int* height) const
{
    // No idea if that is efficient
    GnomePrintConfig *config = gnome_print_config_default();

    double w,h;
    bool result = gnome_print_config_get_page_size( config, &w, &h );
    
    if (!result)
    {
        // Standard PS resolution DIN A4 size.
        w = 595.0;
        h = 842.0;
    }
    
    if (width)
        *width = (int) w;
    if (height)
        *height = (int) h;
}

void wxGnomePrintDC::DoGetSizeMM(int *width, int *height) const
{
    double w,h;
    
    /// Later, for now DIN A4
    w = 210.0;
    h = 297.0;
    
    if (width)
        *width = (int) w;
    if (height)
        *height = (int) h;
}

wxSize wxGnomePrintDC::GetPPI() const
{
    return wxSize(72,72);
}

void wxGnomePrintDC::SetAxisOrientation( bool xLeftRight, bool yBottomUp )
{
    m_signX = (xLeftRight ? 1 : -1);
    m_signY = (yBottomUp  ? 1 : -1);

    ComputeScaleAndOrigin();
}

void wxGnomePrintDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    int h = 0;
    int w = 0;
    GetSize( &w, &h );

    wxDC::SetDeviceOrigin( x, h-y );
}

void wxGnomePrintDC::SetResolution(int ppi)
{
}

int wxGnomePrintDC::GetResolution()
{
    return 72;
}


class wxGnomePrintModule: public wxModule
{
public:
    wxGnomePrintModule() {}
    bool OnInit() { wxPrintFactory::SetPrintFactory( new wxGnomePrintFactory ); return true; }
    void OnExit() { }
    
private:
    DECLARE_DYNAMIC_CLASS(wxGnomePrintModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxGnomePrintModule, wxModule)
   
#endif
    // wxUSE_LIBGNOMEPRINT
