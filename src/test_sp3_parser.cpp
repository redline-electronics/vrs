#include <parsers/sp3.h>
#include <spdlog/spdlog.h>

#include <cstdio>

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    auto sp3 = parsers::SP3("/Users/ryan/Desktop/rtk/zoa2061/igr20950.sp3");
    (void)sp3;

    SPDLOG_INFO("Done!");
}
