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

#ifndef __VSF_ARCH_ABSTRACTION_H__
#define __VSF_ARCH_ABSTRACTION_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"
#include "utilities/vsf_utilities.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

//! \name different models to implement systimer
//! @{
#define VSF_SYSTIMER_IMPL_NONE                                  0
#define VSF_SYSTIMER_IMPL_REQUEST_RESPONSE                      1
#define VSF_SYSTIMER_IMPL_WITH_NORMAL_TIMER                     2
#define VSF_SYSTIMER_IMPL_WITH_COMP_TIMER                       3
#define VSF_SYSTIMER_IMPL_TICK_MODE                             4
//! @}

/*============================ MACROFIED FUNCTIONS ===========================*/

#ifndef VSF_ARCH_ASSERT
#   define VSF_ARCH_ASSERT(...)                 VSF_ASSERT(__VA_ARGS__)
#endif

/*============================ TYPES =========================================*/

/*\note generic prototypes MUST be defined before including the arch header. */
typedef void vsf_irq_handler_t(void *p);
typedef vsf_irq_handler_t vsf_swi_handler_t;

typedef uintalu_t vsf_protect_t;
typedef struct vsf_protect_region_t {
    vsf_protect_t (*enter)(void);
    void (*leave)(vsf_protect_t orig);
} vsf_protect_region_t;

typedef struct vsf_arch_text_region_t vsf_arch_text_region_t;
struct vsf_arch_text_region_t {
    vsf_arch_text_region_t *next;
    uintptr_t start;
    uintptr_t size;
};

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_protect_region_t vsf_protect_region_int;
extern const vsf_protect_region_t vsf_protect_region_none;

/*============================ LOCAL VARIABLES ===============================*/

#ifdef __cplusplus
}
#endif

/*============================ INCLUDES ======================================*/

#if !defined(VSF_ARCH_HEADER)
//! check rtos first
# if    defined(__RTOS__)
#   define  VSF_ARCH_HEADER     "./rtos/rtos_generic.h"
# elif  defined(__CPU_GENERIC__)
#   if defined(__MACOS__)
#     define  VSF_ARCH_HEADER   "./generic/macos/macos_generic.h"
#   elif defined(__LINUX__)
#     define  VSF_ARCH_HEADER   "./generic/linux/linux_generic.h"
#   else
#     error not supported generic arch
#   endif
# elif  (defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'M') || __TARGET_PROFILE_M
#   define VSF_ARCH_HEADER      "./arm/cortex-m/cortex_m_generic.h"
# elif  defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'A' || __TARGET_PROFILE_A
#   define VSF_ARCH_HEADER      "./arm/cortex-a/cortex_a_generic.h"
# elif  defined(__ARM_ARCH_PROFILE) && __ARM_ARCH_PROFILE == 'R' || __TARGET_PROFILE_R
#   define VSF_ARCH_HEADER      "./arm/cortex-r/cortex_r_generic.h"
// check __WIN__ before __LINUX__ because some software(like SDL2) will define __LINUX__
# elif  (defined(__CPU_X86__) || defined(__CPU_X64__)) && defined(__WIN__)
#   define  VSF_ARCH_HEADER     "./x86/win/win_generic.h"
# elif  (defined(__CPU_X86__) || defined(__CPU_X64__) || defined(__CPU_WEBASSEMBLY__)) && defined(__LINUX__)
#   define  VSF_ARCH_HEADER     "./generic/linux/linux_generic.h"
# elif  defined(__CPU_MCS51__)
#   define  VSF_ARCH_HEADER     "./mcs51/mcs51_generic.h"
# elif  defined(__CPU_RV__) || defined(__CPU_RISCV__)
#   define  VSF_ARCH_HEADER     "./rv/rv_generic.h"
# elif  defined(__CPU_ARM9__)
#   define  VSF_ARCH_HEADER     "./arm/arm9/arm9_generic.h"
# else
#   warning no supported architecture found, use default arch template!
#   define  VSF_ARCH_HEADER     "./template/template_generic.h"
# endif
#endif
#include VSF_ARCH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_ARCH_SWI_NUM
#   define VSF_ARCH_SWI_NUM                 0
#endif
#ifndef VSF_DEV_SWI_NUM
#   define VSF_DEV_SWI_NUM                  0
#endif

