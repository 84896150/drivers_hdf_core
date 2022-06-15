/*
 * This is an automatically generated HDF config file. Do not modify it manually.
 */

#ifndef HCS_CONFIG_HDF_MACRO_TEST_HEADER_H
#define HCS_CONFIG_HDF_MACRO_TEST_HEADER_H

#include "hdf_base.h"

// hcs node macros: root
#define root_exists 1
#define root_foreach_child(func) \
    func(root_device_info) \
    func(root_platform) \
    func(root_input_config) \
    func(root_wlan_config) \
    func(root_accel_bmi160_chip_config) \
    func(root_sensorConfig) \
    func(root_als_bh1745_chip_config) \
    func(root_gyro_bmi160_chip_config) \
    func(root_barometer_bmp180_chip_config) \
    func(root_hall_ak8789_chip_config) \
    func(root_magnetic_lsm303_chip_config) \
    func(root_proximity_apds9960_chip_config) \
    func(root_gravity_vitural_config) \
    func(root_platfrom) \
    func(root_lightConfig) \
    func(root_vibratorConfig) \
    func(root_linearVibratorConfig) \
    func(root_backlightConfig) \
    func(root_audio_info) \
    func(root_fingerprint_info) \
    func(root_data_type_test) \
    func(root_fingerprint_three) \
    func(root_gpio_config) \
    func(root_i2c_config) \
    func(root_pwm_test) \
    func(root_spi_test) \
    func(root_sdio_config) \
    func(root_emmc_config) \
    func(root_uart_test)

#define root_foreach_child_vargs(func, ...) \
    func(root_device_info, __VA_ARGS__) \
    func(root_platform, __VA_ARGS__) \
    func(root_input_config, __VA_ARGS__) \
    func(root_wlan_config, __VA_ARGS__) \
    func(root_accel_bmi160_chip_config, __VA_ARGS__) \
    func(root_sensorConfig, __VA_ARGS__) \
    func(root_als_bh1745_chip_config, __VA_ARGS__) \
    func(root_gyro_bmi160_chip_config, __VA_ARGS__) \
    func(root_barometer_bmp180_chip_config, __VA_ARGS__) \
    func(root_hall_ak8789_chip_config, __VA_ARGS__) \
    func(root_magnetic_lsm303_chip_config, __VA_ARGS__) \
    func(root_proximity_apds9960_chip_config, __VA_ARGS__) \
    func(root_gravity_vitural_config, __VA_ARGS__) \
    func(root_platfrom, __VA_ARGS__) \
    func(root_lightConfig, __VA_ARGS__) \
    func(root_vibratorConfig, __VA_ARGS__) \
    func(root_linearVibratorConfig, __VA_ARGS__) \
    func(root_backlightConfig, __VA_ARGS__) \
    func(root_audio_info, __VA_ARGS__) \
    func(root_fingerprint_info, __VA_ARGS__) \
    func(root_data_type_test, __VA_ARGS__) \
    func(root_fingerprint_three, __VA_ARGS__) \
    func(root_gpio_config, __VA_ARGS__) \
    func(root_i2c_config, __VA_ARGS__) \
    func(root_pwm_test, __VA_ARGS__) \
    func(root_spi_test, __VA_ARGS__) \
    func(root_sdio_config, __VA_ARGS__) \
    func(root_emmc_config, __VA_ARGS__) \
    func(root_uart_test, __VA_ARGS__)

#define root_module_exists 1
#define root_module "hisilicon,hi35xx_chip"

// hcs node macros: root/device_info
#define root_device_info_exists 1
#define root_device_info_foreach_child(func) \
    func(root_device_info_platform) \
    func(root_device_info_display) \
    func(root_device_info_input) \
    func(root_device_info_network) \
    func(root_device_info_sensor) \
    func(root_device_info_usb_pnp_linux) \
    func(root_device_info_audio) \
    func(root_device_info_vibrator) \
    func(root_device_info_dsoftbus) \
    func(root_device_info_light) \
    func(root_device_info_test_host)

#define root_device_info_foreach_child_vargs(func, ...) \
    func(root_device_info_platform, __VA_ARGS__) \
    func(root_device_info_display, __VA_ARGS__) \
    func(root_device_info_input, __VA_ARGS__) \
    func(root_device_info_network, __VA_ARGS__) \
    func(root_device_info_sensor, __VA_ARGS__) \
    func(root_device_info_usb_pnp_linux, __VA_ARGS__) \
    func(root_device_info_audio, __VA_ARGS__) \
    func(root_device_info_vibrator, __VA_ARGS__) \
    func(root_device_info_dsoftbus, __VA_ARGS__) \
    func(root_device_info_light, __VA_ARGS__) \
    func(root_device_info_test_host, __VA_ARGS__)

#define root_device_info_match_attr_exists 1
#define root_device_info_match_attr "hdf_manager"

// hcs node macros: root/device_info/platform
#define root_device_info_platform_exists 1
#define root_device_info_platform_foreach_child(func) \
    func(root_device_info_platform_device_gpio) \
    func(root_device_info_platform_device_watchdog) \
    func(root_device_info_platform_device_rtc) \
    func(root_device_info_platform_device_uart) \
    func(root_device_info_platform_device_i2c) \
    func(root_device_info_platform_device_spi) \
    func(root_device_info_platform_device_sdio) \
    func(root_device_info_platform_device_emmc) \
    func(root_device_info_platform_device_pwm) \
    func(root_device_info_platform_device_mipi_dsi)

#define root_device_info_platform_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_gpio, __VA_ARGS__) \
    func(root_device_info_platform_device_watchdog, __VA_ARGS__) \
    func(root_device_info_platform_device_rtc, __VA_ARGS__) \
    func(root_device_info_platform_device_uart, __VA_ARGS__) \
    func(root_device_info_platform_device_i2c, __VA_ARGS__) \
    func(root_device_info_platform_device_spi, __VA_ARGS__) \
    func(root_device_info_platform_device_sdio, __VA_ARGS__) \
    func(root_device_info_platform_device_emmc, __VA_ARGS__) \
    func(root_device_info_platform_device_pwm, __VA_ARGS__) \
    func(root_device_info_platform_device_mipi_dsi, __VA_ARGS__)

#define root_device_info_platform_hostName_exists 1
#define root_device_info_platform_hostName "platform_host"
#define root_device_info_platform_priority_exists 1
#define root_device_info_platform_priority 50

// hcs node macros: root/device_info/platform/device_gpio
#define root_device_info_platform_device_gpio_exists 1
#define root_device_info_platform_device_gpio_foreach_child(func) \
    func(root_device_info_platform_device_gpio_device0) \
    func(root_device_info_platform_device_gpio_device1)

