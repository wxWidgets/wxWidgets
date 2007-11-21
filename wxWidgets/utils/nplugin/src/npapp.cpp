/*
 * File:	NPApp.cc
 * Purpose:	wxPluginApp implementation
 * Author:	Julian Smart
 * Created:	1997
 * Updated:
 * Copyright:   (c) Julian Smart
 */

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "NPApp.h"
#include "NPFrame.h"

#include <windows.h>

IMPLEMENT_ABSTRACT_CLASS(wxPluginApp, wxApp)

wxPluginApp *wxGetPluginApp(void)
{
    if ( wxTheApp && wxTheApp->IsKindOf(CLASSINFO(wxPluginApp)))
        return (wxPluginApp *)wxTheApp;
    else
        return NULL;
}

wxPluginApp::wxPluginApp(void)
{
  m_data.m_argc = NULL;
  m_data.m_argn = NULL;
  m_data.m_argv = NULL;
  m_data.m_type = 0;
  m_data.m_instance = 0;
  m_data.m_mode = 0;
  m_data.m_window = 0;
}

wxPluginApp::~wxPluginApp(void)
{
    if ( m_data.m_argn )
        delete[] m_data.m_argn;
    if ( m_data.m_argv )
        delete[] m_data.m_argv;
}

// Add a frame
void wxPluginApp::AddFrame(wxPluginFrame *frame)
{
    m_frames.Append(frame);
}

// Remove a frame
void wxPluginApp::RemoveFrame(wxPluginFrame *frame)
{
    m_frames.DeleteObject(frame);
}

// Find a frame given a NP instance
wxPluginFrame *wxPluginApp::FindFrame(NPP instance)
{
    wxNode *node = m_frames.First();
    while ( node )
    {
        wxPluginFrame *frame = (wxPluginFrame *)node->Data();
        if ( frame->GetInstance() == instance )
        {
            return frame;
        }
        node = node->Next();
    }
    return NULL;
}

void wxPluginApp::SetAttributeValues(const int n, char *argn[], char *argv[])
{
    if ( m_data.m_argn )
        delete[] m_data.m_argn;
    if ( m_data.m_argv )
        delete[] m_data.m_argv;

    m_data.m_argc = n;

    m_data.m_argn = new wxString[n];
    m_data.m_argv = new wxString[n];
    int i;
    for ( i = 0; i < n ; i ++)
    {
        m_data.m_argn[i] = argn[i];
        m_data.m_argv[i] = argv[i];
    }
}

///////////////////////////////////////////////////////////////
// Netscape Plugin API calls routed via wxPluginApp

NPError   wxPluginApp::NPP_Destroy(NPP instance, NPSavedData** save)
{
    wxPluginFrame *frame = FindFrame(instance);
    if ( frame )
    {
        frame->Close();
    }
    return NPERR_NO_ERROR;
}

NPError   wxPluginApp::NPP_DestroyStream(NPP instance, NPStream* stream, NPError reason)
{
   return NPERR_NO_ERROR;
}

/*
jref      wxPluginApp::NPP_GetJavaClass(void)
{
   return 0;
}
*/

// This can't work now because we don't have a global app until wxEntry is called.
// We'll put the wxEntry call in npshell.cpp instead.
NPError   wxPluginApp::NPP_Initialize(void)
{
    static int init = FALSE;

    if ( init == TRUE )
        MessageBox(NULL, "wxPluginApp::NPP_Initialize:\nabout to call wxEntry for 2nd time!!!", "wxPlugin", MB_OK);

    wxEntry((WXHINSTANCE) GetModuleHandle(NULL));

    init = TRUE;

//    MessageBox(NULL, "wxPluginApp::NPP_Initialize: have called wxEntry", "wxPlugin", MB_OK);
    return NPERR_NO_ERROR;
}

