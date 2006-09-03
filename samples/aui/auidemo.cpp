///////////////////////////////////////////////////////////////////////////////
// Name:        auidemo.cpp
// Purpose:     wxaui: wx advanced user interface - sample/test program
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-10-03
// RCS-ID:      $Id$
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wx.h"

#include "wx/grid.h"
#include "wx/treectrl.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/clipbrd.h"
#include "wx/image.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/imaglist.h"
#include "wx/dataobj.h"

#include "wx/aui/aui.h"
#include "../sample.xpm"

// -- application --

class MyApp : public wxApp
{
public:
    bool OnInit();
};

DECLARE_APP(MyApp)
IMPLEMENT_APP(MyApp)


class wxSizeReportCtrl;

// -- frame --

class MyFrame : public wxFrame
{
    enum
    {
        ID_CreateTree = wxID_HIGHEST+1,
        ID_CreateGrid,
        ID_CreateText,
        ID_CreateHTML,
        ID_CreateSizeReport,
        ID_GridContent,
        ID_TextContent,
        ID_TreeContent,
        ID_HTMLContent,
        ID_NotebookContent,
        ID_SizeReportContent,
        ID_CreatePerspective,
        ID_CopyPerspectiveCode,
        ID_AllowFloating,
        ID_AllowActivePane,
        ID_TransparentHint,
        ID_TransparentHintFade,
        ID_DisableVenetian,
        ID_DisableVenetianFade,
        ID_TransparentDrag,
        ID_NoGradient,
        ID_VerticalGradient,
        ID_HorizontalGradient,
        ID_Settings,
        ID_FirstPerspective = ID_CreatePerspective+1000
    };

public:
    MyFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

    ~MyFrame();

    wxDockArt* GetDockArt();
    void DoUpdate();

private:
    wxTextCtrl* CreateTextCtrl(const wxString& text = wxEmptyString);
    wxGrid* CreateGrid();
    wxTreeCtrl* CreateTreeCtrl();
    wxSizeReportCtrl* CreateSizeReportCtrl(int width = 80, int height = 80);
    wxPoint GetStartPosition();
    wxHtmlWindow* CreateHTMLCtrl();
    wxAuiMultiNotebook* CreateNotebook();

    wxString GetIntroText();

private:

    void OnEraseBackground(wxEraseEvent& evt);
    void OnSize(wxSizeEvent& evt);

    void OnCreateTree(wxCommandEvent& evt);
    void OnCreateGrid(wxCommandEvent& evt);
    void OnCreateHTML(wxCommandEvent& evt);
    void OnCreateNotebook(wxCommandEvent& evt);
    void OnCreateText(wxCommandEvent& evt);
    void OnCreateSizeReport(wxCommandEvent& evt);
    void OnChangeContentPane(wxCommandEvent& evt);
    void OnCreatePerspective(wxCommandEvent& evt);
    void OnCopyPerspectiveCode(wxCommandEvent& evt);
    void OnRestorePerspective(wxCommandEvent& evt);
    void OnSettings(wxCommandEvent& evt);
    void OnExit(wxCommandEvent& evt);
    void OnAbout(wxCommandEvent& evt);

    void OnGradient(wxCommandEvent& evt);
    void OnManagerFlag(wxCommandEvent& evt);
    void OnUpdateUI(wxUpdateUIEvent& evt);

    void OnPaneClose(wxFrameManagerEvent& evt);
    
private:

    wxFrameManager m_mgr;
    wxArrayString m_perspectives;
    wxMenu* m_perspectives_menu;

    DECLARE_EVENT_TABLE()
};



// -- wxSizeReportCtrl --
// (a utility control that always reports it's client size)

class wxSizeReportCtrl : public wxControl
{
public:

    wxSizeReportCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     wxFrameManager* mgr = NULL)
                     : wxControl(parent, id, pos, size, wxNO_BORDER)
    {
        m_mgr = mgr;
    }

private:

    void OnPaint(wxPaintEvent& WXUNUSED(evt))
    {
        wxPaintDC dc(this);
        wxSize size = GetClientSize();
        wxString s;
        int h, w, height;

        s.Printf(wxT("Size: %d x %d"), size.x, size.y);

        dc.SetFont(*wxNORMAL_FONT);
        dc.GetTextExtent(s, &w, &height);
        height += 3;
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(0, 0, size.x, size.y);
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine(0, 0, size.x, size.y);
        dc.DrawLine(0, size.y, size.x, 0);
        dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2));

        if (m_mgr)
        {
            wxPaneInfo pi = m_mgr->GetPane(this);

            s.Printf(wxT("Layer: %d"), pi.dock_layer);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*1));

            s.Printf(wxT("Dock: %d Row: %d"), pi.dock_direction, pi.dock_row);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*2));

            s.Printf(wxT("Position: %d"), pi.dock_pos);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*3));

            s.Printf(wxT("Proportion: %d"), pi.dock_proportion);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*4));
        }
    }

    void OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
    {
        // intentionally empty
    }

    void OnSize(wxSizeEvent& WXUNUSED(evt))
    {
        Refresh();
    }
private:

    wxFrameManager* m_mgr;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxSizeReportCtrl, wxControl)
    EVT_PAINT(wxSizeReportCtrl::OnPaint)
    EVT_SIZE(wxSizeReportCtrl::OnSize)
    EVT_ERASE_BACKGROUND(wxSizeReportCtrl::OnEraseBackground)
END_EVENT_TABLE()






class SettingsPanel : public wxPanel
{
    enum
    {
        ID_PaneBorderSize = wxID_HIGHEST+1,
        ID_SashSize,
        ID_CaptionSize,
        ID_BackgroundColor,
        ID_SashColor,
        ID_InactiveCaptionColor,
        ID_InactiveCaptionGradientColor,
        ID_InactiveCaptionTextColor,
        ID_ActiveCaptionColor,
        ID_ActiveCaptionGradientColor,
        ID_ActiveCaptionTextColor,
        ID_BorderColor,
        ID_GripperColor
    };

public:

