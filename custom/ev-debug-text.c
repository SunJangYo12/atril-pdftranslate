#include "config.h"

#include <glib.h>
#include <string.h>

#include "ev-debug-text.h"
#include "ev-page-cache.h"

#include "ev-view.h"
#include "ev-view-private.h"

#include <glib.h>
#include <pango/pango.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <math.h>


void
ev_debug_overlay_show_cb (GtkButton *button,
                          gpointer   data)
{
    EvView *view = EV_VIEW (data);

    view->overlay_hidden = FALSE;

	gtk_widget_hide(view->btn_window);

    gtk_widget_queue_draw (GTK_WIDGET (view));
}

static void
show_button (EvView *view,
                       gint x,
                       gint y)
{
    GtkWidget *toplevel;
    gint root_x;
    gint root_y;

    if (!view->btn_window) {

        GtkWidget *button;

        toplevel =
            gtk_widget_get_toplevel (GTK_WIDGET (view));


        view->btn_window =
            gtk_window_new (GTK_WINDOW_POPUP);

        gtk_window_set_decorated (
			GTK_WINDOW (view->btn_window),
            FALSE);

        gtk_window_set_resizable (
            GTK_WINDOW (view->btn_window),
            FALSE);
        button = gtk_button_new_with_label ("Show");

        view->overlay_show_button = button;

        gtk_container_add (
            GTK_CONTAINER (view->btn_window),
            button);

        g_signal_connect (
            button,
            "clicked",
            G_CALLBACK (ev_debug_overlay_show_cb),
            view);
        gtk_widget_show (button);
    }
    /*
     * Koordinat x,y relatif terhadap EvView.
     *
     * Ubah ke koordinat root window.
     */
    gdk_window_get_origin (
        gtk_widget_get_window (GTK_WIDGET (view)),
        &root_x,
        &root_y);

    gtk_window_move (
        GTK_WINDOW (view->btn_window),
        root_x + x,
        root_y + y);
    gtk_widget_show (view->btn_window);
}

static void
ev_debug_overlay_hide_cb (GtkMenuItem *item,
                          gpointer     data)
{
    EvView *view = EV_VIEW (data);

	view->overlay_hidden = TRUE;

    g_print ("HIDE OVERLAY: page=%d block=%u\n",
             view->translate_page,
             view->translate_index);

	view->hidden_overlay_page = view->translate_page;
    view->hidden_overlay_index = view->translate_index;

	gint x = (gint)view->translate_rect.x2;
	gint y = (gint)view->translate_rect.y1;
	show_button(view, x, y);

	gtk_widget_queue_draw (GTK_WIDGET (view));
}

static void
ev_debug_overlay_delete_cb (GtkMenuItem *item,
                            gpointer     data)
{
    EvView *view = EV_VIEW (data);

    g_print ("DELETE OVERLAY: page=%d block=%u\n",
             view->translate_page,
             view->translate_index);
}

void
ev_debug_show_overlay_menu (EvView *view)
{
    GtkWidget *menu;
    GtkWidget *item;

    menu = gtk_menu_new ();

    item = gtk_menu_item_new_with_label ("Hide");
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK (ev_debug_overlay_hide_cb),
                      view);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    item = gtk_menu_item_new_with_label ("Hapus");
    g_signal_connect (item,
                      "activate",
                      G_CALLBACK (ev_debug_overlay_delete_cb),
                      view);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

    gtk_widget_show_all (menu);

    gtk_menu_popup (GTK_MENU (menu),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    3,
                    gtk_get_current_event_time ());
}