#ifndef __BYTE_ORDER
#   error "__BYTE_ORDER MUST be defined in arch"
#endif

#define VSF_SWI_NUM                         (VSF_ARCH_SWI_NUM + VSF_DEV_SWI_NUM)

#define ___constant_swab16(__x) ((uint16_t)(                                    \
    (((uint16_t)(__x) & (uint16_t)0x00FFU) << 8) |                              \
    (((uint16_t)(__x) & (uint16_t)0xFF00U) >> 8)))

#define ___constant_swab32(__x) ((uint32_t)(                                    \
    (((uint32_t)(__x) & (uint32_t)0x000000ffUL) << 24) |                        \
    (((uint32_t)(__x) & (uint32_t)0x0000ff00UL) <<  8) |                        \
    (((uint32_t)(__x) & (uint32_t)0x00ff0000UL) >>  8) |                        \
    (((uint32_t)(__x) & (uint32_t)0xff000000UL) >> 24)))

#define ___constant_swab64(__x) ((uint64_t)(                                    \
    (((uint64_t)(__x) & (uint64_t)0x00000000000000ffULL) << 56) |               \
    (((uint64_t)(__x) & (uint64_t)0x000000000000ff00ULL) << 40) |               \
    (((uint64_t)(__x) & (uint64_t)0x0000000000ff0000ULL) << 24) |               \
    (((uint64_t)(__x) & (uint64_t)0x00000000ff000000ULL) <<  8) |               \
    (((uint64_t)(__x) & (uint64_t)0x000000ff00000000ULL) >>  8) |               \
    (((uint64_t)(__x) & (uint64_t)0x0000ff0000000000ULL) >> 24) |               \
    (((uint64_t)(__x) & (uint64_t)0x00ff000000000000ULL) >> 40) |               \
    (((uint64_t)(__x) & (uint64_t)0xff00000000000000ULL) >> 56)))

#define bswap16                             bswap_16
#define bswap32                             bswap_32
#define bswap64                             bswap_64
#if __BYTE_ORDER == __BIG_ENDIAN
#   ifndef htobe16
#       define htobe16
#   endif
#   ifndef htole16
#       define htole16                      bswap_16
#   endif
#   ifndef be16toh
#       define be16toh
#   endif
#   ifndef le16toh
#       define le16toh                      bswap_16
#   endif
#   ifndef htobe32
#       define htobe32
#   endif
#   ifndef htole32
#       define htole32                      bswap_32
#   endif
#   ifndef be32toh
#       define be32toh
#   endif
#   ifndef le32toh
#       define le32toh                      bswap_32
#   endif
#   ifndef htobe64
#       define htobe64
#   endif
#   ifndef htole64
#       define htole64                      bswap_64
#   endif
#   ifndef be64toh
#       define be64toh
#   endif
#   ifndef le64toh
#       define le64toh                      bswap_64
#   endif

#   define cpu_to_le16                      bswap_16
#   define cpu_to_le32                      bswap_32
#   define cpu_to_le64                      bswap_64
#   define le16_to_cpu                      bswap_16
#   define le32_to_cpu                      bswap_32
#   define le64_to_cpu                      bswap_64
#   define cpu_to_be16
#   define cpu_to_be32
#   define cpu_to_be64
#   define be16_to_cpu
#   define be32_to_cpu
#   define be64_to_cpu

#   define __constant_cpu_to_le16           ___constant_swab16
#   define __constant_le16_to_cpu           ___constant_swab16
#   define __constant_cpu_to_le32           ___constant_swab32
#   define __constant_le32_to_cpu           ___constant_swab32
#   define __constant_cpu_to_le64           ___constant_swab64
#   define __constant_le64_to_cpu           ___constant_swab64
#   define __constant_cpu_to_be16
#   define __constant_be16_to_cpu
#   define __constant_cpu_to_be32
#   define __constant_be32_to_cpu
#   define __constant_cpu_to_be64
#   define __constant_be64_to_cpu
#else
#   ifndef htobe16
#       define htobe16                      bswap_16
#   endif
#   ifndef htole16
#       define htole16
#   endif
#   ifndef be16toh
#       define be16toh                      bswap_16
#   endif
#   ifndef le16toh
#       define le16toh
#   endif
#   ifndef htobe32
#       define htobe32                      bswap_32
#   endif
#   ifndef htole32
#       define htole32
#   endif
#   ifndef be32toh
#       define be32toh                      bswap_32
#   endif
#   ifndef le32toh
#       define le32toh
#   endif
#   ifndef htobe64
#       define htobe64                      bswap_64
#   endif
#   ifndef htole64
#       define htole64
#   endif
#   ifndef be64toh
#       define be64toh                      bswap_64
#   endif
#   ifndef le64toh
#       define le64toh
#   endif

