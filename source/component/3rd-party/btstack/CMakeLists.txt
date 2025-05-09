if(NOT VSF_BTSTACK_PATH)
    set(VSF_BTSTACK_PATH ${VSF_SRC_PATH}/component/3rd-party/btstack/raw)
endif()

vsf_add_include_directories(
    ${VSF_BTSTACK_PATH}/src
    ${VSF_BTSTACK_PATH}/chipset
    ${VSF_BTSTACK_PATH}/3rd-party/bluedroid/encoder/include
    ${VSF_BTSTACK_PATH}/3rd-party/bluedroid/decoder/include
    ${VSF_BTSTACK_PATH}/3rd-party/yxml
)
#vsf_add_compile_definitions(
#)

if(VSF_BTSTACK_USE_BCM20702)
    vsf_add_sources(
        firmware/bcm/bcm20702/bt_firmware_image.c
        port/hci_transport_h2_vsf.c
        port/hci_transport_h4_vsf.c

        ${VSF_BTSTACK_PATH}/chipset/bcm/btstack_chipset_bcm.c
        ${VSF_BTSTACK_PATH}/chipset/bcm/btstack_chipset_bcm_download_firmware.c
    )
endif()

if(VSF_BTSTACK_USE_CSR)
    vsf_add_sources(
        ${VSF_BTSTACK_PATH}/chipset/csr/btstack_chipset_csr.c
    )
endif()

if(VSF_BTSTACK_USE_LINKKEY_MEMORY)
    vsf_add_sources(
        ${VSF_BTSTACK_PATH}/src/classic/btstack_link_key_db_memory.c
    )
endif()

vsf_add_sources(
    port/btstack_run_loop_vsf.c
    port/hci_dump_vsf.c

    ${VSF_BTSTACK_PATH}/src/classic/hid_device.c
    ${VSF_BTSTACK_PATH}/src/classic/rfcomm.c
    ${VSF_BTSTACK_PATH}/src/classic/sdp_server.c
    ${VSF_BTSTACK_PATH}/src/classic/sdp_util.c
    ${VSF_BTSTACK_PATH}/src/classic/spp_server.c

    ${VSF_BTSTACK_PATH}/src/btstack_hid_parser.c
    ${VSF_BTSTACK_PATH}/src/ad_parser.c
    ${VSF_BTSTACK_PATH}/src/btstack_linked_list.c
    ${VSF_BTSTACK_PATH}/src/btstack_memory.c
    ${VSF_BTSTACK_PATH}/src/btstack_memory_pool.c
    ${VSF_BTSTACK_PATH}/src/btstack_run_loop.c
    ${VSF_BTSTACK_PATH}/src/btstack_util.c
    ${VSF_BTSTACK_PATH}/src/hci.c
    ${VSF_BTSTACK_PATH}/src/hci_cmd.c
    ${VSF_BTSTACK_PATH}/src/hci_dump.c
    ${VSF_BTSTACK_PATH}/src/l2cap.c
    ${VSF_BTSTACK_PATH}/src/l2cap_signaling.c
)

if(VSF_BTSTACK_CONFIG_PATH)
    message(STATUS "VSF_BTSTACK_CONFIG_PATH: ${VSF_BTSTACK_CONFIG_PATH}")
    vsf_add_include_directories(
        ${VSF_BTSTACK_CONFIG_PATH}
    )
endif()
