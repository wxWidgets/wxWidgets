#define _GNU_SOURCE
#include "gdk_imlib.h"
#include "gdk_imlib_private.h"

void
g_PaletteAlloc(int num, int *cols)
{
   XColor              xcl;
   int                 i;
   int                 r, g, b;

   if (id->palette)
      free(id->palette);
   id->palette = malloc(sizeof(GdkImlibColor) * num);
   if (id->palette_orig)
      free(id->palette_orig);
   id->palette_orig = malloc(sizeof(GdkImlibColor) * num);
   for (i = 0; i < num; i++)
     {
	r = cols[(i * 3) + 0];
	g = cols[(i * 3) + 1];
	b = cols[(i * 3) + 2];
	xcl.red = (unsigned short)((r << 8) | (r));
	xcl.green = (unsigned short)((g << 8) | (g));
	xcl.blue = (unsigned short)((b << 8) | (b));
	xcl.flags = DoRed | DoGreen | DoBlue;
	XAllocColor(id->x.disp, id->x.root_cmap, &xcl);
	id->palette[i].r = xcl.red >> 8;
	id->palette[i].g = xcl.green >> 8;
	id->palette[i].b = xcl.blue >> 8;
	id->palette[i].pixel = xcl.pixel;
	id->palette_orig[i].r = r;
	id->palette_orig[i].g = g;
	id->palette_orig[i].b = b;
	id->palette_orig[i].pixel = xcl.pixel;
     }
   id->num_colors = num;
}

gint
gdk_imlib_load_colors(char *file)
{
   FILE               *f;
   char                s[256];
   int                 i;
   int                 pal[768];
   int                 r, g, b;
   int                 rr, gg, bb;

   f = fopen(file, "r");
   if (!f)
     {
	fprintf(stderr, "GImLib ERROR: Cannot find palette file %s\n", file);
	return 0;
     }
   i = 0;
   while (fgets(s, 256, f))
     {
	if (s[0] == '0')
	  {
	     sscanf(s, "%x %x %x", &r, &g, &b);
	     if (r < 0)
		r = 0;
	     if (r > 255)
		r = 255;
	     if (g < 0)
		g = 0;
	     if (g > 255)
		g = 255;
	     if (b < 0)
		b = 0;
	     if (b > 255)
		b = 255;
	     pal[i++] = r;
	     pal[i++] = g;
	     pal[i++] = b;
	  }
	if (i >= 768)
	   break;
     }
   fclose(f);
   g_PaletteAlloc((i / 3), pal);
   if (id->fast_rgb)
      free(id->fast_rgb);
   id->fast_rgb = malloc(sizeof(int) * 32 * 32 * 32);

   for (r = 0; r < 32; r++)
     {
	for (g = 0; g < 32; g++)
	  {
	     for (b = 0; b < 32; b++)
	       {
		  rr = (r << 3) | (r >> 2);
		  gg = (g << 3) | (g >> 2);
		  bb = (b << 3) | (b >> 2);
		  INDEX_RGB(r, g, b) = gindex_best_color_match(&rr, &gg, &bb);
	       }
	  }
     }
   return 1;
}

void
gdk_imlib_free_colors()
{
   int                 i;
   unsigned long       pixels[256];

   for (i = 0; i < id->num_colors; i++)
      pixels[i] = id->palette[i].pixel;
   XFreeColors(id->x.disp, id->x.root_cmap, pixels, id->num_colors, 0);
   id->num_colors = 0;
}