#   define cpu_to_le16
#   define cpu_to_le32
#   define cpu_to_le64
#   define le16_to_cpu
#   define le32_to_cpu
#   define le64_to_cpu
#   define cpu_to_be16                      bswap_16
#   define cpu_to_be32                      bswap_32
#   define cpu_to_be64                      bswap_64
#   define be16_to_cpu                      bswap_16
#   define be32_to_cpu                      bswap_32
#   define be64_to_cpu                      bswap_64

#   define __constant_cpu_to_le16
#   define __constant_le16_to_cpu
#   define __constant_cpu_to_le32
#   define __constant_le32_to_cpu
#   define __constant_cpu_to_le64
#   define __constant_le64_to_cpu
#   define __constant_cpu_to_be16           ___constant_swab16
#   define __constant_be16_to_cpu           ___constant_swab16
#   define __constant_cpu_to_be32           ___constant_swab32
#   define __constant_be32_to_cpu           ___constant_swab32
#   define __constant_cpu_to_be64           ___constant_swab64
#   define __constant_be64_to_cpu           ___constant_swab64
#endif

#define DECLARE_ENDIAN_FUNC(__bitlen)                                           \
extern uint_fast##__bitlen##_t cpu_to_le##__bitlen##p(uint##__bitlen##_t *);    \
extern uint_fast##__bitlen##_t cpu_to_be##__bitlen##p(uint##__bitlen##_t *);    \
extern uint_fast##__bitlen##_t le##__bitlen##_to_cpup(uint##__bitlen##_t *);    \
extern uint_fast##__bitlen##_t be##__bitlen##_to_cpup(uint##__bitlen##_t *);    \
extern void cpu_to_le##__bitlen##s(uint##__bitlen##_t *);                       \
extern void cpu_to_be##__bitlen##s(uint##__bitlen##_t *);                       \
extern void le##__bitlen##_to_cpus(uint##__bitlen##_t *);                       \
extern void be##__bitlen##_to_cpus(uint##__bitlen##_t *);                       \
extern uint_fast##__bitlen##_t get_unaligned_cpu##__bitlen(const void *);       \
extern uint_fast##__bitlen##_t get_unaligned_le##__bitlen(const void *);        \
extern uint_fast##__bitlen##_t get_unaligned_be##__bitlen(const void *);        \
extern void put_unaligned_cpu##__bitlen(uint_fast##__bitlen##_t, void *);       \
extern void put_unaligned_le##__bitlen(uint_fast##__bitlen##_t, void *);        \
extern void put_unaligned_be##__bitlen(uint_fast##__bitlen##_t, void *);

#define __vsf_protect_region(__region)                                          \
    for (vsf_protect_t VSF_MACRO_SAFE_NAME(protect_status),                     \
            VSF_MACRO_SAFE_NAME(local_cnt) = 1;                                 \
        (((__region) != NULL) && VSF_MACRO_SAFE_NAME(local_cnt)--) ?            \
            ((VSF_MACRO_SAFE_NAME(protect_status) = (__region)->enter()), true) : false;\
        (__region)->leave(VSF_MACRO_SAFE_NAME(protect_status)))
#define vsf_protect_region(__region)        __vsf_protect_region(__region)
#define vsf_protect_region_exit(__region)   continue

#define vsf_protect_region_simple(__region, ...)                                \
        do {                                                                    \
            if (__region != NULL) {                                             \
                vsf_protect_t VSF_MACRO_SAFE_NAME(protect_status) = (__region)->enter();\
                    __VA_ARGS__;                                                \
                (__region)->leave(VSF_MACRO_SAFE_NAME(protect_status));         \
            }                                                                   \
        } while (0);

/*! \note TODO: looks like the following macro, i.e. __vsf_interrupt_safe is
 *        deprecated, should remove it when sufficient time passed.
 *        (start from Jan 01 2021)
 */
