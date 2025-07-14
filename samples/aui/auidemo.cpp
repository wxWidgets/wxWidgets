///////////////////////////////////////////////////////////////////////////////
// Name:        auidemo.cpp
// Purpose:     wxaui: wx advanced user interface - sample/test program
// Author:      Benjamin I. Williams
// Created:     2005-10-03
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#include "wx/app.h"
#include "wx/grid.h"
#include "wx/treectrl.h"
#include "wx/spinctrl.h"
#include "wx/artprov.h"
#include "wx/clipbrd.h"
#include "wx/image.h"
#include "wx/choice.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/imaglist.h"
#include "wx/dataobj.h"
#include "wx/dcclient.h"
#include "wx/bmpbuttn.h"
#include "wx/log.h"
#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/sstream.h"
#include "wx/statusbr.h"
#include "wx/msgdlg.h"
#include "wx/textdlg.h"
#include "wx/stattext.h"
#include "wx/checkbox.h"

#include "wx/aui/aui.h"
#include "wx/aui/serializer.h"

#include "wx/xml/xml.h"

#include "../sample.xpm"

#include <map>

// -- application --

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

wxDECLARE_APP(MyApp);
wxIMPLEMENT_APP(MyApp);


class wxSizeReportCtrl;

// -- frame --

class MyFrame : public wxFrame
{
    enum
    {
        ID_CreateTree = wxID_HIGHEST,
        ID_CreateGrid,
        ID_CreateText,
        ID_CreateHTML,
        ID_CreateNotebook,
        ID_CreateSizeReport,
        ID_GridContent,
        ID_TextContent,
        ID_TreeContent,
        ID_HTMLContent,
        ID_NotebookContent,
        ID_SizeReportContent,
        ID_CreatePerspective,
        ID_CopyLayout,
        ID_PasteLayout,
        ID_EditNotebookLayout,
        ID_AllowFloating,
        ID_AllowActivePane,
        ID_TransparentHint,
        ID_VenetianBlindsHint,
        ID_RectangleHint,
        ID_NoHint,
        ID_HintFade,
        ID_TransparentDrag,
        ID_NoGradient,
        ID_VerticalGradient,
        ID_HorizontalGradient,
        ID_LiveUpdate,
        ID_AllowToolbarResizing,
        ID_Settings,
        ID_CustomizeToolbar,
        ID_DropDownToolbarItem,
        ID_NotebookNoCloseButton,
        ID_NotebookCloseButton,
        ID_NotebookCloseButtonAll,
        ID_NotebookCloseButtonActive,
        ID_NotebookAllowTabMove,
        ID_NotebookAllowTabExternalMove,
        ID_NotebookAllowTabSplit,
        ID_NotebookWindowList,
        ID_NotebookScrollButtons,
        ID_NotebookTabFixedWidth,
        ID_NotebookTabPin,
        ID_NotebookTabUnpin,
        ID_NotebookMultiLine,
        ID_NotebookNextTab,
        ID_NotebookPrevTab,
        ID_NotebookArtDefault,
        ID_NotebookArtFlat,
#ifdef wxHAS_NATIVE_TABART
        ID_NotebookArtNative,
#endif // wxHAS_NATIVE_TABART
        ID_NotebookArtGloss,
        ID_NotebookArtSimple,
        ID_NotebookAlignTop,
        ID_NotebookAlignBottom,
        ID_NotebookSplit,
        ID_NotebookUnsplit,
        ID_NotebookNewTab,
        ID_NotebookDeleteTab,
        ID_NotebookNormalTab,
        ID_NotebookPinTab,
        ID_NotebookLockTab,
        ID_3CHECK,
        ID_UI_2CHECK_UPDATED,
        ID_UI_3CHECK_UPDATED,

        ID_SampleItem,

        ID_FirstPerspective = ID_CreatePerspective+1000
    };

public:
    MyFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

    wxAuiDockArt* GetDockArt();
    void DoUpdate();

private:
    wxTextCtrl* CreateTextCtrl(const wxString& text = wxEmptyString);
    wxGrid* CreateGrid();
    wxTreeCtrl* CreateTreeCtrl();
    wxSizeReportCtrl* CreateSizeReportCtrl(const wxSize &size = wxWindow::FromDIP(wxSize(80, 80), nullptr));
    wxPoint GetStartPosition();
    wxHtmlWindow* CreateHTMLCtrl(wxWindow* parent = nullptr);
    wxAuiNotebook* CreateNotebook();

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
    void OnDropDownToolbarItem(wxAuiToolBarEvent& evt);
    void OnCreatePerspective(wxCommandEvent& evt);
    void OnCopyLayout(wxCommandEvent& evt);
    void OnPasteLayout(wxCommandEvent& evt);
    void OnEditNotebookLayout(wxCommandEvent& evt);
    void OnRestorePerspective(wxCommandEvent& evt);
    void OnSettings(wxCommandEvent& evt);
    void OnCustomizeToolbar(wxCommandEvent& evt);
    void OnAllowNotebookDnD(wxAuiNotebookEvent& evt);
    void OnNotebookPageClose(wxAuiNotebookEvent& evt);
    void OnNotebookPageClosed(wxAuiNotebookEvent& evt);
    void OnNotebookPageChanging(wxAuiNotebookEvent &evt);
    void OnNotebookTabRightClick(wxAuiNotebookEvent &evt);
    void OnNotebookTabBackgroundDClick(wxAuiNotebookEvent &evt);
    void OnExit(wxCommandEvent& evt);
    void OnAbout(wxCommandEvent& evt);
    void OnTabAlignment(wxCommandEvent &evt);

    void OnGradient(wxCommandEvent& evt);
    void OnToolbarResizing(wxCommandEvent& evt);
    void OnManagerFlag(wxCommandEvent& evt);
    void OnNotebookFlag(wxCommandEvent& evt);
    void OnUpdateUI(wxUpdateUIEvent& evt);
    void OnUpdateTabKindUI(wxUpdateUIEvent& evt);

    void OnNotebookNextOrPrev(wxCommandEvent& evt);
    void OnNotebookSplit(wxCommandEvent& evt);
    void OnNotebookUnsplit(wxCommandEvent& evt);
    void OnNotebookNewTab(wxCommandEvent& evt);
    void OnNotebookDeleteTab(wxCommandEvent& evt);
    void OnNotebookSetTabKind(wxCommandEvent& evt);

    void OnPaneClose(wxAuiManagerEvent& evt);

    void OnCheckboxUpdateUI(wxUpdateUIEvent& evt);

private:

    wxAuiManager m_mgr;
    wxArrayString m_perspectives;
    wxMenu* m_perspectivesMenu;
    long m_notebookStyle;
    enum class TabArt
    {
        Default,
#ifdef wxHAS_NATIVE_TABART
        Native,
#endif // wxHAS_NATIVE_TABART
        Flat,
        Gloss,
        Simple
    } m_notebookTheme = TabArt::Default;

    wxDECLARE_EVENT_TABLE();
};


// -- wxSizeReportCtrl --
// (a utility control that always reports it's client size)

class wxSizeReportCtrl : public wxControl
{
public:

    wxSizeReportCtrl(wxWindow* parent, wxWindowID id = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     wxAuiManager* mgr = nullptr)
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

        s.Printf("Size: %d x %d", size.x, size.y);

        dc.SetFont(*wxNORMAL_FONT);
        dc.GetTextExtent(s, &w, &height);
        height += FromDIP(3);
        dc.SetBrush(*wxWHITE_BRUSH);
        dc.SetPen(*wxWHITE_PEN);
        dc.DrawRectangle(0, 0, size.x, size.y);
        dc.SetPen(*wxLIGHT_GREY_PEN);
        dc.DrawLine(0, 0, size.x, size.y);
        dc.DrawLine(0, size.y, size.x, 0);
        dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2));

        if (m_mgr)
        {
            wxAuiPaneInfo pi = m_mgr->GetPane(this);

            s.Printf("Layer: %d", pi.dock_layer);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*1));

            s.Printf("Dock: %d Row: %d", pi.dock_direction, pi.dock_row);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*2));

            s.Printf("Position: %d", pi.dock_pos);
            dc.GetTextExtent(s, &w, &h);
            dc.DrawText(s, (size.x-w)/2, ((size.y-(height*5))/2)+(height*3));

            s.Printf("Proportion: %d", pi.dock_proportion);
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

    wxAuiManager* m_mgr;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxSizeReportCtrl, wxControl)
    EVT_PAINT(wxSizeReportCtrl::OnPaint)
    EVT_SIZE(wxSizeReportCtrl::OnSize)
    EVT_ERASE_BACKGROUND(wxSizeReportCtrl::OnEraseBackground)
wxEND_EVENT_TABLE()


