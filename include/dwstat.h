#ifndef DWSTAT__HEADER
#define DWSTAT__HEADER


#include <linux/limits.h>



#define PERIOD_MSECS 500
#define MESSAGE_LENGTH 127
#define MAX_CONFIG_SIZE 2047

#define LOCAL_CONF "/.dstat.conf"

#define MAX_HOME_LENGTH ( PATH_MAX - sizeof( LOCAL_CONF ) )


#endif

