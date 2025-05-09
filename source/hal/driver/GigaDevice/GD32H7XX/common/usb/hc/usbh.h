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

#ifndef __OSA_HAL_DRIVER_GIGADEVICE_GD32H7XX_USBH_H__
#define __OSA_HAL_DRIVER_GIGADEVICE_GD32H7XX_USBH_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "../usb.h"

#if VSF_USE_USB_HOST == ENABLED && VSF_USBH_USE_HCD_DWCOTG == ENABLED

/*============================ MACROS ========================================*/

#define vsf_hw_usbh_ep_number           16
#define vsf_hw_usbh_ep_is_dma           false

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

extern vsf_err_t vsf_hw_usbh_init(vsf_hw_usb_t *hc, usb_hc_ip_cfg_t *cfg);
extern void vsf_hw_usbh_get_info(vsf_hw_usb_t *hc, usb_hc_ip_info_t *info);
extern void vsf_hw_usbh_irq_enable(vsf_hw_usb_t *hc);
extern void vsf_hw_usbh_irq_disable(vsf_hw_usb_t *hc);
extern void vsf_hw_usbh_irq(vsf_hw_usb_t *hc);

#endif
#endif
/* EOF */
