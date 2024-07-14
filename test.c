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
    tools_log("TEST");
    tools_warn("TEST");
    tools_error("TEST");
    //
    return 0;
}

