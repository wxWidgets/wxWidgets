/////////////////////////////////////////////////////////////////////////////
// Name:        taskbarbutton.cpp
// Purpose:     wxTaskBarButton sample
// Author:      Chaobin Zhang <zhchbin@gmail.com>
// Created:     2014-04-30
// Copyright:   (c) 2014 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/progdlg.h"
    #include "wx/wx.h"
#endif

#include "wx/stdpaths.h"
#include "wx/taskbarbutton.h"

enum
{
    ProgressValueSlider = wxID_HIGHEST,
    VisibilityRadio,
    ThumbnailTooltipSetBtn,
    ProgressStateChoice,
    SetOverlayIconBtn,
    ClearOverlayIconBtn,
    SetThumbnailClipBtn,
    RestoreThumbnailClipBtn,
    AddThumbBarButtonBtn,
    RemoveThumbBarButtonBtn,
};

enum
{
    ThumbnailToolbarBtn_0 = wxID_HIGHEST + 100,
    ThumbnailToolbarBtn_1,
    ThumbnailToolbarBtn_2,
    ThumbnailToolbarBtn_3,
    ThumbnailToolbarBtn_4,
    ThumbnailToolbarBtn_5,
    ThumbnailToolbarBtn_6
};

namespace {

wxBitmap CreateBitmap(const wxColour& colour, int w, int h)
{
    wxMemoryDC dc;
    wxBitmap bmp(w, h);
    dc.SelectObject(bmp);

    // Draw transparent background
    wxColour magic(255, 0, 255);
    wxBrush magicBrush(magic);
    dc.SetBrush(magicBrush);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawRectangle(0, 0, w, h);

    // Draw image content
    dc.SetBrush(wxBrush(colour));
    dc.DrawCircle(h / 2, h / 2 + 1, h / 2);
    dc.SelectObject(wxNullBitmap);

    // Finalize transparency with a mask
    wxMask *mask = new wxMask(bmp, magic);
    bmp.SetMask(mask);
    return bmp;
}

wxIcon CreateRandomIcon()
{
    static int counter = 0;
    static const wxColour* colours[] =
    {
        wxBLACK,
        wxWHITE,
        wxRED,
        wxBLUE,
        wxGREEN,
        wxCYAN,
        wxLIGHT_GREY
    };

    wxIcon icon;
    icon.CopyFromBitmap(CreateBitmap(*(colours[counter]), 16, 16));
    counter += 1;
    counter = counter % WXSIZEOF(colours);
    return icon;
}

}  // namespace

class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString& title);

