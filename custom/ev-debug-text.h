#ifndef EV_DEBUG_TEXT_H
#define EV_DEBUG_TEXT_H

#include <cairo.h>
#include <gdk/gdk.h>
#include "ev-page-cache.h"

gboolean
ev_view_point_in_translate_resize_handle (EvView  *view,
                                          gdouble  x,
                                          gdouble  y);

gboolean
ev_view_point_in_translate_overlay (EvView *view,
                                    gdouble x,
                                    gdouble y);
void
ev_debug_draw_blocks (EvPageCache      *cache, EvView *view,
                      cairo_t          *cr,
                      gint              page,
                      GdkRectangle     *real_page_area,
                      cairo_surface_t  *page_surface,
                   	  gdouble          document_width,
                   	  gdouble          document_height);

gboolean ev_debug_is_soft_return (EvPageCache  *cache,
                                  EvRectangle  *areas,
                                  guint         n_areas,
                                  PangoLogAttr *log_attrs,
                                  gint          offset);

void ev_debug_dump_page_text (EvPageCache *cache,
                              gint          page);

void ev_debug_dump_blocks (EvPageCache *cache,
                              gint          page);

#endif
