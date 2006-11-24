/////////////////////////////////////////////////////////////////////////////
// Name:        src/gtk/gnome/gprint.cpp
// Author:      Robert Roebling
// Purpose:     Implement GNOME printing support
// Created:     09/20/04
// RCS-ID:      $Id$
// Copyright:   Robert Roebling
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/gtk/gnome/gprint.h"

#if wxUSE_LIBGNOMEPRINT

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/dcmemory.h"
    #include "wx/icon.h"
    #include "wx/math.h"
    #include "wx/image.h"
    #include "wx/module.h"
#endif

#include "wx/fontutil.h"
#include "wx/gtk/private.h"
#include "wx/dynlib.h"
#include "wx/paper.h"

#include <libgnomeprint/gnome-print.h>
#include <libgnomeprint/gnome-print-pango.h>
#include <libgnomeprint/gnome-print-config.h>
#include <libgnomeprintui/gnome-print-dialog.h>
#include <libgnomeprintui/gnome-print-job-preview.h>
#include <libgnomeprintui/gnome-print-paper-selector.h>

static const double RAD2DEG  = 180.0 / M_PI;

#include "wx/html/forcelnk.h"
FORCE_LINK_ME(gnome_print)

//----------------------------------------------------------------------------
// wxGnomePrintLibrary
//----------------------------------------------------------------------------

#define wxDL_METHOD_DEFINE( rettype, name, args, shortargs, defret ) \
    typedef rettype (* name ## Type) args ; \
    name ## Type pfn_ ## name; \
    rettype name args \
    { if (m_ok) return pfn_ ## name shortargs ; return defret; }

#define wxDL_METHOD_LOAD( lib, name, success ) \
    pfn_ ## name = (name ## Type) lib->GetSymbol( wxT(#name), &success ); \
    if (!success) return;

class wxGnomePrintLibrary
{
public:
    wxGnomePrintLibrary();
    ~wxGnomePrintLibrary();

    bool IsOk();
    void InitializeMethods();

private:
    bool              m_ok;
    wxDynamicLibrary *m_gnome_print_lib;
    wxDynamicLibrary *m_gnome_printui_lib;

public:
    wxDL_METHOD_DEFINE( gint, gnome_print_newpath,
        (GnomePrintContext *pc), (pc), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_moveto,
        (GnomePrintContext *pc, gdouble x, gdouble y), (pc, x, y), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_lineto,
        (GnomePrintContext *pc, gdouble x, gdouble y), (pc, x, y), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_arcto,
        (GnomePrintContext *pc, gdouble x, gdouble y, gdouble radius, gdouble angle1, gdouble angle2, gint direction ), (pc, x, y, radius, angle1, angle2, direction), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_curveto,
        (GnomePrintContext *pc, gdouble x1, gdouble y1, gdouble x2, gdouble y2, gdouble x3, gdouble y3), (pc, x1, y1, x2, y2, x3, y3), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_closepath,
        (GnomePrintContext *pc), (pc), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_stroke,
        (GnomePrintContext *pc), (pc), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_fill,
        (GnomePrintContext *pc), (pc), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_setrgbcolor,
        (GnomePrintContext *pc, gdouble r, gdouble g, gdouble b), (pc, r, g, b), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_setlinewidth,
        (GnomePrintContext *pc, gdouble width), (pc, width), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_setdash,
        (GnomePrintContext *pc, gint n_values, const gdouble *values, gdouble offset), (pc, n_values, values, offset), 0 )

    wxDL_METHOD_DEFINE( gint, gnome_print_rgbimage,
        (GnomePrintContext *pc, const guchar *data, gint width, gint height, gint rowstride), (pc, data, width, height, rowstride ), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_rgbaimage,
        (GnomePrintContext *pc, const guchar *data, gint width, gint height, gint rowstride), (pc, data, width, height, rowstride ), 0 )

    wxDL_METHOD_DEFINE( gint, gnome_print_concat,
        (GnomePrintContext *pc, const gdouble *matrix), (pc, matrix), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_scale,
        (GnomePrintContext *pc, gdouble sx, gdouble sy), (pc, sx, sy), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_rotate,
        (GnomePrintContext *pc, gdouble theta), (pc, theta), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_translate,
        (GnomePrintContext *pc, gdouble x, gdouble y), (pc, x, y), 0 )

    wxDL_METHOD_DEFINE( gint, gnome_print_gsave,
        (GnomePrintContext *pc), (pc), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_grestore,
        (GnomePrintContext *pc), (pc), 0 )

    wxDL_METHOD_DEFINE( gint, gnome_print_beginpage,
        (GnomePrintContext *pc, const guchar* name), (pc, name), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_showpage,
        (GnomePrintContext *pc), (pc), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_end_doc,
        (GnomePrintContext *pc), (pc), 0 )

    wxDL_METHOD_DEFINE( PangoLayout*, gnome_print_pango_create_layout,
        (GnomePrintContext *gpc), (gpc), NULL )
    wxDL_METHOD_DEFINE( void, gnome_print_pango_layout,
        (GnomePrintContext *gpc, PangoLayout *layout), (gpc, layout), /**/ )

    wxDL_METHOD_DEFINE( GnomePrintJob*, gnome_print_job_new,
        (GnomePrintConfig *config), (config), NULL )
    wxDL_METHOD_DEFINE( GnomePrintContext*, gnome_print_job_get_context,
        (GnomePrintJob *job), (job), NULL )
    wxDL_METHOD_DEFINE( gint, gnome_print_job_close,
        (GnomePrintJob *job), (job), 0 )
    wxDL_METHOD_DEFINE( gint, gnome_print_job_print,
        (GnomePrintJob *job), (job), 0 )
    wxDL_METHOD_DEFINE( gboolean, gnome_print_job_get_page_size,
        (GnomePrintJob *job, gdouble *width, gdouble *height), (job, width, height), 0 )

    wxDL_METHOD_DEFINE( GnomePrintUnit*, gnome_print_unit_get_by_abbreviation,
        (const guchar *abbreviation), (abbreviation), NULL )
    wxDL_METHOD_DEFINE( gboolean, gnome_print_convert_distance,
        (gdouble *distance, const GnomePrintUnit *from, const GnomePrintUnit *to), (distance, from, to), false )

    wxDL_METHOD_DEFINE( GnomePrintConfig*, gnome_print_config_default,
        (void), (), NULL )
    wxDL_METHOD_DEFINE( gboolean, gnome_print_config_set,
        (GnomePrintConfig *config, const guchar *key, const guchar *value), (config, key, value), false )
    wxDL_METHOD_DEFINE( gboolean, gnome_print_config_get_length,
        (GnomePrintConfig *config, const guchar *key, gdouble *val, const GnomePrintUnit **unit), (config, key, val, unit), false )

    wxDL_METHOD_DEFINE( GtkWidget*, gnome_print_dialog_new,
        (GnomePrintJob *gpj, const guchar *title, gint flags), (gpj, title, flags), NULL )
    wxDL_METHOD_DEFINE( void, gnome_print_dialog_construct_range_page,
        (GnomePrintDialog *gpd, gint flags, gint start, gint end,
        const guchar *currentlabel, const guchar *rangelabel),
        (gpd, flags, start, end, currentlabel, rangelabel), /**/ )
    wxDL_METHOD_DEFINE( void, gnome_print_dialog_get_copies,
        (GnomePrintDialog *gpd, gint *copies, gboolean *collate), (gpd, copies, collate), /**/ )
    wxDL_METHOD_DEFINE( void, gnome_print_dialog_set_copies,
        (GnomePrintDialog *gpd, gint copies, gint collate), (gpd, copies, collate), /**/ )
    wxDL_METHOD_DEFINE( GnomePrintRangeType, gnome_print_dialog_get_range,
        (GnomePrintDialog *gpd), (gpd), GNOME_PRINT_RANGETYPE_NONE )
    wxDL_METHOD_DEFINE( int, gnome_print_dialog_get_range_page,
        (GnomePrintDialog *gpd, gint *start, gint *end), (gpd, start, end), 0 )

    wxDL_METHOD_DEFINE( GtkWidget*, gnome_paper_selector_new_with_flags,
        (GnomePrintConfig *config, gint flags), (config, flags), NULL )

    wxDL_METHOD_DEFINE( GtkWidget*, gnome_print_job_preview_new,
        (GnomePrintJob *gpm, const guchar *title), (gpm, title), NULL )
};

wxGnomePrintLibrary::wxGnomePrintLibrary()
{
    m_gnome_print_lib = NULL;
    m_gnome_printui_lib = NULL;

    wxLogNull log;

    m_gnome_print_lib = new wxDynamicLibrary( wxT("libgnomeprint-2-2.so.0") );
    m_ok = m_gnome_print_lib->IsLoaded();
    if (!m_ok) return;

    m_gnome_printui_lib = new wxDynamicLibrary( wxT("libgnomeprintui-2-2.so.0") );
    m_ok = m_gnome_printui_lib->IsLoaded();
    if (!m_ok) return;

    InitializeMethods();
}

wxGnomePrintLibrary::~wxGnomePrintLibrary()
{
    if (m_gnome_print_lib)
        delete m_gnome_print_lib;
    if (m_gnome_printui_lib)
        delete m_gnome_printui_lib;
}

bool wxGnomePrintLibrary::IsOk()
{
    return m_ok;
}

void wxGnomePrintLibrary::InitializeMethods()
{
    m_ok = false;
    bool success;

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_newpath, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_moveto, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_lineto, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_curveto, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_arcto, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_closepath, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_stroke, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_fill, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_setrgbcolor, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_setlinewidth, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_setdash, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_rgbimage, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_rgbaimage, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_concat, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_scale, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_rotate, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_translate, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_gsave, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_grestore, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_beginpage, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_showpage, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_end_doc, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_pango_create_layout, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_pango_layout, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_job_new, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_job_get_context, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_job_close, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_job_print, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_job_get_page_size, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_unit_get_by_abbreviation, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_convert_distance, success )

    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_config_default, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_config_set, success )
    wxDL_METHOD_LOAD( m_gnome_print_lib, gnome_print_config_get_length, success )

    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_print_dialog_new, success )
    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_print_dialog_construct_range_page, success )
    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_print_dialog_get_copies, success )
    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_print_dialog_set_copies, success )
    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_print_dialog_get_range, success )
    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_print_dialog_get_range_page, success )

    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_paper_selector_new_with_flags, success )

    wxDL_METHOD_LOAD( m_gnome_printui_lib, gnome_print_job_preview_new, success )

    m_ok = true;
}

