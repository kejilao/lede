/*
 *  TP-LINK TL-WR840N v2/v3 / TL-WR841N/ND v9/v11 / TL-WR842N/ND v3
 *
 *  Copyright (C) 2014 Matthias Schiffer <mschiffer@universe-factory.net>
 *  Copyright (C) 2016 Cezary Jackiewicz <cezary@eko.one.pl>
 *  Copyright (C) 2016 Stijn Segers <francesco.borromini@gmail.com>
 *  Copyright (C) 2017 Vaclav Svoboda <svoboda@neng.cz>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "dev-ap9x-pci.h"
#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define TL_WR841NV9_GPIO_LED_4G_S1 	15
#define TL_WR841NV9_GPIO_LED_4G_S2 	16
#define TL_WR841NV9_GPIO_LED_WLAN	14
#define TL_WR841NV9_GPIO_LED_WAN	11
#define TL_WR841NV9_GPIO_LED_LAN 	4

#define TL_WR841NV9_GPIO_BTN_RESET	17

#define TL_WR841NV9_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WR841NV9_KEYS_DEBOUNCE_INTERVAL (3 * TL_WR841NV9_KEYS_POLL_INTERVAL)

static const char *tl_wr841n_v9_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_wr841n_v9_flash_data = {
	.part_probes	= tl_wr841n_v9_part_probes,
};

static struct gpio_led tl_wr841n_v9_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:green:lan",
		.gpio		= TL_WR841NV9_GPIO_LED_LAN,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:wan",
		.gpio		= TL_WR841NV9_GPIO_LED_WAN,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:wlan",
		.gpio		= TL_WR841NV9_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:4G_S1",
		.gpio		= TL_WR841NV9_GPIO_LED_4G_S1,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:4G_S2",
		.gpio		= TL_WR841NV9_GPIO_LED_4G_S2,
		.active_low	= 1,
	}
};

static struct gpio_keys_button tl_wr841n_v9_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_WR841NV9_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR841NV9_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init tl_ap143_setup(void)
{
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ath79_register_m25p80(&tl_wr841n_v9_flash_data);
	ath79_gpio_function_setup(AR934X_GPIO_FUNC_JTAG_DISABLE,
		AR934X_GPIO_FUNC_CLK_OBS4_EN);
	ath79_gpio_output_select(0, 0);
	ath79_gpio_output_select(1, 0);
	ath79_gpio_output_select(2, 0);
	ath79_gpio_output_select(3, 0);
	ath79_gpio_output_select(4, 0);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);
	ath79_register_usb();
	ath79_register_pci();

	/* LAN */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, ee + 2, -1);
	ath79_register_eth(1);

	/* WAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, ee + 2, -2);
	ath79_register_eth(0);

	ath79_register_wmac(ee, NULL);
}

static void __init tl_wr841n_v9_setup(void)
{
	tl_ap143_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(tl_wr841n_v9_leds_gpio),
				 tl_wr841n_v9_leds_gpio);

	ath79_register_gpio_keys_polled(1, TL_WR841NV9_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(tl_wr841n_v9_gpio_keys),
					tl_wr841n_v9_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_TL_WR841N_V9, "TL-WR841N-v9", "TP-LINK TL-WR841N/ND v9",
	     tl_wr841n_v9_setup);

