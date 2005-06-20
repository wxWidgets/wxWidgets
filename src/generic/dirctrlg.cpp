/////////////////////////////////////////////////////////////////////////////
// Name:        dirctrlg.cpp
// Purpose:     wxGenericDirCtrl
// Author:      Harm van der Heijden, Robert Roebling, Julian Smart
// Modified by:
// Created:     12/12/98
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden, Robert Roebling and Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef __EMX__
#define __OS2__
#endif

#if wxUSE_DIRDLG

#include "wx/utils.h"
#include "wx/dialog.h"
#include "wx/button.h"
#include "wx/layout.h"
#include "wx/msgdlg.h"
#include "wx/textctrl.h"
#include "wx/textdlg.h"
#include "wx/filefn.h"
#include "wx/cmndata.h"
#include "wx/gdicmn.h"
#include "wx/intl.h"
#include "wx/imaglist.h"
#include "wx/icon.h"
#include "wx/log.h"
#include "wx/sizer.h"
#include "wx/tokenzr.h"
#include "wx/dir.h"
#include "wx/settings.h"

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

#include "wx/generic/dirctrlg.h"

#if defined(__WXMAC__)
  #include  "wx/mac/private.h"  // includes mac headers
#endif

#ifdef __WXMSW__
#include <windows.h>

// FIXME - Mingw32 1.0 has both _getdrive() and _chdrive(). For now, let's assume
//         older releases don't, but it should be verified and the checks modified
//         accordingly.
#if !defined(__WXWINE__) && (!defined(__GNUWIN32__) || \
    (defined(__MINGW32_MAJOR_VERSION) && __MINGW32_MAJOR_VERSION >= 1))
  #include <direct.h>
  #include <stdlib.h>
  #include <ctype.h>
#endif

#endif

#ifdef __OS2__

#define INCL_BASE
#include <os2.h>
#ifndef __EMX__
#include <direct.h>
#endif
#include <stdlib.h>
#include <ctype.h>

#endif // __OS2__

#if defined(__WXMAC__)
#  include "MoreFilesExtras.h"
#endif

#ifdef __BORLANDC__
#include "dos.h"
#endif

// If compiled under Windows, this macro can cause problems
#ifdef GetFirstChild
#undef GetFirstChild
#endif

/* Closed folder */
/* Copyright (c) Julian Smart */
static const char * icon1_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 112 2",
"r  c #F7FAFD",
"%. c #3562AF",
"=. c #3F6AB2",
"-  c #4975BE",
"O. c #A5BDE6",
"k  c #446EB4",
"d  c #B9CDEC",
"5  c #AFC5EA",
"@  c #6790D4",
".. c #769CDA",
"M  c #6787BD",
"D  c #CDDAF1",
"F  c #D7E2F5",
"]  c #99B5E4",
"%  c #4772B8",
";. c #476FB3",
">. c #4C73B5",
"@. c #5B7FBA",
"`  c #4F80CF",
"*. c #3B67B0",
"O  c #406BB2",
"N  c #406BB3",
"R  c #6891D6",
"j  c #6888BD",
"(  c #D8E3F5",
"$  c #3460A9",
"&  c #4873BA",
"'  c #5C89D3",
"-. c #436CB2",
"u  c #4870B4",
"[  c #B8CBEC",
"7  c #C2D3EF",
"H  c #3C68B1",
"a  c #A2BBE6",
"+  c #6E96D8",
":  c #698FCE",
"9  c #D4E0F4",
"t  c #FCFDFE",
":. c #4971B4",
"3  c #4E75B6",
"<  c #6C93D1",
"6  c #B9CCEC",
"c  c #C3D4EF",
"l  c #80A3DD",
"|  c #4276CC",
"q  c #E6EDF9",
"e  c #F0F5FC",
"#. c #2E5AA6",
"x  c #A3BCE6",
"s  c #ADC4E9",
"B  c #749BDA",
"1. c #567AB7",
",  c #6A90D0",
"#  c #6589C5",
"G  c #6586BD",
"8  c #CBD9F1",
"g  c #D5E1F4",
"z  c #8DACE0",
"p  c #97B4E3",
"   c None",
"*  c #4A75BC",
"+. c #C4D5EF",
"V  c #81A4DD",
"m  c #ECF2FA",
"n  c #E7EEF9",
"Y  c #3966B1",
"Z  c #A4BDE6",
"A  c #AEC5E9",
"o  c #436BAD",
"T  c #5C88D2",
"J  c #6690D5",
"<. c #5277B6",
")  c #6183BC",
"f  c #CCDAF1",
"v  c #D6E2F5",
"o. c #98B5E3",
"Q  c #8EADE1",
" . c #5080CF",
"=  c #4B76BE",
"I  c #B6CAEC",
"S  c #BBCEED",
"_  c #3563AF",
"!  c #A5BEE6",
"/  c #CDDBF2",
"X. c #8FAEE1",
"0  c #DFE8F7",
"$. c #3160AE",
"}  c #3160AF",
"4  c #A1BBE5",
"1  c #6D93D1",
"{  c #5E81BB",
"U  c #6385BC",
"^  c #C4D4EF",
"2  c #597DBC",
"K  c #81A3DD",
"i  c #8BABE0",
"w  c #ECF1FA",
"h  c #F1F5FC",
"&. c #3965B0",
"X  c #3E69B1",
"P  c #AEC4E9",
"~  c #B8CCEC",
";  c #5C84C6",
"E  c #759BDA",
"y  c #6B8ABE",
"b  c #E0E9F7",
"C  c #98B4E3",
".  c #325EA7",
">  c #6990CE",
",. c #5076B6",
"W  c #82A4DD",
"L  c #8CACE0",
/* pixels */
"                                ",
"                                ",
"  . X X X o                     ",
"  O + + + @ #                   ",
"  $ % & * = - ; : : > , < 1 2   ",
"  3 4 5 6 7 8 9 0 q w e r t y   ",
"  u i p a s d 7 f g 0 q w h j   ",
"  k l z p x 5 6 c f v b n m M   ",
"  N B V z C Z A S c D F b n G   ",
"  H J B K L C x P I c f v b U   ",
"  Y T R E W Q C ! P ~ ^ / ( )   ",
"  _ ` ' R E W z ] Z P [ ^ / {   ",
"  } |  .' R ..V X.o.O.P [ +.@.  ",
"  #.$.%.&.*.=.-.;.:.>.,.<.1.*.  ",
"                                ",
"                                "
};

