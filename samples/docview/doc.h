/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/doc.h
// Purpose:     Document classes
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_SAMPLES_DOCVIEW_DOC_H_
#define _WX_SAMPLES_DOCVIEW_DOC_H_

#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/vector.h"
#include "wx/image.h"

// This sample is written to build both with wxUSE_STD_IOSTREAM==0 and 1, which
// somewhat complicates its code but is necessary in order to support building
// it under all platforms and in all build configurations
//
// In your own code you would normally use one set of the stream classes only
// and so wouldn't need these typedefs and preprocessor conditions.
#if wxUSE_STD_IOSTREAM
    typedef std::istream DocumentIstream;
    typedef std::ostream DocumentOstream;
#else // !wxUSE_STD_IOSTREAM
    typedef wxInputStream DocumentIstream;
    typedef wxOutputStream DocumentOstream;
#endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM

// ----------------------------------------------------------------------------
// The document class and its helpers
// ----------------------------------------------------------------------------

// Represents a line from one point to the other
struct DoodleLine
{
    DoodleLine() { /* leave fields uninitialized */ }

    DoodleLine(const wxPoint& pt1, const wxPoint& pt2)
        : x1(pt1.x), y1(pt1.y), x2(pt2.x), y2(pt2.y)
    {
    }

    wxInt32 x1;
    wxInt32 y1;
    wxInt32 x2;
    wxInt32 y2;
};

typedef wxVector<DoodleLine> DoodleLines;

// Contains a list of lines: represents a mouse-down doodle
class DoodleSegment
{
public:
    DocumentOstream& SaveObject(DocumentOstream& stream);
    DocumentIstream& LoadObject(DocumentIstream& stream);

    bool IsEmpty() const { return m_lines.empty(); }
    void AddLine(const wxPoint& pt1, const wxPoint& pt2)
    {
        m_lines.push_back(DoodleLine(pt1, pt2));
    }
    const DoodleLines& GetLines() const { return m_lines; }

private:
    DoodleLines m_lines;
};

typedef wxVector<DoodleSegment> DoodleSegments;


// The drawing document (model) class itself
class DrawingDocument : public wxDocument
{
public:
    DrawingDocument() : wxDocument() { }

    DocumentOstream& SaveObject(DocumentOstream& stream) override;
    DocumentIstream& LoadObject(DocumentIstream& stream) override;

    // add a new segment to the document
    void AddDoodleSegment(const DoodleSegment& segment);

    // remove the last segment, if any, and copy it in the provided pointer if
    // not null and return true or return false and do nothing if there are no
    // segments
    bool PopLastSegment(DoodleSegment *segment);

    // get direct access to our segments (for DrawingView)
    const DoodleSegments& GetSegments() const { return m_doodleSegments; }

private:
    void DoUpdate();

    DoodleSegments m_doodleSegments;

    wxDECLARE_DYNAMIC_CLASS(DrawingDocument);
};


// ----------------------------------------------------------------------------
// Some operations (which can be done and undone by the view) on the document:
// ----------------------------------------------------------------------------

// Base class for all operations on DrawingDocument
class DrawingCommand : public wxCommand
{
public:
    DrawingCommand(DrawingDocument *doc,
                   const wxString& name,
                   const DoodleSegment& segment = DoodleSegment())
        : wxCommand(true, name),
          m_doc(doc),
          m_segment(segment)
    {
    }

protected:
    bool DoAdd() { m_doc->AddDoodleSegment(m_segment); return true; }
    bool DoRemove() { return m_doc->PopLastSegment(&m_segment); }

private:
    DrawingDocument * const m_doc;
    DoodleSegment m_segment;
};

// The command for adding a new segment
class DrawingAddSegmentCommand : public DrawingCommand
{
public:
    DrawingAddSegmentCommand(DrawingDocument *doc, const DoodleSegment& segment)
        : DrawingCommand(doc, "Add new segment", segment)
    {
    }

    virtual bool Do() override { return DoAdd(); }
    virtual bool Undo() override { return DoRemove(); }
};

// The command for removing the last segment
class DrawingRemoveSegmentCommand : public DrawingCommand
{
public:
    DrawingRemoveSegmentCommand(DrawingDocument *doc)
        : DrawingCommand(doc, "Remove last segment")
    {
    }

    virtual bool Do() override { return DoRemove(); }
    virtual bool Undo() override { return DoAdd(); }
};


// ----------------------------------------------------------------------------
// wxTextDocument: wxDocument and wxTextCtrl married
// ----------------------------------------------------------------------------

class wxTextDocument : public wxDocument
{
public:
    wxTextDocument() : wxDocument() { }
    wxTextDocument(const wxTextDocument&) = delete;
    wxTextDocument &operator=(const wxTextDocument&) = delete;

    virtual bool OnCreate(const wxString& path, long flags) override;

    virtual wxTextCtrl* GetTextCtrl() const = 0;

    virtual bool IsModified() const override;
    virtual void Modify(bool mod) override;

protected:
    virtual bool DoSaveDocument(const wxString& filename) override;
    virtual bool DoOpenDocument(const wxString& filename) override;

    void OnTextChange(wxCommandEvent& event);

    wxDECLARE_ABSTRACT_CLASS(wxTextDocument);
};

// ----------------------------------------------------------------------------
// A very simple text document class
// ----------------------------------------------------------------------------

class TextEditDocument : public wxTextDocument
{
public:
    TextEditDocument() : wxTextDocument() { }
    TextEditDocument(const TextEditDocument&) = delete;
    TextEditDocument &operator=(const TextEditDocument&) = delete;

    virtual wxTextCtrl* GetTextCtrl() const override;

    wxDECLARE_DYNAMIC_CLASS(TextEditDocument);
};

// ----------------------------------------------------------------------------
// Image and image details document classes (both are read-only for simplicity)
// ----------------------------------------------------------------------------

// This is a normal document containing an image, just like TextEditDocument
// above contains some text. It can be created from an image file on disk as
// usual.
class ImageDocument : public wxDocument
{
public:
    ImageDocument() : wxDocument() { }
    ImageDocument(const ImageDocument&) = delete;
    ImageDocument &operator=(const ImageDocument&) = delete;

    virtual bool OnOpenDocument(const wxString& file) override;

    wxImage GetImage() const { return m_image; }

protected:
    virtual bool DoOpenDocument(const wxString& file) override;

private:
    wxImage m_image;

    wxDECLARE_DYNAMIC_CLASS(ImageDocument);
};

// This is a child document of ImageDocument: this document doesn't
// correspond to any file on disk, it's part of ImageDocument and can't be
// instantiated independently of it.
class ImageDetailsDocument : public wxDocument
{
public:
    ImageDetailsDocument(ImageDocument *parent);
    ImageDetailsDocument(const ImageDetailsDocument&) = delete;
    ImageDetailsDocument &operator=(const ImageDetailsDocument&) = delete;

    // accessors for ImageDetailsView
    wxSize GetSize() const { return m_size; }
    unsigned long GetNumColours() const { return m_numColours; }
    wxBitmapType GetType() const { return m_type; }
    bool HasAlpha() const { return m_hasAlpha; }

private:
    // some information about the image we choose to show to the user
    wxSize m_size;
    unsigned long m_numColours;
    wxBitmapType m_type;
    bool m_hasAlpha;
};

#endif // _WX_SAMPLES_DOCVIEW_DOC_H_
