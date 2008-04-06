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
class DoodleLine: public wxObject
{
public:
    wxInt32 x1;
    wxInt32 y1;
    wxInt32 x2;
    wxInt32 y2;
};

// Contains a list of lines: represents a mouse-down doodle
class DoodleSegment: public wxObject
{
public:
    wxList m_lines;

    DoodleSegment() : wxObject() {}
    DoodleSegment(const DoodleSegment&);
    virtual ~DoodleSegment();

    void Draw(wxDC* dc);

#if wxUSE_STD_IOSTREAM
    wxSTD ostream& SaveObject(wxSTD ostream&);
    wxSTD istream& LoadObject(wxSTD istream&);
#else
    wxOutputStream& SaveObject(wxOutputStream&);
    wxInputStream& LoadObject(wxInputStream&);
#endif
};

class DrawingDocument: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(DrawingDocument)
public:
    wxList m_doodleSegments;

    DrawingDocument() : wxDocument() {}
    virtual ~DrawingDocument();

#if wxUSE_STD_IOSTREAM
    wxSTD ostream& SaveObject(wxSTD ostream&);
    wxSTD istream& LoadObject(wxSTD istream&);
#else
    wxOutputStream& SaveObject(wxOutputStream&);
    wxInputStream& LoadObject(wxInputStream&);
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

    bool Do();
    bool Undo();
};

class TextEditView;
class TextEditDocument : public wxDocument
{
  DECLARE_DYNAMIC_CLASS(TextEditDocument)
public:
/*
    wxSTD ostream& SaveObject(wxSTD ostream&);
    wxSTD istream& LoadObject(wxSTD istream&);
*/
    TextEditView* GetFirstView() const;

    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool IsModified() const;
    virtual void Modify(bool mod);

    TextEditDocument() {}
    virtual ~TextEditDocument() {}
};


#endif
