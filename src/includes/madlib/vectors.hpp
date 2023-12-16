#pragma once

#include <fstream>
#include <vector>
#include <string>
#include "str.hpp"
#include "ERROR.hpp"

using namespace std;

namespace madlib {

    string vector_concat(
        const vector<string>& strings, 
        const string& separator = " ", 
        str_sanitizer_func_t sanitizer = nullptr, 
        const string& allowed_chars = str_sanitizer_default_allowed_chars,
        const char replacement = '_'
    ) {
        string result;
        
        for (size_t i = 0; i < strings.size(); ++i) {
            result += 
                sanitizer 
                    ? sanitizer(strings[i], allowed_chars, replacement) 
                    : strings[i];
            
            // Add the separator unless it's the last element
            if (i < strings.size() - 1) result += separator;
        }
        
        return result;
    }

    /**
     * @note plain-old-data only
     */
    template<typename T>
    void vector_save(const string &filename, const vector<T>& data) {
        ofstream file(filename, ios::binary);
        if (!file.is_open()) throw ERROR("Error opening file for writing: " + filename);
        for (const T& item : data) file.write(reinterpret_cast<const char*>(&item), sizeof(T));
        file.close();
    }
    
    /**
     * @note plain-old-data only
     */
    template<typename T>
    vector<T> vector_load(const string &filename) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) 
            throw ERROR("Unable to open file for reading: " + filename);
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
    vector<T>& vector_load(const string &filename, vector<T>& data) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) throw ERROR("Unable to open file for reading: " + filename);
        data.clear();
        T item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(T))) data.push_back(item);
        file.close();
        return data;
    }

    template<typename T>
    bool vector_compare(const vector<T>& data1, const vector<T>& data2) {
        return data1 == data2;
    }

    // Add a function to compare vectors of custom objects
    template<typename T>
    bool vector_compare(const vector<T>& v1, const vector<T>& v2, bool (*compareFunc)(const T&, const T&)) {
        if (v1.size() != v2.size()) return false;
        for (size_t i = 0; i < v1.size(); ++i) if (!compareFunc(v1[i], v2[i])) return false;
        return true;
    }

    template<typename T>
    T* vector_create(vector<T*>& v) {
        T* elem = new T();
        v.push_back(elem);
        return elem;
    }

    template<typename T, typename... Args>
    T* vector_create(vector<T*>& v, Args&&... args) {
        T* elem = new T(forward<Args>(args)...);
        v.push_back(elem);
        return elem;
    }

    template<typename T>
    void vector_destroy(vector<T*>& v) {
        for (T* elem: v) 
            if (elem) {
                delete elem;
                elem = nullptr;
            }
        v.clear();
    }

    template<typename T>
    bool vector_has(const vector<T>& v, size_t at) {
        return v.size() > at;
    }

    template<typename T>
    T vector_min(vector<T> values) {
        if (values.empty()) throw ERROR("Empty set has no minimum");
        T minVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] < minVal) minVal = values[i];
        return minVal;
    }

    template<typename T>
    T vector_max(vector<T> values) {
        if (values.empty()) throw ERROR("Empty set has no maximun");
        T maxVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] > maxVal) maxVal = values[i];
        return maxVal;
    }

    template<typename T>
    void vector_sort(vector<T>& values) {
        sort(values.begin(), values.end());
    }

    template<typename T>
    void vector_unique(vector<T>& values) {
        sort(values.begin(), values.end());
        auto last = unique(values.begin(), values.end());
        values.erase(last, values.end());
    }

    template<typename T>
    bool vector_contains(const vector<T>& values, T value) {
        return std::find(values.begin(), values.end(), value) != values.end();
    }

}