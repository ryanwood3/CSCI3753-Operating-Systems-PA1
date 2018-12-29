
#include<linux/uaccess.h>
#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<asm/uaccess.h>


#define BUFFER_SIZE 1024
#define DEVICE_NAME "simple_char_driver"

/* Define device_buffer and other global data structures you will need here */

static int major_number=240;
static char* device_buffer;



ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */

	if(length>BUFFER_SIZE-*offset)
		length=BUFFER_SIZE-*offset;

	if(copy_to_user(buffer,device_buffer+*offset,length))
		printk(KERN_ALERT "Error bytes uncopied\n");
	else
		printk(KERN_ALERT "%zd bytes read\n",length);

	*offset=*offset+length;

	return 0;
}



ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */

	if(length>BUFFER_SIZE-*offset)
		length=BUFFER_SIZE-*offset;

	if(copy_from_user(device_buffer+*offset,buffer,length))
		printk(KERN_ALERT "Error bytes uncopied\n");
	else
		printk(KERN_ALERT "Bytes Writen: %zd\n",length);
	
	*offset=*offset+length-1;

	return length;
}


int simple_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	static int open_count=0;
	open_count++;
	printk(KERN_ALERT "Device has been opened %d times\n",open_count);
	return 0;
}

int simple_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	static int close_count=0;
	close_count++;
	printk(KERN_ALERT "Device has been closed %d times\n",close_count);
	return 0;
}

loff_t simple_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t new_pos=0;
	switch(whence)
	{
	case 0:
		new_pos=offset;
		break;
	case 1:
		new_pos=pfile->f_pos+offset;
		break;
	case 2:
		new_pos=BUFFER_SIZE-offset;
		break;
	}

	if(new_pos>BUFFER_SIZE-1)
	{
		new_pos=pfile->f_pos;
		printk(KERN_ALERT "Error: Attempting to seek beyond end of file\n");
	}
	else if(new_pos<0)
	{
		new_pos=pfile->f_pos;
		printk(KERN_ALERT "Error: Attempting to seek before beginning of file\n");
	}
	pfile->f_pos=new_pos;
	return 0;
}

struct file_operations simple_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	.read=simple_char_driver_read,
	.write=simple_char_driver_write,
	.open=simple_char_driver_open,
	.release=simple_char_driver_close,
	.llseek=simple_char_driver_seek,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};

static int simple_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	/* register the device */
	device_buffer=kmalloc(BUFFER_SIZE,GFP_KERNEL);
	if(register_chrdev(major_number, DEVICE_NAME, &simple_char_driver_file_operations)<0)
	{
		printk(KERN_ALERT "Registering character device failed with %d\n", major_number);
    		return major_number;
	}

	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);	

	return 0;
}

static void simple_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	/* unregister  the device using the register_chrdev() function. */
	unregister_chrdev(major_number, DEVICE_NAME);
	printk(KERN_ALERT "inside %s funciton\n",__FUNCTION__);
	memset(device_buffer,'\0',BUFFER_SIZE);
	kfree(device_buffer);

}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);