// Usage:
//  __vsf_interrupt_safe(code)
#ifndef __vsf_interrupt_safe
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#       define __vsf_interrupt_safe(__code)                                     \
        {                                                                       \
            vsf_gint_state_t VSF_MACRO_SAFE_NAME(gint_state) = vsf_disable_interrupt();\
                __code;                                                         \
            vsf_set_interrupt(VSF_MACRO_SAFE_NAME(gint_state));                 \
        }
#   else
#       define __vsf_interrupt_safe(...)                                        \
        {                                                                       \
            vsf_gint_state_t VSF_MACRO_SAFE_NAME(gint_state) = vsf_disable_interrupt();\
                __VA_ARGS__;                                                    \
            vsf_set_interrupt(VSF_MACRO_SAFE_NAME(gint_state));                 \
        }
#   endif
#endif

// Usage:
//  vsf_interrupt_safe() {
//      code
//  }
#ifndef vsf_interrupt_safe
#   define vsf_interrupt_safe()             vsf_protect_region(&vsf_protect_region_int)
#endif


// Usage:
//  vsf_interrupt_safe_simple(
//      code
//  )
#ifndef vsf_interrupt_safe_simple

#   define vsf_interrupt_safe_simple(...)                                       \
                vsf_protect_region_simple(&vsf_protect_region_int, __VA_ARGS__)

#endif


#define vsf_protect_interrupt()             vsf_disable_interrupt()
#define vsf_unprotect_interrupt(__state)    vsf_set_interrupt(__state)
#define vsf_protect_none()                  (0)
#define vsf_unprotect_none(__state)         VSF_UNUSED_PARAM(__state)

#define vsf_protect_int                     vsf_protect_interrupt
#define vsf_unprotect_int                   vsf_unprotect_interrupt

#define __vsf_protect(__type)               vsf_protect_##__type
#define __vsf_unprotect(__type)             vsf_unprotect_##__type
#define vsf_protect(__type)                 __vsf_protect(__type)
#define vsf_unprotect(__type)               __vsf_unprotect(__type)

// atom

#ifndef vsf_atom64_op
#   define vsf_atom64_op(__ptr, ...)                                            \
        ({                                                                      \
            vsf_protect_t VSF_MACRO_SAFE_NAME(orig) = vsf_protect_int();        \
            int64_t _ = (*(int64_t *)(__ptr));                                  \
            *(int64_t *)(__ptr) = (__VA_ARGS__);                                \
            vsf_unprotect_int(VSF_MACRO_SAFE_NAME(orig));                       \
            _;                                                                  \
        })
#endif

#ifndef vsf_atom32_op
#   define vsf_atom32_op(__ptr, ...)                                            \
        ({                                                                      \
            vsf_protect_t VSF_MACRO_SAFE_NAME(orig) = vsf_protect_int();        \
            int32_t _ = (*(int32_t *)(__ptr));                                  \
            *(int32_t *)(__ptr) = (__VA_ARGS__);                                \
            vsf_unprotect_int(VSF_MACRO_SAFE_NAME(orig));                       \
            _;                                                                  \
        })
#endif

#ifndef vsf_atom16_op
#   define vsf_atom16_op(__ptr, ...)                                            \
        ({                                                                      \
            vsf_protect_t VSF_MACRO_SAFE_NAME(orig) = vsf_protect_int();        \
            int16_t _ = (*(int16_t *)(__ptr));                                  \
            *(int16_t *)(__ptr) = (__VA_ARGS__);                                \
            vsf_unprotect_int(VSF_MACRO_SAFE_NAME(orig));                       \
            _;                                                                  \
        })
#endif

#ifndef vsf_atom8_op
#   define vsf_atom8_op(__ptr, ...)                                             \
        ({                                                                      \
            vsf_protect_t VSF_MACRO_SAFE_NAME(orig) = vsf_protect_int();        \
            int8_t _ = (*(int8_t *)(__ptr));                                    \
            *(int8_t *)(__ptr) = (__VA_ARGS__);                                 \
            vsf_unprotect_int(VSF_MACRO_SAFE_NAME(orig));                       \
            _;                                                                  \
        })
#endif

#ifndef vsf_atom_add
#   define vsf_atom_add(__bitlen, __ptr, __value)                               \
        VSF_MCONNECT(vsf_atom, __bitlen, _op)((__ptr), _ + (__value))
