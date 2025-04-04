/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

/*============================ INCLUDES ======================================*/
#include "../vsf_tgui_cfg.h"

#if VSF_USE_TINY_GUI == ENABLED

#define __VSF_TGUI_CONTROLS_LABEL_CLASS_IMPLEMENT
// for tDirtyRegion is enabled
#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_label.h"
#include "../view/vsf_tgui_v.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const i_tgui_control_methods_t c_tVLabel= {

    .tView = {
        .Init =     (vsf_tgui_v_method_t *)&vsf_tgui_label_v_init,
        .Depose =   (vsf_tgui_v_method_t *)&vsf_tgui_label_v_depose,
        .Render =   (vsf_tgui_v_method_render_t *)&vsf_tgui_label_v_render,
        .Update =   (vsf_tgui_v_method_t *)&vsf_tgui_label_v_update,
    },
    .Init =     (vsf_tgui_control_method_t *)&vk_tgui_label_init,
    .Update =   (vsf_tgui_control_method_t *)&vk_tgui_label_update,

};

/*============================ IMPLEMENTATION ================================*/

fsm_rt_t vsf_tgui_label_msg_handler(    vsf_tgui_label_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t *)control_ptr,
                                            ptMSG,
                                            &c_tVLabel);
}

vsf_tgui_size_t vsf_tgui_label_text_get_size(   vsf_tgui_label_t* label_ptr,
                                                uint16_t *line_count_ptr,
                                                uint8_t *char_height_ptr)
{
    VSF_TGUI_ASSERT(label_ptr != NULL);

#if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
    if (label_ptr->tLabel.bIsChanged) {
        label_ptr->tLabel.tInfoCache.tStringSize = vsf_tgui_text_get_size(
                                        __vk_tgui_label_v_get_font(label_ptr),
                                        &label_ptr->tLabel,
                                        line_count_ptr,
                                        char_height_ptr);
    }
    return label_ptr->tLabel.tInfoCache.tStringSize;
#else
    vsf_tgui_size_t size = vsf_tgui_text_get_size(
                                        __vk_tgui_label_v_get_font(label_ptr),
                                        &label_ptr->tLabel,
                                        line_count_ptr,
                                        char_height_ptr);

    return size;
#endif
}

fsm_rt_t vk_tgui_label_update(vsf_tgui_label_t* ptLabel)
{
    if (fsm_rt_cpl == vk_tgui_control_update((vsf_tgui_control_t *)ptLabel)) {
    #if VSF_TGUI_CFG_TEXT_SIZE_INFO_CACHING == ENABLED
        if (ptLabel->tLabel.bIsChanged) {
            ptLabel->tLabel.tInfoCache.tStringSize =
                vsf_tgui_label_text_get_size(   ptLabel, 
                                                &(ptLabel->tLabel.tInfoCache.hwLines),
                                                NULL);
        }
    #endif

        if (ptLabel->bIsAutoSize) {

            vsf_tgui_size_t tNewSize =
                __vk_tgui_label_v_get_minimal_rendering_size(ptLabel);
            vsf_tgui_size_t *ptControlSize =
                    &(ptLabel->use_as__vsf_tgui_control_t.tRegion.tSize);

            ptControlSize->iHeight = tNewSize.iHeight;
            ptControlSize->iWidth = vsf_max(tNewSize.iWidth, ptControlSize->iWidth);

        }

        return fsm_rt_cpl;
    }

    return fsm_rt_on_going;
}

fsm_rt_t vk_tgui_label_init(vsf_tgui_label_t* ptLabel)
{
    return vk_tgui_control_init((vsf_tgui_control_t*)ptLabel);
}

void vsf_tgui_label_set_text(   vsf_tgui_label_t* ptLabel,
                                VSF_TGUI_CFG_STRING_TYPE *pstrText)
{
#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    vsf_tgui_region_t temp_region = {
        .tSize      = vsf_tgui_label_text_get_size(ptLabel, NULL, NULL),
    };
    vsf_tgui_region_t text_dirty_region = {
        .tSize      = ptLabel->tSize,
    };
    vsf_tgui_region_update_with_align(&text_dirty_region, &temp_region, ptLabel->tLabel.u4Align);
    vsf_tgui_region_unite(&ptLabel->tDirtyRegion, &ptLabel->tDirtyRegion, &text_dirty_region);
#endif

    vsf_tgui_text_set(&ptLabel->tLabel, pstrText);

#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    temp_region = (vsf_tgui_region_t) {
        .tSize      = vsf_tgui_label_text_get_size(ptLabel, NULL, NULL),
    };
    text_dirty_region = (vsf_tgui_region_t) {
        .tSize      = ptLabel->tSize,
    };
    vsf_tgui_region_update_with_align(&text_dirty_region, &temp_region, ptLabel->tLabel.u4Align);
    vsf_tgui_region_unite(&ptLabel->tDirtyRegion, &ptLabel->tDirtyRegion, &text_dirty_region);
#endif
}

#endif


/* EOF */
