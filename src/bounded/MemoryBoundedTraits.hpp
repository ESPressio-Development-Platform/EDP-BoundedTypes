#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

namespace ESPressio::Bounded {

    /// Classifies whether a Type is a recursively ownable bounded value, a non-owning view, or has not been qualified.
    enum class MemoryBoundedCategory : std::uint8_t {
        Unqualified = 0,
        BoundedValue = 1,
        NonOwningView = 2
    };

    /// Describes the bounded-memory qualification of a Type.
    template<class TValue>
    struct MemoryBoundedTraits {

        /// Classifies arithmetic, enumeration, and null pointer scalar Types as intrinsically bounded values; all other unknown Types remain unqualified.
        static constexpr MemoryBoundedCategory Category =
            (
                std::is_arithmetic_v<std::remove_cv_t<TValue>> ||
                std::is_enum_v<std::remove_cv_t<TValue>> ||
                std::is_same_v<std::remove_cv_t<TValue>, std::nullptr_t>
            )
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;

        /// Indicates whether correct use of the Type depends on the lifetime of externally-owned state.
        static constexpr bool HasExternalLifetimeDependencies = false;

    };

    /// Reports whether a Type is compiler-qualified as a recursively ownable memory-bounded value.
    template<class TValue>
    inline constexpr bool IsMemoryBoundedValue =
        !std::is_reference_v<TValue> &&
        (
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::Category ==
            MemoryBoundedCategory::BoundedValue
        );

    /// Reports whether a Type is explicitly classified as a non-owning view or handle.
    template<class TValue>
    inline constexpr bool IsNonOwningView =
        !std::is_reference_v<TValue> &&
        (
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::Category ==
            MemoryBoundedCategory::NonOwningView
        );

    namespace Detail {

        /// Reports whether every Type in a parameter pack is a compiler-qualified memory-bounded value.
        template<class... TValueTypes>
        inline constexpr bool AreAllMemoryBoundedValues = (
            true &&
            ... &&
            IsMemoryBoundedValue<TValueTypes>
        );

        /// Reports whether any Type in a parameter pack carries explicit external lifetime dependencies.
        template<class... TValueTypes>
        inline constexpr bool HasAnyExternalLifetimeDependency = (
            false ||
            ... ||
            MemoryBoundedTraits<std::remove_cv_t<TValueTypes>>::HasExternalLifetimeDependencies
        );

    } // ESPressio::Bounded::Detail

    /// Provides a reusable certification base for a user-defined Type whose complete owned member graph is explicitly declared.
    template<bool THasExternalLifetimeDependencies, class... TOwnedMemberTypes>
    struct MemoryBoundedValueDeclaration {

        static_assert(
            Detail::AreAllMemoryBoundedValues<TOwnedMemberTypes...>,
            "Every declared owned member Type must itself be a certified memory-bounded value."
        );

        /// Classifies the certified Type as a recursively ownable bounded value.
        static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::BoundedValue;

        /// Indicates whether this Type or any recursively owned member depends on externally-owned lifetime state.
        static constexpr bool HasExternalLifetimeDependencies =
            THasExternalLifetimeDependencies ||
            Detail::HasAnyExternalLifetimeDependency<TOwnedMemberTypes...>;

    };

    /// Classifies raw pointers as non-owning handles rather than recursively ownable bounded values.
    template<class TValue>
    struct MemoryBoundedTraits<TValue*> {

        /// Raw pointers refer to externally-owned storage and therefore do not qualify as owned bounded values.
        static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;

        /// A raw pointer always depends on the externally-managed lifetime of its pointee.
        static constexpr bool HasExternalLifetimeDependencies = true;

    };

    /// Classifies string views as non-owning views rather than recursively ownable bounded values.
    template<class TCharacter, class TTraits>
    struct MemoryBoundedTraits<std::basic_string_view<TCharacter, TTraits>> {

        /// A string view does not own the storage it references.
        static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;

        /// A string view depends on the lifetime of externally-owned character storage.
        static constexpr bool HasExternalLifetimeDependencies = true;

    };

    /// Classifies reference wrappers as non-owning handles rather than recursively ownable bounded values.
    template<class TValue>
    struct MemoryBoundedTraits<std::reference_wrapper<TValue>> {

        /// A reference wrapper does not own the object it references.
        static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::NonOwningView;

        /// A reference wrapper depends on the lifetime of an externally-owned object.
        static constexpr bool HasExternalLifetimeDependencies = true;

    };

    /// Recursively qualifies a built-in fixed-extent array when its element Type is itself memory-bounded.
    template<class TValue, std::size_t TExtent>
    struct MemoryBoundedTraits<TValue[TExtent]> {

