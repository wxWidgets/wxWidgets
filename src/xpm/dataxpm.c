/*
 * Copyright (C) 1989-94 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*****************************************************************************\
* data.c:                                                                     *
*                                                                             *
*  XPM library                                                                *
*  IO utilities                                                               *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/* Official version number */
static char *RCS_Version = "$XpmVersion: 3.4b $";

/* Internal version number */
static char *RCS_Id = "$Id$";

#include "xpm34p.h"
#ifdef VMS
#include "sys$library:stat.h"
#include "sys$library:ctype.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#endif

#include <string.h>

LFUNC(ParseComment, int, (xpmData * mdata));

static int
ParseComment(xpmData *mdata)
{
    if (mdata->type == XPMBUFFER) {
	register char c;
	register unsigned int n = 0;
	unsigned int notend;
	char *s, *s2;

	s = mdata->Comment;
	*s = mdata->Bcmt[0];

	/* skip the string beginning comment */
	s2 = mdata->Bcmt;
	do {
	    c = *mdata->cptr++;
	    *++s = c;
	    n++;
	    s2++;
	} while (c == *s2 && *s2 != '\0' && c && c != mdata->Bos);

	if (*s2 != '\0') {
	    /* this wasn't the beginning of a comment */
	    mdata->cptr -= n;
	    return 0;
	}
	/* store comment */
	mdata->Comment[0] = *s;
	s = mdata->Comment;
	notend = 1;
	n = 0;
	while (notend) {
	    s2 = mdata->Ecmt;
	    while (*s != *s2 && c && c != mdata->Bos) {
		c = *mdata->cptr++;
		*++s = c;
		n++;
	    }
	    mdata->CommentLength = n;
	    do {
		c = *mdata->cptr++;
		n++;
		*++s = c;
		s2++;
	    } while (c == *s2 && *s2 != '\0' && c && c != mdata->Bos);
	    if (*s2 == '\0') {
		/* this is the end of the comment */
		notend = 0;
		mdata->cptr--;
	    }
	}
	return 0;
    } else {
	FILE *file = mdata->stream.file;
	register int c;
	register unsigned int n = 0, a;
	unsigned int notend;
	char *s, *s2;

	s = mdata->Comment;
	*s = mdata->Bcmt[0];

	/* skip the string beginning comment */
	s2 = mdata->Bcmt;
	do {
	    c = getc(file);
	    *++s = c;
	    n++;
	    s2++;
	} while (c == *s2 && *s2 != '\0'
		 && c != EOF && c != mdata->Bos);

	if (*s2 != '\0') {
	    /* this wasn't the beginning of a comment */
	    /* put characters back in the order that we got them */
	    for (a = n; a > 0; a--, s--)
		ungetc(*s, file);
	    return 0;
	}
	/* store comment */
	mdata->Comment[0] = *s;
	s = mdata->Comment;
	notend = 1;
	n = 0;
	while (notend) {
	    s2 = mdata->Ecmt;
	    while (*s != *s2 && c != EOF && c != mdata->Bos) {
		c = getc(file);
		*++s = c;
		n++;
	    }
	    mdata->CommentLength = n;
	    do {
		c = getc(file);
		n++;
		*++s = c;
		s2++;
	    } while (c == *s2 && *s2 != '\0'
		     && c != EOF && c != mdata->Bos);
	    if (*s2 == '\0') {
		/* this is the end of the comment */
		notend = 0;
		ungetc(*s, file);
	    }
	}
	return 0;
    }
}

/*
 * skip to the end of the current string and the beginning of the next one
 */
int
xpmNextString(xpmData *mdata)
{
    if (!mdata->type)
	mdata->cptr = (mdata->stream.data)[++mdata->line];
    else if (mdata->type == XPMBUFFER) {
	register char c;

	/* get to the end of the current string */
	if (mdata->Eos)
	    while ((c = *mdata->cptr++) && c != mdata->Eos);

	/*
	 * then get to the beginning of the next string looking for possible
	 * comment
	 */
	if (mdata->Bos) {
	    while ((c = *mdata->cptr++) && c != mdata->Bos)
		if (mdata->Bcmt && c == mdata->Bcmt[0])
		    ParseComment(mdata);
	} else if (mdata->Bcmt) {	/* XPM2 natural */
	    while ((c = *mdata->cptr++) == mdata->Bcmt[0])
		ParseComment(mdata);
	    mdata->cptr--;
	}
    } else {
	register int c;
	FILE *file = mdata->stream.file;

	/* get to the end of the current string */
	if (mdata->Eos)
	    while ((c = getc(file)) != mdata->Eos && c != EOF);

	/*
	 * then get to the beginning of the next string looking for possible
	 * comment
	 */
	if (mdata->Bos) {
	    while ((c = getc(file)) != mdata->Bos && c != EOF)
		if (mdata->Bcmt && c == mdata->Bcmt[0])
		    ParseComment(mdata);

	} else if (mdata->Bcmt) {	/* XPM2 natural */
	    while ((c = getc(file)) == mdata->Bcmt[0])
		ParseComment(mdata);
	    ungetc(c, file);
	}
    }
    return 0;
}


