#include <cstdint>

#include <ESPressio_BoundedTypes.hpp>

namespace TestSupport {

    /// Minimal source Type used to exercise a legacy adapter without generic success interpretation.
    struct Source final {
    };

    /// Result vocabulary deliberately remains integration-specific.
    enum class ConversionResult : std::uint8_t {
        Succeeded = 0,
        Failed = 1
    };

} // TestSupport

namespace ESPressio::Bounded {

    /// Declares an available legacy adapter without the stronger generic success predicate.
    template<>
    struct TypeConversionAdapter<TestSupport::Source, int> final {

        /// Operation-specific result Type owned by the integration.
        using ResultType = TestSupport::ConversionResult;

        /// Indicates that the conversion itself remains available.
        static constexpr bool IsAvailable = true;

        /// Indicates that this test conversion is non-throwing.
        static constexpr bool IsNoexcept = true;

        /// Produces a deterministic successful conversion result.
        static ResultType Convert(
            const TestSupport::Source& source,
            int& target
        ) noexcept {
            static_cast<void>(source);
            target = 1;
            return ResultType::Succeeded;
        }

    };

} // ESPressio::Bounded

/// Intentionally fails because generic success interpretation was not supplied by the adapter owner.
int main() {
    const auto result = TestSupport::ConversionResult::Succeeded;

    return ESPressio::Bounded::IsTypeConversionSuccessful<TestSupport::Source, int>(
        result
    ) ? 0 : 1;
}
