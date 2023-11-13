// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 BayLibre SAS
 * Author: Fabien Parent <fparent@baylibre.com>
 */

#include <common.h>
#include <dm.h>
#include <efi_loader.h>
#include <net.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/arm-smccc.h>

#define MT8195_UPDATABLE_IMAGES	5

#if CONFIG_IS_ENABLED(EFI_HAVE_CAPSULE_SUPPORT)
static struct efi_fw_image fw_images[MT8195_UPDATABLE_IMAGES] = {0};

struct efi_capsule_update_info update_info = {
	.dfu_string = "mmc 0=bl2.img raw 0x0 0x400000 mmcpart 1;"
			"fip.bin part 0 1;firmware.vfat part 0 3;u-boot-env.bin raw 0x0 0x400000 mmcpart 2",
	.images = fw_images,
};

u8 num_image_type_guids = MT8195_UPDATABLE_IMAGES;
#endif

#if defined(CONFIG_EFI_HAVE_CAPSULE_SUPPORT) && defined(CONFIG_EFI_PARTITION)
enum mt8195_updatable_images {
	MT8195_BL2_IMAGE = 1,
	MT8195_FIP_IMAGE,
	MT8195_FW_IMAGE,
	MT8195_ENV_IMAGE,
	MT8195_FIT_IMAGE,
};

static bool board_is_mt8195_demo(void)
{
	return CONFIG_IS_ENABLED(TARGET_MT8195) &&
		of_machine_is_compatible("mediatek,mt8195-demo");
}

static bool board_is_genio_1200_evk(void)
{
	return CONFIG_IS_ENABLED(TARGET_MT8195) &&
		of_machine_is_compatible("mediatek,genio-1200-evk");
}

static bool board_is_genio_1200_evk_ufs(void)
{
	return CONFIG_IS_ENABLED(TARGET_MT8195) &&
		of_machine_is_compatible("mediatek,genio-1200-evk-ufs");
}

void mediatek_capsule_update_board_setup(void)
{
	fw_images[0].image_index = MT8195_FIT_IMAGE;
	fw_images[1].image_index = MT8195_FIP_IMAGE;
	fw_images[2].image_index = MT8195_BL2_IMAGE;
	fw_images[3].image_index = MT8195_FW_IMAGE;
	fw_images[4].image_index = MT8195_ENV_IMAGE;

	if (board_is_mt8195_demo()) {
		efi_guid_t image_type_guid = MT8195_DEMO_FIT_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = MT8195_DEMO_FIP_IMAGE_GUID;
		efi_guid_t bl2_image_type_guid = MT8195_DEMO_BL2_IMAGE_GUID;
		efi_guid_t fw_image_type_guid = MT8195_DEMO_FW_IMAGE_GUID;
		efi_guid_t env_image_type_guid = MT8195_DEMO_ENV_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);
		guidcpy(&fw_images[2].image_type_id, &bl2_image_type_guid);
		guidcpy(&fw_images[3].image_type_id, &fw_image_type_guid);
		guidcpy(&fw_images[4].image_type_id, &env_image_type_guid);

		fw_images[0].fw_name = u"MT8195-DEMO-FIT";
		fw_images[1].fw_name = u"MT8195-DEMO-FIP";
		fw_images[2].fw_name = u"MT8195-DEMO-BL2";
		fw_images[3].fw_name = u"MT8195-DEMO-FW";
		fw_images[4].fw_name = u"MT8195-DEMO-ENV";
	} else if (board_is_genio_1200_evk()) {
		efi_guid_t image_type_guid = GENIO_1200_EVK_FIT_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = GENIO_1200_EVK_FIP_IMAGE_GUID;
		efi_guid_t bl2_image_type_guid = GENIO_1200_EVK_BL2_IMAGE_GUID;
		efi_guid_t fw_image_type_guid = GENIO_1200_EVK_FW_IMAGE_GUID;
		efi_guid_t env_image_type_guid = GENIO_1200_EVK_ENV_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);
		guidcpy(&fw_images[2].image_type_id, &bl2_image_type_guid);
		guidcpy(&fw_images[3].image_type_id, &fw_image_type_guid);
		guidcpy(&fw_images[4].image_type_id, &env_image_type_guid);

		fw_images[0].fw_name = u"GENIO-1200-EVK-FIT";
		fw_images[1].fw_name = u"GENIO-1200-EVK-FIP";
		fw_images[2].fw_name = u"GENIO-1200-EVK-BL2";
		fw_images[3].fw_name = u"GENIO-1200-EVK-FW";
		fw_images[4].fw_name = u"GENIO-1200-EVK-ENV";
	} else if (board_is_genio_1200_evk_ufs()) {
		efi_guid_t image_type_guid = GENIO_1200_EVK_UFS_FIT_IMAGE_GUID;
		efi_guid_t uboot_image_type_guid = GENIO_1200_EVK_UFS_FIP_IMAGE_GUID;
		efi_guid_t bl2_image_type_guid = GENIO_1200_EVK_UFS_BL2_IMAGE_GUID;
		efi_guid_t fw_image_type_guid = GENIO_1200_EVK_UFS_FW_IMAGE_GUID;
		efi_guid_t env_image_type_guid = GENIO_1200_EVK_UFS_ENV_IMAGE_GUID;

		guidcpy(&fw_images[0].image_type_id, &image_type_guid);
		guidcpy(&fw_images[1].image_type_id, &uboot_image_type_guid);
		guidcpy(&fw_images[2].image_type_id, &bl2_image_type_guid);
		guidcpy(&fw_images[3].image_type_id, &fw_image_type_guid);
		guidcpy(&fw_images[4].image_type_id, &env_image_type_guid);

		fw_images[0].fw_name = u"GENIO-1200-EVK-UFS-FIT";
		fw_images[1].fw_name = u"GENIO-1200-EVK-UFS-FIP";
		fw_images[2].fw_name = u"GENIO-1200-EVK-UFS-BL2";
		fw_images[3].fw_name = u"GENIO-1200-EVK-UFS-FW";
		fw_images[4].fw_name = u"GENIO-1200-EVK-UFS-ENV";
	}
}
#endif /* CONFIG_EFI_HAVE_CAPSULE_SUPPORT && CONFIG_EFI_PARTITION */

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
	writel(0, 0x10007000);

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

