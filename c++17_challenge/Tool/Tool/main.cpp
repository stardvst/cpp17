/**
    author: Lilia Manukyan
    email: lilia.manukyan@hotmail.com

    c++17 features used: 
    1. std::filesystem
    2. std::optional
    3. init-statement for if
*/

#include <experimental/filesystem>
#include <algorithm>
#include <optional>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

namespace fs = std::experimental::filesystem;

std::optional<int> get_col_index(const std::string &columns, const std::string &column) {
    std::istringstream iss(columns);
    std::string current;

    // set initial value of column count
    int count = -1;

    while(std::getline(iss, current, ',')) {
        // found another column so increment the count
        ++count;

        // if the current column equals to the user specified one, return its index
        if(current == column) {
            return count;
        }
    }

    return {};
}

std::vector<std::string> get_fields(const std::string &line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string field;

    // split the line (by comma) into fields 
    while(std::getline(iss, field, ',')) {
        result.push_back(field);
    }

    return result;
}

std::string construct_line(const std::vector<std::string> &data) {
    std::string line;

    // append each field and a comma to the result
    for(const auto &field : data) {
        line.append(field);
        line.append(",");
    }

    // replace the last comma with a newline
    *line.rbegin() = '\n';

    return line;
}

int main(int argc, char **argv) {  

    // check the number of command line arguments
    if(argc != 5) {
        std::cerr << "ERROR! usage: " << argv[0] << " <input_file> <column> <new_value> <output_file>" << std::endl;
        return 0;
    }

    fs::path file(argv[1]); // input file name is specified at argv[1]
    const auto filename = argv[1];

    /**
        do the following checks:
        1. does a file with specified name exist?
        2. is the file a .csv file?
    */
    if(!(fs::exists(file) && file.extension() == ".csv")) {
        std::cerr << "wrong input file, " << filename << std::endl;
        return 0;
    }

    // if the file is empty, output a message to the console
    if(fs::is_empty(file)) {
        std::cout << "input file missing" << std::endl;
        return 0;
    }

    // associate the file with a stream
    std::ifstream input_file;
    input_file.open(filename);

    // not much we can do if the file couldn't be opened
    if(!input_file.is_open()) {
        std::cerr << "couldn't open the specified file" << std::endl;
        return 0;
    }

    // we got here, everything is okay! so let's do the actual job

    // get the first line of the file (column names are here)
    std::string columns;
    std::getline(input_file, columns);

    // saving this number to check whether each line has this amount of fields
    const auto fields = std::count(columns.begin(), columns.end(), ',') + 1;

    // get the index of the user specified column among columns, or {} if no such column was found
    if(const auto index = get_col_index(columns, std::string(argv[2])); index) {
        const auto col_index = index.value();

        // create the output file
        std::ofstream output_file;
        output_file.open(argv[4], std::ios::trunc); // if the file exists, discard its contents

        // not much we can do if we can't open the output file
        if(!output_file.is_open()) {
            std::cerr << "can't open output file" << std::endl;
            return 0;
        }

        // write column names to the output file
        output_file << columns << '\n';

        const std::string new_value = argv[3];

        std::string line;
        while(std::getline(input_file, line)) {
            
            // make sure the line has correct number of fields
            if(std::count(line.begin(), line.end(), ',') + 1 != fields) {
                std::cerr << "insufficient data" << std::endl;
                return 0;
            }

            // get vector of fields (separated by comma) in a line
            auto data = get_fields(line);
            data[col_index] = new_value;

            // construct the line (add commas between fields, newline at the end)
            line = construct_line(data);

            // write the new data to the output file
            output_file << line;
        }

        // close the output file
        output_file.close();
    } else {
        std::cout << "column name doesn't exist in the input file" << std::endl;
    }

    // close the input file
    input_file.close();

    return 0;
}