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

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_IMPLEMENT
declare_class(vsf_tgui_t)
#include "./vsf_tgui_control.h"
#include "../view/vsf_tgui_v.h"

#include "../vsf_tgui.h"


#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED
#   if VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER != ENABLED
#       error To use TGUI TIMER, you have to enable callback timer in kernel \
by setting macro VSF_KERNEL_CFG_SUPPORT_CALLBACK_TIMER to ENABLED
#   endif
#endif
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static const i_tgui_control_methods_t c_tVControl= {

    .tView = {
        .Init =     &vsf_tgui_control_v_init,
        .Depose =   &vsf_tgui_control_v_depose,
        .Render =   &vsf_tgui_control_v_render,
        .Update =   &vsf_tgui_control_v_update,
    },
    .Update =   &vk_tgui_control_update,
    .Init =     &vk_tgui_control_init,

};

static const i_tgui_control_methods_t c_tVContainer= {

    .tView = {
        .Init =     (vsf_tgui_v_method_t *)&vsf_tgui_container_v_init,
        .Depose =   (vsf_tgui_v_method_t *)&vsf_tgui_container_v_depose,
        .Render =   (vsf_tgui_v_method_render_t *)&vsf_tgui_container_v_render,
        .ContainerPostRender = (vsf_tgui_v_method_render_t *)&vsf_tgui_container_v_post_render,
        .Update =   (vsf_tgui_v_method_t *)&vsf_tgui_container_v_update,
    },
    .Update =   (vsf_tgui_control_method_t *)&vk_tgui_container_update,
    .Init =     (vsf_tgui_control_method_t *)&vk_tgui_container_init,

};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


/*----------------------------------------------------------------------------*
 *  Timer                                                                     *
 *----------------------------------------------------------------------------*/
#if VSF_TGUI_CFG_SUPPORT_TIMER == ENABLED

static void __vk_tgui_control_timer_handler(vsf_tgui_timer_t *ptTimer)
{
    vsf_protect_t tProtectStatus;
    VSF_TGUI_ASSERT(NULL != ptTimer);

    const vsf_tgui_root_container_t* ptTopContainer
        = vk_tgui_control_get_top(ptTimer->target_ptr);

    if (NULL == ptTopContainer) {
        return ;
    }

    vk_tgui_send_timer_event(ptTopContainer->gui_ptr, ptTimer->target_ptr);

    tProtectStatus = vsf_protect_sched();
        if (ptTimer->bEnabled && ptTimer->bIsRepeat) {
            ptTimer->u29Interval = vsf_max(100, ptTimer->u29Interval);
            vsf_callback_timer_add_ms(  &ptTimer->use_as__vsf_callback_timer_t,
                                        ptTimer->u29Interval);
        } else {
            ptTimer->Status.bIsWorking = false;
        }
    vsf_unprotect_sched(tProtectStatus);
}

void vsf_tgui_timer_init(   vsf_tgui_timer_t *ptTimer,
                            const vsf_tgui_control_t *control_ptr)
{
    VSF_TGUI_ASSERT(NULL != ptTimer);
    VSF_TGUI_ASSERT(NULL != control_ptr);

    ptTimer->target_ptr = control_ptr;
    ptTimer->use_as__vsf_callback_timer_t.on_timer
        = (void (*)(vsf_callback_timer_t *timer))__vk_tgui_control_timer_handler;

    if (ptTimer->bEnabled){
        vsf_tgui_timer_enable(ptTimer);
    }
}

void vsf_tgui_timer_enable(vsf_tgui_timer_t *ptTimer)
{
    vsf_protect_t tProtectStatus;
    VSF_TGUI_ASSERT(NULL != ptTimer);

    tProtectStatus = vsf_protect_sched();
        do {
            if (ptTimer->Status.bIsWorking) {
                break;
            }
            if (ptTimer->bIsRepeat) {
                ptTimer->u29Interval = vsf_max(100, ptTimer->u29Interval);
            }
            ptTimer->Status.bIsWorking = true;
            ptTimer->Status.bEnabled = true;
            vsf_callback_timer_add_ms(  &ptTimer->use_as__vsf_callback_timer_t,
                                        ptTimer->u29Interval);
        } while(0);
    vsf_unprotect_sched(tProtectStatus);
}

void vsf_tgui_timer_disable(vsf_tgui_timer_t *ptTimer)
{

    vsf_protect_t tProtectStatus;
    VSF_TGUI_ASSERT(NULL != ptTimer);

    tProtectStatus = vsf_protect_sched();
        do {
            if (!ptTimer->Status.bIsWorking) {
                break;
            }
            vsf_callback_timer_remove(&(ptTimer->use_as__vsf_callback_timer_t));
            ptTimer->Status.bIsWorking = false;
        } while(0);
        ptTimer->Status.bEnabled = false;
    vsf_unprotect_sched(tProtectStatus);
}

bool vsf_tgui_timer_is_working(vsf_tgui_timer_t *ptTimer)
{
    VSF_TGUI_ASSERT(NULL != ptTimer);
    return ptTimer->Status.bIsWorking;
}

#endif

/*----------------------------------------------------------------------------*
 *  Region                                                                    *
 *----------------------------------------------------------------------------*/

/*! \brief get the relative location information base on the location information
 *!        of or derived from target control till parent control.
 *! \param control_ptr    the Target Control Address
 *! \param parent_ptr     the Parent Control Address, absolute location will be return if NULL
 *! \param ptLocation   the Location buffer which has already stored the location
 *!                     information of or derived from the target control
 *! \return the location buffer address passed with ptLocation
 */
vsf_tgui_location_t* __vk_tgui_calculate_control_location_from_parent(
                                                const vsf_tgui_control_t *control_ptr,
                                                const vsf_tgui_control_t *parent_ptr,
                                                vsf_tgui_location_t *ptLocation)
{
    VSF_TGUI_ASSERT(NULL != control_ptr && NULL != ptLocation);

    do {
        control_ptr = (const vsf_tgui_control_t *)control_ptr->parent_ptr;
        if (control_ptr == NULL) {
            break;
        }
        VSF_TGUI_ASSERT(control_ptr->Attribute._.is_container);

        vsf_tgui_container_t *container_ptr = (vsf_tgui_container_t*)control_ptr;
        ptLocation->iX += container_ptr->tRegion.tLocation.iX;
        ptLocation->iY += container_ptr->tRegion.tLocation.iY;
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
        ptLocation->iX += container_ptr->tContainerPadding.chLeft;
        ptLocation->iY += container_ptr->tContainerPadding.chTop;
#endif
    } while (control_ptr != parent_ptr);

    return ptLocation;
}

vsf_tgui_location_t *vsf_tgui_control_get_location(const vsf_tgui_control_t* control_ptr)
{
    return (vsf_tgui_location_t *)&(control_ptr->tRegion.tLocation);
}

