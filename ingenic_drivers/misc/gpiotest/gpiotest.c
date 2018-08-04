    #include <linux/init.h>  
    #include <linux/module.h>  
    #include <linux/kernel.h>  
    #include <linux/gpio.h>                 // Required for the GPIO functions  
    #include <linux/interrupt.h>            // Required for the IRQ code
    #include <linux/delay.h>
      
    MODULE_LICENSE("GPL");  
    MODULE_AUTHOR("Test");  
    MODULE_DESCRIPTION("A Button test driver");  
    MODULE_VERSION("0.1");  

    static int __init test_init(void){  
       int result = 0;  


       int i,j,pin;
       int err;
       printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST\n");

       for(i = 0; i < 4;i++){
           for(j = 3; j < 32;j++){
                pin = 32 * i + j;
                printk(KERN_INFO "Requesting Pin %d \n", pin);
                msleep(1000);
                if(
                pin == 13

                ){
                    printk(KERN_INFO "GPIO_TEST: Pin %d override\n", pin);
                    continue;
                }
                if(gpio_is_valid(pin)==true){
                    err = gpio_request(pin, "test");
                    if(err==0){
                        gpio_free(pin);
                    }else{
                        printk(KERN_INFO "GPIO_TEST: Pin %d is busy\n", pin);
                    }

                }

           }
       }

       return result;  
    }  
      
    /** @brief The LKM cleanup function 
    *  Similar to the initialization function, it is static. The __exit macro notifies that if this 
    *  code is used for a built-in driver (not a LKM) that this function is not required. Used to release the 
    *  GPIOs and display cleanup messages. 
    */  
    static void __exit test_exit(void){
       printk(KERN_INFO "GPIO_TEST: Goodbye!\n");  
    }  

      
    /// This next calls are  mandatory -- they identify the initialization function  
    /// and the cleanup function (as above).  
    module_init(test_init);  
    module_exit(test_exit);  