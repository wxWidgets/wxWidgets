#define _GNU_SOURCE

#include "../gdk_imlib/gdk_imlib.h"
#include "../gdk_imlib/gdk_imlib_private.h"

void
gcalc_map_tables(GdkImlibImage * im)
{
   int                 i;
   double              g, b, c, ii, v;

   if (!im)
      return;

   g = ((double)im->mod.gamma) / 256;
   b = ((double)im->mod.brightness) / 256;
   c = ((double)im->mod.contrast) / 256;
   if (g < 0.01)
      g = 0.01;

   for (i = 0; i < 256; i++)
     {
	ii = ((double)i) / 256;
	v = ((ii - 0.5) * c) + 0.5 + (b - 1);
	if (v > 0)
	   v = pow(((ii - 0.5) * c) + 0.5 + (b - 1), 1 / g) * 256;
	else
	   v = 0;
	if (v > 255)
	   v = 255;
	else if (v < 0)
	   v = 0;
	im->rmap[i] = (unsigned char)v;
	im->gmap[i] = (unsigned char)v;
	im->bmap[i] = (unsigned char)v;
     }
   g = ((double)im->rmod.gamma) / 256;
   b = ((double)im->rmod.brightness) / 256;
   c = ((double)im->rmod.contrast) / 256;
   if (g < 0.01)
      g = 0.01;

   for (i = 0; i < 256; i++)
     {
	ii = ((double)im->rmap[i]) / 256;
	v = ((ii - 0.5) * c) + 0.5 + (b - 1);
	if (v > 0)
	   v = pow(((ii - 0.5) * c) + 0.5 + (b - 1), 1 / g) * 256;
	else
	   v = 0;
	if (v > 255)
	   v = 255;
	else if (v < 0)
	   v = 0;
	im->rmap[i] = (unsigned char)v;
     }
   g = ((double)im->gmod.gamma) / 256;
   b = ((double)im->gmod.brightness) / 256;
   c = ((double)im->gmod.contrast) / 256;
   if (g < 0.01)
      g = 0.01;

   for (i = 0; i < 256; i++)
     {
	ii = ((double)im->gmap[i]) / 256;
	v = ((ii - 0.5) * c) + 0.5 + (b - 1);
	if (v > 0)
	   v = pow(((ii - 0.5) * c) + 0.5 + (b - 1), 1 / g) * 256;
	else
	   v = 0;
	if (v > 255)
	   v = 255;
	else if (v < 0)
	   v = 0;
	im->gmap[i] = (unsigned char)v;
     }
   g = ((double)im->bmod.gamma) / 256;
   b = ((double)im->bmod.brightness) / 256;
   c = ((double)im->bmod.contrast) / 256;
   if (g < 0.01)
      g = 0.01;
   for (i = 0; i < 256; i++)
     {
	ii = ((double)im->bmap[i]) / 256;
	v = ((ii - 0.5) * c) + 0.5 + (b - 1);
	if (v > 0)
	   v = pow(((ii - 0.5) * c) + 0.5 + (b - 1), 1 / g) * 256;
	else
	   v = 0;
	if (v > 255)
	   v = 255;
	else if (v < 0)
	   v = 0;
	im->bmap[i] = (unsigned char)v;
     }
   gdirty_pixmaps(im);
}

gint
gdk_imlib_load_file_to_pixmap(char *filename, GdkPixmap ** pmap, GdkBitmap ** mask)
{
   GdkImlibImage      *im;

   im = gdk_imlib_load_image(filename);
   if (!im)
     {
	if (pmap)
	   *pmap = NULL;
	if (mask)
	   *mask = NULL;
	return 0;
     }
   if (!gdk_imlib_render(im, im->rgb_width, im->rgb_height))
     {
	gdk_imlib_destroy_image(im);
	if (pmap)
	   *pmap = NULL;
	if (mask)
	   *mask = NULL;
	return 0;
     }
   if (pmap)
      *pmap = gdk_imlib_move_image(im);
   if (mask)
      *mask = gdk_imlib_move_mask(im);
   gdk_imlib_destroy_image(im);
   return 1;
}

void
gdk_imlib_set_image_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im || !mod)
      return;
   im->mod.gamma = mod->gamma;
   im->mod.brightness = mod->brightness;
   im->mod.contrast = mod->contrast;
   gcalc_map_tables(im);
}

void
gdk_imlib_set_image_red_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im || !mod)
      return;
   im->rmod.gamma = mod->gamma;
   im->rmod.brightness = mod->brightness;
   im->rmod.contrast = mod->contrast;
   gcalc_map_tables(im);
}

void
gdk_imlib_set_image_green_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im || !mod)
      return;
   im->gmod.gamma = mod->gamma;
   im->gmod.brightness = mod->brightness;
   im->gmod.contrast = mod->contrast;
   gcalc_map_tables(im);
}

void
gdk_imlib_set_image_blue_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im | !mod)
      return;
   im->bmod.gamma = mod->gamma;
   im->bmod.brightness = mod->brightness;
   im->bmod.contrast = mod->contrast;
   gcalc_map_tables(im);
}

void
gdk_imlib_get_image_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im || !mod)
      return;
   mod->gamma = im->mod.gamma;
   mod->brightness = im->mod.brightness;
   mod->contrast = im->mod.contrast;
   gcalc_map_tables(im);
}

void
gdk_imlib_get_image_red_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im || !mod)
      return;
   mod->gamma = im->rmod.gamma;
   mod->brightness = im->rmod.brightness;
   mod->contrast = im->rmod.contrast;
}