    SettingsPanel(wxWindow* parent, MyFrame* frame)
            : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
    {
        //wxBoxSizer* vert = new wxBoxSizer(wxVERTICAL);

        //vert->Add(1, 1, 1, wxEXPAND);

        wxBoxSizer* s1 = new wxBoxSizer(wxHORIZONTAL);
        m_border_size = new wxSpinCtrl(this, ID_PaneBorderSize, wxEmptyString, wxDefaultPosition, wxSize(50,20));
        s1->Add(1, 1, 1, wxEXPAND);
        s1->Add(new wxStaticText(this, wxID_ANY, wxT("Pane Border Size:")));
        s1->Add(m_border_size);
        s1->Add(1, 1, 1, wxEXPAND);
        s1->SetItemMinSize((size_t)1, 180, 20);
        //vert->Add(s1, 0, wxEXPAND | wxLEFT | wxBOTTOM, 5);

        wxBoxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
        m_sash_size = new wxSpinCtrl(this, ID_SashSize, wxEmptyString, wxDefaultPosition, wxSize(50,20));
        s2->Add(1, 1, 1, wxEXPAND);
        s2->Add(new wxStaticText(this, wxID_ANY, wxT("Sash Size:")));
        s2->Add(m_sash_size);
        s2->Add(1, 1, 1, wxEXPAND);
        s2->SetItemMinSize((size_t)1, 180, 20);
        //vert->Add(s2, 0, wxEXPAND | wxLEFT | wxBOTTOM, 5);

        wxBoxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
        m_caption_size = new wxSpinCtrl(this, ID_CaptionSize, wxEmptyString, wxDefaultPosition, wxSize(50,20));
        s3->Add(1, 1, 1, wxEXPAND);
        s3->Add(new wxStaticText(this, wxID_ANY, wxT("Caption Size:")));
        s3->Add(m_caption_size);
        s3->Add(1, 1, 1, wxEXPAND);
        s3->SetItemMinSize((size_t)1, 180, 20);
        //vert->Add(s3, 0, wxEXPAND | wxLEFT | wxBOTTOM, 5);

        //vert->Add(1, 1, 1, wxEXPAND);


        wxBitmap b = CreateColorBitmap(*wxBLACK);

        wxBoxSizer* s4 = new wxBoxSizer(wxHORIZONTAL);
        m_background_color = new wxBitmapButton(this, ID_BackgroundColor, b, wxDefaultPosition, wxSize(50,25));
        s4->Add(1, 1, 1, wxEXPAND);
        s4->Add(new wxStaticText(this, wxID_ANY, wxT("Background Color:")));
        s4->Add(m_background_color);
        s4->Add(1, 1, 1, wxEXPAND);
        s4->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s5 = new wxBoxSizer(wxHORIZONTAL);
        m_sash_color = new wxBitmapButton(this, ID_SashColor, b, wxDefaultPosition, wxSize(50,25));
        s5->Add(1, 1, 1, wxEXPAND);
        s5->Add(new wxStaticText(this, wxID_ANY, wxT("Sash Color:")));
        s5->Add(m_sash_color);
        s5->Add(1, 1, 1, wxEXPAND);
        s5->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s6 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_color = new wxBitmapButton(this, ID_InactiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
        s6->Add(1, 1, 1, wxEXPAND);
        s6->Add(new wxStaticText(this, wxID_ANY, wxT("Normal Caption:")));
        s6->Add(m_inactive_caption_color);
        s6->Add(1, 1, 1, wxEXPAND);
        s6->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s7 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_gradient_color = new wxBitmapButton(this, ID_InactiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
        s7->Add(1, 1, 1, wxEXPAND);
        s7->Add(new wxStaticText(this, wxID_ANY, wxT("Normal Caption Gradient:")));
        s7->Add(m_inactive_caption_gradient_color);
        s7->Add(1, 1, 1, wxEXPAND);
        s7->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s8 = new wxBoxSizer(wxHORIZONTAL);
        m_inactive_caption_text_color = new wxBitmapButton(this, ID_InactiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
        s8->Add(1, 1, 1, wxEXPAND);
        s8->Add(new wxStaticText(this, wxID_ANY, wxT("Normal Caption Text:")));
        s8->Add(m_inactive_caption_text_color);
        s8->Add(1, 1, 1, wxEXPAND);
        s8->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s9 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_color = new wxBitmapButton(this, ID_ActiveCaptionColor, b, wxDefaultPosition, wxSize(50,25));
        s9->Add(1, 1, 1, wxEXPAND);
        s9->Add(new wxStaticText(this, wxID_ANY, wxT("Active Caption:")));
        s9->Add(m_active_caption_color);
        s9->Add(1, 1, 1, wxEXPAND);
        s9->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s10 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_gradient_color = new wxBitmapButton(this, ID_ActiveCaptionGradientColor, b, wxDefaultPosition, wxSize(50,25));
        s10->Add(1, 1, 1, wxEXPAND);
        s10->Add(new wxStaticText(this, wxID_ANY, wxT("Active Caption Gradient:")));
        s10->Add(m_active_caption_gradient_color);
        s10->Add(1, 1, 1, wxEXPAND);
        s10->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s11 = new wxBoxSizer(wxHORIZONTAL);
        m_active_caption_text_color = new wxBitmapButton(this, ID_ActiveCaptionTextColor, b, wxDefaultPosition, wxSize(50,25));
        s11->Add(1, 1, 1, wxEXPAND);
        s11->Add(new wxStaticText(this, wxID_ANY, wxT("Active Caption Text:")));
        s11->Add(m_active_caption_text_color);
        s11->Add(1, 1, 1, wxEXPAND);
        s11->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s12 = new wxBoxSizer(wxHORIZONTAL);
        m_border_color = new wxBitmapButton(this, ID_BorderColor, b, wxDefaultPosition, wxSize(50,25));
        s12->Add(1, 1, 1, wxEXPAND);
        s12->Add(new wxStaticText(this, wxID_ANY, wxT("Border Color:")));
        s12->Add(m_border_color);
        s12->Add(1, 1, 1, wxEXPAND);
        s12->SetItemMinSize((size_t)1, 180, 20);

        wxBoxSizer* s13 = new wxBoxSizer(wxHORIZONTAL);
        m_gripper_color = new wxBitmapButton(this, ID_GripperColor, b, wxDefaultPosition, wxSize(50,25));
        s13->Add(1, 1, 1, wxEXPAND);
        s13->Add(new wxStaticText(this, wxID_ANY, wxT("Gripper Color:")));
        s13->Add(m_gripper_color);
        s13->Add(1, 1, 1, wxEXPAND);
        s13->SetItemMinSize((size_t)1, 180, 20);

        wxGridSizer* grid_sizer = new wxGridSizer(2);
        grid_sizer->SetHGap(5);
        grid_sizer->Add(s1);  grid_sizer->Add(s4);
        grid_sizer->Add(s2);  grid_sizer->Add(s5);
        grid_sizer->Add(s3);  grid_sizer->Add(s13);
        grid_sizer->Add(1,1); grid_sizer->Add(s12);
        grid_sizer->Add(s6);  grid_sizer->Add(s9);
        grid_sizer->Add(s7);  grid_sizer->Add(s10);
        grid_sizer->Add(s8);  grid_sizer->Add(s11);

        wxBoxSizer* cont_sizer = new wxBoxSizer(wxVERTICAL);
        cont_sizer->Add(grid_sizer, 1, wxEXPAND | wxALL, 5);
        SetSizer(cont_sizer);
        GetSizer()->SetSizeHints(this);

        m_frame = frame;
        m_border_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_ART_PANE_BORDER_SIZE));
        m_sash_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_ART_SASH_SIZE));
        m_caption_size->SetValue(frame->GetDockArt()->GetMetric(wxAUI_ART_CAPTION_SIZE));

