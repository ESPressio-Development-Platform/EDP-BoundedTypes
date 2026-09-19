#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

#include "ESPressio_BoundedTypes.hpp"

namespace {

    /// Counts host heap allocations so bounded operations can be proven allocation-free.
    std::size_t AllocationCount = 0U;

} // anonymous

/// Counts one host allocation and delegates the actual storage request to malloc for test instrumentation.
void* operator new(
    std::size_t size
) {
    ++AllocationCount;

    if (auto* allocation = std::malloc(size)) { return allocation; }

    throw std::bad_alloc();
}

/// Releases storage allocated by the host allocation instrumentation.
void operator delete(
    void* allocation
) noexcept {
    std::free(allocation);
}

/// Releases sized storage allocated by the host allocation instrumentation.
void operator delete(
    void* allocation,
    std::size_t
) noexcept {
    std::free(allocation);
}

namespace TestSupport {

    namespace Memory = ESPressio::Memory;
    namespace Framework = ESPressio::System::CompositionFramework;
    namespace PortableMemory = ESPressio::Platform::Portable::Memory;

    /// Stateless ByteOperations provider used to prove compile-time provider substitution.
    class CountingByteOperationsProvider final : public Framework::Provider<
        Memory::Domain,
        Framework::Provides<
            Framework::Offer<Memory::ByteOperations>
        >
    > {

        public:

            // Operation counters.

            /// Counts CopyBytes calls made through this test provider.
            static std::size_t CopyCount;

            /// Counts MoveBytes calls made through this test provider.
            static std::size_t MoveCount;

            /// Counts FillBytes calls made through this test provider.
            static std::size_t FillCount;

            /// Counts CompareBytes calls made through this test provider.
            static std::size_t CompareCount;


            // Byte operations.

            /// Copies one non-overlapping byte range.
            void CopyBytes(
                void* destination,
                const void* source,
                std::size_t byteCount
            ) const noexcept {
                ++CopyCount;

                PortableMemory::ByteOperationsProvider{}.CopyBytes(
                    destination,
                    source,
                    byteCount
                );
            }

            /// Moves one byte range while permitting source/destination overlap.
            void MoveBytes(
                void* destination,
                const void* source,
                std::size_t byteCount
            ) const noexcept {
                ++MoveCount;

                PortableMemory::ByteOperationsProvider{}.MoveBytes(
                    destination,
                    source,
                    byteCount
                );
            }

            /// Fills one byte range.
            void FillBytes(
                void* destination,
                std::uint8_t value,
                std::size_t byteCount
            ) const noexcept {
                ++FillCount;

                PortableMemory::ByteOperationsProvider{}.FillBytes(
                    destination,
                    value,
                    byteCount
                );
            }

            /// Compares two ranges as unsigned bytes.
            Memory::ByteComparison CompareBytes(
                const void* left,
                const void* right,
                std::size_t byteCount
            ) const noexcept {
                ++CompareCount;

                return PortableMemory::ByteOperationsProvider{}.CompareBytes(
                    left,
                    right,
                    byteCount
                );
            }

    };

    std::size_t CountingByteOperationsProvider::CopyCount = 0U;
    std::size_t CountingByteOperationsProvider::MoveCount = 0U;
    std::size_t CountingByteOperationsProvider::FillCount = 0U;
    std::size_t CountingByteOperationsProvider::CompareCount = 0U;


    /// Tracks non-trivial object lifetime and move behavior while remaining fully memory-bounded.
    class LifetimeValue final {

    private:

        // Stored test value.

        /// Stores the observable payload used by sequence assertions.
        int _value = 0;

    public:

        // Lifetime counters.

        /// Counts currently live test objects.
        static int LiveCount;

        /// Counts completed constructions of any kind.
        static int ConstructionCount;

        /// Counts completed destructions.
        static int DestructionCount;

        /// Creates a default value and records one live object.
        LifetimeValue() noexcept {
            ++LiveCount;
            ++ConstructionCount;
        }

