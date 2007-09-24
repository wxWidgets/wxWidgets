/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/print.cpp
// Author:      Anthony Bretaudeau
// Purpose:     GTK printing support
// Created:     2007-08-25
// RCS-ID:      $Id: print.cpp,v 1 2007-08-25 05:44:44 PC Exp $
// Copyright:   (c) 2007 wxWidgets development team
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_GTKPRINT

#include "wx/gtk/print.h"

#ifndef WX_PRECOMP
#include "wx/log.h"
#include "wx/dcmemory.h"
#include "wx/icon.h"
#include "wx/math.h"
#include "wx/image.h"
#include "wx/module.h"
#include "wx/crt.h"
#endif

#include "wx/fontutil.h"
#include "wx/gtk/private.h"
#include "wx/dynlib.h"
#include "wx/paper.h"
#include "wx/rawbmp.h"

#include <gtk/gtk.h>
#include <gtk/gtkpagesetupunixdialog.h>

#include "wx/link.h"
wxFORCE_LINK_THIS_MODULE(gtk_print)

#if wxUSE_LIBGNOMEPRINT
#include "wx/gtk/gnome/gprint.h"
#endif

// Usefull to convert angles from/to Rad to/from Deg.
static const double RAD2DEG  = 180.0 / M_PI;
static const double DEG2RAD  = M_PI / 180.0;

static wxCairoLibrary* gs_cairo = NULL;

//----------------------------------------------------------------------------
// wxGtkPrintModule
// Initialized when starting the app : if it successfully load the gtk-print framework,
// it uses it. If not, it falls back to gnome print (see /gtk/gnome/gprint.cpp) then
// to postscript if gnomeprint is not available.
//----------------------------------------------------------------------------

class wxGtkPrintModule: public wxModule
{
public:
    wxGtkPrintModule()
    {
#if wxUSE_LIBGNOMEPRINT
        // This module must be initialized AFTER gnomeprint's one
        AddDependency(CLASSINFO(wxGnomePrintModule));
#endif
    }
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxGtkPrintModule)
};

bool wxGtkPrintModule::OnInit()
{
    gs_cairo = wxCairoLibrary::Get();
    if (gs_cairo && gtk_check_version(2,10,0) == NULL)
        wxPrintFactory::SetPrintFactory( new wxGtkPrintFactory );
    return true;
}

void wxGtkPrintModule::OnExit()
{
    gs_cairo = NULL;
}

IMPLEMENT_DYNAMIC_CLASS(wxGtkPrintModule, wxModule)

//----------------------------------------------------------------------------
// wxGtkPrintFactory
//----------------------------------------------------------------------------

wxPrinterBase* wxGtkPrintFactory::CreatePrinter( wxPrintDialogData *data )
{
    return new wxGtkPrinter( data );
}

wxPrintPreviewBase *wxGtkPrintFactory::CreatePrintPreview( wxPrintout *preview,
                                                    wxPrintout *printout,
                                                    wxPrintDialogData *data )
{
    return new wxGtkPrintPreview( preview, printout, data );
}

wxPrintPreviewBase *wxGtkPrintFactory::CreatePrintPreview( wxPrintout *preview,
                                                    wxPrintout *printout,
                                                    wxPrintData *data )
{
    return new wxGtkPrintPreview( preview, printout, data );
}

wxPrintDialogBase *wxGtkPrintFactory::CreatePrintDialog( wxWindow *parent,
                                                  wxPrintDialogData *data )
{
    return new wxGtkPrintDialog( parent, data );
}

wxPrintDialogBase *wxGtkPrintFactory::CreatePrintDialog( wxWindow *parent,
                                                  wxPrintData *data )
{
    return new wxGtkPrintDialog( parent, data );
}

wxPageSetupDialogBase *wxGtkPrintFactory::CreatePageSetupDialog( wxWindow *parent,
                                                          wxPageSetupDialogData * data )
{
    return new wxGtkPageSetupDialog( parent, data );
}

bool wxGtkPrintFactory::HasPrintSetupDialog()
{
    return false;
}

wxDialog *wxGtkPrintFactory::CreatePrintSetupDialog( wxWindow *parent, wxPrintData *data )
{
    return NULL;
}

wxDC* wxGtkPrintFactory::CreatePrinterDC( const wxPrintData& data )
{
    return new wxGtkPrintDC(data);
}

bool wxGtkPrintFactory::HasOwnPrintToFile()
{
    return true;
}

bool wxGtkPrintFactory::HasPrinterLine()
{
    return true;
}

wxString wxGtkPrintFactory::CreatePrinterLine()
{
    // redundant now
    return wxEmptyString;
}

bool wxGtkPrintFactory::HasStatusLine()
{
    // redundant now
    return true;
}

wxString wxGtkPrintFactory::CreateStatusLine()
{
    // redundant now
    return wxEmptyString;
}

wxPrintNativeDataBase *wxGtkPrintFactory::CreatePrintNativeData()
{
    return new wxGtkPrintNativeData;
}

//----------------------------------------------------------------------------
// Callback functions for Gtk Printings.
//----------------------------------------------------------------------------

// We use it to pass useful objets to gtk printing callback functions.
struct wxPrinterToGtkData
{
   wxGtkPrinter * printer;
   wxPrintout * printout;
};

extern "C"
{
    static void gtk_begin_print_callback (GtkPrintOperation *operation, GtkPrintContext *context, gpointer user_data)
    {
        wxPrinterToGtkData *data = (wxPrinterToGtkData *) user_data;

        data->printer->BeginPrint(data->printout, operation, context);
    }

    static void gtk_draw_page_print_callback (GtkPrintOperation *operation, GtkPrintContext *context, gint page_nr, gpointer user_data)
    {
        wxPrinterToGtkData *data = (wxPrinterToGtkData *) user_data;

        data->printer->DrawPage(data->printout, operation, context, page_nr);
    }

    static void gtk_end_print_callback (GtkPrintOperation *operation, GtkPrintContext *context, gpointer user_data)
    {
        wxPrintout *printout = (wxPrintout *) user_data;

        printout->OnEndPrinting();
    }

    static gboolean gtk_preview_print_callback (GtkPrintOperation *operation, GtkPrintOperationPreview *preview, GtkPrintContext *context, GtkWindow *parent, gpointer user_data)
    {
        wxPrintout *printout = (wxPrintout *) user_data;

        printout->SetIsPreview(true);

        /* We create a cairo context with 72dpi resolution. This resolution is only used for positionning. */
        cairo_t *cairo = gdk_cairo_create(GTK_WIDGET(parent)->window);
        gtk_print_context_set_cairo_context(context, cairo, 72, 72);

        return false;
    }
}

//----------------------------------------------------------------------------
// wxGtkPrintNativeData
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGtkPrintNativeData, wxPrintNativeDataBase)

wxGtkPrintNativeData::wxGtkPrintNativeData()
{
    m_config = gtk_print_settings_new();
}

wxGtkPrintNativeData::~wxGtkPrintNativeData()
{
    g_object_unref (m_config);
}

// Convert datas stored in m_config to a wxPrintData.
// Called by wxPrintData::ConvertFromNative().
bool wxGtkPrintNativeData::TransferTo( wxPrintData &data )
{
    if(!m_config)
        return false;

    GtkPrintQuality quality = gtk_print_settings_get_quality(m_config);
    if (quality == GTK_PRINT_QUALITY_HIGH)
        data.SetQuality(wxPRINT_QUALITY_HIGH);
    else if (quality == GTK_PRINT_QUALITY_LOW)
        data.SetQuality(wxPRINT_QUALITY_LOW);
    else if (quality == GTK_PRINT_QUALITY_DRAFT)
        data.SetQuality(wxPRINT_QUALITY_DRAFT);
    else
        data.SetQuality(wxPRINT_QUALITY_MEDIUM);

    data.SetNoCopies(gtk_print_settings_get_n_copies(m_config));

    data.SetColour(gtk_print_settings_get_use_color(m_config));

    switch (gtk_print_settings_get_duplex(m_config))
    {
        case GTK_PRINT_DUPLEX_SIMPLEX:      data.SetDuplex (wxDUPLEX_SIMPLEX);
                                            break;

        case GTK_PRINT_DUPLEX_HORIZONTAL:   data.SetDuplex (wxDUPLEX_HORIZONTAL);
                                            break;

        default:
        case GTK_PRINT_DUPLEX_VERTICAL:      data.SetDuplex (wxDUPLEX_VERTICAL);
                                            break;
    }

    GtkPageOrientation orientation = gtk_print_settings_get_orientation (m_config);
    if (orientation == GTK_PAGE_ORIENTATION_PORTRAIT)
    {
        data.SetOrientation(wxPORTRAIT);
        data.SetOrientationReversed(false);
    }
    else if (orientation == GTK_PAGE_ORIENTATION_LANDSCAPE)
    {
        data.SetOrientation(wxLANDSCAPE);
        data.SetOrientationReversed(false);
    }
    else if (orientation == GTK_PAGE_ORIENTATION_REVERSE_PORTRAIT)
    {
        data.SetOrientation(wxPORTRAIT);
        data.SetOrientationReversed(true);
    }
    else if (orientation == GTK_PAGE_ORIENTATION_REVERSE_LANDSCAPE)
    {
        data.SetOrientation(wxLANDSCAPE);
        data.SetOrientationReversed(true);
    }

    data.SetCollate(gtk_print_settings_get_collate (m_config));

    // Paper formats : these are the most common paper formats.
    GtkPaperSize *paper_size = gtk_print_settings_get_paper_size (m_config);
    if (!paper_size)
        data.SetPaperId(wxPAPER_NONE);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new (GTK_PAPER_NAME_A3)))
        data.SetPaperId(wxPAPER_A3);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new (GTK_PAPER_NAME_A4)))
        data.SetPaperId(wxPAPER_A4);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new (GTK_PAPER_NAME_A5)))
        data.SetPaperId(wxPAPER_A5);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new (GTK_PAPER_NAME_B5)))
        data.SetPaperId(wxPAPER_B5);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new (GTK_PAPER_NAME_LETTER)))
        data.SetPaperId(wxPAPER_LETTER);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new (GTK_PAPER_NAME_LEGAL)))
        data.SetPaperId(wxPAPER_LEGAL);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new (GTK_PAPER_NAME_EXECUTIVE)))
        data.SetPaperId(wxPAPER_EXECUTIVE);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"na_number-10")))
        data.SetPaperId(wxPAPER_ENV_10);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-c5")))
        data.SetPaperId(wxPAPER_ENV_C5);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-c6")))
        data.SetPaperId(wxPAPER_ENV_C6);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"jis-b5")))
        data.SetPaperId(wxPAPER_B5_TRANSVERSE);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-b5")))
        data.SetPaperId(wxPAPER_ENV_B5);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"na_monarch")))
        data.SetPaperId(wxPAPER_ENV_MONARCH);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"engineering-c")))
        data.SetPaperId( wxPAPER_CSHEET);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"engineering-d")))
        data.SetPaperId( wxPAPER_DSHEET);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"engineering-e")))
        data.SetPaperId( wxPAPER_ESHEET);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"letter")))
        data.SetPaperId( wxPAPER_LETTERSMALL);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"engineering-b")))
        data.SetPaperId( wxPAPER_TABLOID);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"ledger")))
        data.SetPaperId( wxPAPER_LEDGER);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"statement")))
        data.SetPaperId( wxPAPER_STATEMENT);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( GTK_PAPER_NAME_A4 )))
        data.SetPaperId( wxPAPER_A4SMALL);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-b4")))
        data.SetPaperId( wxPAPER_B4);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"folio")))
        data.SetPaperId( wxPAPER_FOLIO);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"quarto")))
        data.SetPaperId( wxPAPER_QUARTO);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"10x14")))
        data.SetPaperId( wxPAPER_10X14);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"ledger")))
        data.SetPaperId( wxPAPER_11X17);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"letter")))
        data.SetPaperId( wxPAPER_NOTE);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"na-number-9-envelope")))
        data.SetPaperId( wxPAPER_ENV_9);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"number-11")))
        data.SetPaperId( wxPAPER_ENV_11);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"number-12")))
        data.SetPaperId( wxPAPER_ENV_12);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"number-14")))
        data.SetPaperId( wxPAPER_ENV_14);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-designated")))
        data.SetPaperId( wxPAPER_ENV_DL);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-c3")))
        data.SetPaperId( wxPAPER_ENV_C3);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-c4")))
        data.SetPaperId( wxPAPER_ENV_C4);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"c6/c5")))
        data.SetPaperId( wxPAPER_ENV_C65);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-b4")))
        data.SetPaperId( wxPAPER_ENV_B4);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"iso-b6")))
            data.SetPaperId( wxPAPER_ENV_B6);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"Italian")))
        data.SetPaperId( wxPAPER_ENV_ITALY);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"personal")))
        data.SetPaperId( wxPAPER_ENV_PERSONAL);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"fanfold-us")))
        data.SetPaperId( wxPAPER_FANFOLD_US);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"fanfold-European")))
        data.SetPaperId( wxPAPER_FANFOLD_STD_GERMAN);
    else if (gtk_paper_size_is_equal(paper_size,gtk_paper_size_new ( (const gchar*)"foolscap")))
        data.SetPaperId( wxPAPER_FANFOLD_LGL_GERMAN);
    else
        data.SetPaperId(wxPAPER_NONE);
    return true;
}

