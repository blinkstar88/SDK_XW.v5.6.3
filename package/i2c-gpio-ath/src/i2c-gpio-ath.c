/*
 * i2c adapter on atheros gpio
 *
 * Copyright (C) 2012 Ubiquiti Networks.
 * Author Antanas Gadeikis (kaleda@ubnt.com)
 * Based on i2c-gpio.c by Denis Bodor
 * Based on i2c-parport-light.c by Jean Delvare
 * Based on i2c_gpio by olg & Yoshinori Sato
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* for copy_from_user */

#define DRIVER_VERSION "0.1"

#define NAME "gpio_i2c"

/* GPIO low level functions */
typedef unsigned int ATH11n_REG;
#define sysRegRead(phys)		readl(phys)
#define sysRegWrite(phys, val)	writel(val, phys)
//#define sysRegRead(phys)			(*(volatile ATH11n_REG *)KSEG1ADDR(phys))
//#define sysRegWrite(phys, val)	((*(volatile ATH11n_REG *)KSEG1ADDR(phys)) = (val))


#define ATH11n_BASE 			0xb0000000
#define ATH11n_GPIO_DI          (ATH11n_BASE + 0x4048) /* intput register */
#define ATH11n_GPIO_DO          (ATH11n_BASE + 0x4048) /* output register */
#define ATH11n_GPIO_CR          (ATH11n_BASE + 0x404c) /* control register */

#define	gpio_read_shift	10

#define GPIO_CLOCK	(0)
#define GPIO_DATA	(1)


/* set a GPIO bit */
static void
bit_gpio_set(u32 gpio)
{
	u32 reg;
	u32 mask = 1 << gpio;
	reg = sysRegRead(ATH11n_GPIO_DO);
	reg = reg | mask; /* set bit */
	sysRegWrite(ATH11n_GPIO_DO,reg);
}

/* clear a GPIO bit */
static void
bit_gpio_clear(u32 gpio)
{
	u32 reg;
	u32 mask = 1 << gpio;
	reg = sysRegRead(ATH11n_GPIO_DO);
	reg = reg & ~mask; /* clear bit */
	sysRegWrite(ATH11n_GPIO_DO,reg);
}

/* get a GPIO bit */
static int
bit_gpio_get(u32 gpio)
{
	u32 mask = 1 << gpio_read_shift + gpio;
	return (sysRegRead(ATH11n_GPIO_DI) & mask);
}

/* ----- GPIO direction control ------------------------------------------- */
static void
gpio_cfg_output(u32 gpio)
{
	u32 reg = 0;
	u32 mask = 0x03 << 2 * gpio;

	/* If configured as output, set output to drive always */
	reg = sysRegRead(ATH11n_GPIO_CR);
	reg |= mask;
	sysRegWrite(ATH11n_GPIO_CR, reg);
}


static void
gpio_cfg_input(u32 gpio)
{
	u32 reg = 0;
	u32 mask = 0x03 << 2 * gpio;

	/* If configured as input, set output to tristate */
	reg = sysRegRead(ATH11n_GPIO_CR);
	reg &= ~mask;
	sysRegWrite(ATH11n_GPIO_CR, reg);
}

static void
gpio_cfg_opendrain(u32 gpio)
{
	u32 reg = 0;
	u32 mask = 0x01 << 2 * gpio;
	u32 clr_mask = 0x03 << 2 * gpio;

	/* If configured as output, set output to drive always */
	reg = sysRegRead(ATH11n_GPIO_CR);
	reg &= ~clr_mask;
	reg |= mask;
	sysRegWrite(ATH11n_GPIO_CR, reg);
}


/* ----- I2C algorithm call-back functions and structures ----------------- */
static void
bit_gpio_setscl(void *data, int state)
{
	if (state) {
		bit_gpio_set(GPIO_CLOCK);
	} else {
		bit_gpio_clear(GPIO_CLOCK);
	}
	gpio_cfg_opendrain(GPIO_CLOCK);
}

static void
bit_gpio_setsda(void *data, int state)
{
	if (state) {
		bit_gpio_set(GPIO_DATA);
	} else {
		bit_gpio_clear(GPIO_DATA);
	}
	gpio_cfg_opendrain(GPIO_DATA);
}

static int
bit_gpio_getscl(void *data)
{
	gpio_cfg_input(GPIO_CLOCK);
	return bit_gpio_get(GPIO_CLOCK);
}

static int
bit_gpio_getsda(void *data)
{
	gpio_cfg_input(GPIO_DATA);
	return bit_gpio_get(GPIO_DATA);
}

/* Encapsulate the functions above in the correct structure
   Note that getscl will be set to NULL by the attaching code for adapters
   that cannot read SCL back */
static struct i2c_algo_bit_data i2c_gpio_algo_data =
{
	.setsda		= bit_gpio_setsda,
	.setscl		= bit_gpio_setscl,
	.getsda		= bit_gpio_getsda,
	.getscl		= bit_gpio_getscl,
	600,		/* udelay, half-clock-cycle time in microsecs, i.e. clock is (500 / udelay) KHz */ 
	600,		/* mdelay, in millisecs, unused                                                 */
	100,		/* timeout, in jiffies                                                          */
	/* delays are high, GPIO are very slow                                 */
}; 

/* ----- I2c structure ---------------------------------------------------- */
static struct i2c_adapter i2c_gpio_adapter = {
	.owner		= THIS_MODULE,
	.algo_data	= &i2c_gpio_algo_data,
	.name		= "GPIO adapter",
};

/* ----- Module loading, unloading and information ------------------------ */
static __init int
i2c_gpio_init(void)
{
	u32 reg;
	/* disable JTAG */
	reg = sysRegRead(ATH11n_BASE + 0x04054);
	reg |= 1 << 17;
	sysRegWrite(ATH11n_BASE + 0x04054, reg);
	/* init muxer magic. Yes, it is hardcoded */
	reg = sysRegRead(ATH11n_BASE + 0x04060);
	reg &= ~0x07FF;
	sysRegWrite(ATH11n_BASE + 0x04060, reg);
	/* end of magic */

	/* Reset hardware to a sane state (SCL and SDA high) */
	gpio_cfg_opendrain(GPIO_CLOCK);
	gpio_cfg_opendrain(GPIO_DATA);
	bit_gpio_setsda(NULL, 1);
	bit_gpio_setscl(NULL, 1);

	if (i2c_bit_add_bus(&i2c_gpio_adapter) < 0)
	{
		printk(KERN_ERR NAME ": adapter %s registration failed\n",
				i2c_gpio_adapter.name);
		return -ENODEV;
	}

	printk (KERN_NOTICE NAME ": module loaded\n");

	return 0;
}

static void i2c_gpio_exit(void)
{
	i2c_del_adapter(&i2c_gpio_adapter);
	printk(KERN_INFO NAME ": unloaded\n");
}

module_init(i2c_gpio_init);
module_exit(i2c_gpio_exit);

MODULE_AUTHOR("Antanas Gadeikis <kaleda@ubnt.com>");
MODULE_DESCRIPTION("ATH11n GPIO i2c bus v" DRIVER_VERSION);
MODULE_LICENSE("GPL");
