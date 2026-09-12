#include "config.h"

#include <glib.h>
#include <string.h>

#include "ev-debug-text.h"
#include "ev-page-cache.h"

#include "ev-view.h"

#include <glib.h>
#include <pango/pango.h>


void
ev_debug_draw_blocks (EvPageCache  *cache,
                   cairo_t         *cr,
                   gint             page,
                   GdkRectangle    *real_page_area,
                   cairo_surface_t *page_surface,
				   gdouble		    document_width,
				   gdouble		    document_height)
{
    const gchar *text;
    EvRectangle *areas = NULL;
    PangoLogAttr *log_attrs = NULL;
    guint n_areas = 0;
    gulong n_log_attrs = 0;

    gint n_chars = 0;
    gint start = 0;
    gint block_no = 0;
    gint i;

    gdouble scale_x;
    gdouble scale_y;

    const gchar *p;

    text = ev_page_cache_get_text (cache, page);

    if (!text)
        return;

    if (!ev_page_cache_get_text_layout (cache,
                                        page,
                                        &areas,
                                        &n_areas))
        return;

    if (!ev_page_cache_get_text_log_attrs (cache,
                                           page,
                                           &log_attrs,
                                           &n_log_attrs))
        return;

    if (n_areas == 0 || n_log_attrs == 0)
        return;

    /*
     * Jumlah Unicode character.
     *
     * areas[] dan log_attrs[] menggunakan logical character
     * offset, bukan byte offset UTF-8.
     */
    for (p = text; *p; p = g_utf8_next_char (p))
        n_chars++;

    if (n_chars > (gint)n_areas)
        n_chars = n_areas;

    if (n_chars > (gint)n_log_attrs)
        n_chars = n_log_attrs;

    /*
     * Mapping koordinat text_layout -> koordinat Cairo page.
     *
     * Ini mengikuti cara Atril sendiri melakukan scaling
     * selection region.
     */
    scale_x = (gdouble)real_page_area->width /
				document_width;

    scale_y = (gdouble)real_page_area->height /
				document_height;

    /*
     * Warna sementara untuk debug.
     */
    cairo_save (cr);

    cairo_set_line_width (cr, 2.0);

    /*
     * Cari HARD RETURN dan gambar setiap block.
     */
    for (i = 0; i < n_chars; i++) {
        gunichar c;

        p = g_utf8_offset_to_pointer (text, i);
        c = g_utf8_get_char (p);

        if (c != '\n')
            continue;

        /*
         * Sama persis dengan debug block sebelumnya.
         */
        if (ev_debug_is_soft_return (cache,
                                   areas,
                                   n_areas,
                                   log_attrs,
                                   i))
            continue;

        /*
         * HARD RETURN:
         * gambar block [start, i).
         */
        {
            EvRectangle rect;
            gboolean have_rect = FALSE;
            gint j;

            for (j = start;
                 j < i && j < (gint)n_areas;
                 j++) {

                EvRectangle *a = &areas[j];

                if (a->x1 == a->x2 &&
                    a->y1 == a->y2)
                    continue;

                if (!have_rect) {
                    rect = *a;
                    have_rect = TRUE;
                } else {
                    if (a->x1 < rect.x1)
                        rect.x1 = a->x1;

                    if (a->y1 < rect.y1)
                        rect.y1 = a->y1;

                    if (a->x2 > rect.x2)
                        rect.x2 = a->x2;

                    if (a->y2 > rect.y2)
                        rect.y2 = a->y2;
                }
            }

            if (have_rect) {
                gdouble x;
                gdouble y;
                gdouble width;
                gdouble height;

                x = real_page_area->x +
                    rect.x1 * scale_x;

                y = real_page_area->y +
                    rect.y1 * scale_y;

                width = (rect.x2 - rect.x1) * scale_x;
                height = (rect.y2 - rect.y1) * scale_y;

                /*
                 * Rectangle block.
                 */
                cairo_rectangle (cr,
                                 x,
                                 y,
                                 width,
                                 height);

                cairo_stroke (cr);

                /*
                 * Nomor block.
                 */
				{
				    gchar label[32];

				    g_snprintf (label, sizeof (label),
				                "B%d", block_no);

				    cairo_move_to (cr,
				                   x + 2.0,
				                   y + 12.0);

				    cairo_show_text (cr, label);
				}
                /*cairo_move_to (cr,
                               x + 2.0,
                               y + 12.0);

                cairo_show_text (cr, g_strdup_printf ("B%d", block_no));*/
            }

            block_no++;
            start = i + 1;
        }
    }

    /*
     * Block terakhir setelah HARD RETURN terakhir.
     */
    if (start < n_chars) {
        EvRectangle rect;
        gboolean have_rect = FALSE;
        gint j;

        for (j = start;
             j < n_chars && j < (gint)n_areas;
             j++) {

            EvRectangle *a = &areas[j];

            if (a->x1 == a->x2 &&
                a->y1 == a->y2)
                continue;

            if (!have_rect) {
                rect = *a;
                have_rect = TRUE;
            } else {
                if (a->x1 < rect.x1)
                    rect.x1 = a->x1;

                if (a->y1 < rect.y1)
                    rect.y1 = a->y1;

                if (a->x2 > rect.x2)
                    rect.x2 = a->x2;

                if (a->y2 > rect.y2)
                    rect.y2 = a->y2;
            }
        }

        if (have_rect) {
            gdouble x;
            gdouble y;
            gdouble width;
            gdouble height;

            x = real_page_area->x +
                rect.x1 * scale_x;

            y = real_page_area->y +
                rect.y1 * scale_y;

            width = (rect.x2 - rect.x1) * scale_x;
            height = (rect.y2 - rect.y1) * scale_y;

            cairo_rectangle (cr,
                             x,
                             y,
                             width,
                             height);

            cairo_stroke (cr);

			{
			    gchar label[32];
			    g_snprintf (label, sizeof (label),
			                "B%d", block_no);

			    cairo_move_to (cr,
			                   x + 2.0,
			                   y + 12.0);

			    cairo_show_text (cr, label);
			}
            /*cairo_move_to (cr,
                           x + 2.0,
                           y + 12.0);

            cairo_show_text (cr, g_strdup_printf ("B%d", block_no));*/
        }
    }


    cairo_restore (cr);
}


