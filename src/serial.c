#include "serial.h"

static comList comPorts;
static u16 usedPort = 0x0000;

static void Init()
{
    static bool isInitialized = false;
    if (isInitialized) return;
    comPorts[0] = 0;
    comPorts[1] = 0;
    comPorts[2] = 0;
    comPorts[3] = 0;
    isInitialized = true;
    comPorts[0] = com1;
    if (com1 == 0) return;
    comPorts[1] = com2;
    if (com2 == 0) return;
    comPorts[2] = com3;
    if (com3 == 0) return;
    comPorts[3] = com4;
}

static bool init_serial(u8 portNum) {
    Init();
    static bool isInitialized = false;
    if (isInitialized) return true;
    u16 port = 0x0000;
    usedPort = port;
    if (portNum == 0 || portNum > 4 || (port = comPorts[portNum -1]) == 0)
        return false;
    outb(port + 1, 0x00);    // Disable all interrupts
    outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00);    //                  (hi byte)
    outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(port + 2, 0xc7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0b);    // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1e);    // Set in loopback mode, test the serial chip
    
    // Test serial chip (send byte 0xAE and check if serial returns same byte)
    outb(port + 0, 0xae);
    
    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(port + 0) != 0xae) return false;
    
    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(port + 4, 0x0F);
    isInitialized = true;
    usedPort = port;
    return true;
}

void printComPorts() {
    Init();
    printStr("Available ports for serial kernel debugging :");
    u8 index = 0;
    while (index < 4 && comPorts[index] != 0) {
        printStr("\r\n\tCOM");
        printStr(int2str((u64)index + 1));
        printStr(" : port 0x");
        printStr(hex2strw(comPorts[index]));
        ++index;
    }
    if (index == 0) printStr(" None");
    printStr("\r\n");
}

bool initComPort(const u8 port) {
    Init();
    return init_serial(port);
}

bool serial_received() {
    Init();
    return (bool)(inb(usedPort + 5) & 1); // reads LSR:RBF
}
 
u8 read_serial() {
    Init();
    while (! serial_received()); // waits for serial read buffer 
    return inb(usedPort);
}

bool is_transmit_empty() {
    Init();
    return (bool)(inb(usedPort + 5) & 0x20); //reads LSR:THRE
}
 
void write_serial(u8 a) {
    Init();
    while (! is_transmit_empty()); // waits for room in serial write buffer
    outb(usedPort, a);
}

void pushStr2Serial(const u8 *str) {
    Init();
    u8 c;
    while((c = *str)) {
        write_serial(c);
        ++str;
    }
}
