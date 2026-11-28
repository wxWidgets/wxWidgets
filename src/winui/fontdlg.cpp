/////////////////////////////////////////////////////////////////////////////
// Name:        src/winui/fontdlg.cpp
// Purpose:     wxFontDialog for wxWinUI: the font dialog of Windows, made
//              of WinUI controls
// Author:      wxWidgets development team
// Created:     2026-11-22
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#include "wx/winui/winui.h"

#if wxUSE_FONTDLG && wxUSE_WINUI3

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/choice.h"
    #include "wx/dcmemory.h"
    #include "wx/intl.h"
    #include "wx/listbox.h"
    #include "wx/msgdlg.h"
    #include "wx/panel.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
    #include "wx/statbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif

#include "wx/bmpcbox.h"
#include "wx/fontdlg.h"
#include "wx/fontutil.h"
#include "wx/msw/private.h"
#include "wx/msw/private/comptr.h"
#include "wx/winui/private/listitemfont.h"
#include "wx/winui/private/themecolours.h"

#include <dwrite_1.h>

#include <algorithm>
#include <cmath>
#include <vector>

wxIMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog);

namespace
{

// The sizes offered for any font, the ones of the dialog of Windows.
const int gs_fontSizes[] =
{
    8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72
};

// The rows a list shows without scrolling, and the height of one of them in
// the WinUI list.
const int LIST_ROWS = 7;
const int LIST_ROW_DIP = 32;

// The sixteen colours of the dialog of Windows, in its order.
struct wxWinUIFontColour
{
    const char *name;
    unsigned char red, green, blue;
};

const wxWinUIFontColour gs_fontColours[] =
{
    { wxTRANSLATE("Black"),   0,   0,   0   },
    { wxTRANSLATE("Maroon"),  128, 0,   0   },
    { wxTRANSLATE("Green"),   0,   128, 0   },
    { wxTRANSLATE("Olive"),   128, 128, 0   },
    { wxTRANSLATE("Navy"),    0,   0,   128 },
    { wxTRANSLATE("Purple"),  128, 0,   128 },
    { wxTRANSLATE("Teal"),    0,   128, 128 },
    { wxTRANSLATE("Gray"),    128, 128, 128 },
    { wxTRANSLATE("Silver"),  192, 192, 192 },
    { wxTRANSLATE("Red"),     255, 0,   0   },
    { wxTRANSLATE("Lime"),    0,   255, 0   },
    { wxTRANSLATE("Yellow"),  255, 255, 0   },
    { wxTRANSLATE("Blue"),    0,   0,   255 },
    { wxTRANSLATE("Fuchsia"), 255, 0,   255 },
    { wxTRANSLATE("Aqua"),    0,   255, 255 },
    { wxTRANSLATE("White"),   255, 255, 255 },
};

// One style of a family: "Regular", "Semibold Italic", "Narrow Bold"...
struct wxWinUIFontFace
{
    wxString name;

    // As DirectWrite gives them: 1 to 999 and 1 to 9.
    int weight = DWRITE_FONT_WEIGHT_NORMAL;
    int stretch = DWRITE_FONT_STRETCH_NORMAL;

    // Italic or oblique.
    bool italic = false;

    // Made by slanting the upright face.
    bool slanted = false;

    // What GDI calls this face, which is what the dialog gives back: the
    // family is not always the one shown ("Segoe UI Light" for the light
    // style of "Segoe UI").
    LOGFONTW lf;
};

struct wxWinUIFontFamily
{
    wxString name;
    wxCOMPtr<IDWriteFontFamily> family;

    // Its name would not be readable in its own glyphs.
    bool symbol = false;

    bool facesLoaded = false;
    std::vector<wxWinUIFontFace> faces;
};

wxString wxWinUIGetLocalizedName(IDWriteLocalizedStrings *names)
{
    if ( !names )
        return wxString();

    UINT32 index = 0;
    BOOL exists = FALSE;

    // In the language of the interface, not in the one of the formats.
    wchar_t locale[LOCALE_NAME_MAX_LENGTH];
    if ( ::LCIDToLocaleName(
            MAKELCID(::GetUserDefaultUILanguage(), SORT_DEFAULT),
            locale, LOCALE_NAME_MAX_LENGTH, 0) )
    {
        names->FindLocaleName(locale, &index, &exists);
    }
    if ( !exists )
        names->FindLocaleName(L"en-us", &index, &exists);
    if ( !exists )
        index = 0;

    UINT32 length = 0;
    if ( FAILED(names->GetStringLength(index, &length)) )
        return wxString();

    std::vector<wchar_t> buffer(length + 1);
    if ( FAILED(names->GetString(index, buffer.data(), length + 1)) )
        return wxString();

    return wxString(buffer.data(), length);
}

// The sample of the dialog of Windows changes with the script.
wxString wxWinUIGetFontSampleText(int charset)
{
    switch ( charset )
    {
        case GREEK_CHARSET:
            return wxString(L"AaBbΑαΒβ");
        case TURKISH_CHARSET:
            return wxString(L"AaBbĞğŞş");
        case EASTEUROPE_CHARSET:
            return wxString(L"AaBbÁáÔô");
        case RUSSIAN_CHARSET:
            return wxString(L"AaBbБбФф");
        case VIETNAMESE_CHARSET:
            return wxString(L"AaBbƯưƠơ");
        case HEBREW_CHARSET:
            return wxString(L"AaBbנסשת");
        case ARABIC_CHARSET:
            return wxString(L"AaBbابجدهوز");
        case THAI_CHARSET:
            return wxString(L"AaBbอักษรไทย");
        case SHIFTJIS_CHARSET:
            return wxString(L"Aaあぁアァ亜宇");
        case GB2312_CHARSET:
            return wxString(L"微软中文软件");
        case CHINESEBIG5_CHARSET:
            return wxString(L"中文字型範例");
        case HANGUL_CHARSET:
            return wxString(L"가나다AaBbYyZz");
    }

    return wxString(wxS("AaBbYyZz"));
}

double wxWinUIFontLuminance(const wxColour& colour)
{
    return (0.299 * colour.Red() + 0.587 * colour.Green() +
            0.114 * colour.Blue()) / 255.0;
}

// A script of a face, as GDI names it.
struct wxWinUIFontScript
{
    wxString name;
    int charset;
};

// What only GDI tells of a face.
struct wxWinUIFontGdiInfo
{
    std::vector<wxWinUIFontScript> scripts;