class SettingsPanel : public wxPanel
{
    enum
    {
        ID_PaneBorderSize = wxID_HIGHEST,
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
            : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
              m_frame(frame)
    {
        //wxBoxSizer* vert = new wxBoxSizer(wxVERTICAL);

        //vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);

        wxSize const elementSize = FromDIP(wxSize(180, 20));

        wxBoxSizer* s1 = new wxBoxSizer(wxHORIZONTAL);
        m_borderSize = new wxSpinCtrl(this, ID_PaneBorderSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
        s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s1->Add(new wxStaticText(this, wxID_ANY, "Pane Border Size:"));
        s1->Add(m_borderSize);
        s1->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s1->SetItemMinSize((size_t)1, elementSize);
        //vert->Add(s1, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        wxBoxSizer* s2 = new wxBoxSizer(wxHORIZONTAL);
        m_sashSize = new wxSpinCtrl(this, ID_SashSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
        s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s2->Add(new wxStaticText(this, wxID_ANY, "Sash Size:"));
        s2->Add(m_sashSize);
        s2->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s2->SetItemMinSize((size_t)1, elementSize);
        //vert->Add(s2, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        wxBoxSizer* s3 = new wxBoxSizer(wxHORIZONTAL);
        m_captionSize = new wxSpinCtrl(this, ID_CaptionSize, wxString::Format("%d", frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE)), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, FromDIP(100), frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));
        s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s3->Add(new wxStaticText(this, wxID_ANY, "Caption Size:"));
        s3->Add(m_captionSize);
        s3->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s3->SetItemMinSize((size_t)1, elementSize);
        //vert->Add(s3, 0, wxEXPAND | wxLEFT | wxBOTTOM, FromDIP(5));

        //vert->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);


        wxBitmapBundle const b = CreateColorBitmap(*wxBLACK);
        wxSize const bitmapSize = FromDIP(wxSize(50, 25));

        wxBoxSizer* s4 = new wxBoxSizer(wxHORIZONTAL);
        m_backgroundColor = new wxBitmapButton(this, ID_BackgroundColor, b, wxDefaultPosition, bitmapSize);
        s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s4->Add(new wxStaticText(this, wxID_ANY, "Background Color:"));
        s4->Add(m_backgroundColor);
        s4->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s4->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s5 = new wxBoxSizer(wxHORIZONTAL);
        m_sashColor = new wxBitmapButton(this, ID_SashColor, b, wxDefaultPosition, bitmapSize);
        s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s5->Add(new wxStaticText(this, wxID_ANY, "Sash Color:"));
        s5->Add(m_sashColor);
        s5->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s5->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s6 = new wxBoxSizer(wxHORIZONTAL);
        m_inactiveCaptionColor = new wxBitmapButton(this, ID_InactiveCaptionColor, b, wxDefaultPosition, bitmapSize);
        s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s6->Add(new wxStaticText(this, wxID_ANY, "Normal Caption:"));
        s6->Add(m_inactiveCaptionColor);
        s6->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s6->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s7 = new wxBoxSizer(wxHORIZONTAL);
        m_inactiveCaptionGradientColor = new wxBitmapButton(this, ID_InactiveCaptionGradientColor, b, wxDefaultPosition, bitmapSize);
        s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s7->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Gradient:"));
        s7->Add(m_inactiveCaptionGradientColor);
        s7->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s7->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s8 = new wxBoxSizer(wxHORIZONTAL);
        m_inactiveCaptionTextColor = new wxBitmapButton(this, ID_InactiveCaptionTextColor, b, wxDefaultPosition, bitmapSize);
        s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s8->Add(new wxStaticText(this, wxID_ANY, "Normal Caption Text:"));
        s8->Add(m_inactiveCaptionTextColor);
        s8->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s8->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s9 = new wxBoxSizer(wxHORIZONTAL);
        m_activeCaptionColor = new wxBitmapButton(this, ID_ActiveCaptionColor, b, wxDefaultPosition, bitmapSize);
        s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s9->Add(new wxStaticText(this, wxID_ANY, "Active Caption:"));
        s9->Add(m_activeCaptionColor);
        s9->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s9->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s10 = new wxBoxSizer(wxHORIZONTAL);
        m_activeCaptionGradientColor = new wxBitmapButton(this, ID_ActiveCaptionGradientColor, b, wxDefaultPosition, bitmapSize);
        s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s10->Add(new wxStaticText(this, wxID_ANY, "Active Caption Gradient:"));
        s10->Add(m_activeCaptionGradientColor);
        s10->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s10->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s11 = new wxBoxSizer(wxHORIZONTAL);
        m_activeCaptionTextColor = new wxBitmapButton(this, ID_ActiveCaptionTextColor, b, wxDefaultPosition, bitmapSize);
        s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s11->Add(new wxStaticText(this, wxID_ANY, "Active Caption Text:"));
        s11->Add(m_activeCaptionTextColor);
        s11->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s11->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s12 = new wxBoxSizer(wxHORIZONTAL);
        m_borderColor = new wxBitmapButton(this, ID_BorderColor, b, wxDefaultPosition, bitmapSize);
        s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s12->Add(new wxStaticText(this, wxID_ANY, "Border Color:"));
        s12->Add(m_borderColor);
        s12->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s12->SetItemMinSize((size_t)1, elementSize);

        wxBoxSizer* s13 = new wxBoxSizer(wxHORIZONTAL);
        m_gripperColor = new wxBitmapButton(this, ID_GripperColor, b, wxDefaultPosition, bitmapSize);
        s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s13->Add(new wxStaticText(this, wxID_ANY, "Gripper Color:"));
        s13->Add(m_gripperColor);
        s13->Add(FromDIP(1), FromDIP(1), 1, wxEXPAND);
        s13->SetItemMinSize((size_t)1, elementSize);

        wxGridSizer* gridSizer = new wxGridSizer(2);
        gridSizer->SetHGap(FromDIP(5));
        gridSizer->Add(s1);  gridSizer->Add(s4);
        gridSizer->Add(s2);  gridSizer->Add(s5);
        gridSizer->Add(s3);  gridSizer->Add(s13);
        gridSizer->Add(FromDIP(1),FromDIP(1)); gridSizer->Add(s12);
        gridSizer->Add(s6);  gridSizer->Add(s9);
        gridSizer->Add(s7);  gridSizer->Add(s10);
        gridSizer->Add(s8);  gridSizer->Add(s11);

        wxBoxSizer* contSizer = new wxBoxSizer(wxVERTICAL);
        contSizer->Add(gridSizer, 1, wxEXPAND | wxALL, FromDIP(5));
        SetSizer(contSizer);
        GetSizer()->SetSizeHints(this);

        m_borderSize->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE));
        m_sashSize->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_SASH_SIZE));
        m_captionSize->SetValue(frame->GetDockArt()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE));

        UpdateColors();
    }

private:

    wxBitmapBundle CreateColorBitmap(const wxColour& c)
    {
        wxVector<wxBitmap> bitmaps;

        for (int i = 100; i <= 400; i += 25)
        {
            wxSize size(25 * i / 100, 14 * i / 100);
            wxImage image(size);
            for (int x = 0; x < size.x; ++x)
                for (int y = 0; y < size.y; ++y)
                {
                    wxColour pixcol = c;
                    if (x == 0 || x == size.x || y == 0 || y == size.y)
                        pixcol = *wxBLACK;
                    image.SetRGB(x, y, pixcol.Red(), pixcol.Green(), pixcol.Blue());
                }
            bitmaps.push_back(wxBitmap(image));
        }

        return wxBitmapBundle::FromBitmaps(bitmaps);
    }

    void UpdateColors()
    {
        wxColour bk = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BACKGROUND_COLOUR);
        m_backgroundColor->SetBitmapLabel(CreateColorBitmap(bk));

        wxColour cap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
        m_inactiveCaptionColor->SetBitmapLabel(CreateColorBitmap(cap));

        wxColour capgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR);
        m_inactiveCaptionGradientColor->SetBitmapLabel(CreateColorBitmap(capgrad));

        wxColour captxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR);
        m_inactiveCaptionTextColor->SetBitmapLabel(CreateColorBitmap(captxt));

        wxColour acap = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR);
        m_activeCaptionColor->SetBitmapLabel(CreateColorBitmap(acap));

        wxColour acapgrad = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR);
        m_activeCaptionGradientColor->SetBitmapLabel(CreateColorBitmap(acapgrad));

        wxColour acaptxt = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR);
        m_activeCaptionTextColor->SetBitmapLabel(CreateColorBitmap(acaptxt));

        wxColour sash = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_SASH_COLOUR);
        m_sashColor->SetBitmapLabel(CreateColorBitmap(sash));

        wxColour border = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_BORDER_COLOUR);
        m_borderColor->SetBitmapLabel(CreateColorBitmap(border));

        wxColour gripper = m_frame->GetDockArt()->GetColor(wxAUI_DOCKART_GRIPPER_COLOUR);
        m_gripperColor->SetBitmapLabel(CreateColorBitmap(gripper));
    }

    void OnPaneBorderSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnSashSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_SASH_SIZE,
                                         event.GetPosition());
        m_frame->DoUpdate();
    }

    void OnCaptionSize(wxSpinEvent& event)
    {
        m_frame->GetDockArt()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE,
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
            case ID_BackgroundColor:              var = wxAUI_DOCKART_BACKGROUND_COLOUR; break;
            case ID_SashColor:                    var = wxAUI_DOCKART_SASH_COLOUR; break;
            case ID_InactiveCaptionColor:         var = wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR; break;
            case ID_InactiveCaptionGradientColor: var = wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_InactiveCaptionTextColor:     var = wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_ActiveCaptionColor:           var = wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR; break;
            case ID_ActiveCaptionGradientColor:   var = wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR; break;
            case ID_ActiveCaptionTextColor:       var = wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR; break;
            case ID_BorderColor:                  var = wxAUI_DOCKART_BORDER_COLOUR; break;
            case ID_GripperColor:                 var = wxAUI_DOCKART_GRIPPER_COLOUR; break;
            default: return;
        }

        m_frame->GetDockArt()->SetColor(var, dlg.GetColourData().GetColour());
        m_frame->DoUpdate();
        UpdateColors();
    }

private:

    MyFrame* m_frame;
    wxSpinCtrl* m_borderSize;
    wxSpinCtrl* m_sashSize;
    wxSpinCtrl* m_captionSize;
    wxBitmapButton* m_inactiveCaptionTextColor;
    wxBitmapButton* m_inactiveCaptionGradientColor;
    wxBitmapButton* m_inactiveCaptionColor;
    wxBitmapButton* m_activeCaptionTextColor;
    wxBitmapButton* m_activeCaptionGradientColor;
    wxBitmapButton* m_activeCaptionColor;
    wxBitmapButton* m_sashColor;
    wxBitmapButton* m_backgroundColor;
    wxBitmapButton* m_borderColor;
    wxBitmapButton* m_gripperColor;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(SettingsPanel, wxPanel)
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
wxEND_EVENT_TABLE()


bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxFrame* frame = new MyFrame(nullptr,
                                 wxID_ANY,
                                 "wxAUI Sample Application",
                                 wxDefaultPosition,
                                 wxWindow::FromDIP(wxSize(800, 600), nullptr));
    frame->Show();

    return true;
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(MyFrame::ID_CreateTree, MyFrame::OnCreateTree)
    EVT_MENU(MyFrame::ID_CreateGrid, MyFrame::OnCreateGrid)
    EVT_MENU(MyFrame::ID_CreateText, MyFrame::OnCreateText)
    EVT_MENU(MyFrame::ID_CreateHTML, MyFrame::OnCreateHTML)
    EVT_MENU(MyFrame::ID_CreateSizeReport, MyFrame::OnCreateSizeReport)
    EVT_MENU(MyFrame::ID_CreateNotebook, MyFrame::OnCreateNotebook)
    EVT_MENU(MyFrame::ID_CreatePerspective, MyFrame::OnCreatePerspective)
    EVT_MENU(MyFrame::ID_CopyLayout, MyFrame::OnCopyLayout)
    EVT_MENU(MyFrame::ID_PasteLayout, MyFrame::OnPasteLayout)
    EVT_MENU(MyFrame::ID_EditNotebookLayout, MyFrame::OnEditNotebookLayout)
    EVT_MENU(ID_AllowFloating, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_VenetianBlindsHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_RectangleHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NoHint, MyFrame::OnManagerFlag)
    EVT_MENU(ID_HintFade, MyFrame::OnManagerFlag)
    EVT_MENU(ID_TransparentDrag, MyFrame::OnManagerFlag)
    EVT_MENU(ID_LiveUpdate, MyFrame::OnManagerFlag)
    EVT_MENU(ID_AllowActivePane, MyFrame::OnManagerFlag)
    EVT_MENU(ID_NotebookTabFixedWidth, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookTabPin, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookTabUnpin, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookMultiLine, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookNoCloseButton, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButton, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonAll, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookCloseButtonActive, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabMove, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabExternalMove, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookAllowTabSplit, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookScrollButtons, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookWindowList, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtDefault, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtFlat, MyFrame::OnNotebookFlag)
#ifdef wxHAS_NATIVE_TABART
    EVT_MENU(ID_NotebookArtNative, MyFrame::OnNotebookFlag)
#endif // wxHAS_NATIVE_TABART
    EVT_MENU(ID_NotebookArtGloss, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookArtSimple, MyFrame::OnNotebookFlag)
    EVT_MENU(ID_NotebookNextTab, MyFrame::OnNotebookNextOrPrev)
    EVT_MENU(ID_NotebookPrevTab, MyFrame::OnNotebookNextOrPrev)
    EVT_MENU(ID_NotebookAlignTop,     MyFrame::OnTabAlignment)
    EVT_MENU(ID_NotebookAlignBottom,  MyFrame::OnTabAlignment)
    EVT_MENU(ID_NotebookSplit, MyFrame::OnNotebookSplit)
    EVT_MENU(ID_NotebookUnsplit, MyFrame::OnNotebookUnsplit)
    EVT_MENU(ID_NotebookNewTab, MyFrame::OnNotebookNewTab)
    EVT_MENU(ID_NotebookDeleteTab, MyFrame::OnNotebookDeleteTab)
    EVT_MENU(ID_NotebookNormalTab, MyFrame::OnNotebookSetTabKind)
    EVT_MENU(ID_NotebookPinTab, MyFrame::OnNotebookSetTabKind)
    EVT_MENU(ID_NotebookLockTab, MyFrame::OnNotebookSetTabKind)
    EVT_MENU(ID_NoGradient, MyFrame::OnGradient)
    EVT_MENU(ID_VerticalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_HorizontalGradient, MyFrame::OnGradient)
    EVT_MENU(ID_AllowToolbarResizing, MyFrame::OnToolbarResizing)
    EVT_MENU(ID_Settings, MyFrame::OnSettings)
    EVT_MENU(ID_CustomizeToolbar, MyFrame::OnCustomizeToolbar)
    EVT_MENU(ID_GridContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TreeContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_TextContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_SizeReportContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_HTMLContent, MyFrame::OnChangeContentPane)
    EVT_MENU(ID_NotebookContent, MyFrame::OnChangeContentPane)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
    EVT_UPDATE_UI(ID_NotebookTabFixedWidth, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookTabPin, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookTabUnpin, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookMultiLine, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookNoCloseButton, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButton, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonAll, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookCloseButtonActive, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabMove, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabExternalMove, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookAllowTabSplit, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookScrollButtons, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookWindowList, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookNormalTab, MyFrame::OnUpdateTabKindUI)
    EVT_UPDATE_UI(ID_NotebookPinTab, MyFrame::OnUpdateTabKindUI)
    EVT_UPDATE_UI(ID_NotebookLockTab, MyFrame::OnUpdateTabKindUI)
    EVT_UPDATE_UI(ID_AllowFloating, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VenetianBlindsHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_RectangleHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoHint, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HintFade, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_TransparentDrag, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_LiveUpdate, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NoGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_VerticalGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_HorizontalGradient, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_AllowToolbarResizing, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookArtDefault, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookArtFlat, MyFrame::OnUpdateUI)
