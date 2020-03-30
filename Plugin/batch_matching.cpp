#include "batch_matching.h"

batch_matching & batch_matching::get_instance()
{
    static batch_matching instance;

    return instance;
}

void batch_matching::registerStep(const char * pattern, std::size_t desiredSize, callback_type callback)
{
    _steps.emplace_back(pattern, desiredSize, callback, std::vector<memory_pointer>{});
}

void batch_matching::clear()
{
    _steps.clear();
}

void batch_matching::peformSearch()
{
    byte_pattern pattern_obj;

    for (auto &step : _steps)
    {
        pattern_obj.find_pattern(std::get<0>(step).c_str());
        std::get<3>(step) = pattern_obj.get();
    }
}

bool batch_matching::isAllSucceed() const
{
    return std::all_of(_steps.begin(), _steps.end(),
        [](const step_type &step)
    {
        return std::get<1>(step) == std::get<3>(step).size();
    });
}

void batch_matching::runCallbacks()
{
    for (auto &step : _steps)
    {
        std::get<2>(step)(std::get<3>(step));
    }
}
