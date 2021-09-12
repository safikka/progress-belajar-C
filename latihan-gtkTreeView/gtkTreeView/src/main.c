#include <gtk/gtk.h>
#include <glib.h>

struct widget_container_t{
    GtkWidget *window;
    GtkWidget *label_tanggal;
    GtkWidget *calendar;
    GtkWidget *expander;
    GtkWidget *btn_reset;
} ui_widget;

GtkTreeStore *treeStore;
GtkTreeView *treeView;
GtkTreeViewColumn *cx1;
GtkTreeViewColumn *cx2;
GtkTreeSelection *selection;
GtkCellRenderer *cr1;
GtkCellRenderer *cr2;
GtkBuilder *builder;

gboolean gtk_get_object_helper(GtkWidget **_widget_ , gchar *_widget_name_,...){
	if(_widget_name_ == NULL){
		g_print("ERROR: WIDGET NAME NOT VALID ! (null)\n");
		return TRUE;
	}
	gchar *widget_name;
	va_list aptr;
	va_start(aptr, _widget_name_);
    widget_name = g_strdup_vprintf(_widget_name_,aptr);
	va_end(aptr);

	*_widget_ = GTK_WIDGET(gtk_builder_get_object(builder, widget_name));
	if(*_widget_==NULL) g_print("ERROR: FAILED GET %s\n",widget_name);
	g_free(widget_name);
	return FALSE;
}

void on_select_change(GtkTreeSelection *treeselection,gpointer user_data){
    gchar *value;
    GtkTreeIter iter;
    GtkTreeModel *model;

    if(gtk_tree_selection_get_selected(treeselection, &model,&iter) == FALSE){
        return;
    }

    gtk_tree_model_get(model,&iter,0,&value,-1);
    g_print("Col 0 = %s; ",value);
    gtk_tree_model_get(model,&iter,1,&value,-1);
    g_print("Col 1 = %s; \n",value);

}

static void
calendar_day_selected (GtkCalendar *widget,gpointer data)
{
    g_print("Day selected\n");
    // if(gtk_expander_get_expanded(GTK_EXPANDER(ui_widget.expander))){
        guint year, month, day;
        gchar *detail;

        gtk_calendar_get_date (GTK_CALENDAR (widget), &year, &month, &day);
        detail = g_strdup_printf ("%04d-%02d-%02d", year, month + 1, day);
        gulong block;
        // g_signal_handler_block(ui_widget.label_tanggal,block);
        gtk_label_set_text (GTK_LABEL (ui_widget.label_tanggal), detail);
        // g_signal_handler_unblock(ui_widget.label_tanggal,block);

        gtk_expander_set_expanded(GTK_EXPANDER(ui_widget.expander),FALSE);
        // gtk_calendar_mark_day (GTK_CALENDAR (ui_widget.calendar), day);
        // gtk_calendar_select_day (GTK_CALENDAR (ui_widget.calendar), 0);
        g_free (detail);
    // }
}

static void
calendar_prev_month (GtkCalendar* widget,gpointer data)
{
  gtk_calendar_clear_marks(GTK_CALENDAR (widget));
  gtk_calendar_select_day (GTK_CALENDAR (ui_widget.calendar), 0);
  g_print("prev Month selected\n");
}


static void
calendar_next_month (GtkCalendar* widget,gpointer data)
{
  g_print("nexy month selected\n");
}

static void
calendar_prev_year (GtkCalendar* widget, gpointer data)
{
  g_print("prev year selected\n");
}

static void
calendar_next_year (GtkCalendar* widget, gpointer data)
{
  g_print("nect year selected\n");
}

static void on_reset_btn(GtkWidget *widget,gpointer unused){
  gtk_calendar_clear_marks(GTK_CALENDAR (ui_widget.calendar));
  gtk_calendar_select_day (GTK_CALENDAR (ui_widget.calendar), 0);
  g_print("reset btn clicked\n");
}