#ifdef wxHAS_NATIVE_TABART
    EVT_UPDATE_UI(ID_NotebookArtNative, MyFrame::OnUpdateUI)
#endif // wxHAS_NATIVE_TABART
    EVT_UPDATE_UI(ID_NotebookArtGloss, MyFrame::OnUpdateUI)
    EVT_UPDATE_UI(ID_NotebookArtSimple, MyFrame::OnUpdateUI)
    EVT_MENU_RANGE(MyFrame::ID_FirstPerspective, MyFrame::ID_FirstPerspective+1000,
                   MyFrame::OnRestorePerspective)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_DropDownToolbarItem, MyFrame::OnDropDownToolbarItem)
    EVT_AUI_PANE_CLOSE(MyFrame::OnPaneClose)
    EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, MyFrame::OnAllowNotebookDnD)
    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MyFrame::OnNotebookPageClose)
    EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, MyFrame::OnNotebookPageClosed)
    EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnNotebookPageChanging)
    EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(wxID_ANY, MyFrame::OnNotebookTabRightClick)
    EVT_AUINOTEBOOK_BG_DCLICK(wxID_ANY, MyFrame::OnNotebookTabBackgroundDClick)
    EVT_UPDATE_UI(ID_UI_2CHECK_UPDATED, MyFrame::OnCheckboxUpdateUI)
    EVT_UPDATE_UI(ID_UI_3CHECK_UPDATED, MyFrame::OnCheckboxUpdateUI)
wxEND_EVENT_TABLE()