static wxGnomePrintLibrary* gs_lgp = NULL;

//----------------------------------------------------------------------------
// wxGnomePrintNativeData
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrintNativeData, wxPrintNativeDataBase)

wxGnomePrintNativeData::wxGnomePrintNativeData()
{
    m_config = gs_lgp->gnome_print_config_default();
    m_job = gs_lgp->gnome_print_job_new( m_config );
}

wxGnomePrintNativeData::~wxGnomePrintNativeData()
{
    g_object_unref (m_config);
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
    return new wxGnomePrintPreview( preview, printout, data );
}

wxPrintPreviewBase *wxGnomePrintFactory::CreatePrintPreview( wxPrintout *preview,
                                                    wxPrintout *printout,
                                                    wxPrintData *data )
{
    return new wxGnomePrintPreview( preview, printout, data );
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

wxPageSetupDialogBase *wxGnomePrintFactory::CreatePageSetupDialog( wxWindow *parent,
                                                          wxPageSetupDialogData * data )
{
//  The native page setup dialog is broken. It
//  miscalculates newly entered values for the
//  margins if you have not chose "points" but
//  e.g. centimerters.
//  This has been fixed in GNOME CVS (maybe
//  fixed in libgnomeprintui 2.8.1)

    return new wxGnomePageSetupDialog( parent, data );
}

bool wxGnomePrintFactory::HasPrintSetupDialog()
{
    return false;
}

wxDialog *wxGnomePrintFactory::CreatePrintSetupDialog( wxWindow *parent, wxPrintData *data )
{
    return NULL;
}

wxDC* wxGnomePrintFactory::CreatePrinterDC( const wxPrintData& data )
{
    return new wxGnomePrintDC(data);
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

    m_widget = gs_lgp->gnome_print_dialog_new( native->GetPrintJob(),
                                       (guchar*)"Print",
                                       GNOME_PRINT_DIALOG_RANGE|GNOME_PRINT_DIALOG_COPIES );

    int flag = 0;
    if (m_printDialogData.GetEnableSelection())
        flag |= GNOME_PRINT_RANGE_SELECTION;
    if (m_printDialogData.GetEnablePageNumbers())
        flag |= GNOME_PRINT_RANGE_ALL|GNOME_PRINT_RANGE_RANGE;

    gs_lgp->gnome_print_dialog_construct_range_page( (GnomePrintDialog*) m_widget,
                                             flag,
                                             m_printDialogData.GetMinPage(),
                                             m_printDialogData.GetMaxPage(),
                                             NULL,
                                             NULL );
}

wxGnomePrintDialog::~wxGnomePrintDialog()
{
    m_widget = NULL;
}

int wxGnomePrintDialog::ShowModal()
{
    // Transfer data from m_printDalogData to dialog here

    int response = gtk_dialog_run (GTK_DIALOG (m_widget));

    if (response == GNOME_PRINT_DIALOG_RESPONSE_CANCEL)
    {
        gtk_widget_destroy(m_widget);
        m_widget = NULL;

        return wxID_CANCEL;
    }

    gint copies = 1;
    gboolean collate = false;
    gs_lgp->gnome_print_dialog_get_copies( (GnomePrintDialog*) m_widget, &copies, &collate );
    m_printDialogData.SetNoCopies( copies );
    m_printDialogData.SetCollate( collate );

    switch (gs_lgp->gnome_print_dialog_get_range( (GnomePrintDialog*) m_widget ))
    {
        case GNOME_PRINT_RANGE_SELECTION:
            m_printDialogData.SetSelection( true );
            break;
        case GNOME_PRINT_RANGE_ALL:
            m_printDialogData.SetAllPages( true );
            m_printDialogData.SetFromPage( 0 );
            m_printDialogData.SetToPage( 9999 );
            break;
        case GNOME_PRINT_RANGE_RANGE:
        default:
            gint start,end;
            gs_lgp->gnome_print_dialog_get_range_page( (GnomePrintDialog*) m_widget, &start, &end );
            m_printDialogData.SetFromPage( start );
            m_printDialogData.SetToPage( end );
            break;
    }

    gtk_widget_destroy(m_widget);
    m_widget = NULL;

    if (response == GNOME_PRINT_DIALOG_RESPONSE_PREVIEW)
        return wxID_PREVIEW;

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
// wxGnomePageSetupDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePageSetupDialog, wxPageSetupDialogBase)

wxGnomePageSetupDialog::wxGnomePageSetupDialog( wxWindow *parent,
                            wxPageSetupDialogData* data )
{
    if (data)
        m_pageDialogData = *data;

    wxGnomePrintNativeData *native =
      (wxGnomePrintNativeData*) m_pageDialogData.GetPrintData().GetNativeData();

    // This is required as the page setup dialog
    // calculates wrong values otherwise.
    gs_lgp->gnome_print_config_set( native->GetPrintConfig(),
                            (const guchar*) GNOME_PRINT_KEY_PREFERED_UNIT,
                            (const guchar*) "Pts" );

    m_widget = gtk_dialog_new();

    gtk_window_set_title( GTK_WINDOW(m_widget), wxGTK_CONV( _("Page setup") ) );

    GtkWidget *main = gs_lgp->gnome_paper_selector_new_with_flags( native->GetPrintConfig(),
        GNOME_PAPER_SELECTOR_MARGINS|GNOME_PAPER_SELECTOR_FEED_ORIENTATION );
    gtk_container_set_border_width (GTK_CONTAINER (main), 8);
    gtk_widget_show (main);

    gtk_container_add( GTK_CONTAINER (GTK_DIALOG (m_widget)->vbox), main );

    gtk_dialog_set_has_separator (GTK_DIALOG (m_widget), TRUE);

    gtk_dialog_add_buttons (GTK_DIALOG (m_widget),
                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                            GTK_STOCK_OK, GTK_RESPONSE_OK,
                            NULL);

    gtk_dialog_set_default_response (GTK_DIALOG (m_widget),
                            GTK_RESPONSE_OK);
}

wxGnomePageSetupDialog::~wxGnomePageSetupDialog()
{
}

wxPageSetupDialogData& wxGnomePageSetupDialog::GetPageSetupDialogData()
{
    return m_pageDialogData;
}

int wxGnomePageSetupDialog::ShowModal()
{
    wxGnomePrintNativeData *native =
      (wxGnomePrintNativeData*) m_pageDialogData.GetPrintData().GetNativeData();
    GnomePrintConfig *config = native->GetPrintConfig();

    // Transfer data from m_pageDialogData to native dialog

    int ret = gtk_dialog_run( GTK_DIALOG(m_widget) );

    if (ret == GTK_RESPONSE_OK)
    {
        // Transfer data back to m_pageDialogData

        // I don't know how querying the last parameter works
        // I cannot test it as the dialog is currently broken
        // anyways (it only works for points).
        double ml,mr,mt,mb,pw,ph;
        gs_lgp->gnome_print_config_get_length (config,
            (const guchar*) GNOME_PRINT_KEY_PAGE_MARGIN_LEFT, &ml, NULL);
        gs_lgp->gnome_print_config_get_length (config,
            (const guchar*) GNOME_PRINT_KEY_PAGE_MARGIN_RIGHT, &mr, NULL);
        gs_lgp->gnome_print_config_get_length (config,
            (const guchar*) GNOME_PRINT_KEY_PAGE_MARGIN_TOP, &mt, NULL);
        gs_lgp->gnome_print_config_get_length (config,
            (const guchar*) GNOME_PRINT_KEY_PAGE_MARGIN_BOTTOM, &mb, NULL);
        gs_lgp->gnome_print_config_get_length (config,
            (const guchar*) GNOME_PRINT_KEY_PAPER_WIDTH, &pw, NULL);
        gs_lgp->gnome_print_config_get_length (config,
            (const guchar*) GNOME_PRINT_KEY_PAPER_HEIGHT, &ph, NULL);

        // This probably assumes that the user entered the
        // values in Pts. Since that is the only the dialog
        // works right now, we need to fix this later.
        const GnomePrintUnit *mm_unit = gs_lgp->gnome_print_unit_get_by_abbreviation( (const guchar*) "mm" );
        const GnomePrintUnit *pts_unit = gs_lgp->gnome_print_unit_get_by_abbreviation( (const guchar*) "Pts" );
        gs_lgp->gnome_print_convert_distance( &ml, pts_unit, mm_unit );
        gs_lgp->gnome_print_convert_distance( &mr, pts_unit, mm_unit );
        gs_lgp->gnome_print_convert_distance( &mt, pts_unit, mm_unit );
        gs_lgp->gnome_print_convert_distance( &mb, pts_unit, mm_unit );
        gs_lgp->gnome_print_convert_distance( &pw, pts_unit, mm_unit );
        gs_lgp->gnome_print_convert_distance( &ph, pts_unit, mm_unit );

        m_pageDialogData.SetMarginTopLeft( wxPoint( (int)(ml+0.5), (int)(mt+0.5)) );
        m_pageDialogData.SetMarginBottomRight( wxPoint( (int)(mr+0.5), (int)(mb+0.5)) );

        m_pageDialogData.SetPaperSize( wxSize( (int)(pw+0.5), (int)(ph+0.5) ) );

#if 0
        wxPrintf( wxT("paper %d %d, top margin %d\n"),
            m_pageDialogData.GetPaperSize().x,
            m_pageDialogData.GetPaperSize().y,
            m_pageDialogData.GetMarginTopLeft().x );
#endif

        ret = wxID_OK;
    }
    else
    {
        ret = wxID_CANCEL;
    }

    gtk_widget_destroy( m_widget );
    m_widget = NULL;

    return ret;
}

bool wxGnomePageSetupDialog::Validate()
{
    return true;
}

bool wxGnomePageSetupDialog::TransferDataToWindow()
{
    return true;
}

bool wxGnomePageSetupDialog::TransferDataFromWindow()
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
    m_native_preview = false;
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
    wxGnomePrintNativeData *native =
        (wxGnomePrintNativeData*) printdata.GetNativeData();

    GnomePrintJob *job = gs_lgp->gnome_print_job_new( native->GetPrintConfig() );
    m_gpc = gs_lgp->gnome_print_job_get_context (job);

    // The GnomePrintJob is temporarily stored in the
    // native print data as the native print dialog
    // needs to access it.
    native->SetPrintJob( job );


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

    if (m_native_preview)
        printout->SetIsPreview(true);

    if (!dc)
    {
        gs_lgp->gnome_print_job_close( job );
        g_object_unref (job);
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    wxSize ScreenPixels = wxGetDisplaySize();
    wxSize ScreenMM = wxGetDisplaySizeMM();

    printout->SetPPIScreen( (int) ((ScreenPixels.GetWidth() * 25.4) / ScreenMM.GetWidth()),
                            (int) ((ScreenPixels.GetHeight() * 25.4) / ScreenMM.GetHeight()) );
    printout->SetPPIPrinter( wxGnomePrintDC::GetResolution(),
                             wxGnomePrintDC::GetResolution() );

    printout->SetDC(dc);

    int w, h;
    dc->GetSize(&w, &h);
    printout->SetPageSizePixels((int)w, (int)h);
    printout->SetPaperRectPixels(wxRect(0, 0, w, h));
    int mw, mh;
    dc->GetSizeMM(&mw, &mh);
    printout->SetPageSizeMM((int)mw, (int)mh);
    printout->OnPreparePrinting();

    // Get some parameters from the printout, if defined
    int fromPage, toPage;
    int minPage, maxPage;
    printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

    if (maxPage == 0)
    {
        gs_lgp->gnome_print_job_close( job );
        g_object_unref (job);
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    printout->OnBeginPrinting();

    int minPageNum = minPage, maxPageNum = maxPage;

    if ( !m_printDialogData.GetAllPages() )
    {
        minPageNum = m_printDialogData.GetFromPage();
        maxPageNum = m_printDialogData.GetToPage();
    }


    int copyCount;
    for ( copyCount = 1;
          copyCount <= m_printDialogData.GetNoCopies();
          copyCount++ )
    {
        if (!printout->OnBeginDocument(minPageNum, maxPageNum))
        {
            wxLogError(_("Could not start printing."));
            sm_lastError = wxPRINTER_ERROR;
            break;
        }

        int pn;
        for ( pn = minPageNum;
              pn <= maxPageNum && printout->HasPage(pn);
              pn++ )
        {
            dc->StartPage();
            printout->OnPrintPage(pn);
            dc->EndPage();
        }

        printout->OnEndDocument();
        printout->OnEndPrinting();
    }

    gs_lgp->gnome_print_job_close( job );
    if (m_native_preview)
    {
        const wxCharBuffer title(wxGTK_CONV_SYS(_("Print preview")));
        GtkWidget *preview = gs_lgp->gnome_print_job_preview_new
                                     (
                                        job,
                                        (const guchar *)title.data()
                                     );
        gtk_widget_show(preview);
    }
    else
    {
        gs_lgp->gnome_print_job_print( job );
    }

    g_object_unref (job);
    delete dc;

    return (sm_lastError == wxPRINTER_NO_ERROR);
}

wxDC* wxGnomePrinter::PrintDialog( wxWindow *parent )
{
    wxGnomePrintDialog dialog( parent, &m_printDialogData );
    int ret = dialog.ShowModal();
    if (ret == wxID_CANCEL)
    {
        sm_lastError = wxPRINTER_CANCELLED;
        return NULL;
    }

    m_native_preview = ret == wxID_PREVIEW;

    m_printDialogData = dialog.GetPrintDialogData();
    return new wxGnomePrintDC( this );
}

bool wxGnomePrinter::Setup( wxWindow *parent )
{
    return false;
}

//-----------------------------------------------------------------------------
// wxGnomePrintDC
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrintDC, wxDC)

wxGnomePrintDC::wxGnomePrintDC( wxGnomePrinter *printer )
{
    m_printer = printer;

    m_gpc = printer->GetPrintContext();
    m_job = NULL; // only used and destroyed when created with wxPrintData

    m_layout = gs_lgp->gnome_print_pango_create_layout( m_gpc );
    m_fontdesc = pango_font_description_from_string( "Sans 12" );

    m_currentRed = 0;
    m_currentBlue = 0;
    m_currentGreen = 0;

    m_signX =  1;  // default x-axis left to right
    m_signY = -1;  // default y-axis bottom up -> top down
    
    GetSize( NULL, &m_deviceOffsetY );
}

wxGnomePrintDC::wxGnomePrintDC( const wxPrintData& data )
{
    m_printer = NULL;
    m_printData = data;

    wxGnomePrintNativeData *native =
        (wxGnomePrintNativeData*) m_printData.GetNativeData();

    GnomePrintJob *job = gs_lgp->gnome_print_job_new( native->GetPrintConfig() );
    m_gpc = gs_lgp->gnome_print_job_get_context (job);
    m_job = job; // only used and destroyed when created with wxPrintData

    m_layout = gs_lgp->gnome_print_pango_create_layout( m_gpc );
    m_fontdesc = pango_font_description_from_string( "Sans 12" );

    m_currentRed = 0;
    m_currentBlue = 0;
    m_currentGreen = 0;

    m_signX =  1;  // default x-axis left to right
    m_signY = -1;  // default y-axis bottom up -> top down    
    
    GetSize( NULL, &m_deviceOffsetY );
}

wxGnomePrintDC::~wxGnomePrintDC()
{
    if (m_job)
        g_object_unref (m_job);
}

bool wxGnomePrintDC::IsOk() const
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

    gs_lgp->gnome_print_moveto ( m_gpc, XLOG2DEV(x1), YLOG2DEV(y1) );
    gs_lgp->gnome_print_lineto ( m_gpc, XLOG2DEV(x2), YLOG2DEV(y2) );
    gs_lgp->gnome_print_stroke ( m_gpc);

    CalcBoundingBox( x1, y1 );
    CalcBoundingBox( x2, y2 );
}

void wxGnomePrintDC::DoCrossHair(wxCoord x, wxCoord y)
{
}

void wxGnomePrintDC::DoDrawArc(wxCoord x1,wxCoord y1,wxCoord x2,wxCoord y2,wxCoord xc,wxCoord yc)
{
    double dx = x1 - xc;
    double dy = y1 - yc;
    double radius = sqrt((double)(dx*dx+dy*dy));
    double alpha1, alpha2;
    if (x1 == x2 && y1 == y2)
    {
        alpha1 = 0.0;
        alpha2 = 360.0;
    }
    else
    if (radius == 0.0)
    {
        alpha1 = alpha2 = 0.0;
    }
    else
    {
        alpha1 = (x1 - xc == 0) ?
            (y1 - yc < 0) ? 90.0 : -90.0 :
            -atan2(double(y1-yc), double(x1-xc)) * RAD2DEG;
        alpha2 = (x2 - xc == 0) ?
            (y2 - yc < 0) ? 90.0 : -90.0 :
            -atan2(double(y2-yc), double(x2-xc)) * RAD2DEG;

        while (alpha1 <= 0)   alpha1 += 360;
        while (alpha2 <= 0)   alpha2 += 360; // adjust angles to be between
        while (alpha1 > 360)  alpha1 -= 360; // 0 and 360 degree
        while (alpha2 > 360)  alpha2 -= 360;
    }

        if (m_brush.GetStyle() != wxTRANSPARENT)
        {
            SetBrush( m_brush );
            gs_lgp->gnome_print_moveto ( m_gpc, XLOG2DEV(xc), YLOG2DEV(yc) );
            gs_lgp->gnome_print_arcto( m_gpc, XLOG2DEV(xc), YLOG2DEV(yc), XLOG2DEVREL((int)radius), alpha1, alpha2, 0 );

            gs_lgp->gnome_print_fill( m_gpc );
        }

        if (m_pen.GetStyle() != wxTRANSPARENT)
        {
            SetPen (m_pen);
            gs_lgp->gnome_print_newpath( m_gpc );
            gs_lgp->gnome_print_moveto ( m_gpc, XLOG2DEV(xc), YLOG2DEV(yc) );
            gs_lgp->gnome_print_arcto( m_gpc, XLOG2DEV(xc), YLOG2DEV(yc), XLOG2DEVREL((int)radius), alpha1, alpha2, 0 );
            gs_lgp->gnome_print_closepath( m_gpc );

            gs_lgp->gnome_print_stroke( m_gpc );
        }

    CalcBoundingBox (x1, y1);
    CalcBoundingBox (x2, y2);
    CalcBoundingBox (xc, yc);
}

void wxGnomePrintDC::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
    x += w/2;
    y += h/2;

    int xx = XLOG2DEV(x);
    int yy = YLOG2DEV(y);

    gs_lgp->gnome_print_gsave( m_gpc );

    gs_lgp->gnome_print_translate( m_gpc, xx, yy );
    double scale = (double)YLOG2DEVREL(h) / (double) XLOG2DEVREL(w);
    gs_lgp->gnome_print_scale( m_gpc, 1.0, scale );

    xx = 0;
    yy = 0;

    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );

        gs_lgp->gnome_print_moveto ( m_gpc, xx, yy );
        gs_lgp->gnome_print_arcto( m_gpc, xx, yy,
            XLOG2DEVREL(w)/2, sa, ea, 0 );
        gs_lgp->gnome_print_moveto ( m_gpc, xx, yy );

        gs_lgp->gnome_print_fill( m_gpc );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);

        gs_lgp->gnome_print_arcto( m_gpc, xx, yy,
            XLOG2DEVREL(w)/2, sa, ea, 0 );

        gs_lgp->gnome_print_stroke( m_gpc );
    }

    gs_lgp->gnome_print_grestore( m_gpc );

    CalcBoundingBox( x, y );
    CalcBoundingBox( x+w, y+h );
}

