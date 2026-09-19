#include <ESPressio_BoundedTypes.hpp>

namespace Memory = ESPressio::Memory;
namespace Framework = ESPressio::System::CompositionFramework;

/// Invalid stateful provider used to prove BoundedTypes rejects per-value provider state requirements.
class StatefulByteOperationsProvider final : public Framework::Provider<
    Memory::Domain,
    Framework::Provides<
        Framework::Offer<Memory::ByteOperations>
    >
> {

    private:

        /// Deliberate runtime state which makes this provider unsuitable for zero-state bounded values.
        [[maybe_unused]] int _state = 0;

    public:

        /// Copies one range.
        void CopyBytes(
            void*,
            const void*,
            std::size_t
        ) const noexcept {
        }

        /// Moves one range.
        void MoveBytes(
            void*,
            const void*,
            std::size_t
        ) const noexcept {
        }

        /// Fills one range.
        void FillBytes(
            void*,
            std::uint8_t,
            std::size_t
        ) const noexcept {
        }

        /// Compares one range.
        Memory::ByteComparison CompareBytes(
            const void*,
            const void*,
            std::size_t
        ) const noexcept {
            return Memory::ByteComparison::Equal;
        }

};

int main() {
    ESPressio::Bounded::String<8U, StatefulByteOperationsProvider> value;
    static_cast<void>(
        value.Assign("invalid")
    );
    return 0;
}
