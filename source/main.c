#include <stdio.h>

#include <switch.h>

#define MODULE_HBL 347
#define PMC_PHYS_ADDR 0x7000E400

/*Shamelessly stolen from the HBL loader*/
/*These are required for applet applications since we're not using a loader*/
void __libnx_initheap(void)
{
    static char g_innerheap[0x20000];

    extern char* fake_heap_start;
    extern char* fake_heap_end;

    fake_heap_start = &g_innerheap[0];
    fake_heap_end   = &g_innerheap[sizeof g_innerheap];
}

void __appInit(void)
{
    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 1));

    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalSimple(MAKERESULT(MODULE_HBL, 2));

    fsdevInit();
}

int main(int argc, char **argv)
{
    /*Get the memory mapping for the PMC regs*/
    u64 virt_addr = 0;
    Result res = svcQueryIoMapping(&virt_addr, PMC_PHYS_ADDR, 0xC00);
    if(res)
        fatalSimple(MAKERESULT(222, 1337)); // error if we fail
    u64 scratch0 = virt_addr + 0x50; //Scratch register 0 is at PMC base + 0x50
    *(u32 *)scratch0 = 1 << 1; //Bit 1 set to 1 makes the SoC go into RCM on reboot
    *(u32 *)virt_addr |= 1 << 4; //reboot without clearing scratch 0
    while(1); //we should never reach here
}
