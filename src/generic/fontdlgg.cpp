/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/fontdlgg.cpp
// Purpose:     Generic font dialog
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "fontdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_FONTDLG && (!defined(__WXGTK__) || defined(__WXUNIVERSAL__))

#ifndef WX_PRECOMP
    #include <stdio.h>
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/listbox.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/layout.h"
    #include "wx/dcclient.h"
    #include "wx/choice.h"
    #include "wx/checkbox.h"
    #include "wx/intl.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "wx/cmndata.h"
#include "wx/sizer.h"
#include "wx/fontdlg.h"

//-----------------------------------------------------------------------------
// helper class - wxFontPreviewer
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxFontPreviewer : public wxWindow
{
public:
    wxFontPreviewer(wxWindow *parent) : wxWindow(parent, -1) {}

private:
    void OnPaint(wxPaintEvent& event);
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxFontPreviewer, wxWindow)
    EVT_PAINT(wxFontPreviewer::OnPaint)
END_EVENT_TABLE()

void wxFontPreviewer::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxSize size = GetSize();
    wxFont font = GetFont();

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(0, 0, size.x, size.y);

    if ( font.Ok() )
    {
        dc.SetFont(font);
        // Calculate vertical centre
        long w, h;
        dc.GetTextExtent( wxT("X"), &w, &h);
        dc.SetTextForeground(GetForegroundColour());
        dc.SetClippingRegion(2, 2, size.x-4, size.y-4);
        dc.DrawText(_("ABCDEFGabcdefg12345"), 
                     10, size.y/2 - h/2);
        dc.DestroyClippingRegion();
    }
}

//-----------------------------------------------------------------------------
// wxGenericFontDialog
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericFontDialog, wxDialog)

BEGIN_EVENT_TABLE(wxGenericFontDialog, wxDialog)
    EVT_CHECKBOX(wxID_FONT_UNDERLINE, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_STYLE, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_WEIGHT, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_FAMILY, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_COLOUR, wxGenericFontDialog::OnChangeFont)
    EVT_CHOICE(wxID_FONT_SIZE, wxGenericFontDialog::OnChangeFont)
    EVT_CLOSE(wxGenericFontDialog::OnCloseWindow)
END_EVENT_TABLE()


#define NUM_COLS 48
static wxString wxColourDialogNames[NUM_COLS]={wxT("ORANGE"),
                    wxT("GOLDENROD"),
                    wxT("WHEAT"),
                    wxT("SPRING GREEN"),
                    wxT("SKY BLUE"),
                    wxT("SLATE BLUE"),
                    wxT("MEDIUM VIOLET RED"),
                    wxT("PURPLE"),

                    wxT("RED"),
                    wxT("YELLOW"),
                    wxT("MEDIUM SPRING GREEN"),
                    wxT("PALE GREEN"),
                    wxT("CYAN"),
                    wxT("LIGHT STEEL BLUE"),
                    wxT("ORCHID"),
                    wxT("LIGHT MAGENTA"),

                    wxT("BROWN"),
                    wxT("YELLOW"),
                    wxT("GREEN"),
                    wxT("CADET BLUE"),
                    wxT("MEDIUM BLUE"),
                    wxT("MAGENTA"),
                    wxT("MAROON"),
                    wxT("ORANGE RED"),

                    wxT("FIREBRICK"),
                    wxT("CORAL"),
                    wxT("FOREST GREEN"),
                    wxT("AQUARAMINE"),
                    wxT("BLUE"),
                    wxT("NAVY"),
                    wxT("THISTLE"),
                    wxT("MEDIUM VIOLET RED"),

                    wxT("INDIAN RED"),
                    wxT("GOLD"),
                    wxT("MEDIUM SEA GREEN"),
                    wxT("MEDIUM BLUE"),
                    wxT("MIDNIGHT BLUE"),
                    wxT("GREY"),
                    wxT("PURPLE"),
                    wxT("KHAKI"),

                    wxT("BLACK"),
                    wxT("MEDIUM FOREST GREEN"),
                    wxT("KHAKI"),
                    wxT("DARK GREY"),
                    wxT("SEA GREEN"),
                    wxT("LIGHT GREY"),
                    wxT("MEDIUM SLATE BLUE"),
                    wxT("WHITE")
                    };

/*
 * Generic wxFontDialog
 */

void wxGenericFontDialog::Init()
{
  m_useEvents = FALSE;
  m_previewer = NULL;
  Create( m_parent ) ;
}

wxGenericFontDialog::~wxGenericFontDialog()
{
}

void wxGenericFontDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
  EndModal(wxID_CANCEL);
}

