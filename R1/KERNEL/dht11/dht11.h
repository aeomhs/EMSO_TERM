#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/cdev.h>

#define DEV_NAME "dht11"

#define MAX_TIMING 85
#define DHT11 21

/*
	function : dht11 on
*/
static void dht11_read(void);
/*
	function : dht11 repeat 
*/
static int dht11_open(struct inode *inode, struct file *file);