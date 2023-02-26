#include <iostream>
#include <bitset>
#include "Riscv_core.h"

using namespace std;

int main()
{
   uint32_t x = 1;
   int32_t z = -8>>x;
   std::cout << "x = " << std::bitset<32>(x)  << std::endl;
   std::cout << "z = " << z  << std::endl;
    return 0;
}