/* Open folder */
/* Copyright (c) Julian Smart */
static const char * icon2_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 57 1",
"u c #83A5DD",
"h c #83A5DE",
"> c #DAE3F2",
": c #F3F7FC",
"t c #4A79C6",
"s c #274E8F",
"n c #4D71AB",
"5 c #C2D0E8",
"9 c #84A6DE",
"x c #3761A5",
"p c #ACC3E8",
"$ c #ACC3E9",
"= c #D9E4F5",
"o c #91AFE2",
"f c #5886D1",
"2 c #B9CCEC",
"& c #C3D4EF",
"; c #EBF1FA",
"7 c #4270BC",
"# c #9EB8E5",
"0 c #ADC4E9",
"y c #658FD5",
"  c None",
"@ c #688DCA",
"< c #779CDA",
"r c #2A5498",
"l c #254A87",
"g c #6690D5",
"M c #5778AE",
"m c #5274AD",
"a c #D6E2F4",
", c #4672BA",
"v c #4168A8",
"c c #3C64A7",
". c #416BB2",
"- c #E3EBF8",
"z c #305CA3",
"X c #446CAE",
"O c #5882C8",
"N c #5D7DB0",
"3 c #C3D3EF",
"b c #476DAA",
"k c #ADC3E9",
"e c #E4ECF8",
"d c #4576C7",
"q c #C4D4EF",
"* c #CEDCF2",
"i c #90AFE1",
"4 c #ECF1FA",
"B c #4D72AE",
"% c #B8CCEC",
"8 c #759BDA",
"+ c #617FB1",
"1 c #9DB8E4",
"j c #9DB8E5",
"6 c #3762AA",
"w c #CFDDF2",
/* pixels */
"                ",
"                ",
"                ",
" ....           ",
" Xooo.          ",
" Xoooo.......   ",
" Xoooooooooo.   ",
" XoOOOOOOOOOOO+ ",
" Xo@#$%&*=-;:>+ ",
" X,<o1$23*=-45+ ",
" 6789o#02qw=e+  ",
" rty8ui#p%&*a+  ",
" sdfg8hojk2&+   ",
" lzxcvbnmMN+B   ",
"                ",
"                "
};

/* File */
/* Copyright (c) Julian Smart */
static const char * icon3_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 29 1",
"$ c #7198D9",
"5 c #DCE6F6",
"< c #FFFFFF",
"= c #9AB6E4",
"6 c #EAF0FA",
"1 c #6992D7",
"9 c #5886D2",
"3 c #F7F9FD",
"8 c #F0F5FC",
"* c #A8C0E8",
"  c None",
"0 c #FDFEFF",
"% c #C4D5F0",
"2 c #E2EAF8",
"O c #4377CD",
"o c #487BCE",
": c #6B94D7",
"- c #89A9DF",
"; c #5584D1",
"@ c #3569BF",
"+ c #3A70CA",
"> c #D2DFF4",
"# c #2E5CA8",
"7 c #FAFCFE",
"4 c #F5F8FD",
", c #638ED5",
"X c #5282D0",
"& c #B8CCEC",
". c #376EC9",
/* pixels */
"  ..XoO+@#$     ",
"  .%%&*=-;:;    ",
"  .>>%&*=,<=X   ",
"  $%%%%&*1<<=X  ",
"  $2-----,oXO+  ",
"  $2345>%&*=-+  ",
"  $2--------=+  ",
"  $244625>%&*O  ",
"  $2--------&o  ",
"  $27348625>%X  ",
"  $2-------->9  ",
"  $2<073486259  ",
"  $2--------2,  ",
"  $2<<<0734861  ",
"  $$$$$$$$$$1$  ",
"                "
};

/* Computer */
/* Copyright (c) Julian Smart */
static const char * icon4_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 42 1",
"r c #4E7FD0",
"$ c #7198D9",
"; c #DCE6F6",
"q c #FFFFFF",
"u c #4A7CCE",
"# c #779DDB",
"w c #95B2E3",
"y c #7FA2DD",
"f c #3263B4",
"= c #EAF0FA",
"< c #B1C7EB",
"% c #6992D7",
"9 c #D9E4F5",
"o c #9BB7E5",
"6 c #F7F9FD",
", c #BED0EE",
"3 c #F0F5FC",
"1 c #A8C0E8",
"  c None",
"0 c #FDFEFF",
"4 c #C4D5F0",
"@ c #81A4DD",
"e c #4377CD",
"- c #E2EAF8",
"i c #9FB9E5",
"> c #CCDAF2",
"+ c #89A9DF",
"s c #5584D1",
"t c #5D89D3",
": c #D2DFF4",
"5 c #FAFCFE",
"2 c #F5F8FD",
"8 c #DFE8F7",
"& c #5E8AD4",
"X c #638ED5",
"a c #CEDCF2",
"p c #90AFE2",
"d c #2F5DA9",
"* c #5282D0",
"7 c #E5EDF9",
". c #A2BCE6",
"O c #8CACE0",
/* pixels */
"                ",
"  .XXXXXXXXXXX  ",
"  oXO++@#$%&*X  ",
"  oX=-;:>,<1%X  ",
"  oX23=-;:4,$X  ",
"  oX5633789:@X  ",
"  oX05623=78+X  ",
"  oXqq05623=OX  ",
"  oX,,,,,<<<$X  ",
"  wXXXXXXXXXXe  ",
"  XrtX%$$y@+O,, ",
"  uyiiiiiiiii@< ",
" ouiiiiiiiiiip<a",
" rustX%$$y@+Ow,,",
" dfffffffffffffd",
"                "
};

