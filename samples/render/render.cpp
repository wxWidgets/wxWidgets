/////////////////////////////////////////////////////////////////////////////
// Name:        render.cpp
// Purpose:     Render wxWidgets sample
// Author:      Vadim Zeitlin
// Created:     04.08.03
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
    #include "wx/dc.h"
    #include "wx/dcclient.h"
    #include "wx/panel.h"
    #include "wx/menu.h"
    #include "wx/textdlg.h"
    #include "wx/log.h"
    #include "wx/msgdlg.h"
    #include "wx/icon.h"
    #include "wx/image.h"
#endif

#include "wx/apptrait.h"
#include "wx/artprov.h"
#include "wx/renderer.h"
#include "wx/dcgraph.h"

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// A renderer class draws the header buttons in a "special" way
class MyRenderer : public wxDelegateRendererNative
{
public:
    MyRenderer() : wxDelegateRendererNative(wxRendererNative::GetDefault()) { }

    virtual int DrawHeaderButton(wxWindow *WXUNUSED(win),
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int WXUNUSED(flags) = 0,
                                  wxHeaderSortIconType WXUNUSED(sortArrow)
                                    = wxHDR_SORT_ICON_NONE,
                                  wxHeaderButtonParams* params = nullptr) override
    {
        wxDCBrushChanger setBrush(dc, *wxBLUE_BRUSH);
        wxDCTextColourChanger setFgCol(dc, *wxWHITE);
        wxDCTextBgModeChanger setBgMode(dc, wxBRUSHSTYLE_TRANSPARENT);
        dc.DrawRoundedRectangle(rect, 5);

        wxString label;
        if ( params )
            label = params->m_labelText;
        dc.DrawLabel(label, wxNullBitmap, rect, wxALIGN_CENTER);
        return rect.width;
    }
};

// To use a different renderer from the very beginning we must override
// wxAppTraits method creating the renderer (another, simpler, alternative is
// to call wxRendererNative::Set() a.s.a.p. which should work in 99% of the
// cases, but we show this here just for completeness)
class MyTraits : public wxGUIAppTraits
{
    virtual wxRendererNative *CreateRenderer() override
    {
        // it will be deleted on program shutdown by wxWidgets itself
        return new MyRenderer;
    }
};

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;

    // if we want MyTraits to be used we must override CreateTraits()
    virtual wxAppTraits *CreateTraits() override { return new MyTraits; }
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame();
    virtual ~MyFrame();

private:
    // event handlers (these functions should _not_ be virtual)
    void OnDrawDisabled(wxCommandEvent& event)
        { OnToggleDrawFlag(event, wxCONTROL_DISABLED); }
    void OnDrawFocused(wxCommandEvent& event)
        { OnToggleDrawFlag(event, wxCONTROL_FOCUSED); }
    void OnDrawPressed(wxCommandEvent& event)
        { OnToggleDrawFlag(event, wxCONTROL_PRESSED); }
    void OnDrawChecked(wxCommandEvent& event)
        { OnToggleDrawFlag(event, wxCONTROL_CHECKED); }
    void OnDrawHot(wxCommandEvent& event)
        { OnToggleDrawFlag(event, wxCONTROL_CURRENT); }
    void OnDrawUndetermined(wxCommandEvent &event)
        { OnToggleDrawFlag(event, wxCONTROL_UNDETERMINED); }
    void OnDrawSpecial(wxCommandEvent &event)
        { OnToggleDrawFlag(event, wxCONTROL_SPECIAL); }

    void OnAlignLeft(wxCommandEvent& WXUNUSED(event))
        { OnChangeAlign(wxALIGN_LEFT); }
    void OnAlignCentre(wxCommandEvent& WXUNUSED(event))
        { OnChangeAlign(wxALIGN_CENTRE); }
    void OnAlignRight(wxCommandEvent& WXUNUSED(event))
        { OnChangeAlign(wxALIGN_RIGHT); }

    void OnUseIcon(wxCommandEvent& event);
    void OnUseBitmap(wxCommandEvent& event);

    void OnUseGeneric(wxCommandEvent& event);