private:
    wxDECLARE_EVENT_TABLE();

    void OnSetProgressValue(wxScrollEvent& WXUNUSED(event));
    void OnVisibilityChange(wxCommandEvent& WXUNUSED(event));
    void OnSetThumbnailTooltipBtn(wxCommandEvent& WXUNUSED(event));
    void OnChoice(wxCommandEvent& event);
    void OnSetOverlayIcon(wxCommandEvent& WXUNUSED(event));
    void OnClearOverlayIcon(wxCommandEvent& WXUNUSED(event));
    void OnSetOrRestoreThumbnailClip(wxCommandEvent& event);
    void OnAddThubmBarButton(wxCommandEvent& WXUNUSED(event));
    void OnRemoveThubmBarButton(wxCommandEvent& WXUNUSED(event));
    void OnThumbnailToolbarBtnClicked(wxCommandEvent& event);

    wxSlider *m_slider;
    wxRadioBox *m_visibilityRadioBox;
    wxTextCtrl *m_textCtrl;
    wxChoice *m_stateChoice;

    typedef wxVector<wxThumbBarButton*> wxThumbBarButtons;
    wxThumbBarButtons m_thumbBarButtons;
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxTaskBarJumpList jumpList;
    wxTaskBarJumpListItem *item1 = new wxTaskBarJumpListItem(
        nullptr,
        wxTASKBAR_JUMP_LIST_TASK,
        "Task 1",
        wxStandardPaths::Get().GetExecutablePath(),
        wxEmptyString,
        "Test Task",
        wxStandardPaths::Get().GetExecutablePath(),
        0);
    wxTaskBarJumpListItem *item2 = new wxTaskBarJumpListItem(
        nullptr,
        wxTASKBAR_JUMP_LIST_TASK,
        "Task 2",
        wxStandardPaths::Get().GetExecutablePath(),
        wxEmptyString,
        "Test Task",
        wxStandardPaths::Get().GetExecutablePath(),
        0);
    jumpList.GetTasks().Append(item1);
    jumpList.GetTasks().Append(
        new wxTaskBarJumpListItem(nullptr, wxTASKBAR_JUMP_LIST_SEPARATOR));
    jumpList.GetTasks().Append(item2);
    jumpList.ShowRecentCategory();
    jumpList.ShowFrequentCategory();

    wxTaskBarJumpListCategory* customCategory =
        new wxTaskBarJumpListCategory(&jumpList, "Custom");
    wxTaskBarJumpListItem* item3 = new wxTaskBarJumpListItem(
        customCategory,
        wxTASKBAR_JUMP_LIST_DESTINATION,
        "Help",
        wxStandardPaths::Get().GetExecutablePath(),
        "--help",
        "wxTaskBarButton help.",
        wxStandardPaths::Get().GetExecutablePath(),
        0);
    customCategory->Append(item3);
    jumpList.AddCustomCategory(customCategory);

    const wxTaskBarJumpListCategory& frequentCategory =
        jumpList.GetFrequentCategory();
    const wxTaskBarJumpListItems& frequentItems = frequentCategory.GetItems();
    for ( size_t i = 0; i < frequentItems.size(); ++i )
    {
        wxLogMessage(frequentItems[i]->GetFilePath());
    }

    const wxTaskBarJumpListCategory& recentCategory =
        jumpList.GetRecentCategory();
    const wxTaskBarJumpListItems& recentItems = recentCategory.GetItems();
    for ( size_t i = 0; i < recentItems.size(); ++i )
    {
        wxLogMessage(recentItems[i]->GetFilePath());
    }

    MyFrame *frame = new MyFrame("wxTaskBarButton App");
    if ( !frame->MSWGetTaskBarButton() )
    {
        wxLogError("Task bar button API is not available on this system, sorry.");
        return false;
    }

    frame->Show(true);

    return true;
}

