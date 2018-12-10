/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/fontenum.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"
#include "wx/fontenum.h"
#include <QFontDatabase>

bool wxFontEnumerator::EnumerateFacenames( wxFontEncoding WXUNUSED(encoding), bool fixedWidthOnly)
{
    QFontDatabase fontDatabase;
    const QStringList allFonts = fontDatabase.families(QFontDatabase::Any);
    for( QStringList::const_iterator i = allFonts.begin(); i != allFonts.end(); ++i)
    {
        const QString &fontFamily = *i;
        if ( !fixedWidthOnly || fontDatabase.isFixedPitch(fontFamily) )
        {
            this->OnFacename(fontFamily.toStdString());
        }
    }

    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& WXUNUSED(facename))
{
    return false;
}

#ifdef wxHAS_UTF8_FONTS
bool wxFontEnumerator::EnumerateEncodingsUTF8(const wxString& facename)
{
    return false;
}
#endif
