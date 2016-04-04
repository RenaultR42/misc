/*
 * Driver to get TVP5158 information from HDVPSS firmware through FVID2 API.
 *
 * Copyright (C) 2015 Nexvision
 * Author: Charles-Antoine Couret <charles-antoine.couret@nexvision.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/dma-mapping.h>
#include <linux/kobject.h>
#include <linux/slab.h>
#include <media/davinci/vpss.h>
#include <linux/fvid2.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <asm/uaccess.h>
#include <linux/vps_proxyserver.h>
#include "core.h"

/* This module use HDVPSS requests to get or set some TVP5158 registers.
   It must be used with TI's SDK for OMX acceleration purpose and modified HDVPSS 
   firmware (and TI's patched kernel) to get these IOCTL without use the TVP5158 handler.
   Because the TVP5158 is initialized by HDVPSS and OMX and we cannot get this handler easly...
   It's a ugly way but it is useful in our case  It should not be commited upstream (not relevant).
 */

#define VPS_VIDEO_DECODER_DEV_PER_PORT_MAX 2

enum regs_io {
    REGS_READ = 0,
    REGS_WRITE
};

enum fvid2_ioctl {
    IOCTL_VPS_VIDEO_DECODER_GET_CHIP_ID = FVID2_USER_BASE + 0x00060000u,
    IOCTL_VPS_VIDEO_DECODER_RESET,
    IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_MODE,
    IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS,
    IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_COLOR,
    IOCTL_VPS_VIDEO_DECODER_REG_WRITE = IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_COLOR + 0x08,
    IOCTL_VPS_VIDEO_DECODER_REG_READ,
};

/* To avoid concurrent access. One request allowed in the same time. */
static DEFINE_MUTEX(tvp_info);

/* Structure to request on one channel its video information which provides by TVP5158 */
struct Vps_VideoDecoderVideoStatusParams {
	uint32_t channelNum;
};

/* Structure to get video information provides by TVP5158 */
struct Vps_VideoDecoderVideoStatus {
	uint32_t isVideoDetect;
	uint32_t frameWidth;
	uint32_t frameHeight;
	uint32_t frameInterval;
	uint32_t isInterlaced;
};

/* Structure to get or set registers into TVP5158 */
struct Vps_VideoDecoderRegRdWrParams {
	uint32_t deviceNum;
	uint32_t numRegs;

/* These fields are uint8_t * in the FVID2 API
 * but it is mandatory to have a shared memory address between A8 (Linux) and M3 (HDVPSS).
 * This address is saved by uint32_t
 */
	uint32_t regAddr;
	uint32_t regValue8;
};


static struct tvp_parameters_fvid2 {
	u32 videoInfoPhy;
	struct Vps_VideoDecoderVideoStatus *videoInfo;
	u32 videoArgsPhy;
	struct Vps_VideoDecoderVideoStatusParams *videoArgs;

	u32 writeEnablePhy;
	struct Vps_VideoDecoderRegRdWrParams *writeRegs;
	uint8_t *regsWriteValue;
	uint8_t *regsWriteAddr;

	u32 readRegsPhy;
	struct Vps_VideoDecoderRegRdWrParams *readRegs;
	uint8_t *regsReadValue;
	uint8_t *regsReadAddr;
} tvp_parameters;

struct vps_fvid2_ctrl *handler = NULL;
static struct vps_payload_info *tvp_payload_info;

/* We use system handler to send orders. Because OMX creates the TVP driver in HDVPSS side
 * and we cannot create this driver too. Driver which is needed to send message.
 * So, you need to adapt HDVPSS firmware to capture TVP's IOCTL with another handler driver.
 */
void tvp5158_set_handler(void *new_handler)
{
	handler = (struct vps_fvid2_ctrl *) new_handler;
}