#define root_device_info_platform_device_gpio_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_gpio_device0, __VA_ARGS__) \
    func(root_device_info_platform_device_gpio_device1, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_gpio/device0
#define root_device_info_platform_device_gpio_device0_exists 1
#define root_device_info_platform_device_gpio_device0_policy_exists 1
#define root_device_info_platform_device_gpio_device0_policy 2
#define root_device_info_platform_device_gpio_device0_priority_exists 1
#define root_device_info_platform_device_gpio_device0_priority 10
#define root_device_info_platform_device_gpio_device0_permission_exists 1
#define root_device_info_platform_device_gpio_device0_permission 420
#define root_device_info_platform_device_gpio_device0_moduleName_exists 1
#define root_device_info_platform_device_gpio_device0_moduleName "HDF_PLATFORM_GPIO_MANAGER"
#define root_device_info_platform_device_gpio_device0_serviceName_exists 1
#define root_device_info_platform_device_gpio_device0_serviceName "HDF_PLATFORM_GPIO_MANAGER"
#define root_device_info_platform_device_gpio_device0_preload_exists 1
#define root_device_info_platform_device_gpio_device0_preload 0
#define root_device_info_platform_device_gpio_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_gpio_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/platform/device_gpio/device1
#define root_device_info_platform_device_gpio_device1_exists 1
#define root_device_info_platform_device_gpio_device1_policy_exists 1
#define root_device_info_platform_device_gpio_device1_policy 0
#define root_device_info_platform_device_gpio_device1_priority_exists 1
#define root_device_info_platform_device_gpio_device1_priority 10
#define root_device_info_platform_device_gpio_device1_permission_exists 1
#define root_device_info_platform_device_gpio_device1_permission 420
#define root_device_info_platform_device_gpio_device1_moduleName_exists 1
#define root_device_info_platform_device_gpio_device1_moduleName "linux_gpio_adapter"
#define root_device_info_platform_device_gpio_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_gpio_device1_deviceMatchAttr "linux_gpio_adapter"
#define root_device_info_platform_device_gpio_device1_preload_exists 1
#define root_device_info_platform_device_gpio_device1_preload 0
#define root_device_info_platform_device_gpio_device1_serviceName_exists 1
#define root_device_info_platform_device_gpio_device1_serviceName ""

// hcs node macros: root/device_info/platform/device_watchdog
#define root_device_info_platform_device_watchdog_exists 1
#define root_device_info_platform_device_watchdog_foreach_child(func) \
    func(root_device_info_platform_device_watchdog_device0)

#define root_device_info_platform_device_watchdog_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_watchdog_device0, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_watchdog/device0
#define root_device_info_platform_device_watchdog_device0_exists 1
#define root_device_info_platform_device_watchdog_device0_policy_exists 1
#define root_device_info_platform_device_watchdog_device0_policy 1
#define root_device_info_platform_device_watchdog_device0_priority_exists 1
#define root_device_info_platform_device_watchdog_device0_priority 20
#define root_device_info_platform_device_watchdog_device0_permission_exists 1
#define root_device_info_platform_device_watchdog_device0_permission 420
#define root_device_info_platform_device_watchdog_device0_moduleName_exists 1
#define root_device_info_platform_device_watchdog_device0_moduleName "HDF_PLATFORM_WATCHDOG"
#define root_device_info_platform_device_watchdog_device0_serviceName_exists 1
#define root_device_info_platform_device_watchdog_device0_serviceName "HDF_PLATFORM_WATCHDOG_0"
#define root_device_info_platform_device_watchdog_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_watchdog_device0_deviceMatchAttr "hisilicon_hi35xx_watchdog_0"
#define root_device_info_platform_device_watchdog_device0_preload_exists 1
#define root_device_info_platform_device_watchdog_device0_preload 0

// hcs node macros: root/device_info/platform/device_rtc
#define root_device_info_platform_device_rtc_exists 1
#define root_device_info_platform_device_rtc_foreach_child(func) \
    func(root_device_info_platform_device_rtc_device0)

#define root_device_info_platform_device_rtc_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_rtc_device0, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_rtc/device0
#define root_device_info_platform_device_rtc_device0_exists 1
#define root_device_info_platform_device_rtc_device0_policy_exists 1
#define root_device_info_platform_device_rtc_device0_policy 1
#define root_device_info_platform_device_rtc_device0_priority_exists 1
#define root_device_info_platform_device_rtc_device0_priority 30
#define root_device_info_platform_device_rtc_device0_permission_exists 1
#define root_device_info_platform_device_rtc_device0_permission 420
#define root_device_info_platform_device_rtc_device0_moduleName_exists 1
#define root_device_info_platform_device_rtc_device0_moduleName "HDF_PLATFORM_RTC"
#define root_device_info_platform_device_rtc_device0_serviceName_exists 1
#define root_device_info_platform_device_rtc_device0_serviceName "HDF_PLATFORM_RTC"
#define root_device_info_platform_device_rtc_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_rtc_device0_deviceMatchAttr "hisilicon_hi35xx_rtc"
#define root_device_info_platform_device_rtc_device0_preload_exists 1
#define root_device_info_platform_device_rtc_device0_preload 0

// hcs node macros: root/device_info/platform/device_uart
#define root_device_info_platform_device_uart_exists 1
#define root_device_info_platform_device_uart_foreach_child(func) \
    func(root_device_info_platform_device_uart_device0) \
    func(root_device_info_platform_device_uart_device1)

#define root_device_info_platform_device_uart_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_uart_device0, __VA_ARGS__) \
    func(root_device_info_platform_device_uart_device1, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_uart/device0
#define root_device_info_platform_device_uart_device0_exists 1
#define root_device_info_platform_device_uart_device0_policy_exists 1
#define root_device_info_platform_device_uart_device0_policy 1
#define root_device_info_platform_device_uart_device0_priority_exists 1
#define root_device_info_platform_device_uart_device0_priority 40
#define root_device_info_platform_device_uart_device0_permission_exists 1
#define root_device_info_platform_device_uart_device0_permission 420
#define root_device_info_platform_device_uart_device0_moduleName_exists 1
#define root_device_info_platform_device_uart_device0_moduleName "HDF_PLATFORM_UART"
#define root_device_info_platform_device_uart_device0_serviceName_exists 1
#define root_device_info_platform_device_uart_device0_serviceName "HDF_PLATFORM_UART_0"
#define root_device_info_platform_device_uart_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_uart_device0_deviceMatchAttr "hisilicon_hi35xx_uart_0"
#define root_device_info_platform_device_uart_device0_preload_exists 1
#define root_device_info_platform_device_uart_device0_preload 0

// hcs node macros: root/device_info/platform/device_uart/device1
#define root_device_info_platform_device_uart_device1_exists 1
#define root_device_info_platform_device_uart_device1_policy_exists 1
#define root_device_info_platform_device_uart_device1_policy 2
#define root_device_info_platform_device_uart_device1_permission_exists 1
#define root_device_info_platform_device_uart_device1_permission 420
#define root_device_info_platform_device_uart_device1_priority_exists 1
#define root_device_info_platform_device_uart_device1_priority 40
#define root_device_info_platform_device_uart_device1_moduleName_exists 1
#define root_device_info_platform_device_uart_device1_moduleName "HDF_PLATFORM_UART"
#define root_device_info_platform_device_uart_device1_serviceName_exists 1
#define root_device_info_platform_device_uart_device1_serviceName "HDF_PLATFORM_UART_1"
#define root_device_info_platform_device_uart_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_uart_device1_deviceMatchAttr "hisilicon_hi35xx_uart_1"
#define root_device_info_platform_device_uart_device1_preload_exists 1
#define root_device_info_platform_device_uart_device1_preload 0

// hcs node macros: root/device_info/platform/device_i2c
#define root_device_info_platform_device_i2c_exists 1
#define root_device_info_platform_device_i2c_foreach_child(func) \
    func(root_device_info_platform_device_i2c_device0) \
    func(root_device_info_platform_device_i2c_device1)

#define root_device_info_platform_device_i2c_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_i2c_device0, __VA_ARGS__) \
    func(root_device_info_platform_device_i2c_device1, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_i2c/device0
#define root_device_info_platform_device_i2c_device0_exists 1
#define root_device_info_platform_device_i2c_device0_policy_exists 1
#define root_device_info_platform_device_i2c_device0_policy 2
#define root_device_info_platform_device_i2c_device0_priority_exists 1
#define root_device_info_platform_device_i2c_device0_priority 50
#define root_device_info_platform_device_i2c_device0_permission_exists 1
#define root_device_info_platform_device_i2c_device0_permission 420
#define root_device_info_platform_device_i2c_device0_moduleName_exists 1
#define root_device_info_platform_device_i2c_device0_moduleName "HDF_PLATFORM_I2C_MANAGER"
#define root_device_info_platform_device_i2c_device0_serviceName_exists 1
#define root_device_info_platform_device_i2c_device0_serviceName "HDF_PLATFORM_I2C_MANAGER"
#define root_device_info_platform_device_i2c_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_i2c_device0_deviceMatchAttr "hdf_platform_i2c_manager"
#define root_device_info_platform_device_i2c_device0_preload_exists 1
#define root_device_info_platform_device_i2c_device0_preload 0

// hcs node macros: root/device_info/platform/device_i2c/device1
#define root_device_info_platform_device_i2c_device1_exists 1
#define root_device_info_platform_device_i2c_device1_policy_exists 1
#define root_device_info_platform_device_i2c_device1_policy 0
#define root_device_info_platform_device_i2c_device1_priority_exists 1
#define root_device_info_platform_device_i2c_device1_priority 55
#define root_device_info_platform_device_i2c_device1_permission_exists 1
#define root_device_info_platform_device_i2c_device1_permission 420
#define root_device_info_platform_device_i2c_device1_moduleName_exists 1
#define root_device_info_platform_device_i2c_device1_moduleName "linux_i2c_adapter"
#define root_device_info_platform_device_i2c_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_i2c_device1_deviceMatchAttr "linux_i2c_adapter"
#define root_device_info_platform_device_i2c_device1_preload_exists 1
#define root_device_info_platform_device_i2c_device1_preload 0
#define root_device_info_platform_device_i2c_device1_serviceName_exists 1
#define root_device_info_platform_device_i2c_device1_serviceName ""

// hcs node macros: root/device_info/platform/device_spi
#define root_device_info_platform_device_spi_exists 1
#define root_device_info_platform_device_spi_foreach_child(func) \
    func(root_device_info_platform_device_spi_device0) \
    func(root_device_info_platform_device_spi_device1) \
    func(root_device_info_platform_device_spi_device2)

#define root_device_info_platform_device_spi_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_spi_device0, __VA_ARGS__) \
    func(root_device_info_platform_device_spi_device1, __VA_ARGS__) \
    func(root_device_info_platform_device_spi_device2, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_spi/device0
#define root_device_info_platform_device_spi_device0_exists 1
#define root_device_info_platform_device_spi_device0_policy_exists 1
#define root_device_info_platform_device_spi_device0_policy 1
#define root_device_info_platform_device_spi_device0_priority_exists 1
#define root_device_info_platform_device_spi_device0_priority 60
#define root_device_info_platform_device_spi_device0_permission_exists 1
#define root_device_info_platform_device_spi_device0_permission 420
#define root_device_info_platform_device_spi_device0_moduleName_exists 1
#define root_device_info_platform_device_spi_device0_moduleName "HDF_PLATFORM_SPI"
#define root_device_info_platform_device_spi_device0_serviceName_exists 1
#define root_device_info_platform_device_spi_device0_serviceName "HDF_PLATFORM_SPI_0"
#define root_device_info_platform_device_spi_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_spi_device0_deviceMatchAttr "hisilicon_hi35xx_spi_0"
#define root_device_info_platform_device_spi_device0_preload_exists 1
#define root_device_info_platform_device_spi_device0_preload 0

// hcs node macros: root/device_info/platform/device_spi/device1
#define root_device_info_platform_device_spi_device1_exists 1
#define root_device_info_platform_device_spi_device1_policy_exists 1
#define root_device_info_platform_device_spi_device1_policy 1
#define root_device_info_platform_device_spi_device1_priority_exists 1
#define root_device_info_platform_device_spi_device1_priority 60
#define root_device_info_platform_device_spi_device1_permission_exists 1
#define root_device_info_platform_device_spi_device1_permission 420
#define root_device_info_platform_device_spi_device1_moduleName_exists 1
#define root_device_info_platform_device_spi_device1_moduleName "HDF_PLATFORM_SPI"
#define root_device_info_platform_device_spi_device1_serviceName_exists 1
#define root_device_info_platform_device_spi_device1_serviceName "HDF_PLATFORM_SPI_1"
#define root_device_info_platform_device_spi_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_spi_device1_deviceMatchAttr "hisilicon_hi35xx_spi_1"
#define root_device_info_platform_device_spi_device1_preload_exists 1
#define root_device_info_platform_device_spi_device1_preload 0

// hcs node macros: root/device_info/platform/device_spi/device2
#define root_device_info_platform_device_spi_device2_exists 1
#define root_device_info_platform_device_spi_device2_policy_exists 1
#define root_device_info_platform_device_spi_device2_policy 1
#define root_device_info_platform_device_spi_device2_priority_exists 1
#define root_device_info_platform_device_spi_device2_priority 60
#define root_device_info_platform_device_spi_device2_permission_exists 1
#define root_device_info_platform_device_spi_device2_permission 420
#define root_device_info_platform_device_spi_device2_moduleName_exists 1
#define root_device_info_platform_device_spi_device2_moduleName "HDF_PLATFORM_SPI"
#define root_device_info_platform_device_spi_device2_serviceName_exists 1
#define root_device_info_platform_device_spi_device2_serviceName "HDF_PLATFORM_SPI_2"
#define root_device_info_platform_device_spi_device2_deviceMatchAttr_exists 1
#define root_device_info_platform_device_spi_device2_deviceMatchAttr "hisilicon_hi35xx_spi_2"
#define root_device_info_platform_device_spi_device2_preload_exists 1
#define root_device_info_platform_device_spi_device2_preload 0

// hcs node macros: root/device_info/platform/device_sdio
#define root_device_info_platform_device_sdio_exists 1
#define root_device_info_platform_device_sdio_foreach_child(func) \
    func(root_device_info_platform_device_sdio_device0)

#define root_device_info_platform_device_sdio_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_sdio_device0, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_sdio/device0
#define root_device_info_platform_device_sdio_device0_exists 1
#define root_device_info_platform_device_sdio_device0_policy_exists 1
#define root_device_info_platform_device_sdio_device0_policy 1
#define root_device_info_platform_device_sdio_device0_priority_exists 1
#define root_device_info_platform_device_sdio_device0_priority 70
#define root_device_info_platform_device_sdio_device0_permission_exists 1
#define root_device_info_platform_device_sdio_device0_permission 420
#define root_device_info_platform_device_sdio_device0_moduleName_exists 1
#define root_device_info_platform_device_sdio_device0_moduleName "HDF_PLATFORM_SDIO"
#define root_device_info_platform_device_sdio_device0_serviceName_exists 1
#define root_device_info_platform_device_sdio_device0_serviceName "HDF_PLATFORM_MMC_2"
#define root_device_info_platform_device_sdio_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_sdio_device0_deviceMatchAttr "hisilicon_hi35xx_sdio_0"
#define root_device_info_platform_device_sdio_device0_preload_exists 1
#define root_device_info_platform_device_sdio_device0_preload 0

// hcs node macros: root/device_info/platform/device_emmc
#define root_device_info_platform_device_emmc_exists 1
#define root_device_info_platform_device_emmc_foreach_child(func) \
    func(root_device_info_platform_device_emmc_device0)

#define root_device_info_platform_device_emmc_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_emmc_device0, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_emmc/device0
#define root_device_info_platform_device_emmc_device0_exists 1
#define root_device_info_platform_device_emmc_device0_policy_exists 1
#define root_device_info_platform_device_emmc_device0_policy 2
#define root_device_info_platform_device_emmc_device0_priority_exists 1
#define root_device_info_platform_device_emmc_device0_priority 20
#define root_device_info_platform_device_emmc_device0_permission_exists 1
#define root_device_info_platform_device_emmc_device0_permission 420
#define root_device_info_platform_device_emmc_device0_moduleName_exists 1
#define root_device_info_platform_device_emmc_device0_moduleName "HDF_PLATFORM_EMMC"
#define root_device_info_platform_device_emmc_device0_serviceName_exists 1
#define root_device_info_platform_device_emmc_device0_serviceName "HDF_PLATFORM_MMC_0"
#define root_device_info_platform_device_emmc_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_emmc_device0_deviceMatchAttr "hisilicon_hi35xx_emmc_0"
#define root_device_info_platform_device_emmc_device0_preload_exists 1
#define root_device_info_platform_device_emmc_device0_preload 0

// hcs node macros: root/device_info/platform/device_pwm
#define root_device_info_platform_device_pwm_exists 1
#define root_device_info_platform_device_pwm_foreach_child(func) \
    func(root_device_info_platform_device_pwm_device0) \
    func(root_device_info_platform_device_pwm_device1)

#define root_device_info_platform_device_pwm_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_pwm_device0, __VA_ARGS__) \
    func(root_device_info_platform_device_pwm_device1, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_pwm/device0
#define root_device_info_platform_device_pwm_device0_exists 1
#define root_device_info_platform_device_pwm_device0_policy_exists 1
#define root_device_info_platform_device_pwm_device0_policy 1
#define root_device_info_platform_device_pwm_device0_priority_exists 1
#define root_device_info_platform_device_pwm_device0_priority 80
#define root_device_info_platform_device_pwm_device0_permission_exists 1
#define root_device_info_platform_device_pwm_device0_permission 420
#define root_device_info_platform_device_pwm_device0_moduleName_exists 1
#define root_device_info_platform_device_pwm_device0_moduleName "HDF_PLATFORM_PWM"
#define root_device_info_platform_device_pwm_device0_serviceName_exists 1
#define root_device_info_platform_device_pwm_device0_serviceName "HDF_PLATFORM_PWM_0"
#define root_device_info_platform_device_pwm_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_pwm_device0_deviceMatchAttr "hisilicon_hi35xx_pwm_0"
#define root_device_info_platform_device_pwm_device0_preload_exists 1
#define root_device_info_platform_device_pwm_device0_preload 0

// hcs node macros: root/device_info/platform/device_pwm/device1
#define root_device_info_platform_device_pwm_device1_exists 1
#define root_device_info_platform_device_pwm_device1_policy_exists 1
#define root_device_info_platform_device_pwm_device1_policy 1
#define root_device_info_platform_device_pwm_device1_priority_exists 1
#define root_device_info_platform_device_pwm_device1_priority 80
#define root_device_info_platform_device_pwm_device1_permission_exists 1
#define root_device_info_platform_device_pwm_device1_permission 420
#define root_device_info_platform_device_pwm_device1_moduleName_exists 1
#define root_device_info_platform_device_pwm_device1_moduleName "HDF_PLATFORM_PWM"
#define root_device_info_platform_device_pwm_device1_serviceName_exists 1
#define root_device_info_platform_device_pwm_device1_serviceName "HDF_PLATFORM_PWM_1"
#define root_device_info_platform_device_pwm_device1_deviceMatchAttr_exists 1
#define root_device_info_platform_device_pwm_device1_deviceMatchAttr "hisilicon_hi35xx_pwm_1"
#define root_device_info_platform_device_pwm_device1_preload_exists 1
#define root_device_info_platform_device_pwm_device1_preload 0

// hcs node macros: root/device_info/platform/device_mipi_dsi
#define root_device_info_platform_device_mipi_dsi_exists 1
#define root_device_info_platform_device_mipi_dsi_foreach_child(func) \
    func(root_device_info_platform_device_mipi_dsi_device0)

#define root_device_info_platform_device_mipi_dsi_foreach_child_vargs(func, ...) \
    func(root_device_info_platform_device_mipi_dsi_device0, __VA_ARGS__)


// hcs node macros: root/device_info/platform/device_mipi_dsi/device0
#define root_device_info_platform_device_mipi_dsi_device0_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_policy_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_policy 0
#define root_device_info_platform_device_mipi_dsi_device0_priority_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_priority 150
#define root_device_info_platform_device_mipi_dsi_device0_permission_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_permission 420
#define root_device_info_platform_device_mipi_dsi_device0_moduleName_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_moduleName "HDF_MIPI_TX"
#define root_device_info_platform_device_mipi_dsi_device0_serviceName_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_serviceName "HDF_MIPI_TX"
#define root_device_info_platform_device_mipi_dsi_device0_preload_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_preload 0
#define root_device_info_platform_device_mipi_dsi_device0_deviceMatchAttr_exists 1
#define root_device_info_platform_device_mipi_dsi_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/display
#define root_device_info_display_exists 1
#define root_device_info_display_foreach_child(func) \
    func(root_device_info_display_device_hdf_disp) \
    func(root_device_info_display_device_hi35xx_disp) \
    func(root_device_info_display_device_lcd) \
    func(root_device_info_display_device_pwm_bl) \
    func(root_device_info_display_device_backlight)

#define root_device_info_display_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_hdf_disp, __VA_ARGS__) \
    func(root_device_info_display_device_hi35xx_disp, __VA_ARGS__) \
    func(root_device_info_display_device_lcd, __VA_ARGS__) \
    func(root_device_info_display_device_pwm_bl, __VA_ARGS__) \
    func(root_device_info_display_device_backlight, __VA_ARGS__)

#define root_device_info_display_hostName_exists 1
#define root_device_info_display_hostName "display_host"

// hcs node macros: root/device_info/display/device_hdf_disp
#define root_device_info_display_device_hdf_disp_exists 1
#define root_device_info_display_device_hdf_disp_foreach_child(func) \
    func(root_device_info_display_device_hdf_disp_device0)

#define root_device_info_display_device_hdf_disp_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_hdf_disp_device0, __VA_ARGS__)


// hcs node macros: root/device_info/display/device_hdf_disp/device0
#define root_device_info_display_device_hdf_disp_device0_exists 1
#define root_device_info_display_device_hdf_disp_device0_policy_exists 1
#define root_device_info_display_device_hdf_disp_device0_policy 2
#define root_device_info_display_device_hdf_disp_device0_priority_exists 1
#define root_device_info_display_device_hdf_disp_device0_priority 140
#define root_device_info_display_device_hdf_disp_device0_permission_exists 1
#define root_device_info_display_device_hdf_disp_device0_permission 432
#define root_device_info_display_device_hdf_disp_device0_moduleName_exists 1
#define root_device_info_display_device_hdf_disp_device0_moduleName "HDF_DISP"
#define root_device_info_display_device_hdf_disp_device0_serviceName_exists 1
#define root_device_info_display_device_hdf_disp_device0_serviceName "hdf_disp"
#define root_device_info_display_device_hdf_disp_device0_preload_exists 1
#define root_device_info_display_device_hdf_disp_device0_preload 0
#define root_device_info_display_device_hdf_disp_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_hdf_disp_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/display/device_hi35xx_disp
#define root_device_info_display_device_hi35xx_disp_exists 1
#define root_device_info_display_device_hi35xx_disp_foreach_child(func) \
    func(root_device_info_display_device_hi35xx_disp_device0)

#define root_device_info_display_device_hi35xx_disp_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_hi35xx_disp_device0, __VA_ARGS__)


// hcs node macros: root/device_info/display/device_hi35xx_disp/device0
#define root_device_info_display_device_hi35xx_disp_device0_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_policy_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_policy 0
#define root_device_info_display_device_hi35xx_disp_device0_priority_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_priority 120
#define root_device_info_display_device_hi35xx_disp_device0_moduleName_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_moduleName "HI351XX_DISP"
#define root_device_info_display_device_hi35xx_disp_device0_preload_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_preload 0
#define root_device_info_display_device_hi35xx_disp_device0_permission_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_permission 436
#define root_device_info_display_device_hi35xx_disp_device0_serviceName_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_serviceName ""
#define root_device_info_display_device_hi35xx_disp_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_hi35xx_disp_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/display/device_lcd
#define root_device_info_display_device_lcd_exists 1
#define root_device_info_display_device_lcd_foreach_child(func) \
    func(root_device_info_display_device_lcd_device0)

#define root_device_info_display_device_lcd_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_lcd_device0, __VA_ARGS__)


// hcs node macros: root/device_info/display/device_lcd/device0
#define root_device_info_display_device_lcd_device0_exists 1
#define root_device_info_display_device_lcd_device0_policy_exists 1
#define root_device_info_display_device_lcd_device0_policy 0
#define root_device_info_display_device_lcd_device0_priority_exists 1
#define root_device_info_display_device_lcd_device0_priority 100
#define root_device_info_display_device_lcd_device0_preload_exists 1
#define root_device_info_display_device_lcd_device0_preload 0
#define root_device_info_display_device_lcd_device0_moduleName_exists 1
#define root_device_info_display_device_lcd_device0_moduleName "LCD_ICN9700"
#define root_device_info_display_device_lcd_device0_permission_exists 1
#define root_device_info_display_device_lcd_device0_permission 436
#define root_device_info_display_device_lcd_device0_serviceName_exists 1
#define root_device_info_display_device_lcd_device0_serviceName ""
#define root_device_info_display_device_lcd_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_lcd_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/display/device_pwm_bl
#define root_device_info_display_device_pwm_bl_exists 1
#define root_device_info_display_device_pwm_bl_foreach_child(func) \
    func(root_device_info_display_device_pwm_bl_device0)

#define root_device_info_display_device_pwm_bl_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_pwm_bl_device0, __VA_ARGS__)


// hcs node macros: root/device_info/display/device_pwm_bl/device0
#define root_device_info_display_device_pwm_bl_device0_exists 1
#define root_device_info_display_device_pwm_bl_device0_policy_exists 1
#define root_device_info_display_device_pwm_bl_device0_policy 0
#define root_device_info_display_device_pwm_bl_device0_priority_exists 1
#define root_device_info_display_device_pwm_bl_device0_priority 95
#define root_device_info_display_device_pwm_bl_device0_preload_exists 1
#define root_device_info_display_device_pwm_bl_device0_preload 0
#define root_device_info_display_device_pwm_bl_device0_moduleName_exists 1
#define root_device_info_display_device_pwm_bl_device0_moduleName "PWM_BL"
#define root_device_info_display_device_pwm_bl_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_pwm_bl_device0_deviceMatchAttr "pwm_bl_dev"
#define root_device_info_display_device_pwm_bl_device0_permission_exists 1
#define root_device_info_display_device_pwm_bl_device0_permission 436
#define root_device_info_display_device_pwm_bl_device0_serviceName_exists 1
#define root_device_info_display_device_pwm_bl_device0_serviceName ""

// hcs node macros: root/device_info/display/device_backlight
#define root_device_info_display_device_backlight_exists 1
#define root_device_info_display_device_backlight_foreach_child(func) \
    func(root_device_info_display_device_backlight_device0)

#define root_device_info_display_device_backlight_foreach_child_vargs(func, ...) \
    func(root_device_info_display_device_backlight_device0, __VA_ARGS__)


// hcs node macros: root/device_info/display/device_backlight/device0
#define root_device_info_display_device_backlight_device0_exists 1
#define root_device_info_display_device_backlight_device0_policy_exists 1
#define root_device_info_display_device_backlight_device0_policy 2
#define root_device_info_display_device_backlight_device0_priority_exists 1
#define root_device_info_display_device_backlight_device0_priority 90
#define root_device_info_display_device_backlight_device0_preload_exists 1
#define root_device_info_display_device_backlight_device0_preload 0
#define root_device_info_display_device_backlight_device0_permission_exists 1
#define root_device_info_display_device_backlight_device0_permission 432
#define root_device_info_display_device_backlight_device0_moduleName_exists 1
#define root_device_info_display_device_backlight_device0_moduleName "HDF_BL"
#define root_device_info_display_device_backlight_device0_serviceName_exists 1
#define root_device_info_display_device_backlight_device0_serviceName "hdf_bl"
#define root_device_info_display_device_backlight_device0_deviceMatchAttr_exists 1
#define root_device_info_display_device_backlight_device0_deviceMatchAttr ""
#define root_device_info_display_priority_exists 1
#define root_device_info_display_priority 100

// hcs node macros: root/device_info/input
#define root_device_info_input_exists 1
#define root_device_info_input_foreach_child(func) \
    func(root_device_info_input_device_input_manager) \
    func(root_device_info_input_device_hdf_touch) \
    func(root_device_info_input_device_touch_chip) \
    func(root_device_info_input_device_hdf_hid) \
    func(root_device_info_input_device_hdf_encoder)

#define root_device_info_input_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_input_manager, __VA_ARGS__) \
    func(root_device_info_input_device_hdf_touch, __VA_ARGS__) \
    func(root_device_info_input_device_touch_chip, __VA_ARGS__) \
    func(root_device_info_input_device_hdf_hid, __VA_ARGS__) \
    func(root_device_info_input_device_hdf_encoder, __VA_ARGS__)

#define root_device_info_input_hostName_exists 1
#define root_device_info_input_hostName "input_host"
#define root_device_info_input_priority_exists 1
#define root_device_info_input_priority 100

// hcs node macros: root/device_info/input/device_input_manager
#define root_device_info_input_device_input_manager_exists 1
#define root_device_info_input_device_input_manager_foreach_child(func) \
    func(root_device_info_input_device_input_manager_device0)

#define root_device_info_input_device_input_manager_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_input_manager_device0, __VA_ARGS__)


// hcs node macros: root/device_info/input/device_input_manager/device0
#define root_device_info_input_device_input_manager_device0_exists 1
#define root_device_info_input_device_input_manager_device0_policy_exists 1
#define root_device_info_input_device_input_manager_device0_policy 2
#define root_device_info_input_device_input_manager_device0_priority_exists 1
#define root_device_info_input_device_input_manager_device0_priority 100
#define root_device_info_input_device_input_manager_device0_preload_exists 1
#define root_device_info_input_device_input_manager_device0_preload 0
#define root_device_info_input_device_input_manager_device0_permission_exists 1
#define root_device_info_input_device_input_manager_device0_permission 432
#define root_device_info_input_device_input_manager_device0_moduleName_exists 1
#define root_device_info_input_device_input_manager_device0_moduleName "HDF_INPUT_MANAGER"
#define root_device_info_input_device_input_manager_device0_serviceName_exists 1
#define root_device_info_input_device_input_manager_device0_serviceName "hdf_input_host"
#define root_device_info_input_device_input_manager_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_input_manager_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/input/device_hdf_touch
#define root_device_info_input_device_hdf_touch_exists 1
#define root_device_info_input_device_hdf_touch_foreach_child(func) \
    func(root_device_info_input_device_hdf_touch_device0)

#define root_device_info_input_device_hdf_touch_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_hdf_touch_device0, __VA_ARGS__)


// hcs node macros: root/device_info/input/device_hdf_touch/device0
#define root_device_info_input_device_hdf_touch_device0_exists 1
#define root_device_info_input_device_hdf_touch_device0_policy_exists 1
#define root_device_info_input_device_hdf_touch_device0_policy 2
#define root_device_info_input_device_hdf_touch_device0_priority_exists 1
#define root_device_info_input_device_hdf_touch_device0_priority 120
#define root_device_info_input_device_hdf_touch_device0_preload_exists 1
#define root_device_info_input_device_hdf_touch_device0_preload 0
#define root_device_info_input_device_hdf_touch_device0_permission_exists 1
#define root_device_info_input_device_hdf_touch_device0_permission 432
#define root_device_info_input_device_hdf_touch_device0_moduleName_exists 1
#define root_device_info_input_device_hdf_touch_device0_moduleName "HDF_TOUCH"
#define root_device_info_input_device_hdf_touch_device0_serviceName_exists 1
#define root_device_info_input_device_hdf_touch_device0_serviceName "hdf_input_event1"
#define root_device_info_input_device_hdf_touch_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_hdf_touch_device0_deviceMatchAttr "touch_device1"

// hcs node macros: root/device_info/input/device_touch_chip
#define root_device_info_input_device_touch_chip_exists 1
#define root_device_info_input_device_touch_chip_foreach_child(func) \
    func(root_device_info_input_device_touch_chip_device0)

#define root_device_info_input_device_touch_chip_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_touch_chip_device0, __VA_ARGS__)


// hcs node macros: root/device_info/input/device_touch_chip/device0
#define root_device_info_input_device_touch_chip_device0_exists 1
#define root_device_info_input_device_touch_chip_device0_policy_exists 1
#define root_device_info_input_device_touch_chip_device0_policy 0
#define root_device_info_input_device_touch_chip_device0_priority_exists 1
#define root_device_info_input_device_touch_chip_device0_priority 130
#define root_device_info_input_device_touch_chip_device0_preload_exists 1
#define root_device_info_input_device_touch_chip_device0_preload 0
#define root_device_info_input_device_touch_chip_device0_permission_exists 1
#define root_device_info_input_device_touch_chip_device0_permission 432
#define root_device_info_input_device_touch_chip_device0_moduleName_exists 1
#define root_device_info_input_device_touch_chip_device0_moduleName "HDF_TOUCH_GT911"
#define root_device_info_input_device_touch_chip_device0_serviceName_exists 1
#define root_device_info_input_device_touch_chip_device0_serviceName "hdf_touch_gt911_service"
#define root_device_info_input_device_touch_chip_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_touch_chip_device0_deviceMatchAttr "zsj_gt911_5p5"

// hcs node macros: root/device_info/input/device_hdf_hid
#define root_device_info_input_device_hdf_hid_exists 1
#define root_device_info_input_device_hdf_hid_foreach_child(func) \
    func(root_device_info_input_device_hdf_hid_device0)

#define root_device_info_input_device_hdf_hid_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_hdf_hid_device0, __VA_ARGS__)


// hcs node macros: root/device_info/input/device_hdf_hid/device0
#define root_device_info_input_device_hdf_hid_device0_exists 1
#define root_device_info_input_device_hdf_hid_device0_policy_exists 1
#define root_device_info_input_device_hdf_hid_device0_policy 2
#define root_device_info_input_device_hdf_hid_device0_priority_exists 1
#define root_device_info_input_device_hdf_hid_device0_priority 200
#define root_device_info_input_device_hdf_hid_device0_preload_exists 1
#define root_device_info_input_device_hdf_hid_device0_preload 0
#define root_device_info_input_device_hdf_hid_device0_permission_exists 1
#define root_device_info_input_device_hdf_hid_device0_permission 432
#define root_device_info_input_device_hdf_hid_device0_moduleName_exists 1
#define root_device_info_input_device_hdf_hid_device0_moduleName "HDF_HID"
#define root_device_info_input_device_hdf_hid_device0_serviceName_exists 1
#define root_device_info_input_device_hdf_hid_device0_serviceName ""
#define root_device_info_input_device_hdf_hid_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_hdf_hid_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/input/device_hdf_encoder
#define root_device_info_input_device_hdf_encoder_exists 1
#define root_device_info_input_device_hdf_encoder_foreach_child(func) \
    func(root_device_info_input_device_hdf_encoder_device0)

#define root_device_info_input_device_hdf_encoder_foreach_child_vargs(func, ...) \
    func(root_device_info_input_device_hdf_encoder_device0, __VA_ARGS__)


// hcs node macros: root/device_info/input/device_hdf_encoder/device0
#define root_device_info_input_device_hdf_encoder_device0_exists 1
#define root_device_info_input_device_hdf_encoder_device0_policy_exists 1
#define root_device_info_input_device_hdf_encoder_device0_policy 2
#define root_device_info_input_device_hdf_encoder_device0_priority_exists 1
#define root_device_info_input_device_hdf_encoder_device0_priority 200
#define root_device_info_input_device_hdf_encoder_device0_preload_exists 1
#define root_device_info_input_device_hdf_encoder_device0_preload 0
#define root_device_info_input_device_hdf_encoder_device0_permission_exists 1
#define root_device_info_input_device_hdf_encoder_device0_permission 432
#define root_device_info_input_device_hdf_encoder_device0_moduleName_exists 1
#define root_device_info_input_device_hdf_encoder_device0_moduleName "HDF_ENCODER"
#define root_device_info_input_device_hdf_encoder_device0_deviceMatchAttr_exists 1
#define root_device_info_input_device_hdf_encoder_device0_deviceMatchAttr "EC11"
#define root_device_info_input_device_hdf_encoder_device0_serviceName_exists 1
#define root_device_info_input_device_hdf_encoder_device0_serviceName ""

// hcs node macros: root/device_info/network
#define root_device_info_network_exists 1
#define root_device_info_network_foreach_child(func) \
    func(root_device_info_network_device_wifi) \
    func(root_device_info_network_device_wlan_chips)

#define root_device_info_network_foreach_child_vargs(func, ...) \
    func(root_device_info_network_device_wifi, __VA_ARGS__) \
    func(root_device_info_network_device_wlan_chips, __VA_ARGS__)

#define root_device_info_network_hostName_exists 1
#define root_device_info_network_hostName "network_host"

// hcs node macros: root/device_info/network/device_wifi
#define root_device_info_network_device_wifi_exists 1
#define root_device_info_network_device_wifi_foreach_child(func) \
    func(root_device_info_network_device_wifi_device0)

#define root_device_info_network_device_wifi_foreach_child_vargs(func, ...) \
    func(root_device_info_network_device_wifi_device0, __VA_ARGS__)


// hcs node macros: root/device_info/network/device_wifi/device0
#define root_device_info_network_device_wifi_device0_exists 1
#define root_device_info_network_device_wifi_device0_policy_exists 1
#define root_device_info_network_device_wifi_device0_policy 2
#define root_device_info_network_device_wifi_device0_priority_exists 1
#define root_device_info_network_device_wifi_device0_priority 100
#define root_device_info_network_device_wifi_device0_preload_exists 1
#define root_device_info_network_device_wifi_device0_preload 0
#define root_device_info_network_device_wifi_device0_permission_exists 1
#define root_device_info_network_device_wifi_device0_permission 436
#define root_device_info_network_device_wifi_device0_moduleName_exists 1
#define root_device_info_network_device_wifi_device0_moduleName "HDF_WIFI"
#define root_device_info_network_device_wifi_device0_deviceMatchAttr_exists 1
#define root_device_info_network_device_wifi_device0_deviceMatchAttr "hdf_wlan_driver"
#define root_device_info_network_device_wifi_device0_serviceName_exists 1
#define root_device_info_network_device_wifi_device0_serviceName "hdfwifi"

// hcs node macros: root/device_info/network/device_wlan_chips
#define root_device_info_network_device_wlan_chips_exists 1
#define root_device_info_network_device_wlan_chips_foreach_child(func) \
    func(root_device_info_network_device_wlan_chips_device0)

#define root_device_info_network_device_wlan_chips_foreach_child_vargs(func, ...) \
    func(root_device_info_network_device_wlan_chips_device0, __VA_ARGS__)


// hcs node macros: root/device_info/network/device_wlan_chips/device0
#define root_device_info_network_device_wlan_chips_device0_exists 1
#define root_device_info_network_device_wlan_chips_device0_policy_exists 1
#define root_device_info_network_device_wlan_chips_device0_policy 0
#define root_device_info_network_device_wlan_chips_device0_preload_exists 1
#define root_device_info_network_device_wlan_chips_device0_preload 2
#define root_device_info_network_device_wlan_chips_device0_moduleName_exists 1
#define root_device_info_network_device_wlan_chips_device0_moduleName "HDF_WLAN_CHIPS"
#define root_device_info_network_device_wlan_chips_device0_deviceMatchAttr_exists 1
#define root_device_info_network_device_wlan_chips_device0_deviceMatchAttr "hdf_wlan_chips_hi3881"
#define root_device_info_network_device_wlan_chips_device0_serviceName_exists 1
#define root_device_info_network_device_wlan_chips_device0_serviceName "hisi"
#define root_device_info_network_device_wlan_chips_device0_priority_exists 1
#define root_device_info_network_device_wlan_chips_device0_priority 100
#define root_device_info_network_device_wlan_chips_device0_permission_exists 1
#define root_device_info_network_device_wlan_chips_device0_permission 436
#define root_device_info_network_priority_exists 1
#define root_device_info_network_priority 100

// hcs node macros: root/device_info/sensor
#define root_device_info_sensor_exists 1
#define root_device_info_sensor_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_manager) \
    func(root_device_info_sensor_device_sensor_accel) \
    func(root_device_info_sensor_device_sensor_bmi160) \
    func(root_device_info_sensor_device_sensor_als) \
    func(root_device_info_sensor_device_sensor_bh1745) \
    func(root_device_info_sensor_device_sensor_gyro) \
    func(root_device_info_sensor_device_sensor_gyro_bmi160) \
    func(root_device_info_sensor_device_sensor_barometer) \
    func(root_device_info_sensor_device_sensor_bmp180) \
    func(root_device_info_sensor_device_sensor_hall) \
    func(root_device_info_sensor_device_sensor_ak8789) \
    func(root_device_info_sensor_device_sensor_magnetic) \
    func(root_device_info_sensor_device_sensor_lsm303) \
    func(root_device_info_sensor_device_sensor_proximity) \
    func(root_device_info_sensor_device_sensor_apds9960) \
    func(root_device_info_sensor_device_sensor_gravity)

#define root_device_info_sensor_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_manager, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_accel, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_bmi160, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_als, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_bh1745, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_gyro, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_gyro_bmi160, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_barometer, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_bmp180, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_hall, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_ak8789, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_magnetic, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_lsm303, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_proximity, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_apds9960, __VA_ARGS__) \
    func(root_device_info_sensor_device_sensor_gravity, __VA_ARGS__)

#define root_device_info_sensor_hostName_exists 1
#define root_device_info_sensor_hostName "sensor_host"

// hcs node macros: root/device_info/sensor/device_sensor_manager
#define root_device_info_sensor_device_sensor_manager_exists 1
#define root_device_info_sensor_device_sensor_manager_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_manager_device0)

#define root_device_info_sensor_device_sensor_manager_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_manager_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_manager/device0
#define root_device_info_sensor_device_sensor_manager_device0_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_policy 2
#define root_device_info_sensor_device_sensor_manager_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_priority 100
#define root_device_info_sensor_device_sensor_manager_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_preload 0
#define root_device_info_sensor_device_sensor_manager_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_permission 436
#define root_device_info_sensor_device_sensor_manager_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_moduleName "HDF_SENSOR_MGR_AP"
#define root_device_info_sensor_device_sensor_manager_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_serviceName "hdf_sensor_manager_ap"
#define root_device_info_sensor_device_sensor_manager_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_manager_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/sensor/device_sensor_accel
#define root_device_info_sensor_device_sensor_accel_exists 1
#define root_device_info_sensor_device_sensor_accel_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_accel_device0)

#define root_device_info_sensor_device_sensor_accel_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_accel_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_accel/device0
#define root_device_info_sensor_device_sensor_accel_device0_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_policy 1
#define root_device_info_sensor_device_sensor_accel_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_priority 110
#define root_device_info_sensor_device_sensor_accel_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_preload 0
#define root_device_info_sensor_device_sensor_accel_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_permission 436
#define root_device_info_sensor_device_sensor_accel_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_moduleName "HDF_SENSOR_ACCEL"
#define root_device_info_sensor_device_sensor_accel_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_serviceName "hdf_sensor_accel"
#define root_device_info_sensor_device_sensor_accel_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_accel_device0_deviceMatchAttr "hdf_sensor_accel_driver"

// hcs node macros: root/device_info/sensor/device_sensor_bmi160
#define root_device_info_sensor_device_sensor_bmi160_exists 1
#define root_device_info_sensor_device_sensor_bmi160_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_bmi160_device0)

#define root_device_info_sensor_device_sensor_bmi160_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_bmi160_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_bmi160/device0
#define root_device_info_sensor_device_sensor_bmi160_device0_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_policy 1
#define root_device_info_sensor_device_sensor_bmi160_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_priority 120
#define root_device_info_sensor_device_sensor_bmi160_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_preload 0
#define root_device_info_sensor_device_sensor_bmi160_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_permission 436
#define root_device_info_sensor_device_sensor_bmi160_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_moduleName "HDF_SENSOR_ACCEL_BMI160"
#define root_device_info_sensor_device_sensor_bmi160_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_serviceName "hdf_accel_bmi160"
#define root_device_info_sensor_device_sensor_bmi160_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_bmi160_device0_deviceMatchAttr "hdf_sensor_accel_bmi160_driver"

// hcs node macros: root/device_info/sensor/device_sensor_als
#define root_device_info_sensor_device_sensor_als_exists 1
#define root_device_info_sensor_device_sensor_als_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_als_device0)

#define root_device_info_sensor_device_sensor_als_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_als_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_als/device0
#define root_device_info_sensor_device_sensor_als_device0_exists 1
#define root_device_info_sensor_device_sensor_als_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_als_device0_policy 1
#define root_device_info_sensor_device_sensor_als_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_als_device0_priority 110
#define root_device_info_sensor_device_sensor_als_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_als_device0_preload 0
#define root_device_info_sensor_device_sensor_als_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_als_device0_permission 436
#define root_device_info_sensor_device_sensor_als_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_als_device0_moduleName "HDF_SENSOR_ALS"
#define root_device_info_sensor_device_sensor_als_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_als_device0_serviceName "hdf_sensor_als"
#define root_device_info_sensor_device_sensor_als_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_als_device0_deviceMatchAttr "hdf_sensor_als_driver"

// hcs node macros: root/device_info/sensor/device_sensor_bh1745
#define root_device_info_sensor_device_sensor_bh1745_exists 1
#define root_device_info_sensor_device_sensor_bh1745_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_bh1745_device0)

#define root_device_info_sensor_device_sensor_bh1745_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_bh1745_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_bh1745/device0
#define root_device_info_sensor_device_sensor_bh1745_device0_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_policy 1
#define root_device_info_sensor_device_sensor_bh1745_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_priority 120
#define root_device_info_sensor_device_sensor_bh1745_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_preload 0
#define root_device_info_sensor_device_sensor_bh1745_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_permission 436
#define root_device_info_sensor_device_sensor_bh1745_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_moduleName "HDF_SENSOR_ALS_BH1745"
#define root_device_info_sensor_device_sensor_bh1745_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_serviceName "hdf_als_bh1745"
#define root_device_info_sensor_device_sensor_bh1745_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_bh1745_device0_deviceMatchAttr "hdf_sensor_als_bh1745_driver"

// hcs node macros: root/device_info/sensor/device_sensor_gyro
#define root_device_info_sensor_device_sensor_gyro_exists 1
#define root_device_info_sensor_device_sensor_gyro_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_gyro_device0)

#define root_device_info_sensor_device_sensor_gyro_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_gyro_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_gyro/device0
#define root_device_info_sensor_device_sensor_gyro_device0_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_policy 1
#define root_device_info_sensor_device_sensor_gyro_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_priority 110
#define root_device_info_sensor_device_sensor_gyro_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_preload 0
#define root_device_info_sensor_device_sensor_gyro_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_permission 436
#define root_device_info_sensor_device_sensor_gyro_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_moduleName "HDF_SENSOR_GYRO"
#define root_device_info_sensor_device_sensor_gyro_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_serviceName "hdf_sensor_gyro"
#define root_device_info_sensor_device_sensor_gyro_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_gyro_device0_deviceMatchAttr "hdf_sensor_gyro_driver"

// hcs node macros: root/device_info/sensor/device_sensor_gyro_bmi160
#define root_device_info_sensor_device_sensor_gyro_bmi160_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_gyro_bmi160_device0)

#define root_device_info_sensor_device_sensor_gyro_bmi160_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_gyro_bmi160_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_gyro_bmi160/device0
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_policy 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_priority 120
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_preload 0
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_permission 436
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_moduleName "HDF_SENSOR_GYRO_BMI160"
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_serviceName "hdf_gyro_bmi160"
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_gyro_bmi160_device0_deviceMatchAttr "hdf_sensor_gyro_bmi160_driver"

// hcs node macros: root/device_info/sensor/device_sensor_barometer
#define root_device_info_sensor_device_sensor_barometer_exists 1
#define root_device_info_sensor_device_sensor_barometer_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_barometer_device0)

#define root_device_info_sensor_device_sensor_barometer_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_barometer_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_barometer/device0
#define root_device_info_sensor_device_sensor_barometer_device0_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_policy 1
#define root_device_info_sensor_device_sensor_barometer_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_priority 110
#define root_device_info_sensor_device_sensor_barometer_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_preload 0
#define root_device_info_sensor_device_sensor_barometer_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_permission 436
#define root_device_info_sensor_device_sensor_barometer_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_moduleName "HDF_SENSOR_BAROMETER"
#define root_device_info_sensor_device_sensor_barometer_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_serviceName "hdf_sensor_barometer"
#define root_device_info_sensor_device_sensor_barometer_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_barometer_device0_deviceMatchAttr "hdf_sensor_barometer_driver"

// hcs node macros: root/device_info/sensor/device_sensor_bmp180
#define root_device_info_sensor_device_sensor_bmp180_exists 1
#define root_device_info_sensor_device_sensor_bmp180_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_bmp180_device0)

#define root_device_info_sensor_device_sensor_bmp180_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_bmp180_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_bmp180/device0
#define root_device_info_sensor_device_sensor_bmp180_device0_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_policy 1
#define root_device_info_sensor_device_sensor_bmp180_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_priority 120
#define root_device_info_sensor_device_sensor_bmp180_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_preload 0
#define root_device_info_sensor_device_sensor_bmp180_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_permission 436
#define root_device_info_sensor_device_sensor_bmp180_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_moduleName "HDF_SENSOR_BAROMETER_BMP180"
#define root_device_info_sensor_device_sensor_bmp180_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_serviceName "hdf_barometer_bmp180"
#define root_device_info_sensor_device_sensor_bmp180_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_bmp180_device0_deviceMatchAttr "hdf_sensor_barometer_bmp180_driver"

// hcs node macros: root/device_info/sensor/device_sensor_hall
#define root_device_info_sensor_device_sensor_hall_exists 1
#define root_device_info_sensor_device_sensor_hall_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_hall_device0)

#define root_device_info_sensor_device_sensor_hall_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_hall_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_hall/device0
#define root_device_info_sensor_device_sensor_hall_device0_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_policy 1
#define root_device_info_sensor_device_sensor_hall_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_priority 110
#define root_device_info_sensor_device_sensor_hall_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_preload 2
#define root_device_info_sensor_device_sensor_hall_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_permission 436
#define root_device_info_sensor_device_sensor_hall_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_moduleName "HDF_SENSOR_HALL"
#define root_device_info_sensor_device_sensor_hall_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_serviceName "hdf_sensor_hall"
#define root_device_info_sensor_device_sensor_hall_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_hall_device0_deviceMatchAttr "hdf_sensor_hall_driver"

// hcs node macros: root/device_info/sensor/device_sensor_ak8789
#define root_device_info_sensor_device_sensor_ak8789_exists 1
#define root_device_info_sensor_device_sensor_ak8789_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_ak8789_device0)

#define root_device_info_sensor_device_sensor_ak8789_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_ak8789_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_ak8789/device0
#define root_device_info_sensor_device_sensor_ak8789_device0_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_policy 1
#define root_device_info_sensor_device_sensor_ak8789_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_priority 120
#define root_device_info_sensor_device_sensor_ak8789_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_preload 2
#define root_device_info_sensor_device_sensor_ak8789_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_permission 436
#define root_device_info_sensor_device_sensor_ak8789_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_moduleName "HDF_SENSOR_HALL_AK8789"
#define root_device_info_sensor_device_sensor_ak8789_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_serviceName "hdf_hall_ak8789"
#define root_device_info_sensor_device_sensor_ak8789_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_ak8789_device0_deviceMatchAttr "hdf_sensor_hall_ak8789_driver"

// hcs node macros: root/device_info/sensor/device_sensor_magnetic
#define root_device_info_sensor_device_sensor_magnetic_exists 1
#define root_device_info_sensor_device_sensor_magnetic_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_magnetic_device0)

#define root_device_info_sensor_device_sensor_magnetic_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_magnetic_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_magnetic/device0
#define root_device_info_sensor_device_sensor_magnetic_device0_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_policy 1
#define root_device_info_sensor_device_sensor_magnetic_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_priority 110
#define root_device_info_sensor_device_sensor_magnetic_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_preload 0
#define root_device_info_sensor_device_sensor_magnetic_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_permission 436
#define root_device_info_sensor_device_sensor_magnetic_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_moduleName "HDF_SENSOR_MAGNETIC"
#define root_device_info_sensor_device_sensor_magnetic_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_serviceName "hdf_sensor_magnetic"
#define root_device_info_sensor_device_sensor_magnetic_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_magnetic_device0_deviceMatchAttr "hdf_sensor_magnetic_driver"

// hcs node macros: root/device_info/sensor/device_sensor_lsm303
#define root_device_info_sensor_device_sensor_lsm303_exists 1
#define root_device_info_sensor_device_sensor_lsm303_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_lsm303_device0)

#define root_device_info_sensor_device_sensor_lsm303_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_lsm303_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_lsm303/device0
#define root_device_info_sensor_device_sensor_lsm303_device0_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_policy 1
#define root_device_info_sensor_device_sensor_lsm303_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_priority 120
#define root_device_info_sensor_device_sensor_lsm303_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_preload 0
#define root_device_info_sensor_device_sensor_lsm303_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_permission 436
#define root_device_info_sensor_device_sensor_lsm303_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_moduleName "HDF_SENSOR_MAGNETIC_LSM303"
#define root_device_info_sensor_device_sensor_lsm303_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_serviceName "hdf_magnetic_lsm303"
#define root_device_info_sensor_device_sensor_lsm303_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_lsm303_device0_deviceMatchAttr "hdf_sensor_magnetic_lsm303_driver"

// hcs node macros: root/device_info/sensor/device_sensor_proximity
#define root_device_info_sensor_device_sensor_proximity_exists 1
#define root_device_info_sensor_device_sensor_proximity_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_proximity_device0)

#define root_device_info_sensor_device_sensor_proximity_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_proximity_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_proximity/device0
#define root_device_info_sensor_device_sensor_proximity_device0_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_policy 1
#define root_device_info_sensor_device_sensor_proximity_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_priority 110
#define root_device_info_sensor_device_sensor_proximity_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_preload 0
#define root_device_info_sensor_device_sensor_proximity_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_permission 436
#define root_device_info_sensor_device_sensor_proximity_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_moduleName "HDF_SENSOR_PROXIMITY"
#define root_device_info_sensor_device_sensor_proximity_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_serviceName "hdf_sensor_proximity"
#define root_device_info_sensor_device_sensor_proximity_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_proximity_device0_deviceMatchAttr "hdf_sensor_proximity_driver"

// hcs node macros: root/device_info/sensor/device_sensor_apds9960
#define root_device_info_sensor_device_sensor_apds9960_exists 1
#define root_device_info_sensor_device_sensor_apds9960_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_apds9960_device0)

#define root_device_info_sensor_device_sensor_apds9960_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_apds9960_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_apds9960/device0
#define root_device_info_sensor_device_sensor_apds9960_device0_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_policy 1
#define root_device_info_sensor_device_sensor_apds9960_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_priority 120
#define root_device_info_sensor_device_sensor_apds9960_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_preload 0
#define root_device_info_sensor_device_sensor_apds9960_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_permission 436
#define root_device_info_sensor_device_sensor_apds9960_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_moduleName "HDF_SENSOR_PROXIMITY_APDS9960"
#define root_device_info_sensor_device_sensor_apds9960_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_serviceName "hdf_proximity_apds9960"
#define root_device_info_sensor_device_sensor_apds9960_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_apds9960_device0_deviceMatchAttr "hdf_sensor_proximity_adps9960_driver"

// hcs node macros: root/device_info/sensor/device_sensor_gravity
#define root_device_info_sensor_device_sensor_gravity_exists 1
#define root_device_info_sensor_device_sensor_gravity_foreach_child(func) \
    func(root_device_info_sensor_device_sensor_gravity_device0)

#define root_device_info_sensor_device_sensor_gravity_foreach_child_vargs(func, ...) \
    func(root_device_info_sensor_device_sensor_gravity_device0, __VA_ARGS__)


// hcs node macros: root/device_info/sensor/device_sensor_gravity/device0
#define root_device_info_sensor_device_sensor_gravity_device0_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_policy_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_policy 1
#define root_device_info_sensor_device_sensor_gravity_device0_priority_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_priority 130
#define root_device_info_sensor_device_sensor_gravity_device0_preload_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_preload 0
#define root_device_info_sensor_device_sensor_gravity_device0_permission_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_permission 436
#define root_device_info_sensor_device_sensor_gravity_device0_moduleName_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_moduleName "HDF_SENSOR_GRAVITY"
#define root_device_info_sensor_device_sensor_gravity_device0_serviceName_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_serviceName "hdf_sensor_gravity"
#define root_device_info_sensor_device_sensor_gravity_device0_deviceMatchAttr_exists 1
#define root_device_info_sensor_device_sensor_gravity_device0_deviceMatchAttr "hdf_sensor_gravity_driver"
#define root_device_info_sensor_priority_exists 1
#define root_device_info_sensor_priority 100

// hcs node macros: root/device_info/usb_pnp_linux
#define root_device_info_usb_pnp_linux_exists 1
#define root_device_info_usb_pnp_linux_foreach_child(func) \
    func(root_device_info_usb_pnp_linux_device_usb_pnp_linux)

#define root_device_info_usb_pnp_linux_foreach_child_vargs(func, ...) \
    func(root_device_info_usb_pnp_linux_device_usb_pnp_linux, __VA_ARGS__)

#define root_device_info_usb_pnp_linux_hostName_exists 1
#define root_device_info_usb_pnp_linux_hostName "usb_pnp_linux_host"

// hcs node macros: root/device_info/usb_pnp_linux/device_usb_pnp_linux
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_foreach_child(func) \
    func(root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0)

#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_foreach_child_vargs(func, ...) \
    func(root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0, __VA_ARGS__)


// hcs node macros: root/device_info/usb_pnp_linux/device_usb_pnp_linux/device0
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_policy_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_policy 2
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_priority_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_priority 100
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_preload_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_preload 0
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_permission_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_permission 436
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_moduleName_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_moduleName "HDF_USB_PNP_NOTIFY"
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_serviceName_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_serviceName "hdf_usb_pnp_notify_service"
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_deviceMatchAttr_exists 1
#define root_device_info_usb_pnp_linux_device_usb_pnp_linux_device0_deviceMatchAttr "hdf_usb_pnp_notify_config"
#define root_device_info_usb_pnp_linux_priority_exists 1
#define root_device_info_usb_pnp_linux_priority 100

// hcs node macros: root/device_info/audio
#define root_device_info_audio_exists 1
#define root_device_info_audio_foreach_child(func) \
    func(root_device_info_audio_device_dai0) \
    func(root_device_info_audio_device_codec) \
    func(root_device_info_audio_device_codec_ex) \
    func(root_device_info_audio_device_dsp) \
    func(root_device_info_audio_device_dma) \
    func(root_device_info_audio_device_audio) \
    func(root_device_info_audio_device_stream) \
    func(root_device_info_audio_device_control)

#define root_device_info_audio_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_dai0, __VA_ARGS__) \
    func(root_device_info_audio_device_codec, __VA_ARGS__) \
    func(root_device_info_audio_device_codec_ex, __VA_ARGS__) \
    func(root_device_info_audio_device_dsp, __VA_ARGS__) \
    func(root_device_info_audio_device_dma, __VA_ARGS__) \
    func(root_device_info_audio_device_audio, __VA_ARGS__) \
    func(root_device_info_audio_device_stream, __VA_ARGS__) \
    func(root_device_info_audio_device_control, __VA_ARGS__)

#define root_device_info_audio_hostName_exists 1
#define root_device_info_audio_hostName "audio_host"
#define root_device_info_audio_priority_exists 1
#define root_device_info_audio_priority 60

// hcs node macros: root/device_info/audio/device_dai0
#define root_device_info_audio_device_dai0_exists 1
#define root_device_info_audio_device_dai0_foreach_child(func) \
    func(root_device_info_audio_device_dai0_device0)

#define root_device_info_audio_device_dai0_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_dai0_device0, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_dai0/device0
#define root_device_info_audio_device_dai0_device0_exists 1
#define root_device_info_audio_device_dai0_device0_policy_exists 1
#define root_device_info_audio_device_dai0_device0_policy 1
#define root_device_info_audio_device_dai0_device0_priority_exists 1
#define root_device_info_audio_device_dai0_device0_priority 50
#define root_device_info_audio_device_dai0_device0_preload_exists 1
#define root_device_info_audio_device_dai0_device0_preload 0
#define root_device_info_audio_device_dai0_device0_permission_exists 1
#define root_device_info_audio_device_dai0_device0_permission 438
#define root_device_info_audio_device_dai0_device0_moduleName_exists 1
#define root_device_info_audio_device_dai0_device0_moduleName "DAI_HI3516"
#define root_device_info_audio_device_dai0_device0_serviceName_exists 1
#define root_device_info_audio_device_dai0_device0_serviceName "dai_service"
#define root_device_info_audio_device_dai0_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_dai0_device0_deviceMatchAttr "hdf_dai_driver"

// hcs node macros: root/device_info/audio/device_codec
#define root_device_info_audio_device_codec_exists 1
#define root_device_info_audio_device_codec_foreach_child(func) \
    func(root_device_info_audio_device_codec_device0)

#define root_device_info_audio_device_codec_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_codec_device0, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_codec/device0
#define root_device_info_audio_device_codec_device0_exists 1
#define root_device_info_audio_device_codec_device0_policy_exists 1
#define root_device_info_audio_device_codec_device0_policy 1
#define root_device_info_audio_device_codec_device0_priority_exists 1
#define root_device_info_audio_device_codec_device0_priority 50
#define root_device_info_audio_device_codec_device0_preload_exists 1
#define root_device_info_audio_device_codec_device0_preload 0
#define root_device_info_audio_device_codec_device0_permission_exists 1
#define root_device_info_audio_device_codec_device0_permission 438
#define root_device_info_audio_device_codec_device0_moduleName_exists 1
#define root_device_info_audio_device_codec_device0_moduleName "CODEC_HI3516"
#define root_device_info_audio_device_codec_device0_serviceName_exists 1
#define root_device_info_audio_device_codec_device0_serviceName "codec_service_0"
#define root_device_info_audio_device_codec_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_codec_device0_deviceMatchAttr "hdf_codec_driver"

// hcs node macros: root/device_info/audio/device_codec_ex
#define root_device_info_audio_device_codec_ex_exists 1
#define root_device_info_audio_device_codec_ex_foreach_child(func) \
    func(root_device_info_audio_device_codec_ex_device0)

#define root_device_info_audio_device_codec_ex_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_codec_ex_device0, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_codec_ex/device0
#define root_device_info_audio_device_codec_ex_device0_exists 1
#define root_device_info_audio_device_codec_ex_device0_policy_exists 1
#define root_device_info_audio_device_codec_ex_device0_policy 1
#define root_device_info_audio_device_codec_ex_device0_priority_exists 1
#define root_device_info_audio_device_codec_ex_device0_priority 50
#define root_device_info_audio_device_codec_ex_device0_preload_exists 1
#define root_device_info_audio_device_codec_ex_device0_preload 0
#define root_device_info_audio_device_codec_ex_device0_permission_exists 1
#define root_device_info_audio_device_codec_ex_device0_permission 438
#define root_device_info_audio_device_codec_ex_device0_moduleName_exists 1
#define root_device_info_audio_device_codec_ex_device0_moduleName "CODEC_TFA9879"
#define root_device_info_audio_device_codec_ex_device0_serviceName_exists 1
#define root_device_info_audio_device_codec_ex_device0_serviceName "codec_service_1"
#define root_device_info_audio_device_codec_ex_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_codec_ex_device0_deviceMatchAttr "hdf_codec_driver_ex"

// hcs node macros: root/device_info/audio/device_dsp
#define root_device_info_audio_device_dsp_exists 1
#define root_device_info_audio_device_dsp_foreach_child(func) \
    func(root_device_info_audio_device_dsp_device0)

#define root_device_info_audio_device_dsp_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_dsp_device0, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_dsp/device0
#define root_device_info_audio_device_dsp_device0_exists 1
#define root_device_info_audio_device_dsp_device0_policy_exists 1
#define root_device_info_audio_device_dsp_device0_policy 1
#define root_device_info_audio_device_dsp_device0_priority_exists 1
#define root_device_info_audio_device_dsp_device0_priority 50
#define root_device_info_audio_device_dsp_device0_preload_exists 1
#define root_device_info_audio_device_dsp_device0_preload 0
#define root_device_info_audio_device_dsp_device0_permission_exists 1
#define root_device_info_audio_device_dsp_device0_permission 438
#define root_device_info_audio_device_dsp_device0_moduleName_exists 1
#define root_device_info_audio_device_dsp_device0_moduleName "DSP"
#define root_device_info_audio_device_dsp_device0_serviceName_exists 1
#define root_device_info_audio_device_dsp_device0_serviceName "dsp_service_0"
#define root_device_info_audio_device_dsp_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_dsp_device0_deviceMatchAttr "hdf_dsp_driver"

// hcs node macros: root/device_info/audio/device_dma
#define root_device_info_audio_device_dma_exists 1
#define root_device_info_audio_device_dma_foreach_child(func) \
    func(root_device_info_audio_device_dma_device0)

#define root_device_info_audio_device_dma_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_dma_device0, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_dma/device0
#define root_device_info_audio_device_dma_device0_exists 1
#define root_device_info_audio_device_dma_device0_policy_exists 1
#define root_device_info_audio_device_dma_device0_policy 1
#define root_device_info_audio_device_dma_device0_priority_exists 1
#define root_device_info_audio_device_dma_device0_priority 50
#define root_device_info_audio_device_dma_device0_preload_exists 1
#define root_device_info_audio_device_dma_device0_preload 0
#define root_device_info_audio_device_dma_device0_permission_exists 1
#define root_device_info_audio_device_dma_device0_permission 438
#define root_device_info_audio_device_dma_device0_moduleName_exists 1
#define root_device_info_audio_device_dma_device0_moduleName "DMA_HI3516"
#define root_device_info_audio_device_dma_device0_serviceName_exists 1
#define root_device_info_audio_device_dma_device0_serviceName "dma_service_0"
#define root_device_info_audio_device_dma_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_dma_device0_deviceMatchAttr "hdf_dma_driver"

// hcs node macros: root/device_info/audio/device_audio
#define root_device_info_audio_device_audio_exists 1
#define root_device_info_audio_device_audio_foreach_child(func) \
    func(root_device_info_audio_device_audio_device0) \
    func(root_device_info_audio_device_audio_device1)

#define root_device_info_audio_device_audio_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_audio_device0, __VA_ARGS__) \
    func(root_device_info_audio_device_audio_device1, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_audio/device0
#define root_device_info_audio_device_audio_device0_exists 1
#define root_device_info_audio_device_audio_device0_policy_exists 1
#define root_device_info_audio_device_audio_device0_policy 2
#define root_device_info_audio_device_audio_device0_priority_exists 1
#define root_device_info_audio_device_audio_device0_priority 60
#define root_device_info_audio_device_audio_device0_preload_exists 1
#define root_device_info_audio_device_audio_device0_preload 0
#define root_device_info_audio_device_audio_device0_permission_exists 1
#define root_device_info_audio_device_audio_device0_permission 438
#define root_device_info_audio_device_audio_device0_moduleName_exists 1
#define root_device_info_audio_device_audio_device0_moduleName "HDF_AUDIO"
#define root_device_info_audio_device_audio_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_audio_device0_deviceMatchAttr "hdf_audio_driver_0"
#define root_device_info_audio_device_audio_device0_serviceName_exists 1
#define root_device_info_audio_device_audio_device0_serviceName "hdf_audio_codec_dev0"

// hcs node macros: root/device_info/audio/device_audio/device1
#define root_device_info_audio_device_audio_device1_exists 1
#define root_device_info_audio_device_audio_device1_policy_exists 1
#define root_device_info_audio_device_audio_device1_policy 2
#define root_device_info_audio_device_audio_device1_priority_exists 1
#define root_device_info_audio_device_audio_device1_priority 60
#define root_device_info_audio_device_audio_device1_preload_exists 1
#define root_device_info_audio_device_audio_device1_preload 0
#define root_device_info_audio_device_audio_device1_permission_exists 1
#define root_device_info_audio_device_audio_device1_permission 438
#define root_device_info_audio_device_audio_device1_moduleName_exists 1
#define root_device_info_audio_device_audio_device1_moduleName "HDF_AUDIO"
#define root_device_info_audio_device_audio_device1_deviceMatchAttr_exists 1
#define root_device_info_audio_device_audio_device1_deviceMatchAttr "hdf_audio_driver_1"
#define root_device_info_audio_device_audio_device1_serviceName_exists 1
#define root_device_info_audio_device_audio_device1_serviceName "hdf_audio_smartpa_dev0"

// hcs node macros: root/device_info/audio/device_stream
#define root_device_info_audio_device_stream_exists 1
#define root_device_info_audio_device_stream_foreach_child(func) \
    func(root_device_info_audio_device_stream_device0) \
    func(root_device_info_audio_device_stream_device1)

#define root_device_info_audio_device_stream_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_stream_device0, __VA_ARGS__) \
    func(root_device_info_audio_device_stream_device1, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_stream/device0
#define root_device_info_audio_device_stream_device0_exists 1
#define root_device_info_audio_device_stream_device0_policy_exists 1
#define root_device_info_audio_device_stream_device0_policy 2
#define root_device_info_audio_device_stream_device0_priority_exists 1
#define root_device_info_audio_device_stream_device0_priority 80
#define root_device_info_audio_device_stream_device0_preload_exists 1
#define root_device_info_audio_device_stream_device0_preload 0
#define root_device_info_audio_device_stream_device0_permission_exists 1
#define root_device_info_audio_device_stream_device0_permission 438
#define root_device_info_audio_device_stream_device0_moduleName_exists 1
#define root_device_info_audio_device_stream_device0_moduleName "HDF_AUDIO_STREAM"
#define root_device_info_audio_device_stream_device0_serviceName_exists 1
#define root_device_info_audio_device_stream_device0_serviceName "hdf_audio_render"
#define root_device_info_audio_device_stream_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_stream_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/audio/device_stream/device1
#define root_device_info_audio_device_stream_device1_exists 1
#define root_device_info_audio_device_stream_device1_policy_exists 1
#define root_device_info_audio_device_stream_device1_policy 2
#define root_device_info_audio_device_stream_device1_priority_exists 1
#define root_device_info_audio_device_stream_device1_priority 80
#define root_device_info_audio_device_stream_device1_preload_exists 1
#define root_device_info_audio_device_stream_device1_preload 0
#define root_device_info_audio_device_stream_device1_permission_exists 1
#define root_device_info_audio_device_stream_device1_permission 438
#define root_device_info_audio_device_stream_device1_moduleName_exists 1
#define root_device_info_audio_device_stream_device1_moduleName "HDF_AUDIO_STREAM"
#define root_device_info_audio_device_stream_device1_serviceName_exists 1
#define root_device_info_audio_device_stream_device1_serviceName "hdf_audio_capture"
#define root_device_info_audio_device_stream_device1_deviceMatchAttr_exists 1
#define root_device_info_audio_device_stream_device1_deviceMatchAttr ""

// hcs node macros: root/device_info/audio/device_control
#define root_device_info_audio_device_control_exists 1
#define root_device_info_audio_device_control_foreach_child(func) \
    func(root_device_info_audio_device_control_device0)

#define root_device_info_audio_device_control_foreach_child_vargs(func, ...) \
    func(root_device_info_audio_device_control_device0, __VA_ARGS__)


// hcs node macros: root/device_info/audio/device_control/device0
#define root_device_info_audio_device_control_device0_exists 1
#define root_device_info_audio_device_control_device0_policy_exists 1
#define root_device_info_audio_device_control_device0_policy 2
#define root_device_info_audio_device_control_device0_priority_exists 1
#define root_device_info_audio_device_control_device0_priority 80
#define root_device_info_audio_device_control_device0_preload_exists 1
#define root_device_info_audio_device_control_device0_preload 0
#define root_device_info_audio_device_control_device0_permission_exists 1
#define root_device_info_audio_device_control_device0_permission 438
#define root_device_info_audio_device_control_device0_moduleName_exists 1
#define root_device_info_audio_device_control_device0_moduleName "HDF_AUDIO_CONTROL"
#define root_device_info_audio_device_control_device0_serviceName_exists 1
#define root_device_info_audio_device_control_device0_serviceName "hdf_audio_control"
#define root_device_info_audio_device_control_device0_deviceMatchAttr_exists 1
#define root_device_info_audio_device_control_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/vibrator
#define root_device_info_vibrator_exists 1
#define root_device_info_vibrator_foreach_child(func) \
    func(root_device_info_vibrator_device_vibrator) \
    func(root_device_info_vibrator_device_linear_vibrator)

#define root_device_info_vibrator_foreach_child_vargs(func, ...) \
    func(root_device_info_vibrator_device_vibrator, __VA_ARGS__) \
    func(root_device_info_vibrator_device_linear_vibrator, __VA_ARGS__)

#define root_device_info_vibrator_hostName_exists 1
#define root_device_info_vibrator_hostName "vibrator_host"

// hcs node macros: root/device_info/vibrator/device_vibrator
#define root_device_info_vibrator_device_vibrator_exists 1
#define root_device_info_vibrator_device_vibrator_foreach_child(func) \
    func(root_device_info_vibrator_device_vibrator_device0)

#define root_device_info_vibrator_device_vibrator_foreach_child_vargs(func, ...) \
    func(root_device_info_vibrator_device_vibrator_device0, __VA_ARGS__)


// hcs node macros: root/device_info/vibrator/device_vibrator/device0
#define root_device_info_vibrator_device_vibrator_device0_exists 1
#define root_device_info_vibrator_device_vibrator_device0_policy_exists 1
#define root_device_info_vibrator_device_vibrator_device0_policy 2
#define root_device_info_vibrator_device_vibrator_device0_priority_exists 1
#define root_device_info_vibrator_device_vibrator_device0_priority 100
#define root_device_info_vibrator_device_vibrator_device0_preload_exists 1
#define root_device_info_vibrator_device_vibrator_device0_preload 0
#define root_device_info_vibrator_device_vibrator_device0_permission_exists 1
#define root_device_info_vibrator_device_vibrator_device0_permission 436
#define root_device_info_vibrator_device_vibrator_device0_moduleName_exists 1
#define root_device_info_vibrator_device_vibrator_device0_moduleName "HDF_VIBRATOR"
#define root_device_info_vibrator_device_vibrator_device0_serviceName_exists 1
#define root_device_info_vibrator_device_vibrator_device0_serviceName "hdf_misc_vibrator"
#define root_device_info_vibrator_device_vibrator_device0_deviceMatchAttr_exists 1
#define root_device_info_vibrator_device_vibrator_device0_deviceMatchAttr "hdf_vibrator_driver"

// hcs node macros: root/device_info/vibrator/device_linear_vibrator
#define root_device_info_vibrator_device_linear_vibrator_exists 1
#define root_device_info_vibrator_device_linear_vibrator_foreach_child(func) \
    func(root_device_info_vibrator_device_linear_vibrator_device0)

#define root_device_info_vibrator_device_linear_vibrator_foreach_child_vargs(func, ...) \
    func(root_device_info_vibrator_device_linear_vibrator_device0, __VA_ARGS__)


// hcs node macros: root/device_info/vibrator/device_linear_vibrator/device0
#define root_device_info_vibrator_device_linear_vibrator_device0_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_policy_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_policy 1
#define root_device_info_vibrator_device_linear_vibrator_device0_priority_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_priority 105
#define root_device_info_vibrator_device_linear_vibrator_device0_preload_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_preload 0
#define root_device_info_vibrator_device_linear_vibrator_device0_permission_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_permission 436
#define root_device_info_vibrator_device_linear_vibrator_device0_moduleName_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_moduleName "HDF_LINEAR_VIBRATOR"
#define root_device_info_vibrator_device_linear_vibrator_device0_serviceName_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_serviceName "hdf_misc_linear_vibrator"
#define root_device_info_vibrator_device_linear_vibrator_device0_deviceMatchAttr_exists 1
#define root_device_info_vibrator_device_linear_vibrator_device0_deviceMatchAttr "hdf_linear_vibrator_driver"
#define root_device_info_vibrator_priority_exists 1
#define root_device_info_vibrator_priority 100

// hcs node macros: root/device_info/dsoftbus
#define root_device_info_dsoftbus_exists 1
#define root_device_info_dsoftbus_foreach_child(func) \
    func(root_device_info_dsoftbus_device_dsoftbus)

#define root_device_info_dsoftbus_foreach_child_vargs(func, ...) \
    func(root_device_info_dsoftbus_device_dsoftbus, __VA_ARGS__)

#define root_device_info_dsoftbus_hostName_exists 1
#define root_device_info_dsoftbus_hostName "dsoftbus_host"

// hcs node macros: root/device_info/dsoftbus/device_dsoftbus
#define root_device_info_dsoftbus_device_dsoftbus_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_foreach_child(func) \
    func(root_device_info_dsoftbus_device_dsoftbus_device0)

#define root_device_info_dsoftbus_device_dsoftbus_foreach_child_vargs(func, ...) \
    func(root_device_info_dsoftbus_device_dsoftbus_device0, __VA_ARGS__)


// hcs node macros: root/device_info/dsoftbus/device_dsoftbus/device0
#define root_device_info_dsoftbus_device_dsoftbus_device0_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_policy_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_policy 2
#define root_device_info_dsoftbus_device_dsoftbus_device0_priority_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_priority 100
#define root_device_info_dsoftbus_device_dsoftbus_device0_preload_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_preload 0
#define root_device_info_dsoftbus_device_dsoftbus_device0_permission_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_permission 436
#define root_device_info_dsoftbus_device_dsoftbus_device0_moduleName_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_moduleName "HDF_DSOFTBUS"
#define root_device_info_dsoftbus_device_dsoftbus_device0_serviceName_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_serviceName "hdf_dsoftbus"
#define root_device_info_dsoftbus_device_dsoftbus_device0_deviceMatchAttr_exists 1
#define root_device_info_dsoftbus_device_dsoftbus_device0_deviceMatchAttr ""
#define root_device_info_dsoftbus_priority_exists 1
#define root_device_info_dsoftbus_priority 100

// hcs node macros: root/device_info/light
#define root_device_info_light_exists 1
#define root_device_info_light_foreach_child(func) \
    func(root_device_info_light_device_light)

#define root_device_info_light_foreach_child_vargs(func, ...) \
    func(root_device_info_light_device_light, __VA_ARGS__)

#define root_device_info_light_hostName_exists 1
#define root_device_info_light_hostName "light_host"

// hcs node macros: root/device_info/light/device_light
#define root_device_info_light_device_light_exists 1
#define root_device_info_light_device_light_foreach_child(func) \
    func(root_device_info_light_device_light_device0)

#define root_device_info_light_device_light_foreach_child_vargs(func, ...) \
    func(root_device_info_light_device_light_device0, __VA_ARGS__)


// hcs node macros: root/device_info/light/device_light/device0
#define root_device_info_light_device_light_device0_exists 1
#define root_device_info_light_device_light_device0_policy_exists 1
#define root_device_info_light_device_light_device0_policy 2
#define root_device_info_light_device_light_device0_priority_exists 1
#define root_device_info_light_device_light_device0_priority 100
#define root_device_info_light_device_light_device0_preload_exists 1
#define root_device_info_light_device_light_device0_preload 0
#define root_device_info_light_device_light_device0_permission_exists 1
#define root_device_info_light_device_light_device0_permission 436
#define root_device_info_light_device_light_device0_moduleName_exists 1
#define root_device_info_light_device_light_device0_moduleName "HDF_LIGHT"
#define root_device_info_light_device_light_device0_serviceName_exists 1
#define root_device_info_light_device_light_device0_serviceName "hdf_light"
#define root_device_info_light_device_light_device0_deviceMatchAttr_exists 1
#define root_device_info_light_device_light_device0_deviceMatchAttr "hdf_light_driver"
#define root_device_info_light_priority_exists 1
#define root_device_info_light_priority 100

// hcs node macros: root/device_info/test_host
#define root_device_info_test_host_exists 1
#define root_device_info_test_host_foreach_child(func) \
    func(root_device_info_test_host_device_hdf_test) \
    func(root_device_info_test_host_device_platform_test) \
    func(root_device_info_test_host_device_sensor_test)

#define root_device_info_test_host_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_hdf_test, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test, __VA_ARGS__) \
    func(root_device_info_test_host_device_sensor_test, __VA_ARGS__)

#define root_device_info_test_host_hostName_exists 1
#define root_device_info_test_host_hostName "test_host"
#define root_device_info_test_host_priority_exists 1
#define root_device_info_test_host_priority 180

// hcs node macros: root/device_info/test_host/device_hdf_test
#define root_device_info_test_host_device_hdf_test_exists 1
#define root_device_info_test_host_device_hdf_test_foreach_child(func) \
    func(root_device_info_test_host_device_hdf_test_device0) \
    func(root_device_info_test_host_device_hdf_test_device1) \
    func(root_device_info_test_host_device_hdf_test_device2)

#define root_device_info_test_host_device_hdf_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_hdf_test_device0, __VA_ARGS__) \
    func(root_device_info_test_host_device_hdf_test_device1, __VA_ARGS__) \
    func(root_device_info_test_host_device_hdf_test_device2, __VA_ARGS__)


// hcs node macros: root/device_info/test_host/device_hdf_test/device0
#define root_device_info_test_host_device_hdf_test_device0_exists 1
#define root_device_info_test_host_device_hdf_test_device0_policy_exists 1
#define root_device_info_test_host_device_hdf_test_device0_policy 2
#define root_device_info_test_host_device_hdf_test_device0_priority_exists 1
#define root_device_info_test_host_device_hdf_test_device0_priority 10
#define root_device_info_test_host_device_hdf_test_device0_preload_exists 1
#define root_device_info_test_host_device_hdf_test_device0_preload 0
#define root_device_info_test_host_device_hdf_test_device0_permission_exists 1
#define root_device_info_test_host_device_hdf_test_device0_permission 420
#define root_device_info_test_host_device_hdf_test_device0_moduleName_exists 1
#define root_device_info_test_host_device_hdf_test_device0_moduleName "HDF_TEST"
#define root_device_info_test_host_device_hdf_test_device0_serviceName_exists 1
#define root_device_info_test_host_device_hdf_test_device0_serviceName "HDF_TEST"
#define root_device_info_test_host_device_hdf_test_device0_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_hdf_test_device0_deviceMatchAttr ""

// hcs node macros: root/device_info/test_host/device_hdf_test/device1
#define root_device_info_test_host_device_hdf_test_device1_exists 1
#define root_device_info_test_host_device_hdf_test_device1_policy_exists 1
#define root_device_info_test_host_device_hdf_test_device1_policy 2
#define root_device_info_test_host_device_hdf_test_device1_priority_exists 1
#define root_device_info_test_host_device_hdf_test_device1_priority 10
#define root_device_info_test_host_device_hdf_test_device1_preload_exists 1
#define root_device_info_test_host_device_hdf_test_device1_preload 0
#define root_device_info_test_host_device_hdf_test_device1_permission_exists 1
#define root_device_info_test_host_device_hdf_test_device1_permission 420
#define root_device_info_test_host_device_hdf_test_device1_moduleName_exists 1
#define root_device_info_test_host_device_hdf_test_device1_moduleName "sample_driver"
#define root_device_info_test_host_device_hdf_test_device1_serviceName_exists 1
#define root_device_info_test_host_device_hdf_test_device1_serviceName "sample_service"
#define root_device_info_test_host_device_hdf_test_device1_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_hdf_test_device1_deviceMatchAttr ""

// hcs node macros: root/device_info/test_host/device_hdf_test/device2
#define root_device_info_test_host_device_hdf_test_device2_exists 1
#define root_device_info_test_host_device_hdf_test_device2_policy_exists 1
#define root_device_info_test_host_device_hdf_test_device2_policy 2
#define root_device_info_test_host_device_hdf_test_device2_priority_exists 1
#define root_device_info_test_host_device_hdf_test_device2_priority 10
#define root_device_info_test_host_device_hdf_test_device2_preload_exists 1
#define root_device_info_test_host_device_hdf_test_device2_preload 0
#define root_device_info_test_host_device_hdf_test_device2_permission_exists 1
#define root_device_info_test_host_device_hdf_test_device2_permission 420
#define root_device_info_test_host_device_hdf_test_device2_moduleName_exists 1
#define root_device_info_test_host_device_hdf_test_device2_moduleName "pm_test_driver"
#define root_device_info_test_host_device_hdf_test_device2_serviceName_exists 1
#define root_device_info_test_host_device_hdf_test_device2_serviceName "pm_test_service"
#define root_device_info_test_host_device_hdf_test_device2_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_hdf_test_device2_deviceMatchAttr ""

// hcs node macros: root/device_info/test_host/device_platform_test
#define root_device_info_test_host_device_platform_test_exists 1
#define root_device_info_test_host_device_platform_test_foreach_child(func) \
    func(root_device_info_test_host_device_platform_test_gpio_tester) \
    func(root_device_info_test_host_device_platform_test_i2c_tester) \
    func(root_device_info_test_host_device_platform_test_pwm_test) \
    func(root_device_info_test_host_device_platform_test_spi_test) \
    func(root_device_info_test_host_device_platform_test_emmc_tester) \
    func(root_device_info_test_host_device_platform_test_sdio_tester) \
    func(root_device_info_test_host_device_platform_test_watchdog_test) \
    func(root_device_info_test_host_device_platform_test_mipi_dsi_test) \
    func(root_device_info_test_host_device_platform_test_uart_tester)

#define root_device_info_test_host_device_platform_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_platform_test_gpio_tester, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_i2c_tester, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_pwm_test, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_spi_test, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_emmc_tester, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_sdio_tester, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_watchdog_test, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_mipi_dsi_test, __VA_ARGS__) \
    func(root_device_info_test_host_device_platform_test_uart_tester, __VA_ARGS__)


// hcs node macros: root/device_info/test_host/device_platform_test/gpio_tester
#define root_device_info_test_host_device_platform_test_gpio_tester_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_policy_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_policy 1
#define root_device_info_test_host_device_platform_test_gpio_tester_priority_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_priority 101
#define root_device_info_test_host_device_platform_test_gpio_tester_preload_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_preload 0
#define root_device_info_test_host_device_platform_test_gpio_tester_permission_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_permission 420
#define root_device_info_test_host_device_platform_test_gpio_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_moduleName "PLATFORM_GPIO_TEST"
#define root_device_info_test_host_device_platform_test_gpio_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_deviceMatchAttr "GPIO_TEST"
#define root_device_info_test_host_device_platform_test_gpio_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_gpio_tester_serviceName "GPIO_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/i2c_tester
#define root_device_info_test_host_device_platform_test_i2c_tester_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_policy_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_policy 2
#define root_device_info_test_host_device_platform_test_i2c_tester_priority_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_priority 101
#define root_device_info_test_host_device_platform_test_i2c_tester_preload_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_preload 0
#define root_device_info_test_host_device_platform_test_i2c_tester_permission_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_permission 420
#define root_device_info_test_host_device_platform_test_i2c_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_moduleName "PLATFORM_I2C_TEST"
#define root_device_info_test_host_device_platform_test_i2c_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_deviceMatchAttr "I2C_TEST"
#define root_device_info_test_host_device_platform_test_i2c_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_i2c_tester_serviceName "I2C_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/pwm_test
#define root_device_info_test_host_device_platform_test_pwm_test_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_policy_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_policy 1
#define root_device_info_test_host_device_platform_test_pwm_test_priority_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_priority 110
#define root_device_info_test_host_device_platform_test_pwm_test_preload_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_preload 0
#define root_device_info_test_host_device_platform_test_pwm_test_permission_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_permission 420
#define root_device_info_test_host_device_platform_test_pwm_test_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_moduleName "PLATFORM_PWM_TEST"
#define root_device_info_test_host_device_platform_test_pwm_test_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_serviceName "PWM_TEST"
#define root_device_info_test_host_device_platform_test_pwm_test_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_pwm_test_deviceMatchAttr "PWM_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/spi_test
#define root_device_info_test_host_device_platform_test_spi_test_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_policy_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_policy 1
#define root_device_info_test_host_device_platform_test_spi_test_priority_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_priority 100
#define root_device_info_test_host_device_platform_test_spi_test_preload_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_preload 0
#define root_device_info_test_host_device_platform_test_spi_test_permission_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_permission 420
#define root_device_info_test_host_device_platform_test_spi_test_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_moduleName "PLATFORM_SPI_TEST"
#define root_device_info_test_host_device_platform_test_spi_test_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_serviceName "SPI_TEST"
#define root_device_info_test_host_device_platform_test_spi_test_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_spi_test_deviceMatchAttr "SPI_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/emmc_tester
#define root_device_info_test_host_device_platform_test_emmc_tester_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_policy_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_policy 1
#define root_device_info_test_host_device_platform_test_emmc_tester_priority_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_priority 100
#define root_device_info_test_host_device_platform_test_emmc_tester_preload_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_preload 0
#define root_device_info_test_host_device_platform_test_emmc_tester_permission_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_permission 420
#define root_device_info_test_host_device_platform_test_emmc_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_moduleName "PLATFORM_EMMC_TEST"
#define root_device_info_test_host_device_platform_test_emmc_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_serviceName "EMMC_TEST"
#define root_device_info_test_host_device_platform_test_emmc_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_emmc_tester_deviceMatchAttr "EMMC_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/sdio_tester
#define root_device_info_test_host_device_platform_test_sdio_tester_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_policy_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_policy 1
#define root_device_info_test_host_device_platform_test_sdio_tester_priority_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_priority 100
#define root_device_info_test_host_device_platform_test_sdio_tester_preload_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_preload 0
#define root_device_info_test_host_device_platform_test_sdio_tester_permission_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_permission 420
#define root_device_info_test_host_device_platform_test_sdio_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_moduleName "PLATFORM_SDIO_TEST"
#define root_device_info_test_host_device_platform_test_sdio_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_serviceName "SDIO_TEST"
#define root_device_info_test_host_device_platform_test_sdio_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_sdio_tester_deviceMatchAttr "SDIO_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/watchdog_test
#define root_device_info_test_host_device_platform_test_watchdog_test_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_policy_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_policy 1
#define root_device_info_test_host_device_platform_test_watchdog_test_priority_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_priority 110
#define root_device_info_test_host_device_platform_test_watchdog_test_preload_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_preload 0
#define root_device_info_test_host_device_platform_test_watchdog_test_permission_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_permission 432
#define root_device_info_test_host_device_platform_test_watchdog_test_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_moduleName "PLATFORM_WATCHDOG_TEST"
#define root_device_info_test_host_device_platform_test_watchdog_test_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_serviceName "WATCHDOG_TEST"
#define root_device_info_test_host_device_platform_test_watchdog_test_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_watchdog_test_deviceMatchAttr "WATCHDOG_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/mipi_dsi_test
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_policy_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_policy 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_priority_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_priority 100
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_preload_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_preload 0
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_permission_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_permission 420
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_moduleName "PLATFORM_MIPI_DSI_TEST"
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_serviceName "PLATFORM_MIPI_DSI_TEST"
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_mipi_dsi_test_deviceMatchAttr "PLATFORM_MIPI_DSI_TEST"

// hcs node macros: root/device_info/test_host/device_platform_test/uart_tester
#define root_device_info_test_host_device_platform_test_uart_tester_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_policy_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_policy 1
#define root_device_info_test_host_device_platform_test_uart_tester_priority_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_priority 100
#define root_device_info_test_host_device_platform_test_uart_tester_preload_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_preload 0
#define root_device_info_test_host_device_platform_test_uart_tester_permission_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_permission 420
#define root_device_info_test_host_device_platform_test_uart_tester_moduleName_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_moduleName "PLATFORM_UART_TEST"
#define root_device_info_test_host_device_platform_test_uart_tester_serviceName_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_serviceName "UART_TEST"
#define root_device_info_test_host_device_platform_test_uart_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_platform_test_uart_tester_deviceMatchAttr "UART_TEST"

// hcs node macros: root/device_info/test_host/device_sensor_test
#define root_device_info_test_host_device_sensor_test_exists 1
#define root_device_info_test_host_device_sensor_test_foreach_child(func) \
    func(root_device_info_test_host_device_sensor_test_sensor_tester)

#define root_device_info_test_host_device_sensor_test_foreach_child_vargs(func, ...) \
    func(root_device_info_test_host_device_sensor_test_sensor_tester, __VA_ARGS__)


// hcs node macros: root/device_info/test_host/device_sensor_test/sensor_tester
#define root_device_info_test_host_device_sensor_test_sensor_tester_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_policy_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_policy 0
#define root_device_info_test_host_device_sensor_test_sensor_tester_priority_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_priority 100
#define root_device_info_test_host_device_sensor_test_sensor_tester_preload_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_preload 0
#define root_device_info_test_host_device_sensor_test_sensor_tester_permission_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_permission 420
#define root_device_info_test_host_device_sensor_test_sensor_tester_moduleName_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_moduleName "HDF_SENSOR_TEST_DRIVER"
#define root_device_info_test_host_device_sensor_test_sensor_tester_serviceName_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_serviceName ""
#define root_device_info_test_host_device_sensor_test_sensor_tester_deviceMatchAttr_exists 1
#define root_device_info_test_host_device_sensor_test_sensor_tester_deviceMatchAttr ""

// hcs node macros: root/platform
#define root_platform_exists 1
#define root_platform_foreach_child(func) \
    func(root_platform_i2c_config) \
    func(root_platform_watchdog_config) \
    func(root_platform_pwm_config) \
    func(root_platform_uart_config) \
    func(root_platform_sdio_config) \
    func(root_platform_emmc_config) \
    func(root_platform_spi_config)

#define root_platform_foreach_child_vargs(func, ...) \
    func(root_platform_i2c_config, __VA_ARGS__) \
    func(root_platform_watchdog_config, __VA_ARGS__) \
    func(root_platform_pwm_config, __VA_ARGS__) \
    func(root_platform_uart_config, __VA_ARGS__) \
    func(root_platform_sdio_config, __VA_ARGS__) \
    func(root_platform_emmc_config, __VA_ARGS__) \
    func(root_platform_spi_config, __VA_ARGS__)


// hcs node macros: root/platform/i2c_config
#define root_platform_i2c_config_exists 1
#define root_platform_i2c_config_foreach_child(func) \
    func(root_platform_i2c_config_controller_0x120b0000) \
    func(root_platform_i2c_config_controller_0x120b1000) \
    func(root_platform_i2c_config_controller_0x120b2000) \
    func(root_platform_i2c_config_controller_0x120b3000) \
    func(root_platform_i2c_config_controller_0x120b4000) \
    func(root_platform_i2c_config_controller_0x120b5000) \
    func(root_platform_i2c_config_controller_0x120b6000) \
    func(root_platform_i2c_config_controller_0x120b7000)

#define root_platform_i2c_config_foreach_child_vargs(func, ...) \
    func(root_platform_i2c_config_controller_0x120b0000, __VA_ARGS__) \
    func(root_platform_i2c_config_controller_0x120b1000, __VA_ARGS__) \
    func(root_platform_i2c_config_controller_0x120b2000, __VA_ARGS__) \
    func(root_platform_i2c_config_controller_0x120b3000, __VA_ARGS__) \
    func(root_platform_i2c_config_controller_0x120b4000, __VA_ARGS__) \
    func(root_platform_i2c_config_controller_0x120b5000, __VA_ARGS__) \
    func(root_platform_i2c_config_controller_0x120b6000, __VA_ARGS__) \
    func(root_platform_i2c_config_controller_0x120b7000, __VA_ARGS__)

#define root_platform_i2c_config_match_attr_exists 1
#define root_platform_i2c_config_match_attr "linux_i2c_adapter"

// hcs node macros: root/platform/i2c_config/controller_0x120b0000
#define root_platform_i2c_config_controller_0x120b0000_exists 1
#define root_platform_i2c_config_controller_0x120b0000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b0000_bus 0

// hcs node macros: root/platform/i2c_config/controller_0x120b1000
#define root_platform_i2c_config_controller_0x120b1000_exists 1
#define root_platform_i2c_config_controller_0x120b1000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b1000_bus 1

// hcs node macros: root/platform/i2c_config/controller_0x120b2000
#define root_platform_i2c_config_controller_0x120b2000_exists 1
#define root_platform_i2c_config_controller_0x120b2000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b2000_bus 2

// hcs node macros: root/platform/i2c_config/controller_0x120b3000
#define root_platform_i2c_config_controller_0x120b3000_exists 1
#define root_platform_i2c_config_controller_0x120b3000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b3000_bus 3

// hcs node macros: root/platform/i2c_config/controller_0x120b4000
#define root_platform_i2c_config_controller_0x120b4000_exists 1
#define root_platform_i2c_config_controller_0x120b4000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b4000_bus 4

// hcs node macros: root/platform/i2c_config/controller_0x120b5000
#define root_platform_i2c_config_controller_0x120b5000_exists 1
#define root_platform_i2c_config_controller_0x120b5000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b5000_bus 5

// hcs node macros: root/platform/i2c_config/controller_0x120b6000
#define root_platform_i2c_config_controller_0x120b6000_exists 1
#define root_platform_i2c_config_controller_0x120b6000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b6000_bus 6

// hcs node macros: root/platform/i2c_config/controller_0x120b7000
#define root_platform_i2c_config_controller_0x120b7000_exists 1
#define root_platform_i2c_config_controller_0x120b7000_bus_exists 1
#define root_platform_i2c_config_controller_0x120b7000_bus 7

// hcs node macros: root/platform/watchdog_config
#define root_platform_watchdog_config_exists 1
#define root_platform_watchdog_config_foreach_child(func) \
    func(root_platform_watchdog_config_device_0x12050000)

#define root_platform_watchdog_config_foreach_child_vargs(func, ...) \
    func(root_platform_watchdog_config_device_0x12050000, __VA_ARGS__)


// hcs node macros: root/platform/watchdog_config/device_0x12050000
#define root_platform_watchdog_config_device_0x12050000_exists 1
#define root_platform_watchdog_config_device_0x12050000_id_exists 1
#define root_platform_watchdog_config_device_0x12050000_id 0
#define root_platform_watchdog_config_device_0x12050000_match_attr_exists 1
#define root_platform_watchdog_config_device_0x12050000_match_attr "hisilicon_hi35xx_watchdog_0"
#define root_platform_watchdog_config_device_0x12050000_serviceName_exists 1
#define root_platform_watchdog_config_device_0x12050000_serviceName "HDF_PLATFORM_WATCHDOG_0"

// hcs node macros: root/platform/pwm_config
#define root_platform_pwm_config_exists 1
#define root_platform_pwm_config_foreach_child(func) \
    func(root_platform_pwm_config_device_0x12070000) \
    func(root_platform_pwm_config_device_0x12070020)

#define root_platform_pwm_config_foreach_child_vargs(func, ...) \
    func(root_platform_pwm_config_device_0x12070000, __VA_ARGS__) \
    func(root_platform_pwm_config_device_0x12070020, __VA_ARGS__)


// hcs node macros: root/platform/pwm_config/device_0x12070000
#define root_platform_pwm_config_device_0x12070000_exists 1
#define root_platform_pwm_config_device_0x12070000_match_attr_exists 1
#define root_platform_pwm_config_device_0x12070000_match_attr "hisilicon_hi35xx_pwm_0"
#define root_platform_pwm_config_device_0x12070000_serviceName_exists 1
#define root_platform_pwm_config_device_0x12070000_serviceName ""
#define root_platform_pwm_config_device_0x12070000_num_exists 1
#define root_platform_pwm_config_device_0x12070000_num 0
#define root_platform_pwm_config_device_0x12070000_base_exists 1
#define root_platform_pwm_config_device_0x12070000_base 302448640

// hcs node macros: root/platform/pwm_config/device_0x12070020
#define root_platform_pwm_config_device_0x12070020_exists 1
#define root_platform_pwm_config_device_0x12070020_match_attr_exists 1
#define root_platform_pwm_config_device_0x12070020_match_attr "hisilicon_hi35xx_pwm_1"
#define root_platform_pwm_config_device_0x12070020_num_exists 1
#define root_platform_pwm_config_device_0x12070020_num 1
#define root_platform_pwm_config_device_0x12070020_base_exists 1
#define root_platform_pwm_config_device_0x12070020_base 302448672
#define root_platform_pwm_config_device_0x12070020_serviceName_exists 1
#define root_platform_pwm_config_device_0x12070020_serviceName ""

// hcs node macros: root/platform/uart_config
#define root_platform_uart_config_exists 1
#define root_platform_uart_config_foreach_child(func) \
    func(root_platform_uart_config_device_uart_0x0000) \
    func(root_platform_uart_config_device_uart_0x0001)

#define root_platform_uart_config_foreach_child_vargs(func, ...) \
    func(root_platform_uart_config_device_uart_0x0000, __VA_ARGS__) \
    func(root_platform_uart_config_device_uart_0x0001, __VA_ARGS__)


// hcs node macros: root/platform/uart_config/device_uart_0x0000
#define root_platform_uart_config_device_uart_0x0000_exists 1
#define root_platform_uart_config_device_uart_0x0000_match_attr_exists 1
#define root_platform_uart_config_device_uart_0x0000_match_attr "hisilicon_hi35xx_uart_0"
#define root_platform_uart_config_device_uart_0x0000_serviceName_exists 1
#define root_platform_uart_config_device_uart_0x0000_serviceName ""
#define root_platform_uart_config_device_uart_0x0000_driver_name_exists 1
#define root_platform_uart_config_device_uart_0x0000_driver_name "ttyAMA"
#define root_platform_uart_config_device_uart_0x0000_num_exists 1
#define root_platform_uart_config_device_uart_0x0000_num 0

// hcs node macros: root/platform/uart_config/device_uart_0x0001
#define root_platform_uart_config_device_uart_0x0001_exists 1
#define root_platform_uart_config_device_uart_0x0001_num_exists 1
#define root_platform_uart_config_device_uart_0x0001_num 1
#define root_platform_uart_config_device_uart_0x0001_match_attr_exists 1
#define root_platform_uart_config_device_uart_0x0001_match_attr "hisilicon_hi35xx_uart_1"
#define root_platform_uart_config_device_uart_0x0001_serviceName_exists 1
#define root_platform_uart_config_device_uart_0x0001_serviceName ""
#define root_platform_uart_config_device_uart_0x0001_driver_name_exists 1
#define root_platform_uart_config_device_uart_0x0001_driver_name "ttyAMA"

// hcs node macros: root/platform/sdio_config
#define root_platform_sdio_config_exists 1
#define root_platform_sdio_config_foreach_child(func) \
    func(root_platform_sdio_config_controller_0x2dd1)

#define root_platform_sdio_config_foreach_child_vargs(func, ...) \
    func(root_platform_sdio_config_controller_0x2dd1, __VA_ARGS__)


// hcs node macros: root/platform/sdio_config/controller_0x2dd1
#define root_platform_sdio_config_controller_0x2dd1_exists 1
#define root_platform_sdio_config_controller_0x2dd1_match_attr_exists 1
#define root_platform_sdio_config_controller_0x2dd1_match_attr "hisilicon_hi35xx_sdio_0"
#define root_platform_sdio_config_controller_0x2dd1_hostId_exists 1
#define root_platform_sdio_config_controller_0x2dd1_hostId 2
#define root_platform_sdio_config_controller_0x2dd1_devType_exists 1
#define root_platform_sdio_config_controller_0x2dd1_devType 2

// hcs node macros: root/platform/emmc_config
#define root_platform_emmc_config_exists 1
#define root_platform_emmc_config_foreach_child(func) \
    func(root_platform_emmc_config_controller_0x00)

#define root_platform_emmc_config_foreach_child_vargs(func, ...) \
    func(root_platform_emmc_config_controller_0x00, __VA_ARGS__)


// hcs node macros: root/platform/emmc_config/controller_0x00
#define root_platform_emmc_config_controller_0x00_exists 1
#define root_platform_emmc_config_controller_0x00_match_attr_exists 1
#define root_platform_emmc_config_controller_0x00_match_attr "hisilicon_hi35xx_emmc_0"
#define root_platform_emmc_config_controller_0x00_hostId_exists 1
#define root_platform_emmc_config_controller_0x00_hostId 0
#define root_platform_emmc_config_controller_0x00_devType_exists 1
#define root_platform_emmc_config_controller_0x00_devType 0

// hcs node macros: root/platform/spi_config
#define root_platform_spi_config_exists 1
#define root_platform_spi_config_foreach_child(func) \
    func(root_platform_spi_config_controller_0x120c0000) \
    func(root_platform_spi_config_controller_0x120c1000) \
    func(root_platform_spi_config_controller_0x120c2000)

#define root_platform_spi_config_foreach_child_vargs(func, ...) \
    func(root_platform_spi_config_controller_0x120c0000, __VA_ARGS__) \
    func(root_platform_spi_config_controller_0x120c1000, __VA_ARGS__) \
    func(root_platform_spi_config_controller_0x120c2000, __VA_ARGS__)


// hcs node macros: root/platform/spi_config/controller_0x120c0000
#define root_platform_spi_config_controller_0x120c0000_exists 1
#define root_platform_spi_config_controller_0x120c0000_busNum_exists 1
#define root_platform_spi_config_controller_0x120c0000_busNum 0
#define root_platform_spi_config_controller_0x120c0000_numCs_exists 1
#define root_platform_spi_config_controller_0x120c0000_numCs 1
#define root_platform_spi_config_controller_0x120c0000_match_attr_exists 1
#define root_platform_spi_config_controller_0x120c0000_match_attr "hisilicon_hi35xx_spi_0"
#define root_platform_spi_config_controller_0x120c0000_serviceName_exists 1
#define root_platform_spi_config_controller_0x120c0000_serviceName ""

// hcs node macros: root/platform/spi_config/controller_0x120c1000
#define root_platform_spi_config_controller_0x120c1000_exists 1
#define root_platform_spi_config_controller_0x120c1000_match_attr_exists 1
#define root_platform_spi_config_controller_0x120c1000_match_attr "hisilicon_hi35xx_spi_1"
#define root_platform_spi_config_controller_0x120c1000_busNum_exists 1
#define root_platform_spi_config_controller_0x120c1000_busNum 1
#define root_platform_spi_config_controller_0x120c1000_numCs_exists 1
#define root_platform_spi_config_controller_0x120c1000_numCs 2
#define root_platform_spi_config_controller_0x120c1000_serviceName_exists 1
#define root_platform_spi_config_controller_0x120c1000_serviceName ""

// hcs node macros: root/platform/spi_config/controller_0x120c2000
#define root_platform_spi_config_controller_0x120c2000_exists 1
#define root_platform_spi_config_controller_0x120c2000_match_attr_exists 1
#define root_platform_spi_config_controller_0x120c2000_match_attr "hisilicon_hi35xx_spi_2"
#define root_platform_spi_config_controller_0x120c2000_busNum_exists 1
#define root_platform_spi_config_controller_0x120c2000_busNum 2
#define root_platform_spi_config_controller_0x120c2000_numCs_exists 1
#define root_platform_spi_config_controller_0x120c2000_numCs 1
#define root_platform_spi_config_controller_0x120c2000_serviceName_exists 1
#define root_platform_spi_config_controller_0x120c2000_serviceName ""

// hcs node macros: root/input_config
#define root_input_config_exists 1
#define root_input_config_foreach_child(func) \
    func(root_input_config_touchConfig) \
    func(root_input_config_keyConfig) \
    func(root_input_config_encoderConfig)

#define root_input_config_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig, __VA_ARGS__) \
    func(root_input_config_keyConfig, __VA_ARGS__) \
    func(root_input_config_encoderConfig, __VA_ARGS__)


// hcs node macros: root/input_config/touchConfig
#define root_input_config_touchConfig_exists 1
#define root_input_config_touchConfig_foreach_child(func) \
    func(root_input_config_touchConfig_touch0)

#define root_input_config_touchConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0, __VA_ARGS__)


// hcs node macros: root/input_config/touchConfig/touch0
#define root_input_config_touchConfig_touch0_exists 1
#define root_input_config_touchConfig_touch0_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_boardConfig) \
    func(root_input_config_touchConfig_touch0_chipConfig)

#define root_input_config_touchConfig_touch0_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_boardConfig, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_chipConfig, __VA_ARGS__)


// hcs node macros: root/input_config/touchConfig/touch0/boardConfig
#define root_input_config_touchConfig_touch0_boardConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_boardConfig_inputAttr) \
    func(root_input_config_touchConfig_touch0_boardConfig_busConfig) \
    func(root_input_config_touchConfig_touch0_boardConfig_pinConfig) \
    func(root_input_config_touchConfig_touch0_boardConfig_powerConfig) \
    func(root_input_config_touchConfig_touch0_boardConfig_featureConfig)

#define root_input_config_touchConfig_touch0_boardConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_boardConfig_inputAttr, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_boardConfig_busConfig, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_boardConfig_pinConfig, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_boardConfig_powerConfig, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_boardConfig_featureConfig, __VA_ARGS__)

#define root_input_config_touchConfig_touch0_boardConfig_match_attr_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_match_attr "touch_device1"

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/inputAttr
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_inputType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_inputType 0
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionX_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionX 480
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionY_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_solutionY 960
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_devName_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_inputAttr_devName "main_touch"

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/busConfig
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busType 0
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busNum_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_busNum 6
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_clkGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_clkGpio 86
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_dataGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_dataGpio 87
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cClkIomux_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cClkIomux_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cClkIomux_data { \
    290390088, 1027 \
}
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cDataIomux_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cDataIomux_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_busConfig_i2cDataIomux_data { \
    290390092, 1027 \
}

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/pinConfig
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstGpio 3
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intGpio_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intGpio 4
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstRegCfg_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstRegCfg_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_rstRegCfg_data { \
    288293012, 1024 \
}
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intRegCfg_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intRegCfg_array_size 2
#define root_input_config_touchConfig_touch0_boardConfig_pinConfig_intRegCfg_data { \
    288293016, 1024 \
}

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/powerConfig
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccType 2
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccNum_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccNum 20
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccValue_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vccValue 1800
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciType_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciType 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciNum_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciNum 12
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciValue_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_powerConfig_vciValue 3300

// hcs node macros: root/input_config/touchConfig/touch0/boardConfig/featureConfig
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_capacitanceTest_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_capacitanceTest 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gestureMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gestureMode 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gloverMOde_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_gloverMOde 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_coverMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_coverMode 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_chargerMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_chargerMode 0
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_knuckleMode_exists 1
#define root_input_config_touchConfig_touch0_boardConfig_featureConfig_knuckleMode 0

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig
#define root_input_config_touchConfig_touch0_chipConfig_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3)

#define root_input_config_touchConfig_touch0_chipConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2, __VA_ARGS__) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3, __VA_ARGS__)


// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip0
#define root_input_config_touchConfig_touch0_chipConfig_chip0_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence)

#define root_input_config_touchConfig_touch0_chipConfig_chip0_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence, __VA_ARGS__)

#define root_input_config_touchConfig_touch0_chipConfig_chip0_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_match_attr "zsj_gt911_5p5"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipInfo "ZIDN45100"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipVersion 0
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_chipName "gt911"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_vendorName "zsj"
#define root_input_config_touchConfig_touch0_chipConfig_chip0_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip0_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_deviceAddr 93
#define root_input_config_touchConfig_touch0_chipConfig_chip0_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_irqFlag 2
#define root_input_config_touchConfig_touch0_chipConfig_chip0_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_maxSpeed 400

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip0/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOnSeq_data { \
    4, 0, 1, 0, 3, 0, 1, 10, \
    3, 1, 2, 60, 4, 2, 0, 0 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip0_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence)

#define root_input_config_touchConfig_touch0_chipConfig_chip1_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence, __VA_ARGS__)

#define root_input_config_touchConfig_touch0_chipConfig_chip1_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_match_attr "zsj_gt911_4p0"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipInfo "ZIDN45101"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipVersion 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_chipName "gt911"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_vendorName "zsj"
#define root_input_config_touchConfig_touch0_chipConfig_chip1_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip1_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_deviceAddr 93
#define root_input_config_touchConfig_touch0_chipConfig_chip1_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_irqFlag 2
#define root_input_config_touchConfig_touch0_chipConfig_chip1_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_maxSpeed 400

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip1/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOnSeq_data { \
    4, 0, 1, 0, 3, 0, 1, 10, \
    3, 1, 2, 60, 4, 2, 0, 0 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip1_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip2
#define root_input_config_touchConfig_touch0_chipConfig_chip2_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence)

#define root_input_config_touchConfig_touch0_chipConfig_chip2_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence, __VA_ARGS__)

#define root_input_config_touchConfig_touch0_chipConfig_chip2_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_match_attr "tg_gt911_7p0"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_vendorName "tg"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipInfo "ZIDN45102"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipVersion 2
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_chipName "gt911"
#define root_input_config_touchConfig_touch0_chipConfig_chip2_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip2_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_deviceAddr 93
#define root_input_config_touchConfig_touch0_chipConfig_chip2_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_irqFlag 2
#define root_input_config_touchConfig_touch0_chipConfig_chip2_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_maxSpeed 400

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip2/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOnSeq_data { \
    4, 0, 1, 0, 3, 0, 1, 10, \
    3, 1, 2, 60, 4, 2, 0, 0 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip2_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip3
#define root_input_config_touchConfig_touch0_chipConfig_chip3_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_foreach_child(func) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence)

#define root_input_config_touchConfig_touch0_chipConfig_chip3_foreach_child_vargs(func, ...) \
    func(root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence, __VA_ARGS__)

#define root_input_config_touchConfig_touch0_chipConfig_chip3_match_attr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_match_attr "st_ft6336_2p35"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipName "ft6336"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_vendorName_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_vendorName "st"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipInfo_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipInfo "ZIDN53200"
#define root_input_config_touchConfig_touch0_chipConfig_chip3_deviceAddr_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_deviceAddr 56
#define root_input_config_touchConfig_touch0_chipConfig_chip3_irqFlag_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_irqFlag 1

// hcs node macros: root/input_config/touchConfig/touch0/chipConfig/chip3/powerSequence
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOnSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOnSeq_array_size 16
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOnSeq_data { \
    4, 2, 0, 0, 3, 1, 1, 2, \
    3, 0, 2, 5, 3, 1, 2, 200 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_suspendSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_suspendSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_suspendSeq_data { \
    3, 0, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_resumeSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_resumeSeq_array_size 4
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_resumeSeq_data { \
    3, 1, 2, 10 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOffSeq_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOffSeq_array_size 8
#define root_input_config_touchConfig_touch0_chipConfig_chip3_powerSequence_powerOffSeq_data { \
    3, 0, 2, 10, 1, 0, 2, 20 \
}
#define root_input_config_touchConfig_touch0_chipConfig_chip3_busType_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_busType 0
#define root_input_config_touchConfig_touch0_chipConfig_chip3_maxSpeed_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_maxSpeed 400
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipVersion_exists 1
#define root_input_config_touchConfig_touch0_chipConfig_chip3_chipVersion 0

// hcs node macros: root/input_config/keyConfig
#define root_input_config_keyConfig_exists 1
#define root_input_config_keyConfig_foreach_child(func) \
    func(root_input_config_keyConfig_keyInfoList)

#define root_input_config_keyConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_keyConfig_keyInfoList, __VA_ARGS__)

#define root_input_config_keyConfig_keyList_exists 1
#define root_input_config_keyConfig_keyList_array_size 7
#define root_input_config_keyConfig_keyList_data { \
    "power", "VolUp", "VolDown", "Up", "Down", "Left", "Right" \
}

// hcs node macros: root/input_config/keyConfig/keyInfoList
#define root_input_config_keyConfig_keyInfoList_exists 1
#define root_input_config_keyConfig_keyInfoList_foreach_child(func) \
    func(root_input_config_keyConfig_keyInfoList_key1) \
    func(root_input_config_keyConfig_keyInfoList_key2) \
    func(root_input_config_keyConfig_keyInfoList_key3)

#define root_input_config_keyConfig_keyInfoList_foreach_child_vargs(func, ...) \
    func(root_input_config_keyConfig_keyInfoList_key1, __VA_ARGS__) \
    func(root_input_config_keyConfig_keyInfoList_key2, __VA_ARGS__) \
    func(root_input_config_keyConfig_keyInfoList_key3, __VA_ARGS__)


// hcs node macros: root/input_config/keyConfig/keyInfoList/key1
#define root_input_config_keyConfig_keyInfoList_key1_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_match_attr_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_match_attr "key_device0"
#define root_input_config_keyConfig_keyInfoList_key1_inputType_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_inputType 1
#define root_input_config_keyConfig_keyInfoList_key1_keyName_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_keyName "power"
#define root_input_config_keyConfig_keyInfoList_key1_gpioNum_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_gpioNum 1
#define root_input_config_keyConfig_keyInfoList_key1_irqFlag_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_irqFlag 3
#define root_input_config_keyConfig_keyInfoList_key1_debounceTime_exists 1
#define root_input_config_keyConfig_keyInfoList_key1_debounceTime 80

// hcs node macros: root/input_config/keyConfig/keyInfoList/key2
#define root_input_config_keyConfig_keyInfoList_key2_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_keyName_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_keyName "volUp"
#define root_input_config_keyConfig_keyInfoList_key2_gpioNum_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_gpioNum 31
#define root_input_config_keyConfig_keyInfoList_key2_irqFlag_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_irqFlag 1
#define root_input_config_keyConfig_keyInfoList_key2_debounceTime_exists 1
#define root_input_config_keyConfig_keyInfoList_key2_debounceTime 80

// hcs node macros: root/input_config/keyConfig/keyInfoList/key3
#define root_input_config_keyConfig_keyInfoList_key3_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_keyName_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_keyName "volDown"
#define root_input_config_keyConfig_keyInfoList_key3_gpioNum_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_gpioNum 32
#define root_input_config_keyConfig_keyInfoList_key3_irqFlag_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_irqFlag 1
#define root_input_config_keyConfig_keyInfoList_key3_debounceTime_exists 1
#define root_input_config_keyConfig_keyInfoList_key3_debounceTime 80

// hcs node macros: root/input_config/encoderConfig
#define root_input_config_encoderConfig_exists 1
#define root_input_config_encoderConfig_foreach_child(func) \
    func(root_input_config_encoderConfig_encoder1)

#define root_input_config_encoderConfig_foreach_child_vargs(func, ...) \
    func(root_input_config_encoderConfig_encoder1, __VA_ARGS__)


// hcs node macros: root/input_config/encoderConfig/encoder1
#define root_input_config_encoderConfig_encoder1_exists 1
#define root_input_config_encoderConfig_encoder1_match_attr_exists 1
#define root_input_config_encoderConfig_encoder1_match_attr "EC11"
#define root_input_config_encoderConfig_encoder1_inputType_exists 1
#define root_input_config_encoderConfig_encoder1_inputType 34
#define root_input_config_encoderConfig_encoder1_gpioClk_exists 1
#define root_input_config_encoderConfig_encoder1_gpioClk 29
#define root_input_config_encoderConfig_encoder1_gpioDT_exists 1
#define root_input_config_encoderConfig_encoder1_gpioDT 30
#define root_input_config_encoderConfig_encoder1_gpioSW_exists 1
#define root_input_config_encoderConfig_encoder1_gpioSW 31

// hcs node macros: root/wlan_config
#define root_wlan_config_exists 1
#define root_wlan_config_foreach_child(func) \
    func(root_wlan_config_moduleConfig) \
    func(root_wlan_config_deviceList) \
    func(root_wlan_config_chipList)

#define root_wlan_config_foreach_child_vargs(func, ...) \
    func(root_wlan_config_moduleConfig, __VA_ARGS__) \
    func(root_wlan_config_deviceList, __VA_ARGS__) \
    func(root_wlan_config_chipList, __VA_ARGS__)

#define root_wlan_config_match_attr_exists 1
#define root_wlan_config_match_attr "hdf_wlan_driver"
#define root_wlan_config_hostChipName_exists 1
#define root_wlan_config_hostChipName "hi3516dv300"

// hcs node macros: root/wlan_config/moduleConfig
#define root_wlan_config_moduleConfig_exists 1
#define root_wlan_config_moduleConfig_foreach_child(func) \
    func(root_wlan_config_moduleConfig_Station) \
    func(root_wlan_config_moduleConfig_HostAp) \
    func(root_wlan_config_moduleConfig_P2P) \
    func(root_wlan_config_moduleConfig_Mac80211) \
    func(root_wlan_config_moduleConfig_Phy)

#define root_wlan_config_moduleConfig_foreach_child_vargs(func, ...) \
    func(root_wlan_config_moduleConfig_Station, __VA_ARGS__) \
    func(root_wlan_config_moduleConfig_HostAp, __VA_ARGS__) \
    func(root_wlan_config_moduleConfig_P2P, __VA_ARGS__) \
    func(root_wlan_config_moduleConfig_Mac80211, __VA_ARGS__) \
    func(root_wlan_config_moduleConfig_Phy, __VA_ARGS__)

#define root_wlan_config_moduleConfig_featureMap_exists 1
#define root_wlan_config_moduleConfig_featureMap 65535
#define root_wlan_config_moduleConfig_msgName_exists 1
#define root_wlan_config_moduleConfig_msgName "WalService"

// hcs node macros: root/wlan_config/moduleConfig/Station
#define root_wlan_config_moduleConfig_Station_exists 1
#define root_wlan_config_moduleConfig_Station_name_exists 1
#define root_wlan_config_moduleConfig_Station_name "sta"
#define root_wlan_config_moduleConfig_Station_mode_exists 1
#define root_wlan_config_moduleConfig_Station_mode 1

// hcs node macros: root/wlan_config/moduleConfig/HostAp
#define root_wlan_config_moduleConfig_HostAp_exists 1
#define root_wlan_config_moduleConfig_HostAp_name_exists 1
#define root_wlan_config_moduleConfig_HostAp_name "ap"
#define root_wlan_config_moduleConfig_HostAp_mode_exists 1
#define root_wlan_config_moduleConfig_HostAp_mode 0
#define root_wlan_config_moduleConfig_HostAp_vapResNum_exists 1
#define root_wlan_config_moduleConfig_HostAp_vapResNum 1
#define root_wlan_config_moduleConfig_HostAp_userResNum_exists 1
#define root_wlan_config_moduleConfig_HostAp_userResNum 8

// hcs node macros: root/wlan_config/moduleConfig/P2P
#define root_wlan_config_moduleConfig_P2P_exists 1
#define root_wlan_config_moduleConfig_P2P_name_exists 1
#define root_wlan_config_moduleConfig_P2P_name "p2p"
#define root_wlan_config_moduleConfig_P2P_mode_exists 1
#define root_wlan_config_moduleConfig_P2P_mode 0

// hcs node macros: root/wlan_config/moduleConfig/Mac80211
#define root_wlan_config_moduleConfig_Mac80211_exists 1
#define root_wlan_config_moduleConfig_Mac80211_mode_exists 1
#define root_wlan_config_moduleConfig_Mac80211_mode 1

// hcs node macros: root/wlan_config/moduleConfig/Phy
#define root_wlan_config_moduleConfig_Phy_exists 1
#define root_wlan_config_moduleConfig_Phy_mode_exists 1
#define root_wlan_config_moduleConfig_Phy_mode 1

// hcs node macros: root/wlan_config/deviceList
#define root_wlan_config_deviceList_exists 1
#define root_wlan_config_deviceList_foreach_child(func) \
    func(root_wlan_config_deviceList_device0)

#define root_wlan_config_deviceList_foreach_child_vargs(func, ...) \
    func(root_wlan_config_deviceList_device0, __VA_ARGS__)


// hcs node macros: root/wlan_config/deviceList/device0
#define root_wlan_config_deviceList_device0_exists 1
#define root_wlan_config_deviceList_device0_foreach_child(func) \
    func(root_wlan_config_deviceList_device0_powers) \
    func(root_wlan_config_deviceList_device0_reset) \
    func(root_wlan_config_deviceList_device0_bus)

#define root_wlan_config_deviceList_device0_foreach_child_vargs(func, ...) \
    func(root_wlan_config_deviceList_device0_powers, __VA_ARGS__) \
    func(root_wlan_config_deviceList_device0_reset, __VA_ARGS__) \
    func(root_wlan_config_deviceList_device0_bus, __VA_ARGS__)

#define root_wlan_config_deviceList_device0_deviceInstId_exists 1
#define root_wlan_config_deviceList_device0_deviceInstId 0

// hcs node macros: root/wlan_config/deviceList/device0/powers
#define root_wlan_config_deviceList_device0_powers_exists 1
#define root_wlan_config_deviceList_device0_powers_foreach_child(func) \
    func(root_wlan_config_deviceList_device0_powers_power0) \
    func(root_wlan_config_deviceList_device0_powers_power1)

#define root_wlan_config_deviceList_device0_powers_foreach_child_vargs(func, ...) \
    func(root_wlan_config_deviceList_device0_powers_power0, __VA_ARGS__) \
    func(root_wlan_config_deviceList_device0_powers_power1, __VA_ARGS__)


// hcs node macros: root/wlan_config/deviceList/device0/powers/power0
#define root_wlan_config_deviceList_device0_powers_power0_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_powerSeqDelay_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_powerSeqDelay 0
#define root_wlan_config_deviceList_device0_powers_power0_powerType_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_powerType 1
#define root_wlan_config_deviceList_device0_powers_power0_gpioId_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_gpioId 91
#define root_wlan_config_deviceList_device0_powers_power0_activeLevel_exists 1
#define root_wlan_config_deviceList_device0_powers_power0_activeLevel 1

// hcs node macros: root/wlan_config/deviceList/device0/powers/power1
#define root_wlan_config_deviceList_device0_powers_power1_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_powerSeqDelay_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_powerSeqDelay 0
#define root_wlan_config_deviceList_device0_powers_power1_powerType_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_powerType 0
#define root_wlan_config_deviceList_device0_powers_power1_gpioId_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_gpioId 1
#define root_wlan_config_deviceList_device0_powers_power1_activeLevel_exists 1
#define root_wlan_config_deviceList_device0_powers_power1_activeLevel 1

// hcs node macros: root/wlan_config/deviceList/device0/reset
#define root_wlan_config_deviceList_device0_reset_exists 1
#define root_wlan_config_deviceList_device0_reset_resetType_exists 1
#define root_wlan_config_deviceList_device0_reset_resetType 1
#define root_wlan_config_deviceList_device0_reset_gpioId_exists 1
#define root_wlan_config_deviceList_device0_reset_gpioId 90
#define root_wlan_config_deviceList_device0_reset_activeLevel_exists 1
#define root_wlan_config_deviceList_device0_reset_activeLevel 0
#define root_wlan_config_deviceList_device0_reset_resetHoldTime_exists 1
#define root_wlan_config_deviceList_device0_reset_resetHoldTime 16
#define root_wlan_config_deviceList_device0_bootUpTimeOut_exists 1
#define root_wlan_config_deviceList_device0_bootUpTimeOut 32

// hcs node macros: root/wlan_config/deviceList/device0/bus
#define root_wlan_config_deviceList_device0_bus_exists 1
#define root_wlan_config_deviceList_device0_bus_busType_exists 1
#define root_wlan_config_deviceList_device0_bus_busType 0
#define root_wlan_config_deviceList_device0_bus_busIdx_exists 1
#define root_wlan_config_deviceList_device0_bus_busIdx 2
#define root_wlan_config_deviceList_device0_bus_funcNum_exists 1
#define root_wlan_config_deviceList_device0_bus_funcNum_array_size 1
#define root_wlan_config_deviceList_device0_bus_funcNum_data { \
    1 \
}
#define root_wlan_config_deviceList_device0_bus_timeout_exists 1
#define root_wlan_config_deviceList_device0_bus_timeout 1000
#define root_wlan_config_deviceList_device0_bus_blockSize_exists 1
#define root_wlan_config_deviceList_device0_bus_blockSize 512

// hcs node macros: root/wlan_config/chipList
#define root_wlan_config_chipList_exists 1
#define root_wlan_config_chipList_foreach_child(func) \
    func(root_wlan_config_chipList_chipHi3881)

#define root_wlan_config_chipList_foreach_child_vargs(func, ...) \
    func(root_wlan_config_chipList_chipHi3881, __VA_ARGS__)


// hcs node macros: root/wlan_config/chipList/chipHi3881
#define root_wlan_config_chipList_chipHi3881_exists 1
#define root_wlan_config_chipList_chipHi3881_foreach_child(func) \
    func(root_wlan_config_chipList_chipHi3881_sdio)

#define root_wlan_config_chipList_chipHi3881_foreach_child_vargs(func, ...) \
    func(root_wlan_config_chipList_chipHi3881_sdio, __VA_ARGS__)

#define root_wlan_config_chipList_chipHi3881_match_attr_exists 1
#define root_wlan_config_chipList_chipHi3881_match_attr "hdf_wlan_chips_hi3881"
#define root_wlan_config_chipList_chipHi3881_driverName_exists 1
#define root_wlan_config_chipList_chipHi3881_driverName "hisi"

// hcs node macros: root/wlan_config/chipList/chipHi3881/sdio
#define root_wlan_config_chipList_chipHi3881_sdio_exists 1
#define root_wlan_config_chipList_chipHi3881_sdio_vendorId_exists 1
#define root_wlan_config_chipList_chipHi3881_sdio_vendorId 662
#define root_wlan_config_chipList_chipHi3881_sdio_deviceId_exists 1
#define root_wlan_config_chipList_chipHi3881_sdio_deviceId_array_size 1
#define root_wlan_config_chipList_chipHi3881_sdio_deviceId_data { \
    21319 \
}

// hcs node macros: root/accel_bmi160_chip_config
#define root_accel_bmi160_chip_config_exists 1
#define root_accel_bmi160_chip_config_foreach_child(func) \
    func(root_accel_bmi160_chip_config_sensorInfo) \
    func(root_accel_bmi160_chip_config_sensorBusConfig) \
    func(root_accel_bmi160_chip_config_sensorIdAttr) \
    func(root_accel_bmi160_chip_config_sensorDirection) \
    func(root_accel_bmi160_chip_config_sensorRegConfig)

#define root_accel_bmi160_chip_config_foreach_child_vargs(func, ...) \
    func(root_accel_bmi160_chip_config_sensorInfo, __VA_ARGS__) \
    func(root_accel_bmi160_chip_config_sensorBusConfig, __VA_ARGS__) \
    func(root_accel_bmi160_chip_config_sensorIdAttr, __VA_ARGS__) \
    func(root_accel_bmi160_chip_config_sensorDirection, __VA_ARGS__) \
    func(root_accel_bmi160_chip_config_sensorRegConfig, __VA_ARGS__)

#define root_accel_bmi160_chip_config_match_attr_exists 1
#define root_accel_bmi160_chip_config_match_attr "hdf_sensor_accel_bmi160_driver"

// hcs node macros: root/accel_bmi160_chip_config/sensorInfo
#define root_accel_bmi160_chip_config_sensorInfo_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorName_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorName "accelerometer"
#define root_accel_bmi160_chip_config_sensorInfo_vendorName_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_vendorName "borsh_bmi160"
#define root_accel_bmi160_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorTypeId 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorId_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_sensorId 1
#define root_accel_bmi160_chip_config_sensorInfo_power_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_power 850
#define root_accel_bmi160_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_accel_bmi160_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_accel_bmi160_chip_config_sensorInfo_maxRange_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_maxRange 8
#define root_accel_bmi160_chip_config_sensorInfo_accuracy_exists 1
#define root_accel_bmi160_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/accel_bmi160_chip_config/sensorBusConfig
#define root_accel_bmi160_chip_config_sensorBusConfig_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_busType_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_busType 0
#define root_accel_bmi160_chip_config_sensorBusConfig_busNum_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_busNum 6
#define root_accel_bmi160_chip_config_sensorBusConfig_busAddr_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_busAddr 104
#define root_accel_bmi160_chip_config_sensorBusConfig_regWidth_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_regWidth 1
#define root_accel_bmi160_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_accel_bmi160_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/accel_bmi160_chip_config/sensorIdAttr
#define root_accel_bmi160_chip_config_sensorIdAttr_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_chipName_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_chipName "bmi160"
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdRegister 0
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_accel_bmi160_chip_config_sensorIdAttr_chipIdValue 209

// hcs node macros: root/accel_bmi160_chip_config/sensorDirection
#define root_accel_bmi160_chip_config_sensorDirection_exists 1
#define root_accel_bmi160_chip_config_sensorDirection_direction_exists 1
#define root_accel_bmi160_chip_config_sensorDirection_direction 5
#define root_accel_bmi160_chip_config_sensorDirection_convert_exists 1
#define root_accel_bmi160_chip_config_sensorDirection_convert_array_size 48
#define root_accel_bmi160_chip_config_sensorDirection_convert_data { \
    0, 0, 0, 0, 1, 2, 1, 0, \
    0, 1, 0, 2, 0, 0, 1, 0, \
    1, 2, 0, 1, 0, 1, 0, 2, \
    1, 0, 1, 0, 1, 2, 0, 0, \
    1, 1, 0, 2, 0, 1, 1, 0, \
    1, 2, 1, 1, 1, 1, 0, 2 \
}

// hcs node macros: root/accel_bmi160_chip_config/sensorRegConfig
#define root_accel_bmi160_chip_config_sensorRegConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_initSeqConfig_array_size 20
#define root_accel_bmi160_chip_config_sensorRegConfig_initSeqConfig_data { \
    126, 182, 255, 1, 5, 2, 0, 0, \
    0, 0, 126, 16, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_accel_bmi160_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_enableSeqConfig_array_size 30
#define root_accel_bmi160_chip_config_sensorRegConfig_enableSeqConfig_data { \
    126, 17, 255, 1, 5, 2, 0, 0, \
    0, 0, 65, 3, 255, 1, 0, 2, \
    0, 0, 0, 0, 64, 8, 255, 1, \
    0, 2, 0, 0, 0, 0 \
}
#define root_accel_bmi160_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_accel_bmi160_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_accel_bmi160_chip_config_sensorRegConfig_disableSeqConfig_data { \
    126, 16, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/sensorConfig
#define root_sensorConfig_exists 1

// hcs node macros: root/als_bh1745_chip_config
#define root_als_bh1745_chip_config_exists 1
#define root_als_bh1745_chip_config_foreach_child(func) \
    func(root_als_bh1745_chip_config_sensorInfo) \
    func(root_als_bh1745_chip_config_sensorBusConfig) \
    func(root_als_bh1745_chip_config_sensorIdAttr) \
    func(root_als_bh1745_chip_config_sensorRegConfig) \
    func(root_als_bh1745_chip_config_extendAlsRegConfig)

#define root_als_bh1745_chip_config_foreach_child_vargs(func, ...) \
    func(root_als_bh1745_chip_config_sensorInfo, __VA_ARGS__) \
    func(root_als_bh1745_chip_config_sensorBusConfig, __VA_ARGS__) \
    func(root_als_bh1745_chip_config_sensorIdAttr, __VA_ARGS__) \
    func(root_als_bh1745_chip_config_sensorRegConfig, __VA_ARGS__) \
    func(root_als_bh1745_chip_config_extendAlsRegConfig, __VA_ARGS__)

#define root_als_bh1745_chip_config_match_attr_exists 1
#define root_als_bh1745_chip_config_match_attr "hdf_sensor_als_bh1745_driver"

// hcs node macros: root/als_bh1745_chip_config/sensorInfo
#define root_als_bh1745_chip_config_sensorInfo_exists 1
#define root_als_bh1745_chip_config_sensorInfo_sensorName_exists 1
#define root_als_bh1745_chip_config_sensorInfo_sensorName "als"
#define root_als_bh1745_chip_config_sensorInfo_vendorName_exists 1
#define root_als_bh1745_chip_config_sensorInfo_vendorName "rohm_bh1745"
#define root_als_bh1745_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_als_bh1745_chip_config_sensorInfo_sensorTypeId 5
#define root_als_bh1745_chip_config_sensorInfo_sensorId_exists 1
#define root_als_bh1745_chip_config_sensorInfo_sensorId 5
#define root_als_bh1745_chip_config_sensorInfo_power_exists 1
#define root_als_bh1745_chip_config_sensorInfo_power 230
#define root_als_bh1745_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_als_bh1745_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_als_bh1745_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_als_bh1745_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_als_bh1745_chip_config_sensorInfo_maxRange_exists 1
#define root_als_bh1745_chip_config_sensorInfo_maxRange 8
#define root_als_bh1745_chip_config_sensorInfo_accuracy_exists 1
#define root_als_bh1745_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/als_bh1745_chip_config/sensorBusConfig
#define root_als_bh1745_chip_config_sensorBusConfig_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_busType_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_busType 0
#define root_als_bh1745_chip_config_sensorBusConfig_busNum_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_busNum 6
#define root_als_bh1745_chip_config_sensorBusConfig_busAddr_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_busAddr 56
#define root_als_bh1745_chip_config_sensorBusConfig_regWidth_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_regWidth 1
#define root_als_bh1745_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_als_bh1745_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/als_bh1745_chip_config/sensorIdAttr
#define root_als_bh1745_chip_config_sensorIdAttr_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_chipName_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_chipName "bh1745"
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdRegister 146
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_als_bh1745_chip_config_sensorIdAttr_chipIdValue 224

// hcs node macros: root/als_bh1745_chip_config/sensorRegConfig
#define root_als_bh1745_chip_config_sensorRegConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_initSeqConfig_array_size 40
#define root_als_bh1745_chip_config_sensorRegConfig_initSeqConfig_data { \
    64, 192, 255, 1, 5, 2, 0, 0, \
    0, 0, 66, 144, 255, 1, 5, 2, \
    0, 0, 0, 0, 68, 2, 255, 1, \
    5, 2, 0, 0, 0, 0, 65, 1, \
    255, 1, 5, 2, 0, 0, 0, 0 \
}
#define root_als_bh1745_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_enableSeqConfig_array_size 20
#define root_als_bh1745_chip_config_sensorRegConfig_enableSeqConfig_data { \
    66, 16, 16, 1, 5, 4, 0, 0, \
    0, 0, 68, 2, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_als_bh1745_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_als_bh1745_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_als_bh1745_chip_config_sensorRegConfig_disableSeqConfig_data { \
    66, 0, 16, 1, 5, 4, 0, 0, \
    0, 0 \
}

// hcs node macros: root/als_bh1745_chip_config/extendAlsRegConfig
#define root_als_bh1745_chip_config_extendAlsRegConfig_exists 1
#define root_als_bh1745_chip_config_extendAlsRegConfig_time_exists 1
#define root_als_bh1745_chip_config_extendAlsRegConfig_time_array_size 60
#define root_als_bh1745_chip_config_extendAlsRegConfig_time_data { \
    65, 0, 7, 1, 5, 0, 1, 0, \
    0, 0, 65, 1, 7, 1, 5, 0, \
    1, 0, 0, 0, 65, 2, 7, 1, \
    5, 0, 1, 0, 0, 0, 65, 3, \
    7, 1, 5, 0, 1, 0, 0, 0, \
    65, 4, 7, 1, 5, 0, 1, 0, \
    0, 0, 65, 5, 7, 1, 5, 0, \
    1, 0, 0, 0 \
}
#define root_als_bh1745_chip_config_extendAlsRegConfig_gain_exists 1
#define root_als_bh1745_chip_config_extendAlsRegConfig_gain_array_size 30
#define root_als_bh1745_chip_config_extendAlsRegConfig_gain_data { \
    66, 0, 3, 1, 5, 0, 1, 0, \
    0, 0, 66, 1, 3, 1, 5, 0, \
    1, 0, 0, 0, 66, 2, 3, 1, \
    5, 0, 1, 0, 0, 0 \
}

// hcs node macros: root/gyro_bmi160_chip_config
#define root_gyro_bmi160_chip_config_exists 1
#define root_gyro_bmi160_chip_config_foreach_child(func) \
    func(root_gyro_bmi160_chip_config_sensorInfo) \
    func(root_gyro_bmi160_chip_config_sensorBusConfig) \
    func(root_gyro_bmi160_chip_config_sensorIdAttr) \
    func(root_gyro_bmi160_chip_config_sensorDirection) \
    func(root_gyro_bmi160_chip_config_sensorRegConfig)

#define root_gyro_bmi160_chip_config_foreach_child_vargs(func, ...) \
    func(root_gyro_bmi160_chip_config_sensorInfo, __VA_ARGS__) \
    func(root_gyro_bmi160_chip_config_sensorBusConfig, __VA_ARGS__) \
    func(root_gyro_bmi160_chip_config_sensorIdAttr, __VA_ARGS__) \
    func(root_gyro_bmi160_chip_config_sensorDirection, __VA_ARGS__) \
    func(root_gyro_bmi160_chip_config_sensorRegConfig, __VA_ARGS__)

#define root_gyro_bmi160_chip_config_match_attr_exists 1
#define root_gyro_bmi160_chip_config_match_attr "hdf_sensor_gyro_bmi160_driver"

// hcs node macros: root/gyro_bmi160_chip_config/sensorInfo
#define root_gyro_bmi160_chip_config_sensorInfo_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_sensorName_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_sensorName "gyroscope"
#define root_gyro_bmi160_chip_config_sensorInfo_vendorName_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_vendorName "borsh_bmi160"
#define root_gyro_bmi160_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_sensorTypeId 2
#define root_gyro_bmi160_chip_config_sensorInfo_sensorId_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_sensorId 2
#define root_gyro_bmi160_chip_config_sensorInfo_power_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_power 180
#define root_gyro_bmi160_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_gyro_bmi160_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_gyro_bmi160_chip_config_sensorInfo_maxRange_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_maxRange 8
#define root_gyro_bmi160_chip_config_sensorInfo_accuracy_exists 1
#define root_gyro_bmi160_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/gyro_bmi160_chip_config/sensorBusConfig
#define root_gyro_bmi160_chip_config_sensorBusConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_busType_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_busType 0
#define root_gyro_bmi160_chip_config_sensorBusConfig_busNum_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_busNum 6
#define root_gyro_bmi160_chip_config_sensorBusConfig_busAddr_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_busAddr 104
#define root_gyro_bmi160_chip_config_sensorBusConfig_regWidth_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_regWidth 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_gyro_bmi160_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/gyro_bmi160_chip_config/sensorIdAttr
#define root_gyro_bmi160_chip_config_sensorIdAttr_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipName_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipName "bmi160"
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdRegister 0
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_gyro_bmi160_chip_config_sensorIdAttr_chipIdValue 209

// hcs node macros: root/gyro_bmi160_chip_config/sensorDirection
#define root_gyro_bmi160_chip_config_sensorDirection_exists 1
#define root_gyro_bmi160_chip_config_sensorDirection_direction_exists 1
#define root_gyro_bmi160_chip_config_sensorDirection_direction 5
#define root_gyro_bmi160_chip_config_sensorDirection_convert_exists 1
#define root_gyro_bmi160_chip_config_sensorDirection_convert_array_size 48
#define root_gyro_bmi160_chip_config_sensorDirection_convert_data { \
    0, 0, 0, 0, 1, 2, 1, 0, \
    0, 1, 0, 2, 0, 0, 1, 0, \
    1, 2, 0, 1, 0, 1, 0, 2, \
    1, 0, 1, 0, 1, 2, 0, 0, \
    1, 1, 0, 2, 0, 1, 1, 0, \
    1, 2, 1, 1, 1, 1, 0, 2 \
}

// hcs node macros: root/gyro_bmi160_chip_config/sensorRegConfig
#define root_gyro_bmi160_chip_config_sensorRegConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_initSeqConfig_array_size 20
#define root_gyro_bmi160_chip_config_sensorRegConfig_initSeqConfig_data { \
    126, 182, 255, 1, 55, 2, 0, 0, \
    0, 0, 126, 20, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_gyro_bmi160_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_enableSeqConfig_array_size 30
#define root_gyro_bmi160_chip_config_sensorRegConfig_enableSeqConfig_data { \
    126, 21, 255, 1, 55, 2, 0, 0, \
    0, 0, 67, 0, 255, 1, 0, 4, \
    0, 0, 0, 0, 66, 8, 255, 1, \
    0, 2, 0, 0, 0, 0 \
}
#define root_gyro_bmi160_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_gyro_bmi160_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_gyro_bmi160_chip_config_sensorRegConfig_disableSeqConfig_data { \
    126, 20, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/barometer_bmp180_chip_config