static gchar *
ev_debug_get_text_in_rect (EvPageCache      *cache,
                           gint              page,
                           EvRectangle      *screen_rect,
                           GdkRectangle     *real_page_area,
                           gdouble           scale_x,
                           gdouble           scale_y)
{
    const gchar *text;
    EvRectangle *areas = NULL;
    PangoLogAttr *log_attrs = NULL;
    guint n_areas = 0;
    gulong n_log_attrs = 0;

    gint n_chars = 0;
    gint i;
    const gchar *p;

    GString *result;
    gboolean have_previous = FALSE;
    gdouble previous_y = 0.0;

    text = ev_page_cache_get_text (cache, page);

    if (!text)
        return g_strdup ("");

    if (!ev_page_cache_get_text_layout (cache,
                                        page,
                                        &areas,
                                        &n_areas))
        return g_strdup ("");

    if (!ev_page_cache_get_text_log_attrs (cache,
                                           page,
                                           &log_attrs,
                                           &n_log_attrs))
        return g_strdup ("");

    for (p = text; *p; p = g_utf8_next_char (p))
        n_chars++;

    if (n_chars > (gint)n_areas)
        n_chars = n_areas;

    if (n_chars > (gint)n_log_attrs)
        n_chars = n_log_attrs;

    result = g_string_new ("");

    for (i = 0; i < n_chars; i++) {
        EvRectangle *a;
        gdouble x;
        gdouble y;
        gdouble width;
        gdouble height;
        gdouble cx;
        gdouble cy;
        gunichar c;
        const gchar *char_ptr;

        a = &areas[i];

        if (a->x1 == a->x2 &&
            a->y1 == a->y2)
            continue;

        /*
         * PDF/document coordinate -> screen coordinate.
         */
        x = real_page_area->x + a->x1 * scale_x;
        y = real_page_area->y + a->y1 * scale_y;

        width = (a->x2 - a->x1) * scale_x;
        height = (a->y2 - a->y1) * scale_y;

        cx = x + width / 2.0;
        cy = y + height / 2.0;

        /*
         * Karakter harus berada di dalam overlay.
         */
        if (cx < screen_rect->x1 ||
            cx > screen_rect->x2 ||
            cy < screen_rect->y1 ||
            cy > screen_rect->y2)
            continue;

        /*
         * Jika pindah ke baris berikutnya,
         * masukkan newline.
         */
        if (have_previous &&
            fabs (cy - previous_y) > 3.0) {

            if (result->len > 0 &&
                result->str[result->len - 1] != '\n')
                g_string_append_c (result, '\n');
        }

        char_ptr = g_utf8_offset_to_pointer (text, i);
        c = g_utf8_get_char (char_ptr);

        if (c == '\n') {
            if (result->len > 0 &&
                result->str[result->len - 1] != '\n')
                g_string_append_c (result, '\n');
        } else {
            g_string_append_unichar (result, c);
        }

        previous_y = cy;
        have_previous = TRUE;
    }

    /*
     * Buang newline di akhir.
     */
    while (result->len > 0 &&
           result->str[result->len - 1] == '\n') {

        g_string_truncate (result, result->len - 1);
    }

    return g_string_free (result, FALSE);
}

static gchar *
ev_debug_get_translate_dir (EvView *view)
{
    gchar *pdf_path;
    gchar *dir;

    if (!view->document)
        return NULL;

    pdf_path = g_filename_from_uri (ev_document_get_uri (view->document),
                                    NULL,
                                    NULL);

    if (!pdf_path)
        return NULL;

    dir = g_strdup_printf ("%s_translate", pdf_path);

    g_free (pdf_path);

    return dir;
}

static gchar *
ev_debug_get_page_dir (EvView *view,
                       gint    page)
{
    gchar *base_dir;
    gchar *page_dir;

    base_dir = ev_debug_get_translate_dir (view);

    if (!base_dir)
        return NULL;

    page_dir = g_strdup_printf ("%s/page%d",
                                base_dir,
                                page + 1);

    g_free (base_dir);

    return page_dir;
}

static gchar *
ev_debug_escape_text (const gchar *text)
{
    GString *s;
    const gchar *p;

    s = g_string_new ("");

    for (p = text; p && *p; p = g_utf8_next_char (p)) {
        gunichar c = g_utf8_get_char (p);

        if (c == '\\')
            g_string_append (s, "\\\\");
        else if (c == '"')
            g_string_append (s, "\\\"");
        else if (c == '\n')
            //g_string_append (s, "\\n");
            g_string_append (s, " ");
        else
            g_string_append_unichar (s, c);
    }

    return g_string_free (s, FALSE);
}

