/*
 * Copyright (C) 2009 Texas Instruments Inc
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option)any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <linux/module.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/completion.h>
#include <asm/gpio.h>

#define TEST_OUTPUT

static int gpio_num;
static int test_loop=3;

module_param(gpio_num, int, S_IRUSR);
module_param(test_loop, int, S_IRUSR);

DECLARE_COMPLETION(work);

static irqreturn_t handler (int irq, void * dev)
{
  printk("Interrupt is triggered for irq: %d\n", irq);
	complete_all(&work);
	return IRQ_HANDLED;
}


int init_module() 
{
	int status;
  int toggle = 0;
  int i;

	init_completion(&work);

  printk("\nTesting gpio %d \n", gpio_num);

	status = gpio_request(gpio_num, "gpio_test\n");
	if (status < 0) {
		printk("ERROR can not open GPIO %d\n", gpio_num);
		return status;
	}

  status = request_irq(gpio_to_irq(gpio_num), handler, 0, "gpio_test", NULL);
  if(status < 0) {
    printk(KERN_ERR "error %d requesting GPIO IRQ %d\n", status, gpio_num);
    return status;
  }
  //irq_set_irq_type(gpio_to_irq(gpio_num), IRQ_TYPE_EDGE_RISING);
  irq_set_irq_type(gpio_to_irq(gpio_num), IRQ_TYPE_EDGE_BOTH);

#ifdef TEST_OUTPUT
  gpio_direction_output(gpio_num, 0);
#else
  gpio_direction_input(gpio_num);
#endif

  if(gpio_get_value(gpio_num) == 0)
    printk("pin is OFF, Waiting for the pin to be ON\n");
  else
    printk("pin is ON, Waiting for the pin to be OFF\n");

#ifdef TEST_OUTPUT 
  for (i = 0; i < test_loop; i++) {
    toggle = !(toggle);
    gpio_set_value(gpio_num, toggle);
    if(gpio_get_value(gpio_num) == 0)
      printk("pin is OFF\n");
    else
      printk("pin is ON\n");
  }
#endif
	wait_for_completion_interruptible(&work);

	printk(".. done\n");

	return 0;
}

void cleanup_module(void) {
	
	gpio_free(gpio_num);

	free_irq(gpio_to_irq(gpio_num), NULL);
}

MODULE_LICENSE("GPL");