#define root_barometer_bmp180_chip_config_exists 1
#define root_barometer_bmp180_chip_config_foreach_child(func) \
    func(root_barometer_bmp180_chip_config_sensorInfo) \
    func(root_barometer_bmp180_chip_config_sensorBusConfig) \
    func(root_barometer_bmp180_chip_config_sensorIdAttr) \
    func(root_barometer_bmp180_chip_config_sensorRegConfig)

#define root_barometer_bmp180_chip_config_foreach_child_vargs(func, ...) \
    func(root_barometer_bmp180_chip_config_sensorInfo, __VA_ARGS__) \
    func(root_barometer_bmp180_chip_config_sensorBusConfig, __VA_ARGS__) \
    func(root_barometer_bmp180_chip_config_sensorIdAttr, __VA_ARGS__) \
    func(root_barometer_bmp180_chip_config_sensorRegConfig, __VA_ARGS__)

#define root_barometer_bmp180_chip_config_match_attr_exists 1
#define root_barometer_bmp180_chip_config_match_attr "hdf_sensor_barometer_bmp180_driver"

// hcs node macros: root/barometer_bmp180_chip_config/sensorInfo
#define root_barometer_bmp180_chip_config_sensorInfo_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_sensorName_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_sensorName "barometer"
#define root_barometer_bmp180_chip_config_sensorInfo_vendorName_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_vendorName "borsh_bmp180"
#define root_barometer_bmp180_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_sensorTypeId 8
#define root_barometer_bmp180_chip_config_sensorInfo_sensorId_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_sensorId 8
#define root_barometer_bmp180_chip_config_sensorInfo_power_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_power 650
#define root_barometer_bmp180_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_barometer_bmp180_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_barometer_bmp180_chip_config_sensorInfo_maxRange_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_maxRange 8
#define root_barometer_bmp180_chip_config_sensorInfo_accuracy_exists 1
#define root_barometer_bmp180_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/barometer_bmp180_chip_config/sensorBusConfig
#define root_barometer_bmp180_chip_config_sensorBusConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_busType_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_busType 0
#define root_barometer_bmp180_chip_config_sensorBusConfig_busNum_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_busNum 6
#define root_barometer_bmp180_chip_config_sensorBusConfig_busAddr_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_busAddr 119
#define root_barometer_bmp180_chip_config_sensorBusConfig_regWidth_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_regWidth 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_barometer_bmp180_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/barometer_bmp180_chip_config/sensorIdAttr
#define root_barometer_bmp180_chip_config_sensorIdAttr_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipName_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipName "bmp180"
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdRegister 208
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_barometer_bmp180_chip_config_sensorIdAttr_chipIdValue 85