static void
ev_debug_save_block_position (EvView       *view,
                              gint          page,
                              gint          block_no,
                              EvRectangle  *screen_rect,
                              GdkRectangle *real_page_area,
                              gdouble       scale_x,
                              gdouble       scale_y,
                              gdouble       document_width,
                              gdouble       document_height,
							  const gchar  *text_original,
							  gboolean	    overwrite)
{
    gchar *page_dir;
    gchar *filename;
    gchar *content;

    gdouble x1;
    gdouble y1;
    gdouble x2;
    gdouble y2;

    page_dir = ev_debug_get_page_dir (view, page);

    if (!page_dir)
        return;

    if (g_mkdir_with_parents (page_dir, 0755) != 0) {
        g_warning ("Cannot create directory: %s", page_dir);
        g_free (page_dir);
        return;
    }

    filename = g_strdup_printf ("%s/block%d.txt",
                                page_dir,
                                block_no + 1);

    /*
     * screen coordinate -> document coordinate
     */
    x1 = (screen_rect->x1 - real_page_area->x) / scale_x;
    y1 = (screen_rect->y1 - real_page_area->y) / scale_y;

    x2 = (screen_rect->x2 - real_page_area->x) / scale_x;
    y2 = (screen_rect->y2 - real_page_area->y) / scale_y;

    /*
     * Normalisasi terhadap ukuran halaman.
     */
    x1 /= document_width;
    y1 /= document_height;
    x2 /= document_width;
    y2 /= document_height;

	gchar *escaped_text;
	escaped_text = ev_debug_escape_text (text_original);

	content = g_strdup_printf (
	    "posisi_overlay=\"%.10f,%.10f,%.10f,%.10f\"\n"
	    "text_original=\"%s\"\n"
	    "text_translate=\"\"\n",
	    x1, y1, x2, y2,
	    escaped_text);

	g_free (escaped_text);

	if (!overwrite &&
	    g_file_test (filename, G_FILE_TEST_EXISTS)) {

	    g_free (filename);
	    g_free (page_dir);
	    return;
	}

    /*
     * g_file_set_contents() otomatis overwrite.
     */
    if (!g_file_set_contents (filename,
                              content,
                              -1,
                              NULL)) {

        g_warning ("Cannot save overlay: %s", filename);
    } else {
        g_print ("SAVE OVERLAY: page=%d block=%d "
                 "pos=%f,%f - %f,%f\n",
                 page,
                 block_no + 1,
                 x1, y1, x2, y2);
    }

    g_free (content);
    g_free (filename);
    g_free (page_dir);
}

static gboolean
ev_debug_load_block_position (EvView       *view,
                              gint          page,
                              gint          block_no,
                              EvRectangle  *rect,
                              gdouble       document_width,
                              gdouble       document_height)
{
    gchar *page_dir;
    gchar *filename;
    gchar *content;
    gdouble x1, y1, x2, y2;
    gboolean result = FALSE;

    page_dir = ev_debug_get_page_dir (view, page);

    if (!page_dir)
        return FALSE;

    filename = g_strdup_printf ("%s/block%d.txt",
                                page_dir,
                                block_no + 1);

    if (!g_file_get_contents (filename, &content, NULL, NULL))
        goto out;

    if (sscanf (content,
                "posisi_overlay=\"%lf,%lf,%lf,%lf\"",
                &x1, &y1, &x2, &y2) != 4)
        goto out;

    /*
     * File menggunakan koordinat relatif 0.0 - 1.0.
     * Kembalikan ke koordinat dokumen.
     */
    rect->x1 = x1 * document_width;
    rect->y1 = y1 * document_height;
    rect->x2 = x2 * document_width;
    rect->y2 = y2 * document_height;

    result = TRUE;

out:
    g_free (content);
    g_free (filename);
    g_free (page_dir);

    return result;
}


static void
ev_debug_print_text_in_rect (EvPageCache  *cache,
                             EvView       *view,
                             gint          page,
                             EvRectangle  *overlay_rect,
                             gdouble       document_width,
                             gdouble       document_height,
                             GdkRectangle *real_page_area)
{
    const gchar *text;
    EvRectangle *areas = NULL;
    guint n_areas = 0;
    gint i;
    GString *result;

    gdouble scale_x;
    gdouble scale_y;

    text = ev_page_cache_get_text (cache, page);

    if (!text)
        return;

    if (!ev_page_cache_get_text_layout (cache,
                                        page,
                                        &areas,
                                        &n_areas))
        return;

    if (!areas || n_areas == 0)
        return;

    scale_x = (gdouble)real_page_area->width /
              document_width;

    scale_y = (gdouble)real_page_area->height /
              document_height;

    result = g_string_new ("");

    for (i = 0; i < (gint)n_areas; i++) {
        EvRectangle *a = &areas[i];

        gdouble cx;
        gdouble cy;

        if (a->x1 == a->x2 &&
            a->y1 == a->y2)
            continue;

        /*
         * Titik tengah karakter dalam koordinat Cairo.
         */
        cx = real_page_area->x +
             ((a->x1 + a->x2) / 2.0) * scale_x;

        cy = real_page_area->y +
             ((a->y1 + a->y2) / 2.0) * scale_y;

        if (cx >= overlay_rect->x1 &&
            cx <= overlay_rect->x2 &&
            cy >= overlay_rect->y1 &&
            cy <= overlay_rect->y2) {

            const gchar *p;
            gunichar c;

            p = g_utf8_offset_to_pointer (text, i);
            c = g_utf8_get_char (p);

            g_string_append_unichar (result, c);
        }
    }

    g_print ("\n");
    g_print ("========== OVERLAY TEXT ==========\n");
    g_print ("PAGE: %d\n", page);
    g_print ("%s\n", result->str);
    g_print ("==================================\n");

    g_string_free (result, TRUE);
}

