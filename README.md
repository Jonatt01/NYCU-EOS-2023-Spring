# NYCU-EOS-2023-Spring

## Pre-test
- The codes inside Pre-test folder is not the test of 2023. These are sample questions from previous pre-test `questions/past_exams_1`.
- The pre-test questions of 2023 is in `questions/entrance_exam_2023`.

## Pre-lab
- This lab is only environment setting.

## Lab3
- led_driver.c is the driver for the leds connnected to GPIO pins

BUILD
- `$ sudo make`  to crosscompile the driver for RPi 
- use `scp` command to copy the kernel object file led_driver.ko from virtualbox to RPi
  - Before executing `scp`, make sure 
    - **1. `$ rmmod led_driver` the driver module has removed**
    - **2. old led_driver.ko in RPi has deleted**
- `$ gcc writer.c -o writer` on RPi to compile writer.c to extuable(shared modules)
- `$ gcc reader.c -o reader` on RPi to compile writer.c to extuable(shared modules)

DEMO
- `$ insmod led_driver.ko` under the folder which has led_driver.ko in RPi

REMINDER
- execute `make` under lab3/ folder, which is the same path as linux/ folder in virtualbox
```
|-Documents
   |-linux
   |-lab3
      |-leddriver.c
      |-Makefile
      |-writer.c
```

## Lab3-2
- Just replace the leds in Lab3 by 7th-segment, so we hace to use more GPIO pins than Lab3. (only driver need to be modified)
- Have to know how to use the common cathode 7-segment.

## Lab4
- mydev.c is the driver for virtual 16-segment.

BUILD
- `$ make RPI_build` to crosscompile the driver for RPi 
- use `scp` command to copy the kernel object file mydev.ko from virtualbox to Rpi
- `$ gcc writer.c -o writer` on RPi to compile writer.c to extuable(shared modules)
- `$ gcc reader.c -o reader` on RPi to compile writer.c to extuable(shared modules)
- :star: if its the first time using this program on RPi, make sure you `mknod` first, otherwise /dev/mydev won't appear in the /dev/ folder. 

DEMO (`indmod` and `rmmod` are included in demo.sh)
- :star: All the files has to put in the same folder.
- `$ sudo python2 ./seg <port>` to open 16-segment on virtualbox (act as virtual device)
- `$ sudo ./demo` on RPi to demo lab4 (before demo, please check if the Name, ip, port are correct)

## HW1
- hw1.c is the main function.
- seg_driver.c is the driver for 7 segment device.
- led_driver.c is the driver for leds.
- hw1.c pass an char array of three elements representing # each section waiting people to led_driver.c
- hw1.c pass an char array of one element which is the number to display.

BUILD
- `$ sudo make` to crosscompile the driver for RPi
  - Notice that we compiled source file to two kernel source file and add them to the obj-m list.
  - 他會自己做名字的對應
DEMO
- `$ insmod led_driver.ko` under the folder which has led_driver.ko in RPi
- `$ insmod seg_driver.ko` under the folder which has seg_driver.ko in RPi
- `$ sudo ./hw1` to run the main file for hw1.

## Lab5
- make sure you have installed "sl" and "tmux" already
  - command : `$ sudo apt install sl tmux`
- `crtl-b` and `:kill-session` to close the session, otherwise you will leave a zombie process.
- cpmmand for checking if there is any zombie process : `$ ps aux | grep defunct | grep -v grep`

BUILD
- We can make on the RPi directly because lab5.c is small enough.
- `$ make` in the `lab5` folder.
DEMO
- `sudo ./demo.sh` in the `lab5` folder.

## HW2
SOME POINT OF THIS LAB
- Key point for my method : Make use of a 9*3 table to save the information of waiting people of each area.
  - In that way, we can report in the correct order with minimum effort. Only need to traverse throught the table.
- Also you have to get familiar with `strtok` function.

DEMO
- `sudo ./hw2_checker 127.0.0.1 8080`

## Lab6
TO BE COMPLETE

## Lab7
TO BE COMPLETE

## HW3
- The code is inside the folder `311513025_eos_hw3`.
- The `somedata.txt` file is the category of testing data which I capture from checker.
- array_operation.c contains the function which deals with arrays. array_operation.h is the header file.
- create_socket.c is the program same as hw2 and some labs which need to use socket. Just modulization shorten the server and client code. create_socket.h is the header file.
- translate.c can do two categories of things. First, translate the string to index. Second, report the status to client when client enter command like `Report | Science Park | Adult 5 | Elder 6`.

DEMO
- `sudo ./hw3_checker 127.0.0.1 8888`

TODO
- 合併還沒做，但助教測資抓不出來 (小確幸^^)
