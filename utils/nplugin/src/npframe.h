/*
 * File:	NPFrame.h
 * Purpose:	wxPluginFrame declaration
 * Author:	Julian Smart
 * Created:	1997
 * Updated:	
 * Copyright:   (c) Julian Smart
 */

#ifndef __PLUGINFRAME__
#define __PLUGINFRAME__

#include "wx/frame.h"
#include "NPApp.h"
#include "npapi.h"

WXDLLEXPORT extern const char *wxFrameNameStr;

class wxPrinterDC;
class WXDLLEXPORT wxPluginFrame: public wxFrame
{
  DECLARE_DYNAMIC_CLASS(wxPluginFrame)

public:
  wxPluginFrame(void);
  inline wxPluginFrame(const wxPluginData& data)
  {
      m_npWindow = NULL;
      m_npInstance = NULL;
      m_nAttributes = 0;
      m_names = NULL;
      m_values = NULL;

      Create(data);
  }

  ~wxPluginFrame(void);

  bool Create(const wxPluginData& data);

  // Sets and subclasses the platform-specific window handle
  virtual bool SetNPWindow(NPWindow *window);
  inline NPWindow *GetNPWindow(void) { return m_npWindow; }

  void SetClientSize(const int width, const int height);
  void GetClientSize(int *width, int *height) const;

  void GetSize(int *width, int *height) const ;
  void GetPosition(int *x, int *y) const ;
  void SetSize(const int x, const int y, const int width, const int height, const int sizeFlags = wxSIZE_AUTO);

  // Accessors
  inline int GetAttributeCount(void) const { return m_nAttributes; }
  inline wxString GetAttributeName(const int n) { return m_names[n]; }
  inline wxString GetAttributeValue(const int n) { return m_values[n]; }

  void SetAttributeValues(const int n, const char* argn[], const char *argv[]);
  void SetAttributeValues(const int n, const wxString* argn, const wxString* argv);
  inline void SetInstance(const NPP instance) { m_npInstance = instance; };
  inline NPP GetInstance(void) { return m_npInstance; }

  // Overridables: low-level
  virtual NPError OnNPNewStream(NPMIMEType type, NPStream *stream, bool seekable, uint16* stype);
  virtual void OnNPNewFile(NPStream *stream, const wxString& fname);
  virtual void OnNPPrint(NPPrint* printInfo);

  // Overridables: high-level
  virtual void OnPrint(wxPrinterDC& dc, wxRectangle& rect);
  virtual void OnDraw(wxDC& dc);

protected:

  wxString*     m_names;
  wxString*     m_values;
  int           m_nAttributes;
  NPP           m_npInstance;
  NPWindow*     m_npWindow;
};

#endif

