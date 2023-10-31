#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

namespace madlib {

    class Vector {
    public:
        static string concat(const vector<string>& strings, const string& separator = " ") {
            string result;
            
            for (size_t i = 0; i < strings.size(); ++i) {
                result += strings[i];
                
                // Add the separator unless it's the last element
                if (i < strings.size() - 1) result += separator;
            }
            
            return result;
        }

        /**
         * @note plain-old-data only
         */
        template<typename T>
        static void save(const string &filename, const vector<T>& data) {
            ofstream file(filename, ios::binary);
            if (!file.is_open()) throw ERROR("Error opening file for writing: " + filename);
            for (const T& item : data) file.write(reinterpret_cast<const char*>(&item), sizeof(T));
            file.close();
        }
        
        /**
         * @note plain-old-data only
         */
        template<typename T>
        static vector<T> load(const string &filename) {
            ifstream file(filename, ios::binary);
            if (!file.is_open()) throw ERROR("Error opening file for reading: " + filename);
            T item;
            vector<T> data;
            while (file.read(reinterpret_cast<char*>(&item), sizeof(T))) data.push_back(item);
            file.close();
            return data;
        }
        
        /**
         * @note plain-old-data only
         */
        template<typename T>
        static vector<T>& load(const string &filename, vector<T>& data) {
            ifstream file(filename, ios::binary);
            if (!file.is_open()) throw ERROR("Error opening file for reading: " + filename);
            T item;
            while (file.read(reinterpret_cast<char*>(&item), sizeof(T))) data.push_back(item);
            file.close();
            return data;
        }

        template<typename T>
        static bool compare(const vector<T>& data1, const vector<T>& data2) {
            return data1 == data2;
        }

        // Add a function to compare vectors of custom objects
        template<typename T>
        static bool compare(const vector<T>& v1, const vector<T>& v2, bool (*compareFunc)(const T&, const T&)) {
            if (v1.size() != v2.size()) return false;
            for (size_t i = 0; i < v1.size(); ++i) if (!compareFunc(v1[i], v2[i])) return false;
            return true;
        }

        template<typename T>
        static void destroy(vector<T*> v) {
            for (T* elem: v) 
                if (elem) {
                    delete elem;
                    elem = NULL;
                }
            v.clear();
        }

     };
    
}