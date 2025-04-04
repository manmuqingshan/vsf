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
#include "osa_service/vsf_osa_service_cfg.h"

#if VSF_USE_MSG_TREE == ENABLED

#define __VSF_MSG_TREE_CLASS_IMPLEMENT
#include "vsf_msg_tree.h"
/*============================ MACROS ========================================*/

#undef vsf_this
#define vsf_this                (*this_ptr)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

implement_vsf_rng_buf(__bfs_node_fifo_t, uint16_t, NO_RNG_BUF_PROTECT)

void vsf_msgt_init(vsf_msgt_t* obj_ptr, const vsf_msgt_cfg_t* cfg_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr && NULL != cfg_ptr);
    VSF_OSA_SERVICE_ASSERT(     (NULL != cfg_ptr->interface_ptr)
                            ||  (0 != cfg_ptr->type_num));

    memset(obj_ptr, 0, sizeof(*obj_ptr));
    vsf_this.NodeTypes = *cfg_ptr;
}

const vsf_msgt_node_t* vsf_msgt_get_next_node_within_container(const vsf_msgt_node_t* node_ptr)
{
    do {
        if (NULL == node_ptr) {
            break;
        }
        //! check offset of next node
        if (0 == node_ptr->Offset.next) {
            //!< the last node
            node_ptr = NULL;
            break;
        }
        //! point to next node with offset
        node_ptr = (vsf_msgt_node_t*)((intptr_t)node_ptr + (intptr_t)(node_ptr->Offset.next));
    } while(0);
    return node_ptr;
}

static bool __msgt_check_status(vsf_msgt_t* obj_ptr,
                                const vsf_msgt_node_t* node_ptr,
                                uint_fast8_t status_msk )
{
    vsf_msgt_method_status_t* status_fn = NULL;
    uint_fast8_t id = node_ptr->id;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    status_fn = vsf_this.NodeTypes.interface_ptr[id].Status;

    if (NULL != status_fn) {
        //!< get status
        vsf_msgt_node_status_t Status = (*status_fn)((vsf_msgt_node_t*)node_ptr);
        if ((Status & status_msk) != status_msk) {
            return false;
        }
    }

    return true;
}
/*----------------------------------------------------------------------------*
 * Node shooting algorithm                                                    *
 *----------------------------------------------------------------------------*/

static bool __msgt_shoot(   vsf_msgt_t* obj_ptr,
                            const vsf_msgt_node_t *node_ptr,
                            uintptr_t bullet_info_ptr)
{
    uint_fast8_t id = node_ptr->id;
    bool result = false;
    vsf_msgt_method_shoot_t* shoot_fn = NULL;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    shoot_fn = vsf_this.NodeTypes.interface_ptr[id].Shoot;

    if (NULL != shoot_fn) {
        result = shoot_fn(node_ptr, bullet_info_ptr);
    }

    return result;
}

const vsf_msgt_node_t * vsf_msgt_shoot_top_node(  vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *root_ptr,
                                            uintptr_t bullet_info_ptr)
{
    const vsf_msgt_node_t* item_ptr = NULL;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);

    do {
        if (NULL == root_ptr) {
            break;
        }

        if (!__msgt_check_status(   obj_ptr,
                                    root_ptr,
                                    VSF_MSGT_NODE_VALID)) {
            break;
        }

        //! shoot the root item
        if (__msgt_shoot(obj_ptr, root_ptr, bullet_info_ptr)) {
            item_ptr = (vsf_msgt_node_t *)root_ptr;
        } else if (root_ptr->Attribute._.is_container){
            if (!root_ptr->Attribute._.is_transparent) {
                //! not in the range
                break;
            }
            //! transparent container
        } else {
            //! not in the range
            break;
        }

        //! check nodes in this container
        if (!__msgt_check_status(   obj_ptr,
                                    item_ptr,
                                    VSF_MSGT_NODE_HIDE_CONTENT)) {
            vsf_msgt_container_t* container_ptr = (vsf_msgt_container_t*)item_ptr;
            const vsf_msgt_node_t * node_ptr = container_ptr->node_ptr;

            while (NULL != node_ptr) {
                if (__msgt_shoot(obj_ptr, node_ptr, bullet_info_ptr)) {
                    //! shoot a node
                    item_ptr = node_ptr;
                    break;
                }

                node_ptr = vsf_msgt_get_next_node_within_container(node_ptr);
            }

        } while (0);

    } while(0);
    return item_ptr;
}


