/////////////////////////////////////////////////////////////////////////////
// Name:        gprint.cpp
// Author:      Robert Roebling
// Purpose:     Implement GNOME printing support
// Created:     09/20/04
// Copyright:   Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "gprint.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "gprint.h"
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
    return new wxGenericPrintDialog( parent, data );
}

wxPrintDialogBase *wxGnomePrintFactory::CreatePrintDialog( wxWindow *parent, 
                                                  wxPrintData *data )
{
    return new wxGenericPrintDialog( parent, data );
}
                                                  
bool wxGnomePrintFactory::HasPrintSetupDialog()
{
    return true;
}

wxDialog *wxGnomePrintFactory::CreatePrintSetupDialog( wxWindow *parent, wxPrintData *data )
{
    return new wxGnomePrintSetupDialog( parent, data );
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
    // We should query "gnome_config_default" here
    return _("GNOME print");
}

bool wxGnomePrintFactory::HasStatusLine()
{
    return true;    
}

wxString wxGnomePrintFactory::CreateStatusLine()
{
    // We should query "gnome_config_default" here
    return _("Ready");
}

wxPrintNativeDataBase *wxGnomePrintFactory::CreatePrintNativeData()
{
    return new wxGnomePrintNativeData;
}

//----------------------------------------------------------------------------
// wxGnomePrintSetupDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrintSetupDialog, wxDialog)

wxGnomePrintSetupDialog::wxGnomePrintSetupDialog( wxWindow *parent, wxPrintData *data )
{
    wxGnomePrintNativeData *native =
      (wxGnomePrintNativeData*) data->GetNativeData();
      
    m_widget = gnome_print_dialog_new (native->GetPrintJob(), (guchar*)"Print setup", 0);
}

wxGnomePrintSetupDialog::~wxGnomePrintSetupDialog()
{
    m_widget = NULL;
}

int wxGnomePrintSetupDialog::ShowModal()
{
    int response = gtk_dialog_run (GTK_DIALOG (m_widget));
    gtk_widget_destroy(m_widget);
    m_widget = NULL;
    
	if (response == GNOME_PRINT_DIALOG_RESPONSE_CANCEL)
        return wxID_CANCEL;

    return wxID_OK;
}

bool wxGnomePrintSetupDialog::Validate()
{
    return true;
}

bool wxGnomePrintSetupDialog::TransferDataToWindow()
{
    return true;
}

bool wxGnomePrintSetupDialog::TransferDataFromWindow()
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

    GnomePrintJob *job = data->GetPrintJob();
    m_gpc = gnome_print_job_get_context (job);
    
    wxDC *dc;
    
    if (prompt)
        dc = PrintDialog( parent );
    else
        dc = new wxGnomePrintDC( this );
    
    if (!dc)
    {
        gnome_print_job_close( job );
        return false;
    }

    printout->SetDC(dc);

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
    wxPrintDialog dialog( parent, &m_printDialogData );
    if (dialog.ShowModal() == wxID_CANCEL)
    {
        sm_lastError = wxPRINTER_ERROR;
        return NULL;
    }
    
    return new wxGnomePrintDC( this );
}

bool wxGnomePrinter::Setup( wxWindow *parent )
{
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

	gnome_print_moveto ( m_gpc, XDEV2LOG(x1), YDEV2LOG(y1) );
	gnome_print_lineto ( m_gpc, XDEV2LOG(x2), YDEV2LOG(y2) );
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
}

void wxGnomePrintDC::DoDrawBitmap( const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask )
{
}

void wxGnomePrintDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y )
{
    if (m_textForegroundColour.Ok())
    {
        unsigned char red = m_textForegroundColour.Red();
        unsigned char blue = m_textForegroundColour.Blue();
        unsigned char green = m_textForegroundColour.Green();

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

    x = XLOG2DEV(x);
    y = YLOG2DEV(y);
    
    wxPrintf( wxT("x,y: %d,%d\n"), x, y );

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

	gnome_print_moveto (m_gpc, x, y);
	gnome_print_pango_layout( m_gpc, m_layout );

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

    int oldStyle = m_pen.GetStyle();

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

void wxGnomePrintDC::DoGetTextExtent(const wxString& string, wxCoord *x, wxCoord *y,
                     wxCoord *descent,
                     wxCoord *externalLeading,
                     wxFont *theFont ) const
{
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
        
    wxPrintf( wxT("size %d,%d\n"), *width, *height );
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
