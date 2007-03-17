/////////////////////////////////////////////////////////////////////////////
// Name:        tex2any.h
// Purpose:     Latex conversion header
// Author:      Julian Smart
// Modified by:
// Created:     7.9.93
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "wx/utils.h"
#include "wx/list.h"
#include "wx/hash.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/txtstrm.h"
#include "wxhlpblk.h"

/*
 * Conversion modes
 *
 */

#define TEX_RTF  1
#define TEX_XLP  2
#define TEX_HTML 3

/*
 * We have a list of macro definitions which we must define
 * in advance to enable the parsing to recognize macros.
 */

#define FORBID_OK         0
#define FORBID_WARN       1
#define FORBID_ABSOLUTELY 2


#ifdef __WXMSW__
  const unsigned long MAX_LINE_BUFFER_SIZE = 600;
#else
  const unsigned long MAX_LINE_BUFFER_SIZE = 11000;
#endif

class TexMacroDef: public wxObject
{
 public:
  int no_args;
  wxChar *name;
  bool ignore;
  int forbidden;
  int macroId;

  TexMacroDef(int the_id, const wxChar *the_name, int n, bool ig, bool forbidLevel = FORBID_OK);
  virtual ~TexMacroDef(void);
};

#define CHUNK_TYPE_MACRO    1
#define CHUNK_TYPE_ARG      2
#define CHUNK_TYPE_STRING   3

/*
 We have nested lists to represent the Tex document.
 Each element of a list of chunks can be one of:
  - a plain string
  - a macro with/without arguments. Arguments are lists of TexChunks.

Example (\toplevel is implicit but made explicit here):

AddMacroDef(ltMYMAT, "mymat", 2);

\toplevel{The cat sat on the \mymat{very coarse and {\it cheap}}{mat}}.

Parsed as:

TexChunk: type = macro, name = toplevel, no_args = 1
  Children:

    TexChunk: type = argument

      Children:
        TexChunk: type = string, value = "The cat sat on the "
        TexChunk: type = macro, name = mymat, no_args = 2

          Children:
            TexChunk: type = argument

              Children:
                TexChunk: type = string, value = "very coarse and "
                TexChunk: type = macro, name = it, no_args = 1

                  Children:
                    TexChunk: type = argument

                      Children:
                        TexChunk: type = string, value = "cheap"

            TexChunk: type = argument

              Children:
                TexChunk: type = string, value = mat
 */

class TexChunk
{
 public:
  int type;
//  char *name;
  TexMacroDef *def;
  wxChar *value;
  int macroId;
  int no_args;
  int argn;
  bool optional;      // Is an optional argument

  wxList children;
  TexChunk(int the_type, TexMacroDef *the_def = NULL);
  TexChunk(TexChunk& toCopy);
  virtual ~TexChunk(void);
};

// Represents a topic, used for generating a table of contents file (.cnt).
// Also for storing keywords found in a topic, a list of which is then inserted
// into the topic in the next pass.
class TexTopic: public wxObject
{
 public:
  // This flag is set to indicate that the topic has children.
  // If this is the case, we know to insert a 'book' icon at this level,
  // not just a 'page' icon. We don't want to have to open a book only
  // to find there's only one page in it. We might force a book to be used if
  // a top-level topic has no children (?)
  bool hasChildren;
  wxChar *filename;
  wxStringList *keywords;
  TexTopic(wxChar *f = NULL);
  virtual ~TexTopic(void);
};
extern wxHashTable TopicTable;
void AddKeyWordForTopic(wxChar *topic, wxChar *entry, wxChar *filename = NULL);
void ClearKeyWordTable(void);

extern wxChar wxTex2RTFBuffer[];
extern TexChunk     *TopLevel;
extern wxHashTable  MacroDefs;
extern wxStringList IgnorableInputFiles; // Ignorable \input files, e.g. psbox.tex

bool read_a_line(wxChar *buf);
bool TexLoadFile(const wxString& filename);
int ParseArg(TexChunk *thisArg, wxList& children, wxChar *buffer, int pos,
           wxChar *environment = NULL, bool parseArgToBrace = true, TexChunk *customMacroArgs = NULL);
