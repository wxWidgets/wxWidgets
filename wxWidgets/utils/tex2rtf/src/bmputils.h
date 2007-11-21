/////////////////////////////////////////////////////////////////////////////
// Name:        bmputils.h
// Purpose:     Utilities for manipulating bitmap and metafile images for
//              the purposes of conversion to RTF
// Author:      Julian Smart
// Modified by: Wlodzimiez ABX Skiba 2003/2004 Unicode support
//              Ron Lee
// Created:     7.9.93
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

static char hexArray[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B',
  'C', 'D', 'E', 'F' };

void DecToHex(int dec, wxChar *buf)
{
  int firstDigit = (int)(dec/16.0);
  int secondDigit = (int)(dec - (firstDigit*16.0));
  buf[0] = hexArray[firstDigit];
  buf[1] = hexArray[secondDigit];
  buf[2] = 0;
}

static unsigned int getshort(FILE *fp)
{
  int c, c1;
  c = getc(fp);  c1 = getc(fp);
  return ((unsigned int) c) + (((unsigned int) c1) << 8);
}

static unsigned long getint(FILE *fp)
{
  int c, c1, c2, c3;
  c = getc(fp);  c1 = getc(fp);  c2 = getc(fp);  c3 = getc(fp);
  return (long)((long) c) +
         (((long) c1) << 8) +
         (((long) c2) << 16) +
         (((long) c3) << 24);
}

bool GetBMPHeader(FILE *fp, int *Width, int *Height, int *Planes, int *BitsPerPixel)
{
  // Remember about all fields but store only important ones
  unsigned long /*
                bfSize,
                bfOffBits,
                biSize,
                */
                biWidth,
                biHeight,
                biPlanes,
                biBitCount
                /*        ,
                biCompression,
                biSizeImage,
                biXPelsPerMeter,
                biYPelsPerMeter,
                biClrUsed,
                biClrImportant
                */
                ;

  /* read the file type (first two bytes) */
  int c = getc(fp); int c1 = getc(fp);
  if (c!='B' || c1!='M') { return false; }

  /* bfSize = */          getint(fp);
  getshort(fp);         /* reserved and ignored */
  getshort(fp);
  /* bfOffBits = */       getint(fp);

  /* biSize          = */ getint(fp);
  biWidth         = getint(fp);
  biHeight        = getint(fp);
  biPlanes        = getshort(fp);
  biBitCount      = getshort(fp);
  /* biCompression   = */ getint(fp);
  /* biSizeImage     = */ getint(fp);
  /* biXPelsPerMeter = */ getint(fp);
  /* biYPelsPerMeter = */ getint(fp);
  /* biClrUsed       = */ getint(fp);
  /* biClrImportant  = */ getint(fp);

  *Width = (int)biWidth;
  *Height = (int)biHeight;
  *Planes = (int)biPlanes;
  *BitsPerPixel = (int)biBitCount;

//  fseek(fp, bfOffBits, SEEK_SET);

  return true;
}

static int scanLineWidth = 0;

bool OutputBitmapHeader(FILE *fd, bool isWinHelp = false)
{
  int Width, Height, Planes, BitsPerPixel;
  if (!GetBMPHeader(fd, &Width, &Height, &Planes, &BitsPerPixel))
    return false;

  scanLineWidth = (int)((float)Width/(8.0/(float)BitsPerPixel));
  if ((float)((int)(scanLineWidth/2.0)) != (float)(scanLineWidth/2.0))
    scanLineWidth ++;

  int goalW = 15*Width;
  int goalH = 15*Height;

  TexOutput(_T("{\\pict"));
  if (isWinHelp) TexOutput(_T("\\wbitmap0"));
  else TexOutput(_T("\\dibitmap)"));

  wxChar buf[50];
  TexOutput(_T("\\picw")); wxSnprintf(buf, sizeof(buf), _T("%d"), Width); TexOutput(buf);
  TexOutput(_T("\\pich")); wxSnprintf(buf, sizeof(buf), _T("%d"), Height); TexOutput(buf);
  TexOutput(_T("\\wbmbitspixel")); wxSnprintf(buf, sizeof(buf), _T("%d"), BitsPerPixel); TexOutput(buf);
  TexOutput(_T("\\wbmplanes")); wxSnprintf(buf, sizeof(buf), _T("%d"), Planes); TexOutput(buf);
  TexOutput(_T("\\wbmwidthbytes")); wxSnprintf(buf, sizeof(buf), _T("%d"), scanLineWidth); TexOutput(buf);
  TexOutput(_T("\\picwgoal")); wxSnprintf(buf, sizeof(buf), _T("%d"), goalW); TexOutput(buf);
  TexOutput(_T("\\pichgoal")); wxSnprintf(buf, sizeof(buf), _T("%d"), goalH); TexOutput(buf);
  TexOutput(_T("\n"));
  return true;
}


