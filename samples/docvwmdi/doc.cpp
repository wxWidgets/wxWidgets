/////////////////////////////////////////////////////////////////////////////
// Name:        doc.cpp
// Purpose:     Implements document functionality
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#if wxUSE_STD_IOSTREAM
    #include "wx/ioswrap.h"
#else
    #include "wx/txtstrm.h"
#endif

#include "doc.h"
#include "view.h"

IMPLEMENT_DYNAMIC_CLASS(DrawingDocument, wxDocument)

DrawingDocument::~DrawingDocument(void)
{
    WX_CLEAR_LIST(wxList, m_doodleSegments)
}

#if wxUSE_STD_IOSTREAM
wxSTD ostream& DrawingDocument::SaveObject(wxSTD ostream& stream)
{
    wxDocument::SaveObject(stream);

    wxInt32 n = m_doodleSegments.GetCount();
    stream << n << wxT('\n');

    wxList::compatibility_iterator node = m_doodleSegments.GetFirst();
    while (node)
    {
        DoodleSegment *segment = (DoodleSegment*)node->GetData();
        segment->SaveObject(stream);
        stream << wxT('\n');

        node = node->GetNext();
    }
    return stream;
}
#else
wxOutputStream& DrawingDocument::SaveObject(wxOutputStream& stream)
{
    wxDocument::SaveObject(stream);

    wxTextOutputStream text_stream( stream );

    wxInt32 n = m_doodleSegments.GetCount();
    text_stream << n << wxT('\n');

    wxList::compatibility_iterator node = m_doodleSegments.GetFirst();
    while (node)
    {
        DoodleSegment* segment = (DoodleSegment*)node->GetData();
        segment->SaveObject(stream);
        text_stream << wxT('\n');

        node = node->GetNext();
    }

    return stream;
}
#endif

#if wxUSE_STD_IOSTREAM
wxSTD istream& DrawingDocument::LoadObject(wxSTD istream& stream)
{
    wxDocument::LoadObject(stream);

    wxInt32 n = 0;
    stream >> n;

    for (int i = 0; i < n; i++)
    {
        DoodleSegment *segment = new DoodleSegment;
        segment->LoadObject(stream);
        m_doodleSegments.Append(segment);
    }

    return stream;
}
#else
wxInputStream& DrawingDocument::LoadObject(wxInputStream& stream)
{
    wxDocument::LoadObject(stream);

    wxTextInputStream text_stream( stream );

    wxInt32 n = 0;
    text_stream >> n;

    for (int i = 0; i < n; i++)
    {
        DoodleSegment* segment = new DoodleSegment;
        segment->LoadObject(stream);
        m_doodleSegments.Append(segment);
    }

    return stream;
}
#endif

DoodleSegment::DoodleSegment(const DoodleSegment& seg) : wxObject()
{
    wxList::compatibility_iterator node = seg.m_lines.GetFirst();
    while (node)
    {
        DoodleLine* line = (DoodleLine*)node->GetData();
        DoodleLine* newLine = new DoodleLine;
        newLine->x1 = line->x1;
        newLine->y1 = line->y1;
        newLine->x2 = line->x2;
        newLine->y2 = line->y2;

        m_lines.Append(newLine);

        node = node->GetNext();
    }
}

DoodleSegment::~DoodleSegment(void)
{
    WX_CLEAR_LIST(wxList, m_lines)
}

#if wxUSE_STD_IOSTREAM
wxSTD ostream& DoodleSegment::SaveObject(wxSTD ostream& stream)
{
    wxInt32 n = m_lines.GetCount();
    stream << n << wxT('\n');

    wxList::compatibility_iterator node = m_lines.GetFirst();
    while (node)
    {
        DoodleLine *line = (DoodleLine *)node->GetData();
        stream << line->x1 << wxT(" ") <<
                       line->y1 << wxT(" ") <<
               line->x2 << wxT(" ") <<
               line->y2 << wxT("\n");
        node = node->GetNext();
    }

    return stream;
}
#else
wxOutputStream &DoodleSegment::SaveObject(wxOutputStream& stream)
{
    wxTextOutputStream text_stream( stream );

    wxInt32 n = m_lines.GetCount();
    text_stream << n << wxT('\n');

    wxList::compatibility_iterator node = m_lines.GetFirst();
    while (node)
    {
        DoodleLine* line = (DoodleLine*)node->GetData();
        text_stream << line->x1 << wxT(" ") <<
                       line->y1 << wxT(" ") <<
               line->x2 << wxT(" ") <<
               line->y2 << wxT("\n");
        node = node->GetNext();
    }

    return stream;
}
#endif

