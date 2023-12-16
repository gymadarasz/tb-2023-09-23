#pragma once

#include <map>
#include <string>
#include <vector>

using namespace std;

namespace madlib {


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


}