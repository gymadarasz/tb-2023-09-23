#pragma once

#include <vector>
#include <map>
#include <dlfcn.h>

#include "../../../libs/clib/clib/err.hpp"

using namespace std;
using namespace clib;

namespace madlib {

    template <typename InstanceT>
    class Factory {
    protected:
        struct LibraryInfo {
            void* handle;
            vector<InstanceT*> instances;
        };

        map<string, LibraryInfo> libraryHandles;

        // Helper function to check if an instance is in the map
        bool isInstanceInMap(InstanceT* instance) {
            for (const auto& entry : libraryHandles) {
                const auto& instances = entry.second.instances;
                if (find(instances.begin(), instances.end(), instance) != instances.end()) {
                    return true;
                }
            }
            return false;
        }

        template <typename U>
        bool deleteInstanceSafely(U* instance) {
            // TODO: Perform any necessary cleanup and delete the instance
            delete instance;

            // Return true if deletion was successful
            return true; // Change this logic based on your actual deletion requirements
        }

    public:

        virtual ~Factory() {
            // Close all loaded libraries and delete instances
            for (const auto& entry : libraryHandles) {
                for (InstanceT* instance : entry.second.instances) {
                    deleteInstanceSafely(instance);
                }
                dlclose(entry.second.handle);
            }
        }

        template <typename... Args>
        InstanceT* createInstance(const string& libraryName, Args... args) {
            // Check if the library is already loaded
            if (libraryHandles.find(libraryName) == libraryHandles.end()) {
                void* handle = dlopen(libraryName.c_str(), RTLD_LAZY);
                if (!handle) {
                    throw ERROR("Couldn't open the library '" + libraryName + "': " + dlerror());
                }
                libraryHandles[libraryName] = {handle, {}};
            }

            using CreateFunction = InstanceT* (*)(Args...);
            // Use the constructor name as the symbol to load
            string constructorSymbol = "create" + filename_extract(libraryName, true);
            CreateFunction createFunc = (CreateFunction)dlsym(libraryHandles[libraryName].handle, constructorSymbol.c_str());

            const char* dlsym_error = dlerror();
            if (dlsym_error) {
                throw ERROR("Error loading dlsym: " + string(dlsym_error));
            }

            InstanceT* instance = createFunc(args...);
            libraryHandles[libraryName].instances.push_back(instance);
            return instance;
        }
        
        template <typename... Args>
        InstanceT* updateInstance(InstanceT* existingInstance, const string& libraryName, Args... args) {
            if (existingInstance) {
                // Check if the existing instance is in the map
                if (!isInstanceInMap(existingInstance)) {
                    throw ERROR("Existing instance not found in the factory map.");
                }

                // Delete the existing instance
                deleteInstance(existingInstance);
            }

            // Call createInstance to create a new instance
            return createInstance(libraryName, args...);
        }

        void deleteInstance(InstanceT* instance) {
            // Find the library info containing the instance
            for (auto& entry : libraryHandles) {
                auto& instances = entry.second.instances;
                auto it = find(instances.begin(), instances.end(), instance);
                if (it != instances.end()) {
                    instances.erase(it);

                    // Delete instance and check for success
                    if (deleteInstanceSafely(instance)) {
                        // Remove entry from map if instances vector becomes empty
                        if (instances.empty()) {
                            dlclose(entry.second.handle);
                            libraryHandles.erase(entry.first);
                        }
                    }
                    return;
                }
            }

            throw ERROR("Instance not found for deletion.");
        }

    };

}