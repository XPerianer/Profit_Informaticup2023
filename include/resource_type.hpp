#pragma once
#include <initializer_list>

#include "subtype.hpp"

namespace profit {

using ResourceType = Subtype;

constexpr auto RESOURCE_TYPES = {ResourceType::TYPE0, ResourceType::TYPE1, ResourceType::TYPE2,
                                 ResourceType::TYPE3, ResourceType::TYPE4, ResourceType::TYPE5,
                                 ResourceType::TYPE6, ResourceType::TYPE7};

constexpr size_t RESOURCE_TYPE_COUNT = SUBTYPE_COUNT;
}  // namespace profit
