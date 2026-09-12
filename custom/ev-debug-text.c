#include "config.h"

#include <glib.h>
#include <string.h>

#include "ev-debug-text.h"
#include "ev-page-cache.h"

void
ev_debug_dump_page_text (EvPageCache *cache,
                         gint         page)
{
	const gchar *text;
	EvRectangle *areas = NULL;
	PangoLogAttr *log_attrs = NULL;
	guint n_areas = 0;
	gulong n_attrs = 0;

	const gchar *p;
	guint offset = 0;

	if (!cache) {
		g_print ("DEBUG TEXT: page cache unavailable\n");
		return;
	}

	g_print ("\n");
	g_print ("============================================================\n");
	g_print ("TEXT DEBUG: PAGE %d\n", page);
	g_print ("============================================================\n");

	text = ev_page_cache_get_text (cache, page);

	if (!text) {
		g_print ("TEXT = NULL\n");
		return;
	}

	g_print ("TEXT BYTES = %zu\n", strlen (text));
	g_print ("TEXT:\n%s\n", text);

	if (!ev_page_cache_get_text_layout (cache,
					    page,
					    &areas,
					    &n_areas)) {
		g_print ("TEXT LAYOUT = unavailable\n");
		return;
	}

	if (!ev_page_cache_get_text_log_attrs (cache,
					       page,
					       &log_attrs,
					       &n_attrs)) {
		g_print ("TEXT LOG ATTRS = unavailable\n");
		return;
	}

	g_print ("\n");
	g_print ("AREAS = %u\n", n_areas);
	g_print ("ATTRS = %lu\n", n_attrs);

	g_print ("\n");
	g_print ("--------------------------------------------------------------------------\n");
	g_print ("OFF  CHAR   CODE     X1       Y1       X2       Y2"
		 "   WHITE WS WE MB\n");
	g_print ("--------------------------------------------------------------------------\n");

	p = text;

	while (*p != '\0') {
		gunichar ch;
		gchar utf8_char[8];
		gint len;

		ch = g_utf8_get_char (p);

		len = g_unichar_to_utf8 (ch, utf8_char);
		utf8_char[len] = '\0';

		if (offset >= n_areas || offset >= n_attrs)
			break;

		if (ch == '\n')
			g_strlcpy (utf8_char, "\\n", sizeof (utf8_char));
		else if (ch == '\r')
			g_strlcpy (utf8_char, "\\r", sizeof (utf8_char));
		else if (ch == '\t')
			g_strlcpy (utf8_char, "\\t", sizeof (utf8_char));
		else if (ch == ' ')
			g_strlcpy (utf8_char, "SP", sizeof (utf8_char));
		else if (g_unichar_iscntrl (ch))
			g_strlcpy (utf8_char, "CTRL", sizeof (utf8_char));

		g_print ("%4u  %-5s U+%04X  "
			 "%8.2f %8.2f %8.2f %8.2f  "
			 "%5d %2d %2d %2d\n",

			 offset,
			 utf8_char,
			 ch,

			 areas[offset].x1,
			 areas[offset].y1,
			 areas[offset].x2,
			 areas[offset].y2,

			 log_attrs[offset].is_white,
			 log_attrs[offset].is_word_start,
			 log_attrs[offset].is_word_end,
			 log_attrs[offset].is_mandatory_break);

		p = g_utf8_next_char (p);
		offset++;
	}

	g_print ("--------------------------------------------------------------------------\n");

	g_print ("ITERATED = %u\n", offset);
	g_print ("AREAS    = %u\n", n_areas);
	g_print ("ATTRS    = %lu\n", n_attrs);

	g_print ("============================================================\n");
	g_print ("END TEXT DEBUG: PAGE %d\n", page);
	g_print ("============================================================\n");
}