MyFrame::MyFrame(const wxString& title)
       : wxFrame(nullptr, wxID_ANY, title)
{
    wxPanel *panel = new wxPanel(this);
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer *gs = new wxFlexGridSizer(4, 2, 10, 10);

    // SetProgressValue section.
    wxStaticBoxSizer *spvSizer =
        new wxStaticBoxSizer(wxVERTICAL, panel, "SetProgressValue");
    int flags = wxSL_MIN_MAX_LABELS | wxSL_VALUE_LABEL | wxSL_AUTOTICKS;
    m_slider = new wxSlider(spvSizer->GetStaticBox(), ProgressValueSlider,
                            0, 0, 100,
                            wxDefaultPosition, wxSize(250, -1),
                            flags);
    m_slider->SetTickFreq(10);
    spvSizer->Add(m_slider);

    // Show/Hide in Taskbar section.
    const wxString labels[] =
    {
        "&Show in Taskbar",
        "&Hide in Taskbar"
    };
    m_visibilityRadioBox = new wxRadioBox(panel, VisibilityRadio, "Visibility:",
                                          wxDefaultPosition, wxDefaultSize,
                                          WXSIZEOF(labels), labels,
                                          1, wxRA_SPECIFY_ROWS);
   // SetThumbnailTooltip section.
    wxStaticBoxSizer *sttSizer =
        new wxStaticBoxSizer(wxVERTICAL, panel, "SetThumbnailTooltip");
    m_textCtrl = new wxTextCtrl(panel, wxID_ANY);
    wxButton *btn = new wxButton(panel, ThumbnailTooltipSetBtn, "Set");
    sttSizer->Add(m_textCtrl, 1, wxEXPAND | wxALL, 2);
    sttSizer->Add(btn, 1, wxEXPAND | wxALL, 2);

    // SetProgressState section.
    wxStaticBoxSizer *spsSizer =
        new wxStaticBoxSizer(wxVERTICAL, panel, "SetProgressState");
    const wxString choices[] =
    {
        "wxNoProgress",
        "wxIndeterminate",
        "wxNormal",
        "wxError",
        "wxPaused"
    };
    m_stateChoice = new wxChoice(panel, ProgressStateChoice,
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(choices), choices);
    spsSizer->Add(m_stateChoice, 0, wxALL | wxGROW, 5);

    // SetOverlayIcon section.
    wxStaticBoxSizer *soiSizer =
        new wxStaticBoxSizer(wxVERTICAL, panel, "SetOverlayIcon");
    wxButton *setOverlayIconBtn =
        new wxButton(panel, SetOverlayIconBtn, "Set Overlay Icon");
    wxButton *clearOverlayIconBtn =
        new wxButton(panel, ClearOverlayIconBtn, "Clear Overlay Icon");
    soiSizer->Add(setOverlayIconBtn, 1, wxEXPAND | wxALL, 2);
    soiSizer->Add(clearOverlayIconBtn, 1, wxEXPAND | wxALL, 2);

    // SetThumbnailClip section.
    wxStaticBoxSizer *stcSizer =
        new wxStaticBoxSizer(wxVERTICAL, panel, "SetThumbnailClip");
    wxButton *setThumbnailClipBtn =
        new wxButton(panel, SetThumbnailClipBtn, "Set Thumbnail Clip");
    wxButton *restoreThumbnailClipBtn =
        new wxButton(panel, RestoreThumbnailClipBtn,
                     "Restore Thumbnail Clip");
    stcSizer->Add(setThumbnailClipBtn, 1, wxEXPAND | wxALL, 2);
    stcSizer->Add(restoreThumbnailClipBtn, 1, wxEXPAND | wxALL, 2);

    // Thumbnail Toolbar Buttons section.
    wxStaticBoxSizer *ttbSizer =
        new wxStaticBoxSizer(wxVERTICAL, panel, "ThumbBar Buttons");
    wxButton *addThumbBarButtonBtn =
        new wxButton(panel, AddThumbBarButtonBtn, "Add ThumbBar Button");
    wxButton *showThumbnailToolbarBtn =
        new wxButton(panel, RemoveThumbBarButtonBtn,
                     "Remove Last ThumbBar Button");
    ttbSizer->Add(addThumbBarButtonBtn, 1, wxEXPAND | wxALL, 2);
    ttbSizer->Add(showThumbnailToolbarBtn, 1, wxEXPAND | wxALL, 2);

    gs->Add(spvSizer, 0, wxEXPAND);
    gs->Add(m_visibilityRadioBox, 0, wxEXPAND);
    gs->Add(sttSizer, 0, wxEXPAND);
    gs->Add(spsSizer, 0, wxEXPAND);
    gs->Add(soiSizer, 0, wxEXPAND);
    gs->Add(stcSizer, 0, wxEXPAND);
    gs->Add(ttbSizer, 0, wxEXPAND);

    wxStaticText *text = new wxStaticText(
        panel, wxID_ANY, "Welcome to wxTaskBarButton sample");
    mainSizer->Add(text, 0, wxALIGN_CENTRE_HORIZONTAL);
    mainSizer->Add(gs);

    panel->SetSizer(mainSizer);

    SetIcon(wxICON(sample));
    SetSize(537, 420);
    Centre();
}

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_COMMAND_SCROLL(ProgressValueSlider, MyFrame::OnSetProgressValue)
    EVT_RADIOBOX(VisibilityRadio, MyFrame::OnVisibilityChange)
    EVT_BUTTON(ThumbnailTooltipSetBtn, MyFrame::OnSetThumbnailTooltipBtn)
    EVT_CHOICE(ProgressStateChoice, MyFrame::OnChoice)
    EVT_BUTTON(SetOverlayIconBtn, MyFrame::OnSetOverlayIcon)
    EVT_BUTTON(ClearOverlayIconBtn, MyFrame::OnClearOverlayIcon)
    EVT_BUTTON(SetThumbnailClipBtn, MyFrame::OnSetOrRestoreThumbnailClip)
    EVT_BUTTON(RestoreThumbnailClipBtn, MyFrame::OnSetOrRestoreThumbnailClip)
    EVT_BUTTON(AddThumbBarButtonBtn, MyFrame::OnAddThubmBarButton)
    EVT_BUTTON(RemoveThumbBarButtonBtn, MyFrame::OnRemoveThubmBarButton)
    EVT_BUTTON(ThumbnailToolbarBtn_0, MyFrame::OnThumbnailToolbarBtnClicked)
    EVT_BUTTON(ThumbnailToolbarBtn_1, MyFrame::OnThumbnailToolbarBtnClicked)
    EVT_BUTTON(ThumbnailToolbarBtn_2, MyFrame::OnThumbnailToolbarBtnClicked)
    EVT_BUTTON(ThumbnailToolbarBtn_3, MyFrame::OnThumbnailToolbarBtnClicked)
    EVT_BUTTON(ThumbnailToolbarBtn_4, MyFrame::OnThumbnailToolbarBtnClicked)
    EVT_BUTTON(ThumbnailToolbarBtn_5, MyFrame::OnThumbnailToolbarBtnClicked)
    EVT_BUTTON(ThumbnailToolbarBtn_6, MyFrame::OnThumbnailToolbarBtnClicked)
