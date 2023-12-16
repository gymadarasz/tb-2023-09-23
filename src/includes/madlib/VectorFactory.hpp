#pragma once

#include <vector>

#include "vectors.hpp"

using namespace std;

namespace madlib {

    template <typename T>
    class VectorFactory {
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

        virtual ~VectorFactory() {
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

}