const vsf_msgt_node_t * vsf_msgt_shoot_node(  vsf_msgt_t* obj_ptr,
                                        const vsf_msgt_node_t *root_ptr,
                                        uintptr_t bullet_info_ptr)
{
    const vsf_msgt_node_t* item_ptr = NULL;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);


    do {
        const vsf_msgt_node_t* node_ptr = root_ptr;

        do {
            node_ptr = vsf_msgt_shoot_top_node(obj_ptr, node_ptr, bullet_info_ptr);
            if (NULL == node_ptr) {
                break;
            }

            if (item_ptr == node_ptr) {
                //! the same container (we only hit a container)
                break;
            }
            item_ptr = node_ptr;
            if (!item_ptr->Attribute._.is_container) {
                //!< the leaf node
                break;
            } /* else; find a container, check the contained nodes*/

        } while(true);

    } while (0);

    return item_ptr;
}

/*----------------------------------------------------------------------------*
 * Message Handing                                                            *
 *----------------------------------------------------------------------------*/

static void __vsf_msg_handling_init(   __vsf_msgt_msg_handling_fsm_t* fsm_ptr,
                                vsf_msgt_t* obj_ptr,
                                vsf_msgt_msg_t* msg_ptr,
                                uint_fast8_t status_msk)
{
    fsm_ptr->state = 0;
    fsm_ptr->msg_ptr = msg_ptr;
    fsm_ptr->status_msk = status_msk;
}

#undef THIS_FSM_STATE
#define THIS_FSM_STATE  fsm_ptr->state

#define RESET_MSGT_HANDLING_FSM()         \
        do { THIS_FSM_STATE = 0; } while(0)


fsm_rt_t __vsf_msg_handling(__vsf_msgt_msg_handling_fsm_t *fsm_ptr,
                            vsf_msgt_t* obj_ptr,
                            const vsf_msgt_node_t* node_ptr)
{
    enum {
        START = 0,
        CHECK_STATUS,
        GET_HANDLER,
        HANDLE_FSM,
#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        HANDLE_SUBCALL,
#endif
    };
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    switch (THIS_FSM_STATE) {
        case START:
            if (0 == fsm_ptr->status_msk) {
                THIS_FSM_STATE = GET_HANDLER;
                break;
            } else {
                THIS_FSM_STATE ++;
            }
            //break;
        case CHECK_STATUS:
            if (node_ptr->id >= vsf_this.NodeTypes.type_num) {
                //! illegal Node ID detected, ignore this
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_err;
            } else {
                if (!__msgt_check_status(   obj_ptr,
                                            node_ptr,
                                            //(VSF_MSGT_NODE_VALID | VSF_MSGT_NODE_ENABLED)
                                            fsm_ptr->status_msk
                                            )) {
                    //! not valid or enabled
                    RESET_MSGT_HANDLING_FSM();
                    return fsm_rt_err;
                }
                THIS_FSM_STATE = GET_HANDLER;
            }
            //break;
        case GET_HANDLER: {
            const vsf_msgt_handler_t* handler_fn = NULL;
            uint_fast8_t id = node_ptr->id;

            handler_fn = &vsf_this.NodeTypes.interface_ptr[id].msg_handler;

            if (NULL == handler_fn->fn.fsm_fn) {
                //! empty handler
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_err;
            }
            fsm_ptr->handler_fn = handler_fn;

            switch (handler_fn->u2_type) {
                case VSF_MSGT_NODE_HANDLER_TYPE_FSM:
                    THIS_FSM_STATE = HANDLE_FSM;
                    break;
    #if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
                case VSF_MSGT_NODE_HANDLER_TYPE_SUBCALL:
                    if (NULL == vsf_eda_get_cur()) {
                        //! it's not possible to call sub eda
                        RESET_MSGT_HANDLING_FSM();
                        return (fsm_rt_t)VSF_ERR_NOT_SUPPORT;
                    }
                    THIS_FSM_STATE = HANDLE_SUBCALL;
                    break;
    #endif
                case VSF_MSGT_NODE_HANDLER_TYPE_EDA: {
                    vsf_err_t tErr = vsf_eda_post_msg(handler_fn->fn.eda_ptr, fsm_ptr->msg_ptr);
                    VSF_OSA_SERVICE_ASSERT(tErr == VSF_ERR_NONE);
                    VSF_UNUSED_PARAM(tErr);
                    RESET_MSGT_HANDLING_FSM();
                    return fsm_rt_cpl;
                }
                default:
                    //!< unknown type
                    RESET_MSGT_HANDLING_FSM();
                    return fsm_rt_err;
            }
            break;
        }

        case HANDLE_FSM: {
            fsm_rt_t fsm_rt = fsm_ptr->handler_fn->fn.fsm_fn((vsf_msgt_node_t *)node_ptr, fsm_ptr->msg_ptr);
            if (fsm_rt_cpl == fsm_rt) {
                //! message has been handled
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_cpl;
            } else if (fsm_rt < 0) {
                //! message is not handled
                RESET_MSGT_HANDLING_FSM();
                return fsm_rt_err;
            } else {
                return fsm_rt;
            }
            break;
        }

#if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        case HANDLE_SUBCALL:
            //! todo add support for subcall
            RESET_MSGT_HANDLING_FSM();
            return fsm_rt_err;
#endif
    }

    return fsm_rt_on_going;
}


