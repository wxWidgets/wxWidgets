/////////////////////////////////////////////////////////////////////////////
// Name:        memory.h
// Purpose:     MDI classes
// Author:      Arthur Seaton, Julian Smart
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MEMORYH__
#define _WX_MEMORYH__

#ifdef __GNUG__
#pragma interface "memory.h"
#endif

#include "wx/defs.h"

/*
  The macro which will be expanded to include the file and line number
  info, or to be a straight call to the new operator.
*/

#if (WXDEBUG && USE_MEMORY_TRACING) || USE_DEBUG_CONTEXT

#include <stddef.h>

#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif

#include "wx/string.h"

#if WXDEBUG

// #ifndef WXDEBUG_NEW
// #define WXDEBUG_NEW new(__FILE__,__LINE__)
// #endif

void * wxDebugAlloc(size_t size, char * fileName, int lineNum, bool isObject, bool isVect = FALSE);
void wxDebugFree(void * buf, bool isVect = FALSE);

// Global versions of the new and delete operators.
// Currently, these merely call malloc and free; only the wxObject
// operators do something interesting. But this allows WXDEBUG_NEW to
// work for all 'new's in a file.
#if USE_GLOBAL_MEMORY_OPERATORS

#ifdef new
#undef new
#endif

void * operator new (size_t size, char * fileName, int lineNum);
void operator delete (void * buf);

// VC++ 6.0
#if _MSC_VER >= 1200
void operator delete(void *buf, char*, int);
#endif

#if !( defined (_MSC_VER) && (_MSC_VER <= 1020) )
void * operator new[] (size_t size, char * fileName, int lineNum);
void operator delete[] (void * buf);
#endif

#endif
#endif

typedef unsigned int wxMarkerType;

/*
  Define the struct which will be placed at the start of all dynamically
  allocated memory.
*/

class WXDLLEXPORT wxMemStruct {

friend class WXDLLEXPORT wxDebugContext; // access to the m_next pointer for list traversal.

public:
public:
    int AssertList ();

    size_t RequestSize () { return m_reqSize; }
    wxMarkerType Marker () { return m_firstMarker; }

    // When an object is deleted we set the id slot to a specific value.
    inline void SetDeleted ();
    inline int IsDeleted ();

    int Append ();
    int Unlink ();

    // Used to determine if the object is really a wxMemStruct.
    // Not a foolproof test by any means, but better than none I hope!
    int AssertIt ();

    // Do all validation on a node.
    int ValidateNode ();

    // Check the integrity of a node and of the list, node by node.
    int CheckBlock ();
    int CheckAllPrevious ();

    // Print a single node.
    void PrintNode ();

    // Called when the memory linking functions get an error.
    void ErrorMsg (const char *);
    void ErrorMsg ();

    inline void *GetActualData(void) const { return m_actualData; }

    void Dump(void);

public:
    // Check for underwriting. There are 2 of these checks. This one
    // inside the struct and another right after the struct.
    wxMarkerType        m_firstMarker;

    // File name and line number are from cpp.
    char*               m_fileName;
    int                 m_lineNum;

    // The amount of memory requested by the caller.
    size_t              m_reqSize;

    // Used to try to verify that we really are dealing with an object
    // of the required class. Can be 1 of 2 values these indicating a valid
    // wxMemStruct object, or a deleted wxMemStruct object.
    wxMarkerType        m_id;

    wxMemStruct *       m_prev;
    wxMemStruct *       m_next;

    void *              m_actualData;
    bool                m_isObject;
};


typedef void (wxMemStruct::*PmSFV) ();


/*
  Debugging class. This will only have a single instance, but it\'s
  a reasonable way to keep everything together and to make this
  available for change if needed by someone else.
  A lot of this stuff would be better off within the wxMemStruct class, but
  it\'s stuff which we need to access at times when there is no wxMemStruct
  object so we use this class instead. Think of it as a collection of
  globals which have to do with the wxMemStruct class.
*/

class WXDLLEXPORT wxDebugContext {

protected:
    // Used to set alignment for markers.
    static size_t CalcAlignment ();

