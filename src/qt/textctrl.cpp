/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/textctrl.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/textctrl.h"
#include "wx/qt/converter.h"

wxTextCtrl::wxTextCtrl()
{
}

wxTextCtrl::wxTextCtrl(wxWindow *parent,
           wxWindowID id,
           const wxString &value,
           const wxPoint &pos,
           const wxSize &size,
           long style,
           const wxValidator& validator,
           const wxString &name)
{
    Create( parent, id, value, pos, size, style, validator, name );
}

bool wxTextCtrl::Create(wxWindow *parent,
            wxWindowID id,
            const wxString &value,
            const wxPoint &pos,
            const wxSize &size,
            long style,
            const wxValidator& validator,
            const wxString &name)
{
    m_qtTextEdit = new QTextEdit( wxQtConvertString( value ), parent->GetHandle() );

    return QtCreateControl( parent, id, pos, size, style, validator, name );
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    return wxSize( 100, 50 ); // wxTextCtrlBase::DoGetBestSize();

}
int wxTextCtrl::GetLineLength(long lineNo) const
{
    return 0;
}

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    return wxString();
}

int wxTextCtrl::GetNumberOfLines() const
{
    return 0;
}

bool wxTextCtrl::IsModified() const
{
    return false;
}

void wxTextCtrl::MarkDirty()
{
}

void wxTextCtrl::DiscardEdits()
{
}

bool wxTextCtrl::SetStyle(long start, long end, const wxTextAttr& style)
{
    return false;
}

bool wxTextCtrl::GetStyle(long position, wxTextAttr& style)
{
    return false;
}

bool wxTextCtrl::SetDefaultStyle(const wxTextAttr& style)
{
    return false;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    return 0;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    return false;
}

void wxTextCtrl::ShowPosition(long pos)
{
}

bool wxTextCtrl::DoLoadFile(const wxString& file, int fileType)
{
    return false;
}

bool wxTextCtrl::DoSaveFile(const wxString& file, int fileType)
{
    return false;
}

QTextEdit *wxTextCtrl::GetHandle() const
{
    return m_qtTextEdit;
}
