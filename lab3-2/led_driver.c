/*
******************************************************************************
* \file led_driver.c
* \details Simple GPIO driver explanation
* \author EmbeTronicX
* \Tested with Linux raspberrypi 5.4.51-v7l+
******************************************************************************
*/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/gpio.h> //GPIO

//LED is connected to this GPIO
#define GPIO_21 (21)
#define GPIO_14 (14) //LSB a
#define GPIO_15 (15) // b
#define GPIO_18 (18) // c
#define GPIO_23 (23) // d
#define GPIO_24 (24) // e
#define GPIO_25 (25) // f
#define GPIO_8 (8) // g


dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp,
char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp,
const char *buf, size_t len, loff_t * off);
/******************************************************/

//File operation structure
static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .read = etx_read,
    .write = etx_write,
    .open = etx_open,
    .release = etx_release,
};
/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file){
    pr_info("Device File Opened...!!!\n");
    return 0;
}
/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file){
    pr_info("Device File Closed...!!!\n");
    return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
    uint8_t gpio_state = 0;

    //reading GPIO value
    gpio_state = gpio_get_value(GPIO_21);

    //write to user
    len = 1;
    if( copy_to_user(buf, &gpio_state, len) > 0) {
        pr_err("ERROR: Not all the bytes have been copied to user\n");
    }
    pr_info("Read function : GPIO_21 = %d \n", gpio_state);
    return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){
    uint8_t rec_buf[10] = {0};
    int num[10][7]={
        {1,1,1,1,1,1,0}, // 0
        {0,1,1,0,0,0,0}, // 1
        {1,1,0,1,1,0,1}, // 2
        {1,1,1,1,0,0,1}, // 3
        {0,1,1,0,0,1,1}, // 4
        {1,0,1,1,0,1,1}, // 5
        {1,0,1,1,1,1,1}, // 6
        {1,1,1,0,0,0,0}, // 7
        {1,1,1,1,1,1,1}, // 8
        {1,1,1,1,0,1,1}, // 9
    };

    if( copy_from_user( rec_buf, buf, len ) > 0) {
    pr_err("ERROR: Not all the bytes have been copied from user\n");
    }

    int i=0;
    // Because for loop initial declarations are only allowed in C99 or C11 mode
    while(i<9){
        pr_info("%d number\n",i);
        int j=0;
        while(j<9){
            pr_info("DDDD This number is %d\n", rec_buf[j]-'0');
            pr_info("CCCC This number is %c\n", rec_buf[j]);
            j++;
        }

        gpio_set_value(GPIO_14,num[rec_buf[i]-'0'][0]);
        pr_info("a: %d",num[rec_buf[i]-'0'][0]);
        gpio_set_value(GPIO_15,num[rec_buf[i]-'0'][1]);
        pr_info("b: %d",num[rec_buf[i]-'0'][1]);
        gpio_set_value(GPIO_18,num[rec_buf[i]-'0'][2]);
        pr_info("c: %d",num[rec_buf[i]-'0'][2]);
        gpio_set_value(GPIO_23,num[rec_buf[i]-'0'][3]);
        pr_info("d: %d",num[rec_buf[i]-'0'][3]);
        gpio_set_value(GPIO_24,num[rec_buf[i]-'0'][4]);
        pr_info("e: %d",num[rec_buf[i]-'0'][4]);
        gpio_set_value(GPIO_25,num[rec_buf[i]-'0'][5]);
        pr_info("f: %d",num[rec_buf[i]-'0'][5]);
        gpio_set_value(GPIO_8,num[rec_buf[i]-'0'][6]);
        pr_info("g: %d",num[rec_buf[i]-'0'][6]);

        fsleep(1000000);

        i++;
    }


    // pr_info("Write Function : GPIO_21 Set = %c\n", rec_buf[0]);
    // for(int i = 0;i<len;i++){
    //     printk()
    // }
    // if (rec_buf[0]=='1') {
    // //set the GPIO value to HIGH
    // gpio_set_value(GPIO_21, 1);
    // } else if (rec_buf[0]=='0') {
    // //set the GPIO value to LOW
    // gpio_set_value(GPIO_21, 0);
    // } else {
    // pr_err("Unknown command : Please provide either 1 or 0 \n");
    // }
    return len;
}
/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
        pr_err("Cannot allocate major number\n");
        goto r_unreg;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&etx_cdev,&fops);

    /*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        pr_err("Cannot add the device to the system\n");
        goto r_del;
    }
    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        pr_err("Cannot create the struct class\n");
        goto r_class;
    }
    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        pr_err( "Cannot create the Device \n");
        goto r_device;
    }
    //Checking the GPIO is valid or not
    if(gpio_is_valid(GPIO_21) == false){
        pr_err("GPIO %d is not valid\n", GPIO_21);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_14) == false){
        pr_err("GPIO %d is not valid\n", GPIO_14);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_15) == false){
        pr_err("GPIO %d is not valid\n", GPIO_15);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_18) == false){
        pr_err("GPIO %d is not valid\n", GPIO_18);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_23) == false){
        pr_err("GPIO %d is not valid\n", GPIO_23);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_24) == false){
        pr_err("GPIO %d is not valid\n", GPIO_24);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_25) == false){
        pr_err("GPIO %d is not valid\n", GPIO_25);
        goto r_device;
    }
    if(gpio_is_valid(GPIO_8) == false){
        pr_err("GPIO %d is not valid\n", GPIO_8);
        goto r_device;
    }

    //Requesting the GPIO
    if(gpio_request(GPIO_21,"GPIO_21") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_21);
        goto r_gpio;
    }
    if(gpio_request(GPIO_14,"GPIO_14") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_14);
        goto r_gpio;
    }
    if(gpio_request(GPIO_15,"GPIO_15") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_15);
        goto r_gpio;
    }
    if(gpio_request(GPIO_18,"GPIO_18") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_18);
        goto r_gpio;
    }
    if(gpio_request(GPIO_23,"GPIO_23") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_23);
        goto r_gpio;
    }
    if(gpio_request(GPIO_24,"GPIO_24") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_24);
        goto r_gpio;
    }
    if(gpio_request(GPIO_25,"GPIO_25") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_25);
        goto r_gpio;
    }
    if(gpio_request(GPIO_8,"GPIO_8") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_8);
        goto r_gpio;
    }
    //configure the GPIO as output
    gpio_direction_output(GPIO_21, 0);
    gpio_direction_output(GPIO_14, 0);
    gpio_direction_output(GPIO_15, 0);
    gpio_direction_output(GPIO_18, 0);
    gpio_direction_output(GPIO_23, 0);
    gpio_direction_output(GPIO_24, 0);
    gpio_direction_output(GPIO_25, 0);
    gpio_direction_output(GPIO_8, 0);

    /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
    ** Now you can change the gpio values by using below commands also.
    ** echo 1 > /sys/class/gpio/gpio21/value (turn ON the LED)
    ** echo 0 > /sys/class/gpio/gpio21/value (turn OFF the LED)
    ** cat /sys/class/gpio/gpio21/value (read the value LED)
    **
    ** the second argument prevents the direction from being changed.
    */
    gpio_export(GPIO_21, false);
    gpio_export(GPIO_14, false);
    gpio_export(GPIO_15, false);
    gpio_export(GPIO_18, false);
    gpio_export(GPIO_23, false);
    gpio_export(GPIO_24, false);
    gpio_export(GPIO_25, false);
    gpio_export(GPIO_8, false);
    pr_info("Device Driver Insert...Done!!!\n");

    return 0;

    r_gpio:
        gpio_free(GPIO_21);
        gpio_free(GPIO_14);
        gpio_free(GPIO_15);
        gpio_free(GPIO_18);
        gpio_free(GPIO_23);
        gpio_free(GPIO_24);
        gpio_free(GPIO_25);
        gpio_free(GPIO_8);
    r_device:
        device_destroy(dev_class,dev);
    r_class:
        class_destroy(dev_class);
    r_del:
        cdev_del(&etx_cdev);
    r_unreg:
        unregister_chrdev_region(dev,1);
    return -1;
}
/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
    gpio_unexport(GPIO_21);
    gpio_free(GPIO_21);
    gpio_unexport(GPIO_14);
    gpio_free(GPIO_14);
    gpio_unexport(GPIO_15);
    gpio_free(GPIO_15);
    gpio_unexport(GPIO_18);
    gpio_free(GPIO_18);
    gpio_unexport(GPIO_23);
    gpio_free(GPIO_23);
    gpio_unexport(GPIO_24);
    gpio_free(GPIO_24);
    gpio_unexport(GPIO_25);
    gpio_free(GPIO_25);
    gpio_unexport(GPIO_8);
    gpio_free(GPIO_8);

    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");