wxEND_EVENT_TABLE()

void MyFrame::OnSetProgressValue(wxScrollEvent& WXUNUSED(event))
{
    static bool s_hasRangeSet = false;
    if ( !s_hasRangeSet )
    {
      MSWGetTaskBarButton()->SetProgressRange(100);
      s_hasRangeSet = true;
    }

    MSWGetTaskBarButton()->SetProgressValue(m_slider->GetValue());
}

void MyFrame::OnVisibilityChange(wxCommandEvent& WXUNUSED(event))
{
    if ( m_visibilityRadioBox->GetSelection() == 0 )
        MSWGetTaskBarButton()->Show();
    else
        MSWGetTaskBarButton()->Hide();
}

void MyFrame::OnSetThumbnailTooltipBtn(wxCommandEvent& WXUNUSED(event))
{
    MSWGetTaskBarButton()->SetThumbnailTooltip(m_textCtrl->GetLineText(0));
}

void MyFrame::OnChoice(wxCommandEvent& event)
{
    int sel = event.GetSelection();
    wxTaskBarButtonState state;
    switch(sel)
    {
        case 0:
            state = wxTASKBAR_BUTTON_NO_PROGRESS;
            break;
        case 1:
            state = wxTASKBAR_BUTTON_INDETERMINATE;
            break;
        case 2:
            state = wxTASKBAR_BUTTON_NORMAL;
            break;
        case 3:
            state = wxTASKBAR_BUTTON_ERROR;
            break;
        case 4:
            state = wxTASKBAR_BUTTON_PAUSED;
            break;
        default:
            state = wxTASKBAR_BUTTON_NO_PROGRESS;
            break;
    }

    MSWGetTaskBarButton()->SetProgressValue(m_slider->GetValue());
    MSWGetTaskBarButton()->SetProgressState(state);
}

void MyFrame::OnSetOverlayIcon(wxCommandEvent& WXUNUSED(event))
{
    MSWGetTaskBarButton()->SetOverlayIcon(CreateRandomIcon());
}

void MyFrame::OnClearOverlayIcon(wxCommandEvent& WXUNUSED(event))
{
    MSWGetTaskBarButton()->SetOverlayIcon(wxNullIcon);
}

void MyFrame::OnSetOrRestoreThumbnailClip(wxCommandEvent& event)
{
    wxRect rect;
    if ( event.GetId() == SetThumbnailClipBtn )
    {
        static const int CLIP_LENGTH = 100;
        int height, width;
        GetClientSize(&width, &height);
        rect.SetX((width - CLIP_LENGTH) / 2);
        rect.SetY((height - CLIP_LENGTH) / 2);
        rect.SetHeight(CLIP_LENGTH);
        rect.SetWidth(CLIP_LENGTH);
    }

    MSWGetTaskBarButton()->SetThumbnailClip(rect);
}

void MyFrame::OnAddThubmBarButton(wxCommandEvent& WXUNUSED(event))
{
    if ( m_thumbBarButtons.size() >= 7 )
        return;

    wxThumbBarButton *button =
        new wxThumbBarButton(m_thumbBarButtons.size() + ThumbnailToolbarBtn_0 ,
                             CreateRandomIcon());
    MSWGetTaskBarButton()->AppendThumbBarButton(button);
    m_thumbBarButtons.push_back(button);
}

void MyFrame::OnRemoveThubmBarButton(wxCommandEvent& WXUNUSED(event))
{
    if ( m_thumbBarButtons.empty() )
        return;

    wxThumbBarButton* button = m_thumbBarButtons.back();
    m_thumbBarButtons.pop_back();
    delete MSWGetTaskBarButton()->RemoveThumbBarButton(button);
}

void MyFrame::OnThumbnailToolbarBtnClicked(wxCommandEvent& event)
{
    wxLogMessage("Thumbnail Toolbar Button %d is clicked.", event.GetId());
}