// hcs node macros: root/barometer_bmp180_chip_config/sensorRegConfig
#define root_barometer_bmp180_chip_config_sensorRegConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_initSeqConfig_array_size 10
#define root_barometer_bmp180_chip_config_sensorRegConfig_initSeqConfig_data { \
    224, 182, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}
#define root_barometer_bmp180_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_enableSeqConfig_array_size 0
#define root_barometer_bmp180_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_barometer_bmp180_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_barometer_bmp180_chip_config_sensorRegConfig_disableSeqConfig_data { \
    224, 182, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/hall_ak8789_chip_config
#define root_hall_ak8789_chip_config_exists 1
#define root_hall_ak8789_chip_config_foreach_child(func) \
    func(root_hall_ak8789_chip_config_sensorInfo) \
    func(root_hall_ak8789_chip_config_sensorBusConfig) \
    func(root_hall_ak8789_chip_config_sensorIdAttr) \
    func(root_hall_ak8789_chip_config_hallDelayedConfig)

#define root_hall_ak8789_chip_config_foreach_child_vargs(func, ...) \
    func(root_hall_ak8789_chip_config_sensorInfo, __VA_ARGS__) \
    func(root_hall_ak8789_chip_config_sensorBusConfig, __VA_ARGS__) \
    func(root_hall_ak8789_chip_config_sensorIdAttr, __VA_ARGS__) \
    func(root_hall_ak8789_chip_config_hallDelayedConfig, __VA_ARGS__)

