/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        statbmp.cpp
// Purpose:     Part of the widgets sample showing wxStaticBitmap
// Author:      Marcin Wojdyr
// Created:     2008-06-19
// Copyright:   (c) 2008 Marcin Wojdyr
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/button.h"
    #include "wx/image.h"
    #include "wx/radiobox.h"
    #include "wx/statbmp.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#include "wx/artprov.h"
#include "wx/filename.h"

#include "wx/generic/statbmpg.h"
#include "wx/sizer.h"
#include "wx/filepicker.h"

#include "widgets.h"
#include "icons/statbmp.xpm"


class StatBmpWidgetsPage : public WidgetsPage
{
public:
    StatBmpWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist)
        : WidgetsPage(book, imaglist, statbmp_xpm) {}

    virtual void CreateContent() override;
    virtual wxWindow *GetWidget() const override { return m_statbmp; }
    virtual void RecreateWidget() override;

private:
    void OnFileChange(wxFileDirPickerEvent &WXUNUSED(ev)) { RecreateWidget(); }
    void OnRadioChange(wxCommandEvent &WXUNUSED(ev)) { RecreateWidget(); }

    void OnMouseEvent(wxMouseEvent& WXUNUSED(event))
    {
        wxLogMessage("wxStaticBitmap clicked.");
    }

    wxStaticBitmapBase *m_statbmp;
    wxFilePickerCtrl *m_filepicker;
    wxRadioBox *m_radio;
    wxRadioBox *m_scaleRadio;
    wxStaticBoxSizer *m_sbsizer;

    DECLARE_WIDGETS_PAGE(StatBmpWidgetsPage)
};

IMPLEMENT_WIDGETS_PAGE(StatBmpWidgetsPage, "StaticBitmap",
                       ALL_CTRLS);

void StatBmpWidgetsPage::CreateContent()
{

    static const wxString choices[] = { "native", "generic" };
    m_radio = new wxRadioBox(this, wxID_ANY, "Implementation",
                             wxDefaultPosition, wxDefaultSize,
                             WXSIZEOF(choices), choices);
    static const wxString scaleChoices[] = { "None", "Fill", "Aspect Fit", "Aspect Fill" };
    m_scaleRadio = new wxRadioBox(this, wxID_ANY, "Scale Mode",
                                  wxDefaultPosition, wxDefaultSize,
                                  WXSIZEOF(scaleChoices), scaleChoices);

    wxString testImage;
#if wxUSE_LIBPNG
    wxPathList pathlist;
    pathlist.Add(".");
    pathlist.Add("..");
    pathlist.Add("../image");
    pathlist.Add("../../../samples/image");

    wxFileName fn(pathlist.FindValidPath("toucan.png"));
    if ( fn.FileExists() )
        testImage = fn.GetFullPath();
#endif // wxUSE_LIBPNG
    m_filepicker = new wxFilePickerCtrl(this, wxID_ANY, testImage);

    m_sbsizer = new wxStaticBoxSizer(wxVERTICAL, this, "wxStaticBitmap inside");

    wxSizer *leftsizer = new wxBoxSizer(wxVERTICAL);
    leftsizer->Add(m_radio, wxSizerFlags().Expand().Border());
    leftsizer->Add(m_scaleRadio, wxSizerFlags().Expand().Border());
    leftsizer->Add(m_filepicker, wxSizerFlags().Expand().Border());
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(leftsizer, wxSizerFlags().Border());
    sizer->Add(m_sbsizer, wxSizerFlags().Center());
    SetSizer(sizer);

    wxInitAllImageHandlers();

    Bind(wxEVT_FILEPICKER_CHANGED, &StatBmpWidgetsPage::OnFileChange, this);
    Bind(wxEVT_RADIOBOX, &StatBmpWidgetsPage::OnRadioChange, this);

    m_statbmp = nullptr;
    RecreateWidget();
}

void StatBmpWidgetsPage::RecreateWidget()
{
    wxDELETE(m_statbmp);

    wxBitmap bmp;

    wxString filepath = m_filepicker->GetPath();
    if ( !filepath.empty() )
    {
        wxImage image(filepath);
        if ( image.IsOk() )
        {
            bmp = image;
        }
        else
        {
            wxLogMessage("Reading image from file '%s' failed.", filepath);
        }
    }

    if ( !bmp.IsOk() )
    {
        // Show at least something.
        bmp = wxArtProvider::GetBitmap(wxART_MISSING_IMAGE);
    }

    long style = GetAttrs().m_defaultFlags;

    if (m_radio->GetSelection() == 0)
    {
        m_statbmp = new wxStaticBitmap(m_sbsizer->GetStaticBox(), wxID_ANY, bmp,
                                       wxDefaultPosition, wxDefaultSize,
                                       style);
    }
    else
    {
        m_statbmp = new wxGenericStaticBitmap(m_sbsizer->GetStaticBox(), wxID_ANY, bmp,
                                              wxDefaultPosition, wxDefaultSize,
                                              style);
    }

    NotifyWidgetRecreation(m_statbmp);

    wxStaticBitmapBase::ScaleMode scaleMode = (wxStaticBitmapBase::ScaleMode) m_scaleRadio->GetSelection();
    m_statbmp->SetScaleMode(scaleMode);
    if ( m_statbmp->GetScaleMode() != scaleMode )
        wxLogError("Scale mode not supported by current implementation");
    wxSizerItem* sbsizerItem = GetSizer()->GetItem(m_sbsizer);
    if ( scaleMode == wxStaticBitmapBase::Scale_None )
    {
        sbsizerItem->SetProportion(0);
        sbsizerItem->SetFlag(wxCENTER);
    }
    else
    {
        sbsizerItem->SetProportion(1);
        sbsizerItem->SetFlag(wxEXPAND);
    }
    m_sbsizer->Add(m_statbmp, wxSizerFlags(1).Expand());
    GetSizer()->Layout();
    m_statbmp->Bind(wxEVT_LEFT_DOWN, &StatBmpWidgetsPage::OnMouseEvent, this);

    // When switching from generic to native control on wxMSW under Wine,
    // the explicit Refresh() is necessary
    m_statbmp->Refresh();
}

