/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/doc.cpp
// Purpose:     Implements document functionality
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if wxUSE_STD_IOSTREAM
    #include <iostream>
#else
    #include "wx/txtstrm.h"
#endif
#include "wx/wfstream.h"

#include "doc.h"
#include "view.h"

// ----------------------------------------------------------------------------
// DrawingDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(DrawingDocument, wxDocument);

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
    if ( count < 0 )
    {
        wxLogWarning("Drawing document corrupted: invalid segments count.");
#if wxUSE_STD_IOSTREAM
        istream.clear(std::ios::badbit);
#else
        istream.Reset(wxSTREAM_READ_ERROR);
#endif
        return istream;
    }

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
// TextEditDocument: wxDocument and wxTextCtrl married
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(TextEditDocument, wxDocument);

bool TextEditDocument::OnCreate(const wxString& path, long flags)
{
    if ( !wxDocument::OnCreate(path, flags) )
        return false;

    // subscribe to changes in the text control to update the document state
    // when it's modified
    GetTextCtrl()->Bind(wxEVT_TEXT, &TextEditDocument::OnTextChange, this);

    return true;
}

// Since text windows have their own method for saving to/loading from files,
// we override DoSave/OpenDocument instead of Save/LoadObject
bool TextEditDocument::DoSaveDocument(const wxString& filename)
{
    return GetTextCtrl()->SaveFile(filename);
}

bool TextEditDocument::DoOpenDocument(const wxString& filename)
{
    if ( !GetTextCtrl()->LoadFile(filename) )
        return false;

    // we're not modified by the user yet
    Modify(false);

    return true;
}

bool TextEditDocument::IsModified() const
{
    wxTextCtrl* wnd = GetTextCtrl();
    return wxDocument::IsModified() || (wnd && wnd->IsModified());
}

void TextEditDocument::Modify(bool modified)
{
    wxDocument::Modify(modified);

    wxTextCtrl* wnd = GetTextCtrl();
    if (wnd && !modified)
    {
        wnd->DiscardEdits();
    }
}

void TextEditDocument::OnTextChange(wxCommandEvent& event)
{
    Modify(true);

    event.Skip();
}

wxTextCtrl* TextEditDocument::GetTextCtrl() const
{
    wxView* view = GetFirstView();
    return view ? wxStaticCast(view, TextEditView)->GetText() : nullptr;
}

// ----------------------------------------------------------------------------
// ImageDocument and ImageDetailsDocument implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(ImageDocument, wxDocument);

bool ImageDocument::DoOpenDocument(const wxString& file)
{
    return m_image.LoadFile(file);
}

bool ImageDocument::OnOpenDocument(const wxString& filename)
{
    if ( !wxDocument::OnOpenDocument(filename) )
        return false;

    // we don't have a wxDocTemplate for the image details document as it's
    // never created by wxWidgets automatically, instead just do it manually
    ImageDetailsDocument * const docDetails = new ImageDetailsDocument(this);
    docDetails->SetFilename(filename);

    new ImageDetailsView(docDetails);

    return true;
}

ImageDetailsDocument::ImageDetailsDocument(ImageDocument *parent)
    : wxDocument(parent)
{
    const wxImage image = parent->GetImage();

    m_size.x = image.GetWidth();
    m_size.y = image.GetHeight();
    m_numColours = image.CountColours();
    m_type = image.GetType();
    m_hasAlpha = image.HasAlpha();
}
