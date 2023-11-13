// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 MediaTek Inc.
 * Author: Macpaul Lin <macpaul.lin@mediatek.com>
 */

#include <common.h>
#include <dm.h>
#include <efi_loader.h>
#include <net.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/arm-smccc.h>

#define MT8370_UPDATABLE_IMAGES	5

#if CONFIG_IS_ENABLED(EFI_HAVE_CAPSULE_SUPPORT)
static struct efi_fw_image fw_images[MT8370_UPDATABLE_IMAGES] = {0};

struct efi_capsule_update_info update_info = {
	.dfu_string = "mmc 0=bl2.img raw 0x0 0x400000 mmcpart 1;"
			"fip.bin part 0 1;firmware.vfat part 0 3;u-boot-env.bin raw 0x0 0x400000 mmcpart 2",
	.images = fw_images,
};

u8 num_image_type_guids = MT8370_UPDATABLE_IMAGES;
#endif

#if defined(CONFIG_EFI_HAVE_CAPSULE_SUPPORT) && defined(CONFIG_EFI_PARTITION)
enum mt8390_updatable_images {
	MT8370_BL2_IMAGE = 1,
	MT8370_FIP_IMAGE,
	MT8370_FW_IMAGE,
	MT8370_ENV_IMAGE,
	MT8370_FIT_IMAGE,
};

static bool board_is_genio_510_evk(void)
{
	return CONFIG_IS_ENABLED(TARGET_MT8370) &&
		of_machine_is_compatible("mediatek,genio-510-evk");
}

void mediatek_capsule_update_board_setup(void)
{
	fw_images[0].image_index = MT8370_FIT_IMAGE;
	fw_images[1].image_index = MT8370_FIP_IMAGE;
	fw_images[2].image_index = MT8370_BL2_IMAGE;
	fw_images[3].image_index = MT8370_FW_IMAGE;
	fw_images[4].image_index = MT8370_ENV_IMAGE;

	if (board_is_genio_510_evk()) {
		efi_guid_t image_type_guid = GENIO_510_EVK_FIT_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = GENIO_510_EVK_FIP_IMAGE_GUID;
		efi_guid_t bl2_image_type_guid = GENIO_510_EVK_BL2_IMAGE_GUID;
		efi_guid_t fw_image_type_guid = GENIO_510_EVK_FW_IMAGE_GUID;
		efi_guid_t env_image_type_guid = GENIO_510_EVK_ENV_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);
		guidcpy(&fw_images[2].image_type_id, &bl2_image_type_guid);
		guidcpy(&fw_images[3].image_type_id, &fw_image_type_guid);
		guidcpy(&fw_images[4].image_type_id, &env_image_type_guid);

		fw_images[0].fw_name = u"GENIO-510-EVK-FIT";
		fw_images[1].fw_name = u"GENIO-510-EVK-FIP";
		fw_images[2].fw_name = u"GENIO-510-EVK-BL2";
		fw_images[3].fw_name = u"GENIO-510-EVK-FW";
		fw_images[4].fw_name = u"GENIO-510-EVK-ENV";
	}
}
#endif /* CONFIG_EFI_HAVE_CAPSULE_SUPPORT && CONFIG_EFI_PARTITION */

/**
 * mediatek_part_name - get the part name
 *
 * Retrieve the part name of platform description.
 *
 * Return:
 * * > 0 - the part name invoked
 * * 0   - error or no part name invoked
 */
u32 mediatek_part_name(void)
{
	struct arm_smccc_res res __maybe_unused;
	u32 ret = 0;

	arm_smccc_smc(MTK_SIP_PARTNAME_ID, 0, 0, 0, 0, 0, 0, 0, &res);
	ret = res.a1;

	if (res.a0)
		return 0;
	else
		return ret;
}

int board_init(void)
{
	struct udevice *dev;
	int ret;

	if (CONFIG_IS_ENABLED(USB_GADGET)) {
		ret = uclass_get_device(UCLASS_USB_GADGET_GENERIC, 0, &dev);
		if (ret) {
			pr_err("%s: Cannot find USB device\n", __func__);
			return ret;
		}
	}

	if (CONFIG_IS_ENABLED(USB_ETHER))
		usb_ether_init();

	printf("Disabling WDT\n");
	writel(0, 0x10005100);

	printf("Enabling SCP SRAM\n");
	for (unsigned int val = 0xFFFFFFFF; val != 0U;) {
		val = val >> 1;
		writel(val, 0x1072102C);
	}

	if (IS_ENABLED(CONFIG_EFI_HAVE_CAPSULE_SUPPORT) &&
	    IS_ENABLED(CONFIG_EFI_PARTITION))
		mediatek_capsule_update_board_setup();

	return 0;
}