void
gdk_imlib_get_image_green_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im || !mod)
      return;
   mod->gamma = im->gmod.gamma;
   mod->brightness = im->gmod.brightness;
   mod->contrast = im->gmod.contrast;
}

void
gdk_imlib_get_image_blue_modifier(GdkImlibImage * im, GdkImlibColorModifier * mod)
{
   if (!im || !mod)
      return;
   mod->gamma = im->bmod.gamma;
   mod->brightness = im->bmod.brightness;
   mod->contrast = im->bmod.contrast;
}

void
gdk_imlib_set_image_red_curve(GdkImlibImage * im, unsigned char *mod)
{
   int                 i;

   if (!im || !mod)
      return;
   for (i = 0; i < 256; i++)
      im->rmap[i] = mod[i];
   gdirty_pixmaps(im);
   im->mod.contrast = 257;
}

void
gdk_imlib_set_image_green_curve(GdkImlibImage * im, unsigned char *mod)
{
   int                 i;

   if (!im || !mod)
      return;
   for (i = 0; i < 256; i++)
      im->gmap[i] = mod[i];
   gdirty_pixmaps(im);
   im->mod.contrast = 257;
}

void
gdk_imlib_set_image_blue_curve(GdkImlibImage * im, unsigned char *mod)
{
   int                 i;

   if (!im || !mod)
      return;
   for (i = 0; i < 256; i++)
      im->bmap[i] = mod[i];
   gdirty_pixmaps(im);
   im->mod.contrast = 257;
}

void
gdk_imlib_get_image_red_curve(GdkImlibImage * im, unsigned char *mod)
{
   int                 i;

   if ((!im) || (!mod))
      return;
   for (i = 0; i < 256; i++)
      mod[i] = im->rmap[i];
}

void
gdk_imlib_get_image_green_curve(GdkImlibImage * im, unsigned char *mod)
{
   int                 i;

   if (!im || !mod)
      return;
   for (i = 0; i < 256; i++)
      mod[i] = im->gmap[i];
}

void
gdk_imlib_get_image_blue_curve(GdkImlibImage * im, unsigned char *mod)
{
   int                 i;

   if (!im || !mod)
      return;
   for (i = 0; i < 256; i++)
      mod[i] = im->bmap[i];
}

void
gdk_imlib_apply_modifiers_to_rgb(GdkImlibImage * im)
{
   int                 x, y;
   unsigned char      *ptr;

   if (!im)
      return;
   ptr = im->rgb_data;
   for (y = 0; y < im->rgb_height; y++)
     {
	for (x = 0; x < im->rgb_width; x++)
	  {
	     *ptr = im->rmap[*ptr];
	     ptr++;
	     *ptr = im->gmap[*ptr];
	     ptr++;
	     *ptr = im->bmap[*ptr];
	     ptr++;
	  }
     }
   im->mod.gamma = 256;
   im->mod.brightness = 256;
   im->mod.contrast = 256;
   im->rmod.gamma = 256;
   im->rmod.brightness = 256;
   im->rmod.contrast = 256;
   im->gmod.gamma = 256;
   im->gmod.brightness = 256;
   im->gmod.contrast = 256;
   im->bmod.gamma = 256;
   im->bmod.brightness = 256;
   im->bmod.contrast = 256;
   gcalc_map_tables(im);
   gdirty_images(im);
   gdirty_pixmaps(im);
}

void
gdk_imlib_crop_image(GdkImlibImage * im, gint x, gint y, gint w, gint h)
{
   unsigned char      *data;
   int                 xx, yy, w3, w4;
   unsigned char      *ptr1, *ptr2;

   if (!im)
      return;
   if (x < 0)
     {
	w += x;
	x = 0;
     }
   if (y < 0)
     {
	h += y;
	y = 0;
     }
   if (x >= im->rgb_width)
      return;
   if (y >= im->rgb_height)
      return;
   if (w <= 0)
      return;
   if (h <= 0)
      return;
   if (x + w > im->rgb_width)
      w = im->rgb_width - x;
   if (y + h > im->rgb_height)
      h = im->rgb_height - y;
   if (w <= 0)
      return;
   if (h <= 0)
      return;

   w3 = im->rgb_width * 3;
   w4 = (im->rgb_width - w) * 3;
   data = malloc(w * h * 3);
   if (data == NULL)
      return;
   ptr1 = im->rgb_data + (y * w3) + (x * 3);
   ptr2 = data;
   for (yy = 0; yy < h; yy++)
     {
	for (xx = 0; xx < w; xx++)
	  {
	     *ptr2++ = *ptr1++;
	     *ptr2++ = *ptr1++;
	     *ptr2++ = *ptr1++;
	  }
	ptr1 += w4;
     }
   free(im->rgb_data);
   im->rgb_data = data;
   if (im->border.left > x)
      im->border.left = im->border.left - x;
   else
      im->border.left = 0;
   if (im->border.top > y)
      im->border.top = im->border.top - y;
   else
      im->border.top = 0;
   if (im->rgb_width - im->border.right < x + w)
      im->border.right = im->border.right - (im->rgb_width - (x + w));
   else
      im->border.right = 0;
   if (im->rgb_height - im->border.bottom < y + h)
      im->border.bottom = im->border.bottom - (im->rgb_height - (y + h));
   else
      im->border.bottom = 0;
   im->rgb_width = w;
   im->rgb_height = h;
   gdirty_images(im);
   gdirty_pixmaps(im);
}