        UpdateColors();
    }

private:

    wxBitmap CreateColorBitmap(const wxColour& c)
    {
        wxImage image;
        image.Create(25,14);
        for (int x = 0; x < 25; ++x)
            for (int y = 0; y < 14; ++y)
            {
                wxColour pixcol = c;
                if (x == 0 || x == 24 || y == 0 || y == 13)
                    pixcol = *wxBLACK;
                image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
            }
        return wxBitmap(image);
    }

    void UpdateColors()
    {
        wxColour bk = m_frame->GetDockArt()->GetColor(wxAUI_ART_BACKGROUND_COLOUR);
        m_background_color->SetBitmapLabel(CreateColorBitmap(bk));

        wxColour cap = m_frame->GetDockArt()->GetColor(wxAUI_ART_INACTIVE_CAPTION_COLOUR);
        m_inactive_caption_color->SetBitmapLabel(CreateColorBitmap(cap));

        wxColour capgrad = m_frame->GetDockArt()->GetColor(wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR);
        m_inactive_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(capgrad));

        wxColour captxt = m_frame->GetDockArt()->GetColor(wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR);
        m_inactive_caption_text_color->SetBitmapLabel(CreateColorBitmap(captxt));

        wxColour acap = m_frame->GetDockArt()->GetColor(wxAUI_ART_ACTIVE_CAPTION_COLOUR);
        m_active_caption_color->SetBitmapLabel(CreateColorBitmap(acap));

        wxColour acapgrad = m_frame->GetDockArt()->GetColor(wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR);
        m_active_caption_gradient_color->SetBitmapLabel(CreateColorBitmap(acapgrad));

        wxColour acaptxt = m_frame->GetDockArt()->GetColor(wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR);
        m_active_caption_text_color->SetBitmapLabel(CreateColorBitmap(acaptxt));

        wxColour sash = m_frame->GetDockArt()->GetColor(wxAUI_ART_SASH_COLOUR);
        m_sash_color->SetBitmapLabel(CreateColorBitmap(sash));

        wxColour border = m_frame->GetDockArt()->GetColor(wxAUI_ART_BORDER_COLOUR);
        m_border_color->SetBitmapLabel(CreateColorBitmap(border));

        wxColour gripper = m_frame->GetDockArt()->GetColor(wxAUI_ART_GRIPPER_COLOUR);
        m_gripper_color->SetBitmapLabel(CreateColorBitmap(gripper));
    }

    void OnPaneBorderSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_ART_PANE_BORDER_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnSashSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_ART_SASH_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnCaptionSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_ART_CAPTION_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnSetColor(wxCommandEvent& event)
    {
        wxColourDialog dlg(m_frame);
        dlg.SetTitle(_("Color Picker"));
        if (dlg.ShowModal() != wxID_OK)
            return;

        int var = 0;
        switch (event.GetId())
        {
            case ID_BackgroundColor:              var = wxAUI_ART_BACKGROUND_COLOUR; break;
            case ID_SashColor:                    var = wxAUI_ART_SASH_COLOUR; break;
            case ID_InactiveCaptionColor:         var = wxAUI_ART_INACTIVE_CAPTION_COLOUR; break;
            case ID_InactiveCaptionGradientColor: var = wxAUI_ART_INACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_InactiveCaptionTextColor:     var = wxAUI_ART_INACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_ActiveCaptionColor:           var = wxAUI_ART_ACTIVE_CAPTION_COLOUR; break;
            case ID_ActiveCaptionGradientColor:   var = wxAUI_ART_ACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_ActiveCaptionTextColor:       var = wxAUI_ART_ACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_BorderColor:                  var = wxAUI_ART_BORDER_COLOUR; break;
            case ID_GripperColor:                 var = wxAUI_ART_GRIPPER_COLOUR; break;
            default: return;
        }

        m_frame->GetDockArt()->SetColor(var, dlg.GetColourData().GetColour());
        m_frame->DoUpdate();
        UpdateColors();
    }

