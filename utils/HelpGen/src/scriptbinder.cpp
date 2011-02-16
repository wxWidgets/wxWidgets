/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     22/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Aleskandars Gluchovas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <string.h>
#include <memory.h>

#include <stdio.h>   // import sprintf() (for doubles)
#include <stdlib.h>  // import atoi()    (for integers)

#include "scriptbinder.h"

// helper functions

static size_t wx_log2(size_t nr)
{
    size_t tmp = 0;
    while (nr >= 2 )
    {
        nr /= 2;
        ++tmp;
    }

    return tmp;
}

/***** Implementation for class ScriptStream *****/

ScriptStream::ScriptStream()
    : m_pBuf(0),
      m_Size(0),
      m_Capacity(0)
{}

ScriptStream::~ScriptStream()
{
    delete [] m_pBuf;
}

void ScriptStream::WriteBytes( const void* srcBuf, size_t count )
{
    if ( !count ) return;

    // increase the capacity if necessary
    if ( m_Size + count > m_Capacity )
    {
        m_Capacity =
            (  0x2 << (wx_log2( m_Size + count ) + 1 ) );

        if ( m_Capacity < 128 ) m_Capacity = 128;

        char* oldBuf = m_pBuf;

        m_pBuf = new char[m_Capacity];

        if ( oldBuf )
        {
            memcpy( m_pBuf, oldBuf, m_Size );
            delete [] oldBuf;
        }
    }

    // append new data
    memcpy( &m_pBuf[m_Size], srcBuf, count );

    m_Size += count;
}

ScriptStream& ScriptStream::operator<<( const char* str )
{
    WriteBytes( str, strlen( str ) );

    return *this;
}

ScriptStream& ScriptStream::operator<<( const wxString& str )
{
    if ( str.length() < 512 )
    {
        char buf[512];
        size_t len = str.length();

        for( size_t i = 0; i != len; ++i )
            buf[i] = str[i];

        WriteBytes( buf, len );
    }
    else
        WriteBytes( str.c_str(), str.length() );

    return *this;
}

ScriptStream& ScriptStream::operator<<( char ch )
{
    WriteBytes( &ch, 1 );

    return *this;
}

void ScriptStream::endl()
{
    char ch = '\n';
    WriteBytes( &ch, 1 );
}

/***** Implementation for class ScriptTemplate *****/

ScriptTemplate::ScriptTemplate( const wxString& templateText )
{
    wxString tmp = templateText;

    m_TText = (char*)malloc( tmp.length() + 1 );

    strcpy( m_TText, tmp.c_str() );
}

ScriptTemplate::~ScriptTemplate()
{
    for( size_t i = 0; i != m_Vars.size(); ++i )

        delete m_Vars[i];

    free( m_TText );
}

bool ScriptTemplate::HasVar( const char* name )
{
    for( size_t i = 0; i != m_Vars.size(); ++i )

        if ( strcmp( m_Vars[i]->m_Name, name ) == 0 )

            return 1;

    return 0;
}

void ScriptTemplate::AddStringVar ( const char* name, int ofs )
{
    m_Vars.push_back( new TVarInfo( name, ofs, TVAR_STRING ) );
}

void ScriptTemplate::AddIntegerVar( const char* name, int ofs )
{
    m_Vars.push_back( new TVarInfo( name, ofs, TVAR_INTEGER ) );
}

void ScriptTemplate::AddDoubleVar ( const char* name, int ofs )
{
    m_Vars.push_back( new TVarInfo( name, ofs, TVAR_DOUBLE ) );
}

void ScriptTemplate::AddObjectRefArray( const char*     name,
                                        int                ofsRefToFirstObj,
                                        int                ofsObjSizeInt,
                                        int                ofsObjRefTempl
                                      )
{
    TArrayInfo* pInfo = new TArrayInfo( name );

    m_Vars.push_back( pInfo );

    pInfo->m_RefOfs         = ofsRefToFirstObj;
    pInfo->m_SizeIntOfs     = ofsObjSizeInt;
    pInfo->m_ObjRefTemplOfs = ofsObjRefTempl;
}