int ParseMacroBody(const wxChar *macro_name, TexChunk *parent, int no_args,
           wxChar *buffer, int pos, wxChar *environment = NULL, bool parseArgToBrace = true, TexChunk *customMacroArgs = NULL);
void TraverseDocument(void);
void TraverseFromChunk(TexChunk *chunk, wxNode *thisNode = NULL, bool childrenOnly = false);
#define TraverseChildrenFromChunk(arg) TraverseFromChunk(arg, NULL, true)
void SetCurrentOutput(FILE *fd);
void SetCurrentOutputs(FILE *fd1, FILE *fd2);
extern FILE *CurrentOutput1;
extern FILE *CurrentOutput2;
void AddMacroDef(int the_id, const wxChar *name, int n, bool ignore = false, bool forbidden = false);
void TexInitialize(int bufSize);
void TexCleanUp(void);
void TexOutput(const wxChar *s, bool ordinaryText = false);
wxChar *GetArgData(TexChunk *chunk);
wxChar *GetArgData(void);             // Get the string for the current argument
int GetNoArgs(void);                // Get the number of arguments for the current macro
TexChunk *GetArgChunk(void);        // Get the chunk for the current argument
TexChunk *GetTopLevelChunk(void);   // Get the chunk for the top level
TexChunk *GetNextChunk(void);       // Look ahead to the next chunk
bool IsArgOptional(void);           // Is this argument an optional argument?
void DefineDefaultMacros(void);     // Optional set of default macros
int GetCurrentColumn(void);         // number of characters on current line
wxChar *ConvertCase(wxChar *s);         // Convert case, according to upperCaseNames setting.
extern wxPathList TexPathList;      // Path list, can be used for file searching.

extern bool StringMatch(const wxChar *one, const wxChar *two, bool subString = true, bool exact = false);

// Define a variable value from the .ini file
wxChar *RegisterSetting(const wxString& settingName, const wxString& settingValue, bool interactive = true);

// Major document styles
#define LATEX_REPORT    1
#define LATEX_ARTICLE   2
#define LATEX_LETTER    3
#define LATEX_BOOK      4
#define LATEX_SLIDES    5

extern TexChunk *DocumentTitle;
extern TexChunk *DocumentAuthor;
extern TexChunk *DocumentDate;
extern int DocumentStyle;
extern int MinorDocumentStyle;
extern wxChar *BibliographyStyleString;
extern wxChar *DocumentStyleString;
extern wxChar *MinorDocumentStyleString;

extern int normalFont;
extern int smallFont;
extern int tinyFont;
extern int largeFont1;
extern int LargeFont2;
extern int LARGEFont3;
extern int hugeFont1;
extern int HugeFont2;
extern int HUGEFont3;

/*
 * USER-ADJUSTABLE SETTINGS
 *
 */

// Section font sizes
extern int chapterFont;
extern int sectionFont;
extern int subsectionFont;
extern int titleFont;
extern int authorFont;
extern bool winHelp;  // Output in Windows Help format if true, linear otherwise
extern bool isInteractive;
extern bool runTwice;
extern int convertMode;
extern bool checkCurlyBraces;
extern bool checkSyntax;
extern bool stopRunning;
extern int  mirrorMargins;
extern bool headerRule;
extern bool footerRule;
extern int labelIndentTab;  // From left indent to item label (points)
extern int itemIndentTab;   // From left indent to item (points)
extern bool useUpButton;
extern int htmlBrowseButtons;
extern bool useHeadingStyles; // Insert \s1, s2 etc.
extern bool useWord; // Insert Word table of contents, etc. etc.
extern bool indexSubsections; // put subsections in index
extern bool compatibilityMode;
extern bool generateHPJ;      // Generate WinHelp HPJ file
extern wxChar *winHelpTitle;    // Title for Windows Help file
extern int defaultTableColumnWidth;
extern wxChar *bitmapMethod;
extern bool truncateFilenames; // Truncate for DOS
extern int  winHelpVersion;    // Version e.g. 4 for Win95
extern bool winHelpContents;   // Generate .cnt file
extern bool htmlIndex;         // Generate .htx HTML index file
extern bool htmlFrameContents; // Use frames for HTML contents page
extern wxChar *htmlStylesheet; // Use this CSS stylesheet for HTML pages
extern int  contentsDepth;     // Depth of contents for linear RTF files
extern bool upperCaseNames;    // Filenames; default is lower case
extern wxChar *backgroundImageString; // HTML background image
extern wxChar *backgroundColourString; // HTML background colour
extern wxChar *textColourString; // HTML text colour
extern wxChar *linkColourString; // HTML link colour
extern wxChar *followedLinkColourString; // HTML followed link colour
extern bool combineSubSections; // Stop splitting files below section
extern bool htmlWorkshopFiles;  // generate HTML Help Workshop project files
extern bool ignoreBadRefs;      // Don't insert (REF NOT FOUND)
extern wxChar *htmlFaceName;      // HTML face name

