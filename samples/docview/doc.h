/////////////////////////////////////////////////////////////////////////////
// Name:        doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef __DOCSAMPLEH__
#define __DOCSAMPLEH__

#include "wx/docview.h"

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
    wxList lines;
    
    DoodleSegment(void);
    DoodleSegment(DoodleSegment& seg);
    ~DoodleSegment(void);
    
    void Draw(wxDC *dc);
#if wxUSE_STD_IOSTREAM
    ostream& SaveObject(ostream& text_stream);
    istream& LoadObject(istream& text_stream);
#else
    wxOutputStream& SaveObject(wxOutputStream& stream);
    wxInputStream& LoadObject(wxInputStream& stream);
#endif
    
};

class DrawingDocument: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(DrawingDocument)
private:
public:
    wxList doodleSegments;
    
    DrawingDocument(void);
    ~DrawingDocument(void);
    
#if wxUSE_STD_IOSTREAM
    ostream& SaveObject(ostream& text_stream);
    istream& LoadObject(istream& text_stream);
#else
    wxOutputStream& SaveObject(wxOutputStream& stream);
    wxInputStream& LoadObject(wxInputStream& stream);
#endif
    
    inline wxList& GetDoodleSegments(void) const { return (wxList&) doodleSegments; };
};

#define DOODLE_CUT          1
#define DOODLE_ADD          2

class DrawingCommand: public wxCommand
{
protected:
    DoodleSegment *segment;
    DrawingDocument *doc;
    int cmd;
public:
    DrawingCommand(const wxString& name, int cmd, DrawingDocument *ddoc, DoodleSegment *seg);
    ~DrawingCommand(void);
    
    bool Do(void);
    bool Undo(void);
};

class TextEditDocument: public wxDocument
{
    DECLARE_DYNAMIC_CLASS(TextEditDocument)
private:
public:
/*
ostream& SaveObject(ostream& stream);
istream& LoadObject(istream& stream);
    */
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnOpenDocument(const wxString& filename);
    virtual bool IsModified(void) const;
    virtual void Modify(bool mod);
    
    TextEditDocument(void) {}
    ~TextEditDocument(void) {}
};


#endif