#define root_hall_ak8789_chip_config_match_attr_exists 1
#define root_hall_ak8789_chip_config_match_attr "hdf_sensor_hall_ak8789_driver"

// hcs node macros: root/hall_ak8789_chip_config/sensorInfo
#define root_hall_ak8789_chip_config_sensorInfo_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_sensorName_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_sensorName "hallrometer"
#define root_hall_ak8789_chip_config_sensorInfo_vendorName_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_vendorName "akm_ak8789"
#define root_hall_ak8789_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_sensorTypeId 10
#define root_hall_ak8789_chip_config_sensorInfo_sensorId_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_sensorId 10
#define root_hall_ak8789_chip_config_sensorInfo_power_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_power 9
#define root_hall_ak8789_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_hall_ak8789_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_hall_ak8789_chip_config_sensorInfo_maxRange_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_maxRange 8
#define root_hall_ak8789_chip_config_sensorInfo_accuracy_exists 1
#define root_hall_ak8789_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/hall_ak8789_chip_config/sensorBusConfig
#define root_hall_ak8789_chip_config_sensorBusConfig_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_busType_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_busType 2
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq1_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq1 24
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq2_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_gpioIrq2 25
#define root_hall_ak8789_chip_config_sensorBusConfig_busNum_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_busNum 6
#define root_hall_ak8789_chip_config_sensorBusConfig_busAddr_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_busAddr 0
#define root_hall_ak8789_chip_config_sensorBusConfig_regWidth_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_regWidth 1
#define root_hall_ak8789_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_hall_ak8789_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/hall_ak8789_chip_config/sensorIdAttr
#define root_hall_ak8789_chip_config_sensorIdAttr_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_chipName_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_chipName "ak8789"
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdRegister 0
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_hall_ak8789_chip_config_sensorIdAttr_chipIdValue 0

