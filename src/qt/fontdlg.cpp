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

    GetQFontDialog()->setCurrentFont(m_fontData.GetInitialFont().GetHandle());

    const int flags = m_fontData.GetRestrictSelection();

    if ( flags != wxFONTRESTRICT_NONE )
    {
        GetQFontDialog()->setOption(QFontDialog::ScalableFonts,
                                    flags | wxFONTRESTRICT_SCALABLE);

        GetQFontDialog()->setOption(QFontDialog::MonospacedFonts,
                                    flags | wxFONTRESTRICT_FIXEDPITCH);
    }

    return wxFontDialogBase::DoCreate(parent);
}

void wxFontDialog::QtDontUseNativeDialog()
{
    GetQFontDialog()->setOption(QFontDialog::DontUseNativeDialog, true);
}

QFontDialog* wxFontDialog::GetQFontDialog() const
{
    return static_cast<QFontDialog*>(m_qtWindow);
}

#endif // wxUSE_FONTDLG