void
gdk_imlib_changed_image(GdkImlibImage * im)
{
   if (!im)
      return;
   gdirty_images(im);
   gdirty_pixmaps(im);
}

void
gdk_imlib_apply_image(GdkImlibImage * im, GdkWindow * p)
{
   GdkPixmap          *pp, *mm;
   int                 w, h;

   if (!im || !p)
      return;
   gdk_window_get_size(p, &w, &h);
   if (w <= 0 || h <= 0)
      return;
   gdk_imlib_render(im, w, h);
   pp = gdk_imlib_move_image(im);
   mm = gdk_imlib_move_mask(im);
   gdk_window_set_back_pixmap(p, pp, 0);
   if (mm)
      gdk_window_shape_combine_mask(p, mm, 0, 0);
   gdk_window_clear(p);
   gdk_imlib_free_pixmap(pp);
}

void
gdk_imlib_paste_image(GdkImlibImage * im, GdkWindow * p, gint x, gint y, gint w, gint h)
{
   GdkGC              *gc;
   GdkPixmap          *pp, *mm;

   if (!im || !p)
      return;
   if ((w <= 0) || (h <= 0))
      return;
   gc = gdk_gc_new(p);
   gdk_imlib_render(im, w, h);
   pp = gdk_imlib_move_image(im);
   mm = gdk_imlib_move_mask(im);
   if (mm)
     {
	gdk_gc_set_clip_mask(gc, mm);
	gdk_gc_set_clip_origin(gc, x, y);
     }
   gdk_draw_pixmap(p, gc, pp, 0, 0, x, y, w, h);
   gdk_imlib_free_pixmap(pp);
   gdk_gc_destroy(gc);
}

void
gdk_imlib_paste_image_border(GdkImlibImage * im, GdkWindow * p, gint x, gint y, gint w, gint h)
{
   GdkGC              *gc;
   GdkPixmap          *pp, *mm;

   if (!im)
      return;

   if (w <= 0 || h <= 0)
      return;
   gc = gdk_gc_new(p);
   gdk_imlib_render(im, w, h);
   pp = gdk_imlib_move_image(im);
   mm = gdk_imlib_move_mask(im);
   if (mm)
     {
	gdk_gc_set_clip_mask(gc, mm);
	gdk_gc_set_clip_origin(gc, x, y);
     }
   if ((w <= (im->border.left + im->border.right)) ||
       (h <= (im->border.top + im->border.bottom)))
      gdk_draw_pixmap(p, gc, pp, 0, 0, x, y, w, h);
   else
     {
	gdk_draw_pixmap(p, gc, pp, 0, 0, x, y, w, im->border.top);
	gdk_draw_pixmap(p, gc, pp, 0, h - im->border.bottom,
			x, y + (h - im->border.bottom),
			w, im->border.bottom);
	gdk_draw_pixmap(p, gc, pp, 0, im->border.top,
			x, y + im->border.top,
		   im->border.left, h - (im->border.top + im->border.bottom));
	gdk_draw_pixmap(p, gc, pp, w - im->border.right, h - im->border.bottom,
		      x + (w - im->border.right), y + (h - im->border.bottom),
		  im->border.right, h - (im->border.top + im->border.bottom));
     }
   gdk_imlib_free_pixmap(pp);
   gdk_gc_destroy(gc);
}

void
gdk_imlib_flip_image_horizontal(GdkImlibImage * im)
{
   unsigned char      *ptr1, *ptr2, r, rr;
   int                 x, y;
   int                 w3;

   if (!im)
      return;
   w3 = im->rgb_width * 3;
   for (y = 0; y < im->rgb_height; y++)
     {
	ptr1 = im->rgb_data + (y * w3);
	ptr2 = im->rgb_data + (y * w3) + w3 - 3;
	for (x = 0; x < im->rgb_width >> 1; x++)
	  {
	     r = *ptr1;
	     rr = *ptr2;
	     *ptr2++ = r;
	     *ptr1++ = rr;

	     r = *ptr1;
	     rr = *ptr2;
	     *ptr2++ = r;
	     *ptr1++ = rr;

	     r = *ptr1;
	     rr = *ptr2;
	     *ptr2 = r;
	     *ptr1++ = rr;

	     ptr2 -= 5;
	  }
     }
   w3 = im->border.left;
   im->border.left = im->border.right;
   im->border.right = w3;
   gdirty_images(im);
   gdirty_pixmaps(im);
}

void
gdk_imlib_flip_image_vertical(GdkImlibImage * im)
{
   unsigned char      *ptr1, *ptr2, r, rr;
   int                 x, y, yy;
   int                 w3;

   if (!im)
      return;

   w3 = im->rgb_width * 3;
   for (yy = im->rgb_height - 1, y = 0; y < im->rgb_height >> 1; y++, yy--)
     {
	ptr1 = im->rgb_data + (y * w3);
	ptr2 = im->rgb_data + (yy * w3);
	for (x = 0; x < im->rgb_width; x++)
	  {
	     r = *ptr1;
	     rr = *ptr2;
	     *ptr2++ = r;
	     *ptr1++ = rr;
	     r = *ptr1;
	     rr = *ptr2;
	     *ptr2++ = r;
	     *ptr1++ = rr;
	     r = *ptr1;
	     rr = *ptr2;
	     *ptr2++ = r;
	     *ptr1++ = rr;
	  }
     }
   w3 = im->border.top;
   im->border.top = im->border.bottom;
   im->border.bottom = w3;
   gdirty_images(im);
   gdirty_pixmaps(im);
}

