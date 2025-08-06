#include "gui.h"
#include "os.h"
#include "log.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "etn_fs.h"
#include "user.h"
#include "ebd_fs.h"
#include "etn_fs.h"
#include "fs.h"

#if (GUI_TASK_MONITOR == 0)
#    include "ui.h"
#else
static lv_obj_t * ui_screen_main               = NULL;
static lv_obj_t * ui_main_container_boot       = NULL;
static lv_obj_t * ui_main_container_ebd        = NULL;
static lv_obj_t * ui_main_label_ebdinfo        = NULL;
static lv_obj_t * ui_main_label_ebdcnt         = NULL;
static lv_obj_t * ui_main_container_etn        = NULL;
static lv_obj_t * ui_main_label_etninfo        = NULL;
static lv_obj_t * ui_main_label_etncnt         = NULL;
static lv_obj_t * ui_main_container_time       = NULL;
static lv_obj_t * ui_main_label_timeh          = NULL;
static lv_obj_t * ui_main_label_timehm         = NULL;
static lv_obj_t * ui_main_label_timem          = NULL;
static lv_obj_t * ui_main_label_timems         = NULL;
static lv_obj_t * ui_main_label_times          = NULL;
static lv_obj_t * ui_main_chart_meminfo        = NULL;
static lv_obj_t * ui_main_chart_meminfo_Xaxis  = NULL;
static lv_obj_t * ui_main_chart_meminfo_Yaxis1 = NULL;
static lv_obj_t * ui_main_chart_meminfo_Yaxis2 = NULL;
static lv_obj_t * ui_main_container_item       = NULL;
static lv_obj_t * ui_main_label_min            = NULL;
static lv_obj_t * ui_main_label_now            = NULL;
static lv_obj_t * ui_main_label_draw           = NULL;
static lv_obj_t * ui_main_label_lcd            = NULL;
static lv_obj_t * ui_main_label_gui            = NULL;
static lv_obj_t * ui_main_label_tmr            = NULL;
static lv_obj_t * ui_main_label_idle           = NULL;

static void timer_cb(lv_timer_t * timer) {
    static uint8_t time_s = 0, time_m = 0, time_h = 0;

    time_s = (time_s + 1) % 60;
    if (time_s == 0) {
        time_m = (time_m + 1) % 60;
        if (time_m == 0) {
            time_h = (time_h + 1) % 60;
            // if (time_h == 0) {}
            lv_label_set_text_fmt(ui_main_label_timeh, "%u", time_h);
        }
        lv_label_set_text_fmt(ui_main_label_timem, "%u", time_m);
    }
    lv_label_set_text_fmt(ui_main_label_times, "%u", time_s);
}

