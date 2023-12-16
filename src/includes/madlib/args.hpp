#pragma once

#include <map>
#include "ERROR.hpp"

using namespace std;

namespace madlib {


    typedef map<const string, string> args_t;
    typedef map<const char, string> args_shortcuts_t;

    args_t args_parse(int argc, const char* argv[], const args_shortcuts_t* shorts = nullptr) {
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

    const string args_get(const args_t& args, const string& key, const string* defval = nullptr) {
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