vsf_tgui_size_t *vsf_tgui_control_get_size(const vsf_tgui_control_t* control_ptr)
{
    return (vsf_tgui_size_t *)&(control_ptr->tRegion.tSize);
}

vsf_tgui_location_t * vsf_tgui_control_calculate_absolute_location(
                                            const vsf_tgui_control_t* control_ptr,
                                            vsf_tgui_location_t* ptOffset)
{
    //const vsf_msgt_node_t* node_ptr;
    VSF_TGUI_ASSERT(NULL != control_ptr);
    VSF_TGUI_ASSERT(NULL != ptOffset);

    ptOffset->iX += control_ptr->tRegion.tLocation.iX;
    ptOffset->iY += control_ptr->tRegion.tLocation.iY;

    return __vk_tgui_calculate_control_location_from_parent(control_ptr, NULL, ptOffset);
}


/*! \brief If you get a relative region inside a control, this function can calculate
 *!        the absolute location and store the result in the region you specified.
 *!
 *! \NOTE  As the location info of the region you passed to this
 *!        function will be changed, please do **NOT** pass the control's own region
 *!        to this function.
 *!        Due to this reason aforementioned, you should creat a copy of the target
 *!        region and use it with this function.
 *!
 *! \param control_ptr  the address of the target control
 *! \param region_ptr   the address of the region which you want to calculate
 *! \return the same region address you passed to the function
 *!
 */
vsf_tgui_region_t* vsf_tgui_control_calculate_absolute_region(
                                                const vsf_tgui_control_t *control_ptr,
                                                vsf_tgui_region_t *region_ptr)
{
    return (vsf_tgui_region_t *)
                vsf_tgui_control_calculate_absolute_location(
                                                control_ptr,
                                                (vsf_tgui_location_t *)region_ptr);
}

vsf_tgui_region_t * vsf_tgui_get_absolute_control_region(
                                                const vsf_tgui_control_t* control_ptr,
                                                vsf_tgui_region_t* ptRegionBuffer)
{
    VSF_TGUI_ASSERT(NULL != control_ptr && NULL != ptRegionBuffer);

    *ptRegionBuffer = control_ptr->tRegion;

    __vk_tgui_calculate_control_location_from_parent(control_ptr, NULL, &(ptRegionBuffer->tLocation));
    return ptRegionBuffer;
}

bool vsf_tgui_control_is_in_range(      const vsf_tgui_region_t *region_ptr,
                                        const vsf_tgui_location_t *ptLocation)
{
    bool result = false;
    VSF_TGUI_ASSERT(NULL != region_ptr && NULL != ptLocation);

    do {

        if (    (ptLocation->iX < region_ptr->tLocation.iX)
            ||  (ptLocation->iY < region_ptr->tLocation.iY)) {
            break;
        }

        {/*! to mitigate C89/90 warning */
            int_fast16_t iX = region_ptr->tLocation.iX + region_ptr->tSize.iWidth;
            int_fast16_t iY = region_ptr->tLocation.iY + region_ptr->tSize.iHeight;
            if (    (ptLocation->iX >= iX)
                ||  (ptLocation->iY >= iY)) {
                break;
            }
        }

        result = true;
    } while(0);

    return result;
}

/*! \brief get the visible region (with absolute location)
 *! \param control_ptr    the target control
 *! \param region_ptr     the region buffer
 *! \retval true        visible
 *! \retval false       invisible
 */
bool vsf_tgui_control_get_visible_region(   const vsf_tgui_control_t* control_ptr,
                                            vsf_tgui_region_t * ptRegionBuffer)
{
    bool bHasIntersect = true;

    *ptRegionBuffer = control_ptr->tRegion;

#if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
    do {
        const vsf_tgui_container_t* parent_ptr =
            (const vsf_tgui_container_t*)control_ptr->use_as__vsf_msgt_node_t.parent_ptr;

        while (NULL != parent_ptr) {
            __vk_tgui_calculate_control_location_from_parent(control_ptr,
                    (vsf_tgui_control_t *)parent_ptr, &ptRegionBuffer->tLocation);
            bHasIntersect = vsf_tgui_region_intersect(ptRegionBuffer, ptRegionBuffer, &parent_ptr->tRegion);
            if (!bHasIntersect) {
                break;
            }
            control_ptr = &parent_ptr->use_as__vsf_tgui_control_t;
            parent_ptr = (const vsf_tgui_container_t *)control_ptr->use_as__vsf_msgt_node_t.parent_ptr;
        }
    } while (0);
#endif
    return bHasIntersect;
}


vsf_tgui_region_t* vsf_tgui_control_get_relative_region(
                                                const vsf_tgui_control_t* control_ptr,
                                                vsf_tgui_region_t* ptAbsoluteRegion)
{
    vsf_tgui_region_t tRegion = control_ptr->tRegion;

    VSF_TGUI_ASSERT(NULL != control_ptr && NULL != ptAbsoluteRegion);

    __vk_tgui_calculate_control_location_from_parent(control_ptr, NULL, &(tRegion.tLocation));

    vsf_tgui_region_get_relative_region(ptAbsoluteRegion, &tRegion, ptAbsoluteRegion);

    return ptAbsoluteRegion;
}


vsf_tgui_region_t * vsf_tgui_control_generate_dirty_region_from_parent_dirty_region(
                                    const vsf_tgui_control_t *parent_ptr,
                                    const vsf_tgui_region_t *ptParentDirtyRegion,
                                    const vsf_tgui_control_t *ptPrivate,
                                    vsf_tgui_region_t *ptNewDirtyRegionBuffer)
{
    VSF_TGUI_ASSERT(NULL != parent_ptr && NULL != ptPrivate && NULL != ptNewDirtyRegionBuffer);
    vsf_tgui_region_t tParentDirtyRegion;
    if (NULL != ptParentDirtyRegion) {
        tParentDirtyRegion = *ptParentDirtyRegion;
    } else {
        tParentDirtyRegion = parent_ptr->tRegion;
    }

    vsf_tgui_control_calculate_absolute_region(parent_ptr, &tParentDirtyRegion);
    vsf_tgui_get_absolute_control_region(ptPrivate, ptNewDirtyRegionBuffer);
    if (!vsf_tgui_region_intersect(  ptNewDirtyRegionBuffer,
                                ptNewDirtyRegionBuffer,
                               &tParentDirtyRegion)) {
        return NULL;
    }
    vsf_tgui_control_get_relative_region(ptPrivate, ptNewDirtyRegionBuffer);

    return ptNewDirtyRegionBuffer;
}


