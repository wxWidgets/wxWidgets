#include <gdk/gdk.h>

#ifndef SYSTEM_IMRC
#define SYSTEM_IMRC "/etc/imrc"
#endif /* endef SYSTEM_IMRC */

typedef struct _GdkImlibBorder
  {
     gint                left, right;
     gint                top, bottom;
  }
GdkImlibBorder;

typedef struct _GdkImlibColor
  {
     gint                r, g, b;
     gint                pixel;
  }
GdkImlibColor;

typedef struct _GdkImlibColorModifier
  {
     gint                gamma;
     gint                brightness;
     gint                contrast;
  }
GdkImlibColorModifier;

typedef struct _GdkImlibImage
  {
     gint                rgb_width, rgb_height;
     unsigned char      *rgb_data;
     unsigned char      *alpha_data;
     gchar              *filename;
/* the below information is private */
     gint                width, height;
     GdkImlibColor       shape_color;
     GdkImlibBorder      border;
     GdkPixmap          *pixmap;
     GdkBitmap          *shape_mask;
     gchar               cache;
     GdkImlibColorModifier mod, rmod, gmod, bmod;
     unsigned char       rmap[256], gmap[256], bmap[256];
  }
GdkImlibImage;

typedef struct _GdkImlibSaveInfo
  {
     int                 quality;
     int                 scaling;
     int                 xjustification;
     int                 yjustification;
     int                 page_size;
     char                color;
  }
GdkImlibSaveInfo;

typedef struct _GdkImlibInitParams
  {
     int                 flags;
     int                 visualid;
     char               *palettefile;
     char                sharedmem;
     char                sharedpixmaps;
     char                paletteoverride;
     char                remap;
     char                fastrender;
     char                hiquality;
     char                dither;
     int                 imagecachesize;
     int                 pixmapcachesize;
  }
GdkImlibInitParams;

#define PARAMS_VISUALID        1<<0
#define PARAMS_PALETTEFILE     1<<1
#define PARAMS_SHAREDMEM       1<<2
#define PARAMS_SHAREDPIXMAPS   1<<3
#define PARAMS_PALETTEOVERRIDE 1<<4
#define PARAMS_REMAP           1<<5
#define PARAMS_FASTRENDER      1<<6
#define PARAMS_HIQUALITY       1<<7
#define PARAMS_DITHER          1<<8
#define PARAMS_IMAGECACHESIZE  1<<9
#define PARAMS_PIXMAPCACHESIZE 1<<10

#define PAGE_SIZE_EXECUTIVE    0
#define PAGE_SIZE_LETTER       1
#define PAGE_SIZE_LEGAL        2
#define PAGE_SIZE_A4           3
#define PAGE_SIZE_A3           4
#define PAGE_SIZE_A5           5
#define PAGE_SIZE_FOLIO        6

#define RT_PLAIN_PALETTE       0
#define RT_PLAIN_PALETTE_FAST  1
#define RT_DITHER_PALETTE      2
#define RT_DITHER_PALETTE_FAST 3
#define RT_PLAIN_TRUECOL       4
/* a special high-quality renderer for people with 15 and 16bpp that dithers */
#define RT_DITHER_TRUECOL      5
