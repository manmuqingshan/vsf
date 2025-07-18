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

#ifndef __VSF_TEMPLATE_DMA_H__
#define __VSF_TEMPLATE_DMA_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability.
 *
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_DMA_CFG_MULTI_CLASS
#   define VSF_DMA_CFG_MULTI_CLASS                        ENABLED
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_DMA_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 *
 * \~chinese
 * @brief 可重新定义宏 VSF_DMA_CFG_PREFIX，以在应用代码中直接调用
 * 特定驱动。
 */
#ifndef VSF_DMA_CFG_PREFIX
#   if VSF_DMA_CFG_MULTI_CLASS == ENABLED
#       define VSF_DMA_CFG_PREFIX                         vsf
#   elif defined(VSF_HW_DMA_COUNT) && (VSF_HW_DMA_COUNT != 0)
#       define VSF_DMA_CFG_PREFIX                         vsf_hw
#   else
#       define VSF_DMA_CFG_PREFIX                         vsf
#   endif
#endif

/**
 * \~english
 * @brief Disable VSF_DMA_CFG_FUNCTION_RENAME to use the original function
 * names (e.g., vsf_dma_init()).
 *
 * \~chinese
 * @brief 禁用 VSF_DMA_CFG_FUNCTION_RENAME 以使用原始函数名
 * (例如 vsf_dma_init())。
 */
#ifndef VSF_DMA_CFG_FUNCTION_RENAME
#   define VSF_DMA_CFG_FUNCTION_RENAME                  ENABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement channel mode type in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现通道模式类型的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE    DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement interrupt mask type in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现中断掩码类型的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif


/**
 * \~english
 * @brief Enable the option to reimplement CFG type configuration in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现 CFG 类型配置的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_CFG_TYPE_CFG
#   define VSF_DMA_CFG_REIMPLEMENT_CFG_TYPE_CFG         DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement Channal Hint type configuration in specific hardware drivers
 *
 * \~chinese
 * @brief 启用在特定硬件驱动中重新实现 Channal Hint 类型配置的选项
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT    DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement channel configuration type.
 * For compatibility, do not delete members when redefining vsf_dma_channel_cfg_t
 *
 * \~chinese
 * @brief 启用重新实现通道配置类型的选项。
 * 为保证兼容性，重新定义vsf_dma_channel_cfg_t时不要删除成员
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG     DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement DMA channel scatter-gather configuration type.
 *
 * \~chinese
 * @brief 启用重新实现DMA通道scatter-gather配置类型的选项。
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG  \
                                                        DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement DMA configuration type.
 * For compatibility, do not delete members when redefining vsf_dma_cfg_t
 * The vsf_dma_isr_handler_t type also needs to be redefined
 *
 * \~chinese
 * @brief 启用重新实现DMA配置类型的选项。
 * 为保证兼容性，重新定义vsf_dma_cfg_t时不要删除成员
 * 同时也需要重新定义vsf_dma_isr_handler_t类型
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CFG             DISABLED
#endif

/**
 * \~english
 * @brief Enable the option to reimplement capability type.
 * For compatibility, do not delete members when redefining vsf_dma_capability_t
 *
 * \~chinese
 * @brief 启用重新实现能力类型的选项。
 * 为保证兼容性，重新定义vsf_dma_capability_t时不要删除成员
 */
#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY     DISABLED
#endif

/**
 * \~english
 * @brief Enable inheriting HAL capability to reuse common capability definitions
 *
 * \~chinese
 * @brief 启用继承HAL能力以重用通用能力定义
 */
#ifndef VSF_DMA_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_DMA_CFG_INHERIT_HAL_CAPABILITY            ENABLED
#endif

/**
 \~english
 @brief DMA scatter-gather configuration macro

 \~chinese
 @brief DMA scatter-gather 配置宏
 @note: 使用宏 VSF_DMA_CHANNEL_SG_ARRAY 来初始化 vsf_dma_channel_sg_desc_t 结构体数组以保证兼容性。
 在特定的硬件驱动中，可以根据传入的 __COUNT 重新指定结构体数组的大小，例如实际数组大小为 __COUNT+1。
 */
