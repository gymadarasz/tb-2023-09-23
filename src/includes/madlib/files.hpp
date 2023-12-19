#pragma once

#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include "ERROR.hpp"

using namespace std;

namespace madlib {

    vector<string> file_find_by_extension(const filesystem::path& folder = ".", const string& pattern = "") {
        vector<string> files;

        for (const auto& entry : filesystem::directory_iterator(folder)) {
            // cout << "ENTRY:" << entry.path() << endl;
            if (
                (entry.is_directory() || entry.is_regular_file()) && 
                (
                    pattern.empty() || 
                    entry.path().filename().string().find(pattern) != string::npos
                )
            ) {
                // cout << "FILE:" << entry.path() << endl;
                files.push_back(entry.path().string());
            } else if (entry.is_directory()) {
                // Recursively search in subdirectories
                vector<string> subs = file_find_by_extension(entry.path(), pattern);
                files.insert(files.end(), subs.begin(), subs.end());
            }
        }

        return files;
    }

    vector<string> file_find_by_extensions(const filesystem::path& folder, const vector<string>& patterns) {
        vector<string> files;

        for (const string& pattern : patterns) {
            vector<string> matches = file_find_by_extension(folder, pattern);
            files.insert(files.end(), matches.begin(), matches.end());
        }

        return files;
    }

    string file_replace_extension(const string& filename, const string& extension) {
        // Find the position of the last dot (.) in the file name
        size_t lastDotPos = filename.find_last_of('.');

        // Check if the lastDotPos is not at the end of the filename
        if (lastDotPos != string::npos && lastDotPos != filename.length() - 1) {
            // If the extension already has a leading dot, use it as is
            // Otherwise, add a leading dot
            return filename.substr(0, lastDotPos) + (extension[0] == '.' ? "" : ".") + extension;
        }

        // If there's no dot in the file name or the dot is at the end,
        // simply append the extension with a leading dot if needed
        return filename + (extension[0] == '.' ? "" : ".") + extension;
    }

    bool file_exists(const string& filePath) {
        filesystem::path pathToCheck(filePath);
        return filesystem::exists(pathToCheck);
    }

    void file_create_path(const string& path) {
        filesystem::path pathToCreate(path);

        try {
            // Create the directory and its parent directories (if they don't exist)
            filesystem::create_directories(pathToCreate);
        } catch (const exception& e) {
            throw ERROR("Unable to create folder: " + string(e.what()));
        }
    }

    time_t file_get_mtime(const string& filePath) {
        struct stat fileInfo;
        if (stat(filePath.c_str(), &fileInfo) != 0)
            throw ERROR("Unable to get file information.");
        
        return fileInfo.st_mtime;
    }

    string file_get_contents(const string& filename) {
        ifstream file(filename);
        if (!file.is_open())
            throw ERROR("Unable to open file for reading: " + filename);

        string content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
        file.close();

        return content;
    }

    void file_put_contents(const string& filename, const string& data, bool append = false) {
        // Check if the file is a symlink
        if (filesystem::is_symlink(filename))
            throw ERROR("Symlink detected. Refusing to open: " + filename);

        // Check if the file is a special file (e.g., character or block device)
        if (filesystem::is_character_file(filename) || filesystem::is_block_file(filename))
            throw ERROR("Special file detected. Refusing to open: " + filename);
            
        ofstream file;
        // FlawFinder: ignore
        file.open(filename, append ? ios::out | ios::app : ios::out);

        if (!file.is_open())
            throw ERROR("Unable to open file for writing: " + filename);

        file << data;
        file.close();
    }
}