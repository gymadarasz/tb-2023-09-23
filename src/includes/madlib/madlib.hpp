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


using namespace std;
using namespace chrono;

#define ERROR(msg) runtime_error(string(msg) + ": " + string(__FILE__) + ":" + to_string(__LINE__)); 
#define ERR_UNIMP ERROR("Unimplemented");
#define ERR_MISSING ERROR("Missing");
#define ERR_INVALID ERROR("Invalid");

namespace madlib {
    
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

    const ms_t MS_PER_SEC = 1000;
    const ms_t MS_PER_MIN = 60 * MS_PER_SEC;
    const ms_t MS_PER_HOUR = 60 * MS_PER_MIN;
    const ms_t MS_PER_DAY = 24 * MS_PER_HOUR;
    const ms_t MS_PER_WEEK = 7 * MS_PER_DAY;

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

        for (const auto& entry : vals) {
            result = str_replace(result, entry.first, entry.second);
        }

        return result;
    }

    bool str_start_with(const std::string& needle, const std::string& haystack) {
        return (haystack.length() >= needle.length()) && (haystack.compare(0, needle.length(), needle) == 0);
    }


    string trim(const string& str) {
        // Find the first non-whitespace character from the beginning
        size_t start = 0;
        while (start < str.length() && isspace(str[start])) {
            start++;
        }

        // Find the first non-whitespace character from the end
        size_t end = str.length();
        while (end > start && isspace(str[end - 1])) {
            end--;
        }

        // Extract the trimmed substring
        return str.substr(start, end - start);
    }
        
    string normalize_datetime(const string& datetime) {
        string tpl = "0000-01-01 00:00:00.000";
        const string trimed = trim(datetime);

        const size_t trimedLength = trimed.length();
        for (size_t i = 0; i < trimedLength; i++) {
            tpl[i] = trimed[i];
        }

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
        if (local) {
            localtime_r(&sec, &converted_time);
        }
        else {
            gmtime_r(&sec, &converted_time);
        }

        ostringstream oss;
        oss << put_time(&converted_time, fmt);

        if (millis) {
            oss << "." << setfill('0') << setw(3) << mil;
        }

        return oss.str();
    }

    inline void sleep(unsigned long ms) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    }

    inline double randd(double min, double max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dis(min, max);
        return dis(gen);
    }

    inline long randl(long min, long max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<long> dis(min, max);
        return dis(gen);
    }

    inline int randi(int min, int max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dis(min, max);
        return dis(gen);
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

    template<typename T>
    T min(vector<T> values) {
        if (values.empty()) throw ERROR("Empty set has no minimum");
        T minVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] < minVal) minVal = values[i];
        return minVal;
    }

    template<typename T>
    T max(vector<T> values) {
        if (values.empty()) throw ERROR("Empty set has no maximun");
        T maxVal = values[0];
        for (size_t i = 1; i < values.size(); ++i) if (values[i] > maxVal) maxVal = values[i];
        return maxVal;
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

    string exec(const string& command, bool captureOutput = true, bool showOutput = false) {
        array<char, 128> buffer;
        string result;

        // Open the command for reading, redirecting stderr to stdout
        FILE* pipe = popen((command + " 2>&1").c_str(), "r");
        if (!pipe) {
            throw ERROR("Failed to execute command.");
        }

        // Read the output (if requested)
        if (captureOutput || showOutput) {
            while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
                result += buffer.data();
            }
            if (showOutput) cout << result;
        }

        // Close the pipe
        pclose(pipe);

        return result;
    }

    bool is_numeric(const string& str) {
        // Regular expression pattern for numeric strings
        regex pattern(R"(\s*[-+]?\d+\s*|\s*[-+]?(\d*\.\d+|\d+\.\d*)([eE][-+]?\d+)?\s*)");
        
        return regex_match(str, pattern);
    }

    string str_to_lower(const string& str) {
        string ret = "";
        for (size_t i = 0; i < str.length(); i++) {
            if (isupper(str[i])) ret += (char)tolower(str[i]);
            else ret += str[i];
        }
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

    #define __DIR__ path_normalize(path_extract(__FILE__))
    
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

        if (lastDotPos != string::npos) {
            // Create a string with the part before the last dot and the extension
            return filename.substr(0, lastDotPos) + "." + extension;
        } 
        // If there's no dot in the file name, simply append the extension
        return filename + "." + extension;
    }

    string path_normalize(const string& filepath) {
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

    string path_extract(const string& filepath) {
        size_t lastSlashPos = filepath.find_last_of('/');
        if (lastSlashPos != string::npos) {
            return filepath.substr(0, lastSlashPos);
        }
        // If there's no directory separator, return an empty string or the whole path, depending on your preference.
        return "";  // Alternatively, you can return filepath;
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
            throw ERROR("Error creating directory: " + string(e.what()));
        }
    }

    time_t file_get_mtime(const string& filePath) {
        struct stat fileInfo;
        if (stat(filePath.c_str(), &fileInfo) != 0) {
            throw ERROR("Error getting file information.");
        }
        
        return fileInfo.st_mtime;
    }

    void file_put_contents(const string& filename, const string& data, bool append = false) {
        // Check if the file is a symlink
        if (filesystem::is_symlink(filename))
            throw ERROR("Error: Symlink detected. Refusing to open: " + filename);

        // Check if the file is a special file (e.g., character or block device)
        if (filesystem::is_character_file(filename) || filesystem::is_block_file(filename))
            throw ERROR("Error: Special file detected. Refusing to open: " + filename);
            
        ofstream file;
        // FlawFinder: ignore
        file.open(filename, append ? ios::out | ios::app : ios::out);

        if (!file.is_open())
            throw ERROR("Error: Unable to open file for writing: " + filename);

        file << data;
        file.close();
    }

    string file_get_contents(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw ERROR("Error: Unable to open file for reading: " + filename);
        }

        string content((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
        file.close();

        return content;
    }

    string vector_concat(const vector<string>& strings, const string& separator = " ") {
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
    vector<T>& vector_load(const string &filename, vector<T>& data) {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) throw ERROR("Error opening file for reading: " + filename);
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
        if (v.size() > 0) throw ERROR("EEJH??");
    }

    template<typename T>
    bool vector_has(const vector<T>& v, size_t at) {
        return v.size() > at;
    }  

    template<typename KeyT, typename T>
    bool map_has(const map<KeyT, T>& m, KeyT key) {
        return !m.empty() && m.count(key) > 0;
    }
    template<typename T>
    bool map_has(const map<string, T>& m, string key) {
        return map_has<string, T>(m, key);
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

    string zenity(const string& args, const string& err = "/dev/null") {
        return trim(exec("echo $(zenity " + args + " 2>" + err + ")"));
    }

    string zenity_file_selection(const string& title = "", const string& err = "/dev/null") {
        return zenity("--file-selection --title '" + title + "'", err);
    }

    class Log {
    protected:
        string filename;

    public:
        Log(const string& f = "app.log") : filename(f) {}

        const Log& date() const {
            write("[" + ms_to_datetime() + "] ");
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        const Log& writeln(Args... args) const {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            write(message + "\n");
            return *this;
        }

        // Variadic template for writeln method
        template <typename... Args>
        const Log& write(Args... args) const {
            // Concatenate all arguments into a single string
            string message = concat(args...);
            file_put_contents(filename, message, true);
            return *this;
        }
    } logger;

    #define LOG(...) logger.date().writeln(__VA_ARGS__)

    // class CommandLineApplication {
    // public:
    //     typedef vector<string> CommandArguments;
    //     typedef int (*CommandFunction)(CommandLineApplication*, CommandArguments);
        
    //     class Command {
    //     protected:
    //         CommandFunction function;
    //         string description = "";
    //     public:
    //         Command(
    //             CommandFunction function, string description = ""
    //         ):
    //             function(function), description(description) 
    //         {}

    //         CommandFunction getFunction() const {
    //             return function;
    //         }

    //         const string& getDescription() const {
    //             return description;
    //         }
    //     };

    //     typedef map<string, Command> CommandMap;

    //     const Command DefaultHelpCommand = Command(
    //         help, "Default command that shows this screen");

    // protected:
    //     string execName;
    //     CommandMap cmds;
    //     CommandArguments args = {};
    //     int result = 1;
    // public:
    //     CommandLineApplication(
    //         int argc, char* argv[], CommandMap cmds
    //     ): 
    //         cmds(cmds) 
    //     {
    //         if (!map_key_exists(this->cmds, "help"))
    //             this->cmds.insert(make_pair("help", DefaultHelpCommand));
    //         execName = argv[0];
    //         for (int i = 1; i < argc; i++) args.push_back(argv[i]);
    //         Command cmd = vector_has(args, 0) && map_has(this->cmds, args.at(0))
    //             ? this->cmds.at(args.at(0)) : DefaultHelpCommand;
    //         try {
    //             CommandFunction cmdf = cmd.getFunction();
    //             result = cmdf(this, args);
    //         } catch (exception& e) {
    //             logger.date().writeln("Command error: " + string(e.what()));
    //             exit(1);
    //         }
    //     }

    //     static int help(CommandLineApplication* that, CommandArguments) {
    //         string help = str_replace(
    //             "Usages:\n"
    //             "\t  $ {execName} command [...arguments]\n"
    //             "Commands:\n",
    //             "{execName}", that->execName
    //         );
    //         for (const auto& cmd: that->cmds) {
    //             help += 
    //                 "\n - " + cmd.first + 
    //                 "\n\t" + str_replace(cmd.second.getDescription(), "\n", "\n\t") + "\n";
    //         }
    //         cout << help;
    //         return 0;
    //     }

    //     int getResult() const {
    //         return result;
    //     }
    // };

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
            if (it != instances.end()) {
                delete *it;
                instances.erase(it);
            }
        }

        void destroy() {
            vector_destroy<T>(instances);
        }

        ~Factory() {
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

    inline int reg_match(const string& pattern, const string& str, vector<string>* matches = nullptr) {
        regex r(pattern);
        smatch m;
        if (regex_search(str, m, r)) {
            if (matches != nullptr) {
                // Clear the vector before adding new matches
                matches->clear();
                for (unsigned int i = 0; i < m.size(); i++) {
                    matches->push_back(m[i].str());
                }
            }
            return 1;
        }
        return 0;
    }

    typedef map<const string, string> args_t;
    typedef map<const char, string> args_shortcuts_t;

    args_t args_parse(int argc, const char* argv[], const args_shortcuts_t* shorts = NULL) {
        args_t args;
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                string key = string(argv[i]).substr(argv[i][1] == '-' ? 2 : 1);
                if (key.empty()) throw ERROR("Empty argument key");
                if (argv[i][1] != '-') {
                    if (key.length() != 1) throw ERROR("Invalid argument key: " + string(argv[i]));
                    if (key.length() == 1 && shorts && shorts->count(key[0])) key = shorts->at(key[0]);
                }
                string value = i < argc - 1 && argv[i + 1][0] != '-' ? argv[i + 1] : "";
                args[key] = value;
            }
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
}