#endif
#ifndef vsf_atom_dec
#   define vsf_atom_dec(__bitlen, __ptr, __value)                               \
        vsf_atom_add(__bitlen, (__ptr), - (__value))
#endif
#ifndef vsf_atom_or
#   define vsf_atom_or(__bitlen, __ptr, __value)                                \
        VSF_MCONNECT(vsf_atom, __bitlen, _op)((__ptr), _ | (__value))
#endif
#ifndef vsf_atom_and
#   define vsf_atom_and(__bitlen, __ptr, __value)                               \
        VSF_MCONNECT(vsf_atom, __bitlen, _op)((__ptr), _ & (__value))
#endif
#ifndef vsf_atom_xor
#   define vsf_atom_xor(__bitlen, __ptr, __value)                               \
        VSF_MCONNECT(vsf_atom, __bitlen, _op)((__ptr), _ ^ (__value))
#endif
#ifndef vsf_atom_xchg
#   define vsf_atom_xchg(__bitlen, __ptr, __new_value)                          \
        VSF_MCONNECT(vsf_atom, __bitlen, _op)((__ptr), (__new_value))
#endif

// host invoke
#define vsf_arch_host_invoke_ctx_t(__name)  VSF_MCONNECT(__name, _ctx_t)
#define declare_vsf_arch_host_invoke(__name)                                    \
        typedef struct vsf_arch_host_invoke_ctx_t(__name)                       \
                vsf_arch_host_invoke_ctx_t(__name);                             \
        typedef struct VSF_MCONNECT(__name, _t) VSF_MCONNECT(__name, _t);       \
        extern void VSF_MCONNECT(__name, _callback_t)                           \
                (vsf_arch_host_invoke_ctx_t(__name) *ctx);                      \
        extern VSF_MCONNECT(__name, _t) VSF_MCONNECT(__name, _instance);
#define dcl_vsf_arch_host_invoke        declare_vsf_arch_host_invoke
#define define_vsf_arch_host_invoke(__name, ...)                                \
        typedef struct vsf_arch_host_invoke_ctx_t(__name) {                     \
            int __dummy;                                                        \
            __VA_ARGS__                                                         \
        } vsf_arch_host_invoke_ctx_t(__name);                                   \
        typedef struct VSF_MCONNECT(__name, _t) {                               \
            implement(vsf_arch_irq_thread_t);                                   \
            vsf_arch_irq_request_t request;                                     \
            vsf_eda_t *caller;                                                  \
            vsf_arch_host_invoke_ctx_t(__name) ctx;                             \
        } VSF_MCONNECT(__name, _t);
#define def_vsf_arch_host_invoke        define_vsf_arch_host_invoke
#define implement_vsf_arch_host_invoke(__name)                                  \
        VSF_MCONNECT(__name, _t) VSF_MCONNECT(__name, _instance);               \
        extern void VSF_MCONNECT(__name, _callback_t)                           \
                (vsf_arch_host_invoke_ctx_t(__name) *ctx);                      \
        static void VSF_MCONNECT(__name, _thread)(void *arg)                    \
        {                                                                       \
            vsf_eda_t *caller;                                                  \
            VSF_MCONNECT(__name, _t) *ctx = arg;                                \
            __vsf_arch_irq_set_background(&ctx->use_as__vsf_arch_irq_thread_t); \
            while (1) {                                                         \
                __vsf_arch_irq_request_pend(&ctx->request);                     \
                VSF_MCONNECT(__name, _callback_t)(&ctx->ctx);                   \
                                                                                \
                __vsf_arch_irq_start(&ctx->use_as__vsf_arch_irq_thread_t);      \
                    caller = ctx->caller;                                       \
                    if (caller != NULL) {                                       \
                        ctx->caller = NULL;                                     \
                        vsf_eda_post_evt(caller, VSF_EVT_USER);                 \
                    }                                                           \
                __vsf_arch_irq_end(&ctx->use_as__vsf_arch_irq_thread_t, false); \
            }                                                                   \
        }                                                                       \
        void VSF_MCONNECT(__name, _callback_t)(vsf_arch_host_invoke_ctx_t(__name) *_)
