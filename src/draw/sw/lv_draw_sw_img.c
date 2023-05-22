/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../lv_img_cache.h"
#include "../../core/lv_disp.h"
#include "../../core/lv_disp_private.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_mem.h"
#include "../../misc/lv_math.h"
#include LV_COLOR_EXTERN_INCLUDE

/*********************
 *      DEFINES
 *********************/
#define MAX_BUF_SIZE (uint32_t) lv_disp_get_hor_res(_lv_refr_get_disp_refreshing())

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


void lv_draw_sw_layer(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * draw_dsc, const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if(layer_to_draw->buf == NULL) return;

    lv_img_dsc_t img_dsc;
    img_dsc.header.w = lv_area_get_width(&layer_to_draw->buf_area);
    img_dsc.header.h = lv_area_get_height(&layer_to_draw->buf_area);
    img_dsc.header.cf = layer_to_draw->color_format;
    img_dsc.header.always_zero = 0;
    img_dsc.header.chroma_keyed = 0;
    img_dsc.data = layer_to_draw->buf;

    lv_draw_img_dsc_t new_draw_dsc;
    lv_memcpy(&new_draw_dsc, draw_dsc, sizeof(lv_draw_img_dsc_t));
    new_draw_dsc.src = &img_dsc;

    lv_draw_sw_img(draw_unit, &new_draw_dsc, coords);

#if LV_USE_LAYER_DEBUG || LV_USE_PARALLEL_DRAW_DEBUG
    lv_area_t area_rot;
    lv_area_copy(&area_rot, coords);
    if(draw_dsc->angle || draw_dsc->zoom != LV_ZOOM_NONE) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_img_buf_get_transformed_area(&area_rot, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

        area_rot.x1 += coords->x1;
        area_rot.y1 += coords->y1;
        area_rot.x2 += coords->x1;
        area_rot.y2 += coords->y1;
    }
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &area_rot, draw_unit->clip_area)) return;
#endif

#if LV_USE_LAYER_DEBUG
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(layer_to_draw->color_format == LV_COLOR_FORMAT_ARGB8888 ? 0xff0000 : 0x00ff00);
    rect_dsc.border_color = rect_dsc.bg_color;
    rect_dsc.bg_opa = LV_OPA_20;
    rect_dsc.border_opa = LV_OPA_60;
    rect_dsc.border_width = 2;
    lv_draw_sw_rect(draw_unit, &rect_dsc, &area_rot);
#endif

#if LV_USE_PARALLEL_DRAW_DEBUG
    uint32_t idx = 0;
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    lv_draw_unit_t * draw_unit_tmp = disp->draw_unit_head;
    while(draw_unit_tmp != draw_unit) {
        draw_unit_tmp = draw_unit_tmp->next;
        idx++;
    }

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_palette_main(idx % _LV_PALETTE_LAST);
    rect_dsc.border_color = rect_dsc.bg_color;
    rect_dsc.bg_opa = LV_OPA_10;
    rect_dsc.border_opa = LV_OPA_100;
    rect_dsc.border_width = 2;
    lv_draw_sw_rect(draw_unit, &rect_dsc, &area_rot);

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

    lv_area_t txt_area;
    txt_area.x1 = draw_area.x1;
    txt_area.x2 = draw_area.x1 + txt_size.x - 1;
    txt_area.y2 = draw_area.y2;
    txt_area.y1 = draw_area.y2 - txt_size.y + 1;

    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_black();
    lv_draw_sw_rect(draw_unit, &rect_dsc, &txt_area);

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", idx);
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.text = buf;
    lv_draw_sw_label(draw_unit, &label_dsc, &txt_area);
#endif
}



LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_img(lv_draw_unit_t * draw_unit, const lv_draw_img_dsc_t * draw_dsc,
                                          const lv_area_t * coords)
{


    lv_area_t transformed_area;
    lv_area_copy(&transformed_area, coords);
    if(draw_dsc->angle || draw_dsc->zoom != LV_ZOOM_NONE) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_img_buf_get_transformed_area(&transformed_area, w, h, draw_dsc->angle, draw_dsc->zoom, &draw_dsc->pivot);

        transformed_area.x1 += coords->x1;
        transformed_area.y1 += coords->y1;
        transformed_area.x2 += coords->x1;
        transformed_area.y2 += coords->y1;
    }

    lv_area_t draw_area; /*Common area of cilp_area and coords. The image is visible only here*/
    /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
    if(!_lv_area_intersect(&draw_area, draw_unit->clip_area, &transformed_area)) {
        return;
    }

    const lv_area_t * clip_area_ori = draw_unit->clip_area;
    draw_unit->clip_area = &draw_area;

    bool transformed = draw_dsc->angle != 0 || draw_dsc->zoom != LV_ZOOM_NONE ? true : false;

    lv_img_decoder_dsc_t decoder_dsc;
    lv_img_decoder_open(&decoder_dsc, draw_dsc->src, draw_dsc->recolor, -1);
    const uint8_t * src_buf = decoder_dsc.img_data;

    lv_color_format_t cf = decoder_dsc.header.cf;


    lv_draw_sw_blend_dsc_t blend_dsc;

    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;

    /*The simplest case just copy the pixels into the draw_buf*/
    if(!transformed && cf == LV_COLOR_FORMAT_NATIVE && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        blend_dsc.src_buf = (const lv_color_t *)src_buf;
        blend_dsc.blend_area = coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && cf == LV_COLOR_FORMAT_A8) {
        lv_area_t clipped_coords;
        if(!_lv_area_intersect(&clipped_coords, coords, draw_unit->clip_area)) return;

        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_area = coords;
        blend_dsc.src_buf = NULL;
        blend_dsc.color = draw_dsc->recolor;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        blend_dsc.blend_area = coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && cf == LV_COLOR_FORMAT_RGB565A8 && draw_dsc->recolor_opa == LV_OPA_TRANSP) {
        lv_coord_t src_w = lv_area_get_width(coords);
        lv_coord_t src_h = lv_area_get_height(coords);
        blend_dsc.src_buf = src_buf;
        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_buf += 2 * src_w * src_h;
        blend_dsc.blend_area = coords;
        blend_dsc.mask_area = coords;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        //        lv_area_t blend_area;
        //        blend_dsc.blend_area = &blend_area;
        //        blend_area.x1 = draw_unit->clip_area->x1;
        //        blend_area.x2 = draw_unit->clip_area->x2;
        //        blend_area.y1 = draw_unit->clip_area->y1;
        //        blend_area.y2 = draw_unit->clip_area->y2;
        //
        //        lv_coord_t src_w = lv_area_get_width(coords);
        //        lv_coord_t src_h = lv_area_get_height(coords);
        //        lv_coord_t blend_h = lv_area_get_height(&blend_area);
        //        lv_coord_t blend_w = lv_area_get_width(&blend_area);
        //
        //        uint32_t max_buf_size = MAX_BUF_SIZE;
        //        uint32_t blend_size = lv_area_get_size(&blend_area);
        //        uint32_t buf_h;
        //        uint32_t buf_w = blend_w;
        //        if(blend_size <= max_buf_size) {
        //            buf_h = blend_h;
        //        }
        //        else {
        //            /*Round to full lines*/
        //            buf_h = max_buf_size / blend_w;
        //        }
        //
        //        /*Create buffers and masks*/
        //        uint32_t buf_size = buf_w * buf_h;
        //
        //        lv_color_t * rgb_buf = lv_malloc(buf_size * sizeof(lv_color_t));
        //        lv_opa_t * mask_buf = lv_malloc(buf_size);
        //        blend_dsc.mask_buf = mask_buf;
        //        blend_dsc.mask_area = &blend_area;
        //        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        //        blend_dsc.src_buf = rgb_buf;
        //        lv_coord_t y_last = blend_area.y2;
        //        blend_area.y2 = blend_area.y1 + buf_h - 1;
        //
        //        lv_draw_sw_mask_res_t mask_res_def = (cf != LV_COLOR_FORMAT_NATIVE || draw_dsc->angle ||
        //                                              draw_dsc->zoom != LV_ZOOM_NONE) ?
        //                                             LV_DRAW_SW_MASK_RES_CHANGED : LV_DRAW_SW_MASK_RES_FULL_COVER;
        //        blend_dsc.mask_res = mask_res_def;
        //
        //        if(cf == LV_COLOR_FORMAT_A8) {
        //            lv_color_buf_fill(rgb_buf, draw_dsc->recolor, buf_size);
        //        }
        //
        //
        //        lv_draw_img_sup_t sup;
        //        sup.alpha_color = draw_dsc->recolor;
        //        sup.palette = decoder_dsc.palette;
        //        sup.palette_size = decoder_dsc.palette_size;
        //        sup.chroma_keyed = cf == LV_COLOR_FORMAT_NATIVE_CHROMA_KEYED ? true : false;
        //        sup.chroma_key_color = LV_COLOR_CHROMA_KEY;
        //
        //        while(blend_area.y1 <= y_last) {
        //            /*Apply transformations if any or separate the channels*/
        //            lv_area_t transform_area;
        //            lv_area_copy(&transform_area, &blend_area);
        //            lv_area_move(&transform_area, -coords->x1, -coords->y1);
        //            if(transformed) {
        //                lv_draw_sw_transform(draw_unit, &transform_area, src_buf, src_w, src_h, src_w,
        //                                     draw_dsc, &sup, cf, rgb_buf, mask_buf);
        //            }
        //            else {
        //                convert_cb(&transform_area, src_buf, src_w, src_h, src_w, &sup, cf, rgb_buf, mask_buf);
        //            }
        //
        //            /*Apply recolor*/
        //            if(draw_dsc->recolor_opa > LV_OPA_MIN) {
        //                uint16_t premult_v[3];
        //                lv_opa_t recolor_opa = draw_dsc->recolor_opa;
        //                lv_color_t recolor = draw_dsc->recolor;
        //                LV_COLOR_PREMULT(recolor, recolor_opa, premult_v);
        //                recolor_opa = 255 - recolor_opa;
        //                uint32_t i;
        //                for(i = 0; i < buf_size; i++) {
        //                    rgb_buf[i] = LV_COLOR_MIX_PREMULT(premult_v, rgb_buf[i], recolor_opa);
        //                }
        //            }
        //
        //            /*Blend*/
        //            lv_draw_sw_blend(draw_unit, &blend_dsc);
        //
        //            /*Go the the next lines*/
        //            blend_area.y1 = blend_area.y2 + 1;
        //            blend_area.y2 = blend_area.y1 + buf_h - 1;
        //            if(blend_area.y2 > y_last) blend_area.y2 = y_last;
        //        }
        //
        //        lv_free(mask_buf);
        //        lv_free(rgb_buf);
    }
    draw_unit->clip_area = clip_area_ori;

    lv_img_decoder_close(&decoder_dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SW*/