#if wxUSE_STD_IOSTREAM
wxSTD istream& DoodleSegment::LoadObject(wxSTD istream& stream)
{
    wxInt32 n = 0;
    stream >> n;

    for (int i = 0; i < n; i++)
    {
        DoodleLine *line = new DoodleLine;
        stream >> line->x1 >>
                       line->y1 >>
               line->x2 >>
               line->y2;
        m_lines.Append(line);
    }

    return stream;
}
#else
wxInputStream &DoodleSegment::LoadObject(wxInputStream& stream)
{
    wxTextInputStream text_stream( stream );

    wxInt32 n = 0;
    text_stream >> n;

    for (int i = 0; i < n; i++)
    {
        DoodleLine* line = new DoodleLine;
        text_stream >> line->x1 >>
                       line->y1 >>
               line->x2 >>
               line->y2;
        m_lines.Append(line);
    }

    return stream;
}
#endif
void DoodleSegment::Draw(wxDC *dc)
{
    wxList::compatibility_iterator node = m_lines.GetFirst();
    while (node)
    {
        DoodleLine* line = (DoodleLine*)node->GetData();
        dc->DrawLine(line->x1, line->y1, line->x2, line->y2);
        node = node->GetNext();
    }
}

/*
 * Implementation of drawing command
 */

DrawingCommand::DrawingCommand(const wxString& name, int command, DrawingDocument* doc, DoodleSegment* seg) :
    wxCommand(true, name)
{
    m_doc = doc;
    m_segment = seg;
    m_cmd = command;
}

DrawingCommand::~DrawingCommand(void)
{
    if (m_segment)
        delete m_segment;
}

bool DrawingCommand::Do(void)
{
    switch (m_cmd)
    {
        case DOODLE_CUT:
            // Cut the last segment
            if (m_doc->GetDoodleSegments().GetCount() > 0)
            {
                wxList::compatibility_iterator node = m_doc->GetDoodleSegments().GetLast();
                if (m_segment)
                    delete m_segment;

                m_segment = (DoodleSegment*)node->GetData();
                m_doc->GetDoodleSegments().Erase(node);

                m_doc->Modify(true);
                m_doc->UpdateAllViews();
            }
            break;
        case DOODLE_ADD:
            m_doc->GetDoodleSegments().Append(new DoodleSegment(*m_segment));
            m_doc->Modify(true);
            m_doc->UpdateAllViews();
            break;
    }
    return true;
}

bool DrawingCommand::Undo(void)
{
    switch (m_cmd)
    {
        case DOODLE_CUT:
        {
            // Paste the segment
            if (m_segment)
            {
                m_doc->GetDoodleSegments().Append(m_segment);
                m_doc->Modify(true);
                m_doc->UpdateAllViews();
                m_segment = NULL;
            }
            m_doc->Modify(true);
            m_doc->UpdateAllViews();
            break;
        }
        case DOODLE_ADD:
        {
            // Cut the last segment
            if (m_doc->GetDoodleSegments().GetCount() > 0)
            {
                wxList::compatibility_iterator node = m_doc->GetDoodleSegments().GetLast();
                DoodleSegment* seg = (DoodleSegment*)node->GetData();
                delete seg;
                m_doc->GetDoodleSegments().Erase(node);

                m_doc->Modify(true);
                m_doc->UpdateAllViews();
            }
       }
    }
    return true;
}

IMPLEMENT_DYNAMIC_CLASS(TextEditDocument, wxDocument)

// Since text windows have their own method for saving to/loading from files,
// we override OnSave/OpenDocument instead of Save/LoadObject
bool TextEditDocument::OnSaveDocument(const wxString& filename)
{
    TextEditView* view = GetFirstView();

    if (!view->m_textsw->SaveFile(filename))
        return false;
    Modify(false);
    return true;
}

bool TextEditDocument::OnOpenDocument(const wxString& filename)
{
    TextEditView *view = GetFirstView();
    if (!view->m_textsw->LoadFile(filename))
        return false;

    SetFilename(filename, true);
    Modify(false);
    UpdateAllViews();
    return true;
}

bool TextEditDocument::IsModified(void) const
{
    TextEditView* view = GetFirstView();
    return (wxDocument::IsModified() || (view && view->m_textsw->IsModified()));
}

void TextEditDocument::Modify(bool mod)
{
    TextEditView* view = GetFirstView();

    wxDocument::Modify(mod);

    if ((!mod) && view && view->m_textsw)
    {
        view->m_textsw->DiscardEdits();
    }
}

TextEditView* TextEditDocument::GetFirstView() const
{
   wxView* view = wxDocument::GetFirstView();
   return view ? wxStaticCast(view, TextEditView) : NULL;
}