void wxGnomePrintDC::DoDrawPoint(wxCoord x, wxCoord y)
{
}

void wxGnomePrintDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    if (n <= 0) return;

    SetPen (m_pen);

    int i;
    for ( i =0; i<n ; i++ )
        CalcBoundingBox( points[i].x+xoffset, points[i].y+yoffset);

    gs_lgp->gnome_print_moveto ( m_gpc, XLOG2DEV(points[0].x+xoffset), YLOG2DEV(points[0].y+yoffset) );

    for (i = 1; i < n; i++)
        gs_lgp->gnome_print_lineto ( m_gpc, XLOG2DEV(points[i].x+xoffset), YLOG2DEV(points[i].y+yoffset) );

    gs_lgp->gnome_print_stroke ( m_gpc);
}

void wxGnomePrintDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    if (n==0) return;

    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );

        int x = points[0].x + xoffset;
        int y = points[0].y + yoffset;
        CalcBoundingBox( x, y );
        gs_lgp->gnome_print_newpath( m_gpc );
        gs_lgp->gnome_print_moveto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
        int i;
        for (i = 1; i < n; i++)
        {
            int x = points[i].x + xoffset;
            int y = points[i].y + yoffset;
            gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
            CalcBoundingBox( x, y );
        }
        gs_lgp->gnome_print_closepath( m_gpc );
        gs_lgp->gnome_print_fill( m_gpc );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);

        int x = points[0].x + xoffset;
        int y = points[0].y + yoffset;
        gs_lgp->gnome_print_newpath( m_gpc );
        gs_lgp->gnome_print_moveto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
        int i;
        for (i = 1; i < n; i++)
        {
            int x = points[i].x + xoffset;
            int y = points[i].y + yoffset;
            gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
            CalcBoundingBox( x, y );
        }
        gs_lgp->gnome_print_closepath( m_gpc );
        gs_lgp->gnome_print_stroke( m_gpc );
    }
}