/* Drive */
/* Copyright (c) Julian Smart */
static const char * icon5_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 37 1",
"o c #4E7FD0",
"= c #B9CDED",
"0 c #DCE6F6",
"@ c #295193",
"1 c #FFFFFF",
"* c #C6D6F0",
"O c #4A7CCE",
"e c #B0C6EA",
"3 c #95B2E3",
"r c #2D59A3",
"6 c #B1C7EB",
"$ c #D9E4F5",
"i c #3060AE",
"y c #264C8A",
"% c #214279",
"; c #E1E9F7",
"7 c #F0F5FC",
"9 c #A8C0E8",
"> c #3161B1",
"  c None",
"- c #B5C9EB",
"X c #487BCE",
", c #3A70CA",
": c #3569BF",
"q c #305FAC",
"8 c #A5BEE7",
"< c #5D89D3",
"4 c #D2DFF4",
". c #3366B9",
"+ c #4075CC",
"2 c #638ED5",
"# c #3467BC",
"t c #2F5DA9",
"5 c #D6E1F4",
"u c #070D17",
"w c #A2BCE6",
"& c #CFDDF3",
/* pixels */
"                ",
"   .XoooXO+@    ",
"  #$$%%%%$$$X   ",
"  #$&**=-$$$#   ",
" #;;=+o:>,<1o2  ",
" #33+45--56,1,  ",
" #1o17&89&70q1. ",
"##1+17&oo&70o1. ",
"#1%wo45--56,e%1.",
"#11146wwww61111o",
"r>>>>>>>>>>>>>>t",
">**>;;;;;;;;;;;>",
">**>;;;;;;;;yu;>",
"i**#;;;;;;;;;;;>",
">>>>>>>>>>>>>>>>",
"                "
};

/* CD-ROM */
/* Copyright (c) Julian Smart */
static const char *icon6_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 35 1",
"$ c #EDF2FB",
"O c #4E7FD0",
". c #7198D9",
"q c #DCE6F6",
"y c #2E5BA6",
"# c #FFFFFF",
"& c #7FA2DD",
"6 c #B1C7EB",
"X c #356AC1",
"* c #9BB7E5",
"; c #85A7DF",
"1 c #ADC4E9",
"9 c #CBD9F1",
"3 c #3161B1",
"  c None",
"- c #487BCE",
": c #9FB9E5",
"4 c #AEC5EA",
"7 c #89A9DF",
"@ c #98B5E4",
"8 c #5584D1",
"r c #3569BF",
"% c #3A70CA",
"t c #305FAC",
", c #5D89D3",
"w c #D2DFF4",
"o c #3366B9",
"e c #A6BFE8",
"+ c #638ED5",
"2 c #90AFE2",
"0 c #3467BC",
"< c #2F5DA9",
"> c #B3C8EB",
"= c #A2BCE6",
"5 c #8CACE0",
/* pixels */
"     .XooOo     ",
"    X+@###$o    ",
"   %&*=#####o   ",
"   O@=*######-  ",
"  o;:>*,<,#1*o  ",
"  o2:**<#<11*%  ",
"  o;**#,3,14*X  ",
"  o5#####16=7%  ",
"   X$####1=*X   ",
"   8%9###*7X0o  ",
"  Xqw+O33X3,&,o ",
" Xe61e:11111,,%3",
" rr000oo333tyyyo",
" r:###########:o",
" XXXr000oo333tty",
"                "
};

/* Floppy */
/* Copyright (c) Julian Smart */
static const char * icon7_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 17 1",
": c #DCE6F6",
"+ c #FFFFFF",
"= c #C6D6F0",
"> c #2C58A0",
". c #2D59A3",
"- c #284F90",
"; c #2B569D",
"O c #214279",
"$ c #1C3866",
"# c #BED0EE",
"  c None",
"X c #162C50",
"@ c #638ED5",
"o c #3467BC",
"& c #D6E1F4",
"% c #3C72CA",
"* c #87A8DF",
/* pixels */
"   .XoooooooXO  ",
"   .o+++++++.O  ",
"   .o+OOOOO+.O  ",
"   .o+++++++.O  ",
"   .o@@@@@@@.O  ",
"   ..........O  ",
"   ..#+++++#.O  ",
"   ..+$O+++#.O  ",
"   ..+$O+++#.O  ",
"  %&.........*% ",
" %=+++++++++++&%",
" --------------;",
" -:::::::::::::-",
" -:X:XXXXXXXXX:>",
" ---------------",
"                "
};

/* Removeable (same as drive for now) */
/* Copyright (c) Julian Smart */
static const char * icon8_xpm[] = {
/* columns rows colors chars-per-pixel */
"16 16 37 1",
"o c #4E7FD0",
"= c #B9CDED",
"0 c #DCE6F6",
"@ c #295193",
"1 c #FFFFFF",
"* c #C6D6F0",
"O c #4A7CCE",
"e c #B0C6EA",
"3 c #95B2E3",
"r c #2D59A3",
"6 c #B1C7EB",
"$ c #D9E4F5",
"i c #3060AE",
"y c #264C8A",
"% c #214279",
"; c #E1E9F7",
"7 c #F0F5FC",
"9 c #A8C0E8",
"> c #3161B1",
"  c None",
"- c #B5C9EB",
"X c #487BCE",
", c #3A70CA",
": c #3569BF",
"q c #305FAC",
"8 c #A5BEE7",
"< c #5D89D3",
"4 c #D2DFF4",
". c #3366B9",
"+ c #4075CC",
"2 c #638ED5",
"# c #3467BC",
"t c #2F5DA9",
"5 c #D6E1F4",
"u c #070D17",
"w c #A2BCE6",
"& c #CFDDF3",
/* pixels */
"                ",
"   .XoooXO+@    ",
"  #$$%%%%$$$X   ",
"  #$&**=-$$$#   ",
" #;;=+o:>,<1o2  ",
" #33+45--56,1,  ",
" #1o17&89&70q1. ",
"##1+17&oo&70o1. ",
"#1%wo45--56,e%1.",
"#11146wwww61111o",
"r>>>>>>>>>>>>>>t",
">**>;;;;;;;;;;;>",
">**>;;;;;;;;yu;>",
"i**#;;;;;;;;;;;>",
">>>>>>>>>>>>>>>>",
"                "
};