/*
 * skip whitespace and compute the following unsigned int,
 * returns 1 if one is found and 0 if not
 */
int
xpmNextUI(xpmData *mdata, unsigned int *ui_return)
{
    char buf[BUFSIZ];
    int l;

    l = xpmNextWord(mdata, buf, BUFSIZ);
    return atoui(buf, l, ui_return);
}

/*
 * skip whitespace and return the following word
 */
unsigned int
xpmNextWord(xpmData *mdata, char *buf, unsigned int buflen)
{
    register unsigned int n = 0;
    int c;

    if (!mdata->type || mdata->type == XPMBUFFER) {
	while (isspace(c = *mdata->cptr) && c != mdata->Eos)
	    mdata->cptr++;
	do {
	    c = *mdata->cptr++;
	    *buf++ = c;
	    n++;
	} while (!isspace(c) && c != mdata->Eos && n < buflen);
	n--;
	mdata->cptr--;
    } else {
	FILE *file = mdata->stream.file;

	while ((c = getc(file)) != EOF && isspace(c) && c != mdata->Eos);
	while (!isspace(c) && c != mdata->Eos && c != EOF && n < buflen) {
	    *buf++ = c;
	    n++;
	    c = getc(file);
	}
	ungetc(c, file);
    }
    return (n);
}

/*
 * return end of string - WARNING: malloc!
 */
int
xpmGetString(xpmData *mdata, char **sptr, unsigned int *l)
{
    unsigned int i, n = 0;
    int c;
    char *p, *q, buf[BUFSIZ];

    if (!mdata->type || mdata->type == XPMBUFFER) {
	if (mdata->cptr) {
	    char *start;

	    while (isspace(c = *mdata->cptr) && c != mdata->Eos)
		mdata->cptr++;
	    start = mdata->cptr;
	    while ((c = *mdata->cptr) && c != mdata->Eos)
		mdata->cptr++;
	    n = mdata->cptr - start + 1;
	    p = (char *) XpmMalloc(n);
	    if (!p)
		return (XpmNoMemory);
	    strncpy(p, start, n);
	    if (mdata->type)		/* XPMBUFFER */
		p[n - 1] = '\0';
	}
    } else {
	FILE *file = mdata->stream.file;

	while ((c = getc(file)) != EOF && isspace(c) && c != mdata->Eos);
	if (c == EOF)
	    return (XpmFileInvalid);
	p = NULL;
	i = 0;
	q = buf;
	p = (char *) XpmMalloc(1);
	while (c != mdata->Eos && c != EOF) {
	    if (i == BUFSIZ) {
		/* get to the end of the buffer */
		/* malloc needed memory */
		q = (char *) XpmRealloc(p, n + i);
		if (!q) {
		    XpmFree(p);
		    return (XpmNoMemory);
		}
		p = q;
		q += n;
		/* and copy what we already have */
		strncpy(q, buf, i);
		n += i;
		i = 0;
		q = buf;
	    }
	    *q++ = c;
	    i++;
	    c = getc(file);
	}
	if (c == EOF) {
	    XpmFree(p);
	    return (XpmFileInvalid);
	}
	if (n + i != 0) {
	    /* malloc needed memory */
	    q = (char *) XpmRealloc(p, n + i + 1);
	    if (!q) {
		XpmFree(p);
		return (XpmNoMemory);
	    }
	    p = q;
	    q += n;
	    /* and copy the buffer */
	    strncpy(q, buf, i);
	    n += i;
	    p[n++] = '\0';
	} else {
	    *p = '\0';
	    n = 1;
	}
	ungetc(c, file);
    }
    *sptr = p;
    *l = n;
    return (XpmSuccess);
}

/*
 * get the current comment line
 */