int main(int argc, char *argv[])
{  
    gtk_init(&argc,&argv);
    builder = gtk_builder_new_from_file("ui.glade");
    gtk_get_object_helper(&ui_widget.window,"window");
    gtk_get_object_helper(&ui_widget.label_tanggal,"label_tanggal");
    gtk_get_object_helper(&ui_widget.calendar,"calendar");
    gtk_get_object_helper(&ui_widget.expander,"calendar_expander");
    gtk_get_object_helper(&ui_widget.btn_reset,"btn_reset");
    
    time_t epoch;
    time(&epoch);
    gtk_calendar_select_month(GTK_CALENDAR (ui_widget.calendar), 8,2021);
    gtk_calendar_mark_day (GTK_CALENDAR (ui_widget.calendar), 8);

    treeView  = GTK_TREE_VIEW(gtk_builder_get_object(builder,"tv1"));
    treeStore = GTK_TREE_STORE(gtk_builder_get_object(builder,"treeStore"));
    selection = GTK_TREE_SELECTION(gtk_builder_get_object(builder,"select"));
    cx1       = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder,"cx1"));
    cx2       = GTK_TREE_VIEW_COLUMN(gtk_builder_get_object(builder,"cx2"));
    cr1       = GTK_CELL_RENDERER(gtk_builder_get_object(builder,"cr1"));
    cr2       = GTK_CELL_RENDERER(gtk_builder_get_object(builder,"cr2"));
    gtk_cell_renderer_set_alignment(cr1,0.5,0);
    gtk_cell_renderer_set_alignment(cr2,0.5,0);

    gtk_builder_connect_signals(builder,NULL);
    g_signal_connect (ui_widget.btn_reset, "clicked", G_CALLBACK (on_reset_btn),NULL);

    g_signal_connect (ui_widget.calendar, "day_selected", G_CALLBACK (calendar_day_selected),NULL);
    g_signal_connect (ui_widget.calendar, "prev_month", G_CALLBACK (calendar_prev_month),NULL);
    g_signal_connect (ui_widget.calendar, "next_month", G_CALLBACK (calendar_next_month),NULL);
    g_signal_connect (ui_widget.calendar, "prev_year", G_CALLBACK (calendar_prev_year),NULL);
    g_signal_connect (ui_widget.calendar, "next_year", G_CALLBACK (calendar_next_year),NULL);

    g_signal_connect_swapped(G_OBJECT(ui_widget.window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(selection,"changed",G_CALLBACK(on_select_change),NULL);

    gtk_tree_view_column_add_attribute(cx1,cr1,"text",0);
    gtk_tree_view_column_add_attribute(cx2,cr2,"text",1);
    GtkTreeIter iter;
    GtkTreeIter iterChild1;
    GtkTreeIter iterChild2;

    gtk_tree_store_append(treeStore,&iter,NULL);
    gtk_tree_store_set(treeStore,&iter,0,"row 1",-1);
    gtk_tree_store_set(treeStore,&iter,1,"row 1 data",-1);

    // gtk_tree_store_append(treeStore,&iterChild1,&iter);
    // gtk_tree_store_set(treeStore,&iterChild1,0,"row 1 child",-1);
    // gtk_tree_store_set(treeStore,&iterChild1,1,"row 1 child data",-1);

    gtk_tree_store_append(treeStore,&iter,NULL);
    gtk_tree_store_set(treeStore,&iter,0,"row 2",-1);
    gtk_tree_store_set(treeStore,&iter,1,"row 2 data",-1);

    // gtk_tree_store_append(treeStore,&iterChild1,&iter);
    // gtk_tree_store_set(treeStore,&iterChild1,0,"row 2 child",-1);
    // gtk_tree_store_set(treeStore,&iterChild1,1,"row 2 child data",-1);

    // gtk_tree_store_append(treeStore,&iterChild2,&iterChild1);
    // gtk_tree_store_set(treeStore,&iterChild1,0,"row 2 child of child",-1);
    // gtk_tree_store_set(treeStore,&iterChild1,1,"row 2 child of child data",-1);
    selection = gtk_tree_view_get_selection(treeView);
    gtk_widget_show_all(ui_widget.window);

    gtk_main();
    return 0;
}