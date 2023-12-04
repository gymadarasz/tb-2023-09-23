#pragma once

#define EXPORT_CLASS(class) \
    extern "C" class* create##class(void* context = nullptr) { \
        return new class(context); \
    } \
    extern "C" void destroy##class(class* instance) { \
        delete instance; \
    }

class Shared {
public:
    explicit Shared(void* = nullptr) {}
};