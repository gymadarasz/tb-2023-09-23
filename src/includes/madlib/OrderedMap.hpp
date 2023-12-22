#pragma once

#include "maps.hpp"

using namespace std;

namespace madlib {

    template <typename KeyT, typename ValueT>
    class OrderedMap {
    protected:
        map<KeyT, ValueT> data;
        vector<KeyT> order;
    public:
        void insert(const KeyT& key, const ValueT& value) {
            // Check if the key already exists
            if (data.find(key) == data.end()) {
                // Insert the key-value pair into the map
                data[key] = value;
                // Store the key in the order vector
                order.push_back(key);
            } else {
                // Key already exists, update the value
                data[key] = value;
            }
        }

        const vector<KeyT>& getOrder() const {
            return order;
        }

        ValueT& at(const KeyT& key) {
            if (!has(key)) throw ERROR("Undefined key: ");
            return data.at(key);
        }

        bool has(const KeyT& key) const {
            return map_has(data, key);
        }
    };

}