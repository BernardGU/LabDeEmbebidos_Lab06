# LabDeEmbebidos_Lab06

Activity for Lab 06 of the Embedded Systems Lab.

## Part I

Part I is a GCC C Executable Project created in ATMEL Studio for the ATSAMD21G18A microcontroller.

It uses the internal Timer/Counter 3 of the device to generate a square wave of 57.25us that can be visualized in the BUILTIN_LED of the Arduino Zero. The source file for this project was translated by us from a provided ASM file to a C file.

## Part II

Part II is a GCC C Executable Project created in ATMEL Studio for the ATSAMD21G18A microcontroller.

It interfaces the Arduino Zero with an RTC module using I2C and FTDI module using UART. The main program configures both interfaces, initializes the RTC with a hard-coded date and time, and then queries continuously the time to send it to the serial interface that is read with a computer.

## Source Files
```
LabDeEmbebidosLab_06
├── PartI
│   └── PartI
│       ├── main.c
├── PartII
│   └── PartII
│       ├── main.c
│       ├── myprintf.c
│       ├── myprintf.h
│       ├── myprintf.c
│       ├── RTCControl.c
│       └── RTCControl.h
└── README.md
```
- `PartI` > `main.c` : Initializes the system, uses the data structures defined in the `sam.h` library file to configure the output pins and TC3 module. It then enters an infinite loop that continuously waits for the TC3 to finish, toggles an output pin and resets the TC3 count. This generates a square wave in the output pin.

- `PartII` > `main.c` : Initializes the system, and configures the I2C and UART interface of the device. It then initializes the RTC with a hard-coded date and time. Finally, it enters an infinite loop that continously reads the current RTC value and sends it through the UART serial interface. 

- `PartII` > `RTCControl.c` : Helper functions to control the I2C interface to talk to the RTC module.

- `PartII` > `RTCControl.h` : Header file of `RTCControl.c`. It includes helper structs to parse the data read from the RTC module.

- `PartII` > `myprintf.c` : Helper functions to print and format strings. (Provided by the lab instructor).

- `PartII` > `myprintf.h` : Header file of `myprintf.c`. (Provided by the lab instructor).

## Authors

* A01196694 - Eric Jordan Reyna [**EricJord4n**](https://github.com/EricJord4n)
* A01410922 - Gustavo De Los Ríos Alatorre [**GustavoDLRA**](https://github.com/GustavoDLRA)
* A01196934 - Bernardo Clemente Gutiérrez Cantú [**BernardGU**](https://github.com/BernardGU)