int
xpmGetCmt(xpmData *mdata, char **cmt)
{
    if (!mdata->type)
	*cmt = NULL;
    else if (mdata->CommentLength) {
	*cmt = (char *) XpmMalloc(mdata->CommentLength + 1);
	strncpy(*cmt, mdata->Comment, mdata->CommentLength);
	(*cmt)[mdata->CommentLength] = '\0';
	mdata->CommentLength = 0;
    } else
	*cmt = NULL;
    return 0;
}

/*
 * open the given file to be read as an xpmData which is returned.
 */
int
xpmReadFile(char *filename, xpmData *mdata)
{
#ifdef ZPIPE
    char *compressfile, buf[BUFSIZ];
    struct stat status;

#endif

    if (!filename) {
	mdata->stream.file = (stdin);
	mdata->type = XPMFILE;
    } else {
#ifdef ZPIPE
	if (((int) strlen(filename) > 2) &&
	    !strcmp(".Z", filename + (strlen(filename) - 2))) {
	    mdata->type = XPMPIPE;
	    sprintf(buf, "uncompress -c %s", filename);
	    if (!(mdata->stream.file = popen(buf, "r")))
		return (XpmOpenFailed);

	} else if (((int) strlen(filename) > 3) &&
		   !strcmp(".gz", filename + (strlen(filename) - 3))) {
	    mdata->type = XPMPIPE;
	    sprintf(buf, "gunzip -qc %s", filename);
	    if (!(mdata->stream.file = popen(buf, "r")))
		return (XpmOpenFailed);

	} else {
	    if (!(compressfile = (char *) XpmMalloc(strlen(filename) + 4)))
		return (XpmNoMemory);

	    strcpy(compressfile, filename);
	    strcat(compressfile, ".Z");
	    if (!stat(compressfile, &status)) {
		sprintf(buf, "uncompress -c %s", compressfile);
		if (!(mdata->stream.file = popen(buf, "r"))) {
		    XpmFree(compressfile);
		    return (XpmOpenFailed);
		}
		mdata->type = XPMPIPE;
	    } else {
		strcpy(compressfile, filename);
		strcat(compressfile, ".gz");
		if (!stat(compressfile, &status)) {
		    sprintf(buf, "gunzip -c %s", compressfile);
		    if (!(mdata->stream.file = popen(buf, "r"))) {
			XpmFree(compressfile);
			return (XpmOpenFailed);
		    }
		    mdata->type = XPMPIPE;
		} else {
#endif
		    if (!(mdata->stream.file = fopen(filename, "r"))) {
#ifdef ZPIPE
			XpmFree(compressfile);
#endif
			return (XpmOpenFailed);
		    }
		    mdata->type = XPMFILE;
#ifdef ZPIPE
		}
	    }
	    XpmFree(compressfile);
	}
#endif
    }
    mdata->CommentLength = 0;
    return (XpmSuccess);
}

/*
 * open the given file to be written as an xpmData which is returned
 */
int
xpmWriteFile(char *filename, xpmData *mdata)
{
#ifdef ZPIPE
    char buf[BUFSIZ];

#endif

    if (!filename) {
	mdata->stream.file = (stdout);
	mdata->type = XPMFILE;
    } else {
#ifdef ZPIPE
	if ((int) strlen(filename) > 2
	    && !strcmp(".Z", filename + (strlen(filename) - 2))) {
	    sprintf(buf, "compress > %s", filename);
	    if (!(mdata->stream.file = popen(buf, "w")))
		return (XpmOpenFailed);

	    mdata->type = XPMPIPE;
	} else if ((int) strlen(filename) > 3
		   && !strcmp(".gz", filename + (strlen(filename) - 3))) {
	    sprintf(buf, "gzip -q > %s", filename);
	    if (!(mdata->stream.file = popen(buf, "w")))
		return (XpmOpenFailed);

	    mdata->type = XPMPIPE;
	} else {
#endif
	    if (!(mdata->stream.file = fopen(filename, "w")))
		return (XpmOpenFailed);

	    mdata->type = XPMFILE;
#ifdef ZPIPE
	}
#endif
    }
    return (XpmSuccess);
}

/*
 * open the given array to be read or written as an xpmData which is returned
 */
