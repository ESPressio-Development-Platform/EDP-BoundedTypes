#include "ESPressio_BoundedTypes.hpp"

struct UncertifiedValue final {
    int Value = 0;
};

ESPressio::Bounded::Vector<UncertifiedValue, 4U> InvalidValue;

int main() {
    return 0;
}
