#include <glib.h>
#include <glib-object.h>
#include <libappindicator/app-indicator.h>

#define TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE            (test_libappindicator_fallback_item_get_type ())
#define TEST_LIBAPPINDICATOR_FALLBACK_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE, TestLibappindicatorFallbackItem))
#define TEST_LIBAPPINDICATOR_FALLBACK_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE, TestLibappindicatorFallbackItemClass))
#define IS_TEST_LIBAPPINDICATOR_FALLBACK_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE))
#define IS_TEST_LIBAPPINDICATOR_FALLBACK_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE))
#define TEST_LIBAPPINDICATOR_FALLBACK_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE, TestLibappindicatorFallbackItemClass))

typedef struct _TestLibappindicatorFallbackItem      TestLibappindicatorFallbackItem;
typedef struct _TestLibappindicatorFallbackItemClass TestLibappindicatorFallbackItemClass;

struct _TestLibappindicatorFallbackItemClass {
	AppIndicatorClass parent_class;

};

struct _TestLibappindicatorFallbackItem {
	AppIndicator parent;

};

GType test_libappindicator_fallback_item_get_type (void);

#define TEST_LIBAPPINDICATOR_FALLBACK_ITEM_GET_PRIVATE(o) \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE, TestLibappindicatorFallbackItemPrivate))

static void test_libappindicator_fallback_item_class_init (TestLibappindicatorFallbackItemClass *klass);
static void test_libappindicator_fallback_item_init       (TestLibappindicatorFallbackItem *self);
static GtkStatusIcon * fallback (AppIndicator * indicator);
static void unfallback (AppIndicator * indicator, GtkStatusIcon * status_icon);

G_DEFINE_TYPE (TestLibappindicatorFallbackItem, test_libappindicator_fallback_item, APP_INDICATOR_TYPE);

static void
test_libappindicator_fallback_item_class_init (TestLibappindicatorFallbackItemClass *klass)
{
	AppIndicatorClass * aiclass = APP_INDICATOR_CLASS(klass);

	aiclass->fallback = fallback;
	aiclass->unfallback = unfallback;
}

static void
test_libappindicator_fallback_item_init (TestLibappindicatorFallbackItem *self)
{
}

GMainLoop * mainloop = NULL;
gboolean passed = FALSE;

enum {
	STATE_INIT,
	STATE_FALLBACK,
	STATE_UNFALLBACK,
	STATE_REFALLBACK,
	STATE_REUNFALLBACK
};

gint state = STATE_INIT;

static GtkStatusIcon *
fallback (AppIndicator * indicator)
{
	g_debug("Fallback");
	if (state == STATE_INIT) {
		state = STATE_FALLBACK;
	} else if (state == STATE_UNFALLBACK) {
		state = STATE_REFALLBACK;
	} else {
		g_debug("Error, fallback in state: %d", state);
		passed = FALSE;
	}
	return (GtkStatusIcon *)5;
}

static void
unfallback (AppIndicator * indicator, GtkStatusIcon * status_icon)
{
	g_debug("Unfallback");
	if (state == STATE_FALLBACK) {
		state = STATE_UNFALLBACK;
	} else if (state == STATE_REFALLBACK) {
		state = STATE_REUNFALLBACK;
		passed = TRUE;
		g_main_loop_quit(mainloop);
	} else {
		g_debug("Error, unfallback in state: %d", state);
		passed = FALSE;
	}
	return;
}

gboolean
kill_func (gpointer data)
{
	g_debug("Kill Function");
	g_main_loop_quit(mainloop);
	return FALSE;
}

int
main (int argc, char ** argv)
{
	gtk_init(&argc, &argv);

	TestLibappindicatorFallbackItem * item = g_object_new(TEST_LIBAPPINDICATOR_FALLBACK_ITEM_TYPE,
		"id", "test-id",
		"category", "Other",
		"icon-name", "bob",
		NULL);
	
	GtkWidget * menu = gtk_menu_new();
	app_indicator_set_menu(APP_INDICATOR(item), GTK_MENU(menu));

	g_timeout_add_seconds(1, kill_func, NULL);

	mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(mainloop);

	g_object_unref(G_OBJECT(item));

	if (passed) {
		return 0;
	} else {
		return 1;
	}
}