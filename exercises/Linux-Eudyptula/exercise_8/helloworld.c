#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/fs.h>

#define DRIVER_AUTHOR	"Charles-Antoine Couret"
#define DRIVER_DESC	"My First module Eudyptula"
#define EUDYPTULA_ID	"c9680074cfb9"
#define BUFFER_SIZE	1024

static struct dentry *dir, *id_file;
static char id_value[BUFFER_SIZE];

static ssize_t debug_id_reader(struct file *fp, char __user *buf,
			size_t len, loff_t *ppos)
{
	return simple_read_from_buffer(buf, len, ppos, EUDYPTULA_ID,
		strlen(EUDYPTULA_ID));
}

static ssize_t debug_id_writer(struct file *fp, const char __user *buf,
			size_t len, loff_t *ppos)
{
	char tmp[BUFFER_SIZE];
	int size = strlen(EUDYPTULA_ID);

	if (len >= BUFFER_SIZE)
		return -EINVAL;

	if (simple_write_to_buffer(tmp, size, ppos, buf, len) < size)
		return -EINVAL;

	tmp[size] = '\0';
	return strncmp(tmp, EUDYPTULA_ID, size) ? -EINVAL : size;
}

static const struct file_operations fops_id = {
	.read = debug_id_reader,
	.write = debug_id_writer,
};

static int init_debug(void)
{
	dir = debugfs_create_dir("eudyptula", NULL);
	id_file = debugfs_create_file("id", 0666, dir, id_value, &fops_id);
	if (!id_file) {
		pr_err("Error creating id_file file");
		return -ENODEV;
	}

	return 0;
}

static void cleanup_debug(void)
{
	debugfs_remove_recursive(dir);
}

module_init(init_debug);
module_exit(cleanup_debug);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
