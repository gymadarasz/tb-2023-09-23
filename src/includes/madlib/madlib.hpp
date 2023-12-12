#pragma once

#include <thread>
#include <chrono>
#include <random>
#include <stdexcept>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dlfcn.h>
#include <poll.h>

using namespace std;
using namespace chrono;

namespace madlib {

    #define MS_PER_SEC 1000ul
    #define MS_PER_MIN (60ul * MS_PER_SEC)
    #define MS_PER_HOUR (60ul * MS_PER_MIN)
    #define MS_PER_DAY (24ul * MS_PER_HOUR)
    #define MS_PER_WEEK (7ul * MS_PER_DAY)

    #define COLOR_DEFAULT "\033[0;0;0m"
    #define COLOR_ERROR "\033[31m"
    #define COLOR_ALERT "\033[31m"
    #define COLOR_WARNING "\033[33m"
    #define COLOR_INFO "\033[36m"
    #define COLOR_SUCCESS "\033[32m"
    #define COLOR_DEBUG "\033[35m"
    #define COLOR_FILENAME "\033[90m"
    #define COLOR_DATETIME "\033[90m"

    #define QUOTEME_1(x) #x
    #define QUOTEME(x) QUOTEME_1(x)

    #define __DIR__ madlib::path_normalize(madlib::path_extract(__FILE__))

    #define __FILE_LINE__ COLOR_FILENAME + madlib::path_normalize(__FILE__) + ":" + to_string(__LINE__) + COLOR_DEFAULT
    #define __DATE_TIME__ COLOR_DATETIME "[" + madlib::ms_to_datetime() + "] " COLOR_DEFAULT

    #define ERROR(msg) runtime_error(COLOR_ERROR "Error: " __FILE_LINE__ + " " + string(msg)); 
    #define ERR_UNIMP ERROR("Unimplemented");
    #define ERR_MISSING ERROR("Missing");
    #define ERR_INVALID ERROR("Invalid");

    #define LOG(...) logger.date().writeln(__VA_ARGS__)
    #define DBG(...) logger.date().writeln(__FILE_LINE__, " ", COLOR_DEBUG, __VA_ARGS__, COLOR_DEFAULT)

    
    /**
        DBG_COUNTER;
        if (DBG_counter == 1) {
            DBG_BREAK;
        }
    */
    int DBG_counter = 0;
    #define DBG_COUNTER cout << "DBG counter: " << ++madlib::DBG_counter << " at " << __FILE__ << ":" << __LINE__ << endl;
    #define DBG_BREAK cout << "DBG break at " << __FILE__ << ":" << __LINE__ << endl;

    

    typedef long ms_t;

    const ms_t second = 1000;
    const ms_t minute = 60 * second;
    const ms_t hour = 60 * minute;
    const ms_t day = 24 * hour;
    const ms_t week = 7 * day;

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

    const string str_sanitizer_default_allowed_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_- ";
    typedef string (*str_sanitizer_func_t)(const string&, const string&);