// hcs node macros: root/hall_ak8789_chip_config/hallDelayedConfig
#define root_hall_ak8789_chip_config_hallDelayedConfig_exists 1
#define root_hall_ak8789_chip_config_hallDelayedConfig_delayedTime_exists 1
#define root_hall_ak8789_chip_config_hallDelayedConfig_delayedTime 100

// hcs node macros: root/magnetic_lsm303_chip_config
#define root_magnetic_lsm303_chip_config_exists 1
#define root_magnetic_lsm303_chip_config_foreach_child(func) \
    func(root_magnetic_lsm303_chip_config_sensorInfo) \
    func(root_magnetic_lsm303_chip_config_sensorBusConfig) \
    func(root_magnetic_lsm303_chip_config_sensorIdAttr) \
    func(root_magnetic_lsm303_chip_config_sensorDirection) \
    func(root_magnetic_lsm303_chip_config_sensorRegConfig)

#define root_magnetic_lsm303_chip_config_foreach_child_vargs(func, ...) \
    func(root_magnetic_lsm303_chip_config_sensorInfo, __VA_ARGS__) \
    func(root_magnetic_lsm303_chip_config_sensorBusConfig, __VA_ARGS__) \
    func(root_magnetic_lsm303_chip_config_sensorIdAttr, __VA_ARGS__) \
    func(root_magnetic_lsm303_chip_config_sensorDirection, __VA_ARGS__) \
    func(root_magnetic_lsm303_chip_config_sensorRegConfig, __VA_ARGS__)