/*----------------------------------------------------------------------------*
 * Backward Message Propagation                                               *
 *----------------------------------------------------------------------------*/
#undef THIS_FSM_STATE
#define THIS_FSM_STATE  this_ptr->BW.state

#define RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM()                                 \
        do { THIS_FSM_STATE = 0; } while(0)

fsm_rt_t vsf_msgt_backward_propagate_msg(   vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t *node_ptr,
                                            vsf_msgt_msg_t *msg_ptr)
{
    enum {
        START = 0,
        MSG_HANDLING,
        GET_PARENT,
    };
    fsm_rt_t fsm_rt = fsm_rt_on_going;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);


    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != msg_ptr);

    switch (THIS_FSM_STATE) {
        case START:
            if (NULL == node_ptr) {
                return (fsm_rt_t)VSF_ERR_INVALID_PTR;
            }
            vsf_this.BW.msg_handling.node_ptr = node_ptr;
            if (vsf_this.NodeTypes.type_num > 0 && NULL == vsf_this.NodeTypes.interface_ptr) {
                return (fsm_rt_t)VSF_ERR_INVALID_PARAMETER;
            }
            __vsf_msg_handling_init(&vsf_this.BW.msg_handling,
                                    obj_ptr,
                                    msg_ptr,
                                    (VSF_MSGT_NODE_VALID | VSF_MSGT_NODE_ENABLED));
            THIS_FSM_STATE++;
            //break;

        case MSG_HANDLING:
            fsm_rt = __vsf_msg_handling(&vsf_this.BW.msg_handling, obj_ptr, vsf_this.BW.msg_handling.node_ptr);
            if (fsm_rt < 0) {
                THIS_FSM_STATE = GET_PARENT;
                break;
            } else if (fsm_rt_cpl == fsm_rt) {
                RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM();
                //return fsm_rt_cpl;
            } else if (fsm_rt >= fsm_rt_user) {
                RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM();
            }
            return fsm_rt;

        case GET_PARENT:
            //! backward propagate
            if (NULL == vsf_this.BW.msg_handling.node_ptr->parent_ptr) {
                //! no parent
                RESET_MSGT_BACKWARD_PROPAGATE_MSG_FSM();
                return (fsm_rt_t)VSF_MSGT_ERR_MSG_NOT_HANDLED;
            }
            vsf_this.BW.msg_handling.node_ptr =
                (const vsf_msgt_node_t *)vsf_this.BW.msg_handling.node_ptr->parent_ptr;
            THIS_FSM_STATE = MSG_HANDLING;
            break;
    }


    return fsm_rt_on_going;
}

const vsf_msgt_node_t *vsf_msgt_backward_propagate_msg_get_last_node(
                                                            vsf_msgt_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);

    return vsf_this.BW.msg_handling.node_ptr;
}

