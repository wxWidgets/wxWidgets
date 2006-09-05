/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ========================================================================
*
*    The contents of this file are subject to the wxWindows License
*    Version 3.0 (the "License"); you may not use this file except in
*    compliance with the License. You may obtain a copy of the License at
*    http://www.wxwindows.org/licence3.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*  ========================================================================
*
* Language:		ANSI C++
* Environment:	Any
*
* Description:  Header file for the Preprocessor of the #if SSI directive
* in wxHTML.
*
****************************************************************************/

#ifndef __WX_PREPIFELSE_H
#define __WX_PREPIFELSE_H

#include "wx/object.h"
#include "wx/html/htmlproc.h"

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
If Else Preprocessor class Definition
****************************************************************************/
class wxIfElsePrep : public wxHtmlProcessor {
private:
    //DECLARE_DYNAMIC_CLASS(wxIfElsePrep);

public:
    wxIfElsePrep() : wxHtmlProcessor() {}
    virtual ~wxIfElsePrep() {}
	
	// Process input text and return processed result
    wxString Process(const wxString& text) const;

    // Return priority value of this processor. The higher, the sooner
    // is the processor applied to the text.
    int GetPriority() const { return wxHTML_PRIORITY_SYSTEM-2; }
	};
	
	
#endif // __WX_PREPECHO_H