// Put datas given by the wxPrintData into m_config.
// Called by wxPrintData::ConvertToNative().
bool wxGtkPrintNativeData::TransferFrom( const wxPrintData &data )
{
    if(!m_config)
        return false;

    wxPrintQuality quality = data.GetQuality();
    if (quality == wxPRINT_QUALITY_HIGH)
        gtk_print_settings_set_quality (m_config, GTK_PRINT_QUALITY_HIGH);
    else if (quality == wxPRINT_QUALITY_MEDIUM)
        gtk_print_settings_set_quality (m_config, GTK_PRINT_QUALITY_NORMAL);
    else if (quality == wxPRINT_QUALITY_LOW)
        gtk_print_settings_set_quality (m_config, GTK_PRINT_QUALITY_LOW);
    else if (quality == wxPRINT_QUALITY_DRAFT)
        gtk_print_settings_set_quality (m_config, GTK_PRINT_QUALITY_DRAFT);
    else if (quality > 1)
        gtk_print_settings_set_resolution (m_config, quality);
    else
        gtk_print_settings_set_quality (m_config, GTK_PRINT_QUALITY_NORMAL);

    gtk_print_settings_set_n_copies(m_config, data.GetNoCopies());

    gtk_print_settings_set_use_color(m_config, data.GetColour());

    switch (data.GetDuplex())
    {
        case wxDUPLEX_SIMPLEX:      gtk_print_settings_set_duplex (m_config, GTK_PRINT_DUPLEX_SIMPLEX);
                                break;

        case wxDUPLEX_HORIZONTAL:   gtk_print_settings_set_duplex (m_config, GTK_PRINT_DUPLEX_HORIZONTAL);
                                break;

        default:
        case wxDUPLEX_VERTICAL:      gtk_print_settings_set_duplex (m_config, GTK_PRINT_DUPLEX_VERTICAL);
                                break;
    }

    if (!data.IsOrientationReversed())
    {
        if (data.GetOrientation() == wxLANDSCAPE)
            gtk_print_settings_set_orientation (m_config, GTK_PAGE_ORIENTATION_LANDSCAPE);
        else
            gtk_print_settings_set_orientation (m_config, GTK_PAGE_ORIENTATION_PORTRAIT);
    }
    else {
        if (data.GetOrientation() == wxLANDSCAPE)
            gtk_print_settings_set_orientation (m_config, GTK_PAGE_ORIENTATION_REVERSE_LANDSCAPE);
        else
            gtk_print_settings_set_orientation (m_config, GTK_PAGE_ORIENTATION_REVERSE_PORTRAIT);
    }

    gtk_print_settings_set_collate (m_config, data.GetCollate());

    // Paper formats: these are the most common paper formats.
    switch (data.GetPaperId())
    {
        case wxPAPER_A3:        gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_A3));
                                break;
        case wxPAPER_A4:        gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_A4));
                                break;
        case wxPAPER_A5:        gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_A5));
                                break;
        case wxPAPER_B5_TRANSVERSE:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "jis-b5"));
                                break;
        case wxPAPER_B5:        gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_B5));
                                break;
        case wxPAPER_LETTER:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_LETTER));
                                break;
        case wxPAPER_LEGAL:     gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_LEGAL));
                                break;
        case wxPAPER_EXECUTIVE: gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_EXECUTIVE));
                                break;
        case wxPAPER_ENV_10:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "na_number-10"));
                                break;
        case wxPAPER_ENV_C5:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-c5"));
                                break;
        case wxPAPER_ENV_C6:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-c6"));
                                break;
        case wxPAPER_ENV_B5:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-c5b5"));
                                break;
        case wxPAPER_ENV_MONARCH:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "na_monarch"));
                                break;
        case wxPAPER_CSHEET:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "engineering-c"));
                                break;
        case wxPAPER_DSHEET:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "engineering-d"));
                                break;
        case wxPAPER_ESHEET:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "engineering-e"));
                                break;
        case wxPAPER_LETTERSMALL:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "letter"));
                                break;
        case wxPAPER_TABLOID:   gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "engineering-b"));
                                break;
        case wxPAPER_LEDGER:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "ledger"));
                                break;
        case wxPAPER_STATEMENT:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "statement"));
                                break;
        case wxPAPER_A4SMALL:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new (GTK_PAPER_NAME_A4));
                                break;
        case wxPAPER_B4:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-b4"));
                                break;
        case wxPAPER_FOLIO:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "folio"));
                                break;
        case wxPAPER_QUARTO:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "quarto"));
                                break;
        case wxPAPER_10X14:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "10x14"));
                                break;
        case wxPAPER_11X17:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "ledger"));
                                break;
        case wxPAPER_NOTE:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "letter"));
                                break;
        case wxPAPER_ENV_9:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "na-number-9-envelope"));
                                break;
        case wxPAPER_ENV_11:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "number-11"));
                                break;
        case wxPAPER_ENV_12:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "number-12"));
                                break;
        case wxPAPER_ENV_14:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "number-14"));
                                break;
        case wxPAPER_ENV_DL:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-designated"));
                                break;
        case wxPAPER_ENV_C3:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-c3"));
                                break;
        case wxPAPER_ENV_C4:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-c4"));
                                break;
        case wxPAPER_ENV_C65:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "c6/c5"));
                                break;
        case wxPAPER_ENV_B4:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-b4"));
                                break;
        case wxPAPER_ENV_B6:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "iso-b6"));
                                break;
        case wxPAPER_ENV_ITALY:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "Italian"));
                                break;
        case wxPAPER_ENV_PERSONAL:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "personal"));
                                break;
        case wxPAPER_FANFOLD_US:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "fanfold-us"));
                                break;
        case wxPAPER_FANFOLD_STD_GERMAN:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "fanfold-European"));
                                break;
        case wxPAPER_FANFOLD_LGL_GERMAN:    gtk_print_settings_set_paper_size(m_config, gtk_paper_size_new ((const gchar*) "foolscap"));
                                break;
        case wxPAPER_NONE:
        default:                break;
    }

    return true;
}

void wxGtkPrintNativeData::SetPrintConfig( GtkPrintSettings * config )
{
    if (config)
        m_config = gtk_print_settings_copy(config);
}

// Extract page setup from settings.
GtkPageSetup* wxGtkPrintNativeData::GetPageSetupFromSettings(GtkPrintSettings* settings)
{
    GtkPageSetup* page_setup = gtk_page_setup_new();
    gtk_page_setup_set_orientation (page_setup, gtk_print_settings_get_orientation (settings));

    GtkPaperSize *paper_size = gtk_print_settings_get_paper_size (settings);
    if (paper_size != NULL)
        gtk_page_setup_set_paper_size_and_default_margins (page_setup, paper_size);

    return page_setup;
}

// Insert page setup into a given GtkPrintSettings.
void wxGtkPrintNativeData::SetPageSetupToSettings(GtkPrintSettings* settings, GtkPageSetup* page_setup)
{
    gtk_print_settings_set_orientation ( settings, gtk_page_setup_get_orientation (page_setup));
    gtk_print_settings_set_paper_size ( settings, gtk_page_setup_get_paper_size (page_setup));
}

//----------------------------------------------------------------------------
// wxGtkPrintDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGtkPrintDialog, wxPrintDialogBase)

wxGtkPrintDialog::wxGtkPrintDialog( wxWindow *parent, wxPrintDialogData *data )
                    : wxPrintDialogBase(parent, wxID_ANY, _("Print"),
                               wxPoint(0, 0), wxSize(600, 600),
                               wxDEFAULT_DIALOG_STYLE |
                               wxTAB_TRAVERSAL)
{
    if (data)
        m_printDialogData = *data;

    m_parent = parent;
    SetShowDialog(true);
}

