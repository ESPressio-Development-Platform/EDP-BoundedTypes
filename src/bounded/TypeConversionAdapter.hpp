#pragma once

#include <type_traits>
#include <utility>

namespace ESPressio::Bounded {

    /// Marker result Type used only when no conversion adapter has been supplied for a source/target Type pair.
    struct UnsupportedTypeConversionResult final {
    };

    /// Defines the default unavailable conversion adapter for a source/target Type pair.
    /// @tparam TSource Source Type presented for conversion.
    /// @tparam TTarget Destination Type to be populated by conversion.
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
    /// @tparam TSource Source Type presented for conversion.
    /// @tparam TTarget Destination Type to be populated by conversion.
    template<class TSource, class TTarget>
    inline constexpr bool IsTypeConversionAvailable = TypeConversionAdapter<
        TSource,
        TTarget
    >::IsAvailable;

    /// Reports whether an available adapter exposes the standard generic success predicate required by generic conversion consumers.
    /// @tparam TSource Source Type presented for conversion.
    /// @tparam TTarget Destination Type populated by conversion.
    template<class TSource, class TTarget>
    inline constexpr bool HasTypeConversionSuccessPredicate = []() consteval {
        using Adapter = TypeConversionAdapter<
            TSource,
            TTarget
        >;

        if constexpr (!Adapter::IsAvailable) { return false; }

        if constexpr (
            requires(const typename Adapter::ResultType& result) {
                Adapter::IsSuccessful(result);
            }
        ) {
            return
                std::is_same_v<
                    decltype(
                        Adapter::IsSuccessful(
                            std::declval<const typename Adapter::ResultType&>()
                        )
                    ),
                    bool
                > &&
                noexcept(
                    Adapter::IsSuccessful(
                        std::declval<const typename Adapter::ResultType&>()
                    )
                );
        }

        return false;
    }();

    /// Interprets one adapter-specific result through the adapter's standard success predicate.
    /// @tparam TSource Source Type represented by the adapter result.
    /// @tparam TTarget Destination Type represented by the adapter result.
    /// @param result Operation-specific conversion result produced by the adapter.
    /// @return true only when the adapter identifies the supplied result as successful.
    template<class TSource, class TTarget>
    constexpr bool IsTypeConversionSuccessful(
        const typename TypeConversionAdapter<TSource, TTarget>::ResultType& result
    ) noexcept {
        using Adapter = TypeConversionAdapter<
            TSource,
            TTarget
        >;

        if constexpr (HasTypeConversionSuccessPredicate<TSource, TTarget>) {
            return Adapter::IsSuccessful(
                result
            );
        } else {
            static_assert(
                HasTypeConversionSuccessPredicate<TSource, TTarget>,
                "Generic conversion-success interpretation requires an available TypeConversionAdapter specialization exposing static constexpr bool IsSuccessful(ResultType) noexcept."
            );

            return false;
        }
    }

} // ESPressio::Bounded