void wxGnomePrintDC::DoDrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    wxDC::DoDrawPolyPolygon( n, count, points, xoffset, yoffset, fillStyle );
}

void wxGnomePrintDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );

        gs_lgp->gnome_print_newpath( m_gpc );
        gs_lgp->gnome_print_moveto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
        gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y) );
        gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y + height) );
        gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x), YLOG2DEV(y + height) );
        gs_lgp->gnome_print_closepath( m_gpc );
        gs_lgp->gnome_print_fill( m_gpc );

        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);

        gs_lgp->gnome_print_newpath( m_gpc );
        gs_lgp->gnome_print_moveto( m_gpc, XLOG2DEV(x), YLOG2DEV(y) );
        gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y) );
        gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x + width), YLOG2DEV(y + height) );
        gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV(x), YLOG2DEV(y + height) );
        gs_lgp->gnome_print_closepath( m_gpc );
        gs_lgp->gnome_print_stroke( m_gpc );

        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }
}

void wxGnomePrintDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
    wxCoord rad = (wxCoord) radius;

    if (m_brush.GetStyle() != wxTRANSPARENT)
    {
        SetBrush(m_brush);
        gs_lgp->gnome_print_newpath(m_gpc);
        gs_lgp->gnome_print_moveto(m_gpc,XLOG2DEV(x + rad),YLOG2DEV(y));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x + rad),YLOG2DEV(y),
                                    XLOG2DEV(x),YLOG2DEV(y),
                                    XLOG2DEV(x),YLOG2DEV(y + rad));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x),YLOG2DEV(y + height - rad));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x),YLOG2DEV(y + height - rad),
                                    XLOG2DEV(x),YLOG2DEV(y + height),
                                    XLOG2DEV(x + rad),YLOG2DEV(y + height));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x + width - rad),YLOG2DEV(y + height));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x + width - rad),YLOG2DEV(y + height),
                                    XLOG2DEV(x + width),YLOG2DEV(y + height),
                                    XLOG2DEV(x + width),YLOG2DEV(y + height - rad));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x + width),YLOG2DEV(y + rad));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x + width),YLOG2DEV(y + rad),
                                    XLOG2DEV(x + width),YLOG2DEV(y),
                                    XLOG2DEV(x + width - rad),YLOG2DEV(y));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x + rad),YLOG2DEV(y));
        gs_lgp->gnome_print_closepath(m_gpc);
        gs_lgp->gnome_print_fill(m_gpc);

        CalcBoundingBox(x,y);
        CalcBoundingBox(x+width,y+height);
    }

    if (m_pen.GetStyle() != wxTRANSPARENT)
    {
        SetPen(m_pen);
        gs_lgp->gnome_print_newpath(m_gpc);
        gs_lgp->gnome_print_moveto(m_gpc,XLOG2DEV(x + rad),YLOG2DEV(y));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x + rad),YLOG2DEV(y),
                                    XLOG2DEV(x),YLOG2DEV(y),
                                    XLOG2DEV(x),YLOG2DEV(y + rad));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x),YLOG2DEV(y + height - rad));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x),YLOG2DEV(y + height - rad),
                                    XLOG2DEV(x),YLOG2DEV(y + height),
                                    XLOG2DEV(x + rad),YLOG2DEV(y + height));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x + width - rad),YLOG2DEV(y + height));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x + width - rad),YLOG2DEV(y + height),
                                    XLOG2DEV(x + width),YLOG2DEV(y + height),
                                    XLOG2DEV(x + width),YLOG2DEV(y + height - rad));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x + width),YLOG2DEV(y + rad));
        gs_lgp->gnome_print_curveto(m_gpc,
                                    XLOG2DEV(x + width),YLOG2DEV(y + rad),
                                    XLOG2DEV(x + width),YLOG2DEV(y),
                                    XLOG2DEV(x + width - rad),YLOG2DEV(y));
        gs_lgp->gnome_print_lineto(m_gpc,XLOG2DEV(x + rad),YLOG2DEV(y));
        gs_lgp->gnome_print_closepath(m_gpc);
        gs_lgp->gnome_print_stroke(m_gpc);

        CalcBoundingBox(x,y);
        CalcBoundingBox(x+width,y+height);
    }
}