        /// Creates a value with an explicit payload and records one live object.
        explicit LifetimeValue(
            int value
        ) noexcept
            : _value(value) {
            ++LiveCount;
            ++ConstructionCount;
        }

        /// Copies a value and records one live object.
        LifetimeValue(
            const LifetimeValue& source
        ) noexcept
            : _value(source._value) {
            ++LiveCount;
            ++ConstructionCount;
        }

        /// Moves a value, records one live object, and marks the source payload as moved-from for testing.
        LifetimeValue(
            LifetimeValue&& source
        ) noexcept
            : _value(source._value) {
            source._value = -1;
            ++LiveCount;
            ++ConstructionCount;
        }

        /// Copies the observable payload.
        LifetimeValue& operator=(
            const LifetimeValue& source
        ) noexcept {
            _value = source._value;
            return *this;
        }

        /// Moves the observable payload and marks the source payload as moved-from for testing.
        LifetimeValue& operator=(
            LifetimeValue&& source
        ) noexcept {
            _value = source._value;
            source._value = -1;
            return *this;
        }

        /// Records destruction of one live test object.
        ~LifetimeValue() noexcept {
            --LiveCount;
            ++DestructionCount;
        }

        /// Returns the observable payload.
        int Value() const noexcept {
            return _value;
        }

        /// Reports whether two lifetime test values contain equal payloads.
        bool operator==(
            const LifetimeValue& other
        ) const noexcept {
            return _value == other._value;
        }

        /// Reports whether this lifetime test value sorts before another payload.
        bool operator<(
            const LifetimeValue& other
        ) const noexcept {
            return _value < other._value;
        }

    };

    int LifetimeValue::LiveCount = 0;
    int LifetimeValue::ConstructionCount = 0;
    int LifetimeValue::DestructionCount = 0;

    /// Represents an external collaborator whose lifetime is intentionally not owned by a bounded value.
    struct ExternalPolicy final {

        /// Stores one arbitrary policy marker used only by trait tests.
        int Marker = 0;

    };

    /// Demonstrates a custom bounded value that carries an explicit external lifetime dependency.
    class ExternallyDependentValue final {

    private:

        // Owned and non-owned state.

        /// Stores the complete owned bounded payload.
        int _value = 0;

        /// References a collaborator whose lifetime is owned externally.
        ExternalPolicy* _policy = nullptr;

    public:

        /// Creates a bounded value associated with an externally-owned policy.
        ExternallyDependentValue(
            int value,
            ExternalPolicy& policy
        ) noexcept
            : _value(value),
              _policy(
                &policy
            ) {
        }

        /// Returns the bounded payload.
        int Value() const noexcept {
            return _value;
        }

        /// Returns the externally-owned policy.
        const ExternalPolicy& Policy() const noexcept {
            return *_policy;
        }

    };

    /// Represents a source text Type whose conversion adapter is supplied by its owning test integration.
    struct ExternalText final {

        /// References caller-owned text bytes.
        const char* Data = nullptr;

        /// Stores the explicit source byte length.
        std::size_t Size = 0U;

    };

    /// Represents a target text Type whose conversion adapter is supplied by its owning test integration.
    struct ExternalBuffer final {

        /// Stores converted text bytes.
        std::array<char, 16U> Data{};

        /// Stores the converted logical text length.
        std::size_t Size = 0U;

    };

    /// Describes the outcomes of the test integration's external text conversions.
    enum class ExternalTextConversionResult : std::uint8_t {
        Succeeded = 0,
        CapacityExceeded = 1,
        InvalidSource = 2
    };

} // TestSupport

namespace ESPressio::Bounded {

    /// Certifies the non-trivial lifetime test value as owning only one bounded integer.
    template<>
    struct MemoryBoundedTraits<TestSupport::LifetimeValue> : MemoryBoundedValueDeclaration<
        false,
        int
    > {
    };

    /// Certifies the test value as memory-bounded while recording its external policy lifetime dependency.
    template<>
    struct MemoryBoundedTraits<TestSupport::ExternallyDependentValue> : MemoryBoundedValueDeclaration<
        true,
        int
    > {
    };