inline void ScriptTemplate::PrintVar( TVarInfo*   pInfo,
                                    void*       dataObj,
                                    ScriptStream& stm )
{
    char buf[128];

    switch ( pInfo->m_Type )
    {
        case TVAR_INTEGER :
            {
                sprintf(buf, "%d",*( (int*) ((char*)dataObj + pInfo->m_Ofs) ) );

                stm.WriteBytes( buf, strlen(buf ) );
                break;
            }

        case TVAR_STRING :
            {
                wxString& str = *( (wxString*) ((char*)dataObj+pInfo->m_Ofs) );

                const char* cs = str.c_str();
#ifdef DEBUG_WEIRED_OFFSETS
                cout << "DBG:: cs address is " << (int)cs << endl;
                cout << "DBG:: str address is " << (int)(&str) << endl;
                cout << "DBG:: dataObj points to " << (int)dataObj << endl;
                cout << "DBG:: pInfo->m_Ofs value is " << (int)pInfo->m_Ofs << endl;
                cout << "DBG:: d+pInfo->m_Ofs is " << (int)((char*)dataObj + pInfo->m_Ofs) << endl;
                cout << "DBG:: pInfo->m_Name is " << pInfo->m_Name << endl;
                cout << "DBG:: pInfo->m_Type is " << pInfo->m_Type << endl;
                cout << "DBG:: end of dump. " << endl;

                cout << "DBG:: cs value is " << endl << cs << endl;
#endif
                stm.WriteBytes( cs, strlen(cs) );
                break;
            }

        case TVAR_DOUBLE :
            {
                sprintf( buf, "%f",
                         *( (double*)( (char*)dataObj+pInfo->m_Ofs)  ) );

                stm.WriteBytes( buf, strlen(buf ) );
                break;
            }

        case TVAR_REF_ARRAY :
            {
                TArrayInfo& info = *((TArrayInfo*)pInfo);

                int sz = *((int*) ( (char*)dataObj+info.m_SizeIntOfs ));
                if ( !sz )
                {
                    // DBG::
                    int u = 0;
                    ++u;
                    break;
                }

                int*   array = *((int**)( (char*)dataObj+info.m_RefOfs ));

                ScriptTemplate* pRefTempl;

                for( int i = 0; i != sz; ++i )
                {
                    pRefTempl =
                        *((ScriptTemplate**)((char*)(array[i])+info.m_ObjRefTemplOfs));

                    pRefTempl->PrintScript( (void*)array[i], stm );
                }

                break;
            }

        default : break;
    }
}

void ScriptTemplate::PrintScript( void* dataObj, ScriptStream& stm )
{
    char* cur   = m_TText;

    // template parsing loop
    do
    {
        char* start = cur;

        while( *cur != '\0' && *cur != '$' ) ++cur;

        // flush text collected between variables
        stm.WriteBytes( start, cur - start );

        if ( *cur == '\0' ) break;

        cur += 2; // skip to the name of the var

        start = cur;

        while( *cur != ')' ) ++cur;

        // put terminating zero temorarely

        *cur = '\0';

        // look up variable

        size_t sz = m_Vars.size();
        // bool found = false;

        for( size_t i = 0; i != sz; ++i )
        {
            if ( strcmp( m_Vars[i]->m_Name, start ) == 0 )
            {
                PrintVar( m_Vars[i], dataObj, stm );

                *cur = ')';    // remove terminating zero
                ++cur;
                // found = 1;
                break;
            }
        }

        // variable referred by template script is not
        // registered to this tempalte object
        // ASSERT( found );

    } while(1);
}

/***** implementation for class ScriptSection *****/

int ScriptSection::m_IdCounter = 0;

ScriptSection::ScriptSection( const wxString& name,
                              const wxString& body,
                              ScriptTemplate* pSectionTemplate,
                              ScriptTemplate* pReferenceTemplate,
                              bool            autoHide,
                              bool            sorted
                            )
    : m_pParent  ( NULL ),

      m_Name     ( name ),
      m_Body     ( body ),

      m_AutoHide ( autoHide ),
      m_SortOn   ( sorted ),

      m_pSectTempl( pSectionTemplate ),
      m_pRefTempl ( pReferenceTemplate ),

      m_RefCount( 0 ),
      m_ArrSize( 0 )
{
    // generate GUID

    wxChar buf[32];
    wxSprintf( buf, _T("%d"), ++m_IdCounter );
    m_Id = buf;
}

ScriptSection::~ScriptSection()
{
    SectListT lst = m_Subsections;

    while( m_Subsections.size() )

        m_Subsections[0]->RemoveRef();

    for( size_t i = 0; i != m_References.size(); ++i )

        m_References[i]->RemoveRef();
}

void ScriptSection::AddRef()
{
    ++m_RefCount;
}

void ScriptSection::RemoveRef()
{
    if ( !m_RefCount || !(--m_RefCount) )
    {
        if (m_pParent)
        {
            // remove ourselves from parent's list

            SectListT& lst = m_pParent->m_Subsections;
            for( size_t i = 0; i != lst.size(); ++i )

                if ( lst[i] == this )
                {
                    lst.erase( &lst[i] );
                    break;
                }
        }

        delete this;
    }
}

ScriptSection* ScriptSection::GetSubsection( const char* name )
{
    // FOR NOW:: fixed section name length
    char buf[128];

    size_t cur = 0;

    while( name[cur] && name[cur] != '/' )
    {
        buf[cur] = name[cur];
        ++cur;
    }

    // ASSERT( cur < sizeof(buf) );

    buf[cur] = '\0';

    size_t sz = m_Subsections.size();

    for( size_t i = 0; i != sz; ++i )
    {
        // DBG::
        //ScriptSection& sect = *m_Subsections[i];

        if ( m_Subsections[i]->m_Name == buf )
        {
            if ( name[cur] == '/' )

                // search recursivelly
                return m_Subsections[i]->GetSubsection( &name[cur+1] );
            else
                return m_Subsections[i];
        }
    }

    return 0;
}