void wxGnomePrintDC::makeEllipticalPath(wxCoord x, wxCoord y,
                                        wxCoord width, wxCoord height)
{
    double r = 4 * (sqrt (2) - 1) / 3;
    double  halfW = 0.5 * width,
            halfH = 0.5 * height,
            halfWR = r * halfW,
            halfHR = r * halfH;
    wxCoord halfWI = (wxCoord) halfW,
            halfHI = (wxCoord) halfH;

    gs_lgp->gnome_print_newpath( m_gpc );

    // Approximate an ellipse using four cubic splines, clockwise from 0 deg */
    gs_lgp->gnome_print_moveto( m_gpc,
                XLOG2DEV(x + width),
                YLOG2DEV(y + halfHI) );
    gs_lgp->gnome_print_curveto( m_gpc,
                XLOG2DEV(x + width),
                YLOG2DEV(y + (wxCoord) rint (halfH + halfHR)),
                XLOG2DEV(x + (wxCoord) rint(halfW + halfWR)),
                YLOG2DEV(y + height),
                XLOG2DEV(x + halfWI),
                YLOG2DEV(y + height) );
    gs_lgp->gnome_print_curveto( m_gpc,
                XLOG2DEV(x + (wxCoord) rint(halfW - halfWR)),
                YLOG2DEV(y + height),
                XLOG2DEV(x),
                YLOG2DEV(y + (wxCoord) rint (halfH + halfHR)),
                XLOG2DEV(x), YLOG2DEV(y+halfHI) );
    gs_lgp->gnome_print_curveto( m_gpc,
                XLOG2DEV(x),
                YLOG2DEV(y + (wxCoord) rint (halfH - halfHR)),
                XLOG2DEV(x + (wxCoord) rint (halfW - halfWR)),
                YLOG2DEV(y),
                XLOG2DEV(x+halfWI), YLOG2DEV(y) );
    gs_lgp->gnome_print_curveto( m_gpc,
                XLOG2DEV(x + (wxCoord) rint(halfW + halfWR)),
                YLOG2DEV(y),
                XLOG2DEV(x + width),
                YLOG2DEV(y + (wxCoord) rint(halfH - halfHR)),
                XLOG2DEV(x + width), YLOG2DEV(y + halfHI) );

    gs_lgp->gnome_print_closepath(m_gpc);
}

void wxGnomePrintDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    if (m_brush.GetStyle () != wxTRANSPARENT)
    {
        SetBrush( m_brush );
        makeEllipticalPath( x, y, width, height );
        gs_lgp->gnome_print_fill( m_gpc );
        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }

    if (m_pen.GetStyle () != wxTRANSPARENT)
    {
        SetPen (m_pen);
        makeEllipticalPath( x, y, width, height );
        gs_lgp->gnome_print_stroke( m_gpc );
        CalcBoundingBox( x, y );
        CalcBoundingBox( x + width, y + height );
    }
}

#if wxUSE_SPLINES
void wxGnomePrintDC::DoDrawSpline(wxList *points)
{
    SetPen (m_pen);

    double c, d, x1, y1, x2, y2, x3, y3;
    wxPoint *p, *q;

    wxList::compatibility_iterator node = points->GetFirst();
    p = (wxPoint *)node->GetData();
    x1 = p->x;
    y1 = p->y;

    node = node->GetNext();
    p = (wxPoint *)node->GetData();
    c = p->x;
    d = p->y;
    x3 =
         (double)(x1 + c) / 2;
    y3 =
         (double)(y1 + d) / 2;

    gs_lgp->gnome_print_newpath( m_gpc );
    gs_lgp->gnome_print_moveto( m_gpc, XLOG2DEV((wxCoord)x1), YLOG2DEV((wxCoord)y1) );
    gs_lgp->gnome_print_lineto( m_gpc, XLOG2DEV((wxCoord)x3), YLOG2DEV((wxCoord)y3) );

    CalcBoundingBox( (wxCoord)x1, (wxCoord)y1 );
    CalcBoundingBox( (wxCoord)x3, (wxCoord)y3 );

    node = node->GetNext();
    while (node)
    {
        q = (wxPoint *)node->GetData();

        x1 = x3;
        y1 = y3;
        x2 = c;
        y2 = d;
        c = q->x;
        d = q->y;
        x3 = (double)(x2 + c) / 2;
        y3 = (double)(y2 + d) / 2;

        gs_lgp->gnome_print_curveto(m_gpc,
            XLOG2DEV((wxCoord)x1), YLOG2DEV((wxCoord)y1),
            XLOG2DEV((wxCoord)x2), YLOG2DEV((wxCoord)y2),
            XLOG2DEV((wxCoord)x3), YLOG2DEV((wxCoord)y3) );

        CalcBoundingBox( (wxCoord)x1, (wxCoord)y1 );
        CalcBoundingBox( (wxCoord)x3, (wxCoord)y3 );

        node = node->GetNext();
    }

    gs_lgp->gnome_print_lineto ( m_gpc, XLOG2DEV((wxCoord)c), YLOG2DEV((wxCoord)d) );

    gs_lgp->gnome_print_stroke( m_gpc );
}
#endif // wxUSE_SPLINES

