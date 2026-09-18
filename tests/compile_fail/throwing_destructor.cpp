#include "ESPressio_BoundedTypes.hpp"

struct ThrowingDestructor final {
    ~ThrowingDestructor() noexcept(false) {
    }
};

namespace ESPressio::Bounded {

    template<>
    struct MemoryBoundedTraits<::ThrowingDestructor> : MemoryBoundedValueDeclaration<false> {
    };

} // ESPressio::Bounded

ESPressio::Bounded::Vector<ThrowingDestructor, 4U> InvalidValue;

int main() {
    return 0;
}
