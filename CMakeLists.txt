SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake")
list(APPEND CMAKE_MODULE_PATH cmake)
include(cmake/functions.cmake)

SET(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/Core/1986ve92.ld")

set(FPU_FLAGS "")

SET(COMMON_FLAGS
        "-mcpu=cortex-m3 -mthumb ${FPU_FLAGS} -ffunction-sections -fdata-sections \
         -g -fno-common -fmessage-length=0 -specs=nano.specs")

SET(CMAKE_C_FLAGS_INIT "${COMMON_FLAGS} -std=gnu11")
SET(CMAKE_CXX_FLAGS_INIT "${COMMON_FLAGS} -std=gnu++14 -fno-rtti -fno-exceptions -fno-threadsafe-statics")
SET(CMAKE_EXE_LINKER_FLAGS_INIT "-Wl,-gc-sections,--print-memory-usage")

project(MilandrBase C CXX ASM)

# TODO Раскомментировать для подключение Python
#find_package(Python3 COMPONENTS Interpreter REQUIRED)
#set(PYTHON_VENV_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/venv/Scripts/python)
#add_custom_command(
#        OUTPUT venv
#        COMMAND ${Python3_EXECUTABLE} -m venv venv
#)
#add_custom_target(
#        venv.stamp
#        DEPENDS venv Tools/requirements.txt
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Tools/requirements.txt requirements.txt
#        COMMAND ${PYTHON_VENV_EXECUTABLE} -m pip install -r requirements.txt --upgrade
#)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(POSTFIX "d")
    add_definitions("-DCONFIG_LOG_MAXIMUM_LEVEL=MDR_LOG_VERBOSE")
    add_definitions("-DDEBUG")
    add_definitions("-DUSE_ASSERT_INFO=2")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(POSTFIX "")
    add_definitions("-DCONFIG_LOG_MAXIMUM_LEVEL=MDR_LOG_NONE")
else()
    message(WARNING "Unknown build type")
    message(FATAL_ERROR "Set CMAKE_BUILD_TYPE to Debug or Release and reconfigure")
endif()


option(DISABLE_START_CONTROL "Disable Start control" OFF)
if(DISABLE_START_CONTROL)
    add_definitions("-DDISABLE_START_CONTROL")
endif(DISABLE_START_CONTROL)

# Исходные коды библиотек
set(SPL_SRC
        "Drivers/SPL/src/MDR32F9Qx_port.c"
        "Drivers/SPL/src/MDR32F9Qx_rst_clk.c"
        "Drivers/SPL/src/MDR32F9Qx_eeprom.c"
        "Drivers/SPL/src/MDR32F9Qx_timer.c"
        "Drivers/SPL/src/MDR32F9Qx_usb.c"
        "Drivers/SPL/src/MDR32F9Qx_ssp.c"
        "Drivers/SPL/src/MDR32F9Qx_dma.c"
        "Drivers/SPL/src/MDR32F9Qx_i2c.c"
        "Drivers/SPL/src/USB_Library/MDR32F9Qx_usb_device.c"
        "Drivers/SPL/src/USB_Library/MDR32F9Qx_usb_HID.c"
    )

set(SEGGER_SRC
        "Middlewares/SEGGER/SEGGER_RTT.c"
        "Middlewares/SEGGER/SEGGER_RTT_printf.c"
    )

set(FREERTOS_SRC
        "Middlewares/FreeRTOS/Source/tasks.c"
        "Middlewares/FreeRTOS/Source/list.c"
        "Middlewares/FreeRTOS/Source/queue.c"
        "Middlewares/FreeRTOS/Source/timers.c"
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c"
        "Middlewares/FreeRTOS/Source/portable/MemMang/heap_4.c"

        # Exception handlers
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM3/exception_handlers.c"
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM3/initialize_hardware.c"
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM3/reset_hardware.c"
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM3/startup.c"
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM3/vectors.c"

        # Newlib stubs
        "Middlewares/FreeRTOS/Source/portable/GCC/newlib/_cxx.cpp"
        "Middlewares/FreeRTOS/Source/portable/GCC/newlib/_exit.c"
        "Middlewares/FreeRTOS/Source/portable/GCC/newlib/_sbrk.c"
        "Middlewares/FreeRTOS/Source/portable/GCC/newlib/_syscalls.c"
        )

set(FREERTOS_INC
        "Middlewares/FreeRTOS/Source/include"
        "Middlewares/FreeRTOS/Source/portable/GCC/ARM_CM3"
        )

set(LOGGING_SRC
        "Middlewares/logging/log.cpp"
        "Middlewares/logging/log_freertos.cpp"
        "Middlewares/logging/log_buffers.cpp"
    )


set(IICSLAVE_SRC
        "Middlewares/iicslave/iicslave.cpp"
    )


set(COMMON_SRC ${HAL_LL_SRC} ${SEGGER_SRC} ${LOGGING_SRC} ${STARTUP_SRC} ${MACS_TARGET_SRC} ${SPL_SRC}
        ${IICSLAVE_SRC} ${FREERTOS_SRC})

set(STARTUP_INC "startup")
set(SPL_INC "Drivers/SPL/" "Drivers/SPL/inc" "Drivers/SPL/inc/USB_Library")
set(CMSIS_INC "Drivers/CMSIS/MDR32Fx/CoreSupport/CM3" "Drivers/CMSIS/MDR32Fx/DeviceSupport/MDR1986VE9x/inc")
set(APP_INC "Core/inc")
set(SEGGER_INC "Middlewares/SEGGER")
set(LOGGING_INC "Middlewares/logging" "Middlewares/logging/include")
set(IICSLAVE_INC "Middlewares/iicslave")

include_directories(${STARTUP_INC})
include_directories(${CMSIS_INC})
include_directories(${SPL_INC})
include_directories(${APP_INC})
include_directories(${SEGGER_INC})
include_directories(${LOGGING_INC})
include_directories(${IICSLAVE_INC})
include_directories(${FREERTOS_INC})

set(COMMON_DEFINITIONS -DMDR1986VE9=1 -DUSE_MDR1986VE92)

add_definitions(${COMMON_DEFINITIONS})

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DUSE_FULL_ASSERT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DUSE_FULL_ASSERT")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

set(PROJECT_SRC
        "Core/src/main.cpp"
        "Core/src/main_app.cpp"
        "Core/src/SSPDmaTask.cpp"
        "Core/src/SSPIrqTask.cpp"
        "Core/src/SSPPollTask.cpp"
        "Core/src/SSPSlaveTask.cpp"
        "Core/src/IICSlaveTask.cpp"
        "Core/src/IICMasterTask.cpp"
        "Core/src/system_MDR32F9Qx.c"
        "Core/src/errors.cpp"
    )

set(FLASH_START_ADDRESS "0x08000000")
set(FLASH_SIZE "131072")

set(PN "MilandrBase${POSTFIX}")
add_executable(${PN}.elf ${PROJECT_SRC} ${COMMON_SRC})
set_options(${PN} ${LINKER_SCRIPT} ${FLASH_START_ADDRESS} ${FLASH_SIZE})
target_compile_definitions(${PN}.elf PUBLIC "SEAL_LEVEL=1")
# TODO Раскомментировать для зависимости от Python
# add_dependencies(${PN}.elf venv.stamp)

