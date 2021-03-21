#include <linux/init.h>
#include <linux/module.h>

static int hello_init(void){
	while(1){
		printk(KERN_ALERT "TEST: Hi World\n");
	}
	return 0;
}

static void hello_exit(void){
	printk(KERN_ALERT "TEST: Bye World\n");
}

module_init(hello_init);
module_exit(hello_exit);