// Names to help with internationalisation
extern wxChar *ContentsNameString;
extern wxChar *AbstractNameString;
extern wxChar *GlossaryNameString;
extern wxChar *ReferencesNameString;
extern wxChar *FiguresNameString;
extern wxChar *TablesNameString;
extern wxChar *FigureNameString;
extern wxChar *TableNameString;
extern wxChar *IndexNameString;
extern wxChar *ChapterNameString;
extern wxChar *SectionNameString;
extern wxChar *SubsectionNameString;
extern wxChar *SubsubsectionNameString;
extern wxChar *UpNameString;

/*
 * HTML button identifiers: what kind of browse buttons
 * are placed in HTML files, if any.
 *
 */

#define HTML_BUTTONS_NONE       0
#define HTML_BUTTONS_BITMAP     1
#define HTML_BUTTONS_TEXT       2

/*
 * Section numbering
 *
 */

extern int chapterNo;
extern int sectionNo;
extern int subsectionNo;
extern int subsubsectionNo;
extern int figureNo;
extern int tableNo;

extern int ParSkip;
extern int ParIndent;

extern bool isSync;

// Set by client and by Tex2Any
extern TexChunk *currentSection;

// Header/footers/pagestyle
extern TexChunk *      LeftHeaderOdd;
extern TexChunk *      LeftFooterOdd;
extern TexChunk *      CentreHeaderOdd;
extern TexChunk *      CentreFooterOdd;
extern TexChunk *      RightHeaderOdd;
extern TexChunk *      RightFooterOdd;
extern TexChunk *      LeftHeaderEven;
extern TexChunk *      LeftFooterEven;
extern TexChunk *      CentreHeaderEven;
extern TexChunk *      CentreFooterEven;
extern TexChunk *      RightHeaderEven;
extern TexChunk *      RightFooterEven;
extern wxChar *        PageStyle;

// Repeat the currentSection, either real (Chapter) or simulated (References)
extern void OutputCurrentSection(void);
extern void OutputCurrentSectionToString(wxChar *buf);
extern void OutputChunkToString(TexChunk *chunk, wxChar *buf);

// Called by Tex2Any to simulate a section
extern void FakeCurrentSection(wxChar *fakeSection, bool addToContents = true);

/*
 * Local to Tex2Any library
 *
 */

extern wxChar *currentArgData;
extern bool haveArgData; // If true, we're simulating the data.
void StartSimulateArgument(wxChar *data);
void EndSimulateArgument(void);

/*
 * Client-defined
 *
 */

// Called on start/end of macro examination
void OnMacro(int macroId, int no_args, bool start);

// Called on start/end of argument examination.
// Return true at the start of an argument to traverse
// (output) the argument.
bool OnArgument(int macroId, int arg_no, bool start);

// Default: library-defined
void DefaultOnMacro(int macroId, int no_args, bool start);

// Default: library-defined
bool DefaultOnArgument(int macroId, int arg_no, bool start);

// Called on error
void OnError(const wxString& msg);

// Called for information
void OnInform(const wxString& msg);

// Special yield wrapper
void Tex2RTFYield(bool force = false);

/*
 * Useful utilities
 *
 */

// Look for \label macro, use this ref name if found or
// make up a topic name otherwise.
wxChar *FindTopicName(TexChunk *chunk);
// Force the current topic to be this (e.g. force 'references' label).
void ForceTopicName(const wxChar *name);
void ResetTopicCounter(void);