gboolean
ev_view_point_in_translate_resize_handle (EvView  *view,
                                          gdouble  x,
                                          gdouble  y)
{
    gdouble handle_size = 12.0;

    if (view->translate_page < 0)
        return FALSE;

    return x >= view->translate_rect.x2 - handle_size &&
           x <= view->translate_rect.x2 + handle_size &&
           y >= view->translate_rect.y2 - handle_size &&
           y <= view->translate_rect.y2 + handle_size;
}

gboolean
ev_view_point_in_translate_overlay (EvView  *view,
                                    gdouble  x,
                                    gdouble  y)
{
	if (view->translate_page < 0)
		return FALSE;

	return x >= view->translate_rect.x1 &&
	       x <= view->translate_rect.x2 &&
	       y >= view->translate_rect.y1 &&
	       y <= view->translate_rect.y2;
}

static gchar *
ev_debug_load_block_original_text (EvView *view,
                                   gint    page,
                                   gint    block_no)
{
    gchar *page_dir;
    gchar *filename;
    gchar *contents = NULL;
    gsize length = 0;
    gchar *text = NULL;
    gchar *p;
    gchar *end;

    page_dir = ev_debug_get_page_dir (view, page);
    if (!page_dir)
        return NULL;

    filename = g_strdup_printf ("%s/block%d.txt",
                                page_dir,
                                block_no + 1);

    if (!g_file_get_contents (filename,
                               &contents,
                               &length,
                               NULL)) {
        g_free (filename);
        g_free (page_dir);
        return NULL;
    }

    /*
     * Cari:
     * text_original="..."
     */
    p = strstr (contents, "text_original=\"");

    if (p) {
        p += strlen ("text_original=\"");

        end = strchr (p, '"');

        if (end) {
            text = g_strndup (p, end - p);
        }
    }

    g_free (contents);
    g_free (filename);
    g_free (page_dir);

    return text;
}