MyFrame::MyFrame(wxWindow* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxFrame(parent, id, title, pos, size, style)
{
    // tell wxAuiManager to manage this frame
    m_mgr.SetManagedWindow(this);

    // set frame icon
    SetIcon(wxIcon(sample_xpm));

    // set up default notebook style
    m_notebookStyle = wxAUI_NB_DEFAULT_STYLE |
                      wxAUI_NB_TAB_EXTERNAL_MOVE |
                      wxAUI_NB_PIN_ON_ACTIVE_TAB |
                      wxAUI_NB_UNPIN_ON_ALL_PINNED |
                      wxNO_BORDER;

    // create menu
    wxMenuBar* mb = new wxMenuBar;

    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT);

    wxMenu* viewMenu = new wxMenu;
    viewMenu->Append(ID_CreateText, _("Create Text Control"));
    viewMenu->Append(ID_CreateHTML, _("Create HTML Control"));
    viewMenu->Append(ID_CreateTree, _("Create Tree"));
    viewMenu->Append(ID_CreateGrid, _("Create Grid"));
    viewMenu->Append(ID_CreateNotebook, _("Create Notebook"));
    viewMenu->Append(ID_CreateSizeReport, _("Create Size Reporter"));
    viewMenu->AppendSeparator();
    viewMenu->Append(ID_GridContent, _("Use a Grid for the Content Pane"));
    viewMenu->Append(ID_TextContent, _("Use a Text Control for the Content Pane"));
    viewMenu->Append(ID_HTMLContent, _("Use an HTML Control for the Content Pane"));
    viewMenu->Append(ID_TreeContent, _("Use a Tree Control for the Content Pane"));
    viewMenu->Append(ID_NotebookContent, _("Use a wxAuiNotebook control for the Content Pane"));
    viewMenu->Append(ID_SizeReportContent, _("Use a Size Reporter for the Content Pane"));

    wxMenu* optionsMenu = new wxMenu;
    optionsMenu->AppendRadioItem(ID_TransparentHint, _("Transparent Hint"));
    optionsMenu->AppendRadioItem(ID_VenetianBlindsHint, _("Venetian Blinds Hint"));
    optionsMenu->AppendRadioItem(ID_RectangleHint, _("Rectangle Hint"));
    optionsMenu->AppendRadioItem(ID_NoHint, _("No Hint"));
    optionsMenu->AppendSeparator();
    optionsMenu->AppendCheckItem(ID_HintFade, _("Hint Fade-in"));
    optionsMenu->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
    optionsMenu->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
    optionsMenu->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
    // Only show "live resize" toggle if it's actually functional.
    if ( !wxAuiManager::AlwaysUsesLiveResize() )
        optionsMenu->AppendCheckItem(ID_LiveUpdate, _("Live Resize Update"));
    optionsMenu->AppendSeparator();
    optionsMenu->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
    optionsMenu->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
    optionsMenu->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
    optionsMenu->AppendSeparator();
    optionsMenu->AppendCheckItem(ID_AllowToolbarResizing, _("Allow Toolbar Resizing"));
    optionsMenu->AppendSeparator();
    optionsMenu->Append(ID_Settings, _("Settings Pane"));

    wxMenu* notebookMenu = new wxMenu;
    notebookMenu->AppendRadioItem(ID_NotebookArtDefault, _("Default Theme"));
    notebookMenu->AppendRadioItem(ID_NotebookArtFlat, _("Flat Theme"));
#ifdef wxHAS_NATIVE_TABART
    notebookMenu->AppendRadioItem(ID_NotebookArtNative, _("Native-like Theme"));
#endif // wxHAS_NATIVE_TABART
    notebookMenu->AppendRadioItem(ID_NotebookArtGloss, _("Glossy Theme"));
    notebookMenu->AppendRadioItem(ID_NotebookArtSimple, _("Simple Theme"));
    notebookMenu->AppendSeparator();
    notebookMenu->AppendRadioItem(ID_NotebookNoCloseButton, _("No Close Button"));
    notebookMenu->AppendRadioItem(ID_NotebookCloseButton, _("Close Button at Right"));
    notebookMenu->AppendRadioItem(ID_NotebookCloseButtonAll, _("Close Button on All Tabs"));
    notebookMenu->AppendRadioItem(ID_NotebookCloseButtonActive, _("Close Button on Active Tab"));
    notebookMenu->AppendSeparator();
    notebookMenu->AppendRadioItem(ID_NotebookAlignTop, _("Tab Top Alignment"));
    notebookMenu->AppendRadioItem(ID_NotebookAlignBottom, _("Tab Bottom Alignment"));
    notebookMenu->AppendSeparator();
    notebookMenu->AppendCheckItem(ID_NotebookAllowTabMove, _("Allow Tab Move"));
    notebookMenu->AppendCheckItem(ID_NotebookAllowTabExternalMove, _("Allow External Tab Move"));
    notebookMenu->AppendCheckItem(ID_NotebookAllowTabSplit, _("Allow Notebook Split"));
    notebookMenu->AppendCheckItem(ID_NotebookScrollButtons, _("Scroll Buttons Visible"));
    notebookMenu->AppendCheckItem(ID_NotebookWindowList, _("Window List Button Visible"));
    notebookMenu->AppendCheckItem(ID_NotebookTabFixedWidth, _("Fixed-width Tabs"));
    notebookMenu->AppendCheckItem(ID_NotebookTabPin, _("Show &Pin Button"));
    notebookMenu->AppendCheckItem(ID_NotebookTabUnpin, _("Show &Unpin Buttons"));
    notebookMenu->AppendCheckItem(ID_NotebookMultiLine, _("Tabs on &Multiple Lines"));
    notebookMenu->AppendSeparator();
    notebookMenu->Append(ID_NotebookNextTab, _("Switch to next tab\tCtrl-F6"));
    notebookMenu->Append(ID_NotebookPrevTab, _("Switch to previous tab\tShift-Ctrl-F6"));
    notebookMenu->AppendSeparator();
    notebookMenu->Append(ID_NotebookSplit, _("&Split Notebook"));
    notebookMenu->Append(ID_NotebookUnsplit, _("&Unsplit Notebook"));
    notebookMenu->Append(ID_NotebookNewTab, _("Add a &New Tab"));
    notebookMenu->Append(ID_NotebookDeleteTab, _("&Delete Last Tab"));
    notebookMenu->AppendSeparator();
    notebookMenu->AppendRadioItem(ID_NotebookNormalTab, _("Make Current Tab Normal\tCtrl-0"));
    notebookMenu->AppendRadioItem(ID_NotebookPinTab, _("Pin Current Tab\tCtrl-1"));
    notebookMenu->AppendRadioItem(ID_NotebookLockTab, _("Lock Current Tab\tCtrl-2"));

    m_perspectivesMenu = new wxMenu;
    m_perspectivesMenu->Append(ID_CreatePerspective, _("Create Perspective"));
    m_perspectivesMenu->Append(ID_CopyLayout, _("Copy Layout to Clipboard as XML\tCtrl-C"));
    m_perspectivesMenu->Append(ID_PasteLayout, _("Paste XML Layout from Clipboard\tCtrl-V"));
    m_perspectivesMenu->AppendSeparator();
    m_perspectivesMenu->Append(ID_EditNotebookLayout, _("Edit &Notebook Layout..."));
    m_perspectivesMenu->AppendSeparator();
    m_perspectivesMenu->Append(ID_FirstPerspective+0, _("Default Startup"));
    m_perspectivesMenu->Append(ID_FirstPerspective+1, _("All Panes"));

    wxMenu* helpMenu = new wxMenu;
    helpMenu->Append(wxID_ABOUT);

    mb->Append(fileMenu, _("&File"));
    mb->Append(viewMenu, _("&View"));
    mb->Append(m_perspectivesMenu, _("&Perspectives"));
    mb->Append(optionsMenu, _("&Options"));
    mb->Append(notebookMenu, _("&Notebook"));
    mb->Append(helpMenu, _("&Help"));

    SetMenuBar(mb);

    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));


    // min size for the frame itself isn't completely done.
    // see the end up wxAuiManager::Update() for the test
    // code. For now, just hard code a frame minimum size
    SetMinSize(FromDIP(wxSize(400,300)));



    // prepare a few custom overflow elements for the toolbars' overflow buttons

    wxAuiToolBarItemArray prependItems;
    wxAuiToolBarItemArray appendItems;
    wxAuiToolBarItem item;
    item.SetKind(wxITEM_SEPARATOR);
    appendItems.Add(item);
    item.SetKind(wxITEM_NORMAL);
    item.SetId(ID_CustomizeToolbar);
    item.SetLabel(_("Customize..."));
    appendItems.Add(item);


    // If using wxUPDATE_UI_PROCESS_ALL (the default),
    // some of the problems handling controls in toolbar
    // are masked by the calls to wxCheckBox::UpdateWindowUI()
    wxUpdateUIEvent::SetMode(wxUPDATE_UI_PROCESS_SPECIFIED);
    // create some toolbars
    wxAuiToolBar* tb1 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb1->SetExtraStyle(tb1->GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES);
    tb1->AddTool(ID_SampleItem+1, "Test", wxArtProvider::GetBitmapBundle(wxART_ERROR));
    tb1->AddSeparator();
    tb1->AddTool(ID_SampleItem+2, "Test", wxArtProvider::GetBitmapBundle(wxART_QUESTION));
    tb1->AddTool(ID_SampleItem+3, "Test", wxArtProvider::GetBitmapBundle(wxART_INFORMATION));
    tb1->AddTool(ID_SampleItem+4, "Test", wxArtProvider::GetBitmapBundle(wxART_WARNING));
    tb1->AddTool(ID_SampleItem+5, "Test", wxArtProvider::GetBitmapBundle(wxART_MISSING_IMAGE));
    tb1->SetCustomOverflowItems(prependItems, appendItems);
    tb1->Realize();


    wxAuiToolBar* tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
    tb2->SetExtraStyle(tb2->GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES);

    wxBitmapBundle tb2_bmp1 = wxArtProvider::GetBitmapBundle(wxART_QUESTION, wxART_OTHER, wxSize(16,16));
    tb2->AddTool(ID_SampleItem+6, "Disabled", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+7, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+8, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+9, "Test", tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(ID_SampleItem+10, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+11, "Test", tb2_bmp1);
    tb2->AddSeparator();
    tb2->AddTool(ID_SampleItem+12, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+13, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+14, "Test", tb2_bmp1);
    tb2->AddTool(ID_SampleItem+15, "Test", tb2_bmp1);
    tb2->SetCustomOverflowItems(prependItems, appendItems);
    tb2->EnableTool(ID_SampleItem+6, false);
    tb2->Realize();


    wxAuiToolBar* tb3 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
    tb3->SetExtraStyle(tb3->GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES);
    wxBitmapBundle tb3_bmp1 = wxArtProvider::GetBitmapBundle(wxART_FOLDER, wxART_OTHER, wxSize(16,16));
    tb3->AddTool(ID_SampleItem+16, "Check 1", tb3_bmp1, "Check 1", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+17, "Check 2", tb3_bmp1, "Check 2", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+18, "Check 3", tb3_bmp1, "Check 3", wxITEM_CHECK);
    tb3->AddTool(ID_SampleItem+19, "Check 4", tb3_bmp1, "Check 4", wxITEM_CHECK);
    tb3->AddSeparator();
    tb3->AddTool(ID_SampleItem+20, "Radio 1", tb3_bmp1, "Radio 1", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+21, "Radio 2", tb3_bmp1, "Radio 2", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+22, "Radio 3", tb3_bmp1, "Radio 3", wxITEM_RADIO);
    tb3->AddSeparator();
    tb3->AddTool(ID_SampleItem+23, "Radio 1 (Group 2)", tb3_bmp1, "Radio 1 (Group 2)", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+24, "Radio 2 (Group 2)", tb3_bmp1, "Radio 2 (Group 2)", wxITEM_RADIO);
    tb3->AddTool(ID_SampleItem+25, "Radio 3 (Group 2)", tb3_bmp1, "Radio 3 (Group 2)", wxITEM_RADIO);
    tb3->SetCustomOverflowItems(prependItems, appendItems);
    tb3->Realize();


    wxAuiToolBar* tb4 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_OVERFLOW |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
    tb4->SetExtraStyle(tb4->GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES);
    wxBitmapBundle tb4_bmp1 = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
    tb4->AddTool(ID_DropDownToolbarItem, "Item 1", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+23, "Item 2", tb4_bmp1);
    tb4->SetToolSticky(ID_SampleItem+23, true);
    tb4->AddTool(ID_SampleItem+24, "Disabled", tb4_bmp1);
    tb4->EnableTool(ID_SampleItem+24, false); // Just to show disabled items look
    tb4->AddTool(ID_SampleItem+25, "Item 4", tb4_bmp1);
    tb4->AddSeparator();
    tb4->AddTool(ID_SampleItem+26, "Item 5", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+27, "Item 6", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+28, "Item 7", tb4_bmp1);
    tb4->AddTool(ID_SampleItem+29, "Item 8", tb4_bmp1);
    tb4->SetToolDropDown(ID_DropDownToolbarItem, true);
    tb4->SetCustomOverflowItems(prependItems, appendItems);
    tb4->AddControl(new wxStaticText(tb4, wxID_ANY, "Choose wisely: "));
    wxChoice* choice = new wxChoice(tb4, ID_SampleItem+35);
    choice->AppendString("Good choice");
    choice->AppendString("Better choice");
    tb4->AddControl(choice);
#if wxUSE_CHECKBOX
    wxCheckBox* checkbox1 = new wxCheckBox(tb4, ID_3CHECK,
                                                "Checkbox",
                                                wxDefaultPosition, wxDefaultSize,
                                                wxCHK_3STATE | wxCHK_ALLOW_3RD_STATE_FOR_USER);
    checkbox1->SetMinSize(checkbox1->GetSizeFromText(checkbox1->GetLabelText()));
    tb4->AddControl(checkbox1);
    wxCheckBox* checkbox2 = new wxCheckBox(tb4, ID_UI_2CHECK_UPDATED,
                                                "2Checkbox UI Updated",
                                                wxDefaultPosition, wxDefaultSize,
                                                wxCHK_2STATE);
    checkbox2->SetMinSize(checkbox2->GetSizeFromText(checkbox2->GetLabelText()));
    checkbox2->Disable();
    tb4->AddControl(checkbox2);
    wxCheckBox* checkbox3 = new wxCheckBox(tb4, ID_UI_3CHECK_UPDATED,
                                                "3Checkbox UI Updated",
                                                wxDefaultPosition, wxDefaultSize,
                                                wxCHK_3STATE);
    checkbox3->SetMinSize(checkbox3->GetSizeFromText(checkbox3->GetLabelText()));
    checkbox3->Disable();
    tb4->AddControl(checkbox3);
#endif
    tb4->Realize();


    wxAuiToolBar* tb5 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
    tb5->SetExtraStyle(tb5->GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES);
    tb5->AddTool(ID_SampleItem+30, "Test", wxArtProvider::GetBitmapBundle(wxART_ERROR));
    tb5->AddSeparator();
    tb5->AddTool(ID_SampleItem+31, "Test", wxArtProvider::GetBitmapBundle(wxART_QUESTION));
    tb5->AddTool(ID_SampleItem+32, "Test", wxArtProvider::GetBitmapBundle(wxART_INFORMATION));
    tb5->AddTool(ID_SampleItem+33, "Test", wxArtProvider::GetBitmapBundle(wxART_WARNING));
    tb5->AddTool(ID_SampleItem+34, "Test with help", wxArtProvider::GetBitmapBundle(wxART_MISSING_IMAGE), wxNullBitmap, wxITEM_NORMAL, "Short Help", "This is long help on the status bar", nullptr);
    tb5->SetCustomOverflowItems(prependItems, appendItems);
    tb5->Realize();

    // add a bunch of panes
    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test1").Caption("Pane Caption").
                  Top());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test2").Caption("Client Size Reporter").
                  Bottom().Position(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test3").Caption("Client Size Reporter").
                  Bottom().
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test4").Caption("Pane Caption").
                  Left());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test5").Caption("No Close Button").
                  Right().CloseButton(false));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test6").Caption("Client Size Reporter").
                  Right().Row(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test7").Caption("Float Client Size 111x222").
                  MinSize(FromDIP(wxSize(11,22))).
                  FloatingClientSize(wxSize(111, 222)).
                  Left().Layer(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
                  Name("test8").Caption("Tree Pane").
                  Left().Layer(1).Position(1).
                  CloseButton(true).MaximizeButton(true));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test9").Caption("Min Size 200x100").
                  BestSize(FromDIP(wxSize(200,100))).MinSize(FromDIP(wxSize(200,100))).
                  Bottom().Layer(1).
                  CloseButton(true).MaximizeButton(true));

    wxWindow* wnd10 = CreateTextCtrl("This pane will prompt the user before hiding.");

    // Give this pane an icon, too, just for testing.
    int iconSize = m_mgr.GetArtProvider()->GetMetricForWindow(wxAUI_DOCKART_CAPTION_SIZE, this);

    // Make it even to use 16 pixel icons with default 17 caption height.
    iconSize &= ~1;

    m_mgr.AddPane(wnd10, wxAuiPaneInfo().
                  Name("test10").Caption("Text Pane with Hide Prompt").
                  Bottom().Layer(1).Position(1).
                  Icon(wxArtProvider::GetBitmapBundle(wxART_WARNING,
                                                      wxART_OTHER,
                                                      wxSize(iconSize, iconSize))));

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Name("test11").Caption("Fixed Pane").
                  Bottom().Layer(1).Position(2).Fixed());


    m_mgr.AddPane(new SettingsPanel(this,this), wxAuiPaneInfo().
                  Name("settings").Caption("Dock Manager Settings").
                  Dockable(false).Float().Hide());

    // create some center panes

    m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().Name("grid_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().Name("tree_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().Name("sizereport_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().Name("text_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().Name("html_content").
                  CenterPane().Hide());

    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name("notebook_content").
                  CenterPane().PaneBorder(false));

    // add the toolbars to the manager
    m_mgr.AddPane(tb1, wxAuiPaneInfo().
                  Name("tb1").Caption("Big Toolbar").
                  ToolbarPane().Top());

    m_mgr.AddPane(tb2, wxAuiPaneInfo().
                  Name("tb2").Caption("Toolbar 2 (Horizontal)").
                  ToolbarPane().Top().Row(1));

    m_mgr.AddPane(tb3, wxAuiPaneInfo().
                  Name("tb3").Caption("Toolbar 3").
                  ToolbarPane().Top().Row(1).Position(1));

    m_mgr.AddPane(tb4, wxAuiPaneInfo().
                  Name("tb4").Caption("Sample Bookmark Toolbar").
                  ToolbarPane().Top().Row(2));

    m_mgr.AddPane(tb5, wxAuiPaneInfo().
                  Name("tb5").Caption("Sample Vertical Toolbar").
                  ToolbarPane().Left().
                  GripperTop());

    m_mgr.AddPane(new wxButton(this, wxID_ANY, _("Test Button")),
                  wxAuiPaneInfo().Name("tb6").
                  ToolbarPane().Top().Row(2).Position(1).
                  LeftDockable(false).RightDockable(false));

    // make some default perspectives

    wxString perspectiveAll = m_mgr.SavePerspective();

    for ( auto& pane : m_mgr.GetAllPanes() )
    {
        if (!pane.IsToolbar())
            pane.Hide();
    }
    m_mgr.GetPane("tb1").Hide();
    m_mgr.GetPane("tb6").Hide();
    m_mgr.GetPane("test8").Show().Left().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("test10").Show().Bottom().Layer(0).Row(0).Position(0);
    m_mgr.GetPane("notebook_content").Show();
    wxString perspectiveDefault = m_mgr.SavePerspective();

    m_perspectives.Add(perspectiveDefault);
    m_perspectives.Add(perspectiveAll);

    // "commit" all changes made to wxAuiManager
    m_mgr.Update();
}

wxAuiDockArt* MyFrame::GetDockArt()
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

void MyFrame::OnSettings(wxCommandEvent& WXUNUSED(evt))
{
    // show the settings pane, and float it
    wxAuiPaneInfo& floatingPane = m_mgr.GetPane("settings").Float().Show();

    if (floatingPane.floating_pos == wxDefaultPosition)
        floatingPane.FloatingPosition(GetStartPosition());

    m_mgr.Update();
}