gboolean
ev_debug_is_soft_return (EvPageCache  *cache,
                      EvRectangle  *areas,
                      guint         n_areas,
                      PangoLogAttr *log_attrs,
                      gint          offset)
{
	EvRectangle *this_line_start;
	EvRectangle *this_line_end;
	EvRectangle *next_line_start;
	EvRectangle *next_line_end;
	EvRectangle *next_word_end;

	gdouble line_spacing;
	gdouble this_line_height;
	gdouble next_word_width;

	gint prev_offset;
	gint next_offset;

	if (!log_attrs[offset].is_white)
		return FALSE;

	if (n_areas <= offset + 1)
		return FALSE;

	prev_offset = offset - 1;
	next_offset = offset + 1;

	/*
	 * Same test as Atril's treat_as_soft_return().
	 */
	if (!log_attrs[next_offset].is_word_start &&
	    (next_offset + 1 >= n_areas ||
	     !log_attrs[next_offset + 1].is_word_start))
		return FALSE;

	this_line_end = areas + prev_offset;
	next_line_start = areas + next_offset;

	this_line_height =
		this_line_end->y2 - this_line_end->y1;

	if (ABS (this_line_height -
		 (next_line_start->y2 - next_line_start->y1)) > 0.25)
		return FALSE;

	line_spacing =
		next_line_start->y1 - this_line_end->y2;

	if (line_spacing - this_line_height > 1)
		return FALSE;

	for (;
	     prev_offset > 0 &&
	     !log_attrs[prev_offset].is_mandatory_break;
	     prev_offset--);

	this_line_start = areas + prev_offset;

	if (ABS (this_line_start->x1 -
		 next_line_start->x1) > 20)
		return FALSE;

	for (;
	     next_offset < n_areas &&
	     !log_attrs[next_offset].is_word_end;
	     next_offset++);

	if (next_offset >= n_areas)
		return FALSE;

	next_word_end = areas + next_offset;

	next_word_width =
		next_word_end->x2 - next_line_start->x1;

	for (;
	     next_offset + 1 < n_areas &&
	     !log_attrs[next_offset + 1].is_mandatory_break;
	     next_offset++);

	next_line_end = areas + next_offset;

	if (next_line_end->x2 -
	    (this_line_end->x2 + next_word_width) > 20)
		return FALSE;

	return TRUE;
}