wxGtkPrintDialog::wxGtkPrintDialog( wxWindow *parent, wxPrintData *data )
                    : wxPrintDialogBase(parent, wxID_ANY, _("Print"),
                               wxPoint(0, 0), wxSize(600, 600),
                               wxDEFAULT_DIALOG_STYLE |
                               wxTAB_TRAVERSAL)
{
    if (data)
        m_printDialogData = *data;

    m_parent = parent;
    SetShowDialog(true);
}


wxGtkPrintDialog::~wxGtkPrintDialog()
{
}

// This is called even if we actually don't want the dialog to appear.
int wxGtkPrintDialog::ShowModal()
{
    GtkPrintOperationResult response;

    // We need to restore the settings given in the constructor.
    wxPrintData data = m_printDialogData.GetPrintData();
    wxGtkPrintNativeData *native =
      (wxGtkPrintNativeData*) data.GetNativeData();
    data.ConvertToNative();

    GtkPrintSettings * settings = native->GetPrintConfig();

    // We have to restore pages to print here because they're stored in a wxPrintDialogData and ConvertToNative only works for wxPrintData.
    int fromPage = m_printDialogData.GetFromPage();
    int toPage = m_printDialogData.GetToPage();
    if (m_printDialogData.GetSelection())
        gtk_print_settings_set_print_pages(settings, GTK_PRINT_PAGES_CURRENT);
    else if (m_printDialogData.GetAllPages())
        gtk_print_settings_set_print_pages(settings, GTK_PRINT_PAGES_ALL);
    else {
        gtk_print_settings_set_print_pages(settings, GTK_PRINT_PAGES_RANGES);
        GtkPageRange *range;
        range = g_new (GtkPageRange, 1);
        range[0].start = fromPage-1;
        range[0].end = (toPage >= fromPage) ? toPage-1 : fromPage-1;
        gtk_print_settings_set_page_ranges (settings, range, 1);
    }

    // If the settings are OK, we restore it.
    if (settings != NULL)
        gtk_print_operation_set_print_settings (native->GetPrintJob(), settings);
    gtk_print_operation_set_default_page_setup (native->GetPrintJob(), native->GetPageSetupFromSettings(settings));

    // Show the dialog if needed.
    GError* gError = NULL;
    if (GetShowDialog())
        response = gtk_print_operation_run (native->GetPrintJob(), GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG, GTK_WINDOW(gtk_widget_get_toplevel(m_parent->m_widget) ), &gError);
    else
        response = gtk_print_operation_run (native->GetPrintJob(), GTK_PRINT_OPERATION_ACTION_PRINT, GTK_WINDOW(gtk_widget_get_toplevel(m_parent->m_widget)), &gError);

    // Does everything went well?
    if (response == GTK_PRINT_OPERATION_RESULT_CANCEL)
    {
        return wxID_CANCEL;
    }
    else if (response == GTK_PRINT_OPERATION_RESULT_ERROR)
    {
        g_error_free (gError);
        wxLogError(_("Error while printing: ") + wxString::Format(_("%s"), gError->message));
        return wxID_NO; // We use wxID_NO because there is no wxID_ERROR available
    }

    // Now get the settings and save it.
    GtkPrintSettings* newSettings = gtk_print_operation_get_print_settings (native->GetPrintJob());
    native->SetPrintConfig(newSettings);
    data.ConvertFromNative();

    // Same problem as a few lines before.
    switch (gtk_print_settings_get_print_pages(newSettings))
    {
        case GTK_PRINT_PAGES_CURRENT:
            m_printDialogData.SetSelection( true );
            break;
        case GTK_PRINT_PAGES_RANGES:
            {// wxWidgets doesn't support multiple ranges, so we can only save the first one even if the user wants to print others.
            // For example, the user enters "1-3;5-7" in the dialog: pages 1-3 and 5-7 will be correctly printed when the user
            // will hit "OK" button. However we can only save 1-3 in the print data.
            gint num_ranges = 0;
            GtkPageRange* range;
            range = gtk_print_settings_get_page_ranges (newSettings, &num_ranges);
            if (num_ranges >= 1)
            {
                m_printDialogData.SetFromPage( range[0].start );
                m_printDialogData.SetToPage( range[0].end );
            }
            else {
                m_printDialogData.SetAllPages( true );
                m_printDialogData.SetFromPage( 0 );
                m_printDialogData.SetToPage( 9999 );
            }
            break;}
        case GTK_PRINT_PAGES_ALL:
        default:
            m_printDialogData.SetAllPages( true );
            m_printDialogData.SetFromPage( 0 );
            m_printDialogData.SetToPage( 9999 );
            break;
    }

    return wxID_OK;
}

//----------------------------------------------------------------------------
// wxGtkPageSetupDialog
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGtkPageSetupDialog, wxPageSetupDialogBase)

wxGtkPageSetupDialog::wxGtkPageSetupDialog( wxWindow *parent,
                            wxPageSetupDialogData* data )
{
    if (data)
        m_pageDialogData = *data;

    m_parent = parent;
}

wxGtkPageSetupDialog::~wxGtkPageSetupDialog()
{
}

int wxGtkPageSetupDialog::ShowModal()
{
    // Get the config.
    m_pageDialogData.GetPrintData().ConvertToNative();
    wxGtkPrintNativeData *native = (wxGtkPrintNativeData*) m_pageDialogData.GetPrintData().GetNativeData();
    GtkPrintSettings* nativeData = native->GetPrintConfig();

    // We only need the pagesetup data which are part of the settings.
    GtkPageSetup* oldPageSetup = native->GetPageSetupFromSettings(nativeData);

    // If the user used a custom paper format the last time he printed, we have to restore it too.
    if (m_pageDialogData.GetPrintData().GetPaperId() == wxPAPER_NONE)
    {
        wxSize customPaperSize = m_pageDialogData.GetPaperSize();
        if (customPaperSize.GetWidth() > 0 && customPaperSize.GetHeight() > 0)
        {
            wxString title = _("Custom size");
            GtkPaperSize* customSize = gtk_paper_size_new_custom ("custom", title.mb_str(), (gdouble) customPaperSize.GetWidth(), (gdouble) customPaperSize.GetHeight(), GTK_UNIT_MM);
            gtk_page_setup_set_paper_size_and_default_margins (oldPageSetup, customSize);
            g_object_unref(customSize);
        }
    }

    // Now show the dialog.
    GtkPageSetup* newPageSetup = gtk_print_run_page_setup_dialog (GTK_WINDOW(m_parent->m_widget),
                                                           oldPageSetup,
                                                           nativeData);

    int ret;
    if (newPageSetup != oldPageSetup)
    {
        native->SetPageSetupToSettings(nativeData, newPageSetup);
        m_pageDialogData.GetPrintData().ConvertFromNative();

        // Store custom paper format if any.
        if (m_pageDialogData.GetPrintData().GetPaperId() == wxPAPER_NONE)
        {
            gdouble ml,mr,mt,mb,pw,ph;
            ml = gtk_page_setup_get_left_margin (newPageSetup, GTK_UNIT_MM);
            mr = gtk_page_setup_get_right_margin (newPageSetup, GTK_UNIT_MM);
            mt = gtk_page_setup_get_top_margin (newPageSetup, GTK_UNIT_MM);
            mb = gtk_page_setup_get_bottom_margin (newPageSetup, GTK_UNIT_MM);

            pw = gtk_page_setup_get_paper_width (newPageSetup, GTK_UNIT_MM);
            ph = gtk_page_setup_get_paper_height (newPageSetup, GTK_UNIT_MM);

            m_pageDialogData.SetMarginTopLeft( wxPoint( (int)(ml+0.5), (int)(mt+0.5)) );
            m_pageDialogData.SetMarginBottomRight( wxPoint( (int)(mr+0.5), (int)(mb+0.5)) );

            m_pageDialogData.SetPaperSize( wxSize( (int)(pw+0.5), (int)(ph+0.5) ) );
        }

        ret = wxID_OK;
    }
    else
    {
        ret = wxID_CANCEL;
    }

    return ret;
}

//----------------------------------------------------------------------------
// wxGtkPrinter
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGtkPrinter, wxPrinterBase)

wxGtkPrinter::wxGtkPrinter( wxPrintDialogData *data ) :
    wxPrinterBase( data )
{
    m_gpc = NULL;

    if (data)
        m_printDialogData = *data;
}

wxGtkPrinter::~wxGtkPrinter()
{
}

bool wxGtkPrinter::Print(wxWindow *parent, wxPrintout *printout, bool prompt )
{
    if (!printout)
    {
        sm_lastError = wxPRINTER_ERROR;
        return false;
    }

    // Let's correct the PageInfo just in case the app gives wrong values.
    int fromPage, toPage;
    int minPage, maxPage;
    printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);
    m_printDialogData.SetAllPages(true);

    if (minPage < 1) minPage = 1;
    if (maxPage < 1) maxPage = 9999;
    if (maxPage < minPage) maxPage = minPage;

    m_printDialogData.SetMinPage(minPage);
    m_printDialogData.SetMaxPage(maxPage);
    if (fromPage != 0)
    {
        if (fromPage < minPage) fromPage = minPage;
        else if (fromPage > maxPage) fromPage = maxPage;
        m_printDialogData.SetFromPage(fromPage);
    }
    if (toPage != 0)
    {
        m_printDialogData.SetToPage(toPage);
        if (toPage > maxPage) toPage = maxPage;
        else if (toPage < minPage) toPage = minPage;
    }

    if (((minPage != fromPage) && fromPage != 0) || ((maxPage != toPage) && toPage != 0)) m_printDialogData.SetAllPages(false);


    wxPrintData printdata = GetPrintDialogData().GetPrintData();
    wxGtkPrintNativeData *native = (wxGtkPrintNativeData*) printdata.GetNativeData();

    GtkPrintOperation *printOp = gtk_print_operation_new ();

    native->SetPrintJob( printOp );

    printout->SetIsPreview(false);

    wxPrinterToGtkData dataToSend;
    dataToSend.printer = this;
    dataToSend.printout = printout;

    // These Gtk signals are caught here.
    g_signal_connect (printOp, "begin-print", G_CALLBACK (gtk_begin_print_callback), &dataToSend);
    g_signal_connect (printOp, "draw-page", G_CALLBACK (gtk_draw_page_print_callback), &dataToSend);
    g_signal_connect (printOp, "end-print", G_CALLBACK (gtk_end_print_callback), printout);
    g_signal_connect (printOp, "preview", G_CALLBACK (gtk_preview_print_callback), printout);

    // This is used to setup the DC and
    // show the dialog if desired
    wxGtkPrintDialog dialog( parent, &m_printDialogData );
    dialog.SetPrintDC(m_dc);
    dialog.SetShowDialog(prompt);

    // doesn't necessarily show
    int ret = dialog.ShowModal();
    if (ret == wxID_CANCEL)
    {
        sm_lastError = wxPRINTER_CANCELLED;
    }
    if (ret == wxID_NO)
    {
        sm_lastError = wxPRINTER_ERROR;
        wxFAIL_MSG(_("The print dialog returned an error."));
    }

    g_object_unref (printOp);

    return (sm_lastError == wxPRINTER_NO_ERROR);
}