void MyFrame::OnCustomizeToolbar(wxCommandEvent& WXUNUSED(evt))
{
    wxMessageBox(_("Customize Toolbar clicked"));
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

    m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, gradient);
    m_mgr.Update();
}

void MyFrame::OnToolbarResizing(wxCommandEvent& WXUNUSED(evt))
{
    for ( auto& pane : m_mgr.GetAllPanes() )
    {
        if ( wxDynamicCast(pane.window, wxAuiToolBar) )
        {
            pane.Resizable(!pane.IsResizable());
        }
    }

    m_mgr.Update();
}

void MyFrame::OnManagerFlag(wxCommandEvent& event)
{
    unsigned int flag = 0;

#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
    if (event.GetId() == ID_TransparentDrag ||
        event.GetId() == ID_TransparentHint ||
        event.GetId() == ID_HintFade)
    {
        wxMessageBox("This option is presently only available on wxGTK, wxMSW and wxMac");
        return;
    }
#endif

    int id = event.GetId();

    if (id == ID_TransparentHint ||
        id == ID_VenetianBlindsHint ||
        id == ID_RectangleHint ||
        id == ID_NoHint)
    {
        unsigned int flags = m_mgr.GetFlags();
        flags &= ~wxAUI_MGR_TRANSPARENT_HINT;
        flags &= ~wxAUI_MGR_VENETIAN_BLINDS_HINT;
        flags &= ~wxAUI_MGR_RECTANGLE_HINT;
        m_mgr.SetFlags(flags);
    }

    switch (id)
    {
        case ID_AllowFloating: flag = wxAUI_MGR_ALLOW_FLOATING; break;
        case ID_TransparentDrag: flag = wxAUI_MGR_TRANSPARENT_DRAG; break;
        case ID_HintFade: flag = wxAUI_MGR_HINT_FADE; break;
        case ID_AllowActivePane: flag = wxAUI_MGR_ALLOW_ACTIVE_PANE; break;
        case ID_TransparentHint: flag = wxAUI_MGR_TRANSPARENT_HINT; break;
        case ID_VenetianBlindsHint: flag = wxAUI_MGR_VENETIAN_BLINDS_HINT; break;
        case ID_RectangleHint: flag = wxAUI_MGR_RECTANGLE_HINT; break;
        case ID_LiveUpdate: flag = wxAUI_MGR_LIVE_RESIZE; break;
    }

    if (flag)
    {
        m_mgr.SetFlags(m_mgr.GetFlags() ^ flag);

        // Propagate the flags to the notebooks too.
        for ( auto& pane : m_mgr.GetAllPanes() )
        {
            if ( auto* const nb = wxDynamicCast(pane.window, wxAuiNotebook) )
            {
                nb->SetManagerFlags(m_mgr.GetFlags());
            }
        }
    }

    m_mgr.Update();
}


void MyFrame::OnNotebookFlag(wxCommandEvent& event)
{
    int id = event.GetId();

    if (id == ID_NotebookNoCloseButton ||
        id == ID_NotebookCloseButton ||
        id == ID_NotebookCloseButtonAll ||
        id == ID_NotebookCloseButtonActive)
    {
        m_notebookStyle &= ~(wxAUI_NB_CLOSE_BUTTON |
                              wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
                              wxAUI_NB_CLOSE_ON_ALL_TABS);

        switch (id)
        {
            case ID_NotebookNoCloseButton: break;
            case ID_NotebookCloseButton: m_notebookStyle |= wxAUI_NB_CLOSE_BUTTON; break;
            case ID_NotebookCloseButtonAll: m_notebookStyle |= wxAUI_NB_CLOSE_ON_ALL_TABS; break;
            case ID_NotebookCloseButtonActive: m_notebookStyle |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB; break;
        }
    }

    if (id == ID_NotebookAllowTabMove)
    {
        m_notebookStyle ^= wxAUI_NB_TAB_MOVE;
    }
    if (id == ID_NotebookAllowTabExternalMove)
    {
        m_notebookStyle ^= wxAUI_NB_TAB_EXTERNAL_MOVE;
    }
     else if (id == ID_NotebookAllowTabSplit)
    {
        m_notebookStyle ^= wxAUI_NB_TAB_SPLIT;
    }
     else if (id == ID_NotebookWindowList)
    {
        m_notebookStyle ^= wxAUI_NB_WINDOWLIST_BUTTON;
    }
     else if (id == ID_NotebookScrollButtons)
    {
        m_notebookStyle ^= wxAUI_NB_SCROLL_BUTTONS;
    }
     else if (id == ID_NotebookTabFixedWidth)
    {
        m_notebookStyle ^= wxAUI_NB_TAB_FIXED_WIDTH;
    }
    else if (id == ID_NotebookTabPin)
    {
        m_notebookStyle ^= wxAUI_NB_PIN_ON_ACTIVE_TAB;
    }
    else if (id == ID_NotebookTabUnpin)
    {
        m_notebookStyle ^= wxAUI_NB_UNPIN_ON_ALL_PINNED;
    }
    else if (id == ID_NotebookMultiLine)
    {
        m_notebookStyle ^= wxAUI_NB_MULTILINE;
    }


    // If we've turned on pin or unpin buttons, check that it's compatible with
    // the currently used tab art.
    if ( m_notebookStyle & (wxAUI_NB_PIN_ON_ACTIVE_TAB |
                            wxAUI_NB_UNPIN_ON_ALL_PINNED) )
    {
        if ( m_notebookTheme == TabArt::Simple )
        {
            wxLogError("Pin icons not supported by the currently used theme.");

            m_notebookStyle &= ~(wxAUI_NB_PIN_ON_ACTIVE_TAB |
                                 wxAUI_NB_UNPIN_ON_ALL_PINNED);
        }
    }

    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( auto* const nb = wxDynamicCast(pane.window, wxAuiNotebook) )
        {
            wxAuiTabArt* art = nullptr;
            if (id == ID_NotebookArtDefault)
            {
                m_notebookTheme = TabArt::Default;
                art = new wxAuiDefaultTabArt;
            }
            else if (id == ID_NotebookArtFlat)
            {
                m_notebookTheme = TabArt::Flat;
                art = new wxAuiFlatTabArt;
            }
#ifdef wxHAS_NATIVE_TABART
            else if (id == ID_NotebookArtNative)
            {
                m_notebookTheme = TabArt::Native;
                art = new wxAuiNativeTabArt;
            }
#endif // wxHAS_NATIVE_TABART
            else if (id == ID_NotebookArtGloss)
            {
                m_notebookTheme = TabArt::Gloss;
                art = new wxAuiGenericTabArt;
            }
            else if (id == ID_NotebookArtSimple)
            {
                if ( m_notebookStyle & (wxAUI_NB_PIN_ON_ACTIVE_TAB |
                                        wxAUI_NB_UNPIN_ON_ALL_PINNED) )
                {
                    // wxAuiSimpleTabArt doesn't have any support for pin or
                    // unpin icons.
                    wxLogError("Simple theme does not support pinned tabs.");
                }
                else
                {
                    m_notebookTheme = TabArt::Simple;
                    art = new wxAuiSimpleTabArt;
                }
            }

            if (art)
            {
                nb->SetArtProvider(art);
            }


            nb->SetWindowStyleFlag(m_notebookStyle);
            nb->Refresh();
        }
    }


}


void MyFrame::OnUpdateUI(wxUpdateUIEvent& event)
{
    unsigned int flags = m_mgr.GetFlags();

    switch (event.GetId())
    {
        case ID_NoGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE);
            break;
        case ID_VerticalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL);
            break;
        case ID_HorizontalGradient:
            event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL);
            break;
        case ID_AllowToolbarResizing:
        {
            for ( const auto& pane : m_mgr.GetAllPanes() )
            {
                if ( wxDynamicCast(pane.window, wxAuiToolBar) )
                {
                    event.Check(pane.IsResizable());
                    break;
                }
            }
            break;
        }
        case ID_AllowFloating:
            event.Check((flags & wxAUI_MGR_ALLOW_FLOATING) != 0);
            break;
        case ID_TransparentDrag:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_DRAG) != 0);
            break;
        case ID_TransparentHint:
            event.Check((flags & wxAUI_MGR_TRANSPARENT_HINT) != 0);
            break;
        case ID_LiveUpdate:
            event.Check((flags & wxAUI_MGR_LIVE_RESIZE) != 0);
            break;
        case ID_VenetianBlindsHint:
            event.Check((flags & wxAUI_MGR_VENETIAN_BLINDS_HINT) != 0);
            break;
        case ID_RectangleHint:
            event.Check((flags & wxAUI_MGR_RECTANGLE_HINT) != 0);
            break;
        case ID_NoHint:
            event.Check(((wxAUI_MGR_TRANSPARENT_HINT |
                          wxAUI_MGR_VENETIAN_BLINDS_HINT |
                          wxAUI_MGR_RECTANGLE_HINT) & flags) == 0);
            break;
        case ID_HintFade:
            event.Check((flags & wxAUI_MGR_HINT_FADE) != 0);
            break;

        case ID_NotebookNoCloseButton:
            event.Check((m_notebookStyle & (wxAUI_NB_CLOSE_BUTTON|wxAUI_NB_CLOSE_ON_ALL_TABS|wxAUI_NB_CLOSE_ON_ACTIVE_TAB)) == 0);
            break;
        case ID_NotebookCloseButton:
            event.Check((m_notebookStyle & wxAUI_NB_CLOSE_BUTTON) != 0);
            break;
        case ID_NotebookCloseButtonAll:
            event.Check((m_notebookStyle & wxAUI_NB_CLOSE_ON_ALL_TABS) != 0);
            break;
        case ID_NotebookCloseButtonActive:
            event.Check((m_notebookStyle & wxAUI_NB_CLOSE_ON_ACTIVE_TAB) != 0);
            break;
        case ID_NotebookAllowTabSplit:
            event.Check((m_notebookStyle & wxAUI_NB_TAB_SPLIT) != 0);
            break;
        case ID_NotebookAllowTabMove:
            event.Check((m_notebookStyle & wxAUI_NB_TAB_MOVE) != 0);
            break;
        case ID_NotebookAllowTabExternalMove:
            event.Check((m_notebookStyle & wxAUI_NB_TAB_EXTERNAL_MOVE) != 0);
            break;
        case ID_NotebookScrollButtons:
            event.Check((m_notebookStyle & wxAUI_NB_SCROLL_BUTTONS) != 0);
            break;
        case ID_NotebookWindowList:
            event.Check((m_notebookStyle & wxAUI_NB_WINDOWLIST_BUTTON) != 0);
            break;
        case ID_NotebookTabFixedWidth:
            event.Check((m_notebookStyle & wxAUI_NB_TAB_FIXED_WIDTH) != 0);
            break;
        case ID_NotebookTabPin:
            event.Check((m_notebookStyle & wxAUI_NB_PIN_ON_ACTIVE_TAB) != 0);
            break;
        case ID_NotebookTabUnpin:
            event.Check((m_notebookStyle & wxAUI_NB_UNPIN_ON_ALL_PINNED) != 0);
            break;
        case ID_NotebookMultiLine:
            event.Check((m_notebookStyle & wxAUI_NB_MULTILINE) != 0);
            break;
        case ID_NotebookArtDefault:
            event.Check(m_notebookTheme == TabArt::Default);
            break;
        case ID_NotebookArtFlat:
            event.Check(m_notebookTheme == TabArt::Flat);
            break;
#ifdef wxHAS_NATIVE_TABART
        case ID_NotebookArtNative:
            event.Check(m_notebookTheme == TabArt::Native);
            break;
