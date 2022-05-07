#include <iostream>
#include <boost/dll/runtime_symbol_info.hpp>
#include <boost/process/search_path.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

void test_boost()
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

void test_stl()
{
    std::vector<std::string> v1{"a", "b", "c"};
    std::vector<std::string> v2{"b", "a", "a"};
    std::unordered_set<std::string> s1(v1.begin(), v1.end());
    std::unordered_set<std::string> s2(v2.begin(), v2.end());
    std::cout << (s1 == s2) << std::endl;
}

int main()
{
    test_stl();
}