#if defined(__DOS__)

bool wxIsDriveAvailable(const wxString& dirName)
{
    // FIXME_MGL - this method leads to hang up under Watcom for some reason
#ifndef __WATCOMC__
    if ( dirName.Len() == 3 && dirName[1u] == wxT(':') )
    {
        wxString dirNameLower(dirName.Lower());
        // VS: always return TRUE for removable media, since Win95 doesn't
        //     like it when MS-DOS app accesses empty floppy drive
        return (dirNameLower[0u] == wxT('a') ||
                dirNameLower[0u] == wxT('b') ||
                wxPathExists(dirNameLower));
    }
    else
#endif
        return TRUE;
}

#elif defined(__WINDOWS__) || defined(__OS2__)

int setdrive(int drive)
{
#if defined(__GNUWIN32__) && \
    (defined(__MINGW32_MAJOR_VERSION) && __MINGW32_MAJOR_VERSION >= 1)
    return _chdrive(drive);
#else
	wxChar  newdrive[4];

	if (drive < 1 || drive > 31)
		return -1;
	newdrive[0] = (wxChar)(wxT('A') + drive - 1);
	newdrive[1] = wxT(':');
#ifdef __OS2__
	newdrive[2] = wxT('\\');
	newdrive[3] = wxT('\0');
#else
	newdrive[2] = wxT('\0');
#endif
#if defined(__WXMSW__)
#ifdef __WIN16__
    if (wxSetWorkingDirectory(newdrive))
#else
	if (::SetCurrentDirectory(newdrive))
#endif
#else
    // VA doesn't know what LPSTR is and has its own set
	if (!DosSetCurrentDir((PSZ)newdrive))
#endif
		return 0;
	else
		return -1;
#endif // !GNUWIN32
}

bool wxIsDriveAvailable(const wxString& dirName)
{
#ifdef __WIN32__
    UINT errorMode = SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
#endif
    bool success = TRUE;

    // Check if this is a root directory and if so,
    // whether the drive is avaiable.
    if (dirName.Len() == 3 && dirName[(size_t)1] == wxT(':'))
    {
        wxString dirNameLower(dirName.Lower());
#if defined(__WXWINE__) || (defined(__GNUWIN32__) && \
    !(defined(__MINGW32_MAJOR_VERSION) && __MINGW32_MAJOR_VERSION >= 1))
        success = wxPathExists(dirNameLower);
#else
        #if defined(__OS2__)
        // Avoid changing to drive since no media may be inserted.
        if (dirNameLower[(size_t)0] == 'a' || dirNameLower[(size_t)0] == 'b')
            return success;
        #endif
        int currentDrive = _getdrive();
        int thisDrive = (int) (dirNameLower[(size_t)0] - 'a' + 1) ;
        int err = setdrive( thisDrive ) ;
        setdrive( currentDrive );

        if (err == -1)
        {
            success = FALSE;
        }
#endif
    }
#ifdef __WIN32__
    (void) SetErrorMode(errorMode);
#endif

    return success;
}
#endif // __WINDOWS__ || __OS2__


// Function which is called by quick sort. We want to override the default wxArrayString behaviour,
// and sort regardless of case.
static int LINKAGEMODE wxDirCtrlStringCompareFunction(const void *first, const void *second)
{
    wxString *strFirst = (wxString *)first;
    wxString *strSecond = (wxString *)second;

    return strFirst->CmpNoCase(*strSecond);
}

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

wxDirItemData::wxDirItemData(const wxString& path, const wxString& name,
                             bool isDir)
{
    m_path = path;
    m_name = name;
    /* Insert logic to detect hidden files here
     * In UnixLand we just check whether the first char is a dot
     * For FileNameFromPath read LastDirNameInThisPath ;-) */
    // m_isHidden = (bool)(wxFileNameFromPath(*m_path)[0] == '.');
    m_isHidden = FALSE;
    m_isExpanded = FALSE;
    m_isDir = isDir;
}

wxDirItemData::~wxDirItemData()
{
}

void wxDirItemData::SetNewDirName(const wxString& path)
{
    m_path = path;
    m_name = wxFileNameFromPath(path);
}

bool wxDirItemData::HasSubDirs() const
{
    if (m_path.IsEmpty())
        return FALSE;

    wxDir dir;
    {
        wxLogNull nolog;
        if ( !dir.Open(m_path) )
            return FALSE;
    }

    return dir.HasSubDirs();
}

bool wxDirItemData::HasFiles(const wxString& WXUNUSED(spec)) const
{
    if (m_path.IsEmpty())
        return FALSE;

    wxDir dir;
    {
        wxLogNull nolog;
        if ( !dir.Open(m_path) )
            return FALSE;
    }

    return dir.HasFiles();
}

//-----------------------------------------------------------------------------
// wxGenericDirCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGenericDirCtrl, wxControl)

BEGIN_EVENT_TABLE(wxGenericDirCtrl, wxControl)
  EVT_TREE_ITEM_EXPANDING     (wxID_TREECTRL, wxGenericDirCtrl::OnExpandItem)
  EVT_TREE_ITEM_COLLAPSED     (wxID_TREECTRL, wxGenericDirCtrl::OnCollapseItem)
  EVT_TREE_BEGIN_LABEL_EDIT   (wxID_TREECTRL, wxGenericDirCtrl::OnBeginEditItem)
  EVT_TREE_END_LABEL_EDIT     (wxID_TREECTRL, wxGenericDirCtrl::OnEndEditItem)
  EVT_SIZE                    (wxGenericDirCtrl::OnSize)
