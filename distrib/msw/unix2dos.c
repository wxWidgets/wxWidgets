/*****************************************************************************
dos2unix, unix2dos: translate unix <-> dos text files by 
                       -> adding a CR before each LF
                       <- removing the CR from CRLF pairs 

Use a modern compiler (gcc);  name the executable either unix2dos or dos2unix,
and link to the other name. A warning will be printed if the input of unix2dos
contains CR's, as it may have already been a dos file (output still produced).
The warning is not produced if input is stdin.
Jim Martino				        +++++++++++++++++++++++++++++
jrm@chow.mat.jhu.edu			+   PLACE COMMERCIAL HERE   +
jmartino@jhunix.hcf.jhu.edu		+++++++++++++++++++++++++++++
Modified by Julian Smart, September 2002
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CR '\015'  /* define CR character */
#define LF '\012'  /* define LF character */

char *prog;      /* global var: program name as called */
int warning=0;   /* global var: flag for unix2dos if input has CR's */

void usage();
void translate(FILE *, FILE *, int unix2Dos);

main(int argc, char *argv[])
{
    FILE *fp, *outFile;
    int i;
    int unix2Dos;
    
    prog = argv[0]; /* program name as called */
    
    unix2Dos = 1;
    
    i = 1;
    if (argc > 1)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            usage();
        }
        else if (strcmp(argv[1], "--dos2unix") == 0)
        {
            unix2Dos = 0;
        }
        else if (strcmp(argv[1], "--unix2dos") == 0)
        {
            unix2Dos = 1;
        }
        else
        {
            usage();
        }
    }
    else
        usage();
    
    i ++;
    if (i == argc)
        translate(stdin, stdout, unix2Dos);
    else
    {
        while (i < argc)
        {
            char tmpFile[512];
            sprintf(tmpFile, "%s.tmp", argv[i]);
            
            fp = fopen(argv[i], "rb");
            if (!fp)
            {
                fprintf(stderr, "Cannot open %s.\n", argv[i]);
                i ++;
                continue;
            }
            outFile = fopen(tmpFile, "wb");
            if (!outFile)
            {
                fprintf(stderr, "Cannot open %s.\n", tmpFile);
                exit(1);
            }
            translate(fp, outFile, unix2Dos);
            
            if (warning) /* unix2dos acting on a possible DOS file */
            {
                fprintf(stderr,"%s: %s may have already been in DOS format. Not converted.\n",
                        prog, argv[i]);
                warning = 0;
            }
            fclose(fp);
            fclose(outFile);
#ifdef _WINDOWS
            remove(argv[i]);
#else
            unlink(argv[i]);
#endif
            rename(tmpFile, argv[i]);
            
            i ++;
        }
    }
}

/* translate: unix2dos-change LF to CRLF
dos2unix-strip out CR only before LF */

void translate(FILE *ifp, FILE *ofp, int unix2Dos)
{
    int c,d;
    
    if (!unix2Dos)
    {
        /* DOS2Unix */
        while ((c = getc(ifp)) != EOF)
        {
            if (c == CR)
            {
                switch(d = getc(ifp))
                { /* check to see if LF follows */
                    case LF:
                        putc(d,ofp);         /* if so, ignore CR */
                        break;
                    default:
                        putc(c,ofp);         /* if not, output CR and following char */
                        putc(d,ofp);
                }
            }
            else
                putc(c, ofp); /* c is not a CR */
        }
     }
     else
        {
            /* Unix2DOS */
            while ((c = getc(ifp)) != EOF){
                if (c == CR)
                    warning = 1;   /* set warning flag: input file may be a DOS file */
                if (c == LF && (warning == 0))
                    putc(CR, ofp); /* add CR before each LF */
                putc(c, ofp);
            }
        }
}

void usage()
{
    fprintf(stderr, "Usage:\n%s [--dos2unix] [--unix2dos] file1 file2 ...\nor\n%s [--dos2unix] [--unix2dos] < infile > outfile\n", prog, prog);
    exit(0);
}