void wxGtkPrinter::BeginPrint(wxPrintout *printout, GtkPrintOperation *operation, GtkPrintContext *context)
{
    wxPrintData printdata = GetPrintDialogData().GetPrintData();
    wxGtkPrintNativeData *native = (wxGtkPrintNativeData*) printdata.GetNativeData();

    SetPrintContext(context);
    native->SetPrintContext( context );

    wxGtkPrintDC *printDC = new wxGtkPrintDC( printdata );
    m_dc = printDC;

    if (!m_dc->IsOk())
    {
        if (sm_lastError != wxPRINTER_CANCELLED)
        {
            sm_lastError = wxPRINTER_ERROR;
            wxFAIL_MSG(_("The wxGtkPrintDC cannot be used."));
        }
        return;
    }
    wxSize ScreenPixels = wxGetDisplaySize();
    wxSize ScreenMM = wxGetDisplaySizeMM();

    printout->SetPPIScreen( (int) ((ScreenPixels.GetWidth() * 25.4) / ScreenMM.GetWidth()),
                            (int) ((ScreenPixels.GetHeight() * 25.4) / ScreenMM.GetHeight()) );
    printout->SetPPIPrinter( printDC->GetResolution(),
                             printDC->GetResolution() );

    printout->SetDC(m_dc);

    int w, h;
    m_dc->GetSize(&w, &h);
    printout->SetPageSizePixels((int)w, (int)h);
    printout->SetPaperRectPixels(wxRect(0, 0, w, h));
    int mw, mh;
    m_dc->GetSizeMM(&mw, &mh);
    printout->SetPageSizeMM((int)mw, (int)mh);
    printout->OnPreparePrinting();

    // Get some parameters from the printout, if defined.
    int fromPage, toPage;
    int minPage, maxPage;
    printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

    if (maxPage == 0)
    {
        sm_lastError = wxPRINTER_ERROR;
        wxFAIL_MSG(_("wxPrintout::GetPageInfo gives a null maxPage."));
        return;
    }

    printout->OnBeginPrinting();

    int numPages = 0;

    // If we're not previewing we need to calculate the number of pages to print.
    // If we're previewing, Gtk Print will render every pages without wondering about the page ranges the user may
    // have defined in the dialog. So the number of pages is the maximum available.
    if (!printout->IsPreview())
    {
        GtkPrintSettings * settings = gtk_print_operation_get_print_settings (operation);
        switch (gtk_print_settings_get_print_pages(settings))
        {
            case GTK_PRINT_PAGES_CURRENT:
                numPages = 1;
                break;
            case GTK_PRINT_PAGES_RANGES:
                {gint num_ranges = 0;
                GtkPageRange* range;
                int i;
                range = gtk_print_settings_get_page_ranges (settings, &num_ranges);
                for (i=0; i<num_ranges; i++)
                {
                    if (range[i].end < range[i].start) range[i].end = range[i].start;
                    if (range[i].start < minPage-1) range[i].start = minPage-1;
                    if (range[i].end > maxPage-1) range[i].end = maxPage-1;
                    if (range[i].start > maxPage-1) range[i].start = maxPage-1;
                    numPages += range[i].end - range[i].start + 1;
                }
                gtk_print_settings_set_page_ranges (settings, range, 1);
                break;}
            case GTK_PRINT_PAGES_ALL:
            default:
                numPages = maxPage - minPage + 1;
                break;
        }
    }
    else numPages = maxPage - minPage + 1;

    gtk_print_operation_set_n_pages(operation, numPages);
}

void wxGtkPrinter::DrawPage(wxPrintout *printout, GtkPrintOperation *operation, GtkPrintContext *context, int page_nr)
{
    int fromPage, toPage, minPage, maxPage, startPage, endPage;
    printout->GetPageInfo(&minPage, &maxPage, &fromPage, &toPage);

    int numPageToDraw = page_nr + minPage;
    if (numPageToDraw < minPage) numPageToDraw = minPage;
    if (numPageToDraw > maxPage) numPageToDraw = maxPage;

    GtkPrintSettings * settings = gtk_print_operation_get_print_settings (operation);
    switch (gtk_print_settings_get_print_pages(settings))
    {
        case GTK_PRINT_PAGES_CURRENT:
            g_object_get_property((GObject*) operation, (const gchar *) "current-page", (GValue*) &startPage);
            g_object_get_property((GObject*) operation, (const gchar *) "current-page", (GValue*) &endPage);
            break;
        case GTK_PRINT_PAGES_RANGES:
            {gint num_ranges = 0;
            GtkPageRange* range;
            range = gtk_print_settings_get_page_ranges (settings, &num_ranges);
            // We don't need to verify these values as it has already been done in wxGtkPrinter::BeginPrint.
            if (num_ranges >= 1)
            {
                startPage = range[0].start + 1;
                endPage = range[0].end + 1;
            }
            else {
                startPage = minPage;
                endPage = maxPage;
            }
            break;}
        case GTK_PRINT_PAGES_ALL:
        default:
            startPage = minPage;
            endPage = maxPage;
            break;
    }

    if(numPageToDraw == startPage)
    {
        if (!printout->OnBeginDocument(startPage, endPage))
        {
            wxLogError(_("Could not start printing."));
            sm_lastError = wxPRINTER_ERROR;
        }
    }

    // The app can render the page numPageToDraw.
    if (printout->HasPage(numPageToDraw))
    {
        m_dc->StartPage();
        printout->OnPrintPage(numPageToDraw);
        m_dc->EndPage();
    }


    if(numPageToDraw == endPage)
    {
        printout->OnEndDocument();
    }
}

wxDC* wxGtkPrinter::PrintDialog( wxWindow *parent )
{
    wxGtkPrintDialog dialog( parent, &m_printDialogData );

    dialog.SetPrintDC(m_dc);
    dialog.SetShowDialog(true);

    int ret = dialog.ShowModal();

    if (ret == wxID_CANCEL)
    {
        sm_lastError = wxPRINTER_CANCELLED;
        return NULL;
    }
    if (ret == wxID_NO)
    {
        sm_lastError = wxPRINTER_ERROR;
        wxFAIL_MSG(_("The print dialog returned an error."));
        return NULL;
    }

    m_printDialogData = dialog.GetPrintDialogData();
    return new wxGtkPrintDC( m_printDialogData.GetPrintData() );
}

bool wxGtkPrinter::Setup( wxWindow *parent )
{
    // Obsolete, for backward compatibility.
    return false;
}

//-----------------------------------------------------------------------------
// wxGtkPrintDC
//-----------------------------------------------------------------------------

#define XLOG2DEV(x)     ((double)(LogicalToDeviceX(x)) * m_DEV2PS)
#define XLOG2DEVREL(x)  ((double)(LogicalToDeviceXRel(x)) * m_DEV2PS)
#define YLOG2DEV(x)     ((double)(LogicalToDeviceY(x)) * m_DEV2PS)
#define YLOG2DEVREL(x)  ((double)(LogicalToDeviceYRel(x)) * m_DEV2PS)

IMPLEMENT_CLASS(wxGtkPrintDC, wxDC)

wxGtkPrintDC::wxGtkPrintDC( const wxPrintData& data )
{
    m_printData = data;

    wxGtkPrintNativeData *native =
        (wxGtkPrintNativeData*) m_printData.GetNativeData();

    m_gpc = native->GetPrintContext();

    // Match print quality to resolution (high = 1200dpi)
    m_resolution = m_printData.GetQuality(); // (int) gtk_print_context_get_dpi_x( m_gpc );
    if (m_resolution < 0)
        m_resolution = (1 << (m_resolution+4)) *150;

    m_PS2DEV = (double)m_resolution / 72.0;
    m_DEV2PS = 72.0 / (double)m_resolution;

    m_context = gtk_print_context_create_pango_context( m_gpc );
    m_layout = gtk_print_context_create_pango_layout ( m_gpc );
    m_fontdesc = pango_font_description_from_string( "Sans 12" );

    m_cairo = gtk_print_context_get_cairo_context ( m_gpc );

    m_currentRed = 0;
    m_currentBlue = 0;
    m_currentGreen = 0;

    m_signX = 1;  // default x-axis left to right.
    m_signY = 1;  // default y-axis bottom up -> top down.

    // By default the origin of the cairo context is in the upper left
    // corner of the printable area. We need to translate it so that it
    // is in the upper left corner of the paper (without margins)
    GtkPageSetup *setup = gtk_print_context_get_page_setup( m_gpc );
    gdouble ml, mt;
    ml = gtk_page_setup_get_left_margin (setup, GTK_UNIT_POINTS);
    mt = gtk_page_setup_get_top_margin (setup, GTK_UNIT_POINTS);
    gs_cairo->cairo_translate(m_cairo, -ml, -mt);
}

wxGtkPrintDC::~wxGtkPrintDC()
{
    g_object_unref(m_context);
    g_object_unref(m_layout);
}

bool wxGtkPrintDC::IsOk() const
{
    return (m_gpc != NULL);
}

bool wxGtkPrintDC::DoFloodFill(wxCoord x1, wxCoord y1, const wxColour &col, int style )
{
    // We can't access the given coord as a cairo context is scalable, ie a coord doesn't mean anything in this context.
    wxFAIL_MSG(_("not implemented"));
    return false;
}