static void ui_init(void) {
    /********** 初始化显示器与主题 **********/

    lv_disp_t * const  disp = lv_display_get_default();
    lv_theme_t * const theme =
        lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE),
                              lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    lv_display_set_theme(disp, theme);

    /********** 初始化图形界面布局 **********/

    ui_screen_main = lv_obj_create(NULL);
    lv_obj_remove_flag(ui_screen_main, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_text_font(ui_screen_main, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_boot = lv_obj_create(ui_screen_main);
    lv_obj_remove_style_all(ui_main_container_boot);
    lv_obj_set_width(ui_main_container_boot, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_container_boot, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_flex_flow(ui_main_container_boot, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_boot, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_boot,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_pad_left(ui_main_container_boot, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_boot, 5,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_boot, 3, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_boot, 3,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_main_container_boot, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_main_container_boot, 4,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_ebd = lv_obj_create(ui_main_container_boot);
    lv_obj_remove_style_all(ui_main_container_ebd);
    lv_obj_set_width(ui_main_container_ebd, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_container_ebd, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_flex_flow(ui_main_container_ebd, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_ebd, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_ebd,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                           LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_radius(ui_main_container_ebd, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_main_container_ebd, lv_color_hex(0x444444),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_main_container_ebd, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_container_ebd, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_ebd, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_ebd, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_ebd, 2,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_main_container_ebd, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_ebdinfo = lv_label_create(ui_main_container_ebd);
    lv_obj_set_width(ui_main_label_ebdinfo, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_ebdinfo, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text(ui_main_label_ebdinfo, "EBD:");
    lv_obj_remove_flag(ui_main_label_ebdinfo,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_label_ebdcnt = lv_label_create(ui_main_container_ebd);
    lv_obj_set_width(ui_main_label_ebdcnt, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_ebdcnt, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text_fmt(ui_main_label_ebdcnt, "%u", fs_get_boot_cnt(ebd_fs_hdl()));
    lv_obj_remove_flag(ui_main_label_ebdcnt,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_container_etn = lv_obj_create(ui_main_container_boot);
    lv_obj_remove_style_all(ui_main_container_etn);
    lv_obj_set_width(ui_main_container_etn, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_container_etn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_flex_flow(ui_main_container_etn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_etn, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_etn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                           LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_radius(ui_main_container_etn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_main_container_etn, lv_color_hex(0x444444),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_main_container_etn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_container_etn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_etn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_etn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_etn, 2,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_main_container_etn, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_etninfo = lv_label_create(ui_main_container_etn);
    lv_obj_set_width(ui_main_label_etninfo, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_etninfo, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text(ui_main_label_etninfo, "ETN:");
    lv_obj_remove_flag(ui_main_label_etninfo,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_label_etncnt = lv_label_create(ui_main_container_etn);
    lv_obj_set_width(ui_main_label_etncnt, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_etncnt, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text_fmt(ui_main_label_etncnt, "%u", fs_get_boot_cnt(etn_fs_hdl()));
    lv_obj_remove_flag(ui_main_label_etncnt,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_container_time = lv_obj_create(ui_screen_main);
    lv_obj_remove_style_all(ui_main_container_time);
    lv_obj_set_width(ui_main_container_time, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_container_time, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_container_time, LV_ALIGN_TOP_RIGHT);
    lv_obj_set_flex_flow(ui_main_container_time, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_time, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_time,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_pad_left(ui_main_container_time, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_time, 8,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_time, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_time, 5,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_main_container_time, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_timeh = lv_label_create(ui_main_container_time);
    lv_obj_set_width(ui_main_label_timeh, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_timeh, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text(ui_main_label_timeh, "0");
    lv_obj_remove_flag(ui_main_label_timeh,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_label_timehm = lv_label_create(ui_main_container_time);
    lv_obj_set_width(ui_main_label_timehm, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_timehm, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text(ui_main_label_timehm, ":");
    lv_obj_remove_flag(ui_main_label_timehm,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_label_timem = lv_label_create(ui_main_container_time);
    lv_obj_set_width(ui_main_label_timem, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_timem, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text(ui_main_label_timem, "0");
    lv_obj_remove_flag(ui_main_label_timem,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_label_timems = lv_label_create(ui_main_container_time);
    lv_obj_set_width(ui_main_label_timems, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_timems, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text(ui_main_label_timems, ":");
    lv_obj_remove_flag(ui_main_label_timems,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_label_times = lv_label_create(ui_main_container_time);
    lv_obj_set_width(ui_main_label_times, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_times, LV_SIZE_CONTENT);  /// 1
    lv_label_set_text(ui_main_label_times, "0");
    lv_obj_remove_flag(ui_main_label_times,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_chart_meminfo = lv_chart_create(ui_screen_main);
    lv_obj_set_width(ui_main_chart_meminfo, 200);
    lv_obj_set_height(ui_main_chart_meminfo, 170);
    lv_obj_set_x(ui_main_chart_meminfo, 8);
    lv_obj_set_y(ui_main_chart_meminfo, -30);
    lv_obj_set_align(ui_main_chart_meminfo, LV_ALIGN_BOTTOM_MID);
    lv_obj_remove_flag(ui_main_chart_meminfo,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_add_flag(ui_main_chart_meminfo,
                    LV_OBJ_FLAG_OVERFLOW_VISIBLE);  // make scales visible - Should it be
                                                    // forced to True?
    // lv_obj_remove_flag( ui_main_chart_meminfo, LV_OBJ_FLAG_SCROLLABLE );    //no
    // chart-zoom in LVGL9 - Shouldn't it be forced to False?
    lv_chart_set_type(ui_main_chart_meminfo, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(ui_main_chart_meminfo, 7);
    lv_chart_set_div_line_count(ui_main_chart_meminfo, 3, 10);

    ui_main_chart_meminfo_Xaxis = lv_scale_create(ui_main_chart_meminfo);
    lv_scale_set_mode(ui_main_chart_meminfo_Xaxis, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_set_size(ui_main_chart_meminfo_Xaxis, lv_pct(100), 12);
    lv_obj_set_align(ui_main_chart_meminfo_Xaxis, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_y(ui_main_chart_meminfo_Xaxis,
                 12 + lv_obj_get_style_pad_bottom(ui_main_chart_meminfo, LV_PART_MAIN) +
                     lv_obj_get_style_border_width(ui_main_chart_meminfo, LV_PART_MAIN));
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Xaxis, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Xaxis, 1,
                                LV_PART_ITEMS);  // LVGL-9.1 ticks are thicker by default
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Xaxis, 1, LV_PART_INDICATOR);
    lv_obj_set_style_length(ui_main_chart_meminfo_Xaxis, 0,
                            LV_PART_ITEMS);  // minor tick length
    lv_obj_set_style_length(ui_main_chart_meminfo_Xaxis, 5,
                            LV_PART_INDICATOR);  // major tick length
    lv_scale_set_range(ui_main_chart_meminfo_Xaxis, 0, 7 > 0 ? 7 - 1 : 0);
    lv_scale_set_total_tick_count(ui_main_chart_meminfo_Xaxis,
                                  (7 > 0 ? 7 - 1 : 0) * 0 + 1);
    lv_scale_set_major_tick_every(ui_main_chart_meminfo_Xaxis, 0 >= 1 ? 0 : 1);
    lv_scale_set_label_show(ui_main_chart_meminfo_Xaxis, false);
    ui_main_chart_meminfo_Yaxis1 = lv_scale_create(ui_main_chart_meminfo);
    lv_scale_set_mode(ui_main_chart_meminfo_Yaxis1, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_obj_set_size(ui_main_chart_meminfo_Yaxis1, 50, lv_pct(100));
    lv_obj_set_align(ui_main_chart_meminfo_Yaxis1, LV_ALIGN_LEFT_MID);
    lv_obj_set_x(ui_main_chart_meminfo_Yaxis1,
                 -50 - lv_obj_get_style_pad_left(ui_main_chart_meminfo, LV_PART_MAIN) -
                     lv_obj_get_style_border_width(ui_main_chart_meminfo, LV_PART_MAIN) +
                     2);
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Yaxis1, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Yaxis1, 1, LV_PART_ITEMS);
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Yaxis1, 1, LV_PART_INDICATOR);
    lv_obj_set_style_length(ui_main_chart_meminfo_Yaxis1, 5,
                            LV_PART_ITEMS);  // minor tick length
    lv_obj_set_style_length(ui_main_chart_meminfo_Yaxis1, 5,
                            LV_PART_INDICATOR);  // major tick length
    lv_scale_set_total_tick_count(ui_main_chart_meminfo_Yaxis1,
                                  (5 > 0 ? 5 - 1 : 0) * 1 + 1);
    lv_scale_set_major_tick_every(ui_main_chart_meminfo_Yaxis1, 1 >= 1 ? 1 : 1);
    ui_main_chart_meminfo_Yaxis2 = lv_scale_create(ui_main_chart_meminfo);
    lv_scale_set_mode(ui_main_chart_meminfo_Yaxis2, LV_SCALE_MODE_VERTICAL_RIGHT);
    lv_obj_set_size(ui_main_chart_meminfo_Yaxis2, 25, lv_pct(100));
    lv_obj_set_align(ui_main_chart_meminfo_Yaxis2, LV_ALIGN_RIGHT_MID);
    lv_obj_set_x(ui_main_chart_meminfo_Yaxis2,
                 25 + lv_obj_get_style_pad_right(ui_main_chart_meminfo, LV_PART_MAIN) +
                     lv_obj_get_style_border_width(ui_main_chart_meminfo, LV_PART_MAIN) +
                     1);
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Yaxis2, 0, LV_PART_MAIN);
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Yaxis2, 1, LV_PART_ITEMS);
    lv_obj_set_style_line_width(ui_main_chart_meminfo_Yaxis2, 1, LV_PART_INDICATOR);
    lv_obj_set_style_length(ui_main_chart_meminfo_Yaxis2, 5,
                            LV_PART_ITEMS);  // minor tick length
    lv_obj_set_style_length(ui_main_chart_meminfo_Yaxis2, 10,
                            LV_PART_INDICATOR);  // major tick length
    lv_scale_set_total_tick_count(ui_main_chart_meminfo_Yaxis2,
                                  (5 > 0 ? 5 - 1 : 0) * 0 + 1);
    lv_scale_set_major_tick_every(ui_main_chart_meminfo_Yaxis2, 0 >= 1 ? 0 : 1);
    lv_scale_set_label_show(ui_main_chart_meminfo_Yaxis2, false);
    lv_chart_series_t * ui_main_chart_meminfo_series_1 = lv_chart_add_series(
        ui_main_chart_meminfo, lv_color_hex(0x808080), LV_CHART_AXIS_PRIMARY_Y);
    static lv_coord_t ui_main_chart_meminfo_series_1_array[] = {70, 60, 80, 90,
                                                                70, 80, 70};
    lv_chart_set_ext_y_array(ui_main_chart_meminfo, ui_main_chart_meminfo_series_1,
                             ui_main_chart_meminfo_series_1_array);

    lv_obj_set_style_line_color(ui_main_chart_meminfo_Xaxis, lv_color_hex(0x808080),
                                LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_main_chart_meminfo_Xaxis, 255,
                              LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_main_chart_meminfo_Yaxis1, lv_color_hex(0x808080),
                                LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_main_chart_meminfo_Yaxis1, 255,
                              LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_main_chart_meminfo_Yaxis2, lv_color_hex(0x808080),
                                LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_main_chart_meminfo_Yaxis2, 255,
                              LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_main_chart_meminfo_Xaxis, lv_color_hex(0x808080),
                                LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_main_chart_meminfo_Xaxis, 255,
                              LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_main_chart_meminfo_Yaxis1, lv_color_hex(0x808080),
                                LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_main_chart_meminfo_Yaxis1, 255,
                              LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui_main_chart_meminfo_Yaxis2, lv_color_hex(0x808080),
                                LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui_main_chart_meminfo_Yaxis2, 255,
                              LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_main_chart_meminfo_Xaxis, lv_color_hex(0x808080),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_main_chart_meminfo_Xaxis, 255,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_main_chart_meminfo_Yaxis1, lv_color_hex(0x808080),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_main_chart_meminfo_Yaxis1, 255,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_main_chart_meminfo_Yaxis2, lv_color_hex(0x808080),
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui_main_chart_meminfo_Yaxis2, 255,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_main_chart_meminfo_Xaxis, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_main_chart_meminfo_Yaxis1, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_main_chart_meminfo_Yaxis2, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    // This workaround (an invisible outline) is needed because without it chart
    // overflow-visible doesn't work in LVGL-9.1
    lv_obj_set_style_outline_pad(
        ui_main_chart_meminfo, LV_MAX3(12, 50, 25),
        LV_PART_MAIN |
            LV_STATE_DEFAULT);  // workaround for ineffective 'overflow visible' flag
    lv_obj_set_style_outline_width(ui_main_chart_meminfo, -1,
                                   LV_PART_MAIN | LV_STATE_DEFAULT);
    ui_main_container_item = lv_obj_create(ui_screen_main);
    lv_obj_remove_style_all(ui_main_container_item);
    lv_obj_set_height(ui_main_container_item, 26);
    lv_obj_set_width(ui_main_container_item, lv_pct(100));
    lv_obj_set_align(ui_main_container_item, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_flex_flow(ui_main_container_item, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_item, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_item,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_pad_left(ui_main_container_item, 30,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_item, 0,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_item, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_main_container_item, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_main_container_item, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_min = lv_label_create(ui_main_container_item);
    lv_obj_set_width(ui_main_label_min, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_min, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_min, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_min, "min");
    lv_obj_remove_flag(ui_main_label_min,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_min, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_label_min, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_label_min, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_label_min, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_label_min, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_now = lv_label_create(ui_main_container_item);
    lv_obj_set_width(ui_main_label_now, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_now, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_now, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_now, "now");
    lv_obj_remove_flag(ui_main_label_now,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_now, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_label_now, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_label_now, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_label_now, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_label_now, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_draw = lv_label_create(ui_main_container_item);
    lv_obj_set_width(ui_main_label_draw, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_draw, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_draw, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_draw, "draw");
    lv_obj_remove_flag(ui_main_label_draw,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_draw, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_label_draw, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_label_draw, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_label_draw, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_label_draw, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_lcd = lv_label_create(ui_main_container_item);
    lv_obj_set_width(ui_main_label_lcd, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_lcd, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_lcd, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_lcd, "lcd");
    lv_obj_remove_flag(ui_main_label_lcd,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_lcd, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_label_lcd, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_label_lcd, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_label_lcd, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_label_lcd, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_gui = lv_label_create(ui_main_container_item);
    lv_obj_set_width(ui_main_label_gui, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_gui, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_gui, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_gui, "gui");
    lv_obj_remove_flag(ui_main_label_gui,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_gui, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_label_gui, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_label_gui, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_label_gui, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_label_gui, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_tmr = lv_label_create(ui_main_container_item);
    lv_obj_set_width(ui_main_label_tmr, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_tmr, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_tmr, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_tmr, "tmr");
    lv_obj_remove_flag(ui_main_label_tmr,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_tmr, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_label_tmr, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_label_tmr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_label_tmr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_label_tmr, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_idle = lv_label_create(ui_main_container_item);
    lv_obj_set_width(ui_main_label_idle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_idle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_idle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_idle, "idle");
    lv_obj_remove_flag(ui_main_label_idle,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_idle, &lv_font_montserrat_10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_label_idle, 7, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_label_idle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_label_idle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_label_idle, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_timer_create(timer_cb, 1000, NULL);

    lv_screen_load(ui_screen_main);
}
#endif

BaseType_t gui_init(void) {
    lv_init();
    lv_tick_set_cb(xTaskGetTickCount);
#if ADD_ETN_FS
    lv_littlefs_set_handler(etn_fs_hdl());
#endif
    lv_port_disp_init();
    ui_init();
    OS_PRTF(NEWS_LOG, "init lvgl done!");

    return pdPASS;
}

__attribute__((noreturn)) void gui_task(void * task_arg) {
    while (1) {
        uint32_t slp_tm = lv_timer_handler();
        vTaskDelay((slp_tm > GUI_MIN_SLP_MS) ? slp_tm : GUI_MIN_SLP_MS);
    }
}