bool wxGnomePrintDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
            wxDC *source, wxCoord xsrc, wxCoord ysrc, int rop, bool useMask,
            wxCoord xsrcMask, wxCoord ysrcMask)
{
    wxCHECK_MSG( source, false, wxT("invalid source dc") );

    // blit into a bitmap
    wxBitmap bitmap( width, height );
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, width, height, source, xsrc, ysrc, rop); /* TODO: Blit transparently? */
    memDC.SelectObject(wxNullBitmap);

    // draw bitmap. scaling and positioning is done there
    DrawBitmap( bitmap, xdest, ydest );

    return true;
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

        gs_lgp->gnome_print_gsave( m_gpc );
        double matrix[6];
        matrix[0] = XLOG2DEVREL(width);
        matrix[1] = 0;
        matrix[2] = 0;
        matrix[3] = YLOG2DEVREL(height);
        matrix[4] = XLOG2DEV(x);
        matrix[5] = YLOG2DEV(y+height);
        gs_lgp->gnome_print_concat( m_gpc, matrix );
        gs_lgp->gnome_print_moveto(  m_gpc, 0, 0 );
        if (has_alpha)
            gs_lgp->gnome_print_rgbaimage( m_gpc, (guchar *)raw_image, width, height, rowstride );
        else
            gs_lgp->gnome_print_rgbimage( m_gpc, (guchar *)raw_image, width, height, rowstride );
        gs_lgp->gnome_print_grestore( m_gpc );
    }
    else
    {
        wxImage image = bitmap.ConvertToImage();

        if (!image.Ok()) return;

        gs_lgp->gnome_print_gsave( m_gpc );
        double matrix[6];
        matrix[0] = XLOG2DEVREL(image.GetWidth());
        matrix[1] = 0;
        matrix[2] = 0;
        matrix[3] = YLOG2DEVREL(image.GetHeight());
        matrix[4] = XLOG2DEV(x);
        matrix[5] = YLOG2DEV(y+image.GetHeight());
        gs_lgp->gnome_print_concat( m_gpc, matrix );
        gs_lgp->gnome_print_moveto(  m_gpc, 0, 0 );
        gs_lgp->gnome_print_rgbimage( m_gpc, (guchar*) image.GetData(), image.GetWidth(), image.GetHeight(), image.GetWidth()*3 );
        gs_lgp->gnome_print_grestore( m_gpc );
    }
}

void wxGnomePrintDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y )
{
    DoDrawRotatedText( text, x, y, 0.0 );
}

void wxGnomePrintDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
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

        if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
        {
            double redPS = (double)(red) / 255.0;
            double bluePS = (double)(blue) / 255.0;
            double greenPS = (double)(green) / 255.0;

            gs_lgp->gnome_print_setrgbcolor( m_gpc, redPS, greenPS, bluePS );

            m_currentRed = red;
            m_currentBlue = blue;
            m_currentGreen = green;
        }
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
        gs_lgp->gnome_print_moveto (m_gpc, x, y);
        if (fabs(angle) > 0.00001)
        {
            gs_lgp->gnome_print_gsave( m_gpc );
            gs_lgp->gnome_print_rotate( m_gpc, angle );
            gs_lgp->gnome_print_pango_layout( m_gpc, m_layout );
            gs_lgp->gnome_print_grestore( m_gpc );
        }
        else
        {
            gs_lgp->gnome_print_pango_layout( m_gpc, m_layout );
        }

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
        gs_lgp->gnome_print_moveto (m_gpc, x, y);
        if (fabs(angle) > 0.00001)
        {
            gs_lgp->gnome_print_gsave( m_gpc );
            gs_lgp->gnome_print_rotate( m_gpc, angle );
            gs_lgp->gnome_print_pango_layout( m_gpc, m_layout );
            gs_lgp->gnome_print_grestore( m_gpc );
        }
        else
        {
            gs_lgp->gnome_print_pango_layout( m_gpc, m_layout );
        }
    }

    if (underlined)
    {
        // undo underline attributes setting:
        pango_layout_set_attributes(m_layout, NULL);
    }

    CalcBoundingBox (x + w, y + h);
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

    m_pen = pen;

    gs_lgp->gnome_print_setlinewidth( m_gpc, XLOG2DEVREL( 1000 * m_pen.GetWidth() ) / 1000.0f );

    static const double dotted[] =  {2.0, 5.0};
    static const double short_dashed[] = {4.0, 4.0};
    static const double wxCoord_dashed[] = {4.0, 8.0};
    static const double dotted_dashed[] = {6.0, 6.0, 2.0, 6.0};

    switch (m_pen.GetStyle())
    {
        case wxDOT:           gs_lgp->gnome_print_setdash( m_gpc, 2, dotted, 0 ); break;
        case wxSHORT_DASH:    gs_lgp->gnome_print_setdash( m_gpc, 2, short_dashed, 0 ); break;
        case wxLONG_DASH:     gs_lgp->gnome_print_setdash( m_gpc, 2, wxCoord_dashed, 0 ); break;
        case wxDOT_DASH:      gs_lgp->gnome_print_setdash( m_gpc, 4, dotted_dashed, 0 );  break;
        case wxUSER_DASH:
        {
            // It may be noted that libgnomeprint between at least
            // versions 2.8.0 and 2.12.1 makes a copy of the dashes
            // and then leak the memory since it doesn't set the
            // internal flag "privatedash" to 0.
            wxDash *wx_dashes;
            int num = m_pen.GetDashes (&wx_dashes);
            gdouble *g_dashes = g_new( gdouble, num );
            int i;
            for (i = 0; i < num; ++i)
                g_dashes[i] = (gdouble) wx_dashes[i];
            gs_lgp -> gnome_print_setdash( m_gpc, num, g_dashes, 0);
            g_free( g_dashes );
        }
        break;
        case wxSOLID:
        case wxTRANSPARENT:
        default:              gs_lgp->gnome_print_setdash( m_gpc, 0, NULL, 0 );   break;
    }


    unsigned char red = m_pen.GetColour().Red();
    unsigned char blue = m_pen.GetColour().Blue();
    unsigned char green = m_pen.GetColour().Green();

    if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue))
    {
        double redPS = (double)(red) / 255.0;
        double bluePS = (double)(blue) / 255.0;
        double greenPS = (double)(green) / 255.0;

        gs_lgp->gnome_print_setrgbcolor( m_gpc, redPS, greenPS, bluePS );

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

        gs_lgp->gnome_print_setrgbcolor( m_gpc, redPS, greenPS, bluePS );

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
    return true;
}