void wxGtkPrintDC::DoGradientFillConcentric(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, const wxPoint& circleCenter)
{
    wxCoord xC = circleCenter.x;
    wxCoord yC = circleCenter.y;
    wxCoord xR = rect.x;
    wxCoord yR = rect.y;
    wxCoord w =  rect.width;
    wxCoord h = rect.height;

    double radius = sqrt((w/2)*(w/2)+(h/2)*(h/2));

    unsigned char redI = initialColour.Red();
    unsigned char blueI = initialColour.Blue();
    unsigned char greenI = initialColour.Green();
    unsigned char alphaI = initialColour.Alpha();
    unsigned char redD = destColour.Red();
    unsigned char blueD = destColour.Blue();
    unsigned char greenD = destColour.Green();
    unsigned char alphaD = destColour.Alpha();

    double redIPS = (double)(redI) / 255.0;
    double blueIPS = (double)(blueI) / 255.0;
    double greenIPS = (double)(greenI) / 255.0;
    double alphaIPS = (double)(alphaI) / 255.0;
    double redDPS = (double)(redD) / 255.0;
    double blueDPS = (double)(blueD) / 255.0;
    double greenDPS = (double)(greenD) / 255.0;
    double alphaDPS = (double)(alphaD) / 255.0;

    // Create a pattern with the gradient.
    cairo_pattern_t* gradient;
    gradient = gs_cairo->cairo_pattern_create_radial (XLOG2DEV(xC+xR), YLOG2DEV(yC+yR), 0, XLOG2DEV(xC+xR), YLOG2DEV(yC+yR), radius * m_DEV2PS );
    gs_cairo->cairo_pattern_add_color_stop_rgba (gradient, 0.0, redIPS, greenIPS, blueIPS, alphaIPS);
    gs_cairo->cairo_pattern_add_color_stop_rgba (gradient, 1.0, redDPS, greenDPS, blueDPS, alphaDPS);

    // Fill the rectangle with this pattern.
    gs_cairo->cairo_set_source(m_cairo, gradient);
    gs_cairo->cairo_rectangle (m_cairo, XLOG2DEV(xR), YLOG2DEV(yR), XLOG2DEVREL(w), YLOG2DEVREL(h) );
    gs_cairo->cairo_fill(m_cairo);

    gs_cairo->cairo_pattern_destroy(gradient);

    CalcBoundingBox(xR, yR);
    CalcBoundingBox(xR+w, yR+h);
}

void wxGtkPrintDC::DoGradientFillLinear(const wxRect& rect, const wxColour& initialColour, const wxColour& destColour, wxDirection nDirection)
{
    wxCoord x = rect.x;
    wxCoord y = rect.y;
    wxCoord w = rect.width;
    wxCoord h = rect.height;

    unsigned char redI = initialColour.Red();
    unsigned char blueI = initialColour.Blue();
    unsigned char greenI = initialColour.Green();
    unsigned char alphaI = initialColour.Alpha();
    unsigned char redD = destColour.Red();
    unsigned char blueD = destColour.Blue();
    unsigned char greenD = destColour.Green();
    unsigned char alphaD = destColour.Alpha();

    double redIPS = (double)(redI) / 255.0;
    double blueIPS = (double)(blueI) / 255.0;
    double greenIPS = (double)(greenI) / 255.0;
    double alphaIPS = (double)(alphaI) / 255.0;
    double redDPS = (double)(redD) / 255.0;
    double blueDPS = (double)(blueD) / 255.0;
    double greenDPS = (double)(greenD) / 255.0;
    double alphaDPS = (double)(alphaD) / 255.0;

    // Create a pattern with the gradient.
    cairo_pattern_t* gradient;
    gradient = gs_cairo->cairo_pattern_create_linear (XLOG2DEV(x), YLOG2DEV(y), XLOG2DEV(x+w), YLOG2DEV(y));

    if (nDirection == wxWEST)
    {
        gs_cairo->cairo_pattern_add_color_stop_rgba (gradient, 0.0, redDPS, greenDPS, blueDPS, alphaDPS);
        gs_cairo->cairo_pattern_add_color_stop_rgba (gradient, 1.0, redIPS, greenIPS, blueIPS, alphaIPS);
    }
    else {
        gs_cairo->cairo_pattern_add_color_stop_rgba (gradient, 0.0, redIPS, greenIPS, blueIPS, alphaIPS);
        gs_cairo->cairo_pattern_add_color_stop_rgba (gradient, 1.0, redDPS, greenDPS, blueDPS, alphaDPS);
    }

    // Fill the rectangle with this pattern.
    gs_cairo->cairo_set_source(m_cairo, gradient);
    gs_cairo->cairo_rectangle (m_cairo, XLOG2DEV(x), YLOG2DEV(y), XLOG2DEVREL(w), YLOG2DEVREL(h) );
    gs_cairo->cairo_fill(m_cairo);

    gs_cairo->cairo_pattern_destroy(gradient);

    CalcBoundingBox(x, y);
    CalcBoundingBox(x+w, y+h);
}

bool wxGtkPrintDC::DoGetPixel(wxCoord x1, wxCoord y1, wxColour *col) const
{
    // We can't access the given coord as a cairo context is scalable, ie a coord doesn't mean anything in this context.
    wxFAIL_MSG(_("not implemented"));
    return false;
}

void wxGtkPrintDC::DoDrawLine(wxCoord x1, wxCoord y1, wxCoord x2, wxCoord y2)
{
    if  (m_pen.GetStyle() == wxTRANSPARENT) return;

    SetPen( m_pen );
    gs_cairo->cairo_move_to ( m_cairo, XLOG2DEV(x1), YLOG2DEV(y1) );
    gs_cairo->cairo_line_to ( m_cairo, XLOG2DEV(x2), YLOG2DEV(y2) );
    gs_cairo->cairo_stroke ( m_cairo );

    CalcBoundingBox( x1, y1 );
    CalcBoundingBox( x2, y2 );
}

void wxGtkPrintDC::DoCrossHair(wxCoord x, wxCoord y)
{
    int w, h;
    DoGetSize(&w, &h);

    SetPen(m_pen);

    gs_cairo->cairo_move_to (m_cairo, XLOG2DEV(x), 0);
    gs_cairo->cairo_line_to (m_cairo, XLOG2DEV(x), YLOG2DEVREL(h));
    gs_cairo->cairo_move_to (m_cairo, 0, YLOG2DEV(y));
    gs_cairo->cairo_line_to (m_cairo, XLOG2DEVREL(w), YLOG2DEV(y));

    gs_cairo->cairo_stroke (m_cairo);
    CalcBoundingBox( 0, 0 );
    CalcBoundingBox( w, h );
}

void wxGtkPrintDC::DoDrawArc(wxCoord x1,wxCoord y1,wxCoord x2,wxCoord y2,wxCoord xc,wxCoord yc)
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
            atan2(double(y1-yc), double(x1-xc)) * RAD2DEG;
        alpha2 = (x2 - xc == 0) ?
            (y2 - yc < 0) ? 90.0 : -90.0 :
            atan2(double(y2-yc), double(x2-xc)) * RAD2DEG;

        while (alpha1 <= 0)   alpha1 += 360;
        while (alpha2 <= 0)   alpha2 += 360; // adjust angles to be between.
        while (alpha1 > 360)  alpha1 -= 360; // 0 and 360 degree.
        while (alpha2 > 360)  alpha2 -= 360;
    }

    alpha1 *= DEG2RAD;
    alpha2 *= DEG2RAD;

    gs_cairo->cairo_new_path(m_cairo);

    gs_cairo->cairo_arc_negative ( m_cairo, XLOG2DEV(xc), YLOG2DEV(yc), XLOG2DEVREL((int)radius), alpha1, alpha2);
    gs_cairo->cairo_line_to(m_cairo, XLOG2DEV(xc), YLOG2DEV(yc));
    gs_cairo->cairo_close_path (m_cairo);

    SetBrush( m_brush );
    gs_cairo->cairo_fill_preserve( m_cairo );

    SetPen (m_pen);
    gs_cairo->cairo_stroke( m_cairo );

    CalcBoundingBox (x1, y1);
    CalcBoundingBox (xc, yc);
    CalcBoundingBox (x2, y2);
}

void wxGtkPrintDC::DoDrawEllipticArc(wxCoord x,wxCoord y,wxCoord w,wxCoord h,double sa,double ea)
{
    gs_cairo->cairo_save( m_cairo );

    gs_cairo->cairo_new_path(m_cairo);

    gs_cairo->cairo_translate( m_cairo, XLOG2DEV((wxCoord) (x + w / 2.)), XLOG2DEV((wxCoord) (y + h / 2.)) );
    double scale = (double)YLOG2DEVREL(h) / (double) XLOG2DEVREL(w);
    gs_cairo->cairo_scale( m_cairo, 1.0, scale );

    gs_cairo->cairo_arc_negative ( m_cairo, 0, 0, XLOG2DEVREL(w/2), -sa*DEG2RAD, -ea*DEG2RAD);

    SetPen (m_pen);
    gs_cairo->cairo_stroke_preserve( m_cairo );

    gs_cairo->cairo_line_to(m_cairo, 0,0);

    SetBrush( m_brush );
    gs_cairo->cairo_fill( m_cairo );

    gs_cairo->cairo_restore( m_cairo );

    CalcBoundingBox( x, y);
    CalcBoundingBox( x+w, y+h );
}

void wxGtkPrintDC::DoDrawPoint(wxCoord x, wxCoord y)
{
    if  (m_pen.GetStyle() == wxTRANSPARENT) return;

    SetPen( m_pen );

    gs_cairo->cairo_move_to ( m_cairo, XLOG2DEV(x), YLOG2DEV(y) );
    gs_cairo->cairo_line_to ( m_cairo, XLOG2DEV(x), YLOG2DEV(y) );
    gs_cairo->cairo_stroke ( m_cairo );

    CalcBoundingBox( x, y );
}

void wxGtkPrintDC::DoDrawLines(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset)
{
    if (m_pen.GetStyle() == wxTRANSPARENT) return;

    if (n <= 0) return;

    SetPen (m_pen);

    int i;
    for ( i =0; i<n ; i++ )
        CalcBoundingBox( points[i].x+xoffset, points[i].y+yoffset);

    gs_cairo->cairo_move_to ( m_cairo, XLOG2DEV(points[0].x+xoffset), YLOG2DEV(points[0].y+yoffset) );

    for (i = 1; i < n; i++)
        gs_cairo->cairo_line_to ( m_cairo, XLOG2DEV(points[i].x+xoffset), YLOG2DEV(points[i].y+yoffset) );

    gs_cairo->cairo_stroke ( m_cairo);
}

void wxGtkPrintDC::DoDrawPolygon(int n, wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    if (n==0) return;

    gs_cairo->cairo_save(m_cairo);
    if (fillStyle == wxWINDING_RULE)
        gs_cairo->cairo_set_fill_rule( m_cairo, CAIRO_FILL_RULE_WINDING);
    else
        gs_cairo->cairo_set_fill_rule( m_cairo, CAIRO_FILL_RULE_EVEN_ODD);

    int x = points[0].x + xoffset;
    int y = points[0].y + yoffset;
    gs_cairo->cairo_new_path(m_cairo);
    gs_cairo->cairo_move_to( m_cairo, XLOG2DEV(x), YLOG2DEV(y) );
    int i;
    for (i = 1; i < n; i++)
    {
        int x = points[i].x + xoffset;
        int y = points[i].y + yoffset;
        gs_cairo->cairo_line_to( m_cairo, XLOG2DEV(x), YLOG2DEV(y) );
    }
    gs_cairo->cairo_close_path(m_cairo);

    SetBrush( m_brush );
    gs_cairo->cairo_fill_preserve( m_cairo );

    SetPen (m_pen);
    gs_cairo->cairo_stroke( m_cairo );

    CalcBoundingBox( x, y );

    gs_cairo->cairo_restore(m_cairo);
}

