/* rexxhelp.c */

#if defined(API_DOC) && defined(OS2DLL)

#define UNZIP_INTERNAL
#include "../unzip.h"
#include "../version.h"

APIDocStruct REXXDetails[] = {
  { "UZDROPFUNCS"  , "UZDropFuncs"  ,
               "call UZDropFuncs",
               "Use this function to drop all the loaded UnZip functions.\n"
"\t\tOnce this function is processed by a REXX program, the\n"
"\t\tUnZip functions are not accessible in any OS/2 sessions.\n" },

  { "UZLOADFUNCS"  , "UZLoadFuncs"  ,
               "call UZLoadFuncs",
               "Use this function to make all of the UnZip functions\n"
"		in this package available to all OS/2 sessions.\n\n"
"  Example:	call RxFuncAdd 'UZLoadFuncs', 'UNZIPAPI', 'UZLoadFuncs'\n"
"		call UZLoadFuncs\n" },

  { "UZFILETREE"   , "UZFileTree"   ,
               "rc = UZFileTree(zipfile, stem, [include], [exclude], [options])\n\n"
"	zipfile	- Name of ZIP file to search\n"
"	stem	- Name of the stem variable for results\n"
"		  Note: stem.0 contains the number of files found.\n"
"	include - Optional stem variable specifying a list of files (including\n"
"		  wildcards) to include.  stem.0 must indicate number of items.\n"
"	exclude - Optional stem variable specifying a list of files (including\n"
"		  wildcards) to exclude.  stem.0 must indicate number of items.\n"
"	    NOTE: If lists are not needed, filespec strings may be passed.\n"
"	options - One of the following:\n"
"		  'O' - Give file names only.  This is the default.\n"
"		  'F' - Give file statistics in the form:\n"
"			Length Date Time Name\n"
"		  'Z' - Also give ZIP statistics in the form:\n"
"			Length Method Size Ratio Date Time CRC-32 Name",
               "Finds all files in the specified ZIP with the specified\n"
"		filespec and places their descriptions in a stem variable.\n\n"
"	rc:	Return codes\n"
"		0	Successful\n"
"		2	Error.  Not enough memory.\n\n"
"	Examples:\n"
"		/* Return a list of all .NDX files in pcboard.qwk */\n"
"		rc = UZFileTree('pcboard.qwk', 'stem.', '*.ndx')\n\n"
"		/* Return a list of all files except *.NDX and *.DAT */\n"
"		exc.0 = 2; exc.1 = '*.ndx'; exc.2 = '*.dat'\n"
"		rc = UZFileTree('pcboard.qwk', 'stem.',,'exc.')\n" },

  { "UZUNZIP"      , "UZUnZip"      ,
               "rc = UZUnZip('parameters', [stem])\n\n"
"	parameters	- The entire list of parameters you would use from\n"
"			  the command-line\n"
"	stem		- The name of an optional stem variable where any\n"
"			  output should be redirected.\n"
"			  NOTE: If a stem is not specified, all output will\n"
"				go to the console.",
               "Provide a direct entry point to the command line interface.\n\n"
"	rc:	UnZip return code\n\n"
"  Examples:	/* Test the archive 'unzip51s.zip' and return output in stem.*/\n"
"		rc = UZUnZip('-t unzip51s.zip','stem.')\n"
"		/* Extract the archive, display output on screen */\n"
"		call UZUnZip 'doom.zip'\n"
"		/* Extract all .NDX files from the archive */\n"
"		call UZUnZip 'pcboard.qwk *.ndx','stem.'\n" },

  { "UZUNZIPTOVAR" , "UZUnZipToVar" ,
               "rc = UZUnZipToVar('zipfile', 'filename', [stem])\n\n"
"	zipfile	 - Name of ZIP file to search\n"
"	filename - Name of file to extract from zipfile\n"
"	stem	 - Optional stem variable to extract the file to.\n"
"		   If you specify a stem variable, the file will be extracted\n"
"		   to the variable, one line per index, stem.0 containing a\n"
"		   line count.  In this case, 0 will be returned in rc.\n"
"		   If NO stem variable is specified, the entire file will be\n"
"		   extracted to rc.",
               "Unzip one file to a variable.\n\n"
"	rc:	If no stem variable is specified, rc contains the contents of\n"
"		the extracted file if successful or an error-code if not.\n"
"		If a stem variable IS specified, rc contains 0 if successful.\n"},

  /* GRR:  "include" and "exclude" used to be identified as stem variables
   *       (Daniel H bug report)
   */
  { "UZUNZIPTOSTEM", "UZUnZipToStem",
               "rc = UZUnZipToStem(zipfile, stem, [include], [exclude], [mode])\n"
"	zipfile	- Name of ZIP file to search\n"
"	stem	- Stem variable used to store the extracted files\n"
"	include - Optional string variable specifying a list of files (including\n"
"		  wildcards) to include.  stem.0 must indicate number of items.\n"
"	exclude - Optional string variable specifying a list of files (including\n"
"		  wildcards) to exclude.  stem.0 must indicate number of items.\n"
"	    NOTE: If lists are not needed, filespec strings may be passed.\n"
"	mode	- Optional mode parameter specifies either 'F'lat (the default)\n"
"		  or 'T'ree mode.\n"
"		-- In flat mode, each file is stored in stem.fullname i.e.\n"
"		   stem.os2/dll/unzipapi.c.  A list of files is created in\n"
"		   stem.<index>\n"
"		-- In tree mode, slashes are converted to periods in the\n"
"		   pathname thus the above file would have been stored in\n"
"		   stem.OS2.DLL.unzipapi.c and an index stored for each\n"
"		   directory, i.e. stem.OS2.DLL.<index> = \"unzipapi.c\",\n"
"		   stem.OS2.<index> = \"DLL/\", stem.<index> = \"OS2/\"",
               "Unzip files to a stem variable.\n\n"
"	Example:	Assuming a file unzip.zip containing:\n"
"			  unzip.c, unshrink.c, extract.c,\n"
"			  os2/makefile.os2, os2/os2.c\n"
"			  os2/dll/dll.def, os2/dll/unzipapi.c\n\n"
"		rc = UZUnZipToStem('unzip.zip', 'stem.')\n"
"		Returns:	stem.0 = 7\n"
"				stem.1 = unzip.c\n"
"				stem.2 = unshrink.c\n"
"				stem.3 = extract.c\n"
"				stem.4 = os2/makefile.os2\n"
"				stem.5 = os2/os2.c\n"
"				stem.6 = os2/dll/dll.def\n"
"				stem.7 = os2/dll/unzipapi.c\n"
"			And the following contain the contents of the\n"
"			various files:\n"
"				stem.unzip.c\n"
"				stem.unshrink.c\n"
"				stem.extract.c\n"
"				stem.os2/makefile.os2\n"
"				stem.os2/os2.c\n"
"				stem.os2/dll/dll.def\n"
"				stem.os2/dll/unzipapi.c\n\n"
"		rc = UZUnZipToStem('unzip.zip', 'stem.',,,'TREE')\n"
"		Returns:	stem.0 = 4\n"
"				stem.1 = unzip.c\n"
"				stem.2 = unshrink.c\n"
"				stem.3 = extract.c\n"
"				stem.4 = OS2/\n"
"				stem.OS2.0 = 3\n"
"				stem.OS2.1 = makefile.os2\n"
"				stem.OS2.2 = os2.c\n"
"				stem.OS2.3 = DLL/\n"
"				stem.OS2.DLL.0 = 2\n"
"				stem.OS2.DLL.1 = def\n"
"				stem.OS2.DLL.2 = unzipapi.c\n"
"\n"
"			And the following contain the contents of the\n"
"			various programs:\n"
"				stem.unzip.c\n"
"				stem.unshrink.c\n"
"				stem.extract.c\n"
"				stem.OS2.makefile.os2\n"
"				stem.OS2.os2.c\n"
"				stem.OS2.DLL.dll.def\n"
"				stem.OS2.DLL.unzipapi.c\n" },

  { "UZVER"        , "UZVer"        ,
               "rc = UZVer([option])\n\n"
"	rc	String containing UnZip version info in the form 'x.xx'\n"
"		If option is 'L' then info is in the form 'x.xx of <date>",
               "Returns the version number of UnZip\n" },

  { "UZAPIVER"     , "UZAPIVer"     ,
               "rc = UZAPIVer([option])\n\n"
"	rc	String containing API version info in the form 'x.xx'\n"
"		If option is 'L' then info is in the form 'x.xx of <date>",
               "Returns the version number of the API\n" },
  { 0 }
};

char *REXXBrief = "\
REXX functions:\n\
  UZDropFuncs     -- Makes all functions in this package unknown to REXX\n\
  UZLoadFuncs     -- Makes all functions in this package known to REXX\n\
  UZFileTree      -- Searches for files matching a given filespec\n\
  UZUnZip	  -- UnZip command-line entry point\n\
  UZUnZipToVar    -- Unzip one file to a variable\n\
  UZUnZipToStem   -- Unzip files to a variable array\n\
  UZVer           -- Returns the UnZip version number\n\
  UZAPIVer        -- Returns the API version number\n";


#endif /* API_DOC && OS2DLL */