#if wxUSE_DYNLIB_CLASS
    void OnLoad(wxCommandEvent& event);
    void OnUnload(wxCommandEvent& event);
#endif // wxUSE_DYNLIB_CLASS
    void OnToggleLayoutDirection(wxCommandEvent &evt);

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnToggleDrawFlag(wxCommandEvent& event, int flag);
    void OnChangeAlign(int align);

#if wxUSE_GRAPHICS_CONTEXT
    void OnGraphicContextNone(wxCommandEvent& evt);
    void OnGraphicContextDefault(wxCommandEvent& evt);
#if wxUSE_CAIRO
    void OnGraphicContextCairo(wxCommandEvent& evt);
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    void OnGraphicContextGDIPlus(wxCommandEvent& evt);
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    void OnGraphicContextDirect2D(wxCommandEvent& evt);
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT

    class MyPanel *m_panel;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// a very simple class just to have something to draw on
class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent) : wxPanel(parent)
    {
        m_flags = 0;
        m_align = wxALIGN_LEFT;
        m_useIcon =
        m_useBitmap =
        m_useGeneric = false;
#if wxUSE_GRAPHICS_CONTEXT
        m_renderer = nullptr;
#endif
    }

    int GetFlags() const { return m_flags; }
    void SetFlags(int flags) { m_flags = flags; }

    void SetAlignment(int align) { m_align = align; }
    void SetUseIcon(bool useIcon) { m_useIcon = useIcon; }
    void SetUseBitmap(bool useBitmap) { m_useBitmap = useBitmap; }
    void SetUseGeneric(bool useGeneric) { m_useGeneric = useGeneric; }
#if wxUSE_GRAPHICS_CONTEXT
    void UseGraphicRenderer(wxGraphicsRenderer* renderer);
#endif