    // Zero when the face is unknown to GDI.
    BYTE pitchAndFamily = 0;
};

int CALLBACK wxWinUIFontGdiProc(const LOGFONTW *lf, const TEXTMETRICW *,
                                DWORD, LPARAM lParam)
{
    wxWinUIFontGdiInfo * const
        info = reinterpret_cast<wxWinUIFontGdiInfo *>(lParam);

    if ( !info->pitchAndFamily )
        info->pitchAndFamily = lf->lfPitchAndFamily;

    for ( const wxWinUIFontScript& script : info->scripts )
    {
        if ( script.charset == lf->lfCharSet )
            return 1;
    }

    const ENUMLOGFONTEXW * const
        full = reinterpret_cast<const ENUMLOGFONTEXW *>(lf);

    wxWinUIFontScript script;
    script.name = full->elfScript;
    script.charset = lf->lfCharSet;
    if ( !script.name.empty() )
        info->scripts.push_back(script);

    return 1;
}

// The item of a list which is the text, or else the first one starting with
// it, as the lists of the dialog of Windows follow what is typed above them.
int wxWinUIFindFontListItem(const wxListBox *list, const wxString& text,
                            bool *exact)
{
    *exact = false;
    if ( text.empty() )
        return wxNOT_FOUND;

    const wxString lower = text.Lower();
    int prefix = wxNOT_FOUND;
    const unsigned int count = list->GetCount();
    for ( unsigned int n = 0; n < count; ++n )
    {
        const wxString item = list->GetString(n).Lower();
        if ( item == lower )
        {
            *exact = true;
            return n;
        }

        if ( prefix == wxNOT_FOUND && item.StartsWith(lower) )
            prefix = n;
    }

    return prefix;
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWinUIFontDialogImpl
// ----------------------------------------------------------------------------

class wxWinUIFontDialogImpl
{
public:
    explicit wxWinUIFontDialogImpl(wxFontDialog *dialog)
        : m_dialog(dialog)
    {
    }

    void Create();

private:
    void LoadFamilies();
    void LoadFaces(wxWinUIFontFamily& family);

    void CreateControls();
    wxSizer *CreateColumn(const wxString& label, const wxString& name,
                          int widthDIP,
                          wxTextCtrl **text, wxListBox **list);
    wxBitmap CreateSwatch(const wxColour& colour) const;
    void BindColumnKeys(wxTextCtrl *text, wxListBox *list);

    void SelectInitialFont();
    void ShowSelections();
    bool IsSelectionInView(const wxListBox *list) const;

    // What the user did.
    void OnFamilyText();
    void OnFamilyList();
    void OnStyleText();
    void OnStyleList();
    void OnSizeText();
    void OnSizeList();
    void OnListMoved(wxListBox *list);
    void OnOK(wxCommandEvent& event);

    // Shows the styles and the scripts of a family.
    void SetFamily(int index);
    void SetFace(int index, bool remember);
    int FindClosestFace(const wxWinUIFontFamily& family) const;
    void UpdateScripts();
    void UpdateSample();
    void LayoutSample();

    const wxWinUIFontFace *GetFace() const;
    bool GetPointSize(double *size) const;
    int GetCharset() const;
    wxColour GetColour() const;
    wxFont GetFont() const;

    // Checks what was typed, as the dialog of Windows does, and stores the
    // choice in the font data.
    bool Accept();
    void Refuse(const wxString& message, wxTextCtrl *text);

    wxFontDialog * const m_dialog;

    wxCOMPtr<IDWriteFactory> m_factory;
    wxCOMPtr<IDWriteGdiInterop> m_gdiInterop;
    std::vector<wxWinUIFontFamily> m_families;

    wxTextCtrl *m_familyText = nullptr;
    wxListBox *m_familyList = nullptr;
    wxTextCtrl *m_styleText = nullptr;
    wxListBox *m_styleList = nullptr;
    wxTextCtrl *m_sizeText = nullptr;
    wxListBox *m_sizeList = nullptr;
    wxCheckBox *m_strikeout = nullptr;
    wxCheckBox *m_underline = nullptr;
    wxBitmapComboBox *m_colour = nullptr;
    wxPanel *m_samplePanel = nullptr;
    wxStaticText *m_sampleText = nullptr;
    wxChoice *m_script = nullptr;

    // The family whose styles are shown and the style chosen in it.
    int m_family = wxNOT_FOUND;
    int m_face = wxNOT_FOUND;

    // The style and the script last asked for: another family is shown in
    // the closest ones it has.
    int m_wantedWeight = DWRITE_FONT_WEIGHT_NORMAL;
    int m_wantedStretch = DWRITE_FONT_STRETCH_NORMAL;
    bool m_wantedItalic = false;
    int m_wantedCharset = DEFAULT_CHARSET;

    // The scripts of the style chosen, and what GDI says of its pitch and
    // of its family: the font given back says it too, as the one of the
    // dialog of Windows.
    std::vector<wxWinUIFontScript> m_scripts;
    BYTE m_pitchAndFamily = 0;

    std::vector<wxColour> m_colours;

    // False until a colour is given by the program or chosen by the user:
    // the sample is then in the colour of the text of the theme.
    bool m_colourChosen = false;

    // The effects of the initial font, kept when they cannot be changed.
    bool m_initialUnderline = false;
    bool m_initialStrikeout = false;

    // How many times the lists are still to be asked to show their
    // selection, see the idle handler.
    enum { SCROLL_ATTEMPTS = 200 };
    int m_scrollAttempts = SCROLL_ATTEMPTS;

    bool m_ready = false;
};

void wxWinUIFontDialogImpl::Create()
{
    LoadFamilies();
    CreateControls();
    SelectInitialFont();

    m_ready = true;
    UpdateSample();

    m_dialog->GetSizer()->SetSizeHints(m_dialog);
    m_dialog->CentreOnParent();

    m_familyText->SetFocus();
    m_familyText->SelectAll();
}

void wxWinUIFontDialogImpl::LoadFamilies()
{
    const wxFontData& data = m_dialog->GetFontData();

    if ( FAILED(::DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown **>(&m_factory))) )
    {
        return;
    }

    m_factory->GetGdiInterop(&m_gdiInterop);

    wxCOMPtr<IDWriteFontCollection> collection;
    if ( FAILED(m_factory->GetSystemFontCollection(&collection, FALSE)) )
        return;

    const bool fixedOnly =
        (data.GetRestrictSelection() & wxFONTRESTRICT_FIXEDPITCH) != 0;

    const UINT32 count = collection->GetFontFamilyCount();
    m_families.reserve(count);
    for ( UINT32 n = 0; n < count; ++n )
    {
        wxWinUIFontFamily family;
        if ( FAILED(collection->GetFontFamily(n, &family.family)) )
            continue;

        wxCOMPtr<IDWriteLocalizedStrings> names;
        if ( FAILED(family.family->GetFamilyNames(&names)) )
            continue;

        family.name = wxWinUIGetLocalizedName(names);
        if ( family.name.empty() )
            continue;

        wxCOMPtr<IDWriteFont> font;
        if ( SUCCEEDED(family.family->GetFirstMatchingFont(
                DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                DWRITE_FONT_STYLE_NORMAL, &font)) )
        {
            family.symbol = font->IsSymbolFont() != FALSE;

            if ( fixedOnly )
            {
                wxCOMPtr<IDWriteFont1> font1;
                if ( FAILED(font->QueryInterface(
                        __uuidof(IDWriteFont1),
                        reinterpret_cast<void **>(&font1))) ||
                     !font1->IsMonospacedFont() )
                {
                    continue;
                }
            }
        }

        if ( family.symbol && !data.GetAllowSymbols() )
            continue;

        m_families.push_back(family);
    }

    std::sort(m_families.begin(), m_families.end(),
              [](const wxWinUIFontFamily& a, const wxWinUIFontFamily& b)
              {
                  return a.name.CmpNoCase(b.name) < 0;
              });
}

void wxWinUIFontDialogImpl::LoadFaces(wxWinUIFontFamily& family)
{
    if ( family.facesLoaded )
        return;

    family.facesLoaded = true;
    if ( !family.family || !m_gdiInterop )
        return;

    // The simulated styles are among them: a family without a bold or a
    // slanted face has "Bold" and "Oblique" all the same, as in Windows.
    const UINT32 count = family.family->GetFontCount();
    for ( UINT32 n = 0; n < count; ++n )
    {
        wxCOMPtr<IDWriteFont> font;
        if ( FAILED(family.family->GetFont(n, &font)) )
            continue;

        wxWinUIFontFace face;

        wxCOMPtr<IDWriteLocalizedStrings> names;
        if ( SUCCEEDED(font->GetFaceNames(&names)) )
            face.name = wxWinUIGetLocalizedName(names);
        if ( face.name.empty() )
            continue;

        face.weight = font->GetWeight();
        face.stretch = font->GetStretch();
        face.italic = font->GetStyle() != DWRITE_FONT_STYLE_NORMAL;
        face.slanted =
            (font->GetSimulations() & DWRITE_FONT_SIMULATIONS_OBLIQUE) != 0;

        BOOL isSystemFont = FALSE;
        if ( FAILED(m_gdiInterop->ConvertFontToLOGFONT(
                font, &face.lf, &isSystemFont)) )
        {
            continue;
        }

        bool known = false;
        for ( const wxWinUIFontFace& other : family.faces )
        {
            if ( other.name == face.name )
            {
                known = true;
                break;
            }
        }

        if ( !known )
            family.faces.push_back(face);
    }

    // DirectWrite slants the upright faces even of a family which has true
    // italics: "Oblique" beside "Italic". The dialog of Windows shows the
    // true ones alone.
    std::vector<wxWinUIFontFace> faces;
    for ( const wxWinUIFontFace& face : family.faces )
    {
        bool redundant = false;
        if ( face.slanted )
        {
            for ( const wxWinUIFontFace& other : family.faces )
            {
                if ( other.italic && !other.slanted &&
                     other.weight == face.weight &&
                     other.stretch == face.stretch )
                {
                    redundant = true;
                    break;
                }
            }
        }

        if ( !redundant )
            faces.push_back(face);
    }
    family.faces.swap(faces);

    std::stable_sort(
        family.faces.begin(), family.faces.end(),
        [](const wxWinUIFontFace& a, const wxWinUIFontFace& b)
        {
            if ( a.stretch != b.stretch )
                return a.stretch < b.stretch;
            if ( a.weight != b.weight )
                return a.weight < b.weight;
            return !a.italic && b.italic;
        });
}

wxSizer *
wxWinUIFontDialogImpl::CreateColumn(const wxString& label,
                                    const wxString& name, int widthDIP,
                                    wxTextCtrl **text, wxListBox **list)
{
    wxBoxSizer * const column = new wxBoxSizer(wxVERTICAL);

    const int gap = m_dialog->FromDIP(4);
    column->Add(new wxStaticText(m_dialog, wxID_ANY, label),
                0, wxBOTTOM, gap);

    // Named, to be found by a program driving the dialog.
    *text = new wxTextCtrl(m_dialog, wxID_ANY);
    (*text)->SetName(name + wxS("Text"));
    column->Add(*text, 0, wxEXPAND | wxBOTTOM, gap);

    // In a thin frame, as a list of Windows is.
    wxPanel * const frame = new wxPanel(
        m_dialog, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxBORDER_THEME | wxTAB_TRAVERSAL);
    const wxSize listSize =
        m_dialog->FromDIP(wxSize(widthDIP, LIST_ROWS * LIST_ROW_DIP));
    *list = new wxListBox(
        frame, wxID_ANY, wxDefaultPosition, listSize,
        0, nullptr, wxLB_SINGLE);
    (*list)->SetName(name + wxS("List"));

    wxBoxSizer * const inside = new wxBoxSizer(wxVERTICAL);
    inside->Add(*list, 1, wxEXPAND);
    frame->SetSizer(inside);

    // Set, not left to be computed: the size a window with a border asks
    // for is found from the size it has at the time, and so changes between
    // a window not yet laid out and the same one on screen.
    frame->SetMinSize(listSize + frame->GetWindowBorderSize());
    column->Add(frame, 1, wxEXPAND);

    return column;
}

wxBitmap wxWinUIFontDialogImpl::CreateSwatch(const wxColour& colour) const
{
    // The swatch of a colour of WinUI: rounded, with a thin outline.
    return wxWinUICreateColourSwatch(colour, wxSize(16, 16),
                                     m_dialog->GetDPIScaleFactor());
}

void wxWinUIFontDialogImpl::BindColumnKeys(wxTextCtrl *text, wxListBox *list)
{
    // The arrows move in the list from the text field above it.
    text->Bind(wxEVT_KEY_DOWN, [this, list](wxKeyEvent& event)
    {
        int delta = 0;
        switch ( event.GetKeyCode() )
        {
            case WXK_UP:
                delta = -1;
                break;
            case WXK_DOWN:
                delta = 1;
                break;
            case WXK_PAGEUP:
                delta = -(LIST_ROWS - 1);
                break;
            case WXK_PAGEDOWN:
                delta = LIST_ROWS - 1;
                break;
        }

        const int count = static_cast<int>(list->GetCount());
        if ( !delta || !count )
        {
            event.Skip();
            return;
        }

        int n = list->GetSelection();
        n = n == wxNOT_FOUND ? 0 : wxMin(count - 1, wxMax(0, n + delta));
        list->SetSelection(n);
        list->EnsureVisible(n);
        OnListMoved(list);
    });
}

void wxWinUIFontDialogImpl::CreateControls()
{
    const wxFontData& data = m_dialog->GetFontData();

    const int gap = m_dialog->FromDIP(12);
    const int inner = m_dialog->FromDIP(8);

    // Two columns and two rows: the fonts above the effects, the styles and
    // the sizes above the sample and the script.
    wxFlexGridSizer * const grid = new wxFlexGridSizer(2, gap, gap);
    grid->AddGrowableCol(0, 1);
    grid->AddGrowableCol(1, 1);
    grid->AddGrowableRow(0, 1);

    grid->Add(CreateColumn(_("&Font:"), wxS("fontFamily"), 240,
                           &m_familyText, &m_familyList),
              1, wxEXPAND);

    wxBoxSizer * const styleAndSize = new wxBoxSizer(wxHORIZONTAL);
    styleAndSize->Add(
        CreateColumn(_("Font st&yle:"), wxS("fontStyle"), 180,
                     &m_styleText, &m_styleList),
        1, wxEXPAND | wxRIGHT, gap);
    styleAndSize->Add(
        CreateColumn(_("&Size:"), wxS("fontSize"), 84,
                     &m_sizeText, &m_sizeList),
        0, wxEXPAND);
    grid->Add(styleAndSize, 1, wxEXPAND);

    // Each name in its own typeface, each style in the style it names.
    wxWinUISetListItemFontProvider(
        m_familyList,
        [this](unsigned int n, wxWinUIListItemFont& font)
        {
            if ( n >= m_families.size() || m_families[n].symbol )
                return false;

            font.family = m_families[n].name;
            return true;
        });
    wxWinUISetListItemFontProvider(
        m_styleList,
        [this](unsigned int n, wxWinUIListItemFont& font)
        {
            if ( m_family == wxNOT_FOUND )
                return false;

            const wxWinUIFontFamily& family = m_families[m_family];
            if ( family.symbol || n >= family.faces.size() )
                return false;

            font.family = family.name;
            font.weight = family.faces[n].weight;
            font.stretch = family.faces[n].stretch;
            font.italic = family.faces[n].italic;
            return true;
        });

    // The sizes in the font of the dialog, in rows like the ones of the two
    // other lists.
    wxWinUISetListItemFontProvider(
        m_sizeList,
        [](unsigned int, wxWinUIListItemFont&) { return false; });

    wxArrayString names;
    names.reserve(m_families.size());
    for ( const wxWinUIFontFamily& family : m_families )
        names.push_back(family.name);
    m_familyList->Set(names);

    wxArrayString sizes;
    for ( const int size : gs_fontSizes )
    {
        if ( (data.m_minSize && size < data.m_minSize) ||
             (data.m_maxSize && size > data.m_maxSize) )
        {
            continue;
        }

        sizes.push_back(wxString::Format(wxS("%d"), size));
    }
    m_sizeList->Set(sizes);

    if ( data.GetEnableEffects() )
    {
        wxStaticBoxSizer * const
            effects = new wxStaticBoxSizer(wxVERTICAL, m_dialog, _("Effects"));
        wxWindow * const box = effects->GetStaticBox();

        m_strikeout = new wxCheckBox(box, wxID_ANY, _("Stri&keout"));
        effects->Add(m_strikeout, 0, wxLEFT | wxRIGHT | wxTOP, inner);

        m_underline = new wxCheckBox(box, wxID_ANY, _("&Underline"));
        effects->Add(m_underline, 0, wxLEFT | wxRIGHT | wxTOP, inner);

        effects->Add(new wxStaticText(box, wxID_ANY, _("&Color:")),
                     0, wxLEFT | wxRIGHT | wxTOP, inner);

        m_colour = new wxBitmapComboBox(
            box, wxID_ANY, wxString(), wxDefaultPosition, wxDefaultSize,
            0, nullptr, wxCB_READONLY);
        for ( const wxWinUIFontColour& entry : gs_fontColours )
        {
            const wxColour colour(entry.red, entry.green, entry.blue);
            m_colours.push_back(colour);
            m_colour->Append(wxGetTranslation(entry.name),
                             CreateSwatch(colour));
        }
        effects->Add(m_colour, 0, wxEXPAND | wxALL, inner);

        grid->Add(effects, 0, wxEXPAND);
    }
    else
    {
        grid->AddSpacer(0);
    }

    wxBoxSizer * const right = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer * const
        sample = new wxStaticBoxSizer(wxVERTICAL, m_dialog, _("Sample"));
    m_samplePanel = new wxPanel(sample->GetStaticBox(), wxID_ANY);

    // Both set: the size of the sample must not follow the one of its text,
    // which is as large as the font chosen.
    m_samplePanel->SetMinSize(m_dialog->FromDIP(wxSize(220, 64)));
    m_sampleText = new wxStaticText(m_samplePanel, wxID_ANY,
                                    wxWinUIGetFontSampleText(ANSI_CHARSET));
    sample->Add(m_samplePanel, 1, wxEXPAND | wxALL, inner);
    right->Add(sample, 1, wxEXPAND);

    right->Add(new wxStaticText(m_dialog, wxID_ANY, _("Sc&ript:")),
               0, wxTOP, inner);
    m_script = new wxChoice(m_dialog, wxID_ANY);
    right->Add(m_script, 0, wxEXPAND | wxTOP, m_dialog->FromDIP(4));

    grid->Add(right, 1, wxEXPAND);

    wxBoxSizer * const top = new wxBoxSizer(wxVERTICAL);
    top->Add(grid, 1, wxEXPAND | wxALL, m_dialog->FromDIP(16));

    long buttons = wxOK | wxCANCEL;
    if ( data.GetShowHelp() )
        buttons |= wxHELP;
    if ( wxSizer * const sizer = m_dialog->CreateStdDialogButtonSizer(buttons) )
    {
        top->Add(sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM,
                 m_dialog->FromDIP(16));
    }

    m_dialog->SetSizer(top);

    m_familyText->Bind(wxEVT_TEXT, [this](wxCommandEvent&) { OnFamilyText(); });
    m_familyList->Bind(wxEVT_LISTBOX, [this](wxCommandEvent&) { OnFamilyList(); });
    m_styleText->Bind(wxEVT_TEXT, [this](wxCommandEvent&) { OnStyleText(); });
    m_styleList->Bind(wxEVT_LISTBOX, [this](wxCommandEvent&) { OnStyleList(); });
    m_sizeText->Bind(wxEVT_TEXT, [this](wxCommandEvent&) { OnSizeText(); });
    m_sizeList->Bind(wxEVT_LISTBOX, [this](wxCommandEvent&) { OnSizeList(); });

    BindColumnKeys(m_familyText, m_familyList);
    BindColumnKeys(m_styleText, m_styleList);
    BindColumnKeys(m_sizeText, m_sizeList);

    if ( m_strikeout )
    {
        m_strikeout->Bind(wxEVT_CHECKBOX,
                          [this](wxCommandEvent&) { UpdateSample(); });
        m_underline->Bind(wxEVT_CHECKBOX,
                          [this](wxCommandEvent&) { UpdateSample(); });
        m_colour->Bind(wxEVT_COMBOBOX, [this](wxCommandEvent&)
        {
            m_colourChosen = true;
            UpdateSample();
        });
    }

    m_script->Bind(wxEVT_CHOICE, [this](wxCommandEvent&)
    {
        m_wantedCharset = GetCharset();
        UpdateSample();
    });

    m_samplePanel->Bind(wxEVT_SIZE, [this](wxSizeEvent& event)
    {
        LayoutSample();
        event.Skip();
    });

    m_dialog->Bind(wxEVT_BUTTON, &wxWinUIFontDialogImpl::OnOK, this, wxID_OK);

    // A list scrolls only once it is on screen and laid out, which is some
    // time after the dialog is shown: ask again until the font is in view.
    m_dialog->Bind(wxEVT_SHOW, [this](wxShowEvent& event)
    {
        event.Skip();
        if ( event.IsShown() )
            m_scrollAttempts = SCROLL_ATTEMPTS;
    });
    m_dialog->Bind(wxEVT_IDLE, [this](wxIdleEvent& event)
    {
        event.Skip();
        if ( !m_scrollAttempts || !m_dialog->IsShown() )
            return;

        if ( IsSelectionInView(m_familyList) )
        {
            m_scrollAttempts = 0;
            return;
        }

        --m_scrollAttempts;
        ShowSelections();
        event.RequestMore();
    });
}

bool wxWinUIFontDialogImpl::IsSelectionInView(const wxListBox *list) const
{
    const int n = list->GetSelection();
    if ( n == wxNOT_FOUND )
        return true;

    const int top = list->GetTopItem();
    const int page = list->GetCountPerPage();
    return top != wxNOT_FOUND && page > 0 && n >= top && n < top + page;
}

void wxWinUIFontDialogImpl::ShowSelections()
{
    // The font chosen is the first of its list, as in the dialog of Windows.
    const int family = m_familyList->GetSelection();
    if ( family != wxNOT_FOUND )
        m_familyList->SetFirstItem(family);

    const int style = m_styleList->GetSelection();
    if ( style != wxNOT_FOUND )
        m_styleList->EnsureVisible(style);

    const int size = m_sizeList->GetSelection();
    if ( size != wxNOT_FOUND )
        m_sizeList->EnsureVisible(size);
}

void wxWinUIFontDialogImpl::SelectInitialFont()
{
    const wxFontData& data = m_dialog->GetFontData();

    wxFont font = data.GetInitialFont();
    if ( !font.IsOk() )
        font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    LOGFONTW lf = font.GetNativeFontInfo()->lf;

    wxString familyName = lf.lfFaceName;
    m_wantedWeight = lf.lfWeight ? lf.lfWeight : FW_NORMAL;
    m_wantedItalic = lf.lfItalic != 0;
    m_wantedCharset = lf.lfCharSet;

    // The family the dialog shows is not always the one GDI names: the
    // light face of "Segoe UI" is "Segoe UI Light" for GDI.
    wxCOMPtr<IDWriteFont> dwFont;
    if ( m_gdiInterop &&
         SUCCEEDED(m_gdiInterop->CreateFontFromLOGFONT(&lf, &dwFont)) )
    {
        wxCOMPtr<IDWriteFontFamily> family;
        wxCOMPtr<IDWriteLocalizedStrings> names;
        if ( SUCCEEDED(dwFont->GetFontFamily(&family)) &&
             SUCCEEDED(family->GetFamilyNames(&names)) )
        {
            const wxString name = wxWinUIGetLocalizedName(names);
            if ( !name.empty() )
            {
                familyName = name;
                m_wantedWeight = dwFont->GetWeight();
                m_wantedStretch = dwFont->GetStretch();
                m_wantedItalic =
                    dwFont->GetStyle() != DWRITE_FONT_STYLE_NORMAL;
            }
        }
    }

    const double size = font.GetFractionalPointSize();
    const wxString sizeText = size == std::floor(size)
        ? wxString::Format(wxS("%d"), static_cast<int>(size))
        : wxString::Format(wxS("%g"), size);
    m_sizeText->ChangeValue(sizeText);
    m_sizeList->SetSelection(m_sizeList->FindString(sizeText));

    m_initialUnderline = font.GetUnderlined();
    m_initialStrikeout = font.GetStrikethrough();
    if ( m_strikeout )
    {
        m_underline->SetValue(m_initialUnderline);
        m_strikeout->SetValue(m_initialStrikeout);

        // The colour of the program stays its choice even when it is not
        // one of the sixteen.
        const wxColour colour = data.GetColour();
        m_colourChosen = colour.IsOk();
        int selection = 0;
        if ( colour.IsOk() )
        {
            selection = wxNOT_FOUND;
            for ( std::size_t n = 0; n < m_colours.size(); ++n )
            {
                if ( m_colours[n].GetRGB() == colour.GetRGB() )
                {
                    selection = static_cast<int>(n);
                    break;
                }
            }

            if ( selection == wxNOT_FOUND )
            {
                m_colours.push_back(colour);
                selection = m_colour->Append(_("Custom"),
                                             CreateSwatch(colour));
            }
        }
        m_colour->SetSelection(selection);
    }

    m_familyText->ChangeValue(familyName);

    bool exact = false;
    const int index =
        wxWinUIFindFontListItem(m_familyList, familyName, &exact);
    if ( exact )
    {
        m_familyList->SetSelection(index);
        m_familyList->SetFirstItem(index);
        SetFamily(index);
    }
}

// ----------------------------------------------------------------------------
// what the user did
// ----------------------------------------------------------------------------

void wxWinUIFontDialogImpl::OnFamilyText()
{
    if ( !m_ready )
        return;

    bool exact = false;
    const int n = wxWinUIFindFontListItem(
        m_familyList, m_familyText->GetValue(), &exact);
    m_familyList->SetSelection(n);
    if ( n == wxNOT_FOUND )
        return;

    m_familyList->SetFirstItem(n);
    SetFamily(n);
}

void wxWinUIFontDialogImpl::OnFamilyList()
{
    const int n = m_familyList->GetSelection();
    if ( !m_ready || n == wxNOT_FOUND )
        return;

    m_familyText->ChangeValue(m_familyList->GetString(n));
    SetFamily(n);
}

void wxWinUIFontDialogImpl::OnStyleText()
{
    if ( !m_ready )
        return;

    bool exact = false;
    const int n = wxWinUIFindFontListItem(
        m_styleList, m_styleText->GetValue(), &exact);
    m_styleList->SetSelection(n);
    if ( n == wxNOT_FOUND )
        return;

    m_styleList->EnsureVisible(n);
    SetFace(n, true);
}

void wxWinUIFontDialogImpl::OnStyleList()
{
    const int n = m_styleList->GetSelection();
    if ( !m_ready || n == wxNOT_FOUND )
        return;

    m_styleText->ChangeValue(m_styleList->GetString(n));
    SetFace(n, true);
}

void wxWinUIFontDialogImpl::OnSizeText()
{
    if ( !m_ready )
        return;

    wxString text = m_sizeText->GetValue();
    text.Trim().Trim(false);
    const int n = text.empty() ? wxNOT_FOUND : m_sizeList->FindString(text);
    m_sizeList->SetSelection(n);
    if ( n != wxNOT_FOUND )
        m_sizeList->EnsureVisible(n);

    UpdateSample();
}

void wxWinUIFontDialogImpl::OnSizeList()
{
    const int n = m_sizeList->GetSelection();
    if ( !m_ready || n == wxNOT_FOUND )
        return;

    m_sizeText->ChangeValue(m_sizeList->GetString(n));
    UpdateSample();
}

void wxWinUIFontDialogImpl::OnListMoved(wxListBox *list)
{
    if ( list == m_familyList )
        OnFamilyList();
    else if ( list == m_styleList )
        OnStyleList();
    else
        OnSizeList();
}

void wxWinUIFontDialogImpl::OnOK(wxCommandEvent& event)
{
    // The dialog closes only when what was typed is a font.
    if ( Accept() )
        event.Skip();
}

// ----------------------------------------------------------------------------
// the family, its styles and its scripts
// ----------------------------------------------------------------------------

void wxWinUIFontDialogImpl::SetFamily(int index)
{
    if ( index == m_family || index < 0 ||
         index >= static_cast<int>(m_families.size()) )
    {
        return;
    }

    wxWinUIFontFamily& family = m_families[index];
    LoadFaces(family);

    // Before the list is filled: the typeface of each of its items is asked
    // for as it is.
    m_family = index;
    m_face = wxNOT_FOUND;

    wxArrayString names;
    for ( const wxWinUIFontFace& face : family.faces )
        names.push_back(face.name);
    m_styleList->Set(names);

    const int face = FindClosestFace(family);
    if ( face != wxNOT_FOUND )
    {
        m_styleList->SetSelection(face);
        m_styleList->EnsureVisible(face);
        m_styleText->ChangeValue(names[face]);
    }

    SetFace(face, false);
}

void wxWinUIFontDialogImpl::SetFace(int index, bool remember)
{
    m_face = index;

    const wxWinUIFontFace * const face = GetFace();
    if ( face && remember )
    {
        m_wantedWeight = face->weight;
        m_wantedStretch = face->stretch;
        m_wantedItalic = face->italic;
    }

    UpdateScripts();
    UpdateSample();
}

int
wxWinUIFontDialogImpl::FindClosestFace(const wxWinUIFontFamily& family) const
{
    int best = wxNOT_FOUND;
    double bestDistance = 0.0;
    for ( std::size_t n = 0; n < family.faces.size(); ++n )
    {
        const wxWinUIFontFace& face = family.faces[n];

        // The width first, then the slope, then the weight.
        const double distance =
            std::abs(face.stretch - m_wantedStretch) * 100.0 +
            (face.italic != m_wantedItalic ? 50.0 : 0.0) +
            std::abs(face.weight - m_wantedWeight) / 100.0;
        if ( best == wxNOT_FOUND || distance < bestDistance )
        {
            best = static_cast<int>(n);
            bestDistance = distance;
        }
    }

    return best;
}

void wxWinUIFontDialogImpl::UpdateScripts()
{
    wxWinUIFontGdiInfo info;
    if ( const wxWinUIFontFace * const face = GetFace() )
    {
        LOGFONTW lf;
        wxZeroMemory(lf);
        lf.lfCharSet = DEFAULT_CHARSET;
        wxStrlcpy(lf.lfFaceName, face->lf.lfFaceName, WXSIZEOF(lf.lfFaceName));

        ::EnumFontFamiliesExW(ScreenHDC(), &lf, wxWinUIFontGdiProc,
                              reinterpret_cast<LPARAM>(&info), 0);
    }

    m_scripts = info.scripts;
    m_pitchAndFamily = info.pitchAndFamily;

    wxArrayString names;
    int selection = m_scripts.empty() ? wxNOT_FOUND : 0;
    for ( std::size_t n = 0; n < m_scripts.size(); ++n )
    {
        names.push_back(m_scripts[n].name);
        if ( m_scripts[n].charset == m_wantedCharset )
            selection = static_cast<int>(n);
    }

    m_script->Set(names);
    m_script->SetSelection(selection);
    m_script->Enable(!m_scripts.empty());
}

void wxWinUIFontDialogImpl::UpdateSample()
{
    if ( !m_ready )
        return;

    const wxFont font = GetFont();
    if ( font.IsOk() )
        m_sampleText->SetFont(font);
    m_sampleText->SetLabel(wxWinUIGetFontSampleText(GetCharset()));

    // In the colour chosen, on a ground it can be read on.
    wxColour colour;
    wxColour ground;
    if ( m_colourChosen )
    {
        colour = GetColour();

        const double text = wxWinUIFontLuminance(colour);
        const double window = wxWinUIFontLuminance(
            wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        if ( std::abs(text - window) < 0.35 )
            ground = text > 0.5 ? wxColour(32, 32, 32) : *wxWHITE;
    }

    m_sampleText->SetForegroundColour(colour);
    m_samplePanel->SetBackgroundColour(ground);
    m_samplePanel->Refresh();

    LayoutSample();
}

void wxWinUIFontDialogImpl::LayoutSample()
{
    // In the middle, and cut on both sides when larger than the sample.
    const wxSize area = m_samplePanel->GetClientSize();
    const wxSize best = m_sampleText->GetBestSize();
    m_sampleText->SetSize((area.x - best.x) / 2, (area.y - best.y) / 2,
                          best.x, best.y);
}

// ----------------------------------------------------------------------------
// the choice
// ----------------------------------------------------------------------------

const wxWinUIFontFace *wxWinUIFontDialogImpl::GetFace() const
{
    if ( m_family == wxNOT_FOUND || m_face == wxNOT_FOUND )
        return nullptr;

    const wxWinUIFontFamily& family = m_families[m_family];
    if ( m_face >= static_cast<int>(family.faces.size()) )
        return nullptr;

    return &family.faces[m_face];
}

bool wxWinUIFontDialogImpl::GetPointSize(double *size) const
{
    wxString text = m_sizeText->GetValue();
    text.Trim().Trim(false);

    return (text.ToDouble(size) || text.ToCDouble(size)) &&
           *size >= 1.0 && *size <= 1638.0;
}

int wxWinUIFontDialogImpl::GetCharset() const
{
    const int n = m_script ? m_script->GetSelection() : wxNOT_FOUND;
    if ( n == wxNOT_FOUND || n >= static_cast<int>(m_scripts.size()) )
        return DEFAULT_CHARSET;

    return m_scripts[n].charset;
}

wxColour wxWinUIFontDialogImpl::GetColour() const
{
    const int n = m_colour ? m_colour->GetSelection() : wxNOT_FOUND;
    if ( n == wxNOT_FOUND || n >= static_cast<int>(m_colours.size()) )
    {
        // As the dialog of Windows: black unless the program said otherwise.
        const wxColour colour = m_dialog->GetFontData().GetColour();
        return colour.IsOk() ? colour : *wxBLACK;
    }

    return m_colours[n];
}

wxFont wxWinUIFontDialogImpl::GetFont() const
{
    const wxWinUIFontFace * const face = GetFace();
    double pointSize = 0.0;
    if ( !face || !GetPointSize(&pointSize) )
        return wxNullFont;

    LOGFONTW lf = face->lf;
    lf.lfHeight = wxNativeFontInfo::GetLogFontHeightAtPPI(
        pointSize, wxGetDPIofHDC(ScreenHDC()).y);
    lf.lfWidth = 0;
    lf.lfUnderline = m_underline ? m_underline->GetValue()
                                 : m_initialUnderline;
    lf.lfStrikeOut = m_strikeout ? m_strikeout->GetValue()
                                 : m_initialStrikeout;

    if ( m_pitchAndFamily )
        lf.lfPitchAndFamily = m_pitchAndFamily;

    const int charset = GetCharset();
    if ( charset != DEFAULT_CHARSET )
        lf.lfCharSet = static_cast<BYTE>(charset);

    // The size in points is the one typed, not the one found again from a
    // height in pixels.
    wxFont font(wxNativeFontInfo(lf, nullptr));
    font.SetFractionalPointSize(pointSize);
    return font;
}

void wxWinUIFontDialogImpl::Refuse(const wxString& message, wxTextCtrl *text)
{
    wxMessageBox(message, m_dialog->GetTitle(), wxOK | wxICON_INFORMATION,
                 m_dialog);
    text->SetFocus();
    text->SelectAll();
}

bool wxWinUIFontDialogImpl::Accept()
{
    wxFontData& data = m_dialog->GetFontData();

    if ( m_family == wxNOT_FOUND ||
         !m_familyText->GetValue().IsSameAs(m_families[m_family].name, false) )
    {
        Refuse(_("There is no font with that name.\n"
                 "Choose a font from the list of fonts."), m_familyText);
        return false;
    }

    const wxWinUIFontFace * const face = GetFace();
    if ( !face || !m_styleText->GetValue().IsSameAs(face->name, false) )
    {
        Refuse(_("This font is not available in that style.\n"
                 "Choose a style from the list of styles."), m_styleText);
        return false;
    }

    double pointSize = 0.0;
    if ( !GetPointSize(&pointSize) )
    {
        Refuse(_("Size must be a number."), m_sizeText);
        return false;
    }

    if ( (data.m_minSize && pointSize < data.m_minSize) ||
         (data.m_maxSize && pointSize > data.m_maxSize) )
    {
        Refuse(data.m_maxSize
                ? wxString::Format(_("Size must be between %d and %d points."),
                                   data.m_minSize, data.m_maxSize)
                : wxString::Format(_("Size must be at least %d points."),
                                   data.m_minSize),
               m_sizeText);
        return false;
    }

    const wxFont font = GetFont();
    if ( !font.IsOk() )
        return false;

    const LOGFONTW lf = font.GetNativeFontInfo()->lf;

    data.SetChosenFont(font);
    data.SetColour(GetColour());
    data.EncodingInfo().facename = lf.lfFaceName;
    data.EncodingInfo().charset = lf.lfCharSet;

    return true;
}

// ----------------------------------------------------------------------------
// wxFontDialog
// ----------------------------------------------------------------------------

wxFontDialog::~wxFontDialog()
{
    delete m_impl;
}

bool wxFontDialog::DoCreate(wxWindow *parent)
{
    parent = GetParentForModalDialog(parent, 0);

    if ( !wxDialog::Create(parent, wxID_ANY, _("Font"),
                           wxDefaultPosition, wxDefaultSize,
                           wxDEFAULT_DIALOG_STYLE,
                           wxASCII_STR("fontdialog")) )
    {
        wxFAIL_MSG( wxT("wxFontDialog creation failed") );
        return false;
    }

    m_impl = new wxWinUIFontDialogImpl(this);
    m_impl->Create();

    return true;
}

#endif // wxUSE_FONTDLG && wxUSE_WINUI3
