/////////////////////////////////////////////////////////////////////////////
// Name:        doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __DOC_H__
#define __DOC_H__

#include "wx/docview.h"
#include "wx/cmdproc.h"

// Plots a line from one point to the other
class DoodleLine : public wxObject
{
public:
    wxInt32 x1;
    wxInt32 y1;
    wxInt32 x2;
    wxInt32 y2;
};

// Contains a list of lines: represents a mouse-down doodle
class DoodleSegment : public wxObject
{
public:
    wxList m_lines;

    DoodleSegment() : wxObject() {}
    DoodleSegment(const DoodleSegment& seg);
    virtual ~DoodleSegment();

    void Draw(wxDC *dc);
#if wxUSE_STD_IOSTREAM
    wxSTD ostream& SaveObject(wxSTD ostream& text_stream);
    wxSTD istream& LoadObject(wxSTD istream& text_stream);
#else
    wxOutputStream& SaveObject(wxOutputStream& stream);
    wxInputStream& LoadObject(wxInputStream& stream);
#endif

};

class DrawingDocument : public wxDocument
{
    DECLARE_DYNAMIC_CLASS(DrawingDocument)
private:
public:
    wxList m_doodleSegments;

    DrawingDocument() : wxDocument() {}
    virtual ~DrawingDocument();

#if wxUSE_STD_IOSTREAM
    wxSTD ostream& SaveObject(wxSTD ostream& text_stream);
    wxSTD istream& LoadObject(wxSTD istream& text_stream);
#else
    wxOutputStream& SaveObject(wxOutputStream& stream);
    wxInputStream& LoadObject(wxInputStream& stream);
#endif

    inline wxList& GetDoodleSegments() const { return (wxList&) m_doodleSegments; };
};

#define DOODLE_CUT          1
#define DOODLE_ADD          2

class DrawingCommand : public wxCommand
{
protected:
    DoodleSegment*   m_segment;
    DrawingDocument* m_doc;
    int m_cmd;
public:
    DrawingCommand(const wxString& name, int cmd, DrawingDocument*, DoodleSegment*);
    virtual ~DrawingCommand();

    bool Do(void);
    bool Undo(void);
};

class TextEditView;
class TextEditDocument : public wxDocument
{
    DECLARE_DYNAMIC_CLASS(TextEditDocument)
public:
    TextEditDocument() : wxDocument() {}
    virtual ~TextEditDocument() {}
/*
    wxSTD ostream& SaveObject(wxSTD ostream&);
    wxSTD istream& LoadObject(wxSTD istream&);
*/
    TextEditView* GetFirstView() const;

    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool IsModified(void) const;
    virtual void Modify(bool mod);
};


#endif