private:

    MyFrame* m_frame;
    wxSpinCtrl* m_border_size;
    wxSpinCtrl* m_sash_size;
    wxSpinCtrl* m_caption_size;
    wxBitmapButton* m_inactive_caption_text_color;
    wxBitmapButton* m_inactive_caption_gradient_color;
    wxBitmapButton* m_inactive_caption_color;
    wxBitmapButton* m_active_caption_text_color;
    wxBitmapButton* m_active_caption_gradient_color;
    wxBitmapButton* m_active_caption_color;
    wxBitmapButton* m_sash_color;
    wxBitmapButton* m_background_color;
    wxBitmapButton* m_border_color;
    wxBitmapButton* m_gripper_color;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(SettingsPanel, wxPanel)
    EVT_SPINCTRL(ID_PaneBorderSize, SettingsPanel::OnPaneBorderSize)
    EVT_SPINCTRL(ID_SashSize, SettingsPanel::OnSashSize)
    EVT_SPINCTRL(ID_CaptionSize, SettingsPanel::OnCaptionSize)
    EVT_BUTTON(ID_BackgroundColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_SashColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionGradientColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_InactiveCaptionTextColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionGradientColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_ActiveCaptionTextColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_BorderColor, SettingsPanel::OnSetColor)
    EVT_BUTTON(ID_GripperColor, SettingsPanel::OnSetColor)
END_EVENT_TABLE()







bool MyApp::OnInit()
{
    wxFrame* frame = new MyFrame(NULL,
                                 wxID_ANY,
                                 wxT("wxAUI Sample Application"),
                                 wxDefaultPosition,
                                 wxSize(800, 600));
    SetTopWindow(frame);
    frame->Show();

    return true;
}



BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(MyFrame::ID_CreateTree, MyFrame::OnCreateTree)
    EVT_MENU(MyFrame::ID_CreateGrid, MyFrame::OnCreateGrid)
    EVT_MENU(MyFrame::ID_CreateText, MyFrame::OnCreateText)
    EVT_MENU(MyFrame::ID_CreateHTML, MyFrame::OnCreateHTML)
    EVT_MENU(MyFrame::ID_CreateSizeReport, MyFrame::OnCreateSizeReport)
    EVT_MENU(MyFrame::ID_CreatePerspective, MyFrame::OnCreatePerspective)
    EVT_MENU(MyFrame::ID_CopyPerspectiveCode, MyFrame::OnCopyPerspectiveCode)
    EVT_MENU(ID_AllowFloating, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentHintFade, MyFrame::OnManagerFlag)
    EVT_MENU(ID_DisableVenetian, MyFrame::OnManagerFlag)
    EVT_MENU(ID_DisableVenetianFade, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentDrag, MyFrame::OnManagerFlag)
    EVT_MENU(ID_AllowActivePane, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NoGradient, MyFrame::OnGradient)
    EVT_MENU(ID_VerticalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_HorizontalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_Settings, MyFrame::OnSettings)
    EVT_MENU(ID_GridContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TreeContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TextContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_SizeReportContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_HTMLContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_NotebookContent, MyFrame::OnChangeContentPane)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_UPDATE_UI(ID_AllowFloating, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentHintFade, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_DisableVenetian, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_DisableVenetianFade, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentDrag, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VerticalGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HorizontalGradient, MyFrame::OnUpdateUI)
    EVT_MENU_RANGE(MyFrame::ID_FirstPerspective, MyFrame::ID_FirstPerspective+1000,
                   MyFrame::OnRestorePerspective)
    EVT_AUI_PANECLOSE(MyFrame::OnPaneClose)
END_EVENT_TABLE()