// Parse unit eg. 14, 12pt, 34cm and return value in points.
int ParseUnitArgument(const wxChar *unitArg);

// Set small, large, normal etc. point sizes for reference size
void SetFontSizes(int pointSize);

/*
 * Strip off any extension (dot something) from end of file,
 * IF one exists. Inserts zero into buffer.
 *
 */

void StripExtension(wxChar *buffer);

/*
 * Reference structure
 *
 */

class TexRef: public wxObject
{
 public:
  wxChar *refLabel;      // Reference label
  wxChar *refFile;       // Reference filename (can be NULL)
  wxChar *sectionNumber; // Section or figure number (as a string)
  wxChar *sectionName; // name e.g. 'section'
  TexRef(const wxChar *label, const wxChar *file, const wxChar *section, const wxChar *sectionN = NULL);
  virtual ~TexRef(void);
};

/*
 * Add a reference
 *
 */

void AddTexRef(wxChar *name, wxChar *file = NULL, wxChar *sectionName = NULL,
         int chapter = 0, int section = 0, int subsection = 0, int subsubsection = 0);

/*
 * Read and write reference file (.ref), to resolve refs for second pass.
 *
 */
void WriteTexReferences(wxChar *filename);
void ReadTexReferences(wxChar *filename);

/*
 * Bibliography stuff
 *
 */

class BibEntry: public wxObject
{
 public:
  wxChar *key;

  /*
   * book, inbook, article, phdthesis, inproceedings, techreport
   */
  wxChar *type;

  /*
   * Possible fields
   *
   */
  wxChar *editor;
  wxChar *title;
  wxChar *booktitle;
  wxChar *author;
  wxChar *journal;
  wxChar *volume;
  wxChar *number;
  wxChar *year;
  wxChar *month;
  wxChar *pages;
  wxChar *chapter;
  wxChar *publisher;
  wxChar *address;
  wxChar *institution;
  wxChar *organization;
  wxChar *comment;

  inline BibEntry(void)
  {
    key = NULL;
    type = NULL;
    editor = NULL;
    title = NULL;
    booktitle = NULL;
    author = NULL;
    journal = NULL;
    volume = NULL;
    number = NULL;
    chapter = NULL;
    year = NULL;
    month = NULL;
    pages = NULL;
    publisher = NULL;
    address = NULL;
    institution = NULL;
    organization = NULL;
    comment = NULL;
  }
};

extern wxList BibList;
extern wxStringList CitationList;

bool ReadBib(const wxChar *filename);
void OutputBib(void);
void ResolveBibReferences(void);
void AddCitation(const wxChar *citeKey);
TexRef *FindReference(const wxChar *key);

/*
 * Ability to customize, or at least suppress unknown macro errors
 *
 */

extern wxList CustomMacroList;

#define CUSTOM_MACRO_IGNORE 0
#define CUSTOM_MACRO_OUTPUT 1
#define CUSTOM_MACRO_MARK   2

class CustomMacro: public wxObject
{
public:
    wxChar *macroName;
    wxChar *macroBody;
    int noArgs;
    inline CustomMacro(const wxChar *name, int args, wxChar *body)
    {
        noArgs = args;
        macroName = wxStrcpy(new wxChar[wxStrlen(name) + 1], name);
        if (body)
            macroBody = wxStrcpy(new wxChar[wxStrlen(body) + 1], body);
        else
            macroBody = NULL;
    }
    virtual ~CustomMacro();
};

bool ReadCustomMacros(const wxString& filename);
void ShowCustomMacros(void);
CustomMacro *FindCustomMacro(wxChar *name);
wxChar *ParseMultifieldString(wxChar *s, int *pos);

/*
 * Colour table stuff
 *
 */

class ColourTableEntry: public wxObject
{
 public:
  wxChar *name;
  unsigned int red;
  unsigned int green;
  unsigned int blue;

  ColourTableEntry(const wxChar *theName, unsigned int r,  unsigned int g,  unsigned int b);
  virtual ~ColourTableEntry(void);
};

