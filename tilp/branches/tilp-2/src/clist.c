/* Hey EMACS -*- linux-c -*- */
/* $Id$ */

/*  TiLP - Ti Linking Program
 *  Copyright (C) 1999-2005  Romain Lievin
 *
 *  This program is free software you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "gstruct.h"
#include "support.h"
#include "labels.h"
#include "clist_rbm.h"
#include "tilp_core.h"

#ifdef __WIN32__
#include "dirent.h"	// S_ISDIR
#endif

static GtkListStore *list;

enum 
{ 
	COLUMN_ICON, COLUMN_NAME, COLUMN_TYPE, COLUMN_SIZE, COLUMN_DATE, 
	COLUMN_DATA,
};

#define CLIST_NVCOLS	(5)		// 4 visible columns
#define CLIST_NCOLS		(6)		// 7 real columns


/* Initialization */

static gint column2index(GtkWidget* list, GtkTreeViewColumn* column)
{
	gint i;

	for (i = 0; i < CLIST_NVCOLS; i++) 
	{
		GtkTreeViewColumn *col;

		col = gtk_tree_view_get_column(GTK_TREE_VIEW(list), i);
		if (col == column)
			return i;
	}

	return -1;
}

static gboolean select_func(
				GtkTreeSelection* selection,
			    GtkTreeModel* model,
			    GtkTreePath* path,
			    gboolean path_currently_selected,
			    gpointer data)
{
	GtkTreeIter iter;
	FileEntry *fe;

	gtk_tree_model_get_iter(model, &iter, path);
	gtk_tree_model_get(model, &iter, COLUMN_DATA, &fe, -1);

	if (S_ISDIR(fe->attrib))
		return FALSE;

	return TRUE;
}

static void tree_selection_changed(GtkTreeSelection* selection, gpointer user_data)
{
	GList *list;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *sel;

	// destroy selection
	tilp_clist_selection_destroy();

	// clear ctree selection(one selection active at a time)
	sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(ctree_wnd));
	gtk_tree_selection_unselect_all(sel);

	// create a new selection
	list = gtk_tree_selection_get_selected_rows(selection, &model);
	while (list != NULL) 
	{
		GtkTreePath *path = list->data;
		FileEntry *fe;
		gchar *full_path;

		gtk_tree_model_get_iter(model, &iter, path);
		gtk_tree_model_get(model, &iter, COLUMN_DATA, &fe, -1);

		local.selection = g_list_append(local.selection, fe);
		full_path = g_strconcat(local.cwdir, G_DIR_SEPARATOR_S, fe->name, NULL);
		local.file_selection = g_list_append(local.file_selection, full_path);
		list = g_list_next(list);
	}
}

void clist_refresh(void);
static void column_clicked(GtkTreeViewColumn* column, gpointer user_data)
{
	int col = column2index(user_data, column);
	
	switch(col)
	{
	case COLUMN_NAME:
		options.local_sort = SORT_BY_NAME;
		clist_refresh();
		break;
	case COLUMN_TYPE:
		options.local_sort = SORT_BY_TYPE;
		clist_refresh();
		break;
	case COLUMN_SIZE:
		options.local_sort = SORT_BY_SIZE;
		clist_refresh();
		break;
	case COLUMN_DATE:
		options.local_sort = SORT_BY_DATE;
		clist_refresh();
		break;
	default: break;
	}
}

