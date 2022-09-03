#
# Copyright (c) 2021 Huawei Device Co., Ltd.
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
KHDF_CAMERA_RK3568_DIR = $(KHDF_CAMERA_BASE_ROOT_DIR)/device/board/hihope/rk3568/camera_adapter/

obj-$(CONFIG_DRIVERS_HDF_CAMERA) += \
          $(KHDF_CAMERA_ROOT_DIR)/core/src/hdf_camera_core.o \
          $(KHDF_CAMERA_ROOT_DIR)/common/src/hdf_camera_config_parser.o \
          $(KHDF_CAMERA_ROOT_DIR)/common/src/hdf_camera_utils.o \
          $(KHDF_CAMERA_ROOT_DIR)/manager/device_manager/src/hdf_camera_device_manager.o \
          $(KHDF_CAMERA_ROOT_DIR)/device_impl/src/hdf_camera_sensor.o \
          $(KHDF_CAMERA_ROOT_DIR)/dispatch/src/hdf_camera_dispatch.o \
          $(KHDF_CAMERA_ROOT_DIR)/event/src/hdf_camera_event.o \

obj-$(CONFIG_DRIVERS_HDF_CAMERA) += $(KHDF_CAMERA_RK3568_DIR)

ccflags-$(CONFIG_DRIVERS_HDF_CAMERA) +=-I$(srctree)/drivers/hdf/framework/include/utils \
										-I$(srctree)/drivers/hdf/framework/utils/include \
										-I$(srctree)/drivers/hdf/khdf/osal/include \
										-I$(srctree)/drivers/hdf/framework/include/core \
										-I$(srctree)/drivers/hdf/framework/core/common/include/host \
										-I$(srctree)/drivers/hdf/framework/core/shared/include \
										-I$(srctree)/drivers/hdf/framework/core/host/include \
										-I$(srctree)/drivers/hdf/framework/core/manager/include \
										-I$(srctree)/drivers/hdf/framework/include \
										-I$(srctree)/drivers/hdf/framework/include/osal \
										-I$(srctree)/drivers/hdf/framework/include/platform \
										-I$(srctree)/drivers/hdf/framework/include/config \
										-I$(srctree)/drivers/hdf/framework/model/camera/core/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/event/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/device_impl/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/dispatch/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/common/include \
										-I$(srctree)/drivers/hdf/framework/model/camera/manager/device_manager/include \
										-I$(srctree)/../../../../device/board/hihope/rk3568/camera_adapter \
										-I$(srctree)/bounds_checking_function/include