#endif // wxHAS_NATIVE_TABART
        case ID_NotebookArtGloss:
            event.Check(m_notebookTheme == TabArt::Gloss);
            break;
        case ID_NotebookArtSimple:
            event.Check(m_notebookTheme == TabArt::Simple);
            break;

    }
}

void MyFrame::OnUpdateTabKindUI(wxUpdateUIEvent& event)
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    if ( !book )
    {
        event.Enable(false);
        return;
    }

    const auto sel = book->GetSelection();
    if ( sel == wxNOT_FOUND )
    {
        event.Enable(false);
        return;
    }

    const int kind = static_cast<int>(book->GetPageKind(sel));

    event.Check(kind == event.GetId() - ID_NotebookNormalTab);
}

void MyFrame::OnNotebookSetTabKind(wxCommandEvent& event)
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    if ( !book )
        return;

    const auto sel = book->GetSelection();
    if ( sel == wxNOT_FOUND )
        return;

    book->SetPageKind(sel,
        static_cast<wxAuiTabKind>(event.GetId() - ID_NotebookNormalTab)
    );
}


void MyFrame::OnNotebookNextOrPrev(wxCommandEvent& evt)
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    book->AdvanceSelection(evt.GetId() == ID_NotebookNextTab);
}


void MyFrame::OnNotebookSplit(wxCommandEvent& WXUNUSED(evt))
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    if ( !book || book->GetPageCount() < 3 )
    {
        wxLogWarning("Not enough pages to split.");
        return;
    }

    book->Split(0, wxRIGHT);
    book->Split(1, wxRIGHT);
    book->Split(2, wxBOTTOM);
}

void MyFrame::OnNotebookUnsplit(wxCommandEvent& WXUNUSED(evt))
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    book->UnsplitAll();
}

void MyFrame::OnNotebookNewTab(wxCommandEvent& WXUNUSED(evt))
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    book->AddPage(new wxTextCtrl(book, wxID_ANY, "New Tab",
                                 wxDefaultPosition, wxDefaultSize,
                                 wxTE_MULTILINE | wxNO_BORDER),
                  wxString::Format("Tab %zu", book->GetPageCount() + 1),
                  true /* select */);
}


void MyFrame::OnNotebookDeleteTab(wxCommandEvent& WXUNUSED(evt))
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    auto numPages = book->GetPageCount();
    if ( !numPages )
    {
        wxLogWarning("No pages to delete.");
        return;
    }

    book->DeletePage(numPages - 1);
}


void MyFrame::OnPaneClose(wxAuiManagerEvent& evt)
{
    if (evt.pane->name == "test10")
    {
        int res = wxMessageBox("Are you sure you want to close/hide this pane?",
                               "wxAUI",
                               wxYES_NO,
                               this);
        if (res != wxYES)
            evt.Veto();
    }
}

// copy state from user-controllable checkbox to disabled checkbox
void MyFrame::OnCheckboxUpdateUI(wxUpdateUIEvent& evt)
{
#if wxUSE_CHECKBOX
    wxASSERT(evt.IsCheckable());
    wxWindow* tb4 = m_mgr.GetPane("tb4").window;
    wxWindow* wnd = tb4->FindWindow(ID_3CHECK);
    wxCheckBox* src = wxCheckCast<wxCheckBox>(wnd);
    if (!evt.Is3State())
    {
        if (src->Get3StateValue() != wxCHK_UNDETERMINED)
        {
            evt.Show(true);
            evt.Check(src->Get3StateValue() != wxCHK_UNCHECKED);
        }
        else
        {
            evt.Show(false);
        }
    }
    else
    {
        evt.Set3StateValue(src->Get3StateValue());
    }
#endif
}

void MyFrame::OnCreatePerspective(wxCommandEvent& WXUNUSED(event))
{
    wxTextEntryDialog dlg(this, "Enter a name for the new perspective:",
                          "wxAUI Test");

    dlg.SetValue(wxString::Format("Perspective %u", unsigned(m_perspectives.GetCount() + 1)));
    if (dlg.ShowModal() != wxID_OK)
        return;

    if (m_perspectives.GetCount() == 0)
    {
        m_perspectivesMenu->AppendSeparator();
    }

    m_perspectivesMenu->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
    m_perspectives.Add(m_mgr.SavePerspective());
}

// Sample serializer and deserializer implementations for saving and loading layouts.
class MyXmlSerializer : public wxAuiSerializer
{
public:
    MyXmlSerializer() = default;

    wxString GetXML() const
    {
        wxStringOutputStream oss;
        if ( !m_doc.Save(oss) )
        {
            wxLogError("Failed to save XML document.");
        }

        return oss.GetString();
    }

    // Implement wxAuiSerializer methods.
    virtual void BeforeSave() override
    {
        m_root = new wxXmlNode(wxXML_ELEMENT_NODE, "aui-layout");
        m_root->AddAttribute("version", "3.3.0");

        m_doc.SetRoot(m_root);
    }

    virtual void BeforeSavePanes() override
    {
        m_panes.reset(new wxXmlNode(wxXML_ELEMENT_NODE, "panes"));
    }

    virtual void SavePane(const wxAuiPaneLayoutInfo& pane) override
    {
        auto node = new wxXmlNode(wxXML_ELEMENT_NODE, "pane");
        node->AddAttribute("name", pane.name);

        AddDockLayout(node, pane);

        AddChild(node, "floating-rect",
                 wxRect(pane.floating_pos, pane.floating_size));
        AddChild(node, "floating-client-rect",
                 wxRect(pane.floating_pos, pane.floating_client_size));

        // Don't bother creating many "maximized" nodes with value 0 when we
        // can have at most one of them with value 1.
        if ( pane.is_maximized )
            AddChild(node, "maximized", 1);

        // Also don't mark visible pages (as most of them are) as being so.
        if ( pane.is_hidden )
            AddChild(node, "hidden", 1);

        m_panes->AddChild(node);
    }

    virtual void AfterSavePanes() override
    {
        m_root->AddChild(m_panes.release());
    }


    virtual void BeforeSaveNotebooks() override
    {
        m_books.reset(new wxXmlNode(wxXML_ELEMENT_NODE, "notebooks"));
    }

    virtual void BeforeSaveNotebook(const wxString& name) override
    {
        m_book.reset(new wxXmlNode(wxXML_ELEMENT_NODE, "notebook"));
        m_book->AddAttribute("name", name);
    }

    virtual void SaveNotebookTabControl(const wxAuiTabLayoutInfo& tab) override
    {
        auto node = new wxXmlNode(wxXML_ELEMENT_NODE, "tab");

        AddDockLayout(node, tab);

        AddPagesList(node, "pages", tab.pages);
        AddPagesList(node, "pinned", tab.pinned);
        AddChild(node, "active", tab.active);

        m_book->AddChild(node);
    }

    virtual void AfterSaveNotebook() override
    {
        m_books->AddChild(m_book.release());
    }

    virtual void AfterSaveNotebooks() override
    {
        m_root->AddChild(m_books.release());
    }


    virtual void AfterSave() override {}

private:
    void AddChild(wxXmlNode* parent, const wxString& name, const wxString& value)
    {
        auto node = new wxXmlNode(parent, wxXML_ELEMENT_NODE, name);
        node->AddChild(new wxXmlNode(wxXML_TEXT_NODE, {}, value));
    }

    void AddChild(wxXmlNode* parent, const wxString& name, int value)
    {
        // Don't save 0 values, they're the default.
        if ( value )
            AddChild(parent, name, wxString::Format("%u", value));
    }

    void AddChild(wxXmlNode* parent, const wxString& name, const wxRect& rect)
    {
        if ( rect.GetPosition() != wxDefaultPosition ||
                rect.GetSize() != wxDefaultSize )
        {
            AddChild(parent, name,
                     wxString::Format("%d,%d %dx%d",
                                      rect.x, rect.y, rect.width, rect.height));
        }
    }

    // Common helper of SavePane() and SaveNotebookTabControl() which both need
    // to save the same layout information.
    void AddDockLayout(wxXmlNode* node, const wxAuiDockLayoutInfo& layout)
    {
        AddChild(node, "direction", layout.dock_direction);
        AddChild(node, "layer", layout.dock_layer);
        AddChild(node, "row", layout.dock_row);
        AddChild(node, "position", layout.dock_pos);
        AddChild(node, "proportion", layout.dock_proportion);
        AddChild(node, "size", layout.dock_size);
    }

    // Helper of SaveNotebookTabControl(): add a node with the given name
    // containing the comma-separated list of page indices if there are any.
    void
    AddPagesList(wxXmlNode* node,
                 const wxString& name,
                 const std::vector<int>& pages)
    {
        if ( !pages.empty() )
        {
            wxString pagesList;
            for ( auto page : pages )
            {
                if ( !pagesList.empty() )
                    pagesList << ',';

                pagesList << page;
            }

            AddChild(node, name, pagesList);
        }
    }


    wxXmlDocument m_doc;

    // Non-owning pointer to the root node of m_doc.
    wxXmlNode* m_root = nullptr;

    // The other pointers are only set temporarily, until they are added to the
    // document -- this ensures that we don't leak memory if an exception is
    // thrown before this happens.
    std::unique_ptr<wxXmlNode> m_panes;
    std::unique_ptr<wxXmlNode> m_books;
    std::unique_ptr<wxXmlNode> m_book;
};

class MyXmlDeserializer : public wxAuiDeserializer
{
public:
    explicit MyXmlDeserializer(wxAuiManager& manager, const wxString& xml)
        : wxAuiDeserializer(manager)
    {
        wxStringInputStream iss(xml);
        if ( !m_doc.Load(iss) )
            throw std::runtime_error("Failed to load XML document.");

        const auto root = m_doc.GetDocumentNode();
        const auto layout = root->GetChildren();
        if ( !layout )
            throw std::runtime_error("Missing layout node");
        if ( layout->GetName() != "aui-layout" )
            throw std::runtime_error("Unexpected XML node name");
        if ( layout->GetAttribute("version") != "3.3.0" )
            throw std::runtime_error("Unexpected XML version");
        if ( layout->GetNext() )
            throw std::runtime_error("Unexpected multiple layout nodes");

        // Check that we only have the top level nodes that we expect.
        //
        // This is nice to detect errors in this sample, but note that this
        // might not be the best strategy for a real application, which might
        // decide to to gracefully ignore unknown nodes instead of failing, or
        // at least save the format version in the XML file to be able to give
        // a better error message.
        for ( wxXmlNode* node = layout->GetChildren(); node; node = node->GetNext() )
        {
            if ( node->GetName() == "panes" )
            {
                if ( m_panes )
                    throw std::runtime_error("Unexpected multiple panes nodes");

                m_panes = node;
            }
            else if ( node->GetName() == "notebooks" )
            {
                if ( m_books )
                    throw std::runtime_error("Unexpected multiple notebooks nodes");

                m_books = node;
            }
            else
            {
                throw std::runtime_error("Unexpected node name");
            }
        }
    }

    // Implement wxAuiDeserializer methods.
    virtual std::vector<wxAuiPaneLayoutInfo> LoadPanes() override
    {
        std::vector<wxAuiPaneLayoutInfo> panes;

        for ( wxXmlNode* node = m_panes->GetChildren(); node; node = node->GetNext() )
        {
            if ( node->GetName() != "pane" )
                throw std::runtime_error("Unexpected pane node name");

            {
                wxAuiPaneLayoutInfo pane{node->GetAttribute("name")};

                for ( wxXmlNode* child = node->GetChildren(); child; child = child->GetNext() )
                {
                    if ( LoadDockLayout(child, pane) )
                        continue;

                    const wxString& name = child->GetName();
                    const wxString& content = child->GetNodeContent();

                    if ( name == "floating-rect" )
                    {
                        auto rect = GetRect(content);

                        pane.floating_pos = rect.GetPosition();
                        pane.floating_size = rect.GetSize();
                    }
                    else if ( name == "floating-client-rect" )
                    {
                        auto rect = GetRect(content);

                        pane.floating_pos = rect.GetPosition();
                        pane.floating_client_size = rect.GetSize();
                    }
                    else if ( name == "maximized" )
                    {
                        pane.is_maximized = GetInt(content) != 0;
                    }
                    else if ( name == "hidden" )
                    {
                        pane.is_hidden = GetInt(content) != 0;
                    }
                    else
                    {
                        throw std::runtime_error("Unexpected pane child node name");
                    }
                }

                panes.push_back(pane);
            }
        }

        return panes;
    }

