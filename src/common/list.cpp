////////////////////////////////////////////////////////////////////////////////
// Name:        list.cpp
// Purpose:     wxList implementation
// Author:      Julian Smart
// Modified by: VZ at 16/11/98: WX_DECLARE_LIST() and typesafe lists added
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
////////////////////////////////////////////////////////////////////////////////

// =============================================================================
// declarations
// =============================================================================

// -----------------------------------------------------------------------------
// headers
// -----------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "list.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/list.h"
    #include "wx/utils.h"   // for copystring() (beurk...)
#endif

// =============================================================================
// implementation
// =============================================================================

// -----------------------------------------------------------------------------
// wxListKey
// -----------------------------------------------------------------------------

wxListKey wxDefaultListKey;

bool wxListKey::operator==(wxListKeyValue value) const
{
    switch ( m_keyType )
    {
        default:
            wxFAIL_MSG(wxT("bad key type."));
            // let compiler optimize the line above away in release build
            // by not putting return here...

        case wxKEY_STRING:
            return wxStrcmp(m_key.string, value.string) == 0;

        case wxKEY_INTEGER:
            return m_key.integer == value.integer;
    }
}

// -----------------------------------------------------------------------------
// wxNodeBase
// -----------------------------------------------------------------------------

wxNodeBase::wxNodeBase(wxListBase *list,
                       wxNodeBase *previous, wxNodeBase *next,
                       void *data, const wxListKey& key)
{
    m_list = list;
    m_data = data;
    m_previous = previous;
    m_next = next;

    switch ( key.GetKeyType() )
    {
        case wxKEY_NONE:
            break;

        case wxKEY_INTEGER:
            m_key.integer = key.GetNumber();
            break;

        case wxKEY_STRING:
            // to be free()d later
            m_key.string = wxStrdup(key.GetString());
            break;

        default:
            wxFAIL_MSG(wxT("invalid key type"));
    }

    if ( previous )
        previous->m_next = this;

    if ( next )
        next->m_previous = this;
}

wxNodeBase::~wxNodeBase()
{
    // handle the case when we're being deleted from the list by the user (i.e.
    // not by the list itself from DeleteNode) - we must do it for
    // compatibility with old code
    if ( m_list != NULL )
    {
        if ( m_list->m_keyType == wxKEY_STRING )
        {
            free(m_key.string);
        }

        m_list->DetachNode(this);
    }
}

int wxNodeBase::IndexOf() const
{
    wxCHECK_MSG( m_list, wxNOT_FOUND, wxT("node doesn't belong to a list in IndexOf"));

    // It would be more efficient to implement IndexOf() completely inside
    // wxListBase (only traverse the list once), but this is probably a more
    // reusable way of doing it. Can always be optimized at a later date (since
    // IndexOf() resides in wxListBase as well) if efficiency is a problem.
    int i;
    wxNodeBase *prev = m_previous;

    for( i = 0; prev; i++ )
    {
        prev = prev->m_previous;
    }

    return i;
}

// -----------------------------------------------------------------------------
// wxListBase
// -----------------------------------------------------------------------------

void wxListBase::Init(wxKeyType keyType)
{
  m_nodeFirst =
  m_nodeLast = (wxNodeBase *) NULL;
  m_count = 0;
  m_destroy = FALSE;
  m_keyType = keyType;
}

wxListBase::wxListBase(size_t count, void *elements[])
{
  Init();

  for ( size_t n = 0; n < count; n++ )
  {
      Append(elements[n]);
  }
}

void wxListBase::DoCopy(const wxListBase& list)
{
    wxASSERT_MSG( !list.m_destroy,
                  wxT("copying list which owns it's elements is a bad idea") );

    m_count = list.m_count;
    m_destroy = list.m_destroy;
    m_keyType = list.m_keyType;
    m_nodeFirst =
    m_nodeLast = (wxNodeBase *) NULL;

    for ( wxNodeBase *node = list.GetFirst(); node; node = node->GetNext() )
    {
        Append(node);
    }
}

