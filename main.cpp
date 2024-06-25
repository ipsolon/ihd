/*
* Copyright 2024 Ipsolon Research
*
* SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <iostream>

#include <boost/format.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "ihd.h"

int main(int argc, char *argv[])
{
    std::cout << "Revision: " << ihd::get_version_string() << std::endl;
    std::string file, args;
    size_t total_num_samps;
    double freq, total_time;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "help message")
        ("file", po::value<std::string>(&file)->default_value("isrp_samples.dat"), "name of the file to write binary samples to")
        ("duration", po::value<double>(&total_time)->default_value(0), "total number of seconds to receive")
        ("nsamps", po::value<size_t>(&total_num_samps)->default_value(0), "total number of samples to receive")
        ("freq", po::value<double>(&freq)->default_value(0.0), "RF center frequency in Hz")

        ("args", po::value<std::string>(&args)->default_value(""), "ISRP device address args")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")) {
        std::cout << boost::format("IHD RX samples to file %s") % desc << std::endl;
        std::cout
            << std::endl
            << "This application streams data from a single channel of a Ipsolon device to a file.\n"
            << std::endl;
        return ~0;
    }

    /************************************************************************
     * Create device and block controls
     ***********************************************************************/
    std::cout << std::endl;
    std::cout << boost::format("Creating the ISRP device with: %s...") % args << std::endl;

    ihd::multi_isrp::sptr isrp = ihd::multi_isrp::make(args);
    isrp->get_rx_rate(0);

    return 0;
}
