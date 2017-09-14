#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/jiffies.h>

#define DRIVER_AUTHOR	"Charles-Antoine Couret"
#define DRIVER_DESC	"My First module Eudyptula"
#define EUDYPTULA_ID	"c9680074cfb9"
#define BUFFER_SIZE	1024

static struct dentry *dir, *id_file, *jiffies_file, *foo_file;
static char id_value[BUFFER_SIZE], foo_value[PAGE_SIZE];

/* To avoid concurrent access. One request allowed in the same time. */
static DEFINE_MUTEX(debug_foo_mutex);

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

static ssize_t debug_foo_reader(struct file *fp, char __user *buf,
			size_t len, loff_t *ppos)
{
	int ret;
	mutex_lock(&debug_foo_mutex);

	ret = simple_read_from_buffer(buf, len, ppos, foo_value,
		strlen(foo_value));
	mutex_unlock(&debug_foo_mutex);
	return ret;
}

static ssize_t debug_foo_writer(struct file *fp, const char __user *buf,
			size_t len, loff_t *ppos)
{
	int ret;
	mutex_lock(&debug_foo_mutex);

	if (len >= BUFFER_SIZE)
		return -EINVAL;

	ret = simple_write_to_buffer(foo_value, PAGE_SIZE - 1, ppos, buf, len);

	foo_value[ret] = '\0';
	mutex_unlock(&debug_foo_mutex);
	return ret;
}

static const struct file_operations fops_foo = {
	.read = debug_foo_reader,
	.write = debug_foo_writer,
};

static int init_debug(void)
{
	dir = debugfs_create_dir("eudyptula", NULL);
	id_file = debugfs_create_file("id", 0666, dir, id_value, &fops_id);
	if (!id_file) {
		pr_err("Error creating id_file file");
		return -ENODEV;
	}

	jiffies_file = debugfs_create_u64("jiffies", 0444, dir, (u64 *)&jiffies);
	if (!jiffies_file) {
		pr_err("Error creating jiffies_file file");
		return -ENODEV;
	}

	foo_file = debugfs_create_file("foo", 0644, dir, foo_value, &fops_foo);
	if (!foo_file) {
		pr_err("Error creating foo_file file");
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
