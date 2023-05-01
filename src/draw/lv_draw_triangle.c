/**
 * @file lv_draw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../core/lv_obj.h"
#include "lv_draw_triangle.h"
#include "../misc/lv_math.h"
#include "../misc/lv_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_triangle_dsc_init(lv_draw_triangle_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_triangle_dsc_t));
    dsc->bg_color = lv_color_white();
    dsc->bg_grad.stops[0].color = lv_color_white();
    dsc->bg_grad.stops[1].color = lv_color_black();
    dsc->bg_grad.stops[1].frac = 0xFF;
    dsc->bg_grad.stops_count = 2;
}

void lv_draw_triangle(struct _lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc)
{
    lv_area_t a;
    a.x1 = LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    a.y1 = LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    a.x2 = LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    a.y2 = LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_draw_task_t * t = lv_draw_add_task(layer, &a);

    t->draw_dsc = lv_malloc(sizeof(*dsc));
    lv_memcpy(t->draw_dsc, dsc, sizeof(*dsc));
    t->type = LV_DRAW_TASK_TYPE_TRIANLGE;

    lv_draw_dsc_base_t * base_dsc = t->draw_dsc;
    base_dsc->layer = layer;

    if(base_dsc->obj && lv_obj_has_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS)) {
        /*Disable sending LV_EVENT_DRAW_TASK_ADDED first to avoid triggering recursive
         *event calls due draw task adds in the event*/
        lv_obj_clear_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
        lv_obj_send_event(dsc->base.obj, LV_EVENT_DRAW_TASK_ADDED, t);
        lv_obj_add_flag(base_dsc->obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    }

    lv_draw_dispatch();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
