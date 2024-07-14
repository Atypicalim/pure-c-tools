// #define TEST

#ifdef TEST
#include "test.h"
#else
#include "tools.h"
#endif

int main(int argc, char const *argv[])
{
    //
    printf("tools...\n");
    pct_tools_warning("TEST");
    //
    return 0;
}