#define imp_vsf_arch_host_invoke        implement_vsf_arch_host_invoke
#define initialize_vsf_arch_host_invoke(__name, __priority)                     \
        do {                                                                    \
            memset(&VSF_MCONNECT(__name, _instance), 0,                         \
                    sizeof(VSF_MCONNECT(__name, _t)));                          \
            __vsf_arch_irq_request_init(&VSF_MCONNECT(__name, _instance).request);\
            __vsf_arch_irq_init(                                                \
                    &VSF_MCONNECT(__name, _instance).use_as__vsf_arch_irq_thread_t,\
                    VSF_STR(__name), VSF_MCONNECT(__name, _thread), (__priority));\
        } while (0)
#define init_vsf_arch_host_invoke       initialize_vsf_arch_host_invoke
#define vsf_arch_host_invoke_nb(__name, __eda_ptr, __ctx_ptr)                   \
        do {                                                                    \
            VSF_MCONNECT(__name, _instance).ctx = *(__ctx_ptr);                 \
            VSF_MCONNECT(__name, _instance).caller = (__eda_ptr);               \
            __vsf_arch_irq_request_send(&VSF_MCONNECT(__name, _instance).request);\
        } while (0)
#define vsf_arch_host_invoke_get_result_nb(__name, __ctx_ptr)                   \
        do {                                                                    \
            *(__ctx_ptr) = VSF_MCONNECT(__name, _instance).ctx;                 \
        } while (0)
#define vsf_arch_host_invoke_in_thread(__name, __ctx_ptr)                       \
        do {                                                                    \
            vsf_eda_t VSF_MACRO_SAFE_NAME(eda) = vsf_eda_get_cur();             \
            VSF_ARCH_ASSERT(VSF_MACRO_SAFE_NAME(eda) != NULL);                  \
            vsf_arch_host_invoke_nb(__name, VSF_MACRO_SAFE_NAME(eda), (__ctx_ptr));\
            vsf_thread_wfe(VSF_EVT_USER);                                       \
            vsf_arch_host_invoke_get_result_nb(__name, __ctx_ptr);              \
        } while (0)

/*============================ PROTOTYPES ====================================*/

#if VSF_ARCH_PROVIDE_HEAP == ENABLED
extern void * vsf_arch_heap_malloc(uint_fast32_t size, uint_fast32_t alignment);
extern void * vsf_arch_heap_realloc(void *buffer, uint_fast32_t size);
extern void vsf_arch_heap_free(void *buffer);
extern unsigned int vsf_arch_heap_alignment(void);
extern uint_fast32_t vsf_arch_heap_size(void *buffer);
#   if VSF_ARCH_HEAP_HAS_STATISTICS == ENABLED
typedef struct vsf_arch_heap_statistics_t {
    uint32_t all_size;
    uint32_t used_size;
} vsf_arch_heap_statistics_t;
extern void vsf_arch_heap_statistics(vsf_arch_heap_statistics_t *statistics);
#   endif
#endif

/*----------------------------------------------------------------------------*
 * Architecture Infrastructure                                                *
 *----------------------------------------------------------------------------*/

#ifndef VSF_FFS32
// returns the first bits(start from 0) which is '1'
//  if all bits are 0s, -1 is returned
extern int_fast8_t vsf_ffs32(uint_fast32_t);
#endif

#ifndef VSF_FFZ32
extern int_fast8_t vsf_ffz32(uint_fast32_t);
#endif

#ifndef VSF_MSB32
extern int_fast8_t vsf_msb32(uint_fast32_t);
#endif

#ifndef VSF_CLZ32
extern uint_fast8_t vsf_clz32(uint_fast32_t);
#endif

#if VSF_APPLET_USE_ARCH == ENABLED
typedef struct vsf_arch_vplt_t {
    vsf_vplt_info_t info;

    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(bswap_16);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(bswap_32);
    VSF_APPLET_VPLT_ENTRY_FUNC_DEF(bswap_64);
} vsf_arch_vplt_t;
#   ifndef __VSF_APPLET__
extern __VSF_VPLT_DECORATOR__ vsf_arch_vplt_t vsf_arch_vplt;
#   endif
#endif

#if     defined(__VSF_APPLET__) && (defined(__VSF_APPLET_LIB__) || defined(__VSF_APPLET_ARCH_LIB__))\
    && VSF_APPLET_CFG_ABI_PATCH != ENABLED && VSF_APPLET_USE_ARCH == ENABLED