MyFrame::MyFrame(wxWindow* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxFrame(parent, id, title, pos, size, style)
{
    // tell wxFrameManager to manage this frame
    m_mgr.SetManagedWindow(this);

    // set frame icon
    SetIcon(wxIcon(sample_xpm));

    // create menu
    wxMenuBar* mb = new wxMenuBar;

    wxMenu* file_menu = new wxMenu;
    file_menu->Append(wxID_EXIT, _("Exit"));

    wxMenu* view_menu = new wxMenu;
    view_menu->Append(ID_CreateText, _("Create Text Control"));
    view_menu->Append(ID_CreateHTML, _("Create HTML Control"));
    view_menu->Append(ID_CreateTree, _("Create Tree"));
    view_menu->Append(ID_CreateGrid, _("Create Grid"));
    view_menu->Append(ID_CreateSizeReport, _("Create Size Reporter"));
    view_menu->AppendSeparator();
    view_menu->Append(ID_GridContent, _("Use a Grid for the Content Pane"));
    view_menu->Append(ID_TextContent, _("Use a Text Control for the Content Pane"));
    view_menu->Append(ID_HTMLContent, _("Use an HTML Control for the Content Pane"));
    view_menu->Append(ID_TreeContent, _("Use a Tree Control for the Content Pane"));
    view_menu->Append(ID_NotebookContent, _("Use a AUI wxMultiNotebook control for the Content Pane"));
    view_menu->Append(ID_SizeReportContent, _("Use a Size Reporter for the Content Pane"));

    wxMenu* options_menu = new wxMenu;
    options_menu->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
    options_menu->AppendCheckItem(ID_TransparentHint, _("Transparent Hint"));
    options_menu->AppendCheckItem(ID_TransparentHintFade, _("Transparent Hint Fade-in"));
    options_menu->AppendCheckItem(ID_DisableVenetian, _("Disable Venetian Blinds Hint"));
    options_menu->AppendCheckItem(ID_DisableVenetianFade, _("Disable Venetian Blinds Hint Fade-in"));
    options_menu->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
    options_menu->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
    options_menu->AppendSeparator();
    options_menu->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
    options_menu->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
    options_menu->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
    options_menu->AppendSeparator();
    options_menu->Append(ID_Settings, _("Settings Pane"));

    m_perspectives_menu = new wxMenu;
    m_perspectives_menu->Append(ID_CreatePerspective, _("Create Perspective"));
    m_perspectives_menu->Append(ID_CopyPerspectiveCode, _("Copy Perspective Data To Clipboard"));
    m_perspectives_menu->AppendSeparator();
    m_perspectives_menu->Append(ID_FirstPerspective+0, _("Default Startup"));
    m_perspectives_menu->Append(ID_FirstPerspective+1, _("All Panes"));

    wxMenu* help_menu = new wxMenu;
    help_menu->Append(wxID_ABOUT, _("About..."));

    mb->Append(file_menu, _("File"));
    mb->Append(view_menu, _("View"));
    mb->Append(m_perspectives_menu, _("Perspectives"));
    mb->Append(options_menu, _("Options"));
    mb->Append(help_menu, _("Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));


    // min size for the frame itself isn't completely done.
    // see the end up wxFrameManager::Update() for the test
    // code. For now, just hard code a frame minimum size
    SetMinSize(wxSize(400,300));

    // create some toolbars
    wxToolBar* tb1 = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxTB_FLAT | wxTB_NODIVIDER);
    tb1->SetToolBitmapSize(wxSize(48,48));
    tb1->AddTool(101, wxT("Test"), wxArtProvider::GetBitmap(wxART_ERROR));
    tb1->AddSeparator();
    tb1->AddTool(102, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb1->AddTool(103, wxT("Test"), wxArtProvider::GetBitmap(wxART_INFORMATION));
    tb1->AddTool(103, wxT("Test"), wxArtProvider::GetBitmap(wxART_WARNING));
    tb1->AddTool(103, wxT("Test"), wxArtProvider::GetBitmap(wxART_MISSING_IMAGE));
    tb1->Realize();



    wxToolBar* tb2 = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxTB_FLAT | wxTB_NODIVIDER);
    tb2->SetToolBitmapSize(wxSize(16,16));

    wxBitmap tb2_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->AddTool(101, wxT("Test"), tb2_bmp1);
    tb2->Realize();


    wxToolBar* tb3 = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxTB_FLAT | wxTB_NODIVIDER);
    tb3->SetToolBitmapSize(wxSize(16,16));
    wxBitmap tb3_bmp1 = wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16,16));
    tb3->AddTool(101, wxT("Test"), tb3_bmp1);
    tb3->AddTool(101, wxT("Test"), tb3_bmp1);
    tb3->AddTool(101, wxT("Test"), tb3_bmp1);
    tb3->AddTool(101, wxT("Test"), tb3_bmp1);
    tb3->AddSeparator();
    tb3->AddTool(101, wxT("Test"), tb3_bmp1);
    tb3->AddTool(101, wxT("Test"), tb3_bmp1);
    tb3->Realize();



    wxToolBar* tb4 = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxTB_FLAT | wxTB_NODIVIDER | wxTB_HORZ_TEXT);
    tb4->SetToolBitmapSize(wxSize(16,16));
    wxBitmap tb4_bmp1 = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
    tb4->AddTool(101, wxT("Item 1"), tb4_bmp1);
    tb4->AddTool(101, wxT("Item 2"), tb4_bmp1);
    tb4->AddTool(101, wxT("Item 3"), tb4_bmp1);
    tb4->AddTool(101, wxT("Item 4"), tb4_bmp1);
    tb4->AddSeparator();
    tb4->AddTool(101, wxT("Item 5"), tb4_bmp1);
    tb4->AddTool(101, wxT("Item 6"), tb4_bmp1);
    tb4->AddTool(101, wxT("Item 7"), tb4_bmp1);
    tb4->AddTool(101, wxT("Item 8"), tb4_bmp1);
    tb4->Realize();

    // create some toolbars
    wxToolBar* tb5 = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxTB_FLAT | wxTB_NODIVIDER | wxTB_VERTICAL);
    tb5->SetToolBitmapSize(wxSize(48,48));
    tb5->AddTool(101, wxT("Test"), wxArtProvider::GetBitmap(wxART_ERROR));
    tb5->AddSeparator();
    tb5->AddTool(102, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
    tb5->AddTool(103, wxT("Test"), wxArtProvider::GetBitmap(wxART_INFORMATION));
    tb5->AddTool(103, wxT("Test"), wxArtProvider::GetBitmap(wxART_WARNING));
    tb5->AddTool(103, wxT("Test"), wxArtProvider::GetBitmap(wxART_MISSING_IMAGE));
    tb5->Realize();


    // add a bunch of panes
    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test1")).Caption(wxT("Pane Caption")).
                  Top());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test2")).Caption(wxT("Client Size Reporter")).
                  Bottom().Position(1));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test3")).Caption(wxT("Client Size Reporter")).
                  Bottom());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test4")).Caption(wxT("Pane Caption")).
                  Left());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test5")).Caption(wxT("No Close Button")).
                  Right().CloseButton(false));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test6")).Caption(wxT("Client Size Reporter")).
                  Right().Row(1));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test7")).Caption(wxT("Client Size Reporter")).
                  Left().Layer(1));

    m_mgr.AddPane(CreateTreeCtrl(), wxPaneInfo().
                  Name(wxT("test8")).Caption(wxT("Tree Pane")).
                  Left().Layer(1).Position(1));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test9")).Caption(wxT("Min Size 200x100")).
                  BestSize(wxSize(200,100)).MinSize(wxSize(200,100)).
                  Bottom().Layer(1));

    wxWindow* wnd10 = CreateTextCtrl(wxT("This pane will prompt the user before hiding."));
    m_mgr.AddPane(wnd10, wxPaneInfo().
                  Name(wxT("test10")).Caption(wxT("Text Pane with Hide Prompt")).
                  Bottom().Layer(1).Position(1));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("test11")).Caption(wxT("Fixed Pane")).
                  Bottom().Layer(1).Position(2).Fixed());


    m_mgr.AddPane(new SettingsPanel(this,this), wxPaneInfo().
                  Name(wxT("settings")).Caption(wxT("Dock Manager Settings")).
                  Dockable(false).Float().Hide());

    // create some center panes

    m_mgr.AddPane(CreateGrid(), wxPaneInfo().Name(wxT("grid_content")).
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTreeCtrl(), wxPaneInfo().Name(wxT("tree_content")).
                  CenterPane().Hide());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().Name(wxT("sizereport_content")).
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTextCtrl(), wxPaneInfo().Name(wxT("text_content")).
                  CenterPane().Hide());

    m_mgr.AddPane(CreateHTMLCtrl(), wxPaneInfo().Name(wxT("html_content")).
                  CenterPane());

    m_mgr.AddPane(CreateNotebook(), wxPaneInfo().Name(wxT("notebook_content")).
                  CenterPane());

    // add the toolbars to the manager

    m_mgr.AddPane(tb1, wxPaneInfo().
                  Name(wxT("tb1")).Caption(wxT("Big Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));

    m_mgr.AddPane(tb2, wxPaneInfo().
                  Name(wxT("tb2")).Caption(wxT("Toolbar 2")).
                  ToolbarPane().Top().Row(1).
                  LeftDockable(false).RightDockable(false));

    m_mgr.AddPane(tb3, wxPaneInfo().
                  Name(wxT("tb3")).Caption(wxT("Toolbar 3")).
                  ToolbarPane().Top().Row(1).Position(1).
                  LeftDockable(false).RightDockable(false));

    m_mgr.AddPane(tb4, wxPaneInfo().
                  Name(wxT("tb4")).Caption(wxT("Sample Bookmark Toolbar")).
                  ToolbarPane().Top().Row(2).
                  LeftDockable(false).RightDockable(false));

    m_mgr.AddPane(tb5, wxPaneInfo().
                  Name(wxT("tb5")).Caption(wxT("Sample Vertical Toolbar")).
                  ToolbarPane().Left().
                  GripperTop().
                  TopDockable(false).BottomDockable(false));

    m_mgr.AddPane(new wxButton(this, wxID_ANY, _("Test Button")),
                  wxPaneInfo().Name(wxT("tb6")).
                  ToolbarPane().Top().Row(2).Position(1).
                  LeftDockable(false).RightDockable(false));


    // make some default perspectives

    wxString perspective_all = m_mgr.SavePerspective();

    int i, count;
    wxPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    for (i = 0, count = all_panes.GetCount(); i < count; ++i)
        if (!all_panes.Item(i).IsToolbar())
            all_panes.Item(i).Hide();
    m_mgr.GetPane(wxT("tb1")).Hide();
    m_mgr.GetPane(wxT("tb6")).Hide();
    m_mgr.GetPane(wxT("test8")).Show().Left().Layer(0).Row(0).Position(0);
    m_mgr.GetPane(wxT("test10")).Show().Bottom().Layer(0).Row(0).Position(0);
    m_mgr.GetPane(wxT("html_content")).Show();
    wxString perspective_default = m_mgr.SavePerspective();

    m_perspectives.Add(perspective_default);
    m_perspectives.Add(perspective_all);

    // "commit" all changes made to wxFrameManager
    m_mgr.Update();
}

MyFrame::~MyFrame()
{
    m_mgr.UnInit();
}

wxDockArt* MyFrame::GetDockArt()
{
    return m_mgr.GetArtProvider();
}

void MyFrame::DoUpdate()
{
    m_mgr.Update();
}


void MyFrame::OnEraseBackground(wxEraseEvent& event)
{
    event.Skip();
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    event.Skip();
}

void MyFrame::OnSettings(wxCommandEvent& WXUNUSED(event))
{
    // show the settings pane, and float it
    wxPaneInfo& floating_pane = m_mgr.GetPane(wxT("settings")).Float().Show();

    if (floating_pane.floating_pos == wxDefaultPosition)
        floating_pane.FloatingPosition(GetStartPosition());

    m_mgr.Update();
}


void MyFrame::OnGradient(wxCommandEvent& event)
{
    int gradient = 0;

    switch (event.GetId())
    {
        case ID_NoGradient:         gradient = wxAUI_GRADIENT_NONE; break;
        case ID_VerticalGradient:   gradient = wxAUI_GRADIENT_VERTICAL; break;
        case ID_HorizontalGradient: gradient = wxAUI_GRADIENT_HORIZONTAL; break;
    }

    m_mgr.GetArtProvider()->SetMetric(wxAUI_ART_GRADIENT_TYPE, gradient);
    m_mgr.Update();
}

void MyFrame::OnManagerFlag(wxCommandEvent& event)
{
    unsigned int flag = 0;

#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
    if (event.GetId() == ID_TransparentDrag ||
        event.GetId() == ID_TransparentHint ||
        event.GetId() == ID_TransparentHintFade)
    {
        wxMessageBox(wxT("This option is presently only available on wxGTK, wxMSW and wxMac"));
        return;
    }
#endif

    switch (event.GetId())
    {
        case ID_AllowFloating: flag = wxAUI_MGR_ALLOW_FLOATING; break;
        case ID_TransparentDrag: flag = wxAUI_MGR_TRANSPARENT_DRAG; break;
        case ID_TransparentHint: flag = wxAUI_MGR_TRANSPARENT_HINT; break;
        case ID_TransparentHintFade: flag = wxAUI_MGR_TRANSPARENT_HINT_FADE; break;
        case ID_DisableVenetian: flag = wxAUI_MGR_DISABLE_VENETIAN_BLINDS; break;
        case ID_DisableVenetianFade: flag = wxAUI_MGR_DISABLE_VENETIAN_BLINDS_FADE; break;
        case ID_AllowActivePane: flag = wxAUI_MGR_ALLOW_ACTIVE_PANE; break;
    }
    m_mgr.SetFlags(m_mgr.GetFlags() ^ flag);
    m_mgr.Update();
}

void MyFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    unsigned int flags = m_mgr.GetFlags();

    switch (event.GetId())
    {
        case ID_NoGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_ART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE ? true : false);
            break;
        case ID_VerticalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_ART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL ? true : false);
            break;
        case ID_HorizontalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_ART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL ? true : false);
            break;
        case ID_AllowFloating:
            event.Check(flags & wxAUI_MGR_ALLOW_FLOATING ? true : false);
            break;
        case ID_TransparentDrag:
            event.Check(flags & wxAUI_MGR_TRANSPARENT_DRAG ? true : false);
            break;
        case ID_TransparentHint:
            event.Check(flags & wxAUI_MGR_TRANSPARENT_HINT ? true : false);
            break;
        case ID_TransparentHintFade:
            event.Check(flags & wxAUI_MGR_TRANSPARENT_HINT_FADE ? true : false);
            break;
        case ID_DisableVenetian:
            event.Check(flags & wxAUI_MGR_DISABLE_VENETIAN_BLINDS ? true : false);
            break;
        case ID_DisableVenetianFade:
            event.Check(flags & wxAUI_MGR_DISABLE_VENETIAN_BLINDS_FADE ? true : false);
            break;
    }
}