static void
ev_debug_draw_wrapped_text (cairo_t       *cr,
                            const gchar   *text,
                            gdouble        x,
                            gdouble        y,
                            gdouble        width,
                            gdouble        height)
{
    gchar **words;
    gchar *line;
    gint i;
    gdouble line_height;
    gdouble cursor_y;

    if (!text || !*text)
        return;

    /*
     * Font text overlay.
     */
    cairo_select_font_face (
        cr,
        "Sans",
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size (cr, 13.0);

    line_height = 17.0;
    cursor_y = y + line_height;

    words = g_strsplit_set (text, " \t\r\n", -1);

    line = g_strdup ("");

    for (i = 0; words[i] != NULL; i++) {

        gchar *candidate;
        cairo_text_extents_t extents;

        if (!words[i][0])
            continue;

        if (line[0])
            candidate =
                g_strdup_printf ("%s %s",
                                 line,
                                 words[i]);
        else
            candidate =
                g_strdup (words[i]);

        cairo_text_extents (cr, candidate, &extents);

        /*
         * Kalau baris sudah melebihi lebar overlay,
         * gambar baris lama lalu mulai baris baru.
         */
        if (extents.width > width - 10.0 &&
            line[0]) {

            cairo_move_to (cr,
                           x + 5.0,
                           cursor_y);

            cairo_show_text (cr, line);

            cursor_y += line_height;

            g_free (line);
            line = g_strdup (words[i]);

        } else {

            g_free (line);
            line = candidate;
        }

        /*
         * Jangan menggambar keluar dari tinggi overlay.
         */
        if (cursor_y > y + height - 2.0)
            break;
    }

    /*
     * Gambar baris terakhir.
     */
    if (line[0] &&
        cursor_y <= y + height - 2.0) {

        cairo_move_to (cr,
                       x + 5.0,
                       cursor_y);

        cairo_show_text (cr, line);
    }

    g_free (line);
    g_strfreev (words);
}

void
ev_debug_draw_blocks (EvPageCache  *cache, EvView *view,
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

	if (view->overlay_save_pending &&
	    view->translate_page == page) {

	    gchar *text_original;

	    text_original = ev_debug_get_text_in_rect (
	        view->page_cache,
	        page,
	        &view->translate_rect,
	        real_page_area,
	        scale_x,
	        scale_y);

	    ev_debug_save_block_position (
	        view,
	        page,
	        view->translate_index,
	        &view->translate_rect,
	        real_page_area,
	        scale_x,
	        scale_y,
	        document_width,
	        document_height,
	        text_original,
	        TRUE);

	    g_print ("SAVED TEXT:\n%s\n", text_original);

	    g_free (text_original);

	    view->overlay_save_pending = FALSE;
	}

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
				gboolean hidden;

				if (view->translate_page == page &&
				    view->translate_index == block_no &&
					(view->overlay_in_drag || view->overlay_in_resize)) {

				    x = view->translate_rect.x1;
				    y = view->translate_rect.y1;

				    width = view->translate_rect.x2 -
				            view->translate_rect.x1;

				    height = view->translate_rect.y2 -
				             view->translate_rect.y1;
				} else {
                    EvRectangle saved_rect;

                    if (ev_debug_load_block_position (
                            view,
                            page,
                            block_no,
                            &saved_rect,
                            document_width,
                            document_height)) {

                        /*
                         * Ada posisi tersimpan.
                         * Gunakan posisi dari file.
                         */
                        rect = saved_rect;

                        /*g_print ("LOAD OVERLAY: page=%d block=%d "
                                 "rect=%f,%f - %f,%f\n",
                                 page,
                                 block_no + 1,
                                 rect.x1,
                                 rect.y1,
                                 rect.x2,
                                 rect.y2);*/
                    }

                    x = real_page_area->x + rect.x1 * scale_x;
                    y = real_page_area->y + rect.y1 * scale_y;
                    width = (rect.x2 - rect.x1) * scale_x;
                    height = (rect.y2 - rect.y1) * scale_y;

	                hidden =
	                    view->overlay_hidden &&
	                    view->hidden_overlay_page == page &&
	                    view->hidden_overlay_index == block_no;
				}

				if (!hidden && view->mouse_x >= x &&
				    view->mouse_x <= x + width &&
				    view->mouse_y >= y &&
				    view->mouse_y <= y + height) {

				    if (!view->overlay_in_drag &&
				        !view->overlay_in_resize) {

				        view->translate_rect.x1 = x;
				        view->translate_rect.y1 = y;
				        view->translate_rect.x2 = x + width;
				        view->translate_rect.y2 = y + height;

				        view->translate_page = page;
				        view->translate_index = block_no;
				    }
				}
                /*
                 * Rectangle block.
                 */
				if (!hidden) {
					cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	                cairo_rectangle (cr,
	                                 x,
	                                 y,
	                                 width,
	                                 height);

	                cairo_fill (cr);

					// resize view
					if (view->translate_page == page &&
					    view->translate_index == block_no) {

						cairo_set_source_rgb(cr, 0.4, 0.2, 1.0);
					    cairo_rectangle (cr,
					                     x + width - 6.0,
					                     y + height - 6.0,
					                     12.0,
					                     12.0);

					    cairo_fill (cr);
					}
					{
					    gchar *text_original;

					    text_original =
					        ev_debug_load_block_original_text (
					            view,
					            page,
					            block_no);

					    if (text_original && *text_original) {

					        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
							ev_debug_draw_wrapped_text(
								cr, text_original, x, y, width, height);
					    }

					    g_free (text_original);
					}
				}

				EvRectangle initial_screen_rect;

				initial_screen_rect.x1 =
				    real_page_area->x + rect.x1 * scale_x;
				initial_screen_rect.y1 =
				    real_page_area->y + rect.y1 * scale_y;
				initial_screen_rect.x2 =
				    real_page_area->x + rect.x2 * scale_x;
				initial_screen_rect.y2 =
				    real_page_area->y + rect.y2 * scale_y;

				gchar *text_original;

				text_original = ev_debug_get_text_in_rect (
				    cache,
				    page,
				    &initial_screen_rect,
				    real_page_area,
				    scale_x,
				    scale_y);
				ev_debug_save_block_position (
				    view,
				    page,
				    block_no,
				    &initial_screen_rect,
				    real_page_area,
				    scale_x,
				    scale_y,
				    document_width,
				    document_height,
				    text_original,
				    FALSE);
				g_free (text_original);
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
			gboolean hidden;

			if (view->translate_page == page &&
			    view->translate_index == block_no &&
		        (view->overlay_in_drag || view->overlay_in_resize)) {

			    x = view->translate_rect.x1;
			    y = view->translate_rect.y1;

			    width = view->translate_rect.x2 -
			            view->translate_rect.x1;

			    height = view->translate_rect.y2 -
			             view->translate_rect.y1;

			} else {
                EvRectangle saved_rect;

                if (ev_debug_load_block_position (
                        view,
                        page,
                        block_no,
                        &saved_rect,
                        document_width,
                        document_height)) {

                    /*
                     * Ada posisi tersimpan.
                     * Gunakan posisi dari file.
                     */
                    rect = saved_rect;
                }

                x = real_page_area->x + rect.x1 * scale_x;
                y = real_page_area->y + rect.y1 * scale_y;
                width = (rect.x2 - rect.x1) * scale_x;
                height = (rect.y2 - rect.y1) * scale_y;

				hidden =
				    view->overlay_hidden &&
				    view->hidden_overlay_page == page &&
				    view->hidden_overlay_index == block_no;
			}

			if (!hidden && view->mouse_x >= x &&
			    view->mouse_x <= x + width &&
			    view->mouse_y >= y &&
			    view->mouse_y <= y + height) {

			    if (!view->overlay_in_drag &&
			        !view->overlay_in_resize) {

			        view->translate_rect.x1 = x;
			        view->translate_rect.y1 = y;
			        view->translate_rect.x2 = x + width;
			        view->translate_rect.y2 = y + height;

			        view->translate_page = page;
			        view->translate_index = block_no;
			    }
			}

			if (!hidden) {
				cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
	            cairo_rectangle (cr,
	                             x,
	                             y,
	                             width,
	                             height);
	            cairo_stroke (cr);
			}

			//resize view
			if (!hidden && view->translate_page == page &&
			    view->translate_index == block_no) {

				cairo_set_source_rgb(cr, 0.4, 0.2, 1.0);
			    cairo_rectangle (cr,
			                     x + width - 6.0,
			                     y + height - 6.0,
			                     12.0,
			                     12.0);

			    cairo_fill (cr);
			}
			if (!hidden) {
			    gchar *text_original;

			    text_original =
			        ev_debug_load_block_original_text (
			            view,
			            page,
			            block_no);

			    if (text_original && *text_original) {

			        cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
					ev_debug_draw_wrapped_text(
                                cr, text_original, x, y, width, height);
			    }

			    g_free (text_original);
			}
			{
                EvRectangle save_rect;

                save_rect.x1 = rect.x1;
                save_rect.y1 = rect.y1;
                save_rect.x2 = rect.x2;
                save_rect.y2 = rect.y2;
            }

			EvRectangle initial_screen_rect;

			initial_screen_rect.x1 =
			    real_page_area->x + rect.x1 * scale_x;
			initial_screen_rect.y1 =
			    real_page_area->y + rect.y1 * scale_y;
			initial_screen_rect.x2 =
			    real_page_area->x + rect.x2 * scale_x;
			initial_screen_rect.y2 =
			    real_page_area->y + rect.y2 * scale_y;

			gchar *text_original;

			text_original = ev_debug_get_text_in_rect (
			    cache,
			    page,
			    &initial_screen_rect,
			    real_page_area,
			    scale_x,
			    scale_y);
			ev_debug_save_block_position (
			    view,
			    page,
			    block_no,
			    &initial_screen_rect,
			    real_page_area,
			    scale_x,
			    scale_y,
			    document_width,
			    document_height,
			    text_original,
			    FALSE);
			g_free (text_original);
  		}
    }

	if (view->overlay_text_print_pending &&
	    view->translate_page == page) {

	    ev_debug_print_text_in_rect (cache,
	                                 view,
	                                 page,
	                                 &view->translate_rect,
	                                 document_width,
	                                 document_height,
	                                 real_page_area);

	    view->overlay_text_print_pending = FALSE;
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
		 (next_line_start->y2 - next_line_start->y1)) > 2.0)
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