static void
debug_dump_newlines (EvPageCache  *cache,
                     gint          page)
{
	const gchar *text;
	EvRectangle *areas = NULL;
	PangoLogAttr *log_attrs = NULL;
	guint n_areas = 0;
	gulong n_attrs = 0;

	const gchar *p;
	guint offset = 0;

	text = ev_page_cache_get_text (cache, page);

	if (!text)
		return;

	if (!ev_page_cache_get_text_layout (cache,
					    page,
					    &areas,
					    &n_areas))
		return;

	if (!ev_page_cache_get_text_log_attrs (cache,
					       page,
					       &log_attrs,
					       &n_attrs))
		return;

	g_print ("\n");
	g_print ("============================================================\n");
	g_print ("NEWLINE ANALYSIS: PAGE %d\n", page);
	g_print ("============================================================\n");

	p = text;

	while (*p != '\0' &&
	       offset < n_areas &&
	       offset < n_attrs) {

		gunichar ch;

		ch = g_utf8_get_char (p);

		if (ch == '\n') {
			gboolean soft;
			gint prev;
			gint next;

			prev = (offset > 0) ? offset - 1 : -1;
			next = offset + 1;

			soft = ev_debug_is_soft_return (cache,
						      areas,
						      n_areas,
						      log_attrs,
						      offset);

			g_print ("\n");
			g_print ("NEWLINE @ %u  => %s\n",
				 offset,
				 soft ? "SOFT RETURN" :
					"HARD RETURN");

			g_print ("  newline:\n");
			g_print ("    area = %.2f %.2f %.2f %.2f\n",
				 areas[offset].x1,
				 areas[offset].y1,
				 areas[offset].x2,
				 areas[offset].y2);

			g_print ("    white=%d word_start=%d "
				 "word_end=%d mandatory=%d\n",
				 log_attrs[offset].is_white,
				 log_attrs[offset].is_word_start,
				 log_attrs[offset].is_word_end,
				 log_attrs[offset].is_mandatory_break);

			if (prev >= 0) {
				gunichar prev_ch;

				prev_ch = g_utf8_get_char (
					g_utf8_prev_char (p));

				g_print ("  previous @ %d: U+%04X '%c'\n",
					 prev,
					 prev_ch,
					 g_unichar_isprint (prev_ch)
					 ? (gchar) prev_ch : ' ');
				g_print ("    area = %.2f %.2f %.2f %.2f\n",
					 areas[prev].x1,
					 areas[prev].y1,
					 areas[prev].x2,
					 areas[prev].y2);
			}

			if (next < (gint)n_areas &&
			    next < (gint)n_attrs &&
			    *(p + 1) != '\0') {

				const gchar *next_p;
				gunichar next_ch;

				next_p = g_utf8_next_char (p);
				next_ch = g_utf8_get_char (next_p);

				g_print ("  next @ %d: U+%04X '%c'\n",
					 next,
					 next_ch,
					 g_unichar_isprint (next_ch)
					 ? (gchar) next_ch : ' ');

				g_print ("    area = %.2f %.2f %.2f %.2f\n",
					 areas[next].x1,
					 areas[next].y1,
					 areas[next].x2,
					 areas[next].y2);

				g_print ("    white=%d word_start=%d "
					 "word_end=%d mandatory=%d\n",
					 log_attrs[next].is_white,
					 log_attrs[next].is_word_start,
					 log_attrs[next].is_word_end,
					 log_attrs[next].is_mandatory_break);
			}
		}

		p = g_utf8_next_char (p);
		offset++;
	}

	g_print ("\n");
	g_print ("============================================================\n");
	g_print ("END NEWLINE ANALYSIS\n");
	g_print ("============================================================\n");
}

