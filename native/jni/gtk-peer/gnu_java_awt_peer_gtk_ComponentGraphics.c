/* gnu_java_awt_peer_gtk_ComponentGraphics.c
   Copyright (C) 2006 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

#include "jcl.h"
#include "gtkpeer.h"
#include <cairo-xlib.h>
#include <gdk/gdktypes.h>
#include <gdk/gdkprivate.h>
#include <gdk/gdkx.h>
#include <X11/extensions/Xrender.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf/gdk-pixdata.h>

#include <cairo-ft.h>
#include <cairo-xlib.h>

#include <stdio.h>
#include <stdlib.h>

#include "gnu_java_awt_peer_gtk_ComponentGraphics.h"


static void 
cp_java_awt_peer_gtk_ComponentGraphics_grab_current_drawable(GtkWidget *widget,
	GdkDrawable **draw, GdkWindow **win)
{  
  g_assert (widget != NULL);
  g_assert (draw != NULL);
  g_assert (win != NULL);

  *win = widget->window;

  *draw = *win;
  gdk_window_get_internal_paint_info (*win, draw, 0, 0); 
  /*  g_object_ref (*draw); */
  /* FIXME: Unref this. */
}

/**
 * Returns whether the XRender extension is supported
 */
JNIEXPORT jboolean JNICALL 
Java_gnu_java_awt_peer_gtk_ComponentGraphics_hasXRender
  (JNIEnv *env __attribute__ ((unused)), jclass cls __attribute__ ((unused)))
{ 
  int ev = 0, err = 0; 
  if( XRenderQueryExtension (GDK_DISPLAY (), &ev, &err) )
    return JNI_TRUE;
  return JNI_FALSE;
}


JNIEXPORT jlong JNICALL 
Java_gnu_java_awt_peer_gtk_ComponentGraphics_initState
  (JNIEnv *env, jobject obj __attribute__ ((unused)), jobject peer)
{
  Drawable draw;
  Display * dpy;
  Visual * vis;
  GdkDrawable *drawable;
  cairo_surface_t *surface;
  GdkWindow *win;
  GtkWidget *widget = NULL;
  void *ptr = NULL;
  int width, height;
  cairo_t *cr;

  gdk_threads_enter();

  ptr = NSA_GET_PTR (env, peer);
  g_assert (ptr != NULL);

  widget = GTK_WIDGET (ptr);
  g_assert (widget != NULL);

  cp_java_awt_peer_gtk_ComponentGraphics_grab_current_drawable (widget, &drawable, &win);
  g_assert (drawable != NULL);

  width = widget->allocation.width;
  height = widget->allocation.height;

  g_assert (drawable != NULL);

  draw = gdk_x11_drawable_get_xid(drawable);
  g_assert (draw != (XID) 0);
  
  dpy = gdk_x11_drawable_get_xdisplay(drawable);
  g_assert (dpy != NULL);
  
  vis = gdk_x11_visual_get_xvisual(gdk_drawable_get_visual(drawable));
  g_assert (vis != NULL);
  
  surface = cairo_xlib_surface_create (dpy, draw, vis, width, height);
  g_assert (surface != NULL);

  cr = cairo_create (surface);
  g_assert(cr != NULL);

  gdk_threads_leave();

  return PTR_TO_JLONG(cr);
}

JNIEXPORT void JNICALL 
Java_gnu_java_awt_peer_gtk_ComponentGraphics_start_1gdk_1drawing
  (JNIEnv *env __attribute__ ((unused)), jobject obj __attribute__ ((unused)))
{
  gdk_threads_enter();
}

JNIEXPORT void JNICALL 
Java_gnu_java_awt_peer_gtk_ComponentGraphics_end_1gdk_1drawing
  (JNIEnv *env __attribute__ ((unused)), jobject obj __attribute__ ((unused)))
{
  gdk_threads_leave();
}
