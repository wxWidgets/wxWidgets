/////////////////////////////////////////////////////////////////////////////
// Name:        msgdlgg.cpp
// Purpose:     wxGenericMessageDialog
// Author:      Julian Smart, Robert Roebling
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Markus Holzem, Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "msgdlgg.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/statbmp.h"
    #include "wx/layout.h"
    #include "wx/intl.h"
    #include "wx/icon.h"
#   include "wx/app.h"
#endif

#include <stdio.h>
#include <string.h>

#include "wx/generic/msgdlgg.h"

#if wxUSE_STATLINE
  #include "wx/statline.h"
#endif

// ----------------------------------------------------------------------------
// icons
// ----------------------------------------------------------------------------



#if !USE_SHARED_LIBRARY
BEGIN_EVENT_TABLE(wxGenericMessageDialog, wxDialog)
        EVT_BUTTON(wxID_YES, wxGenericMessageDialog::OnYes)
        EVT_BUTTON(wxID_NO, wxGenericMessageDialog::OnNo)
        EVT_BUTTON(wxID_CANCEL, wxGenericMessageDialog::OnCancel)
END_EVENT_TABLE()

IMPLEMENT_CLASS(wxGenericMessageDialog, wxDialog)
#endif

#ifdef _WXGTK__
#   include "wx/gtk/info.xpm"
#   include "wx/gtk/error.xpm"
#   include "wx/gtk/question.xpm"
#   include "wx/gtk/warning.xpm"
#else
   // MSW icons are in the ressources, for all other platforms - in XPM files
#   ifndef __WXMSW__
#      include "wx/generic/info.xpm"
#      include "wx/generic/question.xpm"
#      include "wx/generic/warning.xpm"
#      include "wx/generic/error.xpm"
#   endif // __WXMSW__
#endif
   