    // Returns the amount of padding needed after something of the given
    // size. This is so that when we cast pointers backwards and forwards
    // the pointer value will be valid for a wxMarkerType.
    static size_t GetPadding (const size_t size) ;

    // Traverse the list.
    static void TraverseList (PmSFV, wxMemStruct *from = NULL);

    static streambuf *m_streamBuf;
    static ostream *m_debugStream;

    static int debugLevel;
    static bool debugOn;

public:
    // Set a checkpoint to dump only the memory from
    // a given point
    static wxMemStruct *checkPoint;

    wxDebugContext(void);
    ~wxDebugContext(void);

    static bool HasStream(void) { return (m_debugStream != NULL); };
    static ostream& GetStream(void) { return *m_debugStream; }
    static streambuf *GetStreamBuf(void) { return m_streamBuf; }
    static void SetStream(ostream *stream, streambuf *buf = NULL);
    static bool SetFile(const wxString& file);
    static bool SetStandardError(void);

    static int GetLevel(void) { return debugLevel; }
    static void SetLevel(int level) { debugLevel = level; }

    static bool GetDebugMode(void) { return debugOn; }
    static void SetDebugMode(bool flag) { debugOn = flag; }

    static void SetCheckpoint(bool all = FALSE);
    static wxMemStruct *GetCheckpoint(void) { return checkPoint; }
    
    // Calculated from the request size and any padding needed
    // before the final marker.
    static size_t PaddedSize (const size_t reqSize);

    // Calc the total amount of space we need from the system
    // to satisfy a caller request. This includes all padding.
    static size_t TotSize (const size_t reqSize);

    // Return valid pointers to offsets within the allocated memory.
    static char * StructPos (const char * buf);
    static char * MidMarkerPos (const char * buf);
    static char * CallerMemPos (const char * buf);
    static char * EndMarkerPos (const char * buf, const size_t size);

    // Given a pointer to the start of the caller requested area
    // return a pointer to the start of the entire alloc\'d buffer.
    static char * StartPos (const char * caller);

    // Access to the list.
    static wxMemStruct * GetHead () { return m_head; }
    static wxMemStruct * GetTail () { return m_tail; }

    // Set the list sentinals.
    static wxMemStruct * SetHead (wxMemStruct * st) { return (m_head = st); }
    static wxMemStruct * SetTail (wxMemStruct * st) { return (m_tail = st); }

    // If this is set then every new operation checks the validity
    // of the all previous nodes in the list.
    static bool GetCheckPrevious () { return m_checkPrevious; }
    static void SetCheckPrevious (bool value) { m_checkPrevious = value; }

    // Checks all nodes, or all nodes if checkAll is TRUE
    static int Check(bool checkAll = FALSE);

    // Print out the list of wxMemStruct nodes.
    static bool PrintList(void);

    // Dump objects
    static bool Dump(void);

    // Print statistics
    static bool PrintStatistics(bool detailed = TRUE);

    // Print out the classes in the application.
    static bool PrintClasses(void);

    // Count the number of non-wxDebugContext-related objects
    // that are outstanding
    static int CountObjectsLeft(void);

private:
    // Store these here to allow access to the list without
    // needing to have a wxMemStruct object.
    static wxMemStruct*         m_head;
    static wxMemStruct*         m_tail;

    // Set to FALSE if we're not checking all previous nodes when
    // we do a new. Set to TRUE when we are.
    static bool                 m_checkPrevious;
};

// Output a debug mess., in a system dependent fashion.
void WXDLLEXPORT wxTrace(const char *fmt ...);
void WXDLLEXPORT wxTraceLevel(int level, const char *fmt ...);

#define WXTRACE wxTrace
#define WXTRACELEVEL wxTraceLevel

#else // else part for the #if WXDEBUG

inline void wxTrace(const char *WXUNUSED(fmt)) {}
inline void wxTraceLevel(int WXUNUSED(level), const char *WXUNUSED(fmt)) {}

#define WXTRACE TRUE ? (void)0 : wxTrace
#define WXTRACELEVEL TRUE ? (void)0 : wxTraceLevel
// #define WXDEBUG_NEW new

#endif // WXDEBUG

#endif
    // _WX_MEMORYH__