void
xpmOpenArray(char **data, xpmData *mdata)
{
    mdata->type = XPMARRAY;
    mdata->stream.data = data;
    mdata->cptr = *data;
    mdata->line = 0;
    mdata->CommentLength = 0;
    mdata->Bcmt = mdata->Ecmt = NULL;
    mdata->Bos = mdata->Eos = '\0';
    mdata->format = 0;			/* this can only be Xpm 2 or 3 */
}

/*
 * open the given buffer to be read or written as an xpmData which is returned
 */
void
xpmOpenBuffer(char *buffer, xpmData *mdata)
{
    mdata->type = XPMBUFFER;
    mdata->cptr = buffer;
    mdata->CommentLength = 0;
}

/*
 * close the file related to the xpmData if any
 */
int
xpmDataClose(xpmData *mdata)
{
    switch (mdata->type) {
    case XPMARRAY:
    case XPMBUFFER:
	break;
    case XPMFILE:
	if (mdata->stream.file != (stdout) && mdata->stream.file != (stdin))
	    fclose(mdata->stream.file);
	break;
#ifdef ZPIPE
    case XPMPIPE:
	pclose(mdata->stream.file);
	break;
#endif
    }
    return 0;
}

xpmDataType xpmDataTypes[] =
{
    "", "!", "\n", '\0', '\n', "", "", "", "",	/* Natural type */
    "C", "/*", "*/", '"', '"', ",\n", "static char *", "[] = {\n", "};\n",
    "Lisp", ";", "\n", '"', '"', "\n", "(setq ", " '(\n", "))\n",
#ifdef VMS
    NULL
#else
    NULL, NULL, NULL, 0, 0, NULL, NULL, NULL, NULL
#endif
};

/*
 * parse xpm header
 */
int
xpmParseHeader(xpmData *mdata)
{
    char buf[BUFSIZ];
    int l, n = 0;

    if (mdata->type) {
	mdata->Bos = '\0';
	mdata->Eos = '\n';
	mdata->Bcmt = mdata->Ecmt = NULL;
	l = xpmNextWord(mdata, buf, BUFSIZ);
	if (l == 7 && !strncmp("#define", buf, 7)) {
	    /* this maybe an XPM 1 file */
	    char *ptr;

	    l = xpmNextWord(mdata, buf, BUFSIZ);
	    if (!l)
		return (XpmFileInvalid);
	    ptr = strchr(buf, '_');
	    if (!ptr || strncmp("_format", ptr, l - (ptr - buf)))
		return XpmFileInvalid;
	    /* this is definitely an XPM 1 file */
	    mdata->format = 1;
	    n = 1;			/* handle XPM1 as mainly XPM2 C */
	} else {

	    /*
	     * skip the first word, get the second one, and see if this is
	     * XPM 2 or 3
	     */
	    l = xpmNextWord(mdata, buf, BUFSIZ);
	    if ((l == 3 && !strncmp("XPM", buf, 3)) ||
		(l == 4 && !strncmp("XPM2", buf, 4))) {
		if (l == 3)
		    n = 1;		/* handle XPM as XPM2 C */
		else {
		    /* get the type key word */
		    l = xpmNextWord(mdata, buf, BUFSIZ);

		    /*
		     * get infos about this type
		     */
		    while (xpmDataTypes[n].type
			   && strncmp(xpmDataTypes[n].type, buf, l))
			n++;
		}
		mdata->format = 0;
	    } else
		/* nope this is not an XPM file */
		return XpmFileInvalid;
	}
	if (xpmDataTypes[n].type) {
	    if (n == 0) {		/* natural type */
		mdata->Bcmt = xpmDataTypes[n].Bcmt;
		mdata->Ecmt = xpmDataTypes[n].Ecmt;
		xpmNextString(mdata);	/* skip the end of the headerline */
		mdata->Bos = xpmDataTypes[n].Bos;
		mdata->Eos = xpmDataTypes[n].Eos;
	    } else {
		mdata->Bcmt = xpmDataTypes[n].Bcmt;
		mdata->Ecmt = xpmDataTypes[n].Ecmt;
		if (!mdata->format) {	/* XPM 2 or 3 */
		    mdata->Bos = xpmDataTypes[n].Bos;
		    mdata->Eos = '\0';
		    /* get to the beginning of the first string */
		    xpmNextString(mdata);
		    mdata->Eos = xpmDataTypes[n].Eos;
		} else			/* XPM 1 skip end of line */
		    xpmNextString(mdata);
	    }
	} else
	    /* we don't know about that type of XPM file... */
	    return XpmFileInvalid;
    }
    return XpmSuccess;
}