void wxGnomePrintDC::EndDoc()
{
    gs_lgp->gnome_print_end_doc( m_gpc );
}

void wxGnomePrintDC::StartPage()
{
    gs_lgp->gnome_print_beginpage( m_gpc, (const guchar*) "page" );
}

void wxGnomePrintDC::EndPage()
{
    gs_lgp->gnome_print_showpage( m_gpc );
}

wxCoord wxGnomePrintDC::GetCharHeight() const
{
    pango_layout_set_text( m_layout, "H", 1 );

    int w,h;
    pango_layout_get_pixel_size( m_layout, &w, &h );

    return h;
}

wxCoord wxGnomePrintDC::GetCharWidth() const
{
    pango_layout_set_text( m_layout, "H", 1 );

    int w,h;
    pango_layout_get_pixel_size( m_layout, &w, &h );

    return w;
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

    if (string.empty())
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
        *width = (wxCoord)(w / m_scaleX);
    if (height)
        *height = (wxCoord)(h / m_scaleY);
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
    wxGnomePrintNativeData *native =
      (wxGnomePrintNativeData*) m_printData.GetNativeData();

    // Query page size. This seems to omit the margins
    double pw,ph;
    gs_lgp->gnome_print_job_get_page_size( native->GetPrintJob(), &pw, &ph );

    if (width)
        *width = (int) (pw + 0.5);
    if (height)
        *height = (int) (ph + 0.5);
}

void wxGnomePrintDC::DoGetSizeMM(int *width, int *height) const
{
    wxGnomePrintNativeData *native =
      (wxGnomePrintNativeData*) m_printData.GetNativeData();

    // This code assumes values in Pts.

    double pw,ph;
    gs_lgp->gnome_print_job_get_page_size( native->GetPrintJob(), &pw, &ph );

    // Convert to mm.

    const GnomePrintUnit *mm_unit = gs_lgp->gnome_print_unit_get_by_abbreviation( (const guchar*) "mm" );
    const GnomePrintUnit *pts_unit = gs_lgp->gnome_print_unit_get_by_abbreviation( (const guchar*) "Pts" );
    gs_lgp->gnome_print_convert_distance( &pw, pts_unit, mm_unit );
    gs_lgp->gnome_print_convert_distance( &ph, pts_unit, mm_unit );

    if (width)
        *width = (int) (pw + 0.5);
    if (height)
        *height = (int) (ph + 0.5);
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

void wxGnomePrintDC::SetLogicalOrigin( wxCoord x, wxCoord y )
{
    wxDC::SetLogicalOrigin( x, y );
}

void wxGnomePrintDC::SetDeviceOrigin( wxCoord x, wxCoord y )
{
    wxDC::SetDeviceOrigin( x, y );
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
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxGnomePrintModule)
};

bool wxGnomePrintModule::OnInit()
{
    gs_lgp = new wxGnomePrintLibrary;
    if (gs_lgp->IsOk())
        wxPrintFactory::SetPrintFactory( new wxGnomePrintFactory );
    return true;
}

void wxGnomePrintModule::OnExit()
{
    delete gs_lgp;
}

IMPLEMENT_DYNAMIC_CLASS(wxGnomePrintModule, wxModule)

// ----------------------------------------------------------------------------
// Print preview
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGnomePrintPreview, wxPrintPreviewBase)

void wxGnomePrintPreview::Init(wxPrintout * WXUNUSED(printout),
                                    wxPrintout * WXUNUSED(printoutForPrinting))
{
    DetermineScaling();
}

wxGnomePrintPreview::wxGnomePrintPreview(wxPrintout *printout,
                                                   wxPrintout *printoutForPrinting,
                                                   wxPrintDialogData *data)
                        : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    Init(printout, printoutForPrinting);
}

wxGnomePrintPreview::wxGnomePrintPreview(wxPrintout *printout,
                                                   wxPrintout *printoutForPrinting,
                                                   wxPrintData *data)
                        : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    Init(printout, printoutForPrinting);
}

wxGnomePrintPreview::~wxGnomePrintPreview()
{
}

bool wxGnomePrintPreview::Print(bool interactive)
{
    if (!m_printPrintout)
        return false;

    wxPrinter printer(& m_printDialogData);
    return printer.Print(m_previewFrame, m_printPrintout, interactive);
}

void wxGnomePrintPreview::DetermineScaling()
{
    wxPaperSize paperType = m_printDialogData.GetPrintData().GetPaperId();
    if (paperType == wxPAPER_NONE)
        paperType = wxPAPER_NONE;

    wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
    if (!paper)
        paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

    if (paper)
    {
        wxSize ScreenPixels = wxGetDisplaySize();
        wxSize ScreenMM = wxGetDisplaySizeMM();

        m_previewPrintout->SetPPIScreen( (int) ((ScreenPixels.GetWidth() * 25.4) / ScreenMM.GetWidth()),
                                         (int) ((ScreenPixels.GetHeight() * 25.4) / ScreenMM.GetHeight()) );
        m_previewPrintout->SetPPIPrinter(wxGnomePrintDC::GetResolution(), wxGnomePrintDC::GetResolution());

        wxSize sizeDevUnits(paper->GetSizeDeviceUnits());
        
        // TODO: get better resolution information from wxGnomePrintDC, if possible.

        sizeDevUnits.x = (wxCoord)((float)sizeDevUnits.x * wxGnomePrintDC::GetResolution() / 72.0);
        sizeDevUnits.y = (wxCoord)((float)sizeDevUnits.y * wxGnomePrintDC::GetResolution() / 72.0);
        wxSize sizeTenthsMM(paper->GetSize());
        wxSize sizeMM(sizeTenthsMM.x / 10, sizeTenthsMM.y / 10);

        // If in landscape mode, we need to swap the width and height.
        if ( m_printDialogData.GetPrintData().GetOrientation() == wxLANDSCAPE )
        {
            m_pageWidth = sizeDevUnits.y;
            m_pageHeight = sizeDevUnits.x;
            m_previewPrintout->SetPageSizeMM(sizeMM.y, sizeMM.x);
        }
        else
        {
            m_pageWidth = sizeDevUnits.x;
            m_pageHeight = sizeDevUnits.y;
            m_previewPrintout->SetPageSizeMM(sizeMM.x, sizeMM.y);
        }
        m_previewPrintout->SetPageSizePixels(m_pageWidth, m_pageHeight);
        m_previewPrintout->SetPaperRectPixels(wxRect(0, 0, m_pageWidth, m_pageHeight));

        // At 100%, the page should look about page-size on the screen.
        m_previewScaleX = (float)0.8 * 72.0 / (float)wxGnomePrintDC::GetResolution();
        m_previewScaleY = m_previewScaleX;
    }
}

#endif
    // wxUSE_LIBGNOMEPRINT