/*----------------------------------------------------------------------------*
 * Forward Message Propagation using Pre-Order Traversal Algorithm             *
 *----------------------------------------------------------------------------*/
VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree"
        ".vsf_msgt_forward_propagate_msg_pre_order_traversal_init")
void vsf_msgt_forward_propagate_msg_pre_order_traversal_init(vsf_msgt_t* obj_ptr,
                                             bool is_support_container_post_handling)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);

    vsf_this.FWPOT.state = 0;
    vsf_this.FWPOT.is_support_container_post_handling = is_support_container_post_handling;
}

VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree"
        ".vsf_msgt_forward_propagate_msg_pre_order_traversal_setting")
void vsf_msgt_forward_propagate_msg_pre_order_traversal_setting(vsf_msgt_t* obj_ptr,
                                                bool is_support_container_post_handling)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    vsf_this.FWPOT.is_support_container_post_handling = is_support_container_post_handling;
}

#undef THIS_FSM_STATE
#define THIS_FSM_STATE  vsf_this.FWPOT.state

#define RESET_MSGT_FW_POT_PROPAGATE_MSG_FSM()                                   \
        do { THIS_FSM_STATE = 0; } while(0)

VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree"
        ".vsf_msgt_forward_propagate_msg_pre_order_traversal")
fsm_rt_t vsf_msgt_forward_propagate_msg_pre_order_traversal(
                                            vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* root_ptr,
                                            vsf_msgt_msg_t* msg_ptr,
                                            uint_fast8_t status_msk)
{
    enum {
        START = 0,
        VISIT_ITEM,
        FETCH_NEXT_ITEM,
    };

    fsm_rt_t fsm_rt = fsm_rt_on_going;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != msg_ptr);


    switch (THIS_FSM_STATE) {
        case START:
            if (NULL == root_ptr) {
                return (fsm_rt_t)VSF_ERR_INVALID_PTR;
            }
            if (    (vsf_this.NodeTypes.type_num > 0)
                &&  (NULL == vsf_this.NodeTypes.interface_ptr)) {
                return (fsm_rt_t)VSF_ERR_INVALID_PARAMETER;
            }
            vsf_this.FWPOT.msg_handling.node_ptr = root_ptr;
            ((vsf_msgt_node_t* )root_ptr)->Attribute._.is_visited = false;
            __vsf_msg_handling_init(&vsf_this.FWPOT.msg_handling,
                                    obj_ptr,
                                    msg_ptr,
                                    status_msk);

            THIS_FSM_STATE++;
            // break;

        case VISIT_ITEM:
            fsm_rt = __vsf_msg_handling(  &vsf_this.FWPOT.msg_handling,
                                        obj_ptr,
                                        vsf_this.FWPOT.msg_handling.node_ptr);
            if (    fsm_rt < 0
                ||  fsm_rt_cpl == fsm_rt) {

                THIS_FSM_STATE = FETCH_NEXT_ITEM;
                // fall through
            } else {
                return fsm_rt;
            }

        case FETCH_NEXT_ITEM: {
            const vsf_msgt_node_t* item_ptr = vsf_this.FWPOT.msg_handling.node_ptr;
            const vsf_msgt_node_t* temp_ptr = NULL;
            const vsf_msgt_container_t* container_ptr =
                (const vsf_msgt_container_t *)item_ptr;

            if (false == (((vsf_msgt_node_t* )item_ptr)->Attribute._.is_visited)) {
                //! a normal node is visited
                ((vsf_msgt_node_t* )item_ptr)->Attribute._.is_visited = true;
                if (item_ptr->Attribute._.is_container && fsm_rt == fsm_rt_cpl) {
                    temp_ptr = container_ptr->node_ptr;
                    if (NULL == temp_ptr && vsf_this.FWPOT.is_support_container_post_handling) {
                        //! a corner case for container post handling
                        vsf_this.FWPOT.msg_handling.node_ptr = (const vsf_msgt_node_t *)item_ptr;
                        THIS_FSM_STATE = VISIT_ITEM;
                        break;
                    }
                } else if (item_ptr == root_ptr) {
                    RESET_MSGT_FW_POT_PROPAGATE_MSG_FSM();
                    return fsm_rt_cpl;
                }
            } else if (item_ptr == root_ptr) {
                RESET_MSGT_FW_POT_PROPAGATE_MSG_FSM();
                return fsm_rt_cpl;
            }

            while(NULL == temp_ptr) {
                temp_ptr = vsf_msgt_get_next_node_within_container(item_ptr);
                if (NULL != temp_ptr) {
                    break;
                }
                //! it is the last item in the container, return to previous level
                item_ptr = (const vsf_msgt_node_t*)item_ptr->parent_ptr;

                if (NULL == item_ptr) {
                    //! it is the top container / root container, cpl
                    RESET_MSGT_FW_POT_PROPAGATE_MSG_FSM();
                    return fsm_rt_cpl;
                }

                if (vsf_this.FWPOT.is_support_container_post_handling) {
                    //! container post handling
                    break;
                }
            }

            if (NULL != temp_ptr) {
                vsf_this.FWPOT.msg_handling.node_ptr = (const vsf_msgt_node_t *)temp_ptr;
                ((vsf_msgt_node_t* )temp_ptr)->Attribute._.is_visited = false;
            } else {
                //! container post handling
                vsf_this.FWPOT.msg_handling.node_ptr = (const vsf_msgt_node_t *)item_ptr;
            }

            THIS_FSM_STATE = VISIT_ITEM;

            break;
        }
    }

    return fsm_rt_on_going;
}


VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_offset_tree_init")
void vsf_msgt_offset_tree_init(const vsf_msgt_container_t * root_ptr)
{
    //bool ignore_item = false;
    if (NULL == root_ptr) {
        return ;
    }

    vsf_msgt_container_t *node_ptr = (vsf_msgt_container_t *)root_ptr;
    vsf_msgt_container_t *parent_ptr = node_ptr;

    while(NULL != node_ptr) {

        //! go deep
        do {
            if (!node_ptr->Attribute._.is_container) {
                break;
            } else if (NULL == ((vsf_msgt_container_t *)node_ptr)->node_ptr) {
                //! this is an empty container
                break;
            } else {
                //! visit node
                if (root_ptr != (vsf_msgt_container_t *)node_ptr) {
                    //! update parent
                    node_ptr->parent_ptr = parent_ptr;
                }

                parent_ptr = node_ptr;
                //! update node list pointer
                (*(intptr_t *)&(((vsf_msgt_container_t *)node_ptr)->node_ptr)) += (intptr_t)root_ptr;
                node_ptr = (vsf_msgt_container_t *)(((vsf_msgt_container_t *)node_ptr)->node_ptr);
            }
        } while(true);


        //! visit node
        if (root_ptr != node_ptr) {
            //! update parent
            node_ptr->parent_ptr = parent_ptr;
        }


visit_node:
        do {

            const vsf_msgt_node_t* temp_ptr = NULL;
            if (node_ptr == root_ptr) {
                //! visited the entry node
                return ;
            }
            do {
                temp_ptr = vsf_msgt_get_next_node_within_container((vsf_msgt_node_t *)node_ptr);
                if (NULL != temp_ptr) {
                    node_ptr = (vsf_msgt_container_t *)temp_ptr;
                    break;
                }
                //! it is the last item in the container
                node_ptr = (vsf_msgt_container_t *)node_ptr->parent_ptr;

                if (NULL == node_ptr) {
                    //! it is the top container
                    return ;
                } /*else if (node_ptr == root_ptr) {
                    //! it is the last item
                    THIS_FSM_STATE = VISIT_ITEM;
                    break;
                } */
                parent_ptr = node_ptr->parent_ptr;

                goto visit_node;
            } while(0);

        } while(0);
    }
}


/*----------------------------------------------------------------------------*
 * Forward Message Propagation using DFS algorithm                            *
 *----------------------------------------------------------------------------*/

VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_dfs")
void vsf_msgt_forward_propagate_msg_dfs_init(vsf_msgt_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);

    vsf_this.FWDFS.state = 0;
}

#undef THIS_FSM_STATE
#define THIS_FSM_STATE  vsf_this.FWDFS.state

