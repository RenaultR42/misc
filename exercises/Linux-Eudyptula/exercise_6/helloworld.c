#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>

#define EUDYPTULA_ID "c9680074cfb9"
#define BUFFER_SIZE 1024

static ssize_t hello_write(struct file *file, const char __user *buf,
		       size_t len, loff_t *ppos)
{
	char tmp[BUFFER_SIZE];
	int size = strlen(EUDYPTULA_ID);

	if (simple_write_to_buffer(tmp, size, ppos, buf, len) < size)
		return -EINVAL;

	tmp[size] = '\0';
	return strncmp(tmp, EUDYPTULA_ID, size) ? -EINVAL : size;
}

static ssize_t hello_read(struct file *file, char __user *buf,
		       size_t len, loff_t *ppos)
{
	return simple_read_from_buffer(buf, len, ppos, EUDYPTULA_ID,
		strlen(EUDYPTULA_ID));
}

static const struct file_operations hello_fops = {
	.owner		= THIS_MODULE,
	.write		= hello_write,
	.read		= hello_read,
};

static struct miscdevice hello_device = {
	.minor =	MISC_DYNAMIC_MINOR,
	.name =		"eudyptula",
	.fops =		&hello_fops,
};

static int __init hello_init(void)
{
	int err;

	err = misc_register(&hello_device);
	if (err) {
		printk(KERN_ERR "Can not register char device driver: %d\n",
		       err);
		return err;
	}

	printk(KERN_DEBUG "Hello world.\n");
	return 0;
}

static void __exit hello_exit(void)
{
	misc_deregister(&hello_device);
	printk(KERN_DEBUG "Goodbye\n");
}

module_init(hello_init)
module_exit(hello_exit)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charles-Antoine Couret");
MODULE_DESCRIPTION("My first character device driver");
