#ifndef CONSOLE_H
#define CONSOLE_H

#include "ui/qemu-pixman.h"
#include "qom/object.h"
#include "qapi/qmp/qdict.h"
#include "qemu/notify.h"
#include "qapi-types.h"
#include "qemu/error-report.h"
#include "qapi/error.h"

#ifdef CONFIG_OPENGL
# include <epoxy/gl.h>
#endif

/* keyboard/mouse support */

#define MOUSE_EVENT_LBUTTON 0x01
#define MOUSE_EVENT_RBUTTON 0x02
#define MOUSE_EVENT_MBUTTON 0x04
#define MOUSE_EVENT_WHEELUP 0x08
#define MOUSE_EVENT_WHEELDN 0x10

/* identical to the ps/2 keyboard bits */
#define QEMU_SCROLL_LOCK_LED (1 << 0)
#define QEMU_NUM_LOCK_LED    (1 << 1)
#define QEMU_CAPS_LOCK_LED   (1 << 2)

/* in ms */
#define GUI_REFRESH_INTERVAL_DEFAULT    30
#define GUI_REFRESH_INTERVAL_IDLE     3000

/* Color number is match to standard vga palette */
enum qemu_color_names {
    QEMU_COLOR_BLACK   = 0,
    QEMU_COLOR_BLUE    = 1,
    QEMU_COLOR_GREEN   = 2,
    QEMU_COLOR_CYAN    = 3,
    QEMU_COLOR_RED     = 4,
    QEMU_COLOR_MAGENTA = 5,
    QEMU_COLOR_YELLOW  = 6,
    QEMU_COLOR_WHITE   = 7
};
/* Convert to curses char attributes */
#define ATTR2CHTYPE(c, fg, bg, bold) \
    ((bold) << 21 | (bg) << 11 | (fg) << 8 | (c))

typedef void QEMUPutKBDEvent(void *opaque, int keycode);
typedef void QEMUPutLEDEvent(void *opaque, int ledstate);
typedef void QEMUPutMouseEvent(void *opaque, int dx, int dy, int dz, int buttons_state);

typedef struct QEMUPutMouseEntry QEMUPutMouseEntry;
typedef struct QEMUPutKbdEntry QEMUPutKbdEntry;
typedef struct QEMUPutLEDEntry QEMUPutLEDEntry;

QEMUPutKbdEntry *qemu_add_kbd_event_handler(QEMUPutKBDEvent *func,
                                            void *opaque);
QEMUPutMouseEntry *qemu_add_mouse_event_handler(QEMUPutMouseEvent *func,
                                                void *opaque, int absolute,
                                                const char *name);
void qemu_remove_mouse_event_handler(QEMUPutMouseEntry *entry);
void qemu_activate_mouse_event_handler(QEMUPutMouseEntry *entry);

QEMUPutLEDEntry *qemu_add_led_event_handler(QEMUPutLEDEvent *func, void *opaque);
void qemu_remove_led_event_handler(QEMUPutLEDEntry *entry);

void kbd_put_ledstate(int ledstate);

struct MouseTransformInfo {
    /* Touchscreen resolution */
    int x;
    int y;
    /* Calibration values as used/generated by tslib */
    int a[7];
};

void hmp_mouse_set(Monitor *mon, const QDict *qdict);

/* keysym is a unicode code except for special keys (see QEMU_KEY_xxx
   constants) */
#define QEMU_KEY_ESC1(c) ((c) | 0xe100)
#define QEMU_KEY_BACKSPACE  0x007f
#define QEMU_KEY_UP         QEMU_KEY_ESC1('A')
#define QEMU_KEY_DOWN       QEMU_KEY_ESC1('B')
#define QEMU_KEY_RIGHT      QEMU_KEY_ESC1('C')
#define QEMU_KEY_LEFT       QEMU_KEY_ESC1('D')
#define QEMU_KEY_HOME       QEMU_KEY_ESC1(1)
#define QEMU_KEY_END        QEMU_KEY_ESC1(4)
#define QEMU_KEY_PAGEUP     QEMU_KEY_ESC1(5)
#define QEMU_KEY_PAGEDOWN   QEMU_KEY_ESC1(6)
#define QEMU_KEY_DELETE     QEMU_KEY_ESC1(3)

