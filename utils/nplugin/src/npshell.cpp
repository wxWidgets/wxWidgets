//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//
// npshell.cpp
//
// This file defines a "shell" plugin that plugin developers can use
// as the basis for a real plugin.  This shell just provides empty
// implementations of all functions that the plugin can implement
// that will be called by Netscape (the NPP_xxx methods defined in 
// npapi.h). 
//
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

#ifndef _NPAPI_H_
#include "npapi.h"
#endif

#include <windows.h>
#include <string.h>
#include <stdio.h>

#include "NPApp.h"

//
// Instance state information about the plugin.
//
// *Developers*: Use this struct to hold per-instance
//               information that you'll need in the
//               various functions in this file.
//
typedef struct _PluginInstance
{
    NPWindow*       fWindow;
    uint16          fMode;
    
} PluginInstance;



//------------------------------------------------------------------------------------
// NPP_Initialize:
//------------------------------------------------------------------------------------
NPError NPP_Initialize(void)
{
//    MessageBox(NULL, "NPP_Initialize", "NPTest", MB_OK);

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_Initialize();
    else
        return NPERR_NO_ERROR;
}


//------------------------------------------------------------------------------------
// NPP_Shutdown:
//------------------------------------------------------------------------------------
void NPP_Shutdown(void)
{
//    MessageBox(NULL, "NPP_Shutdown", "wxPlugin", MB_OK);

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        app->NPP_Shutdown();
}


//------------------------------------------------------------------------------------
// NPP_New:
//------------------------------------------------------------------------------------
NPError NP_LOADDS
NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16 mode,
                int16 argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved)
{
//    MessageBox(NULL, "NPP_New", "NPTest", MB_OK);

    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_New(pluginType, instance, mode, argc, argn, argv, saved);
    else
        return NPERR_NO_ERROR;
}

//------------------------------------------------------------------------------------
// NPP_Destroy:
//------------------------------------------------------------------------------------
NPError NP_LOADDS
NPP_Destroy(NPP instance, NPSavedData** save)
{
//    MessageBox(NULL, "NPP_Destroy", "NPTest", MB_OK);

    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_Destroy(instance, save);
    else
        return NPERR_NO_ERROR;
}


//------------------------------------------------------------------------------------
// NPP_SetWindow:
//------------------------------------------------------------------------------------
NPError NP_LOADDS
NPP_SetWindow(NPP instance, NPWindow* window)
{
//    MessageBox(NULL, "NPP_SetWindow", "NPTest", MB_OK);

    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_SetWindow(instance, window);
    else
        return NPERR_NO_ERROR;
}


//------------------------------------------------------------------------------------
// NPP_NewStream:
//------------------------------------------------------------------------------------
NPError NP_LOADDS
NPP_NewStream(NPP instance,
                            NPMIMEType type,
                            NPStream *stream, 
                            NPBool seekable,
                            uint16 *stype)
{
//    MessageBox(NULL, "NPP_NewStream", "NPTest", MB_OK);

    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_NewStream(instance, type, stream, seekable, stype);
    else
        return NPERR_NO_ERROR;
}



//
// *Developers*: 
// These next 2 functions are directly relevant in a plug-in which handles the
// data in a streaming manner.  If you want zero bytes because no buffer space
// is YET available, return 0.  As long as the stream has not been written
// to the plugin, Navigator will continue trying to send bytes.  If the plugin
// doesn't want them, just return some large number from NPP_WriteReady(), and
// ignore them in NPP_Write().  For a NP_ASFILE stream, they are still called
// but can safely be ignored using this strategy.
//

static int32 STREAMBUFSIZE = 0X0FFFFFFF;   // If we are reading from a file in NPAsFile
                                    // mode so we can take any size stream in our
                                    // write call (since we ignore it)

//------------------------------------------------------------------------------------
// NPP_WriteReady:
//------------------------------------------------------------------------------------
int32 NP_LOADDS
NPP_WriteReady(NPP instance, NPStream *stream)
{
    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_WriteReady(instance, stream);
    else
        return STREAMBUFSIZE;

    return STREAMBUFSIZE;   // Number of bytes ready to accept in NPP_Write()
}



//------------------------------------------------------------------------------------
// NPP_Write:
//------------------------------------------------------------------------------------
int32 NP_LOADDS
NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{
    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_Write(instance, stream, offset, len, buffer);
    else
        return len;              // The number of bytes accepted
}



//------------------------------------------------------------------------------------
// NPP_DestroyStream:
//------------------------------------------------------------------------------------
NPError NP_LOADDS
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
    if (instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        return app->NPP_DestroyStream(instance, stream, reason);
    else
        return NPERR_NO_ERROR;
}


//------------------------------------------------------------------------------------
// NPP_StreamAsFile:
//------------------------------------------------------------------------------------
void NP_LOADDS
NPP_StreamAsFile(NPP instance, NPStream *stream, const char* fname)
{
    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        app->NPP_StreamAsFile(instance, stream, fname);
}



//------------------------------------------------------------------------------------
// NPP_Print:
//------------------------------------------------------------------------------------
void NP_LOADDS
NPP_Print(NPP instance, NPPrint* printInfo)
{
    if (printInfo == NULL)   // trap invalid parm
        return;
    if ( instance == NULL )
        return;

    wxPluginApp *app = wxGetPluginApp();
    if ( app )
        app->NPP_Print(instance, printInfo);
}


//------------------------------------------------------------------------------------
// NPP_HandleEvent:
// Mac-only.
//------------------------------------------------------------------------------------
int16 NPP_HandleEvent(NPP instance, void* event)
{
    NPBool eventHandled = FALSE;
    if (instance == NULL)
        return eventHandled;
        
    PluginInstance* This = (PluginInstance*) instance->pdata;
    
    //
    // *Developers*: The "event" passed in is a Macintosh
    // EventRecord*.  The event.what field can be any of the
    // normal Mac event types, or one of the following additional
    // types defined in npapi.h: getFocusEvent, loseFocusEvent,
    // adjustCursorEvent.  The focus events inform your plugin
    // that it will become, or is no longer, the recepient of
    // key events.  If your plugin doesn't want to receive key
    // events, return false when passed at getFocusEvent.  The
    // adjustCursorEvent is passed repeatedly when the mouse is
    // over your plugin; if your plugin doesn't want to set the
    // cursor, return false.  Handle the standard Mac events as
    // normal.  The return value for all standard events is currently
    // ignored except for the key event: for key events, only return
    // true if your plugin has handled that particular key event. 
    //
    
    return eventHandled;
}