private:
    void OnPaint(wxPaintEvent&)
    {
        wxPaintDC pdc(this);

#if wxUSE_GRAPHICS_CONTEXT
        wxGCDC gdc;
        if ( m_renderer )
        {
            wxGraphicsContext* ctx = m_renderer->CreateContext(pdc);
            gdc.SetBackground(GetBackgroundColour());
            gdc.SetGraphicsContext(ctx);
        }

        wxDC& dc = m_renderer ? static_cast<wxDC&>(gdc) : pdc;
#else
        wxDC& dc = pdc;
#endif

        wxRendererNative& renderer = m_useGeneric ? wxRendererNative::GetGeneric()
                                                  : wxRendererNative::Get();

        int x1 = FromDIP(10),    // text offset
            x2 = FromDIP(300),   // drawing offset
            y = FromDIP(10);

        const int lineHeight = dc.GetCharHeight();
        dc.DrawText("Demonstration of various wxRenderer functions:", x1, y);
        y += lineHeight;
        wxString flagsString;
        if ( m_flags & wxCONTROL_DISABLED )
            flagsString += "wxCONTROL_DISABLED ";
        if ( m_flags & wxCONTROL_FOCUSED )
            flagsString += "wxCONTROL_FOCUSED ";
        if ( m_flags & wxCONTROL_PRESSED )
            flagsString += "wxCONTROL_PRESSED ";
        if ( m_flags & wxCONTROL_CURRENT )
            flagsString += "wxCONTROL_CURRENT ";
        if ( m_flags & wxCONTROL_CHECKED )
            flagsString += "wxCONTROL_CHECKED ";
        if ( m_flags & wxCONTROL_UNDETERMINED )
            flagsString += "wxCONTROL_UNDETERMINED ";
        if (m_flags & wxCONTROL_SPECIAL)
            flagsString += "wxCONTROL_SPECIAL ";
        if (flagsString.empty())
            flagsString = "(none)";
        dc.DrawText("Using flags: " + flagsString, x1, y);
        y += lineHeight*3;

        const wxCoord heightListItem = FromDIP(48);
        const wxCoord widthListItem = 30*GetCharWidth();

        {

        dc.DrawText("DrawItemText() alignment", x1, y);

        wxRect textRect(x2, y, widthListItem, heightListItem);
        wxDCBrushChanger setBrush(dc, *wxTRANSPARENT_BRUSH);
        wxDCPenChanger setPen(dc, *wxGREEN_PEN);
        dc.DrawRectangle(textRect);

        renderer.DrawItemText(this, dc, L"Top Left (\u1ED6)", textRect);
        renderer.DrawItemText(this, dc, "Bottom right", textRect,
            wxALIGN_BOTTOM | wxALIGN_RIGHT);

        y += lineHeight + heightListItem;

        }

        const wxCoord heightHdr = renderer.GetHeaderButtonHeight(this);
        const wxCoord width = 15*GetCharWidth();

        const wxHeaderSortIconType
            hdrSortIcon = m_useIcon ? wxHDR_SORT_ICON_UP
                                    : wxHDR_SORT_ICON_NONE;

        wxHeaderButtonParams hdrParams;
        hdrParams.m_labelText = "Header";
        hdrParams.m_labelAlignment = m_align;
        if ( m_useBitmap )
        {
            hdrParams.m_labelBitmap = wxArtProvider::GetBitmap(wxART_WARNING,
                                                               wxART_LIST);
        }

        // Note that we need to use GetDefault() explicitly to show the default
        // implementation.
        dc.DrawText("DrawHeaderButton() (default)", x1, y);
        wxRendererNative::GetDefault().DrawHeaderButton(this, dc,
                                  wxRect(x2, y, width, heightHdr), m_flags,
                                  hdrSortIcon, &hdrParams);
        y += lineHeight + heightHdr;

        dc.DrawText("DrawHeaderButton() (overridden)", x1, y);
        renderer.DrawHeaderButton(this, dc,
                                  wxRect(x2, y, width, heightHdr), m_flags,
                                  hdrSortIcon, &hdrParams);
        y += lineHeight + heightHdr;

        dc.DrawText("DrawCheckBox()", x1, y);
        const wxSize sizeCheck = renderer.GetCheckBoxSize(this);
        renderer.DrawCheckBox(this, dc,
                              wxRect(wxPoint(x2, y), sizeCheck), m_flags);
        y += lineHeight + sizeCheck.y;

        dc.DrawText("DrawCheckMark()", x1, y);
        const wxSize sizeMark = renderer.GetCheckMarkSize(this);
        renderer.DrawCheckMark(this, dc,
                               wxRect(wxPoint(x2, y), sizeMark), m_flags);
        y += lineHeight + sizeMark.y;

        const wxString notImplementedText = "(generic version unimplemented)";

        dc.DrawText("DrawRadioBitmap()", x1, y);
        if ( m_useGeneric )
            dc.DrawText(notImplementedText, x2, y);
        else
            renderer.DrawRadioBitmap(this, dc,
                                     wxRect(wxPoint(x2, y), sizeCheck), m_flags);

        y += lineHeight + sizeCheck.y;

        dc.DrawText("DrawCollapseButton()", x1, y);
        const wxSize sizeCollapse = renderer.GetCollapseButtonSize(this, dc);
        renderer.DrawCollapseButton(this, dc,
                                    wxRect(wxPoint(x2, y), sizeCollapse), m_flags);
        y += lineHeight + sizeCollapse.y;

        dc.DrawText("DrawTreeItemButton()", x1, y);
        const wxSize sizeExpand = renderer.GetExpanderSize(this);
        renderer.DrawTreeItemButton(this, dc,
                                    wxRect(wxPoint(x2, y), sizeExpand), m_flags);
        y += lineHeight + sizeExpand.y;

#ifdef wxHAS_DRAW_TITLE_BAR_BITMAP
        dc.DrawText("DrawTitleBarBitmap()", x1, y);
        wxRect rBtn(x2, y, FromDIP(21), FromDIP(21));
        renderer.DrawTitleBarBitmap(this, dc, rBtn,
                                    wxTITLEBAR_BUTTON_HELP, m_flags);
        rBtn.x += 2*rBtn.width;
        renderer.DrawTitleBarBitmap(this, dc, rBtn,
                                    wxTITLEBAR_BUTTON_ICONIZE, m_flags);
        rBtn.x += 2*rBtn.width;
        renderer.DrawTitleBarBitmap(this, dc, rBtn,
                                    wxTITLEBAR_BUTTON_RESTORE, m_flags);
        rBtn.x += 2*rBtn.width;
        renderer.DrawTitleBarBitmap(this, dc, rBtn,
                                    wxTITLEBAR_BUTTON_MAXIMIZE, m_flags);
        rBtn.x += 2*rBtn.width;
        renderer.DrawTitleBarBitmap(this, dc, rBtn,
                                    wxTITLEBAR_BUTTON_CLOSE, m_flags);

        y += lineHeight + rBtn.height;
#endif // wxHAS_DRAW_TITLE_BAR_BITMAP

        // The meanings of those are reversed for the vertical gauge below.
        const wxCoord heightGauge = FromDIP(24);
        const wxCoord widthGauge = 30*GetCharWidth();

        dc.DrawText("DrawGauge()", x1, y);
        renderer.DrawGauge(this, dc,
            wxRect(x2, y, widthGauge, heightGauge), 25, 100, m_flags);
        renderer.DrawGauge(this, dc,
            wxRect(x2 + widthGauge + FromDIP(30), y + heightGauge - widthGauge,
                   heightGauge, widthGauge),
            25, 100, m_flags | wxCONTROL_SPECIAL);

        y += lineHeight + heightGauge;

        dc.DrawText("DrawItemSelectionRect()", x1, y);
        renderer.DrawItemSelectionRect(this, dc,
            wxRect(x2, y, widthListItem, heightListItem), m_flags | wxCONTROL_SELECTED);

        renderer.DrawItemText(this, dc, "DrawItemText()",
            wxRect(x2, y, widthListItem, heightListItem).Inflate(-2, -2), m_align, m_flags | wxCONTROL_SELECTED);

        y += heightListItem;

        y += lineHeight;
        dc.DrawText("DrawChoice()", x1, y);
        if ( m_useGeneric )
            dc.DrawText(notImplementedText, x2, y);
        else
            renderer.DrawChoice(this, dc,
                                wxRect(x2, y, width, GetCharHeight() * 3 / 2), m_flags);
    }

    int m_flags;
    int m_align;
    bool m_useIcon,
         m_useBitmap,
         m_useGeneric;
