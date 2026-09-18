#include "ESPressio_BoundedTypes.hpp"

struct ThrowingCopy final {

    ThrowingCopy() noexcept = default;

    ThrowingCopy(
        const ThrowingCopy&
    ) noexcept(false) {
    }

    ThrowingCopy(
        ThrowingCopy&&
    ) noexcept = default;

    ~ThrowingCopy() noexcept = default;

};

namespace ESPressio::Bounded {

    template<>
    struct MemoryBoundedTraits<::ThrowingCopy> : MemoryBoundedValueDeclaration<false> {
    };

} // ESPressio::Bounded

int main() {
    ESPressio::Bounded::Vector<ThrowingCopy, 4U> values;
    ThrowingCopy value;

    values.PushBack(value);
    return 0;
}