#define RESET_MSGT_FW_DFS_PROPAGATE_MSG_FSM()                                   \
        do { THIS_FSM_STATE = 0; } while(0)

VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_dfs")
fsm_rt_t vsf_msgt_forward_propagate_msg_dfs(vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* root_ptr,
                                            vsf_msgt_msg_t* msg_ptr)
{
    enum {
        START = 0,
        FETCH_DEEPEST_ITEM,
        VISIT_ITEM,
        FETCH_NEXT_ITEM,
        VISIT_THE_ENTRY_NODE,
    };
    fsm_rt_t fsm_rt = fsm_rt_on_going;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != msg_ptr);

    switch (THIS_FSM_STATE) {
        case START:
            if (NULL == root_ptr) {
                return (fsm_rt_t)VSF_ERR_INVALID_PTR;
            }
            if (    (vsf_this.NodeTypes.type_num > 0)
                &&  (NULL == vsf_this.NodeTypes.interface_ptr)) {
                return (fsm_rt_t)VSF_ERR_INVALID_PARAMETER;
            }
            vsf_this.FWDFS.msg_handling.node_ptr = root_ptr;

            __vsf_msg_handling_init(&vsf_this.FWDFS.msg_handling,
                                    obj_ptr,
                                    msg_ptr,
                                    (VSF_MSGT_NODE_VALID | VSF_MSGT_NODE_ENABLED));

            THIS_FSM_STATE++;
            // break;

        case FETCH_DEEPEST_ITEM: {
            const vsf_msgt_container_t* item_ptr =
                (const vsf_msgt_container_t *)vsf_this.FWDFS.msg_handling.node_ptr;

            do {
                if (!item_ptr->use_as__vsf_msgt_node_t.Attribute._.is_container) {
                    break;
                } else if (NULL == item_ptr->node_ptr) {
                    //! this is an empty container
                    break;
                } else {
                    item_ptr = (const vsf_msgt_container_t*)item_ptr->node_ptr;
                }
            } while(true);

            vsf_this.FWDFS.msg_handling.node_ptr = (const vsf_msgt_node_t *)item_ptr;
            THIS_FSM_STATE = VISIT_ITEM;
            //break;
        }

        case VISIT_ITEM:
            fsm_rt = __vsf_msg_handling(  &vsf_this.FWDFS.msg_handling,
                                        obj_ptr,
                                        vsf_this.FWDFS.msg_handling.node_ptr);
            if (    fsm_rt < 0
                ||  fsm_rt_cpl == fsm_rt) {
                THIS_FSM_STATE = FETCH_NEXT_ITEM;
                break;
            }
            return fsm_rt;

            //break;

        case FETCH_NEXT_ITEM: {
                const vsf_msgt_node_t* item_ptr = vsf_this.FWDFS.msg_handling.node_ptr;
                const vsf_msgt_node_t* temp_ptr = NULL;
                if (item_ptr == root_ptr) {
                    //! visited the entry node
                    RESET_MSGT_FW_DFS_PROPAGATE_MSG_FSM();
                    return fsm_rt_cpl;
                }
                do {
                    temp_ptr = vsf_msgt_get_next_node_within_container(item_ptr);
                    if (NULL != temp_ptr) {
                        item_ptr = temp_ptr;
                        THIS_FSM_STATE = FETCH_DEEPEST_ITEM;
                        break;
                    }
                    //! it is the last item in the container
                    item_ptr = (const vsf_msgt_node_t*)item_ptr->parent_ptr;

                    if (NULL == item_ptr) {
                        //! it is the top container
                        RESET_MSGT_FW_DFS_PROPAGATE_MSG_FSM();
                        return fsm_rt_cpl;
                    } /*else if (item_ptr == root_ptr) {
                        //! it is the last item
                        THIS_FSM_STATE = VISIT_ITEM;
                        break;
                    } */
                    THIS_FSM_STATE = VISIT_ITEM;
                } while(0);

                vsf_this.FWDFS.msg_handling.node_ptr = item_ptr;
                break;
            }
    }

    return fsm_rt_on_going;
}

/*----------------------------------------------------------------------------*
 * Forward Message Propagation using BFS algorithm                            *
 *----------------------------------------------------------------------------*/

VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
void vsf_msgt_forward_propagate_msg_bfs_init(   vsf_msgt_t* obj_ptr,
                                                uint16_t *fifo_buffer_ptr,
                                                uint_fast16_t buff_size,
                                                bool is_support_container_post_handling)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != fifo_buffer_ptr);
    VSF_OSA_SERVICE_ASSERT(buff_size > 2);

    vsf_this.FWBFS.state = 0;
    vsf_this.FWBFS.is_support_container_post_handling = is_support_container_post_handling;
    vsf_rng_buf_prepare(__bfs_node_fifo_t,
                        &(vsf_this.FWBFS.fifo),
                        fifo_buffer_ptr,
                        buff_size);
}

VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs_setting")
void vsf_msgt_forward_propagate_msg_bfs_setting(vsf_msgt_t* obj_ptr,
                                                bool is_support_container_post_handling)
{
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);
    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    vsf_this.FWBFS.is_support_container_post_handling = is_support_container_post_handling;
}

#undef THIS_FSM_STATE
#define THIS_FSM_STATE  vsf_this.FWBFS.state

#define RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM()                                   \
        do { THIS_FSM_STATE = 0; } while(0)

VSF_CAL_SECTION(".text.vsf.osa_service.msg_tree.vsf_msgt_forward_propagate_msg_bfs")
fsm_rt_t vsf_msgt_forward_propagate_msg_bfs(vsf_msgt_t* obj_ptr,
                                            const vsf_msgt_node_t* node_ptr,
                                            vsf_msgt_msg_t* msg_ptr,
                                            uint_fast8_t status_msk)
{
    enum {
        START = 0,
        FETCH_ITEM,
        VISIT_ITEM,
    };
    fsm_rt_t fsm_rt = fsm_rt_on_going;
    uint16_t hwOffset = 0;
    class_internal(obj_ptr, this_ptr, vsf_msgt_t);

    VSF_OSA_SERVICE_ASSERT(NULL != obj_ptr);
    VSF_OSA_SERVICE_ASSERT(NULL != msg_ptr);

    switch (THIS_FSM_STATE) {
        case START:
            if (NULL == node_ptr) {
                return (fsm_rt_t)VSF_ERR_INVALID_PTR;
            }
            if (    (vsf_this.NodeTypes.type_num > 0)
                &&  (NULL == vsf_this.NodeTypes.interface_ptr)) {
                return (fsm_rt_t)VSF_ERR_INVALID_PARAMETER;
            }
            VSF_OSA_SERVICE_ASSERT (NULL != vsf_this.FWBFS.fifo.buffer_ptr);

            ((vsf_msgt_node_t* )node_ptr)->Attribute._.is_visited = false;

            if (!vsf_rng_buf_send_one(  __bfs_node_fifo_t,
                                        &(vsf_this.FWBFS.fifo),
                                        0)) {
                return (fsm_rt_t)VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
            }

            __vsf_msg_handling_init(&vsf_this.FWBFS.msg_handling,
                                    obj_ptr,
                                    msg_ptr,
                                    status_msk);
            THIS_FSM_STATE ++;
            // break;

        case FETCH_ITEM:
            if (!vsf_rng_buf_get_one(   __bfs_node_fifo_t,
                                        &(vsf_this.FWBFS.fifo),
                                        &hwOffset)) {
                //! search complete
                RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM();
                return fsm_rt_cpl;
            }
            //! calculate target item address
            vsf_this.FWBFS.msg_handling.node_ptr =
                (vsf_msgt_node_t *)((uintptr_t)node_ptr + hwOffset);
            THIS_FSM_STATE = VISIT_ITEM;
            //break;

        case VISIT_ITEM:
            fsm_rt = __vsf_msg_handling(  &vsf_this.FWBFS.msg_handling,
                                        obj_ptr,
                                        vsf_this.FWBFS.msg_handling.node_ptr);
            if (    fsm_rt < 0
                ||  fsm_rt_cpl == fsm_rt) {
                THIS_FSM_STATE = FETCH_ITEM;

                if (vsf_this.FWBFS.is_support_container_post_handling) {
                    //! check whether the node is visited or not
                    if (vsf_this.FWBFS.msg_handling.node_ptr->Attribute._.is_visited) {
                        break;
                    }

                    //! mark node as visited
                    ((vsf_msgt_node_t *)(vsf_this.FWBFS.msg_handling.node_ptr))->Attribute._.is_visited = true;
                }

                if (    vsf_this.FWBFS.msg_handling.node_ptr->Attribute._.is_container
                    &&  fsm_rt == fsm_rt_cpl) {
                    //! add nodes to fifo
                    vsf_msgt_container_t* container_ptr =
                        (vsf_msgt_container_t*)vsf_this.FWBFS.msg_handling.node_ptr;
                    vsf_msgt_node_t *temp_ptr =
                        (vsf_msgt_node_t *)container_ptr->node_ptr;

                    //! put all nodes into fifo
                    while(NULL != temp_ptr) {
                        hwOffset =
                            (uint16_t)((uintptr_t)temp_ptr
                                     - (uintptr_t)node_ptr /*vsf_this.FWBFS.msg_handling.node_ptr*/);

                        //! mark child node as unvisited
                        temp_ptr->Attribute._.is_visited = false;
                        if (!vsf_rng_buf_send_one(  __bfs_node_fifo_t,
                                                    &(vsf_this.FWBFS.fifo),
                                                    hwOffset)) {
                            RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM();
                            return (fsm_rt_t)
                                VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
                        }
                        temp_ptr = (vsf_msgt_node_t *)vsf_msgt_get_next_node_within_container(temp_ptr);
                    }

                    if (vsf_this.FWBFS.is_support_container_post_handling) {
                    //! send the container node again
                        hwOffset =
                            (uint16_t)((uintptr_t)vsf_this.FWBFS.msg_handling.node_ptr
                                     - (uintptr_t)node_ptr /*vsf_this.FWBFS.msg_handling.node_ptr*/);

                        if (!vsf_rng_buf_send_one(  __bfs_node_fifo_t,
                                                    &(vsf_this.FWBFS.fifo),
                                                    hwOffset)) {
                            RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM();
                            return (fsm_rt_t)
                                VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
                        }
                    }
                }
                break;

            } else if (VSF_MSGT_ERR_REQUEST_VISIT_PARENT == (vsf_msgt_err_t)fsm_rt) {
                THIS_FSM_STATE = FETCH_ITEM;

                //! visit parent is requested
                const vsf_msgt_node_t* temp_ptr = NULL;
                if (NULL != vsf_this.FWBFS.msg_handling.node_ptr->parent_ptr) {
                    temp_ptr = (const vsf_msgt_node_t *)
                                vsf_this.FWBFS.msg_handling.node_ptr->parent_ptr;
                } else {
                    /* no parent, revisit itself again to maintain the same behaviour*/
                    temp_ptr = vsf_this.FWBFS.msg_handling.node_ptr;
                }

                hwOffset =
                    (uint16_t)((uintptr_t)temp_ptr - (uintptr_t)node_ptr);
                if (!vsf_rng_buf_send_one(__bfs_node_fifo_t,
                    &(vsf_this.FWBFS.fifo),
                    hwOffset)) {
                    RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM();
                    return (fsm_rt_t)
                        VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
                }

                break;

            } else if (VSF_MSGT_ERR_REUQEST_VISIT_AGAIN == (vsf_msgt_err_t)fsm_rt) {
                THIS_FSM_STATE = FETCH_ITEM;

                const vsf_msgt_node_t* temp_ptr = vsf_this.FWBFS.msg_handling.node_ptr;
                hwOffset =
                    (uint16_t)((uintptr_t)temp_ptr - (uintptr_t)node_ptr);
                if (!vsf_rng_buf_send_one(__bfs_node_fifo_t,
                    &(vsf_this.FWBFS.fifo),
                    hwOffset)) {
                    RESET_MSGT_FW_BFS_PROPAGATE_MSG_FSM();
                    return (fsm_rt_t)
                        VSF_ERR_PROVIDED_RESOURCE_NOT_SUFFICIENT;
                }
                break;
            }

            return fsm_rt;
    }

    return fsm_rt_on_going;
}

#endif
/* EOF */
