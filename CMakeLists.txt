set(PROJECT pico-dual-cdc)
cmake_minimum_required(VERSION 3.5)

set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(${PROJECT} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_executable(${PROJECT}
    src/led.c
    src/main.c
    src/usb_descriptors.c
)
target_include_directories(${PROJECT} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include   # So pico-sdk can find our tusb_config.h
)
target_link_libraries(${PROJECT}
    pico_stdlib
)

if (PICO_CYW43_SUPPORTED)
    # The LED on the Pico-W is attached to the CYW43 chip, so we need to include that
    # in order to blink the LED. The cyw43 module adds 250K of size to the binary so
    # we can't run it from RAM.
    target_link_libraries(${PROJECT} pico_cyw43_arch_none)
else()
    # On the normal pico, we can run from RAM.
    pico_set_binary_type(${PROJECT} no_flash)
endif()

add_compile_definitions(
   PICO_STDIO_USB_USE_DEFAULT_DESCRIPTORS=0             # We provide our own USB descriptors
   PICO_STDIO_USB_ENABLE_RESET_VIA_VENDOR_INTERFACE=1   # Enable the USB Reset feature
)

pico_add_extra_outputs(${PROJECT})      # Builds the .uf2 and other files.
pico_enable_stdio_usb(${PROJECT} 1)     # Enable stdio over USB interface 0.
pico_enable_stdio_uart(${PROJECT} 0)    # Disable stdio over UART.


# Convenience target for flashing.
add_custom_target(flash
    COMMENT "Flashing ${PROJECT} to target device..."
    COMMAND picotool load -x -f ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}.uf2
)
add_dependencies(flash ${PROJECT})
