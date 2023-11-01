#include <iostream>
#include <cstdio> // Include the cstdio header
// #include <cstring> // Include the cstring header for string functions

#include "includes/madlib/Tools.hpp"

using namespace std;
using namespace madlib;

int main()
{
    string filename = exec("zenity --file-selection --title 'Open File'");
    cout << "FILE:" << filename << endl;
    return 0;
}