void clist_init(void)
{
	GtkTreeView *view = GTK_TREE_VIEW(clist_wnd);
	GtkTreeModel *model = GTK_TREE_MODEL(list);
	GtkCellRenderer *renderer;
	GtkTreeSelection *selection;
	gint i;

	list = gtk_list_store_new(CLIST_NCOLS, GDK_TYPE_PIXBUF,
					G_TYPE_STRING, G_TYPE_STRING, 
					G_TYPE_STRING, G_TYPE_STRING, 
				   G_TYPE_POINTER
			       );
	model = GTK_TREE_MODEL(list);

	gtk_tree_view_set_model(view, model);
	gtk_tree_view_set_headers_visible(view, TRUE);
	gtk_tree_view_set_headers_clickable(view, TRUE);
	gtk_tree_view_set_rules_hint(view, FALSE);

	renderer = gtk_cell_renderer_pixbuf_new();
	gtk_tree_view_insert_column_with_attributes(view, -1, "",
						    renderer, "pixbuf",
						    COLUMN_ICON, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(view, -1, _("Name"),
						    renderer, "text",
						    COLUMN_NAME, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(view, -1, _("Type"),
						    renderer, "text",
						    COLUMN_TYPE, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(view, -1, _("Size"),
						    renderer, "text",
						    COLUMN_SIZE, NULL);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_insert_column_with_attributes(view, -1, _("Date"),
						    renderer, "text",
						    COLUMN_DATE, NULL);

	for (i = 0; i < CLIST_NVCOLS; i++) 
	{
		GtkTreeViewColumn *col;

		col = gtk_tree_view_get_column(view, i);
		gtk_tree_view_column_set_resizable(col, TRUE);

		gtk_tree_view_column_set_clickable(col, TRUE);
		g_signal_connect(G_OBJECT(col), "clicked", G_CALLBACK(column_clicked), view);
	}

	selection = gtk_tree_view_get_selection(view);
	gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
	gtk_tree_selection_set_select_function(selection, select_func, NULL, NULL);
	g_signal_connect(G_OBJECT(selection), "changed",
			 G_CALLBACK(tree_selection_changed), NULL);
}

#ifdef __WIN32__
#define strcasecmp	_stricmp
#endif

/* Management */

void clist_refresh(void)
{
	GtkTreeView *view = GTK_TREE_VIEW(clist_wnd);
	GtkTreeViewColumn *col;
	GtkTreeIter iter;
	GdkPixbuf *pix1, *pix2, *pix;
	GList *dirlist;
	gsize br, bw;
	gchar *utf8;

	// reparse folders
	tilp_clist_selection_destroy();
	tilp_dirlist_local();
	gtk_list_store_clear(list);

	// sort files
	switch (options.local_sort) 
	{
	case SORT_BY_NAME:
		tilp_file_sort_by_name();
		
		col = gtk_tree_view_get_column(view, COLUMN_NAME);
		gtk_tree_view_column_set_sort_indicator(col, TRUE);
		gtk_tree_view_column_set_sort_order(col, options.local_sort_order ? GTK_SORT_ASCENDING : GTK_SORT_DESCENDING);
		break;
	case SORT_BY_TYPE:
		tilp_file_sort_by_type();
		col = gtk_tree_view_get_column(view, COLUMN_TYPE);
		gtk_tree_view_column_set_sort_indicator(col, TRUE);
		gtk_tree_view_column_set_sort_order(col, options.local_sort_order ? GTK_SORT_ASCENDING : GTK_SORT_DESCENDING);
		break;
	case SORT_BY_DATE:
		tilp_file_sort_by_date();
		col = gtk_tree_view_get_column(view, COLUMN_DATE);
		gtk_tree_view_column_set_sort_indicator(col, TRUE);
		gtk_tree_view_column_set_sort_order(col, options.local_sort_order ? GTK_SORT_ASCENDING : GTK_SORT_DESCENDING);
		break;
	case SORT_BY_SIZE:
		tilp_file_sort_by_size();
		col = gtk_tree_view_get_column(view, COLUMN_SIZE);
		gtk_tree_view_column_set_sort_indicator(col, TRUE);
		gtk_tree_view_column_set_sort_order(col, options.local_sort_order ? GTK_SORT_ASCENDING : GTK_SORT_DESCENDING);
		break;
	}

	pix1 = create_pixbuf("up.ico");
	pix2 = create_pixbuf("clist_dir.xpm");

	for (dirlist = local.dirlist; dirlist != NULL; dirlist = dirlist->next) 
	{
		FileEntry *fe = (FileEntry *) dirlist->data;
		gboolean b;

		b = options.show_all || S_ISDIR(fe->attrib) ||
			tifiles_file_is_ti(fe->name) ||
			tifiles_file_is_tib(fe->name) ||
			(tifiles_file_get_model(fe->name) == options.calc_model);
		if(!b)
			continue;

		if (S_ISDIR(fe->attrib)) 
		{
			pix = strcmp(fe->name, "..") ? pix2 : pix1; 
		} 
		else 
		{
			char icon_name[256];

			strcpy(icon_name, tifiles_file_get_icon(fe->name));

			if (!strcmp(icon_name, ""))
				strcpy(icon_name, "TIicon1");

			strcat(icon_name, ".ico");
			tilp_file_underscorize(icon_name);
			pix = create_pixbuf(icon_name);
		}

		// utf8
		utf8 = g_filename_to_utf8(fe->name, -1, &br, &bw, NULL);

		gtk_list_store_append(list, &iter);
		gtk_list_store_set(list, &iter, COLUMN_NAME,
				   utf8,
				   COLUMN_TYPE, tilp_file_get_type(fe),
				   COLUMN_SIZE, tilp_file_get_size(fe),
				   COLUMN_DATE, tilp_file_get_date(fe),
				   COLUMN_DATA, (gpointer) fe, 
                   COLUMN_ICON, pix, 
                   -1);
		g_object_unref(pix);
	}
}


/* Callbacks */

GLADE_CB gboolean
on_treeview2_button_press_event(GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
	GtkTreeView *view = GTK_TREE_VIEW(widget);
	GtkTreeModel *model = GTK_TREE_MODEL(list);
	GtkTreePath *path;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	GdkEventButton *bevent;
	gint tx = (gint) event->x;
	gint ty = (gint) event->y;
	gint cx, cy;
	FileEntry *fe;

	gtk_tree_view_get_path_at_pos(view, tx, ty, &path, &column, &cx, &cy);

	switch (event->type) 
	{
	case GDK_BUTTON_PRESS:
		if (event->button == 3) 
		{
			bevent = (GdkEventButton *) (event);

			gtk_menu_popup(GTK_MENU(create_clist_rbm()),
				       NULL, NULL, NULL, NULL,
				       bevent->button, bevent->time);

			return TRUE;
		}
		break;

	case GDK_2BUTTON_PRESS:
		if (path == NULL)
			return FALSE;

		gtk_tree_model_get_iter(model, &iter, path);
		gtk_tree_model_get(model, &iter, COLUMN_DATA, &fe, -1);

		if (S_ISDIR(fe->attrib)) 
		{
			// go into folder
			tilp_chdir(fe->name);
			g_free(local.cwdir);
			local.cwdir = g_get_current_dir();

			clist_refresh();
			labels_refresh();
		} 
		break;
	default:
		break;
	}

	return FALSE;		// pass event on
}


#include <gdk/gdkkeysyms.h>


/* Key pressed */
GLADE_CB gboolean
on_treeview2_key_press_event(GtkWidget* widget, GdkEventKey* event,
			     gpointer user_data)
{
	if (event->keyval == GDK_Delete) 
	{
		//on_delete_file1_activate(NULL, NULL);
		return TRUE;
	}

	if ((event->state == GDK_CONTROL_MASK) &&
	    ((event->keyval == GDK_X) || (event->keyval == GDK_x))) 
	{
		//on_cut1_activate(NULL, NULL);
		return TRUE;
	}

	if ((event->state == GDK_CONTROL_MASK) &&
	    ((event->keyval == GDK_c) || (event->keyval == GDK_C))) 
	{
		//on_copy1_activate(NULL, NULL);
		return TRUE;
	}

	if ((event->state == GDK_CONTROL_MASK) &&
	    ((event->keyval == GDK_V) || (event->keyval == GDK_v))) 
	{
		//on_paste1_activate(NULL, NULL);
		return TRUE;
	}
	return FALSE;
}