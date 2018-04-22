#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/jiffies.h>

#define DRIVER_AUTHOR	"Charles-Antoine Couret"
#define DRIVER_DESC	"My First module Eudyptula"
#define EUDYPTULA_ID	"c9680074cfb9"
#define BUFFER_SIZE	1024

static char foo_value[PAGE_SIZE];
static struct kobject *sysfs_kobject;

/* To avoid concurrent access. One request allowed in the same time. */
static DEFINE_MUTEX(debug_foo_mutex);

static ssize_t sysfs_id_reader(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s", EUDYPTULA_ID);
}

static ssize_t sysfs_id_writer(struct kobject *kobj,
				struct kobj_attribute *attr, const char *buf,
				size_t count)
{
	if (count != strlen(EUDYPTULA_ID))
		return -EINVAL;

	return strncmp(buf, EUDYPTULA_ID, count) ? -EINVAL : count;
}

static struct kobj_attribute id_attributes = __ATTR(id, 0644, sysfs_id_reader,
							sysfs_id_writer);

static ssize_t sysfs_jiffies_reader(struct kobject *kobj,
					struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%ld", jiffies);
}

static struct kobj_attribute jiffies_attributes = __ATTR(jiffies, 0444,
						sysfs_jiffies_reader, NULL);

static ssize_t sysfs_foo_reader(struct kobject *kobj,
				struct kobj_attribute *attr, char *buf)
{
	int ret;

	mutex_lock(&debug_foo_mutex);
	ret = sprintf(buf, "%s", foo_value);
	mutex_unlock(&debug_foo_mutex);

	return ret;
}

static ssize_t sysfs_foo_writer(struct kobject *kobj,
				struct kobj_attribute *attr, const char *buf,
				size_t count)
{
	int ret;

	if (count >= PAGE_SIZE)
		return -EINVAL;

	mutex_lock(&debug_foo_mutex);
	ret = snprintf(foo_value, PAGE_SIZE - 1, "%s", buf);
	mutex_unlock(&debug_foo_mutex);

	return ret;
}

static struct kobj_attribute foo_attributes = __ATTR(foo, 0644,
						sysfs_foo_reader,
						sysfs_foo_writer);

static int init_sysfs(void)
{
	sysfs_kobject = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!sysfs_kobject) {
		pr_err("Error creating sysfs_kobject");
		return -ENOMEM;
	}

	if (sysfs_create_file(sysfs_kobject, &id_attributes.attr)) {
		pr_err("Error creating id_file file");
		goto error;
	}

	if (sysfs_create_file(sysfs_kobject, &jiffies_attributes.attr)) {
		pr_err("Error creating jiffies_file file");
		goto error;
	}

	if (sysfs_create_file(sysfs_kobject, &foo_attributes.attr)) {
		pr_err("Error creating foo_file file");
		goto error;
	}

	return 0;

error:
	kobject_put(sysfs_kobject);
	return -ENOMEM;
}

static void cleanup_sysfs(void)
{
	sysfs_remove_file(sysfs_kobject, &id_attributes.attr);
	sysfs_remove_file(sysfs_kobject, &jiffies_attributes.attr);
	sysfs_remove_file(sysfs_kobject, &foo_attributes.attr);

	kobject_put(sysfs_kobject);
}

module_init(init_sysfs);
module_exit(cleanup_sysfs);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

