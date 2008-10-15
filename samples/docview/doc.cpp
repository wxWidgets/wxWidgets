/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/doc.cpp
// Purpose:     Implements document functionality
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif

#include "doc.h"
#include "view.h"

// ----------------------------------------------------------------------------
// DrawingDocument implementation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(DrawingDocument, wxDocument)

DocumentOstream& DrawingDocument::SaveObject(DocumentOstream& ostream)
{
#if wxUSE_STD_IOSTREAM
    DocumentOstream& stream = ostream;
#else
    wxTextOutputStream stream(ostream);
#endif

    wxDocument::SaveObject(ostream);

    const wxInt32 count = m_doodleSegments.size();
    stream << count << '\n';

    for ( int n = 0; n < count; n++ )
    {
        m_doodleSegments[n].SaveObject(ostream);
        stream << '\n';
    }

    return ostream;
}

DocumentIstream& DrawingDocument::LoadObject(DocumentIstream& istream)
{
#if wxUSE_STD_IOSTREAM
    DocumentIstream& stream = istream;
#else
    wxTextInputStream stream(istream);
#endif

    wxDocument::LoadObject(istream);

    wxInt32 count = 0;
    stream >> count;

    for ( int n = 0; n < count; n++ )
    {
        DoodleSegment segment;
        segment.LoadObject(istream);
        m_doodleSegments.push_back(segment);
    }

    return istream;
}

void DrawingDocument::DoUpdate()
{
    Modify(true);
    UpdateAllViews();
}

void DrawingDocument::AddDoodleSegment(const DoodleSegment& segment)
{
    m_doodleSegments.push_back(segment);

    DoUpdate();
}

bool DrawingDocument::PopLastSegment(DoodleSegment *segment)
{
    if ( m_doodleSegments.empty() )
        return false;

    if ( segment )
        *segment = m_doodleSegments.back();

    m_doodleSegments.pop_back();

    DoUpdate();

    return true;
}

// ----------------------------------------------------------------------------
// DoodleSegment implementation
// ----------------------------------------------------------------------------

DocumentOstream& DoodleSegment::SaveObject(DocumentOstream& ostream)
{
#if wxUSE_STD_IOSTREAM
    DocumentOstream& stream = ostream;
#else
    wxTextOutputStream stream(ostream);
#endif

    const wxInt32 count = m_lines.size();
    stream << count << '\n';

    for ( int n = 0; n < count; n++ )
    {
        const DoodleLine& line = m_lines[n];
        stream
            << line.x1 << ' '
            << line.y1 << ' '
            << line.x2 << ' '
            << line.y2 << '\n';
    }

    return ostream;
}

DocumentIstream& DoodleSegment::LoadObject(DocumentIstream& istream)
{
#if wxUSE_STD_IOSTREAM
    DocumentIstream& stream = istream;
#else
    wxTextInputStream stream(istream);
#endif

    wxInt32 count = 0;
    stream >> count;

    for ( int n = 0; n < count; n++ )
    {
        DoodleLine line;
        stream
            >> line.x1
            >> line.y1
            >> line.x2
            >> line.y2;
        m_lines.push_back(line);
    }

    return istream;
}

// ----------------------------------------------------------------------------
// TextEditDocument implementation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(TextEditDocument, wxDocument)

// Since text windows have their own method for saving to/loading from files,
// we override DoSave/OpenDocument instead of Save/LoadObject
bool TextEditDocument::DoSaveDocument(const wxString& filename)
{
    return GetFirstView()->GetText()->SaveFile(filename);
}

bool TextEditDocument::DoOpenDocument(const wxString& filename)
{
    return GetFirstView()->GetText()->LoadFile(filename);
}

bool TextEditDocument::IsModified() const
{
    TextEditView* view = GetFirstView();
    return wxDocument::IsModified() || (view && view->GetText()->IsModified());
}

void TextEditDocument::Modify(bool modified)
{
    TextEditView* view = GetFirstView();

    wxDocument::Modify(modified);

    if ( !modified && view && view->GetText() )
        view->GetText()->DiscardEdits();
}

TextEditView* TextEditDocument::GetFirstView() const
{
    wxView* view = wxDocument::GetFirstView();
    return view ? wxStaticCast(view, TextEditView) : NULL;
}