END_EVENT_TABLE()

wxGenericDirCtrl::wxGenericDirCtrl(void)
{
    Init();
}

bool wxGenericDirCtrl::Create(wxWindow *parent,
                              const wxWindowID id,
                              const wxString& dir,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxString& filter,
                              int defaultFilter,
                              const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style, wxDefaultValidator, name))
        return FALSE;

    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

    Init();

    long treeStyle = wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT;

    if (style & wxDIRCTRL_EDIT_LABELS)
        treeStyle |= wxTR_EDIT_LABELS;

    if ((style & wxDIRCTRL_3D_INTERNAL) == 0)
        treeStyle |= wxNO_BORDER;
    else
        treeStyle |= wxBORDER_SUNKEN;

    long filterStyle = 0;
    if ((style & wxDIRCTRL_3D_INTERNAL) == 0)
        filterStyle |= wxNO_BORDER;
    else
        filterStyle |= wxBORDER_SUNKEN;

    m_treeCtrl = new wxTreeCtrl(this, wxID_TREECTRL, pos, size, treeStyle);

    if (!filter.IsEmpty() && (style & wxDIRCTRL_SHOW_FILTERS))
        m_filterListCtrl = new wxDirFilterListCtrl(this, wxID_FILTERLISTCTRL, wxDefaultPosition, wxDefaultSize, filterStyle);

    m_defaultPath = dir;
    m_filter = filter;

    SetFilterIndex(defaultFilter);

    if (m_filterListCtrl)
        m_filterListCtrl->FillFilterList(filter, defaultFilter);

    m_imageList = new wxImageList(16, 16, TRUE);
    m_imageList->Add(wxIcon(icon1_xpm));
    m_imageList->Add(wxIcon(icon2_xpm));
    m_imageList->Add(wxIcon(icon3_xpm));
    m_imageList->Add(wxIcon(icon4_xpm));
    m_imageList->Add(wxIcon(icon5_xpm));
    m_imageList->Add(wxIcon(icon6_xpm));
    m_imageList->Add(wxIcon(icon7_xpm));
    m_imageList->Add(wxIcon(icon8_xpm));
    m_treeCtrl->AssignImageList(m_imageList);

    m_showHidden = FALSE;
    wxDirItemData* rootData = new wxDirItemData(wxT(""), wxT(""), TRUE);

    wxString rootName;

#if defined(__WINDOWS__) || defined(__OS2__) || defined(__DOS__)
    rootName = _("Computer");
#else
    rootName = _("Sections");
#endif

    m_rootId = m_treeCtrl->AddRoot( rootName, 3, -1, rootData);
    m_treeCtrl->SetItemHasChildren(m_rootId);
    ExpandDir(m_rootId); // automatically expand first level

    // Expand and select the default path
    if (!m_defaultPath.IsEmpty())
        ExpandPath(m_defaultPath);

    DoResize();

    return TRUE;
}

wxGenericDirCtrl::~wxGenericDirCtrl()
{
}

void wxGenericDirCtrl::Init()
{
    m_showHidden = FALSE;
    m_imageList = NULL;
    m_currentFilter = 0;
    m_currentFilterStr = wxEmptyString; // Default: any file
    m_treeCtrl = NULL;
    m_filterListCtrl = NULL;
}

void wxGenericDirCtrl::ShowHidden( bool show )
{
    m_showHidden = show;

    wxString path = GetPath();
    ReCreateTree();
    SetPath(path);
}

void wxGenericDirCtrl::AddSection(const wxString& path, const wxString& name, int imageId)
{
    wxDirItemData *dir_item = new wxDirItemData(path,name,TRUE);

    wxTreeItemId id = m_treeCtrl->AppendItem( m_rootId, name, imageId, -1, dir_item);

    m_treeCtrl->SetItemHasChildren(id);
}

void wxGenericDirCtrl::SetupSections()
{
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)

#ifdef __WIN32__
    wxChar driveBuffer[256];
    size_t n = (size_t) GetLogicalDriveStrings(255, driveBuffer);
    size_t i = 0;
    while (i < n)
    {
        wxString path, name;
        path.Printf(wxT("%c:\\"), driveBuffer[i]);
        name.Printf(wxT("(%c:)"), driveBuffer[i]);

        int imageId = 4;
        int driveType = ::GetDriveType(path);
        switch (driveType)
        {
            case DRIVE_REMOVABLE:
                if (path == wxT("a:\\") || path == wxT("b:\\"))
                    imageId = 6; // Floppy
                else
                    imageId = 7;
                break;
            case DRIVE_FIXED:
                imageId = 4;
                break;
            case DRIVE_REMOTE:
                imageId = 4;
                break;
            case DRIVE_CDROM:
                imageId = 5;
                break;
            default:
                imageId = 4;
                break;
        }

        AddSection(path, name, imageId);

        while (driveBuffer[i] != wxT('\0'))
            i ++;
        i ++;
        if (driveBuffer[i] == wxT('\0'))
            break;
    }
#else // !__WIN32__
    int drive;

    /* If we can switch to the drive, it exists. */
    for( drive = 1; drive <= 26; drive++ )
    {
        wxString path, name;
        path.Printf(wxT("%c:\\"), (char) (drive + 'a' - 1));
        name.Printf(wxT("(%c:)"), (char) (drive + 'A' - 1));

        if (wxIsDriveAvailable(path))
        {
            AddSection(path, name, (drive <= 2) ? 6/*floppy*/ : 4/*disk*/);
        }
    }
#endif // __WIN32__/!__WIN32__

#elif defined(__WXMAC__)
    FSSpec volume ;
    short index = 1 ;
    while(1) {
      short actualCount = 0 ;
      if ( OnLine( &volume , 1 , &actualCount , &index ) != noErr || actualCount == 0 )
        break ;

      wxString name = wxMacFSSpec2MacFilename( &volume ) ;
      AddSection(name + wxFILE_SEP_PATH, name, 0);
    }