wxListBase::~wxListBase()
{
  wxNodeBase *each = m_nodeFirst;
  while ( each != NULL )
  {
      wxNodeBase *next = each->GetNext();
      DoDeleteNode(each);
      each = next;
  }
}

wxNodeBase *wxListBase::AppendCommon(wxNodeBase *node)
{
    if ( !m_nodeFirst )
    {
        m_nodeFirst = node;
        m_nodeLast = m_nodeFirst;
    }
    else
    {
        m_nodeLast->m_next = node;
        m_nodeLast = node;
    }

    m_count++;

    return node;
}

wxNodeBase *wxListBase::Append(void *object)
{
    // all objects in a keyed list should have a key
    wxCHECK_MSG( m_keyType == wxKEY_NONE, (wxNodeBase *)NULL,
                 wxT("need a key for the object to append") );

    wxNodeBase *node = CreateNode(m_nodeLast, (wxNodeBase *)NULL, object);

    return AppendCommon(node);
}

wxNodeBase *wxListBase::Append(long key, void *object)
{
    wxCHECK_MSG( (m_keyType == wxKEY_INTEGER) ||
                 (m_keyType == wxKEY_NONE && m_count == 0),
                 (wxNodeBase *)NULL,
                 wxT("can't append object with numeric key to this list") );

    wxNodeBase *node = CreateNode(m_nodeLast, (wxNodeBase *)NULL, object, key);
    return AppendCommon(node);
}

wxNodeBase *wxListBase::Append (const wxChar *key, void *object)
{
    wxCHECK_MSG( (m_keyType == wxKEY_STRING) ||
                 (m_keyType == wxKEY_NONE && m_count == 0),
                 (wxNodeBase *)NULL,
                 wxT("can't append object with string key to this list") );

    wxNodeBase *node = CreateNode(m_nodeLast, (wxNodeBase *)NULL, object, key);
    return AppendCommon(node);
}

wxNodeBase *wxListBase::Insert(wxNodeBase *position, void *object)
{
    // all objects in a keyed list should have a key
    wxCHECK_MSG( m_keyType == wxKEY_NONE, (wxNodeBase *)NULL,
                 wxT("need a key for the object to insert") );

    wxCHECK_MSG( !position || position->m_list == this, (wxNodeBase *)NULL,
                 wxT("can't insert before a node from another list") );

    // previous and next node for the node being inserted
    wxNodeBase *prev, *next;
    if ( position )
    {
        prev = position->GetPrevious();
        next = position;
    }
    else
    {
        // inserting in the beginning of the list
        prev = (wxNodeBase *)NULL;
        next = m_nodeFirst;
    }

    wxNodeBase *node = CreateNode(prev, next, object);
    if ( !m_nodeFirst )
    {
        m_nodeLast = node;
    }

    if ( prev == NULL )
    {
        m_nodeFirst = node;
    }

    m_count++;

    return node;
}

wxNodeBase *wxListBase::Item(size_t n) const
{
    for ( wxNodeBase *current = GetFirst(); current; current = current->GetNext() )
    {
        if ( n-- == 0 )
        {
            return current;
        }
    }

    wxFAIL_MSG( wxT("invalid index in wxListBase::Item") );

    return (wxNodeBase *)NULL;
}

wxNodeBase *wxListBase::Find(const wxListKey& key) const
{
    wxASSERT_MSG( m_keyType == key.GetKeyType(),
                  wxT("this list is not keyed on the type of this key") );

    for ( wxNodeBase *current = GetFirst(); current; current = current->GetNext() )
    {
        if ( key == current->m_key )
        {
            return current;
        }
    }

    // not found
    return (wxNodeBase *)NULL;
}

wxNodeBase *wxListBase::Find(void *object) const
{
    for ( wxNodeBase *current = GetFirst(); current; current = current->GetNext() )
    {
        if ( current->GetData() == object )
            return current;
    }

    // not found
    return (wxNodeBase *)NULL;
}

