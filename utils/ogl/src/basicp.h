/////////////////////////////////////////////////////////////////////////////
// Name:        basicp.h
// Purpose:     Private OGL classes and definitions
// Author:      Julian Smart
// Modified by:
// Created:     12/07/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _OGL_BASICP_H_
#define _OGL_BASICP_H_

#ifdef __GNUG__
#pragma interface "basicp.h"
#endif

#define CONTROL_POINT_SIZE       6

class wxShapeTextLine: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxShapeTextLine)
public:
   wxShapeTextLine(float the_x = 0.0, float the_y = 0.0, const wxString& the_line = "");
   ~wxShapeTextLine();

   inline float GetX() const { return m_x; }
   inline float GetY() const { return m_y; }

   inline void SetX(float x) { m_x = x; }
   inline void SetY(float y) { m_y = y; }

   inline void SetText(const wxString& text) { m_line = text; }
   inline wxString GetText() const { return m_line; }

protected:
   wxString     m_line;
   float        m_x;
   float        m_y;
};

class wxControlPoint: public wxRectangleShape
{
 DECLARE_DYNAMIC_CLASS(wxControlPoint)

 public:
  wxControlPoint(wxShapeCanvas *the_canvas = NULL, wxShape *object = NULL, float size = 0.0, float the_xoffset = 0.0,
    float the_yoffset = 0.0, int the_type = 0);
  ~wxControlPoint();

  void OnDraw(wxDC& dc);
  void OnErase(wxDC& dc);
  void OnDrawContents(wxDC& dc);
  void OnDragLeft(bool draw, float x, float y, int keys=0, int attachment = 0);
  void OnBeginDragLeft(float x, float y, int keys=0, int attachment = 0);
  void OnEndDragLeft(float x, float y, int keys=0, int attachment = 0);

  bool GetAttachmentPosition(int attachment, float *x, float *y,
                                     int nth = 0, int no_arcs = 1, wxLineShape *line = NULL);
  int GetNumberOfAttachments();

  inline void SetEraseObject(bool er) { m_eraseObject = er; }

public:
  int           m_type;
  float         m_xoffset;
  float         m_yoffset;
  wxShape*      m_shape;
  wxCursor*     m_oldCursor;
  bool          m_eraseObject; // If TRUE, erases object before dragging handle.

};

class wxPolygonControlPoint: public wxControlPoint
{
 DECLARE_DYNAMIC_CLASS(wxPolygonControlPoint)

 public:
  wxPolygonControlPoint(wxShapeCanvas *the_canvas = NULL, wxShape *object = NULL, float size = 0.0, wxRealPoint *vertex = NULL,
    float the_xoffset = 0.0, float the_yoffset = 0.0);
  ~wxPolygonControlPoint();

  void OnDragLeft(bool draw, float x, float y, int keys=0, int attachment = 0);
  void OnBeginDragLeft(float x, float y, int keys=0, int attachment = 0);
  void OnEndDragLeft(float x, float y, int keys=0, int attachment = 0);

public:
  wxRealPoint*      m_polygonVertex;
  wxRealPoint       m_originalSize;
  float             m_originalDistance;
};

/*
 * Object regions.
 * Every shape has one or more text regions with various
 * properties. Not all of a region's properties will be used
 * by a shape.
 *
 */

class wxShapeRegion: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxShapeRegion)

 public:
  // Constructor
  wxShapeRegion();
  // Copy constructor
  wxShapeRegion(wxShapeRegion& region);
  // Destructor
  ~wxShapeRegion();

  // Accessors
  inline void SetText(const wxString& s) { m_regionText = s; }
  void SetFont(wxFont *f);
  void SetMinSize(float w, float h);
  void SetSize(float w, float h);
  void SetPosition(float x, float y);
  void SetProportions(float x, float y);
  void SetFormatMode(int mode);
  inline void SetName(const wxString& s) { m_regionName = s; };
  void SetColour(const wxString& col); // Text colour

  inline wxString GetText() const { return m_regionText; }
  inline wxFont *GetFont() const { return m_font; }
  inline void GetMinSize(float *x, float *y) const { *x = m_minWidth; *y = m_minHeight; }
  inline void GetProportion(float *x, float *y) const { *x = m_regionProportionX; *y = m_regionProportionY; }
  inline void GetSize(float *x, float *y) const { *x = m_width; *y = m_height; }
  inline void GetPosition(float *xp, float *yp) const { *xp = m_x; *yp = m_y; }
  inline int GetFormatMode() const { return m_formatMode; }
  inline wxString GetName() const { return m_regionName; }
  inline wxString GetColour() const { return m_textColour; }
  wxColour *GetActualColourObject();
  inline wxList& GetFormattedText() { return m_formattedText; }
  inline wxString GetPenColour() const { return m_penColour; }
  inline int GetPenStyle() const { return m_penStyle; }
  inline void SetPenStyle(int style) { m_penStyle = style; m_actualPenObject = NULL; }
  void SetPenColour(const wxString& col);
  wxPen *GetActualPen();
  inline float GetWidth() const { return m_width; }
  inline float GetHeight() const { return m_height; }

  void ClearText();

public:
  wxString              m_regionText;
  wxList                m_formattedText;   // List of wxShapeTextLines
  wxFont*               m_font;
  float                 m_minHeight;        // If zero, hide region.
  float                 m_minWidth;        // If zero, hide region.
  float                 m_width;
  float                 m_height;
  float                 m_x;
  float                 m_y;

  float                 m_regionProportionX; // Proportion of total object size;
                                             // -1.0 indicates equal proportion
  float                 m_regionProportionY; // Proportion of total object size;
                                             // -1.0 indicates equal proportion

  int                   m_formatMode;        // FORMAT_CENTRE_HORIZ | FORMAT_CENTRE_VERT | FORMAT_NONE
  wxString              m_regionName;
  wxString              m_textColour;
  wxColour*             m_actualColourObject; // For speed purposes

  // New members for specifying divided rectangle division colour/style 30/6/94
  wxString              m_penColour;
  int                   m_penStyle;
  wxPen*                m_actualPenObject;

};

/*
 * User-defined attachment point
 */

class wxAttachmentPoint: public wxObject
{
 DECLARE_DYNAMIC_CLASS(wxAttachmentPoint)

public:
  inline wxAttachmentPoint()
  {
    m_id = 0; m_x = 0.0; m_y = 0.0;
  }

public:
  int           m_id;           // Identifier
  float         m_x;            // x offset from centre of object
  float         m_y;            // y offset from centre of object
};

#endif
  // _OGL_BASICP_H_
