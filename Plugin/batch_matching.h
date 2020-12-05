#pragma once
#include "../common/stdinc.h"
#include "byte_pattern.h"

class batch_matching
{
public:
    typedef std::vector<memory_pointer> container_type;
    typedef std::function<void(const container_type &)> callback_type;
    typedef std::tuple<std::string, std::size_t, callback_type, container_type> step_type;

    void register_step(const char *pattern, std::size_t desired_size, callback_type callback);
    void clear();
    void perform_search();
    bool is_all_succeed() const;
    void run_callbacks();

private:
    std::vector<step_type> _steps;
};