#if wxUSE_GRAPHICS_CONTEXT
    wxGraphicsRenderer* m_renderer;
#endif

    wxDECLARE_EVENT_TABLE();
};

#if wxUSE_GRAPHICS_CONTEXT
void MyPanel::UseGraphicRenderer(wxGraphicsRenderer* renderer)
{
    m_renderer = renderer;
    if ( renderer )
    {
        int major, minor, micro;
        renderer->GetVersion(&major, &minor, &micro);
        wxString str = wxString::Format("Graphics renderer: %s %i.%i.%i",
            renderer->GetName(), major, minor, micro);
        reinterpret_cast<wxFrame*>(GetParent())->GetStatusBar()->SetStatusText(str, 1);
    }
    else
    {
        reinterpret_cast<wxFrame*>(GetParent())->GetStatusBar()->SetStatusText(wxEmptyString, 1);
    }

    Refresh();
}
#endif // wxUSE_GRAPHICS_CONTEXT

wxBEGIN_EVENT_TABLE(MyPanel, wxPanel)
    EVT_PAINT(MyPanel::OnPaint)
wxEND_EVENT_TABLE()

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // our menu items
    Render_DrawDisabled = 100,
    Render_DrawFocused,
    Render_DrawPressed,
    Render_DrawChecked,
    Render_DrawHot,
    Render_DrawUndetermined,
    Render_DrawSpecial,

    Render_AlignLeft,
    Render_AlignCentre,
    Render_AlignRight,

    Render_UseIcon,
    Render_UseBitmap,

    Render_UseGeneric,
