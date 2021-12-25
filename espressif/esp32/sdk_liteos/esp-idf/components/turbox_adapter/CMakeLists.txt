if(BOOTLOADER_BUILD)
    # bootloader only needs FreeRTOS for config, not for anything else
    idf_component_register()
    return()
endif()

idf_build_get_property(target IDF_TARGET)

set(srcs
    "code/port/riscv/port.c"
    "code/port/riscv/portasm.S")

set(include_dirs
    include
    code/port/riscv/include)

set(private_include_dirs
    code/port/riscv/include/freertos
    code/port/riscv
    code/port/priv_include
    .)

list(APPEND srcs
    "code/esp_additions/task_snapshot.c"
    "code/port/port_common.c"
    "code/port/port_systick.c"
    "code/src/turbox_timer.c"
    "code/src/turbox_buffer.c"
    "code/src/turbox_event.c"
    "code/src/turbox_memory.c"
    "code/src/turbox_msg_buffer.c"
    "code/src/turbox_queue.c"
    "code/src/turbox_sem.c"
    "code/src/turbox_stream_buffer.c"
    "code/src/turbox_task.c")
list(APPEND include_dirs
    "include/esp_additions")
list(APPEND private_include_dirs
    "include/freertos"
    "../../../../../../../kernel/liteos_m/utils" 
    "../../../../../../../kernel/liteos_m/kal/cmsis"
    "../../../../../../../kernel/liteos_m/kernel/include"
    "../../../../../../../kernel/liteos_m/utils"
    "../../../../../../../kernel/liteos_m/kernel/arch/risc-v/riscv32/gcc"
    "../../../../../../../kernel/liteos_m/kernel/arch/risc-v/riscv32/gcc/asm"
    "../../../../../../../kernel/liteos_m/targets/esp32"
    "../../../../../../../kernel/liteos_m/targets/esp32/include"
    "../../../../../../../kernel/liteos_m/targets/esp32/include/asm"
    "../../../../../../../utils/native/lite/include"
    "../../../../../../../kernel/liteos_m/kernel/arch/include")

set(required_components esp_system esp_timer app_trace heap harmony)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${include_dirs}
                    PRIV_INCLUDE_DIRS  ${private_include_dirs}
                    LDFRAGMENTS linker.lf
                    REQUIRES ${required_components}
                    PRIV_REQUIRES soc)

idf_component_get_property(COMPONENT_DIR turbox_adapter COMPONENT_DIR)
idf_component_set_property(turbox_adapter ORIG_INCLUDE_PATH "${COMPONENT_DIR}/include/freertos/")

if(CONFIG_FREERTOS_DEBUG_OCDAWARE)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-Wl,--undefined=uxTopUsedPriority")
endif()
# The freertos component provides the `start_app` and `start_app_other_cores`
# if it is included in the build. It then calls `app_main`
# from the main task created, which must be provided by the user.
# Like for `start_app` and `start_app_other_cores`,
# we can't establish dependency on what we don't yet know, so we force the
# linker to not drop this symbol.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u app_main")