bool vsf_tgui_control_shoot(    const vsf_tgui_control_t* control_ptr,
                                const vsf_tgui_location_t *ptLocation)
{
    vsf_tgui_region_t tRegion;
    VSF_TGUI_ASSERT(NULL != control_ptr && NULL != ptLocation);
    uint_fast8_t chMask =   (   VSF_TGUI_CTRL_STATUS_INITIALISED
                            |   VSF_TGUI_CTRL_STATUS_VISIBLE);

    if (chMask != (control_ptr->Status.chStatus & chMask)) {
        return false;
    }

    if (!vsf_tgui_control_get_visible_region(control_ptr, &tRegion)) {
        return false;
    }

    return vsf_tgui_control_is_in_range( &tRegion, ptLocation);
}


/*----------------------------------------------------------------------------*
 *  Status and Attributes                                                     *
 *----------------------------------------------------------------------------*/

 /*! \brief update bIsTransparent bit in control status
  *! \parame control_ptr target control address
  *! \retval true the original value of is_control_transparent is changed
  *! \retval false the set value is the same as the original value, no change is
  *!               made.
  */
bool vsf_tgui_control_set_is_transparent_bit(vsf_tgui_control_t* control_ptr,
                                             bool is_control_transparent)
{
    bool result = false;
    VSF_TGUI_ASSERT(NULL != control_ptr);

    result =       is_control_transparent
                !=  (control_ptr->Status.Values.bIsTransparent);

    if (result) {
        control_ptr->Status.Values.bIsTransparent = is_control_transparent;
    }

    return result;
}

vsf_tgui_status_t vsf_tgui_control_status_get(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);

    return control_ptr->Status;
}

void vsf_tgui_control_status_set(vsf_tgui_control_t* control_ptr, vsf_tgui_status_t Status)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);

    control_ptr->Status = Status;
}

bool vsf_tgui_control_is_container(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);
    return control_ptr->use_as__vsf_msgt_node_t.Attribute._.is_container;
}

vsf_tgui_control_t* vsf_tgui_control_get_parent(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);
    return (vsf_tgui_control_t *)control_ptr->use_as__vsf_msgt_node_t.parent_ptr;
}


uint_fast8_t vk_tgui_container_visible_item_get(const vsf_tgui_container_t *container_ptr)
{
    VSF_TGUI_ASSERT(NULL != container_ptr);
    return container_ptr->chVisibleItemCount;
}

/*----------------------------------------------------------------------------*
 *  Methods and Others                                                        *
 *----------------------------------------------------------------------------*/

const vsf_tgui_root_container_t *
vk_tgui_control_get_top(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT (NULL != control_ptr);

    while (NULL != control_ptr->use_as__vsf_msgt_node_t.parent_ptr) {
        control_ptr = (const vsf_tgui_control_t*)
            control_ptr->use_as__vsf_msgt_node_t.parent_ptr;
    }
    VSF_TGUI_ASSERT(control_ptr->use_as__vsf_msgt_node_t.Attribute._.is_top);

    return (const vsf_tgui_root_container_t*)control_ptr;
}

vsf_tgui_t* vsf_tgui_control_get_gui_instance(const vsf_tgui_control_t* control_ptr)
{
    const vsf_tgui_root_container_t *ptTopContainer = vk_tgui_control_get_top(control_ptr);
    VSF_TGUI_ASSERT (NULL != ptTopContainer && NULL != ptTopContainer->gui_ptr);
    return ptTopContainer->gui_ptr;
}

bool vsf_tgui_control_send_message( const vsf_tgui_control_t* control_ptr,
                                    vsf_tgui_evt_t event)
{
    VSF_TGUI_ASSERT (NULL != control_ptr);

    const vsf_tgui_root_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    if (NULL == event.use_as__vsf_tgui_msg_t.target_ptr) {
        event.use_as__vsf_tgui_msg_t.target_ptr = (vsf_tgui_control_t *)control_ptr;
    }

    return vk_tgui_send_message(ptTopContainer->gui_ptr, event);
}

bool vsf_tgui_control_update(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT (NULL != control_ptr);

    const vsf_tgui_root_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_update(ptTopContainer->gui_ptr, control_ptr);
}

bool vsf_tgui_control_update_tree(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT (NULL != control_ptr);

    const vsf_tgui_root_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_update_tree(ptTopContainer->gui_ptr, control_ptr);
}


#if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
bool vsf_tgui_control_refresh(  const vsf_tgui_control_t *control_ptr,
                                const vsf_tgui_region_t *region_ptr)
{
   VSF_TGUI_ASSERT (NULL != control_ptr);

    const vsf_tgui_root_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_refresh_ex(ptTopContainer->gui_ptr, control_ptr, region_ptr);
}
#endif

bool vsf_tgui_control_set_active(const vsf_tgui_control_t* control_ptr)
{
    VSF_TGUI_ASSERT (NULL != control_ptr);

    const vsf_tgui_root_container_t* ptTopContainer
        = vk_tgui_control_get_top(control_ptr);
    vsf_tgui_evt_t temp_event = {
            .msg = VSF_TGUI_EVT_GET_ACTIVE,
            .target_ptr = (vsf_tgui_control_t*)control_ptr,
        };

    if (NULL == ptTopContainer) {
        return false;
    }

    return vk_tgui_send_message(ptTopContainer->gui_ptr, temp_event);
}


#undef THIS_FSM_STATE
#define THIS_FSM_STATE  control_ptr->tMSGMap.chState

#define RESET_TGUI_USER_MSG_HANDLING_FSM()                          \
        do { THIS_FSM_STATE = 0; } while(0)

