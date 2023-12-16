#pragma once

#include <string>
#include <stdexcept>

#include "macros.hpp"

using namespace std;

namespace madlib {

    #define ERROR(msg) runtime_error(COLOR_ERROR "Error: " __FILE_LINE__ + " " + string(msg)); 
    #define ERR_UNIMP ERROR("Unimplemented");
    #define ERR_MISSING ERROR("Missing");
    #define ERR_INVALID ERROR("Invalid");
}