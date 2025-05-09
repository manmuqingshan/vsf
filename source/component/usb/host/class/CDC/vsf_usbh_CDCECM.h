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

#ifndef __VSF_USBH_CDCECM_H__
#define __VSF_USBH_CDCECM_H__

/*============================ INCLUDES ======================================*/

#include "component/usb/vsf_usb_cfg.h"

#if     VSF_USE_USB_HOST == ENABLED                                             \
    &&  VSF_USE_TCPIP == ENABLED                                                \
    &&  ((VSF_USBH_USE_ECM == ENABLED) || (VSF_USBH_USE_NCM == ENABLED))

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

#if VSF_USBH_USE_ECM == ENABLED
extern const vk_usbh_class_drv_t vk_usbh_ecm_drv;
#   if VSF_USBH_USE_LIBUSB == ENABLED
extern const vk_usbh_class_drv_t vk_usbh_ecm_block_libusb_drv;
#   endif
#endif

#if VSF_USBH_USE_NCM == ENABLED
extern const vk_usbh_class_drv_t vk_usbh_ncm_drv;
#   if VSF_USBH_USE_LIBUSB == ENABLED
extern const vk_usbh_class_drv_t vk_usbh_ncm_block_libusb_drv;
#   endif
#endif

/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif
#endif
