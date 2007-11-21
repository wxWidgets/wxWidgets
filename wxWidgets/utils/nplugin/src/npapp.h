/*
 * File:	NPApp.h
 * Purpose:	wxPluginApp declaration
 * Author:	Julian Smart
 * Created:	1997
 * Updated:	
 * Copyright:   (c) Julian Smart
 */

#ifndef __PLUGINAPP__
#define __PLUGINAPP__

#include "wx/wx.h"
#include "npapi.h"

class wxPluginFrame;

// Data passed to OnNewInstance
class wxPluginData
{
public:
    NPP         m_instance;
    NPMIMEType  m_type;
    NPWindow*   m_window;
    int         m_mode;
    int         m_argc;
    wxString*   m_argn;
    wxString*   m_argv;
};

class WXDLLEXPORT wxPluginApp: public wxApp
{
  DECLARE_ABSTRACT_CLASS(wxPluginApp)

public:
  wxPluginApp(void);
  virtual ~wxPluginApp(void);

  // Find a frame given a NP instance
  wxPluginFrame *FindFrame(NPP instance);

  // Add a frame
  void AddFrame(wxPluginFrame *frame);

  // Remove a frame
  void RemoveFrame(wxPluginFrame *frame);

  // Set attribute/values for the last instance
  void SetAttributeValues(const int n, char *argn[], char *argv[]);

  ///////////////////////////////////////////////////////////////
  // Higher-level API than NP API
  virtual wxPluginFrame *OnNewInstance(const wxPluginData& data) = 0;

  ///////////////////////////////////////////////////////////////
  // Netscape Plugin API calls routed via wxPluginApp

  virtual NPError   NPP_Destroy(NPP instance, NPSavedData** save);
  virtual NPError   NPP_DestroyStream(NPP instance, NPStream* stream, NPError reason);
//  virtual jref      NPP_GetJavaClass(void);
  virtual NPError   NPP_Initialize(void);
  virtual NPError   NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode,
                        int16 argc, char* argn[], char* argv[], NPSavedData* saved);
  virtual NPError   NPP_NewStream(NPP instance, NPMIMEType type, NPStream *stream,
                        NPBool seekable, uint16* stype);
  virtual void      NPP_Print(NPP instance, NPPrint* platformPrint);
  virtual NPError   NPP_SetWindow(NPP instance, NPWindow* window);
  virtual void      NPP_Shutdown(void);
  virtual void      NPP_StreamAsFile(NPP instance, NPStream* stream, const char *fname);
/*
  virtual void      NPP_URLNotify(NPP instance, const char* url, NPReason reason,
                        void* notifyData);
*/
  virtual int32     NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len,
                        void* buf);
  virtual int32     NPP_WriteReady(NPP instance, NPStream* stream);

protected:

  // List of plugin frames
  wxList        m_frames;

  // Temporary NPP_New arguments so we can wait until NPP_SetWindow is called
  // before creating a frame
  wxPluginData  m_data;
};

wxPluginApp *wxGetPluginApp(void);

#endif

