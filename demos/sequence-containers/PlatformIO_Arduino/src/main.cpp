#include <Arduino.h>
#include <ESPressio_BoundedTypes.hpp>

using ESPressio::Bounded::CircularBuffer;
using ESPressio::Bounded::CircularBufferPushOverwriteResult;
using ESPressio::Bounded::Deque;
using ESPressio::Bounded::DequePushBackResult;
using ESPressio::Bounded::Queue;
using ESPressio::Bounded::QueueEnqueueResult;
using ESPressio::Bounded::Stack;
using ESPressio::Bounded::StackPushResult;
using ESPressio::Bounded::Vector;
using ESPressio::Bounded::VectorPushBackResult;

/// Builds each bounded sequence abstraction and reports its logical size.
void setup() {
    Serial.begin(115200);

    Vector<int, 4U> vector;
    vector.PushBack(1);
    vector.PushBack(2);

    Deque<int, 4U> deque;
    deque.PushBack(10);
    deque.PushBack(20);

    Queue<int, 4U> queue;
    queue.Enqueue(100);

    Stack<int, 4U> stack;
    stack.Push(200);

    CircularBuffer<int, 2U> circular;
    circular.PushOverwrite(1);
    circular.PushOverwrite(2);
    circular.PushOverwrite(3);

    Serial.printf(
        "Vector=%u Deque=%u Queue=%u Stack=%u Circular=%u\n",
        static_cast<unsigned>(vector.Size()),
        static_cast<unsigned>(deque.Size()),
        static_cast<unsigned>(queue.Size()),
        static_cast<unsigned>(stack.Size()),
        static_cast<unsigned>(circular.Size())
    );
}

/// Performs no repeated work; this demo is intentionally initialization-only.
void loop() {
}
