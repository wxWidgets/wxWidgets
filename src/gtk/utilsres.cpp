/////////////////////////////////////////////////////////////////////////////
// Name:        utils.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:           wxWindows licence
/////////////////////////////////////////////////////////////////////////////


//#ifdef __GNUG__
//#pragma implementation "utils.h"
//#endif

#include "wx/utils.h"
#include "wx/string.h"
#include "wx/list.h"

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#ifdef __SVR4__
#include <sys/systeminfo.h>
#endif

#include "gdk/gdkx.h"        // GDK_DISPLAY
#include "gdk/gdkprivate.h"  // gdk_progclass

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>

#include "wx/log.h"

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

// Yuck this is really BOTH site and platform dependent
// so we should use some other strategy!
#ifdef __SUN__
    #define DEFAULT_XRESOURCE_DIR "/usr/openwin/lib/app-defaults"
#else
    #define DEFAULT_XRESOURCE_DIR "/usr/lib/X11/app-defaults"
#endif

//-----------------------------------------------------------------------------
// glabal data (data.cpp)
//-----------------------------------------------------------------------------

extern wxResourceCache *wxTheResourceCache;
extern XrmDatabase wxResourceDatabase;

//-----------------------------------------------------------------------------
// utility functions for get/write resources
//-----------------------------------------------------------------------------

static char *GetResourcePath(char *buf, char *name, bool create)
{
    if (create && FileExists(name)) {
        strcpy(buf, name);
        return buf; // Exists so ...
    }
    if (*name == '/')
        strcpy(buf, name);
    else {
        // Put in standard place for resource files if not absolute
        strcpy(buf, DEFAULT_XRESOURCE_DIR);
        strcat(buf, "/");
        strcat(buf, FileNameFromPath(name));
    }
    if (create) {
        // Touch the file to create it
        FILE *fd = fopen(buf, "w");
        if (fd) fclose(fd);
    }
    return buf;
}

// Read $HOME for what it says is home, if not
// read $USER or $LOGNAME for user name else determine
// the Real User, then determine the Real home dir.
static char *GetIniFile(char *dest, const char *filename)
{
    char *home = (char *) NULL;
    if (filename && wxIsAbsolutePath(filename))
    {
      strcpy(dest, filename);
    }
    else
    {
      if ((home = wxGetUserHome(wxString())) != NULL)
      {
        strcpy(dest, home);
        if (dest[strlen(dest) - 1] != '/') strcat(dest, "/");
        if (filename == NULL)
        {
          if ((filename = getenv("XENVIRONMENT")) == NULL) filename = ".Xdefaults";
        }
        else
          if (*filename != '.') strcat(dest, ".");
        strcat(dest, filename);
      }
      else
      {
        dest[0] = '\0';
      }
    }
    return dest;
}

static void wxXMergeDatabases(void)
{
    XrmDatabase homeDB, serverDB, applicationDB;
    char filenamebuf[1024];

    char *filename = &filenamebuf[0];
    char *environment;
    char *classname = gdk_progclass;               // Robert Roebling ??
    char name[256];
    (void)strcpy(name, "/usr/lib/X11/app-defaults/");
    (void)strcat(name, classname ? classname : "wxWindows");

    // Get application defaults file, if any
    if ((applicationDB = XrmGetFileDatabase(name)))
        (void)XrmMergeDatabases(applicationDB, &wxResourceDatabase);

    // Merge server defaults, created by xrdb, loaded as a property of the root
    // window when the server initializes and loaded into the display
    // structure on XOpenDisplay;
    // if not defined, use .Xdefaults
    if (XResourceManagerString(GDK_DISPLAY()) != NULL) {
        serverDB = XrmGetStringDatabase(XResourceManagerString(GDK_DISPLAY()));
    } else {
        (void)GetIniFile(filename, (char *) NULL);
        serverDB = XrmGetFileDatabase(filename);
    }
    if (serverDB)
        XrmMergeDatabases(serverDB, &wxResourceDatabase);

    // Open XENVIRONMENT file, or if not defined, the .Xdefaults,
    // and merge into existing database

    if ((environment = getenv("XENVIRONMENT")) == NULL) {
        size_t len;
        environment = GetIniFile(filename, (const char *) NULL);
        len = strlen(environment);
#if !defined(SVR4) || defined(__sgi)
        (void)gethostname(environment + len, 1024 - len);
#else
        (void)sysinfo(SI_HOSTNAME, environment + len, 1024 - len);
#endif
    }
    if ((homeDB = XrmGetFileDatabase(environment)))
        XrmMergeDatabases(homeDB, &wxResourceDatabase);
}

//-----------------------------------------------------------------------------
// called on application exit
//-----------------------------------------------------------------------------

void wxFlushResources(void)
{
    char nameBuffer[512];

    wxNode *node = wxTheResourceCache->First();
    while (node) {
        char *file = node->key.string;
        // If file doesn't exist, create it first.
        (void)GetResourcePath(nameBuffer, file, TRUE);

        XrmDatabase database = (XrmDatabase)node->Data();
        XrmPutFileDatabase(database, nameBuffer);
        XrmDestroyDatabase(database);
        wxNode *next = node->Next();
//        delete node;
        node = next;
    }
}

