#include <iostream>
#include <bitset>
#include "Riscv_core.h"

using namespace std;

int main()
{
   int64_t x = -7846;
   int32_t z = x;
   std::cout << "x = " << std::bitset<64>(x)  << std::endl;
   std::cout << "z = " << std::bitset<64>(z)  << std::endl;
    return 0;
}
