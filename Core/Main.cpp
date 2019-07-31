#include <fstream>
#include <iostream>
#include <boost/dll.hpp>

int main(int argCount, char **argValues)
{
    std::cout << boost::dll::program_location ().string ().c_str () << std::endl;
    return 0;
}
