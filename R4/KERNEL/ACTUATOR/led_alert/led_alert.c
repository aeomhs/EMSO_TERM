/*
 * 20190524 aeomhs
 * led_alert.c
 * */

#include "led_alert.h"


MODULE_LICENSE("GPL");

static struct timer_list timer;
static struct timer_list alert;
static dev_t dev_num;
static struct cdev *cd_cdev;


static void timer_func(unsigned long data){
    printk("led alert down\n");
    led_down();
}

void led_down(void){
	gpio_set_value(LED_R, 0);
	gpio_set_value(LED_G, 0);
	gpio_set_value(LED_B, 0);
	del_timer(&alert);
}

static void alert_led(unsigned long data){

	alert.data = data + 1;
	alert.expires = jiffies + (HZ);
	
	if(alert.data%2 == 0){
		gpio_set_value(LED_R, 1);
		gpio_set_value(LED_G, 0);
		gpio_set_value(LED_B, 1);
	}
	else{
		gpio_set_value(LED_R, 0);
        gpio_set_value(LED_G, 1);
        gpio_set_value(LED_B, 0);
	}
	add_timer(&alert);
}



static long led_alert_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
        switch( cmd ){
        case TURN_ON_LEDA:
                printk("turn on led alert\n");
                led_down();
                
                gpio_set_value(LED_G, 1);
                
                alert.function = alert_led;
                alert.data = 1L;
                alert.expires = jiffies + (HZ);
                add_timer(&alert);
                
                break;
        case TURN_OFF_LEDA:
                printk("turn off led alert\n");
                led_down();
                break;
        case TIME_LEDA:
                printk("turn on led alert until expire timer : %ld\n", arg);
                led_down();

                /* timer */
                del_timer(&timer);
                timer.function = timer_func;
	            timer.data = 1L;
	            timer.expires = jiffies + (arg*HZ);

                /* alert led on */
	            gpio_set_value(LED_G, 1);

	            alert.function = alert_led;
                alert.data = 1L;
                alert.expires = jiffies + (HZ);
                add_timer(&alert);
	            
	            add_timer(&timer);
                break;
        default:
                return -1;
        }

        return 0;
}

struct file_operations led_alert_fops = {
	.unlocked_ioctl = led_alert_ioctl,
};

static int __init led_alert_init(void){
    /* timer init */
    init_timer(&timer);
    init_timer(&alert);

    /* gpio init */
    gpio_request_one(LED_R, GPIOF_OUT_INIT_LOW, "LED_R");
    gpio_request_one(LED_G, GPIOF_OUT_INIT_LOW, "LED_G");
    gpio_request_one(LED_B, GPIOF_OUT_INIT_LOW, "LED_B");

    /* cdev init */
    alloc_chrdev_region(&dev_num, 0, 1, DEV_LED_ALERT);
    cd_cdev = cdev_alloc();
    cdev_init(cd_cdev, &led_alert_fops);
    cdev_add(cd_cdev, dev_num, 1);
	
    return 0;
}

static void __exit led_alert_exit(void){
    /* timer bye */
   	del_timer(&timer);
   	del_timer(&alert);
   	
    /* cdev free */
    cdev_del(cd_cdev);
    unregister_chrdev_region(dev_num, 1);

    /* gpio free */
    led_down();
    gpio_free(LED_R);
    gpio_free(LED_G);
    gpio_free(LED_B);
}

module_init(led_alert_init);
module_exit(led_alert_exit);