bool wxGenericFontDialog::DoCreate(wxWindow *parent)
{
    if ( !wxDialog::Create( parent , -1 , _T("Choose Font") , wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE,
        _T("fontdialog") ) )
    {
        wxFAIL_MSG( wxT("wxFontDialog creation failed") );
        return FALSE;
    }

  InitializeFont();
  CreateWidgets();
 
  // sets initial font in preview area
  wxCommandEvent dummy;
  OnChangeFont(dummy);
  
  return TRUE;
}

int wxGenericFontDialog::ShowModal()
{
    int ret = wxDialog::ShowModal();

    if (ret != wxID_CANCEL)
    {
      m_fontData.chosenFont = dialogFont;
    }

    return ret;
}

void wxGenericFontDialog::CreateWidgets()
{
  wxBusyCursor bcur;

  wxString
     *families = new wxString[6],
     *styles = new wxString[3],
     *weights = new wxString[3];
  families[0] =  _("Roman");
  families[1] = _("Decorative");
  families[2] = _("Modern");
  families[3] = _("Script");
  families[4] = _("Swiss" );
  families[5] = _("Teletype" );
  styles[0] = _("Normal");
  styles[1] = _("Italic");
  styles[2] = _("Slant");
  weights[0] = _("Normal");
  weights[1] = _("Light");
  weights[2] = _("Bold");

  familyChoice = new wxChoice(this, wxID_FONT_FAMILY, wxDefaultPosition, wxDefaultSize, 5, families);
  styleChoice = new wxChoice(this, wxID_FONT_STYLE, wxDefaultPosition, wxDefaultSize, 3, styles);
  weightChoice = new wxChoice(this, wxID_FONT_WEIGHT, wxDefaultPosition, wxDefaultSize, 3, weights);

  colourChoice = new wxChoice(this, wxID_FONT_COLOUR, wxDefaultPosition, wxDefaultSize, NUM_COLS, wxColourDialogNames);

  wxString *pointSizes = new wxString[40];
  int i;
  for ( i = 0; i < 40; i++)
  {
    wxChar buf[5];
    wxSprintf(buf, wxT("%d"), i + 1);
    pointSizes[i] = buf;
  }

  pointSizeChoice = new wxChoice(this, wxID_FONT_SIZE, wxDefaultPosition, wxDefaultSize, 40, pointSizes);
  underLineCheckBox = new wxCheckBox(this, wxID_FONT_UNDERLINE, _("Underline"));

  m_previewer = new wxFontPreviewer(this);

  wxButton *okButton = new wxButton(this, wxID_OK, _("OK"));
  wxButton *cancelButton = new wxButton(this, wxID_CANCEL, _("Cancel"));

  familyChoice->SetStringSelection( wxFontFamilyIntToString(dialogFont.GetFamily()) );
  styleChoice->SetStringSelection(wxFontStyleIntToString(dialogFont.GetStyle()));
  weightChoice->SetStringSelection(wxFontWeightIntToString(dialogFont.GetWeight()));
  wxString name(wxTheColourDatabase->FindName(m_fontData.fontColour));
  colourChoice->SetStringSelection(name);

  underLineCheckBox->SetValue(dialogFont.GetUnderlined());
  pointSizeChoice->SetSelection(dialogFont.GetPointSize()-1);

  okButton->SetDefault();

  wxSizer *topsizer, *sizer;
  topsizer = new wxBoxSizer(wxVERTICAL);

  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(familyChoice, 0, wxALIGN_CENTER | wxLEFT, 10);
  sizer->Add(styleChoice, 0, wxALIGN_CENTER | wxLEFT, 10);
  sizer->Add(weightChoice, 0, wxALIGN_CENTER | wxLEFT, 10);
  topsizer->Add(sizer, 0, wxLEFT| wxTOP| wxRIGHT, 10);

  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(colourChoice, 0, wxALIGN_CENTER | wxLEFT, 10);
  sizer->Add(pointSizeChoice, 0, wxALIGN_CENTER | wxLEFT, 10);
  sizer->Add(underLineCheckBox, 0, wxALIGN_CENTER | wxLEFT, 10);
  topsizer->Add(sizer, 0, wxLEFT| wxTOP| wxRIGHT, 10);
  
  topsizer->Add(m_previewer, 1, wxALL | wxEXPAND, 10);
  topsizer->SetItemMinSize(m_previewer, 430, 100);

  sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add(okButton, 0, wxRIGHT, 10);
  sizer->Add(cancelButton, 0, wxRIGHT, 10);
  topsizer->Add(sizer, 0, wxALIGN_RIGHT | wxBOTTOM, 10);

  SetAutoLayout(TRUE);
  SetSizer(topsizer);
  topsizer->SetSizeHints(this);
  topsizer->Fit(this);

  Centre(wxBOTH);

  delete[] families;
  delete[] styles;
  delete[] weights;
  delete[] pointSizes;
  m_useEvents = TRUE;
}

