#include <iostream>
#include <boost/dll/runtime_symbol_info.hpp>

int main()
{
    std::cout << boost::dll::program_location() << std::endl;
    auto p = boost::dll::program_location().parent_path() / "Makefile";
    std::cout << p << ": " << boost::filesystem::is_regular_file(p) << std::endl;
}
