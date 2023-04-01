# NYCU-EOS-2023-Spring

## Lab4

BUILD
- `$ make RPI_build` to crosscompile the driver for RPi
- use `scp` command to copy the driver from virtualbox to Rpi
- `$ gcc writer.c -o writer` on RPi to compile writer.c to extuable(shared modules)
- `$ gcc reader.c -o reader` on RPi to compile writer.c to extuable(shared modules)
- :star: if its the first time using this program on RPi, make sure you `mknod` first, otherwise /dev/mydev won't appear in the /dev/ folder. 

DEMO (`indmod` and `rmmod` are included in demo.sh)
- `sudo python2 ./seg <port>` to open 16-segment on virtualbox (act as virtual device)
- '$ sudo ./demo` on RPi to demo lab4 (before demo, please check if the Name, ip, port are correct)