NPError   wxPluginApp::NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode,
                        int16 argc, char* argn[], char* argv[], NPSavedData* saved)
{
//    MessageBox(NULL, "wxPluginApp::NPP_New", "wxPlugin", MB_OK);

    // Save values so frame can be created in first NPP_SetWindow
    if ( m_data.m_instance != 0 )
    {
        MessageBox(NULL, "wxPluginApp::NPP_New: whoops, 2 NPP_New calls in succession without NPP_SetWindow.\n Need to modify my code!", "wxPlugin", MB_OK);
        return NPERR_NO_ERROR;
    }

    m_data.m_instance = instance;
    m_data.m_type = pluginType;
    m_data.m_mode = mode;

    SetAttributeValues(argc, argn, argv);

    // Unfortunately, we may get a stream event before we've got a valid window
    // handle, so we just have to go ahead and create a new instance.
    wxPluginFrame *frame = OnNewInstance(m_data);

    m_data.m_instance = NULL;
    m_data.m_window = NULL;
    delete[] m_data.m_argv;
    delete[] m_data.m_argn;
    m_data.m_argv = NULL;
    m_data.m_argn = NULL;

    return NPERR_NO_ERROR;
}

NPError   wxPluginApp::NPP_NewStream(NPP instance, NPMIMEType type, NPStream *stream,
                        NPBool seekable, uint16* stype)
{
    // By default, we want to receive a file instead of a stream.
    wxPluginFrame *frame = FindFrame(instance);
    if ( frame )
    {
        return frame->OnNPNewStream(type, stream, seekable, stype);
    }
    return NPERR_NO_ERROR;
}

void wxPluginApp::NPP_Print(NPP instance, NPPrint* printInfo)
{
    if (instance == NULL)
        return;

    wxPluginFrame *frame = FindFrame(instance);
    if ( frame )
    {
        frame->OnNPPrint(printInfo);
    }
}

NPError      wxPluginApp::NPP_SetWindow(NPP instance, NPWindow* window)
{
//    MessageBox(NULL, "wxPluginApp::NPP_SetWindow", "wxPlugin", MB_OK);

    if ( window )
        wxDebugMsg("%d\n", (int) window->window);

    wxPluginFrame *frame = FindFrame(instance);
    if ( frame )
    {
        frame->SetNPWindow(window);
    }
    else
    {
#if 0
        // No such frame: must make it.
        if ( m_data.m_instance == NULL )
        {
            MessageBox(NULL, "wxPluginApp::NPP_SetWindow: whoops, no data to create window. SetWindow called in funny order?", "wxPlugin", MB_OK);
            return NPERR_NO_ERROR;
        }

        if ( window->window == NULL )
        {
            // We're receiving a NULL window before we've even received
            // a valid window. Ignore this silly thing.
            return NPERR_NO_ERROR;
        }

        m_data.m_window = window;
        m_data.m_instance = instance;

//        wxPluginFrame *frame = OnNewInstance(m_data);

        m_data.m_instance = NULL;
        m_data.m_window = NULL;
        delete[] m_data.m_argv;
        delete[] m_data.m_argn;
        m_data.m_argv = NULL;
        m_data.m_argn = NULL;
#endif
    }
    return NPERR_NO_ERROR;
}

void      wxPluginApp::NPP_Shutdown(void)
{
    // Clean up wxWidgets
    CleanUp();
}

void      wxPluginApp::NPP_StreamAsFile(NPP instance, NPStream* stream, const char *fname)
{
    wxPluginFrame *frame = FindFrame(instance);
    if ( frame )
    {
        wxString str(fname);
        frame->OnNPNewFile(stream, str);
    }
}

/*
void      wxPluginApp::NPP_URLNotify(NPP instance, const char* url, NPReason reason,
                        void* notifyData)
{
}
*/

int32     wxPluginApp::NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len,
                        void* buf)
{
    return len;              // The number of bytes accepted
}

static int32 STREAMBUFSIZE = 0X0FFFFFFF;   // If we are reading from a file in NPAsFile
                                    // mode so we can take any size stream in our
                                    // write call (since we ignore it)

int32     wxPluginApp::NPP_WriteReady(NPP instance, NPStream* stream)
{
    return STREAMBUFSIZE;   // Number of bytes ready to accept in NPP_Write()
}