extern wxList ColourTable;
extern void AddColour(const wxChar *theName, unsigned int r,  unsigned int g,  unsigned int b);
extern int FindColourPosition(wxChar *theName);
// Converts e.g. "red" -> "#FF0000"
extern bool FindColourHTMLString(wxChar *theName, wxChar *buf);
extern void InitialiseColourTable(void);

#define ltABSTRACT          1
#define ltADDCONTENTSLINE   2
#define ltADDTOCOUNTER      3
#define ltALPH1             4
#define ltALPH2             5
#define ltAPPENDIX          6
#define ltARABIC            7
#define ltARRAY             8
#define ltAUTHOR            9

#define ltBACKSLASH         30
#define ltBASELINESKIP      31
#define ltBF                32
#define ltBIBITEM           33
#define ltBIBLIOGRAPHYSTYLE 34
#define ltBIBLIOGRAPHY      35
#define ltBOXIT             36
#define ltBACKSLASHRAW      37
#define ltBACKGROUND        38
#define ltBACKGROUNDCOLOUR  39
#define ltBACKGROUNDIMAGE   40
#define ltBRCLEAR           41

#define ltCAPTIONSTAR       50
#define ltCAPTION           51
#define ltCDOTS             52
#define ltCENTERLINE        53
#define ltCENTERING         54
#define ltCENTER            55
#define ltCEXTRACT          56
#define ltCHAPTERHEADING    57
#define ltCHAPTERSTAR       58
#define ltCHAPTER           59
#define ltCINSERT           60
#define ltCITE              61
#define ltCLASS             62
#define ltCLEARDOUBLEPAGE   63
#define ltCLEARPAGE         64
#define ltCLINE             65
#define ltCLIPSFUNC         66
#define ltCOLUMNSEP         67
#define ltCOMMENT           68
#define ltCOPYRIGHT         69
#define ltCPARAM            70

#define ltCHEAD             71
#define ltCFOOT             72

#define ltCHAPTERHEADINGSTAR 73

#define ltDATE              90
#define ltDESCRIPTION       91
#define ltDESTRUCT          92
#define ltDOCUMENTSTYLE     93
#define ltDOCUMENT          94
#define ltDOUBLESPACE       95
#define ltDEFINECOLOUR      96
#define ltDEFINECOLOR       97

#define ltEM                120
#define ltENUMERATE         121
#define ltEQUATION          122
#define ltEVENSIDEMARGIN    123

#define ltFBOX              150
#define ltFIGURE            151
#define ltFLUSHLEFT         152
#define ltFLUSHRIGHT        153
#define ltFOOTHEIGHT        154
#define ltFOOTNOTE          155
#define ltFOOTSKIP          156
#define ltFRAMEBOX          157
#define ltFUNCTIONSECTION   158
#define ltFUNC              159
#define ltFIGURESTAR        160
#define ltFOOTNOTESIZE      161
#define ltFOOTNOTEPOPUP     162
#define ltFANCYPLAIN        163
#define ltFCOL              164
#define ltBCOL              165
#define ltFOLLOWEDLINKCOLOUR 166

#define ltGLOSSARY          180
#define ltGLOSS             181

#define ltHEADHEIGHT        200
#define ltHELPGLOSSARY      201
#define ltHELPIGNORE        202
#define ltHELPONLY          203
#define ltHELPINPUT         204
#define ltHELPFONTFAMILY    205
#define ltHELPFONTSIZE      206
#define ltHELPREFN          207
#define ltHELPREF           208
#define ltHFILL             209
#define ltHLINE             210
#define ltHRULE             211
#define ltHSPACESTAR        212
#define ltHSPACE            213
#define ltHSKIPSTAR         214
#define ltHSKIP             215
#define lthuge              216
#define ltHuge              217
#define ltHUGE              218
#define ltHTMLIGNORE        219
#define ltHTMLONLY          220

#define ltINCLUDEONLY       240
#define ltINCLUDE           241
#define ltINDEX             242
#define ltINPUT             243
#define ltITEMIZE           244
#define ltITEM              245
#define ltIMAGE             246
#define ltIT                247
#define ltITEMSEP           248
#define ltINDENTED          249
#define ltIMAGEMAP          250
#define ltIMAGER            251
#define ltIMAGEL            252
#define ltINSERTATLEVEL     253