int wxListBase::IndexOf(void *object) const
{
    wxNodeBase *node = Find( object );

    return node ? node->IndexOf() : wxNOT_FOUND;
}

void wxListBase::DoDeleteNode(wxNodeBase *node)
{
    // free node's data
    if ( m_keyType == wxKEY_STRING )
    {
        free(node->m_key.string);
    }

    if ( m_destroy )
    {
        node->DeleteData();
    }

    // so that the node knows that it's being deleted by the list
    node->m_list = NULL;
    delete node;
}

wxNodeBase *wxListBase::DetachNode(wxNodeBase *node)
{
    wxCHECK_MSG( node, NULL, wxT("detaching NULL wxNodeBase") );
    wxCHECK_MSG( node->m_list == this, NULL,
                 wxT("detaching node which is not from this list") );

    // update the list
    wxNodeBase **prevNext = node->GetPrevious() ? &node->GetPrevious()->m_next
                                                : &m_nodeFirst;
    wxNodeBase **nextPrev = node->GetNext() ? &node->GetNext()->m_previous
                                            : &m_nodeLast;

    *prevNext = node->GetNext();
    *nextPrev = node->GetPrevious();

    m_count--;

    // mark the node as not belonging to this list any more
    node->m_list = NULL;

    return node;
}

bool wxListBase::DeleteNode(wxNodeBase *node)
{
    if ( !DetachNode(node) )
        return FALSE;

    DoDeleteNode(node);

    return TRUE;
}

bool wxListBase::DeleteObject(void *object)
{
    for ( wxNodeBase *current = GetFirst(); current; current = current->GetNext() )
    {
        if ( current->GetData() == object )
        {
            DeleteNode(current);
            return TRUE;
        }
    }

    // not found
    return FALSE;
}

void wxListBase::Clear()
{
    wxNodeBase *current = m_nodeFirst;
    while ( current )
    {
        wxNodeBase *next = current->GetNext();
        DoDeleteNode(current);
        current = next;
    }

    m_nodeFirst =
    m_nodeLast = (wxNodeBase *)NULL;

    m_count = 0;
}

void wxListBase::ForEach(wxListIterateFunction F)
{
    for ( wxNodeBase *current = GetFirst(); current; current = current->GetNext() )
    {
        (*F)(current->GetData());
    }
}

void *wxListBase::FirstThat(wxListIterateFunction F)
{
    for ( wxNodeBase *current = GetFirst(); current; current = current->GetNext() )
    {
        if ( (*F)(current->GetData()) )
            return current->GetData();
    }

    return (wxNodeBase *)NULL;
}

void *wxListBase::LastThat(wxListIterateFunction F)
{
    for ( wxNodeBase *current = GetLast(); current; current = current->GetPrevious() )
    {
        if ( (*F)(current->GetData()) )
            return current->GetData();
    }

    return (wxNodeBase *)NULL;
}

// (stefan.hammes@urz.uni-heidelberg.de)
//
// function for sorting lists. the concept is borrowed from 'qsort'.
// by giving a sort function, arbitrary lists can be sorted.
// method:
// - put wxObject pointers into an array
// - sort the array with qsort
// - put back the sorted wxObject pointers into the list
//
// CAVE: the sort function receives pointers to wxObject pointers (wxObject **),
//       so dereference right!
// EXAMPLE:
//   int listcompare(const void *arg1, const void *arg2)
//   {
//      return(compare(**(wxString **)arg1,
//                     **(wxString **)arg2));
//   }
//
//   void main()
//   {
//     wxListBase list;
//
//     list.Append(new wxString("DEF"));
//     list.Append(new wxString("GHI"));
//     list.Append(new wxString("ABC"));
//     list.Sort(listcompare);
//   }