fsm_rt_t __vk_tgui_control_user_message_handling(   vsf_tgui_control_t* control_ptr,
                                                    const vsf_tgui_evt_t* event_ptr)
{
    uint_fast8_t chCount = control_ptr->tMSGMap.chCount;
    const vsf_tgui_user_evt_handler *ptItems = control_ptr->tMSGMap.ptItems;
    uint_fast16_t hwMSG = event_ptr->use_as__vsf_tgui_msg_t.use_as__vsf_msgt_msg_t.msg;


    VSF_TGUI_ASSERT(NULL != control_ptr && NULL != event_ptr);

    enum {
        START = 0,
        SEARCH_MSG_MAP,
        CHECK_USER_HANDLER_TYPE,
        CALL_FSM,
    };

    switch (THIS_FSM_STATE) {
        case START:
            if ((0 == chCount) || (NULL == ptItems) || (chCount > 254)) {
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            control_ptr->tMSGMap.chIndex = 255;
            THIS_FSM_STATE++;
            //break;
        case SEARCH_MSG_MAP: {
            uint_fast8_t chIndex = 0;
            do {
                //! get mask
                uint_fast16_t hwMask = ptItems->use_as__vsf_tgui_control_handler_t.u10EvtMask;

                //! check message
                if (    (ptItems->use_as__vsf_msgt_msg_t.msg & hwMask)
                    ==  (hwMSG & hwMask)) {
                    control_ptr->tMSGMap.chIndex = chIndex;
                    THIS_FSM_STATE = CHECK_USER_HANDLER_TYPE;
                    break;
                }
                chIndex++;
                ptItems++;
            } while(--chCount);

            if (control_ptr->tMSGMap.chIndex == 255) {
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            //break;
        }

        case CHECK_USER_HANDLER_TYPE:
            switch (ptItems->use_as__vsf_tgui_control_handler_t.u2Type) {
                case VSF_MSGT_NODE_HANDLER_TYPE_FSM:
                #if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
                    {
                        vsf_evt_t msg = ptItems->use_as__vsf_msgt_msg_t.msg;

                        if (NULL != ptItems->use_as__vsf_tgui_control_handler_t.handler_name_ptr &&
                    #if VSF_TGUI_CFG_SHOW_ON_LOAD_EVT_LOG != ENABLED
                            (msg != VSF_TGUI_EVT_ON_LOAD) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_UPDATE_EVT_LOG != ENABLED
                            (msg != VSF_TGUI_EVT_UPDATE) &&
                            (msg != VSF_TGUI_EVT_UPDATE_TREE) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_ON_DEPOSE_EVT_LOG != ENABLED
                            (msg != VSF_TGUI_EVT_ON_DEPOSE) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG != ENABLED
                            (msg != VSF_TGUI_EVT_REFRESH) &&
                            (msg != VSF_TGUI_EVT_POST_REFRESH) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG != ENABLED
                            (msg != VSF_TGUI_EVT_ON_TIME) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_GET_ACTIVE_EVT_LOG != ENABLED
                            (msg != VSF_TGUI_EVT_GET_ACTIVE) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_LOST_ACTIVE_EVT_LOG != ENABLED
                            (msg != VSF_TGUI_EVT_LOST_ACTIVE) &&
                    #endif

                    #if VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG != ENABLED
                            ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_POINTER_EVT & VSF_TGUI_MSG_MSK)) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_KEY_EVT_LOG != ENABLED
                            ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_KEY_EVT & VSF_TGUI_MSG_MSK)) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG != ENABLED
                            ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_GESTURE_EVT & VSF_TGUI_MSG_MSK)) &&
                    #endif
                    #if VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG != ENABLED
                            ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT & VSF_TGUI_MSG_MSK)) &&
                    #endif
                        true) {
                            VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                                            VSF_TRACE_CFG_LINEEND
                                            "[CALL User Event Handler] %s" VSF_TRACE_CFG_LINEEND "\t",
                                            ptItems->use_as__vsf_tgui_control_handler_t.handler_name_ptr);
                        }
                    }
                #endif
                    THIS_FSM_STATE = CALL_FSM;
                    break;
                case VSF_MSGT_NODE_HANDLER_TYPE_EDA: {
                    vsf_err_t tErr = vsf_eda_post_msg(
                                        ptItems->use_as__vsf_tgui_control_handler_t.fn.ptEDA,
                                        (void *)&(event_ptr->use_as__vsf_tgui_msg_t));
                    VSF_OSA_SERVICE_ASSERT(tErr == VSF_ERR_NONE);
                    VSF_UNUSED_PARAM(tErr);
                    RESET_TGUI_USER_MSG_HANDLING_FSM();
                    return fsm_rt_cpl;
                }

                case VSF_MSGT_NODE_HANDLER_TYPE_SUBCALL:
                default:
                    //! haven't support yet
                    RESET_TGUI_USER_MSG_HANDLING_FSM();
                    return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            break;

        case CALL_FSM: {
            fsm_rt_t fsm_rt = ptItems[control_ptr->tMSGMap.chIndex]
                                .use_as__vsf_tgui_control_handler_t
                                    .fn.FSM(control_ptr,
                                            (vsf_tgui_msg_t *)&(event_ptr->use_as__vsf_tgui_msg_t));
            if (    fsm_rt_cpl == fsm_rt
                ||  (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH == fsm_rt
                ||  (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH_PARENT == fsm_rt) {
                //! message has been handled
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return fsm_rt;
            } else if (fsm_rt < 0) {
                //! message is not handled
                RESET_TGUI_USER_MSG_HANDLING_FSM();
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            } else {
                return fsm_rt;
            }
            break;
        }

    }

    return fsm_rt_on_going;
}

