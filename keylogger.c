#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/uaccess.h> 
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <linux/buffer_head.h>

#include <linux/module.h>
#include <linux/file.h>
#include <linux/tty.h>

MODULE_DESCRIPTION("KBD");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");

#define MODULE_NAME		"kbd"

#define KBD_MAJOR		42
#define KBD_MINOR		0
#define KBD_NR_MINORS		1

#define I8042_KBD_IRQ		1
#define I8042_STATUS_REG	0x64
#define I8042_DATA_REG		0x60

#define BUFFER_SIZE		1024
#define SCANCODE_RELEASED_MASK	0x80

struct kbd {
	struct cdev cdev;
	/* TODO 3: add spinlock */
	char buf[BUFFER_SIZE];
	struct class *dev_class;
	dev_t dev;
	size_t put_idx, get_idx, count;
	spinlock_t lock;
} devs[1];

/*
 * Checks if scancode corresponds to key press or release.
 */
static int is_key_press(unsigned int scancode)
{
	return !(scancode & SCANCODE_RELEASED_MASK);
}

/*
 * Return the character of the given scancode.
 * Only works for alphanumeric/space/enter; returns '?' for other
 * characters.
 */

// enum Key { Backspace = 8, Alt = 18, Control = 17, Shift = 16, PrintScreen = 44, Caps = 20 };

static int get_ascii(unsigned int scancode)
{
	static char* row1 = "1234567890";
	static char* row2 = "qwertyuiop";
	static char* row3 = "asdfghjkl";
	static char* row4 = "zxcvbnm";

	scancode &= ~SCANCODE_RELEASED_MASK;
	if(scancode >= 0x02 && scancode <= 0x0b)
		return *(row1 + scancode - 0x02);
	else if(scancode >= 0x10 && scancode <= 0x19)
		return *(row2 + scancode - 0x10);
	else if(scancode >= 0x1e && scancode <= 0x26)
		return *(row3 + scancode - 0x1e);
	else if(scancode >= 0x2c && scancode <= 0x32)
		return *(row4 + scancode - 0x2c);
	else if(scancode == 0x39)
		return ' ';
	else if(scancode == 0x0e) //backspace
		return '<';
	else if(scancode == 0x33 || scancode == 0xb3)
		return ',';
	else if(scancode == 0x29 || scancode == 0xa9)
		return '`';
	else if(scancode == 0x27 || scancode == 0xa7 || scancode == 0x4c)
		return ';';
	else if(scancode == 0x5b || scancode == 0xf0 || scancode == 0x1b)
		return ']';
	else if(scancode == 0x1a || scancode == 0x9a || scancode == 0x54)
		return '[';
	else if(scancode == 0x0d || scancode == 0x8d || scancode == 0x55)
		return '=';
	else if(scancode == 0x35 || scancode == 0xb5 || scancode == 0x4a)
		return '/';
	else if(scancode == 0x1d || scancode == 0x9d || scancode == 0x14) //ctrl
		return 'C';
	else if(scancode == 0x38 || scancode == 0xb8|| scancode == 0x11) //alt
		return 'A';
	else if(scancode == 0x3a || scancode == 0xba|| scancode == 0x58) //caps
		return 'C';
	else if(scancode == 0x0f || scancode == 0x8f || scancode == 0x0d) //tab
		return 'T';
	else if(scancode == 0x01 || scancode == 0x81 || scancode == 0x76) //escape
		return 'E';
	else if(scancode == 0x28 || scancode == 0xa8) //apostrof
		return '\'';
	else if(scancode == 0x4e || scancode == 0xce || scancode == 0x79) //plus
		return '+';
	else if(scancode == 0x0c || scancode == 0x4a || scancode == 0x8c || scancode == 0xca || scancode == 0x4e || scancode == 0x7b) //minus
		return '-';
	else if(scancode == 0x34 || scancode == 0x53 || scancode == 0xd3 || scancode == 0xb4 || scancode == 0x49 || scancode == 0x71) //punct
		return '.';
	else if(scancode == 0x37|| scancode == 0xb7 || scancode == 0xaa || scancode == 0x12 || scancode == 0x7c) //print screen
		return 'P'; 
	else if(scancode == 0x1c)
		return '\n';
	else if(scancode == 0x48 || scancode == 0x75) //up arrow
		return '^';
	else if(scancode == 0x50 || scancode == 0x72) //down arrow
		return 'V';
	else if(scancode == 0x36 || scancode == 0x12 || scancode == 0x59 || scancode == 0x2a) //shift
		return 'S';
	else
		return '?';
}


static void put_char(struct kbd* data, char c)
{
	if (data->count >= BUFFER_SIZE)
		return;

	data->buf[data->put_idx] = c;
	data->put_idx = (data->put_idx + 1) % BUFFER_SIZE;
	data->count++;
}

static bool get_char(char* c, struct kbd* data)
{
	/* TODO 4: get char from buffer; update count and get_idx */
	if (!data->count)
		return false;

	*c = data->buf[data->get_idx];
	--data->count;

	++data->get_idx;
	if (data->get_idx >= BUFFER_SIZE)
	{
		data->get_idx = 0;
	}
	else
	{
		data->get_idx = data->get_idx;
	}

	return true;
}


static void reset_buffer(struct kbd* data)
{
	/* TODO 5: reset count, put_idx, get_idx */
	data->count = 0;
	data->get_idx = 0;
	data->put_idx = 0;
}