#ifndef VSF_DMA_CHANNEL_SG_ARRAY
#   define VSF_DMA_CHANNEL_SG_ARRAY(__NAME, __COUNT, ...)                       \
    vsf_dma_channel_sg_desc_t __NAME[__COUNT] = {                               \
        __VA_ARGS__                                                             \
    };
#endif

/**
 \~english
 @brief DMA scatter-gather item configuration macro

 \~chinese
 @brief DMA scatter-gather 项配置宏
 @note: 使用宏 VSF_DMA_CHANNEL_SG_ITEM 来初始化 @ref vsf_dma_channel_sg_desc_t 对应的结构体以保证兼容性。
 */
#ifndef VSF_DMA_CHANNEL_SG_ITEM
#   define VSF_DMA_CHANNEL_SG_ITEM(__MODE, __SRC_ADDR, __DST_ADDR, __CNT, ...)  \
    {                                                                           \
        .mode = __MODE,                                                         \
        .src_address = __SRC_ADDR,                                              \
        .dst_address = __DST_ADDR,                                              \
        .count = __CNT,                                                         \
        __VA_ARGS__                                                             \
    }
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief DMA API template, used to generate DMA type, specific prefix function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating DMA functions.
 *
 * \~chinese
 * @brief DMA API 模板，用于生成 DMA 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 DMA 函数的前缀。
 */
#define VSF_DMA_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, init,                               VSF_MCONNECT(__prefix_name, _t) *dma_ptr, vsf_dma_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                     dma, fini,                               VSF_MCONNECT(__prefix_name, _t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dma_capability_t,     dma, capability,                         VSF_MCONNECT(__prefix_name, _t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, int8_t,                   dma, channel_request,                    VSF_MCONNECT(__prefix_name, _t) *dma_ptr, vsf_dma_channel_hint_t *channel_hint_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                     dma, channel_release,                    VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_config,                     VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_start,                      VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel, uint32_t src_address, uint32_t dst_address, uint32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_cancel,                     VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_sg_config_desc,             VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel, vsf_dma_isr_t isr, vsf_dma_channel_sg_desc_t *scatter_gather_cfg, uint32_t sg_count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_sg_start,                   VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,                 dma, channel_get_transferred_count,      VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dma_channel_status_t, dma, channel_status,                     VSF_MCONNECT(__prefix_name, _t) *dma_ptr, uint8_t channel)