void
gdk_imlib_rotate_image(GdkImlibImage * im, gint d)
{
   unsigned char      *data;
   int                 x, y, w3, w4;
   unsigned char      *ptr1, *ptr2;

   if (!im)
      return;
   w3 = im->rgb_width * 3;
   w4 = im->rgb_height * 3;

   data = malloc(im->rgb_width * im->rgb_height * 3);
   if (data == NULL)
      return;

   for (y = 0; y < im->rgb_height; y++)
     {
	ptr1 = im->rgb_data + (y * w3);
	ptr2 = data + (y * 3);
	for (x = 0; x < im->rgb_width; x++)
	  {
	     *ptr2++ = *ptr1++;
	     *ptr2++ = *ptr1++;
	     *ptr2 = *ptr1++;
	     ptr2 += w4 - 2;
	  }
     }
   free(im->rgb_data);
   im->rgb_data = data;
   w3 = im->rgb_width;
   im->rgb_width = im->rgb_height;
   im->rgb_height = w3;
   w3 = im->border.top;
   im->border.top = im->border.left;
   im->border.left = w3;
   w3 = im->border.bottom;
   im->border.bottom = im->border.right;
   im->border.right = w3;
   gdirty_images(im);
   gdirty_pixmaps(im);
}

GdkImlibImage      *
gdk_imlib_create_image_from_data(unsigned char *data, unsigned char *alpha, gint w, gint h)
{
   GdkImlibImage      *im;
   char                s[128];

   if (!data || w <= 0 || h <= 0)
      return NULL;

   im = malloc(sizeof(GdkImlibImage));
   if (!im)
      return NULL;

   im->rgb_width = w;
   im->rgb_height = h;
   im->rgb_data = malloc(im->rgb_width * im->rgb_height * 3);
   if (!im->rgb_data)
     {
	free(im);
	return NULL;
     }

   memcpy(im->rgb_data, data, im->rgb_width * im->rgb_height * 3);
/*      im->alpha_data=alpha; */
   im->alpha_data = NULL;
   g_snprintf(s, sizeof(s), "creat_%x_%x", (int)time(NULL), (int)rand());
   im->filename = malloc(strlen(s) + 1);
   if (im->filename)
      strcpy(im->filename, s);
   im->width = 0;
   im->height = 0;
   im->shape_color.r = -1;
   im->shape_color.g = -1;
   im->shape_color.b = -1;
   im->border.left = 0;
   im->border.right = 0;
   im->border.top = 0;
   im->border.bottom = 0;
   im->pixmap = NULL;
   im->shape_mask = NULL;
   im->cache = 1;
   im->mod.gamma = id->mod.gamma;
   im->mod.brightness = id->mod.brightness;
   im->mod.contrast = id->mod.contrast;
   im->rmod.gamma = id->rmod.gamma;
   im->rmod.brightness = id->rmod.brightness;
   im->rmod.contrast = id->rmod.contrast;
   im->gmod.gamma = id->gmod.gamma;
   im->gmod.brightness = id->gmod.brightness;
   im->gmod.contrast = id->gmod.contrast;
   im->bmod.gamma = id->bmod.gamma;
   im->bmod.brightness = id->bmod.brightness;
   im->bmod.contrast = id->bmod.contrast;
   if (id->cache.on_image)
      gadd_image(im, im->filename);
   gcalc_map_tables(im);
   return im;
}

GdkImlibImage      *
gdk_imlib_clone_image(GdkImlibImage * im)
{
   GdkImlibImage      *im2;
   char               *s;

   if (!im)
      return NULL;
   im2 = malloc(sizeof(GdkImlibImage));
   if (!im2)
      return NULL;
   im2->rgb_width = im->rgb_width;
   im2->rgb_height = im->rgb_height;
   im2->rgb_data = malloc(im2->rgb_width * im2->rgb_height * 3);
   if (!im2->rgb_data)
     {
	free(im2);
	return NULL;
     }

   memcpy(im2->rgb_data, im->rgb_data, im2->rgb_width * im2->rgb_height * 3);
   if (im->alpha_data)
     {
	im2->alpha_data = malloc(im2->rgb_width * im2->rgb_height);
	if (!im2->alpha_data)
	  {
	     free(im2->rgb_data);
	     free(im2);
	     return NULL;
	  }
	memcpy(im2->alpha_data, im->alpha_data, im2->rgb_width * im2->rgb_height);
     }
   else
      im2->alpha_data = NULL;
   s = malloc(strlen(im->filename) + 320);
   if (s)
     {
	g_snprintf(s, sizeof(s), "%s_%x_%x", im->filename, (int)time(NULL), (int)rand());
	im2->filename = malloc(strlen(s) + 1);
	if (im2->filename)
	   strcpy(im2->filename, s);
	free(s);
     }
   else
      im2->filename = NULL;
   im2->width = 0;
   im2->height = 0;
   im2->shape_color.r = im->shape_color.r;
   im2->shape_color.g = im->shape_color.g;
   im2->shape_color.b = im->shape_color.b;
   im2->border.left = im->border.left;
   im2->border.right = im->border.right;
   im2->border.top = im->border.top;
   im2->border.bottom = im->border.bottom;
   im2->pixmap = NULL;
   im2->shape_mask = NULL;
   im2->cache = 1;
   im2->mod.gamma = im->mod.gamma;
   im2->mod.brightness = im->mod.brightness;
   im2->mod.contrast = im->mod.contrast;
   im2->rmod.gamma = im->rmod.gamma;
   im2->rmod.brightness = im->rmod.brightness;
   im2->rmod.contrast = im->rmod.contrast;
   im2->gmod.gamma = im->gmod.gamma;
   im2->gmod.brightness = im->gmod.brightness;
   im2->gmod.contrast = im->gmod.contrast;
   im2->bmod.gamma = im->bmod.gamma;
   im2->bmod.brightness = im->bmod.brightness;
   im2->bmod.contrast = im->bmod.contrast;
   gcalc_map_tables(im2);
   if (id->cache.on_image)
      gadd_image(im2, im2->filename);
   return im2;
}

