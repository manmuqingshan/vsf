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

#ifndef __OSA_HAL_X86_WIN_USART_H__
#define __OSA_HAL_X86_WIN_USART_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_USART == ENABLED

#include "../device.h"
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/

typedef struct vsf_usart_t vsf_usart_t;

typedef struct vsf_hostos_usart_device_t {
    vsf_usart_t                         *instance;
    uint8_t                             port;
    char                                name[256];
    char                                friendly_name[256];
} vsf_hostos_usart_device_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern uint8_t vsf_hostos_usart_scan_devices(vsf_hostos_usart_device_t *devices, uint8_t device_num);

#endif
#endif      // __OSA_HAL_X86_WIN_USART_H__
