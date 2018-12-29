Ryan Wood
rywo8554@colorado.edu
Student ID: 102236570

/lib/modules/Makefile
Contains information telling the compiler to make a module object named simple_char_device.o

/lib/modules/simple_char_driver.c
Contains code for my device driver kernel module

/home/user/Dropbox/CSCI3753/PA2/testapp.c
Contains code for interactive program used to test device driver

/home/user/Dropbox/CSCI3753/PA2/README.txt

Once simple_char_driver.c has been written.  Use the following command:
sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules
The kernel module can then be inserted into the kernel using:
sudo insmod simple_char_driver.ko
It can then be removed using:
sudo rmmod simple_char_driver