#define root_magnetic_lsm303_chip_config_match_attr_exists 1
#define root_magnetic_lsm303_chip_config_match_attr "hdf_sensor_magnetic_lsm303_driver"

// hcs node macros: root/magnetic_lsm303_chip_config/sensorInfo
#define root_magnetic_lsm303_chip_config_sensorInfo_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorName_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorName "magnetometer"
#define root_magnetic_lsm303_chip_config_sensorInfo_vendorName_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_vendorName "st_lsm303"
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorTypeId 6
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorId_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_sensorId 6
#define root_magnetic_lsm303_chip_config_sensorInfo_power_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_power 110
#define root_magnetic_lsm303_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_magnetic_lsm303_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_magnetic_lsm303_chip_config_sensorInfo_maxRange_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_maxRange 8
#define root_magnetic_lsm303_chip_config_sensorInfo_accuracy_exists 1
#define root_magnetic_lsm303_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/magnetic_lsm303_chip_config/sensorBusConfig
#define root_magnetic_lsm303_chip_config_sensorBusConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busType_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busType 0
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busNum_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busNum 6
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busAddr_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_busAddr 30
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regWidth_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regWidth 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_magnetic_lsm303_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/magnetic_lsm303_chip_config/sensorIdAttr
#define root_magnetic_lsm303_chip_config_sensorIdAttr_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipName_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipName "lsm303"
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdRegister 10
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_magnetic_lsm303_chip_config_sensorIdAttr_chipIdValue 72

// hcs node macros: root/magnetic_lsm303_chip_config/sensorDirection
#define root_magnetic_lsm303_chip_config_sensorDirection_exists 1
#define root_magnetic_lsm303_chip_config_sensorDirection_direction_exists 1
#define root_magnetic_lsm303_chip_config_sensorDirection_direction 0
#define root_magnetic_lsm303_chip_config_sensorDirection_convert_exists 1
#define root_magnetic_lsm303_chip_config_sensorDirection_convert_array_size 48
#define root_magnetic_lsm303_chip_config_sensorDirection_convert_data { \
    0, 0, 0, 0, 1, 2, 1, 0, \
    0, 1, 0, 2, 0, 0, 1, 0, \
    1, 2, 0, 1, 0, 1, 0, 2, \
    1, 0, 1, 0, 1, 2, 0, 0, \
    1, 1, 0, 2, 0, 1, 1, 0, \
    1, 2, 1, 1, 1, 1, 0, 2 \
}

// hcs node macros: root/magnetic_lsm303_chip_config/sensorRegConfig
#define root_magnetic_lsm303_chip_config_sensorRegConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_initSeqConfig_array_size 30
#define root_magnetic_lsm303_chip_config_sensorRegConfig_initSeqConfig_data { \
    0, 20, 255, 1, 5, 2, 0, 0, \
    0, 0, 1, 160, 255, 1, 5, 2, \
    0, 0, 0, 0, 2, 0, 255, 1, \
    5, 2, 0, 0, 0, 0 \
}
#define root_magnetic_lsm303_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_enableSeqConfig_array_size 10
#define root_magnetic_lsm303_chip_config_sensorRegConfig_enableSeqConfig_data { \
    2, 0, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}
#define root_magnetic_lsm303_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_magnetic_lsm303_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_magnetic_lsm303_chip_config_sensorRegConfig_disableSeqConfig_data { \
    2, 3, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/proximity_apds9960_chip_config
#define root_proximity_apds9960_chip_config_exists 1
#define root_proximity_apds9960_chip_config_foreach_child(func) \
    func(root_proximity_apds9960_chip_config_sensorInfo) \
    func(root_proximity_apds9960_chip_config_sensorBusConfig) \
    func(root_proximity_apds9960_chip_config_sensorIdAttr) \
    func(root_proximity_apds9960_chip_config_sensorRegConfig)

#define root_proximity_apds9960_chip_config_foreach_child_vargs(func, ...) \
    func(root_proximity_apds9960_chip_config_sensorInfo, __VA_ARGS__) \
    func(root_proximity_apds9960_chip_config_sensorBusConfig, __VA_ARGS__) \
    func(root_proximity_apds9960_chip_config_sensorIdAttr, __VA_ARGS__) \
    func(root_proximity_apds9960_chip_config_sensorRegConfig, __VA_ARGS__)

#define root_proximity_apds9960_chip_config_match_attr_exists 1
#define root_proximity_apds9960_chip_config_match_attr "hdf_sensor_proximity_adps9960_driver"

// hcs node macros: root/proximity_apds9960_chip_config/sensorInfo
#define root_proximity_apds9960_chip_config_sensorInfo_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_sensorName_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_sensorName "proximity"
#define root_proximity_apds9960_chip_config_sensorInfo_vendorName_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_vendorName "adps9960"
#define root_proximity_apds9960_chip_config_sensorInfo_sensorTypeId_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_sensorTypeId 12
#define root_proximity_apds9960_chip_config_sensorInfo_sensorId_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_sensorId 12
#define root_proximity_apds9960_chip_config_sensorInfo_power_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_power 230
#define root_proximity_apds9960_chip_config_sensorInfo_firmwareVersion_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_firmwareVersion "1.0"
#define root_proximity_apds9960_chip_config_sensorInfo_hardwareVersion_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_hardwareVersion "1.0"
#define root_proximity_apds9960_chip_config_sensorInfo_maxRange_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_maxRange 8
#define root_proximity_apds9960_chip_config_sensorInfo_accuracy_exists 1
#define root_proximity_apds9960_chip_config_sensorInfo_accuracy 0

// hcs node macros: root/proximity_apds9960_chip_config/sensorBusConfig
#define root_proximity_apds9960_chip_config_sensorBusConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_busType_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_busType 0
#define root_proximity_apds9960_chip_config_sensorBusConfig_busNum_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_busNum 6
#define root_proximity_apds9960_chip_config_sensorBusConfig_busAddr_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_busAddr 57
#define root_proximity_apds9960_chip_config_sensorBusConfig_regWidth_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_regWidth 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_regBigEndian_exists 1
#define root_proximity_apds9960_chip_config_sensorBusConfig_regBigEndian 0

// hcs node macros: root/proximity_apds9960_chip_config/sensorIdAttr
#define root_proximity_apds9960_chip_config_sensorIdAttr_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipName_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipName "adps9960"
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdRegister_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdRegister 146
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdValue_exists 1
#define root_proximity_apds9960_chip_config_sensorIdAttr_chipIdValue 171

// hcs node macros: root/proximity_apds9960_chip_config/sensorRegConfig
#define root_proximity_apds9960_chip_config_sensorRegConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_initSeqConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_initSeqConfig_array_size 20
#define root_proximity_apds9960_chip_config_sensorRegConfig_initSeqConfig_data { \
    128, 5, 255, 1, 5, 2, 0, 0, \
    0, 0, 143, 0, 255, 1, 5, 2, \
    0, 0, 0, 0 \
}
#define root_proximity_apds9960_chip_config_sensorRegConfig_enableSeqConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_enableSeqConfig_array_size 10
#define root_proximity_apds9960_chip_config_sensorRegConfig_enableSeqConfig_data { \
    128, 5, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}
#define root_proximity_apds9960_chip_config_sensorRegConfig_disableSeqConfig_exists 1
#define root_proximity_apds9960_chip_config_sensorRegConfig_disableSeqConfig_array_size 10
#define root_proximity_apds9960_chip_config_sensorRegConfig_disableSeqConfig_data { \
    128, 0, 255, 1, 5, 2, 0, 0, \
    0, 0 \
}

// hcs node macros: root/gravity_vitural_config
#define root_gravity_vitural_config_exists 1
#define root_gravity_vitural_config_foreach_child(func) \
    func(root_gravity_vitural_config_sensorInfo)

#define root_gravity_vitural_config_foreach_child_vargs(func, ...) \
    func(root_gravity_vitural_config_sensorInfo, __VA_ARGS__)

#define root_gravity_vitural_config_match_attr_exists 1
#define root_gravity_vitural_config_match_attr "hdf_sensor_gravity_driver"

// hcs node macros: root/gravity_vitural_config/sensorInfo
#define root_gravity_vitural_config_sensorInfo_exists 1
#define root_gravity_vitural_config_sensorInfo_sensorName_exists 1
#define root_gravity_vitural_config_sensorInfo_sensorName "gravity"
#define root_gravity_vitural_config_sensorInfo_vendorName_exists 1
#define root_gravity_vitural_config_sensorInfo_vendorName "virtual chip"
#define root_gravity_vitural_config_sensorInfo_sensorTypeId_exists 1
#define root_gravity_vitural_config_sensorInfo_sensorTypeId 257
#define root_gravity_vitural_config_sensorInfo_sensorId_exists 1
#define root_gravity_vitural_config_sensorInfo_sensorId 257
#define root_gravity_vitural_config_sensorInfo_power_exists 1
#define root_gravity_vitural_config_sensorInfo_power 200
#define root_gravity_vitural_config_sensorInfo_firmwareVersion_exists 1
#define root_gravity_vitural_config_sensorInfo_firmwareVersion "1.0"
#define root_gravity_vitural_config_sensorInfo_hardwareVersion_exists 1
#define root_gravity_vitural_config_sensorInfo_hardwareVersion "1.0"
#define root_gravity_vitural_config_sensorInfo_maxRange_exists 1
#define root_gravity_vitural_config_sensorInfo_maxRange 8
#define root_gravity_vitural_config_sensorInfo_accuracy_exists 1
#define root_gravity_vitural_config_sensorInfo_accuracy 0

// hcs node macros: root/platfrom
#define root_platfrom_exists 1
#define root_platfrom_foreach_child(func) \
    func(root_platfrom_controller_0x120c1000) \
    func(root_platfrom_controller_0x120c1001) \
    func(root_platfrom_controller_0x120c1030) \
    func(root_platfrom_controller_0x120c1031) \
    func(root_platfrom_controller_0x120c1020) \
    func(root_platfrom_controller_0x120c1010) \
    func(root_platfrom_controller_0x120c1040) \
    func(root_platfrom_controller_0x120c1041)

#define root_platfrom_foreach_child_vargs(func, ...) \
    func(root_platfrom_controller_0x120c1000, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1001, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1030, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1031, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1020, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1010, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1040, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1041, __VA_ARGS__)


// hcs node macros: root/platfrom/controller_0x120c1000
#define root_platfrom_controller_0x120c1000_exists 1
#define root_platfrom_controller_0x120c1000_match_attr_exists 1
#define root_platfrom_controller_0x120c1000_match_attr "hdf_audio_driver_0"
#define root_platfrom_controller_0x120c1000_serviceName_exists 1
#define root_platfrom_controller_0x120c1000_serviceName "hdf_audio_codec_dev0"
#define root_platfrom_controller_0x120c1000_codecName_exists 1
#define root_platfrom_controller_0x120c1000_codecName "codec_service_0"
#define root_platfrom_controller_0x120c1000_platformName_exists 1
#define root_platfrom_controller_0x120c1000_platformName "dma_service_0"
#define root_platfrom_controller_0x120c1000_cpuDaiName_exists 1
#define root_platfrom_controller_0x120c1000_cpuDaiName "dai_service"
#define root_platfrom_controller_0x120c1000_codecDaiName_exists 1
#define root_platfrom_controller_0x120c1000_codecDaiName "codec_dai"
#define root_platfrom_controller_0x120c1000_dspName_exists 1
#define root_platfrom_controller_0x120c1000_dspName "dsp_service_0"
#define root_platfrom_controller_0x120c1000_dspDaiName_exists 1
#define root_platfrom_controller_0x120c1000_dspDaiName "dsp_dai"
#define root_platfrom_controller_0x120c1000_accessoryName_exists 1
#define root_platfrom_controller_0x120c1000_accessoryName ""
#define root_platfrom_controller_0x120c1000_accessoryDaiName_exists 1
#define root_platfrom_controller_0x120c1000_accessoryDaiName ""

// hcs node macros: root/platfrom/controller_0x120c1001
#define root_platfrom_controller_0x120c1001_exists 1
#define root_platfrom_controller_0x120c1001_match_attr_exists 1
#define root_platfrom_controller_0x120c1001_match_attr "hdf_audio_driver_1"
#define root_platfrom_controller_0x120c1001_serviceName_exists 1
#define root_platfrom_controller_0x120c1001_serviceName "hdf_audio_smartpa_dev0"
#define root_platfrom_controller_0x120c1001_accessoryName_exists 1
#define root_platfrom_controller_0x120c1001_accessoryName "codec_service_1"
#define root_platfrom_controller_0x120c1001_platformName_exists 1
#define root_platfrom_controller_0x120c1001_platformName "dma_service_0"
#define root_platfrom_controller_0x120c1001_cpuDaiName_exists 1
#define root_platfrom_controller_0x120c1001_cpuDaiName "dai_service"
#define root_platfrom_controller_0x120c1001_accessoryDaiName_exists 1
#define root_platfrom_controller_0x120c1001_accessoryDaiName "accessory_dai"
#define root_platfrom_controller_0x120c1001_dspName_exists 1
#define root_platfrom_controller_0x120c1001_dspName "dsp_service_0"
#define root_platfrom_controller_0x120c1001_dspDaiName_exists 1
#define root_platfrom_controller_0x120c1001_dspDaiName "dsp_dai"
#define root_platfrom_controller_0x120c1001_codecName_exists 1
#define root_platfrom_controller_0x120c1001_codecName ""
#define root_platfrom_controller_0x120c1001_codecDaiName_exists 1
#define root_platfrom_controller_0x120c1001_codecDaiName ""

// hcs node macros: root/platfrom/controller_0x120c1030
#define root_platfrom_controller_0x120c1030_exists 1
#define root_platfrom_controller_0x120c1030_foreach_child(func) \
    func(root_platfrom_controller_0x120c1030_idInfo) \
    func(root_platfrom_controller_0x120c1030_regConfig)

#define root_platfrom_controller_0x120c1030_foreach_child_vargs(func, ...) \
    func(root_platfrom_controller_0x120c1030_idInfo, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1030_regConfig, __VA_ARGS__)

#define root_platfrom_controller_0x120c1030_match_attr_exists 1
#define root_platfrom_controller_0x120c1030_match_attr "hdf_codec_driver"
#define root_platfrom_controller_0x120c1030_serviceName_exists 1
#define root_platfrom_controller_0x120c1030_serviceName "codec_service_0"
#define root_platfrom_controller_0x120c1030_codecDaiName_exists 1
#define root_platfrom_controller_0x120c1030_codecDaiName "codec_dai"

// hcs node macros: root/platfrom/controller_0x120c1030/idInfo
#define root_platfrom_controller_0x120c1030_idInfo_exists 1
#define root_platfrom_controller_0x120c1030_idInfo_chipName_exists 1
#define root_platfrom_controller_0x120c1030_idInfo_chipName "hi3516"
#define root_platfrom_controller_0x120c1030_idInfo_chipIdRegister_exists 1
#define root_platfrom_controller_0x120c1030_idInfo_chipIdRegister 289144832
#define root_platfrom_controller_0x120c1030_idInfo_chipIdSize_exists 1
#define root_platfrom_controller_0x120c1030_idInfo_chipIdSize 4096

// hcs node macros: root/platfrom/controller_0x120c1030/regConfig
#define root_platfrom_controller_0x120c1030_regConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_initSeqConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_initSeqConfig_array_size 22
#define root_platfrom_controller_0x120c1030_regConfig_initSeqConfig_data { \
    20, 67108866, 24, 4246732804, 28, 1572888, 32, 2206400552, \
    36, 23644, 40, 1245184, 48, 4278409728, 52, 134217729, \
    56, 101065764, 60, 505331713, 20, 67108866 \
}
#define root_platfrom_controller_0x120c1030_regConfig_ctrlParamsSeqConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_ctrlParamsSeqConfig_array_size 72
#define root_platfrom_controller_0x120c1030_regConfig_ctrlParamsSeqConfig_data { \
    8196, 8196, 8, 8, 40, 127, 127, 0, \
    0, 60, 60, 24, 24, 0, 87, 127, \
    1, 0, 56, 56, 31, 31, 0, 1, \
    1, 0, 0, 60, 60, 31, 31, 0, \
    1, 1, 0, 0, 32, 32, 16, 16, \
    0, 15, 31, 0, 0, 32, 32, 24, \
    24, 0, 15, 31, 0, 0, 8192, 8192, \
    16, 16, 0, 7, 7, 0, 0, 4096, \
    4096, 16, 16, 0, 7, 7, 0, 0 \
}
#define root_platfrom_controller_0x120c1030_regConfig_controlsConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_controlsConfig_array_size 24
#define root_platfrom_controller_0x120c1030_regConfig_controlsConfig_data { \
    0, 0, 0, 1, 1, 1, 2, 0, \
    1, 3, 1, 1, 4, 2, 1, 5, \
    2, 1, 8, 6, 0, 9, 6, 0 \
}
#define root_platfrom_controller_0x120c1030_regConfig_daiStartupSeqConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_daiStartupSeqConfig_array_size 9
#define root_platfrom_controller_0x120c1030_regConfig_daiStartupSeqConfig_data { \
    36, 36, 11, 11, 0, 1, 1, 0, \
    1 \
}
#define root_platfrom_controller_0x120c1030_regConfig_daiParamsSeqConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_daiParamsSeqConfig_array_size 27
#define root_platfrom_controller_0x120c1030_regConfig_daiParamsSeqConfig_data { \
    48, 48, 13, 13, 0, 31, 31, 0, \
    0, 28, 28, 6, 6, 0, 3, 3, \
    0, 0, 48, 48, 22, 22, 0, 3, \
    3, 0, 0 \
}
#define root_platfrom_controller_0x120c1030_regConfig_ctrlSapmParamsSeqConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_ctrlSapmParamsSeqConfig_array_size 36
#define root_platfrom_controller_0x120c1030_regConfig_ctrlSapmParamsSeqConfig_data { \
    32, 32, 23, 23, 0, 1, 1, 0, \
    0, 32, 32, 31, 31, 0, 1, 1, \
    0, 0, 48, 48, 27, 27, 0, 1, \
    1, 0, 0, 48, 48, 26, 26, 0, \
    1, 1, 0, 0 \
}
#define root_platfrom_controller_0x120c1030_regConfig_sapmComponent_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_sapmComponent_array_size 72
#define root_platfrom_controller_0x120c1030_regConfig_sapmComponent_data { \
    10, 0, 32, 1, 15, 1, 0, 0, \
    10, 1, 32, 1, 14, 1, 0, 0, \
    11, 2, 20, 1, 11, 1, 0, 0, \
    11, 3, 20, 1, 12, 1, 0, 0, \
    8, 4, 32, 1, 13, 1, 1, 1, \
    8, 5, 32, 1, 12, 1, 2, 1, \
    15, 6, 0, 1, 0, 0, 3, 1, \
    15, 7, 0, 1, 0, 0, 4, 1, \
    0, 8, 0, 1, 0, 0, 0, 0 \
}
#define root_platfrom_controller_0x120c1030_regConfig_sapmConfig_exists 1
#define root_platfrom_controller_0x120c1030_regConfig_sapmConfig_array_size 12
#define root_platfrom_controller_0x120c1030_regConfig_sapmConfig_data { \
    0, 5, 1, 1, 5, 1, 2, 0, \
    1, 3, 0, 1 \
}

// hcs node macros: root/platfrom/controller_0x120c1031
#define root_platfrom_controller_0x120c1031_exists 1
#define root_platfrom_controller_0x120c1031_foreach_child(func) \
    func(root_platfrom_controller_0x120c1031_regConfig)

#define root_platfrom_controller_0x120c1031_foreach_child_vargs(func, ...) \
    func(root_platfrom_controller_0x120c1031_regConfig, __VA_ARGS__)

#define root_platfrom_controller_0x120c1031_match_attr_exists 1
#define root_platfrom_controller_0x120c1031_match_attr "hdf_codec_driver_ex"
#define root_platfrom_controller_0x120c1031_serviceName_exists 1
#define root_platfrom_controller_0x120c1031_serviceName "codec_service_1"
#define root_platfrom_controller_0x120c1031_codecDaiName_exists 1
#define root_platfrom_controller_0x120c1031_codecDaiName "accessory_dai"

// hcs node macros: root/platfrom/controller_0x120c1031/regConfig
#define root_platfrom_controller_0x120c1031_regConfig_exists 1
#define root_platfrom_controller_0x120c1031_regConfig_resetSeqConfig_exists 1
#define root_platfrom_controller_0x120c1031_regConfig_resetSeqConfig_array_size 2
#define root_platfrom_controller_0x120c1031_regConfig_resetSeqConfig_data { \
    0, 0 \
}
#define root_platfrom_controller_0x120c1031_regConfig_initSeqConfig_exists 1
#define root_platfrom_controller_0x120c1031_regConfig_initSeqConfig_array_size 42
#define root_platfrom_controller_0x120c1031_regConfig_initSeqConfig_data { \
    0, 1, 1, 2584, 2, 7, 3, 2584, \
    4, 7, 5, 23005, 6, 50750, 7, 25882, \
    8, 58686, 9, 17942, 10, 54078, 11, 19955, \
    12, 59966, 13, 24288, 14, 63806, 15, 8, \
    16, 37562, 17, 4773, 18, 4, 19, 4145, \
    20, 0 \
}
#define root_platfrom_controller_0x120c1031_regConfig_ctrlParamsSeqConfig_exists 1
#define root_platfrom_controller_0x120c1031_regConfig_ctrlParamsSeqConfig_array_size 27
#define root_platfrom_controller_0x120c1031_regConfig_ctrlParamsSeqConfig_data { \
    19, 19, 0, 0, 0, 188, 255, 1, \
    0, 20, 20, 9, 9, 0, 1, 1, \
    0, 0, 1, 1, 10, 10, 0, 3, \
    3, 0, 0 \
}
#define root_platfrom_controller_0x120c1031_regConfig_daiStartupSeqConfig_exists 1
#define root_platfrom_controller_0x120c1031_regConfig_daiStartupSeqConfig_array_size 9
#define root_platfrom_controller_0x120c1031_regConfig_daiStartupSeqConfig_data { \
    0, 0, 0, 0, 0, 15, 15, 0, \
    9 \
}
#define root_platfrom_controller_0x120c1031_regConfig_daiParamsSeqConfig_exists 1
#define root_platfrom_controller_0x120c1031_regConfig_daiParamsSeqConfig_array_size 27
#define root_platfrom_controller_0x120c1031_regConfig_daiParamsSeqConfig_data { \
    1, 1, 6, 6, 0, 15, 15, 0, \
    0, 1, 1, 3, 3, 0, 7, 7, \
    0, 0, 1, 1, 10, 10, 0, 3, \
    3, 0, 0 \
}
#define root_platfrom_controller_0x120c1031_regConfig_controlsConfig_exists 1
#define root_platfrom_controller_0x120c1031_regConfig_controlsConfig_array_size 9
#define root_platfrom_controller_0x120c1031_regConfig_controlsConfig_data { \
    0, 0, 1, 2, 0, 1, 8, 6, \
    1 \
}

// hcs node macros: root/platfrom/controller_0x120c1020
#define root_platfrom_controller_0x120c1020_exists 1
#define root_platfrom_controller_0x120c1020_foreach_child(func) \
    func(root_platfrom_controller_0x120c1020_idInfo) \
    func(root_platfrom_controller_0x120c1020_regConfig)

#define root_platfrom_controller_0x120c1020_foreach_child_vargs(func, ...) \
    func(root_platfrom_controller_0x120c1020_idInfo, __VA_ARGS__) \
    func(root_platfrom_controller_0x120c1020_regConfig, __VA_ARGS__)