#if wxUSE_DYNLIB_CLASS
    Render_Load,
    Render_Unload,
#endif // wxUSE_DYNLIB_CLASS
    Render_LayoutDir,

#if wxUSE_GRAPHICS_CONTEXT
    DC_DC,
    DC_GC_Default,
#if wxUSE_CAIRO
    DC_GC_Cairo,
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    DC_GC_GDIPlus,
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    DC_GC_Direct2D,
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT

    // standard menu items
    Render_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Render_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Render_DrawDisabled, MyFrame::OnDrawDisabled)
    EVT_MENU(Render_DrawFocused, MyFrame::OnDrawFocused)
    EVT_MENU(Render_DrawPressed, MyFrame::OnDrawPressed)
    EVT_MENU(Render_DrawChecked, MyFrame::OnDrawChecked)
    EVT_MENU(Render_DrawHot, MyFrame::OnDrawHot)
    EVT_MENU(Render_DrawUndetermined, MyFrame::OnDrawUndetermined)
    EVT_MENU(Render_DrawSpecial, MyFrame::OnDrawSpecial)
    EVT_MENU(Render_AlignLeft, MyFrame::OnAlignLeft)
    EVT_MENU(Render_AlignCentre, MyFrame::OnAlignCentre)
    EVT_MENU(Render_AlignRight, MyFrame::OnAlignRight)

    EVT_MENU(Render_UseIcon, MyFrame::OnUseIcon)
    EVT_MENU(Render_UseBitmap, MyFrame::OnUseBitmap)

    EVT_MENU(Render_UseGeneric, MyFrame::OnUseGeneric)
#if wxUSE_DYNLIB_CLASS
    EVT_MENU(Render_Load,  MyFrame::OnLoad)
    EVT_MENU(Render_Unload,MyFrame::OnUnload)
#endif // wxUSE_DYNLIB_CLASS
    EVT_MENU(Render_LayoutDir, MyFrame::OnToggleLayoutDirection)
    EVT_MENU(Render_Quit,  MyFrame::OnQuit)

#if wxUSE_GRAPHICS_CONTEXT
    EVT_MENU(DC_DC, MyFrame::OnGraphicContextNone)
    EVT_MENU(DC_GC_Default, MyFrame::OnGraphicContextDefault)