void wxGtkPrintDC::DoDrawPolyPolygon(int n, int count[], wxPoint points[], wxCoord xoffset, wxCoord yoffset, int fillStyle)
{
    wxDC::DoDrawPolyPolygon( n, count, points, xoffset, yoffset, fillStyle );
}

void wxGtkPrintDC::DoDrawRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    width--;
    height--;

    gs_cairo->cairo_new_path(m_cairo);
    gs_cairo->cairo_rectangle ( m_cairo, XLOG2DEV(x), YLOG2DEV(y), XLOG2DEVREL(width), YLOG2DEVREL(height));

    SetBrush( m_brush );
    gs_cairo->cairo_fill_preserve( m_cairo );

    SetPen (m_pen);
    gs_cairo->cairo_stroke( m_cairo );

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );
}

void wxGtkPrintDC::DoDrawRoundedRectangle(wxCoord x, wxCoord y, wxCoord width, wxCoord height, double radius)
{
    width--;
    height--;

    if (radius < 0.0) radius = - radius * ((width < height) ? width : height);

    wxCoord dd = 2 * (wxCoord) radius;
    if (dd > width) dd = width;
    if (dd > height) dd = height;
    radius = dd / 2;

    wxCoord rad = (wxCoord) radius;

    gs_cairo->cairo_new_path(m_cairo);
    gs_cairo->cairo_move_to(m_cairo,XLOG2DEV(x + rad),YLOG2DEV(y));
    gs_cairo->cairo_curve_to(m_cairo,
                                XLOG2DEV(x + rad),YLOG2DEV(y),
                                XLOG2DEV(x),YLOG2DEV(y),
                                XLOG2DEV(x),YLOG2DEV(y + rad));
    gs_cairo->cairo_line_to(m_cairo,XLOG2DEV(x),YLOG2DEV(y + height - rad));
    gs_cairo->cairo_curve_to(m_cairo,
                                XLOG2DEV(x),YLOG2DEV(y + height - rad),
                                XLOG2DEV(x),YLOG2DEV(y + height),
                                XLOG2DEV(x + rad),YLOG2DEV(y + height));
    gs_cairo->cairo_line_to(m_cairo,XLOG2DEV(x + width - rad),YLOG2DEV(y + height));
    gs_cairo->cairo_curve_to(m_cairo,
                                XLOG2DEV(x + width - rad),YLOG2DEV(y + height),
                                XLOG2DEV(x + width),YLOG2DEV(y + height),
                                XLOG2DEV(x + width),YLOG2DEV(y + height - rad));
    gs_cairo->cairo_line_to(m_cairo,XLOG2DEV(x + width),YLOG2DEV(y + rad));
    gs_cairo->cairo_curve_to(m_cairo,
                                XLOG2DEV(x + width),YLOG2DEV(y + rad),
                                XLOG2DEV(x + width),YLOG2DEV(y),
                                XLOG2DEV(x + width - rad),YLOG2DEV(y));
    gs_cairo->cairo_line_to(m_cairo,XLOG2DEV(x + rad),YLOG2DEV(y));
    gs_cairo->cairo_close_path(m_cairo);

    SetBrush(m_brush);
    gs_cairo->cairo_fill_preserve(m_cairo);

    SetPen(m_pen);
    gs_cairo->cairo_stroke(m_cairo);

    CalcBoundingBox(x,y);
    CalcBoundingBox(x+width,y+height);
}

void wxGtkPrintDC::DoDrawEllipse(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    width--;
    height--;

    gs_cairo->cairo_save (m_cairo);

    gs_cairo->cairo_new_path(m_cairo);

    gs_cairo->cairo_translate (m_cairo, XLOG2DEV((wxCoord) (x + width / 2.)), YLOG2DEV((wxCoord) (y + height / 2.)));
    gs_cairo->cairo_scale(m_cairo, 1, (double)YLOG2DEVREL(height)/(double)XLOG2DEVREL(width));
    gs_cairo->cairo_arc ( m_cairo, 0, 0, XLOG2DEVREL(width/2), 0, 2 * M_PI);

    SetBrush( m_brush );
    gs_cairo->cairo_fill_preserve( m_cairo );

    SetPen (m_pen);
    gs_cairo->cairo_stroke( m_cairo );

    CalcBoundingBox( x, y );
    CalcBoundingBox( x + width, y + height );

    gs_cairo->cairo_restore (m_cairo);
}

#if wxUSE_SPLINES
void wxGtkPrintDC::DoDrawSpline(wxList *points)
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

    gs_cairo->cairo_new_path( m_cairo );
    gs_cairo->cairo_move_to( m_cairo, XLOG2DEV((wxCoord)x1), YLOG2DEV((wxCoord)y1) );
    gs_cairo->cairo_line_to( m_cairo, XLOG2DEV((wxCoord)x3), YLOG2DEV((wxCoord)y3) );

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

        gs_cairo->cairo_curve_to(m_cairo,
            XLOG2DEV((wxCoord)x1), YLOG2DEV((wxCoord)y1),
            XLOG2DEV((wxCoord)x2), YLOG2DEV((wxCoord)y2),
            XLOG2DEV((wxCoord)x3), YLOG2DEV((wxCoord)y3) );

        CalcBoundingBox( (wxCoord)x1, (wxCoord)y1 );
        CalcBoundingBox( (wxCoord)x3, (wxCoord)y3 );

        node = node->GetNext();
    }

    gs_cairo->cairo_line_to ( m_cairo, XLOG2DEV((wxCoord)c), YLOG2DEV((wxCoord)d) );

    gs_cairo->cairo_stroke( m_cairo );
}
#endif // wxUSE_SPLINES

bool wxGtkPrintDC::DoBlit(wxCoord xdest, wxCoord ydest, wxCoord width, wxCoord height,
            wxDC *source, wxCoord xsrc, wxCoord ysrc, int rop, bool useMask,
            wxCoord xsrcMask, wxCoord ysrcMask)
{
    wxCHECK_MSG( source, false, wxT("invalid source dc") );

    // Blit into a bitmap.
    wxBitmap bitmap( width, height );
    wxMemoryDC memDC;
    memDC.SelectObject(bitmap);
    memDC.Blit(0, 0, width, height, source, xsrc, ysrc, rop);
    memDC.SelectObject(wxNullBitmap);

    // Draw bitmap. scaling and positioning is done there.
    DrawBitmap( bitmap, xdest, ydest, useMask );

    return true;
}

void wxGtkPrintDC::DoDrawIcon( const wxIcon& icon, wxCoord x, wxCoord y )
{
    DoDrawBitmap( icon, x, y, true );
}

void wxGtkPrintDC::DoDrawBitmap( const wxBitmap& bitmap, wxCoord x, wxCoord y, bool useMask )
{
    wxCHECK_RET( bitmap.IsOk(), wxT("Invalid bitmap in wxGtkPrintDC::DoDrawBitmap"));

    cairo_surface_t* surface;
    x = wxCoord(XLOG2DEV(x));
    y = wxCoord(YLOG2DEV(y));
    int bw = bitmap.GetWidth();
    int bh = bitmap.GetHeight();
    wxBitmap bmpSource = bitmap;  // we need a non-const instance.
    unsigned char* buffer = new unsigned char[bw*bh*4];
    wxUint32* data = (wxUint32*)buffer;

    wxMask *mask = NULL;
    if (useMask) mask = bmpSource.GetMask();

    // Create a surface object and copy the bitmap pixel data to it. If the image has alpha (or a mask represented as alpha)
    // then we'll use a different format and iterator than if it doesn't.
    if (bmpSource.HasAlpha() || mask)
    {
        surface = gs_cairo->cairo_image_surface_create_for_data(
            buffer, CAIRO_FORMAT_ARGB32, bw, bh, bw*4);
        wxAlphaPixelData pixData(bmpSource, wxPoint(0,0), wxSize(bw, bh));
        wxCHECK_RET( pixData, wxT("Failed to gain raw access to bitmap data."));

        wxAlphaPixelData::Iterator p(pixData);
        int y, x;
        for (y=0; y<bh; y++)
        {
            wxAlphaPixelData::Iterator rowStart = p;
            for (x=0; x<bw; x++)
            {
                // Each pixel in CAIRO_FORMAT_ARGB32 is a 32-bit quantity,
                // with alpha in the upper 8 bits, then red, then green, then
                // blue. The 32-bit quantities are stored native-endian.
                // Pre-multiplied alpha is used.
                unsigned char alpha = p.Alpha();

                if (!bmpSource.HasAlpha() && mask)
                    alpha = 255;

                if (alpha == 0)
                    *data = 0;
                else
                    *data = ( alpha                  << 24
                              | (p.Red() * alpha/255)    << 16
                              | (p.Green() * alpha/255)  <<  8
                              | (p.Blue() * alpha/255) );
                ++data;
                ++p;
            }
            p = rowStart;
            p.OffsetY(pixData, 1);
        }
    }
    else  // no alpha
    {
        surface = gs_cairo->cairo_image_surface_create_for_data(
            buffer, CAIRO_FORMAT_RGB24, bw, bh, bw*4);
        wxNativePixelData pixData(bmpSource, wxPoint(0,0), wxSize(bw, bh));
        wxCHECK_RET( pixData, wxT("Failed to gain raw access to bitmap data."));

        wxNativePixelData::Iterator p(pixData);
        int y, x;
        for (y=0; y<bh; y++)
        {
            wxNativePixelData::Iterator rowStart = p;
            for (x=0; x<bw; x++)
            {
                // Each pixel in CAIRO_FORMAT_RGB24 is a 32-bit quantity, with
                // the upper 8 bits unused. Red, Green, and Blue are stored in
                // the remaining 24 bits in that order.  The 32-bit quantities
                // are stored native-endian.
                *data = ( p.Red() << 16 | p.Green() << 8 | p.Blue() );
                ++data;
                ++p;
            }
            p = rowStart;
            p.OffsetY(pixData, 1);
        }
    }


    gs_cairo->cairo_save(m_cairo);

    // Prepare to draw the image.
    gs_cairo->cairo_translate(m_cairo, x, y);

    // Scale the image
    cairo_filter_t filter = CAIRO_FILTER_BILINEAR;
    cairo_pattern_t* pattern = cairo_pattern_create_for_surface(surface);
    cairo_pattern_set_filter(pattern,filter);
    wxDouble scaleX = (wxDouble) XLOG2DEVREL(bw) / (wxDouble) bw;
    wxDouble scaleY = (wxDouble) YLOG2DEVREL(bh) / (wxDouble) bh;
    cairo_scale(m_cairo, scaleX, scaleY);

    gs_cairo->cairo_set_source(m_cairo, pattern);
    // Use the original size here since the context is scaled already.
    gs_cairo->cairo_rectangle(m_cairo, 0, 0, bw, bh);
    // Fill the rectangle using the pattern.
    gs_cairo->cairo_fill(m_cairo);

    // Clean up.
    gs_cairo->cairo_pattern_destroy(pattern);
    gs_cairo->cairo_surface_destroy(surface);
    delete [] buffer;

    CalcBoundingBox(0,0);
    CalcBoundingBox(bw,bh);

    gs_cairo->cairo_restore(m_cairo);
}