void wxGenericFontDialog::InitializeFont()
{
  int fontFamily = wxSWISS;
  int fontWeight = wxNORMAL;
  int fontStyle = wxNORMAL;
  int fontSize = 12;
  int fontUnderline = FALSE;

  if (m_fontData.initialFont.Ok())
  {
      fontFamily = m_fontData.initialFont.GetFamily();
      fontWeight = m_fontData.initialFont.GetWeight();
      fontStyle = m_fontData.initialFont.GetStyle();
      fontSize = m_fontData.initialFont.GetPointSize();
      fontUnderline = m_fontData.initialFont.GetUnderlined();
  }

  dialogFont = wxFont(fontSize, fontFamily, fontStyle, fontWeight, (fontUnderline != 0));

  if (m_previewer)
      m_previewer->SetFont(dialogFont);
}

void wxGenericFontDialog::OnChangeFont(wxCommandEvent& WXUNUSED(event))
{
  if (!m_useEvents) return;

  int fontFamily = 0;  /* shut up buggy egcs warnings */
  fontFamily = wxFontFamilyStringToInt(WXSTRINGCAST familyChoice->GetStringSelection());
  int fontWeight = 0;
  fontWeight = wxFontWeightStringToInt(WXSTRINGCAST weightChoice->GetStringSelection());
  int fontStyle = 0;
  fontStyle = wxFontStyleStringToInt(WXSTRINGCAST styleChoice->GetStringSelection());
  int fontSize = wxAtoi(pointSizeChoice->GetStringSelection());
  int fontUnderline = underLineCheckBox->GetValue();

  dialogFont = wxFont(fontSize, fontFamily, fontStyle, fontWeight, (fontUnderline != 0));
  m_previewer->SetFont(dialogFont);
  if (colourChoice->GetStringSelection() != wxT(""))
  {
    wxColour *col = (wxColour*) NULL;
    col = wxTheColourDatabase->FindColour(colourChoice->GetStringSelection());
    if (col)
    {
      m_fontData.fontColour = *col;
      m_previewer->SetForegroundColour(*col);
    }
  }
  m_previewer->Refresh();
}

const wxChar *wxFontWeightIntToString(int weight)
{
  switch (weight)
  {
    case wxLIGHT:
      return wxT("Light");
    case wxBOLD:
      return wxT("Bold");
    case wxNORMAL:
    default:
      return wxT("Normal");
  }
}

const wxChar *wxFontStyleIntToString(int style)
{
  switch (style)
  {
    case wxITALIC:
      return wxT("Italic");
    case wxSLANT:
      return wxT("Slant");
    case wxNORMAL:
    default:
      return wxT("Normal");
  }
}

const wxChar *wxFontFamilyIntToString(int family)
{
  switch (family)
  {
    case wxROMAN:
      return wxT("Roman");
    case wxDECORATIVE:
      return wxT("Decorative");
    case wxMODERN:
      return wxT("Modern");
    case wxSCRIPT:
      return wxT("Script");
    case wxTELETYPE:
      return wxT("Teletype");
    case wxSWISS:
    default:
      return wxT("Swiss");
  }
}

int wxFontFamilyStringToInt(wxChar *family)
{
  if (!family)
    return wxSWISS;

  if (wxStrcmp(family, wxT("Roman")) == 0)
    return wxROMAN;
  else if (wxStrcmp(family, wxT("Decorative")) == 0)
    return wxDECORATIVE;
  else if (wxStrcmp(family, wxT("Modern")) == 0)
    return wxMODERN;
  else if (wxStrcmp(family, wxT("Script")) == 0)
    return wxSCRIPT;
  else if (wxStrcmp(family, wxT("Teletype")) == 0)
    return wxTELETYPE;
  else return wxSWISS;
}

int wxFontStyleStringToInt(wxChar *style)
{
  if (!style)
    return wxNORMAL;
  if (wxStrcmp(style, wxT("Italic")) == 0)
    return wxITALIC;
  else if (wxStrcmp(style, wxT("Slant")) == 0)
    return wxSLANT;
  else
    return wxNORMAL;
}

int wxFontWeightStringToInt(wxChar *weight)
{
  if (!weight)
    return wxNORMAL;
  if (wxStrcmp(weight, wxT("Bold")) == 0)
    return wxBOLD;
  else if (wxStrcmp(weight, wxT("Light")) == 0)
    return wxLIGHT;
  else
    return wxNORMAL;
}

#endif
    // wxUSE_FONTDLG