GdkImlibImage      *
gdk_imlib_clone_scaled_image(GdkImlibImage * im, int w, int h)
{
   GdkImlibImage      *im2;
   char               *s;

   if (!im || w <= 0 || h <= 0)
      return NULL;

   im2 = malloc(sizeof(GdkImlibImage));
   if (!im2)
      return NULL;
   im2->rgb_width = w;
   im2->rgb_height = h;
   im2->rgb_data = malloc(w * h * 3);
   if (!im2->rgb_data)
     {
	free(im2);
	return NULL;
     }
   {
      int                 x, y, *xarray;
      unsigned char     **yarray, *ptr, *ptr2, *ptr22;
      int                 l, r, m, pos, inc, w3;

      xarray = malloc(sizeof(int) * w);

      if (!xarray)
	{
	   fprintf(stderr, "ERROR: Cannot allocate X co-ord buffer\n");
	   free(im2->rgb_data);
	   free(im2);
	   return NULL;
	}
      yarray = malloc(sizeof(unsigned char *) * h);

      if (!yarray)
	{
	   fprintf(stderr, "ERROR: Cannot allocate Y co-ord buffer\n");
	   free(xarray);
	   free(im2->rgb_data);
	   free(im2);
	   return NULL;
	}
      ptr22 = im->rgb_data;
      w3 = im->rgb_width * 3;
      inc = 0;
      if (w < im->border.left + im->border.right)
	{
	   l = w >> 1;
	   r = w - l;
	   m = 0;
	}
      else
	{
	   l = im->border.left;
	   r = im->border.right;
	   m = w - l - r;
	}
      if (m > 0)
	 inc = ((im->rgb_width - im->border.left - im->border.right) << 16) / m;
      pos = 0;
      if (l)
	{
	   for (x = 0; x < l; x++)
	     {
		xarray[x] = (pos >> 16) + (pos >> 16) + (pos >> 16);
		pos += 0x10000;
	     }
	}
      if (m)
	{
	   for (x = l; x < l + m; x++)
	     {
		xarray[x] = (pos >> 16) + (pos >> 16) + (pos >> 16);
		pos += inc;
	     }
	}
      pos = (im->rgb_width - r) << 16;
      for (x = w - r; x < w; x++)
	{
	   xarray[x] = (pos >> 16) + (pos >> 16) + (pos >> 16);
	   pos += 0x10000;
	}

      if (h < im->border.top + im->border.bottom)
	{
	   l = h >> 1;
	   r = h - l;
	   m = 0;
	}
      else
	{
	   l = im->border.top;
	   r = im->border.bottom;
	   m = h - l - r;
	}
      if (m > 0)
	 inc = ((im->rgb_height - im->border.top - im->border.bottom) << 16) / m;
      pos = 0;
      for (x = 0; x < l; x++)
	{
	   yarray[x] = ptr22 + ((pos >> 16) * w3);
	   pos += 0x10000;
	}
      if (m)
	{
	   for (x = l; x < l + m; x++)
	     {
		yarray[x] = ptr22 + ((pos >> 16) * w3);
		pos += inc;
	     }
	}
      pos = (im->rgb_height - r) << 16;
      for (x = h - r; x < h; x++)
	{
	   yarray[x] = ptr22 + ((pos >> 16) * w3);
	   pos += 0x10000;
	}

      ptr = im2->rgb_data;
      for (y = 0; y < h; y++)
	{
	   for (x = 0; x < w; x++)
	     {
		ptr2 = yarray[y] + xarray[x];
		*ptr++ = (int)*ptr2++;
		*ptr++ = (int)*ptr2++;
		*ptr++ = (int)*ptr2;
	     }
	}
   }
   if (im->alpha_data)
     {
	im2->alpha_data = NULL;
	/* yet to be filled in */
     }
   else
      im2->alpha_data = NULL;

   s = malloc(strlen(im->filename) + 320);
   if (s)
     {
	g_snprintf(s, sizeof(s), "%s_%x_%x_%x_%x", im->filename, (int)time(NULL), w, h, (int)rand());
	im2->filename = malloc(strlen(s) + 1);
	if (im2->filename)
	   strcpy(im2->filename, s);
	free(s);
     }
   else
      im2->filename = NULL;
   im2->width = 0;
   im2->height = 0;
   im2->shape_color.r = im->shape_color.r;
   im2->shape_color.g = im->shape_color.g;
   im2->shape_color.b = im->shape_color.b;
   im2->border.left = im->border.left;
   im2->border.right = im->border.right;
   im2->border.top = im->border.top;
   im2->border.bottom = im->border.bottom;
   im2->pixmap = NULL;
   im2->shape_mask = NULL;
   im2->cache = 1;
   im2->mod.gamma = im->mod.gamma;
   im2->mod.brightness = im->mod.brightness;
   im2->mod.contrast = im->mod.contrast;
   im2->rmod.gamma = im->rmod.gamma;
   im2->rmod.brightness = im->rmod.brightness;
   im2->rmod.contrast = im->rmod.contrast;
   im2->gmod.gamma = im->gmod.gamma;
   im2->gmod.brightness = im->gmod.brightness;
   im2->gmod.contrast = im->gmod.contrast;
   im2->bmod.gamma = im->bmod.gamma;
   im2->bmod.brightness = im->bmod.brightness;
   im2->bmod.contrast = im->bmod.contrast;
   gcalc_map_tables(im2);
   if (id->cache.on_image)
      gadd_image(im2, im2->filename);
   return im2;
}

