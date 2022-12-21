#pragma once

#include "fields/field.hpp"

namespace profit {

using DepositId = int16_t;
constexpr DepositId NO_COMPONENT = -1;
using ComponentMap = Field<DepositId, NO_COMPONENT, NO_COMPONENT>;

}  // namespace profit