/*============================ TYPES =========================================*/

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == DISABLED
typedef enum vsf_dma_channel_mode_t {
    VSF_DMA_MEMORY_TO_MEMORY        = (0x00 << 0),
    VSF_DMA_MEMORY_TO_PERIPHERAL    = (0x01 << 0),
    VSF_DMA_PERIPHERA_TO_MEMORY     = (0x02 << 0),
    VSF_DMA_PERIPHERA_TO_PERIPHERAL = (0x03 << 0),

    VSF_DMA_SRC_ADDR_INCREMENT      = (0x00 << 2),
    VSF_DMA_SRC_ADDR_DECREMENT      = (0x01 << 2),
    VSF_DMA_SRC_ADDR_NO_CHANGE      = (0x02 << 2),

    VSF_DMA_DST_ADDR_INCREMENT      = (0x00 << 4),
    VSF_DMA_DST_ADDR_DECREMENT      = (0x01 << 4),
    VSF_DMA_DST_ADDR_NO_CHANGE      = (0x02 << 4),

    VSF_DMA_SRC_WIDTH_BYTE_1        = (0x00 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_2       = (0x01 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_4       = (0x02 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_8       = (0x03 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_16      = (0x04 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_32      = (0x05 << 6),

    VSF_DMA_DST_WIDTH_BYTE_1        = (0x00 << 10),
    VSF_DMA_DST_WIDTH_BYTES_2       = (0x01 << 10),
    VSF_DMA_DST_WIDTH_BYTES_4       = (0x02 << 10),
    VSF_DMA_DST_WIDTH_BYTES_8       = (0x03 << 10),
    VSF_DMA_DST_WIDTH_BYTES_16      = (0x04 << 10),
    VSF_DMA_DST_WIDTH_BYTES_32      = (0x05 << 10),

    VSF_DMA_SRC_BURST_LENGTH_1       = (0x00 << 14),
    VSF_DMA_SRC_BURST_LENGTH_2       = (0x01 << 14),
    VSF_DMA_SRC_BURST_LENGTH_4       = (0x02 << 14),
    VSF_DMA_SRC_BURST_LENGTH_8       = (0x03 << 14),
    VSF_DMA_SRC_BURST_LENGTH_16      = (0x04 << 14),
    VSF_DMA_SRC_BURST_LENGTH_32      = (0x05 << 14),
    VSF_DMA_SRC_BURST_LENGTH_64      = (0x05 << 14),
    VSF_DMA_SRC_BURST_LENGTH_128     = (0x06 << 14),

    VSF_DMA_DST_BURST_LENGTH_1       = (0x00 << 18),
    VSF_DMA_DST_BURST_LENGTH_2       = (0x01 << 18),
    VSF_DMA_DST_BURST_LENGTH_4       = (0x02 << 18),
    VSF_DMA_DST_BURST_LENGTH_8       = (0x03 << 18),
    VSF_DMA_DST_BURST_LENGTH_16      = (0x04 << 18),
    VSF_DMA_DST_BURST_LENGTH_32      = (0x05 << 18),
    VSF_DMA_DST_BURST_LENGTH_64      = (0x05 << 18),
    VSF_DMA_DST_BURST_LENGTH_128     = (0x06 << 18),

    //prio
    VSF_DMA_PRIOPIRY_LOW             = (0x00 << 22),
    VSF_DMA_PRIOPIRY_MEDIUM          = (0x01 << 22),
    VSF_DMA_PRIOPIRY_HIGH            = (0x02 << 22),
    VSF_DMA_PRIOPIRY_VERY_HIGH       = (0x03 << 22),

} vsf_dma_channel_mode_t;
#endif

enum {
    VSF_DMA_DIRECTION_COUNT           = 4,
    VSF_DMA_DIRECTION_MASK            = VSF_DMA_MEMORY_TO_MEMORY        |
                                        VSF_DMA_MEMORY_TO_PERIPHERAL    |
                                        VSF_DMA_PERIPHERA_TO_MEMORY     |
                                        VSF_DMA_PERIPHERA_TO_PERIPHERAL,

    VSF_DMA_SRC_ADDR_COUNT            = 3,
    VSF_DMA_SRC_ADDR_MASK             = VSF_DMA_SRC_ADDR_INCREMENT |
                                        VSF_DMA_SRC_ADDR_DECREMENT |
                                        VSF_DMA_SRC_ADDR_NO_CHANGE,

    VSF_DMA_DST_ADDR_COUNT            = 3,
    VSF_DMA_DST_ADDR_MASK             = VSF_DMA_DST_ADDR_INCREMENT |
                                        VSF_DMA_DST_ADDR_DECREMENT |
                                        VSF_DMA_DST_ADDR_NO_CHANGE,

    VSF_DMA_SRC_WIDTH_COUNT           = 6,
    VSF_DMA_SRC_WIDTH_MASK            = VSF_DMA_SRC_WIDTH_BYTE_1   |
                                        VSF_DMA_SRC_WIDTH_BYTES_2  |
                                        VSF_DMA_SRC_WIDTH_BYTES_4  |
                                        VSF_DMA_SRC_WIDTH_BYTES_8  |
                                        VSF_DMA_SRC_WIDTH_BYTES_16 |
                                        VSF_DMA_SRC_WIDTH_BYTES_32,

    VSF_DMA_DST_WIDTH_COUNT           = 6,
    VSF_DMA_DST_WIDTH_MASK            = VSF_DMA_DST_WIDTH_BYTE_1   |
                                        VSF_DMA_DST_WIDTH_BYTES_2  |
                                        VSF_DMA_DST_WIDTH_BYTES_4  |
                                        VSF_DMA_DST_WIDTH_BYTES_8  |
                                        VSF_DMA_DST_WIDTH_BYTES_16 |
                                        VSF_DMA_DST_WIDTH_BYTES_32,

    VSF_DMA_SRC_BURST_COUNT           = 8,
    VSF_DMA_SRC_BURST_MASK            = VSF_DMA_SRC_BURST_LENGTH_1   |
                                        VSF_DMA_SRC_BURST_LENGTH_2   |
                                        VSF_DMA_SRC_BURST_LENGTH_4   |
                                        VSF_DMA_SRC_BURST_LENGTH_8   |
                                        VSF_DMA_SRC_BURST_LENGTH_16  |
                                        VSF_DMA_SRC_BURST_LENGTH_32  |
                                        VSF_DMA_SRC_BURST_LENGTH_64  |
                                        VSF_DMA_SRC_BURST_LENGTH_128,

    VSF_DMA_DST_BURST_COUNT           = 8,
    VSF_DMA_DST_BURST_MASK            = VSF_DMA_DST_BURST_LENGTH_1   |
                                        VSF_DMA_DST_BURST_LENGTH_2   |
                                        VSF_DMA_DST_BURST_LENGTH_4   |
                                        VSF_DMA_DST_BURST_LENGTH_8   |
                                        VSF_DMA_DST_BURST_LENGTH_16  |
                                        VSF_DMA_DST_BURST_LENGTH_32  |
                                        VSF_DMA_DST_BURST_LENGTH_64  |
                                        VSF_DMA_DST_BURST_LENGTH_128,

    VSF_DMA_PRIOPIRY_COUNT            = 4,
    VSF_DMA_PRIOPIRY_MASK             = VSF_DMA_PRIOPIRY_LOW       |
                                        VSF_DMA_PRIOPIRY_MEDIUM    |
                                        VSF_DMA_PRIOPIRY_HIGH      |
                                        VSF_DMA_PRIOPIRY_VERY_HIGH,

    VSF_DMA_MODE_MASK_COUNT           = 8,
    VSF_DMA_MODE_ALL_BITS_MASK        = VSF_DMA_DIRECTION_MASK  |
                                        VSF_DMA_SRC_ADDR_MASK   |
                                        VSF_DMA_DST_ADDR_MASK   |
                                        VSF_DMA_SRC_WIDTH_MASK  |
                                        VSF_DMA_DST_WIDTH_MASK  |
                                        VSF_DMA_SRC_BURST_MASK  |
                                        VSF_DMA_DST_BURST_MASK  |
                                        VSF_DMA_PRIOPIRY_MASK,

};

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_dma_irq_mask_t {
    VSF_DMA_IRQ_MASK_CPL        = (0x1ul << 0),
    VSF_DMA_IRQ_MASK_HALF_CPL   = (0x1ul << 1),
    VSF_DMA_IRQ_MASK_ERROR      = (0x1ul << 2),
} vsf_dma_irq_mask_t;
#endif

enum {
    VSF_DMA_IRQ_COUNT               = 3,
    VSF_DMA_IRQ_ALL_BITS_MASK       = VSF_DMA_IRQ_MASK_CPL      |
                                      VSF_DMA_IRQ_MASK_HALF_CPL |
                                      VSF_DMA_IRQ_MASK_ERROR,
};


#if VSF_DMA_CFG_REIMPLEMENT_CFG_TYPE_CFG == DISABLED
/**
 * \~english
 * @brief Configuration structure for DMA.
 *
 * \~chinese
 * @brief DMA 的配置结构体。
 */
typedef struct vsf_dma_cfg_t {
    vsf_arch_prio_t         prio;
} vsf_dma_cfg_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_HINT == DISABLED
/**
 * \~english
 * @brief DMA channel hint structure
 *
 * \~chinese
 * @brief DMA 通道提示结构体
 */
typedef struct vsf_dma_channel_hint_t {
    //! \~english Specify the DMA channel number (some chips have fixed functionality configurations for DMA channels)
    //! \~chinese 指定具体的DMA通道号（部分芯片中DMA通道具有固定的功能配置）
    uint8_t channel;
    //! \~english Peripheral request line number, specifying the peripheral request source using DMA service.
    //! \~english (Some chips provide a mapping table between peripherals and request lines, refer to chip manual to select the correct request line number).
    //! \~chinese 外设请求线编号，指定使用DMA服务的外设请求源。
    //! \~chinese （部分芯片会提供外设和请求线的对应表，需根据芯片手册选择正确的请求线编号）。
    uint8_t request_line;
} vsf_dma_channel_hint_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_CFG == DISABLED
typedef struct vsf_dma_t vsf_dma_t;

typedef void vsf_dma_isr_handler_t(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask);

typedef struct vsf_dma_isr_t {
    vsf_dma_isr_handler_t *handler_fn;
    void                  *target_ptr;
} vsf_dma_isr_t;

//! dma configuration
typedef struct vsf_dma_channel_cfg_t {
    vsf_dma_channel_mode_t  mode;
    vsf_dma_isr_t           isr;
    //! Index of the peripheral or memory corresponding to the source address of the DMA
    uint8_t                 src_idx;
    //! Index of the peripheral or memory corresponding to the destination address of the DMA
    uint8_t                 dst_idx;
} vsf_dma_channel_cfg_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_SG_CFG == DISABLED
/**
 * \~english
 * @brief DMA scatter-gather descriptor structure
 * @note: The vsf_dma_channel_sg_desc_t structure can be redefined
 * according to the descriptor structure required by DMA Scatter-Gather in
 * specific hardware drivers.
 *
 * \~chinese
 * @brief DMA scatter-gather 描述符结构体
 * @note: vsf_dma_channel_sg_desc_t
 * 结构体，在特定的硬件驱动中可以根据 DMA Scatter-Gather
 * 需要的描述符结构体重新定义。
 */
typedef struct vsf_dma_channel_sg_desc_t {
    vsf_dma_channel_mode_t mode;    //!< DMA channel mode
    uint32_t src_address;           //!< Source address
    uint32_t dst_address;           //!< Destination address
    uint32_t count;                 //!< Number of bytes to be transferred
    uint32_t next;                  //!< Next descriptor address
} vsf_dma_channel_sg_desc_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_dma_channel_status_t {
    union {
        inherit(vsf_peripheral_status_t)
            struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_dma_channel_status_t;
#endif

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_dma_capability_t {
#if VSF_DMA_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_dma_irq_mask_t irq_mask;

    uint32_t max_request_count;
    uint8_t channel_count;
} vsf_dma_capability_t;
#endif

typedef struct vsf_dma_op_t {
/// @cond
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_DMA_APIS(vsf_dma)
} vsf_dma_op_t;

#if VSF_DMA_CFG_MULTI_CLASS == ENABLED
struct vsf_dma_t  {
    const vsf_dma_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief Initialize a DMA instance
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_dma_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise returns error code

 @note It is not necessary to call vsf_dma_fini() to deinitialization.
       vsf_dma_init() should be called before any other DMA API except vsf_dma_capability().

 \~chinese
 @brief 初始化一个 DMA 实例
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] cfg_ptr: 指向结构体 @ref vsf_dma_cfg_t 的指针
 @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码

 @note 失败后不需要调用 vsf_dma_fini() 反初始化。
       vsf_dma_init() 应该在除 vsf_dma_capability() 之外的其他 DMA API 之前调用。
 */
extern vsf_err_t vsf_dma_init(vsf_dma_t *dma_ptr, vsf_dma_cfg_t *cfg_ptr);

/**
 \~english
 @brief Finalize a DMA instance
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return none

 \~chinese
 @brief 终止一个 DMA 实例
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @return 无
 */
extern void vsf_dma_fini(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief Get the capabilities of DMA instance
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return vsf_dma_capability_t: All capabilities of current DMA @ref vsf_dma_capability_t

 \~chinese
 @brief 获取 DMA 实例的能力
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @return vsf_dma_capability_t: 返回当前 DMA 的所有能力 @ref vsf_dma_capability_t
 */
extern vsf_dma_capability_t vsf_dma_capability(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief DMA request a new channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel_hint_ptr: a pointer to DMA channel hint
 @return int8_t: if request was successful, return non-negative channel number, otherwise return error code

 \~chinese
 @brief DMA 请求一个新的通道
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel_hint_ptr: 指向 DMA 通道提示的指针
 @return int8_t: 如果请求成功返回非负数的通道号，否则返回错误码
 */
extern int8_t vsf_dma_channel_request(vsf_dma_t *dma_ptr, vsf_dma_channel_hint_t *channel_hint_ptr);

/**
 \~english
 @brief Release a DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return none

 \~chinese
 @brief DMA 释放一个通道
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道号
 @return 无
 */
extern void vsf_dma_channel_release(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 \~english
 @brief Configure a DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @param[in] cfg_ptr: a pointer to DMA channel configuration
 @return vsf_err_t: VSF_ERR_NONE if the configuration was successful, otherwise returns error code

 \~chinese
 @brief DMA 通道配置
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @param[in] cfg_ptr: 指向 DMA 通道配置的指针
 @return vsf_err_t: 如果配置成功返回 VSF_ERR_NONE，否则返回错误码
 @note: 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
        API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
        这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 */
extern vsf_err_t vsf_dma_channel_config(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_channel_cfg_t *cfg_ptr);

/**
 * \~english
 * @brief Start a DMA channel transfer
 * @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 * @param[in] channel: channel number
 * @param[in] src_address: source address
 * @param[in] dst_address: destination address
 * @param[in] count: counter of data to be transferred (in byte)
 * @return vsf_err_t: VSF_ERR_NONE if the start request was successful, otherwise returns error code
 *
 * \~chinese
 * @brief DMA 开始一个通道的传输
 * @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 * @param[in] channel: 通道序号
 * @param[in] src_address: 源地址
 * @param[in] dst_address: 目标地址
 * @param[in] count: 将要传输的数据的字节数
 * @return vsf_err_t: 如果开始传输成功返回 VSF_ERR_NONE，否则返回错误码
 * @note: 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
        API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
        这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 */
extern vsf_err_t vsf_dma_channel_start(vsf_dma_t *dma_ptr, uint8_t channel, uint32_t src_address, uint32_t dst_address, uint32_t count);

/**
 \~english
 @brief Cancel the transmission of a specific DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_err_t: VSF_ERR_NONE if the cancel request was successful, otherwise returns error code

 \~chinese
 @brief DMA 取消特定通道的传输
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @return vsf_err_t: 如果传输取消成功返回 VSF_ERR_NONE，否则返回错误码
 */
extern vsf_err_t vsf_dma_channel_cancel(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 \~english
 @brief Configure a DMA channel for scatter-gather transfer
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @param[in] isr: DMA interrupt handler
 @param[in] cfg_ptr: a pointer to DMA channel scatter-gather configuration
 @param[in] sg_count: number of scatter-gather configurations
 @return vsf_err_t: VSF_ERR_NONE if the configuration was successful, otherwise returns error code

 \~chinese
 @brief DMA 通道配置为 scatter-gather 传输
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @param[in] isr: DMA 中断处理函数
 @param[in] cfg_ptr: 指向 DMA 通道 scatter-gather 配置的指针
 @param[in] sg_count: scatter-gather 配置的数量
 @return vsf_err_t: 如果配置成功返回 VSF_ERR_NONE，否则返回错误码
 @note: scatter_gather_cfg 指向的结构体应该使用宏 VSF_DMA_CHANNEL_SG_ARRAY 来初始化，以保证更好的兼容性。
 @note: scatter_gather_cfg 指向的结构体需要是可以被 DMA 直接访问的内存区域，
 @note: scatter_gather_cfg 指向的结构体需要调用者确保在 DMA 传输完成之前不会被释放。
 @note: scatter_gather_cfg 指向的结构体的内容可能在配置后被修改成 DMA 实际需要的 Scatter-Gather 描述符结构。
 @note: 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
        API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
        这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 */
extern vsf_err_t vsf_dma_channel_sg_config_desc(vsf_dma_t *dma_ptr, uint8_t channel, vsf_dma_isr_t isr,
            vsf_dma_channel_sg_desc_t *sg_desc_ptr, uint32_t sg_count);

/**
 \~english
 @brief Start a DMA scatter-gather transfer
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_err_t: VSF_ERR_NONE if the start request was successful, otherwise returns error code

 \~chinese
 @brief DMA 开始一个 scatter-gather 传输
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @return vsf_err_t: 如果开始传输成功返回 VSF_ERR_NONE，否则返回错误码
  @note: 对于指定通道，API vsf_dma_channel_sg_config_desc() 和 vsf_dma_channel_sg_start() 必须配合使用。
        API vsf_dma_channel_config() 和 vsf_dma_channel_start() 配对使用。
        这两组API不能混用，只有在当前DMA传输完全结束后，才能切换到另一组API使用。
 */
extern vsf_err_t vsf_dma_channel_sg_start(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 * \~english
 * @brief Get the counter of bytes transferred for a specific DMA channel
 * @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 * @param[in] channel: channel number
 * @return uint32_t: Number of bytes transferred
 * @note: If called after VSF_DMA_IRQ_MASK_CPL interrupt trigger, it should return 0.
 * @note: It is usually called after call vsf_dma_channel_cancel() to get the counter that has been transferred.
 *
 * \~chinese
 * @brief DMA 获取特定通道已经传输的数量
 * @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 * @param[in] channel: 通道序号
 * @return uint32_t: 已传输的字节数
 * @note: 如果在 VSF_DMA_IRQ_MASK_CPL 中断触发之后调用，它应该返回 0。
 * @note: 它通常在调用 vsf_dma_channel_cancel() 之后调用，用于获取已经传输的数量。
 */
extern uint32_t vsf_dma_channel_get_transferred_count(vsf_dma_t *dma_ptr, uint8_t channel);

/**
 \~english
 @brief Get status of a DMA channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_dma_channel_status_t: Status of the DMA channel

 \~chinese
 @brief 获取 DMA 通道状态
 @param[in] dma_ptr: 指向结构体 @ref vsf_dma_t 的指针
 @param[in] channel: 通道序号
 @return vsf_dma_channel_status_t: DMA 通道状态
 */
extern vsf_dma_channel_status_t vsf_dma_channel_status(vsf_dma_t *dma_ptr, uint8_t channel);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_DMA_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_dma_t                                              VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_t)
#   define vsf_dma_init(__DMA, ...)                                 VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_init)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_fini(__DMA)                                      VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_fini)((__vsf_dma_t *)(__DMA))
#   define vsf_dma_capability(__DMA)                                VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_capability)((__vsf_dma_t *)(__DMA))
#   define vsf_dma_channel_request(__DMA, ...)                      VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_request)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_release(__DMA, ...)                      VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_release)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_config(__DMA, ...)                       VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_config)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_start(__DMA, ...)                        VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_start)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_sg_config_desc(__DMA, ...)               VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_sg_config_desc)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_sg_start(__DMA, ...)                     VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_sg_start)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_cancel(__DMA, ...)                       VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_cancel)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_get_transferred_count(__DMA, ...)        VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_get_transferred_count)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_status(__DMA, ...)                       VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_status)((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_DMA_H__*/