void MyFrame::OnPaneClose(wxFrameManagerEvent& evt)
{
    if (evt.pane->name == wxT("test10"))
    {
        int res = wxMessageBox(wxT("Are you sure you want to close/hide this pane?"),
                               wxT("wxAUI"),
                               wxYES_NO,
                               this);
        if (res != wxYES)
            evt.Veto();
    }
}



void MyFrame::OnCreatePerspective(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dlg(this, wxT("Enter a name for the new perspective:"),
                          wxT("wxAUI Test"));

    dlg.SetValue(wxString::Format(wxT("Perspective %d"), m_perspectives.GetCount()+1));
    if (dlg.ShowModal() != wxID_OK)
        return;

    if (m_perspectives.GetCount() == 0)
    {
        m_perspectives_menu->AppendSeparator();
    }

    m_perspectives_menu->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
    m_perspectives.Add(m_mgr.SavePerspective());
}

void MyFrame::OnCopyPerspectiveCode(wxCommandEvent& WXUNUSED(event))
{
    wxString s = m_mgr.SavePerspective();

#if wxUSE_CLIPBOARD
    if (wxTheClipboard->Open())
    {
        wxTheClipboard->SetData(new wxTextDataObject(s));
        wxTheClipboard->Close();
    }
#endif
}

void MyFrame::OnRestorePerspective(wxCommandEvent& event)
{
    m_mgr.LoadPerspective(m_perspectives.Item(event.GetId() - ID_FirstPerspective));
}


