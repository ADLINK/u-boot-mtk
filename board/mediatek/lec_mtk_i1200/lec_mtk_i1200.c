// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 BayLibre SAS
 * Author: Fabien Parent <fparent@baylibre.com>
 */

#include <common.h>
#include <dm.h>
#include <net.h>
#include <asm/io.h>
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