fsm_rt_t __vsf_tgui_control_msg_handler(vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG,
                                        const i_tgui_control_methods_t*ptMethods)
{
    const vsf_tgui_root_container_t* ptTopContainer = vk_tgui_control_get_top(control_ptr);
    vsf_tgui_t *gui_ptr = ptTopContainer->gui_ptr;
    vsf_evt_t msg = ptMSG->use_as__vsf_msgt_msg_t.msg;
    fsm_rt_t result = (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;

    VSF_TGUI_ASSERT(NULL != control_ptr && NULL != ptMSG);

    if (
        (msg != VSF_TGUI_EVT_REFRESH) &&
#if VSF_TGUI_CFG_SHOW_ON_LOAD_EVT_LOG != ENABLED
        (msg != VSF_TGUI_EVT_ON_LOAD) &&
#endif
#if VSF_TGUI_CFG_SHOW_UPDATE_EVT_LOG != ENABLED
        (msg != VSF_TGUI_EVT_UPDATE) &&
        (msg != VSF_TGUI_EVT_UPDATE_TREE) &&
#endif
#if VSF_TGUI_CFG_SHOW_ON_DEPOSE_EVT_LOG != ENABLED
        (msg != VSF_TGUI_EVT_ON_DEPOSE) &&
#endif
#if VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG != ENABLED
        (msg != VSF_TGUI_EVT_ON_TIME) &&
#endif
#if VSF_TGUI_CFG_SHOW_GET_ACTIVE_EVT_LOG != ENABLED
        (msg != VSF_TGUI_EVT_GET_ACTIVE) &&
#endif
#if VSF_TGUI_CFG_SHOW_LOST_ACTIVE_EVT_LOG != ENABLED
        (msg != VSF_TGUI_EVT_LOST_ACTIVE) &&
#endif

#if VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_POINTER_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
#if VSF_TGUI_CFG_SHOW_KEY_EVT_LOG != ENABLED || VSF_TGUI_CFG_SUPPORT_KEY_EVENTS != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_KEY_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
#if VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_GESTURE_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
#if VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG != ENABLED
        ((msg & VSF_TGUI_MSG_MSK) != (VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT & VSF_TGUI_MSG_MSK)) &&
#endif
    true) {
#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
    VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                    VSF_TRACE_CFG_LINEEND
                    "[Control Event]%s" VSF_TRACE_CFG_LINEEND "\t",
                    control_ptr->use_as__vsf_msgt_node_t.node_name_ptr);
#else
    VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                    VSF_TRACE_CFG_LINEEND
                    "[Control Event]");
#endif
    }

    switch(msg & VSF_TGUI_MSG_MSK) {

        case VSF_TGUI_MSG_CONTROL_EVT & VSF_TGUI_MSG_MSK:
            switch(msg & VSF_TGUI_EVT_MSK) {

            #if VSF_TGUI_CFG_SUPPORT_CONSTRUCTOR_SCHEME == ENABLED
                case VSF_TGUI_EVT_ON_LOAD & VSF_TGUI_EVT_MSK:
                #if VSF_TGUI_CFG_SHOW_ON_LOAD_EVT_LOG == ENABLED
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_ON_LOAD");
                #endif
                    ptMethods->Init(control_ptr);
                    ptMethods->tView.Init(gui_ptr, control_ptr);
                    result = fsm_rt_cpl;
                    break;
            #endif

            #if VSF_TGUI_CFG_SUPPORT_DESTRUCTOR_SCHEME == ENABLED
                case VSF_TGUI_EVT_ON_DEPOSE & VSF_TGUI_EVT_MSK:
                #if VSF_TGUI_CFG_SHOW_ON_DEPOSE_EVT_LOG == ENABLED
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_ON_DEPOSE");
                #endif
                    ptMethods->tView.Depose(gui_ptr, control_ptr);
                    result = fsm_rt_cpl;
                    break;
            #endif

                case VSF_TGUI_EVT_UPDATE_TREE & VSF_TGUI_EVT_MSK:
                case VSF_TGUI_EVT_UPDATE & VSF_TGUI_EVT_MSK:
                #if VSF_TGUI_CFG_SHOW_UPDATE_EVT_LOG == ENABLED
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_UPDATE");
                #endif
                    ptMethods->Update(control_ptr);
                    ptMethods->tView.Update(gui_ptr, control_ptr);
                    result = fsm_rt_cpl;
                    break;

            #if VSF_TGUI_CFG_REFRESH_SCHEME != VSF_TGUI_REFRESH_SCHEME_NONE
                case VSF_TGUI_EVT_REFRESH & VSF_TGUI_EVT_MSK: {

                    vsf_tgui_control_refresh_mode_t tMode = VSF_TGUI_CONTROL_REFRESHED_BY_PARENT;
                    vsf_tgui_refresh_evt_t *event_ptr = (vsf_tgui_refresh_evt_t*)ptMSG;
                    vsf_tgui_region_t *temp_ptr = event_ptr->region_ptr;


                #if VSF_TGUI_CFG_SUPPORT_DIRTY_REGION == ENABLED
                    vsf_tgui_region_t tRegion = { 0 };
                    if (!vsf_tgui_control_get_visible_region(control_ptr, &tRegion)) {
                        return fsm_rt_cpl;
                    }
                    if (NULL != event_ptr->region_ptr) {
                        if (!vsf_tgui_region_intersect(&tRegion, &tRegion, event_ptr->region_ptr)) {
                            return fsm_rt_cpl;
                        }
                    }
                    vsf_tgui_control_get_relative_region(control_ptr, &tRegion);
                #else
                    vsf_tgui_region_t tRegion = { .tSize = ptCore->tSize };
                #endif
                    event_ptr->region_ptr = &tRegion;

                    if (control_ptr->Status.Values.__is_the_first_node_for_refresh) {
                        control_ptr->Status.Values.__is_the_first_node_for_refresh = false;
                        tMode = VSF_TGUI_CONTROL_REFRESHED_DIRECTLY_BY_USER;
                    }

                    if (control_ptr->use_as__vsf_msgt_node_t.Attribute._.is_visited) {
                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                                        VSF_TRACE_CFG_LINEEND
                                        "[Control Event]%s" VSF_TRACE_CFG_LINEEND "\t",
                                        control_ptr->use_as__vsf_msgt_node_t.node_name_ptr);
                        VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_POST_REFRESH");
                     #endif

                        ptMSG->use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_POST_REFRESH;
                        result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);

                        if (    ((fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED == result)
                            &&  (NULL != ptMethods->tView.ContainerPostRender)) {
                            result = ptMethods->tView.ContainerPostRender(gui_ptr, control_ptr, &tRegion, tMode);
                        }
                        ptMSG->use_as__vsf_msgt_msg_t.msg = VSF_TGUI_EVT_REFRESH;
                    } else {

                    #if VSF_TGUI_CFG_SHOW_REFRESH_EVT_LOG == ENABLED
                        VSF_TGUI_LOG(   VSF_TRACE_WARNING,
                                        VSF_TRACE_CFG_LINEEND
                                        "[Control Event]%s" VSF_TRACE_CFG_LINEEND "\t",
                                        control_ptr->use_as__vsf_msgt_node_t.node_name_ptr);
                        VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_REFRESH");
                     #endif

                        result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);
                        if (((fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED == result) && !control_ptr->Status.Values.__skip_render) {
                            result = ptMethods->tView.Render(gui_ptr, control_ptr, &tRegion, tMode);
                        }
                    }

                    event_ptr->region_ptr = temp_ptr;

                    return result;
                }
            #endif

            #if VSF_TGUI_CFG_SHOW_ON_TIME_EVT_LOG == ENABLED
                case VSF_TGUI_EVT_ON_TIME & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_EVT_ON_TIME");
                    break;
            #endif

                case VSF_TGUI_EVT_GET_ACTIVE & VSF_TGUI_EVT_MSK:
                #if VSF_TGUI_CFG_SHOW_GET_ACTIVE_EVT_LOG == ENABLED
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_GET_ACTIVE");
                #endif
                    result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);
                    if (result < 0) {
            #if VSF_TGUI_CFG_REFRESH_CONTROL_ON_ACTIVE_STATE_CHANGE == ENABLED
                        result = (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
            #else
                        result = fsm_rt_cpl;
            #endif
                    }
                    return result;

                case VSF_TGUI_EVT_LOST_ACTIVE & VSF_TGUI_EVT_MSK:
                #if VSF_TGUI_CFG_SHOW_LOST_ACTIVE_EVT_LOG == ENABLED
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_LOST_ACTIVE");
                #endif
                    result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG);
                    if (result < 0) {
            #if VSF_TGUI_CFG_REFRESH_CONTROL_ON_ACTIVE_STATE_CHANGE == ENABLED
                        result = (fsm_rt_t)VSF_TGUI_MSG_RT_REFRESH;
            #else
                        result = fsm_rt_cpl;
            #endif
                    }
                    return result;
            }

            while(fsm_rt_on_going == __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t *)ptMSG));
            break;

        //! message contains data
        case VSF_TGUI_MSG_POINTER_EVT & VSF_TGUI_MSG_MSK: {
            vsf_tgui_pointer_evt_t *event_ptr = (vsf_tgui_pointer_evt_t *)ptMSG;
            VSF_UNUSED_PARAM(event_ptr);

        #if VSF_TGUI_CFG_SHOW_POINTER_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_POINTER_EVT\t" );
            VSF_TGUI_LOG(VSF_TRACE_INFO, "Pointer %d ",event_ptr->idx );
            switch(msg & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_POINTER_DOWN & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Down" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        event_ptr->use_as__vsf_tgui_location_t.iX,
                        event_ptr->use_as__vsf_tgui_location_t.iY);

                    break;
                case VSF_TGUI_EVT_POINTER_UP & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Up" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        event_ptr->use_as__vsf_tgui_location_t.iX,
                        event_ptr->use_as__vsf_tgui_location_t.iY);

                    break;
                case VSF_TGUI_EVT_POINTER_CLICK & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Click");
                    break;

            #if VSF_TGUI_CFG_SUPPORT_MOUSE_LIKE_EVENTS == ENABLED

                case VSF_TGUI_EVT_POINTER_ENTER & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Enter");
                    break;

                case VSF_TGUI_EVT_POINTER_LEFT & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Left");
                    break;
            #   if VSF_TGUI_CFG_SUPPORT_MOUSE_MOVE_HANDLING == ENABLED
                case VSF_TGUI_EVT_POINTER_MOVE & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Move" );

                    VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d",
                        event_ptr->use_as__vsf_tgui_location_t.iX,
                        event_ptr->use_as__vsf_tgui_location_t.iY);
                    break;
            #   endif
            #endif

                default:
                break;
            }
        #endif

            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);

        #if VSF_TGUI_CFG_SUPPORT_MOUSE_LIKE_EVENTS == ENABLED
            switch(msg & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_POINTER_ENTER & VSF_TGUI_EVT_MSK:
                case VSF_TGUI_EVT_POINTER_LEFT & VSF_TGUI_EVT_MSK:
                    if (result < 0) {
                        result = (fsm_rt_t)VSF_TGUI_MSG_RT_DONE;
                    }
                    break;
            }
        #endif

            break;
        }
    #if VSF_TGUI_CFG_SUPPORT_KEY_EVENTS == ENABLED
        case VSF_TGUI_MSG_KEY_EVT & VSF_TGUI_MSG_MSK:{
            vsf_tgui_key_evt_t* event_ptr = (vsf_tgui_key_evt_t *)ptMSG;
            VSF_UNUSED_PARAM(event_ptr);

        #if VSF_TGUI_CFG_SHOW_KEY_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_KEY_EVT\t");
            switch (msg & VSF_TGUI_EVT_MSK) {
                case VSF_TGUI_EVT_KEY_DOWN & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Key Down   ");
                    break;
                case VSF_TGUI_EVT_KEY_UP & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Key Up     ");
                    break;
                case VSF_TGUI_EVT_KEY_PRESSED & VSF_TGUI_EVT_MSK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "Key Pressed");
                    break;
                default:
                    break;
            }

            switch(event_ptr->hwKeyValue) {
                case VSF_TGUI_KEY_UP:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_UP]");
                    break;
                case VSF_TGUI_KEY_DOWN:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_DOWN]");
                    break;
                case VSF_TGUI_KEY_LEFT:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_LEFT]");
                    break;
                case VSF_TGUI_KEY_RIGHT:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_RIGHT]");
                    break;
                case VSF_TGUI_KEY_OK:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_OK]");
                    break;
                case VSF_TGUI_KEY_CANCEL:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[KEY_CANCEL]");
                    break;
                default:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\t[0x%04x]", event_ptr->hwKeyValue);
                    break;
            }
        #endif
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
        }
    #endif

        case VSF_TGUI_MSG_GESTURE_EVT & VSF_TGUI_MSG_MSK: {
            vsf_tgui_gesture_evt_t* event_ptr = (vsf_tgui_gesture_evt_t *)ptMSG;
            VSF_UNUSED_PARAM(event_ptr);

        #if VSF_TGUI_CFG_SHOW_GESTURE_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO, "VSF_TGUI_MSG_GESTURE_EVT\t");
            switch (msg) {
                case VSF_TGUI_EVT_GESTURE_SLIDE:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\tFinger %d Slide", event_ptr->idx);
                    break;
                case VSF_TGUI_EVT_GESTURE_WHEEL:
                    VSF_TGUI_LOG(VSF_TRACE_INFO, "\tWheel");
            }

            VSF_TGUI_LOG(  VSF_TRACE_INFO, "\tX = %d\tY = %d in %d ms",
                            event_ptr->delta.use_as__vsf_tgui_location_t.iX,
                            event_ptr->delta.use_as__vsf_tgui_location_t.iY,
                            event_ptr->delta.ms
                        );
        #endif
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
        }

        case VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT & VSF_TGUI_MSG_MSK: {

        #if VSF_TGUI_CFG_SHOW_CONTROL_SPECIFIC_EVT_LOG == ENABLED
            VSF_TGUI_LOG(VSF_TRACE_INFO,
                        "VSF_TGUI_MSG_CONTROL_SPECIFIC_EVT : [0x%04x]\t",
                        ptMSG->use_as__vsf_msgt_msg_t.msg);
        #endif
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
        }

        default:
            //return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            do {
                result = __vk_tgui_control_user_message_handling(control_ptr, (const vsf_tgui_evt_t*)ptMSG);
            } while (fsm_rt_on_going == result);
            break;
    }

    return result;
}


