/*
 *  npapi.h $Revision$
 *  Netscape client plug-in API spec
 */

#ifndef _NPAPI_H_
#define _NPAPI_H_


/* XXX this needs to get out of here */
#if defined(__MWERKS__)
#ifndef XP_MAC
#define XP_MAC
#endif
#endif


/*
 *  Version constants
 */

#define NP_VERSION_MAJOR 0
#define NP_VERSION_MINOR 6



/* 
 *  Basic types
 */
 
#ifndef _UINT16
typedef unsigned short uint16;
#endif
#ifndef _UINT32
typedef unsigned long uint32;
#endif
#ifndef _INT16
typedef short int16;
#endif
#ifndef _INT32
typedef long int32;
#endif

#ifndef FALSE
#define FALSE (0)
#endif
#ifndef TRUE
#define TRUE (1)
#endif
#ifndef NULL
#define NULL (0L)
#endif

typedef unsigned char   NPBool;
typedef void*           NPEvent;
typedef int16           NPError;
typedef char*           NPMIMEType;


/*
 *  NPP is a plug-in's opaque instance handle
 */
typedef struct _NPP
{
    void*   pdata;                /* plug-in private data */
    void*   ndata;                /* netscape private data */
} NPP_t;

typedef NPP_t*  NPP;



typedef struct _NPStream
{
    void*       pdata;                /* plug-in private data */
    void*       ndata;                /* netscape private data */
    const char* url;
    uint32      end;
    uint32      lastmodified;
} NPStream;

typedef struct _NPByteRange
{
    int32                   offset;                /* negative offset means from the end */
    uint32                  length;
    struct _NPByteRange*    next;
} NPByteRange;


typedef struct _NPSavedData
{
    int32   len;
    void*   buf;
} NPSavedData;



typedef struct _NPRect
{
    uint16  top;
    uint16  left;
    uint16  bottom;
    uint16  right;
} NPRect;

typedef struct _NPWindow 
{
    void*   window;                 /* platform specific window handle */
    uint32  x;                      /* position of top left corner relative to a netscape page */
    uint32  y;                   
    uint32  width;                  /* maximum window size */
    uint32  height;
    NPRect  clipRect;               /* clipping rectangle in port coordinates */
} NPWindow;



typedef struct _NPFullPrint
{
    NPBool      pluginPrinted;      /* Set TRUE if plugin handled fullscreen printing */
    NPBool      printOne;           /* TRUE if plugin should print one copy to default printer */
    void*       platformPrint;      /* Platform-specific printing info */
} NPFullPrint;

typedef struct _NPEmbedPrint
{
    NPWindow    window;
    void*       platformPrint;      /* Platform-specific printing info */
} NPEmbedPrint;

typedef struct _NPPrint
{
    uint16              mode;       /* NP_FULL or NP_EMBED */
    union
    {
        NPFullPrint     fullPrint;  /* if mode is NP_FULL */
        NPEmbedPrint    embedPrint; /* if mode is NP_EMBED */
    } print;
} NPPrint;




#ifdef XP_MAC

/*
 *  Mac-specific structures and definitions.
 */
 
#include <Quickdraw.h>
#include <Events.h>

typedef struct NP_Port
{
    CGrafPtr    port;           /* Grafport */
    int32       portx;          /* position inside the topmost window */
    int32       porty;
} NP_Port;

/*
 *  Non-standard event types that can be passed to HandleEvent
 */
#define getFocusEvent       (osEvt + 16)
#define loseFocusEvent      (osEvt + 17)
#define adjustCursorEvent   (osEvt + 18)

#endif /* XP_MAC */




#define NP_EMBED        1
#define NP_FULL         2
#define NP_BACKGROUND   3

#define NP_NORMAL       1
#define NP_SEEK         2
#define NP_ASFILE       3

#define NP_MAXREADY (((unsigned)(~0)<<1)>>1)


/*
 *  Error and reason code definitions.
 */

#define NP_NOERR  0
#define NP_EINVAL 1
#define NP_EABORT 2

#define NPERR_BASE                      0
#define NPERR_NO_ERROR                  (NPERR_BASE + 0)
#define NPERR_GENERIC_ERROR             (NPERR_BASE + 1)
#define NPERR_INVALID_INSTANCE_ERROR    (NPERR_BASE + 2)
#define NPERR_INVALID_FUNCTABLE_ERROR   (NPERR_BASE + 3)
#define NPERR_MODULE_LOAD_FAILED_ERROR  (NPERR_BASE + 4)
#define NPERR_OUT_OF_MEMORY_ERROR       (NPERR_BASE + 5)
#define NPERR_INVALID_PLUGIN_ERROR      (NPERR_BASE + 6)
#define NPERR_INVALID_PLUGIN_DIR_ERROR  (NPERR_BASE + 7)
#define NPERR_INCOMPATIBLE_VERSION_ERROR (NPERR_BASE + 8)

#define NPRES_BASE                      0
#define NPRES_NETWORK_ERR               (NPRES_BASE + 0)
#define NPRES_USER_BREAK                (NPRES_BASE + 1)
#define NPRES_DONE                      (NPRES_BASE + 3)



/*
 *  Function prototypes.
 *  Functions beginning with 'NPP' are functions provided by the plugin that Netscape will call.
 *  Functions beginning with 'NPN' are functions provided by Netscape that the plugin will call.
 */

#if defined(_WINDOWS) && !defined(__WIN32__)
#define NP_LOADDS  _loadds
#else
#define NP_LOADDS
#endif

#ifdef __cplusplus
extern "C" {
#endif

NPError               NPP_Initialize(void);
void                  NPP_Shutdown(void);
NPError     NP_LOADDS NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, char* argn[], char* argv[], NPSavedData* saved);
NPError     NP_LOADDS NPP_Destroy(NPP instance, NPSavedData** save);
NPError     NP_LOADDS NPP_SetWindow(NPP instance, NPWindow* window);
NPError     NP_LOADDS NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype);
NPError     NP_LOADDS NPP_DestroyStream(NPP instance, NPStream* stream, NPError reason);
int32       NP_LOADDS NPP_WriteReady(NPP instance, NPStream* stream);
int32       NP_LOADDS NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer);
void        NP_LOADDS NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname);
void        NP_LOADDS NPP_Print(NPP instance, NPPrint* platformPrint);
int16                 NPP_HandleEvent(NPP instance, void* event);

void        NPN_Version(int* plugin_major, int* plugin_minor, int* netscape_major, int* netscape_minor);
NPError     NPN_GetURL(NPP instance, const char* url, const char* window);
NPError     NPN_PostURL(NPP instance, const char* url, const char* window, uint32 len, const char* buf, NPBool file);
NPError     NPN_RequestRead(NPStream* stream, NPByteRange* rangeList);
NPError     NPN_NewStream(NPP instance, NPMIMEType type, NPStream* stream);
int32       NPN_Write(NPP instance, NPStream* stream, int32 len, void* buffer);
NPError     NPN_DestroyStream(NPP instance, NPStream* stream, NPError reason);
void        NPN_Status(NPP instance, const char* message);
const char* NPN_UserAgent(NPP instance);
void*       NPN_MemAlloc(uint32 size);
void        NPN_MemFree(void* ptr);
uint32      NPN_MemFlush(uint32 size);
void		NPN_ReloadPlugins(NPBool reloadPages);

#ifdef __cplusplus
}  /* end extern "C" */
#endif


#endif /* _NPAPI_H_ */

