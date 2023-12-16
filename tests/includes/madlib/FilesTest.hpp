#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>

#include "../../../src/includes/madlib/files.hpp"

using namespace std;
using namespace madlib;

class FilesTest {
protected:

    // Helper function to create test files
    static void createTestFiles() {
        filesystem::create_directory("testfiles");
        ofstream file1("testfiles/file1.jpg");
        ofstream file2("testfiles/file2.jpg");
        ofstream file3("testfiles/file3.txt");
        file1.close();
        file2.close();
        file3.close();
    }

    // Helper function to remove test files and the testfiles directory
    static void cleanupTestFiles() {
        filesystem::remove_all("testfiles");
    }

public:

    static void testFiles_findByExtension() {
        // Create some test files in the "testfiles" folder
        createTestFiles();

        // Test finding files by extension
        vector<string> jpgFiles = file_find_by_extension("testfiles", "jpg");
        assert(jpgFiles.size() == 2); // Assuming you have two .jpg files in "testfiles"

        vector<string> txtFiles = file_find_by_extension("testfiles", "txt");
        assert(txtFiles.size() == 1); // Assuming you have one .txt file in "testfiles"

        // Clean up: Remove the test files
        cleanupTestFiles();
    }

    static void testFiles_findByExtensions() {
        // Create some test files in the "testfiles" folder
        createTestFiles();

        // Test finding files by multiple extensions
        vector<string> extensions = { "jpg", "txt" };
        vector<string> jpgAndTxtFiles = file_find_by_extensions("testfiles", extensions);
        assert(jpgAndTxtFiles.size() == 3); // Assuming you have three .jpg and .txt files in "testfiles"

        // Clean up: Remove the test files
        cleanupTestFiles();
    }

    static void testFiles_replaceExtension() {
        // Test replacing extension
        assert(file_replace_extension("file.txt", "jpg") == "file.jpg");

        // Test with no extension
        assert(file_replace_extension("file", "jpg") == "file.jpg");

        // Test with empty filename
        assert(file_replace_extension("", "jpg") == ".jpg");

        // Test replacing extension with an empty one
        assert(file_replace_extension("file.txt", "") == "file.");
    }

    static void testFiles_normalizePath() {
        // Test normalizing path
        assert(path_normalize("path/to/../file.txt") == "path/file.txt");
        assert(path_normalize("../path/to/file.txt") == "../path/to/file.txt");
        assert(path_normalize("./path/to/file.txt") == "path/to/file.txt");
    }

    static void testFiles_extractPath() {
        // Test extracting path
        assert(path_extract("path/to/file.txt") == "path/to");
        assert(path_extract("file.txt") == "");
        assert(path_extract("") == "");
    }

    static void testFiles_extractFilename() {
        // Test extracting filename with extension
        assert(filename_extract("path/to/file.txt") == "file.txt");
        assert(filename_extract("file.txt") == "file.txt");
        assert(filename_extract("") == "");

        // Test extracting filename without extension
        assert(filename_extract("path/to/file.txt", true) == "file");
        assert(filename_extract("file.txt", true) == "file");
        assert(filename_extract("", true) == "");
    }

    static void testFiles_exists() {
        // Create some test files in the "testfiles" folder
        createTestFiles();

        // Test file existence
        assert(file_exists("testfiles/file1.jpg") == true);
        assert(file_exists("nonexistent.txt") == false);

        // Clean up: Remove the test files
        cleanupTestFiles();
    }

    static void testFiles_createPath() {
        // Test creating a directory
        string testDir = "test_directory";
        assert(file_exists(testDir) == false);
        file_create_path(testDir);
        assert(file_exists(testDir) == true);

        // Clean up: Remove the created directory
        filesystem::remove_all(testDir);
    }

    static void testFiles_getLastModificationTime() {
        // Test getting last modification time
        string testFile = "test.txt";
        // Create a test file (touch it)
        ofstream outfile(testFile);
        outfile.close();

        time_t lastModTime = file_get_mtime(testFile);
        // Ensure the last modification time is not 0 (epoch)
        assert(lastModTime != 0);

        // Clean up: Remove the test file
        filesystem::remove(testFile);
    }

    static void testFiles_file_get_contents() {
        // Create a test file with known content
        const string filename = "test_file_get_contents.txt";
        const string expected_content = "This is a test content.";

        // Write test content to the file
        file_put_contents(filename, expected_content);

        // Read the content using file_get_contents
        string actual_content;
        try {
            actual_content = file_get_contents(filename);
        } catch (const exception& e) {
            cerr << "Error in testFiles_file_get_contents: " << e.what() << endl;
            return;
        }

        // Check if the actual content matches the expected content
        assert(actual_content == expected_content);

        // Clean up: remove the test file
        remove(filename.c_str());
    }

    static void testFiles_file_put_contents() {
        // Define test data
        const string filename = "test_file_put_contents.txt";
        const string test_content = "This is a test content.";

        // Write test content to the file using file_put_contents
        try {
            file_put_contents(filename, test_content);
        } catch (const exception& e) {
            cerr << "Error in testFiles_file_put_contents: " << e.what() << endl;
            return;
        }

        // Read the content of the file
        ifstream file(filename);
        string actual_content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
        file.close();

        // Check if the actual content matches the expected content
        assert(actual_content == test_content);

        // Clean up: remove the test file
        remove(filename.c_str());
    }
};