wxPoint MyFrame::GetStartPosition()
{
    static int x = 0;
    x += 20;
    wxPoint pt = ClientToScreen(wxPoint(0,0));
    return wxPoint(pt.x + x, pt.y + x);
}

void MyFrame::OnCreateTree(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTreeCtrl(), wxPaneInfo().
                  Name(wxT("Test")).Caption(wxT("Tree Control")).
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(wxSize(150,300)));
    m_mgr.Update();
}

void MyFrame::OnCreateGrid(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateGrid(), wxPaneInfo().
                  Name(wxT("Test")).Caption(wxT("Grid")).
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(wxSize(300,200)));
    m_mgr.Update();
}

void MyFrame::OnCreateHTML(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateHTMLCtrl(), wxPaneInfo().
                  Name(wxT("Test")).Caption(wxT("Grid")).
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(wxSize(300,200)));
    m_mgr.Update();
}

void MyFrame::OnCreateNotebook(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateNotebook(), wxPaneInfo().
                  Name(wxT("Test")).Caption(wxT("Notebook")).
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(wxSize(300,200)));
    m_mgr.Update();
}

void MyFrame::OnCreateText(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTextCtrl(), wxPaneInfo().
                  Name(wxT("Test")).Caption(wxT("Text Control")).
                  Float().FloatingPosition(GetStartPosition()));
    m_mgr.Update();
}

void MyFrame::OnCreateSizeReport(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateSizeReportCtrl(), wxPaneInfo().
                  Name(wxT("Test")).Caption(wxT("Client Size Reporter")).
                  Float().FloatingPosition(GetStartPosition()));
    m_mgr.Update();
}

void MyFrame::OnChangeContentPane(wxCommandEvent& event)
{
    m_mgr.GetPane(wxT("grid_content")).Show(event.GetId() == ID_GridContent ? true:false);
    m_mgr.GetPane(wxT("text_content")).Show(event.GetId() == ID_TextContent ? true:false);
    m_mgr.GetPane(wxT("tree_content")).Show(event.GetId() == ID_TreeContent ? true:false);
    m_mgr.GetPane(wxT("sizereport_content")).Show(event.GetId() == ID_SizeReportContent ? true:false);
    m_mgr.GetPane(wxT("html_content")).Show(event.GetId() == ID_HTMLContent ? true:false);
    m_mgr.GetPane(wxT("notebook_content")).Show(event.GetId() == ID_NotebookContent ? true:false);
    m_mgr.Update();
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("wxAUI Demo\nAn advanced window management library for wxWidgets\n(c) Copyright 2005-2006, Kirix Corporation"), _("About wxAUI Demo"), wxOK, this);
}

wxTextCtrl* MyFrame::CreateTextCtrl(const wxString& ctrl_text)
{
    static int n = 0;

    wxString text;
    if (ctrl_text.Length() > 0)
        text = ctrl_text;
         else
        text.Printf(wxT("This is text box %d"), ++n);

    return new wxTextCtrl(this,wxID_ANY, text,
                          wxPoint(0,0), wxSize(150,90),
                          wxNO_BORDER | wxTE_MULTILINE);
}


wxGrid* MyFrame::CreateGrid()
{
    wxGrid* grid = new wxGrid(this, wxID_ANY,
                              wxPoint(0,0),
                              wxSize(150,250),
                              wxNO_BORDER | wxWANTS_CHARS);
    grid->CreateGrid(50, 20);
    return grid;
}

