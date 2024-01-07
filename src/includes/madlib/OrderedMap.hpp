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
            // Use try_emplace to efficiently insert or update the element
            auto result = data.try_emplace(key, value);

            // If the key already existed, update the value
            if (!result.second) {
                result.first->second = value;
            }

            // Always add the key to the order vector
            order.push_back(key);
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