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
lv_obj_t * ui_screen_main            = NULL;
lv_obj_t * ui_main_container_boot    = NULL;
lv_obj_t * ui_main_container_ebd     = NULL;
lv_obj_t * ui_main_label_ebdinfo     = NULL;
lv_obj_t * ui_main_label_ebdcnt      = NULL;
lv_obj_t * ui_main_container_etn     = NULL;
lv_obj_t * ui_main_label_etninfo     = NULL;
lv_obj_t * ui_main_label_etncnt      = NULL;
lv_obj_t * ui_main_container_time    = NULL;
lv_obj_t * ui_main_label_timeh       = NULL;
lv_obj_t * ui_main_label_timehm      = NULL;
lv_obj_t * ui_main_label_timem       = NULL;
lv_obj_t * ui_main_label_timems      = NULL;
lv_obj_t * ui_main_label_times       = NULL;
lv_obj_t * ui_main_container_mainctn = NULL;
lv_obj_t * ui_main_panel_heappan     = NULL;
lv_obj_t * ui_main_label_heapval     = NULL;
lv_obj_t * ui_main_bar_heapbar       = NULL;
lv_obj_t * ui_main_label_heaptitle   = NULL;
lv_obj_t * ui_main_container_taskctn = NULL;
lv_obj_t * ui_main_panel_drawpan     = NULL;
lv_obj_t * ui_main_container_drawctn = NULL;
lv_obj_t * ui_main_label_drawtitle   = NULL;
lv_obj_t * ui_main_label_drawval     = NULL;
lv_obj_t * ui_main_bar_drawbar       = NULL;
lv_obj_t * ui_main_panel_lcdpan      = NULL;
lv_obj_t * ui_main_container_lcdctn  = NULL;
lv_obj_t * ui_main_label_lcdtitle    = NULL;
lv_obj_t * ui_main_label_lcdval      = NULL;
lv_obj_t * ui_main_bar_lcdbar        = NULL;
lv_obj_t * ui_main_panel_guipan      = NULL;
lv_obj_t * ui_main_container_guictn  = NULL;
lv_obj_t * ui_main_label_guititle    = NULL;
lv_obj_t * ui_main_label_guival      = NULL;
lv_obj_t * ui_main_bar_guibar        = NULL;
lv_obj_t * ui_main_panel_tmrpan      = NULL;
lv_obj_t * ui_main_container_tmrctn  = NULL;
lv_obj_t * ui_main_label_tmrtitle    = NULL;
lv_obj_t * ui_main_label_tmrval      = NULL;
lv_obj_t * ui_main_bar_tmrbar        = NULL;
lv_obj_t * ui_main_panel_idlepan     = NULL;
lv_obj_t * ui_main_container_idlectn = NULL;
lv_obj_t * ui_main_label_idletitle   = NULL;
lv_obj_t * ui_main_label_idleval     = NULL;
lv_obj_t * ui_main_bar_idlebar       = NULL;

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

    if (time_s % 3 == 0) {
        uint8_t val = (1 - (float)xPortGetFreeHeapSize() / configTOTAL_HEAP_SIZE) * 100;
        lv_label_set_text_fmt(ui_main_label_heapval, "%u %%", val);
        lv_bar_set_value(ui_main_bar_heapbar, val, LV_ANIM_ON);
    }
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
    lv_obj_set_style_pad_left(ui_main_container_boot, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_boot, 8,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_boot, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_boot, 6,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_main_container_boot, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_main_container_boot, 8,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_main_container_boot, &lv_font_montserrat_12,
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
    lv_obj_set_style_bg_color(ui_main_container_ebd, lv_color_hex(0x444444),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_main_container_ebd, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_container_ebd, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_ebd, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_ebd, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_ebd, 2,
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
    lv_obj_set_style_bg_color(ui_main_container_etn, lv_color_hex(0x444444),
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_main_container_etn, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_container_etn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_etn, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_etn, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_etn, 2,
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
    lv_obj_set_style_pad_left(ui_main_container_time, 14,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_time, 14,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_time, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_time, 8,
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

    ui_main_container_mainctn = lv_obj_create(ui_screen_main);
    lv_obj_remove_style_all(ui_main_container_mainctn);
    lv_obj_set_height(ui_main_container_mainctn, 214);
    lv_obj_set_width(ui_main_container_mainctn, lv_pct(100));
    lv_obj_set_align(ui_main_container_mainctn, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_flex_flow(ui_main_container_mainctn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_mainctn, LV_FLEX_ALIGN_SPACE_EVENLY,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_mainctn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                           LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_pad_left(ui_main_container_mainctn, 0,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_mainctn, 0,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_mainctn, 8,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_mainctn, 8,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_panel_heappan = lv_obj_create(ui_main_container_mainctn);
    lv_obj_set_width(ui_main_panel_heappan, lv_pct(20));
    lv_obj_set_height(ui_main_panel_heappan, lv_pct(100));
    lv_obj_set_align(ui_main_panel_heappan, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_panel_heappan, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_main_panel_heappan, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_panel_heappan,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                           LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_radius(ui_main_panel_heappan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_panel_heappan, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_panel_heappan, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_panel_heappan, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_panel_heappan, 4,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_main_panel_heappan, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_main_panel_heappan, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_heapval = lv_label_create(ui_main_panel_heappan);
    lv_obj_set_width(ui_main_label_heapval, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_heapval, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_heapval, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_heapval, "0 %");
    lv_obj_remove_flag(ui_main_label_heapval,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_bar_heapbar = lv_bar_create(ui_main_panel_heappan);
    lv_bar_set_value(ui_main_bar_heapbar, 25, LV_ANIM_OFF);
    lv_bar_set_start_value(ui_main_bar_heapbar, 0, LV_ANIM_OFF);
    lv_bar_set_orientation(ui_main_bar_heapbar, LV_BAR_ORIENTATION_VERTICAL);
    lv_obj_set_height(ui_main_bar_heapbar, lv_pct(80));
    lv_obj_set_width(ui_main_bar_heapbar, 12);
    lv_obj_set_align(ui_main_bar_heapbar, LV_ALIGN_CENTER);
    lv_obj_remove_flag(ui_main_bar_heapbar,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM | LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_set_style_radius(ui_main_bar_heapbar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_main_bar_heapbar, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_main_label_heaptitle = lv_label_create(ui_main_panel_heappan);
    lv_obj_set_width(ui_main_label_heaptitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_heaptitle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_heaptitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_heaptitle, "heap");
    lv_obj_remove_flag(ui_main_label_heaptitle,
                       LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_CLICK_FOCUSABLE |
                           LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_SNAPPABLE |
                           LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_ELASTIC |
                           LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_text_font(ui_main_label_heaptitle, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_taskctn = lv_obj_create(ui_main_container_mainctn);
    lv_obj_remove_style_all(ui_main_container_taskctn);
    lv_obj_set_width(ui_main_container_taskctn, lv_pct(68));
    lv_obj_set_height(ui_main_container_taskctn, lv_pct(100));
    lv_obj_set_x(ui_main_container_taskctn, 0);
    lv_obj_set_y(ui_main_container_taskctn, 36);
    lv_obj_set_align(ui_main_container_taskctn, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(ui_main_container_taskctn, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_main_container_taskctn, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_taskctn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                           LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_pad_left(ui_main_container_taskctn, 0,
                              LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_container_taskctn, 0,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_container_taskctn, 0,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_container_taskctn, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(ui_main_container_taskctn, 4,
                             LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(ui_main_container_taskctn, 0,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_panel_drawpan = lv_obj_create(ui_main_container_taskctn);
    lv_obj_set_height(ui_main_panel_drawpan, 34);
    lv_obj_set_width(ui_main_panel_drawpan, lv_pct(100));
    lv_obj_set_align(ui_main_panel_drawpan, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_panel_drawpan, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_main_panel_drawpan, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_panel_drawpan,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                           LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags
    lv_obj_set_style_radius(ui_main_panel_drawpan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_panel_drawpan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_panel_drawpan, 10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_panel_drawpan, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_panel_drawpan, 3,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_drawctn = lv_obj_create(ui_main_panel_drawpan);
    lv_obj_remove_style_all(ui_main_container_drawctn);
    lv_obj_set_width(ui_main_container_drawctn, lv_pct(100));
    lv_obj_set_height(ui_main_container_drawctn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_container_drawctn, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_container_drawctn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_drawctn, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_drawctn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK |
                           LV_OBJ_FLAG_CLICK_FOCUSABLE | LV_OBJ_FLAG_GESTURE_BUBBLE |
                           LV_OBJ_FLAG_SNAPPABLE | LV_OBJ_FLAG_SCROLLABLE |
                           LV_OBJ_FLAG_SCROLL_ELASTIC | LV_OBJ_FLAG_SCROLL_MOMENTUM |
                           LV_OBJ_FLAG_SCROLL_CHAIN);  /// Flags

    ui_main_label_drawtitle = lv_label_create(ui_main_container_drawctn);
    lv_obj_set_width(ui_main_label_drawtitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_drawtitle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_drawtitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_drawtitle, "draw");
    lv_obj_set_style_text_font(ui_main_label_drawtitle, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_drawval = lv_label_create(ui_main_container_drawctn);
    lv_obj_set_width(ui_main_label_drawval, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_drawval, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_drawval, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_drawval, "0 %");

    ui_main_bar_drawbar = lv_bar_create(ui_main_panel_drawpan);
    lv_bar_set_value(ui_main_bar_drawbar, 25, LV_ANIM_OFF);
    lv_bar_set_start_value(ui_main_bar_drawbar, 0, LV_ANIM_OFF);
    lv_obj_set_height(ui_main_bar_drawbar, 8);
    lv_obj_set_width(ui_main_bar_drawbar, lv_pct(100));
    lv_obj_set_align(ui_main_bar_drawbar, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_main_bar_drawbar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_main_bar_drawbar, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_main_panel_lcdpan = lv_obj_create(ui_main_container_taskctn);
    lv_obj_set_height(ui_main_panel_lcdpan, 34);
    lv_obj_set_width(ui_main_panel_lcdpan, lv_pct(100));
    lv_obj_set_align(ui_main_panel_lcdpan, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_panel_lcdpan, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_main_panel_lcdpan, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_panel_lcdpan, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_radius(ui_main_panel_lcdpan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_panel_lcdpan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_panel_lcdpan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_panel_lcdpan, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_panel_lcdpan, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_lcdctn = lv_obj_create(ui_main_panel_lcdpan);
    lv_obj_remove_style_all(ui_main_container_lcdctn);
    lv_obj_set_width(ui_main_container_lcdctn, lv_pct(100));
    lv_obj_set_height(ui_main_container_lcdctn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_container_lcdctn, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_container_lcdctn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_lcdctn, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_lcdctn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_main_label_lcdtitle = lv_label_create(ui_main_container_lcdctn);
    lv_obj_set_width(ui_main_label_lcdtitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_lcdtitle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_lcdtitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_lcdtitle, "lcd");
    lv_obj_set_style_text_font(ui_main_label_lcdtitle, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_lcdval = lv_label_create(ui_main_container_lcdctn);
    lv_obj_set_width(ui_main_label_lcdval, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_lcdval, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_lcdval, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_lcdval, "0 %");

    ui_main_bar_lcdbar = lv_bar_create(ui_main_panel_lcdpan);
    lv_bar_set_value(ui_main_bar_lcdbar, 25, LV_ANIM_OFF);
    lv_bar_set_start_value(ui_main_bar_lcdbar, 0, LV_ANIM_OFF);
    lv_obj_set_height(ui_main_bar_lcdbar, 8);
    lv_obj_set_width(ui_main_bar_lcdbar, lv_pct(100));
    lv_obj_set_align(ui_main_bar_lcdbar, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_main_bar_lcdbar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_main_bar_lcdbar, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_main_panel_guipan = lv_obj_create(ui_main_container_taskctn);
    lv_obj_set_height(ui_main_panel_guipan, 34);
    lv_obj_set_width(ui_main_panel_guipan, lv_pct(100));
    lv_obj_set_align(ui_main_panel_guipan, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_panel_guipan, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_main_panel_guipan, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_panel_guipan, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_radius(ui_main_panel_guipan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_panel_guipan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_panel_guipan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_panel_guipan, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_panel_guipan, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_guictn = lv_obj_create(ui_main_panel_guipan);
    lv_obj_remove_style_all(ui_main_container_guictn);
    lv_obj_set_width(ui_main_container_guictn, lv_pct(100));
    lv_obj_set_height(ui_main_container_guictn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_container_guictn, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_container_guictn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_guictn, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_guictn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_main_label_guititle = lv_label_create(ui_main_container_guictn);
    lv_obj_set_width(ui_main_label_guititle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_guititle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_guititle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_guititle, "gui");
    lv_obj_set_style_text_font(ui_main_label_guititle, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_guival = lv_label_create(ui_main_container_guictn);
    lv_obj_set_width(ui_main_label_guival, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_guival, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_guival, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_guival, "0 %");

    ui_main_bar_guibar = lv_bar_create(ui_main_panel_guipan);
    lv_bar_set_value(ui_main_bar_guibar, 25, LV_ANIM_OFF);
    lv_bar_set_start_value(ui_main_bar_guibar, 0, LV_ANIM_OFF);
    lv_obj_set_height(ui_main_bar_guibar, 8);
    lv_obj_set_width(ui_main_bar_guibar, lv_pct(100));
    lv_obj_set_align(ui_main_bar_guibar, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_main_bar_guibar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_main_bar_guibar, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_main_panel_tmrpan = lv_obj_create(ui_main_container_taskctn);
    lv_obj_set_height(ui_main_panel_tmrpan, 34);
    lv_obj_set_width(ui_main_panel_tmrpan, lv_pct(100));
    lv_obj_set_align(ui_main_panel_tmrpan, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_panel_tmrpan, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_main_panel_tmrpan, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_panel_tmrpan, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_radius(ui_main_panel_tmrpan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_panel_tmrpan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_panel_tmrpan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_panel_tmrpan, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_panel_tmrpan, 3, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_tmrctn = lv_obj_create(ui_main_panel_tmrpan);
    lv_obj_remove_style_all(ui_main_container_tmrctn);
    lv_obj_set_width(ui_main_container_tmrctn, lv_pct(100));
    lv_obj_set_height(ui_main_container_tmrctn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_container_tmrctn, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_container_tmrctn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_tmrctn, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_tmrctn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_main_label_tmrtitle = lv_label_create(ui_main_container_tmrctn);
    lv_obj_set_width(ui_main_label_tmrtitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_tmrtitle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_tmrtitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_tmrtitle, "tmr");
    lv_obj_set_style_text_font(ui_main_label_tmrtitle, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_tmrval = lv_label_create(ui_main_container_tmrctn);
    lv_obj_set_width(ui_main_label_tmrval, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_tmrval, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_tmrval, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_tmrval, "0 %");

    ui_main_bar_tmrbar = lv_bar_create(ui_main_panel_tmrpan);
    lv_bar_set_value(ui_main_bar_tmrbar, 25, LV_ANIM_OFF);
    lv_bar_set_start_value(ui_main_bar_tmrbar, 0, LV_ANIM_OFF);
    lv_obj_set_height(ui_main_bar_tmrbar, 8);
    lv_obj_set_width(ui_main_bar_tmrbar, lv_pct(100));
    lv_obj_set_align(ui_main_bar_tmrbar, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_main_bar_tmrbar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_main_bar_tmrbar, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_main_panel_idlepan = lv_obj_create(ui_main_container_taskctn);
    lv_obj_set_height(ui_main_panel_idlepan, 34);
    lv_obj_set_width(ui_main_panel_idlepan, lv_pct(100));
    lv_obj_set_align(ui_main_panel_idlepan, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_panel_idlepan, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(ui_main_panel_idlepan, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_panel_idlepan, LV_OBJ_FLAG_SCROLLABLE);  /// Flags
    lv_obj_set_style_radius(ui_main_panel_idlepan, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui_main_panel_idlepan, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui_main_panel_idlepan, 10,
                               LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui_main_panel_idlepan, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui_main_panel_idlepan, 3,
                                LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_container_idlectn = lv_obj_create(ui_main_panel_idlepan);
    lv_obj_remove_style_all(ui_main_container_idlectn);
    lv_obj_set_width(ui_main_container_idlectn, lv_pct(100));
    lv_obj_set_height(ui_main_container_idlectn, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_container_idlectn, LV_ALIGN_CENTER);
    lv_obj_set_flex_flow(ui_main_container_idlectn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(ui_main_container_idlectn, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(ui_main_container_idlectn,
                       LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);  /// Flags

    ui_main_label_idletitle = lv_label_create(ui_main_container_idlectn);
    lv_obj_set_width(ui_main_label_idletitle, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_idletitle, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_idletitle, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_idletitle, "idle");
    lv_obj_set_style_text_font(ui_main_label_idletitle, &lv_font_montserrat_12,
                               LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_main_label_idleval = lv_label_create(ui_main_container_idlectn);
    lv_obj_set_width(ui_main_label_idleval, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_main_label_idleval, LV_SIZE_CONTENT);  /// 1
    lv_obj_set_align(ui_main_label_idleval, LV_ALIGN_CENTER);
    lv_label_set_text(ui_main_label_idleval, "0 %");

    ui_main_bar_idlebar = lv_bar_create(ui_main_panel_idlepan);
    lv_bar_set_value(ui_main_bar_idlebar, 25, LV_ANIM_OFF);
    lv_bar_set_start_value(ui_main_bar_idlebar, 0, LV_ANIM_OFF);
    lv_obj_set_height(ui_main_bar_idlebar, 8);
    lv_obj_set_width(ui_main_bar_idlebar, lv_pct(100));
    lv_obj_set_align(ui_main_bar_idlebar, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_main_bar_idlebar, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui_main_bar_idlebar, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);

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