#elif defined(__UNIX__)
    AddSection(wxT("/"), wxT("/"), 3/*computer icon*/);
#else
    #error "Unsupported platform in wxGenericDirCtrl!"
#endif
}

void wxGenericDirCtrl::OnBeginEditItem(wxTreeEvent &event)
{
    // don't rename the main entry "Sections"
    if (event.GetItem() == m_rootId)
    {
        event.Veto();
        return;
    }

    // don't rename the individual sections
    if (m_treeCtrl->GetItemParent( event.GetItem() ) == m_rootId)
    {
        event.Veto();
        return;
    }
}

void wxGenericDirCtrl::OnEndEditItem(wxTreeEvent &event)
{
    if ((event.GetLabel().IsEmpty()) ||
        (event.GetLabel() == _(".")) ||
        (event.GetLabel() == _("..")) ||
        (event.GetLabel().First( wxT("/") ) != wxNOT_FOUND))
    {
        wxMessageDialog dialog(this, _("Illegal directory name."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
        return;
    }

    wxTreeItemId id = event.GetItem();
    wxDirItemData *data = (wxDirItemData*)m_treeCtrl->GetItemData( id );
    wxASSERT( data );

    wxString new_name( wxPathOnly( data->m_path ) );
    new_name += wxString(wxFILE_SEP_PATH);
    new_name += event.GetLabel();

    wxLogNull log;

    if (wxFileExists(new_name))
    {
        wxMessageDialog dialog(this, _("File name exists already."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
    }

    if (wxRenameFile(data->m_path,new_name))
    {
        data->SetNewDirName( new_name );
    }
    else
    {
        wxMessageDialog dialog(this, _("Operation not permitted."), _("Error"), wxOK | wxICON_ERROR );
        dialog.ShowModal();
        event.Veto();
    }
}

void wxGenericDirCtrl::OnExpandItem(wxTreeEvent &event)
{
    wxTreeItemId parentId = event.GetItem();

    // VS: this is needed because the event handler is called from wxTreeCtrl
    //     ctor when wxTR_HIDE_ROOT was specified
    if (!m_rootId.IsOk())
        m_rootId = m_treeCtrl->GetRootItem();

    ExpandDir(parentId);
}

void wxGenericDirCtrl::OnCollapseItem(wxTreeEvent &event )
{
    CollapseDir(event.GetItem());
}

void wxGenericDirCtrl::CollapseDir(wxTreeItemId parentId)
{
    wxTreeItemId child;

    wxDirItemData *data = (wxDirItemData *) m_treeCtrl->GetItemData(parentId);
    if (!data->m_isExpanded)
        return;

    data->m_isExpanded = FALSE;
    long cookie;
    /* Workaround because DeleteChildren has disapeared (why?) and
     * CollapseAndReset doesn't work as advertised (deletes parent too) */
    child = m_treeCtrl->GetFirstChild(parentId, cookie);
    while (child.IsOk())
    {
        m_treeCtrl->Delete(child);
        /* Not GetNextChild below, because the cookie mechanism can't
         * handle disappearing children! */
        child = m_treeCtrl->GetFirstChild(parentId, cookie);
    }
}

void wxGenericDirCtrl::ExpandDir(wxTreeItemId parentId)
{
    wxDirItemData *data = (wxDirItemData *) m_treeCtrl->GetItemData(parentId);

    if (data->m_isExpanded)
        return;

    data->m_isExpanded = TRUE;

    if (parentId == m_treeCtrl->GetRootItem())
    {
        SetupSections();
        return;
    }

    wxASSERT(data);

    wxString search,path,filename;

    wxString dirName(data->m_path);

#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)
    // Check if this is a root directory and if so,
    // whether the drive is avaiable.
    if (!wxIsDriveAvailable(dirName))
    {
        data->m_isExpanded = FALSE;
        //wxMessageBox(wxT("Sorry, this drive is not available."));
        return;
    }
#endif

    // This may take a longish time. Go to busy cursor
    wxBusyCursor busy;

#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)
    if (dirName.Last() == ':')
        dirName += wxString(wxFILE_SEP_PATH);
#endif

    wxArrayString dirs;
    wxArrayString filenames;

    wxDir d;
    wxString eachFilename;

    wxLogNull log;
    d.Open(dirName);

    if (d.IsOpened())
    {
        int style = wxDIR_DIRS;
        if (m_showHidden) style |= wxDIR_HIDDEN;
        if (d.GetFirst(& eachFilename, wxEmptyString, style))
        {
            do
            {
                if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                {
                    dirs.Add(eachFilename);
                }
            }
            while (d.GetNext(& eachFilename));
        }
    }
    dirs.Sort((wxArrayString::CompareFunction) wxDirCtrlStringCompareFunction);

    // Now do the filenames -- but only if we're allowed to
    if ((GetWindowStyle() & wxDIRCTRL_DIR_ONLY) == 0)
    {
        wxLogNull log;

        d.Open(dirName);

        if (d.IsOpened())
        {
            if (d.GetFirst(& eachFilename, m_currentFilterStr, wxDIR_FILES))
            {
                do
                {
                    if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                    {
                        filenames.Add(eachFilename);
                    }
                }
                while (d.GetNext(& eachFilename));
            }
        }
        filenames.Sort((wxArrayString::CompareFunction) wxDirCtrlStringCompareFunction);
    }

    // Add the sorted dirs
    size_t i;
    for (i = 0; i < dirs.Count(); i++)
    {
        wxString eachFilename(dirs[i]);
        path = dirName;
        if (path.Last() != wxFILE_SEP_PATH)
            path += wxString(wxFILE_SEP_PATH);
        path += eachFilename;

        wxDirItemData *dir_item = new wxDirItemData(path,eachFilename,TRUE);
        wxTreeItemId id = m_treeCtrl->AppendItem( parentId, eachFilename, 0, -1, dir_item);
        m_treeCtrl->SetItemImage( id, 1, wxTreeItemIcon_Expanded );

        // Has this got any children? If so, make it expandable.
        // (There are two situations when a dir has children: either it
        // has subdirectories or it contains files that weren't filtered
        // out. The latter only applies to dirctrl with files.)
        if ( dir_item->HasSubDirs() ||
             (((GetWindowStyle() & wxDIRCTRL_DIR_ONLY) == 0) &&
               dir_item->HasFiles(m_currentFilterStr)) )
        {
            m_treeCtrl->SetItemHasChildren(id);
        }
    }

    // Add the sorted filenames
    if ((GetWindowStyle() & wxDIRCTRL_DIR_ONLY) == 0)
    {
        for (i = 0; i < filenames.Count(); i++)
        {
            wxString eachFilename(filenames[i]);
            path = dirName;
            if (path.Last() != wxFILE_SEP_PATH)
                path += wxString(wxFILE_SEP_PATH);
            path += eachFilename;
            //path = dirName + wxString(wxT("/")) + eachFilename;
            wxDirItemData *dir_item = new wxDirItemData(path,eachFilename,FALSE);
            (void)m_treeCtrl->AppendItem( parentId, eachFilename, 2, -1, dir_item);
        }
    }
}

void wxGenericDirCtrl::ReCreateTree()
{
    CollapseDir(m_treeCtrl->GetRootItem());
    ExpandDir(m_treeCtrl->GetRootItem());
}

// Find the child that matches the first part of 'path'.
// E.g. if a child path is "/usr" and 'path' is "/usr/include"
// then the child for /usr is returned.
wxTreeItemId wxGenericDirCtrl::FindChild(wxTreeItemId parentId, const wxString& path, bool& done)
{
    wxString path2(path);

    // Make sure all separators are as per the current platform
    path2.Replace(wxT("\\"), wxString(wxFILE_SEP_PATH));
    path2.Replace(wxT("/"), wxString(wxFILE_SEP_PATH));

    // Append a separator to foil bogus substring matching
    path2 += wxString(wxFILE_SEP_PATH);

    // In MSW or PM, case is not significant
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)
    path2.MakeLower();
#endif

    long cookie;
    wxTreeItemId childId = m_treeCtrl->GetFirstChild(parentId, cookie);
    while (childId.IsOk())
    {
        wxDirItemData* data = (wxDirItemData*) m_treeCtrl->GetItemData(childId);

        if (data && !data->m_path.IsEmpty())
        {
            wxString childPath(data->m_path);
            if (childPath.Last() != wxFILE_SEP_PATH)
                childPath += wxString(wxFILE_SEP_PATH);

            // In MSW and PM, case is not significant
#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)
            childPath.MakeLower();
#endif

            if (childPath.Len() <= path2.Len())
            {
                wxString path3 = path2.Mid(0, childPath.Len());
                if (childPath == path3)
                {
                    if (path3.Len() == path2.Len())
                        done = TRUE;
                    else
                        done = FALSE;
                    return childId;
                }
            }
        }

        childId = m_treeCtrl->GetNextChild(parentId, cookie);
    }
    wxTreeItemId invalid;
    return invalid;
}

// Try to expand as much of the given path as possible,
// and select the given tree item.
bool wxGenericDirCtrl::ExpandPath(const wxString& path)
{
    bool done = FALSE;
    wxTreeItemId id = FindChild(m_rootId, path, done);
    wxTreeItemId lastId = id; // The last non-zero id
    while (id.IsOk() && !done)
    {
        ExpandDir(id);

        id = FindChild(id, path, done);
        if (id.IsOk())
            lastId = id;
    }
    if (lastId.IsOk())
    {
        wxDirItemData *data = (wxDirItemData *) m_treeCtrl->GetItemData(lastId);
        if (data->m_isDir)
        {
            m_treeCtrl->Expand(lastId);
        }
        if ((GetWindowStyle() & wxDIRCTRL_SELECT_FIRST) && data->m_isDir)
        {
            // Find the first file in this directory
            long cookie;
            wxTreeItemId childId = m_treeCtrl->GetFirstChild(lastId, cookie);
            bool selectedChild = FALSE;
            while (childId.IsOk())
            {
                wxDirItemData* data = (wxDirItemData*) m_treeCtrl->GetItemData(childId);

                if (data && data->m_path != wxT("") && !data->m_isDir)
                {
                    m_treeCtrl->SelectItem(childId);
                    m_treeCtrl->EnsureVisible(childId);
                    selectedChild = TRUE;
                    break;
                }
                childId = m_treeCtrl->GetNextChild(lastId, cookie);
            }
            if (!selectedChild)
            {
                m_treeCtrl->SelectItem(lastId);
                m_treeCtrl->EnsureVisible(lastId);
            }
        }
        else
        {
            m_treeCtrl->SelectItem(lastId);
            m_treeCtrl->EnsureVisible(lastId);
        }

        return TRUE;
    }
    else
        return FALSE;
}

wxString wxGenericDirCtrl::GetPath() const
{
    wxTreeItemId id = m_treeCtrl->GetSelection();
    if (id)
    {
        wxDirItemData* data = (wxDirItemData*) m_treeCtrl->GetItemData(id);
        return data->m_path;
    }
    else
        return wxEmptyString;
}

wxString wxGenericDirCtrl::GetFilePath() const
{
    wxTreeItemId id = m_treeCtrl->GetSelection();
    if (id)
    {
        wxDirItemData* data = (wxDirItemData*) m_treeCtrl->GetItemData(id);
        if (data->m_isDir)
            return wxEmptyString;
        else
            return data->m_path;
    }
    else
        return wxEmptyString;
}

void wxGenericDirCtrl::SetPath(const wxString& path)
{
    m_defaultPath = path;
    if (m_rootId)
        ExpandPath(path);
}

// Not used
#if 0
void wxGenericDirCtrl::FindChildFiles(wxTreeItemId id, int dirFlags, wxArrayString& filenames)
{
    wxDirItemData *data = (wxDirItemData *) m_treeCtrl->GetItemData(id);

    // This may take a longish time. Go to busy cursor
    wxBusyCursor busy;

    wxASSERT(data);

    wxString search,path,filename;

    wxString dirName(data->m_path);

#if defined(__WXMSW__) || defined(__OS2__)
    if (dirName.Last() == ':')
        dirName += wxString(wxFILE_SEP_PATH);
#endif

    wxDir d;
    wxString eachFilename;

    wxLogNull log;
    d.Open(dirName);

    if (d.IsOpened())
    {
        if (d.GetFirst(& eachFilename, m_currentFilterStr, dirFlags))
        {
            do
            {
                if ((eachFilename != wxT(".")) && (eachFilename != wxT("..")))
                {
                    filenames.Add(eachFilename);
                }
            }
            while (d.GetNext(& eachFilename)) ;
        }
    }
}
#endif

void wxGenericDirCtrl::SetFilterIndex(int n)
{
    m_currentFilter = n;

    wxString f, d;
    if (ExtractWildcard(m_filter, n, f, d))
        m_currentFilterStr = f;
    else
        m_currentFilterStr = wxT("*.*");
}

void wxGenericDirCtrl::SetFilter(const wxString& filter)
{
    m_filter = filter;

    wxString f, d;
    if (ExtractWildcard(m_filter, m_currentFilter, f, d))
        m_currentFilterStr = f;
    else
        m_currentFilterStr = wxT("*.*");
}

// Extract description and actual filter from overall filter string
bool wxGenericDirCtrl::ExtractWildcard(const wxString& filterStr, int n, wxString& filter, wxString& description)
{
    wxArrayString filters, descriptions;
    int count = ParseFilter(filterStr, filters, descriptions);
    if (count > 0 && n < count)
    {
        filter = filters[n];
        description = descriptions[n];
        return TRUE;
    }

    return FALSE;
}

// Parses the global filter, returning the number of filters.
// Returns 0 if none or if there's a problem.
// filterStr is in the form:
//
// "All files (*.*)|*.*|JPEG Files (*.jpeg)|*.jpg"

int wxGenericDirCtrl::ParseFilter(const wxString& filterStr, wxArrayString& filters, wxArrayString& descriptions)
{
    wxString str(filterStr);

    wxString description, filter;
    int pos;
    bool finished = FALSE;
    do
    {
        pos = str.Find(wxT('|'));
        if (pos == -1)
            return 0; // Problem
        description = str.Left(pos);
        str = str.Mid(pos+1);
        pos = str.Find(wxT('|'));
        if (pos == -1)
        {
            filter = str;
            finished = TRUE;
        }
        else
        {
            filter = str.Left(pos);
            str = str.Mid(pos+1);
        }
        descriptions.Add(description);
        filters.Add(filter);
    }
    while (!finished) ;

    return filters.Count();
}

void wxGenericDirCtrl::DoResize()
{
    wxSize sz = GetClientSize();
    int verticalSpacing = 3;
    if (m_treeCtrl)
    {
        wxSize filterSz ;
        if (m_filterListCtrl)
        {
#ifdef __WXMSW__
            // For some reason, this is required in order for the
            // correct control height to always be returned, rather
            // than the drop-down list height which is sometimes returned.
            wxSize oldSize = m_filterListCtrl->GetSize();
            m_filterListCtrl->SetSize(-1, -1, oldSize.x+10, -1, wxSIZE_USE_EXISTING);
            m_filterListCtrl->SetSize(-1, -1, oldSize.x, -1, wxSIZE_USE_EXISTING);
#endif
            filterSz = m_filterListCtrl->GetSize();
            sz.y -= (filterSz.y + verticalSpacing);
        }
        m_treeCtrl->SetSize(0, 0, sz.x, sz.y);
        if (m_filterListCtrl)
        {
            m_filterListCtrl->SetSize(0, sz.y + verticalSpacing, sz.x, filterSz.y);
            // Don't know why, but this needs refreshing after a resize (wxMSW)
            m_filterListCtrl->Refresh();
        }
    }
}


void wxGenericDirCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    DoResize();
}

//-----------------------------------------------------------------------------
// wxDirFilterListCtrl
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxDirFilterListCtrl, wxChoice)

BEGIN_EVENT_TABLE(wxDirFilterListCtrl, wxChoice)
    EVT_CHOICE(-1, wxDirFilterListCtrl::OnSelFilter)
END_EVENT_TABLE()

bool wxDirFilterListCtrl::Create(wxGenericDirCtrl* parent, const wxWindowID id,
              const wxPoint& pos,
              const wxSize& size,
              long style)
{
    m_dirCtrl = parent;
    return wxChoice::Create(parent, id, pos, size, 0, NULL, style);
}

void wxDirFilterListCtrl::Init()
{
    m_dirCtrl = NULL;
}

void wxDirFilterListCtrl::OnSelFilter(wxCommandEvent& WXUNUSED(event))
{
    int sel = GetSelection();

    wxString currentPath = m_dirCtrl->GetPath();

    m_dirCtrl->SetFilterIndex(sel);

    // If the filter has changed, the view is out of date, so
    // collapse the tree.
    m_dirCtrl->ReCreateTree();

    // Try to restore the selection, or at least the directory
    m_dirCtrl->ExpandPath(currentPath);
}

void wxDirFilterListCtrl::FillFilterList(const wxString& filter, int defaultFilter)
{
    Clear();
    wxArrayString descriptions, filters;
    size_t n = (size_t) m_dirCtrl->ParseFilter(filter, filters, descriptions);

    if (n > 0 && defaultFilter < (int) n)
    {
        size_t i = 0;
        for (i = 0; i < n; i++)
            Append(descriptions[i]);
        SetSelection(defaultFilter);
    }
}

#endif // wxUSE_DIRDLG