#define ltKILL              260

#define ltLABEL             280
#define ltlarge             281
#define ltLarge             282
#define ltLARGE             283
#define ltLATEX             284
#define ltLBOX              285
#define ltLDOTS             286
#define ltLINEBREAK         287
#define ltLISTOFFIGURES     288
#define ltLISTOFTABLES      289
#define ltLHEAD             290
#define ltLFOOT             291
#define ltLATEXIGNORE       292
#define ltLATEXONLY         293
#define ltLOWERCASE         294
#define ltLBRACERAW         295
#define ltLINKCOLOUR        296

#define ltMAKEGLOSSARY      300
#define ltMAKEINDEX         301
#define ltMAKETITLE         302
#define ltMARKRIGHT         303
#define ltMARKBOTH          304
#define ltMARGINPARWIDTH    305
#define ltMARGINPAR         306
#define ltMARGINPARODD      307
#define ltMARGINPAREVEN     308
#define ltMBOX              309
#define ltMEMBERSECTION     310
#define ltMEMBER            311
#define ltMULTICOLUMN       312
#define ltMARGINPARSEP      313

#define ltNEWCOUNTER        330
#define ltNEWLINE           331
#define ltNEWPAGE           332
#define ltNOCITE            333
#define ltNOINDENT          334
#define ltNOLINEBREAK       335
#define ltNOPAGEBREAK       336
#define ltNORMALSIZE        337
#define ltNORMALBOX         338
#define ltNORMALBOXD        339
#define ltNUMBEREDBIBITEM   340

#define ltONECOLUMN         360
#define ltODDSIDEMARGIN     361

#define ltPAGEBREAK         380
#define ltPAGEREF           381
#define ltPAGESTYLE         382
#define ltPAGENUMBERING     383
#define ltPARAGRAPHSTAR     384
#define ltPARAGRAPH         385
#define ltPARAM             386
#define ltPARINDENT         387
#define ltPARSKIP           388
#define ltPARTSTAR          389
#define ltPART              390
#define ltPAR               391
#define ltPFUNC             392
#define ltPICTURE           393
#define ltPOPREF            394
#define ltPOUNDS            395
#define ltPRINTINDEX        396
#define ltPSBOXTO           397
#define ltPSBOX             398
#define ltPOPREFONLY        399

#define ltQUOTE             420
#define ltQUOTATION         421

#define ltRAGGEDBOTTOM      440
#define ltRAGGEDLEFT        441
#define ltRAGGEDRIGHT       442
#define ltREF               443
#define ltRM                444
#define ltROMAN             445
#define ltROMAN2            446
#define ltRTFSP             447
#define ltRULE              448
#define ltRULEDROW          449
#define ltDRULED            450
#define ltRHEAD             451
#define ltRFOOT             452
#define ltROW               453
#define ltRTFIGNORE         454
#define ltRTFONLY           455
#define ltRBRACERAW         456
#define ltREGISTERED        457

#define ltSC                470
#define ltSECTIONHEADING    471
#define ltSECTIONSTAR       472
#define ltSECTION           473
#define ltSETCOUNTER        474
#define ltSF                475
#define ltSHORTCITE         476
#define ltSINGLESPACE       477
#define ltSLOPPYPAR         478
#define ltSLOPPY            479
#define ltSL                480
#define ltSMALL             481
#define ltSUBITEM           482
#define ltSUBPARAGRAPHSTAR  483
#define ltSUBPARAGRAPH      484
#define ltSPECIAL           485
#define ltSUBSECTIONSTAR    486
#define ltSUBSECTION        487
#define ltSUBSUBSECTIONSTAR 488
#define ltSUBSUBSECTION     489
#define ltSCRIPTSIZE        490
#define ltSETHEADER         491
#define ltSETFOOTER         492
#define ltSIZEDBOX          493
#define ltSIZEDBOXD         494
#define ltSECTIONHEADINGSTAR 495
#define ltSS                496
#define ltSETHOTSPOTCOLOUR  497
#define ltSETHOTSPOTCOLOR   498
#define ltSETHOTSPOTUNDERLINE 499
#define ltSETTRANSPARENCY   500

