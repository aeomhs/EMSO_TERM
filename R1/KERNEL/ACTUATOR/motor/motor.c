#include "motor.h"

MODULE_LICENSE("GPL");

int steps[STEPS][4] = {
	{1,0,0,0},
	{1,1,0,0},
	{0,1,0,0},
	{0,1,1,0},
	{0,0,1,0},
	{0,0,1,1},
	{0,0,0,1},
	{1,0,0,1}
};

void setStep(int step){
	gpio_set_value(PIN1, steps[step][0]);
    gpio_set_value(PIN2, steps[step][1]);
    gpio_set_value(PIN3, steps[step][2]);
    gpio_set_value(PIN4, steps[step][3]);
}

void forward(int round, int delay){
	int i=0;
	int j=0;
	for(i=1;i<=64*round;i++){
		for(j=1;j<=64;j++){//5.625도
			setStep(j%8);
			udelay(delay);
		}
	}

}

void moveDegree(int degree, int delay, int direction){
	int i=0;
	if(direction == 0){
		for(i=1;i<=(64*64*degree)/360;i++){//360도
			setStep(i%8);
			udelay(delay);
		}

	}else{
		for(i=1;i<=(64*64*degree)/360;i++){
			setStep(STEPS-i%8);
			udelay(delay);
		}	
	}

}

struct int motor_open(struct inode *inode, struct file *file){
	
	return 0;
}

struct int motor_release(struct inode *inode, struct file *file){
	
	return 0;
}

struct file_operations motor_fops = {
	.open = motor_open,
	.release = motor_release
};

static int __init simple_motor_init(void){
	gpio_request_one(PIN1, GPIOF_OUT_INIT_LOW, "p1");
	gpio_request_one(PIN2, GPIOF_OUT_INIT_LOW, "p2");
	gpio_request_one(PIN3, GPIOF_OUT_INIT_LOW, "p3");
	gpio_request_one(PIN4, GPIOF_OUT_INIT_LOW, "p4");

	alloc_chrdev_region(&dev_num, 0, 1, DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &motor_fops);
	cdev_add(cd_cdev, dev_num, 1);
	
	return 0;
}

static void __exit simple_motor_exit(void){
	gpio_free(PIN1);
	gpio_free(PIN2);
	gpio_free(PIN3);
	gpio_free(PIN4);
}

module_init(simple_motor_init);
module_exit(simple_motor_exit);