fsm_rt_t vsf_tgui_control_msg_handler(  vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t*)control_ptr,
                                            ptMSG,
                                            &c_tVControl);
}

fsm_rt_t vsf_tgui_container_msg_handler(vsf_tgui_container_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG)
{
    return __vsf_tgui_control_msg_handler(  (vsf_tgui_control_t*)control_ptr,
                                            ptMSG,
                                            &c_tVContainer);
}



/*----------------------------------------------------------------------------*
 *  Update Scheme                                                             *
 *----------------------------------------------------------------------------*/

static void __vsf_tgui_container_update_size(vsf_tgui_container_t *container_ptr,
                                             vsf_tgui_size_t* ptSize)
{
    vsf_tgui_size_t *ptControlSize = NULL;

    VSF_TGUI_ASSERT(NULL != container_ptr && NULL != ptSize);
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    do {
        vsf_tgui_margin_t tMargin = container_ptr->tContainerPadding;
        ptSize->iHeight += tMargin.chTop + tMargin.chBottom;
        ptSize->iWidth += tMargin.chLeft + tMargin.chRight;
    } while(0);
#endif
    ptControlSize = &(container_ptr->tRegion.tSize);

    switch(container_ptr->ContainerAttribute.u5Type) {

#if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER == ENABLED
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            ptControlSize->iHeight = vsf_max(ptSize->iHeight, ptControlSize->iHeight);
            ptControlSize->iWidth = ptSize->iWidth;
            break;

        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
            ptControlSize->iHeight = ptSize->iHeight;
            ptControlSize->iWidth = vsf_max(ptSize->iWidth, ptControlSize->iWidth);
            break;

#endif

        default:
        case VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL:
        case VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL:
        case VSF_TGUI_CONTAINER_TYPE_PLANE:
            ptControlSize->iHeight = vsf_max(ptSize->iHeight, ptControlSize->iHeight);
            ptControlSize->iWidth = vsf_max(ptSize->iWidth, ptControlSize->iWidth);
            break;
    }
}