    /// Converts externally-owned text into a bounded String without making EDP-BoundedTypes depend on the external Type.
    template<std::size_t TCapacity>
    struct TypeConversionAdapter<TestSupport::ExternalText, String<TCapacity>> {

        /// Identifies the operation-specific result Type owned by this external test integration.
        using ResultType = TestSupport::ExternalTextConversionResult;

        /// Indicates that this adapter specialization is available.
        static constexpr bool IsAvailable = true;

        /// Indicates that this adapter performs no throwing operation.
        static constexpr bool IsNoexcept = true;

        /// Converts explicit external text bytes into a bounded String.
        static ResultType Convert(
            const TestSupport::ExternalText& source,
            String<TCapacity>& target
        ) noexcept {
            const auto result = target.Assign(
                source.Data,
                source.Size
            );

            if (result == StringAssignmentResult::Succeeded) { return ResultType::Succeeded; }

            if (result == StringAssignmentResult::CapacityExceeded) { return ResultType::CapacityExceeded; }

            return ResultType::InvalidSource;
        }

    };

    /// Converts a bounded String into the external test buffer without changing EDP-BoundedTypes.
    template<std::size_t TCapacity>
    struct TypeConversionAdapter<String<TCapacity>, TestSupport::ExternalBuffer> {

        /// Identifies the operation-specific result Type owned by this external test integration.
        using ResultType = TestSupport::ExternalTextConversionResult;

        /// Indicates that this adapter specialization is available.
        static constexpr bool IsAvailable = true;

        /// Indicates that this adapter performs no throwing operation.
        static constexpr bool IsNoexcept = true;

        /// Converts a bounded String into the external buffer when its fixed storage is sufficient.
        static ResultType Convert(
            const String<TCapacity>& source,
            TestSupport::ExternalBuffer& target
        ) noexcept {
            if (source.Size() >= target.Data.size()) { return ResultType::CapacityExceeded; }

            TestSupport::ExternalBuffer candidate{};

            for (std::size_t index = 0U; index < source.Size(); ++index)
                candidate.Data[index] = source[index];

            candidate.Data[source.Size()] = '\0';
            candidate.Size = source.Size();
            target = candidate;

            return ResultType::Succeeded;
        }

    };

} // ESPressio::Bounded

namespace {

    using namespace ESPressio::Bounded;

    /// Verifies compile-time boundedness and capacity qualification.
    void TestTraits() {
        static_assert(IsMemoryBoundedValue<int>);
        static_assert(IsMemoryBoundedValue<std::array<int, 4U>>);
        static_assert(IsMemoryBoundedValue<std::optional<String<8U>>>);
        static_assert(IsMemoryBoundedValue<std::tuple<int, String<8U>>>);
        static_assert(IsMemoryBoundedValue<std::variant<int, String<8U>>>);
        static_assert(!IsMemoryBoundedValue<std::string>);
        static_assert(!IsMemoryBoundedValue<std::vector<int>>);
        static_assert(!IsMemoryBoundedValue<int*>);
        static_assert(IsNonOwningView<std::string_view>);
        static_assert(IsMemoryBoundedValue<TestSupport::ExternallyDependentValue>);
        static_assert(MemoryBoundedTraits<TestSupport::ExternallyDependentValue>::HasExternalLifetimeDependencies);
        static_assert(CapacityTraits<String<32U>>::Capacity == 32U);
        static_assert(CapacityTraits<String<32U>>::Unit == CapacityUnit::Bytes);
        static_assert(CapacityTraits<Vector<int, 8U>>::Unit == CapacityUnit::Elements);
        static_assert(CapacityTraits<Map<int, int, 4U>>::Unit == CapacityUnit::Entries);
    }