#define ltTABBING           510
#define ltTABLEOFCONTENTS   511
#define ltTABLE             512
#define ltTABULAR           513
#define ltTAB               514
#define ltTEX               515
#define ltTEXTWIDTH         516
#define ltTEXTHEIGHT        517
#define ltTHEBIBLIOGRAPHY   518
#define ltTITLEPAGE         519
#define ltTITLE             520
#define ltTINY              521
#define ltTODAY             522
#define ltTOPMARGIN         523
#define ltTOPSKIP           524
#define ltTT                525
#define ltTYPEIN            526
#define ltTYPEOUT           527
#define ltTWOCOLUMN         528
#define ltTHEPAGE           529
#define ltTHECHAPTER        530
#define ltTHESECTION        531
#define ltTHISPAGESTYLE     532

#define ltTWOCOLWIDTHA      533
#define ltTWOCOLWIDTHB      534
#define ltTWOCOLSPACING     535
#define ltTWOCOLITEM        536
#define ltTWOCOLITEMRULED   537
#define ltTWOCOLLIST        538
#define ltTEXTCOLOUR        539

#define ltUNDERLINE         550
#define ltURLREF            551
#define ltUPPERCASE         552
#define ltUSEPACKAGE        553

#define ltVDOTS             570
#define ltVERBATIMINPUT     571
#define ltVERBATIM          572
#define ltVERB              573
#define ltVERSE             574
#define ltVFILL             575
#define ltVLINE             576
#define ltVOID              577
#define ltVRULE             578
#define ltVSPACESTAR        579
#define ltVSKIPSTAR         580
#define ltVSPACE            581
#define ltVSKIP             582
#define ltVERBSTAR          583

#define ltWXCLIPS           600
#define ltWINHELPIGNORE     601
#define ltWINHELPONLY       602

#define ltXLPIGNORE         603
#define ltXLPONLY           604

#define ltSPACE             620
#define ltBACKSLASHCHAR     621
#define ltPIPE              622
#define ltFORWARDSLASH      623
#define ltUNDERSCORE        624
#define ltAMPERSAND         625
#define ltPERCENT           626
#define ltDOLLAR            627
#define ltHASH              628
#define ltLPARENTH          629
#define ltRPARENTH          630
#define ltLBRACE            631
#define ltRBRACE            632
#define ltEQUALS            633
#define ltRANGLEBRA         634
#define ltLANGLEBRA         635
#define ltPLUS              636
#define ltDASH              637
#define ltSINGLEQUOTE       638
#define ltBACKQUOTE         639
#define ltTILDE             640
#define ltAT_SYMBOL         641

// Characters, not macros but with special Latex significance
#define ltSPECIALDOLLAR     660
#define ltSPECIALDOUBLEDOLLAR 661
#define ltSPECIALTILDE      662
#define ltSPECIALHASH       663
#define ltSPECIALAMPERSAND  664
#define ltSUPERTABULAR      665

// Accents
#define ltACCENT_GRAVE      700
#define ltACCENT_ACUTE      701
#define ltACCENT_CARET      702
#define ltACCENT_UMLAUT     703
#define ltACCENT_TILDE      704
#define ltACCENT_DOT        705
#define ltACCENT_CADILLA    706

// Symbols
#define ltALPHA             800
#define ltBETA              801
#define ltGAMMA             802
#define ltDELTA             803
#define ltEPSILON           804
#define ltVAREPSILON        805
#define ltZETA              806
#define ltETA               807
#define ltTHETA             808
#define ltVARTHETA          809
#define ltIOTA              810
#define ltKAPPA             811
#define ltLAMBDA            812
#define ltMU                813
#define ltNU                814
#define ltXI                815
#define ltPI                816
#define ltVARPI             817
#define ltRHO               818
#define ltVARRHO            819
#define ltSIGMA             820
#define ltVARSIGMA          821
#define ltTAU               822
#define ltUPSILON           823
#define ltPHI               824
#define ltVARPHI            825
#define ltCHI               826
#define ltPSI               827
#define ltOMEGA             828

