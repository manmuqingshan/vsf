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

#ifndef __HAL_DRIVER_COMMON_I2C_MERGER_REQUEST_H__
#define __HAL_DRIVER_COMMON_I2C_MERGER_REQUEST_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if (VSF_HAL_USE_I2C == ENABLED) && (VSF_HAL_USE_MERGER_REQUEST_I2C == ENABLED)

#if defined(__VSF_I2C_MERGER_REQUEST_CLASS_IMPLEMENT)
#   undef __VSF_I2C_MERGER_REQUEST_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/

#ifndef VSF_MREQUEST_I2C_CFG_REQEUST_BUFFER_SIZE
#   define VSF_MREQUEST_I2C_CFG_REQEUST_BUFFER_SIZE     3
#endif

#ifndef VSF_MREQUEST_I2C_CFG_MULTI_CLASS
#   define VSF_MREQUEST_I2C_CFG_MULTI_CLASS              VSF_I2C_CFG_MULTI_CLASS
#endif

#define VSF_MREQUEST_I2C_DEC(__CNT, __IGNORE)                                   \
    extern vsf_mrequest_i2c_t vsf_mrequest_i2c ## __CNT;

#define VSF_MREQUEST_I2C_IMP(__CNT, __PREFIX)                                   \
    vsf_mrequest_i2c_t vsf_mrequest_i2c ## __CNT =                              \
        VSF_HAL_MRQUEST_I2C_INIT(& VSF_MCONNECT(__PREFIX, _i2c, __CNT));

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_HAL_MRQUEST_I2C_CFG_MULTI_CLASS == ENABLED
#define VSF_HAL_MRQUEST_I2C_INIT(__REAL_I2C)  {                                 \
        .i2c_ptr = (vsf_i2c_t *)__REAL_I2C,                                     \
        .vsf_i2c = {                                                            \
            .op = &vsf_mreqeust_i2c_op                                          \
        },                                                                      \
    }
#else
#define VSF_HAL_MRQUEST_I2C_INIT(__REAL_I2C)  {                                 \
        .i2c_ptr = (vsf_i2c_t *)__REAL_I2C,                                     \
    }
#endif
    
#if VSF_MREQUEST_I2C_CFG_MULTI_CLASS == ENABLED
#   define __describe_mrequest_op()         .op = &vsf_mrequest_op,
#else
#   define __describe_mrequest_op()
#endif

#define __describe_mrequest(__name, __real_i2c)\
    vsf_mrequest_t __name = {                                                   \
        __describe_mrequest_op()                                                \
        .i2c_ptr           = __real_i2c,                                        \
    };

#define describe_mrequest(__name, __real_i2c)\
            __describe_mrequest(__name, __real_i2c)

/*============================ INCLUDES ======================================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_mrequest_i2c_t) {
    public_member(
#if VSF_HAL_MRQUEST_I2C_CFG_MULTI_CLASS == ENABLED
        vsf_i2c_t vsf_i2c;
#endif
        vsf_i2c_t *i2c_ptr;
    )

    private_member(
        vsf_i2c_cfg_t cfg;
        vsf_i2c_irq_mask_t irq_mask;
        uint8_t buffer[VSF_MREQUEST_I2C_CFG_REQEUST_BUFFER_SIZE];

        struct {
            vsf_i2c_cmd_t cmd;
            uint8_t *buffer_ptr;
            uint16_t address;
            uint16_t count;
        } request;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/

#define VSF_I2C_CFG_DEC_PREFIX              vsf_mrequest
#define VSF_I2C_CFG_DEC_UPCASE_PREFIX       VSF_MREQUEST
#define VSF_I2C_CFG_DEC_EXTERN_OP           ENABLED
#include "hal/driver/common/i2c/i2c_template.h"

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/


#endif // VSF_HAL_I2C_IMP_REQUEST_BY_CMD
#endif