GdkImlibImage      *
gdk_imlib_create_image_from_xpm_data(char **data)
{
   GdkImlibImage      *im;
   unsigned char      *ptr;
   int                 pc, c, i, j, k, ncolors, cpp, comment, transp, quote,
                       context, len, count, done;
   int                 w, h;
   char               *line, s[65536], tok[65536], col[65536];
   XColor              xcol;
   struct _cmap
     {
	char                str[8];
	char                transp;
	int                 r, g, b;
     }
                      *cmap;
   int                 lookup[128][128];

   if (!data)
      return NULL;
   im = malloc(sizeof(GdkImlibImage));
   if (!im)
      return NULL;
   count = 0;
   transp = 0;
   done = 0;

   c = ' ';
   comment = 0;
   quote = 0;
   context = 0;
   ptr = NULL;

   while (!done)
     {
	line = data[count++];
	if (context == 0)
	  {
	     /* Header */
	     sscanf(line, "%i %i %i %i", &w, &h, &ncolors, &cpp);
	     if (cpp > 7)
	       {
		  fprintf(stderr, "gdk_imlib ERROR: XPM data with characters per pixel > 7 not supported\n");
		  free(im);
		  return NULL;
	       }
	     if (w > 32767)
	       {
		  fprintf(stderr, "gdk_imlib ERROR: Image width > 32767 pixels for data\n");
		  free(im);
		  return NULL;
	       }
	     if (h > 32767)
	       {
		  fprintf(stderr, "gdk_imlib ERROR: Image height > 32767 pixels for data\n");
		  free(im);
		  return NULL;
	       }
	     cmap = malloc(sizeof(struct _cmap) * ncolors);

	     if (!cmap)
	       {
		  free(im);
		  return NULL;
	       }
	     im->rgb_width = w;
	     im->rgb_height = h;
	     im->rgb_data = malloc(im->rgb_width * im->rgb_height * 3);
	     if (!im->rgb_data)
	       {
		  free(cmap);
		  free(im);
		  return NULL;
	       }
	     im->alpha_data = NULL;
	     g_snprintf(s, sizeof(s), "creat_%x_%x", (int)time(NULL), (int)rand());
	     im->filename = malloc(strlen(s) + 1);
	     if (im->filename)
		strcpy(im->filename, s);
	     im->width = 0;
	     im->height = 0;
	     im->border.left = 0;
	     im->border.right = 0;
	     im->border.top = 0;
	     im->border.bottom = 0;
	     im->pixmap = NULL;
	     im->shape_mask = NULL;
	     im->cache = 1;
	     im->mod.gamma = id->mod.gamma;
	     im->mod.brightness = id->mod.brightness;
	     im->mod.contrast = id->mod.contrast;
	     im->rmod.gamma = id->rmod.gamma;
	     im->rmod.brightness = id->rmod.brightness;
	     im->rmod.contrast = id->rmod.contrast;
	     im->gmod.gamma = id->gmod.gamma;
	     im->gmod.brightness = id->gmod.brightness;
	     im->gmod.contrast = id->gmod.contrast;
	     im->bmod.gamma = id->bmod.gamma;
	     im->bmod.brightness = id->bmod.brightness;
	     im->bmod.contrast = id->bmod.contrast;
	     ptr = im->rgb_data;
	     j = 0;
	     context++;
	  }
	else if (context == 1)
	  {
	     int                 colptr;

	     /* Color Table */
	     if (j < ncolors)
	       {
		  tok[0] = 0;
		  col[0] = 0;
		  s[0] = 0;
		  colptr = 0;
		  len = strlen(line);
		  strncpy(cmap[j].str, line, cpp);
		  cmap[j].str[cpp] = 0;
		  cmap[j].r = -1;
		  cmap[j].transp = 0;
		  for (k = cpp; k < len; k++)
		    {
		       if (line[k] != ' ')
			 {
			    sscanf(&line[k], "%65536s", s);
			    k += strlen(s);
			    if ((!strcmp(s, "m")) || (!strcmp(s, "s")) ||
				(!strcmp(s, "g4")) || (!strcmp(s, "g")) ||
				(!strcmp(s, "c")) || (k >= len))
			      {
				 if (k >= len)
				   {
				      int                 ls;

				      ls = strlen(s);

				      if (col[0] && colptr < sizeof(col))
					{
					   strcpy(col + colptr, " ");
					   colptr++;
					}
				      if (colptr + ls <= sizeof(col))
					{
					   strcpy(col + colptr, s);
					   colptr += ls;
					}

				   }
				 if (col[0])
				   {
				      if (!strcasecmp(col, "none"))
					{
					   transp = 1;
					   cmap[j].transp = 1;
					}
				      else
					{
					   if ((cmap[j].r < 0) ||
					       (!strcmp(tok, "c")))
					     {
						XParseColor(id->x.disp,
							    id->x.root_cmap,
							    col, &xcol);
						cmap[j].r = xcol.red >> 8;
						cmap[j].g = xcol.green >> 8;
						cmap[j].b = xcol.blue >> 8;
						if ((cmap[j].r == 255) &&
						    (cmap[j].g == 0) &&
						    (cmap[j].b == 255))
						   cmap[j].r = 254;
					     }
					}
				   }
				 if (strlen(s) < sizeof(tok))
				    strcpy(tok, s);
				 col[0] = 0;
			      }
			    else
			      {
				 int                 ls;

				 ls = strlen(s);

				 if (col[0] && colptr < sizeof(col))
				   {
				      strcpy(col + colptr, " ");
				      colptr++;
				   }
				 if (ls + colptr < sizeof(col))
				   {
				      strcpy(col + colptr, s);
				      colptr += ls;
				   }
			      }
			 }
		    }
	       }
	     j++;
	     if (j >= ncolors)
	       {
		  if (cpp == 1)
		     for (i = 0; i < ncolors; i++)
			lookup[cmap[i].str[0]][cmap[i].str[1]] = i;
		  if (cpp == 2)
		     for (i = 0; i < ncolors; i++)
			lookup[cmap[i].str[0]][cmap[i].str[1]] = i;
		  context++;
	       }
	  }
	else
	  {
	     /* Image Data */
	     i = 0;
	     if (cpp == 0)
	       {
	       }
	     else if (cpp == 1)
	       {
		  if (transp)
		    {
		       for (i = 0; ((i < 65536) && (line[i])); i++)
			 {
			    col[0] = line[i];
			    if (cmap[lookup[col[0]][0]].transp)
			      {
				 *ptr++ = 255;
				 *ptr++ = 0;
				 *ptr++ = 255;
			      }
			    else
			      {
				 *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].r;
				 *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].g;
				 *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].b;
			      }
			 }
		    }
		  else
		    {
		       for (i = 0; ((i < 65536) && (line[i])); i++)
			 {
			    col[0] = line[i];
			    *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].r;
			    *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].g;
			    *ptr++ = (unsigned char)cmap[lookup[col[0]][0]].b;
			 }
		    }
	       }
	     else if (cpp == 2)
	       {
		  if (transp)
		    {
		       for (i = 0; ((i < 65536) && (line[i])); i++)
			 {
			    col[0] = line[i++];
			    col[1] = line[i];
			    if (cmap[lookup[col[0]][col[1]]].transp)
			      {
				 *ptr++ = 255;
				 *ptr++ = 0;
				 *ptr++ = 255;
			      }
			    else
			      {
				 *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].r;
				 *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].g;
				 *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].b;
			      }
			 }
		    }
		  else
		    {
		       for (i = 0; ((i < 65536) && (line[i])); i++)
			 {
			    col[0] = line[i++];
			    col[1] = line[i];
			    *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].r;
			    *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].g;
			    *ptr++ = (unsigned char)cmap[lookup[col[0]][col[1]]].b;
			 }
		    }
	       }
	     else
	       {
		  if (transp)
		    {
		       for (i = 0; ((i < 65536) && (line[i])); i++)
			 {
			    for (j = 0; j < cpp; j++, i++)
			      {
				 col[j] = line[i];
			      }
			    col[j] = 0;
			    i--;
			    for (j = 0; j < ncolors; j++)
			      {
				 if (!strcmp(col, cmap[j].str))
				   {
				      if (cmap[j].transp)
					{
					   *ptr++ = 255;
					   *ptr++ = 0;
					   *ptr++ = 255;
					}
				      else
					{
					   *ptr++ = (unsigned char)cmap[j].r;
					   *ptr++ = (unsigned char)cmap[j].g;
					   *ptr++ = (unsigned char)cmap[j].b;
					}
				      j = ncolors;
				   }
			      }
			 }
		    }
		  else
		    {
		       for (i = 0; ((i < 65536) && (line[i])); i++)
			 {
			    for (j = 0; j < cpp; j++, i++)
			      {
				 col[j] = line[i];
			      }
			    col[j] = 0;
			    i--;
			    for (j = 0; j < ncolors; j++)
			      {
				 if (!strcmp(col, cmap[j].str))
				   {
				      *ptr++ = (unsigned char)cmap[j].r;
				      *ptr++ = (unsigned char)cmap[j].g;
				      *ptr++ = (unsigned char)cmap[j].b;
				      j = ncolors;
				   }
			      }
			 }
		    }
	       }
	  }
	if ((ptr) && ((ptr - im->rgb_data) >= w * h * 3))
	   done = 1;
     }
   if (!transp)
     {
	im->shape_color.r = -1;
	im->shape_color.g = -1;
	im->shape_color.b = -1;
     }
   else
     {
	im->shape_color.r = 255;
	im->shape_color.g = 0;
	im->shape_color.b = 255;
     }
   if (id->cache.on_image)
      gadd_image(im, im->filename);
   gcalc_map_tables(im);
   free(cmap);
   return im;
}