#define SKU_REG 0x10005220

#define SKU_BIT0 0
#define SKU_BIT1 1
#define SKU_BIT2 2
#define SKU_BIT3 3
#define SKU_BIT4 4

struct sku_info {
    uint32_t sku_id;
    char* sku_str;
	const char* dtbo_file;
};

struct sku_info skus[] = {
    {0, "LEC-MTK-I1200-2G-64G-CT(000E)", "conf-memory-2G.dtbo"},
    {1, "LEC-MTK-I1200-4G-64G-CT(100E)", "conf-memory-4G.dtbo"},
    {2, "LEC-MTK-I1200-8G-64G-CT(200E)", "conf-memory-8G.dtbo"},
    {3, "LEC-MTK-I1200-2G-64G-BW-CT(300E)", "conf-memory-2G.dtbo"},
    {4, "LEC-MTK-I1200-4G-64G-BW-CT(400E)", "conf-memory-4G.dtbo"},
    {5, "LEC-MTK-I1200-8G-64G-BW-CT(500E)", "conf-memory-8G.dtbo"},
    {6, "LEC-MTK-I1200-4G-64G-BW-ER(600E)", "conf-memory-4G.dtbo"},
    {7, "LEC-MTK-I1200-8G-64G-BW-ER(700E)", "conf-memory-8G.dtbo"},
    {8, "LEC-MTK-I1200-8G-128G-BW-ER(800E)", "conf-memory-8G.dtbo"}
};

#define PCB_REG			0x10005230
#define PCB_BIT0 20
#define PCB_BIT1 21

#define PCB_PUPD_REG 	0x11F40030
#define PCB_PUPD_BIT0	9
#define PCB_PUPD_BIT1	8


int board_late_init(void)
{
	char *boot_conf_orig = env_get("boot_conf");
	char cmd[128];

    uint32_t sku_id_0 = (*(volatile uint32_t *)SKU_REG >> SKU_BIT0) & 0x1;
    uint32_t sku_id_1 = (*(volatile uint32_t *)SKU_REG >> SKU_BIT1) & 0x1;
    uint32_t sku_id_2 = (*(volatile uint32_t *)SKU_REG >> SKU_BIT2) & 0x1;
    uint32_t sku_id_3 = (*(volatile uint32_t *)SKU_REG >> SKU_BIT3) & 0x1;
    uint32_t sku_id_4 = (*(volatile uint32_t *)SKU_REG >> SKU_BIT4) & 0x1;

    uint32_t sku_id = (sku_id_4 << 4) | (sku_id_3 << 3) | (sku_id_2 << 2) | (sku_id_1 << 1) | sku_id_0;

    int i;
    for (i = 0; i < sizeof(skus)/sizeof(struct sku_info); i++) {
        if (skus[i].sku_id == sku_id) {
            printf("SKU:   %s\n", skus[i].sku_str);
			snprintf(cmd, sizeof(cmd), "setenv boot_conf '%s#%s'", boot_conf_orig, skus[i].dtbo_file);
			run_command(cmd, 0);
            break;
        }
    }

    if (i == sizeof(skus)/sizeof(struct sku_info)) {
        printf("Unknown SKU\n");
    }

    uint32_t reg_value;

    /* Read the original value of the register */
    reg_value = readl(PCB_PUPD_REG);

    /* Clear bits at positions BIT0 and BIT1 */
    reg_value &= ~(1 << PCB_PUPD_BIT0);
    reg_value &= ~(1 << PCB_PUPD_BIT1);

    /* Write back the new value to the register */
    writel(reg_value, PCB_PUPD_REG);

    uint32_t pcb_id_0 = (*(volatile uint32_t *)PCB_REG >> PCB_BIT0) & 0x1;
    uint32_t pcb_id_1 = (*(volatile uint32_t *)PCB_REG >> PCB_BIT1) & 0x1;

    uint32_t pcb_id = (pcb_id_1 << 1) | pcb_id_0;

    switch (pcb_id) {
    case 0:
        printf("PCB:   PCB A1\n");
        break;
    case 1:
        printf("PCB:   PCB A2\n");
        break;
    case 2:
        printf("PCB:   PCB A3\n");
        break;
    case 3:
        printf("PCB:   PCB A4\n");
        break;
    default:
        printf("Unknown PCB\n");
        break;
    }

    return 0;
}