#define QEMU_KEY_CTRL_UP         0xe400
#define QEMU_KEY_CTRL_DOWN       0xe401
#define QEMU_KEY_CTRL_LEFT       0xe402
#define QEMU_KEY_CTRL_RIGHT      0xe403
#define QEMU_KEY_CTRL_HOME       0xe404
#define QEMU_KEY_CTRL_END        0xe405
#define QEMU_KEY_CTRL_PAGEUP     0xe406
#define QEMU_KEY_CTRL_PAGEDOWN   0xe407

void kbd_put_keysym_console(QemuConsole *s, int keysym);
bool kbd_put_qcode_console(QemuConsole *s, int qcode);
void kbd_put_string_console(QemuConsole *s, const char *str, int len);
void kbd_put_keysym(int keysym);

/* consoles */

#define TYPE_QEMU_CONSOLE "qemu-console"
#define QEMU_CONSOLE(obj) \
    OBJECT_CHECK(QemuConsole, (obj), TYPE_QEMU_CONSOLE)
#define QEMU_CONSOLE_GET_CLASS(obj) \
    OBJECT_GET_CLASS(QemuConsoleClass, (obj), TYPE_QEMU_CONSOLE)
#define QEMU_CONSOLE_CLASS(klass) \
    OBJECT_CLASS_CHECK(QemuConsoleClass, (klass), TYPE_QEMU_CONSOLE)

typedef struct QemuConsoleClass QemuConsoleClass;

struct QemuConsoleClass {
    ObjectClass parent_class;
};

#define QEMU_ALLOCATED_FLAG     0x01

struct PixelFormat {
    uint8_t bits_per_pixel;
    uint8_t bytes_per_pixel;
    uint8_t depth; /* color depth in bits */
    uint32_t rmask, gmask, bmask, amask;
    uint8_t rshift, gshift, bshift, ashift;
    uint8_t rmax, gmax, bmax, amax;
    uint8_t rbits, gbits, bbits, abits;
};

struct DisplaySurface {
    pixman_format_code_t format;
    pixman_image_t *image;
    uint8_t flags;
#ifdef CONFIG_OPENGL
    GLenum glformat;
    GLenum gltype;
    GLuint texture;
#endif
};

typedef struct QemuUIInfo {
    /* geometry */
    int       xoff;
    int       yoff;
    uint32_t  width;
    uint32_t  height;
} QemuUIInfo;

/* cursor data format is 32bit RGBA */
typedef struct QEMUCursor {
    int                 width, height;
    int                 hot_x, hot_y;
    int                 refcount;
    uint32_t            data[];
} QEMUCursor;

QEMUCursor *cursor_alloc(int width, int height);
void cursor_get(QEMUCursor *c);
void cursor_put(QEMUCursor *c);
QEMUCursor *cursor_builtin_hidden(void);
QEMUCursor *cursor_builtin_left_ptr(void);
void cursor_print_ascii_art(QEMUCursor *c, const char *prefix);
int cursor_get_mono_bpl(QEMUCursor *c);
void cursor_set_mono(QEMUCursor *c,
                     uint32_t foreground, uint32_t background, uint8_t *image,
                     int transparent, uint8_t *mask);
void cursor_get_mono_image(QEMUCursor *c, int foreground, uint8_t *mask);
void cursor_get_mono_mask(QEMUCursor *c, int transparent, uint8_t *mask);

typedef void *QEMUGLContext;
typedef struct QEMUGLParams QEMUGLParams;

struct QEMUGLParams {
    int major_ver;
    int minor_ver;
};

