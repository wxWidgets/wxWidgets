// PRIVATE STUFF FOLLOWS UNTIL END

// Header signatures for various resources
#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */

// This WIDTHBYTES macro determines the number of BYTES per scan line.
#define WIDTHBYTES( i)   ((i + 31) / 32 * 4)
#define IS_WIN30_DIB( lpbi)  ((*(LPDWORD)( lpbi)) == sizeof( BITMAPINFOHEADER))

WORD         DIBNumColors(LPSTR pv);
WORD         PaletteSize(LPSTR lpbi);


struct tagCURFILEHEADER { WORD wReserved;         // Always 0
                          WORD wResourceType;     // 2 = cursor
                          WORD wResourceCount;    // Number of icons in the file
                        };

typedef struct tagCURFILEHEADER CURFILEHEADER;

struct tagCURFILERES {
    BYTE  bWidth;         // Width of image
    BYTE  bHeight;        // Height of image
    BYTE  bColorCount;    // Number of colors in image (2, 8, or 16)
    BYTE  bReserved1;     // Reserved
    WORD  wXHotspot;      // x coordinate of hotspot
    WORD  wYHotspot;      // y coordinate of hotspot
    DWORD dwDIBSize;      // Size of DIB for this image
    DWORD dwDIBOffset;    // Offset to DIB for this image
  };

typedef struct tagCURFILERES CURFILERES;

HANDLE  ReadCur( LPTSTR szFileName, LPPOINT lpptHotSpot, int *W = 0, int *H = 0);
HBITMAP ColorDDBToMonoDDB( HBITMAP hbm);
HCURSOR MakeCursor( HANDLE hDIB, LPPOINT lpptHotSpot, HINSTANCE hInst);

struct tagICONFILEHEADER {
  WORD wReserved;         // Always 0
  WORD wResourceType;     // 1 = icon
  WORD wResourceCount;    // Number of icons in the file
  };

typedef struct tagICONFILEHEADER ICONFILEHEADER;

struct tagICONFILERES {
    BYTE  bWidth;         // Width of image
    BYTE  bHeight;        // Height of image
    BYTE  bColorCount;    // Number of colors in image (2, 8, or 16)
    BYTE  bReserved1;     // Reserved
    WORD  wReserved2;
    WORD  wReserved3;
    DWORD dwDIBSize;      // Size of DIB for this image
    DWORD dwDIBOffset;    // Offset to DIB for this image
  };

typedef struct tagICONFILERES ICONFILERES;

HANDLE ReadIcon( wxChar *szFileName, int *W = 0, int *H = 0);
HICON  MakeIcon( HANDLE hDIB, HINSTANCE hInst);