    string str_sanitize(const string& input, const string& allowed = str_sanitizer_default_allowed_chars) {
    
        // Replace disallowed characters with a safe character (e.g., '_')
        string sanitized = input;
        for (char& c : sanitized)
            if (allowed.find(c) == string::npos)
                c = '_'; // Replace with a safe character

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


    string trim(const string& str) {
        // Find the first non-whitespace character from the beginning
        size_t start = 0;
        while (start < str.length() && isspace(str[start])) start++;

        // Find the first non-whitespace character from the end
        size_t end = str.length();
        while (end > start && isspace(str[end - 1])) end--;

        // Extract the trimmed substring
        return str.substr(start, end - start);
    }

    inline int reg_match(const string& pattern, const string& str, vector<string>* matches = nullptr) {
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
        
    string normalize_datetime(const string& datetime) {
        string tpl = "0000-01-01 00:00:00.000";
        const string trimed = trim(datetime);

        const size_t trimedLength = trimed.length();
        for (size_t i = 0; i < trimedLength; i++) 
            tpl[i] = trimed[i];

        return tpl;
    }

    ms_t now() {
        // Get the current time_point
        system_clock::time_point currentTime = system_clock::now();

        // Convert it to milliseconds since the epoch
        milliseconds ms = duration_cast<milliseconds>(currentTime.time_since_epoch());

        // Extract the value as a long
        return static_cast<ms_t>(ms.count()); 
    }

    ms_t datetime_to_ms(const string& datetime) {
        if (datetime.empty()) return 0;

        struct tm time_info = {};
        int milliseconds = 0;
        
        size_t size = datetime.size();
        time_info.tm_year = (size > 3 ? stoi(datetime.substr(0, 4)) : 1970) - 1900;
        time_info.tm_mon = (size > 6 ? stoi(datetime.substr(5, 2)) : 1) - 1;
        time_info.tm_mday = size > 9 ? stoi(datetime.substr(8, 2)) : 1;
        time_info.tm_hour = size > 12 ? stoi(datetime.substr(11, 2)) : 0;
        time_info.tm_min = size > 15 ? stoi(datetime.substr(14, 2)) : 0;
        time_info.tm_sec = size > 18 ? stoi(datetime.substr(17, 2)) : 0;
        milliseconds = size > 22 ? stoi(datetime.substr(20, 3)) : 0;

        // Convert the struct tm to milliseconds
        time_t seconds = mktime(&time_info);
        return seconds * 1000 + milliseconds;
    }

    string ms_to_datetime(ms_t ms = now(), const char* fmt = "%Y-%m-%d %H:%M:%S", bool millis = true, bool local = false) {
        long sec = ms / 1000;
        long mil = ms % 1000;

        struct tm converted_time;
        if (local) localtime_r(&sec, &converted_time);
        else gmtime_r(&sec, &converted_time);

        ostringstream oss;
        oss << put_time(&converted_time, fmt);

        if (millis) oss << "." << setfill('0') << setw(3) << mil;

        return oss.str();
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

    inline void sleep(unsigned long ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    }

    mt19937* rand_gen = NULL;

    inline void rand_close() {
        delete rand_gen;
        rand_gen = NULL;
    }

    inline void rand_init() {
        if (rand_gen) rand_close();
        random_device rd; 
        rand_gen = new mt19937(rd());
    }

    inline void rand_init_seed(unsigned int seed) {
        if (rand_gen) rand_close();
        rand_gen = new mt19937(seed);
    }

    inline double randd(double min, double max) {
        if (!rand_gen) rand_init();
        uniform_real_distribution<double> dis(min, max);
        return dis(*rand_gen);
    }

    inline long randl(long min, long max) {
        if (!rand_gen) rand_init();
        uniform_int_distribution<long> dis(min, max);
        return dis(*rand_gen);
    }

    inline int randi(int min, int max) {
        if (!rand_gen) rand_init();
        uniform_int_distribution<int> dis(min, max);
        return dis(*rand_gen);
    }

    template<typename T>
    vector<T>& rand_norm_dist(T mean, T stddev, size_t count, vector<T>& rands) {
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<T> dist(mean, stddev);
        for (size_t i = 0; i < count; i++) rands.push_back(dist(gen));
        return rands;
    }

    template<typename T>
    vector<T> rand_norm_dist(T mean, T stddev, size_t count) {
        vector<T> rands;
        random_device rd;
        mt19937 gen(rd());
        normal_distribution<T> dist(mean, stddev);
        for (size_t i = 0; i < count; i++) rands.push_back(dist(gen));
        return rands;
    }

    template <typename... Args>
    string concat(Args... args) {
        ostringstream stream;
        (stream << ... << args);
        return stream.str();
    }

    vector<string> split(const string& separator, const string& data) {
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

    class Shared {
    public:
        struct Args {
            void* context = nullptr;
        };
        explicit Shared(void* = nullptr) {}
    };

    class Printer: public Shared {
    public:
        using Shared::Shared;
        virtual void print(const string& output) = 0;
        void println(const string& output) {
            print(output + "\n");
        }
        virtual ~Printer() {}
    };

    class Log: public Printer {
    protected:
        string filename;

    public:
        Log(const string& f = "app.log"): Printer(), filename(f) {}

        Log& date() {
            write(__DATE_TIME__);
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        Log& writeln(Args... args) {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            write(message + "\n");
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        Log& write(Args... args) {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            print(message);
            
            return *this;
        }

        void print(const string& message) override final {
            file_put_contents(filename, message, true);
        }

    } logger;

    string exec(const string& command, bool captureOutput = true, bool showOutput = false) {
        array<char, 128> buffer;
        string result;

        // Open the command for reading, redirecting stderr to stdout
        FILE* pipe = popen((command + " 2>&1").c_str(), "r");
        if (!pipe) throw ERROR("Failed to execute command.");

        // Read the output (if requested)
        if (captureOutput || showOutput) {
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
                result += buffer.data();
            if (showOutput) cout << result;
        }

        // Close the pipe
        pclose(pipe);

        return result;
    }

    FILE* pipe_exec(const string& command) {
        // Execute the command in the background
        FILE* pipe = popen(command.c_str(), "w");
        if (!pipe) throw ERROR("Failed to open pipe for command execution.");
        return pipe;
    }

    bool pipe_is_closed(FILE* pipe) {
        // Get the file descriptor associated with the FILE*
        pollfd pfd = { fileno(pipe), POLLOUT, 0 };
        if (poll(&pfd, 1, 1) < 0) return false;
        return pfd.revents & POLLERR;
    }

    bool pipe_send(FILE* pipe, const string& updates) {
        // Check writing to the pipe failed (possibly closed by user), return false
        if (pipe_is_closed(pipe)) return false;
        fprintf(pipe, "%s\n", updates.c_str());
        if (pipe_is_closed(pipe)) return false;
        fflush(pipe);
        return true;

    }

    int pipe_close(FILE* pipe) {
        // Close the pipe
        return pclose(pipe);
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
        if (!(ss >> result)) throw ERROR("Invalid value" + (s.empty() ? "" : ": " + s));
        return result;
    }
    
    vector<string> file_find_by_extension(const filesystem::path& folder = ".", const string& pattern = "") {
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

        // Create a string with the part before the last dot and the extension
        if (lastDotPos != string::npos)
            return filename.substr(0, lastDotPos) + "." + extension;
        
        // If there's no dot in the file name, simply append the extension
        return filename + "." + extension;
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

    string vector_concat(
        const vector<string>& strings, 
        const string& separator = " ", 
        str_sanitizer_func_t sanitizer = NULL, 
        const string& allowed_chars = str_sanitizer_default_allowed_chars
    ) {
        string result;
        
        for (size_t i = 0; i < strings.size(); ++i) {
            result += 
                sanitizer 
                    ? sanitizer(strings[i], allowed_chars) 
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
        if (!file.is_open()) throw ERROR("Unable to open file for reading: " + filename);
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
                elem = NULL;
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

    template<typename KeyT, typename T>
    bool map_has(const map<KeyT, T>& m, KeyT key) {
        return !m.empty() && m.count(key) > 0;
    }
    template<typename T>
    bool map_has(const map<string, T>& m, string key) {
        return map_has<string, T>(m, key);
    }
    template<typename T>
    bool map_has(const map<const string, T>& m, string key) {
        return map_has<const string, T>(m, key);
    }

    template <typename KeyT, typename T>
    vector<KeyT> map_keys(const map<KeyT, T>& m) {
        vector<KeyT> keys;
        for (const auto& pair : m) keys.push_back(pair.first);
        return keys;
    }
    template <typename T>
    vector<string> map_keys(const map<string, T>& m) {
        return map_keys<string, T>(m);
    }

    template <typename KeyT, typename T>
    bool map_key_exists(const map<KeyT, T>& m, const KeyT& key) {
        return m.find(key) != m.end();
    }
    template <typename T>
    bool map_key_exists(const map<string, T>& m, const string& key) {
        return map_key_exists<string, T>(m, key);
    }

    typedef map<const string, string> args_t;
    typedef map<const char, string> args_shortcuts_t;

    args_t args_parse(int argc, const char* argv[], const args_shortcuts_t* shorts = NULL) {
        args_t args;
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] != '-') continue;
            string key = string(argv[i]).substr(argv[i][1] == '-' ? 2 : 1);
            if (key.empty()) throw ERROR("Empty argument key");
            if (argv[i][1] != '-') {
                if (key.length() != 1) 
                    throw ERROR("Invalid argument key: " + string(argv[i]));
                if (shorts && shorts->count(key[0])) key = shorts->at(key[0]);
            }
            string value = i < argc - 1 && argv[i + 1][0] != '-' ? argv[i + 1] : "";
            args[key] = value;
        }
        return args;
    }

    bool args_has(const args_t& args, const string& key) {
        return args.count(key);
    }

    const string args_get(const args_t& args, const string& key, const string* defval = NULL) {
        if (args.count(key)) return args.at(key);
        if (defval) return *defval;
        throw ERROR("Missing argument: " + key);
    }

    const string args_get(const args_t& args, const string& key, const string& defval) {
        const string defstr = string(defval);
        return args_get(args, key, &defstr);
    }

    const string args_get(const args_t& args, const string& key, const char* defval) {
        const string defstr = string(defval);
        return args_get(args, key, &defstr);
    }

    string zenity(const string& args, const string& err = "/dev/null") {
        return trim(exec("echo $(zenity " + args + " 2>" + err + ")"));
    }

    string zenity_combo(
        const string& title, 
        const string& text,
        const string& label, 
        const vector<string>& items,
        const string& err = "/dev/null"
    ) {
        const string cmd = 
            str_replace(
                "--forms "
                    "--title='{title}' "
                    "--text='{text}' "
                    "--add-combo='{label}' "
                    "--combo-values='{items}' ",
                {
                    { "{title}", str_sanitize(title) },
                    { "{text}", str_sanitize(text) },
                    { "{label}", str_sanitize(label) },
                    { "{items}", vector_concat(items, "|", str_sanitize) },
                }
            );

        return zenity(cmd, err);
    }

    string zenity_file_selection(const string& title = "", const string& err = "/dev/null") {
        return zenity("--file-selection --title=\"" + str_sanitize(title) + "\"", err);
    }

    FILE* zenity_progress(const string& title) {
        return pipe_exec("zenity --progress --auto-close --title=\"" + str_sanitize(title) + "\"");
    }

    bool zenity_progress_update(FILE* pipe, int percent) {
        return pipe_send(pipe, to_string(percent));
    }

    bool zenity_progress_update(FILE* pipe, const string& status) {
        return pipe_send(pipe, "# " + status);
    }

    int zenity_progress_close(FILE* pipe) {
        if (!zenity_progress_update(pipe, 100)) return 0;
        return pipe_close(pipe);
    }

    template <typename T>
    class Factory {
    protected:
        vector<T*> instances;
    public:
        T* create() {
            T* instance = new T;
            instances.push_back(instance);
            return instance;
        }
        
        template <typename... Args>
        T* create(Args... args) {
            T* instance = new T(args...);
            instances.push_back(instance);
            return instance;
        }

        void destroy(T* instance) {
            auto it = find(instances.begin(), instances.end(), instance);
            if (it == instances.end()) return;
            delete *it;
            instances.erase(it);
        }

        void destroy() {
            vector_destroy<T>(instances);
        }

        virtual ~Factory() {
            // for (T* instance : instances) {
            //     delete instance;
            // }
            // vector_destroy<T>(instances);
            destroy();
        }

        vector<T*>& getInstances() const {
            return instances;
        }

    };


    #define EXPORT_CLASS(clazz) \
        extern "C" clazz* create##clazz(void* context = nullptr) { \
            return new clazz(context); \
        } \
        extern "C" void destroy##clazz(void* instance, void* context) { \
            if (instance) delete (clazz*)instance; \
            if (context) delete (clazz::Args*)context; \
        }

        
    class SharedFactory {
    protected:
        
        typedef struct { void* instance; void* context; } InstanceAndContext;

        typedef void* (*SharedCreator)(void*);
        typedef void (*SharedDestroyer)(void*, void*);

        typedef struct {
            vector<InstanceAndContext> instanceAndContexts;
            void* handle = NULL;
            SharedCreator creator;
            SharedDestroyer destroyer;
        } SharedInstance;

        map<const string, SharedInstance> imports;

    public:

        SharedFactory() {} 

        virtual ~SharedFactory() {
            for (const auto& pair: imports) {
                const SharedInstance& import = pair.second;
                for (const InstanceAndContext& ic: import.instanceAndContexts)
                    if (import.destroyer) import.destroyer(ic.instance, ic.context);
                if (import.handle) dlclose(import.handle);
            }
        }

        // template<typename A = void*>
        void* create(const string& path, const string& clazz, void* context = NULL) {
            const string source = path_normalize(path + "/" + clazz + ".so");

            if (map_has(imports, source)) {
                void* instance = imports[source].creator(context);
                imports[source].instanceAndContexts.push_back({ instance, context });
                return instance;
            }

            const string create = "create" + clazz;
            const string destroy = "destroy" + clazz;

            void* handle = dlopen((source).c_str(), RTLD_LAZY);
            if (!handle) throw ERROR("Unable to open: " + source + " - " + dlerror());

            SharedCreator creator = (SharedCreator)(dlsym(handle, string(create).c_str()));
            if (!creator) throw ERROR("Unable to create: " + clazz + " - " + dlerror());

            SharedDestroyer destroyer = (SharedDestroyer)(dlsym(handle, string(destroy).c_str()));
            
            void* instance = creator(context);
            if (!instance) throw ERROR("Unable to instanciate: " + clazz);

            imports[source] =  {{ { instance, context } }, handle, creator, destroyer };

            return instance;
        }

    };
}
