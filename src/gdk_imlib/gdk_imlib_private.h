#ifdef _HAVE_STRING_H
#include <string.h>
#elif _HAVE_STRINGS_H
#include <strings.h>
#endif

#ifndef CONVERT_PATH
#define CONVERT_PATH "/usr/local/bin"
#endif

#ifndef NETPBM_PATH
#define NETPBM_PATH  "/usr/local/bin"
#endif

#ifndef CJPEG_PROG
#define CJPEG_PROG "/usr/bin/cjpeg"
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <sys/types.h>

#ifdef _HAVE_STRING_H
#include <string.h>
#elif _HAVE_STRINGS_H
#include <strings.h>
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/shape.h>
#include <X11/cursorfont.h>
#include <gdk/gdkprivate.h>

#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#endif
#ifdef HAVE_LIBPNG
#include <../png/png.h>
#endif
#ifdef HAVE_LIBTIFF
#include <tiffio.h>
#endif
#ifdef HAVE_LIBGIF
#include <gif_lib.h>
#endif

#define BYTE_ORD_24_RGB 0
#define BYTE_ORD_24_RBG 1
#define BYTE_ORD_24_BRG 2
#define BYTE_ORD_24_BGR 3
#define BYTE_ORD_24_GRB 4
#define BYTE_ORD_24_GBR 5

struct image_cache
  {
     gchar              *file;
     GdkImlibImage      *im;
     gint                refnum;
     gchar               dirty;
     struct image_cache *prev;
     struct image_cache *next;
  };

struct pixmap_cache
  {
     GdkImlibImage      *im;
     gchar              *file;
     gchar               dirty;
     gint                width, height;
     GdkPixmap          *pmap;
     GdkBitmap          *shape_mask;
     XImage             *xim, *sxim;
     gint                refnum;
     struct pixmap_cache *prev;
     struct pixmap_cache *next;
  };

typedef struct _xdata
  {
     Display            *disp;
     gint                screen;
     Window              root;
     Visual             *visual;
     gint                depth;
     gint                render_depth;
     Colormap            root_cmap;
     gchar               shm;
     gchar               shmp;
     gint                shm_event;
     XImage             *last_xim;
     XImage             *last_sxim;
     XShmSegmentInfo     last_shminfo;
     XShmSegmentInfo     last_sshminfo;
     Window              base_window;
     GdkWindow          *gdk_win;
     GdkColormap        *gdk_cmap;
  }
Xdata;

typedef struct _imlibdata
  {
     gint                num_colors;
     GdkImlibColor      *palette;
     GdkImlibColor      *palette_orig;
     unsigned char      *fast_rgb;
     gint               *fast_err;
     gint               *fast_erg;
     gint               *fast_erb;
     gint                render_type;
     gint                max_shm;
     Xdata               x;
     gint                byte_order;
     struct _cache
       {
	  gchar               on_image;
	  gint                size_image;
	  gint                num_image;
	  gint                used_image;
	  struct image_cache *image;
	  gchar               on_pixmap;
	  gint                size_pixmap;
	  gint                num_pixmap;
	  gint                used_pixmap;
	  struct pixmap_cache *pixmap;
       }
     cache;
     gchar               fastrend;
     gchar               hiq;
     GdkImlibColorModifier mod, rmod, gmod, bmod;
     unsigned char       rmap[256], gmap[256], bmap[256];
     gchar               fallback;
     gchar               ordered_dither;
  }
ImlibData;

extern ImlibData   *id;

gint                gindex_best_color_match(gint * r, gint * g, gint * b);

void                gdirty_pixmaps(GdkImlibImage * im);
void                gdirty_images(GdkImlibImage * im);
void                gfind_pixmap(GdkImlibImage * im, int width, int height, GdkPixmap ** pmap, GdkBitmap ** mask);
GdkImlibImage      *gfind_image(char *file);
void                gfree_pixmappmap(GdkPixmap * pmap);
void                gfree_image(GdkImlibImage * im);
void                gflush_image(GdkImlibImage * im);
void                gadd_image(GdkImlibImage * im, char *file);
void                gadd_pixmap(GdkImlibImage * im, int width, int height, XImage * xim, XImage * sxim);
void                gclean_caches();
void                gnullify_image(GdkImlibImage * im);

/* char *g_SplitID(char *file); */
char               *g_GetExtension(char *file);

#ifdef HAVE_LIBJPEG
unsigned char      *g_LoadJPEG(FILE * f, int *w, int *h);

#endif /* HAVE_LIBJPEG */
#ifdef HAVE_LIBPNG
unsigned char      *g_LoadPNG(FILE * f, int *w, int *h, int *t);

#endif /* HAVE_LIBPNG */
#ifdef HAVE_LIBTIFF
unsigned char      *g_LoadTIFF(char *f, int *w, int *h, int *t);

#endif /* HAVE_LIBTIFF */
#ifdef HAVE_LIBGIF
unsigned char      *g_LoadGIF(char *f, int *w, int *h, int *t);

#endif /* HAVE_LIBGIF */
unsigned char      *g_LoadXPM(char *f, int *w, int *h, int *t);
unsigned char      *g_LoadPPM(FILE * f, int *w, int *h);

/*
static int          gispnm(char *file);
static int          gisjpeg(char *file);
static int          gispng(char *file);
static int          gistiff(char *file);
static int          giseim(char *file);
static int          gisgif(char *file);
static int          gisxpm(char *file);
*/

GdkPixmap          *gdk_imlib_pixmap_foreign_new(gint width, gint height, gint depth, Pixmap pmap);

void                gcalc_map_tables(GdkImlibImage * im);

void                g_PaletteAlloc(int num, int *cols);

FILE               *open_helper(const char *, const char *, const char *);
int                 close_helper(FILE *);

#define INDEX_RGB(r,g,b)  id->fast_rgb[(r<<10)|(g<<5)|(b)]
#define COLOR_INDEX(i)    id->palette[i].pixel
#define COLOR_RGB(r,g,b)  id->palette[INDEX_RGB(r,g,b)].pixel
#define ERROR_RED(rr,i)   rr-id->palette[i].r;
#define ERROR_GRN(gg,i)   gg-id->palette[i].g;
#define ERROR_BLU(bb,i)   bb-id->palette[i].b;

#define DITHER_ERROR(Der1,Der2,Dex,Der,Deg,Deb) \
ter=&(Der1[Dex]);\
(*ter)+=(Der*7)>>4;ter++;\
(*ter)+=(Deg*7)>>4;ter++;\
(*ter)+=(Deb*7)>>4;\
ter=&(Der2[Dex-6]);\
(*ter)+=(Der*3)>>4;ter++;\
(*ter)+=(Deg*3)>>4;ter++;\
(*ter)+=(Deb*3)>>4;ter++;\
(*ter)+=(Der*5)>>4;ter++;\
(*ter)+=(Deg*5)>>4;ter++;\
(*ter)+=(Deb*5)>>4;ter++;\
(*ter)+=Der>>4;ter++;\
(*ter)+=Deg>>4;ter++;\
(*ter)+=Deb>>4;