void wxDeleteResources(const char *file)
{
    wxLogTrace(wxTraceResAlloc, "Delete: Number = %d", wxTheResourceCache->Number());
    char buffer[500];
    (void)GetIniFile(buffer, file);

    wxNode *node = wxTheResourceCache->Find(buffer);
    if (node) {
        XrmDatabase database = (XrmDatabase)node->Data();
        XrmDestroyDatabase(database);
//        delete node;
    }
}

//-----------------------------------------------------------------------------
// resource functions
//-----------------------------------------------------------------------------

bool wxWriteResource(const wxString& section, const wxString& entry, const wxString& value, const wxString& file )
{
    char buffer[500];

    if (!entry) return FALSE;

    (void)GetIniFile(buffer, file);

    XrmDatabase database;
    wxNode *node = wxTheResourceCache->Find(buffer);
    if (node)
        database = (XrmDatabase)node->Data();
    else {
        database = XrmGetFileDatabase(buffer);
        wxLogTrace(wxTraceResAlloc, "Write: Number = %d", wxTheResourceCache->Number());
        wxTheResourceCache->Append(buffer, (wxObject *)database);
    }
    char resName[300];
    strcpy(resName, !section.IsNull() ? WXSTRINGCAST section : "wxWindows");
    strcat(resName, ".");
    strcat(resName, entry);
    XrmPutStringResource(&database, resName, value);
    return TRUE;
};

bool wxWriteResource(const wxString& section, const wxString& entry, float value, const wxString& file )
{
    char buf[50];
    sprintf(buf, "%.4f", value);
    return wxWriteResource(section, entry, buf, file);
};

bool wxWriteResource(const wxString& section, const wxString& entry, long value, const wxString& file )
{
    char buf[50];
    sprintf(buf, "%ld", value);
    return wxWriteResource(section, entry, buf, file);
};

bool wxWriteResource(const wxString& section, const wxString& entry, int value, const wxString& file )
{
    char buf[50];
    sprintf(buf, "%d", value);
    return wxWriteResource(section, entry, buf, file);
};

bool wxGetResource(const wxString& section, const wxString& entry, char **value, const wxString& file )
{
    if (!wxResourceDatabase)
        wxXMergeDatabases();

    XrmDatabase database;
    if (file) {
        char buffer[500];
        // Is this right? Trying to get it to look in the user's
        // home directory instead of current directory -- JACS
        (void)GetIniFile(buffer, file);

        wxNode *node = wxTheResourceCache->Find(buffer);
        if (node)
            database = (XrmDatabase)node->Data();
        else {
            database = XrmGetFileDatabase(buffer);
            wxLogTrace(wxTraceResAlloc, "Get: Number = %d", wxTheResourceCache->Number());
            wxTheResourceCache->Append(buffer, (wxObject *)database);
        }
    } else
        database = wxResourceDatabase;

    XrmValue xvalue;
    char *str_type[20];
    char buf[150];
    strcpy(buf, section);
    strcat(buf, ".");
    strcat(buf, entry);

    bool success = XrmGetResource(database, buf, "*", str_type, &xvalue);
    // Try different combinations of upper/lower case, just in case...
    if (!success) {
        buf[0] = (isupper(buf[0]) ? tolower(buf[0]) : toupper(buf[0]));
        success = XrmGetResource(database, buf, "*", str_type,        &xvalue);
    }
    if (success) {
        if (*value)
            delete[] *value;
        *value = new char[xvalue.size + 1];
        strncpy(*value, xvalue.addr, (int)xvalue.size);
        return TRUE;
    }
    return FALSE;
};

bool wxGetResource(const wxString& section, const wxString& entry, float *value, const wxString& file )
{
    char *s = (char *) NULL;
    bool succ = wxGetResource(section, entry, &s, file);
    if (succ) {
        *value = (float)strtod(s, (char **) NULL);
        delete[]s;
        return TRUE;
    } else
        return FALSE;
};

bool wxGetResource(const wxString& section, const wxString& entry, long *value, const wxString& file )
{
    char *s = (char *) NULL;
    bool succ = wxGetResource(section, entry, &s, file);
    if (succ) {
        *value = strtol(s, (char **) NULL, 10);
        delete[]s;
        return TRUE;
    } else
        return FALSE;
};

bool wxGetResource(const wxString& section, const wxString& entry, int *value, const wxString& file )
{
    char *s = (char *) NULL;
    bool succ = wxGetResource(section, entry, &s, file);
    if (succ) {
        // Handle True, False here
        // True, Yes, Enables, Set or  Activated
        if (*s == 'T' || *s == 'Y' || *s == 'E' || *s == 'S' || *s == 'A')
            *value = TRUE;
        // False, No, Disabled, Reset, Cleared, Deactivated
        else if (*s == 'F' || *s == 'N' || *s == 'D' || *s == 'R' || *s == 'C')
            *value = FALSE;
        // Handle as Integer
        else
            *value = (int)strtol(s, (char **) NULL, 10);
        delete[]s;
        return TRUE;
    } else
        return FALSE;
};