#define root_platfrom_controller_0x120c1020_match_attr_exists 1
#define root_platfrom_controller_0x120c1020_match_attr "hdf_dai_driver"
#define root_platfrom_controller_0x120c1020_serviceName_exists 1
#define root_platfrom_controller_0x120c1020_serviceName "dai_service"

// hcs node macros: root/platfrom/controller_0x120c1020/idInfo
#define root_platfrom_controller_0x120c1020_idInfo_exists 1
#define root_platfrom_controller_0x120c1020_idInfo_chipName_exists 1
#define root_platfrom_controller_0x120c1020_idInfo_chipName "hi3516"
#define root_platfrom_controller_0x120c1020_idInfo_chipIdRegister_exists 1
#define root_platfrom_controller_0x120c1020_idInfo_chipIdRegister 289079296
#define root_platfrom_controller_0x120c1020_idInfo_chipIdSize_exists 1
#define root_platfrom_controller_0x120c1020_idInfo_chipIdSize 65536

// hcs node macros: root/platfrom/controller_0x120c1020/regConfig
#define root_platfrom_controller_0x120c1020_regConfig_exists 1
#define root_platfrom_controller_0x120c1020_regConfig_ctrlParamsSeqConfig_exists 1
#define root_platfrom_controller_0x120c1020_regConfig_ctrlParamsSeqConfig_array_size 18
#define root_platfrom_controller_0x120c1020_regConfig_ctrlParamsSeqConfig_data { \
    72, 72, 0, 0, 0, 2, 3, 0, \
    0, 72, 72, 0, 0, 0, 2, 3, \
    0, 0 \
}
#define root_platfrom_controller_0x120c1020_regConfig_controlsConfig_exists 1
#define root_platfrom_controller_0x120c1020_regConfig_controlsConfig_array_size 6
#define root_platfrom_controller_0x120c1020_regConfig_controlsConfig_data { \
    6, 4, 0, 7, 4, 0 \
}
#define root_platfrom_controller_0x120c1020_regConfig_daiStartupSeqConfig_exists 1
#define root_platfrom_controller_0x120c1020_regConfig_daiStartupSeqConfig_array_size 36
#define root_platfrom_controller_0x120c1020_regConfig_daiStartupSeqConfig_data { \
    256, 256, 0, 0, 0, 4294967295, 4294967295, 0, \
    1388272, 260, 260, 0, 0, 0, 4294967295, 4294967295, \
    0, 49429, 320, 320, 0, 0, 0, 4294967295, \
    4294967295, 0, 1388272, 324, 324, 0, 0, 0, \
    4294967295, 4294967295, 0, 49429 \
}
#define root_platfrom_controller_0x120c1020_regConfig_daiParamsSeqConfig_exists 1
#define root_platfrom_controller_0x120c1020_regConfig_daiParamsSeqConfig_array_size 72
#define root_platfrom_controller_0x120c1020_regConfig_daiParamsSeqConfig_data { \
    320, 320, 0, 0, 0, 134217727, 134217727, 0, \
    0, 324, 324, 0, 0, 0, 127, 127, \
    0, 0, 8192, 8192, 2, 2, 0, 2, \
    3, 0, 0, 8192, 8192, 4, 4, 0, \
    1, 3, 0, 0, 256, 256, 0, 0, \
    0, 134217727, 134217727, 0, 0, 260, 260, 0, \
    0, 0, 127, 127, 0, 0, 4096, 4096, \
    2, 2, 0, 2, 3, 0, 0, 4096, \
    4096, 4, 4, 0, 1, 3, 0, 0 \
}

// hcs node macros: root/platfrom/controller_0x120c1010
#define root_platfrom_controller_0x120c1010_exists 1
#define root_platfrom_controller_0x120c1010_match_attr_exists 1
#define root_platfrom_controller_0x120c1010_match_attr "hdf_dma_driver"
#define root_platfrom_controller_0x120c1010_serviceName_exists 1
#define root_platfrom_controller_0x120c1010_serviceName "dma_service_0"

// hcs node macros: root/platfrom/controller_0x120c1040
#define root_platfrom_controller_0x120c1040_exists 1
#define root_platfrom_controller_0x120c1040_match_attr_exists 1
#define root_platfrom_controller_0x120c1040_match_attr "hdf_dsp_driver"
#define root_platfrom_controller_0x120c1040_serviceName_exists 1
#define root_platfrom_controller_0x120c1040_serviceName "dsp_service_0"
#define root_platfrom_controller_0x120c1040_dspDaiName_exists 1
#define root_platfrom_controller_0x120c1040_dspDaiName "dsp_dai"

// hcs node macros: root/platfrom/controller_0x120c1041
#define root_platfrom_controller_0x120c1041_exists 1
#define root_platfrom_controller_0x120c1041_match_attr_exists 1
#define root_platfrom_controller_0x120c1041_match_attr "hdf_dsp_driver"
#define root_platfrom_controller_0x120c1041_serviceName_exists 1
#define root_platfrom_controller_0x120c1041_serviceName "dsp_service_0"
#define root_platfrom_controller_0x120c1041_dspDaiName_exists 1
#define root_platfrom_controller_0x120c1041_dspDaiName "dsp_dai"

// hcs node macros: root/lightConfig
#define root_lightConfig_exists 1
#define root_lightConfig_foreach_child(func) \
    func(root_lightConfig_boardConfig)

#define root_lightConfig_foreach_child_vargs(func, ...) \
    func(root_lightConfig_boardConfig, __VA_ARGS__)


// hcs node macros: root/lightConfig/boardConfig
#define root_lightConfig_boardConfig_exists 1
#define root_lightConfig_boardConfig_foreach_child(func) \
    func(root_lightConfig_boardConfig_lightAttr)

#define root_lightConfig_boardConfig_foreach_child_vargs(func, ...) \
    func(root_lightConfig_boardConfig_lightAttr, __VA_ARGS__)

#define root_lightConfig_boardConfig_match_attr_exists 1
#define root_lightConfig_boardConfig_match_attr "hdf_light_driver"

// hcs node macros: root/lightConfig/boardConfig/lightAttr
#define root_lightConfig_boardConfig_lightAttr_exists 1
#define root_lightConfig_boardConfig_lightAttr_foreach_child(func) \
    func(root_lightConfig_boardConfig_lightAttr_light01)

#define root_lightConfig_boardConfig_lightAttr_foreach_child_vargs(func, ...) \
    func(root_lightConfig_boardConfig_lightAttr_light01, __VA_ARGS__)


// hcs node macros: root/lightConfig/boardConfig/lightAttr/light01
#define root_lightConfig_boardConfig_lightAttr_light01_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_lightType_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_lightType_array_size 2
#define root_lightConfig_boardConfig_lightAttr_light01_lightType_data { \
    1, 2 \
}
#define root_lightConfig_boardConfig_lightAttr_light01_busRNum_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_busRNum 31
#define root_lightConfig_boardConfig_lightAttr_light01_busGNum_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_busGNum 30
#define root_lightConfig_boardConfig_lightAttr_light01_busBNum_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_busBNum 29
#define root_lightConfig_boardConfig_lightAttr_light01_lightBrightness_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_lightBrightness 2147483648
#define root_lightConfig_boardConfig_lightAttr_light01_onTime_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_onTime 50
#define root_lightConfig_boardConfig_lightAttr_light01_offTime_exists 1
#define root_lightConfig_boardConfig_lightAttr_light01_offTime 50

// hcs node macros: root/vibratorConfig
#define root_vibratorConfig_exists 1
#define root_vibratorConfig_foreach_child(func) \
    func(root_vibratorConfig_boardConfig)

#define root_vibratorConfig_foreach_child_vargs(func, ...) \
    func(root_vibratorConfig_boardConfig, __VA_ARGS__)


// hcs node macros: root/vibratorConfig/boardConfig
#define root_vibratorConfig_boardConfig_exists 1
#define root_vibratorConfig_boardConfig_foreach_child(func) \
    func(root_vibratorConfig_boardConfig_vibratorAttr) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig)

#define root_vibratorConfig_boardConfig_foreach_child_vargs(func, ...) \
    func(root_vibratorConfig_boardConfig_vibratorAttr, __VA_ARGS__) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig, __VA_ARGS__)

#define root_vibratorConfig_boardConfig_match_attr_exists 1
#define root_vibratorConfig_boardConfig_match_attr "hdf_vibrator_driver"

// hcs node macros: root/vibratorConfig/boardConfig/vibratorAttr
#define root_vibratorConfig_boardConfig_vibratorAttr_exists 1
#define root_vibratorConfig_boardConfig_vibratorAttr_deviceType_exists 1
#define root_vibratorConfig_boardConfig_vibratorAttr_deviceType 1
#define root_vibratorConfig_boardConfig_vibratorAttr_supportPreset_exists 1
#define root_vibratorConfig_boardConfig_vibratorAttr_supportPreset 1

// hcs node macros: root/vibratorConfig/boardConfig/vibratorHapticConfig
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_foreach_child(func) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect)

#define root_vibratorConfig_boardConfig_vibratorHapticConfig_foreach_child_vargs(func, ...) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer, __VA_ARGS__) \
    func(root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect, __VA_ARGS__)


// hcs node macros: root/vibratorConfig/boardConfig/vibratorHapticConfig/haptic_clock_timer
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_effectName_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_effectName "haptic.clock.timer"
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_type_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_type 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_seq_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_seq_array_size 4
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_clock_timer_seq_data { \
    600, 600, 200, 600 \
}

// hcs node macros: root/vibratorConfig/boardConfig/vibratorHapticConfig/haptic_default_effect
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_effectName_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_effectName "haptic.default.effect"
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_type_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_type 0
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_seq_exists 1
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_seq_array_size 4
#define root_vibratorConfig_boardConfig_vibratorHapticConfig_haptic_default_effect_seq_data { \
    0, 3, 800, 1 \
}

// hcs node macros: root/linearVibratorConfig
#define root_linearVibratorConfig_exists 1
#define root_linearVibratorConfig_foreach_child(func) \
    func(root_linearVibratorConfig_boardConfig)

#define root_linearVibratorConfig_foreach_child_vargs(func, ...) \
    func(root_linearVibratorConfig_boardConfig, __VA_ARGS__)


// hcs node macros: root/linearVibratorConfig/boardConfig
#define root_linearVibratorConfig_boardConfig_exists 1
#define root_linearVibratorConfig_boardConfig_foreach_child(func) \
    func(root_linearVibratorConfig_boardConfig_vibratorChipConfig)

#define root_linearVibratorConfig_boardConfig_foreach_child_vargs(func, ...) \
    func(root_linearVibratorConfig_boardConfig_vibratorChipConfig, __VA_ARGS__)

#define root_linearVibratorConfig_boardConfig_match_attr_exists 1
#define root_linearVibratorConfig_boardConfig_match_attr "hdf_linear_vibrator_driver"

// hcs node macros: root/linearVibratorConfig/boardConfig/vibratorChipConfig
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_exists 1
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_busType_exists 1
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_busType 1
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_gpioNum_exists 1
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_gpioNum 31
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_startReg_exists 1
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_startReg 0
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_stopReg_exists 1
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_stopReg 0
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_startMask_exists 1
#define root_linearVibratorConfig_boardConfig_vibratorChipConfig_startMask 0

// hcs node macros: root/backlightConfig
#define root_backlightConfig_exists 1
#define root_backlightConfig_foreach_child(func) \
    func(root_backlightConfig_pwmBacklightConfig)

#define root_backlightConfig_foreach_child_vargs(func, ...) \
    func(root_backlightConfig_pwmBacklightConfig, __VA_ARGS__)


// hcs node macros: root/backlightConfig/pwmBacklightConfig
#define root_backlightConfig_pwmBacklightConfig_exists 1
#define root_backlightConfig_pwmBacklightConfig_match_attr_exists 1
#define root_backlightConfig_pwmBacklightConfig_match_attr "pwm_bl_dev"
#define root_backlightConfig_pwmBacklightConfig_pwmDevNum_exists 1
#define root_backlightConfig_pwmBacklightConfig_pwmDevNum 1
#define root_backlightConfig_pwmBacklightConfig_pwmMaxPeroid_exists 1
#define root_backlightConfig_pwmBacklightConfig_pwmMaxPeroid 100000
#define root_backlightConfig_pwmBacklightConfig_backlightDevName_exists 1
#define root_backlightConfig_pwmBacklightConfig_backlightDevName "hdf_pwm"
#define root_backlightConfig_pwmBacklightConfig_minBrightness_exists 1
#define root_backlightConfig_pwmBacklightConfig_minBrightness 0
#define root_backlightConfig_pwmBacklightConfig_defBrightness_exists 1
#define root_backlightConfig_pwmBacklightConfig_defBrightness 127
#define root_backlightConfig_pwmBacklightConfig_maxBrightness_exists 1
#define root_backlightConfig_pwmBacklightConfig_maxBrightness 255
#define root_match_attr_exists 1
#define root_match_attr "test,config"

// hcs node macros: root/audio_info
#define root_audio_info_exists 1
#define root_audio_info_match_attr_exists 1
#define root_audio_info_match_attr "hw,hw_audio_info"
#define root_audio_info_pa_identifier_exists 1
#define root_audio_info_pa_identifier "smartpakit"
#define root_audio_info_smartpa_addr_exists 1
#define root_audio_info_smartpa_addr 65536
#define root_audio_info_smartpa_num_exists 1
#define root_audio_info_smartpa_num 1
#define root_audio_info_builtin_primary_mic_exist_exists 1
#define root_audio_info_builtin_primary_mic_exist 1
#define root_audio_info_builtin_second_mic_exist_exists 1
#define root_audio_info_builtin_second_mic_exist 1
#define root_audio_info_voice_vol_level_exists 1
#define root_audio_info_voice_vol_level 256
#define root_audio_info_cust_name_exists 1
#define root_audio_info_cust_name "audio_custom_v2"
#define root_audio_info_dual_smartpa_delay_exists 1
#define root_audio_info_dual_smartpa_delay "true"
#define root_audio_info_read_u64data_exists 1
#define root_audio_info_read_u64data 4294967296
#define root_audio_info_status_exists 1
#define root_audio_info_status "ok"

// hcs node macros: root/fingerprint_info
#define root_fingerprint_info_exists 1
#define root_fingerprint_info_foreach_child(func) \
    func(root_fingerprint_info_fingerprint_one) \
    func(root_fingerprint_info_fingerprint_two)

#define root_fingerprint_info_foreach_child_vargs(func, ...) \
    func(root_fingerprint_info_fingerprint_one, __VA_ARGS__) \
    func(root_fingerprint_info_fingerprint_two, __VA_ARGS__)

#define root_fingerprint_info_match_attr_exists 1
#define root_fingerprint_info_match_attr "hw,hw_fingerprint_info"
#define root_fingerprint_info_read_u32_index_exists 1
#define root_fingerprint_info_read_u32_index_array_size 3
#define root_fingerprint_info_read_u32_index_data { \
    1, 256, 65536 \
}
#define root_fingerprint_info_finger_info_exists 1
#define root_fingerprint_info_finger_info root_fingerprint_info_fingerprint_one
#define root_fingerprint_info_audio_info_exists 1
#define root_fingerprint_info_audio_info root_audio_info

// hcs node macros: root/fingerprint_info/fingerprint_one
#define root_fingerprint_info_fingerprint_one_exists 1
#define root_fingerprint_info_fingerprint_one_foreach_child(func) \
    func(root_fingerprint_info_fingerprint_one_one_child)

#define root_fingerprint_info_fingerprint_one_foreach_child_vargs(func, ...) \
    func(root_fingerprint_info_fingerprint_one_one_child, __VA_ARGS__)

#define root_fingerprint_info_fingerprint_one_product_exists 1
#define root_fingerprint_info_fingerprint_one_product "test"
#define root_fingerprint_info_fingerprint_one_chip_exists 1
#define root_fingerprint_info_fingerprint_one_chip "one"

// hcs node macros: root/fingerprint_info/fingerprint_one/one_child
#define root_fingerprint_info_fingerprint_one_one_child_exists 1
#define root_fingerprint_info_fingerprint_one_one_child_status_exists 1
#define root_fingerprint_info_fingerprint_one_one_child_status "ok"
#define root_fingerprint_info_fingerprint_one_status_exists 1
#define root_fingerprint_info_fingerprint_one_status "ok"
#define root_fingerprint_info_dual_fingerprint_exists 1
#define root_fingerprint_info_dual_fingerprint "false"

// hcs node macros: root/fingerprint_info/fingerprint_two
#define root_fingerprint_info_fingerprint_two_exists 1
#define root_fingerprint_info_fingerprint_two_foreach_child(func) \
    func(root_fingerprint_info_fingerprint_two_one_child)

#define root_fingerprint_info_fingerprint_two_foreach_child_vargs(func, ...) \
    func(root_fingerprint_info_fingerprint_two_one_child, __VA_ARGS__)

#define root_fingerprint_info_fingerprint_two_chip_exists 1
#define root_fingerprint_info_fingerprint_two_chip "two"
#define root_fingerprint_info_fingerprint_two_status_exists 1
#define root_fingerprint_info_fingerprint_two_status "disable"
#define root_fingerprint_info_fingerprint_two_product_exists 1
#define root_fingerprint_info_fingerprint_two_product "test"

// hcs node macros: root/fingerprint_info/fingerprint_two/one_child
#define root_fingerprint_info_fingerprint_two_one_child_exists 1
#define root_fingerprint_info_fingerprint_two_one_child_status_exists 1
#define root_fingerprint_info_fingerprint_two_one_child_status "ok"
#define root_fingerprint_info_string_list_names_exists 1
#define root_fingerprint_info_string_list_names_array_size 3
#define root_fingerprint_info_string_list_names_data { \
    "first", "second", "third" \
}
#define root_fingerprint_info_status_exists 1
#define root_fingerprint_info_status "ok"

// hcs node macros: root/data_type_test
#define root_data_type_test_exists 1
#define root_data_type_test_match_attr_exists 1
#define root_data_type_test_match_attr "hw,data_type_test"
#define root_data_type_test_read_four_data_type_exists 1
#define root_data_type_test_read_four_data_type_array_size 4
#define root_data_type_test_read_four_data_type_data { \
    1, 256, 65536, 4294967296 \
}
#define root_data_type_test_test_u8_elem_data_exists 1
#define root_data_type_test_test_u8_elem_data_array_size 8
#define root_data_type_test_test_u8_elem_data_data { \
    0, 1, 2, 3, 4, 5, 6, 7 \
}
#define root_data_type_test_test_u16_elem_data_exists 1
#define root_data_type_test_test_u16_elem_data_array_size 8
#define root_data_type_test_test_u16_elem_data_data { \
    0, 1, 2, 3, 4, 5, 256, 257 \
}
#define root_board_id_exists 1
#define root_board_id_array_size 2
#define root_board_id_data { \
    65536, 256 \
}
#define root_support_Device_exists 1
#define root_support_Device "TP LCD Sensor"

// hcs node macros: root/fingerprint_three
#define root_fingerprint_three_exists 1
#define root_fingerprint_three_foreach_child(func) \
    func(root_fingerprint_three_one_child)

#define root_fingerprint_three_foreach_child_vargs(func, ...) \
    func(root_fingerprint_three_one_child, __VA_ARGS__)

#define root_fingerprint_three_chip_exists 1
#define root_fingerprint_three_chip "three"
#define root_fingerprint_three_status_exists 1
#define root_fingerprint_three_status "disable"
#define root_fingerprint_three_product_exists 1
#define root_fingerprint_three_product "test"

// hcs node macros: root/fingerprint_three/one_child
#define root_fingerprint_three_one_child_exists 1
#define root_fingerprint_three_one_child_status_exists 1
#define root_fingerprint_three_one_child_status "ok"
#define root_modem_id_exists 1
#define root_modem_id_array_size 2
#define root_modem_id_data { \
    4138991926, 0 \
}
#define root_boardId_exists 1
#define root_boardId 8000

// hcs node macros: root/gpio_config
#define root_gpio_config_exists 1
#define root_gpio_config_foreach_child(func) \
    func(root_gpio_config_gpio_controller_tester)

#define root_gpio_config_foreach_child_vargs(func, ...) \
    func(root_gpio_config_gpio_controller_tester, __VA_ARGS__)


// hcs node macros: root/gpio_config/gpio_controller_tester
#define root_gpio_config_gpio_controller_tester_exists 1
#define root_gpio_config_gpio_controller_tester_match_attr_exists 1
#define root_gpio_config_gpio_controller_tester_match_attr "GPIO_TEST"
#define root_gpio_config_gpio_controller_tester_gpio_exists 1
#define root_gpio_config_gpio_controller_tester_gpio 83
#define root_gpio_config_gpio_controller_tester_gpioIrq_exists 1
#define root_gpio_config_gpio_controller_tester_gpioIrq 83

// hcs node macros: root/i2c_config
#define root_i2c_config_exists 1
#define root_i2c_config_foreach_child(func) \
    func(root_i2c_config_i2c_controller_tester)

#define root_i2c_config_foreach_child_vargs(func, ...) \
    func(root_i2c_config_i2c_controller_tester, __VA_ARGS__)


// hcs node macros: root/i2c_config/i2c_controller_tester
#define root_i2c_config_i2c_controller_tester_exists 1
#define root_i2c_config_i2c_controller_tester_match_attr_exists 1
#define root_i2c_config_i2c_controller_tester_match_attr "I2C_TEST"
#define root_i2c_config_i2c_controller_tester_bus_num_exists 1
#define root_i2c_config_i2c_controller_tester_bus_num 6
#define root_i2c_config_i2c_controller_tester_dev_addr_exists 1
#define root_i2c_config_i2c_controller_tester_dev_addr 93
#define root_i2c_config_i2c_controller_tester_reg_addr_exists 1
#define root_i2c_config_i2c_controller_tester_reg_addr 208
#define root_i2c_config_i2c_controller_tester_reg_len_exists 1
#define root_i2c_config_i2c_controller_tester_reg_len 2
#define root_i2c_config_i2c_controller_tester_buf_size_exists 1
#define root_i2c_config_i2c_controller_tester_buf_size 7

// hcs node macros: root/pwm_test
#define root_pwm_test_exists 1
#define root_pwm_test_module_exists 1
#define root_pwm_test_module "pwm_test"
#define root_pwm_test_match_attr_exists 1
#define root_pwm_test_match_attr "PWM_TEST"
#define root_pwm_test_num_exists 1
#define root_pwm_test_num 0
#define root_pwm_test_period_exists 1
#define root_pwm_test_period 999
#define root_pwm_test_duty_exists 1
#define root_pwm_test_duty 333
#define root_pwm_test_polarity_exists 1
#define root_pwm_test_polarity 0
#define root_pwm_test_status_exists 1
#define root_pwm_test_status 1

// hcs node macros: root/spi_test
#define root_spi_test_exists 1
#define root_spi_test_module_exists 1
#define root_spi_test_module "spi_test"
#define root_spi_test_match_attr_exists 1
#define root_spi_test_match_attr "SPI_TEST"
#define root_spi_test_bus_exists 1
#define root_spi_test_bus 0
#define root_spi_test_cs_exists 1
#define root_spi_test_cs 0
#define root_spi_test_len_exists 1
#define root_spi_test_len 8
#define root_spi_test_wbuf_exists 1
#define root_spi_test_wbuf_array_size 8
#define root_spi_test_wbuf_data { \
    161, 178, 195, 212, 229, 246, 120, 144 \
}

// hcs node macros: root/sdio_config
#define root_sdio_config_exists 1
#define root_sdio_config_foreach_child(func) \
    func(root_sdio_config_sdio_controller_tester)

#define root_sdio_config_foreach_child_vargs(func, ...) \
    func(root_sdio_config_sdio_controller_tester, __VA_ARGS__)


// hcs node macros: root/sdio_config/sdio_controller_tester
#define root_sdio_config_sdio_controller_tester_exists 1
#define root_sdio_config_sdio_controller_tester_match_attr_exists 1
#define root_sdio_config_sdio_controller_tester_match_attr "SDIO_TEST"
#define root_sdio_config_sdio_controller_tester_busNum_exists 1
#define root_sdio_config_sdio_controller_tester_busNum 2
#define root_sdio_config_sdio_controller_tester_funcNum_exists 1
#define root_sdio_config_sdio_controller_tester_funcNum 1
#define root_sdio_config_sdio_controller_tester_vendorId_exists 1
#define root_sdio_config_sdio_controller_tester_vendorId 662
#define root_sdio_config_sdio_controller_tester_deviceId_exists 1
#define root_sdio_config_sdio_controller_tester_deviceId 21319

// hcs node macros: root/emmc_config
#define root_emmc_config_exists 1
#define root_emmc_config_foreach_child(func) \
    func(root_emmc_config_emmc_controller_tester)

#define root_emmc_config_foreach_child_vargs(func, ...) \
    func(root_emmc_config_emmc_controller_tester, __VA_ARGS__)


// hcs node macros: root/emmc_config/emmc_controller_tester
#define root_emmc_config_emmc_controller_tester_exists 1
#define root_emmc_config_emmc_controller_tester_match_attr_exists 1
#define root_emmc_config_emmc_controller_tester_match_attr "EMMC_TEST"
#define root_emmc_config_emmc_controller_tester_busNum_exists 1
#define root_emmc_config_emmc_controller_tester_busNum 0
#define root_emmc_config_emmc_controller_tester_hostId_exists 1
#define root_emmc_config_emmc_controller_tester_hostId 0

// hcs node macros: root/uart_test
#define root_uart_test_exists 1
#define root_uart_test_module_exists 1
#define root_uart_test_module "uart_test"
#define root_uart_test_match_attr_exists 1
#define root_uart_test_match_attr "UART_TEST"
#define root_uart_test_port_exists 1
#define root_uart_test_port 1
#define root_uart_test_len_exists 1
#define root_uart_test_len 8
#define root_uart_test_wbuf_exists 1
#define root_uart_test_wbuf_array_size 8
#define root_uart_test_wbuf_data { \
    161, 178, 195, 212, 229, 246, 120, 144 \
}

#endif // HCS_CONFIG_HDF_MACRO_TEST_HEADER_H
