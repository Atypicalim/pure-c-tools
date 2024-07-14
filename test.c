// #define TEST

#ifdef TEST
#include "test.h"
#else
#include "tools.h"
#endif


int main(int argc, char const *argv[])
{
    tools_error("TEST");
    //
    return 0;
}

