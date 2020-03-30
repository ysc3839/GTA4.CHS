#pragma once
#include "byte_pattern.h"
#include <vector>
#include <tuple>
#include <functional>

class batch_matching
{
public:
    typedef std::function<void(const std::vector<memory_pointer> &)> callback_type;
    typedef std::tuple<std::string, std::size_t, callback_type, std::vector<memory_pointer>> step_type;

    static batch_matching &get_instance();
    void registerStep(const char *pattern, std::size_t desiredSize, callback_type callback);
    void clear();
    void peformSearch();
    bool isAllSucceed() const;
    void runCallbacks();

private:
    std::vector<step_type> _steps;
};
