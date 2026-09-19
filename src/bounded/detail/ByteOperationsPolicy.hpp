#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <memory/ByteOperationsProvider.hpp>

namespace ESPressio::Bounded::Detail {

    /// Default stateless ByteOperations provider used by bounded scalar byte sequences.
    using DefaultByteOperationsProvider = ESPressio::Platform::Portable::Memory::ByteOperationsProvider;


    /// Adapts one stateless EDP-Memory ByteOperations provider Type into zero-state compile-time operations.
    ///
    /// Bounded String/Bytes deliberately retain no provider pointer or provider object. The selected provider must
    /// therefore be a stateless, nothrow-default-constructible policy Type.
    ///
    /// @tparam TByteOperationsProvider Concrete provider satisfying the EDP-Memory ByteOperations capability.
    template<class TByteOperationsProvider>
    struct ByteOperationsPolicy final {

        static_assert(
            sizeof(
                ESPressio::Memory::Detail::ByteOperationsProviderTraits<TByteOperationsProvider>
            ) > 0U,
            "Bounded byte operations provider must satisfy the EDP-Memory ByteOperations contract"
        );

        static_assert(
            std::is_empty_v<TByteOperationsProvider>,
            "Bounded String/Bytes require a stateless ByteOperations provider because no runtime provider state is retained"
        );

        static_assert(
            std::is_nothrow_default_constructible_v<TByteOperationsProvider>,
            "Bounded String/Bytes require a nothrow default-constructible ByteOperations provider"
        );


        // Raw byte operations.

        /// Moves one byte range using the selected overlap-safe provider operation.
        static void MoveBytes(
            void* destination,
            const void* source,
            std::size_t byteCount
        ) noexcept {
            TByteOperationsProvider{}.MoveBytes(
                destination,
                source,
                byteCount
            );
        }

        /// Fills one writable byte range using the selected provider operation.
        static void FillBytes(
            void* destination,
            std::uint8_t value,
            std::size_t byteCount
        ) noexcept {
            TByteOperationsProvider{}.FillBytes(
                destination,
                value,
                byteCount
            );
        }

        /// Compares two byte ranges using the selected unsigned-byte lexicographical provider operation.
        static ESPressio::Memory::ByteComparison CompareBytes(
            const void* left,
            const void* right,
            std::size_t byteCount
        ) noexcept {
            return TByteOperationsProvider{}.CompareBytes(
                left,
                right,
                byteCount
            );
        }

    };

} // ESPressio::Bounded::Detail
