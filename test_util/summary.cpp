/*
  Copyright 2019 Equinor ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <opm/io/eclipse/ESmry.hpp>
#include <opm/io/eclipse/ExtESmry.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <getopt.h>

namespace {

enum smryFileType {
     SMSPEC, ESMRY
};

static void printHelp() {

    std::cout << "\nsummary needs a minimum of two arguments. First is smspec filename and then list of vectors  \n"
              << "\nIn addition, the program takes these options (which must be given before the arguments):\n\n"
              << "-h Print help and exit.\n"
              << "-l list all summary vectors.\n"
              << "-n print summary vectors without headers.\n"
              << "-r extract data only for report steps. \n\n";
}

void printHeader(const std::vector<std::string>& keyList, const std::vector<int>& width){

    std::cout << std::endl;

    for (size_t n= 0; n < keyList.size(); n++){
        if (width[n] < 14)
            std::cout << std::setw(16) << keyList[n];
        else
            std::cout << std::setw(width[n] + 2) << keyList[n];
    }

    std::cout << std::endl;
}

std::string formatString(float data, int width){

    std::stringstream stream;

    if (std::fabs(data) < 1e6)
        if (width < 14)
            stream << std::fixed << std::setw(16) << std::setprecision(6) << data;
        else
            stream << std::fixed << std::setw(width + 2) << std::setprecision(6) << data;

    else
       stream << std::scientific << std::setw(16) << std::setprecision(6)  << data;

    return stream.str();
}

} // Anonymous namespace

int main(int argc, char **argv) {

    int c                          = 0;
    bool reportStepsOnly           = false;
    bool listKeys                  = false;
    bool headers                   = true;

    while ((c = getopt(argc, argv, "hrnl")) != -1) {
        switch (c) {
        case 'h':
            printHelp();
            return EXIT_SUCCESS;
        case 'r':
            reportStepsOnly=true;
            break;
        case 'n':
            headers=false;
            break;
        case 'l':
            listKeys=true;
            break;
        default:
            return EXIT_FAILURE;
        }
    }

    const int argOffset = optind;
    if (argOffset > argc - 1) {
        printHelp();
        // Returning failure since the user did not
        // give the correct number of arguments.
        return EXIT_FAILURE;
    }

    std::unique_ptr<Opm::EclIO::ESmry> esmry;
    std::unique_ptr<Opm::EclIO::ExtESmry> ext_esmry;

    std::string filename = argv[argOffset];
    std::filesystem::path inputFileName(filename);

    if (inputFileName.extension()=="")
        inputFileName+=".SMSPEC";

    smryFileType filetype;

    if (const auto& ext = inputFileName.extension();
        (ext == ".SMSPEC") || (ext == ".FSMSPEC"))
    {
        filetype = SMSPEC;
        esmry = std::make_unique<Opm::EclIO::ESmry>(inputFileName);
    } else if (ext == ".ESMRY") {
        filetype = ESMRY;
        ext_esmry = std::make_unique<Opm::EclIO::ExtESmry>(inputFileName);
    } else {
        throw std::runtime_error("invalid input file for summary");
    }

    if (listKeys){
        std::vector<std::string> list;

        switch(filetype) {
            case SMSPEC: list = esmry->keywordList(); break;
            case ESMRY: list = ext_esmry->keywordList(); break;
        }

        for (size_t n = 0; n < list.size(); n++){
            std::cout << std::setw(20) << list[n];

            if (((n+1) % 5)==0){
                std::cout << std::endl;
            }
        }

        std::cout << std::endl;

        return 0;
    }

    std::vector<std::string> smryList;
    for (int i=0; i<argc - argOffset-1; i++) {

        bool hasKey = false;

        switch(filetype) {
        case SMSPEC:
            hasKey = esmry->hasKey(argv[i+argOffset+1]);
            break;
        case ESMRY:
            hasKey = ext_esmry->hasKey(argv[i+argOffset+1]);
            break;
        }

        if (hasKey) {
            smryList.push_back(argv[i+argOffset+1]);
        } else {
            std::vector<std::string> list;

            switch(filetype) {
            case SMSPEC:
                list = esmry->keywordList(argv[i+argOffset+1]);
                break;
            case ESMRY:
                list = ext_esmry->keywordList(argv[i+argOffset+1]);
                break;
            }

            if (list.size()==0) {
                std::string message = "Key " + std::string(argv[i+argOffset+1]) + " not found in summary file " + filename;
                std::cout << "\n!Runtime Error \n >> " << message << "\n\n";
                return EXIT_FAILURE;
            }

            smryList.insert(smryList.end(), list.begin(), list.end());
        }
    }

    if (smryList.size()==0){
        std::string message = "No summary keys specified on command line";
        std::cout << "\n!Runtime Error \n >> " << message << "\n\n";
        return EXIT_FAILURE;
    }

    std::vector<std::vector<float>> smryData;
    std::vector<int> width;

    std::transform(smryList.begin(), smryList.end(), std::back_inserter(width),
                   [](const auto& name) { return name.size(); });

    for (const auto& key : smryList) {
        std::vector<float> vect;

        switch(filetype) {
        case SMSPEC:
            vect = reportStepsOnly ? esmry->get_at_rstep(key) : esmry->get(key);
            break;
        case ESMRY:
            vect = reportStepsOnly ? ext_esmry->get_at_rstep(key) : ext_esmry->get(key);
            break;
        }

        smryData.push_back(vect);
    }
    
    if (headers)
        printHeader(smryList, width);

    for (size_t s=0; s<smryData[0].size(); s++){
        for (size_t n=0; n < smryData.size(); n++){
            if (headers)
                std::cout << formatString(smryData[n][s], width[n]);
            else
                std::cout << std::scientific << std::setprecision(8) << smryData[n][s] << " ";
        }
        std::cout << std::endl;
    }
 
    if (headers)
        std::cout << std::endl;
    
    return 0;
}
