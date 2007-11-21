/////////////////////////////////////////////////////////////////////////////
// Name:        rtfutils.h
// Purpose:     RTF-specific code
// Author:      Julian Smart
// Modified by:
// Created:     7.9.93
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

 /*
  * Write a suitable RTF header.
  *
  */
  
void WriteRTFHeader(FILE *fd);

/*
 * Given a TexChunk with a string value, scans through the string
 * converting Latex-isms into RTF-isms, such as 2 newlines -> \par,
 * and inserting spaces at the start of lines since in Latex, a newline
 * implies a space, but not in RTF.
 *
 */
 
void ProcessText2RTF(TexChunk *chunk);

/*
 * Scan through all chunks starting from the given one,
 * calling ProcessText2RTF to convert Latex-isms to RTF-isms.
 * This should be called after Tex2Any has parsed the file,
 * and before TraverseDocument is called.
 *
 */
 
void Text2RTF(TexChunk *chunk);


/*
 * Keeping track of environments to restore the styles after \pard.
 * Push strings like "\qc" onto stack.
 *
 */

void PushEnvironmentStyle(wxChar *style);

void PopEnvironmentStyle(void);

// Write out the styles, most recent first.
void WriteEnvironmentStyles(void);

// Called on start/end of macro examination
void DefaultRtfOnMacro(wxChar *name, int no_args, bool start);

// Called on start/end of argument examination
bool DefaultRtfOnArgument(wxChar *macro_name, int arg_no, bool start);

// Reset memory of which levels have 'books' (for WinHelp 4 contents file)
void ResetContentsLevels(int level);
