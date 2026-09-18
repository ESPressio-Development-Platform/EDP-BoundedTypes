#pragma once

#include <type_traits>

namespace ESPressio::Bounded::Detail {

    /// Removes reference and cv-qualification from a Type for compile-time adapter and trait matching.
    template<class TValue>
    using NormalizedType = std::remove_cv_t<std::remove_reference_t<TValue>>;

} // ESPressio::Bounded::Detail
