#include<i686/io.h>

#define UNUSEDPORT 0x80

void i686_iowait(){
    i686_outb(UNUSEDPORT, 0);
    /**
     * Why output to the unused port 0x80?
     * Usually, I/O operations are slow compared to CPU operations. The CPU has to wait for the I/O operations to complete before it can continue executing the next instruction.
     * Sometimes we need the CPU to wait for the I/O operations to complete before it can continue executing the next instruction. The i686_iowait function is used to wait for the I/O operations to complete.
     * The i686_iowait function writes to the unused port 0x80. Writing to the unused port 0x80 is a way to wait for the I/O operations to complete. The CPU waits for the I/O operations to complete before it can continue executing the next instruction.
     */
}