#ifndef VSF_APPLET_ARCH_VPLT
#   define VSF_APPLET_ARCH_VPLT                                                 \
            ((vsf_arch_vplt_t *)(((vsf_vplt_t *)vsf_vplt((void *)0))->arch_vplt))
#endif

#define VSF_APPLET_ARCH_ENTRY(__NAME)                                           \
            VSF_APPLET_VPLT_ENTRY_FUNC_ENTRY(VSF_APPLET_ARCH_VPLT, __NAME)
#define VSF_APPLET_ARCH_IMP(...)                                                \
            VSF_APPLET_VPLT_ENTRY_FUNC_IMP(VSF_APPLET_ARCH_VPLT, __VA_ARGS__)

VSF_APPLET_ARCH_IMP(bswap_16, uint_fast16_t, uint_fast16_t value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_ARCH_ENTRY(bswap_16)(value);
}
VSF_APPLET_ARCH_IMP(bswap_32, uint_fast32_t, uint_fast32_t value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_ARCH_ENTRY(bswap_32)(value);
}
VSF_APPLET_ARCH_IMP(bswap_64, uint_fast64_t, uint_fast64_t value) {
    VSF_APPLET_VPLT_ENTRY_FUNC_TRACE();
    return VSF_APPLET_ARCH_ENTRY(bswap_64)(value);
}

#else

extern uint_fast16_t bswap_16(uint_fast16_t);
extern uint_fast32_t bswap_32(uint_fast32_t);
#ifdef UINT64_MAX
extern uint_fast64_t bswap_64(uint_fast64_t);
#endif

DECLARE_ENDIAN_FUNC(16)
DECLARE_ENDIAN_FUNC(32)
#ifdef UINT64_MAX
DECLARE_ENDIAN_FUNC(64)
#endif

#endif

/*----------------------------------------------------------------------------*
 * SWI                                                                        *
 *----------------------------------------------------------------------------*/

extern vsf_err_t vsf_swi_init(  uint_fast8_t idx,
                                vsf_arch_prio_t priority,
                                vsf_swi_handler_t *handler,
                                void *param);
extern void vsf_swi_trigger(uint_fast8_t idx);

/*! \brief trigger a software interrupt
 *! \param idx the index of the software interrupt
 */
extern void vsf_arch_swi_trigger(uint_fast8_t idx);

/*----------------------------------------------------------------------------*
 * System Timer                                                               *
 *----------------------------------------------------------------------------*/

#ifdef VSF_SYSTIMER_CFG_IMPL_MODE
extern vsf_err_t vsf_systimer_start(void);
extern vsf_systimer_tick_t vsf_systimer_get(void);
extern bool vsf_systimer_set(vsf_systimer_tick_t due);
extern void vsf_systimer_set_idle(void);
extern bool vsf_systimer_is_due(vsf_systimer_tick_t due);
extern void vsf_systimer_prio_set(vsf_arch_prio_t priority);
extern uint32_t vsf_systimer_get_freq(void);

extern vsf_systimer_tick_t vsf_systimer_us_to_tick(uint_fast32_t time_us);
extern vsf_systimer_tick_t vsf_systimer_ms_to_tick(uint_fast32_t time_ms);
extern vsf_systimer_tick_t vsf_systimer_tick_to_ms(vsf_systimer_tick_t tick);
extern vsf_systimer_tick_t vsf_systimer_tick_to_us(vsf_systimer_tick_t tick);
#endif


/*----------------------------------------------------------------------------*
 * Interrupt                                                                  *
 *----------------------------------------------------------------------------*/

extern vsf_arch_prio_t vsf_set_base_priority(vsf_arch_prio_t priority);
extern vsf_arch_prio_t vsf_get_base_priority(void);

extern int vsf_get_interrupt_id(void);
extern vsf_gint_state_t vsf_get_interrupt(void);
extern vsf_gint_state_t vsf_set_interrupt(vsf_gint_state_t level);
extern vsf_gint_state_t vsf_disable_interrupt(void);
extern vsf_gint_state_t vsf_enable_interrupt(void);

extern void vsf_arch_sleep(uint_fast32_t mode);
extern void vsf_arch_reset(void);
extern void vsf_arch_shutdown(void);
// vsf_arch_main is only used in some special arch, eg: wasm
//  and normally used with VSF_ARCH_ENTRY_NO_PENDING
extern void vsf_arch_main(void);

#ifdef __cplusplus
}
#endif

#endif
/* EOF */