/* TODO 2: implement interrupt handler */
	/* TODO 3: read the scancode */
	/* TODO 3: interpret the scancode */
	/* TODO 3: display information about the keystrokes */
	/* TODO 3: store ASCII key to buffer */
static irqreturn_t kbd_interrupt_handler(int irq_no, void* arg)
{
	ulong flags;
	u8 scancode = inb(I8042_DATA_REG);
	int pressed = is_key_press(scancode);
	char key = get_ascii(scancode);

	pr_info("IRQ: %d, scancode = 0x%X (%u) pressed=%d ch=%c\n", irq_no, scancode, scancode, pressed, key);

	if(pressed)
	{
		struct kbd* data = (struct kbd*)arg;
		spin_lock_irqsave(&data->lock, flags);
		put_char(data, key);
		spin_unlock_irqrestore(&data->lock, flags);
	}

	return IRQ_NONE;
}

static int kbd_open(struct inode* inode, struct file* file)
{
	struct kbd* data = container_of(inode->i_cdev, struct kbd, cdev);

	file->private_data = data;
	pr_info("%s opened\n", MODULE_NAME);
	return 0;
}

static int kbd_release(struct inode* inode, struct file* file)
{
	pr_info("%s closed\n", MODULE_NAME);
	return 0;
}

/* TODO 5: add write operation and reset the buffer */
static ssize_t kbd_write(struct file* file, const char __user* user_buffer, size_t size, loff_t* offset)
{
	struct kbd *data = (struct kbd *) file->private_data;
	unsigned long flags;

	spin_lock_irqsave(&data->lock, flags);
	reset_buffer(data);
	spin_unlock_irqrestore(&data->lock, flags);

	return size;
}

static ssize_t kbd_read(struct file* file, char __user* user_buffer, size_t size, loff_t* offset)
{
	struct kbd *data = (struct kbd *) file->private_data;
	size_t read = 0;
	/* TODO 4/18: read data from buffer */

	char c;
	unsigned long flags;
	bool ismore = true;

	while(size)
	{
		spin_lock_irqsave(&data->lock, flags);
		ismore = get_char(&c,data);
		spin_unlock_irqrestore(&data->lock, flags);

		if(ismore == false)
			break;

		if(put_user(c, user_buffer++))
			return -EFAULT;

		read++;
		size--;
	}

	return read;
}

static const struct file_operations kbd_fops = {
	.owner = THIS_MODULE,
	.open = kbd_open,
	.release = kbd_release,
	.read = kbd_read,
	/* TODO 5: add write operation */
	.write = kbd_write
};

static int __init  kbd_init(void)
{
	int err;
	struct resource* res;

	devs[0].dev=0;

	err =alloc_chrdev_region(&devs[0].dev , 0, 1, "myDevice");
	if (err != 0) {
		pr_err("register_region failed: %d\n", err);
		goto out;
	}

 /*Creating struct class*/
        if(IS_ERR(devs[0].dev_class = class_create(THIS_MODULE,"myDevice"))){
            pr_err("Cannot create the struct class\n");
            goto out_unregister;
        }

        /*Creating device*/
        if(IS_ERR(device_create(devs[0].dev_class,NULL,devs[0].dev,NULL,"myDevice"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }


	/* TODO 1/8: request the keyboard I/O ports */
	
    res = request_region(I8042_DATA_REG+1,1,MODULE_NAME);
    if(res == NULL)
    {
        err = -EBUSY;
        goto out_unregister ;
    }

    res = request_region(I8042_STATUS_REG+1,1,MODULE_NAME);
    if(res == NULL)
    {
        err = -EBUSY;
        goto out_unregister;
    }

	/* TODO 3: initialize spinlock */
	spin_lock_init(&devs->lock);

	/* TODO 2: Register IRQ handler for keyboard IRQ (IRQ 1). */
	err = request_irq(I8042_KBD_IRQ, kbd_interrupt_handler, IRQF_SHARED, MODULE_NAME, devs);
	if (err)
	{
		pr_err("IRQ request failed\n");
		goto out_release_data_reg;
	}

	cdev_init(&devs[0].cdev, &kbd_fops);
	cdev_add(&devs[0].cdev, devs[0].dev, 1);


	pr_notice("Driver %s loaded\n", MODULE_NAME);
	return 0;

	/*TODO 2: release regions in case of error */
out_release_data_reg:
	release_region(I8042_DATA_REG, 1);

out_unregister:
	unregister_chrdev_region(MKDEV(KBD_MAJOR, KBD_MINOR),KBD_NR_MINORS);

r_device:
        class_destroy(devs[0].dev_class);

out:
	return err;
}

static void __exit  kbd_exit(void)
{
	cdev_del(&devs[0].cdev);

	device_destroy(devs[0].dev_class,devs[0].dev );
    class_destroy(devs[0].dev_class);
    unregister_chrdev_region(devs[0].dev , 1);

	/* TODO 2: Free IRQ. */
	free_irq(I8042_KBD_IRQ - 1, devs);

	/* TODO 1: release keyboard I/O ports */
	release_region(I8042_DATA_REG, 1);
	release_region(I8042_STATUS_REG, 1);

	unregister_chrdev_region(MKDEV(KBD_MAJOR, KBD_MINOR),KBD_NR_MINORS);
	pr_notice("Driver %s unloaded\n", MODULE_NAME);
}

module_init(kbd_init);
module_exit(kbd_exit);
