#include <sys/unistd.h>
#include <errno.h> // required for errno
#include "mcu.h" // required for __get_MSP

// copied from https://sites.google.com/site/stm32discovery/open-source-development-with-the-stm32-discovery/getting-newlib-to-work-with-stm32-and-code-sourcery-lite-eabi
caddr_t _sbrk(int incr)
{
    extern char _ebss; // defined by the linker
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_ebss;
    }
    prev_heap_end = heap_end;

    char * stack = (char*) __get_MSP();
    if (heap_end + incr >  stack)
    {
        errno = ENOMEM;
        return  (caddr_t) -1;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}
