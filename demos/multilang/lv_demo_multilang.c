/**
 * @file lv_demo_multilang.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_multilang.h"

#if LV_USE_DEMO_MULTILANG

/*********************
 *      DEFINES
 *********************/
#define CARD_HEIGHT     180

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void card_create(lv_obj_t * parent, const void * avatart_src, const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_style_t style_card_cont;
static lv_style_t style_placeholder;
static lv_style_t style_card;
static lv_style_t style_remove;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_multilang(void)
{
    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(lv_scr_act(), 0, 0);

    static const lv_coord_t grid_cols[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static const lv_coord_t grid_rows[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_style_init(&style_card_cont);
    lv_style_set_width(&style_card_cont, lv_pct(100));
    lv_style_set_height(&style_card_cont, CARD_HEIGHT);
    lv_style_set_layout(&style_card_cont, LV_LAYOUT_FLEX);
    lv_style_set_flex_flow(&style_card_cont, LV_FLEX_FLOW_ROW);
    lv_style_set_bg_opa(&style_card_cont, 0);
    lv_style_set_border_opa(&style_card_cont, 0);
    lv_style_set_pad_top(&style_card_cont, 5);
    lv_style_set_pad_bottom(&style_card_cont, 10);

    lv_style_init(&style_card);
    lv_style_set_width(&style_card, lv_pct(100));
    lv_style_set_height(&style_card, lv_pct(100));
    lv_style_set_layout(&style_card, LV_LAYOUT_GRID);
    lv_style_set_grid_column_dsc_array(&style_card, grid_cols);
    lv_style_set_grid_row_dsc_array(&style_card, grid_rows);
    lv_style_set_shadow_width(&style_card, 20);
    lv_style_set_shadow_ofs_y(&style_card, 5);

    lv_style_init(&style_remove);
    lv_style_set_width(&style_remove, lv_pct(100));
    lv_style_set_height(&style_remove, lv_pct(100));
    lv_style_set_bg_color(&style_remove, lv_color_hex(0x666666));

    lv_style_init(&style_placeholder);
    lv_style_set_width(&style_placeholder, lv_pct(100));
    lv_style_set_height(&style_placeholder, lv_pct(100));
    lv_style_set_bg_opa(&style_placeholder, 0);
    lv_style_set_border_opa(&style_placeholder, 0);


    uint32_t i;
    for(i = 0; i < 30; i++) {
        LV_IMG_DECLARE(img_demo_widgets_avatar);
        card_create(lv_scr_act(), &img_demo_widgets_avatar,
                    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi rutrum fringilla nibh, vitae faucibus ante tincidunt ut. Curabitur a justo efficitur, dictum turpis in, varius lorem. Maecenas ut massa ipsum. Proin molestie mi eget felis malesuada, vel laoreet orci mattis. Maecenas et ligula at nibh elementum finibus. Cras faucibus dolor vel dignissim malesuada. Morbi quis libero ac turpis vulputate vehicula quis semper sapien. Aliquam a tristique quam.");
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void shrink_anim_cb(void * var, int32_t v)
{
    lv_obj_t * cont = var;
    lv_obj_set_height(cont, (v * CARD_HEIGHT) >> 8);
    lv_obj_set_style_opa(cont, v, 0);
}

static void scroll_event_cb(lv_event_t * e)
{
    lv_indev_t * indev = lv_indev_get_act();
    lv_obj_t * cont = lv_event_get_target(e);
    if(lv_indev_get_scroll_obj(indev) != cont) return;

    lv_coord_t w = lv_obj_get_width(cont);
    lv_coord_t scroll_x = lv_obj_get_scroll_x(cont) - lv_indev_scroll_throw_predict(indev, LV_DIR_HOR);

    if(scroll_x < w / 2) {
        lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_NONE);
        lv_obj_clear_flag(cont, LV_OBJ_FLAG_CLICKABLE);
        lv_indev_wait_release(indev);
        lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_ON);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_exec_cb(&a, shrink_anim_cb);
        lv_anim_set_ready_cb(&a, lv_obj_del_anim_ready_cb);
        lv_anim_set_values(&a, 255, 0);
        lv_anim_set_time(&a, 400);
        lv_anim_set_var(&a, cont);
        lv_anim_start(&a);
    }
}

static void card_create(lv_obj_t * parent, const void * avatart_src, const char * txt)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_add_style(cont, &style_card_cont, 0);
    lv_obj_set_scroll_snap_x(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_add_event(cont, scroll_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);
    //    lv_obj_set_style_opa(cont, 200, 0);

    lv_obj_t * remove = lv_obj_create(cont);
    lv_obj_add_style(remove, &style_remove, 0);
    lv_obj_clear_flag(remove, LV_OBJ_FLAG_SNAPPABLE);
    lv_obj_add_flag(remove, LV_OBJ_FLAG_FLOATING);
    lv_obj_clear_flag(remove, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * placeholder = lv_obj_create(cont);
    lv_obj_add_style(placeholder, &style_placeholder, 0);

    lv_obj_t * card = lv_obj_create(cont);
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN_HOR);

    lv_obj_t * avatar = lv_img_create(card);
    lv_img_set_src(avatar, avatart_src);
    lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 3);

    lv_obj_t * name = lv_label_create(card);
    lv_label_set_text(name, "Toth Mariska");
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t * description = lv_label_create(card);
    lv_label_set_text(description, txt);
    lv_obj_set_flex_grow(description, 1);
    lv_label_set_long_mode(description, LV_LABEL_LONG_DOT);
    lv_obj_set_grid_cell(description, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

    lv_obj_t * btn = lv_btn_create(card);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Add Friend");

    lv_obj_scroll_to_view(card, LV_ANIM_OFF);
}


#endif
