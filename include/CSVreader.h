#ifndef CSVreader_H
#define CSVreader_H

#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
#include <boost/algorithm/string.hpp>



class CSVreader{
    std::string filename;
    std::string delimeter;

    public:
        CSVreader(std::string filename, std::string delimeter): filename(filename), delimeter(delimeter)
        {

        }

        std::vector<std::vector<std::string>> getData();

};


std::vector<std::vector<std::string>> CSVreader::getData(){
    std::ifstream file(filename);
    std::vector<std::vector<std::string>> data;
    // std::vector<std::string> data;
    std::string line = "";
    std::string::size_type sz;
    
    while (getline(file,line)){

        std::vector<std::string> vec;
        boost::algorithm::split(vec,line,boost::is_any_of(delimeter));
        // data.push_back(std::stod (vec.c_str(),&sz));
        data.push_back(vec);
    }
    file.close();
    return data;
}

#endif 