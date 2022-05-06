#include <iostream>
#include <boost/dll/runtime_symbol_info.hpp>

int main()
{
    std::cout << boost::dll::program_location() << std::endl;
}