    virtual std::vector<wxAuiTabLayoutInfo>
    LoadNotebookTabs(const wxString& name) override
    {
        // Find the notebook with the given name.
        for ( wxXmlNode* node = m_books->GetChildren(); node; node = node->GetNext() )
        {
            if ( node->GetName() != "notebook" )
                throw std::runtime_error("Unexpected notebook node name");

            if ( node->GetAttribute("name") == name )
                return LoadNotebookTabs(node);
        }

        // As above, this might not be the best thing to do in a real
        // application, where, perhaps, the XML file was saved by a newer
        // version of the problem, but here we do this for simplicity and to
        // make sure we detect any errors.
        throw std::runtime_error("Notebook with the given name not found");
    }

    // Overriding this function is optional and normally it is not going to be
    // called at all. We only do it here to show the different possibilities,
    // but the serialized pages need to be manually edited to see them.
    virtual bool
    HandleOrphanedPage(wxAuiNotebook& book,
                       int page,
                       wxAuiTabCtrl** tabCtrl,
                       int* tabIndex) override
    {
        // If the first ("Welcome") page is not attached, insert it in the
        // beginning of the main tab control.
        if ( page == 0 )
        {
            // We don't need to change tabCtrl, it's set to the main one by
            // default, but check that this is indeed the case.
            if ( *tabCtrl != book.GetMainTabCtrl() )
            {
                wxLogWarning("Unexpected tab control for an orphaned page!");
                return false;
            }

            *tabIndex = 0;

            return true;
        }

        // This is completely artificial, but just remove the pages after the
        // third one if they are not attached to any tab control.
        return page <= 2;
    }

    virtual wxWindow* CreatePaneWindow(wxAuiPaneInfo& pane) override
    {
        wxLogWarning("Unknown pane \"%s\"", pane.name);
        return nullptr;
    }

private:
    int GetInt(const wxString& str)
    {
        int value;
        if ( !str.ToInt(&value) )
            throw std::runtime_error("Failed to parse integer");

        return value;
    }

    wxSize GetSize(const wxString& str)
    {
        wxString strH;
        const wxString strW = str.BeforeFirst('x', &strH);

        // Special case which wouldn't be parse by ToUInt() below.
        if ( strW == "-1" && strH == strW )
            return wxDefaultSize;

        unsigned int w, h;
        if ( !strW.ToUInt(&w) || !strH.ToUInt(&h) )
            throw std::runtime_error("Failed to parse size");

        return wxSize(w, h);
    }

    wxRect GetRect(const wxString& str)
    {
        wxString strWH;
        const wxString strXY = str.BeforeFirst(' ', &strWH);

        wxString strY;
        const wxString strX = strXY.BeforeFirst(',', &strY);

        int x, y;
        if ( !strX.ToInt(&x) || !strY.ToInt(&y) )
            throw std::runtime_error("Failed to parse position");

        return wxRect(wxPoint(x, y), GetSize(strWH));
    }

    // Common helper of LoadPanes() and LoadNotebookTabs() which both need to
    // load the dock layout information.
    //
    // Returns true if we processed this node.
    bool LoadDockLayout(wxXmlNode* node, wxAuiDockLayoutInfo& info)
    {
        const wxString& name = node->GetName();
        const wxString& content = node->GetNodeContent();

        if ( name == "direction" )
        {
            info.dock_direction = GetInt(content);
        }
        else if ( name == "layer" )
        {
            info.dock_layer = GetInt(content);
        }
        else if ( name == "row" )
        {
            info.dock_row = GetInt(content);
        }
        else if ( name == "position" )
        {
            info.dock_pos = GetInt(content);
        }
        else if ( name == "proportion" )
        {
            info.dock_proportion = GetInt(content);
        }
        else if ( name == "size" )
        {
            info.dock_size = GetInt(content);
        }
        else
        {
            return false;
        }

        return true;
    }

    std::vector<wxAuiTabLayoutInfo> LoadNotebookTabs(wxXmlNode* book)
    {
        std::vector<wxAuiTabLayoutInfo> tabs;

        for ( wxXmlNode* node = book->GetChildren(); node; node = node->GetNext() )
        {
            if ( node->GetName() != "tab" )
                throw std::runtime_error("Unexpected tab node name");

            wxAuiTabLayoutInfo tab;
            for ( wxXmlNode* child = node->GetChildren(); child; child = child->GetNext() )
            {
                if ( LoadDockLayout(child, tab) )
                    continue;

                const auto& pageIndices = wxSplit(child->GetNodeContent(), ',');
                if ( child->GetName() == "pages" )
                {
                    for ( const auto& s : pageIndices )
                        tab.pages.push_back(GetInt(s));
                }
                else if ( child->GetName() == "pinned" )
                {
                    for ( const auto& s : pageIndices )
                        tab.pinned.push_back(GetInt(s));
                }
                else if ( child->GetName() == "active" )
                {
                    tab.active = GetInt(child->GetNodeContent());
                }
                else
                {
                    throw std::runtime_error("Unexpected tab child node name");
                }
            }

            tabs.push_back(tab);
        }

        return tabs;
    }


    wxXmlDocument m_doc;

    // Non-owning pointers to the nodes in m_doc.
    wxXmlNode* m_panes = nullptr;
    wxXmlNode* m_books = nullptr;
};

void MyFrame::OnCopyLayout(wxCommandEvent& WXUNUSED(evt))
{
    MyXmlSerializer ser;
    m_mgr.SaveLayout(ser);

#if wxUSE_CLIPBOARD
    wxClipboardLocker clipLock;

    wxTheClipboard->SetData(new wxTextDataObject(ser.GetXML()));
#endif // wxUSE_CLIPBOARD
}

void MyFrame::OnPasteLayout(wxCommandEvent& WXUNUSED(evt))
{
#if wxUSE_CLIPBOARD
    wxClipboardLocker clipLock;

    wxTextDataObject data;
    if ( !wxTheClipboard->GetData(data) )
    {
        wxLogError("Failed to get XML data from clipboard.");
        return;
    }

    try
    {
        MyXmlDeserializer deser(m_mgr, data.GetText());
        m_mgr.LoadLayout(deser);
    }
    catch ( const std::exception& e )
    {
        wxLogError("Failed to load layout: %s", e.what());
    }
#else
    wxLogError("This menu command requires wxUSE_CLIPBOARD.");
#endif
}

void MyFrame::OnEditNotebookLayout(wxCommandEvent& WXUNUSED(event))
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    MyXmlSerializer ser;

    // This is a hack, but it allows us to reuse the full serializer without
    // duplicating its code.
    ser.BeforeSave();
    ser.BeforeSaveNotebooks();

    // The name here doesn't need to be the same as the notebook name, even
    // though it usually would be.
    book->SaveLayout("notebook", ser);

    // Second part of the hack above.
    ser.AfterSaveNotebooks();
    ser.AfterSave();

    // In a real application, we would save this XML string somewhere and
    // restore it during the next run, but here we just show it and allow
    // editing it interactively to test how changing it affect the layout.
    wxTextEntryDialog dlg(
        this,
        "Current notebook layout (edit and press OK to apply):",
        "wxAUI Sample",
        ser.GetXML(),
        wxOK | wxCANCEL | wxTE_MULTILINE
    );

    if ( dlg.ShowModal() != wxID_OK )
        return;

    try
    {
        MyXmlDeserializer deser(m_mgr, dlg.GetValue());
        book->LoadLayout("notebook", deser);
    }
    catch ( const std::exception& e )
    {
        wxLogError("Failed to load notebook layout: %s", e.what());
    }
}

void MyFrame::OnRestorePerspective(wxCommandEvent& evt)
{
    m_mgr.LoadPerspective(m_perspectives.Item(evt.GetId() - ID_FirstPerspective));
}

void MyFrame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(wxHtmlWindow)))
    {
        int res = wxMessageBox("Are you sure you want to close/hide this notebook page?",
                       "wxAUI",
                       wxYES_NO,
                       this);
        if (res != wxYES)
            evt.Veto();
    }
}

void MyFrame::OnNotebookPageClosed(wxAuiNotebookEvent& evt)
{
    wxAuiNotebook* ctrl = (wxAuiNotebook*)evt.GetEventObject();
    wxUnusedVar(ctrl);

    // selection should always be a valid index
    wxASSERT_MSG( ctrl->GetSelection() < (int)ctrl->GetPageCount(),
                  wxString::Format("Invalid selection %d, only %d pages left",
                                   ctrl->GetSelection(),
                                   (int)ctrl->GetPageCount()) );

    evt.Skip();
}

void MyFrame::OnNotebookPageChanging(wxAuiNotebookEvent& evt)
{
    if ( evt.GetOldSelection() == 3 )
    {
        if ( wxMessageBox( "Are you sure you want to leave this page?\n"
                           "(This demonstrates veto-ing)",
                           "wxAUI",
                           wxICON_QUESTION | wxYES_NO,
                           this ) != wxYES )
        {
            evt.Veto();
        }
    }
}

void MyFrame::OnNotebookTabRightClick(wxAuiNotebookEvent& evt)
{
    wxPoint pt = wxGetMousePosition();

    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    const auto page = evt.GetSelection();
    const auto pageUnderMouse = book->HitTest(book->ScreenToClient(pt));
    if ( pageUnderMouse != page )
    {
        wxLogWarning("Unexpected mismatch: page under mouse is %d (position %d)",
                     pageUnderMouse, book->GetPagePosition(pageUnderMouse).tabIdx);
    }

    wxLogMessage("Right click on page %d (tab position %d)",
                 page, book->GetPagePosition(page).tabIdx);
}

void MyFrame::OnNotebookTabBackgroundDClick(wxAuiNotebookEvent& WXUNUSED(evt))
{
    auto* const book =
        wxCheckCast<wxAuiNotebook>(m_mgr.GetPane("notebook_content").window);

    // Show notebook pages in per-tab visual page order.
    const auto sel = static_cast<size_t>(book->GetSelection());

    wxString pages("Notebook contains the following pages:\n");

    const auto& tabControls = book->GetAllTabCtrls();
    int tab = 0;
    for ( const auto tabCtrl : tabControls )
    {
        if ( tabControls.size() > 1 )
        {
            pages += wxString::Format("\nTab %d", ++tab);

            wxArrayString extras;
            if ( tabCtrl == book->GetMainTabCtrl() )
                extras.push_back("main");

            if ( tabCtrl == book->GetActiveTabCtrl() )
                extras.push_back("active");

            if ( !extras.empty() )
                pages += wxString::Format(" (%s)", wxJoin(extras, ','));

            pages += ":\n";
        }

        int pos = 0;
        for ( auto idx : book->GetPagesInDisplayOrder(tabCtrl) )
        {
            wxString kind;
            switch ( book->GetPageKind(idx) )
            {
                case wxAuiTabKind::Normal:
                    kind = "   "; // Don't show anything for normal tabs.
                    break;

                case wxAuiTabKind::Pinned:
                    kind = wxString::FromUTF8("\xf0\x9f\x93\x8c"); // U+1F4CC
                    break;

                case wxAuiTabKind::Locked:
                    kind = wxString::FromUTF8("\xf0\x9f\x94\x92"); // U+1F512
                    break;
            }

            pages += wxString::Format("%s%s%d. %s\n",
                                      idx == sel ? "*" : "  ",
                                      kind,
                                      pos++,
                                      book->GetPageText(idx));
        }
    }

    pages += wxString::Format("\n* selected");

    wxMessageBox(pages, "wxAUI", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
    // for the purpose of this test application, explicitly
    // allow all notebook drag and drop events
    evt.Allow();
}

wxPoint MyFrame::GetStartPosition()
{
    static int x = 0;
    x += FromDIP(20);
    wxPoint pt = ClientToScreen(wxPoint(0,0));
    return wxPoint(pt.x + x, pt.y + x);
}

void MyFrame::OnCreateTree(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
                  Caption("Tree Control").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(150,300))));
    m_mgr.Update();
}