void wxGtkPrintDC::DoDrawText(const wxString& text, wxCoord x, wxCoord y )
{
    DoDrawRotatedText( text, x, y, 0.0 );
}

void wxGtkPrintDC::DoDrawRotatedText(const wxString& text, wxCoord x, wxCoord y, double angle)
{
    double xx = XLOG2DEV(x);
    double yy = YLOG2DEV(y);

    angle = -angle;

    bool underlined = m_font.Ok() && m_font.GetUnderlined();

// FIXME-UTF8: wouldn't be needed if utf8_str() always returned a buffer
#if wxUSE_UNICODE_UTF8
    const char *data = text.utf8_str();
#else
    const wxCharBuffer data = text.utf8_str();
#endif

    size_t datalen = strlen(data);
    pango_layout_set_text( m_layout, data, datalen);

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
        unsigned char alpha = m_textForegroundColour.Alpha();

        if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue && alpha == m_currentAlpha))
        {
            double redPS = (double)(red) / 255.0;
            double bluePS = (double)(blue) / 255.0;
            double greenPS = (double)(green) / 255.0;
            double alphaPS = (double)(alpha) / 255.0;

            gs_cairo->cairo_set_source_rgba( m_cairo, redPS, greenPS, bluePS, alphaPS );

            m_currentRed = red;
            m_currentBlue = blue;
            m_currentGreen = green;
            m_currentAlpha = alpha;
        }
    }

    int w,h;

    // Scale font description.
    gint oldSize = pango_font_description_get_size( m_fontdesc );
    double size = oldSize;
    size = size * m_scaleX;
    pango_font_description_set_size( m_fontdesc, (gint)size );

    // Actually apply scaled font.
    pango_layout_set_font_description( m_layout, m_fontdesc );

    pango_layout_get_pixel_size( m_layout, &w, &h );

        if ( m_backgroundMode == wxSOLID )
        {
            unsigned char red = m_textBackgroundColour.Red();
            unsigned char blue = m_textBackgroundColour.Blue();
            unsigned char green = m_textBackgroundColour.Green();
            unsigned char alpha = m_textBackgroundColour.Alpha();

            double redPS = (double)(red) / 255.0;
            double bluePS = (double)(blue) / 255.0;
            double greenPS = (double)(green) / 255.0;
            double alphaPS = (double)(alpha) / 255.0;

            gs_cairo->cairo_save(m_cairo);
            gs_cairo->cairo_translate(m_cairo, xx, yy);
            gs_cairo->cairo_set_source_rgba( m_cairo, redPS, greenPS, bluePS, alphaPS );
            gs_cairo->cairo_rotate(m_cairo,angle*DEG2RAD);
            gs_cairo->cairo_rectangle(m_cairo, 0, 0, w, h);   // still in cairo units
            gs_cairo->cairo_fill(m_cairo);
            gs_cairo->cairo_restore(m_cairo);
        }

    // Draw layout.
    gs_cairo->cairo_move_to (m_cairo, xx, yy);

    gs_cairo->cairo_save( m_cairo );

    if (fabs(angle) > 0.00001)
        gs_cairo->cairo_rotate( m_cairo, angle*DEG2RAD );

    gs_cairo->pango_cairo_update_layout (m_cairo, m_layout);
    gs_cairo->pango_cairo_show_layout (m_cairo, m_layout);

    gs_cairo->cairo_restore( m_cairo );

    if (underlined)
    {
        // Undo underline attributes setting
        pango_layout_set_attributes(m_layout, NULL);
    }

    // Reset unscaled size.
    pango_font_description_set_size( m_fontdesc, oldSize );

    // Actually apply unscaled font.
    pango_layout_set_font_description( m_layout, m_fontdesc );

    // Back to device units:
    CalcBoundingBox (x, y);
    CalcBoundingBox (x + w, y + h);
}

void wxGtkPrintDC::Clear()
{
// Clear does nothing for printing, but keep the code
// for later reuse 
/*
    gs_cairo->cairo_save(m_cairo);
    gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_SOURCE);
    SetBrush(m_backgroundBrush);
    gs_cairo->cairo_paint(m_cairo);
    gs_cairo->cairo_restore(m_cairo);
*/
}

void wxGtkPrintDC::SetFont( const wxFont& font )
{
    m_font = font;

    if (m_font.Ok())
    {
        if (m_fontdesc)
            pango_font_description_free( m_fontdesc );

        m_fontdesc = pango_font_description_copy( m_font.GetNativeFontInfo()->description ); // m_fontdesc is now set to device units

        // Scale font description from device units to pango units
        gint oldSize = pango_font_description_get_size( m_fontdesc );
        double size = oldSize *m_DEV2PS;                          // scale to cairo units
        pango_font_description_set_size( m_fontdesc, (gint)size );    // apply to description

        // Actually apply scaled font.
        pango_layout_set_font_description( m_layout, m_fontdesc );
    }
}

void wxGtkPrintDC::SetPen( const wxPen& pen )
{
    if (!pen.Ok()) return;

    m_pen = pen;

    double width;
    
    if (m_pen.GetWidth() <= 0)
        width = 0.1;
    else
        width = (double) m_pen.GetWidth();

    gs_cairo->cairo_set_line_width( m_cairo, width * m_DEV2PS * m_scaleX );
    static const double dotted[] = {2.0, 5.0};
    static const double short_dashed[] = {4.0, 4.0};
    static const double long_dashed[] = {4.0, 8.0};
    static const double dotted_dashed[] = {6.0, 6.0, 2.0, 6.0};

    switch (m_pen.GetStyle())
    {
        case wxDOT:           gs_cairo->cairo_set_dash( m_cairo, dotted, 2, 0 ); break;
        case wxSHORT_DASH:    gs_cairo->cairo_set_dash( m_cairo, short_dashed, 2, 0 ); break;
        case wxLONG_DASH:     gs_cairo->cairo_set_dash( m_cairo, long_dashed, 2, 0 ); break;
        case wxDOT_DASH:      gs_cairo->cairo_set_dash( m_cairo, dotted_dashed, 4, 0 );  break;
        case wxUSER_DASH:
        {
            wxDash *wx_dashes;
            int num = m_pen.GetDashes (&wx_dashes);
            gdouble *g_dashes = g_new( gdouble, num );
            int i;
            for (i = 0; i < num; ++i)
                g_dashes[i] = (gdouble) wx_dashes[i];
            gs_cairo->cairo_set_dash( m_cairo, g_dashes, num, 0);
            g_free( g_dashes );
        }
        break;
        case wxSOLID:
        case wxTRANSPARENT:
        default:              gs_cairo->cairo_set_dash( m_cairo, NULL, 0, 0 );   break;
    }

    switch (m_pen.GetCap())
    {
        case wxCAP_PROJECTING:  gs_cairo->cairo_set_line_cap (m_cairo, CAIRO_LINE_CAP_SQUARE); break;
        case wxCAP_BUTT:        gs_cairo->cairo_set_line_cap (m_cairo, CAIRO_LINE_CAP_BUTT); break;
        case wxCAP_ROUND:
        default:                gs_cairo->cairo_set_line_cap (m_cairo, CAIRO_LINE_CAP_ROUND); break;
    }

    switch (m_pen.GetJoin())
    {
        case wxJOIN_BEVEL:  gs_cairo->cairo_set_line_join (m_cairo, CAIRO_LINE_JOIN_BEVEL); break;
        case wxJOIN_MITER:  gs_cairo->cairo_set_line_join (m_cairo, CAIRO_LINE_JOIN_MITER); break;
        case wxJOIN_ROUND:
        default:            gs_cairo->cairo_set_line_join (m_cairo, CAIRO_LINE_JOIN_ROUND); break;
    }

    unsigned char red = m_pen.GetColour().Red();
    unsigned char blue = m_pen.GetColour().Blue();
    unsigned char green = m_pen.GetColour().Green();
    unsigned char alpha = m_pen.GetColour().Alpha();

    if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue && alpha == m_currentAlpha))
    {
        double redPS = (double)(red) / 255.0;
        double bluePS = (double)(blue) / 255.0;
        double greenPS = (double)(green) / 255.0;
        double alphaPS = (double)(alpha) / 255.0;

        gs_cairo->cairo_set_source_rgba( m_cairo, redPS, greenPS, bluePS, alphaPS );

        m_currentRed = red;
        m_currentBlue = blue;
        m_currentGreen = green;
        m_currentAlpha = alpha;
    }
}