const vsf_tgui_control_t* __vk_tgui_control_get_next_visible_one_within_container(
                                            const vsf_tgui_control_t* item_ptr)
{
    if (NULL == item_ptr) {
        return NULL;
    }
    do {
        item_ptr = (vsf_tgui_control_t*)
            vsf_msgt_get_next_node_within_container(&(item_ptr->use_as__vsf_msgt_node_t));
        if (NULL == item_ptr) {
            break;
        }

        if (vsf_tgui_control_status_get(item_ptr).Values.bIsVisible) {
            break;
        }
    } while (true);

    return item_ptr;
}


static vsf_tgui_size_t* __vsf_tgui_plane_container_update(
                                                vsf_tgui_container_t* container_ptr,
                                                vsf_tgui_size_t* ptSize)
{
    vsf_tgui_size_t tSize = {0};
    vsf_tgui_control_t* item_ptr =
        (vsf_tgui_control_t*)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;

    while (NULL != item_ptr) {
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

        int16_t iWidth = item_ptr->tRegion.tSize.iWidth + item_ptr->tRegion.tLocation.iX;
        int16_t iHeight = item_ptr->tRegion.tSize.iHeight + item_ptr->tRegion.tLocation.iY;
        tSize.iWidth =  vsf_max(tSize.iWidth, iWidth);
        tSize.iHeight = vsf_max(tSize.iHeight, iHeight);
        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
    }

    *ptSize = tSize;

    return ptSize;
}