void MyFrame::OnCreateGrid(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().
                  Caption("Grid").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(300,200))));
    m_mgr.Update();
}

void MyFrame::OnCreateHTML(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().
                  Caption("HTML Control").
                  Float().FloatingPosition(GetStartPosition()).
                  FloatingSize(FromDIP(wxSize(300,200))));
    m_mgr.Update();
}

void MyFrame::OnCreateNotebook(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().
                  Caption("Notebook").
                  Float().FloatingPosition(GetStartPosition()).
                  //FloatingSize(FromDIP(wxSize(300,200))).
                  CloseButton(true).MaximizeButton(true));
    m_mgr.Update();
}

void MyFrame::OnCreateText(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().
                  Caption("Text Control").
                  Float().FloatingPosition(GetStartPosition()));
    m_mgr.Update();
}

void MyFrame::OnCreateSizeReport(wxCommandEvent& WXUNUSED(event))
{
    m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
                  Caption("Client Size Reporter").
                  Float().FloatingPosition(GetStartPosition()).
                  CloseButton(true).MaximizeButton(true));
    m_mgr.Update();
}

void MyFrame::OnChangeContentPane(wxCommandEvent& evt)
{
    m_mgr.GetPane("grid_content").Show(evt.GetId() == ID_GridContent);
    m_mgr.GetPane("text_content").Show(evt.GetId() == ID_TextContent);
    m_mgr.GetPane("tree_content").Show(evt.GetId() == ID_TreeContent);
    m_mgr.GetPane("sizereport_content").Show(evt.GetId() == ID_SizeReportContent);
    m_mgr.GetPane("html_content").Show(evt.GetId() == ID_HTMLContent);
    m_mgr.GetPane("notebook_content").Show(evt.GetId() == ID_NotebookContent);
    m_mgr.Update();
}

void MyFrame::OnDropDownToolbarItem(wxAuiToolBarEvent& evt)
{
    if (evt.IsDropDownClicked())
    {
        wxAuiToolBar* tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());

        tb->SetToolSticky(evt.GetId(), true);

        // create the popup menu
        wxMenu menuPopup;

        // TODO: Use GetBitmapBundle() when wxMenuItem is updated to use it too.
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, FromDIP(wxSize(16,16)));

        wxMenuItem* m1 =  new wxMenuItem(&menuPopup, 10001, _("Drop Down Item 1"));
        m1->SetBitmap(bmp);
        menuPopup.Append(m1);

        wxMenuItem* m2 =  new wxMenuItem(&menuPopup, 10002, _("Drop Down Item 2"));
        m2->SetBitmap(bmp);
        menuPopup.Append(m2);

        wxMenuItem* m3 =  new wxMenuItem(&menuPopup, 10003, _("Drop Down Item 3"));
        m3->SetBitmap(bmp);
        menuPopup.Append(m3);

        wxMenuItem* m4 =  new wxMenuItem(&menuPopup, 10004, _("Drop Down Item 4"));
        m4->SetBitmap(bmp);
        menuPopup.Append(m4);

        // line up our menu with the button
        wxRect rect = tb->GetToolRect(evt.GetId());
        wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
        pt = ScreenToClient(pt);


        PopupMenu(&menuPopup, pt);


        // make sure the button is "un-stuck"
        tb->SetToolSticky(evt.GetId(), false);
    }
}


void MyFrame::OnTabAlignment(wxCommandEvent &evt)
{
    for ( const auto& pane : m_mgr.GetAllPanes() )
    {
        if ( auto* const nb = wxDynamicCast(pane.window, wxAuiNotebook) )
        {
            long style = nb->GetWindowStyleFlag();
            style &= ~(wxAUI_NB_TOP | wxAUI_NB_BOTTOM);
            if (evt.GetId() == ID_NotebookAlignTop)
                style |= wxAUI_NB_TOP;
            else if (evt.GetId() == ID_NotebookAlignBottom)
                style |= wxAUI_NB_BOTTOM;
            nb->SetWindowStyleFlag(style);

            nb->Refresh();
        }
    }
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(_("wxAUI Demo\nAn advanced window management library for wxWidgets\n(c) Copyright 2005-2006, Kirix Corporation"), _("About wxAUI Demo"), wxOK, this);
}

wxTextCtrl* MyFrame::CreateTextCtrl(const wxString& ctrlText)
{
    static int n = 0;

    wxString text;
    if ( !ctrlText.empty() )
        text = ctrlText;
    else
        text.Printf("This is text box %d", ++n);

    return new wxTextCtrl(this,wxID_ANY, text,
                          wxPoint(0,0), FromDIP(wxSize(150,90)),
                          wxNO_BORDER | wxTE_MULTILINE);
}


wxGrid* MyFrame::CreateGrid()
{
    wxGrid* grid = new wxGrid(this, wxID_ANY,
                              wxPoint(0,0),
                              FromDIP(wxSize(150,250)),
                              wxNO_BORDER | wxWANTS_CHARS);
    grid->CreateGrid(50, 20);
    return grid;
}

wxTreeCtrl* MyFrame::CreateTreeCtrl()
{
    wxTreeCtrl* tree = new wxTreeCtrl(this, wxID_ANY,
                                      wxPoint(0,0),
                                      FromDIP(wxSize(160,250)),
                                      wxTR_DEFAULT_STYLE | wxNO_BORDER);

    wxSize size(16, 16);
    wxVector<wxBitmapBundle> images;
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_FOLDER, wxART_OTHER, size));
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, size));
    tree->SetImages(images);

    wxTreeItemId root = tree->AddRoot("wxAUI Project", 0);
    wxArrayTreeItemIds items;



    items.Add(tree->AppendItem(root, "Item 1", 0));
    items.Add(tree->AppendItem(root, "Item 2", 0));
    items.Add(tree->AppendItem(root, "Item 3", 0));
    items.Add(tree->AppendItem(root, "Item 4", 0));
    items.Add(tree->AppendItem(root, "Item 5", 0));


    int i, count;
    for (i = 0, count = items.Count(); i < count; ++i)
    {
        wxTreeItemId id = items.Item(i);
        tree->AppendItem(id, "Subitem 1", 1);
        tree->AppendItem(id, "Subitem 2", 1);
        tree->AppendItem(id, "Subitem 3", 1);
        tree->AppendItem(id, "Subitem 4", 1);
        tree->AppendItem(id, "Subitem 5", 1);
    }


    tree->Expand(root);

    return tree;
}

wxSizeReportCtrl* MyFrame::CreateSizeReportCtrl(const wxSize& size)
{
    wxSizeReportCtrl* ctrl = new wxSizeReportCtrl(this, wxID_ANY,
                                   wxDefaultPosition,
                                   size, &m_mgr);
    return ctrl;
}

wxHtmlWindow* MyFrame::CreateHTMLCtrl(wxWindow* parent)
{
    if (!parent)
        parent = this;

    wxHtmlWindow* ctrl = new wxHtmlWindow(parent, wxID_ANY,
                                   wxDefaultPosition,
                                   FromDIP(wxSize(400,300)));
    ctrl->SetPage(GetIntroText());
    return ctrl;
}

wxAuiNotebook* MyFrame::CreateNotebook()
{
   // create the notebook off-window to avoid flicker
   wxSize clientSize = GetClientSize();

   wxAuiNotebook* ctrl = new wxAuiNotebook(this, wxID_ANY,
                                    wxPoint(clientSize.x, clientSize.y),
                                    FromDIP(wxSize(430,200)),
                                    m_notebookStyle);
   ctrl->Freeze();

   wxBitmapBundle pageBmp = wxArtProvider::GetBitmapBundle(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));

   ctrl->AddPage(CreateHTMLCtrl(ctrl), "Welcome to wxAUI" , false, pageBmp);
   ctrl->SetPageKind(0, wxAuiTabKind::Locked);
   ctrl->SetPageToolTip(0, "Note that this page is locked and can't be moved or closed");

   wxPanel *panel = new wxPanel( ctrl, wxID_ANY );
   wxFlexGridSizer *flex = new wxFlexGridSizer( 4, 2, 0, 0 );
   flex->AddGrowableRow( 0 );
   flex->AddGrowableRow( 3 );
   flex->AddGrowableCol( 1 );
   flex->Add( FromDIP(5), FromDIP(5) );   flex->Add( FromDIP(5), FromDIP(5) );
   flex->Add( new wxStaticText( panel, -1, "wxTextCtrl:" ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxTextCtrl( panel, -1, "", wxDefaultPosition, FromDIP(wxSize(100,-1))),
                1, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxStaticText( panel, -1, "wxSpinCtrl:" ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( new wxSpinCtrl( panel, -1, "5", wxDefaultPosition, wxDefaultSize,
                wxSP_ARROW_KEYS, 5, 50, 5 ), 0, wxALL|wxALIGN_CENTRE, FromDIP(5) );
   flex->Add( FromDIP(5), FromDIP(5) );   flex->Add( FromDIP(5), FromDIP(5) );
   panel->SetSizer( flex );
   ctrl->AddPage( panel, "wxPanel", false, pageBmp );


   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 1", false, pageBmp );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 2" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 4" );

   ctrl->InsertPage( 4, new wxTextCtrl( ctrl, wxID_ANY, "Page inserted before another one",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 3" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 5" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 6" );

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 7 (longer title)" );
   ctrl->SetPageToolTip(ctrl->GetPageCount()-1,
                        "wxTextCtrl 7: and the tooltip message can be even longer!");

   ctrl->AddPage( new wxTextCtrl( ctrl, wxID_ANY, "Some more text",
                wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxNO_BORDER) , "wxTextCtrl 8" );

   ctrl->Thaw();
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
        "<li>Customizable floating/docking behaviour</li>"
        "<li>Completely customizable look-and-feel</li>"
        "<li>Optional transparent window effects (while dragging or docking)</li>"
        "<li>Splittable notebook control</li>"
        "</ul>"
        "<p><b>What's new in 0.9.4?</b></p>"
        "<p>wxAUI 0.9.4, which is bundled with wxWidgets, adds the following features:"
        "<ul>"
        "<li>New wxAuiToolBar class, a toolbar control which integrates more "
        "cleanly with wxAuiFrameManager.</li>"
        "<li>Lots of bug fixes</li>"
        "</ul>"
        "<p><b>What's new in 0.9.3?</b></p>"
        "<p>wxAUI 0.9.3, which is now bundled with wxWidgets, adds the following features:"
        "<ul>"
        "<li>New wxAuiNotebook class, a dynamic splittable notebook control</li>"
        "<li>New wxAuiMDI* classes, a tab-based MDI and drop-in replacement for classic MDI</li>"
        "<li>Maximize/Restore buttons implemented</li>"
        "<li>Better hinting with wxGTK</li>"
        "<li>Class rename.  'wxAui' is now the standard class prefix for all wxAUI classes</li>"
        "<li>Lots of bug fixes</li>"
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