wxTreeCtrl* MyFrame::CreateTreeCtrl()
{
    wxTreeCtrl* tree = new wxTreeCtrl(this, wxID_ANY,
                                      wxPoint(0,0), wxSize(160,250),
                                      wxTR_DEFAULT_STYLE | wxNO_BORDER);

    wxTreeItemId root = tree->AddRoot(wxT("wxAUI Project"));
    wxArrayTreeItemIds items;


    wxImageList* imglist = new wxImageList(16, 16, true, 2);
    imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16,16)));
    imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16)));
    tree->AssignImageList(imglist);

    items.Add(tree->AppendItem(root, wxT("Item 1"), 0));
    items.Add(tree->AppendItem(root, wxT("Item 2"), 0));
    items.Add(tree->AppendItem(root, wxT("Item 3"), 0));
    items.Add(tree->AppendItem(root, wxT("Item 4"), 0));
    items.Add(tree->AppendItem(root, wxT("Item 5"), 0));


    int i, count;
    for (i = 0, count = items.Count(); i < count; ++i)
    {
        wxTreeItemId id = items.Item(i);
        tree->AppendItem(id, wxT("Subitem 1"), 1);
        tree->AppendItem(id, wxT("Subitem 2"), 1);
        tree->AppendItem(id, wxT("Subitem 3"), 1);
        tree->AppendItem(id, wxT("Subitem 4"), 1);
        tree->AppendItem(id, wxT("Subitem 5"), 1);
    }


    tree->Expand(root);

    return tree;
}

wxSizeReportCtrl* MyFrame::CreateSizeReportCtrl(int width, int height)
{
    wxSizeReportCtrl* ctrl = new wxSizeReportCtrl(this, wxID_ANY,
                                   wxDefaultPosition,
                                   wxSize(width, height), &m_mgr);
    return ctrl;
}

wxHtmlWindow* MyFrame::CreateHTMLCtrl()
{
    wxHtmlWindow* ctrl = new wxHtmlWindow(this, wxID_ANY,
                                   wxDefaultPosition,
                                   wxSize(400,300));
    ctrl->SetPage(GetIntroText());
    return ctrl;
}

wxAuiMultiNotebook* MyFrame::CreateNotebook()
{
   wxAuiMultiNotebook* ctrl = new wxAuiMultiNotebook( this, wxID_ANY,
                                    wxDefaultPosition, wxSize(400,300) );
                                    
   wxPanel *panel = new wxPanel( ctrl, wxID_ANY );
   wxFlexGridSizer *flex = new wxFlexGridSizer( 2 );
   flex->AddGrowableRow( 0 );
   flex->AddGrowableRow( 3 );
   flex->AddGrowableCol( 1 );
   flex->Add( 5,5 );   flex->Add( 5,5 );
   flex->Add( new wxStaticText( panel, -1, wxT("wxTextCtrl:") ), 0, wxALL|wxALIGN_CENTRE, 5 );
   flex->Add( new wxTextCtrl( panel, -1, wxT(""), wxDefaultPosition, wxSize(100,-1) ), 
                1, wxALL|wxALIGN_CENTRE, 5 );
   flex->Add( new wxStaticText( panel, -1, wxT("wxSpinCtrl:") ), 0, wxALL|wxALIGN_CENTRE, 5 );
   flex->Add( new wxSpinCtrl( panel, -1, wxT("5"), wxDefaultPosition, wxSize(100,-1), 
                wxSP_ARROW_KEYS, 5, 50, 5 ), 0, wxALL|wxALIGN_CENTRE, 5 );
   flex->Add( 5,5 );   flex->Add( 5,5 );
   panel->SetSizer( flex );
   ctrl->AddPage( panel, wxT("wxPanel") );
   
   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, wxT("Some text"),
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE) , wxT("wxTextCtrl I") );
                
   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, wxT("Some more text"),
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE) , wxT("wxTextCtrl II") );
   
   return ctrl;
}

wxString MyFrame::GetIntroText()
{
    const char* text =
        "<html><body>"
        "<h3>Welcome to wxAUI</h3>"
        "<br/><b>Overview</b><br/>"
        "<p>wxAUI is an Advanced User Interface library for the wxWidgets toolkit "
        "that allows developers to create high-quality, cross-platform user "
        "interfaces quickly and easily.</p>"
        "<p><b>Features</b></p>"
        "<p>With wxAUI, developers can create application frameworks with:</p>"
        "<ul>"
        "<li>Native, dockable floating frames</li>"
        "<li>Perspective saving and loading</li>"
        "<li>Native toolbars incorporating real-time, &quot;spring-loaded&quot; dragging</li>"
        "<li>Customizable floating/docking behavior</li>"
        "<li>Completely customizable look-and-feel</li>"
        "<li>Optional transparent window effects (while dragging or docking)</li>"
        "</ul>"
        "<p><b>What's new in 0.9.2?</b></p>"
        "<p>The following features/fixes have been added since the last version of wxAUI:</p>"
        "<ul>"
        "<li>Support for wxMac</li>"
        "<li>Updates for wxWidgets 2.6.3</li>"
        "<li>Fix to pass more unused events through</li>"
        "<li>Fix to allow floating windows to receive idle events</li>"
        "<li>Fix for minimizing/maximizing problem with transparent hint pane</li>"
        "<li>Fix to not paint empty hint rectangles</li>"
        "<li>Fix for 64-bit compilation</li>"
        "</ul>"
        "<p><b>What changed in 0.9.1?</b></p>"
        "<p>The following features/fixes were added in wxAUI 0.9.1:</p>"
        "<ul>"
        "<li>Support for MDI frames</li>"
        "<li>Gradient captions option</li>"
        "<li>Active/Inactive panes option</li>"
        "<li>Fix for screen artifacts/paint problems</li>"
        "<li>Fix for hiding/showing floated window problem</li>"
        "<li>Fix for floating pane sizing problem</li>"
        "<li>Fix for drop position problem when dragging around center pane margins</li>"
        "<li>LF-only text file formatting for source code</li>"
        "</ul>"
        "<p>See README.txt for more information.</p>"
        "</body></html>";

    return wxString::FromAscii(text);
}