gint
gdk_imlib_data_to_pixmap(char **data, GdkPixmap ** pmap, GdkBitmap ** mask)
{
   GdkImlibImage      *im;

   im = gdk_imlib_create_image_from_xpm_data(data);
   if (!im)
     {
	if (pmap)
	   *pmap = NULL;
	if (mask)
	   *mask = NULL;
	return 0;
     }
   if (!gdk_imlib_render(im, im->rgb_width, im->rgb_height))
     {
	gdk_imlib_destroy_image(im);
	if (pmap)
	   *pmap = NULL;
	if (mask)
	   *mask = NULL;
	return 0;
     }
   if (pmap)
      *pmap = gdk_imlib_move_image(im);
   if (mask)
      *mask = gdk_imlib_move_mask(im);
   gdk_imlib_kill_image(im);
   return 1;
}

#include <signal.h>
#include <sys/wait.h>

/*
 *    Helper library
 */

static int          hpid;
void               *oldpiper;	/* actually sighandler_t but BSD uses sig_t. */

FILE               *
open_helper(const char *instring, const char *fn, const char *mode)
{
   char                buf[256];	/* This is safe since our input strings

					 * 
					 * * are bounded */
   static char        *vec[16];
   char               *p = strdup(instring);
   char               *pp;
   char               *ep;
   int                 vn = 0;
   int                 pid;
   FILE               *fp = NULL;
   char               *ofil = NULL;
   int                 ofd = -1;

   int                 pfd[2];

   if (p == NULL)
      return NULL;

   if (strncmp(instring, "%Q", 2) == 0)
     {
	/*
	 *    Generate a quanting pipeline
	 */
	fprintf(stderr, "Not currently supported: install ImageMagic.\n");
	return NULL;
     }
   /*
    *    Ok split the instring on spaces and translate
    *      %C %P %F and %s
    *
    *      FIXME: We need to handle a format string that begins
    *      %Q to indicate an 8bit quant in the pipeline first.
    */

   pp = p;

   while (vn < 15)
     {
	while (*pp && isspace(*pp))
	   pp++;
	ep = pp;
	while (*ep && !isspace(*ep))
	   ep++;
	if (*pp == 0)
	   break;
	/* pp->ep is now the input string block */
	if (*ep)
	   *ep++ = 0;

	if (strcmp(pp, "%s") == 0)
	   vec[vn] = strdup(fn);
	else if (strncmp(pp, "%P/", 3) == 0)
	  {
	     strcpy(buf, NETPBM_PATH);
	     strcat(buf, pp + 2);
	     if ((vec[vn] = strdup(buf)) == NULL)
		break;
	  }
	else if (strncmp(pp, "%J", 3) == 0)
	  {
	     if ((vec[vn] = strdup(DJPEG_PROG)) == NULL)
		break;
	  }
	else if (strncmp(pp, "%H", 3) == 0)
	  {
	     if ((vec[vn] = strdup(CJPEG_PROG)) == NULL)
		break;
	  }
	else if (strncmp(pp, "%C/", 3) == 0)
	  {
	     strcpy(buf, CONVERT_PATH);
	     strcat(buf, pp + 2);
	     if ((vec[vn] = strdup(buf)) == NULL)
		break;
	  }
	else if (strncmp(pp, ">%s", 3) == 0)
	  {
	     ofil = pp;
	     vn++;
	     pp = ep;
	     continue;
	  }
	else
	  {
	     if ((vec[vn] = strdup(pp)) == NULL)
		break;
	  }
	vn++;
	pp = ep;
     }

   vec[vn] = NULL;

   if (pipe(pfd) == -1)
      goto oops;

   if (*mode == 'r')
     {
	fp = fdopen(pfd[0], "r");
	if (fp == NULL)
	   goto oops;
     }
   else if (*mode == 'w')
     {
	fp = fdopen(pfd[1], "w");
	if (fp == NULL)
	   goto oops;
     }
   else
      goto oops;

   if (ofil != NULL)
      if ((ofd = open(ofil, O_WRONLY | O_TRUNC | O_CREAT)) == -1)
	 goto oops;

   switch (pid = fork())
     {
     case -1:
	break;
     case 0:
	signal(SIGPIPE, SIG_DFL);
	if (*mode == 'r')
	   dup2(pfd[1], 1);
	if (*mode == 'w')
	  {
	     dup2(pfd[0], 0);
	     if (ofd != -1)
	       {
		  dup2(ofd, 1);
		  close(1);
	       }
	  }
	close(pfd[0]);
	close(pfd[1]);
	execv(vec[0], vec);
	perror(vec[0]);
	/*
	 *    This MUST be _exit or we will hit the SIGPIPE
	 *      handler in ways we dont want. We want our parent
	 *      to flush the inherited file buffers not us.
	 */
	_exit(1);
     default:
	hpid = pid;

	if (ofd != -1)
	   close(ofd);
	if (*mode == 'r')
	   close(pfd[1]);
	else
	   close(pfd[0]);
     }
   for (vn = 0; vn < 16; vn++)
      if (vec[vn])
	 free(vec[vn]);
   oldpiper = signal(SIGPIPE, SIG_IGN);
   return fp;

 oops:
   if (ofd != -1)
      close(ofd);
   if (fp)
      fclose(fp);
   for (vn = 0; vn < 16; vn++)
      if (vec[vn])
	 free(vec[vn]);
   return NULL;
}

int
close_helper(FILE * fp)
{
   int                 info;

   fclose(fp);
   signal(SIGPIPE, oldpiper);
   waitpid(hpid, &info, 0);
   return WEXITSTATUS(info);
}