    /// Verifies bounded String invariants, typed failures, overlap handling, and widening assignment.
    void TestString() {
        String<0U> empty;
        assert(empty.Capacity() == 0U);
        assert(empty.Size() == 0U);
        assert(empty.IsEmpty());
        assert(empty.IsFull());
        assert(empty.PushBack('x') == StringPushBackResult::CapacityExceeded);

        String<16U> value;
        assert(value.Assign("abcd") == StringAssignmentResult::Succeeded);
        assert(value.CStr()[value.Size()] == '\0');
        assert(value.Insert(
            2U,
            value.View()
        ) == StringInsertResult::Succeeded);
        assert(value.View() == "ababcdcd");
        assert(value.CStr()[value.Size()] == '\0');

        const char embeddedNull[] = {
            'a',
            '\0',
            'b',
            '\0'
        };
        const auto previous = std::string(value.View());
        assert(value.Assign(embeddedNull) == StringAssignmentResult::EmbeddedNullNotPermitted);
        assert(value.View() == previous);

        assert(value.SetAt(
            0U,
            '\0'
        ) == StringSetAtResult::NullCharacterNotPermitted);
        assert(value.View() == previous);

        assert(value.Erase(
            value.Size(),
            0U
        ) == StringEraseResult::Succeeded);
        assert(value.Erase(
            value.Size(),
            1U
        ) == StringEraseResult::RangeOutOfBounds);

        String<8U> small;
        assert(small.Assign("hello") == StringAssignmentResult::Succeeded);

        String<16U> wide;
        wide = small;
        assert(wide == small);

        String<8U> moved(
            std::move(
                small
            )
        );
        assert(small.IsEmpty());
        assert(moved.View() == "hello");
    }

    /// Verifies bounded byte behavior and explicit initialization of resized storage.
    void TestBytes() {
        Bytes<0U> zero;
        assert(zero.IsEmpty());
        assert(zero.IsFull());
        assert(zero.PushBack(1U) == BytesPushBackResult::CapacityExceeded);

        Bytes<8U> bytes;
        const std::array<std::uint8_t, 3U> source = {
            1U,
            0U,
            3U
        };
        assert(bytes.Assign(source) == BytesAssignmentResult::Succeeded);
        assert(bytes[1U] == 0U);
        assert(bytes.Insert(
            1U,
            bytes.Data(),
            bytes.Size()
        ) == BytesInsertResult::Succeeded);
        assert(bytes.Size() == 6U);
        assert(bytes.Resize(
            8U,
            0xAAU
        ) == BytesResizeResult::Succeeded);
        assert(bytes[6U] == 0xAAU);
        assert(bytes[7U] == 0xAAU);
    }

    /// Verifies raw-slot Vector lifetime, insertion, erase, resize, and deterministic moved-from state.
    void TestVector() {
        TestSupport::LifetimeValue::LiveCount = 0;
        TestSupport::LifetimeValue::ConstructionCount = 0;
        TestSupport::LifetimeValue::DestructionCount = 0;

        {
            Vector<TestSupport::LifetimeValue, 4U> values;
            assert(TestSupport::LifetimeValue::LiveCount == 0);
            assert(values.EmplaceBack(10) == VectorEmplaceBackResult::Succeeded);
            assert(values.EmplaceBack(20) == VectorEmplaceBackResult::Succeeded);
            assert(TestSupport::LifetimeValue::LiveCount == 2);
            assert(values.Insert(
                1U,
                values[0U]
            ) == VectorInsertResult::Succeeded);
            assert(values.Size() == 3U);
            assert(values[1U].Value() == 10);
            assert(values.Erase(
                1U,
                1U
            ) == VectorEraseResult::Succeeded);
            assert(values.Size() == 2U);

            Vector<TestSupport::LifetimeValue, 8U> moved(
                std::move(
                    values
                )
            );
            assert(values.IsEmpty());
            assert(moved.Size() == 2U);
        }

        assert(TestSupport::LifetimeValue::LiveCount == 0);
        assert(TestSupport::LifetimeValue::ConstructionCount == TestSupport::LifetimeValue::DestructionCount);
    }

