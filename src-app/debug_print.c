#include "debug_print.h"

#if defined(DEBUG_PRINT)

extern int debug_writebuff(uint8_t* buf, int len);

extern uint8_t cer_buff_3[CEREAL_BUFFER_SIZE];

int dbg_printf(const char* fmt, ...)
{
    char* loc_buf = (char*)cer_buff_3;
    char * temp = loc_buf;
    va_list arg;
    va_list copy;
    va_start(arg, fmt);
    va_copy(copy, arg);
    int len = vsnprintf(temp, CEREAL_BUFFER_SIZE, fmt, copy);
    va_end(copy);
    if (len < 0) {
        va_end(arg);
        return 0;
    }
    if (len >= CEREAL_BUFFER_SIZE) {
        #if 0
        temp = (char*) malloc(len+1);
        if(temp == NULL) {
            va_end(arg);
            return 0;
        }
        len = vsnprintf(temp, len+1, fmt, arg);
        #else
        va_end(arg);
        return 0;
        #endif
    }
    va_end(arg);
    len = debug_writebuff((uint8_t*)temp, len);
    #if 0
    if (temp != loc_buf){
        free(temp);
    }
    #endif
    return len;
}

#endif