# NYCU-EOS-2023-Spring

## Lab3
- led_driver.c is the driver for the leds connnected to GPIO pins

BUILD
- `$ sudo make`  to crosscompile the driver for RPi 
- use `scp` command to copy the kernel object file led_driver.ko from virtualbox to Rpi
  - Before executing `scp`, make sure 
    - **1. `$ rmmod led_driver` the driver module has removed**
    - **2. old led_driver.ko in RPi has deleted**
- `$ gcc writer.c -o writer` on RPi to compile writer.c to extuable(shared modules)
- `$ gcc reader.c -o reader` on RPi to compile writer.c to extuable(shared modules)

DEMO
- `$ insmod led_driver.ko` under the folder which has led_driver.ko in PIi

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