typedef struct DisplayChangeListenerOps {
    const char *dpy_name;

    void (*dpy_refresh)(DisplayChangeListener *dcl);

    void (*dpy_gfx_update)(DisplayChangeListener *dcl,
                           int x, int y, int w, int h);
    void (*dpy_gfx_switch)(DisplayChangeListener *dcl,
                           struct DisplaySurface *new_surface);
    bool (*dpy_gfx_check_format)(DisplayChangeListener *dcl,
                                 pixman_format_code_t format);

    void (*dpy_text_cursor)(DisplayChangeListener *dcl,
                            int x, int y);
    void (*dpy_text_resize)(DisplayChangeListener *dcl,
                            int w, int h);
    void (*dpy_text_update)(DisplayChangeListener *dcl,
                            int x, int y, int w, int h);

    void (*dpy_mouse_set)(DisplayChangeListener *dcl,
                          int x, int y, int on);
    void (*dpy_cursor_define)(DisplayChangeListener *dcl,
                              QEMUCursor *cursor);

    QEMUGLContext (*dpy_gl_ctx_create)(DisplayChangeListener *dcl,
                                       QEMUGLParams *params);
    void (*dpy_gl_ctx_destroy)(DisplayChangeListener *dcl,
                               QEMUGLContext ctx);
    int (*dpy_gl_ctx_make_current)(DisplayChangeListener *dcl,
                                   QEMUGLContext ctx);
    QEMUGLContext (*dpy_gl_ctx_get_current)(DisplayChangeListener *dcl);

    void (*dpy_gl_scanout)(DisplayChangeListener *dcl,
                           uint32_t backing_id, bool backing_y_0_top,
                           uint32_t backing_width, uint32_t backing_height,
                           uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    void (*dpy_gl_update)(DisplayChangeListener *dcl,
                          uint32_t x, uint32_t y, uint32_t w, uint32_t h);

} DisplayChangeListenerOps;

struct DisplayChangeListener {
    uint64_t update_interval;
    const DisplayChangeListenerOps *ops;
    DisplayState *ds;
    QemuConsole *con;

    QLIST_ENTRY(DisplayChangeListener) next;
};

DisplayState *init_displaystate(void);
DisplaySurface *qemu_create_displaysurface_from(int width, int height,
                                                pixman_format_code_t format,
                                                int linesize, uint8_t *data);
DisplaySurface *qemu_create_displaysurface_pixman(pixman_image_t *image);
DisplaySurface *qemu_create_displaysurface_guestmem(int width, int height,
                                                    pixman_format_code_t format,
                                                    int linesize,
                                                    uint64_t addr);
PixelFormat qemu_default_pixelformat(int bpp);

DisplaySurface *qemu_create_displaysurface(int width, int height);
void qemu_free_displaysurface(DisplaySurface *surface);

static inline int is_surface_bgr(DisplaySurface *surface)
{
    if (PIXMAN_FORMAT_BPP(surface->format) == 32 &&
        PIXMAN_FORMAT_TYPE(surface->format) == PIXMAN_TYPE_ABGR) {
        return 1;
    } else {
        return 0;
    }
}

static inline int is_buffer_shared(DisplaySurface *surface)
{
    return !(surface->flags & QEMU_ALLOCATED_FLAG);
}

void register_displaychangelistener(DisplayChangeListener *dcl);
void update_displaychangelistener(DisplayChangeListener *dcl,
                                  uint64_t interval);
void unregister_displaychangelistener(DisplayChangeListener *dcl);

bool dpy_ui_info_supported(QemuConsole *con);
int dpy_set_ui_info(QemuConsole *con, QemuUIInfo *info);

void dpy_gfx_update(QemuConsole *con, int x, int y, int w, int h);
void dpy_gfx_replace_surface(QemuConsole *con,
                             DisplaySurface *surface);
void dpy_text_cursor(QemuConsole *con, int x, int y);
void dpy_text_update(QemuConsole *con, int x, int y, int w, int h);
void dpy_text_resize(QemuConsole *con, int w, int h);
void dpy_mouse_set(QemuConsole *con, int x, int y, int on);
void dpy_cursor_define(QemuConsole *con, QEMUCursor *cursor);
bool dpy_cursor_define_supported(QemuConsole *con);
bool dpy_gfx_check_format(QemuConsole *con,
                          pixman_format_code_t format);

void dpy_gl_scanout(QemuConsole *con,
                    uint32_t backing_id, bool backing_y_0_top,
                    uint32_t backing_width, uint32_t backing_height,
                    uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void dpy_gl_update(QemuConsole *con,
                   uint32_t x, uint32_t y, uint32_t w, uint32_t h);

QEMUGLContext dpy_gl_ctx_create(QemuConsole *con,
                                QEMUGLParams *params);
void dpy_gl_ctx_destroy(QemuConsole *con, QEMUGLContext ctx);
int dpy_gl_ctx_make_current(QemuConsole *con, QEMUGLContext ctx);
QEMUGLContext dpy_gl_ctx_get_current(QemuConsole *con);

bool console_has_gl(QemuConsole *con);

static inline int surface_stride(DisplaySurface *s)
{
    return pixman_image_get_stride(s->image);
}

static inline void *surface_data(DisplaySurface *s)
{
    return pixman_image_get_data(s->image);
}

static inline int surface_width(DisplaySurface *s)
{
    return pixman_image_get_width(s->image);
}

static inline int surface_height(DisplaySurface *s)
{
    return pixman_image_get_height(s->image);
}

static inline int surface_bits_per_pixel(DisplaySurface *s)
{
    int bits = PIXMAN_FORMAT_BPP(s->format);
    return bits;
}

static inline int surface_bytes_per_pixel(DisplaySurface *s)
{
    int bits = PIXMAN_FORMAT_BPP(s->format);
    return (bits + 7) / 8;
}

static inline pixman_format_code_t surface_format(DisplaySurface *s)
{
    return s->format;
}

#ifdef CONFIG_CURSES
#include <curses.h>
typedef chtype console_ch_t;
extern chtype vga_to_curses[];
#else
typedef unsigned long console_ch_t;
#endif
static inline void console_write_ch(console_ch_t *dest, uint32_t ch)
{
    uint8_t c = ch;
#ifdef CONFIG_CURSES
    if (vga_to_curses[c]) {
        ch &= ~(console_ch_t)0xff;
        ch |= vga_to_curses[c];
    }
#else
    if (c == '\0') {
        ch |= ' ';
    }
#endif
    *dest = ch;
}

typedef struct GraphicHwOps {
    void (*invalidate)(void *opaque);
    void (*gfx_update)(void *opaque);
    void (*text_update)(void *opaque, console_ch_t *text);
    void (*update_interval)(void *opaque, uint64_t interval);
    int (*ui_info)(void *opaque, uint32_t head, QemuUIInfo *info);
    void (*gl_block)(void *opaque, bool block);
} GraphicHwOps;

QemuConsole *graphic_console_init(DeviceState *dev, uint32_t head,
                                  const GraphicHwOps *ops,
                                  void *opaque);
void graphic_console_set_hwops(QemuConsole *con,
                               const GraphicHwOps *hw_ops,
                               void *opaque);

void graphic_hw_update(QemuConsole *con);
void graphic_hw_invalidate(QemuConsole *con);
void graphic_hw_text_update(QemuConsole *con, console_ch_t *chardata);
void graphic_hw_gl_block(QemuConsole *con, bool block);

QemuConsole *qemu_console_lookup_by_index(unsigned int index);
QemuConsole *qemu_console_lookup_by_device(DeviceState *dev, uint32_t head);
QemuConsole *qemu_console_lookup_by_device_name(const char *device_id,
                                                uint32_t head, Error **errp);
bool qemu_console_is_visible(QemuConsole *con);
bool qemu_console_is_graphic(QemuConsole *con);
bool qemu_console_is_fixedsize(QemuConsole *con);
char *qemu_console_get_label(QemuConsole *con);
int qemu_console_get_index(QemuConsole *con);
uint32_t qemu_console_get_head(QemuConsole *con);
QemuUIInfo *qemu_console_get_ui_info(QemuConsole *con);
int qemu_console_get_width(QemuConsole *con, int fallback);
int qemu_console_get_height(QemuConsole *con, int fallback);

void text_consoles_set_display(DisplayState *ds);
void console_select(unsigned int index);
void console_color_init(DisplayState *ds);
void qemu_console_resize(QemuConsole *con, int width, int height);
DisplaySurface *qemu_console_surface(QemuConsole *con);

/* console-gl.c */
typedef struct ConsoleGLState ConsoleGLState;
#ifdef CONFIG_OPENGL
ConsoleGLState *console_gl_init_context(void);
void console_gl_fini_context(ConsoleGLState *gls);
bool console_gl_check_format(DisplayChangeListener *dcl,
                             pixman_format_code_t format);
void surface_gl_create_texture(ConsoleGLState *gls,
                               DisplaySurface *surface);
void surface_gl_update_texture(ConsoleGLState *gls,
                               DisplaySurface *surface,
                               int x, int y, int w, int h);
void surface_gl_render_texture(ConsoleGLState *gls,
                               DisplaySurface *surface);
void surface_gl_destroy_texture(ConsoleGLState *gls,
                               DisplaySurface *surface);
void surface_gl_setup_viewport(ConsoleGLState *gls,
                               DisplaySurface *surface,
                               int ww, int wh);
#endif

/* sdl.c */
#ifdef CONFIG_SDL
void sdl_display_early_init(int opengl);
void sdl_display_init(DisplayState *ds, int full_screen, int no_frame);
#else
static inline void sdl_display_early_init(int opengl)
{
    /* This must never be called if CONFIG_SDL is disabled */
    error_report("SDL support is disabled");
    abort();
}
static inline void sdl_display_init(DisplayState *ds, int full_screen,
                                    int no_frame)
{
    /* This must never be called if CONFIG_SDL is disabled */
    error_report("SDL support is disabled");
    abort();
}
#endif

/* cocoa.m */
#ifdef CONFIG_COCOA
void cocoa_display_init(DisplayState *ds, int full_screen);
#else
static inline void cocoa_display_init(DisplayState *ds, int full_screen)
{
    /* This must never be called if CONFIG_COCOA is disabled */
    error_report("Cocoa support is disabled");
    abort();
}
#endif

/* vnc.c */
void vnc_display_init(const char *id);
void vnc_display_open(const char *id, Error **errp);
void vnc_display_add_client(const char *id, int csock, bool skipauth);
#ifdef CONFIG_VNC
int vnc_display_password(const char *id, const char *password);
int vnc_display_pw_expire(const char *id, time_t expires);
QemuOpts *vnc_parse(const char *str, Error **errp);
int vnc_init_func(void *opaque, QemuOpts *opts, Error **errp);
#else
static inline int vnc_display_password(const char *id, const char *password)
{
    return -ENODEV;
}
static inline int vnc_display_pw_expire(const char *id, time_t expires)
{
    return -ENODEV;
};
static inline QemuOpts *vnc_parse(const char *str, Error **errp)
{
    error_setg(errp, "VNC support is disabled");
    return NULL;
}
static inline int vnc_init_func(void *opaque, QemuOpts *opts, Error **errp)
{
    error_setg(errp, "VNC support is disabled");
    return -1;
}
#endif

/* curses.c */
#ifdef CONFIG_CURSES
void curses_display_init(DisplayState *ds, int full_screen);
#else
static inline void curses_display_init(DisplayState *ds, int full_screen)
{
    /* This must never be called if CONFIG_CURSES is disabled */
    error_report("curses support is disabled");
    abort();
}
#endif

/* input.c */
int index_from_key(const char *key, size_t key_length);

/* gtk.c */
#ifdef CONFIG_GTK
void early_gtk_display_init(int opengl);
void gtk_display_init(DisplayState *ds, bool full_screen, bool grab_on_hover);
#else
static inline void gtk_display_init(DisplayState *ds, bool full_screen,
                                    bool grab_on_hover)
{
    /* This must never be called if CONFIG_GTK is disabled */
    error_report("GTK support is disabled");
    abort();
}

static inline void early_gtk_display_init(int opengl)
{
    /* This must never be called if CONFIG_GTK is disabled */
    error_report("GTK support is disabled");
    abort();
}
#endif

#endif
