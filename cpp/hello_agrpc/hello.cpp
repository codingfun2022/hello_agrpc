#include <iostream>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/process/search_path.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

int main()
{
    std::cout << boost::dll::program_location() << std::endl;
    auto mkf = boost::dll::program_location().parent_path() / "Makefile";
    std::cout << mkf << ": " << boost::filesystem::is_regular_file(mkf) << std::endl;

    auto py_path = boost::process::search_path("python");
    std::cout << py_path << std::endl;

    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::cout << uuid << std::endl;
    std::cout << boost::uuids::to_string(uuid) << std::endl;
}