    /// Verifies ring wrap, FIFO/LIFO adapters, and explicit CircularBuffer overwrite semantics.
    void TestRingContainers() {
        Deque<int, 3U> deque;
        assert(deque.PushBack(1) == DequePushBackResult::Succeeded);
        assert(deque.PushFront(0) == DequePushFrontResult::Succeeded);
        assert(deque.PushBack(2) == DequePushBackResult::Succeeded);
        assert(deque.PopFront() == DequePopFrontResult::Succeeded);
        assert(deque.PushBack(3) == DequePushBackResult::Succeeded);
        assert(deque[0U] == 1);
        assert(deque[1U] == 2);
        assert(deque[2U] == 3);

        Queue<int, 2U> queue;
        assert(queue.Enqueue(10) == QueueEnqueueResult::Succeeded);
        assert(queue.Enqueue(20) == QueueEnqueueResult::Succeeded);
        assert(queue.Enqueue(30) == QueueEnqueueResult::CapacityExceeded);
        int output = 0;
        assert(queue.Dequeue(output) == QueueDequeueResult::Succeeded);
        assert(output == 10);

        Stack<int, 2U> stack;
        assert(stack.Push(10) == StackPushResult::Succeeded);
        assert(stack.Push(20) == StackPushResult::Succeeded);
        assert(stack.Top() == 20);
        assert(stack.Pop(output) == StackPopResult::Succeeded);
        assert(output == 20);

        CircularBuffer<int, 2U> circular;
        assert(circular.Push(1) == CircularBufferPushResult::Succeeded);
        assert(circular.Push(2) == CircularBufferPushResult::Succeeded);
        assert(circular.Push(3) == CircularBufferPushResult::CapacityExceeded);
        assert(circular.PushOverwrite(3) == CircularBufferPushOverwriteResult::Succeeded);
        assert(circular[0U] == 2);
        assert(circular[1U] == 3);

        CircularBuffer<int, 0U> unavailable;
        assert(unavailable.PushOverwrite(1) == CircularBufferPushOverwriteResult::CapacityUnavailable);
    }

    /// Verifies Set uniqueness and semantic equality independent of insertion order.
    void TestSet() {
        Set<int, 4U> first;
        assert(first.Insert(2) == SetInsertResult::Succeeded);
        assert(first.Insert(1) == SetInsertResult::Succeeded);
        assert(first.Insert(2) == SetInsertResult::Duplicate);

        Set<int, 4U> second;
        assert(second.Insert(1) == SetInsertResult::Succeeded);
        assert(second.Insert(2) == SetInsertResult::Succeeded);
        assert(first == second);

        const int* found = nullptr;
        assert(first.Find(
            2,
            found
        ) == SetFindResult::Succeeded);
        assert(*found == 2);
    }

    /// Verifies unique Map keys, mutable mapped values, immutable key views, and semantic equality.
    void TestMap() {
        Map<int, int, 4U> first;
        assert(first.Insert(
            1,
            10
        ) == MapInsertResult::Succeeded);
        assert(first.Insert(
            2,
            20
        ) == MapInsertResult::Succeeded);
        assert(first.Insert(
            1,
            99
        ) == MapInsertResult::DuplicateKey);

        for (auto entry : first)
            if (entry.Key() == 2) {
                entry.Value() = 22;
            }

        int* value = nullptr;
        assert(first.Find(
            2,
            value
        ) == MapFindResult::Succeeded);
        assert(*value == 22);

        assert(first.InsertOrAssign(
            1,
            11
        ) == MapInsertOrAssignResult::Succeeded);

        Map<int, int, 8U> second;
        assert(second.Insert(
            2,
            22
        ) == MapInsertResult::Succeeded);
        assert(second.Insert(
            1,
            11
        ) == MapInsertResult::Succeeded);
        assert(first == second);
    }

