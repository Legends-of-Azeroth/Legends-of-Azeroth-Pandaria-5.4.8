#include "RangeValues.h"

#include "Playerbots.h"

RangeValue::RangeValue(PlayerbotAI* botAI) : ManualSetValue<float>(botAI, 0.f, "range"), Qualified() {}

std::string const RangeValue::Save()
{
    std::ostringstream out;
    out << value;
    return out.str();
}

bool RangeValue::Load(std::string const text)
{
    value = atof(text.c_str());
    return true;
}
