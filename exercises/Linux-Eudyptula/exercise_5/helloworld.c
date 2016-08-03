#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/usb/input.h>
#include <linux/hid.h>

MODULE_AUTHOR("Couret Charles-Antoine");
MODULE_DESCRIPTION("Keyboard Driver Test");
MODULE_LICENSE("GPL");

static struct usb_device_id hello_id_table [] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
		       USB_INTERFACE_SUBCLASS_BOOT,
		       USB_INTERFACE_PROTOCOL_KEYBOARD) },
	{ } 
};
MODULE_DEVICE_TABLE (usb, hello_id_table);

static int __init hello_init(void)
{
	pr_debug("HelloModule: Hello World!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_debug("HelloModule: exit\n");
}

module_init(hello_init);
module_exit(hello_exit);