void wxListBase::Sort(const wxSortCompareFunction compfunc)
{
    // allocate an array for the wxObject pointers of the list
    const size_t num = GetCount();
    void **objArray = new void *[num];
    void **objPtr = objArray;

    // go through the list and put the pointers into the array
    wxNodeBase *node;
    for ( node = GetFirst(); node; node = node->Next() )
    {
        *objPtr++ = node->Data();
    }

    // sort the array
    qsort((void *)objArray,num,sizeof(wxObject *),compfunc);

    // put the sorted pointers back into the list
    objPtr = objArray;
    for ( node = GetFirst(); node; node = node->Next() )
    {
        node->SetData(*objPtr++);
    }

    // free the array
    delete[] objArray;
}

// -----------------------------------------------------------------------------
// wxList (a.k.a. wxObjectList)
// -----------------------------------------------------------------------------

void wxObjectListNode::DeleteData()
{
    delete (wxObject *)GetData();
}

// -----------------------------------------------------------------------------
// wxStringList
// -----------------------------------------------------------------------------

// instead of WX_DEFINE_LIST(wxStringListBase) we define this function
// ourselves
void wxStringListNode::DeleteData()
{
    delete [] (char *)GetData();
}

bool wxStringList::Delete(const wxChar *s)
{
    wxStringListNode *current;

    for ( current = GetFirst(); current; current = current->GetNext() )
    {
        if ( wxStrcmp(current->GetData(), s) == 0 )
        {
            DeleteNode(current);
            return TRUE;
        }
    }

    // not found
    return FALSE;
}

void wxStringList::DoCopy(const wxStringList& other)
{
    wxASSERT( GetCount() == 0 );    // this list must be empty before copying!

    size_t count = other.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        Add(other.Item(n)->GetData());
    }
}

// Variable argument list, terminated by a zero
// Makes new storage for the strings
wxStringList::wxStringList (const wxChar *first, ...)
{
  if ( !first )
    return;

  va_list ap;
  va_start(ap, first);

  const wxChar *s = first;
  for (;;)
  {
      Add(s);

      s = va_arg(ap, const wxChar *);
      //    if (s == NULL)
#ifdef __WXMSW__
      if ((int)(long) s == 0)
#else
      if ((long) s == 0)
#endif
          break;
  }

  va_end(ap);
}

// Only makes new strings if arg is TRUE
wxChar **wxStringList::ListToArray(bool new_copies) const
{
    wxChar **string_array = new wxChar *[GetCount()];
    wxStringListNode *node = GetFirst();
    for (size_t i = 0; i < GetCount(); i++)
    {
        wxChar *s = node->GetData();
        if ( new_copies )
            string_array[i] = copystring(s);
        else
            string_array[i] = s;
        node = node->GetNext();
    }

    return string_array;
}

// Checks whether s is a member of the list
bool wxStringList::Member(const wxChar *s) const
{
    for ( wxStringListNode *node = GetFirst(); node; node = node->GetNext() )
    {
        const wxChar *s1 = node->GetData();
        if (s == s1 || wxStrcmp (s, s1) == 0)
            return TRUE;
    }

    return FALSE;
}

static int LINKAGEMODE
wx_comparestrings(const void *arg1, const void *arg2)
{
  wxChar **s1 = (wxChar **) arg1;
  wxChar **s2 = (wxChar **) arg2;

  return wxStrcmp (*s1, *s2);
}

// Sort a list of strings - deallocates old nodes, allocates new
void wxStringList::Sort()
{
    size_t N = GetCount();
    wxChar **array = new wxChar *[N];
    wxStringListNode *node;

    size_t i = 0;
    for ( node = GetFirst(); node; node = node->GetNext() )
    {
        array[i++] = node->GetData();
    }

    qsort (array, N, sizeof (wxChar *), wx_comparestrings);

    i = 0;
    for ( node = GetFirst(); node; node = node->GetNext() )
        node->SetData( array[i++] );

    delete [] array;
}
