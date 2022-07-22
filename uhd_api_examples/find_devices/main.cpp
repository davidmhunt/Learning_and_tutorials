//including c-standard libraries
#include <iostream>
#include <cstdlib>

//including uhd specific libraries
#include "uhd/types/device_addr.hpp"
#include "uhd/device.hpp"

int main(){
    uhd::device_addr_t hint;
    uhd::device_addrs_t dev_addrs = uhd::device::find(hint);
    std::cout << "Number of devices found: " << dev_addrs.size() << "\n";
    return EXIT_SUCCESS;
}
