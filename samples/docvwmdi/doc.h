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

#ifndef __DOCSAMPLEH__
#define __DOCSAMPLEH__

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
  wxList lines;

  DoodleSegment(void){};
  DoodleSegment(const DoodleSegment& seg);
  ~DoodleSegment(void);

  void Draw(wxDC *dc);

#if wxUSE_STD_IOSTREAM
  wxSTD ostream& SaveObject(wxSTD ostream& text_stream);
  wxSTD istream& LoadObject(wxSTD istream& text_stream);
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

  DrawingDocument(void){};
  ~DrawingDocument(void);

#if wxUSE_STD_IOSTREAM
  wxSTD ostream& SaveObject(wxSTD ostream& text_stream);
  wxSTD istream& LoadObject(wxSTD istream& text_stream);
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
  wxSTD ostream& SaveObject(wxSTD ostream& stream);
  wxSTD istream& LoadObject(wxSTD istream& stream);
*/
  virtual bool IsModified(void) const;
  virtual void Modify(bool mod);

  TextEditDocument(void) {}
  virtual ~TextEditDocument(void) {}
protected:
  virtual bool DoOpenDocument(const wxString& filename);
  virtual bool DoSaveDocument(const wxString& filename);
};


#endif