        /// The array qualifies only when every element Type is compiler-qualified as memory-bounded.
        static constexpr MemoryBoundedCategory Category =
            IsMemoryBoundedValue<TValue>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;

        /// Propagates external lifetime dependencies from the element Type.
        static constexpr bool HasExternalLifetimeDependencies =
            IsMemoryBoundedValue<TValue> &&
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::HasExternalLifetimeDependencies;

    };

    /// Recursively qualifies a standard fixed-extent array when its element Type is itself memory-bounded.
    template<class TValue, std::size_t TExtent>
    struct MemoryBoundedTraits<std::array<TValue, TExtent>> {

        /// The array qualifies only when every element Type is compiler-qualified as memory-bounded.
        static constexpr MemoryBoundedCategory Category =
            IsMemoryBoundedValue<TValue>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;

        /// Propagates external lifetime dependencies from the element Type.
        static constexpr bool HasExternalLifetimeDependencies =
            IsMemoryBoundedValue<TValue> &&
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::HasExternalLifetimeDependencies;

    };

    /// Recursively qualifies an optional when its contained value Type is itself memory-bounded.
    template<class TValue>
    struct MemoryBoundedTraits<std::optional<TValue>> {

        /// The optional qualifies only when its contained value Type is memory-bounded.
        static constexpr MemoryBoundedCategory Category =
            IsMemoryBoundedValue<TValue>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;

        /// Propagates external lifetime dependencies from the optional value Type.
        static constexpr bool HasExternalLifetimeDependencies =
            IsMemoryBoundedValue<TValue> &&
            MemoryBoundedTraits<std::remove_cv_t<TValue>>::HasExternalLifetimeDependencies;

    };

    /// Recursively qualifies a pair when both owned value Types are themselves memory-bounded.
    template<class TFirst, class TSecond>
    struct MemoryBoundedTraits<std::pair<TFirst, TSecond>> {

        /// The pair qualifies only when both value Types are memory-bounded.
        static constexpr MemoryBoundedCategory Category =
            (
                IsMemoryBoundedValue<TFirst> &&
                IsMemoryBoundedValue<TSecond>
            )
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;

        /// Propagates external lifetime dependencies from both pair values.
        static constexpr bool HasExternalLifetimeDependencies =
            (
                IsMemoryBoundedValue<TFirst> &&
                MemoryBoundedTraits<std::remove_cv_t<TFirst>>::HasExternalLifetimeDependencies
            ) ||
            (
                IsMemoryBoundedValue<TSecond> &&
                MemoryBoundedTraits<std::remove_cv_t<TSecond>>::HasExternalLifetimeDependencies
            );

    };

    /// Recursively qualifies a tuple when every owned element Type is itself memory-bounded.
    template<class... TValueTypes>
    struct MemoryBoundedTraits<std::tuple<TValueTypes...>> {

        /// The tuple qualifies only when every owned element Type is memory-bounded.
        static constexpr MemoryBoundedCategory Category =
            Detail::AreAllMemoryBoundedValues<TValueTypes...>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;

        /// Propagates external lifetime dependencies from all tuple element Types.
        static constexpr bool HasExternalLifetimeDependencies =
            Detail::AreAllMemoryBoundedValues<TValueTypes...> &&
            Detail::HasAnyExternalLifetimeDependency<TValueTypes...>;

    };

    /// Recursively qualifies a variant when every possible alternative Type is itself memory-bounded.
    template<class... TValueTypes>
    struct MemoryBoundedTraits<std::variant<TValueTypes...>> {

        /// The variant qualifies only when every possible alternative Type is memory-bounded.
        static constexpr MemoryBoundedCategory Category =
            Detail::AreAllMemoryBoundedValues<TValueTypes...>
                ? MemoryBoundedCategory::BoundedValue
                : MemoryBoundedCategory::Unqualified;

        /// Propagates external lifetime dependencies from all possible alternative Types.
        static constexpr bool HasExternalLifetimeDependencies =
            Detail::AreAllMemoryBoundedValues<TValueTypes...> &&
            Detail::HasAnyExternalLifetimeDependency<TValueTypes...>;

    };

    /// Classifies std::monostate as an intrinsically bounded empty value.
    template<>
    struct MemoryBoundedTraits<std::monostate> {

        /// std::monostate is a self-contained bounded value.
        static constexpr MemoryBoundedCategory Category = MemoryBoundedCategory::BoundedValue;

        /// std::monostate has no external lifetime dependency.
        static constexpr bool HasExternalLifetimeDependencies = false;

    };

} // ESPressio::Bounded