void wxGtkPrintDC::SetBrush( const wxBrush& brush )
{
    if (!brush.Ok()) return;

    m_brush = brush;

    if (m_brush.GetStyle() == wxTRANSPARENT)
    {
        gs_cairo->cairo_set_source_rgba( m_cairo, 0, 0, 0, 0 );
        m_currentRed = 0;
        m_currentBlue = 0;
        m_currentGreen = 0;
        m_currentAlpha = 0;
        return;
    }

    // Brush colour.
    unsigned char red = m_brush.GetColour().Red();
    unsigned char blue = m_brush.GetColour().Blue();
    unsigned char green = m_brush.GetColour().Green();
    unsigned char alpha = m_brush.GetColour().Alpha();

    double redPS = (double)(red) / 255.0;
    double bluePS = (double)(blue) / 255.0;
    double greenPS = (double)(green) / 255.0;
    double alphaPS = (double)(alpha) / 255.0;

    if (!(red == m_currentRed && green == m_currentGreen && blue == m_currentBlue && alpha == m_currentAlpha))
    {
        gs_cairo->cairo_set_source_rgba( m_cairo, redPS, greenPS, bluePS, alphaPS );

        m_currentRed = red;
        m_currentBlue = blue;
        m_currentGreen = green;
        m_currentAlpha = alpha;
    }

    if (m_brush.IsHatch())
    {
        cairo_t * cr;
        cairo_surface_t *surface;
        surface = gs_cairo->cairo_surface_create_similar(gs_cairo->cairo_get_target(m_cairo),CAIRO_CONTENT_COLOR_ALPHA,10,10);
        cr = gs_cairo->cairo_create(surface);
        gs_cairo->cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
        gs_cairo->cairo_set_line_width(cr, 1);
        gs_cairo->cairo_set_line_join(cr,CAIRO_LINE_JOIN_MITER);

        switch (m_brush.GetStyle())
        {
            case wxCROSS_HATCH:
                gs_cairo->cairo_move_to(cr, 5, 0);
                gs_cairo->cairo_line_to(cr, 5, 10);
                gs_cairo->cairo_move_to(cr, 0, 5);
                gs_cairo->cairo_line_to(cr, 10, 5);
                break;
            case wxBDIAGONAL_HATCH:
                gs_cairo->cairo_move_to(cr, 0, 10);
                gs_cairo->cairo_line_to(cr, 10, 0);
                break;
            case wxFDIAGONAL_HATCH:
                gs_cairo->cairo_move_to(cr, 0, 0);
                gs_cairo->cairo_line_to(cr, 10, 10);
                break;
            case wxCROSSDIAG_HATCH:
                gs_cairo->cairo_move_to(cr, 0, 0);
                gs_cairo->cairo_line_to(cr, 10, 10);
                gs_cairo->cairo_move_to(cr, 10, 0);
                gs_cairo->cairo_line_to(cr, 0, 10);
                break;
            case wxHORIZONTAL_HATCH:
                gs_cairo->cairo_move_to(cr, 0, 5);
                gs_cairo->cairo_line_to(cr, 10, 5);
                break;
            case wxVERTICAL_HATCH:
                gs_cairo->cairo_move_to(cr, 5, 0);
                gs_cairo->cairo_line_to(cr, 5, 10);
                break;
            default:
                wxFAIL_MSG(_("Couldn't get hatch style from wxBrush."));
        }

        gs_cairo->cairo_set_source_rgba(cr, redPS, greenPS, bluePS, alphaPS);
        gs_cairo->cairo_stroke (cr);

        gs_cairo->cairo_destroy(cr);
        cairo_pattern_t * pattern = gs_cairo->cairo_pattern_create_for_surface (surface);
        gs_cairo->cairo_surface_destroy(surface);
        gs_cairo->cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
        gs_cairo->cairo_set_source(m_cairo, pattern);
        gs_cairo->cairo_pattern_destroy(pattern);
    }
}

void wxGtkPrintDC::SetLogicalFunction( int function )
{
    if (function == wxCLEAR)
        gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_CLEAR);
    else if (function == wxOR)
        gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_OUT);
    else if (function == wxNO_OP)
        gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_DEST);
    else if (function == wxAND)
        gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_ADD);
    else if (function == wxSET)
        gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_SATURATE);
    else if (function == wxXOR)
        gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_XOR);
    else // wxCOPY or anything else.
        gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_SOURCE);
}

void wxGtkPrintDC::SetBackground( const wxBrush& brush )
{
    m_backgroundBrush = brush;
    gs_cairo->cairo_save(m_cairo);
    gs_cairo->cairo_set_operator (m_cairo, CAIRO_OPERATOR_DEST_OVER);

    SetBrush(m_backgroundBrush);
    gs_cairo->cairo_paint(m_cairo);
    gs_cairo->cairo_restore(m_cairo);
}

void wxGtkPrintDC::SetBackgroundMode(int mode)
{
    if (mode == wxSOLID) m_backgroundMode = wxSOLID;
    else m_backgroundMode = wxTRANSPARENT;
}

void wxGtkPrintDC::DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height)
{
    gs_cairo->cairo_rectangle ( m_cairo, XLOG2DEV(x), YLOG2DEV(y), XLOG2DEVREL(width), YLOG2DEVREL(height));
    gs_cairo->cairo_clip(m_cairo);
}

void wxGtkPrintDC::DestroyClippingRegion()
{
    gs_cairo->cairo_reset_clip(m_cairo);
}

bool wxGtkPrintDC::StartDoc(const wxString& message)
{
    return true;
}

void wxGtkPrintDC::EndDoc()
{
    return;
}

void wxGtkPrintDC::StartPage()
{
    return;
}

void wxGtkPrintDC::EndPage()
{
    return;
}

wxCoord wxGtkPrintDC::GetCharHeight() const
{
    pango_layout_set_text( m_layout, "H", 1 );

    int w,h;
    pango_layout_get_pixel_size( m_layout, &w, &h );

    return wxRound( h * m_PS2DEV );
}

wxCoord wxGtkPrintDC::GetCharWidth() const
{
    pango_layout_set_text( m_layout, "H", 1 );

    int w,h;
    pango_layout_get_pixel_size( m_layout, &w, &h );

    return wxRound( w * m_PS2DEV );
}

void wxGtkPrintDC::DoGetTextExtent(const wxString& string, wxCoord *width, wxCoord *height,
                     wxCoord *descent,
                     wxCoord *externalLeading,
                     const wxFont *theFont ) const
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

    // Set layout's text
    // FIXME-UTF8: wouldn't be needed if utf8_str() always returned a buffer
#if wxUSE_UNICODE_UTF8
    const char *dataUTF8 = string.utf8_str();
#else
    const wxCharBuffer dataUTF8 = string.utf8_str();
#endif

    PangoFontDescription *desc = m_fontdesc;
    if (theFont) desc = theFont->GetNativeFontInfo()->description;

    gint oldSize = pango_font_description_get_size( desc );
    double size = oldSize;
    size = size * m_scaleY;
    pango_font_description_set_size( desc, (gint)size );

    // apply scaled font
    pango_layout_set_font_description( m_layout, desc );

    pango_layout_set_text( m_layout, dataUTF8, strlen(dataUTF8) );

    int w, h;
    pango_layout_get_pixel_size( m_layout, &w, &h );

    if (width)
        *width = wxRound( (double)w / m_scaleX * m_PS2DEV );
    if (height)
        *height = wxRound( (double)h / m_scaleY * m_PS2DEV );

    if (descent)
    {
        PangoLayoutIter *iter = pango_layout_get_iter(m_layout);
        int baseline = pango_layout_iter_get_baseline(iter);
        pango_layout_iter_free(iter);
        *descent = wxRound( (h - PANGO_PIXELS(baseline)) * m_PS2DEV );
    }

    // Reset unscaled size.
    pango_font_description_set_size( desc, oldSize );

    // Reset unscaled font.
    pango_layout_set_font_description( m_layout, m_fontdesc );
}

void wxGtkPrintDC::DoGetSize(int* width, int* height) const
{
    GtkPageSetup *setup = gtk_print_context_get_page_setup( m_gpc );

    if (width)
        *width = wxRound( gtk_page_setup_get_paper_width( setup, GTK_UNIT_POINTS ) * m_PS2DEV );
    if (height)
        *height = wxRound( gtk_page_setup_get_paper_height( setup, GTK_UNIT_POINTS ) * m_PS2DEV );
}

void wxGtkPrintDC::DoGetSizeMM(int *width, int *height) const
{
    GtkPageSetup *setup = gtk_print_context_get_page_setup( m_gpc );

    if (width)
        *width = wxRound( gtk_page_setup_get_paper_width( setup, GTK_UNIT_MM ) );
    if (height)
        *height = wxRound( gtk_page_setup_get_paper_height( setup, GTK_UNIT_MM ) );
}

wxSize wxGtkPrintDC::GetPPI() const
{
    return wxSize( (int)m_resolution, (int)m_resolution );
}

void wxGtkPrintDC::SetPrintData(const wxPrintData& data)
{
    m_printData = data;
}

void wxGtkPrintDC::SetResolution(int ppi)
{
    // We can't change ppi of the GtkPrintContext.
    // TODO: should we really support this?
}

int wxGtkPrintDC::GetResolution()
{
    return m_resolution;
}

// ----------------------------------------------------------------------------
// Print preview
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGtkPrintPreview, wxPrintPreviewBase)

void wxGtkPrintPreview::Init(wxPrintout * WXUNUSED(printout),
                                    wxPrintout * WXUNUSED(printoutForPrinting))
{
    DetermineScaling();
}

wxGtkPrintPreview::wxGtkPrintPreview(wxPrintout *printout,
                                                   wxPrintout *printoutForPrinting,
                                                   wxPrintDialogData *data)
                        : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    Init(printout, printoutForPrinting);
}

wxGtkPrintPreview::wxGtkPrintPreview(wxPrintout *printout,
                                                   wxPrintout *printoutForPrinting,
                                                   wxPrintData *data)
                        : wxPrintPreviewBase(printout, printoutForPrinting, data)
{
    Init(printout, printoutForPrinting);
}

wxGtkPrintPreview::~wxGtkPrintPreview()
{
}

bool wxGtkPrintPreview::Print(bool interactive)
{
    if (!m_printPrintout)
        return false;

    wxPrinter printer(& m_printDialogData);
    return printer.Print(m_previewFrame, m_printPrintout, interactive);
}

void wxGtkPrintPreview::DetermineScaling()
{
    wxPaperSize paperType = m_printDialogData.GetPrintData().GetPaperId();

    wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(paperType);
    if (!paper)
        paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);

    if (paper)
    {
        wxSize ScreenPixels = wxGetDisplaySize();
        wxSize ScreenMM = wxGetDisplaySizeMM();

        m_previewPrintout->SetPPIScreen( (int) ((ScreenPixels.GetWidth() * 25.4) / ScreenMM.GetWidth()),
                                         (int) ((ScreenPixels.GetHeight() * 25.4) / ScreenMM.GetHeight()) );

        // TODO !!!!!!!!!!!!!!!      
        int resolution = 600;
        m_previewPrintout->SetPPIPrinter( resolution, resolution );

        // Get width and height in points (1/72th of an inch)
        wxSize sizeDevUnits(paper->GetSizeDeviceUnits());

        sizeDevUnits.x = wxRound((double)sizeDevUnits.x * (double)resolution / 72.0);
        sizeDevUnits.y = wxRound((double)sizeDevUnits.y * (double)resolution / 72.0);
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
        m_previewScaleX = 0.8 * 72.0 / (double)resolution;
        m_previewScaleY = m_previewScaleX;
    }
}

#endif
     // wxUSE_GTKPRINT