/* The size of shared memory needed to send/receive data with HDVPSS */
static inline int get_payload_size(void)
{
	int size = 0;

	size  = sizeof(struct Vps_VideoDecoderVideoStatus);
	size += sizeof(struct Vps_VideoDecoderVideoStatusParams);

	/* These structures have pointer to some registers values and address, it is mandatory
	 * to allocate these datas in shared memory */
	size += sizeof(struct Vps_VideoDecoderRegRdWrParams) + sizeof(uint8_t) * 2;
	size += sizeof(struct Vps_VideoDecoderRegRdWrParams) + sizeof(uint8_t) * 2;

	return size;
}

static void video_info_to_buffer(struct Vps_VideoDecoderVideoStatus *videoStatus, char *buffer, size_t len, int error)
{
	if (videoStatus != NULL && !error) {
		snprintf(buffer, len, "%d;%u;%ux%u@%u%c", error,
			 videoStatus->isVideoDetect, videoStatus->frameWidth,
			 videoStatus->frameHeight,
             videoStatus->frameInterval, videoStatus->isInterlaced ? 'i' : ' ');
	} else {
		snprintf(buffer, len, "%d;;;;;", error);
	}
}

static void tvp5158_m3_video_info(uint32_t channel, char *buffer, int size)
{
	int err = 0;

	if (handler == NULL) {
		err = FVID2_EFAIL;
	} else {
		mutex_lock(&tvp_info);
		tvp_parameters.videoArgs->channelNum = channel;

		err = vps_fvid2_control(handler, IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS,
				(void *) tvp_parameters.videoArgsPhy,
				(void *) tvp_parameters.videoInfoPhy);

		video_info_to_buffer(tvp_parameters.videoInfo, buffer, size, err);
		mutex_unlock(&tvp_info);
		return;
	}

	video_info_to_buffer(NULL, buffer, size, err);
}

static uint8_t tvp5158_m3_read_regs(uint8_t channel, uint8_t regAddr)
{
	int err = 0;
	uint8_t value;

	if (handler == NULL) {
		err = FVID2_EFAIL;
	} else {
		mutex_lock(&tvp_info);
		*(tvp_parameters.regsWriteAddr) = 0xFF;
		*(tvp_parameters.regsWriteValue) = 0x1 << channel;
		err = vps_fvid2_control(handler, IOCTL_VPS_VIDEO_DECODER_REG_WRITE,
				(void *) tvp_parameters.writeEnablePhy, NULL);

		if (!err) {
			*(tvp_parameters.regsReadAddr) = regAddr;
			err = vps_fvid2_control(handler, IOCTL_VPS_VIDEO_DECODER_REG_READ,
					(void *) tvp_parameters.readRegsPhy, NULL);
		}

		value = *(tvp_parameters.regsReadValue);
		mutex_unlock(&tvp_info);
	}

	if (err)
		return err;
	else
		return value;
}

void tvp5158_m3_write_regs(uint8_t regAddr, uint8_t regValue)
{
	int err = 0;

	if (handler == NULL) {
		return;
	} else {
		mutex_lock(&tvp_info);
		*(tvp_parameters.regsWriteAddr) = 0xFE;
		*(tvp_parameters.regsWriteValue) = 0xF;
		err = vps_fvid2_control(handler, IOCTL_VPS_VIDEO_DECODER_REG_WRITE,
				(void *) tvp_parameters.writeEnablePhy, NULL);

		if (!err) {
			*(tvp_parameters.regsWriteAddr) = regAddr;
			*(tvp_parameters.regsWriteValue) = regValue;
			vps_fvid2_control(handler, IOCTL_VPS_VIDEO_DECODER_REG_WRITE,
					(void *) tvp_parameters.writeEnablePhy, NULL);
		}

		mutex_unlock(&tvp_info);
	}
}

#define ATTR_SHOW_CHANNEL(number)	\
static ssize_t show_##number(struct device *dev,	\
				     struct device_attribute *attr,	\
				     char *buf)	\
{	\
	char info[PAGE_SIZE]; \
	tvp5158_m3_video_info(number, info, PAGE_SIZE);  \
	return snprintf(buf, PAGE_SIZE + 1, "%s\n", info);	\
}

