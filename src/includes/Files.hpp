#pragma once

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

class Files {

public:
    static vector<string> findByExtension(const filesystem::path& folder = ".", const string& pattern = "") {
        vector<string> files;

        for (const auto& entry : filesystem::directory_iterator(folder)) {
            // cout << "ENTRY:" << entry.path() << endl;
            if (
                entry.is_regular_file() && 
                (
                    pattern.empty() || 
                    entry.path().filename().string().find(pattern) != string::npos
                )
            ) {
                // cout << "FILE:" << entry.path() << endl;
                files.push_back(entry.path().string());
            } else if (entry.is_directory()) {
                // Recursively search in subdirectories
                vector<string> subs = findByExtension(entry.path(), pattern);
                files.insert(files.end(), subs.begin(), subs.end());
            }
        }

        return files;
    }

    static vector<string> findByExtensions(const filesystem::path& folder, const vector<string>& patterns) {
        vector<string> files;

        for (const string& pattern : patterns) {
            vector<string> matches = findByExtension(folder, pattern);
            files.insert(files.end(), matches.begin(), matches.end());
        }

        return files;
    }

    static string replaceExtension(const string& filename, const string& extension) {
        // Find the position of the last dot (.) in the file name
        size_t lastDotPos = filename.find_last_of('.');

        if (lastDotPos != string::npos) {
            // Create a new string with the part before the last dot and the new extension
            return filename.substr(0, lastDotPos) + "." + extension;
        } 
        // If there's no dot in the file name, simply append the new extension
        return filename + "." + extension;
    }

    static string normalizePath(const string& filepath) {
        vector<string> components;
        istringstream iss(filepath);
        string token;

        // Tokenize the input path using the directory separator '/'
        while (getline(iss, token, '/')) {
            if (token == "..") {
                // Handle '..' by popping the last component if possible
                if (!components.empty() && components.back() != "..") {
                    components.pop_back();
                } else {
                    components.push_back("..");
                }
            } else if (token != ".") {
                // Skip '.' components and add other components
                components.push_back(token);
            }
        }

        // Reconstruct the normalized path
        string normalized;
        for (const string& component : components) {
            if (!normalized.empty()) {
                normalized += "/";
            }
            normalized += component;
        }

        return normalized;
    }

    static string extractPath(const string& filepath) {
        size_t lastSlashPos = filepath.find_last_of('/');
        if (lastSlashPos != string::npos) {
            return filepath.substr(0, lastSlashPos);
        }
        // If there's no directory separator, return an empty string or the whole path, depending on your preference.
        return "";  // Alternatively, you can return filepath;
    }

    static bool exists(const string& filePath) {
        filesystem::path pathToCheck(filePath);
        return filesystem::exists(pathToCheck);
    }

    static void createPath(const string& path) {
        filesystem::path pathToCreate(path);

        try {
            // Create the directory and its parent directories (if they don't exist)
            filesystem::create_directories(pathToCreate);
        } catch (const exception& e) {
            throw runtime_error("Error creating directory: " + string(e.what()));
        }
    }

    static time_t getLastModificationTime(const string& filePath) {
        struct stat fileInfo;
        if (stat(filePath.c_str(), &fileInfo) != 0) {
            throw runtime_error("Error getting file information.");
        }
        
        return fileInfo.st_mtime;
    }

    static void file_put_contents(const string filename, const string data, bool append = false) {
        try {
            ofstream file;
            if (append) {
                file.open(filename, ios::out | ios::app);
            } else {
                file.open(filename, ios::out);
            }

            if (!file.is_open()) {
                throw runtime_error("Error: Unable to open file for writing: " + filename);
            }

            file << data;
            file.close();
        } catch (const exception& e) {
            throw runtime_error("Error in file_put_contents: " + string(e.what()));
        }
    }

    static string file_get_contents(const string filename) {
        try {
            ifstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Error: Unable to open file for reading: " + filename);
            }

            string content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
            file.close();

            return content;
        } catch (const exception& e) {
            throw runtime_error("Error in file_get_contents: " + string(e.what()));
        }
    }
};
