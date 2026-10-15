#ifndef C_TUIKIT_H
#define C_TUIKIT_H

#include <stddef.h>

#if defined(PNL_STATIC) && defined(__GNUC__)
#define PNL_API static __attribute__((unused))
#elif defined(PNL_STATIC)
#define PNL_API static
#elif defined(_WIN32) && defined(PNL_BUILD_SHARED)
#define PNL_API __declspec(dllexport)
#elif defined(_WIN32) && defined(PNL_SHARED)
#define PNL_API __declspec(dllimport)
#elif defined(__GNUC__) && defined(PNL_BUILD_SHARED)
#define PNL_API __attribute__((visibility("default")))
#else
#define PNL_API
#endif

#if defined(PNL_NO_DEPRECATED_WARNINGS)
#define PNL_DEPRECATED(msg)
#elif defined(__GNUC__)
#define PNL_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define PNL_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define PNL_DEPRECATED(msg)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PNL_OK 0
#define PNL_ERR_ARG 1
#define PNL_ERR_NOMEM 2
#define PNL_ERR_TTY 3
#define PNL_ERR_IO 4
#define PNL_ERR_NOT_FOUND 5
#define PNL_ERR_UNSUPPORTED 6
#define PNL_ERR_MISMATCH 7
#define PNL_ERR_LIMIT 8

#define PNL_MARK_OK 0
#define PNL_MARK_WAIT 1
#define PNL_MARK_FAIL 2

#define PNL_LEVEL_DEBUG 0
#define PNL_LEVEL_INFO 1
#define PNL_LEVEL_WARN 2
#define PNL_LEVEL_ERROR 3

#define PNL_KEY_NONE 0
#define PNL_KEY_UP 0x110000
#define PNL_KEY_DOWN 0x110001
#define PNL_KEY_LEFT 0x110002
#define PNL_KEY_RIGHT 0x110003
#define PNL_KEY_ENTER 0x110004
#define PNL_KEY_ESCAPE 0x110005
#define PNL_KEY_PAGE_UP 0x110006
#define PNL_KEY_PAGE_DOWN 0x110007
#define PNL_KEY_HOME 0x110008
#define PNL_KEY_END 0x110009
#define PNL_KEY_BACKSPACE 0x11000a
#define PNL_KEY_DELETE 0x11000b
#define PNL_KEY_TAB 0x11000c
#define PNL_KEY_BACKTAB 0x11000d
#define PNL_KEY_CLICK 0x11000e
#define PNL_KEY_WHEEL_UP 0x11000f
#define PNL_KEY_WHEEL_DOWN 0x110010

#define PNL_BOLD 0x01
#define PNL_DIM 0x02
#define PNL_REVERSE 0x04
#define PNL_UNDERLINE 0x08
#define PNL_FG(color) ((color) << 8)
#define PNL_BG(color) ((color) << 16)

#define PNL_COLOR_DEFAULT 0
#define PNL_COLOR_BLACK 1
#define PNL_COLOR_RED 2
#define PNL_COLOR_GREEN 3
#define PNL_COLOR_YELLOW 4
#define PNL_COLOR_BLUE 5
#define PNL_COLOR_MAGENTA 6
#define PNL_COLOR_CYAN 7
#define PNL_COLOR_WHITE 8
#define PNL_COLOR_GRAY 9
#define PNL_COLOR_BRIGHT_RED 10
#define PNL_COLOR_BRIGHT_GREEN 11
#define PNL_COLOR_BRIGHT_YELLOW 12
#define PNL_COLOR_BRIGHT_BLUE 13
#define PNL_COLOR_BRIGHT_MAGENTA 14
#define PNL_COLOR_BRIGHT_CYAN 15
#define PNL_COLOR_BRIGHT_WHITE 16

#define PNL_ROLE_ACCENT 32
#define PNL_ROLE_OK 33
#define PNL_ROLE_WARN 34
#define PNL_ROLE_FAIL 35
#define PNL_ROLE_MUTED 36
#define PNL_ROLE_INFO 37
#define PNL_ROLE_SURFACE 38

#define PNL_MOTION_SLIDE 1
#define PNL_MOTION_TRANSITION 2
#define PNL_MOTION_PULSE 4
#define PNL_MOTION_TYPEWRITER 8
#define PNL_MOTION_ENTER 16
#define PNL_MOTION_FEEDBACK 32
#define PNL_MOTION_COUNT 64

#define PNL_COLUMN_RIGHT 1
#define PNL_COLUMN_OPTIONAL 2

#define PNL_SPLIT_ROWS 0
#define PNL_SPLIT_COLUMNS 1
#define PNL_SPLIT_TABS 2
#define PNL_LAYOUT_ROOT 0

#define PNL_METER_ACCENT 0
#define PNL_METER_LEVEL 1
#define PNL_METER_PERCENT 2
#define PNL_METER_GRADIENT(gradient) ((gradient) << 16)

#define PNL_SPINNER_DOTS 0
#define PNL_SPINNER_LINE 1
#define PNL_SPINNER_ARC 2
#define PNL_SPINNER_BLOCKS 3
#define PNL_SPINNER_BOUNCE 4

#define PNL_GRAPH_SPARKLINE 0
#define PNL_GRAPH_CHART 1
#define PNL_GRAPH_PLOT 2

#define PNL_STACK_ROWS 0
#define PNL_STACK_COLUMNS 1
#define PNL_INPUT_SECRET 1
#define PNL_BIG_SMALL 0
#define PNL_BIG_LARGE 1

typedef struct pnl_app pnl_app_t;
typedef struct pnl_canvas pnl_canvas_t;
typedef struct pnl_task pnl_task_t;
typedef struct pnl_layout pnl_layout_t;
typedef struct pnl_value pnl_value_t;
typedef struct pnl_log pnl_log_t;
typedef struct pnl_test pnl_test_t;
typedef struct pnl_widget pnl_widget_t;

typedef void *(*pnl_alloc_fn)(void *ctx, size_t size);
typedef void (*pnl_free_fn)(void *ctx, void *ptr);

typedef struct pnl_allocator {
    pnl_alloc_fn alloc_fn;
    pnl_free_fn free_fn;
    void *ctx;
} pnl_allocator_t;

typedef void (*pnl_status_fn)(void *ctx, pnl_app_t *app);
typedef void (*pnl_action_fn)(void *ctx, pnl_app_t *app);
typedef void (*pnl_timer_fn)(void *ctx, pnl_app_t *app);
typedef void (*pnl_fd_fn)(void *ctx, pnl_app_t *app, int fd);
typedef void (*pnl_draw_fn)(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas);
typedef int (*pnl_key_fn)(void *ctx, pnl_app_t *app, int key);
typedef void (*pnl_terminal_fn)(void *ctx);
typedef void (*pnl_work_fn)(void *ctx, pnl_task_t *task);
typedef const char *(*pnl_cell_fn)(void *ctx, size_t row, size_t column, char *buf, size_t buf_len);
typedef void (*pnl_release_fn)(void *ctx);
typedef int (*pnl_height_fn)(void *ctx, pnl_app_t *app, int width);
typedef const char *(*pnl_hints_fn)(void *ctx);
typedef void (*pnl_change_fn)(void *ctx, pnl_app_t *app, pnl_widget_t *widget);

typedef struct pnl_widget_class {
    pnl_draw_fn draw_fn;
    pnl_key_fn key_fn;
    pnl_height_fn height_fn;
    pnl_hints_fn hints_fn;
    pnl_release_fn release_fn;
} pnl_widget_class_t;

PNL_API int pnl_app_create(const char *title, const pnl_allocator_t *allocator, pnl_app_t **out_app);
PNL_API void pnl_app_destroy(pnl_app_t *app);
PNL_API int pnl_app_on_status(pnl_app_t *app, pnl_status_fn status_fn, void *status_ctx);
PNL_API int pnl_app_set_refresh(pnl_app_t *app, int refresh_ms);
PNL_API int pnl_app_set_theme(pnl_app_t *app, const char *theme);
PNL_API int pnl_app_set_memory(pnl_app_t *app, const char *path);
PNL_API int pnl_app_bind(pnl_app_t *app, const char *key, const char *to);
PNL_API int pnl_app_add_action(pnl_app_t *app, const char *label, pnl_action_fn action_fn, void *action_ctx);
PNL_API int pnl_status_line(pnl_app_t *app, int mark, const char *label, const char *text);
PNL_API int pnl_mouse_position(const pnl_app_t *app, int *out_x, int *out_y);
PNL_API int pnl_notify(pnl_app_t *app, const char *text);
PNL_API int pnl_app_record(pnl_app_t *app, const char *path);
PNL_API int pnl_test_create(pnl_app_t *app, int width, int height, pnl_test_t **out_test);
PNL_API void pnl_test_destroy(pnl_test_t *test);
PNL_API int pnl_test_keys(pnl_test_t *test, const char *keys);
PNL_API int pnl_test_wait(pnl_test_t *test, int ms);
PNL_API const char *pnl_test_screen(const pnl_test_t *test);
PNL_API int pnl_test_expect(pnl_test_t *test, const char *path);
PNL_API int pnl_toast(pnl_app_t *app, int mark, const char *text);
PNL_API int pnl_status_meter(pnl_app_t *app, int mark, const char *label, double fraction);

PNL_API int pnl_value_create(pnl_app_t *app, const char *label, pnl_value_t **out_value);
PNL_API void pnl_value_destroy(pnl_value_t *value);
PNL_API int pnl_value_set(pnl_value_t *value, int mark, const char *text);
PNL_API int pnl_value_set_meter(pnl_value_t *value, int mark, double fraction);
PNL_API int pnl_log_create(pnl_app_t *app, size_t capacity, pnl_log_t **out_log);
PNL_API void pnl_log_destroy(pnl_log_t *logbook);
PNL_API int pnl_log_write(pnl_log_t *logbook, int level, const char *text);
PNL_API int pnl_log_open(pnl_app_t *app, pnl_log_t *logbook, const char *title);
PNL_API int pnl_result(pnl_app_t *app, int mark, const char *text);
PNL_API int pnl_app_run(pnl_app_t *app, int argc, char **argv, int *out_exit_code);
PNL_API int pnl_app_quit(pnl_app_t *app);
PNL_API int pnl_app_redraw(pnl_app_t *app);
PNL_API int pnl_app_motion(const pnl_app_t *app);
PNL_API int pnl_app_enable_motion(pnl_app_t *app, int flags);
PNL_API long long pnl_app_frame_ms(const pnl_app_t *app);
PNL_API int pnl_app_animate(pnl_app_t *app, int duration_ms);
PNL_API double pnl_app_tween(pnl_app_t *app, long long start_ms, int duration_ms);
PNL_API int pnl_app_add_timer(pnl_app_t *app, int interval_ms, pnl_timer_fn timer_fn, void *timer_ctx, int *out_timer);
PNL_API int pnl_app_remove_timer(pnl_app_t *app, int timer);
PNL_API int pnl_app_watch_fd(pnl_app_t *app, int fd, pnl_fd_fn fd_fn, void *fd_ctx);
PNL_API int pnl_app_unwatch_fd(pnl_app_t *app, int fd);
PNL_API int pnl_app_with_terminal(pnl_app_t *app, pnl_terminal_fn terminal_fn, void *terminal_ctx);
PNL_API int pnl_view_push(pnl_app_t *app, const char *title, pnl_draw_fn draw_fn, pnl_key_fn key_fn, void *view_ctx);
PNL_API int pnl_view_push_dialog(pnl_app_t *app, const char *title, int width, int height, pnl_draw_fn draw_fn,
                                 pnl_key_fn key_fn, void *view_ctx);
PNL_API int pnl_view_pop(pnl_app_t *app);
PNL_API int pnl_view_shake(pnl_app_t *app);
PNL_API int pnl_view_set_hints(pnl_app_t *app, const char *hints);
PNL_API int pnl_task_run(pnl_app_t *app, const char *title, pnl_work_fn work_fn, void *work_ctx);
PNL_API void pnl_task_progress(pnl_task_t *task, unsigned long long done, unsigned long long total);
PNL_API int pnl_task_step(pnl_task_t *task, const char *text);
PNL_API int pnl_task_log(pnl_task_t *task, const char *line);
PNL_API int pnl_task_cancelled(pnl_task_t *task);
PNL_API int pnl_task_result(pnl_task_t *task, int mark, const char *text);
PNL_API int pnl_task_then(pnl_task_t *task, pnl_action_fn then_fn, void *then_ctx);
PNL_API int pnl_confirm(pnl_app_t *app, const char *question, int *out_yes);
PNL_API int pnl_input(pnl_app_t *app, const char *prompt, char *text, size_t text_len, int *out_ok);
PNL_API int pnl_pick(pnl_app_t *app, const char *title, const char *const *items, size_t items_len,
                     size_t *out_index, int *out_ok);
PNL_API int pnl_pager(pnl_app_t *app, const char *title, const char *text, size_t text_len);
PNL_API int pnl_table_source(pnl_app_t *app, const char *title, const char *const *headers, const int *column_flags,
                             size_t columns, size_t rows, pnl_cell_fn cell_fn, pnl_release_fn release_fn, void *ctx);
PNL_API int pnl_table(pnl_app_t *app, const char *title, const char *const *headers, const int *column_flags,
                      size_t columns, const char *const *cells, size_t rows);
PNL_API int pnl_layout_create(pnl_app_t *app, int kind, pnl_layout_t **out_layout);
PNL_API void pnl_layout_destroy(pnl_layout_t *layout);
PNL_API int pnl_layout_split(pnl_layout_t *layout, int parent, int kind, const char *title, int *out_node);
PNL_API int pnl_layout_panel(pnl_layout_t *layout, int parent, const char *title, pnl_draw_fn draw_fn,
                             pnl_key_fn key_fn, void *panel_ctx, int *out_node);
PNL_API int pnl_layout_size(pnl_layout_t *layout, int node, int size, int min_size);
PNL_API int pnl_layout_focus(pnl_layout_t *layout, int node);
PNL_API int pnl_layout_hints(pnl_layout_t *layout, int node, const char *hints);
PNL_API int pnl_view_push_layout(pnl_app_t *app, const char *title, const pnl_layout_t *layout);
PNL_API int pnl_layout_widget(pnl_layout_t *layout, int parent, const char *title, pnl_widget_t *widget,
                              int *out_node);

PNL_API int pnl_widget_create(pnl_app_t *app, const pnl_widget_class_t *widget_class, void *widget_ctx,
                              pnl_widget_t **out_widget);
PNL_API void pnl_widget_destroy(pnl_widget_t *widget);
PNL_API void *pnl_widget_ctx(const pnl_widget_t *widget);
PNL_API int pnl_widget_draw(pnl_widget_t *widget, pnl_canvas_t *canvas, int x, int y, int width, int height);
PNL_API int pnl_widget_key(pnl_widget_t *widget, int key);
PNL_API int pnl_widget_height(pnl_widget_t *widget, int width);
PNL_API int pnl_view_push_widget(pnl_app_t *app, const char *title, pnl_widget_t *widget);
PNL_API int pnl_widget_text(pnl_app_t *app, const char *text, int style, pnl_widget_t **out_widget);
PNL_API int pnl_widget_bar(pnl_app_t *app, const char *label, int flags, pnl_widget_t **out_widget);
PNL_API int pnl_widget_graph(pnl_app_t *app, const char *label, int kind, int rows, int flags, size_t capacity,
                             pnl_widget_t **out_widget);
PNL_API int pnl_widget_spinner(pnl_app_t *app, int kind, const char *text, pnl_widget_t **out_widget);
PNL_API int pnl_widget_log(pnl_app_t *app, pnl_log_t *logbook, pnl_widget_t **out_widget);
PNL_API int pnl_widget_stack(pnl_app_t *app, int kind, pnl_widget_t **out_widget);
PNL_API int pnl_widget_add(pnl_widget_t *stack, pnl_widget_t *child, int size);
PNL_API int pnl_widget_set_text(pnl_widget_t *widget, const char *text);
PNL_API int pnl_widget_set_value(pnl_widget_t *widget, double value);
PNL_API int pnl_widget_set_range(pnl_widget_t *widget, double min, double max);
PNL_API int pnl_widget_input(pnl_app_t *app, const char *label, int flags, pnl_widget_t **out_widget);
PNL_API int pnl_widget_check(pnl_app_t *app, const char *label, int checked, pnl_widget_t **out_widget);
PNL_API int pnl_widget_choice(pnl_app_t *app, const char *label, const char *const *options, size_t count,
                              pnl_widget_t **out_widget);
PNL_API int pnl_widget_slider(pnl_app_t *app, const char *label, double min, double max, double step,
                              pnl_widget_t **out_widget);
PNL_API int pnl_widget_button(pnl_app_t *app, const char *label, pnl_action_fn action_fn, void *action_ctx,
                              pnl_widget_t **out_widget);
PNL_API int pnl_widget_on_change(pnl_widget_t *widget, pnl_change_fn change_fn, void *change_ctx);
PNL_API const char *pnl_widget_get_text(const pnl_widget_t *widget);
PNL_API int pnl_widget_get_value(const pnl_widget_t *widget, double *out_value);
PNL_API int pnl_widget_set_focusable(pnl_widget_t *widget, int focusable);
PNL_API int pnl_widget_focus(pnl_widget_t *stack, pnl_widget_t *child);

PNL_API int pnl_canvas_focused(const pnl_canvas_t *canvas);
PNL_API int pnl_canvas_width(const pnl_canvas_t *canvas);
PNL_API int pnl_canvas_height(const pnl_canvas_t *canvas);
PNL_API int pnl_canvas_text(pnl_canvas_t *canvas, int x, int y, int style, const char *text);
PNL_API int pnl_canvas_link(pnl_canvas_t *canvas, int x, int y, int style, const char *text, const char *url);
PNL_API void pnl_canvas_fill(pnl_canvas_t *canvas, int x, int y, int width, int height, int style);
PNL_API int pnl_color_depth(void);
PNL_API int pnl_app_color(pnl_app_t *app, unsigned long rgb, int *out_color);
PNL_API int pnl_app_gradient(pnl_app_t *app, const unsigned long *stops, size_t count, int steps,
                             int *out_gradient);
PNL_API int pnl_gradient_color(int gradient, double fraction);
PNL_API int pnl_canvas_gradient_text(pnl_canvas_t *canvas, int x, int y, int style, int gradient, const char *text);
PNL_API int pnl_canvas_big(pnl_canvas_t *canvas, int x, int y, int size, int style, int gradient, const char *text);
PNL_API int pnl_big_width(int size, const char *text);
PNL_API int pnl_big_height(int size);
PNL_API int pnl_canvas_art(pnl_canvas_t *canvas, int x, int y, int style, int gradient, const char *art);
PNL_API int pnl_app_set_banner(pnl_app_t *app, const char *art, int gradient);
PNL_API int pnl_widget_big(pnl_app_t *app, const char *text, int size, int style, int gradient,
                           pnl_widget_t **out_widget);
PNL_API int pnl_widget_art(pnl_app_t *app, const char *art, int style, int gradient, pnl_widget_t **out_widget);

#define PNL_STEP_WAIT 0
#define PNL_STEP_RUN 1
#define PNL_STEP_OK 2
#define PNL_STEP_FAIL 3
#define PNL_STEP_SKIP 4

PNL_API int pnl_widget_flow(pnl_app_t *app, pnl_widget_t **out_widget);
PNL_API int pnl_widget_add_step(pnl_widget_t *flow, const char *label, int *out_step);
PNL_API int pnl_widget_set_step(pnl_widget_t *flow, int step, int state, const char *detail);
PNL_API int pnl_widget_scene(pnl_app_t *app, pnl_widget_t **out_widget);
PNL_API int pnl_widget_add_scene(pnl_widget_t *scene, int state, const char *frames, int style, int gradient,
                                 int frame_ms);
PNL_API int pnl_app_set_banner_widget(pnl_app_t *app, pnl_widget_t *widget);
PNL_API int pnl_task_show(pnl_task_t *task, pnl_widget_t *widget);
PNL_API void pnl_canvas_gradient_fill(pnl_canvas_t *canvas, int x, int y, int width, int height, int gradient);
PNL_API void pnl_canvas_bar(pnl_canvas_t *canvas, int x, int y, int width, double fraction, int flags);
PNL_API void pnl_canvas_sparkline(pnl_canvas_t *canvas, int x, int y, int width, const double *values,
                                  size_t values_len, double max, int flags);
PNL_API int pnl_canvas_spinner(pnl_canvas_t *canvas, int x, int y, int style, int kind);
PNL_API void pnl_canvas_busy(pnl_canvas_t *canvas, int x, int y, int width, int flags);
PNL_API void pnl_canvas_chart(pnl_canvas_t *canvas, int x, int y, int width, int height, const double *values,
                              size_t values_len, double max, int flags);
PNL_API void pnl_canvas_columns(pnl_canvas_t *canvas, int x, int y, int width, int height, const double *values,
                                const char *const *labels, size_t count, double max, int flags);
PNL_API void pnl_canvas_plot(pnl_canvas_t *canvas, int x, int y, int width, int height, const double *values,
                             size_t values_len, double min, double max, int flags);
PNL_API int pnl_text_width(const char *text);
PNL_API size_t pnl_key_decode(const unsigned char *data, size_t data_len, int *out_key);
PNL_API const char *pnl_strerror(int status);

#ifdef __cplusplus
}
#endif

#endif

#if defined(PNL_IMPLEMENTATION) && !defined(PNL_IMPLEMENTATION_DONE)
#define PNL_IMPLEMENTATION_DONE
#ifndef PNL_INTERNAL_H
#define PNL_INTERNAL_H

#if defined(__linux__) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif


#include <stdint.h>

#if defined(PNL_IMPLEMENTATION_DONE) && defined(__GNUC__)
#define PNL_INTERNAL static __attribute__((unused))
#elif defined(PNL_IMPLEMENTATION_DONE)
#define PNL_INTERNAL static
#else
#define PNL_INTERNAL
#endif

#define PNL_INPUT_MAX 64
#define PNL_WATCH_MAX 64
#define PNL_ESCAPE_WAIT_MS 50
#define PNL_KEY_SUSPEND 0x1a
#define PNL_KEY_CTRL_C 3
#define PNL_KEY_FOCUS_IN 0x110011
#define PNL_KEY_FOCUS_OUT 0x110012
#define PNL_KEY_INSPECT 0x110013
#define PNL_EVENT_KEY 0
#define PNL_EVENT_WAKE 1
#define PNL_EVENT_TIMER 2
#define PNL_EVENT_WATCH 3
#define PNL_EVENT_RESIZE 4
#define PNL_STATUS_TIMER_NONE 0
#define PNL_TASK_TICK_MS 50
#define PNL_FRAME_MS 33
#define PNL_SPIN_MS 80
#define PNL_BUSY_MS 1600
#define PNL_GLIDE_MS 140
#define PNL_MOTION_ALL                                                                                          \
    (PNL_MOTION_SLIDE | PNL_MOTION_TRANSITION | PNL_MOTION_PULSE | PNL_MOTION_TYPEWRITER | PNL_MOTION_ENTER | \
     PNL_MOTION_FEEDBACK | PNL_MOTION_COUNT)
#define PNL_ENTER_MS 150
#define PNL_EXIT_MS 120
#define PNL_CASCADE_STEP_MS 30
#define PNL_CASCADE_STEPS 8
#define PNL_SHAKE_MS 240
#define PNL_COUNT_MS 450
#define PNL_SHIMMER_MS 1200
#define PNL_PULSE_MS 1200
#define PNL_PULSE_DEPTH 7
#define PNL_TYPE_CHAR_MS 12
#define PNL_TYPE_MAX_MS 400
#define PNL_SLOW_LINK_MS 60
#define PNL_TRANSITION_MS 160
#define PNL_LEVEL_FIRST 64
#define PNL_LEVEL_STEPS 64
#define PNL_FADE_FIRST 128
#define PNL_FADE_STEPS 16
#define PNL_FADE_MS 600
#define PNL_GRADIENT_STEPS_MAX 24
#define PNL_DEPTH_NONE 0
#define PNL_DEPTH_16 1
#define PNL_DEPTH_256 2
#define PNL_DEPTH_TRUE 3

#define PNL_STYLE_ATTRS(style) ((uint32_t)(style) & 0xffu)
#define PNL_STYLE_FG(style) (((uint32_t)(style) >> 8) & 0xffu)
#define PNL_STYLE_BG(style) (((uint32_t)(style) >> 16) & 0xffu)
#define PNL_STYLE_LINK(style) (((uint32_t)(style) >> 24) & 0xffu)
#define PNL_LINKS 255
#define PNL_LINK_POOL 8192
#define PNL_PLOT_ROWS 256
#define PNL_WIDGET_FOOTER 256
#define PNL_WIDGET_TEXT 1
#define PNL_WIDGET_BAR 2
#define PNL_WIDGET_GRAPH 3
#define PNL_WIDGET_SPINNER 4
#define PNL_WIDGET_STACK 5
#define PNL_WIDGET_INPUT 6
#define PNL_WIDGET_CHECK 7
#define PNL_WIDGET_CHOICE 8
#define PNL_WIDGET_SLIDER 9
#define PNL_WIDGET_BUTTON 10
#define PNL_WIDGET_BIG 11
#define PNL_WIDGET_ART 12
#define PNL_WIDGET_FLOW 13
#define PNL_WIDGET_SCENE 14

struct pnl_big_widget {
    pnl_app_t *app;
    char *text;
    int size;
    int style;
    int gradient;
};

struct pnl_widget_ops {
    int (*set_text)(pnl_widget_t *widget, const char *text);
    const char *(*get_text)(const pnl_widget_t *widget);
    int (*set_value)(pnl_widget_t *widget, double value);
    int (*get_value)(const pnl_widget_t *widget, double *out_value);
    int (*set_range)(pnl_widget_t *widget, double min, double max);
};

struct pnl_widget {
    pnl_app_t *app;
    pnl_widget_class_t widget_class;
    const struct pnl_widget_ops *ops;
    void *ctx;
    int kind;
    int focusable;
    pnl_change_fn change_fn;
    void *change_ctx;
    char footer_text[PNL_WIDGET_FOOTER];
    const char *footer;
};

struct pnl_mem {
    pnl_alloc_fn alloc_fn;
    pnl_free_fn free_fn;
    void *ctx;
};

struct pnl_cell {
    uint32_t cp;
    uint32_t style;
};

#define PNL_ROLES (PNL_ROLE_SURFACE - PNL_ROLE_ACCENT + 1)

struct pnl_palette {
    int depth;
    int light;
    unsigned long background;
    unsigned long rgb[PNL_ROLES];
    int x256[PNL_ROLES];
    int basic[PNL_ROLES];
    unsigned long custom[256];
    unsigned char custom_used[256];
};

struct pnl_theme {
    unsigned long dark[PNL_ROLES];
    unsigned long light[PNL_ROLES];
    int basic[PNL_ROLES];
};

struct pnl_screen {
    int width;
    int height;
    int front_valid;
    struct pnl_palette palette;
    int spin;
    int pulse;
    int spinning;
    int linear;
    int links_off;
    int links_len;
    size_t link_pool_len;
    unsigned long links_hash;
    unsigned long links_drawn;
    unsigned short *link_at;
    char *link_pool;
    struct pnl_cell *back;
    struct pnl_cell *front;
    char *out;
    size_t out_len;
    size_t out_cap;
};

struct pnl_term {
    int active;
    unsigned int surrogate;
    unsigned char input[PNL_INPUT_MAX];
    size_t input_len;
    int mouse;
    int linear;
    int mouse_x;
    int mouse_y;
    int focus_known;
    int focused;
    int wake_open;
    int wake_fd[2];
    void *wake_event;
    int woken;
};

union pnl_storage {
    long long align_int;
    double align_float;
    void *align_ptr;
    unsigned char bytes[128];
};

struct pnl_thread {
    void (*fn)(void *arg);
    void *arg;
    void *handle;
    union pnl_storage storage;
};

struct pnl_lock {
    union pnl_storage storage;
};

struct pnl_edit {
    char *text;
    size_t cap;
    size_t len;
    size_t cursor;
};

struct pnl_canvas {
    struct pnl_screen *screen;
    int focused;
    int x;
    int y;
    int width;
    int height;
};

struct pnl_action {
    char *label;
    pnl_action_fn fn;
    void *ctx;
};

struct pnl_line {
    int mark;
    double meter;
    char *label;
    char *text;
};

struct pnl_glide {
    double from;
    double to;
    long long start_ms;
};

#define PNL_BINDINGS 64

struct pnl_binding {
    int from;
    int to;
};

struct pnl_memo {
    char *key;
    char *value;
};

struct pnl_seen {
    unsigned long id;
    unsigned long hash;
    long long changed_ms;
    long long count_ms;
    unsigned long count_prefix;
    double count_from;
    double count_to;
    long long meter_ms;
    double meter_from;
    double meter_to;
};

struct pnl_timer {
    int id;
    int interval_ms;
    long long due_ms;
    pnl_timer_fn fn;
    void *ctx;
};

struct pnl_watch {
    int fd;
    pnl_fd_fn fn;
    void *ctx;
};

typedef void (*pnl_drop_fn)(pnl_app_t *app, void *ctx);

struct pnl_view {
    char *title;
    char *hints;
    const char *footer;
    const char *const *footer_ref;
    pnl_draw_fn draw_fn;
    pnl_key_fn key_fn;
    pnl_drop_fn drop_fn;
    void *ctx;
    const int *typing_ref;
    int dialog_width;
    int dialog_height;
    int origin_x;
    int origin_y;
    int box_x;
    int box_y;
    int box_width;
    int box_height;
    long long shown_ms;
};

struct pnl_ghost {
    long long ms;
    int x;
    int y;
    int width;
    int height;
};

#define PNL_CELL_MAX 256
#define PNL_TOASTS 3
#define PNL_TOAST_TEXT 120

struct pnl_toast {
    int mark;
    long long shown_ms;
    char text[PNL_TOAST_TEXT];
};

struct pnl_inspect;

struct pnl_app {
    struct pnl_mem mem;
    char *title;
    char *banner;
    int banner_gradient;
    int banner_set;
    pnl_widget_t *banner_widget;
    struct pnl_action *actions;
    size_t actions_len;
    size_t actions_cap;
    pnl_status_fn status_fn;
    void *status_ctx;
    int status_timer;
    struct pnl_line *lines;
    size_t lines_len;
    size_t lines_cap;
    struct pnl_seen *seen;
    size_t seen_len;
    size_t seen_cap;
    struct pnl_seen *seen_next;
    size_t seen_next_cap;
    int seen_ready;
    int in_status;
    size_t selected;
    size_t menu_top;
    struct pnl_view *views;
    size_t views_len;
    size_t views_cap;
    struct pnl_timer *timers;
    size_t timers_len;
    size_t timers_cap;
    int next_timer;
    struct pnl_watch *watches;
    size_t watches_len;
    size_t watches_cap;
    int tasks_running;
    int motion;
    int motion_flags;
    struct pnl_toast toasts[PNL_TOASTS];
    struct pnl_binding bindings[PNL_BINDINGS];
    size_t bindings_len;
    char *memory_path;
    char *record_path;
    struct pnl_inspect *inspect;
    struct pnl_memo *memos;
    size_t memos_len;
    size_t memos_cap;
    int memos_dirty;
    pnl_value_t **values;
    size_t values_len;
    size_t values_cap;
    int mouse_ox;
    int mouse_oy;
    int menu_y;
    int menu_end;
    size_t toasts_len;
    long reply_ms;
    struct pnl_glide select_glide;
    long long transition_ms;
    long long cascade_ms;
    long long shake_ms;
    long long chosen_ms;
    size_t chosen;
    struct pnl_ghost ghost;
    int transition_shift;
    int anim_frame;
    long long frame_ms;
    long long animate_until_ms;
    int running;
    int cli;
    int cli_json;
    int cli_yes;
    int cli_color;
    int cli_worst;
    int cli_needs_tty;
    int quit;
    int dirty;
    int widget_typing;
    struct pnl_term term;
    struct pnl_screen screen;
    struct pnl_theme theme;
};

PNL_INTERNAL void *pnl_mem_alloc(const struct pnl_mem *mem, size_t size);
PNL_INTERNAL void pnl_mem_free(const struct pnl_mem *mem, void *ptr);
PNL_INTERNAL char *pnl_mem_copy(const struct pnl_mem *mem, const char *text);
PNL_INTERNAL int pnl_mem_grow(const struct pnl_mem *mem, void **items, size_t *cap, size_t needed, size_t item_size);

PNL_INTERNAL size_t pnl_utf8_put(uint32_t cp, char *out);
PNL_INTERNAL size_t pnl_utf8_next(const char *text, size_t text_len, uint32_t *out_cp);
PNL_INTERNAL int pnl_cp_width(uint32_t cp);

PNL_INTERNAL int pnl_screen_resize(struct pnl_screen *screen, const struct pnl_mem *mem, int width, int height);
PNL_INTERNAL void pnl_screen_free(struct pnl_screen *screen, const struct pnl_mem *mem);
PNL_INTERNAL void pnl_screen_clear(struct pnl_screen *screen);
PNL_INTERNAL int pnl_screen_text(struct pnl_screen *screen, int x, int y, int max, uint32_t style, const char *text);
PNL_INTERNAL int pnl_screen_link(struct pnl_screen *screen, int x, int y, int max, uint32_t style, const char *text,
                                 const char *url);
PNL_INTERNAL void pnl_screen_fill(struct pnl_screen *screen, int x, int y, int width, uint32_t style);
PNL_INTERNAL int pnl_screen_render(struct pnl_screen *screen, const struct pnl_mem *mem);

PNL_INTERNAL int pnl_term_available(void);
PNL_INTERNAL int pnl_term_stdout_is_tty(void);
PNL_INTERNAL int pnl_term_stderr_is_tty(void);
PNL_INTERNAL int pnl_term_stdin_is_tty(void);
PNL_INTERNAL int pnl_term_enter(struct pnl_term *term);
PNL_INTERNAL void pnl_term_leave(struct pnl_term *term);
PNL_INTERNAL void pnl_term_suspend(struct pnl_term *term);
PNL_INTERNAL int pnl_term_resized(void);
PNL_INTERNAL void pnl_term_size(int *out_width, int *out_height);
PNL_INTERNAL int pnl_term_write(const char *data, size_t data_len);
PNL_INTERNAL int pnl_term_fill(struct pnl_term *term, int timeout_ms);
PNL_INTERNAL int pnl_term_wake_open(struct pnl_term *term);
PNL_INTERNAL void pnl_term_wake_close(struct pnl_term *term);
PNL_INTERNAL void pnl_term_wake(struct pnl_term *term);
PNL_INTERNAL void pnl_values_lines(pnl_app_t *app);
PNL_INTERNAL int pnl_term_read_key(struct pnl_term *term, int timeout_ms, int *out_key);
PNL_INTERNAL int pnl_term_wait(struct pnl_term *term, const struct pnl_watch *watches, size_t watches_len,
                               int timeout_ms, unsigned char *out_ready);
PNL_INTERNAL long long pnl_term_now_ms(void);
PNL_INTERNAL int pnl_os_enter(struct pnl_term *term);
PNL_INTERNAL void pnl_os_leave(struct pnl_term *term);
PNL_INTERNAL void pnl_os_suspend(struct pnl_term *term);
PNL_INTERNAL int pnl_os_resized(void);
PNL_INTERNAL void pnl_os_size(int *out_width, int *out_height);
PNL_INTERNAL int pnl_os_write(const char *data, size_t data_len);
PNL_INTERNAL void pnl_os_wake(struct pnl_term *term);
PNL_INTERNAL int pnl_os_read_key(struct pnl_term *term, int timeout_ms, int *out_key);
PNL_INTERNAL int pnl_os_wait(struct pnl_term *term, const struct pnl_watch *watches, size_t watches_len,
                             int timeout_ms, unsigned char *out_ready);
PNL_INTERNAL long long pnl_os_now_ms(void);
PNL_INTERNAL int pnl_virtual_active(void);
PNL_INTERNAL int pnl_env_off(const char *name);
PNL_INTERNAL long long pnl_clock_us(void);
PNL_INTERNAL void pnl_inspect_start(pnl_app_t *app);
PNL_INTERNAL void pnl_inspect_free(pnl_app_t *app);
PNL_INTERNAL void pnl_inspect_frame(pnl_app_t *app, long long took_us, size_t bytes);
PNL_INTERNAL void pnl_inspect_event(pnl_app_t *app, int kind, int arg);
PNL_INTERNAL void pnl_inspect_toggle(pnl_app_t *app);
PNL_INTERNAL void pnl_widget_paint(pnl_widget_t *widget, pnl_canvas_t *canvas);
PNL_INTERNAL const char *pnl_widget_hints(const pnl_widget_t *widget);
PNL_INTERNAL int pnl_widget_focusable(const pnl_widget_t *widget);
PNL_INTERNAL int pnl_widget_builtin(pnl_app_t *app, int kind, const pnl_widget_class_t *widget_class,
                                     const struct pnl_widget_ops *ops, void *ctx, pnl_widget_t **out_widget);
PNL_INTERNAL int pnl_widget_copy(pnl_app_t *app, char **slot, const char *text);
PNL_INTERNAL int pnl_banner_draw(pnl_app_t *app, pnl_canvas_t *canvas, int needed);
PNL_INTERNAL void pnl_widget_focus_edge(pnl_widget_t *widget, int last);
PNL_INTERNAL void pnl_widget_changed(pnl_widget_t *widget);
PNL_INTERNAL int pnl_art_size(const char *art, int *out_width);
PNL_INTERNAL int pnl_theme_line(struct pnl_theme *theme, char *line, int *light_set);
PNL_INTERNAL void pnl_memory_parse(pnl_app_t *app, char *data);
PNL_INTERNAL void pnl_keymap_line(pnl_app_t *app, char *line);
PNL_INTERNAL void pnl_edit_draw_styled(const struct pnl_edit *edit, pnl_canvas_t *canvas, int x, int y, int width,
                                       int focused, int style);
PNL_INTERNAL void pnl_record_open(pnl_app_t *app);
PNL_INTERNAL void pnl_record_output(const char *data, size_t data_len, int width, int height);
PNL_INTERNAL void pnl_record_close(void);
PNL_INTERNAL void pnl_nap_ms(int ms);

PNL_INTERNAL int pnl_thread_start(struct pnl_thread *thread, void (*fn)(void *arg), void *arg);
PNL_INTERNAL void pnl_thread_join(struct pnl_thread *thread);
PNL_INTERNAL int pnl_lock_init(struct pnl_lock *lock);
PNL_INTERNAL void pnl_lock_destroy(struct pnl_lock *lock);
PNL_INTERNAL void pnl_lock(struct pnl_lock *lock);
PNL_INTERNAL void pnl_unlock(struct pnl_lock *lock);

PNL_INTERNAL int pnl_color_detect(void);
PNL_INTERNAL int pnl_color_ink(char *buf, unsigned ink, int background, const struct pnl_palette *palette);
PNL_INTERNAL int pnl_is_light(long background_rgb);
PNL_INTERNAL void pnl_palette_init(struct pnl_palette *palette, const struct pnl_theme *theme, int depth, int light,
                                   long background_rgb, int high_contrast);
PNL_INTERNAL int pnl_rgb_to_256(unsigned long rgb);
PNL_INTERNAL int pnl_theme_choose(struct pnl_theme *theme, const char *name);
PNL_INTERNAL int pnl_background_detect(struct pnl_term *term, int depth, long *out_rgb, long *out_reply_ms);
PNL_INTERNAL long pnl_link_probe(struct pnl_term *term);

PNL_INTERNAL void pnl_tool_name(char *out, size_t out_len, const char *title);
PNL_INTERNAL int pnl_key_parse(const char *name, int *out_key);
PNL_INTERNAL void pnl_key_label(int key, char *out, size_t out_len);
PNL_INTERNAL int pnl_keymap_apply(const pnl_app_t *app, int key);
PNL_INTERNAL void pnl_keymap_load(pnl_app_t *app);
PNL_INTERNAL const char *pnl_memory_get(const pnl_app_t *app, const char *kind, const char *name);
PNL_INTERNAL void pnl_memory_set(pnl_app_t *app, const char *kind, const char *name, const char *value);
PNL_INTERNAL void pnl_memory_load(pnl_app_t *app);
PNL_INTERNAL void pnl_memory_save(pnl_app_t *app);
PNL_INTERNAL void pnl_memory_free(pnl_app_t *app);
PNL_INTERNAL void pnl_edit_init(struct pnl_edit *edit, char *text, size_t cap);
PNL_INTERNAL int pnl_edit_key(struct pnl_edit *edit, int key);
PNL_INTERNAL void pnl_edit_draw(const struct pnl_edit *edit, pnl_canvas_t *canvas, int x, int y, int width, int focused);
PNL_INTERNAL int pnl_level_color(double fraction);
PNL_INTERNAL int pnl_fade_style(pnl_app_t *app, const pnl_canvas_t *canvas, int role, long long start_ms, int from_step);
PNL_INTERNAL int pnl_motion_on(const pnl_app_t *app, int flag);
PNL_INTERNAL double pnl_glide_at(pnl_app_t *app, const struct pnl_glide *glide, int flag);
PNL_INTERNAL void pnl_glide_to(pnl_app_t *app, struct pnl_glide *glide, double target, int flag);
PNL_INTERNAL void pnl_dialog_draw(pnl_app_t *app, const pnl_canvas_t *area, struct pnl_view *view, int focused);
PNL_INTERNAL int pnl_menu_mouse(pnl_app_t *app, int key);
PNL_INTERNAL int pnl_label_width(const pnl_app_t *app);
PNL_INTERNAL void pnl_wheel_keys(pnl_app_t *app, pnl_key_fn key_fn, void *ctx, int key);
PNL_INTERNAL int pnl_is_mouse_key(int key);
PNL_INTERNAL long long pnl_toasts_next_ms(const pnl_app_t *app);
PNL_INTERNAL void pnl_toasts_expire(pnl_app_t *app);
PNL_INTERNAL void pnl_toasts_draw(pnl_app_t *app);
PNL_INTERNAL void pnl_ghost_draw(pnl_app_t *app);
PNL_INTERNAL double pnl_ease_out(double t);
PNL_INTERNAL int pnl_shake_offset(pnl_app_t *app);
PNL_INTERNAL void pnl_draw_spinner(pnl_canvas_t *canvas, int x, int y, int style);
PNL_INTERNAL int pnl_text_width_n(const char *text, size_t len);
PNL_INTERNAL int pnl_canvas_clip(pnl_canvas_t *canvas, int x, int y, int max, int style, const char *text);
PNL_INTERNAL int pnl_cli_table(pnl_app_t *app, const char *const *headers, const int *column_flags, size_t columns,
                               size_t rows, pnl_cell_fn cell_fn, void *ctx);
PNL_INTERNAL int pnl_text_contains_at(const char *text, const char *needle);
PNL_INTERNAL int pnl_text_contains(const char *text, const char *needle);

PNL_INTERNAL void pnl_menu_draw(pnl_app_t *app, pnl_canvas_t *canvas);
PNL_INTERNAL void pnl_help(pnl_app_t *app);
PNL_INTERNAL const char *pnl_view_hints(const pnl_app_t *app);
PNL_INTERNAL void pnl_palette(pnl_app_t *app);
PNL_INTERNAL int pnl_menu_key(pnl_app_t *app, int key);
PNL_INTERNAL void pnl_menu_clear_lines(pnl_app_t *app);
PNL_INTERNAL void pnl_menu_refresh(pnl_app_t *app);
PNL_INTERNAL int pnl_valid_mark(int mark);
PNL_INTERNAL int pnl_key_partial(const unsigned char *data, size_t data_len);
PNL_INTERNAL int pnl_term_complete(struct pnl_term *term);
PNL_INTERNAL size_t pnl_key_decode_at(const unsigned char *data, size_t data_len, int *out_key, int *out_x, int *out_y);
PNL_INTERNAL int pnl_run_interactive(pnl_app_t *app);
PNL_INTERNAL int pnl_cli_result(pnl_app_t *app, int mark, const char *text);
PNL_INTERNAL void pnl_cli_progress(const char *step, unsigned long long done, unsigned long long total);
PNL_INTERNAL int pnl_cli_read_line(const char *prompt, char *text, size_t text_len);
PNL_INTERNAL void pnl_task_notify(pnl_app_t *app, const char *title, long long started_ms, const char *outcome);
PNL_INTERNAL void pnl_json_string(const char *text);
PNL_INTERNAL void pnl_cli_log(const pnl_app_t *app, const char *line);
PNL_INTERNAL void pnl_draw_mark(pnl_canvas_t *canvas, int y, int mark, const char *text);
PNL_INTERNAL int pnl_view_push_internal(pnl_app_t *app, const struct pnl_view *view);
PNL_INTERNAL int pnl_loop(pnl_app_t *app, size_t min_views);
PNL_INTERNAL int pnl_view_modal(pnl_app_t *app, struct pnl_view *view);
PNL_INTERNAL void pnl_views_drop(pnl_app_t *app);
PNL_INTERNAL void pnl_timers_run(pnl_app_t *app);
PNL_INTERNAL int pnl_timers_timeout(const pnl_app_t *app);

#endif

#include <stdlib.h>
#include <string.h>

#define PNL_QUERY "\x1b]11;?\x1b\\\x1b[c"
#define PNL_QUERY_WAIT_MS 150

static int pnl_hex(int c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

static long pnl_parse_channel(const unsigned char *text, size_t len, size_t *pos)
{
    long value = 0;
    size_t digits = 0;

    while (*pos < len && pnl_hex(text[*pos]) >= 0) {
        if (digits < 2)
            value = value * 16 + pnl_hex(text[*pos]);
        digits++;
        (*pos)++;
    }
    if (digits == 0)
        return -1;
    return digits == 1 ? value * 17 : value;
}

static long pnl_parse_rgb(const unsigned char *text, size_t len)
{
    static const char prefix[] = "rgb:";
    size_t pos = sizeof(prefix) - 1;
    long rgb = 0;
    int i;

    if (len < pos || memcmp(text, prefix, pos) != 0)
        return -1;
    for (i = 0; i < 3; i++) {
        long channel = pnl_parse_channel(text, len, &pos);

        if (channel < 0 || (i < 2 && (pos >= len || text[pos++] != '/')))
            return -1;
        rgb = rgb << 8 | channel;
    }
    return rgb;
}

static size_t pnl_find(const struct pnl_term *term, const char *needle, size_t from)
{
    size_t len = strlen(needle);
    size_t i;

    for (i = from; i + len <= term->input_len; i++) {
        if (memcmp(term->input + i, needle, len) == 0)
            return i;
    }
    return (size_t)-1;
}

static void pnl_cut(struct pnl_term *term, size_t from, size_t to)
{
    memmove(term->input + from, term->input + to, term->input_len - to);
    term->input_len -= to - from;
}

static long pnl_take_osc(struct pnl_term *term)
{
    size_t start = pnl_find(term, "\x1b]11;", 0);
    size_t end;
    size_t body;
    long rgb;

    if (start == (size_t)-1)
        return -2;
    body = start + 5;
    for (end = body; end < term->input_len && term->input[end] != 0x07 && term->input[end] != 0x1b; end++)
        ;
    if (end >= term->input_len || (term->input[end] == 0x1b && end + 1 >= term->input_len))
        return -2;
    rgb = pnl_parse_rgb(term->input + body, end - body);
    pnl_cut(term, start, end + (term->input[end] == 0x1b ? 2 : 1));
    return rgb;
}

static int pnl_take_da(struct pnl_term *term)
{
    size_t start = pnl_find(term, "\x1b[?", 0);
    size_t end;

    if (start == (size_t)-1)
        return 0;
    for (end = start + 3; end < term->input_len && term->input[end] != 'c'; end++) {
        if (term->input[end] != ';' && (term->input[end] < '0' || term->input[end] > '9'))
            return 0;
    }
    if (end >= term->input_len)
        return 0;
    pnl_cut(term, start, end + 1);
    return 1;
}

static int pnl_background_env(long *out_rgb)
{
    const char *forced = getenv("TUIKIT_BACKGROUND");
    const char *fgbg = getenv("COLORFGBG");
    const char *last;

    if (forced != NULL && (strcmp(forced, "light") == 0 || strcmp(forced, "dark") == 0)) {
        *out_rgb = forced[0] == 'l' ? 0xffffffL : 0x000000L;
        return 1;
    }
    if (fgbg == NULL || (last = (strrchr)(fgbg, ';')) == NULL)
        return 0;
    *out_rgb = atoi(last + 1) == 7 || atoi(last + 1) == 15 ? 0xffffffL : 0x000000L;
    return 1;
}

static long pnl_query(struct pnl_term *term, const char *query, size_t query_len, long *out_rgb, long *out_reply_ms)
{
    long long sent = pnl_term_now_ms();
    long long deadline = sent + PNL_QUERY_WAIT_MS;
    long rgb = -2;

    *out_reply_ms = -1;
    if (pnl_term_write(query, query_len) != PNL_OK)
        return -1;
    for (;;) {
        long long left = deadline - pnl_term_now_ms();
        int answered;

        rgb = pnl_take_osc(term);
        answered = rgb != -2 || pnl_take_da(term);
        if (answered && *out_reply_ms < 0)
            *out_reply_ms = (long)(pnl_term_now_ms() - sent);
        if (answered || left <= 0 || pnl_term_fill(term, (int)left) <= 0)
            break;
    }
    pnl_take_da(term);
    *out_rgb = rgb;
    return rgb;
}

long pnl_link_probe(struct pnl_term *term)
{
    long rgb;
    long reply_ms;

    pnl_query(term, "\x1b[c", 3, &rgb, &reply_ms);
    return reply_ms;
}

int pnl_background_detect(struct pnl_term *term, int depth, long *out_rgb, long *out_reply_ms)
{
    long rgb;

    *out_rgb = -1;
    *out_reply_ms = -1;
    if (pnl_background_env(out_rgb))
        return 1;
    if (depth == PNL_DEPTH_NONE)
        return 0;
    pnl_query(term, PNL_QUERY, sizeof(PNL_QUERY) - 1, &rgb, out_reply_ms);
    if (rgb < 0)
        return 0;
    *out_rgb = rgb;
    return 1;
}

#include <string.h>

#define PNL_GLYPH_ROWS 5
#define PNL_BANNER_ROOM 8

struct pnl_glyph {
    char key;
    const char *rows;
};

static const struct pnl_glyph pnl_glyphs[] = {
    {'0', "#### ## ## ####"}, {'1', " # ##  #  # ###"}, {'2', "###  #####  ###"}, {'3', "###  ####  ####"},
    {'4', "# ## ####  #  #"}, {'5', "####  ###  ####"}, {'6', "####  #### ####"}, {'7', "###  #  #  #  #"},
    {'8', "#### ##### ####"}, {'9', "#### ####  ####"}, {'A', " # # ##### ## #"}, {'B', "## # ### # ### "},
    {'C', " ###  #  #   ##"}, {'D', "## # ## ## ### "}, {'E', "####  ## #  ###"}, {'F', "####  ## #  #  "},
    {'G', " ###  # ## # ##"}, {'H', "# ## ##### ## #"}, {'I', "### #  #  # ###"}, {'J', "  #  #  ## # # "},
    {'K', "# ## ### # ## #"}, {'L', "#  #  #  #  ###"}, {'M', "# ######## ## #"}, {'N', "## # ## ## ## #"},
    {'O', " # # ## ## # # "}, {'P', "## # ### #  #  "}, {'Q', " # # ## ###  ##"}, {'R', "## # ### # ## #"},
    {'S', " ###   #   ### "}, {'T', "### #  #  #  # "}, {'U', "# ## ## ## ####"}, {'V', "# ## ## ## # # "},
    {'W', "# ## ######## #"}, {'X', "# ## # # # ## #"}, {'Y', "# ## # #  #  # "}, {'Z', "###  # # #  ###"},
    {' ', "               "}, {'.', "             # "}, {',', "          #  # "}, {':', "    #     #    "},
    {';', "    #     #  # "}, {'-', "      ###      "}, {'+', "    # ### #    "}, {'=', "   ###   ###   "},
    {'_', "            ###"}, {'%', "# #  # # #  # #"}, {'/', "  #  # # #  #  "}, {'!', " #  #  #     # "},
    {'?', "###  # ##    # "}, {'(', "  # #  #  #   #"}, {')', "#   #  #  # #  "}, {'<', "  # # #   #   #"},
    {'>', "#   #   # # #  "}, {'*', "   # # # # #   "}, {'#', "# ##### ##### #"}, {'\'', " #  #          "},
    {'"', "# ## #         "}, {'$', " ####  #  #### "}, {'@', "#### #####  ###"}, {'|', " #  #  #  #  # "},
};

static const char *pnl_glyph_rows(int c)
{
    size_t i;

    if (c >= 'a' && c <= 'z')
        c -= 'a' - 'A';
    for (i = 0; i < sizeof(pnl_glyphs) / sizeof(pnl_glyphs[0]); i++) {
        if (pnl_glyphs[i].key == c)
            return pnl_glyphs[i].rows;
    }
    return NULL;
}

static int pnl_glyph_narrow(int c)
{
    return c != '\0' && (strchr)(".,:;!'|", c) != NULL;
}

static int pnl_pixel(const char *rows, int narrow, int column, int row)
{
    if (row >= PNL_GLYPH_ROWS)
        return 0;
    return rows[row * 3 + (narrow ? 1 : column)] == '#';
}

static int pnl_glyph_columns(int c)
{
    if (c == ' ')
        return 2;
    return pnl_glyph_narrow(c) ? 1 : 3;
}

static int pnl_big_scale(int size)
{
    return size == PNL_BIG_LARGE ? 2 : 1;
}

int pnl_big_height(int size)
{
    return size == PNL_BIG_LARGE ? PNL_GLYPH_ROWS : (PNL_GLYPH_ROWS + 1) / 2;
}

static int pnl_big_next(const char **text)
{
    uint32_t cp;

    *text += pnl_utf8_next(*text, strlen(*text), &cp);
    return cp < 0x80 && pnl_glyph_rows((int)cp) != NULL ? (int)cp : '?';
}

int pnl_big_width(int size, const char *text)
{
    int columns = 0;
    const char *p = text;

    if (text == NULL || (size != PNL_BIG_SMALL && size != PNL_BIG_LARGE))
        return 0;
    while (*p != '\0') {
        int c = pnl_big_next(&p);

        columns += pnl_glyph_columns(c) + (*p != '\0');
    }
    return columns * pnl_big_scale(size);
}

static int pnl_big_ink(int style, int gradient, int column, int width)
{
    if (gradient == 0)
        return style;
    return (style & ~0xff00) | PNL_FG(pnl_gradient_color(gradient, width > 1 ? (double)column / (width - 1) : 0.0));
}

static void pnl_big_cell(pnl_canvas_t *canvas, int x, int y, int style, int top, int bottom)
{
    if (top && bottom)
        pnl_canvas_text(canvas, x, y, style, "\xe2\x96\x88");
    else if (top)
        pnl_canvas_text(canvas, x, y, style, "\xe2\x96\x80");
    else if (bottom)
        pnl_canvas_text(canvas, x, y, style, "\xe2\x96\x84");
}

static void pnl_big_glyph(pnl_canvas_t *canvas, int x, int y, int size, int style, int gradient, int start,
                          int width, int c)
{
    const char *rows = pnl_glyph_rows(c);
    int narrow = pnl_glyph_narrow(c);
    int columns = pnl_glyph_columns(c);
    int scale = pnl_big_scale(size);
    int column;
    int row;

    for (column = 0; column < columns && c != ' '; column++) {
        int ink = pnl_big_ink(style, gradient, start + column * scale, width);

        for (row = 0; row < pnl_big_height(size); row++) {
            if (size == PNL_BIG_LARGE && pnl_pixel(rows, narrow, column, row))
                pnl_canvas_text(canvas, x + column * 2, y + row, ink, "\xe2\x96\x88\xe2\x96\x88");
            else if (size == PNL_BIG_SMALL)
                pnl_big_cell(canvas, x + column, y + row, ink, pnl_pixel(rows, narrow, column, row * 2),
                             pnl_pixel(rows, narrow, column, row * 2 + 1));
        }
    }
}

int pnl_canvas_big(pnl_canvas_t *canvas, int x, int y, int size, int style, int gradient, const char *text)
{
    int width = pnl_big_width(size, text);
    int start = 0;
    const char *p = text;

    if (canvas == NULL || width == 0)
        return x;
    if (canvas->screen->linear)
        return pnl_canvas_text(canvas, x, y, style, text);
    while (*p != '\0') {
        int c = pnl_big_next(&p);

        pnl_big_glyph(canvas, x + start, y, size, style, gradient, start, width, c);
        start += (pnl_glyph_columns(c) + 1) * pnl_big_scale(size);
    }
    return x + width;
}

int pnl_art_size(const char *art, int *out_width)
{
    int lines = 1;
    int width = 0;
    const char *line = art;

    while (line != NULL) {
        const char *end = (strchr)(line, '\n');
        size_t len = end != NULL ? (size_t)(end - line) : strlen(line);
        int w = pnl_text_width_n(line, len);

        width = w > width ? w : width;
        line = end != NULL && end[1] != '\0' ? end + 1 : NULL;
        lines += line != NULL;
    }
    *out_width = width;
    return lines;
}

int pnl_canvas_art(pnl_canvas_t *canvas, int x, int y, int style, int gradient, const char *art)
{
    int width = 0;
    int lines;
    int row;
    const char *line = art;

    if (canvas == NULL || art == NULL || canvas->screen->linear)
        return 0;
    lines = pnl_art_size(art, &width);
    for (row = 0; row < lines && line != NULL; row++) {
        const char *end = (strchr)(line, '\n');
        size_t len = end != NULL ? (size_t)(end - line) : strlen(line);
        size_t at = 0;
        int column = 0;

        while (at < len) {
            char glyph[8];
            uint32_t cp;
            size_t n = pnl_utf8_next(line + at, len - at, &cp);

            memcpy(glyph, line + at, n);
            glyph[n] = '\0';
            if (glyph[0] != ' ')
                pnl_canvas_text(canvas, x + column, y + row, pnl_big_ink(style, gradient, column, width), glyph);
            column += pnl_text_width(glyph);
            at += n;
        }
        line = end != NULL ? end + 1 : NULL;
    }
    return lines;
}

int pnl_app_set_banner(pnl_app_t *app, const char *art, int gradient)
{
    char *copy = NULL;

    if (app == NULL)
        return PNL_ERR_ARG;
    if (art != NULL && (copy = pnl_mem_copy(&app->mem, art)) == NULL)
        return PNL_ERR_NOMEM;
    pnl_mem_free(&app->mem, app->banner);
    app->banner = copy;
    app->banner_gradient = gradient;
    app->banner_set = 1;
    app->dirty = 1;
    return PNL_OK;
}

int pnl_app_set_banner_widget(pnl_app_t *app, pnl_widget_t *widget)
{
    if (app == NULL || (widget != NULL && widget->app != app))
        return PNL_ERR_ARG;
    app->banner_widget = widget;
    app->dirty = 1;
    return PNL_OK;
}

static int pnl_banner_widget_draw(pnl_app_t *app, pnl_canvas_t *canvas, int needed)
{
    int width = pnl_canvas_width(canvas) - 2;
    int height = pnl_widget_height(app->banner_widget, width);

    if (height <= 0 || height + needed + PNL_BANNER_ROOM / 4 > pnl_canvas_height(canvas))
        return 0;
    pnl_widget_draw(app->banner_widget, canvas, 2, 0, width, height);
    return height + 1;
}

int pnl_banner_draw(pnl_app_t *app, pnl_canvas_t *canvas, int needed)
{
    int style = PNL_BOLD | PNL_FG(PNL_ROLE_ACCENT);
    int width = 0;
    int height;

    if (canvas->screen->linear)
        return 0;
    if (app->banner_widget != NULL)
        return pnl_banner_widget_draw(app, canvas, needed);
    if (!app->banner_set)
        return 0;
    height = app->banner != NULL ? pnl_art_size(app->banner, &width) : pnl_big_height(PNL_BIG_SMALL);
    if (app->banner == NULL)
        width = pnl_big_width(PNL_BIG_SMALL, app->title);
    if (width + 2 > pnl_canvas_width(canvas) || height + needed + PNL_BANNER_ROOM / 4 > pnl_canvas_height(canvas))
        return 0;
    if (app->banner != NULL)
        pnl_canvas_art(canvas, 2, 0, style, app->banner_gradient, app->banner);
    else
        pnl_canvas_big(canvas, 2, 0, PNL_BIG_SMALL, style, app->banner_gradient, app->title);
    return height + 1;
}

static void pnl_big_widget_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_big_widget *big = ctx;

    (void)app;
    pnl_canvas_big(canvas, 0, 0, big->size, big->style, big->gradient, big->text);
}

static int pnl_big_widget_height(void *ctx, pnl_app_t *app, int width)
{
    struct pnl_big_widget *big = ctx;

    (void)width;
    return app->screen.linear ? 1 : pnl_big_height(big->size);
}

static void pnl_art_widget_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_big_widget *art = ctx;

    (void)app;
    pnl_canvas_art(canvas, 0, 0, art->style, art->gradient, art->text);
}

static int pnl_art_widget_height(void *ctx, pnl_app_t *app, int width)
{
    struct pnl_big_widget *art = ctx;
    int art_width = 0;

    (void)width;
    return app->screen.linear ? 0 : pnl_art_size(art->text, &art_width);
}

static void pnl_big_widget_release(void *ctx)
{
    struct pnl_big_widget *big = ctx;

    pnl_mem_free(&big->app->mem, big->text);
    pnl_mem_free(&big->app->mem, big);
}

static int pnl_big_set(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_big_widget *)widget->ctx)->text, text);
}

static const char *pnl_big_get(const pnl_widget_t *widget)
{
    return ((const struct pnl_big_widget *)widget->ctx)->text;
}

static int pnl_big_make(pnl_app_t *app, int kind, const pnl_widget_class_t *widget_class, const char *text,
                        int size, int style, int gradient, pnl_widget_t **out_widget)
{
    struct pnl_big_widget *big = pnl_mem_alloc(&app->mem, sizeof(*big));

    if (big == NULL)
        return PNL_ERR_NOMEM;
    memset(big, 0, sizeof(*big));
    big->app = app;
    big->size = size;
    big->style = style;
    big->gradient = gradient;
    if (pnl_widget_copy(app, &big->text, text) != PNL_OK) {
        pnl_big_widget_release(big);
        return PNL_ERR_NOMEM;
    }
    static const struct pnl_widget_ops big_ops = {pnl_big_set, pnl_big_get, NULL, NULL, NULL};

    return pnl_widget_builtin(app, kind, widget_class, &big_ops, big, out_widget);
}

int pnl_widget_big(pnl_app_t *app, const char *text, int size, int style, int gradient, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t big_class = {pnl_big_widget_draw, NULL, pnl_big_widget_height, NULL,
                                                 pnl_big_widget_release};

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || text == NULL || out_widget == NULL || (size != PNL_BIG_SMALL && size != PNL_BIG_LARGE))
        return PNL_ERR_ARG;
    return pnl_big_make(app, PNL_WIDGET_BIG, &big_class, text, size, style, gradient, out_widget);
}

int pnl_widget_art(pnl_app_t *app, const char *art, int style, int gradient, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t art_class = {pnl_art_widget_draw, NULL, pnl_art_widget_height, NULL,
                                                 pnl_big_widget_release};

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || art == NULL || out_widget == NULL)
        return PNL_ERR_ARG;
    return pnl_big_make(app, PNL_WIDGET_ART, &art_class, art, 0, style, gradient, out_widget);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_EXIT_OK 0
#define PNL_EXIT_FAIL 1
#define PNL_EXIT_USAGE 2
#define PNL_COMMAND_MAX 64

static const char *const pnl_mark_names[] = {"ok", "..", "!!"};
static const char *const pnl_mark_json[] = {"ok", "wait", "fail"};
static const char *const pnl_mark_colors[] = {"\x1b[32m", "\x1b[33m", "\x1b[31m"};

static int pnl_command_char(unsigned char c)
{
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 'a';
    if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c >= 0x80)
        return c;
    return 0;
}

static void pnl_command_of(const char *label, char *out)
{
    size_t len = 0;
    int dash = 0;

    for (; *label != '\0' && len < PNL_COMMAND_MAX - 2; label++) {
        int c = pnl_command_char((unsigned char)*label);

        if (c == 0) {
            dash = 1;
            continue;
        }
        if (dash && len > 0)
            out[len++] = '-';
        out[len++] = (char)c;
        dash = 0;
    }
    out[len] = '\0';
}

void pnl_json_string(const char *text)
{
    putchar('"');
    for (; *text != '\0'; text++) {
        unsigned char c = (unsigned char)*text;

        if (c == '"' || c == '\\')
            printf("\\%c", c);
        else if (c < 0x20)
            printf("\\u%04x", c);
        else
            putchar(c);
    }
    putchar('"');
}

static void pnl_cli_line(pnl_app_t *app, int mark, const char *text)
{
    if (mark > app->cli_worst)
        app->cli_worst = mark;
    if (app->cli_color)
        printf("[%s%s\x1b[0m] %s\n", pnl_mark_colors[mark], pnl_mark_names[mark], text);
    else
        printf("[%s] %s\n", pnl_mark_names[mark], text);
}

static void pnl_json_fields(int mark, const char *text)
{
    printf("{\"mark\":\"%s\",\"text\":", pnl_mark_json[mark]);
    pnl_json_string(text);
}

static void pnl_json_entry(int mark, const char *text)
{
    pnl_json_fields(mark, text);
    putchar('}');
}

int pnl_cli_result(pnl_app_t *app, int mark, const char *text)
{
    if (!app->cli_json) {
        pnl_cli_line(app, mark, text);
        return PNL_OK;
    }
    if (mark > app->cli_worst)
        app->cli_worst = mark;
    pnl_json_entry(mark, text);
    putchar('\n');
    return PNL_OK;
}

void pnl_cli_progress(const char *step, unsigned long long done, unsigned long long total)
{
    if (!pnl_term_stderr_is_tty())
        return;
    if (total > 0)
        fprintf(stderr, "\r\x1b[K%3d%%  %s", (int)(done >= total ? 100 : done * 100 / total), step);
    else
        fprintf(stderr, "\r\x1b[K%s", step);
    fflush(stderr);
}

void pnl_cli_log(const pnl_app_t *app, const char *line)
{
    if (app->cli_json)
        return;
    if (pnl_term_stderr_is_tty())
        fputs("\r\x1b[K", stderr);
    puts(line);
    fflush(stdout);
}

int pnl_cli_read_line(const char *prompt, char *text, size_t text_len)
{
    size_t len;
    int c;

    if (pnl_term_stdin_is_tty()) {
        fprintf(stderr, "%s", prompt);
        fflush(stderr);
    }
    fflush(stdout);
    if (fgets(text, (int)(text_len > 4096 ? 4096 : text_len), stdin) == NULL)
        return 0;
    len = strlen(text);
    if (len > 0 && text[len - 1] == '\n') {
        text[--len] = '\0';
        if (len > 0 && text[len - 1] == '\r')
            text[--len] = '\0';
        return 1;
    }
    while ((c = getchar()) != EOF && c != '\n')
        ;
    return 1;
}

static const char *pnl_cli_cell(const char *const *headers, pnl_cell_fn cell_fn, void *ctx, size_t row,
                                size_t column, char *buf)
{
    const char *text;

    buf[0] = '\0';
    text = row == 0 ? headers[column] : cell_fn(ctx, row - 1, column, buf, PNL_CELL_MAX);

    return text ? text : "";
}

static void pnl_cli_table_json(const char *const *headers, size_t columns, size_t rows, pnl_cell_fn cell_fn,
                               void *ctx)
{
    char buf[PNL_CELL_MAX];
    size_t r;
    size_t c;

    putchar('[');
    for (r = 1; r <= rows; r++) {
        printf(r > 1 ? ",{" : "{");
        for (c = 0; c < columns; c++) {
            if (c > 0)
                putchar(',');
            pnl_json_string(pnl_cli_cell(headers, cell_fn, ctx, 0, c, buf));
            putchar(':');
            pnl_json_string(pnl_cli_cell(headers, cell_fn, ctx, r, c, buf));
        }
        putchar('}');
    }
    printf("]\n");
}

static int pnl_cli_column_width(const char *const *headers, pnl_cell_fn cell_fn, void *ctx, size_t rows,
                                size_t column)
{
    char buf[PNL_CELL_MAX];
    int width = 0;
    size_t r;

    for (r = 0; r <= rows; r++) {
        int w = pnl_text_width(pnl_cli_cell(headers, cell_fn, ctx, r, column, buf));

        width = w > width ? w : width;
    }
    return width;
}

int pnl_cli_table(pnl_app_t *app, const char *const *headers, const int *column_flags, size_t columns, size_t rows,
                  pnl_cell_fn cell_fn, void *ctx)
{
    char buf[PNL_CELL_MAX];
    int *widths;
    size_t r;
    size_t c;

    if (app->cli_json) {
        pnl_cli_table_json(headers, columns, rows, cell_fn, ctx);
        return PNL_OK;
    }
    widths = pnl_mem_alloc(&app->mem, columns * sizeof(*widths));
    if (widths == NULL)
        return PNL_ERR_NOMEM;
    for (c = 0; c < columns; c++)
        widths[c] = pnl_cli_column_width(headers, cell_fn, ctx, rows, c);
    for (r = 0; r <= rows; r++) {
        for (c = 0; c < columns; c++) {
            const char *text = pnl_cli_cell(headers, cell_fn, ctx, r, c, buf);
            int pad = widths[c] - pnl_text_width(text);
            int right = column_flags != NULL && (column_flags[c] & PNL_COLUMN_RIGHT);

            printf("%*s%s%*s", right ? pad : 0, "", text, c + 1 < columns ? (right ? 0 : pad) + 2 : 0, "");
        }
        putchar('\n');
    }
    fflush(stdout);
    pnl_mem_free(&app->mem, widths);
    return PNL_OK;
}

static void pnl_cli_status_line(pnl_app_t *app, const struct pnl_line *line, int label_width)
{
    char percent[16] = "";
    char text[640];

    if (line->meter >= 0.0)
        snprintf(percent, sizeof(percent), "%s%d%%", line->text[0] != '\0' ? "  " : "",
                 (int)(line->meter * 100 + 0.5));
    if (line->label != NULL)
        snprintf(text, sizeof(text), "%s%*s  %s%s", line->label,
                 label_width > pnl_text_width(line->label) ? label_width - pnl_text_width(line->label) : 0, "",
                 line->text, percent);
    else
        snprintf(text, sizeof(text), "%s%s", line->text, percent);
    pnl_cli_line(app, line->mark, text);
}

static void pnl_cli_status(pnl_app_t *app)
{
    size_t i;

    pnl_menu_refresh(app);
    if (app->cli_json)
        printf("{\"status\":[");
    for (i = 0; i < app->lines_len; i++) {
        const struct pnl_line *line = &app->lines[i];

        if (!app->cli_json) {
            pnl_cli_status_line(app, line, pnl_label_width(app));
            continue;
        }
        if (line->mark > app->cli_worst)
            app->cli_worst = line->mark;
        if (i > 0)
            putchar(',');
        printf("{\"mark\":\"%s\"", pnl_mark_json[line->mark]);
        if (line->label != NULL) {
            printf(",\"label\":");
            pnl_json_string(line->label);
        }
        printf(",\"text\":");
        pnl_json_string(line->text);
        if (line->meter >= 0.0)
            printf(",\"meter\":%.3f", line->meter);
        putchar('}');
    }
    if (app->cli_json)
        printf("]}\n");
    pnl_menu_clear_lines(app);
}

static const char *pnl_program_name(const pnl_app_t *app, int argc, char **argv)
{
    const char *name;
    const char *slash;

    if (argc < 1 || argv == NULL || argv[0] == NULL || argv[0][0] == '\0')
        return app->title;
    name = argv[0];
    slash = (strrchr)(name, '/');
    if (slash == NULL)
        slash = (strrchr)(name, '\\');
    return slash ? slash + 1 : name;
}

static void pnl_cli_help(const pnl_app_t *app, const char *program, FILE *out)
{
    char command[PNL_COMMAND_MAX];
    size_t i;

    fprintf(out, "usage: %s [command] [--json] [--yes]\n\n%s\n\ncommands:\n", program, app->title);
    fprintf(out, "  %-20s %s\n", "status", "show the status lines");
    for (i = 0; i < app->actions_len; i++) {
        pnl_command_of(app->actions[i].label, command);
        fprintf(out, "  %-20s %s\n", command, app->actions[i].label);
    }
    fprintf(out, "\nwithout a command it opens the interactive panel, or prints the status when\n"
                 "the output is not a terminal\n");
}

static int pnl_find_action(const pnl_app_t *app, const char *name)
{
    char command[PNL_COMMAND_MAX];
    size_t i;

    for (i = 0; i < app->actions_len; i++) {
        pnl_command_of(app->actions[i].label, command);
        if (strcmp(command, name) == 0)
            return (int)i;
    }
    return -1;
}

struct pnl_args {
    const char *command;
    int help;
    int json;
    int yes;
    const char *bad;
};

static void pnl_parse_args(int argc, char **argv, struct pnl_args *args)
{
    int i;

    memset(args, 0, sizeof(*args));
    for (i = 1; i < argc && argv != NULL; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
            args->help = 1;
        else if (strcmp(arg, "--json") == 0)
            args->json = 1;
        else if (strcmp(arg, "--yes") == 0 || strcmp(arg, "-y") == 0)
            args->yes = 1;
        else if (arg[0] == '-' || args->command != NULL)
            args->bad = args->bad ? args->bad : arg;
        else
            args->command = arg;
    }
}

static int pnl_exit_for(const pnl_app_t *app)
{
    if (app->cli_needs_tty)
        return PNL_EXIT_FAIL;
    return app->cli_worst == PNL_MARK_FAIL ? PNL_EXIT_FAIL : PNL_EXIT_OK;
}

static int pnl_cli_action(pnl_app_t *app, int index)
{
    app->actions[index].fn(app->actions[index].ctx, app);
    fflush(stdout);
    if (app->cli_needs_tty)
        fprintf(stderr, "%s: this action needs an interactive terminal\n", app->actions[index].label);
    return pnl_exit_for(app);
}

static int pnl_cli_dispatch(pnl_app_t *app, const struct pnl_args *args, const char *program)
{
    int index;

    if (args->help) {
        pnl_cli_help(app, program, stdout);
        return PNL_EXIT_OK;
    }
    if (args->bad != NULL) {
        fprintf(stderr, "%s: unexpected argument '%s'\n\n", program, args->bad);
        pnl_cli_help(app, program, stderr);
        return PNL_EXIT_USAGE;
    }
    if (args->command == NULL || strcmp(args->command, "status") == 0) {
        pnl_cli_status(app);
        return pnl_exit_for(app);
    }
    index = pnl_find_action(app, args->command);
    if (index >= 0)
        return pnl_cli_action(app, index);
    fprintf(stderr, "%s: unknown command '%s'\n\n", program, args->command);
    pnl_cli_help(app, program, stderr);
    return PNL_EXIT_USAGE;
}

static int pnl_wants_color(void)
{
    return pnl_color_detect() != PNL_DEPTH_NONE && pnl_term_stdout_is_tty();
}

int pnl_app_run(pnl_app_t *app, int argc, char **argv, int *out_exit_code)
{
    struct pnl_args args;
    int exit_code;

    if (app == NULL || argc < 0 || (argc > 0 && argv == NULL))
        return PNL_ERR_ARG;
    pnl_parse_args(argc, argv, &args);
    if (args.command == NULL && !args.help && args.bad == NULL && !args.json && pnl_term_available()) {
        int status = pnl_run_interactive(app);

        if (out_exit_code != NULL)
            *out_exit_code = status == PNL_OK ? PNL_EXIT_OK : PNL_EXIT_FAIL;
        return status;
    }
    app->cli = 1;
    app->cli_json = args.json;
    app->cli_yes = args.yes;
    app->cli_color = !args.json && pnl_wants_color();
    app->cli_worst = PNL_MARK_OK;
    app->cli_needs_tty = 0;
    exit_code = pnl_cli_dispatch(app, &args, pnl_program_name(app, argc, argv));
    fflush(stdout);
    app->cli = 0;
    if (out_exit_code != NULL)
        *out_exit_code = exit_code;
    return PNL_OK;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_ROLE_FIRST PNL_ROLE_ACCENT
#define PNL_CONTRAST_TEXT 4.5
#define PNL_CONTRAST_MUTED 3.0
#define PNL_CONTRAST_HIGH 7.0
#define PNL_DARK_BACKGROUND 0x1c1c1cL
#define PNL_LIGHT_BACKGROUND 0xf5f5f5L

static double pnl_channel(unsigned long rgb, int shift)
{
    double c = (double)((rgb >> shift) & 0xff) / 255.0;

    return c * c;
}

static double pnl_luminance(unsigned long rgb)
{
    return 0.2126 * pnl_channel(rgb, 16) + 0.7152 * pnl_channel(rgb, 8) + 0.0722 * pnl_channel(rgb, 0);
}

static double pnl_contrast(unsigned long a, unsigned long b)
{
    double la = pnl_luminance(a) + 0.05;
    double lb = pnl_luminance(b) + 0.05;

    return la > lb ? la / lb : lb / la;
}

static unsigned long pnl_mix(unsigned long rgb, unsigned long target, int percent)
{
    unsigned long out = 0;
    int shift;

    for (shift = 0; shift <= 16; shift += 8) {
        long from = (long)((rgb >> shift) & 0xff);
        long to = (long)((target >> shift) & 0xff);

        out |= (unsigned long)(from + (to - from) * percent / 100) << shift;
    }
    return out;
}

static unsigned long pnl_readable(unsigned long rgb, unsigned long background, double minimum)
{
    unsigned long target = pnl_luminance(background) > 0.25 ? 0x000000UL : 0xffffffUL;
    int steps;

    for (steps = 0; steps < 20 && pnl_contrast(rgb, background) < minimum; steps++)
        rgb = pnl_mix(rgb, target, 15);
    return rgb;
}

int pnl_is_light(long background_rgb)
{
    return pnl_luminance((unsigned long)background_rgb) > 0.25;
}

static int pnl_cube_step(unsigned long value)
{
    return value < 48 ? 0 : value < 115 ? 1 : (int)((value - 35) / 40);
}

static unsigned long pnl_cube_value(int step)
{
    return step == 0 ? 0 : (unsigned long)(55 + step * 40);
}

static unsigned long pnl_distance(unsigned long a, unsigned long b)
{
    unsigned long sum = 0;
    int shift;

    for (shift = 0; shift <= 16; shift += 8) {
        long d = (long)((a >> shift) & 0xff) - (long)((b >> shift) & 0xff);

        sum += (unsigned long)(d * d);
    }
    return sum;
}

int pnl_rgb_to_256(unsigned long rgb)
{
    int r = pnl_cube_step((rgb >> 16) & 0xff);
    int g = pnl_cube_step((rgb >> 8) & 0xff);
    int b = pnl_cube_step(rgb & 0xff);
    unsigned long cube = pnl_cube_value(r) << 16 | pnl_cube_value(g) << 8 | pnl_cube_value(b);
    unsigned long average = (((rgb >> 16) & 0xff) + ((rgb >> 8) & 0xff) + (rgb & 0xff)) / 3;
    int gray = average > 238 ? 23 : average < 8 ? 0 : (int)((average - 8) / 10);
    unsigned long level = (unsigned long)(8 + gray * 10);

    if (pnl_distance(rgb, level << 16 | level << 8 | level) < pnl_distance(rgb, cube))
        return 232 + gray;
    return 16 + 36 * r + 6 * g + b;
}

void pnl_palette_init(struct pnl_palette *palette, const struct pnl_theme *theme, int depth, int light,
                      long background_rgb, int high_contrast)
{
    const unsigned long *roles = light ? theme->light : theme->dark;
    unsigned long background = background_rgb >= 0 ? (unsigned long)background_rgb
                                                   : (unsigned long)(light ? PNL_LIGHT_BACKGROUND : PNL_DARK_BACKGROUND);
    int i;

    palette->depth = depth;
    palette->light = light;
    palette->background = background;
    for (i = 0; i < PNL_ROLES; i++) {
        double minimum = i + PNL_ROLE_FIRST == PNL_ROLE_MUTED ? PNL_CONTRAST_MUTED : PNL_CONTRAST_TEXT;

        if (high_contrast)
            minimum = minimum < PNL_CONTRAST_TEXT ? PNL_CONTRAST_TEXT : PNL_CONTRAST_HIGH;

        palette->rgb[i] = roles[i];
        if (i + PNL_ROLE_FIRST != PNL_ROLE_SURFACE)
            palette->rgb[i] = pnl_readable(roles[i], background, minimum);
        palette->x256[i] = pnl_rgb_to_256(palette->rgb[i]);
        palette->basic[i] = theme->basic[i];
    }
}

static int pnl_env_has(const char *name, const char *needle)
{
    const char *value = getenv(name);

    return value != NULL && (strstr)(value, needle) != NULL;
}

int pnl_color_detect(void)
{
    const char *no_color = getenv("NO_COLOR");
    const char *term = getenv("TERM");

    if (no_color != NULL && no_color[0] != '\0')
        return PNL_DEPTH_NONE;
    if (term != NULL && strcmp(term, "dumb") == 0)
        return PNL_DEPTH_NONE;
    if (pnl_env_has("COLORTERM", "truecolor") || pnl_env_has("COLORTERM", "24bit"))
        return PNL_DEPTH_TRUE;
#if defined(_WIN32)
    if (term == NULL)
        return PNL_DEPTH_TRUE;
#endif
    if (pnl_env_has("TERM", "256color") || pnl_env_has("TERM", "direct"))
        return PNL_DEPTH_256;
    return PNL_DEPTH_16;
}

int pnl_color_depth(void)
{
    static const int counts[] = {0, 16, 256, 16777216};

    return counts[pnl_color_detect()];
}

static int pnl_ink_basic(char *buf, unsigned ink, int base, int bright)
{
    if (ink == PNL_COLOR_DEFAULT || ink > PNL_COLOR_BRIGHT_WHITE)
        return sprintf(buf, ";%d", base + 9);
    if (ink <= 8)
        return sprintf(buf, ";%d", base + (int)ink - 1);
    return sprintf(buf, ";%d", bright + (int)ink - 9);
}

static unsigned long pnl_level_rgb(const struct pnl_palette *palette, int step)
{
    unsigned long ok = palette->rgb[PNL_ROLE_OK - PNL_ROLE_FIRST];
    unsigned long warn = palette->rgb[PNL_ROLE_WARN - PNL_ROLE_FIRST];
    unsigned long fail = palette->rgb[PNL_ROLE_FAIL - PNL_ROLE_FIRST];
    int half = PNL_LEVEL_STEPS / 2;

    if (step <= half)
        return pnl_mix(ok, warn, step * 100 / half);
    return pnl_mix(warn, fail, (step - half) * 100 / (PNL_LEVEL_STEPS - 1 - half));
}

static int pnl_level_ink(char *buf, int step, int base, const struct pnl_palette *palette)
{
    unsigned long rgb = pnl_level_rgb(palette, step);
    int basic = step * 3 < PNL_LEVEL_STEPS ? PNL_COLOR_GREEN : step * 3 < PNL_LEVEL_STEPS * 2 ? PNL_COLOR_YELLOW
                                                                                             : PNL_COLOR_RED;

    if (palette->depth == PNL_DEPTH_TRUE)
        return sprintf(buf, ";%d;2;%lu;%lu;%lu", base + 8, (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
    if (palette->depth == PNL_DEPTH_256)
        return sprintf(buf, ";%d;5;%d", base + 8, pnl_rgb_to_256(rgb));
    return pnl_ink_basic(buf, (unsigned)basic, base, base + 60);
}

static int pnl_fade_ink(char *buf, int fade, int base, const struct pnl_palette *palette)
{
    int role = fade / PNL_FADE_STEPS;
    int step = fade % PNL_FADE_STEPS;
    unsigned long rgb = pnl_mix(palette->rgb[role], palette->background, step * 100 / (PNL_FADE_STEPS - 1));

    if (palette->depth == PNL_DEPTH_TRUE)
        return sprintf(buf, ";%d;2;%lu;%lu;%lu", base + 8, (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
    return sprintf(buf, ";%d;5;%d", base + 8, pnl_rgb_to_256(rgb));
}

int pnl_fade_style(pnl_app_t *app, const pnl_canvas_t *canvas, int role, long long start_ms, int from_step)
{
    double t = pnl_app_tween(app, start_ms, PNL_FADE_MS);
    int depth = canvas->screen->palette.depth;
    int step;

    if (t >= 1.0)
        return 0;
    if (depth != PNL_DEPTH_TRUE && depth != PNL_DEPTH_256)
        return t < 0.5 ? PNL_BOLD : 0;
    step = from_step + (int)(t * (PNL_FADE_STEPS - 1 - from_step));
    return PNL_BG(PNL_FADE_FIRST + (role - PNL_ROLE_FIRST) * PNL_FADE_STEPS + step);
}

static const unsigned long pnl_basic_rgb[] = {0x000000UL, 0xcd0000UL, 0x00cd00UL, 0xcdcd00UL, 0x0000eeUL, 0xcd00cdUL,
                                              0x00cdcdUL, 0xe5e5e5UL, 0x7f7f7fUL, 0xff0000UL, 0x00ff00UL, 0xffff00UL,
                                              0x5c5cffUL, 0xff00ffUL, 0x00ffffUL, 0xffffffUL};

static unsigned pnl_nearest_basic(unsigned long rgb)
{
    unsigned best = 0;
    unsigned i;

    for (i = 1; i < sizeof(pnl_basic_rgb) / sizeof(pnl_basic_rgb[0]); i++) {
        if (pnl_distance(rgb, pnl_basic_rgb[i]) < pnl_distance(rgb, pnl_basic_rgb[best]))
            best = i;
    }
    return best + 1;
}

static int pnl_custom_ink(char *buf, unsigned long rgb, int base, int depth)
{
    if (depth == PNL_DEPTH_TRUE)
        return sprintf(buf, ";%d;2;%lu;%lu;%lu", base + 8, (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
    if (depth == PNL_DEPTH_256)
        return sprintf(buf, ";%d;5;%d", base + 8, pnl_rgb_to_256(rgb));
    return pnl_ink_basic(buf, pnl_nearest_basic(rgb), base, base + 60);
}

int pnl_color_ink(char *buf, unsigned ink, int background, const struct pnl_palette *palette)
{
    int base = background ? 40 : 30;
    unsigned long rgb;
    int role;

    if (ink < 256 && palette->custom_used[ink])
        return pnl_custom_ink(buf, palette->custom[ink], base, palette->depth);
    if (ink >= PNL_FADE_FIRST && ink < PNL_FADE_FIRST + PNL_ROLES * PNL_FADE_STEPS)
        return pnl_fade_ink(buf, (int)ink - PNL_FADE_FIRST, base, palette);
    if (ink >= PNL_LEVEL_FIRST && ink < PNL_LEVEL_FIRST + PNL_LEVEL_STEPS)
        return pnl_level_ink(buf, (int)ink - PNL_LEVEL_FIRST, base, palette);
    if (ink < PNL_ROLE_FIRST || ink >= PNL_ROLE_FIRST + PNL_ROLES)
        return pnl_ink_basic(buf, ink, base, base + 60);
    role = (int)ink - PNL_ROLE_FIRST;
    rgb = palette->rgb[role];
    if (palette->depth == PNL_DEPTH_TRUE)
        return sprintf(buf, ";%d;2;%lu;%lu;%lu", base + 8, (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
    if (palette->depth == PNL_DEPTH_256)
        return sprintf(buf, ";%d;5;%d", base + 8, palette->x256[role]);
    return pnl_ink_basic(buf, (unsigned)palette->basic[role], base, base + 60);
}

static const int pnl_custom_ranges[3][2] = {{17, 31}, {240, 255}, {39, 63}};

static int pnl_custom_run(const struct pnl_palette *palette, int length, int gradient)
{
    int r;

    for (r = 0; r < 3; r++) {
        const int *range = pnl_custom_ranges[gradient ? 2 - r : r];
        int start;

        for (start = range[0]; start + length - 1 <= range[1]; start++) {
            int i = 0;

            while (i < length && !palette->custom_used[start + i])
                i++;
            if (i == length)
                return start;
        }
    }
    return -1;
}

int pnl_app_color(pnl_app_t *app, unsigned long rgb, int *out_color)
{
    struct pnl_palette *palette;
    int slot;
    int i;

    if (out_color != NULL)
        *out_color = PNL_COLOR_DEFAULT;
    if (app == NULL || out_color == NULL || rgb > 0xffffffUL)
        return PNL_ERR_ARG;
    palette = &app->screen.palette;
    for (i = 0; i < 256; i++) {
        if (palette->custom_used[i] == 1 && palette->custom[i] == rgb) {
            *out_color = i;
            return PNL_OK;
        }
    }
    slot = pnl_custom_run(palette, 1, 0);
    if (slot < 0)
        return PNL_ERR_LIMIT;
    palette->custom[slot] = rgb;
    palette->custom_used[slot] = 1;
    *out_color = slot;
    return PNL_OK;
}

static unsigned long pnl_gradient_rgb(const unsigned long *stops, size_t count, double t)
{
    double place = t * (double)(count - 1);
    size_t low = (size_t)place;

    if (low >= count - 1)
        return stops[count - 1];
    return pnl_mix(stops[low], stops[low + 1], (int)((place - (double)low) * 100.0 + 0.5));
}

int pnl_app_gradient(pnl_app_t *app, const unsigned long *stops, size_t count, int steps, int *out_gradient)
{
    struct pnl_palette *palette;
    int first;
    int i;

    if (out_gradient != NULL)
        *out_gradient = 0;
    if (app == NULL || stops == NULL || out_gradient == NULL || count < 2 || steps < 2 ||
        steps > PNL_GRADIENT_STEPS_MAX)
        return PNL_ERR_ARG;
    for (i = 0; i < (int)count; i++) {
        if (stops[i] > 0xffffffUL)
            return PNL_ERR_ARG;
    }
    palette = &app->screen.palette;
    first = pnl_custom_run(palette, steps, 1);
    if (first < 0)
        return PNL_ERR_LIMIT;
    for (i = 0; i < steps; i++) {
        palette->custom[first + i] = pnl_gradient_rgb(stops, count, (double)i / (double)(steps - 1));
        palette->custom_used[first + i] = 2;
    }
    *out_gradient = first | steps << 8;
    return PNL_OK;
}

int pnl_gradient_color(int gradient, double fraction)
{
    int first = gradient & 0xff;
    int steps = gradient >> 8 & 0xff;

    if (steps < 2)
        return first;
    fraction = fraction > 0.0 ? (fraction < 1.0 ? fraction : 1.0) : 0.0;
    return first + (int)(fraction * (steps - 1) + 0.5);
}

int pnl_canvas_gradient_text(pnl_canvas_t *canvas, int x, int y, int style, int gradient, const char *text)
{
    size_t count = 0;
    size_t index = 0;
    const char *p;

    if (canvas == NULL || text == NULL)
        return x;
    for (p = text; *p != '\0'; count++) {
        uint32_t cp;

        p += pnl_utf8_next(p, strlen(p), &cp);
    }
    for (p = text; *p != '\0';) {
        char glyph[8];
        uint32_t cp;
        size_t len = pnl_utf8_next(p, strlen(p), &cp);
        double t = count > 1 ? (double)index / (double)(count - 1) : 0.0;

        memcpy(glyph, p, len);
        glyph[len] = '\0';
        x = pnl_canvas_text(canvas, x, y, (style & ~0xff00) | PNL_FG(pnl_gradient_color(gradient, t)), glyph);
        p += len;
        index++;
    }
    return x;
}

void pnl_canvas_gradient_fill(pnl_canvas_t *canvas, int x, int y, int width, int height, int gradient)
{
    int i;

    if (canvas == NULL || width <= 0)
        return;
    for (i = 0; i < width; i++) {
        double t = width > 1 ? (double)i / (double)(width - 1) : 0.0;

        pnl_canvas_fill(canvas, x + i, y, 1, height, PNL_BG(pnl_gradient_color(gradient, t)));
    }
}

#include <string.h>

#define PNL_KEY_CTRL_A 0x01
#define PNL_KEY_CTRL_E 0x05
#define PNL_KEY_CTRL_U 0x15
#define PNL_KEY_CTRL_W 0x17

void pnl_edit_init(struct pnl_edit *edit, char *text, size_t cap)
{
    edit->text = text;
    edit->cap = cap;
    edit->len = strlen(text);
    edit->cursor = edit->len;
}

static size_t pnl_edit_prev(const struct pnl_edit *edit, size_t pos)
{
    while (pos > 0 && ((unsigned char)edit->text[--pos] & 0xc0) == 0x80)
        ;
    return pos;
}

static size_t pnl_edit_next(const struct pnl_edit *edit, size_t pos)
{
    while (pos < edit->len && ((unsigned char)edit->text[++pos] & 0xc0) == 0x80)
        ;
    return pos;
}

static void pnl_edit_cut(struct pnl_edit *edit, size_t from, size_t to)
{
    memmove(edit->text + from, edit->text + to, edit->len - to + 1);
    edit->len -= to - from;
    edit->cursor = from;
}

static int pnl_edit_insert(struct pnl_edit *edit, int cp)
{
    char bytes[4];
    size_t n = pnl_utf8_put((uint32_t)cp, bytes);

    if (edit->len + n >= edit->cap)
        return 1;
    memmove(edit->text + edit->cursor + n, edit->text + edit->cursor, edit->len - edit->cursor + 1);
    memcpy(edit->text + edit->cursor, bytes, n);
    edit->len += n;
    edit->cursor += n;
    return 1;
}

static size_t pnl_edit_word_start(const struct pnl_edit *edit)
{
    size_t pos = edit->cursor;

    while (pos > 0 && edit->text[pos - 1] == ' ')
        pos--;
    while (pos > 0 && edit->text[pos - 1] != ' ')
        pos--;
    return pos;
}

int pnl_edit_key(struct pnl_edit *edit, int key)
{
    if (key == PNL_KEY_LEFT)
        edit->cursor = pnl_edit_prev(edit, edit->cursor);
    else if (key == PNL_KEY_RIGHT)
        edit->cursor = pnl_edit_next(edit, edit->cursor);
    else if (key == PNL_KEY_HOME || key == PNL_KEY_CTRL_A)
        edit->cursor = 0;
    else if (key == PNL_KEY_END || key == PNL_KEY_CTRL_E)
        edit->cursor = edit->len;
    else if (key == PNL_KEY_BACKSPACE && edit->cursor > 0)
        pnl_edit_cut(edit, pnl_edit_prev(edit, edit->cursor), edit->cursor);
    else if (key == PNL_KEY_DELETE && edit->cursor < edit->len)
        pnl_edit_cut(edit, edit->cursor, pnl_edit_next(edit, edit->cursor));
    else if (key == PNL_KEY_CTRL_U)
        pnl_edit_cut(edit, 0, edit->cursor);
    else if (key == PNL_KEY_CTRL_W)
        pnl_edit_cut(edit, pnl_edit_word_start(edit), edit->cursor);
    else if (key >= 0x20 && key < 0x110000 && key != 0x7f)
        return pnl_edit_insert(edit, key);
    else
        return key == PNL_KEY_BACKSPACE || key == PNL_KEY_DELETE;
    return 1;
}

static int pnl_edit_width(const char *text, size_t len)
{
    size_t pos = 0;
    int width = 0;

    while (pos < len) {
        uint32_t cp;
        int w;

        pos += pnl_utf8_next(text + pos, len - pos, &cp);
        w = pnl_cp_width(cp);
        width += w < 0 ? 1 : w;
    }
    return width;
}

void pnl_edit_draw(const struct pnl_edit *edit, pnl_canvas_t *canvas, int x, int y, int width, int focused)
{
    pnl_edit_draw_styled(edit, canvas, x, y, width, focused, 0);
}

void pnl_edit_draw_styled(const struct pnl_edit *edit, pnl_canvas_t *canvas, int x, int y, int width, int focused,
                          int style)
{
    size_t start = 0;
    char cursor_cell[5] = " ";
    size_t cell_len;
    int cursor_x;

    if (width < 2)
        return;
    while (pnl_edit_width(edit->text + start, edit->cursor - start) > width - 1)
        start = pnl_edit_next(edit, start);
    cursor_x = x + pnl_edit_width(edit->text + start, edit->cursor - start);
    pnl_canvas_text(canvas, x, y, style, edit->text + start);
    if (!focused)
        return;
    cell_len = pnl_edit_next(edit, edit->cursor) - edit->cursor;
    if (cell_len > 0 && cell_len < sizeof(cursor_cell)) {
        memcpy(cursor_cell, edit->text + edit->cursor, cell_len);
        cursor_cell[cell_len] = '\0';
    }
    pnl_canvas_text(canvas, cursor_x, y, style | PNL_REVERSE, cursor_cell);
}

#include <stdio.h>
#include <string.h>

#define PNL_FIELD_MAX 256
#define PNL_PRESS_MS 250

struct pnl_input_widget {
    pnl_app_t *app;
    pnl_widget_t *self;
    char *label;
    int flags;
    char text[PNL_FIELD_MAX];
    struct pnl_edit edit;
};

struct pnl_check_widget {
    pnl_app_t *app;
    pnl_widget_t *self;
    char *label;
    int checked;
};

struct pnl_choice_widget {
    pnl_app_t *app;
    pnl_widget_t *self;
    char *label;
    char **options;
    size_t count;
    size_t index;
};

struct pnl_slider_widget {
    pnl_app_t *app;
    pnl_widget_t *self;
    char *label;
    double min;
    double max;
    double step;
    double value;
    struct pnl_glide glide;
    int bar_x;
    int bar_width;
};

struct pnl_button_widget {
    pnl_app_t *app;
    pnl_widget_t *self;
    char *label;
    pnl_action_fn action_fn;
    void *action_ctx;
    long long pressed_ms;
};

static int pnl_form_label(pnl_canvas_t *canvas, const char *label)
{
    int style = pnl_canvas_focused(canvas) ? PNL_BOLD | PNL_FG(PNL_ROLE_ACCENT) : PNL_FG(PNL_ROLE_MUTED);

    if (label == NULL)
        return 0;
    pnl_canvas_text(canvas, 0, 0, style, label);
    return pnl_text_width(label) + 1;
}

static int pnl_form_mouse_x(const pnl_app_t *app)
{
    return app->term.mouse_x - app->mouse_ox;
}

static void pnl_form_release(pnl_app_t *app, char *label, void *widget)
{
    pnl_mem_free(&app->mem, label);
    pnl_mem_free(&app->mem, widget);
}

static int pnl_field_line(void *ctx, pnl_app_t *app, int width)
{
    (void)ctx;
    (void)app;
    (void)width;
    return 1;
}

static void *pnl_form_alloc(pnl_app_t *app, size_t size)
{
    void *widget = pnl_mem_alloc(&app->mem, size);

    if (widget != NULL)
        memset(widget, 0, size);
    return widget;
}

static int pnl_input_set(pnl_widget_t *widget, const char *text)
{
    struct pnl_input_widget *input = widget->ctx;
    size_t len = strlen(text);

    if (len >= sizeof(input->text))
        return PNL_ERR_ARG;
    memcpy(input->text, text, len + 1);
    pnl_edit_init(&input->edit, input->text, sizeof(input->text));
    widget->app->dirty = 1;
    return PNL_OK;
}

static const char *pnl_input_get(const pnl_widget_t *widget)
{
    return ((const struct pnl_input_widget *)widget->ctx)->text;
}

static int pnl_check_set_label(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_check_widget *)widget->ctx)->label, text);
}

static const char *pnl_check_label(const pnl_widget_t *widget)
{
    return ((const struct pnl_check_widget *)widget->ctx)->label;
}

static int pnl_check_set(pnl_widget_t *widget, double value)
{
    ((struct pnl_check_widget *)widget->ctx)->checked = value != 0.0;
    widget->app->dirty = 1;
    return PNL_OK;
}

static int pnl_check_get(const pnl_widget_t *widget, double *out_value)
{
    *out_value = ((const struct pnl_check_widget *)widget->ctx)->checked;
    return PNL_OK;
}

static int pnl_choice_pick(pnl_widget_t *widget, const char *text)
{
    struct pnl_choice_widget *choice = widget->ctx;
    size_t i;

    for (i = 0; i < choice->count; i++) {
        if (strcmp(choice->options[i], text) == 0) {
            choice->index = i;
            widget->app->dirty = 1;
            return PNL_OK;
        }
    }
    return PNL_ERR_NOT_FOUND;
}

static const char *pnl_choice_option(const pnl_widget_t *widget)
{
    const struct pnl_choice_widget *choice = widget->ctx;

    return choice->options[choice->index];
}

static int pnl_choice_set(pnl_widget_t *widget, double value)
{
    struct pnl_choice_widget *choice = widget->ctx;

    if (value < 0.0 || value >= (double)choice->count)
        return PNL_ERR_ARG;
    choice->index = (size_t)value;
    widget->app->dirty = 1;
    return PNL_OK;
}

static int pnl_choice_get(const pnl_widget_t *widget, double *out_value)
{
    *out_value = (double)((const struct pnl_choice_widget *)widget->ctx)->index;
    return PNL_OK;
}

static int pnl_button_set_label(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_button_widget *)widget->ctx)->label, text);
}

static const char *pnl_button_label(const pnl_widget_t *widget)
{
    return ((const struct pnl_button_widget *)widget->ctx)->label;
}

static int pnl_form_finish(pnl_app_t *app, int kind, const pnl_widget_class_t *widget_class,
                           const struct pnl_widget_ops *ops, void *ctx, pnl_widget_t **self, pnl_widget_t **out_widget)
{
    int status = pnl_widget_builtin(app, kind, widget_class, ops, ctx, out_widget);

    if (status != PNL_OK)
        return status;
    *self = *out_widget;
    (*out_widget)->focusable = 1;
    return PNL_OK;
}

static void pnl_field_masked(const struct pnl_input_widget *input, char *masked, struct pnl_edit *edit)
{
    size_t i;
    size_t n = 0;
    size_t cursor = 0;

    for (i = 0; i < input->edit.len; i++) {
        if (((unsigned char)input->text[i] & 0xc0) == 0x80)
            continue;
        if (i < input->edit.cursor)
            cursor++;
        masked[n++] = '*';
    }
    masked[n] = '\0';
    edit->text = masked;
    edit->cap = PNL_FIELD_MAX;
    edit->len = n;
    edit->cursor = cursor;
}

static void pnl_field_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_input_widget *input = ctx;
    int focused = pnl_canvas_focused(canvas);
    int x = pnl_form_label(canvas, input->label);
    int width = pnl_canvas_width(canvas) - x;
    char masked[PNL_FIELD_MAX];
    struct pnl_edit shown = input->edit;

    if (width <= 0)
        return;
    if (focused)
        app->widget_typing = 1;
    if (input->flags & PNL_INPUT_SECRET)
        pnl_field_masked(input, masked, &shown);
    pnl_canvas_fill(canvas, x, 0, width, 1, PNL_BG(PNL_ROLE_SURFACE));
    pnl_edit_draw_styled(&shown, canvas, x, 0, width, focused, PNL_BG(PNL_ROLE_SURFACE));
}

static int pnl_field_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_input_widget *input = ctx;
    char before[PNL_FIELD_MAX];

    (void)app;
    if (key == PNL_KEY_CLICK)
        return 1;
    if (pnl_is_mouse_key(key))
        return 0;
    memcpy(before, input->text, input->edit.len + 1);
    if (!pnl_edit_key(&input->edit, key))
        return 0;
    if (strcmp(before, input->text) != 0)
        pnl_widget_changed(input->self);
    return 1;
}

static const char *pnl_field_hints(void *ctx)
{
    (void)ctx;
    return "enter: next field";
}

static void pnl_field_release(void *ctx)
{
    struct pnl_input_widget *input = ctx;

    pnl_form_release(input->app, input->label, input);
}

int pnl_widget_input(pnl_app_t *app, const char *label, int flags, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t input_class = {pnl_field_draw, pnl_field_key, pnl_field_line, pnl_field_hints,
                                                   pnl_field_release};
    static const struct pnl_widget_ops input_ops = {pnl_input_set, pnl_input_get, NULL, NULL, NULL};
    struct pnl_input_widget *input;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL || (flags & ~PNL_INPUT_SECRET) != 0)
        return PNL_ERR_ARG;
    input = pnl_form_alloc(app, sizeof(*input));
    if (input != NULL)
        input->app = app;
    if (input == NULL || pnl_widget_copy(app, &input->label, label) != PNL_OK) {
        if (input != NULL)
            pnl_field_release(input);
        return PNL_ERR_NOMEM;
    }
    input->app = app;
    input->flags = flags;
    pnl_edit_init(&input->edit, input->text, sizeof(input->text));
    return pnl_form_finish(app, PNL_WIDGET_INPUT, &input_class, &input_ops, input, &input->self, out_widget);
}

static void pnl_check_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_check_widget *check = ctx;
    int focused = pnl_canvas_focused(canvas);
    int box = focused ? PNL_BOLD | PNL_FG(PNL_ROLE_ACCENT) : PNL_FG(PNL_ROLE_MUTED);

    (void)app;
    pnl_canvas_text(canvas, 0, 0, box, check->checked ? "[x]" : "[ ]");
    if (check->label != NULL)
        pnl_canvas_text(canvas, 4, 0, focused ? PNL_BOLD : 0, check->label);
}

static int pnl_check_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_check_widget *check = ctx;

    (void)app;
    if (key != ' ' && key != PNL_KEY_CLICK)
        return 0;
    check->checked = !check->checked;
    pnl_widget_changed(check->self);
    return 1;
}

static const char *pnl_check_hints(void *ctx)
{
    (void)ctx;
    return "space: toggle";
}

static void pnl_check_release(void *ctx)
{
    struct pnl_check_widget *check = ctx;

    pnl_form_release(check->app, check->label, check);
}

int pnl_widget_check(pnl_app_t *app, const char *label, int checked, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t check_class = {pnl_check_draw, pnl_check_key, pnl_field_line, pnl_check_hints,
                                                   pnl_check_release};
    static const struct pnl_widget_ops check_ops = {pnl_check_set_label, pnl_check_label, pnl_check_set, pnl_check_get, NULL};
    struct pnl_check_widget *check;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL)
        return PNL_ERR_ARG;
    check = pnl_form_alloc(app, sizeof(*check));
    if (check != NULL)
        check->app = app;
    if (check == NULL || pnl_widget_copy(app, &check->label, label) != PNL_OK) {
        if (check != NULL)
            pnl_check_release(check);
        return PNL_ERR_NOMEM;
    }
    check->app = app;
    check->checked = checked != 0;
    return pnl_form_finish(app, PNL_WIDGET_CHECK, &check_class, &check_ops, check, &check->self, out_widget);
}

static void pnl_choice_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_choice_widget *choice = ctx;
    int focused = pnl_canvas_focused(canvas);
    int arrows = focused ? PNL_BOLD | PNL_FG(PNL_ROLE_ACCENT) : PNL_FG(PNL_ROLE_MUTED);
    int x = pnl_form_label(canvas, choice->label);

    (void)app;
    x = pnl_canvas_text(canvas, x, 0, arrows, "< ");
    x = pnl_canvas_text(canvas, x, 0, focused ? PNL_BOLD : 0, choice->options[choice->index]);
    pnl_canvas_text(canvas, x, 0, arrows, " >");
}

static int pnl_choice_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_choice_widget *choice = ctx;
    size_t back = choice->index > 0 ? choice->index - 1 : choice->count - 1;

    (void)app;
    if (key == PNL_KEY_LEFT)
        choice->index = back;
    else if (key == PNL_KEY_RIGHT || key == ' ' || key == PNL_KEY_CLICK)
        choice->index = (choice->index + 1) % choice->count;
    else
        return 0;
    pnl_widget_changed(choice->self);
    return 1;
}

static const char *pnl_choice_hints(void *ctx)
{
    (void)ctx;
    return "left/right: change";
}

static void pnl_choice_release(void *ctx)
{
    struct pnl_choice_widget *choice = ctx;
    size_t i;

    for (i = 0; choice->options != NULL && i < choice->count; i++)
        pnl_mem_free(&choice->app->mem, choice->options[i]);
    pnl_mem_free(&choice->app->mem, choice->options);
    pnl_form_release(choice->app, choice->label, choice);
}

static int pnl_choice_copy(struct pnl_choice_widget *choice, const char *const *options, size_t count)
{
    size_t i;

    choice->options = pnl_mem_alloc(&choice->app->mem, count * sizeof(*choice->options));
    if (choice->options == NULL)
        return PNL_ERR_NOMEM;
    memset(choice->options, 0, count * sizeof(*choice->options));
    choice->count = count;
    for (i = 0; i < count; i++) {
        if (pnl_widget_copy(choice->app, &choice->options[i], options[i]) != PNL_OK)
            return PNL_ERR_NOMEM;
    }
    return PNL_OK;
}

int pnl_widget_choice(pnl_app_t *app, const char *label, const char *const *options, size_t count,
                      pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t choice_class = {pnl_choice_draw, pnl_choice_key, pnl_field_line, pnl_choice_hints,
                                                    pnl_choice_release};
    static const struct pnl_widget_ops choice_ops = {pnl_choice_pick, pnl_choice_option, pnl_choice_set, pnl_choice_get, NULL};
    struct pnl_choice_widget *choice;
    size_t i;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL || options == NULL || count == 0)
        return PNL_ERR_ARG;
    for (i = 0; i < count; i++) {
        if (options[i] == NULL)
            return PNL_ERR_ARG;
    }
    choice = pnl_form_alloc(app, sizeof(*choice));
    if (choice == NULL)
        return PNL_ERR_NOMEM;
    choice->app = app;
    if (pnl_widget_copy(app, &choice->label, label) != PNL_OK || pnl_choice_copy(choice, options, count) != PNL_OK) {
        pnl_choice_release(choice);
        return PNL_ERR_NOMEM;
    }
    return pnl_form_finish(app, PNL_WIDGET_CHOICE, &choice_class, &choice_ops, choice, &choice->self, out_widget);
}

static double pnl_slider_fit(const struct pnl_slider_widget *slider, double value)
{
    double steps;

    value = value < slider->min ? slider->min : value > slider->max ? slider->max : value;
    if (slider->step <= 0.0)
        return value;
    steps = (value - slider->min) / slider->step + 0.5;
    if (!(steps < 9.0e18))
        return value;
    value = slider->min + (double)(long long)steps * slider->step;
    return value > slider->max ? slider->max : value;
}

static double pnl_slider_share(const struct pnl_slider_widget *slider, double value)
{
    return slider->max > slider->min ? (value - slider->min) / (slider->max - slider->min) : 0.0;
}

static void pnl_slider_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_slider_widget *slider = ctx;
    int focused = pnl_canvas_focused(canvas);
    int x = pnl_form_label(canvas, slider->label);
    char number[32];
    int width;

    snprintf(number, sizeof(number), " %g", slider->value);
    width = pnl_canvas_width(canvas) - x - (int)strlen(number);
    slider->bar_x = x;
    slider->bar_width = width;
    if (width <= 0)
        return;
    pnl_canvas_bar(canvas, x, 0, width, pnl_glide_at(app, &slider->glide, PNL_MOTION_COUNT),
                   focused ? PNL_METER_ACCENT : 0);
    pnl_canvas_text(canvas, x + width, 0, focused ? PNL_BOLD : PNL_FG(PNL_ROLE_MUTED), number);
}

static void pnl_slider_move(struct pnl_slider_widget *slider, double value)
{
    value = pnl_slider_fit(slider, value);
    if (value == slider->value)
        return;
    slider->value = value;
    pnl_glide_to(slider->app, &slider->glide, pnl_slider_share(slider, value), PNL_MOTION_COUNT);
    pnl_widget_changed(slider->self);
}

static int pnl_slider_click(struct pnl_slider_widget *slider, pnl_app_t *app)
{
    int x = pnl_form_mouse_x(app) - slider->bar_x;

    if (x < 0 || x >= slider->bar_width)
        return 1;
    pnl_slider_move(slider, slider->min + (slider->max - slider->min) * (double)(x + 1) / (double)slider->bar_width);
    return 1;
}

static int pnl_slider_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_slider_widget *slider = ctx;
    double step = slider->step > 0.0 ? slider->step : (slider->max - slider->min) / 20.0;

    if (key == PNL_KEY_CLICK)
        return pnl_slider_click(slider, app);
    if (key == PNL_KEY_LEFT)
        pnl_slider_move(slider, slider->value - step);
    else if (key == PNL_KEY_RIGHT)
        pnl_slider_move(slider, slider->value + step);
    else if (key == PNL_KEY_HOME)
        pnl_slider_move(slider, slider->min);
    else if (key == PNL_KEY_END)
        pnl_slider_move(slider, slider->max);
    else
        return 0;
    return 1;
}

static const char *pnl_slider_hints(void *ctx)
{
    (void)ctx;
    return "left/right: adjust";
}

static void pnl_slider_release(void *ctx)
{
    struct pnl_slider_widget *slider = ctx;

    pnl_form_release(slider->app, slider->label, slider);
}

static int pnl_slider_set_label(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_slider_widget *)widget->ctx)->label, text);
}

static const char *pnl_slider_label(const pnl_widget_t *widget)
{
    return ((const struct pnl_slider_widget *)widget->ctx)->label;
}

static int pnl_slider_set(pnl_widget_t *widget, double value)
{
    struct pnl_slider_widget *slider = widget->ctx;

    slider->value = pnl_slider_fit(slider, value);
    pnl_glide_to(slider->app, &slider->glide, pnl_slider_share(slider, slider->value), PNL_MOTION_COUNT);
    widget->app->dirty = 1;
    return PNL_OK;
}

static int pnl_slider_get(const pnl_widget_t *widget, double *out_value)
{
    *out_value = ((const struct pnl_slider_widget *)widget->ctx)->value;
    return PNL_OK;
}

static int pnl_slider_range(pnl_widget_t *widget, double min, double max)
{
    struct pnl_slider_widget *slider = widget->ctx;

    if (!(max > min))
        return PNL_ERR_ARG;
    slider->min = min;
    slider->max = max;
    return pnl_slider_set(widget, slider->value);
}

int pnl_widget_slider(pnl_app_t *app, const char *label, double min, double max, double step,
                      pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t slider_class = {pnl_slider_draw, pnl_slider_key, pnl_field_line, pnl_slider_hints,
                                                    pnl_slider_release};
    static const struct pnl_widget_ops slider_ops = {pnl_slider_set_label, pnl_slider_label, pnl_slider_set, pnl_slider_get,
                                                     pnl_slider_range};
    struct pnl_slider_widget *slider;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL || !(max > min) || !(step >= 0.0))
        return PNL_ERR_ARG;
    slider = pnl_form_alloc(app, sizeof(*slider));
    if (slider != NULL)
        slider->app = app;
    if (slider == NULL || pnl_widget_copy(app, &slider->label, label) != PNL_OK) {
        if (slider != NULL)
            pnl_slider_release(slider);
        return PNL_ERR_NOMEM;
    }
    slider->app = app;
    slider->min = min;
    slider->max = max;
    slider->step = step;
    slider->value = min;
    return pnl_form_finish(app, PNL_WIDGET_SLIDER, &slider_class, &slider_ops, slider, &slider->self, out_widget);
}

static void pnl_button_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_button_widget *button = ctx;
    int focused = pnl_canvas_focused(canvas);
    int style = focused ? PNL_BOLD | PNL_REVERSE | PNL_FG(PNL_ROLE_ACCENT) : PNL_BG(PNL_ROLE_SURFACE);
    char text[PNL_CELL_MAX];

    if (button->pressed_ms > 0 && pnl_motion_on(app, PNL_MOTION_FEEDBACK) &&
        pnl_app_tween(app, button->pressed_ms, PNL_PRESS_MS) < 1.0)
        style = PNL_BOLD | PNL_REVERSE | PNL_FG(PNL_ROLE_OK);
    snprintf(text, sizeof(text), " %s ", button->label != NULL ? button->label : "");
    pnl_canvas_text(canvas, 0, 0, style, text);
}

static int pnl_button_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_button_widget *button = ctx;

    if (key != PNL_KEY_ENTER && key != ' ' && key != PNL_KEY_CLICK)
        return 0;
    button->pressed_ms = pnl_term_now_ms();
    app->dirty = 1;
    if (button->action_fn != NULL)
        button->action_fn(button->action_ctx, app);
    return 1;
}

static const char *pnl_button_hints(void *ctx)
{
    (void)ctx;
    return "enter: press";
}

static void pnl_button_release(void *ctx)
{
    struct pnl_button_widget *button = ctx;

    pnl_form_release(button->app, button->label, button);
}

int pnl_widget_button(pnl_app_t *app, const char *label, pnl_action_fn action_fn, void *action_ctx,
                      pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t button_class = {pnl_button_draw, pnl_button_key, pnl_field_line, pnl_button_hints,
                                                    pnl_button_release};
    static const struct pnl_widget_ops button_ops = {pnl_button_set_label, pnl_button_label, NULL, NULL, NULL};
    struct pnl_button_widget *button;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL || label == NULL)
        return PNL_ERR_ARG;
    button = pnl_form_alloc(app, sizeof(*button));
    if (button != NULL)
        button->app = app;
    if (button == NULL || pnl_widget_copy(app, &button->label, label) != PNL_OK) {
        if (button != NULL)
            pnl_button_release(button);
        return PNL_ERR_NOMEM;
    }
    button->app = app;
    button->action_fn = action_fn;
    button->action_ctx = action_ctx;
    return pnl_form_finish(app, PNL_WIDGET_BUTTON, &button_class, &button_ops, button, &button->self, out_widget);
}


#include <stdio.h>
#include <string.h>

#define PNL_HELP_MAX 4096
#define PNL_HELP_KEY_WIDTH 14

static const char pnl_menu_hints[] = "up/down: move  enter: run the selected action  1-9: run that action  "
                                     "letter: jump to the next action starting with it  q: quit";
static const char pnl_global_hints[] = "?: this help  :: run any action by name  ctrl-c: quit  "
                                       "ctrl-z: suspend  f12: inspector";

struct pnl_text {
    char data[PNL_HELP_MAX];
    size_t len;
};

static void pnl_text_add(struct pnl_text *text, const char *data, size_t len)
{
    if (len > sizeof(text->data) - 1 - text->len)
        len = sizeof(text->data) - 1 - text->len;
    memcpy(text->data + text->len, data, len);
    text->len += len;
    text->data[text->len] = '\0';
}

static void pnl_text_pad(struct pnl_text *text, size_t width)
{
    while (width-- > 0)
        pnl_text_add(text, " ", 1);
}

static void pnl_help_hint(struct pnl_text *text, const char *hint, size_t len)
{
    size_t key = 1;

    while (key + 1 < len && !(hint[key] == ':' && hint[key + 1] == ' '))
        key++;
    if (key + 1 >= len)
        return;
    pnl_text_add(text, "  ", 2);
    pnl_text_add(text, hint, key);
    pnl_text_pad(text, key < PNL_HELP_KEY_WIDTH ? PNL_HELP_KEY_WIDTH - key : 1);
    pnl_text_add(text, hint + key + 2, len - key - 2);
    pnl_text_add(text, "\n", 1);
}

static void pnl_help_section(struct pnl_text *text, const char *title, const char *hints)
{
    const char *pos = hints;

    pnl_text_add(text, title, strlen(title));
    pnl_text_add(text, "\n\n", 2);
    while (*pos != '\0') {
        const char *end = (strstr)(pos, "  ");
        size_t len = end ? (size_t)(end - pos) : strlen(pos);

        if (len > 0)
            pnl_help_hint(text, pos, len);
        pos += len;
        while (*pos == ' ')
            pos++;
    }
    pnl_text_add(text, "\n", 1);
}

static void pnl_help_bindings(const pnl_app_t *app, struct pnl_text *text)
{
    char from[16];
    char to[16];
    char line[48];
    size_t i;

    if (app->bindings_len == 0)
        return;
    pnl_text_add(text, "your keys\n\n", 11);
    for (i = 0; i < app->bindings_len; i++) {
        pnl_key_label(app->bindings[i].from, from, sizeof(from));
        pnl_key_label(app->bindings[i].to, to, sizeof(to));
        snprintf(line, sizeof(line), "  %-*s%s\n", PNL_HELP_KEY_WIDTH, from, to);
        pnl_text_add(text, line, strlen(line));
    }
}

void pnl_help(pnl_app_t *app)
{
    struct pnl_text text;
    const char *hints = pnl_view_hints(app);

    text.len = 0;
    text.data[0] = '\0';
    pnl_help_section(&text, "this screen", hints ? hints : pnl_menu_hints);
    pnl_help_section(&text, "everywhere", pnl_global_hints);
    pnl_help_bindings(app, &text);
    pnl_pager(app, "help", text.data, text.len);
}

void pnl_palette(pnl_app_t *app)
{
    const char **labels;
    size_t index = 0;
    size_t i;
    int ok = 0;

    if (app->actions_len == 0)
        return;
    labels = pnl_mem_alloc(&app->mem, app->actions_len * sizeof(*labels));
    if (labels == NULL)
        return;
    for (i = 0; i < app->actions_len; i++)
        labels[i] = app->actions[i].label;
    if (pnl_pick(app, "run", labels, app->actions_len, &index, &ok) == PNL_OK && ok && !app->quit)
        app->actions[index].fn(app->actions[index].ctx, app);
    pnl_mem_free(&app->mem, labels);
}

#include <stdio.h>
#include <string.h>

#define PNL_INSPECT_FRAMES 60
#define PNL_INSPECT_EVENTS 16
#define PNL_INSPECT_REFRESH_MS 250
#define PNL_INSPECT_FOOTER "q: back  f12: close"

struct pnl_event {
    long long ms;
    int kind;
    int arg;
};

struct pnl_inspect {
    double frame_ms[PNL_INSPECT_FRAMES];
    size_t frame_bytes[PNL_INSPECT_FRAMES];
    unsigned long frames;
    struct pnl_event events[PNL_INSPECT_EVENTS];
    unsigned long events_len;
    int timer;
    int open;
};

static const char *const pnl_event_names[] = {"key", "wake", "timer", "watch", "resize"};

void pnl_inspect_start(pnl_app_t *app)
{
    if (app->inspect != NULL || pnl_env_off("TUIKIT_INSPECT"))
        return;
    app->inspect = pnl_mem_alloc(&app->mem, sizeof(*app->inspect));
    if (app->inspect != NULL)
        memset(app->inspect, 0, sizeof(*app->inspect));
}

void pnl_inspect_free(pnl_app_t *app)
{
    pnl_mem_free(&app->mem, app->inspect);
    app->inspect = NULL;
}

void pnl_inspect_frame(pnl_app_t *app, long long took_us, size_t bytes)
{
    struct pnl_inspect *inspect = app->inspect;

    if (inspect == NULL)
        return;
    inspect->frame_ms[inspect->frames % PNL_INSPECT_FRAMES] = (double)took_us / 1000.0;
    inspect->frame_bytes[inspect->frames % PNL_INSPECT_FRAMES] = bytes;
    inspect->frames++;
}

void pnl_inspect_event(pnl_app_t *app, int kind, int arg)
{
    struct pnl_inspect *inspect = app->inspect;
    struct pnl_event *event;

    if (inspect == NULL || (kind == PNL_EVENT_TIMER && arg == inspect->timer && inspect->open))
        return;
    event = &inspect->events[inspect->events_len++ % PNL_INSPECT_EVENTS];
    event->ms = pnl_term_now_ms();
    event->kind = kind;
    event->arg = arg;
}

static void pnl_inspect_line(pnl_canvas_t *canvas, int y, const char *label, const char *text)
{
    pnl_canvas_text(canvas, 1, y, PNL_FG(PNL_ROLE_MUTED), label);
    pnl_canvas_text(canvas, 13, y, 0, text);
}

static void pnl_inspect_frames(const struct pnl_inspect *inspect, pnl_canvas_t *canvas, int y)
{
    size_t count = inspect->frames < PNL_INSPECT_FRAMES ? (size_t)inspect->frames : PNL_INSPECT_FRAMES;
    double series[PNL_INSPECT_FRAMES];
    double total = 0.0;
    double worst = 0.0;
    double last = 0.0;
    size_t bytes = 0;
    char text[160];
    size_t i;

    for (i = 0; i < count; i++) {
        size_t slot = (size_t)((inspect->frames - count + i) % PNL_INSPECT_FRAMES);

        series[i] = inspect->frame_ms[slot];
        total += series[i];
        worst = series[i] > worst ? series[i] : worst;
        last = series[i];
        bytes = inspect->frame_bytes[slot];
    }
    snprintf(text, sizeof(text), "%lu drawn  last %.2f ms  average %.2f ms  worst %.2f ms  %zu bytes",
             inspect->frames, last, count > 0 ? total / (double)count : 0.0, worst, bytes);
    pnl_inspect_line(canvas, y, "frames", text);
    pnl_canvas_sparkline(canvas, 13, y + 1, pnl_canvas_width(canvas) - 14, series, count, worst > 0.0 ? worst : 1.0,
                         0);
}

static void pnl_inspect_views(const pnl_app_t *app, pnl_canvas_t *canvas, int y)
{
    char text[256];
    size_t len = (size_t)snprintf(text, sizeof(text), "menu");
    size_t i;

    for (i = 0; i + 1 < app->views_len && len < sizeof(text); i++) {
        const struct pnl_view *view = &app->views[i];

        len += (size_t)snprintf(text + len, sizeof(text) - len, " > %s%s", view->title ? view->title : "view",
                                view->dialog_width > 0 ? " (dialog)" : "");
    }
    pnl_inspect_line(canvas, y, "screens", text);
}

static void pnl_inspect_parts(const pnl_app_t *app, pnl_canvas_t *canvas, int y)
{
    char text[256];

    snprintf(text, sizeof(text),
             "%zu actions  %zu status lines  %zu values  %zu timers  %zu watches  %d tasks  %zu toasts  %zu keys",
             app->actions_len, app->lines_len, app->values_len, app->timers_len - 1, app->watches_len,
             app->tasks_running, app->toasts_len, app->bindings_len);
    pnl_inspect_line(canvas, y, "parts", text);
}

static void pnl_inspect_motion(const pnl_app_t *app, pnl_canvas_t *canvas, int y)
{
    long long left = app->animate_until_ms - pnl_term_now_ms();
    char text[160];

    snprintf(text, sizeof(text), "%s%s  spinner %s  %s", app->motion ? "on" : "off",
             app->motion_flags == PNL_MOTION_ALL ? " (all)" : "", app->screen.spinning ? "turning" : "still",
             left > 0 ? "animating" : "idle");
    pnl_inspect_line(canvas, y, "motion", text);
}

static void pnl_inspect_terminal(const pnl_app_t *app, pnl_canvas_t *canvas, int y)
{
    static const char *const depths[] = {"no color", "16 colors", "256 colors", "true color"};
    char reply[32] = "no reply";
    char text[200];

    if (app->reply_ms >= 0)
        snprintf(reply, sizeof(reply), "reply in %ld ms", app->reply_ms);
    snprintf(text, sizeof(text), "%dx%d  %s  %s background  mouse %s  %s  %d links  %s",
             app->screen.width, app->screen.height, depths[app->screen.palette.depth & 3],
             app->screen.palette.light ? "light" : "dark", app->term.mouse ? "on" : "off",
             !app->term.focus_known ? "focus unknown" : app->term.focused ? "focused" : "unfocused",
             app->screen.links_len, reply);
    pnl_inspect_line(canvas, y, "terminal", text);
}

static void pnl_inspect_event_text(const struct pnl_event *event, char *out, size_t out_len)
{
    char key[32];

    if (event->kind != PNL_EVENT_KEY) {
        snprintf(out, out_len, "%s %d", pnl_event_names[event->kind], event->arg);
        return;
    }
    if (event->arg == PNL_KEY_CLICK || event->arg == PNL_KEY_WHEEL_UP || event->arg == PNL_KEY_WHEEL_DOWN)
        snprintf(key, sizeof(key), "mouse");
    else
        pnl_key_label(event->arg, key, sizeof(key));
    snprintf(out, out_len, "key %s", key);
}

static void pnl_inspect_events(const struct pnl_inspect *inspect, pnl_canvas_t *canvas, int y)
{
    long long now = pnl_term_now_ms();
    int rows = pnl_canvas_height(canvas) - y - 1;
    unsigned long i;

    pnl_canvas_text(canvas, 1, y, PNL_FG(PNL_ROLE_MUTED), "events");
    for (i = 0; i < inspect->events_len && i < PNL_INSPECT_EVENTS && (int)i < rows; i++) {
        const struct pnl_event *event = &inspect->events[(inspect->events_len - 1 - i) % PNL_INSPECT_EVENTS];
        char when[32];
        char text[64];

        snprintf(when, sizeof(when), "%7.2f s ago", (double)(now - event->ms) / 1000.0);
        pnl_inspect_event_text(event, text, sizeof(text));
        pnl_canvas_text(canvas, 13, y + (int)i, PNL_FG(PNL_ROLE_MUTED), when);
        pnl_canvas_text(canvas, 27, y + (int)i, 0, text);
    }
}

static void pnl_inspect_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    const struct pnl_inspect *inspect = ctx;

    pnl_inspect_frames(inspect, canvas, 0);
    pnl_inspect_views(app, canvas, 3);
    pnl_inspect_parts(app, canvas, 4);
    pnl_inspect_motion(app, canvas, 5);
    pnl_inspect_terminal(app, canvas, 6);
    pnl_inspect_events(inspect, canvas, 8);
}

static void pnl_inspect_tick(void *ctx, pnl_app_t *app)
{
    (void)ctx;
    (void)app;
}

static void pnl_inspect_drop(pnl_app_t *app, void *ctx)
{
    struct pnl_inspect *inspect = ctx;

    pnl_app_remove_timer(app, inspect->timer);
    inspect->timer = 0;
    inspect->open = 0;
}

void pnl_inspect_toggle(pnl_app_t *app)
{
    struct pnl_inspect *inspect = app->inspect;
    struct pnl_view view;

    if (inspect == NULL)
        return;
    if (inspect->open) {
        if (app->views[app->views_len - 1].ctx == inspect)
            pnl_view_pop(app);
        return;
    }
    if (pnl_app_add_timer(app, PNL_INSPECT_REFRESH_MS, pnl_inspect_tick, NULL, &inspect->timer) != PNL_OK)
        return;
    memset(&view, 0, sizeof(view));
    view.title = pnl_mem_copy(&app->mem, "inspector");
    view.footer = PNL_INSPECT_FOOTER;
    view.draw_fn = pnl_inspect_draw;
    view.drop_fn = pnl_inspect_drop;
    view.ctx = inspect;
    inspect->open = 1;
    if (view.title == NULL || pnl_view_push_internal(app, &view) != PNL_OK) {
        pnl_mem_free(&app->mem, view.title);
        pnl_inspect_drop(app, inspect);
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_KEYMAP_LINE 128
#define PNL_KEYMAP_PATH 1024

struct pnl_key_name {
    const char *name;
    int key;
};

static const struct pnl_key_name pnl_key_names[] = {
    {"up", PNL_KEY_UP},           {"down", PNL_KEY_DOWN},         {"left", PNL_KEY_LEFT},
    {"right", PNL_KEY_RIGHT},     {"enter", PNL_KEY_ENTER},       {"esc", PNL_KEY_ESCAPE},
    {"escape", PNL_KEY_ESCAPE},   {"pgup", PNL_KEY_PAGE_UP},      {"pageup", PNL_KEY_PAGE_UP},
    {"pgdn", PNL_KEY_PAGE_DOWN},  {"pagedown", PNL_KEY_PAGE_DOWN}, {"home", PNL_KEY_HOME},
    {"end", PNL_KEY_END},         {"backspace", PNL_KEY_BACKSPACE}, {"delete", PNL_KEY_DELETE},
    {"tab", PNL_KEY_TAB},         {"backtab", PNL_KEY_BACKTAB},   {"space", ' '},
    {"f12", PNL_KEY_INSPECT},     {"none", PNL_KEY_NONE},
};

int pnl_key_parse(const char *name, int *out_key)
{
    size_t i;

    for (i = 0; i < sizeof(pnl_key_names) / sizeof(pnl_key_names[0]); i++) {
        if (strcmp(pnl_key_names[i].name, name) == 0) {
            *out_key = pnl_key_names[i].key;
            return 1;
        }
    }
    if (strncmp(name, "ctrl-", 5) == 0 && name[5] >= 'a' && name[5] <= 'z' && name[6] == '\0') {
        *out_key = name[5] - 'a' + 1;
        return 1;
    }
    if ((unsigned char)name[0] > 0x20 && (unsigned char)name[0] < 0x7f && name[1] == '\0') {
        *out_key = (unsigned char)name[0];
        return 1;
    }
    return 0;
}

void pnl_key_label(int key, char *out, size_t out_len)
{
    size_t i;

    for (i = 0; i < sizeof(pnl_key_names) / sizeof(pnl_key_names[0]); i++) {
        if (pnl_key_names[i].key == key) {
            snprintf(out, out_len, "%s", pnl_key_names[i].name);
            return;
        }
    }
    if (key >= 1 && key <= 26)
        snprintf(out, out_len, "ctrl-%c", 'a' + key - 1);
    else if (key >= 0x110000)
        snprintf(out, out_len, "0x%x", (unsigned int)key);
    else if (key >= 0x80 && out_len > 4)
        out[pnl_utf8_put((uint32_t)key, out)] = '\0';
    else if (key >= 0x80)
        snprintf(out, out_len, "?");
    else
        snprintf(out, out_len, "%c", key);
}

int pnl_app_bind(pnl_app_t *app, const char *key, const char *to)
{
    int from_key;
    int to_key;
    size_t i;

    if (app == NULL || key == NULL || to == NULL || !pnl_key_parse(key, &from_key) || !pnl_key_parse(to, &to_key) ||
        from_key == PNL_KEY_NONE || from_key == PNL_KEY_CTRL_C)
        return PNL_ERR_ARG;
    for (i = 0; i < app->bindings_len && app->bindings[i].from != from_key; i++)
        ;
    if (i == PNL_BINDINGS)
        return PNL_ERR_LIMIT;
    app->bindings[i].from = from_key;
    app->bindings[i].to = to_key;
    if (i == app->bindings_len)
        app->bindings_len++;
    return PNL_OK;
}

static int pnl_keymap_typing(const pnl_app_t *app)
{
    const struct pnl_view *top = app->views_len > 0 ? &app->views[app->views_len - 1] : NULL;

    return app->widget_typing || (top != NULL && top->typing_ref != NULL && *top->typing_ref);
}

int pnl_keymap_apply(const pnl_app_t *app, int key)
{
    size_t i;

    if (key >= 0x20 && key < 0x110000 && pnl_keymap_typing(app))
        return key;
    for (i = 0; i < app->bindings_len; i++) {
        if (app->bindings[i].from == key)
            return app->bindings[i].to;
    }
    return key;
}

static char *pnl_keymap_trim(char *text)
{
    char *end;

    while (*text == ' ' || *text == '\t')
        text++;
    end = text + strlen(text);
    while (end > text && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n'))
        *--end = '\0';
    return text;
}

void pnl_keymap_line(pnl_app_t *app, char *line)
{
    char *equals = (strchr)(line, '=');

    line = pnl_keymap_trim(line);
    if (line[0] == '#' || equals == NULL || equals == line)
        return;
    *equals = '\0';
    pnl_app_bind(app, pnl_keymap_trim(line), pnl_keymap_trim(equals + 1));
}

static void pnl_keymap_file(pnl_app_t *app, const char *path)
{
    char line[PNL_KEYMAP_LINE];
    FILE *file = fopen(path, "r");

    if (file == NULL)
        return;
    while (fgets(line, sizeof(line), file) != NULL)
        pnl_keymap_line(app, line);
    fclose(file);
}

static int pnl_keymap_dir(char *out, size_t out_len)
{
#if defined(_WIN32)
    const char *base = getenv("APPDATA");

    if (base == NULL || base[0] == '\0')
        return 0;
    snprintf(out, out_len, "%s", base);
#else
    const char *base = getenv("XDG_CONFIG_HOME");
    const char *home = getenv("HOME");

    if (base != NULL && base[0] == '/')
        snprintf(out, out_len, "%s", base);
    else if (home != NULL && home[0] != '\0')
        snprintf(out, out_len, "%s/.config", home);
    else
        return 0;
#endif
    return 1;
}

void pnl_keymap_load(pnl_app_t *app)
{
    char dir[PNL_KEYMAP_PATH];
    char name[128];
    char path[PNL_KEYMAP_PATH + 160];

    if (!pnl_keymap_dir(dir, sizeof(dir)))
        return;
    pnl_tool_name(name, sizeof(name), app->title);
    snprintf(path, sizeof(path), "%s/tuikit/keys", dir);
    pnl_keymap_file(app, path);
    snprintf(path, sizeof(path), "%s/%s/keys", dir, name);
    pnl_keymap_file(app, path);
}

static int pnl_keys_letter(unsigned char c)
{
    static const int arrows[] = {PNL_KEY_UP, PNL_KEY_DOWN, PNL_KEY_RIGHT, PNL_KEY_LEFT};

    if (c >= 'A' && c <= 'D')
        return arrows[c - 'A'];
    if (c == 'H')
        return PNL_KEY_HOME;
    if (c == 'F')
        return PNL_KEY_END;
    return PNL_KEY_NONE;
}

static int pnl_keys_tilde(const unsigned char *data, size_t end)
{
    static const int codes[] = {PNL_KEY_NONE, PNL_KEY_HOME, PNL_KEY_NONE, PNL_KEY_DELETE, PNL_KEY_END,
                                PNL_KEY_PAGE_UP, PNL_KEY_PAGE_DOWN, PNL_KEY_HOME, PNL_KEY_END};

    if (end == 4 && data[2] == '2' && data[3] == '4')
        return PNL_KEY_INSPECT;
    if (end != 3 || data[2] < '1' || data[2] > '8')
        return PNL_KEY_NONE;
    return codes[data[2] - '0'];
}

static size_t pnl_keys_number(const unsigned char *data, size_t end, size_t at, int *out_value)
{
    int value = 0;

    while (at < end && data[at] >= '0' && data[at] <= '9') {
        if (value < 100000)
            value = value * 10 + (data[at] - '0');
        at++;
    }
    *out_value = value;
    return at;
}

static void pnl_keys_mouse(const unsigned char *data, size_t end, int *out_key, int *out_x, int *out_y)
{
    int button;
    int x;
    int y;
    size_t at = pnl_keys_number(data, end, 3, &button);

    if (at >= end || data[at] != ';')
        return;
    at = pnl_keys_number(data, end, at + 1, &x);
    if (at >= end || data[at] != ';')
        return;
    at = pnl_keys_number(data, end, at + 1, &y);
    if (at != end || x < 1 || y < 1)
        return;
    if (button == 64 || button == 65)
        *out_key = button == 64 ? PNL_KEY_WHEEL_UP : PNL_KEY_WHEEL_DOWN;
    else if (button == 0 && data[end] == 'M')
        *out_key = PNL_KEY_CLICK;
    else
        return;
    *out_x = x - 1;
    *out_y = y - 1;
}

static size_t pnl_keys_csi(const unsigned char *data, size_t data_len, int *out_key, int *out_x, int *out_y)
{
    size_t i;

    for (i = 2; i < data_len; i++) {
        if (data[i] < 0x40 || data[i] > 0x7e)
            continue;
        if (data[2] == '<' && (data[i] == 'M' || data[i] == 'm'))
            pnl_keys_mouse(data, i, out_key, out_x, out_y);
        else if (i == 2 && data[i] == 'Z')
            *out_key = PNL_KEY_BACKTAB;
        else if (i == 2 && (data[i] == 'I' || data[i] == 'O'))
            *out_key = data[i] == 'I' ? PNL_KEY_FOCUS_IN : PNL_KEY_FOCUS_OUT;
        else if (i == 2)
            *out_key = pnl_keys_letter(data[i]);
        else if (data[i] == '~')
            *out_key = pnl_keys_tilde(data, i);
        return i + 1;
    }
    return data_len;
}

static size_t pnl_keys_osc(const unsigned char *data, size_t data_len)
{
    size_t i;

    for (i = 2; i < data_len; i++) {
        if (data[i] == 0x07)
            return i + 1;
        if (data[i] == 0x1b && i + 1 < data_len && data[i + 1] == '\\')
            return i + 2;
    }
    return data_len;
}

static size_t pnl_keys_escape(const unsigned char *data, size_t data_len, int *out_key, int *out_x, int *out_y)
{
    if (data_len > 1 && data[1] == ']')
        return pnl_keys_osc(data, data_len);
    if (data_len == 1 || (data[1] != '[' && data[1] != 'O')) {
        *out_key = PNL_KEY_ESCAPE;
        return 1;
    }
    if (data[1] == '[')
        return pnl_keys_csi(data, data_len, out_key, out_x, out_y);
    if (data_len < 3)
        return data_len;
    *out_key = pnl_keys_letter(data[2]);
    return 3;
}

static size_t pnl_keys_utf8(const unsigned char *data, size_t data_len, int *out_key)
{
    size_t need = data[0] >= 0xf0 ? 4 : data[0] >= 0xe0 ? 3 : 2;
    int code = data[0] & (0x3f >> (need - 1));
    size_t i;

    if (data[0] < 0xc2 || data[0] > 0xf4)
        return 1;
    for (i = 1; i < need && i < data_len; i++) {
        if ((data[i] & 0xc0) != 0x80)
            return i;
        code = (code << 6) | (data[i] & 0x3f);
    }
    if (data_len < need)
        return data_len;
    if (code < 0x110000 && (code < 0xd800 || code > 0xdfff) && code >= (need == 3 ? 0x800 : need == 4 ? 0x10000 : 0))
        *out_key = code;
    return need;
}

static int pnl_keys_partial_escape(const unsigned char *data, size_t data_len)
{
    size_t i;

    if (data_len == 1)
        return 1;
    if (data[1] == 'O')
        return data_len < 3;
    if (data[1] == ']') {
        for (i = 2; i < data_len; i++) {
            if (data[i] == 0x07 || (data[i] == 0x1b && i + 1 < data_len && data[i + 1] == '\\'))
                return 0;
        }
        return 1;
    }
    if (data[1] != '[')
        return 0;
    for (i = 2; i < data_len; i++) {
        if (data[i] >= 0x40 && data[i] <= 0x7e)
            return 0;
    }
    return 1;
}

int pnl_key_partial(const unsigned char *data, size_t data_len)
{
    size_t need;
    size_t i;

    if (data_len == 0)
        return 0;
    if (data[0] == 0x1b)
        return pnl_keys_partial_escape(data, data_len);
    if (data[0] < 0xc2 || data[0] > 0xf4)
        return 0;
    need = data[0] >= 0xf0 ? 4 : data[0] >= 0xe0 ? 3 : 2;
    for (i = 1; i < data_len && i < need; i++) {
        if ((data[i] & 0xc0) != 0x80)
            return 0;
    }
    return data_len < need;
}

int pnl_term_complete(struct pnl_term *term)
{
    while (term->input_len < PNL_INPUT_MAX && pnl_key_partial(term->input, term->input_len)) {
        int filled = pnl_term_fill(term, PNL_ESCAPE_WAIT_MS);

        if (filled <= 0)
            return filled;
    }
    return 1;
}

size_t pnl_key_decode(const unsigned char *data, size_t data_len, int *out_key)
{
    int x;
    int y;

    return pnl_key_decode_at(data, data_len, out_key, &x, &y);
}

size_t pnl_key_decode_at(const unsigned char *data, size_t data_len, int *out_key, int *out_x, int *out_y)
{
    if (out_key == NULL)
        return 0;
    *out_key = PNL_KEY_NONE;
    if (data == NULL || data_len == 0)
        return 0;
    if (data[0] == 0x1b)
        return pnl_keys_escape(data, data_len, out_key, out_x, out_y);
    if (data[0] == '\r' || data[0] == '\n') {
        *out_key = PNL_KEY_ENTER;
        return 1;
    }
    if (data[0] == '\t') {
        *out_key = PNL_KEY_TAB;
        return 1;
    }
    if (data[0] == 0x7f || data[0] == 0x08) {
        *out_key = PNL_KEY_BACKSPACE;
        return 1;
    }
    if (data[0] < 0x80) {
        *out_key = data[0];
        return 1;
    }
    return pnl_keys_utf8(data, data_len, out_key);
}

#include <stdio.h>
#include <string.h>

#define PNL_NODE_PANEL -1
#define PNL_LAYOUT_MIN_COLUMNS 16
#define PNL_LAYOUT_MIN_ROWS 3
#define PNL_LAYOUT_STACK_ROWS 5
#define PNL_LAYOUT_FOOTER_MAX 256
#define PNL_FOCUS_MARK "\xe2\x96\x8e"

struct pnl_node {
    int kind;
    int parent;
    char *title;
    char *hints;
    pnl_draw_fn draw_fn;
    pnl_key_fn key_fn;
    void *ctx;
    pnl_widget_t *widget;
    int size;
    int min_size;
    int active;
};

struct pnl_rect {
    int x;
    int y;
    int width;
    int height;
};

struct pnl_hit {
    int node;
    int tabs;
    int x;
    int y;
    int width;
    int height;
    int origin_x;
    int origin_y;
};

struct pnl_layout {
    pnl_app_t *app;
    struct pnl_node *nodes;
    size_t nodes_len;
    size_t nodes_cap;
    int *visible;
    size_t visible_len;
    size_t visible_cap;
    struct pnl_hit *hits;
    size_t hits_len;
    size_t hits_cap;
    int focus;
    int has_tabs;
    char footer_text[PNL_LAYOUT_FOOTER_MAX];
    const char *footer;
};

static int pnl_layout_valid(const pnl_layout_t *layout, int node)
{
    return layout != NULL && node >= 0 && (size_t)node < layout->nodes_len;
}

static int pnl_layout_add(pnl_layout_t *layout, int parent, int kind, const char *title, int *out_node)
{
    struct pnl_node *node;
    int status;

    status = pnl_mem_grow(&layout->app->mem, (void **)&layout->nodes, &layout->nodes_cap, layout->nodes_len + 1,
                          sizeof(*node));
    if (status != PNL_OK)
        return status;
    node = &layout->nodes[layout->nodes_len];
    memset(node, 0, sizeof(*node));
    node->kind = kind;
    node->parent = parent;
    node->active = -1;
    if (title != NULL && (node->title = pnl_mem_copy(&layout->app->mem, title)) == NULL)
        return PNL_ERR_NOMEM;
    if (out_node != NULL)
        *out_node = (int)layout->nodes_len;
    layout->nodes_len++;
    return PNL_OK;
}

static int pnl_layout_is_split(int kind)
{
    return kind == PNL_SPLIT_ROWS || kind == PNL_SPLIT_COLUMNS || kind == PNL_SPLIT_TABS;
}

int pnl_layout_create(pnl_app_t *app, int kind, pnl_layout_t **out_layout)
{
    pnl_layout_t *layout;
    int status;

    if (out_layout == NULL)
        return PNL_ERR_ARG;
    *out_layout = NULL;
    if (app == NULL || !pnl_layout_is_split(kind))
        return PNL_ERR_ARG;
    layout = pnl_mem_alloc(&app->mem, sizeof(*layout));
    if (layout == NULL)
        return PNL_ERR_NOMEM;
    memset(layout, 0, sizeof(*layout));
    layout->app = app;
    layout->focus = -1;
    status = pnl_layout_add(layout, -1, kind, NULL, NULL);
    if (status != PNL_OK) {
        pnl_layout_destroy(layout);
        return status;
    }
    *out_layout = layout;
    return PNL_OK;
}

void pnl_layout_destroy(pnl_layout_t *layout)
{
    struct pnl_mem mem;
    size_t i;

    if (layout == NULL)
        return;
    mem = layout->app->mem;
    for (i = 0; i < layout->nodes_len; i++) {
        pnl_mem_free(&mem, layout->nodes[i].title);
        pnl_mem_free(&mem, layout->nodes[i].hints);
    }
    pnl_mem_free(&mem, layout->nodes);
    pnl_mem_free(&mem, layout->visible);
    pnl_mem_free(&mem, layout->hits);
    pnl_mem_free(&mem, layout);
}

int pnl_layout_split(pnl_layout_t *layout, int parent, int kind, const char *title, int *out_node)
{
    if (!pnl_layout_valid(layout, parent) || layout->nodes[parent].kind == PNL_NODE_PANEL ||
        !pnl_layout_is_split(kind))
        return PNL_ERR_ARG;
    return pnl_layout_add(layout, parent, kind, title, out_node);
}

int pnl_layout_panel(pnl_layout_t *layout, int parent, const char *title, pnl_draw_fn draw_fn, pnl_key_fn key_fn,
                     void *panel_ctx, int *out_node)
{
    int node;
    int status;

    if (!pnl_layout_valid(layout, parent) || layout->nodes[parent].kind == PNL_NODE_PANEL || draw_fn == NULL)
        return PNL_ERR_ARG;
    status = pnl_layout_add(layout, parent, PNL_NODE_PANEL, title, &node);
    if (status != PNL_OK)
        return status;
    layout->nodes[node].draw_fn = draw_fn;
    layout->nodes[node].key_fn = key_fn;
    layout->nodes[node].ctx = panel_ctx;
    if (layout->focus < 0)
        layout->focus = node;
    if (out_node != NULL)
        *out_node = node;
    return PNL_OK;
}

static void pnl_layout_widget_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    (void)app;
    pnl_widget_paint(ctx, canvas);
}

static int pnl_layout_widget_key(void *ctx, pnl_app_t *app, int key)
{
    (void)app;
    return pnl_widget_key(ctx, key);
}

int pnl_layout_widget(pnl_layout_t *layout, int parent, const char *title, pnl_widget_t *widget, int *out_node)
{
    int node;
    int status;

    if (layout == NULL || widget == NULL)
        return PNL_ERR_ARG;
    status = pnl_layout_panel(layout, parent, title, pnl_layout_widget_draw, pnl_layout_widget_key, widget, &node);
    if (status != PNL_OK)
        return status;
    layout->nodes[node].widget = widget;
    if (out_node != NULL)
        *out_node = node;
    return PNL_OK;
}

int pnl_layout_size(pnl_layout_t *layout, int node, int size, int min_size)
{
    if (!pnl_layout_valid(layout, node) || node == PNL_LAYOUT_ROOT || size < 0 || min_size < 0)
        return PNL_ERR_ARG;
    layout->nodes[node].size = size;
    layout->nodes[node].min_size = min_size;
    return PNL_OK;
}

int pnl_layout_hints(pnl_layout_t *layout, int node, const char *hints)
{
    char *copy;

    if (!pnl_layout_valid(layout, node) || layout->nodes[node].kind != PNL_NODE_PANEL || hints == NULL)
        return PNL_ERR_ARG;
    copy = pnl_mem_copy(&layout->app->mem, hints);
    if (copy == NULL)
        return PNL_ERR_NOMEM;
    pnl_mem_free(&layout->app->mem, layout->nodes[node].hints);
    layout->nodes[node].hints = copy;
    return PNL_OK;
}

int pnl_layout_focus(pnl_layout_t *layout, int node)
{
    if (!pnl_layout_valid(layout, node) || layout->nodes[node].kind != PNL_NODE_PANEL)
        return PNL_ERR_ARG;
    layout->focus = node;
    return PNL_OK;
}

static int pnl_layout_next_child(const pnl_layout_t *layout, int parent, int after)
{
    size_t i;

    for (i = (size_t)(after + 1); i < layout->nodes_len; i++) {
        if (layout->nodes[i].parent == parent)
            return (int)i;
    }
    return -1;
}

static int pnl_layout_min(const pnl_layout_t *layout, int node, int axis)
{
    const struct pnl_node *n = &layout->nodes[node];

    if (n->min_size > 0)
        return n->min_size;
    if (n->size > 0)
        return n->size;
    return axis == PNL_SPLIT_COLUMNS ? PNL_LAYOUT_MIN_COLUMNS : PNL_LAYOUT_MIN_ROWS;
}

static void pnl_layout_hit_add(pnl_layout_t *layout, const pnl_canvas_t *canvas, int node, int tabs,
                               struct pnl_rect rect, int inner_y)
{
    struct pnl_hit *hit;

    if (pnl_mem_grow(&layout->app->mem, (void **)&layout->hits, &layout->hits_cap, layout->hits_len + 1,
                     sizeof(*layout->hits)) != PNL_OK)
        return;
    hit = &layout->hits[layout->hits_len++];
    hit->node = node;
    hit->tabs = tabs;
    hit->x = canvas->x + rect.x;
    hit->y = canvas->y + rect.y;
    hit->width = rect.width;
    hit->height = rect.height;
    hit->origin_x = hit->x;
    hit->origin_y = canvas->y + inner_y;
}

static int pnl_layout_inside(const pnl_layout_t *layout, int node, int ancestor)
{
    while (node >= 0 && node != ancestor)
        node = layout->nodes[node].parent;
    return node == ancestor;
}

static int pnl_layout_first_panel(const pnl_layout_t *layout, int ancestor)
{
    size_t i;

    for (i = 0; i < layout->nodes_len; i++) {
        if (layout->nodes[i].kind == PNL_NODE_PANEL && pnl_layout_inside(layout, (int)i, ancestor))
            return (int)i;
    }
    return -1;
}

static int pnl_layout_active(pnl_layout_t *layout, int tabs)
{
    struct pnl_node *n = &layout->nodes[tabs];

    if (n->active < 0 || layout->nodes[n->active].parent != tabs)
        n->active = pnl_layout_next_child(layout, tabs, -1);
    return n->active;
}

static void pnl_layout_draw_node(pnl_layout_t *layout, pnl_canvas_t *canvas, int node, struct pnl_rect rect);

static void pnl_layout_draw_panel(pnl_layout_t *layout, pnl_canvas_t *canvas, int node, struct pnl_rect rect)
{
    const struct pnl_node *n = &layout->nodes[node];
    struct pnl_canvas inner;
    struct pnl_rect full = rect;
    int focused = node == layout->focus;

    if (pnl_mem_grow(&layout->app->mem, (void **)&layout->visible, &layout->visible_cap, layout->visible_len + 1,
                     sizeof(*layout->visible)) == PNL_OK)
        layout->visible[layout->visible_len++] = node;
    if (n->title != NULL && rect.height > 0 && layout->nodes[n->parent].kind != PNL_SPLIT_TABS) {
        if (focused)
            pnl_canvas_clip(canvas, rect.x, rect.y, rect.width, PNL_FG(PNL_ROLE_ACCENT), PNL_FOCUS_MARK);
        pnl_canvas_clip(canvas, rect.x + 1, rect.y, rect.width - 1,
                        PNL_BOLD | (focused ? PNL_FG(PNL_ROLE_ACCENT) : PNL_FG(PNL_ROLE_MUTED)), n->title);
        rect.y++;
        rect.height--;
    }
    pnl_layout_hit_add(layout, canvas, node, -1, full, rect.y);
    inner.screen = canvas->screen;
    inner.focused = focused && canvas->focused;
    inner.x = canvas->x + rect.x;
    inner.y = canvas->y + rect.y;
    inner.width = rect.width;
    inner.height = rect.height;
    if (inner.width > 0 && inner.height > 0)
        n->draw_fn(n->ctx, layout->app, &inner);
}

static void pnl_layout_draw_tabs(pnl_layout_t *layout, pnl_canvas_t *canvas, int node, struct pnl_rect rect)
{
    int active = pnl_layout_active(layout, node);
    int child;
    int x = rect.x + 1;

    if (active >= 0 && pnl_layout_inside(layout, layout->focus, active))
        pnl_canvas_clip(canvas, rect.x, rect.y, rect.width, PNL_FG(PNL_ROLE_ACCENT), PNL_FOCUS_MARK);

    for (child = pnl_layout_next_child(layout, node, -1); child >= 0; child = pnl_layout_next_child(layout, node, child)) {
        const char *title = layout->nodes[child].title ? layout->nodes[child].title : "-";
        int style = child == active ? PNL_BOLD | PNL_UNDERLINE | PNL_FG(PNL_ROLE_ACCENT) : PNL_FG(PNL_ROLE_MUTED);
        struct pnl_rect label = rect;

        label.x = x;
        label.height = 1;
        x = pnl_canvas_clip(canvas, x, rect.y, rect.x + rect.width - x, style, title);
        label.width = x - label.x;
        pnl_layout_hit_add(layout, canvas, child, node, label, rect.y);
        x = pnl_canvas_clip(canvas, x, rect.y, rect.x + rect.width - x, 0, "  ");
    }
    if (active < 0 || rect.height < 2)
        return;
    rect.y += 2;
    rect.height -= 2;
    pnl_layout_draw_node(layout, canvas, active, rect);
}

static int pnl_layout_shown(const pnl_layout_t *layout, int node, int axis, int length, int gap)
{
    int used = 0;
    int shown = 0;
    int child;

    for (child = pnl_layout_next_child(layout, node, -1); child >= 0; child = pnl_layout_next_child(layout, node, child)) {
        used += pnl_layout_min(layout, child, axis) + (shown > 0 ? gap : 0);
        if (used > length && shown > 0)
            break;
        shown++;
    }
    return shown;
}

static int pnl_layout_children(const pnl_layout_t *layout, int node)
{
    int count = 0;
    int child;

    for (child = pnl_layout_next_child(layout, node, -1); child >= 0; child = pnl_layout_next_child(layout, node, child))
        count++;
    return count;
}

static int pnl_layout_stacks(const pnl_layout_t *layout, int node, struct pnl_rect rect)
{
    int count = pnl_layout_children(layout, node);

    return layout->nodes[node].kind == PNL_SPLIT_COLUMNS &&
           pnl_layout_shown(layout, node, PNL_SPLIT_COLUMNS, rect.width, 1) < count &&
           rect.height >= count * PNL_LAYOUT_STACK_ROWS;
}

static void pnl_layout_draw_split(pnl_layout_t *layout, pnl_canvas_t *canvas, int node, struct pnl_rect rect)
{
    int stacked = pnl_layout_stacks(layout, node, rect);
    int axis = stacked ? PNL_SPLIT_ROWS : layout->nodes[node].kind;
    int length = axis == PNL_SPLIT_COLUMNS ? rect.width : rect.height;
    int gap = axis == PNL_SPLIT_COLUMNS ? 1 : 0;
    int shown = stacked ? pnl_layout_children(layout, node) : pnl_layout_shown(layout, node, axis, length, gap);
    int flexible = 0;
    int left = length - gap * (shown > 0 ? shown - 1 : 0);
    int child = -1;
    int pos = 0;
    int i;

    for (i = 0, child = pnl_layout_next_child(layout, node, -1); i < shown; i++, child = pnl_layout_next_child(layout, node, child)) {
        if (layout->nodes[child].size > 0 && !stacked)
            left -= layout->nodes[child].size;
        else
            flexible++;
    }
    for (i = 0, child = pnl_layout_next_child(layout, node, -1); i < shown; i++, child = pnl_layout_next_child(layout, node, child)) {
        int size = stacked ? 0 : layout->nodes[child].size;
        struct pnl_rect part = rect;

        if (size == 0) {
            size = left > 0 ? left / flexible : 0;
            left -= size;
            flexible--;
        }
        if (size > length - pos)
            size = length - pos;
        if (axis == PNL_SPLIT_COLUMNS) {
            part.x = rect.x + pos;
            part.width = size;
            for (size = 0; i > 0 && size < rect.height; size++)
                pnl_canvas_text(canvas, part.x - 1, rect.y + size, PNL_FG(PNL_ROLE_MUTED), "\xe2\x94\x82");
            pos += part.width + gap;
        } else {
            part.y = rect.y + pos;
            part.height = size;
            pos += part.height + gap;
        }
        pnl_layout_draw_node(layout, canvas, child, part);
    }
}

static void pnl_layout_draw_node(pnl_layout_t *layout, pnl_canvas_t *canvas, int node, struct pnl_rect rect)
{
    int kind = layout->nodes[node].kind;

    if (rect.width <= 0 || rect.height <= 0)
        return;
    if (kind == PNL_NODE_PANEL)
        pnl_layout_draw_panel(layout, canvas, node, rect);
    else if (kind == PNL_SPLIT_TABS)
        pnl_layout_draw_tabs(layout, canvas, node, rect);
    else
        pnl_layout_draw_split(layout, canvas, node, rect);
}

static int pnl_layout_is_visible(const pnl_layout_t *layout, int node)
{
    size_t i;

    for (i = 0; i < layout->visible_len; i++) {
        if (layout->visible[i] == node)
            return 1;
    }
    return 0;
}

static void pnl_layout_footer(pnl_layout_t *layout)
{
    const char *hints = layout->focus >= 0 ? layout->nodes[layout->focus].hints : NULL;
    int fields = layout->focus >= 0 && pnl_widget_focusable(layout->nodes[layout->focus].widget);

    if (hints == NULL && layout->focus >= 0)
        hints = pnl_widget_hints(layout->nodes[layout->focus].widget);

    snprintf(layout->footer_text, sizeof(layout->footer_text), "%s%s%s%sesc: back", hints ? hints : "",
             hints ? "  " : "", fields ? "tab: next  " : layout->visible_len > 1 ? "tab: next panel  " : "",
             layout->has_tabs ? "[ ]: switch tab  " : "");
    layout->footer = layout->footer_text;
}

static void pnl_layout_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    pnl_layout_t *layout = ctx;
    struct pnl_rect rect;

    rect.x = 0;
    rect.y = 0;
    rect.width = pnl_canvas_width(canvas);
    rect.height = pnl_canvas_height(canvas);
    layout->visible_len = 0;
    layout->hits_len = 0;
    pnl_layout_draw_node(layout, canvas, PNL_LAYOUT_ROOT, rect);
    if (layout->visible_len > 0 && !pnl_layout_is_visible(layout, layout->focus)) {
        layout->focus = layout->visible[0];
        app->dirty = 1;
    }
    pnl_layout_footer(layout);
}

static void pnl_layout_cycle(pnl_layout_t *layout, int step)
{
    size_t i;

    for (i = 0; i < layout->visible_len; i++) {
        if (layout->visible[i] == layout->focus)
            break;
    }
    if (layout->visible_len == 0)
        return;
    if (i == layout->visible_len)
        i = 0;
    else
        i = (i + (step > 0 ? 1 : layout->visible_len - 1)) % layout->visible_len;
    layout->focus = layout->visible[i];
}

static int pnl_layout_tabs_of(const pnl_layout_t *layout)
{
    int node = layout->focus;
    size_t i;

    while (node >= 0 && layout->nodes[node].kind != PNL_SPLIT_TABS)
        node = layout->nodes[node].parent;
    for (i = 0; node < 0 && i < layout->nodes_len; i++) {
        if (layout->nodes[i].kind == PNL_SPLIT_TABS)
            node = (int)i;
    }
    return node;
}

static int pnl_layout_switch(pnl_layout_t *layout, int step)
{
    int tabs = pnl_layout_tabs_of(layout);
    int active;
    int child;
    int previous = -1;
    int last = -1;

    if (tabs < 0)
        return 0;
    active = pnl_layout_active(layout, tabs);
    for (child = pnl_layout_next_child(layout, tabs, -1); child >= 0; child = pnl_layout_next_child(layout, tabs, child)) {
        if (child == active)
            previous = last;
        last = child;
    }
    child = step > 0 ? pnl_layout_next_child(layout, tabs, active) : previous;
    if (child < 0)
        child = step > 0 ? pnl_layout_next_child(layout, tabs, -1) : last;
    layout->nodes[tabs].active = child;
    if (pnl_layout_first_panel(layout, child) >= 0)
        layout->focus = pnl_layout_first_panel(layout, child);
    return 1;
}

static const struct pnl_hit *pnl_layout_hit(const pnl_layout_t *layout, int x, int y)
{
    size_t i;

    for (i = 0; i < layout->hits_len; i++) {
        const struct pnl_hit *hit = &layout->hits[i];

        if (x >= hit->x && y >= hit->y && x < hit->x + hit->width && y < hit->y + hit->height)
            return hit;
    }
    return NULL;
}

static int pnl_layout_mouse(pnl_layout_t *layout, pnl_app_t *app, int key)
{
    const struct pnl_hit *hit = pnl_layout_hit(layout, app->term.mouse_x, app->term.mouse_y);
    const struct pnl_node *n;

    if (hit == NULL)
        return 1;
    if (hit->tabs >= 0) {
        if (key == PNL_KEY_CLICK) {
            layout->nodes[hit->tabs].active = hit->node;
            if (pnl_layout_first_panel(layout, hit->node) >= 0)
                layout->focus = pnl_layout_first_panel(layout, hit->node);
        }
        return 1;
    }
    n = &layout->nodes[hit->node];
    if (key == PNL_KEY_CLICK)
        layout->focus = hit->node;
    app->mouse_ox = hit->origin_x;
    app->mouse_oy = hit->origin_y;
    if (n->key_fn != NULL && !n->key_fn(n->ctx, app, key))
        pnl_wheel_keys(app, n->key_fn, n->ctx, key);
    return 1;
}

static int pnl_layout_key(void *ctx, pnl_app_t *app, int key)
{
    pnl_layout_t *layout = ctx;
    const struct pnl_node *focused;

    if (pnl_is_mouse_key(key))
        return pnl_layout_mouse(layout, app, key);
    if (layout->focus >= 0 && layout->nodes[layout->focus].widget != NULL &&
        pnl_widget_key(layout->nodes[layout->focus].widget, key))
        return 1;
    if (key == PNL_KEY_TAB || key == PNL_KEY_BACKTAB) {
        pnl_layout_cycle(layout, key == PNL_KEY_TAB ? 1 : -1);
        if (layout->focus >= 0)
            pnl_widget_focus_edge(layout->nodes[layout->focus].widget, key == PNL_KEY_BACKTAB);
        return 1;
    }
    if (key == '[' || key == ']')
        return pnl_layout_switch(layout, key == ']' ? 1 : -1);
    if (layout->focus < 0)
        return 0;
    focused = &layout->nodes[layout->focus];
    if (focused->widget != NULL)
        return 0;
    return focused->key_fn != NULL && focused->key_fn(focused->ctx, app, key);
}

static void pnl_layout_drop(pnl_app_t *app, void *ctx)
{
    (void)app;
    pnl_layout_destroy(ctx);
}

static pnl_layout_t *pnl_layout_copy(const pnl_layout_t *layout)
{
    pnl_layout_t *copy = pnl_mem_alloc(&layout->app->mem, sizeof(*copy));
    size_t i;

    if (copy == NULL)
        return NULL;
    memset(copy, 0, sizeof(*copy));
    copy->app = layout->app;
    copy->focus = layout->focus;
    if (pnl_mem_grow(&copy->app->mem, (void **)&copy->nodes, &copy->nodes_cap, layout->nodes_len,
                     sizeof(*copy->nodes)) != PNL_OK) {
        pnl_layout_destroy(copy);
        return NULL;
    }
    for (i = 0; i < layout->nodes_len; i++) {
        copy->nodes[i] = layout->nodes[i];
        copy->nodes[i].title = NULL;
        copy->nodes[i].hints = NULL;
        copy->nodes_len++;
        copy->has_tabs |= layout->nodes[i].kind == PNL_SPLIT_TABS;
        if (layout->nodes[i].hints != NULL &&
            (copy->nodes[i].hints = pnl_mem_copy(&copy->app->mem, layout->nodes[i].hints)) == NULL) {
            pnl_layout_destroy(copy);
            return NULL;
        }
        if (layout->nodes[i].title != NULL &&
            (copy->nodes[i].title = pnl_mem_copy(&copy->app->mem, layout->nodes[i].title)) == NULL) {
            pnl_layout_destroy(copy);
            return NULL;
        }
    }
    return copy;
}

int pnl_view_push_layout(pnl_app_t *app, const char *title, const pnl_layout_t *layout)
{
    pnl_layout_t *copy;
    struct pnl_view view;
    int status;

    if (app == NULL || layout == NULL || layout->app != app)
        return PNL_ERR_ARG;
    memset(&view, 0, sizeof(view));
    view.draw_fn = pnl_layout_draw;
    view.key_fn = pnl_layout_key;
    view.drop_fn = pnl_layout_drop;
    if (title != NULL && (view.title = pnl_mem_copy(&app->mem, title)) == NULL)
        return PNL_ERR_NOMEM;
    copy = pnl_layout_copy(layout);
    if (copy != NULL) {
        pnl_layout_footer(copy);
        view.footer_ref = &copy->footer;
    }
    view.ctx = copy;
    status = copy == NULL ? PNL_ERR_NOMEM : pnl_view_push_internal(app, &view);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, view.title);
        pnl_layout_destroy(view.ctx);
    }
    return status;
}

#include <stdio.h>
#include <string.h>

#define PNL_STEPS_MAX 16
#define PNL_STEP_LABEL 32
#define PNL_STEP_DETAIL 48
#define PNL_SCENES_MAX 16
#define PNL_FRAMES_MAX 16
#define PNL_ARROW 3

struct pnl_step {
    char label[PNL_STEP_LABEL];
    char detail[PNL_STEP_DETAIL];
    int state;
};

struct pnl_flow_widget {
    pnl_app_t *app;
    struct pnl_lock lock;
    struct pnl_step steps[PNL_STEPS_MAX];
    int len;
};

struct pnl_scene {
    int state;
    char *frames[PNL_FRAMES_MAX];
    int frames_len;
    int style;
    int gradient;
    int frame_ms;
};

struct pnl_scene_widget {
    pnl_app_t *app;
    struct pnl_lock lock;
    struct pnl_scene scenes[PNL_SCENES_MAX];
    int len;
    int state;
};

static const int pnl_step_roles[] = {PNL_ROLE_MUTED, PNL_ROLE_ACCENT, PNL_ROLE_OK, PNL_ROLE_FAIL, PNL_ROLE_MUTED};
static const char *const pnl_step_icons[] = {"\xc2\xb7", "", "\xe2\x9c\x93", "\xe2\x9c\x97", "-"};

static void pnl_live_copy(char *dest, size_t cap, const char *text)
{
    size_t len = strlen(text);

    if (len >= cap) {
        len = cap - 1;
        while (len > 0 && ((unsigned char)text[len] & 0xc0) == 0x80)
            len--;
    }
    memcpy(dest, text, len);
    dest[len] = '\0';
}

static int pnl_box_width(const struct pnl_step *step)
{
    return pnl_text_width(step->label) + 6;
}

static int pnl_flow_wide(const struct pnl_flow_widget *flow)
{
    int width = 0;
    int i;

    for (i = 0; i < flow->len; i++)
        width += pnl_box_width(&flow->steps[i]) + (i > 0 ? PNL_ARROW : 0);
    return width;
}

static int pnl_flow_details(const struct pnl_flow_widget *flow)
{
    int i;

    for (i = 0; i < flow->len; i++) {
        if (flow->steps[i].detail[0] != '\0')
            return 1;
    }
    return 0;
}

static void pnl_step_icon(pnl_canvas_t *canvas, int x, int y, const struct pnl_step *step)
{
    int style = PNL_BOLD | PNL_FG(pnl_step_roles[step->state]);

    if (step->state == PNL_STEP_RUN)
        pnl_canvas_spinner(canvas, x, y, style, PNL_SPINNER_DOTS);
    else
        pnl_canvas_text(canvas, x, y, style, pnl_step_icons[step->state]);
}

static void pnl_flow_line(pnl_canvas_t *canvas, int x, int y, int width, const char *left, const char *right,
                          int style)
{
    int i;

    pnl_canvas_text(canvas, x, y, style, left);
    for (i = 1; i < width - 1; i++)
        pnl_canvas_text(canvas, x + i, y, style, "\xe2\x94\x80");
    pnl_canvas_text(canvas, x + width - 1, y, style, right);
}

static void pnl_flow_box(pnl_canvas_t *canvas, int x, const struct pnl_step *step, int detail_width)
{
    int width = pnl_box_width(step);
    int border = (step->state == PNL_STEP_RUN ? PNL_BOLD : 0) | PNL_FG(pnl_step_roles[step->state]);
    int label = step->state == PNL_STEP_WAIT || step->state == PNL_STEP_SKIP ? PNL_FG(PNL_ROLE_MUTED) : PNL_BOLD;

    pnl_flow_line(canvas, x, 0, width, "\xe2\x94\x8c", "\xe2\x94\x90", border);
    pnl_canvas_text(canvas, x, 1, border, "\xe2\x94\x82");
    pnl_step_icon(canvas, x + 2, 1, step);
    pnl_canvas_text(canvas, x + 4, 1, label, step->label);
    pnl_canvas_text(canvas, x + width - 1, 1, border, "\xe2\x94\x82");
    pnl_flow_line(canvas, x, 2, width, "\xe2\x94\x94", "\xe2\x94\x98", border);
    if (detail_width > 0)
        pnl_canvas_clip(canvas, x + 1, 3, detail_width, PNL_FG(PNL_ROLE_MUTED), step->detail);
}

static void pnl_flow_arrow(pnl_canvas_t *canvas, int x, int y, const struct pnl_step *from, const struct pnl_step *to)
{
    int spin = canvas->screen->spin;
    int role = from->state == PNL_STEP_OK ? PNL_ROLE_OK : PNL_ROLE_MUTED;
    int i;

    if (to->state == PNL_STEP_RUN)
        role = PNL_ROLE_ACCENT;
    for (i = 0; i < PNL_ARROW; i++) {
        int lit = to->state == PNL_STEP_RUN && spin >= 0 && spin / 3 % PNL_ARROW == i;
        const char *glyph = i == PNL_ARROW - 1 ? ">" : "\xe2\x94\x80";

        pnl_canvas_text(canvas, x + i, y, lit ? PNL_BOLD | PNL_REVERSE | PNL_FG(role) : PNL_FG(role), glyph);
    }
    if (to->state == PNL_STEP_RUN && spin >= 0)
        canvas->screen->spinning = 1;
}

static void pnl_flow_across(const struct pnl_flow_widget *flow, pnl_canvas_t *canvas)
{
    int details = pnl_flow_details(flow);
    int room;
    int x = 0;
    int i;

    for (i = 0; i < flow->len; i++) {
        if (i > 0) {
            pnl_flow_arrow(canvas, x, 1, &flow->steps[i - 1], &flow->steps[i]);
            x += PNL_ARROW;
        }
        room = i + 1 < flow->len ? pnl_box_width(&flow->steps[i]) + PNL_ARROW - 1 : pnl_canvas_width(canvas) - x - 1;
        pnl_flow_box(canvas, x, &flow->steps[i], details ? room : 0);
        x += pnl_box_width(&flow->steps[i]);
    }
}

static void pnl_flow_down(const struct pnl_flow_widget *flow, pnl_canvas_t *canvas)
{
    int label_width = 0;
    int i;

    for (i = 0; i < flow->len; i++) {
        int w = pnl_text_width(flow->steps[i].label);

        label_width = w > label_width ? w : label_width;
    }
    for (i = 0; i < flow->len; i++) {
        const struct pnl_step *step = &flow->steps[i];
        int y = i * 2;

        if (i > 0) {
            int lit = step->state == PNL_STEP_RUN;
            int role = lit ? PNL_ROLE_ACCENT : flow->steps[i - 1].state == PNL_STEP_OK ? PNL_ROLE_OK : PNL_ROLE_MUTED;

            pnl_canvas_text(canvas, 0, y - 1, PNL_FG(role), "\xe2\x94\x82");
        }
        pnl_step_icon(canvas, 0, y, step);
        pnl_canvas_text(canvas, 2, y, step->state == PNL_STEP_WAIT ? PNL_FG(PNL_ROLE_MUTED) : PNL_BOLD, step->label);
        pnl_canvas_text(canvas, label_width + 4, y, PNL_FG(PNL_ROLE_MUTED), step->detail);
    }
}

static void pnl_flow_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_flow_widget *flow = ctx;

    (void)app;
    pnl_lock(&flow->lock);
    if (pnl_flow_wide(flow) <= pnl_canvas_width(canvas) && !canvas->screen->linear)
        pnl_flow_across(flow, canvas);
    else
        pnl_flow_down(flow, canvas);
    pnl_unlock(&flow->lock);
}

static int pnl_flow_height(void *ctx, pnl_app_t *app, int width)
{
    struct pnl_flow_widget *flow = ctx;
    int height;

    pnl_lock(&flow->lock);
    if (pnl_flow_wide(flow) <= width && !app->screen.linear)
        height = 3 + pnl_flow_details(flow);
    else
        height = flow->len > 0 ? flow->len * 2 - 1 : 1;
    pnl_unlock(&flow->lock);
    return height;
}

static void pnl_flow_release(void *ctx)
{
    struct pnl_flow_widget *flow = ctx;

    pnl_lock_destroy(&flow->lock);
    pnl_mem_free(&flow->app->mem, flow);
}

int pnl_widget_flow(pnl_app_t *app, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t flow_class = {pnl_flow_draw, NULL, pnl_flow_height, NULL, pnl_flow_release};
    struct pnl_flow_widget *flow;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL)
        return PNL_ERR_ARG;
    flow = pnl_mem_alloc(&app->mem, sizeof(*flow));
    if (flow == NULL)
        return PNL_ERR_NOMEM;
    memset(flow, 0, sizeof(*flow));
    flow->app = app;
    if (pnl_lock_init(&flow->lock) != PNL_OK) {
        pnl_mem_free(&app->mem, flow);
        return PNL_ERR_NOMEM;
    }
    return pnl_widget_builtin(app, PNL_WIDGET_FLOW, &flow_class, NULL, flow, out_widget);
}

int pnl_widget_add_step(pnl_widget_t *widget, const char *label, int *out_step)
{
    struct pnl_flow_widget *flow;
    int status = PNL_OK;

    if (out_step != NULL)
        *out_step = -1;
    if (widget == NULL || label == NULL || widget->kind != PNL_WIDGET_FLOW)
        return PNL_ERR_ARG;
    flow = widget->ctx;
    pnl_lock(&flow->lock);
    if (flow->len == PNL_STEPS_MAX) {
        status = PNL_ERR_LIMIT;
    } else {
        memset(&flow->steps[flow->len], 0, sizeof(flow->steps[0]));
        pnl_live_copy(flow->steps[flow->len].label, PNL_STEP_LABEL, label);
        if (out_step != NULL)
            *out_step = flow->len;
        flow->len++;
    }
    pnl_unlock(&flow->lock);
    widget->app->dirty = 1;
    return status;
}

int pnl_widget_set_step(pnl_widget_t *widget, int step, int state, const char *detail)
{
    struct pnl_flow_widget *flow;
    int status = PNL_OK;

    if (widget == NULL || widget->kind != PNL_WIDGET_FLOW || state < PNL_STEP_WAIT || state > PNL_STEP_SKIP)
        return PNL_ERR_ARG;
    flow = widget->ctx;
    pnl_lock(&flow->lock);
    if (step < 0 || step >= flow->len) {
        status = PNL_ERR_NOT_FOUND;
    } else {
        flow->steps[step].state = state;
        pnl_live_copy(flow->steps[step].detail, PNL_STEP_DETAIL, detail != NULL ? detail : "");
    }
    pnl_unlock(&flow->lock);
    if (status == PNL_OK)
        pnl_term_wake(&widget->app->term);
    return status;
}

static int pnl_scene_find(const struct pnl_scene_widget *scene, int state)
{
    int i;

    for (i = 0; i < scene->len; i++) {
        if (scene->scenes[i].state == state)
            return i;
    }
    return -1;
}

static const struct pnl_scene *pnl_scene_current(const struct pnl_scene_widget *scene)
{
    int slot = pnl_scene_find(scene, scene->state);

    return slot >= 0 ? &scene->scenes[slot] : NULL;
}

static void pnl_scene_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_scene_widget *scene = ctx;
    const struct pnl_scene *shown;
    int frame = 0;

    if (canvas->screen->linear)
        return;
    pnl_lock(&scene->lock);
    shown = pnl_scene_current(scene);
    if (shown != NULL && shown->frames_len > 1 && canvas->screen->spin >= 0) {
        frame = (int)(pnl_app_frame_ms(app) / shown->frame_ms % shown->frames_len);
        canvas->screen->spinning = 1;
    }
    if (shown != NULL)
        pnl_canvas_art(canvas, 0, 0, shown->style, shown->gradient, shown->frames[frame]);
    pnl_unlock(&scene->lock);
}

static int pnl_scene_height(void *ctx, pnl_app_t *app, int width)
{
    struct pnl_scene_widget *scene = ctx;
    int height = 0;
    int i;
    int j;

    (void)width;
    if (app->screen.linear)
        return 0;
    pnl_lock(&scene->lock);
    for (i = 0; i < scene->len; i++) {
        for (j = 0; j < scene->scenes[i].frames_len; j++) {
            int art_width = 0;
            int h = pnl_art_size(scene->scenes[i].frames[j], &art_width);

            height = h > height ? h : height;
        }
    }
    pnl_unlock(&scene->lock);
    return height;
}

static void pnl_scene_free(struct pnl_scene_widget *scene, struct pnl_scene *shown)
{
    int i;

    for (i = 0; i < shown->frames_len; i++)
        pnl_mem_free(&scene->app->mem, shown->frames[i]);
    shown->frames_len = 0;
}

static void pnl_scene_release(void *ctx)
{
    struct pnl_scene_widget *scene = ctx;
    int i;

    for (i = 0; i < scene->len; i++)
        pnl_scene_free(scene, &scene->scenes[i]);
    pnl_lock_destroy(&scene->lock);
    pnl_mem_free(&scene->app->mem, scene);
}

static int pnl_scene_set_state(pnl_widget_t *widget, double value)
{
    struct pnl_scene_widget *scene = widget->ctx;
    int state;
    int changed;

    if (!(value >= -2147483647.0 && value <= 2147483647.0))
        return PNL_ERR_ARG;
    state = (int)value;

    pnl_lock(&scene->lock);
    changed = scene->state != state;
    scene->state = state;
    pnl_unlock(&scene->lock);
    if (changed)
        pnl_term_wake(&widget->app->term);
    return PNL_OK;
}

static int pnl_scene_state(const pnl_widget_t *widget, double *out_value)
{
    struct pnl_scene_widget *scene = widget->ctx;

    pnl_lock(&scene->lock);
    *out_value = scene->state;
    pnl_unlock(&scene->lock);
    return PNL_OK;
}

int pnl_widget_scene(pnl_app_t *app, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t scene_class = {pnl_scene_draw, NULL, pnl_scene_height, NULL, pnl_scene_release};
    static const struct pnl_widget_ops scene_ops = {NULL, NULL, pnl_scene_set_state, pnl_scene_state, NULL};
    struct pnl_scene_widget *scene;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL)
        return PNL_ERR_ARG;
    scene = pnl_mem_alloc(&app->mem, sizeof(*scene));
    if (scene == NULL)
        return PNL_ERR_NOMEM;
    memset(scene, 0, sizeof(*scene));
    scene->app = app;
    if (pnl_lock_init(&scene->lock) != PNL_OK) {
        pnl_mem_free(&app->mem, scene);
        return PNL_ERR_NOMEM;
    }
    return pnl_widget_builtin(app, PNL_WIDGET_SCENE, &scene_class, &scene_ops, scene, out_widget);
}

static int pnl_scene_split(struct pnl_scene_widget *scene, struct pnl_scene *shown, const char *frames)
{
    const char *start = frames;

    while (start != NULL && shown->frames_len < PNL_FRAMES_MAX) {
        const char *end = (strchr)(start, '\f');
        size_t len = end != NULL ? (size_t)(end - start) : strlen(start);
        char *copy = pnl_mem_alloc(&scene->app->mem, len + 1);

        if (copy == NULL)
            return PNL_ERR_NOMEM;
        memcpy(copy, start, len);
        copy[len] = '\0';
        shown->frames[shown->frames_len++] = copy;
        start = end != NULL ? end + 1 : NULL;
    }
    return PNL_OK;
}

int pnl_widget_add_scene(pnl_widget_t *widget, int state, const char *frames, int style, int gradient, int frame_ms)
{
    struct pnl_scene_widget *scene;
    struct pnl_scene next;
    struct pnl_scene old;
    int slot;
    int status;

    if (widget == NULL || frames == NULL || widget->kind != PNL_WIDGET_SCENE || frame_ms < 0)
        return PNL_ERR_ARG;
    scene = widget->ctx;
    memset(&next, 0, sizeof(next));
    next.state = state;
    next.style = style;
    next.gradient = gradient;
    next.frame_ms = frame_ms > 0 ? frame_ms : 500;
    status = pnl_scene_split(scene, &next, frames);
    pnl_lock(&scene->lock);
    slot = pnl_scene_find(scene, state);
    if (status == PNL_OK && slot < 0 && scene->len == PNL_SCENES_MAX)
        status = PNL_ERR_LIMIT;
    if (status == PNL_OK && slot >= 0) {
        old = scene->scenes[slot];
        scene->scenes[slot] = next;
        next = old;
    } else if (status == PNL_OK) {
        scene->scenes[scene->len++] = next;
        next.frames_len = 0;
    }
    pnl_unlock(&scene->lock);
    pnl_scene_free(scene, &next);
    widget->app->dirty = 1;
    return status;
}


#include <stdio.h>
#include <string.h>
#include <time.h>

#define PNL_LOG_TEXT 240
#define PNL_LOG_AVERAGE 128
#define PNL_LOG_FILTER_MAX 128
#define PNL_LOG_FOOTER "up/down: scroll  end: follow  v: level  /: filter  q: back"
#define PNL_LOG_FILTER_FOOTER "type to filter  enter: keep  esc: clear"
#define PNL_LOG_HINTS "end: follow  v: level  /: filter"

struct pnl_log_entry {
    unsigned long long seq;
    time_t when;
    unsigned int start;
    unsigned char level;
};

struct pnl_log {
    pnl_app_t *app;
    struct pnl_lock lock;
    struct pnl_log_entry *entries;
    size_t capacity;
    char *text;
    size_t text_size;
    size_t head;
    unsigned long long first_seq;
    unsigned long long next_seq;
    int pending;
};

struct pnl_log_view {
    pnl_log_t *logbook;
    size_t *shown;
    size_t shown_len;
    unsigned long long anchor;
    size_t page;
    int follow;
    int level;
    char filter[PNL_LOG_FILTER_MAX];
    struct pnl_edit edit;
    int editing;
    const char *footer;
};

static const char *const pnl_level_names[] = {"debug", "info", "warn", "error"};
static const int pnl_level_roles[] = {PNL_ROLE_MUTED, PNL_ROLE_INFO, PNL_ROLE_WARN, PNL_ROLE_FAIL};

int pnl_log_create(pnl_app_t *app, size_t capacity, pnl_log_t **out_log)
{
    pnl_log_t *logbook;
    int status;

    if (out_log != NULL)
        *out_log = NULL;
    if (app == NULL || capacity == 0 || capacity > ((size_t)-1) / 2 / sizeof(struct pnl_log_entry) ||
        capacity > ((unsigned int)-1 - PNL_LOG_TEXT) / PNL_LOG_AVERAGE || out_log == NULL)
        return PNL_ERR_ARG;
    status = pnl_term_wake_open(&app->term);
    if (status != PNL_OK)
        return status;
    logbook = pnl_mem_alloc(&app->mem, sizeof(*logbook));
    if (logbook == NULL)
        return PNL_ERR_NOMEM;
    memset(logbook, 0, sizeof(*logbook));
    logbook->app = app;
    logbook->capacity = capacity;
    logbook->text_size = capacity * PNL_LOG_AVERAGE + PNL_LOG_TEXT;
    logbook->entries = pnl_mem_alloc(&app->mem, capacity * sizeof(*logbook->entries));
    logbook->text = pnl_mem_alloc(&app->mem, logbook->text_size);
    status = logbook->entries == NULL || logbook->text == NULL ? PNL_ERR_NOMEM : pnl_lock_init(&logbook->lock);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, logbook->entries);
        pnl_mem_free(&app->mem, logbook->text);
        pnl_mem_free(&app->mem, logbook);
        return status;
    }
    *out_log = logbook;
    return PNL_OK;
}

void pnl_log_destroy(pnl_log_t *logbook)
{
    if (logbook == NULL)
        return;
    pnl_lock_destroy(&logbook->lock);
    pnl_mem_free(&logbook->app->mem, logbook->entries);
    pnl_mem_free(&logbook->app->mem, logbook->text);
    pnl_mem_free(&logbook->app->mem, logbook);
}

static const char *pnl_log_text(const pnl_log_t *logbook, const struct pnl_log_entry *entry)
{
    return logbook->text + entry->start;
}

static size_t pnl_log_length(const char *text)
{
    size_t len = 0;

    while (text[len] != '\0' && text[len] != '\n' && len + 1 < PNL_LOG_TEXT)
        len++;
    while (len > 0 && text[len] != '\0' && text[len] != '\n' && ((unsigned char)text[len] & 0xc0) == 0x80)
        len--;
    return len;
}

static void pnl_log_evict(pnl_log_t *logbook, size_t reach)
{
    while (logbook->first_seq < logbook->next_seq) {
        const struct pnl_log_entry *oldest = &logbook->entries[logbook->first_seq % logbook->capacity];
        size_t ahead = (oldest->start + logbook->text_size - logbook->head) % logbook->text_size;

        if (logbook->next_seq - logbook->first_seq < logbook->capacity && ahead >= reach)
            return;
        logbook->first_seq++;
    }
}

static void pnl_log_store(pnl_log_t *logbook, struct pnl_log_entry *entry, const char *text)
{
    size_t len = pnl_log_length(text);
    size_t start = logbook->head + len + 1 > logbook->text_size ? 0 : logbook->head;
    size_t i;

    pnl_log_evict(logbook, start == 0 ? logbook->text_size - logbook->head + len + 1 : len + 1);
    for (i = 0; i < len; i++)
        logbook->text[start + i] = text[i] == '\t' ? ' ' : text[i];
    logbook->text[start + len] = '\0';
    entry->start = (unsigned int)start;
    logbook->head = (start + len + 1) % logbook->text_size;
}

int pnl_log_write(pnl_log_t *logbook, int level, const char *text)
{
    struct pnl_log_entry *entry;
    int wake;

    if (logbook == NULL || text == NULL || level < PNL_LEVEL_DEBUG || level > PNL_LEVEL_ERROR)
        return PNL_ERR_ARG;
    pnl_lock(&logbook->lock);
    pnl_log_evict(logbook, 0);
    entry = &logbook->entries[logbook->next_seq % logbook->capacity];
    pnl_log_store(logbook, entry, text);
    entry->seq = logbook->next_seq++;
    entry->when = time(NULL);
    entry->level = (unsigned char)level;
    wake = !logbook->pending;
    logbook->pending = 1;
    pnl_unlock(&logbook->lock);
    if (wake)
        pnl_term_wake(&logbook->app->term);
    return PNL_OK;
}

static unsigned long long pnl_log_first(const pnl_log_t *logbook)
{
    return logbook->first_seq;
}

static int pnl_log_matches(const struct pnl_log_view *view, const struct pnl_log_entry *entry)
{
    return entry->level >= view->level && pnl_text_contains(pnl_log_text(view->logbook, entry), view->filter);
}

static void pnl_log_collect(struct pnl_log_view *view)
{
    pnl_log_t *logbook = view->logbook;
    unsigned long long seq;

    view->shown_len = 0;
    for (seq = pnl_log_first(logbook); seq < logbook->next_seq; seq++) {
        size_t slot = (size_t)(seq % logbook->capacity);

        if (pnl_log_matches(view, &logbook->entries[slot]))
            view->shown[view->shown_len++] = slot;
    }
}

static size_t pnl_log_anchor_index(const struct pnl_log_view *view)
{
    size_t i = view->shown_len;

    while (i > 0 && view->logbook->entries[view->shown[i - 1]].seq > view->anchor)
        i--;
    return i;
}

static void pnl_log_clock(char *out, size_t out_len, time_t when)
{
    struct tm parts;

#if defined(_WIN32)
    localtime_s(&parts, &when);
#else
    localtime_r(&when, &parts);
#endif
    snprintf(out, out_len, "%02d:%02d:%02d", parts.tm_hour, parts.tm_min, parts.tm_sec);
}

static void pnl_log_draw_entry(pnl_canvas_t *canvas, const pnl_log_t *logbook, const struct pnl_log_entry *entry,
                               int y, const char *filter)
{
    const char *text = pnl_log_text(logbook, entry);
    char clock[16];
    const char *hit = NULL;
    const char *at;

    pnl_log_clock(clock, sizeof(clock), entry->when);
    pnl_canvas_text(canvas, 1, y, PNL_FG(PNL_ROLE_MUTED), clock);
    pnl_canvas_text(canvas, 11, y, PNL_FG(pnl_level_roles[entry->level]), pnl_level_names[entry->level]);
    pnl_canvas_text(canvas, 18, y, entry->level == PNL_LEVEL_DEBUG ? PNL_FG(PNL_ROLE_MUTED) : 0, text);
    for (at = text; filter[0] != '\0' && *at != '\0' && hit == NULL; at++)
        hit = pnl_text_contains_at(at, filter) ? at : NULL;
    if (hit != NULL) {
        char before[PNL_LOG_TEXT];
        char match[PNL_LOG_FILTER_MAX];

        snprintf(before, sizeof(before), "%.*s", (int)(hit - text), text);
        snprintf(match, sizeof(match), "%.*s", (int)strlen(filter), hit);
        pnl_canvas_text(canvas, 18 + pnl_text_width(before), y, PNL_REVERSE, match);
    }
}

static void pnl_log_draw_state(struct pnl_log_view *view, pnl_canvas_t *canvas, size_t end)
{
    char state[96];
    int width = pnl_canvas_width(canvas);
    size_t waiting = view->shown_len - end;

    if (view->follow)
        snprintf(state, sizeof(state), "following  %s+", pnl_level_names[view->level]);
    else
        snprintf(state, sizeof(state), "paused, %zu new  %s+", waiting, pnl_level_names[view->level]);
    pnl_canvas_text(canvas, width - pnl_text_width(state) - 1, 0, PNL_FG(view->follow ? PNL_ROLE_OK : PNL_ROLE_WARN),
                    state);
    if (!view->editing && view->filter[0] == '\0')
        return;
    pnl_canvas_text(canvas, 1, 0, PNL_FG(PNL_ROLE_ACCENT), "/");
    pnl_edit_draw(&view->edit, canvas, 3, 0, width - pnl_text_width(state) - 6, view->editing);
}

static void pnl_log_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_log_view *view = ctx;
    pnl_log_t *logbook = view->logbook;
    int rows = pnl_canvas_height(canvas) - 2;
    size_t end;
    size_t start;
    size_t i;

    (void)app;
    view->page = rows > 1 ? (size_t)rows : 1;
    pnl_lock(&logbook->lock);
    logbook->pending = 0;
    pnl_log_collect(view);
    end = view->follow ? view->shown_len : pnl_log_anchor_index(view);
    if (end < view->page)
        end = view->shown_len < view->page ? view->shown_len : view->page;
    start = rows > 0 && end > (size_t)rows ? end - (size_t)rows : 0;
    pnl_log_draw_state(view, canvas, end);
    for (i = start; i < end; i++)
        pnl_log_draw_entry(canvas, logbook, &logbook->entries[view->shown[i]], 2 + (int)(i - start), view->filter);
    if (view->shown_len == 0)
        pnl_canvas_text(canvas, 1, 2, PNL_FG(PNL_ROLE_MUTED), logbook->next_seq == 0 ? "nothing yet" : "no match");
    pnl_unlock(&logbook->lock);
}

static void pnl_log_scroll(struct pnl_log_view *view, size_t height, int key)
{
    size_t step = key == PNL_KEY_PAGE_UP || key == PNL_KEY_PAGE_DOWN ? height : 1;
    size_t end;

    pnl_lock(&view->logbook->lock);
    pnl_log_collect(view);
    end = view->follow ? view->shown_len : pnl_log_anchor_index(view);
    if (key == PNL_KEY_HOME)
        end = view->shown_len < height ? view->shown_len : height;
    else if (key == PNL_KEY_UP || key == 'k' || key == PNL_KEY_PAGE_UP)
        end = end > height + step ? end - step : (end > height ? height : end);
    else
        end = end + step;
    view->follow = end >= view->shown_len;
    if (!view->follow && end > 0)
        view->anchor = view->logbook->entries[view->shown[end - 1]].seq;
    pnl_unlock(&view->logbook->lock);
}

static int pnl_log_filter_key(struct pnl_log_view *view, int key)
{
    if (key == PNL_KEY_ENTER || key == PNL_KEY_ESCAPE) {
        view->editing = 0;
        view->footer = PNL_LOG_FOOTER;
        if (key == PNL_KEY_ESCAPE) {
            view->filter[0] = '\0';
            pnl_edit_init(&view->edit, view->filter, sizeof(view->filter));
        }
        return 1;
    }
    pnl_edit_key(&view->edit, key);
    return 1;
}

static int pnl_log_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_log_view *view = ctx;

    (void)app;
    if (view->editing && !pnl_is_mouse_key(key))
        return pnl_log_filter_key(view, key);
    if (key == PNL_KEY_WHEEL_UP || key == PNL_KEY_WHEEL_DOWN)
        key = key == PNL_KEY_WHEEL_UP ? PNL_KEY_UP : PNL_KEY_DOWN;
    if (key == '/') {
        view->editing = 1;
        view->footer = PNL_LOG_FILTER_FOOTER;
    } else if (key == 'v')
        view->level = (view->level + 1) % (PNL_LEVEL_ERROR + 1);
    else if (key == PNL_KEY_END || key == 'f' || key == 'G')
        view->follow = 1;
    else if (key == PNL_KEY_UP || key == PNL_KEY_DOWN || key == 'j' || key == 'k' || key == PNL_KEY_PAGE_UP ||
             key == PNL_KEY_PAGE_DOWN || key == PNL_KEY_HOME)
        pnl_log_scroll(view, view->page, key);
    else
        return 0;
    return 1;
}

static void pnl_log_drop(pnl_app_t *app, void *ctx)
{
    struct pnl_log_view *view = ctx;

    pnl_mem_free(&app->mem, view->shown);
    pnl_mem_free(&app->mem, view);
}

static void pnl_log_cli(pnl_log_t *logbook)
{
    pnl_app_t *app = logbook->app;
    unsigned long long seq;
    char clock[16];

    pnl_lock(&logbook->lock);
    if (app->cli_json)
        putchar('[');
    for (seq = pnl_log_first(logbook); seq < logbook->next_seq; seq++) {
        const struct pnl_log_entry *entry = &logbook->entries[seq % logbook->capacity];

        pnl_log_clock(clock, sizeof(clock), entry->when);
        if (!app->cli_json) {
            printf("%s  %-5s  %s\n", clock, pnl_level_names[entry->level], pnl_log_text(logbook, entry));
            continue;
        }
        printf("%s{\"time\":\"%s\",\"level\":\"%s\",\"text\":", seq > pnl_log_first(logbook) ? "," : "", clock,
               pnl_level_names[entry->level]);
        pnl_json_string(pnl_log_text(logbook, entry));
        putchar('}');
    }
    if (app->cli_json)
        puts("]");
    pnl_unlock(&logbook->lock);
    fflush(stdout);
}

static struct pnl_log_view *pnl_log_view_new(pnl_app_t *app, pnl_log_t *logbook)
{
    struct pnl_log_view *view = pnl_mem_alloc(&app->mem, sizeof(*view));

    if (view == NULL)
        return NULL;
    memset(view, 0, sizeof(*view));
    view->logbook = logbook;
    view->follow = 1;
    view->footer = PNL_LOG_FOOTER;
    pnl_edit_init(&view->edit, view->filter, sizeof(view->filter));
    view->shown = pnl_mem_alloc(&app->mem, logbook->capacity * sizeof(*view->shown));
    if (view->shown == NULL) {
        pnl_log_drop(app, view);
        return NULL;
    }
    return view;
}

static const char *pnl_log_hints(void *ctx)
{
    const struct pnl_log_view *view = ctx;

    return view->editing ? PNL_LOG_FILTER_FOOTER : PNL_LOG_HINTS;
}

static void pnl_log_release(void *ctx)
{
    struct pnl_log_view *view = ctx;

    pnl_log_drop(view->logbook->app, view);
}

int pnl_widget_log(pnl_app_t *app, pnl_log_t *logbook, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t log_class = {pnl_log_draw, pnl_log_key, NULL, pnl_log_hints, pnl_log_release};
    struct pnl_log_view *view;
    int status;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || logbook == NULL || logbook->app != app || out_widget == NULL)
        return PNL_ERR_ARG;
    view = pnl_log_view_new(app, logbook);
    if (view == NULL)
        return PNL_ERR_NOMEM;
    status = pnl_widget_create(app, &log_class, view, out_widget);
    if (status != PNL_OK)
        pnl_log_release(view);
    return status;
}

int pnl_log_open(pnl_app_t *app, pnl_log_t *logbook, const char *title)
{
    struct pnl_log_view *view;
    struct pnl_view screen;
    int status;

    if (app == NULL || logbook == NULL || title == NULL || logbook->app != app || !(app->running || app->cli))
        return PNL_ERR_ARG;
    if (app->cli) {
        pnl_log_cli(logbook);
        return PNL_OK;
    }
    view = pnl_log_view_new(app, logbook);
    if (view == NULL)
        return PNL_ERR_NOMEM;
    memset(&screen, 0, sizeof(screen));
    screen.footer_ref = &view->footer;
    screen.typing_ref = &view->editing;
    screen.draw_fn = pnl_log_draw;
    screen.key_fn = pnl_log_key;
    screen.drop_fn = pnl_log_drop;
    screen.ctx = view;
    screen.title = pnl_mem_copy(&app->mem, title);
    status = screen.title == NULL ? PNL_ERR_NOMEM : pnl_view_push_internal(app, &screen);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, screen.title);
        pnl_log_drop(app, view);
    }
    return status;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_MENU_FOOTER "up/down: move  enter: choose  ?: help  q: quit"
#define PNL_VIEW_FOOTER "esc: back"
#define PNL_TITLE_STYLE PNL_BOLD
#define PNL_FOOTER_STYLE PNL_FG(PNL_ROLE_MUTED)

static void pnl_transition_start(pnl_app_t *app, int divisor)
{
    if (!app->running || !pnl_motion_on(app, PNL_MOTION_TRANSITION))
        return;
    app->transition_ms = pnl_term_now_ms();
    app->transition_shift = app->screen.width / divisor;
}

static int pnl_pulse_step(const pnl_app_t *app)
{
    int phase;
    int depth = app->screen.palette.depth;

    if (!pnl_motion_on(app, PNL_MOTION_PULSE) || (depth != PNL_DEPTH_TRUE && depth != PNL_DEPTH_256))
        return -1;
    phase = (int)(app->frame_ms % PNL_PULSE_MS) * 2 * PNL_PULSE_DEPTH / PNL_PULSE_MS;
    return phase < PNL_PULSE_DEPTH ? phase : 2 * PNL_PULSE_DEPTH - phase;
}

static int pnl_transition_offset(pnl_app_t *app)
{
    double t;

    if (app->transition_shift == 0)
        return 0;
    t = pnl_app_tween(app, app->transition_ms, PNL_TRANSITION_MS);
    if (t >= 1.0)
        app->transition_shift = 0;
    t = 1.0 - (1.0 - t) * (1.0 - t);
    return (int)((1.0 - t) * app->transition_shift + 0.5);
}

int pnl_view_push_internal(pnl_app_t *app, const struct pnl_view *view)
{
    int status;

    if (app->cli) {
        app->cli_needs_tty = 1;
        return PNL_ERR_TTY;
    }
    status = pnl_mem_grow(&app->mem, (void **)&app->views, &app->views_cap, app->views_len + 1,
                              sizeof(*app->views));

    if (status != PNL_OK)
        return status;
    app->views[app->views_len] = *view;
    app->views[app->views_len++].shown_ms = pnl_term_now_ms();
    if (view->dialog_width == 0)
        pnl_transition_start(app, 3);
    app->dirty = 1;
    return PNL_OK;
}

int pnl_view_push(pnl_app_t *app, const char *title, pnl_draw_fn draw_fn, pnl_key_fn key_fn, void *view_ctx)
{
    struct pnl_view view;
    int status;

    if (app == NULL || draw_fn == NULL)
        return PNL_ERR_ARG;
    memset(&view, 0, sizeof(view));
    view.draw_fn = draw_fn;
    view.key_fn = key_fn;
    view.ctx = view_ctx;
    if (title != NULL) {
        view.title = pnl_mem_copy(&app->mem, title);
        if (view.title == NULL)
            return PNL_ERR_NOMEM;
    }
    status = pnl_view_push_internal(app, &view);
    if (status != PNL_OK)
        pnl_mem_free(&app->mem, view.title);
    return status;
}

int pnl_view_push_dialog(pnl_app_t *app, const char *title, int width, int height, pnl_draw_fn draw_fn,
                         pnl_key_fn key_fn, void *view_ctx)
{
    struct pnl_view view;
    int status;

    if (app == NULL || draw_fn == NULL || width <= 0 || height <= 0)
        return PNL_ERR_ARG;
    memset(&view, 0, sizeof(view));
    view.draw_fn = draw_fn;
    view.key_fn = key_fn;
    view.ctx = view_ctx;
    view.dialog_width = width;
    view.dialog_height = height;
    if (title != NULL) {
        view.title = pnl_mem_copy(&app->mem, title);
        if (view.title == NULL)
            return PNL_ERR_NOMEM;
    }
    status = pnl_view_push_internal(app, &view);
    if (status != PNL_OK)
        pnl_mem_free(&app->mem, view.title);
    return status;
}

int pnl_view_pop(pnl_app_t *app)
{
    struct pnl_view view;

    if (app == NULL)
        return PNL_ERR_ARG;
    if (app->views_len == 0)
        return PNL_ERR_NOT_FOUND;
    view = app->views[--app->views_len];
    if (view.dialog_width > 0 && pnl_motion_on(app, PNL_MOTION_ENTER) && view.box_width > 0) {
        app->ghost.ms = pnl_term_now_ms();
        app->ghost.x = view.box_x;
        app->ghost.y = view.box_y;
        app->ghost.width = view.box_width;
        app->ghost.height = view.box_height;
        pnl_app_animate(app, PNL_EXIT_MS);
    }
    pnl_mem_free(&app->mem, view.title);
    pnl_mem_free(&app->mem, view.hints);
    if (view.drop_fn != NULL)
        view.drop_fn(app, view.ctx);
    if (view.dialog_width == 0)
        pnl_transition_start(app, 6);
    app->dirty = 1;
    return PNL_OK;
}

int pnl_view_shake(pnl_app_t *app)
{
    if (app == NULL)
        return PNL_ERR_ARG;
    if (app->views_len == 0)
        return PNL_ERR_NOT_FOUND;
    if (pnl_motion_on(app, PNL_MOTION_FEEDBACK)) {
        app->shake_ms = pnl_term_now_ms();
        app->dirty = 1;
    }
    return PNL_OK;
}

int pnl_shake_offset(pnl_app_t *app)
{
    static const int offsets[] = {0, 2, -2, 2, -1, 1, 0};
    double t;

    if (app->shake_ms == 0)
        return 0;
    t = pnl_app_tween(app, app->shake_ms, PNL_SHAKE_MS);
    if (t >= 1.0) {
        app->shake_ms = 0;
        return 0;
    }
    return offsets[(int)(t * 6.0)];
}

int pnl_view_modal(pnl_app_t *app, struct pnl_view *view)
{
    size_t depth = app->views_len;
    int status = pnl_view_push_internal(app, view);

    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, view->title);
        return status;
    }
    status = pnl_loop(app, depth + 1);
    while (app->views_len > depth)
        pnl_view_pop(app);
    return status;
}

int pnl_view_set_hints(pnl_app_t *app, const char *hints)
{
    struct pnl_view *top;
    char *copy;

    if (app == NULL || hints == NULL)
        return PNL_ERR_ARG;
    if (app->views_len == 0)
        return PNL_ERR_NOT_FOUND;
    copy = pnl_mem_copy(&app->mem, hints);
    if (copy == NULL)
        return PNL_ERR_NOMEM;
    top = &app->views[app->views_len - 1];
    pnl_mem_free(&app->mem, top->hints);
    top->hints = copy;
    app->dirty = 1;
    return PNL_OK;
}

const char *pnl_view_hints(const pnl_app_t *app)
{
    const struct pnl_view *top;

    if (app->views_len == 0)
        return NULL;
    top = &app->views[app->views_len - 1];
    if (top->footer_ref != NULL)
        return *top->footer_ref;
    if (top->hints != NULL)
        return top->hints;
    return top->footer != NULL ? top->footer : PNL_VIEW_FOOTER;
}

void pnl_views_drop(pnl_app_t *app)
{
    while (app->views_len > 0)
        pnl_view_pop(app);
}

int pnl_app_add_timer(pnl_app_t *app, int interval_ms, pnl_timer_fn timer_fn, void *timer_ctx, int *out_timer)
{
    struct pnl_timer *timer;
    int status;

    if (app == NULL || interval_ms <= 0)
        return PNL_ERR_ARG;
    status = pnl_mem_grow(&app->mem, (void **)&app->timers, &app->timers_cap, app->timers_len + 1,
                          sizeof(*timer));
    if (status != PNL_OK)
        return status;
    timer = &app->timers[app->timers_len++];
    timer->id = app->next_timer++;
    timer->interval_ms = interval_ms;
    timer->due_ms = pnl_term_now_ms() + interval_ms;
    timer->fn = timer_fn;
    timer->ctx = timer_ctx;
    if (out_timer != NULL)
        *out_timer = timer->id;
    return PNL_OK;
}

int pnl_app_remove_timer(pnl_app_t *app, int timer)
{
    size_t i;

    if (app == NULL)
        return PNL_ERR_ARG;
    for (i = 0; i < app->timers_len; i++) {
        if (app->timers[i].id != timer)
            continue;
        memmove(&app->timers[i], &app->timers[i + 1], (app->timers_len - i - 1) * sizeof(*app->timers));
        app->timers_len--;
        return PNL_OK;
    }
    return PNL_ERR_NOT_FOUND;
}

int pnl_app_motion(const pnl_app_t *app)
{
    return app != NULL && app->motion;
}

long long pnl_app_frame_ms(const pnl_app_t *app)
{
    if (app == NULL)
        return 0;
    return app->frame_ms != 0 ? app->frame_ms : pnl_term_now_ms();
}

int pnl_app_enable_motion(pnl_app_t *app, int flags)
{
    if (app == NULL || (flags & ~PNL_MOTION_ALL) != 0)
        return PNL_ERR_ARG;
    app->motion_flags |= flags;
    return PNL_OK;
}

int pnl_motion_on(const pnl_app_t *app, int flag)
{
    return app->motion && (app->motion_flags & flag) != 0;
}

double pnl_glide_at(pnl_app_t *app, const struct pnl_glide *glide, int flag)
{
    double t;

    if (!pnl_motion_on(app, flag))
        return glide->to;
    t = pnl_app_tween(app, glide->start_ms, PNL_GLIDE_MS);
    t = 1.0 - (1.0 - t) * (1.0 - t);
    return glide->from + (glide->to - glide->from) * t;
}

void pnl_glide_to(pnl_app_t *app, struct pnl_glide *glide, double target, int flag)
{
    if (glide->to == target)
        return;
    glide->from = pnl_motion_on(app, flag) ? pnl_glide_at(app, glide, flag) : target;
    glide->to = target;
    glide->start_ms = pnl_term_now_ms();
}

int pnl_app_animate(pnl_app_t *app, int duration_ms)
{
    long long until;

    if (app == NULL || duration_ms < 0)
        return PNL_ERR_ARG;
    if (!app->motion)
        return PNL_OK;
    until = pnl_term_now_ms() + duration_ms;
    if (until > app->animate_until_ms)
        app->animate_until_ms = until;
    return PNL_OK;
}

double pnl_ease_out(double t)
{
    return 1.0 - (1.0 - t) * (1.0 - t);
}

double pnl_app_tween(pnl_app_t *app, long long start_ms, int duration_ms)
{
    long long elapsed;

    if (app == NULL || !app->motion || duration_ms <= 0)
        return 1.0;
    elapsed = pnl_app_frame_ms(app) - start_ms;
    if (elapsed >= duration_ms)
        return 1.0;
    pnl_app_animate(app, (int)(duration_ms - elapsed));
    return elapsed <= 0 ? 0.0 : (double)elapsed / duration_ms;
}

static long long pnl_frame_wait(const pnl_app_t *app, long long now)
{
    long long next_frame;

    if (app->animate_until_ms <= now)
        return app->tasks_running > 0 ? PNL_TASK_TICK_MS : -1;
    next_frame = app->frame_ms + PNL_FRAME_MS - now;
    return next_frame < 0 ? 0 : next_frame;
}

int pnl_timers_timeout(const pnl_app_t *app)
{
    long long now = pnl_term_now_ms();
    long long next = pnl_frame_wait(app, now);
    size_t i;

    if (app->toasts_len > 0 && (next < 0 || pnl_toasts_next_ms(app) - now < next))
        next = pnl_toasts_next_ms(app) > now ? pnl_toasts_next_ms(app) - now : 0;
    for (i = 0; i < app->timers_len; i++) {
        long long wait = app->timers[i].due_ms - now;

        if (next < 0 || wait < next)
            next = wait < 0 ? 0 : wait;
    }
    return next > 0x7fffffff ? 0x7fffffff : (int)next;
}

void pnl_timers_run(pnl_app_t *app)
{
    long long now = pnl_term_now_ms();
    size_t i;

    for (i = 0; i < app->timers_len; i++) {
        struct pnl_timer *timer = &app->timers[i];
        pnl_timer_fn fn = timer->fn;
        void *ctx = timer->ctx;
        int id = timer->id;

        if (timer->due_ms > now)
            continue;
        timer->due_ms += timer->interval_ms;
        if (timer->due_ms <= now)
            timer->due_ms = now + timer->interval_ms;
        app->dirty = 1;
        pnl_inspect_event(app, PNL_EVENT_TIMER, timer->id);
        if (fn != NULL)
            fn(ctx, app);
        if (i >= app->timers_len || app->timers[i].id != id)
            i = (size_t)-1;
    }
}

#if defined(_WIN32)
int pnl_app_watch_fd(pnl_app_t *app, int fd, pnl_fd_fn fd_fn, void *fd_ctx)
{
    (void)fd_ctx;
    if (app == NULL || fd < 0 || fd_fn == NULL)
        return PNL_ERR_ARG;
    return PNL_ERR_UNSUPPORTED;
}
#else
static size_t pnl_watch_index(const pnl_app_t *app, int fd)
{
    size_t i;

    for (i = 0; i < app->watches_len; i++) {
        if (app->watches[i].fd == fd)
            break;
    }
    return i;
}

int pnl_app_watch_fd(pnl_app_t *app, int fd, pnl_fd_fn fd_fn, void *fd_ctx)
{
    struct pnl_watch *watch;
    size_t i;
    int status;

    if (app == NULL || fd < 0 || fd_fn == NULL)
        return PNL_ERR_ARG;
    i = pnl_watch_index(app, fd);
    if (i == app->watches_len && app->watches_len == PNL_WATCH_MAX)
        return PNL_ERR_LIMIT;
    status = pnl_mem_grow(&app->mem, (void **)&app->watches, &app->watches_cap, i + 1, sizeof(*watch));
    if (status != PNL_OK)
        return status;
    watch = &app->watches[i];
    watch->fd = fd;
    watch->fn = fd_fn;
    watch->ctx = fd_ctx;
    if (i == app->watches_len)
        app->watches_len++;
    return PNL_OK;
}
#endif

int pnl_app_unwatch_fd(pnl_app_t *app, int fd)
{
    size_t i;

    if (app == NULL)
        return PNL_ERR_ARG;
    for (i = 0; i < app->watches_len; i++) {
        if (app->watches[i].fd != fd)
            continue;
        memmove(&app->watches[i], &app->watches[i + 1], (app->watches_len - i - 1) * sizeof(*app->watches));
        app->watches_len--;
        return PNL_OK;
    }
    return PNL_ERR_NOT_FOUND;
}

int pnl_app_redraw(pnl_app_t *app)
{
    if (app == NULL)
        return PNL_ERR_ARG;
    app->dirty = 1;
    return PNL_OK;
}

int pnl_app_quit(pnl_app_t *app)
{
    if (app == NULL)
        return PNL_ERR_ARG;
    app->quit = 1;
    return PNL_OK;
}

int pnl_app_with_terminal(pnl_app_t *app, pnl_terminal_fn terminal_fn, void *terminal_ctx)
{
    int status;

    if (app == NULL || terminal_fn == NULL)
        return PNL_ERR_ARG;
    if (!app->term.active) {
        terminal_fn(terminal_ctx);
        return PNL_OK;
    }
    pnl_term_leave(&app->term);
    terminal_fn(terminal_ctx);
    status = pnl_term_enter(&app->term);
    app->screen.front_valid = 0;
    return status;
}

static void pnl_draw_title(pnl_app_t *app, const struct pnl_view *top)
{
    int x = pnl_screen_text(&app->screen, 0, 0, -1, PNL_TITLE_STYLE, app->title);

    if (top == NULL || top->title == NULL)
        return;
    x = pnl_screen_text(&app->screen, x, 0, -1, PNL_FOOTER_STYLE, " / ");
    pnl_screen_text(&app->screen, x, 0, -1, PNL_TITLE_STYLE, top->title);
}

static void pnl_draw_footer(pnl_app_t *app, const char *footer, int in_dialog)
{
    const char *more = in_dialog ? "" : "?: help";
    int room = app->screen.width - pnl_text_width(more) - 2;
    const char *at = footer;
    const char *cut;
    size_t keep = 0;
    char text[512];

    if (pnl_text_width(footer) <= app->screen.width) {
        pnl_screen_text(&app->screen, 0, app->screen.height - 1, -1, PNL_FOOTER_STYLE, footer);
        return;
    }
    while ((cut = (strstr)(at, "  ")) != NULL && pnl_text_width_n(footer, (size_t)(cut - footer)) <= room) {
        keep = (size_t)(cut - footer);
        at = cut + 2;
    }
    snprintf(text, sizeof(text), "%.*s%s%s", (int)keep, footer, keep > 0 && more[0] ? "  " : "", more);
    pnl_screen_text(&app->screen, 0, app->screen.height - 1, -1, PNL_FOOTER_STYLE, text);
}

static size_t pnl_base_view(const pnl_app_t *app)
{
    size_t base = app->views_len;

    while (base > 0 && app->views[base - 1].dialog_width > 0)
        base--;
    return base;
}

static void pnl_draw_views(pnl_app_t *app, pnl_canvas_t *canvas)
{
    size_t base = pnl_base_view(app);
    size_t i;

    if (base > 0) {
        app->views[base - 1].origin_x = canvas->x;
        app->views[base - 1].origin_y = canvas->y;
    }
    if (base > 0)
        app->views[base - 1].draw_fn(app->views[base - 1].ctx, app, canvas);
    else
        pnl_menu_draw(app, canvas);
    pnl_ghost_draw(app);
    for (i = base; i < app->views_len; i++)
        pnl_dialog_draw(app, canvas, &app->views[i], i + 1 == app->views_len);
}

static int pnl_draw(pnl_app_t *app)
{
    size_t base = pnl_base_view(app);
    const struct pnl_view *top;
    const char *footer = PNL_MENU_FOOTER;
    struct pnl_canvas canvas;
    long long started = pnl_clock_us();
    int width;
    int height;
    int status;

    pnl_term_size(&width, &height);
    status = pnl_screen_resize(&app->screen, &app->mem, width, height);
    if (status != PNL_OK)
        return status;
    pnl_screen_clear(&app->screen);
    pnl_draw_title(app, base > 0 ? &app->views[base - 1] : NULL);
    app->frame_ms = pnl_term_now_ms();
    app->screen.spin = app->motion ? (int)(app->frame_ms / PNL_SPIN_MS % 1000) : -1;
    app->screen.spinning = 0;
    app->screen.pulse = pnl_pulse_step(app);
    canvas.screen = &app->screen;
    app->widget_typing = 0;
    canvas.focused = base == app->views_len;
    canvas.x = pnl_transition_offset(app) + (base == app->views_len ? pnl_shake_offset(app) : 0);
    canvas.y = 2;
    canvas.width = width;
    canvas.height = height > 4 ? height - 4 : 0;
    pnl_draw_views(app, &canvas);
    top = app->views_len ? &app->views[app->views_len - 1] : NULL;
    if (top != NULL)
        footer = pnl_view_hints(app);
    pnl_draw_footer(app, footer, top != NULL && top->dialog_width > 0);
    pnl_toasts_draw(app);
    app->dirty = 0;
    app->anim_frame = 0;
    if (app->screen.spinning)
        pnl_app_animate(app, PNL_SPIN_MS);
    status = pnl_screen_render(&app->screen, &app->mem);
    pnl_inspect_frame(app, pnl_clock_us() - started, app->screen.out_len);
    return status;
}

int pnl_is_mouse_key(int key)
{
    return key == PNL_KEY_CLICK || key == PNL_KEY_WHEEL_UP || key == PNL_KEY_WHEEL_DOWN;
}

int pnl_mouse_position(const pnl_app_t *app, int *out_x, int *out_y)
{
    if (app == NULL || out_x == NULL || out_y == NULL)
        return PNL_ERR_ARG;
    *out_x = app->term.mouse_x - app->mouse_ox;
    *out_y = app->term.mouse_y - app->mouse_oy;
    return PNL_OK;
}

void pnl_wheel_keys(pnl_app_t *app, pnl_key_fn key_fn, void *ctx, int key)
{
    size_t depth = app->views_len;
    int i;

    for (i = 0; i < 3 && key != PNL_KEY_CLICK && app->views_len == depth; i++)
        key_fn(ctx, app, key == PNL_KEY_WHEEL_UP ? PNL_KEY_UP : PNL_KEY_DOWN);
}

static int pnl_outside_dialog(const pnl_app_t *app, const struct pnl_view *view)
{
    int x = app->term.mouse_x;
    int y = app->term.mouse_y;

    return view->dialog_width > 0 && (x < view->box_x || y < view->box_y || x >= view->box_x + view->box_width ||
                                      y >= view->box_y + view->box_height);
}

static void pnl_dispatch_mouse(pnl_app_t *app, int key)
{
    struct pnl_view top;

    if (app->views_len == 0) {
        pnl_menu_mouse(app, key);
        return;
    }
    top = app->views[app->views_len - 1];
    if (top.key_fn == NULL || pnl_outside_dialog(app, &top))
        return;
    app->mouse_ox = top.origin_x;
    app->mouse_oy = top.origin_y;
    if (!top.key_fn(top.ctx, app, key))
        pnl_wheel_keys(app, top.key_fn, top.ctx, key);
}

static void pnl_dispatch_key(pnl_app_t *app, int key)
{
    struct pnl_view top;

    if (key == PNL_KEY_FOCUS_IN || key == PNL_KEY_FOCUS_OUT) {
        app->term.focus_known = 1;
        app->term.focused = key == PNL_KEY_FOCUS_IN;
        return;
    }
    key = pnl_keymap_apply(app, key);
    if (key == PNL_KEY_NONE)
        return;
    pnl_inspect_event(app, PNL_EVENT_KEY, key);
    app->dirty = 1;
    if (key == PNL_KEY_INSPECT) {
        pnl_inspect_toggle(app);
        return;
    }
    if (key == PNL_KEY_CTRL_C) {
        app->quit = 1;
        return;
    }
    if (pnl_is_mouse_key(key)) {
        pnl_dispatch_mouse(app, key);
        return;
    }
    if (app->views_len == 0 && pnl_menu_key(app, key))
        return;
    if (app->views_len > 0) {
        top = app->views[app->views_len - 1];
        if (top.key_fn != NULL && top.key_fn(top.ctx, app, key))
            return;
    }
    if (key == '?')
        pnl_help(app);
    else if (key == ':')
        pnl_palette(app);
    else if (app->views_len > 0 && (key == 'q' || key == PNL_KEY_ESCAPE))
        pnl_view_pop(app);
}

static int pnl_read_keys(pnl_app_t *app)
{
    int status = PNL_OK;

    while (status == PNL_OK && !app->quit) {
        int key = PNL_KEY_NONE;

        status = pnl_term_read_key(&app->term, 0, &key);
        if (status != PNL_OK || key == PNL_KEY_NONE)
            break;
        if (key == PNL_KEY_SUSPEND) {
            pnl_term_suspend(&app->term);
            app->screen.front_valid = 0;
            continue;
        }
        pnl_dispatch_key(app, key);
    }
    return status;
}

static int pnl_watch_alive(const pnl_app_t *app, const struct pnl_watch *watch)
{
    size_t i;

    for (i = 0; i < app->watches_len; i++) {
        if (app->watches[i].fd == watch->fd && app->watches[i].fn == watch->fn && app->watches[i].ctx == watch->ctx)
            return 1;
    }
    return 0;
}

static void pnl_dispatch_watches(pnl_app_t *app, const unsigned char *ready)
{
    struct pnl_watch watches[PNL_WATCH_MAX];
    size_t count = app->watches_len;
    size_t i;

    if (count == 0)
        return;
    memcpy(watches, app->watches, count * sizeof(*watches));
    for (i = 0; i < count && !app->quit; i++) {
        if (!ready[i])
            continue;
        if (!pnl_watch_alive(app, &watches[i]))
            continue;
        app->dirty = 1;
        pnl_inspect_event(app, PNL_EVENT_WATCH, watches[i].fd);
        watches[i].fn(watches[i].ctx, app, watches[i].fd);
    }
}

int pnl_loop(pnl_app_t *app, size_t min_views)
{
    unsigned char ready[PNL_WATCH_MAX];
    int status = PNL_OK;

    while (status == PNL_OK && !app->quit && app->views_len >= min_views) {
        int input;

        if (pnl_term_resized()) {
            app->screen.front_valid = 0;
            pnl_inspect_event(app, PNL_EVENT_RESIZE, 0);
        }
        if (app->dirty || !app->screen.front_valid)
            status = pnl_draw(app);
        if (status != PNL_OK)
            break;
        memset(ready, 0, sizeof(ready));
        input = pnl_term_wait(&app->term, app->watches, app->watches_len, pnl_timers_timeout(app), ready);
        if (input < 0)
            return PNL_ERR_IO;
        if (app->term.woken) {
            app->term.woken = 0;
            app->dirty = 1;
            pnl_inspect_event(app, PNL_EVENT_WAKE, 0);
        }
        if (input > 0)
            status = pnl_read_keys(app);
        pnl_dispatch_watches(app, ready);
        pnl_timers_run(app);
        pnl_toasts_expire(app);
        if (app->tasks_running > 0)
            app->dirty = 1;
        if (!app->dirty && (app->animate_until_ms > pnl_term_now_ms() || app->animate_until_ms > app->frame_ms)) {
            app->dirty = 1;
            app->anim_frame = 1;
        }
    }
    return status;
}

static void pnl_timers_restart(pnl_app_t *app)
{
    long long now = pnl_term_now_ms();
    size_t i;

    for (i = 0; i < app->timers_len; i++)
        app->timers[i].due_ms = now + app->timers[i].interval_ms;
}

static int pnl_link_slow(pnl_app_t *app)
{
    const char *ssh = getenv("SSH_CONNECTION");

    if (ssh == NULL || ssh[0] == '\0')
        return 0;
    if (app->reply_ms < 0)
        app->reply_ms = pnl_link_probe(&app->term);
    return app->reply_ms < 0 || app->reply_ms > PNL_SLOW_LINK_MS;
}

static int pnl_motion_detect(pnl_app_t *app)
{
    const char *motion = getenv("TUIKIT_MOTION");

    if (motion != NULL && strcmp(motion, "all") == 0)
        app->motion_flags = PNL_MOTION_ALL;
    if (pnl_env_off("TUIKIT_MOTION"))
        return 0;
    if (motion != NULL && (strcmp(motion, "on") == 0 || strcmp(motion, "all") == 0))
        return 1;
    return !pnl_link_slow(app);
}

static void pnl_run_palette(pnl_app_t *app)
{
    struct pnl_theme theme = app->theme;
    const char *chosen = getenv("TUIKIT_THEME");
    const char *contrast = getenv("TUIKIT_CONTRAST");
    int depth = pnl_color_detect();
    long background = -1;
    int known = !pnl_virtual_active() && !app->term.linear && pnl_background_detect(&app->term, depth, &background, &app->reply_ms);

    if (chosen != NULL && chosen[0] != '\0')
        pnl_theme_choose(&theme, chosen);
    pnl_palette_init(&app->screen.palette, &theme, depth, known && pnl_is_light(background), known ? background : -1,
                     contrast != NULL && strcmp(contrast, "high") == 0);
}

static size_t pnl_remembered_action(pnl_app_t *app)
{
    const char *label = pnl_memory_get(app, "menu", "action");
    size_t i;

    for (i = 0; label != NULL && i < app->actions_len; i++) {
        if (strcmp(app->actions[i].label, label) == 0)
            return i;
    }
    return 0;
}

static void pnl_remember_action(pnl_app_t *app)
{
    if (app->selected < app->actions_len)
        pnl_memory_set(app, "menu", "action", app->actions[app->selected].label);
}

int pnl_run_interactive(pnl_app_t *app)
{
    const char *linear = getenv("TUIKIT_LINEAR");
    int status;

    app->term.linear = linear != NULL && linear[0] != '\0' && !pnl_env_off("TUIKIT_LINEAR");
    app->screen.linear = app->term.linear && !pnl_virtual_active();
    app->screen.links_off = pnl_env_off("TUIKIT_LINKS");
    app->term.mouse = !pnl_virtual_active() && !app->term.linear && !pnl_env_off("TUIKIT_MOUSE");
    status = pnl_term_enter(&app->term);

    if (status != PNL_OK)
        return status;
    app->running = 1;
    app->cascade_ms = pnl_term_now_ms();
    app->quit = 0;
    if (!pnl_virtual_active()) {
        pnl_memory_load(app);
        pnl_keymap_load(app);
    }
    app->selected = pnl_remembered_action(app);
    app->menu_top = 0;
    pnl_run_palette(app);
    app->motion = pnl_virtual_active() || app->term.linear ? 0 : pnl_motion_detect(app);
    app->select_glide.from = app->select_glide.to = (double)app->selected;
    app->transition_shift = 0;
    app->seen_len = 0;
    app->seen_ready = 0;
    app->animate_until_ms = 0;
    app->dirty = 1;
    app->screen.front_valid = 0;
    pnl_timers_restart(app);
    pnl_inspect_start(app);
    pnl_record_open(app);
    status = pnl_loop(app, 0);
    pnl_record_close();
    pnl_remember_action(app);
    pnl_views_drop(app);
    if (!pnl_virtual_active())
        pnl_memory_save(app);
    app->running = 0;
    app->motion = 0;
    app->frame_ms = 0;
    pnl_term_leave(&app->term);
    pnl_views_drop(app);
    pnl_menu_clear_lines(app);
    return status;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <direct.h>
#define pnl_mkdir(path) _mkdir(path)
#define PNL_SEPARATOR '\\'
#else
#include <sys/stat.h>
#define pnl_mkdir(path) mkdir(path, 0700)
#define PNL_SEPARATOR '/'
#endif

#define PNL_MEMORY_FILE_MAX (1024 * 1024)
#define PNL_MEMORY_KEY_MAX 512
#define PNL_MEMORY_PATH_MAX 1024

static size_t pnl_memory_find(const pnl_app_t *app, const char *key)
{
    size_t i;

    for (i = 0; i < app->memos_len; i++) {
        if (strcmp(app->memos[i].key, key) == 0)
            return i;
    }
    return i;
}

static int pnl_memory_put(pnl_app_t *app, const char *key, const char *value)
{
    size_t i = pnl_memory_find(app, key);
    char *copy = pnl_mem_copy(&app->mem, value);

    if (copy == NULL)
        return PNL_ERR_NOMEM;
    if (i < app->memos_len) {
        pnl_mem_free(&app->mem, app->memos[i].value);
        app->memos[i].value = copy;
        return PNL_OK;
    }
    if (pnl_mem_grow(&app->mem, (void **)&app->memos, &app->memos_cap, app->memos_len + 1,
                     sizeof(*app->memos)) != PNL_OK ||
        (app->memos[i].key = pnl_mem_copy(&app->mem, key)) == NULL) {
        pnl_mem_free(&app->mem, copy);
        return PNL_ERR_NOMEM;
    }
    app->memos[i].value = copy;
    app->memos_len++;
    return PNL_OK;
}

static void pnl_memory_key(char *key, const char *kind, const char *name)
{
    snprintf(key, PNL_MEMORY_KEY_MAX, "%s:%s", kind, name);
}

const char *pnl_memory_get(const pnl_app_t *app, const char *kind, const char *name)
{
    char key[PNL_MEMORY_KEY_MAX];
    size_t i;

    if (app->memory_path == NULL)
        return NULL;
    pnl_memory_key(key, kind, name);
    i = pnl_memory_find(app, key);
    return i < app->memos_len ? app->memos[i].value : NULL;
}

void pnl_memory_set(pnl_app_t *app, const char *kind, const char *name, const char *value)
{
    char key[PNL_MEMORY_KEY_MAX];
    const char *old = pnl_memory_get(app, kind, name);

    if (app->memory_path == NULL || (old != NULL && strcmp(old, value) == 0))
        return;
    pnl_memory_key(key, kind, name);
    if (pnl_memory_put(app, key, value) == PNL_OK)
        app->memos_dirty = 1;
}

static void pnl_memory_unescape(char *text)
{
    char *out = text;

    for (; *text != '\0'; text++) {
        if (*text == '\\' && text[1] != '\0') {
            text++;
            *out++ = *text == 'n' ? '\n' : *text == 't' ? '\t' : *text;
        } else {
            *out++ = *text;
        }
    }
    *out = '\0';
}

void pnl_memory_parse(pnl_app_t *app, char *data)
{
    char *line = data;

    while (line != NULL && *line != '\0') {
        char *next = (strchr)(line, '\n');
        char *tab;

        if (next != NULL)
            *next++ = '\0';
        tab = (strchr)(line, '\t');
        if (tab != NULL) {
            *tab = '\0';
            pnl_memory_unescape(line);
            pnl_memory_unescape(tab + 1);
            pnl_memory_put(app, line, tab + 1);
        }
        line = next;
    }
}

void pnl_memory_load(pnl_app_t *app)
{
    FILE *file;
    char *data;
    size_t len;

    if (app->memory_path == NULL || app->memos_len > 0)
        return;
    file = fopen(app->memory_path, "rb");
    if (file == NULL)
        return;
    data = pnl_mem_alloc(&app->mem, PNL_MEMORY_FILE_MAX + 1);
    if (data != NULL) {
        len = fread(data, 1, PNL_MEMORY_FILE_MAX, file);
        data[len] = '\0';
        pnl_memory_parse(app, data);
    }
    pnl_mem_free(&app->mem, data);
    fclose(file);
}

static void pnl_memory_write(FILE *file, const char *text)
{
    for (; *text != '\0'; text++) {
        if (*text == '\\' || *text == '\n' || *text == '\t')
            fputc('\\', file);
        fputc(*text == '\n' ? 'n' : *text == '\t' ? 't' : *text, file);
    }
}

static void pnl_memory_mkdirs(const char *path)
{
    char dir[PNL_MEMORY_PATH_MAX];
    size_t i;

    snprintf(dir, sizeof(dir), "%s", path);
    for (i = 1; dir[i] != '\0'; i++) {
        if (dir[i] != '/' && dir[i] != PNL_SEPARATOR)
            continue;
        dir[i] = '\0';
        pnl_mkdir(dir);
        dir[i] = PNL_SEPARATOR;
    }
}

void pnl_memory_save(pnl_app_t *app)
{
    char temp[PNL_MEMORY_PATH_MAX];
    FILE *file;
    size_t i;
    int ok;

    if (app->memory_path == NULL || !app->memos_dirty)
        return;
    pnl_memory_mkdirs(app->memory_path);
    snprintf(temp, sizeof(temp), "%s.new", app->memory_path);
    file = fopen(temp, "wb");
    if (file == NULL)
        return;
    for (i = 0; i < app->memos_len; i++) {
        pnl_memory_write(file, app->memos[i].key);
        fputc('\t', file);
        pnl_memory_write(file, app->memos[i].value);
        fputc('\n', file);
    }
    ok = fclose(file) == 0;
#if defined(_WIN32)
    remove(app->memory_path);
#endif
    if (ok && rename(temp, app->memory_path) == 0)
        app->memos_dirty = 0;
    else
        remove(temp);
}

void pnl_memory_free(pnl_app_t *app)
{
    size_t i;

    for (i = 0; i < app->memos_len; i++) {
        pnl_mem_free(&app->mem, app->memos[i].key);
        pnl_mem_free(&app->mem, app->memos[i].value);
    }
    pnl_mem_free(&app->mem, app->memos);
    pnl_mem_free(&app->mem, app->memory_path);
    app->memos = NULL;
    app->memos_len = 0;
    app->memos_cap = 0;
    app->memory_path = NULL;
}

void pnl_tool_name(char *out, size_t out_len, const char *title)
{
    size_t i = 0;

    for (; *title != '\0' && i + 1 < out_len; title++) {
        char c = *title;

        if (c >= 'A' && c <= 'Z')
            c = (char)(c - 'A' + 'a');
        out[i++] = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '_' ? c : '-';
    }
    out[i] = '\0';
}

static int pnl_memory_default(const pnl_app_t *app, char *path)
{
    char name[128];
    const char *base;
    const char *home;

    pnl_tool_name(name, sizeof(name), app->title);
#if defined(_WIN32)
    base = getenv("LOCALAPPDATA");
    home = NULL;
    if (base == NULL || base[0] == '\0')
        return 0;
    snprintf(path, PNL_MEMORY_PATH_MAX, "%s\\%s\\memory", base, name);
#else
    base = getenv("XDG_STATE_HOME");
    home = getenv("HOME");
    if (base != NULL && base[0] == '/')
        snprintf(path, PNL_MEMORY_PATH_MAX, "%s/%s/memory", base, name);
    else if (home != NULL && home[0] != '\0')
        snprintf(path, PNL_MEMORY_PATH_MAX, "%s/.local/state/%s/memory", home, name);
    else
        return 0;
#endif
    (void)home;
    return 1;
}

int pnl_app_set_memory(pnl_app_t *app, const char *path)
{
    char fallback[PNL_MEMORY_PATH_MAX];
    char *copy;

    if (app == NULL || (path != NULL && (path[0] == '\0' || strlen(path) + 5 >= PNL_MEMORY_PATH_MAX)))
        return PNL_ERR_ARG;
    pnl_memory_free(app);
    if (pnl_env_off("TUIKIT_MEMORY"))
        return PNL_OK;
    if (path == NULL && !pnl_memory_default(app, fallback))
        return PNL_OK;
    copy = pnl_mem_copy(&app->mem, path != NULL ? path : fallback);
    if (copy == NULL)
        return PNL_ERR_NOMEM;
    app->memory_path = copy;
    return PNL_OK;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_QUIT_LABEL "Quit"
#define PNL_RESULT_FOOTER "any key: back"
#define PNL_SELECTED_STYLE PNL_BOLD
#define PNL_LABEL_MAX 24

struct pnl_result_view {
    int mark;
    long long shown_ms;
    char *text;
};

static const int pnl_mark_roles[] = {PNL_ROLE_OK, PNL_ROLE_WARN, PNL_ROLE_FAIL};

void pnl_menu_clear_lines(pnl_app_t *app)
{
    size_t i;

    for (i = 0; i < app->lines_len; i++) {
        pnl_mem_free(&app->mem, app->lines[i].label);
        pnl_mem_free(&app->mem, app->lines[i].text);
    }
    app->lines_len = 0;
}

int pnl_valid_mark(int mark)
{
    return mark == PNL_MARK_OK || mark == PNL_MARK_WAIT || mark == PNL_MARK_FAIL;
}

int pnl_label_width(const pnl_app_t *app)
{
    int width = 0;
    size_t i;

    for (i = 0; i < app->lines_len; i++) {
        int w = app->lines[i].label != NULL ? pnl_text_width(app->lines[i].label) : 0;

        if (w > width)
            width = w;
    }
    return width < PNL_LABEL_MAX ? width : PNL_LABEL_MAX;
}

int pnl_status_line(pnl_app_t *app, int mark, const char *label, const char *text)
{
    struct pnl_line *line;
    int status;

    if (app == NULL || text == NULL || !app->in_status || !pnl_valid_mark(mark))
        return PNL_ERR_ARG;
    status = pnl_mem_grow(&app->mem, (void **)&app->lines, &app->lines_cap, app->lines_len + 1, sizeof(*line));
    if (status != PNL_OK)
        return status;
    line = &app->lines[app->lines_len];
    line->text = pnl_mem_copy(&app->mem, text);
    line->label = label != NULL && label[0] != '\0' ? pnl_mem_copy(&app->mem, label) : NULL;
    if (line->text == NULL || (line->label == NULL && label != NULL && label[0] != '\0')) {
        pnl_mem_free(&app->mem, line->text);
        pnl_mem_free(&app->mem, line->label);
        return PNL_ERR_NOMEM;
    }
    line->mark = mark;
    line->meter = -1.0;
    app->lines_len++;
    return PNL_OK;
}

void pnl_draw_mark(pnl_canvas_t *canvas, int y, int mark, const char *text)
{
    static const char *const marks[] = {"ok", "..", "!!"};
    if (mark == PNL_MARK_WAIT && canvas->screen->spin >= 0) {
        pnl_draw_spinner(canvas, 2, y, PNL_FG(pnl_mark_roles[mark]));
    } else if (mark == PNL_MARK_FAIL && canvas->screen->pulse >= 0) {
        canvas->screen->spinning = 1;
        pnl_canvas_text(canvas, 2, y, PNL_FG(PNL_FADE_FIRST + (PNL_ROLE_FAIL - PNL_ROLE_ACCENT) * PNL_FADE_STEPS +
                                             canvas->screen->pulse),
                        marks[mark]);
    } else
        pnl_canvas_text(canvas, 2, y, PNL_FG(pnl_mark_roles[mark]), marks[mark]);
    pnl_canvas_text(canvas, 6, y, 0, text);
}

#define PNL_METER_WIDTH 12

static void pnl_draw_line(pnl_canvas_t *canvas, int y, const struct pnl_line *line, int label_width,
                          int highlight)
{
    char percent[8];
    int x = line->label != NULL ? 6 + label_width + 2 : 6;

    pnl_draw_mark(canvas, y, line->mark, "");
    if (line->label != NULL)
        pnl_canvas_text(canvas, 6, y, 0, line->label);
    if (line->label != NULL && pnl_text_width(line->label) > label_width)
        pnl_canvas_text(canvas, 6 + label_width - 1, y, PNL_FG(PNL_ROLE_MUTED), "\xe2\x80\xa6");
    pnl_canvas_text(canvas, x, y, highlight, line->text);
    if (line->meter < 0.0)
        return;
    if (line->text[0] != '\0')
        x += pnl_text_width(line->text) + 2;
    pnl_canvas_bar(canvas, x, y, PNL_METER_WIDTH, line->meter, PNL_METER_LEVEL);
    snprintf(percent, sizeof(percent), "%3d%%", (int)(line->meter * 100 + 0.5));
    pnl_canvas_text(canvas, x + PNL_METER_WIDTH + 1, y, PNL_FG(PNL_ROLE_MUTED), percent);
}

static unsigned long pnl_hash(unsigned long hash, const char *text)
{
    const unsigned char *p;

    for (p = (const unsigned char *)text; *p != '\0'; p++)
        hash = hash * 33UL + *p;
    return hash;
}

static unsigned long pnl_line_id(const struct pnl_line *line, size_t index)
{
    return line->label != NULL ? pnl_hash(5381UL, line->label) : 5381UL * 31UL + (unsigned long)index;
}

static const struct pnl_seen *pnl_seen_find(const pnl_app_t *app, unsigned long id)
{
    size_t i;

    for (i = 0; i < app->seen_len; i++) {
        if (app->seen[i].id == id)
            return &app->seen[i];
    }
    return NULL;
}

static const char *pnl_number_at(const char *text, double *number)
{
    const char *at = text;
    char *end;

    while (*at != '\0' && !(*at >= '0' && *at <= '9'))
        at++;
    if (*at == '\0')
        return NULL;
    if (at > text && at[-1] == '-')
        at--;
    *number = strtod(at, &end);
    return end > at ? at : NULL;
}

static double pnl_count_shown(pnl_app_t *app, const struct pnl_seen *seen)
{
    double t = seen->count_ms != 0 ? pnl_ease_out(pnl_app_tween(app, seen->count_ms, PNL_COUNT_MS)) : 1.0;

    return seen->count_from + (seen->count_to - seen->count_from) * t;
}

static double pnl_meter_shown(pnl_app_t *app, const struct pnl_seen *seen)
{
    double t = seen->meter_ms != 0 ? pnl_ease_out(pnl_app_tween(app, seen->meter_ms, PNL_COUNT_MS)) : 1.0;

    return seen->meter_from + (seen->meter_to - seen->meter_from) * t;
}

static unsigned long pnl_prefix_hash(const char *text, const char *end)
{
    unsigned long hash = 5381UL + (unsigned long)(end - text);

    for (; text < end; text++)
        hash = hash * 33UL + (unsigned char)*text;
    return hash | 1UL;
}

static void pnl_track_count(pnl_app_t *app, struct pnl_seen *next, const struct pnl_seen *old, const char *text)
{
    double number = 0.0;
    const char *at = pnl_number_at(text, &number);

    next->count_to = number;
    next->count_from = number;
    next->count_ms = 0;
    next->count_prefix = at != NULL ? pnl_prefix_hash(text, at) : 0;
    if (at == NULL || old == NULL || old->count_prefix != next->count_prefix)
        return;
    next->count_from = pnl_count_shown(app, old);
    next->count_ms = pnl_app_frame_ms(app);
}

static void pnl_track_meter(pnl_app_t *app, struct pnl_seen *next, const struct pnl_seen *old, double meter)
{
    next->meter_to = meter;
    next->meter_from = meter;
    next->meter_ms = 0;
    if (old == NULL || old->meter_to == meter || meter < 0.0 || old->meter_to < 0.0)
        return;
    next->meter_from = pnl_meter_shown(app, old);
    next->meter_ms = pnl_app_frame_ms(app);
}

static void pnl_track_motion(pnl_app_t *app, struct pnl_seen *next, const struct pnl_seen *old,
                             const struct pnl_line *line)
{
    if (old != NULL && old->hash == next->hash) {
        next->count_ms = old->count_ms;
        next->count_prefix = old->count_prefix;
        next->count_from = old->count_from;
        next->count_to = old->count_to;
    } else {
        pnl_track_count(app, next, old, line->text);
    }
    pnl_track_meter(app, next, old, line->meter);
    if (old != NULL && old->meter_to == line->meter) {
        next->meter_ms = old->meter_ms;
        next->meter_from = old->meter_from;
    }
}

static void pnl_menu_track(pnl_app_t *app)
{
    long long now = pnl_app_frame_ms(app);
    struct pnl_seen *swap;
    size_t cap;
    size_t i;

    if (pnl_mem_grow(&app->mem, (void **)&app->seen_next, &app->seen_next_cap, app->lines_len,
                     sizeof(*app->seen_next)) != PNL_OK)
        return;
    for (i = 0; i < app->lines_len; i++) {
        struct pnl_seen *next = &app->seen_next[i];
        const struct pnl_seen *old;

        next->id = pnl_line_id(&app->lines[i], i);
        next->hash = pnl_hash(5381UL + (unsigned long)app->lines[i].mark, app->lines[i].text);
        old = pnl_seen_find(app, next->id);
        if (old == NULL)
            next->changed_ms = app->seen_ready ? now : 0;
        else
            next->changed_ms = old->hash != next->hash ? now : old->changed_ms;
        pnl_track_motion(app, next, old, &app->lines[i]);
    }
    swap = app->seen;
    cap = app->seen_cap;
    app->seen = app->seen_next;
    app->seen_cap = app->seen_next_cap;
    app->seen_next = swap;
    app->seen_next_cap = cap;
    app->seen_len = app->lines_len;
    app->seen_ready = 1;
}

void pnl_menu_refresh(pnl_app_t *app)
{
    pnl_menu_clear_lines(app);
    if (app->status_fn != NULL) {
        app->in_status = 1;
        app->status_fn(app->status_ctx, app);
        app->in_status = 0;
    }
    pnl_values_lines(app);
    if (app->running)
        pnl_menu_track(app);
}

static void pnl_draw_item(pnl_app_t *app, pnl_canvas_t *canvas, int y, size_t index)
{
    const char *label = index < app->actions_len ? app->actions[index].label : PNL_QUIT_LABEL;
    int style = index == app->selected ? PNL_SELECTED_STYLE : 0;
    char number[2] = {0, 0};

    if (app->chosen_ms != 0 && index == app->chosen)
        style |= pnl_fade_style(app, canvas, PNL_ROLE_ACCENT, app->chosen_ms, 2);

    if (index < 9 && index < app->actions_len)
        number[0] = (char)('1' + index);
    else if (index == app->actions_len)
        number[0] = 'q';
    pnl_canvas_text(canvas, 4, y, PNL_FG(PNL_ROLE_MUTED), number);
    pnl_canvas_text(canvas, 6, y, style, label);
}

static void pnl_cascade_item(pnl_app_t *app, pnl_canvas_t *canvas, int y, size_t index)
{
    struct pnl_canvas row = *canvas;
    size_t step = index - app->menu_top < PNL_CASCADE_STEPS ? index - app->menu_top : PNL_CASCADE_STEPS;
    double t = 1.0;

    if (pnl_motion_on(app, PNL_MOTION_ENTER))
        t = pnl_app_tween(app, app->cascade_ms + (long long)step * PNL_CASCADE_STEP_MS, PNL_ENTER_MS);
    if (t <= 0.0)
        return;
    row.x += (int)((1.0 - pnl_ease_out(t)) * 8 + 0.5);
    pnl_draw_item(app, &row, y, index);
}

static void pnl_menu_scroll(pnl_app_t *app, size_t rows)
{
    if (rows == 0)
        rows = 1;
    if (app->selected < app->menu_top)
        app->menu_top = app->selected;
    if (app->selected >= app->menu_top + rows)
        app->menu_top = app->selected - rows + 1;
    if (app->menu_top + rows > app->actions_len + 1)
        app->menu_top = app->actions_len + 1 > rows ? app->actions_len + 1 - rows : 0;
}

static void pnl_count_text(pnl_app_t *app, const struct pnl_seen *seen, const char *text, char *out, size_t out_len)
{
    double number;
    const char *at = pnl_number_at(text, &number);
    char *end;
    const char *dot;
    int decimals = 0;

    strtod(at, &end);
    dot = (memchr)(at, '.', (size_t)(end - at));
    if (dot != NULL)
        decimals = (int)(end - dot - 1);
    snprintf(out, out_len, "%.*s%.*f%s", (int)(at - text), text, decimals, pnl_count_shown(app, seen), end);
}

static struct pnl_line pnl_shown_line(pnl_app_t *app, size_t i, char *text, size_t text_len)
{
    struct pnl_line line = app->lines[i];
    const struct pnl_seen *seen = i < app->seen_len ? &app->seen[i] : NULL;

    if (seen == NULL || !pnl_motion_on(app, PNL_MOTION_COUNT))
        return line;
    if (seen->count_ms != 0 && pnl_app_tween(app, seen->count_ms, PNL_COUNT_MS) < 1.0) {
        pnl_count_text(app, seen, line.text, text, text_len);
        line.text = text;
    }
    if (seen->meter_ms != 0)
        line.meter = pnl_meter_shown(app, seen);
    return line;
}

void pnl_menu_draw(pnl_app_t *app, pnl_canvas_t *canvas)
{
    size_t i;
    int y = 0;
    int first;
    int marker;
    int rows;
    int label_width;

    if (!app->anim_frame)
        pnl_menu_refresh(app);
    label_width = pnl_label_width(app);
    y = pnl_banner_draw(app, canvas, (int)(app->lines_len + app->actions_len) + 2);
    for (i = 0; i < app->lines_len; i++) {
        int highlight = i < app->seen_len && app->seen[i].changed_ms != 0
                            ? pnl_fade_style(app, canvas, PNL_ROLE_ACCENT, app->seen[i].changed_ms, 9)
                            : 0;

        char text[PNL_CELL_MAX];
        struct pnl_line line = pnl_shown_line(app, i, text, sizeof(text));

        pnl_draw_line(canvas, y++, &line, label_width, highlight);
    }
    if (app->lines_len > 0)
        y++;
    rows = pnl_canvas_height(canvas) - y;
    pnl_menu_scroll(app, rows > 0 ? (size_t)rows : 0);
    first = y;
    app->menu_y = canvas->y + first;
    for (i = app->menu_top; i <= app->actions_len && y < pnl_canvas_height(canvas); i++)
        pnl_cascade_item(app, canvas, y++, i);
    app->menu_end = canvas->y + y;
    pnl_glide_to(app, &app->select_glide, (double)app->selected, PNL_MOTION_SLIDE);
    marker = first + (int)(pnl_glide_at(app, &app->select_glide, PNL_MOTION_SLIDE) + 0.5) - (int)app->menu_top;
    if (marker >= first && marker < y)
        pnl_canvas_text(canvas, 2, marker, PNL_SELECTED_STYLE | PNL_FG(PNL_ROLE_ACCENT), ">");
    if (app->menu_top > 0)
        pnl_canvas_text(canvas, 0, y - rows, PNL_FG(PNL_ROLE_MUTED), "^");
    if (i <= app->actions_len)
        pnl_canvas_text(canvas, 0, y - 1, PNL_FG(PNL_ROLE_MUTED), "v");
}

static int pnl_menu_lower(int c)
{
    return c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
}

static int pnl_menu_jump(pnl_app_t *app, int key)
{
    size_t i;

    for (i = 1; i <= app->actions_len; i++) {
        size_t index = (app->selected + i) % (app->actions_len + 1);

        if (index < app->actions_len &&
            pnl_menu_lower((unsigned char)app->actions[index].label[0]) == pnl_menu_lower(key)) {
            app->selected = index;
            return 1;
        }
    }
    return 0;
}

static void pnl_menu_run(pnl_app_t *app, size_t index)
{
    app->selected = index;
    app->chosen = index;
    app->chosen_ms = pnl_motion_on(app, PNL_MOTION_FEEDBACK) ? pnl_term_now_ms() : 0;
    app->actions[index].fn(app->actions[index].ctx, app);
}

int pnl_menu_key(pnl_app_t *app, int key)
{
    size_t count = app->actions_len + 1;

    if (key >= '1' && key <= '9' && (size_t)(key - '1') < app->actions_len)
        pnl_menu_run(app, (size_t)(key - '1'));
    else if (key == PNL_KEY_HOME || key == PNL_KEY_PAGE_UP)
        app->selected = 0;
    else if (key == PNL_KEY_END || key == PNL_KEY_PAGE_DOWN)
        app->selected = app->actions_len;
    else if (key == PNL_KEY_UP || key == 'k')
        app->selected = (app->selected + count - 1) % count;
    else if (key == PNL_KEY_DOWN || key == 'j')
        app->selected = (app->selected + 1) % count;
    else if (key == 'q' || key == PNL_KEY_ESCAPE || (key == PNL_KEY_ENTER && app->selected == app->actions_len))
        app->quit = 1;
    else if (key == PNL_KEY_ENTER)
        pnl_menu_run(app, app->selected);
    else if (key < 0x80 && ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')))
        return pnl_menu_jump(app, key);
    else
        return 0;
    return 1;
}

int pnl_menu_mouse(pnl_app_t *app, int key)
{
    int y = app->term.mouse_y;

    if (key == PNL_KEY_WHEEL_UP && app->selected > 0)
        app->selected--;
    else if (key == PNL_KEY_WHEEL_DOWN && app->selected < app->actions_len)
        app->selected++;
    if (key != PNL_KEY_CLICK || y < app->menu_y || y >= app->menu_end)
        return 1;
    app->selected = app->menu_top + (size_t)(y - app->menu_y);
    return pnl_menu_key(app, PNL_KEY_ENTER);
}

static void pnl_result_type(pnl_app_t *app, pnl_canvas_t *canvas, const struct pnl_result_view *view)
{
    size_t len = strlen(view->text);
    size_t chars = 0;
    size_t shown;
    size_t cut;
    size_t i;
    double t;

    if (!pnl_motion_on(app, PNL_MOTION_TYPEWRITER))
        return;
    for (i = 0; i < len; i++)
        chars += ((unsigned char)view->text[i] & 0xc0) != 0x80;
    t = pnl_app_tween(app, view->shown_ms, chars * PNL_TYPE_CHAR_MS < PNL_TYPE_MAX_MS ? (int)chars * PNL_TYPE_CHAR_MS
                                                                                       : PNL_TYPE_MAX_MS);
    if (t >= 1.0)
        return;
    shown = (size_t)(t * (double)chars);
    for (cut = 0; cut < len; cut++) {
        if (((unsigned char)view->text[cut] & 0xc0) != 0x80 && shown-- == 0)
            break;
    }
    pnl_canvas_fill(canvas, 6 + pnl_text_width_n(view->text, cut), 0, pnl_canvas_width(canvas), 1, 0);
}

static void pnl_result_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_result_view *view = ctx;
    int flash;

    if (view->shown_ms == 0)
        view->shown_ms = pnl_app_frame_ms(app);
    pnl_draw_mark(canvas, 0, view->mark, view->text);
    pnl_result_type(app, canvas, view);
    flash = pnl_fade_style(app, canvas, pnl_mark_roles[view->mark], view->shown_ms, 4);
    if (flash != 0)
        pnl_canvas_text(canvas, 1, 0, flash | PNL_BOLD, view->mark == PNL_MARK_OK ? " ok " : view->mark == PNL_MARK_FAIL ? " !! " : " .. ");
}

static int pnl_result_key(void *ctx, pnl_app_t *app, int key)
{
    (void)ctx;
    if (key != PNL_KEY_WHEEL_UP && key != PNL_KEY_WHEEL_DOWN)
        pnl_view_pop(app);
    return 1;
}

static void pnl_result_drop(pnl_app_t *app, void *ctx)
{
    struct pnl_result_view *view = ctx;

    pnl_mem_free(&app->mem, view->text);
    pnl_mem_free(&app->mem, view);
}

int pnl_result(pnl_app_t *app, int mark, const char *text)
{
    struct pnl_result_view *result;
    struct pnl_view view;
    int status;

    if (app == NULL || text == NULL || !pnl_valid_mark(mark) || (!app->running && !app->cli))
        return PNL_ERR_ARG;
    if (app->cli)
        return pnl_cli_result(app, mark, text);
    result = pnl_mem_alloc(&app->mem, sizeof(*result));
    if (result == NULL)
        return PNL_ERR_NOMEM;
    result->mark = mark;
    result->shown_ms = 0;
    result->text = pnl_mem_copy(&app->mem, text);
    memset(&view, 0, sizeof(view));
    view.footer = PNL_RESULT_FOOTER;
    view.draw_fn = pnl_result_draw;
    view.key_fn = pnl_result_key;
    view.drop_fn = pnl_result_drop;
    view.ctx = result;
    status = result->text == NULL ? PNL_ERR_NOMEM : pnl_view_push_internal(app, &view);
    if (status != PNL_OK)
        pnl_result_drop(app, result);
    return status;
}

#include <stdio.h>
#include <string.h>

static const char *const pnl_spark_blocks[] = {"\xe2\x96\x81", "\xe2\x96\x82", "\xe2\x96\x83", "\xe2\x96\x84",
                                               "\xe2\x96\x85", "\xe2\x96\x86", "\xe2\x96\x87", "\xe2\x96\x88"};

static const char *const pnl_eighths[] = {"", "\xe2\x96\x8f", "\xe2\x96\x8e", "\xe2\x96\x8d",
                                          "\xe2\x96\x8c", "\xe2\x96\x8b", "\xe2\x96\x8a", "\xe2\x96\x89"};

static const char *const pnl_spin_frames[] = {"\xe2\xa0\x8b", "\xe2\xa0\x99", "\xe2\xa0\xb9", "\xe2\xa0\xb8",
                                              "\xe2\xa0\xbc", "\xe2\xa0\xb4", "\xe2\xa0\xa6", "\xe2\xa0\xa7",
                                              "\xe2\xa0\x87", "\xe2\xa0\x8f"};

static const char *const pnl_line_frames[] = {"-", "\\", "|", "/"};

static const char *const pnl_arc_frames[] = {"\xe2\x97\x9c", "\xe2\x97\xa0", "\xe2\x97\x9d",
                                             "\xe2\x97\x9e", "\xe2\x97\xa1", "\xe2\x97\x9f"};

static const char *const pnl_block_frames[] = {"\xe2\x96\x96", "\xe2\x96\x98", "\xe2\x96\x9d",
                                               "\xe2\x96\x97"};

static const char *const pnl_bounce_frames[] = {"\xe2\xa0\x81", "\xe2\xa0\x82", "\xe2\xa0\x84",
                                                "\xe2\xa1\x80", "\xe2\xa0\x84", "\xe2\xa0\x82"};

struct pnl_spinner_kind {
    const char *const *frames;
    int len;
};

static const struct pnl_spinner_kind pnl_spinner_kinds[] = {
    {pnl_spin_frames, 10}, {pnl_line_frames, 4}, {pnl_arc_frames, 6}, {pnl_block_frames, 4}, {pnl_bounce_frames, 6}};

void pnl_draw_spinner(pnl_canvas_t *canvas, int x, int y, int style)
{
    pnl_canvas_spinner(canvas, x, y, style, PNL_SPINNER_DOTS);
}

int pnl_canvas_spinner(pnl_canvas_t *canvas, int x, int y, int style, int kind)
{
    const struct pnl_spinner_kind *spinner;
    int spin;

    if (canvas == NULL || kind < 0 || kind > PNL_SPINNER_BOUNCE)
        return PNL_ERR_ARG;
    spinner = &pnl_spinner_kinds[kind];
    spin = canvas->screen->spin;
    if (spin >= 0)
        canvas->screen->spinning = 1;
    pnl_canvas_text(canvas, x, y, style, spinner->frames[spin > 0 ? spin % spinner->len : 0]);
    return PNL_OK;
}

void pnl_canvas_busy(pnl_canvas_t *canvas, int x, int y, int width, int flags)
{
    int length = width / 5 < 3 ? 3 : width / 5;
    int color = flags >> 16 != 0 ? pnl_gradient_color(flags >> 16, 0.5)
                : flags & PNL_METER_LEVEL ? pnl_level_color(0.5) : PNL_ROLE_ACCENT;
    double phase = 0.0;
    int start;
    int eighths;

    if (canvas == NULL || width <= length)
        return;
    if (canvas->screen->spin >= 0) {
        long long now = pnl_term_now_ms() % PNL_BUSY_MS;
        double t = (double)now * 2.0 / PNL_BUSY_MS;

        t = t > 1.0 ? 2.0 - t : t;
        phase = t < 0.5 ? 2.0 * t * t : 1.0 - 2.0 * (1.0 - t) * (1.0 - t);
        canvas->screen->spinning = 1;
    }
    eighths = (int)(phase * (width - length) * 8 + 0.5);
    start = eighths / 8;
    pnl_canvas_fill(canvas, x, y, width, 1, PNL_BG(PNL_ROLE_SURFACE));
    pnl_canvas_fill(canvas, x + start + 1, y, length - 1, 1, PNL_BG(color));
    if (eighths % 8 == 0) {
        pnl_canvas_fill(canvas, x + start, y, 1, 1, PNL_BG(color));
        return;
    }
    pnl_canvas_text(canvas, x + start, y, PNL_FG(PNL_ROLE_SURFACE) | PNL_BG(color), pnl_eighths[eighths % 8]);
    pnl_canvas_text(canvas, x + start + length, y, PNL_FG(color) | PNL_BG(PNL_ROLE_SURFACE),
                    pnl_eighths[eighths % 8]);
}

static double pnl_clamp(double fraction)
{
    if (!(fraction > 0.0))
        return 0.0;
    return fraction > 1.0 ? 1.0 : fraction;
}

int pnl_level_color(double fraction)
{
    return PNL_LEVEL_FIRST + (int)(pnl_clamp(fraction) * (PNL_LEVEL_STEPS - 1) + 0.5);
}

static int pnl_meter_color(double fraction, int flags)
{
    if (flags >> 16 != 0)
        return pnl_gradient_color(flags >> 16, fraction);
    return flags & PNL_METER_LEVEL ? pnl_level_color(fraction) : PNL_ROLE_ACCENT;
}

static int pnl_bar_cell(int flags, int color, int cell, int width)
{
    if (flags >> 16 == 0)
        return color;
    return pnl_gradient_color(flags >> 16, width > 1 ? (double)cell / (double)(width - 1) : 0.0);
}

static void pnl_bar_percent(pnl_canvas_t *canvas, int x, int y, int width, double fraction, int flags, int color)
{
    char text[8];
    int filled = (int)(fraction * width + 0.5);
    int len = snprintf(text, sizeof(text), "%d%%", (int)(fraction * 100.0 + 0.5));
    int start = (width - len) / 2;
    int i;

    for (i = 0; i < len && start >= 0; i++) {
        char glyph[2] = {text[i], '\0'};
        int inside = start + i < filled;
        int ink = pnl_bar_cell(flags, color, start + i, width);

        pnl_canvas_text(canvas, x + start + i, y,
                        PNL_BOLD | (inside ? PNL_BG(ink) | PNL_FG(PNL_ROLE_SURFACE)
                                           : PNL_BG(PNL_ROLE_SURFACE) | PNL_FG(ink)),
                        glyph);
    }
}

void pnl_canvas_bar(pnl_canvas_t *canvas, int x, int y, int width, double fraction, int flags)
{
    int color;
    int eighths;
    int filled;
    int cell;

    if (canvas == NULL || width <= 0)
        return;
    fraction = pnl_clamp(fraction);
    color = pnl_meter_color(fraction, flags);
    eighths = (int)(fraction * width * 8 + 0.5);
    filled = eighths / 8;
    for (cell = 0; cell < filled; cell++)
        pnl_canvas_fill(canvas, x + cell, y, 1, 1, PNL_BG(pnl_bar_cell(flags, color, cell, width)));
    pnl_canvas_fill(canvas, x + filled, y, width - filled, 1, PNL_BG(PNL_ROLE_SURFACE));
    if (eighths % 8 != 0)
        pnl_canvas_text(canvas, x + filled, y, PNL_FG(pnl_bar_cell(flags, color, filled, width)) | PNL_BG(PNL_ROLE_SURFACE),
                        pnl_eighths[eighths % 8]);
    if (flags & PNL_METER_PERCENT)
        pnl_bar_percent(canvas, x, y, width, fraction, flags, color);
}

static void pnl_column(pnl_canvas_t *canvas, int x, int bottom, int width, int height, double fraction, int flags)
{
    int eighths = (int)(pnl_clamp(fraction) * height * 8 + 0.5);
    int row;

    for (row = 0; row < height && eighths > row * 8; row++) {
        int part = eighths - row * 8 >= 8 ? 7 : eighths - row * 8 - 1;
        double level = flags & PNL_METER_LEVEL || flags >> 16 != 0 ? (double)(row + 1) / height : 0.0;
        int i;

        for (i = 0; i < width; i++)
            pnl_canvas_text(canvas, x + i, bottom - row, PNL_FG(pnl_meter_color(level, flags)), pnl_spark_blocks[part]);
    }
}

void pnl_canvas_chart(pnl_canvas_t *canvas, int x, int y, int width, int height, const double *values,
                      size_t values_len, double max, int flags)
{
    size_t first;
    size_t i;

    if (canvas == NULL || values == NULL || width <= 0 || height <= 0 || !(max > 0.0))
        return;
    first = values_len > (size_t)width ? values_len - (size_t)width : 0;
    for (i = first; i < values_len; i++)
        pnl_column(canvas, x + (int)(i - first), y + height - 1, 1, height, values[i] / max, flags);
}

void pnl_canvas_columns(pnl_canvas_t *canvas, int x, int y, int width, int height, const double *values,
                        const char *const *labels, size_t count, double max, int flags)
{
    int slot;
    int bars = labels != NULL ? height - 1 : height;
    size_t i;

    if (canvas == NULL || values == NULL || count == 0 || width < (int)count || bars <= 0 || !(max > 0.0))
        return;
    slot = width / (int)count;
    for (i = 0; i < count; i++) {
        int left = x + (int)i * slot;
        int bar = slot > 2 ? slot - 1 : slot;

        pnl_column(canvas, left, y + bars - 1, bar, bars, values[i] / max, flags);
        if (labels != NULL && labels[i] != NULL)
            pnl_canvas_clip(canvas, left, y + bars, bar, PNL_FG(PNL_ROLE_MUTED), labels[i]);
    }
}

static const unsigned char pnl_braille_bits[2][4] = {{0x01, 0x02, 0x04, 0x40}, {0x08, 0x10, 0x20, 0x80}};

static int pnl_plot_dot(const double *values, size_t at, double min, double max, int rows)
{
    double fraction = pnl_clamp((values[at] - min) / (max - min));

    return (int)((1.0 - fraction) * (rows * 4 - 1) + 0.5);
}

static void pnl_plot_cell(pnl_canvas_t *canvas, int x, int y, int height, const unsigned char *bits, int color)
{
    int row;

    for (row = 0; row < height; row++) {
        char glyph[4];

        if (bits[row] == 0)
            continue;
        glyph[pnl_utf8_put(0x2800u + bits[row], glyph)] = '\0';
        pnl_canvas_text(canvas, x, y + row, PNL_FG(color), glyph);
    }
}

void pnl_canvas_plot(pnl_canvas_t *canvas, int x, int y, int width, int height, const double *values,
                     size_t values_len, double min, double max, int flags)
{
    unsigned char bits[PNL_PLOT_ROWS];
    size_t first;
    size_t dot;
    int previous = -1;

    if (canvas == NULL || values == NULL || width <= 0 || height <= 0 || height > PNL_PLOT_ROWS || !(max > min))
        return;
    first = values_len > (size_t)width * 2 ? values_len - (size_t)width * 2 : 0;
    memset(bits, 0, sizeof(bits));
    for (dot = first; dot < values_len; dot++) {
        int column = (int)((dot - first) % 2);
        int at = pnl_plot_dot(values, dot, min, max, height);
        int from = previous < 0 ? at : previous;
        int step = from <= at ? 1 : -1;
        int row;

        for (row = from; row != at + step; row += step)
            bits[row / 4] |= pnl_braille_bits[column][row % 4];
        previous = at;
        if (column == 1 || dot + 1 == values_len) {
            double fraction = pnl_clamp((values[dot] - min) / (max - min));

            pnl_plot_cell(canvas, x + (int)(dot - first) / 2, y, height, bits, pnl_meter_color(fraction, flags));
            memset(bits, 0, sizeof(bits));
        }
    }
}

void pnl_canvas_sparkline(pnl_canvas_t *canvas, int x, int y, int width, const double *values, size_t values_len,
                          double max, int flags)
{
    size_t first;
    size_t i;

    if (canvas == NULL || values == NULL || width <= 0 || !(max > 0.0))
        return;
    first = values_len > (size_t)width ? values_len - (size_t)width : 0;
    for (i = first; i < values_len; i++) {
        double fraction = pnl_clamp(values[i] / max);
        int block = (int)(fraction * 7 + 0.5);

        pnl_canvas_text(canvas, x + (int)(i - first), y, PNL_FG(pnl_meter_color(fraction, flags)),
                        pnl_spark_blocks[block]);
    }
}

int pnl_status_meter(pnl_app_t *app, int mark, const char *label, double fraction)
{
    int status = pnl_status_line(app, mark, label, "");

    if (status == PNL_OK)
        app->lines[app->lines_len - 1].meter = pnl_clamp(fraction);
    return status;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_NOTIFY_TEXT 512
#define PNL_NOTIFY_AFTER_MS 5000

static int pnl_notify_mode(void)
{
    const char *setting = getenv("TUIKIT_NOTIFY");

    if (setting == NULL || setting[0] == '\0')
        return 2;
    if (pnl_env_off("TUIKIT_NOTIFY"))
        return 0;
    return strcmp(setting, "bell") == 0 ? 1 : 2;
}

static void pnl_notify_clean(char *out, const char *title, const char *text)
{
    size_t i;

    snprintf(out, PNL_NOTIFY_TEXT, "%s: %s", title, text);
    for (i = 0; out[i] != '\0'; i++) {
        if ((unsigned char)out[i] < 0x20 || out[i] == 0x7f)
            out[i] = ' ';
    }
}

static void pnl_notify_send(pnl_app_t *app, const char *text)
{
    char message[PNL_NOTIFY_TEXT];
    char sequence[PNL_NOTIFY_TEXT + 16];
    int mode = pnl_notify_mode();
    int len;

    if (mode == 0)
        return;
    pnl_notify_clean(message, app->title, text);
    len = snprintf(sequence, sizeof(sequence), mode == 2 ? "\a\x1b]9;%s\a" : "\a", message);
    if (len <= 0)
        return;
    if (app->running) {
        pnl_term_write(sequence, strlen(sequence));
        return;
    }
    if (pnl_term_stderr_is_tty()) {
        fputs(sequence, stderr);
        fflush(stderr);
    }
}

int pnl_notify(pnl_app_t *app, const char *text)
{
    if (app == NULL || text == NULL || !(app->running || app->cli))
        return PNL_ERR_ARG;
    if (app->running && app->term.focus_known && app->term.focused)
        return PNL_OK;
    pnl_notify_send(app, text);
    return PNL_OK;
}

void pnl_task_notify(pnl_app_t *app, const char *title, long long started_ms, const char *outcome)
{
    char text[PNL_NOTIFY_TEXT];

    if (pnl_term_now_ms() - started_ms < PNL_NOTIFY_AFTER_MS)
        return;
    snprintf(text, sizeof(text), "%s - %s", title, outcome);
    pnl_notify(app, text);
}

#include <stdio.h>
#include <string.h>

#define PNL_BOX_TL "\xe2\x95\xad"
#define PNL_BOX_TR "\xe2\x95\xae"
#define PNL_BOX_BL "\xe2\x95\xb0"
#define PNL_BOX_BR "\xe2\x95\xaf"
#define PNL_BOX_H "\xe2\x94\x80"
#define PNL_BOX_V "\xe2\x94\x82"
#define PNL_TOAST_MS 3000
#define PNL_TOAST_SLIDE_MS 180

static const char *const pnl_toast_marks[] = {"ok", "..", "!!"};
static const int pnl_toast_roles[] = {PNL_ROLE_OK, PNL_ROLE_WARN, PNL_ROLE_FAIL};

static void pnl_box_edge(struct pnl_screen *screen, int x, int y, int width, int style, const char *left,
                         const char *right)
{
    int i;

    pnl_screen_text(screen, x, y, 1, (uint32_t)style, left);
    for (i = 1; i < width - 1; i++)
        pnl_screen_text(screen, x + i, y, 1, (uint32_t)style, PNL_BOX_H);
    pnl_screen_text(screen, x + width - 1, y, 1, (uint32_t)style, right);
}

static void pnl_box(struct pnl_screen *screen, int x, int y, int width, int height, int style, const char *title)
{
    int row;

    pnl_box_edge(screen, x, y, width, style, PNL_BOX_TL, PNL_BOX_TR);
    for (row = y + 1; row < y + height - 1; row++) {
        pnl_screen_fill(screen, x, row, width, 0);
        pnl_screen_text(screen, x, row, 1, (uint32_t)style, PNL_BOX_V);
        pnl_screen_text(screen, x + width - 1, row, 1, (uint32_t)style, PNL_BOX_V);
    }
    pnl_box_edge(screen, x, y + height - 1, width, style, PNL_BOX_BL, PNL_BOX_BR);
    if (title != NULL && width > 6) {
        int end = pnl_screen_text(screen, x + 2, y, width - 4, PNL_BOLD, " ");

        end = pnl_screen_text(screen, end, y, x + width - 3 - end, PNL_BOLD, title);
        pnl_screen_text(screen, end, y, 1, PNL_BOLD, " ");
    }
}

static int pnl_clamp_size(int want, int room)
{
    return want < 1 ? 1 : want > room ? room : want;
}

static void pnl_box_scaled(struct pnl_screen *screen, int x, int y, int width, int height, double scale, int style)
{
    int w = (int)(width * scale + 0.5);
    int h = (int)(height * scale + 0.5);

    w = w < 5 ? 5 : w;
    h = h < 3 ? 3 : h;
    pnl_box(screen, x + (width - w) / 2, y + (height - h) / 2, w, h, style, NULL);
}

void pnl_ghost_draw(pnl_app_t *app)
{
    struct pnl_ghost *ghost = &app->ghost;
    double t;

    if (ghost->ms == 0)
        return;
    t = pnl_app_tween(app, ghost->ms, PNL_EXIT_MS);
    if (t >= 1.0 || !pnl_motion_on(app, PNL_MOTION_ENTER)) {
        ghost->ms = 0;
        return;
    }
    pnl_box_scaled(&app->screen, ghost->x, ghost->y, ghost->width, ghost->height, 1.0 - 0.6 * pnl_ease_out(t),
                   PNL_FG(PNL_ROLE_MUTED));
}

void pnl_dialog_draw(pnl_app_t *app, const pnl_canvas_t *area, struct pnl_view *view, int focused)
{
    struct pnl_canvas inner;
    int width = pnl_clamp_size(view->dialog_width + 4, area->width - 2);
    int height = pnl_clamp_size(view->dialog_height + 2, area->height);
    int x = area->x + (area->width - width) / 2 + (focused ? pnl_shake_offset(app) : 0);
    int y = area->y + (area->height - height) / 2;
    int style = PNL_FG(focused ? PNL_ROLE_ACCENT : PNL_ROLE_MUTED);
    double t = pnl_motion_on(app, PNL_MOTION_ENTER) ? pnl_app_tween(app, view->shown_ms, PNL_ENTER_MS) : 1.0;

    if (width < 5 || height < 3)
        return;
    view->box_x = x;
    view->box_y = y;
    view->box_width = width;
    view->box_height = height;
    if (t < 1.0) {
        pnl_box_scaled(area->screen, x, y, width, height, 0.4 + 0.6 * pnl_ease_out(t), style);
        return;
    }
    pnl_box(area->screen, x, y, width, height, style, view->title);
    inner.screen = area->screen;
    inner.focused = focused;
    inner.x = x + 2;
    inner.y = y + 1;
    inner.width = width - 4;
    inner.height = height - 2;
    view->origin_x = inner.x;
    view->origin_y = inner.y;
    view->draw_fn(view->ctx, app, &inner);
}

int pnl_toast(pnl_app_t *app, int mark, const char *text)
{
    struct pnl_toast *toast;

    if (app == NULL || text == NULL || !pnl_valid_mark(mark))
        return PNL_ERR_ARG;
    if (app->cli) {
        if (!app->cli_json)
            fprintf(stderr, "[%s] %s\n", pnl_toast_marks[mark], text);
        return PNL_OK;
    }
    if (app->toasts_len == PNL_TOASTS) {
        memmove(app->toasts, app->toasts + 1, sizeof(app->toasts) - sizeof(app->toasts[0]));
        app->toasts_len--;
    }
    toast = &app->toasts[app->toasts_len++];
    toast->mark = mark;
    toast->shown_ms = pnl_term_now_ms();
    snprintf(toast->text, sizeof(toast->text), "%s", text);
    app->dirty = 1;
    return PNL_OK;
}

long long pnl_toasts_next_ms(const pnl_app_t *app)
{
    long long gone;

    if (app->toasts_len == 0)
        return -1;
    gone = app->toasts[0].shown_ms + PNL_TOAST_MS;
    if (pnl_motion_on(app, PNL_MOTION_ENTER) && gone - PNL_EXIT_MS > pnl_term_now_ms())
        return gone - PNL_EXIT_MS;
    return gone;
}

void pnl_toasts_expire(pnl_app_t *app)
{
    long long now = pnl_term_now_ms();
    size_t gone = 0;

    while (gone < app->toasts_len && app->toasts[gone].shown_ms + PNL_TOAST_MS <= now)
        gone++;
    if (gone < app->toasts_len && pnl_motion_on(app, PNL_MOTION_ENTER) &&
        app->toasts[gone].shown_ms + PNL_TOAST_MS - PNL_EXIT_MS <= now)
        pnl_app_animate(app, PNL_EXIT_MS);
    if (gone == 0)
        return;
    memmove(app->toasts, app->toasts + gone, (app->toasts_len - gone) * sizeof(app->toasts[0]));
    app->toasts_len -= gone;
    app->dirty = 1;
}

static void pnl_toast_draw(pnl_app_t *app, const struct pnl_toast *toast, int y)
{
    struct pnl_screen *screen = &app->screen;
    int width = pnl_text_width(toast->text) + 7;
    int style = PNL_BG(PNL_ROLE_SURFACE);
    double t = pnl_motion_on(app, PNL_MOTION_TRANSITION) ? pnl_app_tween(app, toast->shown_ms, PNL_TOAST_SLIDE_MS)
                                                         : 1.0;
    long long left = toast->shown_ms + PNL_TOAST_MS - pnl_app_frame_ms(app);
    int x;

    if (pnl_motion_on(app, PNL_MOTION_ENTER) && left < PNL_EXIT_MS)
        t = left <= 0 ? 0.0 : t * pnl_ease_out((double)left / PNL_EXIT_MS);
    if (t <= 0.0)
        return;
    if (width > screen->width - 2)
        width = screen->width - 2;
    x = screen->width - 1 - (int)(t * width + 0.5);
    pnl_screen_fill(screen, x, y, width, (uint32_t)style);
    pnl_screen_text(screen, x + 1, y, -1, (uint32_t)(style | PNL_BOLD | PNL_FG(pnl_toast_roles[toast->mark])),
                    pnl_toast_marks[toast->mark]);
    pnl_screen_text(screen, x + 5, y, screen->width - x - 6, (uint32_t)style, toast->text);
}

void pnl_toasts_draw(pnl_app_t *app)
{
    size_t i;

    for (i = 0; i < app->toasts_len; i++)
        pnl_toast_draw(app, &app->toasts[i], app->screen.height - 2 - (int)(app->toasts_len - 1 - i));
}

#include <stdio.h>
#include <string.h>

#define PNL_PAGER_TAB 4
#define PNL_SEARCH_MAX 128
#define PNL_PAGER_FOOTER_MAX 192

struct pnl_pager {
    char *text;
    size_t *lines;
    size_t lines_len;
    size_t top;
    struct pnl_glide glide;
    size_t page;
    size_t match;
    int has_match;
    int searching;
    char search[PNL_SEARCH_MAX];
    struct pnl_edit edit;
    char footer_text[PNL_PAGER_FOOTER_MAX];
    const char *footer;
};

static size_t pnl_pager_expanded_len(const char *text, size_t text_len)
{
    size_t len = 0;
    size_t i;

    for (i = 0; i < text_len; i++)
        len += text[i] == '\t' ? PNL_PAGER_TAB : text[i] == '\0' ? 0 : 1;
    return len + 1;
}

static size_t pnl_pager_copy(struct pnl_pager *pager, const char *text, size_t text_len)
{
    size_t out = 0;
    size_t i;
    size_t lines = 1;

    for (i = 0; i < text_len; i++) {
        if (text[i] == '\t') {
            memset(pager->text + out, ' ', PNL_PAGER_TAB);
            out += PNL_PAGER_TAB;
        } else if (text[i] != '\0' && text[i] != '\r') {
            pager->text[out++] = text[i];
        }
        lines += text[i] == '\n';
    }
    pager->text[out] = '\0';
    return out > 0 && pager->text[out - 1] == '\n' ? lines - 1 : lines;
}

static void pnl_pager_split(struct pnl_pager *pager)
{
    char *pos = pager->text;
    size_t i;

    for (i = 0; i < pager->lines_len; i++) {
        char *end = (strchr)(pos, '\n');

        pager->lines[i] = (size_t)(pos - pager->text);
        if (end == NULL)
            break;
        *end = '\0';
        pos = end + 1;
    }
}

static const char *pnl_pager_line(const struct pnl_pager *pager, size_t index)
{
    return pager->text + pager->lines[index];
}

static size_t pnl_pager_last_top(const struct pnl_pager *pager)
{
    return pager->lines_len > pager->page ? pager->lines_len - pager->page : 0;
}

static void pnl_pager_status(struct pnl_pager *pager)
{
    size_t bottom = pager->top + pager->page < pager->lines_len ? pager->top + pager->page : pager->lines_len;

    if (pager->searching) {
        snprintf(pager->footer_text, sizeof(pager->footer_text), "/%s_   enter: find  esc: cancel", pager->search);
    } else {
        snprintf(pager->footer_text, sizeof(pager->footer_text),
                 "%zu-%zu of %zu%s   up/down: scroll  /: search  n/N: next/previous  q: back",
                 pager->lines_len ? pager->top + 1 : 0, bottom, pager->lines_len,
                 pager->search[0] && !pager->has_match ? "  (not found)" : "");
    }
}

static void pnl_pager_draw_line(struct pnl_pager *pager, pnl_canvas_t *canvas, int y, size_t index)
{
    const char *line = pnl_pager_line(pager, index);
    const char *hit;
    size_t needle = strlen(pager->search);
    char head[1024];
    int x;

    pnl_canvas_text(canvas, 1, y, 0, line);
    if (!pager->has_match || index != pager->match || needle == 0)
        return;
    for (hit = line; *hit != '\0' && !pnl_text_contains_at(hit, pager->search); hit++)
        ;
    if (*hit == '\0' || (size_t)(hit - line) >= sizeof(head) || needle >= sizeof(head))
        return;
    memcpy(head, line, (size_t)(hit - line));
    head[hit - line] = '\0';
    x = 1 + pnl_text_width(head);
    memcpy(head, hit, needle);
    head[needle] = '\0';
    pnl_canvas_text(canvas, x, y, PNL_REVERSE, head);
}

static void pnl_pager_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_pager *pager = ctx;
    int height = pnl_canvas_height(canvas);
    size_t shown;
    size_t i;

    pager->page = height > 0 ? (size_t)height : 1;
    if (pager->top > pnl_pager_last_top(pager))
        pager->top = pnl_pager_last_top(pager);
    pnl_glide_to(app, &pager->glide, (double)pager->top, PNL_MOTION_SLIDE);
    shown = (size_t)(pnl_glide_at(app, &pager->glide, PNL_MOTION_SLIDE) + 0.5);
    for (i = 0; i < pager->page && shown + i < pager->lines_len; i++)
        pnl_pager_draw_line(pager, canvas, (int)i, shown + i);
    pnl_pager_status(pager);
}

static void pnl_pager_find(struct pnl_pager *pager, size_t from, int forward)
{
    size_t count;

    pager->has_match = 0;
    if (pager->search[0] == '\0' || pager->lines_len == 0)
        return;
    for (count = 0; count < pager->lines_len; count++) {
        size_t index = forward ? (from + count) % pager->lines_len
                               : (from + pager->lines_len - count % pager->lines_len) % pager->lines_len;

        if (!pnl_text_contains(pnl_pager_line(pager, index), pager->search))
            continue;
        pager->match = index;
        pager->has_match = 1;
        if (index < pager->top || index >= pager->top + pager->page)
            pager->top = index > pager->page / 3 ? index - pager->page / 3 : 0;
        return;
    }
}

static int pnl_pager_search_key(struct pnl_pager *pager, int key)
{
    if (key == PNL_KEY_ENTER || key == PNL_KEY_ESCAPE) {
        pager->searching = 0;
        if (key == PNL_KEY_ESCAPE)
            pager->search[0] = '\0';
        pnl_pager_find(pager, pager->top, 1);
        return 1;
    }
    pnl_edit_key(&pager->edit, key);
    return 1;
}

static void pnl_pager_scroll(struct pnl_pager *pager, int key)
{
    size_t last = pnl_pager_last_top(pager);
    size_t step = key == PNL_KEY_PAGE_UP || key == PNL_KEY_PAGE_DOWN || key == ' ' ? pager->page : 1;

    if (key == PNL_KEY_HOME || key == 'g')
        pager->top = 0;
    else if (key == PNL_KEY_END || key == 'G')
        pager->top = last;
    else if (key == PNL_KEY_UP || key == 'k' || key == PNL_KEY_PAGE_UP)
        pager->top = pager->top > step ? pager->top - step : 0;
    else
        pager->top = pager->top + step < last ? pager->top + step : last;
}

static int pnl_pager_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_pager *pager = ctx;

    (void)app;
    if (pager->searching)
        return pnl_pager_search_key(pager, key);
    if (key == '/') {
        pager->searching = 1;
        pager->search[0] = '\0';
        pnl_edit_init(&pager->edit, pager->search, sizeof(pager->search));
        return 1;
    }
    if (key == 'n' || key == 'N') {
        size_t from = pager->has_match ? pager->match : pager->top;

        pnl_pager_find(pager, key == 'n' ? from + 1 : from + pager->lines_len - 1, key == 'n');
        return 1;
    }
    if (key == 'q' || key == PNL_KEY_ESCAPE)
        return 0;
    if (key != PNL_KEY_UP && key != PNL_KEY_DOWN && key != 'j' && key != 'k' && key != ' ' && key != 'g' &&
        key != 'G' && (key < PNL_KEY_PAGE_UP || key > PNL_KEY_END))
        return 0;
    pnl_pager_scroll(pager, key);
    return 1;
}

static void pnl_pager_drop(pnl_app_t *app, void *ctx)
{
    struct pnl_pager *pager = ctx;

    pnl_mem_free(&app->mem, pager->text);
    pnl_mem_free(&app->mem, pager->lines);
    pnl_mem_free(&app->mem, pager);
}

static int pnl_pager_cli(const char *text, size_t text_len)
{
    fwrite(text, 1, text_len, stdout);
    if (text_len > 0 && text[text_len - 1] != '\n')
        putchar('\n');
    fflush(stdout);
    return PNL_OK;
}

static struct pnl_pager *pnl_pager_create(pnl_app_t *app, const char *text, size_t text_len)
{
    struct pnl_pager *pager = pnl_mem_alloc(&app->mem, sizeof(*pager));

    if (pager == NULL)
        return NULL;
    memset(pager, 0, sizeof(*pager));
    pager->text = pnl_mem_alloc(&app->mem, pnl_pager_expanded_len(text, text_len));
    if (pager->text != NULL) {
        pager->lines_len = pnl_pager_copy(pager, text, text_len);
        pager->lines = pnl_mem_alloc(&app->mem, pager->lines_len * sizeof(*pager->lines));
    }
    if (pager->text == NULL || pager->lines == NULL) {
        pnl_pager_drop(app, pager);
        return NULL;
    }
    pnl_pager_split(pager);
    pager->page = 1;
    pager->footer = pager->footer_text;
    pnl_pager_status(pager);
    return pager;
}

int pnl_pager(pnl_app_t *app, const char *title, const char *text, size_t text_len)
{
    struct pnl_pager *pager;
    struct pnl_view view;
    int status;

    if (app == NULL || title == NULL || (text == NULL && text_len > 0) || (!app->running && !app->cli))
        return PNL_ERR_ARG;
    if (app->cli)
        return pnl_pager_cli(text, text_len);
    pager = pnl_pager_create(app, text ? text : "", text_len);
    if (pager == NULL)
        return PNL_ERR_NOMEM;
    memset(&view, 0, sizeof(view));
    view.footer_ref = &pager->footer;
    view.draw_fn = pnl_pager_draw;
    view.key_fn = pnl_pager_key;
    view.drop_fn = pnl_pager_drop;
    view.ctx = pager;
    view.title = pnl_mem_copy(&app->mem, title);
    status = view.title == NULL ? PNL_ERR_NOMEM : pnl_view_push_internal(app, &view);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, view.title);
        pnl_pager_drop(app, pager);
    }
    return status;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_CONFIRM_FOOTER "y/n  left/right: choose  enter: answer  esc: no"
#define PNL_INPUT_FOOTER "enter: accept  esc: cancel"
#define PNL_INPUT_HISTORY_FOOTER "up/down: history  enter: accept  esc: cancel"
#define PNL_PICK_FOOTER "type to filter  up/down: move  enter: choose  esc: cancel"
#define PNL_FILTER_MAX 128
#define PNL_ANSWER_MAX 16
#define PNL_HISTORY_MAX 20
#define PNL_HISTORY_TEXT 4096

struct pnl_confirm_view {
    const char *question;
    int yes;
    int answered;
};

struct pnl_input_view {
    const char *prompt;
    struct pnl_edit edit;
    const char *history;
    const char *draft;
    size_t recalled;
    int ok;
};

struct pnl_pick_view {
    const char *const *items;
    size_t items_len;
    size_t *shown;
    size_t shown_len;
    size_t selected;
    size_t top;
    char filter[PNL_FILTER_MAX];
    struct pnl_edit edit;
    int ok;
};

static int pnl_confirm_key(void *ctx, pnl_app_t *app, int key);

static int pnl_prompt_ready(const pnl_app_t *app)
{
    return app->running || app->cli;
}

static const int pnl_typing = 1;

static int pnl_prompt_modal(pnl_app_t *app, const char *title, const char *footer, int width, int height,
                            pnl_draw_fn draw_fn, pnl_key_fn key_fn, void *ctx)
{
    struct pnl_view view;

    memset(&view, 0, sizeof(view));
    view.typing_ref = key_fn == pnl_confirm_key ? NULL : &pnl_typing;
    view.dialog_width = width < 24 ? 24 : width;
    view.dialog_height = height;
    view.footer = footer;
    view.draw_fn = draw_fn;
    view.key_fn = key_fn;
    view.ctx = ctx;
    view.title = pnl_mem_copy(&app->mem, title);
    if (view.title == NULL)
        return PNL_ERR_NOMEM;
    return pnl_view_modal(app, &view);
}

static void pnl_confirm_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_confirm_view *view = ctx;

    (void)app;
    pnl_canvas_text(canvas, 0, 0, PNL_BOLD, view->question);
    pnl_canvas_text(canvas, 0, 2, view->yes ? PNL_REVERSE : 0, " yes ");
    pnl_canvas_text(canvas, 7, 2, view->yes ? 0 : PNL_REVERSE, " no ");
}

static int pnl_confirm_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_confirm_view *view = ctx;
    int x;
    int y;

    if (key == PNL_KEY_CLICK) {
        pnl_mouse_position(app, &x, &y);
        if (y != 2 || x < 0 || x > 10 || x == 5 || x == 6)
            return 1;
        view->yes = x < 5;
        key = PNL_KEY_ENTER;
    }
    if (key == PNL_KEY_LEFT || key == PNL_KEY_RIGHT || key == PNL_KEY_TAB || key == 'h' || key == 'l') {
        view->yes = !view->yes;
        return 1;
    }
    if (key == 'y' || key == 'Y' || key == 'n' || key == 'N' || key == PNL_KEY_ESCAPE)
        view->yes = key == 'y' || key == 'Y';
    else if (key != PNL_KEY_ENTER) {
        if (key >= ' ' && key < 0x7f)
            pnl_view_shake(app);
        return 1;
    }
    view->answered = 1;
    pnl_view_pop(app);
    return 1;
}

static int pnl_confirm_cli(pnl_app_t *app, const char *question, int *out_yes)
{
    char answer[PNL_ANSWER_MAX];
    char prompt[512];

    *out_yes = app->cli_yes;
    if (app->cli_yes)
        return PNL_OK;
    if (!pnl_term_stdin_is_tty()) {
        fprintf(stderr, "%s not confirmed: pass --yes\n", question);
        app->cli_worst = PNL_MARK_FAIL;
        return PNL_OK;
    }
    snprintf(prompt, sizeof(prompt), "%s [y/N] ", question);
    if (pnl_cli_read_line(prompt, answer, sizeof(answer)))
        *out_yes = answer[0] == 'y' || answer[0] == 'Y';
    return PNL_OK;
}

int pnl_confirm(pnl_app_t *app, const char *question, int *out_yes)
{
    struct pnl_confirm_view view;
    int status;

    if (app == NULL || question == NULL || out_yes == NULL || !pnl_prompt_ready(app))
        return PNL_ERR_ARG;
    *out_yes = 0;
    if (app->cli)
        return pnl_confirm_cli(app, question, out_yes);
    memset(&view, 0, sizeof(view));
    view.question = question;
    status = pnl_prompt_modal(app, "confirm", PNL_CONFIRM_FOOTER, pnl_text_width(question), 3, pnl_confirm_draw, pnl_confirm_key, &view);
    *out_yes = view.answered && view.yes;
    return status;
}

static void pnl_input_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_input_view *view = ctx;

    (void)app;
    pnl_canvas_text(canvas, 0, 0, PNL_BOLD, view->prompt);
    pnl_canvas_fill(canvas, 0, 2, pnl_canvas_width(canvas), 1, PNL_UNDERLINE);
    pnl_edit_draw(&view->edit, canvas, 0, 2, pnl_canvas_width(canvas), 1);
}

static const char *pnl_history_entry(const char *history, size_t index, size_t *out_len)
{
    const char *end;

    while (history != NULL && index-- > 0) {
        history = (strchr)(history, '\n');
        history = history != NULL ? history + 1 : NULL;
    }
    if (history == NULL || *history == '\0')
        return NULL;
    end = (strchr)(history, '\n');
    *out_len = end != NULL ? (size_t)(end - history) : strlen(history);
    return history;
}

static void pnl_input_recall(struct pnl_input_view *view, int key)
{
    size_t index = key == PNL_KEY_UP ? view->recalled + 1 : view->recalled - 1;
    const char *entry = view->draft;
    size_t len = strlen(view->draft);

    if (key == PNL_KEY_DOWN && view->recalled == 0)
        return;
    if (index > 0 && (entry = pnl_history_entry(view->history, index - 1, &len)) == NULL)
        return;
    view->recalled = index;
    if (len >= view->edit.cap) {
        len = view->edit.cap - 1;
        while (len > 0 && ((unsigned char)entry[len] & 0xc0) == 0x80)
            len--;
    }
    memcpy(view->edit.text, entry, len);
    view->edit.text[len] = '\0';
    pnl_edit_init(&view->edit, view->edit.text, view->edit.cap);
}

static void pnl_history_add(pnl_app_t *app, const char *prompt, const char *text)
{
    const char *old = pnl_memory_get(app, "input", prompt);
    char history[PNL_HISTORY_TEXT];
    size_t used = 0;
    size_t count = 1;
    size_t index;
    size_t len;
    const char *entry;

    if (text[0] == '\0' || (strchr)(text, '\n') != NULL || strlen(text) >= sizeof(history))
        return;
    used = (size_t)snprintf(history, sizeof(history), "%s", text);
    for (index = 0; count < PNL_HISTORY_MAX && (entry = pnl_history_entry(old, index, &len)) != NULL; index++) {
        if ((len == strlen(text) && memcmp(entry, text, len) == 0) || used + len + 2 > sizeof(history))
            continue;
        history[used++] = '\n';
        memcpy(history + used, entry, len);
        used += len;
        history[used] = '\0';
        count++;
    }
    pnl_memory_set(app, "input", prompt, history);
}

static int pnl_input_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_input_view *view = ctx;

    if (key == PNL_KEY_UP || key == PNL_KEY_DOWN) {
        pnl_input_recall(view, key);
        return 1;
    }
    if (key == PNL_KEY_ENTER || key == PNL_KEY_ESCAPE) {
        view->ok = key == PNL_KEY_ENTER;
        pnl_view_pop(app);
        return 1;
    }
    pnl_edit_key(&view->edit, key);
    return 1;
}

int pnl_input(pnl_app_t *app, const char *prompt, char *text, size_t text_len, int *out_ok)
{
    struct pnl_input_view view;
    char *work;
    int status;

    if (app == NULL || prompt == NULL || text == NULL || text_len == 0 || out_ok == NULL ||
        (memchr)(text, '\0', text_len) == NULL || !pnl_prompt_ready(app))
        return PNL_ERR_ARG;
    *out_ok = 0;
    if (app->cli) {
        char line[1024];

        snprintf(line, sizeof(line), "%s: ", prompt);
        *out_ok = pnl_cli_read_line(line, text, text_len);
        return PNL_OK;
    }
    work = pnl_mem_alloc(&app->mem, text_len);
    if (work == NULL)
        return PNL_ERR_NOMEM;
    memcpy(work, text, text_len);
    memset(&view, 0, sizeof(view));
    view.prompt = prompt;
    view.draft = text;
    view.history = pnl_memory_get(app, "input", prompt);
    pnl_edit_init(&view.edit, work, text_len);
    status = pnl_prompt_modal(app, "input", view.history != NULL ? PNL_INPUT_HISTORY_FOOTER : PNL_INPUT_FOOTER, 48, 3,
                              pnl_input_draw, pnl_input_key, &view);
    if (view.ok) {
        memcpy(text, work, view.edit.len + 1);
        pnl_history_add(app, prompt, text);
    }
    *out_ok = view.ok;
    pnl_mem_free(&app->mem, work);
    return status;
}

static int pnl_lower(int c)
{
    return c >= 'A' && c <= 'Z' ? c - 'A' + 'a' : c;
}

int pnl_text_contains_at(const char *text, const char *needle)
{
    size_t i;

    for (i = 0; needle[i] != '\0'; i++) {
        if (pnl_lower((unsigned char)text[i]) != pnl_lower((unsigned char)needle[i]))
            return 0;
    }
    return 1;
}

int pnl_text_contains(const char *text, const char *needle)
{
    if (needle[0] == '\0')
        return 1;
    for (; *text != '\0'; text++) {
        if (pnl_text_contains_at(text, needle))
            return 1;
    }
    return 0;
}

static void pnl_pick_filter(struct pnl_pick_view *view)
{
    size_t i;

    view->shown_len = 0;
    for (i = 0; i < view->items_len; i++) {
        if (pnl_text_contains(view->items[i], view->filter))
            view->shown[view->shown_len++] = i;
    }
    view->selected = 0;
    view->top = 0;
}

static void pnl_pick_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_pick_view *view = ctx;
    int rows = pnl_canvas_height(canvas) - 2;
    size_t i;

    (void)app;
    pnl_canvas_text(canvas, 0, 0, PNL_FG(PNL_ROLE_MUTED), "filter:");
    pnl_edit_draw(&view->edit, canvas, 8, 0, pnl_canvas_width(canvas) - 8, 1);
    if (rows <= 0)
        return;
    if (view->selected < view->top)
        view->top = view->selected;
    if (view->selected >= view->top + (size_t)rows)
        view->top = view->selected - (size_t)rows + 1;
    for (i = view->top; i < view->shown_len && i < view->top + (size_t)rows; i++) {
        int y = 2 + (int)(i - view->top);
        int selected = i == view->selected;

        pnl_canvas_text(canvas, 0, y, selected ? PNL_BOLD : 0, selected ? "> " : "  ");
        pnl_canvas_text(canvas, 2, y, selected ? PNL_BOLD : 0, view->items[view->shown[i]]);
    }
    if (view->shown_len == 0)
        pnl_canvas_text(canvas, 2, 2, PNL_FG(PNL_ROLE_MUTED), "no match");
}

static void pnl_pick_move(struct pnl_pick_view *view, int key)
{
    size_t step = key == PNL_KEY_PAGE_UP || key == PNL_KEY_PAGE_DOWN ? 10 : 1;

    if (view->shown_len == 0)
        return;
    if (key == PNL_KEY_UP || key == PNL_KEY_PAGE_UP)
        view->selected = view->selected > step ? view->selected - step : 0;
    else
        view->selected = view->selected + step < view->shown_len ? view->selected + step : view->shown_len - 1;
}

static int pnl_pick_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_pick_view *view = ctx;
    int x;
    int y;

    if (key == PNL_KEY_WHEEL_UP || key == PNL_KEY_WHEEL_DOWN)
        key = key == PNL_KEY_WHEEL_UP ? PNL_KEY_UP : PNL_KEY_DOWN;
    if (key == PNL_KEY_CLICK) {
        pnl_mouse_position(app, &x, &y);
        if (y < 2 || view->top + (size_t)(y - 2) >= view->shown_len)
            return 1;
        view->selected = view->top + (size_t)(y - 2);
        key = PNL_KEY_ENTER;
    }
    if (key == PNL_KEY_UP || key == PNL_KEY_DOWN || key == PNL_KEY_PAGE_UP || key == PNL_KEY_PAGE_DOWN) {
        pnl_pick_move(view, key);
        return 1;
    }
    if (key == PNL_KEY_ESCAPE || (key == PNL_KEY_ENTER && view->shown_len > 0)) {
        view->ok = key == PNL_KEY_ENTER;
        pnl_view_pop(app);
        return 1;
    }
    if (key == PNL_KEY_ENTER)
        pnl_view_shake(app);
    else if (pnl_edit_key(&view->edit, key))
        pnl_pick_filter(view);
    return 1;
}

static int pnl_pick_cli(const char *title, const char *const *items, size_t items_len, size_t *out_index,
                        int *out_ok)
{
    char line[1024];
    char *end;
    size_t i;

    if (pnl_term_stdin_is_tty()) {
        fprintf(stderr, "%s\n", title);
        for (i = 0; i < items_len; i++)
            fprintf(stderr, "  %zu) %s\n", i + 1, items[i]);
    }
    if (!pnl_cli_read_line("choice: ", line, sizeof(line)))
        return PNL_OK;
    i = (size_t)strtoul(line, &end, 10);
    if (end != line && *end == '\0' && i >= 1 && i <= items_len) {
        *out_index = i - 1;
        *out_ok = 1;
        return PNL_OK;
    }
    for (i = 0; i < items_len; i++) {
        if (strcmp(items[i], line) == 0) {
            *out_index = i;
            *out_ok = 1;
            break;
        }
    }
    return PNL_OK;
}

static int pnl_pick_width(const char *const *items, size_t items_len)
{
    int width = 30;
    size_t i;

    for (i = 0; i < items_len; i++) {
        if (pnl_text_width(items[i]) + 2 > width)
            width = pnl_text_width(items[i]) + 2;
    }
    return width;
}

static void pnl_pick_recall(const pnl_app_t *app, const char *title, struct pnl_pick_view *view)
{
    const char *last = pnl_memory_get(app, "pick", title);
    size_t i;

    for (i = 0; last != NULL && i < view->items_len; i++) {
        if (strcmp(view->items[i], last) == 0) {
            view->selected = i;
            return;
        }
    }
}

int pnl_pick(pnl_app_t *app, const char *title, const char *const *items, size_t items_len, size_t *out_index,
             int *out_ok)
{
    struct pnl_pick_view view;
    size_t i;
    int status;

    if (app == NULL || title == NULL || items == NULL || items_len == 0 || out_index == NULL || out_ok == NULL ||
        !pnl_prompt_ready(app))
        return PNL_ERR_ARG;
    for (i = 0; i < items_len; i++) {
        if (items[i] == NULL)
            return PNL_ERR_ARG;
    }
    *out_ok = 0;
    *out_index = 0;
    if (app->cli)
        return pnl_pick_cli(title, items, items_len, out_index, out_ok);
    memset(&view, 0, sizeof(view));
    view.items = items;
    view.items_len = items_len;
    view.shown = pnl_mem_alloc(&app->mem, items_len * sizeof(*view.shown));
    if (view.shown == NULL)
        return PNL_ERR_NOMEM;
    pnl_edit_init(&view.edit, view.filter, sizeof(view.filter));
    pnl_pick_filter(&view);
    pnl_pick_recall(app, title, &view);
    status = pnl_prompt_modal(app, title, PNL_PICK_FOOTER, pnl_pick_width(items, items_len),
                              items_len + 2 < 16 ? (int)items_len + 2 : 16, pnl_pick_draw, pnl_pick_key, &view);
    if (view.ok) {
        *out_index = view.shown[view.selected];
        *out_ok = 1;
        pnl_memory_set(app, "pick", title, items[*out_index]);
    }
    pnl_mem_free(&app->mem, view.shown);
    return status;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PNL_RECORD_START "\x1b[?25l\x1b[?7l"

struct pnl_recorder {
    FILE *file;
    long long start_ms;
    int width;
    int height;
};

static struct pnl_recorder pnl_recorder;

static void pnl_record_text(FILE *file, const char *text, size_t text_len)
{
    size_t i;

    fputc('"', file);
    for (i = 0; i < text_len; i++) {
        unsigned char c = (unsigned char)text[i];

        if (c == '"' || c == '\\')
            fprintf(file, "\\%c", c);
        else if (c < 0x20 || c == 0x7f)
            fprintf(file, "\\u%04x", c);
        else
            fputc(c, file);
    }
    fputc('"', file);
}

static void pnl_record_event(char kind, const char *data, size_t data_len)
{
    long long elapsed = pnl_term_now_ms() - pnl_recorder.start_ms;

    fprintf(pnl_recorder.file, "[%lld.%03lld, \"%c\", ", elapsed / 1000, elapsed % 1000, kind);
    pnl_record_text(pnl_recorder.file, data, data_len);
    fputs("]\n", pnl_recorder.file);
}

void pnl_record_open(pnl_app_t *app)
{
    const char *path = app->record_path != NULL ? app->record_path : getenv("TUIKIT_RECORD");

    if (path == NULL || path[0] == '\0' || pnl_recorder.file != NULL)
        return;
    pnl_recorder.file = fopen(path, "wb");
    if (pnl_recorder.file == NULL)
        return;
    pnl_term_size(&pnl_recorder.width, &pnl_recorder.height);
    pnl_recorder.start_ms = pnl_term_now_ms();
    fprintf(pnl_recorder.file, "{\"version\": 2, \"width\": %d, \"height\": %d", pnl_recorder.width,
            pnl_recorder.height);
    if (!pnl_virtual_active())
        fprintf(pnl_recorder.file, ", \"timestamp\": %lld", (long long)time(NULL));
    fputs(", \"title\": ", pnl_recorder.file);
    pnl_record_text(pnl_recorder.file, app->title, strlen(app->title));
    fputs("}\n", pnl_recorder.file);
    pnl_record_event('o', PNL_RECORD_START, sizeof(PNL_RECORD_START) - 1);
}

void pnl_record_output(const char *data, size_t data_len, int width, int height)
{
    if (pnl_recorder.file == NULL)
        return;
    if (width != pnl_recorder.width || height != pnl_recorder.height) {
        char size[32];
        int len = snprintf(size, sizeof(size), "%dx%d", width, height);

        pnl_recorder.width = width;
        pnl_recorder.height = height;
        pnl_record_event('r', size, (size_t)len);
    }
    pnl_record_event('o', data, data_len);
}

void pnl_record_close(void)
{
    if (pnl_recorder.file == NULL)
        return;
    fclose(pnl_recorder.file);
    pnl_recorder.file = NULL;
}

int pnl_app_record(pnl_app_t *app, const char *path)
{
    char *copy = NULL;

    if (app == NULL || (path != NULL && path[0] == '\0'))
        return PNL_ERR_ARG;
    if (path != NULL && (copy = pnl_mem_copy(&app->mem, path)) == NULL)
        return PNL_ERR_NOMEM;
    pnl_mem_free(&app->mem, app->record_path);
    app->record_path = copy;
    return PNL_OK;
}

#include <stdio.h>
#include <string.h>

#define PNL_CONTINUATION 0u
#define PNL_BLANK ' '

int pnl_screen_resize(struct pnl_screen *screen, const struct pnl_mem *mem, int width, int height)
{
    size_t count = (size_t)width * (size_t)height;
    struct pnl_cell *back;
    struct pnl_cell *front;

    if (width <= 0 || height <= 0)
        return PNL_ERR_ARG;
    if (screen->link_pool == NULL) {
        screen->link_pool = pnl_mem_alloc(mem, PNL_LINK_POOL + PNL_LINKS * sizeof(*screen->link_at));
        if (screen->link_pool == NULL)
            return PNL_ERR_NOMEM;
        screen->link_at = (unsigned short *)(void *)(screen->link_pool + PNL_LINK_POOL);
    }
    if (width == screen->width && height == screen->height)
        return PNL_OK;
    back = pnl_mem_alloc(mem, count * sizeof(*back));
    front = pnl_mem_alloc(mem, count * sizeof(*front));
    if (back == NULL || front == NULL) {
        pnl_mem_free(mem, back);
        pnl_mem_free(mem, front);
        return PNL_ERR_NOMEM;
    }
    pnl_mem_free(mem, screen->back);
    pnl_mem_free(mem, screen->front);
    screen->back = back;
    screen->front = front;
    screen->width = width;
    screen->height = height;
    screen->front_valid = 0;
    pnl_screen_clear(screen);
    return PNL_OK;
}

void pnl_screen_free(struct pnl_screen *screen, const struct pnl_mem *mem)
{
    pnl_mem_free(mem, screen->back);
    pnl_mem_free(mem, screen->front);
    pnl_mem_free(mem, screen->out);
    pnl_mem_free(mem, screen->link_pool);
    memset(screen, 0, sizeof(*screen));
}

void pnl_screen_clear(struct pnl_screen *screen)
{
    size_t count = (size_t)screen->width * (size_t)screen->height;
    size_t i;

    for (i = 0; i < count; i++) {
        screen->back[i].cp = PNL_BLANK;
        screen->back[i].style = 0;
    }
    screen->links_len = 0;
    screen->link_pool_len = 0;
    screen->links_hash = 5381;
}

static uint32_t pnl_link_add(struct pnl_screen *screen, const char *url, size_t url_len)
{
    int i;

    if (screen->links_off || url_len == 0 || screen->link_pool == NULL)
        return 0;
    for (i = 0; i < screen->links_len; i++) {
        const char *known = screen->link_pool + screen->link_at[i];

        if (strncmp(known, url, url_len) == 0 && known[url_len] == '\0')
            return (uint32_t)(i + 1) << 24;
    }
    if (screen->links_len == PNL_LINKS || screen->link_pool_len + url_len + 1 > PNL_LINK_POOL)
        return 0;
    screen->link_at[screen->links_len++] = (unsigned short)screen->link_pool_len;
    memcpy(screen->link_pool + screen->link_pool_len, url, url_len);
    screen->link_pool_len += url_len;
    screen->link_pool[screen->link_pool_len++] = '\0';
    for (i = 0; i < (int)url_len; i++)
        screen->links_hash = screen->links_hash * 33u + (unsigned char)url[i];
    screen->links_hash = screen->links_hash * 33u + (unsigned long)screen->links_len;
    return (uint32_t)screen->links_len << 24;
}

static size_t pnl_url_length(const char *text, size_t len)
{
    size_t end;

    if (!(len > 8 && strncmp(text, "https://", 8) == 0) && !(len > 7 && strncmp(text, "http://", 7) == 0))
        return 0;
    for (end = 0; end < len && (unsigned char)text[end] > 0x20 && text[end] != 0x7f && text[end] != '"' &&
                  text[end] != '<' && text[end] != '>';
         end++)
        ;
    while (end > 0 && (strchr)(".,;:!?)]'", text[end - 1]) != NULL)
        end--;
    return end;
}

static struct pnl_cell *pnl_screen_at(struct pnl_screen *screen, int x, int y)
{
    return &screen->back[(size_t)y * (size_t)screen->width + (size_t)x];
}

static void pnl_screen_put(struct pnl_screen *screen, int x, int y, uint32_t cp, uint32_t style)
{
    struct pnl_cell *cell = pnl_screen_at(screen, x, y);

    if (cell->cp == PNL_CONTINUATION && x > 0)
        pnl_screen_at(screen, x - 1, y)->cp = PNL_BLANK;
    if (x + 1 < screen->width && pnl_screen_at(screen, x + 1, y)->cp == PNL_CONTINUATION)
        pnl_screen_at(screen, x + 1, y)->cp = PNL_BLANK;
    cell->cp = cp;
    cell->style = style;
}

int pnl_screen_link(struct pnl_screen *screen, int x, int y, int max, uint32_t style, const char *text,
                    const char *url)
{
    size_t len = strlen(text);
    size_t pos = 0;
    size_t link_end = 0;
    uint32_t link = 0;
    int end;

    if (y < 0 || y >= screen->height || x < 0 || x >= screen->width)
        return 0;
    if (url != NULL) {
        link = pnl_link_add(screen, url, strlen(url));
        link_end = len;
    }
    end = max >= 0 && x + max < screen->width ? x + max : screen->width;
    while (pos < len && x < end) {
        uint32_t cp;
        int w;

        if (pos >= link_end) {
            link_end = url == NULL && (text[pos] == 'h') ? pnl_url_length(text + pos, len - pos) : 0;
            link = link_end > 0 ? pnl_link_add(screen, text + pos, link_end) : 0;
            link_end += pos;
        }
        pos += pnl_utf8_next(text + pos, len - pos, &cp);
        w = pnl_cp_width(cp);
        if (w == 0)
            continue;
        if (w < 0) {
            cp = 0xfffd;
            w = 1;
        }
        if (x + w > end)
            break;
        pnl_screen_put(screen, x, y, cp, (style & 0xffffffu) | link);
        if (w == 2)
            pnl_screen_put(screen, x + 1, y, PNL_CONTINUATION, (style & 0xffffffu) | link);
        x += w;
    }
    return x;
}

int pnl_screen_text(struct pnl_screen *screen, int x, int y, int max, uint32_t style, const char *text)
{
    return pnl_screen_link(screen, x, y, max, style, text, NULL);
}

void pnl_screen_fill(struct pnl_screen *screen, int x, int y, int width, uint32_t style)
{
    int end = x + width < screen->width ? x + width : screen->width;

    if (y < 0 || y >= screen->height)
        return;
    for (x = x < 0 ? 0 : x; x < end; x++)
        pnl_screen_put(screen, x, y, PNL_BLANK, style);
}

static int pnl_out(struct pnl_screen *screen, const struct pnl_mem *mem, const char *data, size_t data_len)
{
    int status = pnl_mem_grow(mem, (void **)&screen->out, &screen->out_cap, screen->out_len + data_len, 1);

    if (status != PNL_OK)
        return status;
    memcpy(screen->out + screen->out_len, data, data_len);
    screen->out_len += data_len;
    return PNL_OK;
}

static int pnl_out_style(struct pnl_screen *screen, const struct pnl_mem *mem, uint32_t style)
{
    char buf[64] = "\x1b[0";
    int len = 3;

    if (style & PNL_BOLD)
        len += sprintf(buf + len, ";1");
    if (style & PNL_DIM)
        len += sprintf(buf + len, ";2");
    if (style & PNL_UNDERLINE)
        len += sprintf(buf + len, ";4");
    if ((style & PNL_REVERSE) || (screen->palette.depth == PNL_DEPTH_NONE && PNL_STYLE_BG(style) != PNL_COLOR_DEFAULT))
        len += sprintf(buf + len, ";7");
    if (screen->palette.depth != PNL_DEPTH_NONE) {
        len += pnl_color_ink(buf + len, PNL_STYLE_FG(style), 0, &screen->palette);
        len += pnl_color_ink(buf + len, PNL_STYLE_BG(style), 1, &screen->palette);
    }
    buf[len++] = 'm';
    return pnl_out(screen, mem, buf, (size_t)len);
}

static int pnl_out_cp(struct pnl_screen *screen, const struct pnl_mem *mem, uint32_t cp)
{
    char buf[4];

    return pnl_out(screen, mem, buf, pnl_utf8_put(cp, buf));
}

static int pnl_cell_same(const struct pnl_cell *a, const struct pnl_cell *b)
{
    return a->cp == b->cp && a->style == b->style;
}

struct pnl_pen {
    int x;
    int y;
    uint32_t style;
    int style_known;
    uint32_t link;
};

static int pnl_out_link(struct pnl_screen *screen, const struct pnl_mem *mem, struct pnl_pen *pen, uint32_t link)
{
    const char *url = link > 0 ? screen->link_pool + screen->link_at[link - 1] : "";
    int status;

    pen->link = link;
    status = pnl_out(screen, mem, "\x1b]8;;", 5);
    if (status == PNL_OK)
        status = pnl_out(screen, mem, url, strlen(url));
    if (status == PNL_OK)
        status = pnl_out(screen, mem, "\x1b\\", 2);
    return status;
}

static int pnl_render_cell(struct pnl_screen *screen, const struct pnl_mem *mem, struct pnl_pen *pen, int x, int y)
{
    const struct pnl_cell *cell = pnl_screen_at(screen, x, y);
    int status = PNL_OK;

    if (pen->x != x || pen->y != y) {
        char buf[32];
        int len = sprintf(buf, "\x1b[%d;%dH", y + 1, x + 1);

        status = pnl_out(screen, mem, buf, (size_t)len);
    }
    if (status == PNL_OK && pen->link != PNL_STYLE_LINK(cell->style))
        status = pnl_out_link(screen, mem, pen, PNL_STYLE_LINK(cell->style));
    if (status == PNL_OK && (!pen->style_known || pen->style != cell->style))
        status = pnl_out_style(screen, mem, cell->style);
    if (status == PNL_OK)
        status = pnl_out_cp(screen, mem, cell->cp);
    pen->style = cell->style;
    pen->style_known = 1;
    pen->y = y;
    pen->x = x + (pnl_cp_width(cell->cp) == 2 ? 2 : 1);
    return status;
}

static int pnl_render_row(struct pnl_screen *screen, const struct pnl_mem *mem, struct pnl_pen *pen, int y)
{
    int x;
    int status = PNL_OK;

    for (x = 0; x < screen->width && status == PNL_OK; x++) {
        size_t at = (size_t)y * (size_t)screen->width + (size_t)x;
        int wide = x + 1 < screen->width && screen->back[at + 1].cp == PNL_CONTINUATION;

        if (screen->back[at].cp == PNL_CONTINUATION)
            continue;
        if (screen->front_valid && pnl_cell_same(&screen->back[at], &screen->front[at]) &&
            (!wide || pnl_cell_same(&screen->back[at + 1], &screen->front[at + 1])))
            continue;
        status = pnl_render_cell(screen, mem, pen, x, y);
    }
    return status;
}

static int pnl_linear_same(const struct pnl_screen *screen, int y, int from)
{
    const struct pnl_cell *old = screen->front + (size_t)from * (size_t)screen->width;
    const struct pnl_cell *now = screen->back + (size_t)y * (size_t)screen->width;
    int x;

    for (x = 0; x < screen->width; x++) {
        if (old[x].cp != now[x].cp && !(from == y && old[x].cp == '>' && now[x].cp == ' '))
            return 0;
    }
    return 1;
}

static int pnl_linear_seen(const struct pnl_screen *screen, int y)
{
    int from;

    for (from = 0; from < screen->height; from++) {
        if (pnl_linear_same(screen, y, from))
            return 1;
    }
    return 0;
}

static int pnl_linear_box(uint32_t cp)
{
    return cp >= 0x2500 && cp <= 0x257f;
}

static int pnl_linear_skip(uint32_t cp)
{
    return cp == PNL_CONTINUATION || (cp >= 0x2500 && cp <= 0x259f) || (cp >= 0x2800 && cp <= 0x28ff);
}

static int pnl_linear_row(struct pnl_screen *screen, const struct pnl_mem *mem, int y)
{
    const struct pnl_cell *row = screen->back + (size_t)y * (size_t)screen->width;
    int status = PNL_OK;
    int spaces = 0;
    int written = 0;
    int start = 0;
    int end = screen->width;
    int x;

    while (start < screen->width && !pnl_linear_box(row[start].cp))
        start++;
    while (end > start + 1 && !pnl_linear_box(row[end - 1].cp))
        end--;
    if (end <= start + 1) {
        start = 0;
        end = screen->width;
    }
    for (x = start; x < end && status == PNL_OK; x++) {
        uint32_t cp = pnl_linear_skip(row[x].cp) ? ' ' : row[x].cp;

        if (cp == ' ') {
            spaces++;
            continue;
        }
        if (written && spaces > 0)
            status = pnl_out(screen, mem, "  ", spaces > 1 ? 2 : 1);
        if (status == PNL_OK)
            status = pnl_out_cp(screen, mem, cp);
        spaces = 0;
        written = 1;
    }
    if (status == PNL_OK && written)
        status = pnl_out(screen, mem, "\r\n", 2);
    return status;
}

static int pnl_linear_render(struct pnl_screen *screen, const struct pnl_mem *mem)
{
    int status = PNL_OK;
    int y;

    for (y = 0; y < screen->height && status == PNL_OK; y++) {
        if (!screen->front_valid || !pnl_linear_seen(screen, y))
            status = pnl_linear_row(screen, mem, y);
    }
    return status;
}

static int pnl_diff_render(struct pnl_screen *screen, const struct pnl_mem *mem)
{
    struct pnl_pen pen = {-1, -1, 0, 0, 0};
    int status = PNL_OK;
    int y;

    if (!screen->front_valid)
        status = pnl_out(screen, mem, "\x1b[0m\x1b[2J", 8);
    for (y = 0; y < screen->height && status == PNL_OK; y++)
        status = pnl_render_row(screen, mem, &pen, y);
    if (status == PNL_OK && pen.link > 0)
        status = pnl_out_link(screen, mem, &pen, 0);
    if (status == PNL_OK && pen.style_known)
        status = pnl_out(screen, mem, "\x1b[0m", 4);
    return status;
}

int pnl_screen_render(struct pnl_screen *screen, const struct pnl_mem *mem)
{
    size_t count = (size_t)screen->width * (size_t)screen->height;
    int status;

    screen->out_len = 0;
    if (screen->links_hash != screen->links_drawn)
        screen->front_valid = 0;
    screen->links_drawn = screen->links_hash;
    status = screen->linear ? pnl_linear_render(screen, mem) : pnl_diff_render(screen, mem);
    if (status == PNL_OK && screen->out_len > 0)
        status = pnl_term_write(screen->out, screen->out_len);
    if (status != PNL_OK) {
        screen->front_valid = 0;
        return status;
    }
    if (screen->out_len > 0)
        pnl_record_output(screen->out, screen->out_len, screen->width, screen->height);
    memcpy(screen->front, screen->back, count * sizeof(*screen->front));
    screen->front_valid = 1;
    return PNL_OK;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_TABLE_GAP 2
#define PNL_TABLE_COLUMN_MAX 40
#define PNL_TABLE_MEASURE_ROWS 2000
#define PNL_TABLE_SYNC_ROWS 20000
#define PNL_TABLE_POLL_MS 20
#define PNL_TABLE_CHECK_EVERY 4096
#define PNL_TABLE_FILTER_MAX 128
#define PNL_TABLE_FOOTER "up/down: move  left/right: column  s: sort  /: filter  q: back"
#define PNL_TABLE_FILTER_FOOTER "type to filter  up/down: move  enter: keep  esc: clear"

struct pnl_table_job {
    char filter[PNL_TABLE_FILTER_MAX];
    size_t column;
    int descending;
    int sorted;
};

struct pnl_table {
    pnl_app_t *app;
    char *name;
    size_t columns;
    size_t rows;
    char **headers;
    char **cells;
    pnl_cell_fn cell_fn;
    pnl_release_fn release_fn;
    void *ctx;
    uint32_t *order;
    size_t order_len;
    uint32_t *work;
    size_t work_len;
    uint32_t *scratch;
    double *keys;
    unsigned char *numeric;
    int *widths;
    int *flags;
    unsigned char *hidden;
    size_t hidden_len;
    size_t selected;
    size_t top;
    size_t column;
    struct pnl_table_job want;
    struct pnl_table_job job;
    struct pnl_edit edit;
    int editing;
    const char *footer;
    struct pnl_lock lock;
    int lock_ready;
    struct pnl_thread thread;
    int busy;
    int poll_timer;
    int done;
    int cancel;
};

struct pnl_array_cells {
    const char *const *cells;
    size_t columns;
};

static const char *pnl_table_cell(const struct pnl_table *table, size_t row, size_t column, char *buf)
{
    const char *text;

    buf[0] = '\0';
    text = table->cells != NULL ? table->cells[row * table->columns + column]
                                : table->cell_fn(table->ctx, row, column, buf, PNL_CELL_MAX);

    return text != NULL ? text : "";
}

static int pnl_table_cancelled(struct pnl_table *table, size_t step)
{
    int cancel;

    if (step % PNL_TABLE_CHECK_EVERY != 0)
        return 0;
    pnl_lock(&table->lock);
    cancel = table->cancel;
    pnl_unlock(&table->lock);
    return cancel;
}

static int pnl_table_number(const char *text, double *out)
{
    char *end;

    if (text[0] == '\0')
        return 0;
    *out = strtod(text, &end);
    while (*end == ' ' || *end == '%')
        end++;
    return *end == '\0';
}

static int pnl_table_compare(const struct pnl_table *table, size_t a, size_t b)
{
    char left_buf[PNL_CELL_MAX];
    char right_buf[PNL_CELL_MAX];
    int result;

    if (table->numeric[a] && table->numeric[b]) {
        result = table->keys[a] < table->keys[b] ? -1 : table->keys[a] > table->keys[b];
    } else {
        result = strcmp(pnl_table_cell(table, a, table->job.column, left_buf),
                        pnl_table_cell(table, b, table->job.column, right_buf));
    }
    return table->job.descending ? -result : result;
}

static int pnl_table_keys(struct pnl_table *table)
{
    char buf[PNL_CELL_MAX];
    size_t i;

    for (i = 0; i < table->work_len; i++) {
        size_t row = table->work[i];

        if (pnl_table_cancelled(table, i))
            return 0;
        table->numeric[row] =
            (unsigned char)pnl_table_number(pnl_table_cell(table, row, table->job.column, buf), &table->keys[row]);
    }
    return 1;
}

static int pnl_table_merge(struct pnl_table *table, size_t from, size_t middle, size_t to)
{
    size_t i = from;
    size_t j = middle;
    size_t k = from;

    while (i < middle && j < to) {
        if (pnl_table_cancelled(table, k))
            return 0;
        table->scratch[k++] = pnl_table_compare(table, table->work[j], table->work[i]) < 0 ? table->work[j++]
                                                                                          : table->work[i++];
    }
    while (i < middle)
        table->scratch[k++] = table->work[i++];
    while (j < to)
        table->scratch[k++] = table->work[j++];
    memcpy(table->work + from, table->scratch + from, (to - from) * sizeof(*table->work));
    return 1;
}

static int pnl_table_sort(struct pnl_table *table)
{
    size_t len = table->work_len;
    size_t width;
    size_t from;

    for (width = 1; width < len; width *= 2) {
        for (from = 0; from + width < len; from += 2 * width) {
            size_t to = from + 2 * width < len ? from + 2 * width : len;

            if (!pnl_table_merge(table, from, from + width, to))
                return 0;
        }
    }
    return 1;
}

static int pnl_table_matches(const struct pnl_table *table, size_t row)
{
    char buf[PNL_CELL_MAX];
    size_t c;

    for (c = 0; c < table->columns; c++) {
        if (pnl_text_contains(pnl_table_cell(table, row, c, buf), table->job.filter))
            return 1;
    }
    return 0;
}

static int pnl_table_filter(struct pnl_table *table)
{
    size_t i;

    table->work_len = 0;
    for (i = 0; i < table->rows; i++) {
        if (pnl_table_cancelled(table, i))
            return 0;
        if (table->job.filter[0] == '\0' || pnl_table_matches(table, i))
            table->work[table->work_len++] = (uint32_t)i;
    }
    return 1;
}

static void pnl_table_compute(struct pnl_table *table)
{
    if (pnl_table_filter(table) && table->job.sorted && pnl_table_keys(table))
        pnl_table_sort(table);
}

static void pnl_table_worker(void *arg)
{
    struct pnl_table *table = arg;

    pnl_table_compute(table);
    pnl_lock(&table->lock);
    table->done = 1;
    pnl_unlock(&table->lock);
    pnl_term_wake(&table->app->term);
}

static size_t pnl_table_find(const struct pnl_table *table, size_t row)
{
    size_t i;

    for (i = 0; i < table->order_len; i++) {
        if (table->order[i] == row)
            return i;
    }
    return 0;
}

static void pnl_table_apply(struct pnl_table *table)
{
    uint32_t *swap = table->order;
    int kept = table->selected > 0 && table->selected < table->order_len;
    size_t row = kept ? table->order[table->selected] : 0;

    table->order = table->work;
    table->order_len = table->work_len;
    table->work = swap;
    table->selected = kept ? pnl_table_find(table, row) : 0;
    table->app->dirty = 1;
}

static void pnl_table_finish(struct pnl_table *table)
{
    int cancelled;

    pnl_thread_join(&table->thread);
    pnl_mem_free(&table->app->mem, table->scratch);
    table->scratch = NULL;
    table->busy = 0;
    table->app->tasks_running--;
    pnl_app_remove_timer(table->app, table->poll_timer);
    cancelled = table->cancel;
    table->cancel = 0;
    table->done = 0;
    if (!cancelled)
        pnl_table_apply(table);
}

static void pnl_table_stop(struct pnl_table *table)
{
    if (!table->busy)
        return;
    pnl_lock(&table->lock);
    table->cancel = 1;
    pnl_unlock(&table->lock);
    pnl_table_finish(table);
}

static int pnl_table_buffers(struct pnl_table *table)
{
    size_t count = table->rows ? table->rows : 1;

    if (table->work == NULL)
        table->work = pnl_mem_alloc(&table->app->mem, count * sizeof(*table->work));
    if (table->scratch == NULL)
        table->scratch = pnl_mem_alloc(&table->app->mem, count * sizeof(*table->scratch));
    if (table->keys == NULL)
        table->keys = pnl_mem_alloc(&table->app->mem, count * sizeof(*table->keys));
    if (table->numeric == NULL)
        table->numeric = pnl_mem_alloc(&table->app->mem, count);
    return table->work != NULL && table->scratch != NULL && table->keys != NULL && table->numeric != NULL;
}

static void pnl_table_poll(struct pnl_table *table)
{
    int done;

    if (!table->busy)
        return;
    pnl_lock(&table->lock);
    done = table->done;
    pnl_unlock(&table->lock);
    if (done)
        pnl_table_finish(table);
}

static void pnl_table_tick(void *ctx, pnl_app_t *app)
{
    (void)app;
    pnl_table_poll(ctx);
}

static void pnl_table_start(struct pnl_table *table)
{
    pnl_table_stop(table);
    if (!pnl_table_buffers(table))
        return;
    table->job = table->want;
    if (table->rows > PNL_TABLE_SYNC_ROWS && pnl_term_wake_open(&table->app->term) == PNL_OK &&
        pnl_thread_start(&table->thread, pnl_table_worker, table) == PNL_OK) {
        table->busy = 1;
        table->app->tasks_running++;
        pnl_app_add_timer(table->app, PNL_TABLE_POLL_MS, pnl_table_tick, table, &table->poll_timer);
        return;
    }
    pnl_table_compute(table);
    pnl_mem_free(&table->app->mem, table->scratch);
    table->scratch = NULL;
    pnl_table_apply(table);
}


static void pnl_table_widen(struct pnl_table *table, size_t row)
{
    char buf[PNL_CELL_MAX];
    size_t c;

    for (c = 0; c < table->columns; c++) {
        int width = pnl_text_width(pnl_table_cell(table, row, c, buf));

        if (width > table->widths[c])
            table->widths[c] = width > PNL_TABLE_COLUMN_MAX ? PNL_TABLE_COLUMN_MAX : width;
    }
}

static int pnl_table_total(const struct pnl_table *table)
{
    int total = 1;
    size_t c;

    for (c = 0; c < table->columns; c++) {
        if (!table->hidden[c])
            total += table->widths[c] + PNL_TABLE_GAP;
    }
    return total;
}

static void pnl_table_fit(struct pnl_table *table, int width)
{
    size_t c = table->columns;

    memset(table->hidden, 0, table->columns);
    table->hidden_len = 0;
    while (c-- > 0 && pnl_table_total(table) > width) {
        if (table->flags[c] & PNL_COLUMN_OPTIONAL) {
            table->hidden[c] = 1;
            table->hidden_len++;
        }
    }
    while (table->column > 0 && table->hidden[table->column])
        table->column--;
}

static int pnl_table_text(pnl_canvas_t *canvas, const struct pnl_table *table, size_t c, int x, int y, int style,
                          const char *text)
{
    int pad = table->flags[c] & PNL_COLUMN_RIGHT ? table->widths[c] - pnl_text_width(text) : 0;

    return pnl_canvas_clip(canvas, x + (pad > 0 ? pad : 0), y, table->widths[c], style, text);
}

static void pnl_table_marker(pnl_canvas_t *canvas, const struct pnl_table *table, size_t c, int end)
{
    const char *marker = table->want.descending ? "v" : "^";

    if (table->flags[c] & PNL_COLUMN_RIGHT)
        end -= pnl_text_width(table->headers[c]) + 3;
    pnl_canvas_text(canvas, end + 1, 0, PNL_FG(PNL_ROLE_MUTED), marker);
}

static void pnl_table_draw_header(struct pnl_table *table, pnl_canvas_t *canvas)
{
    char more[24];
    size_t c;
    int x = 1;

    for (c = 0; c < table->columns; c++) {
        int style = PNL_BOLD | (c == table->column ? PNL_UNDERLINE : 0);
        int end;

        if (table->hidden[c])
            continue;
        end = pnl_table_text(canvas, table, c, x, 0, style, table->headers[c]);
        if (table->want.sorted && c == table->want.column)
            pnl_table_marker(canvas, table, c, end);
        x += table->widths[c] + PNL_TABLE_GAP;
    }
    if (table->hidden_len == 0)
        return;
    snprintf(more, sizeof(more), "+%zu", table->hidden_len);
    pnl_canvas_text(canvas, pnl_canvas_width(canvas) - (int)strlen(more), 0, PNL_FG(PNL_ROLE_MUTED), more);
}

static void pnl_table_draw_filter(struct pnl_table *table, pnl_canvas_t *canvas)
{
    char count[64];
    int width = pnl_canvas_width(canvas);

    if (table->busy)
        snprintf(count, sizeof(count), "%s...", table->job.filter[0] != '\0' ? "filtering" : "sorting");
    else
        snprintf(count, sizeof(count), "%zu of %zu", table->order_len, table->rows);
    if (!table->editing && table->want.filter[0] == '\0' && !table->busy)
        return;
    pnl_canvas_text(canvas, width - (int)strlen(count) - 1, 1, PNL_FG(PNL_ROLE_MUTED), count);
    if (!table->editing && table->want.filter[0] == '\0')
        return;
    pnl_canvas_text(canvas, 1, 1, PNL_FG(PNL_ROLE_ACCENT), "/");
    pnl_edit_draw(&table->edit, canvas, 3, 1, width - (int)strlen(count) - 5, table->editing);
}

static void pnl_table_draw_row(struct pnl_table *table, pnl_canvas_t *canvas, int y, size_t index)
{
    int selected = index == table->selected;
    size_t row = table->order[index];
    char buf[PNL_CELL_MAX];
    size_t c;
    int x = 1;

    if (selected)
        pnl_canvas_fill(canvas, 0, y, pnl_canvas_width(canvas), 1, PNL_REVERSE);
    for (c = 0; c < table->columns; c++) {
        if (table->hidden[c])
            continue;
        pnl_table_text(canvas, table, c, x, y, selected ? PNL_REVERSE : 0, pnl_table_cell(table, row, c, buf));
        pnl_canvas_fill(canvas, x + table->widths[c], y, PNL_TABLE_GAP, 1, selected ? PNL_REVERSE : 0);
        x += table->widths[c] + PNL_TABLE_GAP;
    }
}

static void pnl_table_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_table *table = ctx;
    int rows = pnl_canvas_height(canvas) - 2;
    size_t end;
    size_t i;

    (void)app;
    pnl_table_poll(table);
    if (table->order_len > 0 && table->selected >= table->order_len)
        table->selected = table->order_len - 1;
    if (rows > 0 && table->selected < table->top)
        table->top = table->selected;
    if (rows > 0 && table->selected >= table->top + (size_t)rows)
        table->top = table->selected - (size_t)rows + 1;
    if (table->top > table->order_len)
        table->top = 0;
    end = rows > 0 && table->top + (size_t)rows < table->order_len ? table->top + (size_t)rows : table->order_len;
    for (i = table->top; i < end; i++)
        pnl_table_widen(table, table->order[i]);
    pnl_table_fit(table, pnl_canvas_width(canvas));
    pnl_table_draw_header(table, canvas);
    pnl_table_draw_filter(table, canvas);
    for (i = table->top; i < end; i++)
        pnl_table_draw_row(table, canvas, 2 + (int)(i - table->top), i);
    if (table->order_len == 0 && !table->busy && rows > 0 && table->want.filter[0] != '\0')
        pnl_canvas_text(canvas, 1, 2, PNL_FG(PNL_ROLE_MUTED), "no match");
}

static void pnl_table_move(struct pnl_table *table, int key)
{
    size_t step = key == PNL_KEY_PAGE_UP || key == PNL_KEY_PAGE_DOWN ? 10 : 1;

    if (table->order_len == 0)
        return;
    if (key == PNL_KEY_HOME)
        table->selected = 0;
    else if (key == PNL_KEY_END)
        table->selected = table->order_len - 1;
    else if (key == PNL_KEY_UP || key == 'k' || key == PNL_KEY_PAGE_UP)
        table->selected = table->selected > step ? table->selected - step : 0;
    else
        table->selected = table->selected + step < table->order_len ? table->selected + step : table->order_len - 1;
}

static void pnl_table_step_column(struct pnl_table *table, int direction)
{
    size_t c = table->column;

    while ((direction < 0 && c > 0) || (direction > 0 && c + 1 < table->columns)) {
        c = direction < 0 ? c - 1 : c + 1;
        if (!table->hidden[c]) {
            table->column = c;
            return;
        }
    }
}

static void pnl_table_sort_column(struct pnl_table *table)
{
    table->want.descending = table->want.sorted && table->want.column == table->column && !table->want.descending;
    table->want.column = table->column;
    table->want.sorted = 1;
    pnl_table_start(table);
}

static void pnl_table_click(struct pnl_table *table, pnl_app_t *app)
{
    size_t c;
    int x;
    int y;
    int left = 1;

    pnl_mouse_position(app, &x, &y);
    if (y >= 2 && table->top + (size_t)(y - 2) < table->order_len)
        table->selected = table->top + (size_t)(y - 2);
    for (c = 0; y == 0 && c < table->columns; c++) {
        if (table->hidden[c])
            continue;
        if (x >= left && x < left + table->widths[c] + PNL_TABLE_GAP) {
            table->column = c;
            pnl_table_sort_column(table);
            return;
        }
        left += table->widths[c] + PNL_TABLE_GAP;
    }
}

static int pnl_table_filter_key(struct pnl_table *table, int key)
{
    if (key == PNL_KEY_UP || key == PNL_KEY_DOWN || key == PNL_KEY_PAGE_UP || key == PNL_KEY_PAGE_DOWN) {
        pnl_table_move(table, key);
        return 1;
    }
    if (key == PNL_KEY_ENTER || key == PNL_KEY_ESCAPE) {
        table->editing = 0;
        table->footer = PNL_TABLE_FOOTER;
        if (key == PNL_KEY_ENTER)
            return 1;
        table->want.filter[0] = '\0';
        pnl_edit_init(&table->edit, table->want.filter, sizeof(table->want.filter));
        pnl_table_start(table);
        return 1;
    }
    if (pnl_edit_key(&table->edit, key))
        pnl_table_start(table);
    return 1;
}

static int pnl_table_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_table *table = ctx;

    if (table->editing && !pnl_is_mouse_key(key))
        return pnl_table_filter_key(table, key);
    if (key == PNL_KEY_CLICK)
        pnl_table_click(table, app);
    else if (key == '/') {
        table->editing = 1;
        table->footer = PNL_TABLE_FILTER_FOOTER;
    } else if (key == PNL_KEY_LEFT || key == 'h' || key == PNL_KEY_RIGHT || key == 'l')
        pnl_table_step_column(table, key == PNL_KEY_LEFT || key == 'h' ? -1 : 1);
    else if (key == 's')
        pnl_table_sort_column(table);
    else if (key == PNL_KEY_UP || key == PNL_KEY_DOWN || key == 'j' || key == 'k' ||
             (key >= PNL_KEY_PAGE_UP && key <= PNL_KEY_END))
        pnl_table_move(table, key);
    else
        return 0;
    return 1;
}

static void pnl_table_remember(struct pnl_table *table)
{
    char text[PNL_TABLE_FILTER_MAX + 64];

    if (table->name == NULL)
        return;
    snprintf(text, sizeof(text), "%d %zu %d %s", table->want.sorted, table->want.column, table->want.descending,
             table->want.filter);
    pnl_memory_set(table->app, "table", table->name, text);
}

static void pnl_table_recall(struct pnl_table *table, const char *title)
{
    const char *text = pnl_memory_get(table->app, "table", title);
    struct pnl_table_job job;
    int used = 0;

    table->name = pnl_mem_copy(&table->app->mem, title);
    memset(&job, 0, sizeof(job));
    if (text == NULL || sscanf(text, "%d %zu %d %n", &job.sorted, &job.column, &job.descending, &used) != 3 ||
        job.column >= table->columns)
        return;
    snprintf(job.filter, sizeof(job.filter), "%s", text + used);
    if (!job.sorted && job.filter[0] == '\0')
        return;
    table->want = job;
    table->column = job.column;
    pnl_edit_init(&table->edit, table->want.filter, sizeof(table->want.filter));
    pnl_table_start(table);
}

static void pnl_table_drop(pnl_app_t *app, void *ctx)
{
    struct pnl_table *table = ctx;
    size_t i;

    pnl_table_stop(table);
    pnl_table_remember(table);
    pnl_mem_free(&app->mem, table->name);
    if (table->release_fn != NULL)
        table->release_fn(table->ctx);
    for (i = 0; table->headers != NULL && i < table->columns; i++)
        pnl_mem_free(&app->mem, table->headers[i]);
    for (i = 0; table->cells != NULL && i < table->rows * table->columns; i++)
        pnl_mem_free(&app->mem, table->cells[i]);
    if (table->lock_ready)
        pnl_lock_destroy(&table->lock);
    pnl_mem_free(&app->mem, table->headers);
    pnl_mem_free(&app->mem, table->cells);
    pnl_mem_free(&app->mem, table->order);
    pnl_mem_free(&app->mem, table->work);
    pnl_mem_free(&app->mem, table->scratch);
    pnl_mem_free(&app->mem, table->keys);
    pnl_mem_free(&app->mem, table->numeric);
    pnl_mem_free(&app->mem, table->widths);
    pnl_mem_free(&app->mem, table->flags);
    pnl_mem_free(&app->mem, table->hidden);
    pnl_mem_free(&app->mem, table);
}

static char **pnl_table_strings(pnl_app_t *app, const char *const *texts, size_t count)
{
    char **copy = pnl_mem_alloc(&app->mem, (count ? count : 1) * sizeof(*copy));
    size_t i;

    if (copy == NULL)
        return NULL;
    memset(copy, 0, (count ? count : 1) * sizeof(*copy));
    for (i = 0; i < count; i++) {
        copy[i] = pnl_mem_copy(&app->mem, texts[i] != NULL ? texts[i] : "");
        if (copy[i] == NULL)
            break;
    }
    if (i == count)
        return copy;
    while (i-- > 0)
        pnl_mem_free(&app->mem, copy[i]);
    pnl_mem_free(&app->mem, copy);
    return NULL;
}

static int pnl_table_prepare(struct pnl_table *table, const char *const *headers, const int *column_flags)
{
    pnl_app_t *app = table->app;
    size_t columns = table->columns;
    size_t i;

    table->headers = pnl_table_strings(app, headers, columns);
    table->order = pnl_mem_alloc(&app->mem, (table->rows ? table->rows : 1) * sizeof(*table->order));
    table->widths = pnl_mem_alloc(&app->mem, columns * sizeof(*table->widths));
    table->flags = pnl_mem_alloc(&app->mem, columns * sizeof(*table->flags));
    table->hidden = pnl_mem_alloc(&app->mem, columns);
    if (table->headers == NULL || table->order == NULL || table->widths == NULL || table->flags == NULL ||
        table->hidden == NULL)
        return PNL_ERR_NOMEM;
    if (pnl_lock_init(&table->lock) != PNL_OK)
        return PNL_ERR_NOMEM;
    table->lock_ready = 1;
    for (i = 0; i < columns; i++) {
        table->flags[i] = column_flags != NULL ? column_flags[i] : 0;
        table->widths[i] = pnl_text_width(table->headers[i]) + 2;
        if (table->widths[i] > PNL_TABLE_COLUMN_MAX)
            table->widths[i] = PNL_TABLE_COLUMN_MAX;
    }
    memset(table->hidden, 0, columns);
    for (i = 0; i < table->rows; i++)
        table->order[i] = (uint32_t)i;
    table->order_len = table->rows;
    for (i = 0; i < table->rows && i < PNL_TABLE_MEASURE_ROWS; i++)
        pnl_table_widen(table, i);
    table->footer = PNL_TABLE_FOOTER;
    pnl_edit_init(&table->edit, table->want.filter, sizeof(table->want.filter));
    return PNL_OK;
}

static struct pnl_table *pnl_table_new(pnl_app_t *app, size_t columns, size_t rows)
{
    struct pnl_table *table = pnl_mem_alloc(&app->mem, sizeof(*table));

    if (table == NULL)
        return NULL;
    memset(table, 0, sizeof(*table));
    table->app = app;
    table->columns = columns;
    table->rows = rows;
    return table;
}

static int pnl_table_push(pnl_app_t *app, const char *title, struct pnl_table *table)
{
    struct pnl_view view;
    int status;

    memset(&view, 0, sizeof(view));
    view.footer_ref = &table->footer;
    view.typing_ref = &table->editing;
    view.draw_fn = pnl_table_draw;
    view.key_fn = pnl_table_key;
    view.drop_fn = pnl_table_drop;
    view.ctx = table;
    view.title = pnl_mem_copy(&app->mem, title);
    status = view.title == NULL ? PNL_ERR_NOMEM : pnl_view_push_internal(app, &view);
    if (status != PNL_OK)
        pnl_mem_free(&app->mem, view.title);
    return status;
}

static const char *pnl_array_cell(void *ctx, size_t row, size_t column, char *buf, size_t buf_len)
{
    const struct pnl_array_cells *array = ctx;

    (void)buf;
    (void)buf_len;
    return array->cells[row * array->columns + column];
}

static int pnl_table_valid(const pnl_app_t *app, const char *title, const char *const *headers, size_t columns,
                           size_t rows)
{
    return app != NULL && title != NULL && headers != NULL && columns > 0 &&
           columns < ((size_t)-1) / sizeof(char *) && rows <= ((size_t)-1) / sizeof(char *) / (columns + 1) - 1 && rows <= 0xffffffffu &&
           (app->running || app->cli);
}

int pnl_table(pnl_app_t *app, const char *title, const char *const *headers, const int *column_flags,
              size_t columns, const char *const *cells, size_t rows)
{
    struct pnl_array_cells array;
    struct pnl_table *table;
    int status;

    if (!pnl_table_valid(app, title, headers, columns, rows) || (cells == NULL && rows > 0))
        return PNL_ERR_ARG;
    array.cells = cells;
    array.columns = columns;
    if (app->cli)
        return pnl_cli_table(app, headers, column_flags, columns, rows, pnl_array_cell, &array);
    table = pnl_table_new(app, columns, rows);
    if (table == NULL)
        return PNL_ERR_NOMEM;
    table->cells = pnl_table_strings(app, cells, rows * columns);
    status = table->cells == NULL ? PNL_ERR_NOMEM : pnl_table_prepare(table, headers, column_flags);
    if (status == PNL_OK)
        status = pnl_table_push(app, title, table);
    if (status != PNL_OK)
        pnl_table_drop(app, table);
    else
        pnl_table_recall(table, title);
    return status;
}

int pnl_table_source(pnl_app_t *app, const char *title, const char *const *headers, const int *column_flags,
                     size_t columns, size_t rows, pnl_cell_fn cell_fn, pnl_release_fn release_fn, void *ctx)
{
    struct pnl_table *table;
    int status;

    if (!pnl_table_valid(app, title, headers, columns, rows) || cell_fn == NULL)
        return PNL_ERR_ARG;
    if (app->cli) {
        status = pnl_cli_table(app, headers, column_flags, columns, rows, cell_fn, ctx);
        if (status == PNL_OK && release_fn != NULL)
            release_fn(ctx);
        return status;
    }
    table = pnl_table_new(app, columns, rows);
    if (table == NULL)
        return PNL_ERR_NOMEM;
    table->cell_fn = cell_fn;
    table->ctx = ctx;
    status = pnl_table_prepare(table, headers, column_flags);
    if (status == PNL_OK)
        status = pnl_table_push(app, title, table);
    if (status != PNL_OK) {
        pnl_table_drop(app, table);
        return status;
    }
    table->release_fn = release_fn;
    pnl_table_recall(table, title);
    return PNL_OK;
}

#include <stdio.h>
#include <string.h>

#define PNL_TASK_TEXT 256
#define PNL_TASK_LOG 100
#define PNL_TASK_BAR 30
#define PNL_FOOTER_RUNNING "esc: cancel"
#define PNL_FOOTER_CANCELLING "cancelling..."
#define PNL_FOOTER_DONE "any key: back"

struct pnl_task {
    pnl_app_t *app;
    pnl_work_fn fn;
    void *ctx;
    struct pnl_thread thread;
    struct pnl_lock lock;
    int threaded;
    int joined;
    pnl_action_fn then_fn;
    void *then_ctx;
    long long started_ms;
    const char *footer;
    unsigned long long done;
    unsigned long long total;
    int cancel;
    int finished;
    int result_set;
    int result_mark;
    char title[PNL_TASK_TEXT];
    char step[PNL_TASK_TEXT];
    char result[PNL_TASK_TEXT];
    char log[PNL_TASK_LOG][PNL_TASK_TEXT];
    size_t log_start;
    size_t log_len;
    pnl_widget_t *widget;
};

static void pnl_task_copy(char *dest, const char *text)
{
    size_t len = strlen(text);

    if (len >= PNL_TASK_TEXT) {
        len = PNL_TASK_TEXT - 1;
        while (len > 0 && ((unsigned char)text[len] & 0xc0) == 0x80)
            len--;
    }
    memcpy(dest, text, len);
    dest[len] = '\0';
}

static int pnl_task_cli(const pnl_task_t *task)
{
    return !task->threaded;
}

void pnl_task_progress(pnl_task_t *task, unsigned long long done, unsigned long long total)
{
    if (task == NULL)
        return;
    if (pnl_task_cli(task)) {
        task->done = done;
        task->total = total;
        pnl_cli_progress(task->step, done, total);
        return;
    }
    pnl_lock(&task->lock);
    task->done = done;
    task->total = total;
    pnl_unlock(&task->lock);
}

int pnl_task_step(pnl_task_t *task, const char *text)
{
    if (task == NULL || text == NULL)
        return PNL_ERR_ARG;
    if (pnl_task_cli(task)) {
        pnl_task_copy(task->step, text);
        pnl_cli_progress(task->step, task->done, task->total);
        return PNL_OK;
    }
    pnl_lock(&task->lock);
    pnl_task_copy(task->step, text);
    pnl_unlock(&task->lock);
    return PNL_OK;
}

int pnl_task_log(pnl_task_t *task, const char *line)
{
    size_t slot;

    if (task == NULL || line == NULL)
        return PNL_ERR_ARG;
    if (pnl_task_cli(task)) {
        pnl_cli_log(task->app, line);
        pnl_cli_progress(task->step, task->done, task->total);
        return PNL_OK;
    }
    pnl_lock(&task->lock);
    slot = (task->log_start + task->log_len) % PNL_TASK_LOG;
    pnl_task_copy(task->log[slot], line);
    if (task->log_len < PNL_TASK_LOG)
        task->log_len++;
    else
        task->log_start = (task->log_start + 1) % PNL_TASK_LOG;
    pnl_unlock(&task->lock);
    return PNL_OK;
}

int pnl_task_cancelled(pnl_task_t *task)
{
    int cancel;

    if (task == NULL || pnl_task_cli(task))
        return 0;
    pnl_lock(&task->lock);
    cancel = task->cancel;
    pnl_unlock(&task->lock);
    return cancel;
}

int pnl_task_result(pnl_task_t *task, int mark, const char *text)
{
    if (task == NULL || text == NULL || !pnl_valid_mark(mark))
        return PNL_ERR_ARG;
    if (pnl_task_cli(task)) {
        if (pnl_term_stderr_is_tty())
            fputs("\r\x1b[K", stderr);
        return pnl_cli_result(task->app, mark, text);
    }
    pnl_lock(&task->lock);
    pnl_task_copy(task->result, text);
    task->result_mark = mark;
    task->result_set = 1;
    pnl_unlock(&task->lock);
    return PNL_OK;
}

int pnl_task_then(pnl_task_t *task, pnl_action_fn then_fn, void *then_ctx)
{
    if (task == NULL || then_fn == NULL)
        return PNL_ERR_ARG;
    task->then_fn = then_fn;
    task->then_ctx = then_ctx;
    return PNL_OK;
}

static void pnl_task_main(void *arg)
{
    pnl_task_t *task = arg;

    task->fn(task->ctx, task);
    pnl_lock(&task->lock);
    task->finished = 1;
    pnl_unlock(&task->lock);
}

static void pnl_task_outcome(pnl_task_t *task)
{
    const char *outcome = task->result_set ? task->result : task->cancel ? "cancelled" : "finished";

    pnl_task_notify(task->app, task->title, task->started_ms, outcome);
}

static void pnl_task_join(pnl_task_t *task)
{
    if (task->joined)
        return;
    pnl_thread_join(&task->thread);
    task->joined = 1;
    task->app->tasks_running--;
    task->footer = PNL_FOOTER_DONE;
    pnl_task_outcome(task);
    if (task->then_fn != NULL)
        task->then_fn(task->then_ctx, task->app);
}

static void pnl_task_shimmer(pnl_task_t *task, pnl_canvas_t *canvas, int y)
{
    static const int steps[] = {8, 4, 8};
    pnl_app_t *app = task->app;
    int filled = (int)(PNL_TASK_BAR * ((double)task->done / (double)task->total));
    int depth = canvas->screen->palette.depth;
    int head;
    int i;

    if (!pnl_motion_on(app, PNL_MOTION_FEEDBACK) || filled < 4 || task->done >= task->total ||
        (depth != PNL_DEPTH_TRUE && depth != PNL_DEPTH_256))
        return;
    head = (int)(pnl_app_frame_ms(app) % PNL_SHIMMER_MS * (filled + 6) / PNL_SHIMMER_MS) - 3;
    for (i = 0; i < 3; i++) {
        if (head + i >= 0 && head + i < filled)
            pnl_canvas_fill(canvas, 2 + head + i, y, 1, 1,
                            PNL_BG(PNL_FADE_FIRST + steps[i]));
    }
    pnl_app_animate(app, PNL_FRAME_MS * 2);
}

static void pnl_task_draw_progress(pnl_task_t *task, pnl_canvas_t *canvas, int y)
{
    char line[96];

    if (task->total == 0) {
        snprintf(line, sizeof(line), "%llds", (pnl_term_now_ms() - task->started_ms) / 1000);
        pnl_canvas_busy(canvas, 2, y, PNL_TASK_BAR, PNL_METER_ACCENT);
        pnl_canvas_text(canvas, 3 + PNL_TASK_BAR, y, PNL_FG(PNL_ROLE_MUTED), line);
        return;
    }
    pnl_canvas_bar(canvas, 2, y, PNL_TASK_BAR, (double)task->done / (double)task->total, PNL_METER_ACCENT);
    pnl_task_shimmer(task, canvas, y);
    snprintf(line, sizeof(line), "%3d%%  %llu / %llu", (int)(task->done >= task->total ? 100.0 : (double)task->done * 100.0 / (double)task->total),
             task->done, task->total);
    pnl_canvas_text(canvas, 3 + PNL_TASK_BAR, y, 0, line);
}

static void pnl_task_draw_log(pnl_task_t *task, pnl_canvas_t *canvas, int y)
{
    int rows = pnl_canvas_height(canvas) - y;
    size_t shown = rows > 0 ? (size_t)rows : 0;
    size_t first;
    size_t i;

    if (shown > task->log_len)
        shown = task->log_len;
    first = task->log_len - shown;
    for (i = 0; i < shown; i++) {
        const char *line = task->log[(task->log_start + first + i) % PNL_TASK_LOG];

        pnl_canvas_text(canvas, 2, y + (int)i, PNL_FG(PNL_ROLE_MUTED), line);
    }
}

int pnl_task_show(pnl_task_t *task, pnl_widget_t *widget)
{
    if (task == NULL || (widget != NULL && widget->app != task->app))
        return PNL_ERR_ARG;
    if (pnl_task_cli(task))
        return PNL_OK;
    pnl_lock(&task->lock);
    task->widget = widget;
    pnl_unlock(&task->lock);
    pnl_term_wake(&task->app->term);
    return PNL_OK;
}

static int pnl_task_draw_widget(pnl_task_t *task, pnl_canvas_t *canvas)
{
    int width = pnl_canvas_width(canvas) - 2;
    int height;

    if (task->widget == NULL || width <= 0)
        return 0;
    height = pnl_widget_height(task->widget, width);
    if (height <= 0 || height + 6 > pnl_canvas_height(canvas))
        return 0;
    pnl_widget_draw(task->widget, canvas, 2, 0, width, height);
    return height + 1;
}

static void pnl_task_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    pnl_task_t *task = ctx;
    struct pnl_canvas below;
    int finished;
    int top;

    (void)app;
    pnl_lock(&task->lock);
    top = pnl_task_draw_widget(task, canvas);
    below = *canvas;
    below.y += top;
    below.height -= top;
    canvas = &below;
    finished = task->finished;
    if (!finished && canvas->screen->spin >= 0) {
        pnl_draw_spinner(canvas, 2, 0, PNL_FG(PNL_ROLE_ACCENT));
        pnl_canvas_text(canvas, 4, 0, PNL_BOLD, task->step[0] ? task->step : "working");
    } else {
        pnl_canvas_text(canvas, 2, 0, PNL_BOLD, task->step[0] ? task->step : "working");
    }
    if (finished && task->result_set)
        pnl_draw_mark(canvas, 2, task->result_mark, task->result);
    else if (finished)
        pnl_draw_mark(canvas, 2, task->cancel ? PNL_MARK_FAIL : PNL_MARK_OK, task->cancel ? "cancelled" : "finished");
    else
        pnl_task_draw_progress(task, canvas, 2);
    pnl_task_draw_log(task, canvas, 4);
    pnl_unlock(&task->lock);
    if (finished)
        pnl_task_join(task);
}

static int pnl_task_key(void *ctx, pnl_app_t *app, int key)
{
    pnl_task_t *task = ctx;

    if (key == PNL_KEY_WHEEL_UP || key == PNL_KEY_WHEEL_DOWN)
        return 1;
    if (task->joined) {
        pnl_view_pop(app);
        return 1;
    }
    if (key != PNL_KEY_ESCAPE && key != 'q')
        return 1;
    pnl_lock(&task->lock);
    task->cancel = 1;
    pnl_unlock(&task->lock);
    task->footer = PNL_FOOTER_CANCELLING;
    return 1;
}

static void pnl_task_free(pnl_app_t *app, pnl_task_t *task)
{
    if (task->threaded)
        pnl_lock_destroy(&task->lock);
    pnl_mem_free(&app->mem, task);
}

static void pnl_task_drop(pnl_app_t *app, void *ctx)
{
    pnl_task_t *task = ctx;

    pnl_lock(&task->lock);
    task->cancel = 1;
    pnl_unlock(&task->lock);
    pnl_task_join(task);
    pnl_task_free(app, task);
}

static int pnl_task_cli_run(pnl_app_t *app, pnl_task_t *task)
{
    task->fn(task->ctx, task);
    pnl_task_outcome(task);
    if (task->then_fn != NULL)
        task->then_fn(task->then_ctx, app);
    if (pnl_term_stderr_is_tty())
        fputs("\r\x1b[K", stderr);
    fflush(stdout);
    pnl_task_free(app, task);
    return PNL_OK;
}

static int pnl_task_start(pnl_app_t *app, pnl_task_t *task, const char *title)
{
    struct pnl_view view;
    int status;

    memset(&view, 0, sizeof(view));
    view.footer_ref = &task->footer;
    view.draw_fn = pnl_task_draw;
    view.key_fn = pnl_task_key;
    view.ctx = task;
    view.title = pnl_mem_copy(&app->mem, title);
    if (view.title == NULL)
        return PNL_ERR_NOMEM;
    status = pnl_view_push_internal(app, &view);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, view.title);
        return status;
    }
    status = pnl_thread_start(&task->thread, pnl_task_main, task);
    if (status != PNL_OK) {
        app->views_len--;
        pnl_mem_free(&app->mem, view.title);
        return status;
    }
    app->views[app->views_len - 1].drop_fn = pnl_task_drop;
    app->tasks_running++;
    return PNL_OK;
}

int pnl_task_run(pnl_app_t *app, const char *title, pnl_work_fn work_fn, void *work_ctx)
{
    pnl_task_t *task;
    int status;

    if (app == NULL || title == NULL || work_fn == NULL || (!app->running && !app->cli))
        return PNL_ERR_ARG;
    task = pnl_mem_alloc(&app->mem, sizeof(*task));
    if (task == NULL)
        return PNL_ERR_NOMEM;
    memset(task, 0, sizeof(*task));
    task->app = app;
    task->fn = work_fn;
    task->ctx = work_ctx;
    task->started_ms = pnl_term_now_ms();
    pnl_task_copy(task->title, title);
    task->footer = PNL_FOOTER_RUNNING;
    if (app->cli)
        return pnl_task_cli_run(app, task);
    status = pnl_lock_init(&task->lock);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, task);
        return status;
    }
    task->threaded = 1;
    status = pnl_task_start(app, task, title);
    if (status != PNL_OK)
        pnl_task_free(app, task);
    return status;
}

#if !defined(_WIN32)

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define PNL_TERM_START "\x1b[?1049h\x1b[?25l\x1b[?7l\x1b[?1004h"
#define PNL_TERM_MOUSE "\x1b[?1000h\x1b[?1006h"
#define PNL_TERM_END "\x1b[?1004l\x1b[?1006l\x1b[?1000l\x1b[0m\x1b[?7h\x1b[?25h\x1b[?1049l"

static const int pnl_term_signals[] = {SIGTERM, SIGHUP, SIGQUIT, SIGINT};
#define PNL_TERM_SIGNALS (sizeof(pnl_term_signals) / sizeof(pnl_term_signals[0]))

static struct termios pnl_term_saved;
static volatile sig_atomic_t pnl_term_raw;
static volatile sig_atomic_t pnl_term_winch;
static struct sigaction pnl_term_old[PNL_TERM_SIGNALS];
static struct sigaction pnl_term_old_winch;

static int pnl_term_plain;

static void pnl_term_restore(void)
{
    if (!pnl_term_raw)
        return;
    pnl_term_raw = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &pnl_term_saved);
    if (!pnl_term_plain && write(STDOUT_FILENO, PNL_TERM_END, sizeof(PNL_TERM_END) - 1) < 0)
        return;
}

static void pnl_term_on_signal(int sig)
{
    pnl_term_restore();
    signal(sig, SIG_DFL);
    raise(sig);
}

static void pnl_term_on_winch(int sig)
{
    (void)sig;
    pnl_term_winch = 1;
}

static void pnl_term_catch(void)
{
    struct sigaction sa;
    size_t i;

    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = pnl_term_on_signal;
    for (i = 0; i < PNL_TERM_SIGNALS; i++)
        sigaction(pnl_term_signals[i], &sa, &pnl_term_old[i]);
    sa.sa_handler = pnl_term_on_winch;
    sigaction(SIGWINCH, &sa, &pnl_term_old_winch);
}

static void pnl_term_release(void)
{
    size_t i;

    for (i = 0; i < PNL_TERM_SIGNALS; i++)
        sigaction(pnl_term_signals[i], &pnl_term_old[i], NULL);
    sigaction(SIGWINCH, &pnl_term_old_winch, NULL);
}

int pnl_term_available(void)
{
    return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
}

int pnl_term_stdout_is_tty(void)
{
    return isatty(STDOUT_FILENO);
}

int pnl_term_stderr_is_tty(void)
{
    return isatty(STDERR_FILENO);
}

int pnl_term_stdin_is_tty(void)
{
    return isatty(STDIN_FILENO);
}

static int pnl_term_make_raw(void)
{
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &pnl_term_saved) != 0)
        return PNL_ERR_TTY;
    raw = pnl_term_saved;
    raw.c_iflag &= ~(tcflag_t)(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(tcflag_t)OPOST;
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(tcflag_t)(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) != 0)
        return PNL_ERR_TTY;
    pnl_term_raw = 1;
    return PNL_OK;
}

int pnl_os_enter(struct pnl_term *term)
{
    int status;

    if (term->active)
        return PNL_OK;
    status = pnl_term_make_raw();
    if (status != PNL_OK)
        return status;
    pnl_term_catch();
    term->active = 1;
    term->focus_known = 0;
    pnl_term_plain = term->linear;
    status = term->linear ? PNL_OK : pnl_os_write(PNL_TERM_START, sizeof(PNL_TERM_START) - 1);
    if (status == PNL_OK && term->mouse && !term->linear)
        status = pnl_os_write(PNL_TERM_MOUSE, sizeof(PNL_TERM_MOUSE) - 1);
    if (status != PNL_OK)
        pnl_os_leave(term);
    return status;
}

void pnl_os_leave(struct pnl_term *term)
{
    if (!term->active)
        return;
    pnl_term_restore();
    pnl_term_release();
    term->active = 0;
    term->input_len = 0;
}

void pnl_os_suspend(struct pnl_term *term)
{
    pnl_os_leave(term);
    raise(SIGTSTP);
    pnl_os_enter(term);
}

int pnl_os_resized(void)
{
    if (!pnl_term_winch)
        return 0;
    pnl_term_winch = 0;
    return 1;
}

void pnl_os_size(int *out_width, int *out_height)
{
    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0 || ws.ws_col == 0 || ws.ws_row == 0) {
        *out_width = 80;
        *out_height = 24;
        return;
    }
    *out_width = ws.ws_col;
    *out_height = ws.ws_row;
}

int pnl_os_write(const char *data, size_t data_len)
{
    while (data_len > 0) {
        ssize_t written = write(STDOUT_FILENO, data, data_len);

        if (written < 0 && errno == EINTR)
            continue;
        if (written <= 0)
            return PNL_ERR_IO;
        data += written;
        data_len -= (size_t)written;
    }
    return PNL_OK;
}

static int pnl_term_poll_input(int timeout_ms)
{
    struct pollfd pfd;
    int ready;

    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN;
    pfd.revents = 0;
    ready = poll(&pfd, 1, timeout_ms);
    if (ready < 0 && errno == EINTR)
        return 0;
    return ready;
}

int pnl_term_fill(struct pnl_term *term, int timeout_ms)
{
    ssize_t got;
    int ready = pnl_term_poll_input(timeout_ms);

    if (ready <= 0)
        return ready;
    got = read(STDIN_FILENO, term->input + term->input_len, PNL_INPUT_MAX - term->input_len);
    if (got < 0 && (errno == EINTR || errno == EAGAIN))
        return 0;
    if (got <= 0)
        return -1;
    term->input_len += (size_t)got;
    return 1;
}

int pnl_os_read_key(struct pnl_term *term, int timeout_ms, int *out_key)
{
    size_t used;

    *out_key = PNL_KEY_NONE;
    if (term->input_len == 0) {
        int filled = pnl_term_fill(term, timeout_ms);

        if (filled <= 0)
            return filled < 0 ? PNL_ERR_IO : PNL_OK;
    }
    if (pnl_term_complete(term) < 0)
        return PNL_ERR_IO;
    used = pnl_key_decode_at(term->input, term->input_len, out_key, &term->mouse_x, &term->mouse_y);
    memmove(term->input, term->input + used, term->input_len - used);
    term->input_len -= used;
    return PNL_OK;
}

int pnl_term_wake_open(struct pnl_term *term)
{
    int i;

    if (term->wake_open)
        return PNL_OK;
    if (pipe(term->wake_fd) != 0)
        return PNL_ERR_IO;
    for (i = 0; i < 2; i++) {
        fcntl(term->wake_fd[i], F_SETFL, fcntl(term->wake_fd[i], F_GETFL) | O_NONBLOCK);
        fcntl(term->wake_fd[i], F_SETFD, FD_CLOEXEC);
    }
    term->wake_open = 1;
    return PNL_OK;
}

void pnl_term_wake_close(struct pnl_term *term)
{
    if (!term->wake_open)
        return;
    close(term->wake_fd[0]);
    close(term->wake_fd[1]);
    term->wake_open = 0;
}

void pnl_os_wake(struct pnl_term *term)
{
    if (term->wake_open && write(term->wake_fd[1], "w", 1) < 0)
        return;
}

static void pnl_term_drain_wake(struct pnl_term *term)
{
    char buf[64];

    while (read(term->wake_fd[0], buf, sizeof(buf)) > 0)
        ;
    term->woken = 1;
}

int pnl_os_wait(struct pnl_term *term, const struct pnl_watch *watches, size_t watches_len, int timeout_ms,
                  unsigned char *out_ready)
{
    struct pollfd pfds[PNL_WATCH_MAX + 2];
    size_t count = watches_len < PNL_WATCH_MAX ? watches_len : PNL_WATCH_MAX;
    size_t i;
    int ready;

    if (term->input_len > 0)
        timeout_ms = 0;
    pfds[0].fd = STDIN_FILENO;
    pfds[0].events = POLLIN;
    for (i = 0; i < count; i++) {
        pfds[i + 1].fd = watches[i].fd;
        pfds[i + 1].events = POLLIN;
    }
    pfds[count + 1].fd = term->wake_open ? term->wake_fd[0] : -1;
    pfds[count + 1].events = POLLIN;
    for (i = 0; i <= count + 1; i++)
        pfds[i].revents = 0;
    ready = poll(pfds, (nfds_t)(count + 2), timeout_ms);
    if (ready < 0)
        return errno == EINTR ? 0 : -1;
    if (pfds[count + 1].revents != 0)
        pnl_term_drain_wake(term);
    for (i = 0; i < count; i++)
        out_ready[i] = (unsigned char)(pfds[i + 1].revents != 0);
    return term->input_len > 0 || pfds[0].revents != 0;
}

void pnl_nap_ms(int ms)
{
    poll(NULL, 0, ms);
}

long long pnl_clock_us(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

long long pnl_os_now_ms(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

#endif

#if defined(_WIN32)

#include <string.h>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#ifndef DISABLE_NEWLINE_AUTO_RETURN
#define DISABLE_NEWLINE_AUTO_RETURN 0x0008
#endif
#ifndef ENABLE_VIRTUAL_TERMINAL_INPUT
#define ENABLE_VIRTUAL_TERMINAL_INPUT 0x0200
#endif

#define PNL_TERM_START "\x1b[?1049h\x1b[?25l\x1b[?7l\x1b[?1004h"
#define PNL_TERM_MOUSE "\x1b[?1000h\x1b[?1006h"
#define PNL_TERM_END "\x1b[?1004l\x1b[?1006l\x1b[?1000l\x1b[0m\x1b[?7h\x1b[?25h\x1b[?1049l"
#define PNL_TERM_RECORDS 32

static DWORD pnl_term_saved_in;
static DWORD pnl_term_saved_out;
static UINT pnl_term_saved_cp_in;
static UINT pnl_term_saved_cp_out;
static volatile LONG pnl_term_raw;
static volatile LONG pnl_term_winch;
static int pnl_term_last_width;
static int pnl_term_last_height;

static HANDLE pnl_term_in(void)
{
    return GetStdHandle(STD_INPUT_HANDLE);
}

static HANDLE pnl_term_out(void)
{
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

static int pnl_term_is_console(DWORD which)
{
    DWORD mode;

    return GetConsoleMode(GetStdHandle(which), &mode) != 0;
}

static int pnl_term_enable_vt(DWORD which)
{
    HANDLE handle = GetStdHandle(which);
    DWORD mode;

    if (!GetConsoleMode(handle, &mode))
        return 0;
    if (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
        return 1;
    return SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}

int pnl_term_available(void)
{
    return pnl_term_is_console(STD_INPUT_HANDLE) && pnl_term_is_console(STD_OUTPUT_HANDLE);
}

int pnl_term_stdout_is_tty(void)
{
    return pnl_term_enable_vt(STD_OUTPUT_HANDLE);
}

int pnl_term_stderr_is_tty(void)
{
    return pnl_term_enable_vt(STD_ERROR_HANDLE);
}

int pnl_term_stdin_is_tty(void)
{
    return pnl_term_is_console(STD_INPUT_HANDLE);
}

int pnl_os_write(const char *data, size_t data_len)
{
    while (data_len > 0) {
        DWORD chunk = data_len > 0x10000 ? 0x10000 : (DWORD)data_len;
        DWORD written = 0;

        if (!WriteFile(pnl_term_out(), data, chunk, &written, NULL) || written == 0)
            return PNL_ERR_IO;
        data += written;
        data_len -= written;
    }
    return PNL_OK;
}

static int pnl_term_plain;

static void pnl_term_restore(void)
{
    DWORD written;

    if (InterlockedExchange(&pnl_term_raw, 0) == 0)
        return;
    if (!pnl_term_plain)
        WriteFile(pnl_term_out(), PNL_TERM_END, sizeof(PNL_TERM_END) - 1, &written, NULL);
    SetConsoleMode(pnl_term_in(), pnl_term_saved_in);
    SetConsoleMode(pnl_term_out(), pnl_term_saved_out);
    SetConsoleCP(pnl_term_saved_cp_in);
    SetConsoleOutputCP(pnl_term_saved_cp_out);
}

static BOOL WINAPI pnl_term_on_ctrl(DWORD event)
{
    (void)event;
    pnl_term_restore();
    return FALSE;
}

static int pnl_term_make_raw(void)
{
    DWORD in_mode;
    DWORD out_mode;

    if (!GetConsoleMode(pnl_term_in(), &pnl_term_saved_in) || !GetConsoleMode(pnl_term_out(), &pnl_term_saved_out))
        return PNL_ERR_TTY;
    in_mode = pnl_term_saved_in & ~(DWORD)(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    in_mode |= ENABLE_VIRTUAL_TERMINAL_INPUT | ENABLE_WINDOW_INPUT;
    out_mode = pnl_term_saved_out | ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING |
               DISABLE_NEWLINE_AUTO_RETURN;
    if (!SetConsoleMode(pnl_term_out(), out_mode))
        return PNL_ERR_UNSUPPORTED;
    if (!SetConsoleMode(pnl_term_in(), in_mode)) {
        SetConsoleMode(pnl_term_out(), pnl_term_saved_out);
        return PNL_ERR_UNSUPPORTED;
    }
    pnl_term_saved_cp_in = GetConsoleCP();
    pnl_term_saved_cp_out = GetConsoleOutputCP();
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    InterlockedExchange(&pnl_term_raw, 1);
    return PNL_OK;
}

int pnl_os_enter(struct pnl_term *term)
{
    int status;

    if (term->active)
        return PNL_OK;
    status = pnl_term_make_raw();
    if (status != PNL_OK)
        return status;
    SetConsoleCtrlHandler(pnl_term_on_ctrl, TRUE);
    term->active = 1;
    term->focus_known = 0;
    pnl_os_size(&pnl_term_last_width, &pnl_term_last_height);
    pnl_term_plain = term->linear;
    status = term->linear ? PNL_OK : pnl_os_write(PNL_TERM_START, sizeof(PNL_TERM_START) - 1);
    if (status == PNL_OK && term->mouse && !term->linear)
        status = pnl_os_write(PNL_TERM_MOUSE, sizeof(PNL_TERM_MOUSE) - 1);
    if (status != PNL_OK)
        pnl_os_leave(term);
    return status;
}

void pnl_os_leave(struct pnl_term *term)
{
    if (!term->active)
        return;
    pnl_term_restore();
    SetConsoleCtrlHandler(pnl_term_on_ctrl, FALSE);
    term->active = 0;
    term->input_len = 0;
    term->surrogate = 0;
}

void pnl_os_suspend(struct pnl_term *term)
{
    (void)term;
}

void pnl_os_size(int *out_width, int *out_height)
{
    CONSOLE_SCREEN_BUFFER_INFO info;

    if (!GetConsoleScreenBufferInfo(pnl_term_out(), &info)) {
        *out_width = 80;
        *out_height = 24;
        return;
    }
    *out_width = info.srWindow.Right - info.srWindow.Left + 1;
    *out_height = info.srWindow.Bottom - info.srWindow.Top + 1;
}

int pnl_os_resized(void)
{
    int width;
    int height;

    pnl_os_size(&width, &height);
    if (width != pnl_term_last_width || height != pnl_term_last_height) {
        pnl_term_last_width = width;
        pnl_term_last_height = height;
        InterlockedExchange(&pnl_term_winch, 0);
        return 1;
    }
    return InterlockedExchange(&pnl_term_winch, 0) != 0;
}

static void pnl_term_take_unit(struct pnl_term *term, unsigned int unit)
{
    char bytes[4];
    unsigned int cp = unit;
    size_t len;

    if (unit >= 0xd800 && unit <= 0xdbff) {
        term->surrogate = unit;
        return;
    }
    if (unit >= 0xdc00 && unit <= 0xdfff) {
        if (term->surrogate == 0)
            return;
        cp = 0x10000 + ((term->surrogate - 0xd800) << 10) + (unit - 0xdc00);
    }
    term->surrogate = 0;
    len = pnl_utf8_put(cp, bytes);
    if (term->input_len + len > PNL_INPUT_MAX)
        return;
    memcpy(term->input + term->input_len, bytes, len);
    term->input_len += len;
}

static int pnl_term_drain(struct pnl_term *term)
{
    INPUT_RECORD records[PNL_TERM_RECORDS];
    DWORD room = (DWORD)((PNL_INPUT_MAX - term->input_len) / 4);
    DWORD count = 0;
    DWORD i;

    if (room > PNL_TERM_RECORDS)
        room = PNL_TERM_RECORDS;
    if (room == 0)
        return term->input_len > 0;
    if (!GetNumberOfConsoleInputEvents(pnl_term_in(), &count))
        return -1;
    if (count == 0)
        return 0;
    if (!ReadConsoleInputW(pnl_term_in(), records, room, &count))
        return -1;
    for (i = 0; i < count; i++) {
        if (records[i].EventType == WINDOW_BUFFER_SIZE_EVENT)
            InterlockedExchange(&pnl_term_winch, 1);
        if (records[i].EventType != KEY_EVENT || !records[i].Event.KeyEvent.bKeyDown)
            continue;
        if (records[i].Event.KeyEvent.uChar.UnicodeChar != 0)
            pnl_term_take_unit(term, records[i].Event.KeyEvent.uChar.UnicodeChar);
    }
    return term->input_len > 0;
}

int pnl_term_fill(struct pnl_term *term, int timeout_ms)
{
    DWORD wait = timeout_ms < 0 ? INFINITE : (DWORD)timeout_ms;
    long long deadline = pnl_os_now_ms() + (timeout_ms < 0 ? 0 : timeout_ms);
    size_t before = term->input_len;

    for (;;) {
        DWORD result = WaitForSingleObject(pnl_term_in(), wait);

        if (result == WAIT_TIMEOUT)
            return 0;
        if (result != WAIT_OBJECT_0 || pnl_term_drain(term) < 0)
            return -1;
        if (term->input_len > before || timeout_ms == 0)
            return term->input_len > before;
        if (timeout_ms > 0) {
            long long left = deadline - pnl_os_now_ms();

            if (left <= 0)
                return 0;
            wait = (DWORD)left;
        }
    }
}

int pnl_os_read_key(struct pnl_term *term, int timeout_ms, int *out_key)
{
    size_t used;

    *out_key = PNL_KEY_NONE;
    if (term->input_len == 0) {
        int filled = pnl_term_fill(term, timeout_ms);

        if (filled <= 0)
            return filled < 0 ? PNL_ERR_IO : PNL_OK;
    }
    if (pnl_term_complete(term) < 0)
        return PNL_ERR_IO;
    used = pnl_key_decode_at(term->input, term->input_len, out_key, &term->mouse_x, &term->mouse_y);
    memmove(term->input, term->input + used, term->input_len - used);
    term->input_len -= used;
    return PNL_OK;
}

int pnl_term_wake_open(struct pnl_term *term)
{
    if (term->wake_open)
        return PNL_OK;
    term->wake_event = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (term->wake_event == NULL)
        return PNL_ERR_IO;
    term->wake_open = 1;
    return PNL_OK;
}

void pnl_term_wake_close(struct pnl_term *term)
{
    if (!term->wake_open)
        return;
    CloseHandle((HANDLE)term->wake_event);
    term->wake_event = NULL;
    term->wake_open = 0;
}

void pnl_os_wake(struct pnl_term *term)
{
    if (term->wake_open)
        SetEvent((HANDLE)term->wake_event);
}

int pnl_os_wait(struct pnl_term *term, const struct pnl_watch *watches, size_t watches_len, int timeout_ms,
                  unsigned char *out_ready)
{
    (void)watches;
    (void)watches_len;
    (void)out_ready;
    if (term->input_len > 0)
        return 1;
    if (term->wake_open) {
        HANDLE handles[2];
        DWORD result;

        handles[0] = pnl_term_in();
        handles[1] = (HANDLE)term->wake_event;
        result = WaitForMultipleObjects(2, handles, FALSE, timeout_ms < 0 ? INFINITE : (DWORD)timeout_ms);
        if (result == WAIT_OBJECT_0 + 1)
            term->woken = 1;
        if (result != WAIT_OBJECT_0)
            return result == WAIT_FAILED ? -1 : 0;
        timeout_ms = 0;
    }
    return pnl_term_fill(term, timeout_ms) < 0 ? -1 : term->input_len > 0;
}

void pnl_nap_ms(int ms)
{
    Sleep((DWORD)ms);
}

long long pnl_clock_us(void)
{
    LARGE_INTEGER count;
    LARGE_INTEGER rate;

    QueryPerformanceCounter(&count);
    QueryPerformanceFrequency(&rate);
    return (long long)(count.QuadPart / rate.QuadPart * 1000000 +
                       count.QuadPart % rate.QuadPart * 1000000 / rate.QuadPart);
}

long long pnl_os_now_ms(void)
{
    return (long long)GetTickCount64();
}

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_THEME_LINE 256

struct pnl_builtin_theme {
    const char *name;
    struct pnl_theme theme;
};

static const struct pnl_builtin_theme pnl_builtin_themes[] = {
    {"default",
     {{0x5fafffUL, 0x5fd75fUL, 0xd7af00UL, 0xff5f5fUL, 0x8a8a8aUL, 0x87afd7UL, 0x3a3a3aUL},
      {0x005fd7UL, 0x008700UL, 0x875f00UL, 0xd70000UL, 0x6c6c6cUL, 0x005f87UL, 0xdadadaUL},
      {PNL_COLOR_CYAN, PNL_COLOR_GREEN, PNL_COLOR_YELLOW, PNL_COLOR_RED, PNL_COLOR_GRAY, PNL_COLOR_BLUE,
       PNL_COLOR_GRAY}}},
    {"mono",
     {{0xffffffUL, 0xd0d0d0UL, 0xd0d0d0UL, 0xffffffUL, 0x808080UL, 0xb0b0b0UL, 0x3a3a3aUL},
      {0x000000UL, 0x303030UL, 0x303030UL, 0x000000UL, 0x707070UL, 0x505050UL, 0xdadadaUL},
      {PNL_COLOR_WHITE, PNL_COLOR_DEFAULT, PNL_COLOR_DEFAULT, PNL_COLOR_WHITE, PNL_COLOR_GRAY, PNL_COLOR_DEFAULT,
       PNL_COLOR_GRAY}}},
    {"contrast",
     {{0x00d7ffUL, 0x00ff00UL, 0xffff00UL, 0xff3030UL, 0xbcbcbcUL, 0x87d7ffUL, 0x444444UL},
      {0x0000d7UL, 0x006400UL, 0x7a5c00UL, 0xb00000UL, 0x4e4e4eUL, 0x00008bUL, 0xc6c6c6UL},
      {PNL_COLOR_BRIGHT_CYAN, PNL_COLOR_BRIGHT_GREEN, PNL_COLOR_BRIGHT_YELLOW, PNL_COLOR_BRIGHT_RED,
       PNL_COLOR_WHITE, PNL_COLOR_BRIGHT_BLUE, PNL_COLOR_GRAY}}},
    {"warm",
     {{0xffaf5fUL, 0xafd75fUL, 0xffd75fUL, 0xff875fUL, 0x9e8e7eUL, 0xd7af87UL, 0x3a302aUL},
      {0xaf5f00UL, 0x5f8700UL, 0x875f00UL, 0xaf0000UL, 0x7a6a5aUL, 0x875f5fUL, 0xe4dccfUL},
      {PNL_COLOR_YELLOW, PNL_COLOR_GREEN, PNL_COLOR_YELLOW, PNL_COLOR_RED, PNL_COLOR_GRAY, PNL_COLOR_MAGENTA,
       PNL_COLOR_GRAY}}},
};

static const char *const pnl_role_names[PNL_ROLES] = {"accent", "ok", "warn", "fail", "muted", "info", "surface"};

#define PNL_BUILTIN_THEMES (sizeof(pnl_builtin_themes) / sizeof(pnl_builtin_themes[0]))

static int pnl_theme_builtin(struct pnl_theme *theme, const char *name)
{
    size_t i;

    for (i = 0; i < PNL_BUILTIN_THEMES; i++) {
        if (strcmp(pnl_builtin_themes[i].name, name) == 0) {
            *theme = pnl_builtin_themes[i].theme;
            return 1;
        }
    }
    return 0;
}

static char *pnl_trim(char *text)
{
    char *end;

    while (*text == ' ' || *text == '\t')
        text++;
    end = text + strlen(text);
    while (end > text && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\n' || end[-1] == '\r'))
        *--end = '\0';
    return text;
}

static int pnl_theme_role(const char *name)
{
    int i;

    for (i = 0; i < PNL_ROLES; i++) {
        if (strcmp(pnl_role_names[i], name) == 0)
            return i;
    }
    return -1;
}

static int pnl_theme_color(const char *text, unsigned long *out_rgb)
{
    char *end;

    if (text[0] != '#' || strlen(text) != 7)
        return 0;
    *out_rgb = strtoul(text + 1, &end, 16);
    return *end == '\0';
}

int pnl_theme_line(struct pnl_theme *theme, char *line, int *light_set)
{
    char *equal = (strchr)(line, '=');
    char *key;
    unsigned long rgb;
    int light = 0;
    int role;

    if (equal == NULL)
        return 0;
    *equal = '\0';
    key = pnl_trim(line);
    if (strncmp(key, "light.", 6) == 0) {
        light = 1;
        key += 6;
    }
    role = pnl_theme_role(key);
    if (role < 0 || !pnl_theme_color(pnl_trim(equal + 1), &rgb))
        return 0;
    if (light) {
        theme->light[role] = rgb;
        light_set[role] = 1;
    } else {
        theme->dark[role] = rgb;
    }
    return 1;
}

static int pnl_theme_file(struct pnl_theme *theme, const char *path)
{
    char line[PNL_THEME_LINE];
    int light_set[PNL_ROLES] = {0};
    FILE *file = fopen(path, "r");
    int status = PNL_OK;
    int i;

    if (file == NULL)
        return PNL_ERR_NOT_FOUND;
    pnl_theme_builtin(theme, "default");
    while (status == PNL_OK && fgets(line, sizeof(line), file) != NULL) {
        char *text = pnl_trim(line);

        if (text[0] != '\0' && text[0] != '#' && !pnl_theme_line(theme, text, light_set))
            status = PNL_ERR_ARG;
    }
    fclose(file);
    for (i = 0; i < PNL_ROLES; i++) {
        if (!light_set[i])
            theme->light[i] = theme->dark[i];
    }
    return status;
}

int pnl_theme_choose(struct pnl_theme *theme, const char *name)
{
    struct pnl_theme chosen;
    int status = PNL_OK;

    if (!pnl_theme_builtin(&chosen, name))
        status = (strchr)(name, '/') != NULL || (strchr)(name, '\\') != NULL || (strchr)(name, '.') != NULL
                     ? pnl_theme_file(&chosen, name)
                     : PNL_ERR_NOT_FOUND;
    if (status == PNL_OK)
        *theme = chosen;
    return status;
}

int pnl_app_set_theme(pnl_app_t *app, const char *theme)
{
    if (app == NULL || theme == NULL)
        return PNL_ERR_ARG;
    return pnl_theme_choose(&app->theme, theme);
}

#if defined(_WIN32)

#include <windows.h>

typedef char pnl_lock_fits[sizeof(CRITICAL_SECTION) <= sizeof(union pnl_storage) ? 1 : -1];

static DWORD WINAPI pnl_thread_entry(LPVOID arg)
{
    struct pnl_thread *thread = arg;

    thread->fn(thread->arg);
    return 0;
}

int pnl_thread_start(struct pnl_thread *thread, void (*fn)(void *arg), void *arg)
{
    thread->fn = fn;
    thread->arg = arg;
    thread->handle = CreateThread(NULL, 0, pnl_thread_entry, thread, 0, NULL);
    return thread->handle != NULL ? PNL_OK : PNL_ERR_NOMEM;
}

void pnl_thread_join(struct pnl_thread *thread)
{
    WaitForSingleObject((HANDLE)thread->handle, INFINITE);
    CloseHandle((HANDLE)thread->handle);
}

int pnl_lock_init(struct pnl_lock *lock)
{
    CRITICAL_SECTION *cs = (CRITICAL_SECTION *)&lock->storage;

    InitializeCriticalSection(cs);
    return PNL_OK;
}

void pnl_lock_destroy(struct pnl_lock *lock)
{
    DeleteCriticalSection((CRITICAL_SECTION *)&lock->storage);
}

void pnl_lock(struct pnl_lock *lock)
{
    EnterCriticalSection((CRITICAL_SECTION *)&lock->storage);
}

void pnl_unlock(struct pnl_lock *lock)
{
    LeaveCriticalSection((CRITICAL_SECTION *)&lock->storage);
}

#else

#include <pthread.h>

typedef char pnl_lock_fits[sizeof(pthread_mutex_t) <= sizeof(union pnl_storage) ? 1 : -1];
typedef char pnl_thread_fits[sizeof(pthread_t) <= sizeof(union pnl_storage) ? 1 : -1];

static void *pnl_thread_entry(void *arg)
{
    struct pnl_thread *thread = arg;

    thread->fn(thread->arg);
    return NULL;
}

int pnl_thread_start(struct pnl_thread *thread, void (*fn)(void *arg), void *arg)
{
    pthread_t *handle = (pthread_t *)&thread->storage;

    thread->fn = fn;
    thread->arg = arg;
    return pthread_create(handle, NULL, pnl_thread_entry, thread) == 0 ? PNL_OK : PNL_ERR_NOMEM;
}

void pnl_thread_join(struct pnl_thread *thread)
{
    pthread_join(*(pthread_t *)&thread->storage, NULL);
}

int pnl_lock_init(struct pnl_lock *lock)
{
    return pthread_mutex_init((pthread_mutex_t *)&lock->storage, NULL) == 0 ? PNL_OK : PNL_ERR_NOMEM;
}

void pnl_lock_destroy(struct pnl_lock *lock)
{
    pthread_mutex_destroy((pthread_mutex_t *)&lock->storage);
}

void pnl_lock(struct pnl_lock *lock)
{
    pthread_mutex_lock((pthread_mutex_t *)&lock->storage);
}

void pnl_unlock(struct pnl_lock *lock)
{
    pthread_mutex_unlock((pthread_mutex_t *)&lock->storage);
}

#endif

#include <stdio.h>
#include <string.h>

#define PNL_VALUE_TEXT 256

struct pnl_value {
    pnl_app_t *app;
    struct pnl_lock lock;
    char *label;
    int mark;
    double meter;
    int pending;
    char text[PNL_VALUE_TEXT];
};

int pnl_value_create(pnl_app_t *app, const char *label, pnl_value_t **out_value)
{
    pnl_value_t *value;
    int status;

    if (out_value != NULL)
        *out_value = NULL;
    if (app == NULL || label == NULL || out_value == NULL)
        return PNL_ERR_ARG;
    status = pnl_term_wake_open(&app->term);
    if (status != PNL_OK)
        return status;
    status = pnl_mem_grow(&app->mem, (void **)&app->values, &app->values_cap, app->values_len + 1,
                          sizeof(*app->values));
    if (status != PNL_OK)
        return status;
    value = pnl_mem_alloc(&app->mem, sizeof(*value));
    if (value == NULL)
        return PNL_ERR_NOMEM;
    memset(value, 0, sizeof(*value));
    value->app = app;
    value->mark = PNL_MARK_WAIT;
    value->meter = -1.0;
    value->label = pnl_mem_copy(&app->mem, label);
    status = value->label == NULL ? PNL_ERR_NOMEM : pnl_lock_init(&value->lock);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, value->label);
        pnl_mem_free(&app->mem, value);
        return status;
    }
    app->values[app->values_len++] = value;
    *out_value = value;
    return PNL_OK;
}

void pnl_value_destroy(pnl_value_t *value)
{
    pnl_app_t *app;
    size_t i;

    if (value == NULL)
        return;
    app = value->app;
    for (i = 0; i < app->values_len && app->values[i] != value; i++)
        ;
    if (i < app->values_len) {
        memmove(app->values + i, app->values + i + 1, (app->values_len - i - 1) * sizeof(*app->values));
        app->values_len--;
    }
    pnl_lock_destroy(&value->lock);
    pnl_mem_free(&app->mem, value->label);
    pnl_mem_free(&app->mem, value);
    app->dirty = 1;
}

static void pnl_value_store(pnl_value_t *value, int mark, double meter, const char *text)
{
    size_t len = strlen(text);
    int wake;

    if (len >= PNL_VALUE_TEXT) {
        len = PNL_VALUE_TEXT - 1;
        while (len > 0 && ((unsigned char)text[len] & 0xc0) == 0x80)
            len--;
    }

    pnl_lock(&value->lock);
    wake = !value->pending && (value->mark != mark || value->meter != meter || strncmp(value->text, text, len) != 0 ||
                               value->text[len] != '\0');
    value->pending = value->pending || wake;
    value->mark = mark;
    value->meter = meter;
    memcpy(value->text, text, len);
    value->text[len] = '\0';
    pnl_unlock(&value->lock);
    if (wake)
        pnl_term_wake(&value->app->term);
}

int pnl_value_set(pnl_value_t *value, int mark, const char *text)
{
    if (value == NULL || text == NULL || !pnl_valid_mark(mark))
        return PNL_ERR_ARG;
    pnl_value_store(value, mark, -1.0, text);
    return PNL_OK;
}

int pnl_value_set_meter(pnl_value_t *value, int mark, double fraction)
{
    if (value == NULL || !pnl_valid_mark(mark) || fraction != fraction)
        return PNL_ERR_ARG;
    pnl_value_store(value, mark, fraction < 0.0 ? 0.0 : fraction > 1.0 ? 1.0 : fraction, "");
    return PNL_OK;
}

void pnl_values_lines(pnl_app_t *app)
{
    char text[PNL_VALUE_TEXT];
    size_t i;

    app->in_status = 1;
    for (i = 0; i < app->values_len; i++) {
        pnl_value_t *value = app->values[i];
        double meter;
        int mark;

        pnl_lock(&value->lock);
        value->pending = 0;
        mark = value->mark;
        meter = value->meter;
        memcpy(text, value->text, sizeof(text));
        pnl_unlock(&value->lock);
        if (pnl_status_line(app, mark, value->label, text) == PNL_OK)
            app->lines[app->lines_len - 1].meter = meter;
    }
    app->in_status = 0;
}

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PNL_VIRTUAL_KEYS 64
#define PNL_VIRTUAL_TIMEOUT_MS 30000
#define PNL_VIRTUAL_NAP_MS 1
#define PNL_VIRTUAL_BUSY_MS 5
#define PNL_TEST_FILE_MAX (4 * 1024 * 1024)
#define PNL_TEST_PATH_MAX 1024

struct pnl_test {
    pnl_app_t *app;
    struct pnl_lock lock;
    struct pnl_thread thread;
    int width;
    int height;
    int keys[PNL_VIRTUAL_KEYS];
    int keys_x[PNL_VIRTUAL_KEYS];
    int keys_y[PNL_VIRTUAL_KEYS];
    size_t keys_len;
    long long clock_ms;
    long long clock_seen;
    int woken;
    int idle;
    int finished;
    int status;
    char *text;
    size_t text_cap;
};

static pnl_test_t *pnl_test_now;

int pnl_virtual_active(void)
{
    return pnl_test_now != NULL;
}

int pnl_term_enter(struct pnl_term *term)
{
    if (pnl_test_now == NULL)
        return pnl_os_enter(term);
    term->active = 1;
    return PNL_OK;
}

void pnl_term_leave(struct pnl_term *term)
{
    if (pnl_test_now == NULL)
        pnl_os_leave(term);
    else
        term->active = 0;
}

void pnl_term_suspend(struct pnl_term *term)
{
    if (pnl_test_now == NULL)
        pnl_os_suspend(term);
}

int pnl_term_resized(void)
{
    return pnl_test_now == NULL ? pnl_os_resized() : 0;
}

void pnl_term_size(int *out_width, int *out_height)
{
    if (pnl_test_now == NULL) {
        pnl_os_size(out_width, out_height);
        return;
    }
    *out_width = pnl_test_now->width;
    *out_height = pnl_test_now->height;
}

int pnl_term_write(const char *data, size_t data_len)
{
    return pnl_test_now == NULL ? pnl_os_write(data, data_len) : PNL_OK;
}

long long pnl_term_now_ms(void)
{
    long long now;

    if (pnl_test_now == NULL)
        return pnl_os_now_ms();
    pnl_lock(&pnl_test_now->lock);
    now = pnl_test_now->clock_ms;
    pnl_unlock(&pnl_test_now->lock);
    return now;
}

void pnl_term_wake(struct pnl_term *term)
{
    if (pnl_test_now == NULL) {
        pnl_os_wake(term);
        return;
    }
    pnl_lock(&pnl_test_now->lock);
    pnl_test_now->woken = 1;
    pnl_unlock(&pnl_test_now->lock);
}

int pnl_term_read_key(struct pnl_term *term, int timeout_ms, int *out_key)
{
    pnl_test_t *test = pnl_test_now;

    if (test == NULL)
        return pnl_os_read_key(term, timeout_ms, out_key);
    pnl_lock(&test->lock);
    *out_key = PNL_KEY_NONE;
    if (test->keys_len > 0) {
        *out_key = test->keys[0];
        term->mouse_x = test->keys_x[0];
        term->mouse_y = test->keys_y[0];
        test->keys_len--;
        memmove(test->keys, test->keys + 1, test->keys_len * sizeof(*test->keys));
        memmove(test->keys_x, test->keys_x + 1, test->keys_len * sizeof(*test->keys_x));
        memmove(test->keys_y, test->keys_y + 1, test->keys_len * sizeof(*test->keys_y));
    }
    pnl_unlock(&test->lock);
    return PNL_OK;
}

static void pnl_virtual_snapshot(pnl_test_t *test, const struct pnl_screen *screen)
{
    const struct pnl_cell *cells = screen->front_valid ? screen->front : screen->back;
    size_t len = 0;
    int x;
    int y;

    for (y = 0; cells != NULL && y < screen->height && y < test->height; y++) {
        size_t row = len;

        for (x = 0; x < screen->width && x < test->width; x++) {
            uint32_t cp = cells[(size_t)y * (size_t)screen->width + (size_t)x].cp;

            if (cp != 0)
                len += pnl_utf8_put(cp, test->text + len);
            if (cp != 0 && cp != ' ')
                row = len;
        }
        len = row;
        test->text[len++] = '\n';
    }
    test->text[len] = '\0';
}

static int pnl_virtual_ready(pnl_test_t *test)
{
    if (test->keys_len > 0)
        return 1;
    if (test->woken) {
        test->woken = 0;
        return 0;
    }
    if (test->clock_ms != test->clock_seen) {
        test->clock_seen = test->clock_ms;
        return 0;
    }
    return -1;
}

int pnl_term_wait(struct pnl_term *term, const struct pnl_watch *watches, size_t watches_len, int timeout_ms,
                  unsigned char *out_ready)
{
    pnl_test_t *test = pnl_test_now;
    pnl_app_t *app;

    if (test == NULL)
        return pnl_os_wait(term, watches, watches_len, timeout_ms, out_ready);
    app = (pnl_app_t *)(void *)((char *)term - offsetof(pnl_app_t, term));
    for (;;) {
        int ready;

        pnl_lock(&test->lock);
        ready = pnl_virtual_ready(test);
        if (ready == 0)
            term->woken = 1;
        if (ready < 0 && app->tasks_running == 0 && !test->idle) {
            pnl_virtual_snapshot(test, &app->screen);
            test->idle = 1;
        }
        pnl_unlock(&test->lock);
        if (ready >= 0)
            return ready;
        if (timeout_ms == 0)
            return 0;
        if (app->tasks_running > 0) {
            pnl_nap_ms(PNL_VIRTUAL_BUSY_MS);
            return 0;
        }
        pnl_nap_ms(PNL_VIRTUAL_NAP_MS);
    }
}

static int pnl_test_settle(pnl_test_t *test)
{
    long long waited = 0;

    for (;;) {
        int done;

        pnl_lock(&test->lock);
        done = test->finished || (test->idle && test->keys_len == 0);
        pnl_unlock(&test->lock);
        if (done)
            return PNL_OK;
        if (waited >= PNL_VIRTUAL_TIMEOUT_MS)
            return PNL_ERR_IO;
        pnl_nap_ms(PNL_VIRTUAL_NAP_MS);
        waited += PNL_VIRTUAL_NAP_MS;
    }
}

static void pnl_test_main(void *arg)
{
    pnl_test_t *test = arg;
    int status = pnl_run_interactive(test->app);

    pnl_lock(&test->lock);
    test->status = status;
    test->finished = 1;
    pnl_virtual_snapshot(test, &test->app->screen);
    pnl_unlock(&test->lock);
}

static int pnl_test_push_at(pnl_test_t *test, int key, int x, int y)
{
    int full;

    pnl_lock(&test->lock);
    full = test->keys_len == PNL_VIRTUAL_KEYS;
    pnl_unlock(&test->lock);
    if (full && pnl_test_settle(test) != PNL_OK)
        return PNL_ERR_IO;
    pnl_lock(&test->lock);
    if (!test->finished) {
        test->keys[test->keys_len] = key;
        test->keys_x[test->keys_len] = x;
        test->keys_y[test->keys_len] = y;
        test->keys_len++;
        test->idle = 0;
    }
    pnl_unlock(&test->lock);
    return PNL_OK;
}

static int pnl_test_push(pnl_test_t *test, int key)
{
    return pnl_test_push_at(test, key, 0, 0);
}

static int pnl_test_mouse(pnl_test_t *test, const char *token)
{
    int x = 0;
    int y = 0;
    char end = '\0';

    if (strcmp(token, "wheelup") == 0 || strcmp(token, "wheeldown") == 0)
        return pnl_test_push(test, token[5] == 'u' ? PNL_KEY_WHEEL_UP : PNL_KEY_WHEEL_DOWN);
    if (sscanf(token, "click:%d,%d%c", &x, &y, &end) != 2 || x < 0 || y < 0 || x >= test->width ||
        y >= test->height)
        return PNL_ERR_ARG;
    return pnl_test_push_at(test, PNL_KEY_CLICK, x, y);
}

static void pnl_test_free(pnl_test_t *test)
{
    pnl_app_t *app = test->app;

    pnl_lock_destroy(&test->lock);
    pnl_mem_free(&app->mem, test->text);
    pnl_mem_free(&app->mem, test);
}

int pnl_test_create(pnl_app_t *app, int width, int height, pnl_test_t **out_test)
{
    pnl_test_t *test;
    int status;

    if (out_test != NULL)
        *out_test = NULL;
    if (app == NULL || out_test == NULL || width < 10 || height < 4 || width > 1000 || height > 1000 ||
        app->running || app->cli || pnl_test_now != NULL)
        return PNL_ERR_ARG;
    test = pnl_mem_alloc(&app->mem, sizeof(*test));
    if (test == NULL)
        return PNL_ERR_NOMEM;
    memset(test, 0, sizeof(*test));
    test->app = app;
    test->width = width;
    test->height = height;
    test->text_cap = (size_t)width * (size_t)height * 4 + (size_t)height + 1;
    test->text = pnl_mem_alloc(&app->mem, test->text_cap);
    status = test->text == NULL ? PNL_ERR_NOMEM : pnl_lock_init(&test->lock);
    if (status != PNL_OK) {
        pnl_mem_free(&app->mem, test->text);
        pnl_mem_free(&app->mem, test);
        return status;
    }
    test->text[0] = '\0';
    pnl_test_now = test;
    status = pnl_thread_start(&test->thread, pnl_test_main, test);
    if (status != PNL_OK) {
        pnl_test_now = NULL;
        pnl_test_free(test);
        return status;
    }
    status = pnl_test_settle(test);
    if (status != PNL_OK) {
        pnl_test_destroy(test);
        return status;
    }
    *out_test = test;
    return PNL_OK;
}

void pnl_test_destroy(pnl_test_t *test)
{
    if (test == NULL)
        return;
    pnl_lock(&test->lock);
    test->keys_len = 0;
    if (!test->finished)
        test->keys[test->keys_len++] = PNL_KEY_CTRL_C;
    pnl_unlock(&test->lock);
    pnl_thread_join(&test->thread);
    pnl_test_now = NULL;
    pnl_test_free(test);
}

static const char *pnl_test_token(const char *at, char *out, size_t out_len)
{
    size_t len = 0;

    while (*at == ' ')
        at++;
    if (*at == '"') {
        for (at++; *at != '\0' && *at != '"' && len + 1 < out_len; at++)
            out[len++] = *at;
        out[len] = '\0';
        return *at == '"' ? at + 1 : at;
    }
    for (; *at != '\0' && *at != ' ' && len + 1 < out_len; at++)
        out[len++] = *at;
    out[len] = '\0';
    return at;
}

static int pnl_test_text(pnl_test_t *test, const char *text)
{
    int status = PNL_OK;

    while (*text != '\0' && status == PNL_OK) {
        uint32_t cp;

        text += pnl_utf8_next(text, strlen(text), &cp);
        status = pnl_test_push(test, (int)cp);
    }
    return status;
}

int pnl_test_keys(pnl_test_t *test, const char *keys)
{
    char token[256];
    int status = PNL_OK;

    if (test == NULL || keys == NULL)
        return PNL_ERR_ARG;
    while (*keys != '\0' && status == PNL_OK) {
        int quoted;
        int key;

        while (*keys == ' ')
            keys++;
        quoted = *keys == '"';
        keys = pnl_test_token(keys, token, sizeof(token));
        if (quoted)
            status = pnl_test_text(test, token);
        else if (token[0] == '\0')
            break;
        else if (strncmp(token, "click:", 6) == 0 || strncmp(token, "wheel", 5) == 0)
            status = pnl_test_mouse(test, token);
        else if (!pnl_key_parse(token, &key) || key == PNL_KEY_NONE)
            status = PNL_ERR_ARG;
        else
            status = pnl_test_push(test, key);
    }
    return status == PNL_OK ? pnl_test_settle(test) : status;
}

int pnl_test_wait(pnl_test_t *test, int ms)
{
    if (test == NULL || ms < 0)
        return PNL_ERR_ARG;
    pnl_lock(&test->lock);
    test->clock_ms += ms;
    test->idle = test->finished;
    pnl_unlock(&test->lock);
    return pnl_test_settle(test);
}

const char *pnl_test_screen(const pnl_test_t *test)
{
    return test != NULL ? test->text : "";
}

static char *pnl_test_read(pnl_app_t *app, const char *path)
{
    FILE *file = fopen(path, "rb");
    char *data;
    size_t len;

    if (file == NULL)
        return NULL;
    data = pnl_mem_alloc(&app->mem, PNL_TEST_FILE_MAX + 1);
    if (data != NULL) {
        len = fread(data, 1, PNL_TEST_FILE_MAX, file);
        data[len] = '\0';
    }
    fclose(file);
    return data;
}

static int pnl_test_save(const char *path, const char *text)
{
    FILE *file = fopen(path, "wb");
    int ok;

    if (file == NULL)
        return PNL_ERR_IO;
    ok = fputs(text, file) >= 0;
    ok = fclose(file) == 0 && ok;
    return ok ? PNL_OK : PNL_ERR_IO;
}

int pnl_test_expect(pnl_test_t *test, const char *path)
{
    const char *update = getenv("TUIKIT_SNAPSHOTS");
    char fresh[PNL_TEST_PATH_MAX];
    char *saved;
    int same;

    if (test == NULL || path == NULL || strlen(path) + 5 > sizeof(fresh))
        return PNL_ERR_ARG;
    if (update != NULL && strcmp(update, "update") == 0)
        return pnl_test_save(path, test->text);
    snprintf(fresh, sizeof(fresh), "%s.new", path);
    saved = pnl_test_read(test->app, path);
    if (saved == NULL) {
        pnl_test_save(fresh, test->text);
        return PNL_ERR_NOT_FOUND;
    }
    same = strcmp(saved, test->text) == 0;
    pnl_mem_free(&test->app->mem, saved);
    if (same) {
        remove(fresh);
        return PNL_OK;
    }
    pnl_test_save(fresh, test->text);
    return PNL_ERR_MISMATCH;
}

#include <stdio.h>
#include <string.h>

#define PNL_GRAPH_CAPACITY_MAX 4096

struct pnl_text_widget {
    pnl_app_t *app;
    char *text;
    int style;
};

struct pnl_bar_widget {
    pnl_app_t *app;
    char *label;
    int flags;
    struct pnl_glide glide;
};

struct pnl_graph_widget {
    pnl_app_t *app;
    char *label;
    int kind;
    int rows;
    int flags;
    double *values;
    size_t len;
    size_t capacity;
    double min;
    double max;
};

struct pnl_spinner_widget {
    pnl_app_t *app;
    char *text;
    int kind;
};

struct pnl_stack_child {
    pnl_widget_t *widget;
    int size;
    int x;
    int y;
    int width;
    int height;
};

struct pnl_stack_widget {
    pnl_app_t *app;
    int kind;
    struct pnl_stack_child *children;
    size_t len;
    size_t cap;
    int focus;
};

static int pnl_text_set(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_text_widget *)widget->ctx)->text, text);
}

static const char *pnl_text_get(const pnl_widget_t *widget)
{
    return ((const struct pnl_text_widget *)widget->ctx)->text;
}

static int pnl_spinner_set(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_spinner_widget *)widget->ctx)->text, text);
}

static const char *pnl_spinner_get(const pnl_widget_t *widget)
{
    return ((const struct pnl_spinner_widget *)widget->ctx)->text;
}

static int pnl_bar_set_label(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_bar_widget *)widget->ctx)->label, text);
}

static const char *pnl_bar_label(const pnl_widget_t *widget)
{
    return ((const struct pnl_bar_widget *)widget->ctx)->label;
}

static int pnl_bar_set(pnl_widget_t *widget, double value)
{
    struct pnl_bar_widget *bar = widget->ctx;

    pnl_glide_to(widget->app, &bar->glide, value, PNL_MOTION_COUNT);
    widget->app->dirty = 1;
    return PNL_OK;
}

static int pnl_bar_get(const pnl_widget_t *widget, double *out_value)
{
    *out_value = ((const struct pnl_bar_widget *)widget->ctx)->glide.to;
    return PNL_OK;
}

static int pnl_graph_set_label(pnl_widget_t *widget, const char *text)
{
    return pnl_widget_copy(widget->app, &((struct pnl_graph_widget *)widget->ctx)->label, text);
}

static const char *pnl_graph_label(const pnl_widget_t *widget)
{
    return ((const struct pnl_graph_widget *)widget->ctx)->label;
}

static int pnl_graph_push(pnl_widget_t *widget, double value)
{
    struct pnl_graph_widget *graph = widget->ctx;

    if (graph->len == graph->capacity) {
        memmove(graph->values, graph->values + 1, (graph->capacity - 1) * sizeof(*graph->values));
        graph->len--;
    }
    graph->values[graph->len++] = value;
    widget->app->dirty = 1;
    return PNL_OK;
}

static int pnl_graph_last(const pnl_widget_t *widget, double *out_value)
{
    const struct pnl_graph_widget *graph = widget->ctx;

    if (graph->len == 0)
        return PNL_ERR_NOT_FOUND;
    *out_value = graph->values[graph->len - 1];
    return PNL_OK;
}

static int pnl_graph_range(pnl_widget_t *widget, double min, double max)
{
    struct pnl_graph_widget *graph = widget->ctx;

    graph->min = min;
    graph->max = max;
    widget->app->dirty = 1;
    return PNL_OK;
}

int pnl_widget_create(pnl_app_t *app, const pnl_widget_class_t *widget_class, void *widget_ctx,
                      pnl_widget_t **out_widget)
{
    pnl_widget_t *widget;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || widget_class == NULL || widget_class->draw_fn == NULL || out_widget == NULL)
        return PNL_ERR_ARG;
    widget = pnl_mem_alloc(&app->mem, sizeof(*widget));
    if (widget == NULL)
        return PNL_ERR_NOMEM;
    memset(widget, 0, sizeof(*widget));
    widget->app = app;
    widget->widget_class = *widget_class;
    widget->ctx = widget_ctx;
    *out_widget = widget;
    return PNL_OK;
}

void pnl_widget_destroy(pnl_widget_t *widget)
{
    if (widget == NULL)
        return;
    if (widget->widget_class.release_fn != NULL)
        widget->widget_class.release_fn(widget->ctx);
    pnl_mem_free(&widget->app->mem, widget);
}

void *pnl_widget_ctx(const pnl_widget_t *widget)
{
    return widget != NULL ? widget->ctx : NULL;
}

void pnl_widget_paint(pnl_widget_t *widget, pnl_canvas_t *canvas)
{
    if (canvas->width > 0 && canvas->height > 0)
        widget->widget_class.draw_fn(widget->ctx, widget->app, canvas);
}

static void pnl_widget_place(pnl_widget_t *widget, pnl_canvas_t *canvas, int x, int y, int width, int height,
                             int focused)
{
    struct pnl_canvas inner = *canvas;

    inner.focused = focused;
    inner.x = canvas->x + x;
    inner.y = canvas->y + y;
    inner.width = x + width > canvas->width ? canvas->width - x : width;
    inner.height = y + height > canvas->height ? canvas->height - y : height;
    pnl_widget_paint(widget, &inner);
}

int pnl_widget_draw(pnl_widget_t *widget, pnl_canvas_t *canvas, int x, int y, int width, int height)
{
    struct pnl_canvas inner;

    if (widget == NULL || canvas == NULL || x < 0 || y < 0 || width < 0 || height < 0)
        return PNL_ERR_ARG;
    inner = *canvas;
    inner.x = canvas->x + x;
    inner.y = canvas->y + y;
    inner.width = x + width > canvas->width ? canvas->width - x : width;
    inner.height = y + height > canvas->height ? canvas->height - y : height;
    pnl_widget_paint(widget, &inner);
    return PNL_OK;
}

int pnl_widget_key(pnl_widget_t *widget, int key)
{
    if (widget == NULL || widget->widget_class.key_fn == NULL)
        return 0;
    return widget->widget_class.key_fn(widget->ctx, widget->app, key);
}

int pnl_widget_height(pnl_widget_t *widget, int width)
{
    if (widget == NULL || widget->widget_class.height_fn == NULL || width <= 0)
        return 0;
    return widget->widget_class.height_fn(widget->ctx, widget->app, width);
}

const char *pnl_widget_hints(const pnl_widget_t *widget)
{
    if (widget == NULL || widget->widget_class.hints_fn == NULL)
        return NULL;
    return widget->widget_class.hints_fn(widget->ctx);
}

static void pnl_widget_view_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    pnl_widget_t *widget = ctx;
    const char *hints = pnl_widget_hints(widget);

    (void)app;
    snprintf(widget->footer_text, sizeof(widget->footer_text), "%s%s%sesc: back", hints ? hints : "",
             hints ? "  " : "", pnl_widget_focusable(widget) ? "tab: next  " : "");
    pnl_widget_paint(widget, canvas);
}

static int pnl_widget_view_key(void *ctx, pnl_app_t *app, int key)
{
    (void)app;
    if (pnl_widget_key(ctx, key))
        return 1;
    if ((key != PNL_KEY_TAB && key != PNL_KEY_BACKTAB) || !pnl_widget_focusable(ctx))
        return 0;
    pnl_widget_focus_edge(ctx, key == PNL_KEY_BACKTAB);
    return 1;
}

int pnl_view_push_widget(pnl_app_t *app, const char *title, pnl_widget_t *widget)
{
    struct pnl_view view;
    int status;

    if (app == NULL || widget == NULL || widget->app != app)
        return PNL_ERR_ARG;
    memset(&view, 0, sizeof(view));
    view.draw_fn = pnl_widget_view_draw;
    view.key_fn = pnl_widget_view_key;
    view.ctx = widget;
    snprintf(widget->footer_text, sizeof(widget->footer_text), "esc: back");
    widget->footer = widget->footer_text;
    view.footer_ref = &widget->footer;
    if (title != NULL && (view.title = pnl_mem_copy(&app->mem, title)) == NULL)
        return PNL_ERR_NOMEM;
    status = pnl_view_push_internal(app, &view);
    if (status != PNL_OK)
        pnl_mem_free(&app->mem, view.title);
    return status;
}

int pnl_widget_builtin(pnl_app_t *app, int kind, const pnl_widget_class_t *widget_class,
                       const struct pnl_widget_ops *ops, void *ctx, pnl_widget_t **out_widget)
{
    int status = pnl_widget_create(app, widget_class, ctx, out_widget);

    if (status != PNL_OK) {
        widget_class->release_fn(ctx);
        return status;
    }
    (*out_widget)->kind = kind;
    (*out_widget)->ops = ops;
    return PNL_OK;
}

int pnl_widget_copy(pnl_app_t *app, char **slot, const char *text)
{
    char *copy = NULL;

    if (text != NULL && (copy = pnl_mem_copy(&app->mem, text)) == NULL)
        return PNL_ERR_NOMEM;
    pnl_mem_free(&app->mem, *slot);
    *slot = copy;
    app->dirty = 1;
    return PNL_OK;
}

static int pnl_lines(const char *text)
{
    int lines = 1;

    for (; text != NULL && *text != '\0'; text++)
        lines += *text == '\n';
    return text == NULL ? 0 : lines;
}

static void pnl_text_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_text_widget *widget = ctx;
    const char *line = widget->text;
    char buf[PNL_CELL_MAX];
    int y;

    (void)app;
    for (y = 0; line != NULL && y < pnl_canvas_height(canvas); y++) {
        const char *end = (strchr)(line, '\n');
        size_t len = end != NULL ? (size_t)(end - line) : strlen(line);

        snprintf(buf, sizeof(buf), "%.*s", (int)len, line);
        pnl_canvas_text(canvas, 0, y, widget->style, buf);
        line = end != NULL ? end + 1 : NULL;
    }
}

static int pnl_text_height(void *ctx, pnl_app_t *app, int width)
{
    struct pnl_text_widget *widget = ctx;

    (void)app;
    (void)width;
    return pnl_lines(widget->text);
}

static void pnl_text_release(void *ctx)
{
    struct pnl_text_widget *widget = ctx;

    pnl_mem_free(&widget->app->mem, widget->text);
    pnl_mem_free(&widget->app->mem, widget);
}

int pnl_widget_text(pnl_app_t *app, const char *text, int style, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t text_class = {pnl_text_draw, NULL, pnl_text_height, NULL, pnl_text_release};
    static const struct pnl_widget_ops text_ops = {pnl_text_set, pnl_text_get, NULL, NULL, NULL};
    struct pnl_text_widget *widget;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL)
        return PNL_ERR_ARG;
    widget = pnl_mem_alloc(&app->mem, sizeof(*widget));
    if (widget == NULL)
        return PNL_ERR_NOMEM;
    memset(widget, 0, sizeof(*widget));
    widget->app = app;
    widget->style = style;
    if (pnl_widget_copy(app, &widget->text, text != NULL ? text : "") != PNL_OK) {
        pnl_text_release(widget);
        return PNL_ERR_NOMEM;
    }
    return pnl_widget_builtin(app, PNL_WIDGET_TEXT, &text_class, &text_ops, widget, out_widget);
}

static void pnl_label_draw(pnl_canvas_t *canvas, const char *label, int *x)
{
    if (label == NULL)
        return;
    pnl_canvas_text(canvas, 0, 0, PNL_FG(PNL_ROLE_MUTED), label);
    *x = pnl_text_width(label) + 1;
}

static void pnl_bar_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_bar_widget *widget = ctx;
    int x = 0;

    pnl_label_draw(canvas, widget->label, &x);
    pnl_canvas_bar(canvas, x, 0, pnl_canvas_width(canvas) - x,
                   pnl_glide_at(app, &widget->glide, PNL_MOTION_COUNT), widget->flags);
}

static int pnl_one_row(void *ctx, pnl_app_t *app, int width)
{
    (void)ctx;
    (void)app;
    (void)width;
    return 1;
}

static void pnl_bar_release(void *ctx)
{
    struct pnl_bar_widget *widget = ctx;

    pnl_mem_free(&widget->app->mem, widget->label);
    pnl_mem_free(&widget->app->mem, widget);
}

int pnl_widget_bar(pnl_app_t *app, const char *label, int flags, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t bar_class = {pnl_bar_draw, NULL, pnl_one_row, NULL, pnl_bar_release};
    static const struct pnl_widget_ops bar_ops = {pnl_bar_set_label, pnl_bar_label, pnl_bar_set, pnl_bar_get, NULL};
    struct pnl_bar_widget *widget;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL)
        return PNL_ERR_ARG;
    widget = pnl_mem_alloc(&app->mem, sizeof(*widget));
    if (widget == NULL)
        return PNL_ERR_NOMEM;
    memset(widget, 0, sizeof(*widget));
    widget->app = app;
    widget->flags = flags;
    if (pnl_widget_copy(app, &widget->label, label) != PNL_OK) {
        pnl_bar_release(widget);
        return PNL_ERR_NOMEM;
    }
    return pnl_widget_builtin(app, PNL_WIDGET_BAR, &bar_class, &bar_ops, widget, out_widget);
}

static void pnl_graph_bounds(const struct pnl_graph_widget *widget, double *min, double *max)
{
    size_t i;

    *min = widget->min;
    *max = widget->max;
    if (widget->max > widget->min)
        return;
    *min = widget->len > 0 ? widget->values[0] : 0.0;
    *max = *min;
    for (i = 1; i < widget->len; i++) {
        *min = widget->values[i] < *min ? widget->values[i] : *min;
        *max = widget->values[i] > *max ? widget->values[i] : *max;
    }
    if (widget->kind != PNL_GRAPH_PLOT || *max <= *min)
        *min = *min < 0.0 ? *min : 0.0;
    if (*max <= *min)
        *max = *min + 1.0;
}

static void pnl_graph_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_graph_widget *widget = ctx;
    int width = pnl_canvas_width(canvas);
    int top = 0;
    int rows;
    double min;
    double max;

    (void)app;
    if (widget->label != NULL) {
        pnl_canvas_text(canvas, 0, 0, PNL_FG(PNL_ROLE_MUTED), widget->label);
        top = 1;
    }
    rows = widget->rows > 0 ? widget->rows : pnl_canvas_height(canvas) - top;
    if (rows > pnl_canvas_height(canvas) - top)
        rows = pnl_canvas_height(canvas) - top;
    if (rows <= 0)
        return;
    pnl_graph_bounds(widget, &min, &max);
    if (widget->kind == PNL_GRAPH_SPARKLINE)
        pnl_canvas_sparkline(canvas, 0, top, width, widget->values, widget->len, max, widget->flags);
    else if (widget->kind == PNL_GRAPH_CHART)
        pnl_canvas_chart(canvas, 0, top, width, rows, widget->values, widget->len, max, widget->flags);
    else
        pnl_canvas_plot(canvas, 0, top, width, rows, widget->values, widget->len, min, max, widget->flags);
}

static int pnl_graph_height(void *ctx, pnl_app_t *app, int width)
{
    struct pnl_graph_widget *widget = ctx;

    (void)app;
    (void)width;
    if (widget->rows == 0)
        return 0;
    return widget->rows + (widget->label != NULL);
}

static void pnl_graph_release(void *ctx)
{
    struct pnl_graph_widget *widget = ctx;

    pnl_mem_free(&widget->app->mem, widget->values);
    pnl_mem_free(&widget->app->mem, widget->label);
    pnl_mem_free(&widget->app->mem, widget);
}

int pnl_widget_graph(pnl_app_t *app, const char *label, int kind, int rows, int flags, size_t capacity,
                     pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t graph_class = {pnl_graph_draw, NULL, pnl_graph_height, NULL,
                                                   pnl_graph_release};
    static const struct pnl_widget_ops graph_ops = {pnl_graph_set_label, pnl_graph_label, pnl_graph_push,
                                                    pnl_graph_last, pnl_graph_range};
    struct pnl_graph_widget *widget;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL || kind < PNL_GRAPH_SPARKLINE || kind > PNL_GRAPH_PLOT || rows < 0 ||
        rows > PNL_PLOT_ROWS || capacity == 0 || capacity > PNL_GRAPH_CAPACITY_MAX)
        return PNL_ERR_ARG;
    widget = pnl_mem_alloc(&app->mem, sizeof(*widget));
    if (widget == NULL)
        return PNL_ERR_NOMEM;
    memset(widget, 0, sizeof(*widget));
    widget->app = app;
    widget->kind = kind;
    widget->rows = kind == PNL_GRAPH_SPARKLINE ? 1 : rows;
    widget->flags = flags;
    widget->capacity = capacity;
    widget->values = pnl_mem_alloc(&app->mem, capacity * sizeof(*widget->values));
    if (widget->values == NULL || pnl_widget_copy(app, &widget->label, label) != PNL_OK) {
        pnl_graph_release(widget);
        return PNL_ERR_NOMEM;
    }
    return pnl_widget_builtin(app, PNL_WIDGET_GRAPH, &graph_class, &graph_ops, widget, out_widget);
}

static void pnl_spinner_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_spinner_widget *widget = ctx;

    (void)app;
    pnl_canvas_spinner(canvas, 0, 0, PNL_FG(PNL_ROLE_ACCENT), widget->kind);
    if (widget->text != NULL)
        pnl_canvas_text(canvas, 2, 0, 0, widget->text);
}

static void pnl_spinner_release(void *ctx)
{
    struct pnl_spinner_widget *widget = ctx;

    pnl_mem_free(&widget->app->mem, widget->text);
    pnl_mem_free(&widget->app->mem, widget);
}

int pnl_widget_spinner(pnl_app_t *app, int kind, const char *text, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t spinner_class = {pnl_spinner_draw, NULL, pnl_one_row, NULL,
                                                     pnl_spinner_release};
    static const struct pnl_widget_ops spinner_ops = {pnl_spinner_set, pnl_spinner_get, NULL, NULL, NULL};
    struct pnl_spinner_widget *widget;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL || kind < PNL_SPINNER_DOTS || kind > PNL_SPINNER_BOUNCE)
        return PNL_ERR_ARG;
    widget = pnl_mem_alloc(&app->mem, sizeof(*widget));
    if (widget == NULL)
        return PNL_ERR_NOMEM;
    memset(widget, 0, sizeof(*widget));
    widget->app = app;
    widget->kind = kind;
    if (pnl_widget_copy(app, &widget->text, text) != PNL_OK) {
        pnl_spinner_release(widget);
        return PNL_ERR_NOMEM;
    }
    return pnl_widget_builtin(app, PNL_WIDGET_SPINNER, &spinner_class, &spinner_ops, widget, out_widget);
}

static int pnl_stack_sizes(struct pnl_stack_widget *stack, int length, int across)
{
    int left = length - (stack->kind == PNL_STACK_COLUMNS && stack->len > 0 ? (int)stack->len - 1 : 0);
    int flexible = 0;
    size_t i;

    for (i = 0; i < stack->len; i++) {
        struct pnl_stack_child *child = &stack->children[i];

        child->height = child->size;
        if (child->size == 0 && stack->kind == PNL_STACK_ROWS)
            child->height = pnl_widget_height(child->widget, across);
        if (child->height > 0)
            left -= child->height;
        else
            flexible++;
    }
    return flexible > 0 && left > 0 ? left / flexible : 0;
}

static int pnl_stack_first(const struct pnl_stack_widget *stack, int from, int step)
{
    int i;

    for (i = from; i >= 0 && i < (int)stack->len; i += step) {
        if (pnl_widget_focusable(stack->children[i].widget))
            return i;
    }
    return -1;
}

static int pnl_stack_current(struct pnl_stack_widget *stack)
{
    if (stack->focus < 0 || stack->focus >= (int)stack->len ||
        !pnl_widget_focusable(stack->children[stack->focus].widget))
        stack->focus = pnl_stack_first(stack, 0, 1);
    return stack->focus;
}

static int pnl_stack_child_focused(struct pnl_stack_widget *stack, const pnl_canvas_t *canvas, size_t i)
{
    if (!pnl_widget_focusable(stack->children[i].widget))
        return canvas->focused;
    return canvas->focused && (int)i == pnl_stack_current(stack);
}

static void pnl_stack_draw(void *ctx, pnl_app_t *app, pnl_canvas_t *canvas)
{
    struct pnl_stack_widget *stack = ctx;
    int columns = stack->kind == PNL_STACK_COLUMNS;
    int length = columns ? pnl_canvas_width(canvas) : pnl_canvas_height(canvas);
    int across = columns ? pnl_canvas_height(canvas) : pnl_canvas_width(canvas);
    int share = pnl_stack_sizes(stack, length, pnl_canvas_width(canvas));
    int pos = 0;
    size_t i;

    (void)app;
    for (i = 0; i < stack->len; i++) {
        struct pnl_stack_child *child = &stack->children[i];
        int size = child->height > 0 ? child->height : share;

        size = size > length - pos ? length - pos : size;
        child->x = columns ? pos : 0;
        child->y = columns ? 0 : pos;
        child->width = columns ? size : across;
        child->height = columns ? across : size;
        if (size > 0)
            pnl_widget_place(child->widget, canvas, child->x, child->y, child->width, child->height,
                             pnl_stack_child_focused(stack, canvas, i));
        child->x += canvas->x;
        child->y += canvas->y;
        pos += size + columns;
    }
}

static int pnl_stack_mouse(struct pnl_stack_widget *stack, pnl_app_t *app, int key)
{
    size_t i;

    for (i = 0; i < stack->len; i++) {
        const struct pnl_stack_child *child = &stack->children[i];
        int x = app->term.mouse_x;
        int y = app->term.mouse_y;

        if (x < child->x || y < child->y || x >= child->x + child->width || y >= child->y + child->height)
            continue;
        app->mouse_ox = child->x;
        app->mouse_oy = child->y;
        if (key == PNL_KEY_CLICK && pnl_widget_focusable(child->widget)) {
            stack->focus = (int)i;
            app->dirty = 1;
        }
        return pnl_widget_key(child->widget, key) || (key == PNL_KEY_CLICK && stack->focus == (int)i);
    }
    return 0;
}

static int pnl_stack_step(struct pnl_stack_widget *stack, int key)
{
    int back = key == PNL_KEY_BACKTAB || key == PNL_KEY_UP || key == PNL_KEY_LEFT;
    int rows = stack->kind == PNL_STACK_ROWS && (key == PNL_KEY_UP || key == PNL_KEY_DOWN);
    int columns = stack->kind == PNL_STACK_COLUMNS && (key == PNL_KEY_LEFT || key == PNL_KEY_RIGHT);
    int next;

    if (key != PNL_KEY_TAB && key != PNL_KEY_BACKTAB && key != PNL_KEY_ENTER && !rows && !columns)
        return 0;
    next = pnl_stack_first(stack, stack->focus + (back ? -1 : 1), back ? -1 : 1);
    if (next < 0)
        return 0;
    stack->focus = next;
    pnl_widget_focus_edge(stack->children[next].widget, back);
    stack->app->dirty = 1;
    return 1;
}

static int pnl_stack_key(void *ctx, pnl_app_t *app, int key)
{
    struct pnl_stack_widget *stack = ctx;
    int focus;
    size_t i;

    if (pnl_is_mouse_key(key))
        return pnl_stack_mouse(stack, app, key);
    focus = pnl_stack_current(stack);
    if (focus >= 0 && pnl_widget_key(stack->children[focus].widget, key))
        return 1;
    if (focus >= 0 && pnl_stack_step(stack, key))
        return 1;
    for (i = 0; i < stack->len; i++) {
        if (!pnl_widget_focusable(stack->children[i].widget) && pnl_widget_key(stack->children[i].widget, key))
            return 1;
    }
    return 0;
}

static int pnl_stack_height(void *ctx, pnl_app_t *app, int width)
{
    struct pnl_stack_widget *stack = ctx;
    int total = 0;
    size_t i;

    (void)app;
    for (i = 0; i < stack->len; i++) {
        int size = stack->children[i].size > 0 ? stack->children[i].size
                                                : pnl_widget_height(stack->children[i].widget, width);

        if (size == 0)
            return 0;
        total = stack->kind == PNL_STACK_ROWS ? total + size : (size > total ? size : total);
    }
    return total;
}

static const char *pnl_stack_hints(void *ctx)
{
    struct pnl_stack_widget *stack = ctx;
    int focus = pnl_stack_current(stack);
    size_t i;

    if (focus >= 0 && pnl_widget_hints(stack->children[focus].widget) != NULL)
        return pnl_widget_hints(stack->children[focus].widget);
    for (i = 0; i < stack->len; i++) {
        const char *hints = pnl_widget_hints(stack->children[i].widget);

        if (hints != NULL)
            return hints;
    }
    return NULL;
}

static void pnl_stack_release(void *ctx)
{
    struct pnl_stack_widget *stack = ctx;

    pnl_mem_free(&stack->app->mem, stack->children);
    pnl_mem_free(&stack->app->mem, stack);
}

int pnl_widget_stack(pnl_app_t *app, int kind, pnl_widget_t **out_widget)
{
    static const pnl_widget_class_t stack_class = {pnl_stack_draw, pnl_stack_key, pnl_stack_height,
                                                   pnl_stack_hints, pnl_stack_release};
    struct pnl_stack_widget *stack;

    if (out_widget != NULL)
        *out_widget = NULL;
    if (app == NULL || out_widget == NULL || (kind != PNL_STACK_ROWS && kind != PNL_STACK_COLUMNS))
        return PNL_ERR_ARG;
    stack = pnl_mem_alloc(&app->mem, sizeof(*stack));
    if (stack == NULL)
        return PNL_ERR_NOMEM;
    memset(stack, 0, sizeof(*stack));
    stack->app = app;
    stack->kind = kind;
    stack->focus = -1;
    return pnl_widget_builtin(app, PNL_WIDGET_STACK, &stack_class, NULL, stack, out_widget);
}

static int pnl_widget_contains(const pnl_widget_t *root, const pnl_widget_t *target)
{
    const struct pnl_stack_widget *ctx;
    size_t i;

    if (root == target)
        return 1;
    if (root->kind != PNL_WIDGET_STACK)
        return 0;
    ctx = root->ctx;
    for (i = 0; i < ctx->len; i++) {
        if (pnl_widget_contains(ctx->children[i].widget, target))
            return 1;
    }
    return 0;
}

int pnl_widget_add(pnl_widget_t *stack, pnl_widget_t *child, int size)
{
    struct pnl_stack_widget *ctx;
    int status;

    if (stack == NULL || child == NULL || stack->kind != PNL_WIDGET_STACK || child->app != stack->app || size < 0 ||
        pnl_widget_contains(child, stack))
        return PNL_ERR_ARG;
    ctx = stack->ctx;
    status = pnl_mem_grow(&stack->app->mem, (void **)&ctx->children, &ctx->cap, ctx->len + 1,
                          sizeof(*ctx->children));
    if (status != PNL_OK)
        return status;
    memset(&ctx->children[ctx->len], 0, sizeof(ctx->children[0]));
    ctx->children[ctx->len].widget = child;
    ctx->children[ctx->len].size = size;
    ctx->len++;
    stack->app->dirty = 1;
    return PNL_OK;
}

int pnl_widget_focusable(const pnl_widget_t *widget)
{
    const struct pnl_stack_widget *stack;
    size_t i;

    if (widget == NULL)
        return 0;
    if (widget->kind != PNL_WIDGET_STACK)
        return widget->focusable;
    stack = widget->ctx;
    for (i = 0; i < stack->len; i++) {
        if (pnl_widget_focusable(stack->children[i].widget))
            return 1;
    }
    return 0;
}

void pnl_widget_focus_edge(pnl_widget_t *widget, int last)
{
    struct pnl_stack_widget *stack;

    if (widget == NULL || widget->kind != PNL_WIDGET_STACK)
        return;
    stack = widget->ctx;
    stack->focus = last ? pnl_stack_first(stack, (int)stack->len - 1, -1) : pnl_stack_first(stack, 0, 1);
    if (stack->focus >= 0)
        pnl_widget_focus_edge(stack->children[stack->focus].widget, last);
    widget->app->dirty = 1;
}

void pnl_widget_changed(pnl_widget_t *widget)
{
    widget->app->dirty = 1;
    if (widget->change_fn != NULL)
        widget->change_fn(widget->change_ctx, widget->app, widget);
}

int pnl_widget_on_change(pnl_widget_t *widget, pnl_change_fn change_fn, void *change_ctx)
{
    if (widget == NULL)
        return PNL_ERR_ARG;
    widget->change_fn = change_fn;
    widget->change_ctx = change_ctx;
    return PNL_OK;
}

int pnl_widget_set_focusable(pnl_widget_t *widget, int focusable)
{
    if (widget == NULL || widget->kind == PNL_WIDGET_STACK)
        return PNL_ERR_ARG;
    widget->focusable = focusable != 0;
    widget->app->dirty = 1;
    return PNL_OK;
}

int pnl_widget_focus(pnl_widget_t *stack, pnl_widget_t *child)
{
    struct pnl_stack_widget *ctx;
    size_t i;

    if (stack == NULL || child == NULL || stack->kind != PNL_WIDGET_STACK || !pnl_widget_focusable(child))
        return PNL_ERR_ARG;
    ctx = stack->ctx;
    for (i = 0; i < ctx->len; i++) {
        pnl_widget_t *inner = ctx->children[i].widget;

        if (inner == child || (inner->kind == PNL_WIDGET_STACK && pnl_widget_focus(inner, child) == PNL_OK)) {
            ctx->focus = (int)i;
            stack->app->dirty = 1;
            return PNL_OK;
        }
    }
    return PNL_ERR_NOT_FOUND;
}

int pnl_widget_set_text(pnl_widget_t *widget, const char *text)
{
    if (widget == NULL || text == NULL || widget->ops == NULL || widget->ops->set_text == NULL)
        return PNL_ERR_ARG;
    return widget->ops->set_text(widget, text);
}

const char *pnl_widget_get_text(const pnl_widget_t *widget)
{
    if (widget == NULL || widget->ops == NULL || widget->ops->get_text == NULL)
        return NULL;
    return widget->ops->get_text(widget);
}

int pnl_widget_set_value(pnl_widget_t *widget, double value)
{
    if (widget == NULL || !(value == value) || widget->ops == NULL || widget->ops->set_value == NULL)
        return PNL_ERR_ARG;
    return widget->ops->set_value(widget, value);
}

int pnl_widget_get_value(const pnl_widget_t *widget, double *out_value)
{
    if (widget == NULL || out_value == NULL || widget->ops == NULL || widget->ops->get_value == NULL)
        return PNL_ERR_ARG;
    return widget->ops->get_value(widget, out_value);
}

int pnl_widget_set_range(pnl_widget_t *widget, double min, double max)
{
    if (widget == NULL || !(max >= min) || widget->ops == NULL || widget->ops->set_range == NULL)
        return PNL_ERR_ARG;
    return widget->ops->set_range(widget, min, max);
}

#include <string.h>

struct pnl_range {
    uint32_t first;
    uint32_t last;
};

static const struct pnl_range pnl_wide[] = {
    {0x1100, 0x115f}, {0x231a, 0x231b}, {0x2329, 0x232a}, {0x23e9, 0x23ec}, {0x23f0, 0x23f0},
    {0x23f3, 0x23f3}, {0x25fd, 0x25fe}, {0x2614, 0x2615}, {0x2648, 0x2653}, {0x267f, 0x267f},
    {0x2693, 0x2693}, {0x26a1, 0x26a1}, {0x26aa, 0x26ab}, {0x26bd, 0x26be}, {0x26c4, 0x26c5},
    {0x26ce, 0x26ce}, {0x26d4, 0x26d4}, {0x26ea, 0x26ea}, {0x26f2, 0x26f3}, {0x26f5, 0x26f5},
    {0x26fa, 0x26fa}, {0x26fd, 0x26fd}, {0x2705, 0x2705}, {0x270a, 0x270b}, {0x2728, 0x2728},
    {0x274c, 0x274c}, {0x274e, 0x274e}, {0x2753, 0x2755}, {0x2757, 0x2757}, {0x2795, 0x2797},
    {0x27b0, 0x27b0}, {0x27bf, 0x27bf}, {0x2b1b, 0x2b1c}, {0x2b50, 0x2b50}, {0x2b55, 0x2b55},
    {0x2e80, 0x303e}, {0x3041, 0x33ff}, {0x3400, 0x4dbf}, {0x4e00, 0x9fff}, {0xa000, 0xa4cf},
    {0xa960, 0xa97f}, {0xac00, 0xd7a3}, {0xf900, 0xfaff}, {0xfe10, 0xfe19}, {0xfe30, 0xfe6f},
    {0xff00, 0xff60}, {0xffe0, 0xffe6}, {0x16fe0, 0x16fe4}, {0x17000, 0x18cff}, {0x1b000, 0x1b2ff},
    {0x1f004, 0x1f004}, {0x1f0cf, 0x1f0cf}, {0x1f18e, 0x1f18e}, {0x1f191, 0x1f19a}, {0x1f200, 0x1f251},
    {0x1f300, 0x1f320}, {0x1f32d, 0x1f335}, {0x1f337, 0x1f37c}, {0x1f37e, 0x1f393}, {0x1f3a0, 0x1f3ca},
    {0x1f3cf, 0x1f3d3}, {0x1f3e0, 0x1f3f0}, {0x1f3f4, 0x1f3f4}, {0x1f3f8, 0x1f3fa}, {0x1f400, 0x1f43e},
    {0x1f440, 0x1f440}, {0x1f442, 0x1f4fc}, {0x1f4ff, 0x1f53d}, {0x1f54b, 0x1f54e}, {0x1f550, 0x1f567},
    {0x1f57a, 0x1f57a}, {0x1f595, 0x1f596}, {0x1f5a4, 0x1f5a4}, {0x1f5fb, 0x1f64f}, {0x1f680, 0x1f6c5},
    {0x1f6cc, 0x1f6cc}, {0x1f6d0, 0x1f6d2}, {0x1f6d5, 0x1f6d7}, {0x1f6dc, 0x1f6df}, {0x1f6eb, 0x1f6ec},
    {0x1f6f4, 0x1f6fc}, {0x1f7e0, 0x1f7eb}, {0x1f7f0, 0x1f7f0}, {0x1f90c, 0x1f93a}, {0x1f93c, 0x1f945},
    {0x1f947, 0x1f9ff}, {0x1fa70, 0x1faff}, {0x20000, 0x2fffd}, {0x30000, 0x3fffd},
};

static const struct pnl_range pnl_zero[] = {
    {0x0300, 0x036f}, {0x0483, 0x0489}, {0x0591, 0x05bd}, {0x05bf, 0x05bf}, {0x05c1, 0x05c2},
    {0x05c4, 0x05c5}, {0x05c7, 0x05c7}, {0x0610, 0x061a}, {0x064b, 0x065f}, {0x0670, 0x0670},
    {0x06d6, 0x06dc}, {0x06df, 0x06e4}, {0x06e7, 0x06e8}, {0x06ea, 0x06ed}, {0x0900, 0x0902},
    {0x093a, 0x093a}, {0x093c, 0x093c}, {0x0941, 0x0948}, {0x094d, 0x094d}, {0x0951, 0x0957},
    {0x0e31, 0x0e31}, {0x0e34, 0x0e3a}, {0x0e47, 0x0e4e}, {0x1ab0, 0x1aff}, {0x1dc0, 0x1dff},
    {0x200b, 0x200f}, {0x2028, 0x202e}, {0x2060, 0x2064}, {0x20d0, 0x20ff}, {0xfe00, 0xfe0f},
    {0xfe20, 0xfe2f}, {0xfeff, 0xfeff}, {0x1f3fb, 0x1f3ff}, {0xe0000, 0xe0fff},
};

static int pnl_in_ranges(uint32_t cp, const struct pnl_range *ranges, size_t ranges_len)
{
    size_t low = 0;
    size_t high = ranges_len;

    while (low < high) {
        size_t mid = low + (high - low) / 2;

        if (cp < ranges[mid].first)
            high = mid;
        else if (cp > ranges[mid].last)
            low = mid + 1;
        else
            return 1;
    }
    return 0;
}

int pnl_cp_width(uint32_t cp)
{
    if (cp < 0x20 || (cp >= 0x7f && cp < 0xa0))
        return -1;
    if (pnl_in_ranges(cp, pnl_zero, sizeof(pnl_zero) / sizeof(pnl_zero[0])))
        return 0;
    if (pnl_in_ranges(cp, pnl_wide, sizeof(pnl_wide) / sizeof(pnl_wide[0])))
        return 2;
    return 1;
}

size_t pnl_utf8_put(uint32_t cp, char *out)
{
    if (cp < 0x80) {
        out[0] = (char)cp;
        return 1;
    }
    if (cp < 0x800) {
        out[0] = (char)(0xc0 | (cp >> 6));
        out[1] = (char)(0x80 | (cp & 0x3f));
        return 2;
    }
    if (cp < 0x10000) {
        out[0] = (char)(0xe0 | (cp >> 12));
        out[1] = (char)(0x80 | ((cp >> 6) & 0x3f));
        out[2] = (char)(0x80 | (cp & 0x3f));
        return 3;
    }
    out[0] = (char)(0xf0 | (cp >> 18));
    out[1] = (char)(0x80 | ((cp >> 12) & 0x3f));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3f));
    out[3] = (char)(0x80 | (cp & 0x3f));
    return 4;
}

size_t pnl_utf8_next(const char *text, size_t text_len, uint32_t *out_cp)
{
    int key = PNL_KEY_NONE;
    unsigned char first = (unsigned char)text[0];
    size_t used;

    if (first < 0x80) {
        *out_cp = first;
        return 1;
    }
    used = pnl_key_decode((const unsigned char *)text, text_len, &key);
    *out_cp = key == PNL_KEY_NONE ? 0xfffd : (uint32_t)key;
    return used ? used : 1;
}

#include <stdlib.h>
#include <string.h>

static void *pnl_default_alloc(void *ctx, size_t size)
{
    (void)ctx;
    return malloc(size);
}

static void pnl_default_free(void *ctx, void *ptr)
{
    (void)ctx;
    free(ptr);
}

void *pnl_mem_alloc(const struct pnl_mem *mem, size_t size)
{
    return mem->alloc_fn(mem->ctx, size);
}

void pnl_mem_free(const struct pnl_mem *mem, void *ptr)
{
    if (ptr != NULL)
        mem->free_fn(mem->ctx, ptr);
}

char *pnl_mem_copy(const struct pnl_mem *mem, const char *text)
{
    size_t len = strlen(text) + 1;
    char *copy = pnl_mem_alloc(mem, len);

    if (copy != NULL)
        memcpy(copy, text, len);
    return copy;
}

int pnl_mem_grow(const struct pnl_mem *mem, void **items, size_t *cap, size_t needed, size_t item_size)
{
    size_t new_cap;
    void *grown;

    if (needed <= *cap)
        return PNL_OK;
    new_cap = *cap ? *cap : 8;
    while (new_cap < needed && new_cap <= (size_t)-1 / 2)
        new_cap *= 2;
    if (new_cap < needed || new_cap > (size_t)-1 / item_size)
        return PNL_ERR_NOMEM;
    grown = pnl_mem_alloc(mem, new_cap * item_size);
    if (grown == NULL)
        return PNL_ERR_NOMEM;
    if (*items != NULL) {
        memcpy(grown, *items, *cap * item_size);
        pnl_mem_free(mem, *items);
    }
    *items = grown;
    *cap = new_cap;
    return PNL_OK;
}

int pnl_env_off(const char *name)
{
    const char *value = getenv(name);

    return value != NULL && (strcmp(value, "off") == 0 || strcmp(value, "0") == 0);
}

int pnl_app_create(const char *title, const pnl_allocator_t *allocator, pnl_app_t **out_app)
{
    struct pnl_mem mem;
    pnl_app_t *app;

    if (out_app != NULL)
        *out_app = NULL;
    if (out_app == NULL || title == NULL ||
        (allocator != NULL && (allocator->alloc_fn == NULL || allocator->free_fn == NULL)))
        return PNL_ERR_ARG;
    mem.alloc_fn = allocator != NULL ? allocator->alloc_fn : pnl_default_alloc;
    mem.free_fn = allocator != NULL ? allocator->free_fn : pnl_default_free;
    mem.ctx = allocator != NULL ? allocator->ctx : NULL;
    app = pnl_mem_alloc(&mem, sizeof(*app));
    if (app == NULL)
        return PNL_ERR_NOMEM;
    memset(app, 0, sizeof(*app));
    app->mem = mem;
    app->next_timer = 1;
    pnl_theme_choose(&app->theme, "default");
    app->title = pnl_mem_copy(&mem, title);
    if (app->title == NULL) {
        pnl_mem_free(&mem, app);
        return PNL_ERR_NOMEM;
    }
    *out_app = app;
    return PNL_OK;
}

void pnl_app_destroy(pnl_app_t *app)
{
    size_t i;

    if (app == NULL)
        return;
    pnl_term_leave(&app->term);
    pnl_views_drop(app);
    pnl_screen_free(&app->screen, &app->mem);
    for (i = 0; i < app->actions_len; i++)
        pnl_mem_free(&app->mem, app->actions[i].label);
    pnl_menu_clear_lines(app);
    pnl_memory_free(app);
    pnl_mem_free(&app->mem, app->record_path);
    pnl_mem_free(&app->mem, app->banner);
    pnl_inspect_free(app);
    pnl_mem_free(&app->mem, app->actions);
    pnl_mem_free(&app->mem, app->lines);
    pnl_mem_free(&app->mem, app->seen);
    pnl_mem_free(&app->mem, app->seen_next);
    pnl_mem_free(&app->mem, app->views);
    pnl_mem_free(&app->mem, app->values);
    pnl_term_wake_close(&app->term);
    pnl_mem_free(&app->mem, app->timers);
    pnl_mem_free(&app->mem, app->watches);
    pnl_mem_free(&app->mem, app->title);
    pnl_mem_free(&app->mem, app);
}

int pnl_app_on_status(pnl_app_t *app, pnl_status_fn status_fn, void *status_ctx)
{
    if (app == NULL)
        return PNL_ERR_ARG;
    app->status_fn = status_fn;
    app->status_ctx = status_ctx;
    return PNL_OK;
}

int pnl_app_set_refresh(pnl_app_t *app, int refresh_ms)
{
    if (app == NULL || refresh_ms < 0)
        return PNL_ERR_ARG;
    if (app->status_timer != PNL_STATUS_TIMER_NONE)
        pnl_app_remove_timer(app, app->status_timer);
    app->status_timer = PNL_STATUS_TIMER_NONE;
    if (refresh_ms == 0)
        return PNL_OK;
    return pnl_app_add_timer(app, refresh_ms, NULL, NULL, &app->status_timer);
}

int pnl_app_add_action(pnl_app_t *app, const char *label, pnl_action_fn action_fn, void *action_ctx)
{
    struct pnl_action *action;
    int status;

    if (app == NULL || label == NULL || action_fn == NULL)
        return PNL_ERR_ARG;
    status = pnl_mem_grow(&app->mem, (void **)&app->actions, &app->actions_cap, app->actions_len + 1,
                          sizeof(*action));
    if (status != PNL_OK)
        return status;
    action = &app->actions[app->actions_len];
    action->label = pnl_mem_copy(&app->mem, label);
    if (action->label == NULL)
        return PNL_ERR_NOMEM;
    action->fn = action_fn;
    action->ctx = action_ctx;
    app->actions_len++;
    return PNL_OK;
}

int pnl_canvas_focused(const pnl_canvas_t *canvas)
{
    return canvas != NULL && canvas->focused;
}

int pnl_canvas_width(const pnl_canvas_t *canvas)
{
    return canvas ? canvas->width : 0;
}

int pnl_canvas_height(const pnl_canvas_t *canvas)
{
    return canvas ? canvas->height : 0;
}

int pnl_canvas_text(pnl_canvas_t *canvas, int x, int y, int style, const char *text)
{
    int end;

    if (canvas == NULL || text == NULL || x < 0 || y < 0 || x >= canvas->width || y >= canvas->height)
        return x;
    end = pnl_screen_text(canvas->screen, canvas->x + x, canvas->y + y, canvas->width - x, (uint32_t)style, text);
    return end - canvas->x;
}

int pnl_canvas_link(pnl_canvas_t *canvas, int x, int y, int style, const char *text, const char *url)
{
    int end;

    if (canvas == NULL || text == NULL || url == NULL || x < 0 || y < 0 || x >= canvas->width ||
        y >= canvas->height)
        return x;
    end = pnl_screen_link(canvas->screen, canvas->x + x, canvas->y + y, canvas->width - x, (uint32_t)style, text,
                          url[0] != '\0' ? url : NULL);
    return end - canvas->x;
}

int pnl_canvas_clip(pnl_canvas_t *canvas, int x, int y, int max, int style, const char *text)
{
    int end;

    if (canvas == NULL || text == NULL || x < 0 || y < 0 || x >= canvas->width || y >= canvas->height || max <= 0)
        return x;
    if (max > canvas->width - x)
        max = canvas->width - x;
    end = pnl_screen_text(canvas->screen, canvas->x + x, canvas->y + y, max, (uint32_t)style, text);
    return end - canvas->x;
}

void pnl_canvas_fill(pnl_canvas_t *canvas, int x, int y, int width, int height, int style)
{
    int row;

    if (canvas == NULL || x < 0 || y < 0)
        return;
    if (width > canvas->width - x)
        width = canvas->width - x;
    if (height > canvas->height - y)
        height = canvas->height - y;
    for (row = y; row < y + height; row++)
        pnl_screen_fill(canvas->screen, canvas->x + x, canvas->y + row, width, (uint32_t)style);
}

int pnl_text_width(const char *text)
{
    return text == NULL ? 0 : pnl_text_width_n(text, strlen(text));
}

int pnl_text_width_n(const char *text, size_t len)
{
    size_t pos = 0;
    int width = 0;

    while (pos < len) {
        uint32_t cp;
        int w;

        pos += pnl_utf8_next(text + pos, len - pos, &cp);
        w = pnl_cp_width(cp);
        width += w < 0 ? 1 : w;
    }
    return width;
}

const char *pnl_strerror(int status)
{
    switch (status) {
    case PNL_OK:
        return "ok";
    case PNL_ERR_ARG:
        return "invalid argument";
    case PNL_ERR_NOMEM:
        return "out of memory";
    case PNL_ERR_TTY:
        return "no interactive terminal";
    case PNL_ERR_IO:
        return "terminal read or write failed";
    case PNL_ERR_NOT_FOUND:
        return "not found";
    case PNL_ERR_UNSUPPORTED:
        return "not supported on this platform";
    case PNL_ERR_MISMATCH:
        return "screen differs from the snapshot";
    case PNL_ERR_LIMIT:
        return "fixed limit reached";
    default:
        return "unknown status";
    }
}
#endif