    /// Verifies the generic CastTo/CastFrom seam without introducing any external dependency into the library.
    void TestConversionAdapters() {
        String<8U> bounded;
        TestSupport::ExternalText source{
            "hello",
            5U
        };

        assert(bounded.CastFrom(source) == TestSupport::ExternalTextConversionResult::Succeeded);
        assert(bounded.View() == "hello");

        TestSupport::ExternalBuffer target{};
        assert(bounded.CastTo(target) == TestSupport::ExternalTextConversionResult::Succeeded);
        assert(target.Size == 5U);
        assert(target.Data[5U] == '\0');
    }


    /// Verifies ByteOperations abstraction routing, provider substitution, cross-provider copying, and zero per-object provider state.
    void TestByteOperationsProviderSubstitution() {
        using Provider = TestSupport::CountingByteOperationsProvider;

        static_assert(sizeof(String<16U, Provider>) == sizeof(String<16U>));
        static_assert(sizeof(Bytes<16U, Provider>) == sizeof(Bytes<16U>));
        static_assert(IsMemoryBoundedValue<String<16U, Provider>>);
        static_assert(IsMemoryBoundedValue<Bytes<16U, Provider>>);
        static_assert(CapacityTraits<String<16U, Provider>>::Capacity == 16U);
        static_assert(CapacityTraits<Bytes<16U, Provider>>::Capacity == 16U);

        Provider::CopyCount = 0U;
        Provider::MoveCount = 0U;
        Provider::FillCount = 0U;
        Provider::CompareCount = 0U;

        String<16U> defaultText;
        assert(defaultText.Assign("hello") == StringAssignmentResult::Succeeded);

        String<16U, Provider> customText(
            defaultText
        );
        assert(customText == defaultText);
        assert(customText.Append(
            customText.View()
        ) == StringAppendResult::Succeeded);

        const std::array<std::uint8_t, 3U> source = {
            1U,
            2U,
            3U
        };

        Bytes<16U, Provider> customBytes;
        assert(customBytes.Assign(source) == BytesAssignmentResult::Succeeded);
        assert(customBytes.Resize(
            6U,
            0xA5U
        ) == BytesResizeResult::Succeeded);

        Bytes<16U> defaultBytes;
        assert(defaultBytes.Assign(
            customBytes.Data(),
            customBytes.Size()
        ) == BytesAssignmentResult::Succeeded);
        assert(customBytes == defaultBytes);

        assert(Provider::MoveCount > 0U);
        assert(Provider::FillCount > 0U);
        assert(Provider::CompareCount > 0U);
    }

    /// Verifies that representative bounded operations perform no host heap allocation.
    void TestNoHeapOperations() {
        const auto allocationsBefore = AllocationCount;

        String<32U> text;
        assert(text.Assign("bounded") == StringAssignmentResult::Succeeded);
        assert(text.Append("-value") == StringAppendResult::Succeeded);

        Bytes<16U> bytes;
        assert(bytes.Resize(
            16U,
            0x5AU
        ) == BytesResizeResult::Succeeded);

        Vector<int, 8U> vector;
        assert(vector.PushBack(1) == VectorPushBackResult::Succeeded);
        assert(vector.PushBack(2) == VectorPushBackResult::Succeeded);
        assert(vector.Insert(
            1U,
            3
        ) == VectorInsertResult::Succeeded);

        Deque<int, 8U> deque;
        assert(deque.PushFront(1) == DequePushFrontResult::Succeeded);
        assert(deque.PushBack(2) == DequePushBackResult::Succeeded);

        Set<int, 8U> set;
        assert(set.Insert(1) == SetInsertResult::Succeeded);

        Map<int, int, 8U> map;
        assert(map.Insert(
            1,
            2
        ) == MapInsertResult::Succeeded);

        assert(AllocationCount == allocationsBefore);
    }

} // anonymous

/// Runs the complete host validation suite for EDP-BoundedTypes.
int main() {
    TestTraits();
    TestString();
    TestBytes();
    TestVector();
    TestRingContainers();
    TestSet();
    TestMap();
    TestConversionAdapters();
    TestByteOperationsProviderSubstitution();
    TestNoHeapOperations();

    return 0;
}