void
ev_debug_dump_blocks (EvPageCache *cache, gint page)
{
    const gchar *text;
    EvRectangle *areas = NULL;
    PangoLogAttr *log_attrs = NULL;
    guint n_areas = 0;
    gulong n_log_attrs = 0;

    gint i;
    gint start = 0;
    gint block_no = 0;
    gint n_chars = 0;

    const gchar *p;

    text = ev_page_cache_get_text (cache, page);

    if (!text)
        return;

    if (!ev_page_cache_get_text_layout (cache,
                                        page,
                                        &areas,
                                        &n_areas))
        return;

    if (!ev_page_cache_get_text_log_attrs (cache,
                                           page,
                                           &log_attrs,
                                           &n_log_attrs))
        return;

    if (n_areas == 0 || n_log_attrs == 0)
        return;

    /*
     * Hitung jumlah karakter Unicode.
     *
     * Penting:
     * text adalah UTF-8, sehingga jumlah byte tidak sama
     * dengan jumlah character offset yang digunakan oleh
     * areas[] dan log_attrs[].
     */
    for (p = text; *p; p = g_utf8_next_char (p))
        n_chars++;

    /*
     * Pastikan kita tidak membaca melewati array Atril.
     */
    if (n_chars > (gint)n_areas)
        n_chars = n_areas;

    if (n_chars > (gint)n_log_attrs)
        n_chars = n_log_attrs;

    g_print ("\n");
    g_print ("========================================\n");
    g_print ("BLOCK DEBUG: PAGE %d\n", page);
    g_print ("CHARACTERS = %d\n", n_chars);
    g_print ("========================================\n");

    /*
     * Scan seluruh character.
     *
     * SOFT RETURN:
     *     masih bagian dari block yang sama.
     *
     * HARD RETURN:
     *     block ditutup dan block berikutnya dimulai.
     */
    for (i = 0; i < n_chars; i++) {
        gunichar c;

        p = g_utf8_offset_to_pointer (text, i);
        c = g_utf8_get_char (p);

        /*
         * Kita hanya tertarik pada '\n'.
         */
        if (c != '\n')
            continue;

        /*
         * Gunakan heuristic yang sama dengan
         * Atril's treat_as_soft_return().
         */
        {
            gboolean soft;

            soft = ev_debug_is_soft_return (cache,
                                          areas,
                                          n_areas,
                                          log_attrs,
                                          i);

            /*
             * Soft return bukan akhir block.
             */
            if (soft)
                continue;
        }

        /*
         * HARD RETURN.
         *
         * text[start .. i-1] menjadi satu block.
         */
        {
            const gchar *block_start;
            const gchar *block_end;
            gchar *block_text;

            EvRectangle rect;
            gboolean have_rect = FALSE;

            gint j;

            block_start = g_utf8_offset_to_pointer (text, start);
            block_end   = g_utf8_offset_to_pointer (text, i);

            block_text = g_strndup (block_start,
                                    block_end - block_start);

            /*
             * Hitung bounding rectangle block.
             *
             * Rectangle kosong biasanya berasal dari '\n'
             * atau character tanpa geometry.
             */
            for (j = start;
                 j < i && j < (gint)n_areas;
                 j++) {

                EvRectangle *a = &areas[j];

                /*
                 * Abaikan rectangle kosong.
                 */
                if (a->x1 == a->x2 &&
                    a->y1 == a->y2)
                    continue;

                if (!have_rect) {
                    rect = *a;
                    have_rect = TRUE;
                } else {
                    if (a->x1 < rect.x1)
                        rect.x1 = a->x1;

                    if (a->y1 < rect.y1)
                        rect.y1 = a->y1;

                    if (a->x2 > rect.x2)
                        rect.x2 = a->x2;

                    if (a->y2 > rect.y2)
                        rect.y2 = a->y2;
                }
            }

            /*
             * Buang whitespace di awal/akhir hanya untuk
             * tampilan debug.
             */
            g_strstrip (block_text);

            g_print ("\n");
            g_print ("BLOCK %d\n", block_no);
            g_print ("  start = %d\n", start);
            g_print ("  end   = %d\n", i);

            if (have_rect) {
                g_print ("  rect  = %.2f %.2f %.2f %.2f\n",
                         rect.x1,
                         rect.y1,
                         rect.x2,
                         rect.y2);
            } else {
                g_print ("  rect  = NONE\n");
            }

            g_print ("  text  = \"%s\"\n", block_text);

            g_free (block_text);
        }

        block_no++;

        /*
         * Character setelah '\n' menjadi awal block berikutnya.
         */
        start = i + 1;
    }

    /*
     * Sisa text setelah HARD RETURN terakhir.
     *
     * Contohnya "Page 5" pada halaman yang Anda debug.
     */
    if (start < n_chars) {
        const gchar *block_start;
        const gchar *block_end;
        gchar *block_text;

        EvRectangle rect;
        gboolean have_rect = FALSE;

        gint j;

        block_start = g_utf8_offset_to_pointer (text, start);
        block_end   = g_utf8_offset_to_pointer (text, n_chars);

        block_text = g_strndup (block_start,
                                block_end - block_start);

        /*
         * Hitung bounding rectangle block terakhir.
         */
        for (j = start;
             j < n_chars && j < (gint)n_areas;
             j++) {

            EvRectangle *a = &areas[j];

            if (a->x1 == a->x2 &&
                a->y1 == a->y2)
                continue;

            if (!have_rect) {
                rect = *a;
                have_rect = TRUE;
            } else {
                if (a->x1 < rect.x1)
                    rect.x1 = a->x1;

                if (a->y1 < rect.y1)
                    rect.y1 = a->y1;

                if (a->x2 > rect.x2)
                    rect.x2 = a->x2;

                if (a->y2 > rect.y2)
                    rect.y2 = a->y2;
            }
        }

        g_strstrip (block_text);

        if (*block_text != '\0') {
            g_print ("\n");
            g_print ("BLOCK %d\n", block_no);
            g_print ("  start = %d\n", start);
            g_print ("  end   = %d\n", n_chars);

            if (have_rect) {
                g_print ("  rect  = %.2f %.2f %.2f %.2f\n",
                         rect.x1,
                         rect.y1,
                         rect.x2,
                         rect.y2);
            } else {
                g_print ("  rect  = NONE\n");
            }

            g_print ("  text  = \"%s\"\n", block_text);
        }

        g_free (block_text);
    }

    g_print ("\n");
    g_print ("========================================\n");
    g_print ("END BLOCK DEBUG\n");
    g_print ("========================================\n");
}

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

	debug_dump_newlines (cache, page);

	g_print ("============================================================\n");
	g_print ("END TEXT DEBUG: PAGE %d\n", page);
	g_print ("============================================================\n");
}