ATTR_SHOW_CHANNEL(0);
ATTR_SHOW_CHANNEL(1);
ATTR_SHOW_CHANNEL(2);
ATTR_SHOW_CHANNEL(3);

#define ATTR_SHOW_CHANNEL_REGS(number)	\
static uint8_t addrRegs##number = 0; \
static ssize_t show_regs_##number(struct device *dev,	\
				     struct device_attribute *attr,	\
				     char *buf)	\
{	\
	uint8_t value; \
	value = tvp5158_m3_read_regs(number, addrRegs##number); \
	return snprintf(buf, PAGE_SIZE + 1, "Reg 0x%x : 0x%x\n", addrRegs##number, value);	\
} \
\
static ssize_t store_regs_##number(struct device *dev,	\
				     struct device_attribute *attr,	\
				     const char *buf, size_t count)	\
{	\
	int mode, addr, value; \
	sscanf(buf, "%d;0x%x:0x%x", &mode, &addr, &value);	\
\
	if (mode == REGS_READ) \
		addrRegs##number = addr; \
	else \
		tvp5158_m3_write_regs(addr, value); \
	return count; \
}

ATTR_SHOW_CHANNEL_REGS(0);
ATTR_SHOW_CHANNEL_REGS(1);
ATTR_SHOW_CHANNEL_REGS(2);
ATTR_SHOW_CHANNEL_REGS(3);

static DEVICE_ATTR(video_0, 0444, show_0, NULL);
static DEVICE_ATTR(video_1, 0444, show_1, NULL);
static DEVICE_ATTR(video_2, 0444, show_2, NULL);
static DEVICE_ATTR(video_3, 0444, show_3, NULL);

static DEVICE_ATTR(regs_0, 0666, show_regs_0, store_regs_0);
static DEVICE_ATTR(regs_1, 0666, show_regs_1, store_regs_1);
static DEVICE_ATTR(regs_2, 0666, show_regs_2, store_regs_2);
static DEVICE_ATTR(regs_3, 0666, show_regs_3, store_regs_3);

static struct class *tvp5158_m3_class;
static struct device *tvp5158_m3_dev;

static __init int create_sysfs_entries(void)
{
	tvp5158_m3_class = class_create(THIS_MODULE, "tvp5158_m3");
	if (IS_ERR(tvp5158_m3_class)) {
		pr_err("dau_ids: Error in class_create\n");
		return PTR_ERR(tvp5158_m3_class);
	}

	tvp5158_m3_dev = device_create(tvp5158_m3_class, NULL, 0, NULL, "tvp5158_m3");
	if (IS_ERR(tvp5158_m3_dev)) {
		pr_err("dau_ids: Error in device_create\n");
		return PTR_ERR(tvp5158_m3_dev);
	}

	return 0;
}

int __init tvp5158_m3_init(void)
{
	int size, offset = 0;
	int r = 0;

	tvp_payload_info = kzalloc(sizeof(struct vps_payload_info), GFP_KERNEL);
	if (!tvp_payload_info) {
		pr_err("allocated payload info failed.\n");
		return -ENOMEM;
	}

	size = get_payload_size();
	tvp_payload_info->vaddr = vps_sbuf_alloc(size, &tvp_payload_info->paddr);
	if (tvp_payload_info->vaddr == NULL) {
		pr_err("alloc tvp dma buffer failed\n");
		tvp_payload_info->paddr = 0u;
		r = -ENOMEM;
		goto cleanup;
	}

	tvp_payload_info->size = PAGE_ALIGN(size);
	memset(tvp_payload_info->vaddr, 0, tvp_payload_info->size);

	tvp_parameters.videoInfo = setaddr(tvp_payload_info, &offset,
					      &(tvp_parameters.videoInfoPhy),
					      sizeof(struct Vps_VideoDecoderVideoStatus));

	tvp_parameters.videoArgs = setaddr(tvp_payload_info, &offset,
					      &(tvp_parameters.videoArgsPhy),
					      sizeof(struct Vps_VideoDecoderVideoStatusParams));

	tvp_parameters.writeRegs = setaddr(tvp_payload_info, &offset,
					      &(tvp_parameters.writeEnablePhy),
					      sizeof(struct Vps_VideoDecoderRegRdWrParams));
	tvp_parameters.regsWriteAddr = setaddr(tvp_payload_info, &offset,
					      (u32 *)&(tvp_parameters.writeRegs->regAddr),
					      sizeof(uint8_t));
	tvp_parameters.regsWriteValue = setaddr(tvp_payload_info, &offset,
					      (u32 *)&(tvp_parameters.writeRegs->regValue8),
					      sizeof(uint8_t));

	tvp_parameters.readRegs = setaddr(tvp_payload_info, &offset,
					      &(tvp_parameters.readRegsPhy),
					      sizeof(struct Vps_VideoDecoderRegRdWrParams));
	tvp_parameters.regsReadAddr = setaddr(tvp_payload_info, &offset,
					      (u32 *)&(tvp_parameters.readRegs->regAddr),
					      sizeof(uint8_t));
	tvp_parameters.regsReadValue = setaddr(tvp_payload_info, &offset,
					      (u32 *)&(tvp_parameters.readRegs->regValue8),
					      sizeof(uint8_t));

	tvp_parameters.writeRegs->deviceNum = 0;
	tvp_parameters.readRegs->deviceNum = 0;

	tvp_parameters.writeRegs->numRegs = 1;
	tvp_parameters.readRegs->numRegs = 1;

	create_sysfs_entries();
	r |= device_create_file(tvp5158_m3_dev, &dev_attr_video_0);
	r |= device_create_file(tvp5158_m3_dev, &dev_attr_video_1);
	r |= device_create_file(tvp5158_m3_dev, &dev_attr_video_2);
	r |= device_create_file(tvp5158_m3_dev, &dev_attr_video_3);

	r |= device_create_file(tvp5158_m3_dev, &dev_attr_regs_0);
	r |= device_create_file(tvp5158_m3_dev, &dev_attr_regs_1);
	r |= device_create_file(tvp5158_m3_dev, &dev_attr_regs_2);
	r |= device_create_file(tvp5158_m3_dev, &dev_attr_regs_3);

	if (r != 0)
		goto cleanup;

	return 0;

cleanup:
	vps_sbuf_free(tvp_payload_info->paddr, tvp_payload_info->vaddr, tvp_payload_info->size);
	kfree(tvp_payload_info);
	tvp_payload_info = NULL;
	return r;
}

void tvp5158_m3_exit(void)
{
	device_remove_file(tvp5158_m3_dev, &dev_attr_video_0);
	device_remove_file(tvp5158_m3_dev, &dev_attr_video_1);
	device_remove_file(tvp5158_m3_dev, &dev_attr_video_2);
	device_remove_file(tvp5158_m3_dev, &dev_attr_video_3);

	device_remove_file(tvp5158_m3_dev, &dev_attr_regs_0);
	device_remove_file(tvp5158_m3_dev, &dev_attr_regs_1);
	device_remove_file(tvp5158_m3_dev, &dev_attr_regs_2);
	device_remove_file(tvp5158_m3_dev, &dev_attr_regs_3);

	vps_sbuf_free(tvp_payload_info->paddr, tvp_payload_info->vaddr, tvp_payload_info->size);
	kfree(tvp_payload_info);

	device_destroy(tvp5158_m3_class, 0);
	class_unregister(tvp5158_m3_class);
	class_destroy(tvp5158_m3_class);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charles-Antoine Couret <charles-antoine.couret@nexvision.fr>");
MODULE_DESCRIPTION("Driver to get TVP5158 information from HDVPSS firmware");