#define ltCAP_GAMMA         830
#define ltCAP_DELTA         831
#define ltCAP_THETA         832
#define ltCAP_LAMBDA        833
#define ltCAP_XI            834
#define ltCAP_PI            835
#define ltCAP_SIGMA         836
#define ltCAP_UPSILON       837
#define ltCAP_PHI           838
#define ltCAP_PSI           839
#define ltCAP_OMEGA         840

// Binary operation symbols
#define ltLE                850
#define ltLEQ               851
#define ltLL                852
#define ltSUBSET            853
#define ltSUBSETEQ          854
#define ltSQSUBSET          855
#define ltSQSUBSETEQ        856
#define ltIN                857
#define ltVDASH             858
#define ltMODELS            859
#define ltGE                860
#define ltGEQ               861
#define ltGG                862
#define ltSUPSET            863
#define ltSUPSETEQ          864
#define ltSQSUPSET          865
#define ltSQSUPSETEQ        866
#define ltNI                867
#define ltDASHV             868
#define ltPERP              869
#define ltNEQ               870
#define ltDOTEQ             871
#define ltAPPROX            872
#define ltCONG              873
#define ltEQUIV             874
#define ltPROPTO            875
#define ltPREC              876
#define ltPRECEQ            877
#define ltPARALLEL          878
#define ltSIM               879
#define ltSIMEQ             880
#define ltASYMP             881
#define ltSMILE             882
#define ltFROWN             883
#define ltBOWTIE            884
#define ltSUCC              885
#define ltSUCCEQ            886
#define ltMID               887

// Negated relation symbols (selected)
#define ltNOTEQ             890
#define ltNOTIN             891
#define ltNOTSUBSET         892

// Arrows
#define ltLEFTARROW         900
#define ltLEFTARROW2        901
#define ltRIGHTARROW        902
#define ltRIGHTARROW2       903
#define ltLEFTRIGHTARROW    904
#define ltLEFTRIGHTARROW2   905
#define ltUPARROW           906
#define ltUPARROW2          907
#define ltDOWNARROW         908
#define ltDOWNARROW2        909

// Miscellaneous symbols
#define ltALEPH             1000
#define ltWP                1001
#define ltRE                1002
#define ltIM                1003
#define ltEMPTYSET          1004
#define ltNABLA             1005
#define ltSURD              1006
#define ltPARTIAL           1007
#define ltBOT               1008
#define ltFORALL            1009
#define ltEXISTS            1010
#define ltNEG               1011
#define ltSHARP             1012
#define ltANGLE             1013
#define ltTRIANGLE          1014
#define ltCLUBSUIT          1015
#define ltDIAMONDSUIT       1016
#define ltHEARTSUIT         1017
#define ltSPADESUIT         1018
#define ltINFTY             1019

// Binary operation symbols
#define ltPM                1030
#define ltMP                1031
#define ltTIMES             1032
#define ltDIV               1033
#define ltCDOT              1034
#define ltAST               1035
#define ltSTAR              1036
#define ltCAP               1037
#define ltCUP               1038
#define ltVEE               1039
#define ltWEDGE             1040
#define ltCIRC              1041
#define ltBULLET            1042
#define ltDIAMOND           1043
#define ltOSLASH            1044
#define ltBOX               1045
#define ltDIAMOND2          1046
#define ltBIGTRIANGLEDOWN   1047
#define ltOPLUS             1048
#define ltOTIMES            1049

// Latex2e commands
#define ltRMFAMILY          1200
#define ltSFFAMILY          1201
#define ltTTFAMILY          1202
#define ltBFSERIES          1203
#define ltITSHAPE           1204
#define ltSLSHAPE           1205
#define ltSCSHAPE           1206

#define ltMDSERIES          1207
#define ltUPSHAPE           1208

#define ltTEXTRM            1209
#define ltTEXTSF            1210
#define ltTEXTTT            1211
#define ltTEXTBF            1212
#define ltTEXTIT            1213
#define ltTEXTSL            1214
#define ltTEXTSC            1215
#define ltEMPH              1216

#define ltDOCUMENTCLASS     1217

// Space macros
#define ltSMALLSPACE1       1250
#define ltSMALLSPACE2       1251

// Pseudo-macros
#define ltTOPLEVEL          15000
#define ltCUSTOM_MACRO      15001
#define ltSOLO_BLOCK        15002
