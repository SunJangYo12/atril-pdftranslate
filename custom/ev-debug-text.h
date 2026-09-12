#ifndef EV_DEBUG_TEXT_H
#define EV_DEBUG_TEXT_H

#include "ev-page-cache.h"

void ev_debug_dump_page_text (EvPageCache *cache,
                              gint          page);

void ev_debug_dump_blocks (EvPageCache *cache,
                              gint          page);

#endif