void ScriptSection::AddSection( ScriptSection* pSection,
                                bool addToReferencesToo
                              )
{
    m_Subsections.push_back( pSection );

    pSection->AddRef();

    // can add section to multiple containers
    // ASSERT( pSection->m_pParent == 0 );

    pSection->m_pParent = this;

    if ( addToReferencesToo )

        AddReference( pSection );
}

void ScriptSection::AddReference( ScriptSection* pReferredSection )
{
    m_References.push_back( pReferredSection );

    pReferredSection->AddRef();

    // set up mandatory fields used by ScriptTemplate
    m_ArrSize  = m_References.size();
    if ( m_ArrSize )
        m_RefFirst = (void*)&m_References[0];
}

SectListT& ScriptSection::GetSubsections()
{
    return m_Subsections;
}

// static method:
void ScriptSection::RegisterTemplate( ScriptTemplate& sectionTempalte )
{
    int nameOfs, bodyOfs, idOfs,
        arrRefOfs, arrSizeOfs, refTemplOfs;

    // obtaining offsets of member vars

    GET_VAR_OFS( ScriptSection, m_Name,    &nameOfs    )
    GET_VAR_OFS( ScriptSection, m_Body,    &bodyOfs    )
    GET_VAR_OFS( ScriptSection, m_Id,      &idOfs      )
    GET_VAR_OFS( ScriptSection, m_RefFirst,&arrRefOfs  )
    GET_VAR_OFS( ScriptSection, m_ArrSize, &arrSizeOfs )

    GET_VAR_OFS( ScriptSection, m_pRefTempl, &refTemplOfs )

    // registering member variables with given script template

    sectionTempalte.AddStringVar( "NAME", nameOfs );
    sectionTempalte.AddStringVar( "BODY", bodyOfs );
    sectionTempalte.AddStringVar( "ID",   idOfs   );

    sectionTempalte.AddObjectRefArray( "REFLIST",
                                       arrRefOfs, arrSizeOfs, refTemplOfs );
}

void ScriptSection::Print( ScriptStream& stm )
{
    // TBD:: sorting

    // print out this content first
    if ( m_pSectTempl )

        m_pSectTempl->PrintScript( this, stm );

    // attach contents subsections at the end of this content

    for( size_t i = 0; i != m_Subsections.size(); ++i )

        m_Subsections[i]->Print( stm );
}

void ScriptSection::DoRemoveEmptySections(int& nRemoved, SectListT& removedLst)
{
    for( size_t i = 0; i != m_Subsections.size(); ++i )
    {
        ScriptSection& sect = *m_Subsections[i];

        sect.DoRemoveEmptySections( nRemoved, removedLst );

        if (sect.m_AutoHide )

            if ( sect.m_References.size() == 0 )
            {
                bool found = false;
                for( size_t k = 0; k != removedLst.size(); ++k )

                    if ( removedLst[k] == &sect )
                    {
                        found = 1;
                        break;
                    }

                if ( !found )
                {
                    removedLst.push_back( &sect );
                    ++nRemoved;

                    delete &sect;
                    --i;
                }
            }
    }
}

void ScriptSection::DoRemoveDeadLinks( SectListT& removedLst)
{
    size_t dsz = removedLst.size();

    for( size_t i = 0; i != m_Subsections.size(); ++i )
    {
        m_Subsections[i]->DoRemoveDeadLinks( removedLst );
    }

    for( size_t n = 0; n != m_References.size(); ++n )
    {
        for( size_t k = 0; k != dsz; ++k )

            if ( removedLst[k] == m_References[n] )
            {
                m_References.erase( &m_References[n] );
                --n;

                // set up mandatory fields used by ScriptTemplate
                m_ArrSize  = m_References.size();
                if ( m_ArrSize )
                    m_RefFirst = (void*)&m_References[0];

                break;
            }
    }
}


void ScriptSection::RemoveEmptySections()
{
    // FIXME:: this is very_very_very slow alg.! +"doesn't work"

    int nRemoved = 0;

    do
    {
        SectListT removedLst;
        nRemoved = 0;

        DoRemoveEmptySections( nRemoved, removedLst );

        DoRemoveDeadLinks( removedLst );
    }
    while( nRemoved );
}

/***** Iimplementation for class DocGeneratorBase *****/

bool DocGeneratorBase::SaveDocument( const char*    fname,
                                     const char*    fopenOptions,
                                     ScriptSection* pFromSection
                                   )
{
    FILE* fp = fopen( fname, fopenOptions );

    if ( !fp ) return 0;

    ScriptStream stm;

    // check if derived class agrees about saving it
    if ( !OnSaveDocument( stm ) )
    {
        fclose( fp );
        return 0;
    }

    if ( pFromSection )

        pFromSection->Print( stm );
    else
    {
        ScriptSection* pTopSect = GetTopSection();
        // ASSERT( pTopSect );
        pTopSect->Print( stm );
    }

    size_t nWrite = fwrite( stm.GetBuf(), 1, stm.GetBufSize(), fp );

    if ( nWrite != stm.GetBufSize() ) return 0;

    fclose( fp );

    return 1;

    // that^s it
}