#if wxUSE_CAIRO
    EVT_MENU(DC_GC_Cairo, MyFrame::OnGraphicContextCairo)
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    EVT_MENU(DC_GC_GDIPlus, MyFrame::OnGraphicContextGDIPlus)
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    EVT_MENU(DC_GC_Direct2D, MyFrame::OnGraphicContextDirect2D)
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT

    EVT_MENU(Render_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#ifdef __WXOSX__
    // currently the images used by DrawTitleBarBitmap() are hard coded as PNG
    // images inside the library itself so we need to enable PNG support to use
    // this function
    wxImage::AddHandler(new wxPNGHandler);
#endif // OS X

    // create the main application window
    new MyFrame;

    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame()
       : wxFrame(nullptr,
                 wxID_ANY,
                 "Render wxWidgets Sample")
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;
    menuFile->AppendCheckItem(Render_DrawDisabled,
                              "Draw in &disabled state\tCtrl-D");
    menuFile->AppendCheckItem(Render_DrawFocused,
                              "Draw in &focused state\tCtrl-F");
    menuFile->AppendCheckItem(Render_DrawPressed,
                              "Draw in &pressed state\tCtrl-P");
    menuFile->AppendCheckItem(Render_DrawChecked,
                              "Draw in &checked state\tCtrl-C");
    menuFile->AppendCheckItem(Render_DrawHot,
                              "Draw in &hot state\tCtrl-H");
    menuFile->AppendCheckItem(Render_DrawUndetermined,
                              "Draw in unde&termined state\tCtrl-T");
    menuFile->AppendCheckItem(Render_DrawSpecial,
                              "Draw in &special state\tCtrl-S");
    menuFile->AppendSeparator();

    menuFile->AppendRadioItem(Render_AlignLeft, "&Left align\tCtrl-1");
    menuFile->AppendRadioItem(Render_AlignCentre, "C&entre align\tCtrl-2");
    menuFile->AppendRadioItem(Render_AlignRight, "&Right align\tCtrl-3");
    menuFile->AppendSeparator();

    menuFile->AppendCheckItem(Render_UseIcon, "Draw &icon\tCtrl-I");
    menuFile->AppendCheckItem(Render_UseBitmap, "Draw &bitmap\tCtrl-B");
    menuFile->AppendSeparator();

    menuFile->AppendCheckItem(Render_UseGeneric, "Use &generic renderer\tCtrl-G");
#if wxUSE_DYNLIB_CLASS
    menuFile->Append(Render_Load, "&Load renderer...\tCtrl-L");
    menuFile->Append(Render_Unload, "&Unload renderer\tCtrl-U");
#endif // wxUSE_DYNLIB_CLASS
    menuFile->AppendSeparator();
    menuFile->AppendCheckItem(Render_LayoutDir, "Toggle &layout direction\tShift-Ctrl-L");
    menuFile->Check(Render_LayoutDir, GetLayoutDirection() == wxLayout_RightToLeft);
    menuFile->AppendSeparator();
    menuFile->Append(Render_Quit);

#if wxUSE_GRAPHICS_CONTEXT
    wxMenu* menuDC = new wxMenu;
    // Number the different renderer choices consecutively, starting from 0.
    int accel = 0;
    menuDC->AppendRadioItem(DC_DC, wxString::Format("Use wx&DC\t%i", accel++));
    menuDC->AppendRadioItem(DC_GC_Default, wxString::Format("Use default wx&GraphicContext\t%i", accel++));
#if wxUSE_CAIRO
    menuDC->AppendRadioItem(DC_GC_Cairo, wxString::Format("Use &Cairo\t%i", accel++));
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
    menuDC->AppendRadioItem(DC_GC_GDIPlus, wxString::Format("Use &GDI+\t%i", accel++));
#endif
#if wxUSE_GRAPHICS_DIRECT2D
    menuDC->AppendRadioItem(DC_GC_Direct2D,wxString::Format("Use &Direct2D\t%i", accel++));
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Render_About);

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, "&File");
#if wxUSE_GRAPHICS_CONTEXT
    menuBar->Append(menuDC, "&DC");
#endif // wxUSE_GRAPHICS_CONTEXT
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

    m_panel = new MyPanel(this);

    SetClientSize(600, 600);

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome to wxWidgets!");
#endif // wxUSE_STATUSBAR

    Show();
}

MyFrame::~MyFrame()
{
    delete wxRendererNative::Set(nullptr);
}


// event handlers

void MyFrame::OnToggleDrawFlag(wxCommandEvent& event, int flag)
{
    int flags = m_panel->GetFlags();
    if ( event.IsChecked() )
        flags |= flag;
    else
        flags &= ~flag;

    m_panel->SetFlags(flags);
    m_panel->Refresh();
}