wxIcon
wxApp::GetStdIcon(int which) const
{
   switch(which)
   {
   case wxICON_INFORMATION:
      return wxIcon(info_xpm);
      break;
   case wxICON_HAND:
      return wxIcon(error_xpm);
      break;
   case wxICON_QUESTION:
      return wxIcon(question_xpm);
      break;
   case wxICON_EXCLAMATION:
      return wxIcon(warning_xpm);
      break;
   default:
      wxFAIL_MSG("requested non existent standard icon");
      return wxIcon(error_xpm);
      break;
   }
}

   
wxGenericMessageDialog::wxGenericMessageDialog( wxWindow *parent,
                                                const wxString& message,
                                                const wxString& caption,
                                                long style,
                                                const wxPoint& pos)
                      : wxDialog( parent, -1, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
    m_dialogStyle = style;

    wxBeginBusyCursor();

    wxLayoutConstraints *c;
    SetAutoLayout(TRUE);

    wxStaticBitmap *icon = new wxStaticBitmap(this, -1,
                                              wxTheApp->GetStdIcon(style & wxICON_MASK));
    const int iconSize = icon->GetBitmap().GetWidth();

    // split the message in lines
    // --------------------------

    wxArrayString lines;
    wxSize sizeText = SplitTextMessage(message, &lines);
    long widthTextMax = sizeText.GetWidth(),
         heightTextMax = sizeText.GetHeight();
    size_t nLineCount = lines.GetCount();

    // calculate the total dialog size
    enum
    {
        Btn_Ok,
        Btn_Yes,
        Btn_No,
        Btn_Cancel,
        Btn_Max
    };
    wxButton *buttons[Btn_Max] = { NULL, NULL, NULL, NULL };
    int nDefaultBtn = -1;

    // some checks are in order...
    wxASSERT_MSG( !(style & wxOK) || !(style & wxYES_NO),
                  "don't create dialog with both Yes/No and Ok buttons!" );

    wxASSERT_MSG( (style & wxOK ) || (style & wxYES_NO),
                  "don't create dialog with only the Cancel button!" );

    if ( style & wxYES_NO ) {
       buttons[Btn_Yes] = new wxButton(this, wxID_YES, _("Yes"));
       buttons[Btn_No] = new wxButton(this, wxID_NO, _("No"));


       if(style & wxNO_DEFAULT)
          nDefaultBtn = Btn_No;
       else
          nDefaultBtn = Btn_Yes;
    }

    if (style & wxOK) {
        buttons[Btn_Ok] = new wxButton(this, wxID_OK, _("OK"));

        if ( nDefaultBtn == -1 )
            nDefaultBtn = Btn_Ok;
    }

    if (style & wxCANCEL) {
        buttons[Btn_Cancel] = new wxButton(this, wxID_CANCEL, _("Cancel"));
    }

    // get the longest caption and also calc the number of buttons
    size_t nBtn, nButtons = 0;
    int width, widthBtnMax = 0;
    for ( nBtn = 0; nBtn < Btn_Max; nBtn++ ) {
        if ( buttons[nBtn] ) {
            nButtons++;
            GetTextExtent(buttons[nBtn]->GetLabel(), &width, NULL);
            if ( width > widthBtnMax )
                widthBtnMax = width;
        }
    }

    // now we can place the buttons
    if ( widthBtnMax < 75 )
        widthBtnMax = 75;
    else
        widthBtnMax += 10;
    long heightButton = widthBtnMax*23/75;

    // *1.2 baselineskip
    heightTextMax *= 12;
    heightTextMax /= 10;

    long widthButtonsTotal = nButtons * (widthBtnMax + LAYOUT_X_MARGIN) -
                             LAYOUT_X_MARGIN;

    // the size of the dialog
    long widthDlg = wxMax(widthTextMax + iconSize + 4*LAYOUT_X_MARGIN,
                        wxMax(widthButtonsTotal, width)) +
                    2*LAYOUT_X_MARGIN,
         heightDlg = 8*LAYOUT_Y_MARGIN + heightButton +
                     heightTextMax*(nLineCount + 1);

    // create the controls
    // -------------------

    // the icon first
    c = new wxLayoutConstraints;
    c->width.Absolute(iconSize);
    c->height.Absolute(iconSize);
    c->top.SameAs(this, wxTop, 3*LAYOUT_Y_MARGIN);
    c->left.SameAs(this, wxLeft, 2*LAYOUT_X_MARGIN);
    icon->SetConstraints(c);

    wxStaticText *text = NULL;
    for ( size_t nLine = 0; nLine < nLineCount; nLine++ ) {
        c = new wxLayoutConstraints;
        if ( text == NULL )
            c->top.SameAs(this, wxTop, 3*LAYOUT_Y_MARGIN);
        else
            c->top.Below(text);

        c->left.RightOf(icon, 2*LAYOUT_X_MARGIN);
        c->width.Absolute(widthTextMax);
        c->height.Absolute(heightTextMax);
        text = new wxStaticText(this, -1, lines[nLine]);
        text->SetConstraints(c);
    }

    // create the buttons
    wxButton *btnPrevious = (wxButton *)NULL;
    for ( nBtn = 0; nBtn < Btn_Max; nBtn++ ) {
        if ( buttons[nBtn] ) {
            c = new wxLayoutConstraints;

            if ( btnPrevious ) {
                c->left.RightOf(btnPrevious, LAYOUT_X_MARGIN);
            }
            else {
                c->left.SameAs(this, wxLeft,
                               (widthDlg - widthButtonsTotal) / 2);
            }

            c->width.Absolute(widthBtnMax);
            c->top.Below(text, 4*LAYOUT_Y_MARGIN);
            c->height.Absolute(heightButton);
            buttons[nBtn]->SetConstraints(c);

            btnPrevious = buttons[nBtn];
        }
    }

    // set default button
    // ------------------

    if ( nDefaultBtn != -1 ) {
        buttons[nDefaultBtn]->SetDefault();
        buttons[nDefaultBtn]->SetFocus();
    }
    else {
        wxFAIL_MSG( "can't find default button for this dialog." );
    }

    // position the controls and the dialog itself
    // -------------------------------------------

    SetClientSize(widthDlg, heightDlg);

    // SetSizeHints() wants the size of the whole dialog, not just client size
    wxSize sizeTotal = GetSize(),
           sizeClient = GetClientSize();
    SetSizeHints(widthDlg + sizeTotal.GetWidth() - sizeClient.GetWidth(),
                 heightDlg + sizeTotal.GetHeight() - sizeClient.GetHeight());

    Layout();

    Centre(wxCENTER_FRAME | wxBOTH);

    wxEndBusyCursor();
}

void wxGenericMessageDialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_YES );
}

void wxGenericMessageDialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_NO );
}

void wxGenericMessageDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    /* Allow cancellation via ESC/Close button except if
       only YES and NO are specified. */
    if ( (m_dialogStyle & wxYES_NO) != wxYES_NO || (m_dialogStyle & wxCANCEL) )
    {
        EndModal( wxID_CANCEL );
    }
}