bool OutputBitmapData(FILE *fd)
{
  fseek(fd, 14, SEEK_SET);
  int bytesSoFar = 0;
  int ch = getc(fd);
  wxChar hexBuf[3];
  while (ch != EOF)
  {
    if (bytesSoFar == scanLineWidth)
    {
      bytesSoFar = 0;
      TexOutput(_T("\n"));
    }
    DecToHex(ch, hexBuf);
    TexOutput(hexBuf);
    bytesSoFar ++;
    ch = getc(fd);
  }
  TexOutput(_T("\n}\n"));
  return true;
}

#ifdef __WXMSW__
struct mfPLACEABLEHEADER {
    DWORD key;
    HANDLE hmf;
    RECT bbox;
    WORD inch;
    DWORD reserved;
    WORD checksum;
};

// Returns size in TWIPS
bool GetMetafileHeader(FILE *handle, int *width, int *height)
{
  char buffer[40];
  mfPLACEABLEHEADER *theHeader = (mfPLACEABLEHEADER *)&buffer[0];
  fread((void *)theHeader, sizeof(char), sizeof(mfPLACEABLEHEADER), handle);
  if (theHeader->key != 0x9AC6CDD7)
  {
    return false;
  }

  float widthInUnits = (float)theHeader->bbox.right - theHeader->bbox.left;
  float heightInUnits = (float)theHeader->bbox.bottom - theHeader->bbox.top;
  *width = (int)((widthInUnits*1440.0)/theHeader->inch);
  *height = (int)((heightInUnits*1440.0)/theHeader->inch);
  return true;
}

bool OutputMetafileHeader(FILE *handle, bool WXUNUSED(isWinHelp), int userWidth, int userHeight)
{
  int Width, Height;
  if (!GetMetafileHeader(handle, &Width, &Height))
    return false;

  scanLineWidth = 64;
  int goalW = Width;
  int goalH = Height;

  // Scale to user's dimensions if we have the information
  if (userWidth > 0 && userHeight == 0)
  {
    double scaleFactor = ((double)userWidth/(double)goalW);
    goalW = userWidth;
    goalH = (int)((goalH * scaleFactor) + 0.5);
  }
  else if (userWidth == 0 && userHeight > 0)
  {
    double scaleFactor = ((double)userHeight/(double)goalH);
    goalH = userHeight;
    goalW = (int)((goalW * scaleFactor) + 0.5);
  }
  else if (userWidth > 0 && userHeight > 0)
  {
    goalW = userWidth;
    goalH = userHeight;
  }

  TexOutput(_T("{\\pict"));
  TexOutput(_T("\\wmetafile8"));

  wxChar buf[50];
  TexOutput(_T("\\picw")); wxSnprintf(buf, sizeof(buf), _T("%d"), Width); TexOutput(buf);
  TexOutput(_T("\\pich")); wxSnprintf(buf, sizeof(buf), _T("%d"), Height); TexOutput(buf);
  TexOutput(_T("\\picwgoal")); wxSnprintf(buf, sizeof(buf), _T("%d"), goalW); TexOutput(buf);
  TexOutput(_T("\\pichgoal")); wxSnprintf(buf, sizeof(buf), _T("%d"), goalH); TexOutput(buf);
  TexOutput(_T("\n"));
  return true;
}

bool OutputMetafileData(FILE *handle)
{
  int bytesSoFar = 0;
  wxChar hexBuf[3];
  int ch;
  do
  {
    ch = getc(handle);
    if (bytesSoFar == scanLineWidth)
    {
      bytesSoFar = 0;
      TexOutput(_T("\n"));
    }
    if (ch != EOF)
    {
      DecToHex(ch, hexBuf);
      TexOutput(hexBuf);
      bytesSoFar ++;
    }
  } while (ch != EOF);
  TexOutput(_T("\n}\n"));
  return true;
}

#endif

