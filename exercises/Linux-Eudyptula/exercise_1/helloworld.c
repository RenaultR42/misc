#include <linux/module.h>
#include <linux/kernel.h>

#define DRIVER_AUTHOR	"Charles-Antoine Couret"
#define DRIVER_DESC	"My First module Eudyptula"

int init_module(void)
{
	printk(KERN_DEBUG "Hello world.\n");
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_DEBUG "Goodbye\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