#if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER == ENABLED
static vsf_tgui_size_t* __vsf_tgui_line_stream_update_vertical( vsf_tgui_container_t* container_ptr,
                                                                vsf_tgui_size_t* ptSize)
{
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = {0};
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t *item_ptr =
        (vsf_tgui_control_t *)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;

    while(NULL != item_ptr) {
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

        item_ptr->tRegion.tLocation = tLocation;
        iWidth = item_ptr->tRegion.tSize.iWidth;
        iHeight = item_ptr->tRegion.tSize.iHeight;

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        item_ptr->tRegion.tLocation.iX += item_ptr->tMargin.chLeft;
        item_ptr->tRegion.tLocation.iY += item_ptr->tMargin.chTop;
        iWidth += item_ptr->tRegion.tLocation.iX + item_ptr->tMargin.chRight;
        iHeight += item_ptr->tMargin.chTop + item_ptr->tMargin.chBottom;
#endif

        tLocation.iY += iHeight;
        tSize.iHeight += iHeight;
        tSize.iWidth = vsf_max(tSize.iWidth, iWidth);
        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
    }
    *ptSize = tSize;

    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_line_stream_update_horizontal( vsf_tgui_container_t* container_ptr,
                                                                    vsf_tgui_size_t* ptSize)
{
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = { 0 };
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t* item_ptr =
        (vsf_tgui_control_t*)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;

    while (NULL != item_ptr) {
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

        item_ptr->tRegion.tLocation = tLocation;
        iWidth = item_ptr->tRegion.tSize.iWidth;
        iHeight = item_ptr->tRegion.tSize.iHeight;

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        item_ptr->tRegion.tLocation.iX += item_ptr->tMargin.chLeft;
        item_ptr->tRegion.tLocation.iY += item_ptr->tMargin.chTop;
        iWidth += item_ptr->tMargin.chLeft + item_ptr->tMargin.chRight;
        iHeight += item_ptr->tRegion.tLocation.iY + item_ptr->tMargin.chBottom;
#endif

        tLocation.iX += iWidth;
        tSize.iWidth += iWidth;
        tSize.iHeight = vsf_max(tSize.iHeight, iHeight);
        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
    }
    *ptSize = tSize;

    return ptSize;
}
#endif

#if VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER == ENABLED
static vsf_tgui_size_t* __vsf_tgui_stream_update_vertical(  vsf_tgui_container_t* container_ptr,
                                                            vsf_tgui_size_t* ptSize)
{
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = { 0 };
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t* item_ptr =
        (vsf_tgui_control_t*)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;
    int16_t iLineMaxWidth = 0;
    bool is_first_node_in_a_line = true;
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    int16_t iAllowedLineHeight =     container_ptr->iHeight
                                -   container_ptr->tContainerPadding.chTop;
#else
    int16_t iAllowedLineHeight =     container_ptr->iHeight;
#endif
    while (NULL != item_ptr) {
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

        if (iAllowedLineHeight <= 0) {
            //! user doesn't give a reasonable value
            iAllowedLineHeight = item_ptr->iHeight;
        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
            tSize.iHeight = iAllowedLineHeight + item_ptr->tMargin.chTop + item_ptr->tMargin.chBottom;
        #else
            tSize.iHeight = iAllowedLineHeight;
        #endif
        }

        //! place the control
        item_ptr->tRegion.tLocation = tLocation;
        is_first_node_in_a_line = false;

        //! get control size
        iWidth = item_ptr->tRegion.tSize.iWidth;
        iHeight = item_ptr->tRegion.tSize.iHeight;

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        item_ptr->tRegion.tLocation.iX += item_ptr->tMargin.chLeft;
        item_ptr->tRegion.tLocation.iY += item_ptr->tMargin.chTop;
        iWidth += item_ptr->tMargin.chLeft + item_ptr->tMargin.chRight;
        iHeight += item_ptr->tMargin.chTop + item_ptr->tMargin.chBottom;
#endif
        //! update the max height in a line
        iLineMaxWidth = vsf_max(iWidth, iLineMaxWidth);
        //! update iY in the same line
        tLocation.iY += iHeight;

        //! DO **NOT** calculate the minimal required height
        // tSize.iHeight = vsf_max(tLocation.iY, tSize.iHeight);

        if (tLocation.iY >= iAllowedLineHeight) {
            //! already hit the wall, we should start a new line
            tLocation.iX += iLineMaxWidth;
            tLocation.iY = 0;
            tSize.iWidth += iLineMaxWidth;    // update total required width
            iLineMaxWidth = 0;
            is_first_node_in_a_line = true;
        }

        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
        if (NULL == item_ptr){
            if (!is_first_node_in_a_line) {
                tSize.iWidth += iLineMaxWidth;    // update total required width
            }
            break;
        }

        if (((tLocation.iY + item_ptr->iHeight
        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
             + item_ptr->tMargin.chTop
        #endif
            ) > iAllowedLineHeight) && (!is_first_node_in_a_line)) {
            //! the left room is insufficient
            tLocation.iX += iLineMaxWidth;
            tLocation.iY = 0;
            tSize.iWidth += iLineMaxWidth;    // update total required width
            iLineMaxWidth = 0;
        }
    }
    *ptSize = tSize;
    return ptSize;
}

static vsf_tgui_size_t* __vsf_tgui_stream_update_horizontal(vsf_tgui_container_t* container_ptr,
                                                            vsf_tgui_size_t* ptSize)
{
    vsf_tgui_location_t tLocation = { 0 };
    vsf_tgui_size_t tSize = { 0 };
    int16_t iHeight = 0, iWidth = 0;
    vsf_tgui_control_t* item_ptr =
        (vsf_tgui_control_t*)container_ptr->use_as__vsf_msgt_container_t.node_ptr;
    container_ptr->chVisibleItemCount = 0;
    int16_t iLineMaxHeight = 0;
    bool is_first_node_in_a_line = true;
#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_PADDING == ENABLED
    int16_t iAllowedLineWidth =     container_ptr->iWidth
                                -   container_ptr->tContainerPadding.chLeft;
#else
    int16_t iAllowedLineWidth =     container_ptr->iWidth;
#endif
    while (NULL != item_ptr) {
        VSF_TGUI_ASSERT(container_ptr->chVisibleItemCount < 255);
        container_ptr->chVisibleItemCount++;

        if (iAllowedLineWidth <= 0) {
            //! user doesn't give a reasonable value
            iAllowedLineWidth = item_ptr->iWidth;
        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
            tSize.iWidth = iAllowedLineWidth + item_ptr->tMargin.chLeft + item_ptr->tMargin.chRight;
        #else
            tSize.iWidth = iAllowedLineWidth;
        #endif
        }

        //! place the control
        item_ptr->tRegion.tLocation = tLocation;
        is_first_node_in_a_line = false;

        //! get control size
        iWidth = item_ptr->tRegion.tSize.iWidth;
        iHeight = item_ptr->tRegion.tSize.iHeight;

#if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
        item_ptr->tRegion.tLocation.iX += item_ptr->tMargin.chLeft;
        item_ptr->tRegion.tLocation.iY += item_ptr->tMargin.chTop;
        iWidth += item_ptr->tMargin.chLeft + item_ptr->tMargin.chRight;
        iHeight += item_ptr->tMargin.chTop + item_ptr->tMargin.chBottom;
#endif
        //! update the max height in a line
        iLineMaxHeight = vsf_max(iHeight, iLineMaxHeight);
        //! update iX in the same line
        tLocation.iX += iWidth;

        //! DO **NOT** calculate the minimal required width
        // tSize.iWidth = vsf_max(tLocation.iX, tSize.iWidth);

        if (tLocation.iX >= iAllowedLineWidth) {
            //! hit the wall, we should start a new line
            tLocation.iY += iLineMaxHeight;
            tLocation.iX = 0;
            tSize.iHeight += iLineMaxHeight;    // update total required height
            iLineMaxHeight = 0;
            is_first_node_in_a_line = true;
        }

        item_ptr = (vsf_tgui_control_t *)__vk_tgui_control_get_next_visible_one_within_container(item_ptr);
        if (NULL == item_ptr){
            if (!is_first_node_in_a_line) {
                tSize.iHeight += iLineMaxHeight;
            }
            break;
        }
        if (((tLocation.iX + item_ptr->iWidth
        #if VSF_TGUI_CFG_SUPPORT_CONTROL_LAYOUT_MARGIN == ENABLED
             + item_ptr->tMargin.chLeft
        #endif
            ) > iAllowedLineWidth) && (!is_first_node_in_a_line)) {
            //! hit the wall, we should start a new line
            tLocation.iY += iLineMaxHeight;
            tLocation.iX = 0;
            tSize.iHeight += iLineMaxHeight;    // update total required height
            iLineMaxHeight = 0;
        }
    }
    *ptSize = tSize;

    return ptSize;
}
#endif

static fsm_rt_t __vsf_tgui_container_update(vsf_tgui_container_t* container_ptr)
{

    vsf_tgui_size_t tSize;
    VSF_TGUI_ASSERT(NULL != container_ptr);

    switch(container_ptr->ContainerAttribute.u5Type) {

    #if VSF_TGUI_CFG_SUPPORT_STREAM_CONTAINER == ENABLED
        case VSF_TGUI_CONTAINER_TYPE_STREAM_HORIZONTAL:
            /* stream container, with horizontal orientation*/
            /* horizontal orientation: the width is fixed, there could be multiple rows*/
            __vsf_tgui_stream_update_horizontal(container_ptr, &tSize);
            break;

        case VSF_TGUI_CONTAINER_TYPE_STREAM_VERTICAL:
            /* stream container, with orientation either vertical or horizontal */
            /* vertical orientation: the height is fixed, there could be multiple columns*/
            __vsf_tgui_stream_update_vertical(container_ptr, &tSize);
            break;
    #endif
    #if VSF_TGUI_CFG_SUPPORT_LINE_STREAM_CONTAINER == ENABLED
        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_HORIZONTAL:
            /* line stream container, with orientation either vertical or horizontal */
            /* horizontal orientation: only one row*/
            __vsf_tgui_line_stream_update_horizontal(container_ptr, &tSize);
            break;

        case VSF_TGUI_CONTAINER_TYPE_LINE_STREAM_VERTICAL:
            /* line stream container, with orientation either vertical or horizontal */
            /* horizontal orientation: only one row*/
            __vsf_tgui_line_stream_update_vertical(container_ptr, &tSize);
            break;
    #endif

        default:
            /* unknown stream type is treated as plan stream*/
        case VSF_TGUI_CONTAINER_TYPE_PLANE:
            /* controls are placed on a plane with their relative locations */
            if (container_ptr->ContainerAttribute.bIsAutoSize) {
                __vsf_tgui_plane_container_update(container_ptr, &tSize);
            }
            break;
    }

    if (container_ptr->ContainerAttribute.bIsAutoSize) {
        __vsf_tgui_container_update_size(container_ptr, &tSize);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_control_update(vsf_tgui_control_t *control_ptr)
{
    VSF_TGUI_ASSERT(NULL != control_ptr);
    if (vsf_tgui_control_is_container((const vsf_tgui_control_t *)control_ptr)) {
        return __vsf_tgui_container_update((vsf_tgui_container_t *)control_ptr);
    }

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_container_update(vsf_tgui_container_t* container_ptr)
{
    return vk_tgui_control_update((vsf_tgui_control_t *) container_ptr);
}

fsm_rt_t vk_tgui_control_init(vsf_tgui_control_t* control_ptr)
{
    vsf_tgui_status_t Status = vsf_tgui_control_status_get((vsf_tgui_control_t*)control_ptr);
    Status.chStatus |= VSF_TGUI_CTRL_STATUS_INITIALISED;
    Status.Values.__does_contain_builtin_structure = false;

    vsf_tgui_control_status_set((vsf_tgui_control_t*)control_ptr, Status);

    return fsm_rt_cpl;
}

fsm_rt_t vk_tgui_container_init(vsf_tgui_container_t *container_ptr)
{
    return vk_tgui_control_init((vsf_tgui_control_t *)container_ptr);
}

#endif


/* EOF */
