#
# Copyright (c) 2022 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
#

KHDF_CAMERA_BASE_ROOT_DIR = ../../../../../../..
KHDF_CAMERA_ROOT_DIR = ../../../../../framework/model/camera

ifeq ($(CONFIG_DRIVERS_HDF_CAMERA), y)
KHDF_CAMERA_RK3568_DIR = $(KHDF_CAMERA_BASE_ROOT_DIR)/device/board/hihope/rk3568/camera_adapter/
obj-$(CONFIG_DRIVERS_HDF_CAMERA) += $(KHDF_CAMERA_RK3568_DIR)
ccflags-$(CONFIG_DRIVERS_HDF_CAMERA) +=-I$(srctree)/$(KHDF_CAMERA_RK3568_DIR)
endif

obj-$(CONFIG_DRIVERS_HDF_CAMERA) += \
          $(KHDF_CAMERA_ROOT_DIR)/core/src/camera_core.o \
          $(KHDF_CAMERA_ROOT_DIR)/parser/src/camera_config_parser.o \
          $(KHDF_CAMERA_ROOT_DIR)/utils/src/camera_utils.o \
          $(KHDF_CAMERA_ROOT_DIR)/device_manager/src/camera_device_manager.o \
          $(KHDF_CAMERA_ROOT_DIR)/device_impl/src/camera_sensor.o \
          $(KHDF_CAMERA_ROOT_DIR)/dispatch/src/camera_dispatch.o \
		  $(KHDF_CAMERA_ROOT_DIR)/event/src/camera_event.o \

ccflags-$(CONFIG_DRIVERS_HDF_CAMERA) +=-I$(srctree)/drivers/hdf/framework/include \
										-I$(srctree)/drivers/hdf/framework/include/utils \
										-I$(srctree)/drivers/hdf/framework/utils/include \
										-I$(srctree)/drivers/hdf/khdf/osal/include \
										-I$(srctree)/drivers/hdf/framework/core/common/include/host \
										-I$(srctree)/drivers/hdf/framework/core/shared/include \
										-I$(srctree)/drivers/hdf/framework/core/host/include \
										-I$(srctree)/drivers/hdf/framework/core/manager/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/core/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/event/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/device_impl/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/dispatch/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/utils/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/parser/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/device_manager/include \
										-I$(srctree)/bounds_checking_function/include