void MyFrame::OnChangeAlign(int align)
{
    m_panel->SetAlignment(align);
    m_panel->Refresh();
}

void MyFrame::OnUseIcon(wxCommandEvent& event)
{
    m_panel->SetUseIcon(event.IsChecked());
    m_panel->Refresh();
}

void MyFrame::OnUseBitmap(wxCommandEvent& event)
{
    m_panel->SetUseBitmap(event.IsChecked());
    m_panel->Refresh();
}

void MyFrame::OnUseGeneric(wxCommandEvent& event)
{
    m_panel->SetUseGeneric(event.IsChecked());
    m_panel->Refresh();
}

#if wxUSE_DYNLIB_CLASS

void MyFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    static wxString s_name = "renddll";

    wxString name = wxGetTextFromUser
                    (
                        "Name of the renderer to load:",
                        "Render wxWidgets Sample",
                        s_name,
                        this
                    );
    if ( name.empty() )
    {
        // cancelled
        return;
    }

    s_name = name;

    wxRendererNative *renderer = wxRendererNative::Load(name);
    if ( !renderer )
    {
        wxLogError("Failed to load renderer \"%s\".", name);
    }
    else // loaded ok
    {
        delete wxRendererNative::Set(renderer);

        m_panel->Refresh();

        wxLogStatus(this, "Successfully loaded the renderer \"%s\".",
                    name);
    }
}

void MyFrame::OnUnload(wxCommandEvent& WXUNUSED(event))
{
    wxRendererNative *renderer = wxRendererNative::Set(nullptr);
    if ( renderer )
    {
        delete renderer;

        m_panel->Refresh();

        wxLogStatus(this, "Unloaded the previously loaded renderer.");
    }
    else
    {
        wxLogWarning("No renderer to unload.");
    }
}

#endif // wxUSE_DYNLIB_CLASS

void MyFrame::OnToggleLayoutDirection(wxCommandEvent& WXUNUSED(evt))
{
    wxLayoutDirection dir = GetLayoutDirection() == wxLayout_LeftToRight
                                  ? wxLayout_RightToLeft : wxLayout_LeftToRight;
    SetLayoutDirection(dir);
    GetStatusBar()->SetLayoutDirection(dir);
    m_panel->SetLayoutDirection(dir);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Render sample shows how to use custom renderers.\n"
                 "\n"
                 "(c) 2003 Vadim Zeitlin",
                 "About Render wxWidgets Sample",
                 wxOK | wxICON_INFORMATION, this);
}

#if wxUSE_GRAPHICS_CONTEXT
void MyFrame::OnGraphicContextNone(wxCommandEvent& WXUNUSED(evt))
{
    m_panel->UseGraphicRenderer(nullptr);
}

void MyFrame::OnGraphicContextDefault(wxCommandEvent& WXUNUSED(evt))
{
    m_panel->UseGraphicRenderer(wxGraphicsRenderer::GetDefaultRenderer());
}

#if wxUSE_CAIRO
void MyFrame::OnGraphicContextCairo(wxCommandEvent& WXUNUSED(evt))
{
    m_panel->UseGraphicRenderer(wxGraphicsRenderer::GetCairoRenderer());
}
#endif // wxUSE_CAIRO
#ifdef __WXMSW__
#if wxUSE_GRAPHICS_GDIPLUS
void MyFrame::OnGraphicContextGDIPlus(wxCommandEvent& WXUNUSED(evt))
{
    m_panel->UseGraphicRenderer(wxGraphicsRenderer::GetGDIPlusRenderer());
}
#endif
#if wxUSE_GRAPHICS_DIRECT2D
void MyFrame::OnGraphicContextDirect2D(wxCommandEvent& WXUNUSED(evt))
{
    m_panel->UseGraphicRenderer(wxGraphicsRenderer::GetDirect2DRenderer());
}
#endif
#endif // __WXMSW__
#endif // wxUSE_GRAPHICS_CONTEXT
