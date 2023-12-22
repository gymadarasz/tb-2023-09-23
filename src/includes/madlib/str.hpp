#pragma once

#include <map>
#include <regex>
#include <string>

#include "macros.hpp"

using namespace std;

namespace madlib {

    #define __FILE_LINE__ COLOR_FILENAME + madlib::path_normalize(__FILE__) + ":" + to_string(__LINE__) + COLOR_DEFAULT

    string str_replace(const string& str, const string& from, const string& to) {
        string result = str;
        size_t start_pos = 0;
        
        while ((start_pos = result.find(from, start_pos)) != string::npos) {
            result.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        
        return result;
    }

    string str_replace(const string& str, const map<string, string>& vals) {
        string result = str;

        for (const auto& entry : vals)
            result = str_replace(result, entry.first, entry.second);

        return result;
    }
    
    string str_esc(const string& input, const string& chars = "'\\") {
        string result;
        for (char c : input) {
            if (chars.find(c) != string::npos) result += '\\'; // Escape the character
            result += c;
        }
        return result;
    }

    const string str_sanitizer_default_allowed_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_- ";
    typedef string (*str_sanitizer_func_t)(const string&, const string&, const char);
    string str_sanitize(const string& input, const string& allowed = str_sanitizer_default_allowed_chars, const char replacement = '_') {
    
        // Replace disallowed characters with a safe character (e.g., '_')
        string sanitized = input;
        for (char& c : sanitized)
            if (allowed.find(c) == string::npos)
                c = replacement; // Replace with a safe character

        return sanitized;
    }

    bool str_starts_with(const string& prefix, const string& str) {
        return str.length() >= prefix.length() && 
            str.compare(0, prefix.length(), prefix) == 0;
    }

    bool str_ends_with(const string& suffix, const string& str) {
        return suffix.length() <= str.length() && 
            str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }


    string str_trim(const string& str) {
        // Find the first non-whitespace character from the beginning
        size_t start = 0;
        while (start < str.length() && isspace(str[start])) start++;

        // Find the first non-whitespace character from the end
        size_t end = str.length();
        while (end > start && isspace(str[end - 1])) end--;

        // Extract the trimmed substring
        return str.substr(start, end - start);
    }

    inline int regx_match(
        const string& pattern, 
        const string& str, 
        vector<string>* matches = nullptr
    ) {
        regex r(pattern);
        smatch m;
        if (regex_search(str, m, r)) {
            if (matches != nullptr) {
                // Clear the vector before adding more matches
                matches->clear();
                for (unsigned int i = 0; i < m.size(); i++)
                    matches->push_back(m[i].str());
            }
            return 1;
        }
        return 0;
    }

    inline int regx_match_all(
        const string& pattern,
        const string& str,
        vector<string>* matches = nullptr
    ) {
        regex r(pattern);
        smatch m;

        if (matches != nullptr) {
            // Clear the vector before adding more matches
            matches->clear();
        }

        // Use regex_iterator to find all matches
        for (sregex_iterator it(str.begin(), str.end(), r), end; it != end; ++it) {
            if (matches != nullptr) {
                for (const auto& match : *it) {
                    matches->push_back(match.str());
                }
            }
        }

        return static_cast<int>(matches != nullptr && !matches->empty());
    }
        
    string normalize_datetime(const string& datetime) {
        string tpl = "0000-01-01 00:00:00.000";
        const string trimed = str_trim(datetime);

        const size_t trimedLength = trimed.length();
        for (size_t i = 0; i < trimedLength; i++) 
            tpl[i] = trimed[i];

        return tpl;
    }

    string path_normalize(const string& filepath) {
        vector<string> components;
        istringstream iss(filepath);
        string token;

        // Tokenize the input path using the directory separator '/'
        while (getline(iss, token, '/')) {
            if (token == "..") {
                // Handle '..' by popping the last component if possible
                if (!components.empty() && components.back() != "..")
                    components.pop_back();
                else components.push_back("..");
            } else if (token != ".") {
                // Skip '.' components and add other components
                components.push_back(token);
            }
        }

        // Reconstruct the normalized path
        string normalized;
        for (const string& component : components) {
            if (!normalized.empty()) normalized += "/";
            normalized += component;
        }

        return normalized;
    }

    string filename_extract(const string& filepath, bool withoutExtension = false) {
        size_t lastSlashPos = filepath.find_last_of('/');
        size_t lastDotPos = filepath.find_last_of('.');

        if (lastSlashPos != string::npos) {
            // If there's a directory separator, return the substring after the last slash.
            string filename = filepath.substr(lastSlashPos + 1);

            // Check if withoutExtension is true and there's a dot in the entire path.
            // then return the substring before the last dot.
            if (withoutExtension && lastDotPos != string::npos && lastDotPos > lastSlashPos)
                return filename.substr(0, lastDotPos - (lastSlashPos + 1));

            return filename;
        }

        // If there's no directory separator, return the whole path as the filename,
        // then return the substring before the last dot.
        if (withoutExtension && lastDotPos != string::npos)
            return filepath.substr(0, lastDotPos);

        return filepath;
    }


    string path_extract(const string& filepath) {
        size_t lastSlashPos = filepath.find_last_of('/');
        if (lastSlashPos != string::npos) return filepath.substr(0, lastSlashPos);
        // If there's no directory separator, return an empty string or the whole path, depending on your preference.
        return "";  // Alternatively, you can return filepath;
    }

    vector<string> str_split(const string& separator, const string& data) {
        if (data.empty()) return {};

        vector<string> tokens;
        size_t start = 0, end = 0;

        while ((end = data.find(separator, start)) != string::npos) {
            tokens.push_back(data.substr(start, end - start));
            start = end + separator.length();
        }

        // Add the last token (or the only token if no separator found)
        tokens.push_back(data.substr(start));

        return tokens;
    }
    
    string str_concat(const vector<string>& strs, const string& sep) {
        if (strs.empty()) return "";
        string result = strs[0];
        for (size_t i = 1; i < strs.size(); ++i) result += sep + strs[i];
        return result;
    }

    template <typename... Args>
    string concat(Args... args) {
        ostringstream stream;
        (stream << ... << args);
        return stream.str();
    }

    bool is_numeric(const string& str) {
        // Regular expression pattern for numeric strings
        regex pattern(R"(\s*[-+]?\d+\s*|\s*[-+]?(\d*\.\d+|\d+\.\d*)([eE][-+]?\d+)?\s*)");
        return regex_match(str, pattern);
    }

    string str_to_lower(const string& str) {
        string ret = "";
        for (size_t i = 0; i < str.length(); i++)
            if (isupper(str[i])) ret += (char)tolower(str[i]);
            else ret += str[i];
        return ret;
    }

    bool parse_bool(const string& str) {
        if (str.empty()) return false;
        if (str == "0") return false;
        string lower = str_to_lower(str);
        if (lower == "false") return false;
        if (lower == "no") return false;
        return true;
    }

    template<typename T>
    T parse(const string& s) {
        stringstream ss(s);
        T result;
        if (!(ss >> result)) 
            throw runtime_error("parse error"); // ERROR("Invalid value" + (s.empty() ? "" : ": " + s));
        return result;
    }
}