/*
 * Copyright (C) 2009 Michael Guntsche <mike@it-loops.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 */

#include <linux/init.h>
#include <linux/leds.h>
#include <linux/platform_device.h>
#include <asm/io.h>

struct led_config {
	u32 *addr;
	u32 bitmask;
};

static struct led_config ulcfg;

static void rbppc_led_set_user(struct led_classdev *led_cdev,
			       enum led_brightness brightness)
{
	if (brightness)
		setbits32(ulcfg.addr, ulcfg.bitmask);
	else
		clrbits32(ulcfg.addr, ulcfg.bitmask);
}

static struct led_classdev rbppc_led = {
       .name = "user-led",
       .brightness_set = rbppc_led_set_user,
};

static int rbppc_led_probe(struct platform_device *pdev)
{
	struct resource *res = &pdev->resource[0];
	ulcfg.addr = ioremap_nocache(res->start, res->end - res->start + 1);
	ulcfg.bitmask = 1 << (31 - pdev->resource[1].start);

	return led_classdev_register(&pdev->dev, &rbppc_led);
}

static struct platform_driver rbppc_led_driver = {
	.probe	= rbppc_led_probe,
	.driver	= {
		.name = "rbppc-led",
		.owner = THIS_MODULE,
	},
};

static int __init rbppc_led_init(void)
{
	return platform_driver_register(&rbppc_led_driver);
}

static void __exit rbppc_led_exit(void)
{
	platform_driver_unregister(&rbppc_led_driver);
}

module_init(rbppc_led_init);
module_exit(rbppc_led_exit);
