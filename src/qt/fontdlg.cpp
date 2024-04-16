/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/filedlg.cpp
// Author:      Sean D'Epagnier
// Copyright:   (c) 2014 Sean D'Epagnier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTDLG

#include "wx/qt/private/winevent.h"
#include "wx/fontdlg.h"

#include <QtWidgets/QFontDialog>

class wxQtFontDialog : public wxQtEventSignalHandler< QFontDialog, wxFontDialog >
{
public:
    wxQtFontDialog( wxWindow *parent, wxFontDialog *handler)
        : wxQtEventSignalHandler<QFontDialog,wxFontDialog>(parent, handler)
        {
            connect(this, &QFontDialog::currentFontChanged, this, &wxQtFontDialog::updateFont);
        }

    void updateFont(const QFont &font)
        {
            GetHandler()->GetFontData().SetChosenFont(wxFont(font));
        }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog);

bool wxFontDialog::DoCreate(wxWindow *parent)
{
    m_qtWindow = new wxQtFontDialog( parent, this );
    static_cast<QFontDialog*>(m_qtWindow)->setCurrentFont(m_fontData.GetInitialFont().GetHandle());
    return wxFontDialogBase::DoCreate(parent);
}

#endif // wxUSE_FONTDLG
