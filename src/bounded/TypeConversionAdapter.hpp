#pragma once

#include <type_traits>
#include <utility>

namespace ESPressio::Bounded {

    /// Marker result Type used only when no conversion adapter has been supplied for a source/target Type pair.
    struct UnsupportedTypeConversionResult final {
    };

    /// Defines the default unavailable conversion adapter for a source/target Type pair.
    template<class TSource, class TTarget>
    struct TypeConversionAdapter {

        /// Result Type returned by an unavailable adapter; successful specializations replace this with an operation-specific enum.
        using ResultType = UnsupportedTypeConversionResult;

        /// Indicates whether the source/target conversion has been supplied by the Type-owning integration library.
        static constexpr bool IsAvailable = false;

        /// Indicates whether the supplied conversion is guaranteed not to throw.
        static constexpr bool IsNoexcept = true;

        /// Rejects conversion when no source/target specialization has been supplied.
        static ResultType Convert(
            const TSource& source,
            TTarget& target
        ) = delete;

    };

    /// Reports whether a conversion adapter is available for the supplied source/target Type pair.
    template<class TSource, class TTarget>
    inline constexpr bool IsTypeConversionAvailable = TypeConversionAdapter<
        TSource,
        TTarget
    >::IsAvailable;

} // ESPressio::Bounded
