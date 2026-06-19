#pragma once

#include "Stream.hpp"

#include <cstddef>
#include <stdexcept>

template<class T>
class OnlineStatistics {
private:
    std::size_t count;
    T sum;
    T minimum;
    T maximum;
    ArraySequence<T> values;

public:
    OnlineStatistics() : count(0), sum(), minimum(), maximum(), values() {}

    void Add(const T& value) {
        if (count == 0) {
            minimum = value;
            maximum = value;
            sum = value;
        } else {
            if (value < minimum) minimum = value;
            if (maximum < value) maximum = value;
            sum = sum + value;
        }
        values.Append(value);
        ++count;
    }

    void Consume(ReadOnlyStream<T>& stream) {
        while (!stream.IsEndOfStream()) {
            Add(stream.Read());
        }
    }

    std::size_t GetCount() const {
        return count;
    }

    T GetSum() const {
        return sum;
    }

    T GetMin() const {
        if (count == 0) throw std::runtime_error("Statistics is empty");
        return minimum;
    }

    T GetMax() const {
        if (count == 0) throw std::runtime_error("Statistics is empty");
        return maximum;
    }

    double GetMean() const {
        if (count == 0) throw std::runtime_error("Statistics is empty");
        return static_cast<double>(sum) / static_cast<double>(count);
    }

    double GetMedian() const {
        if (count == 0) throw std::runtime_error("Statistics is empty");

        ArraySequence<T> sorted(values);
        for (int i = 1; i < sorted.GetSize(); ++i) {
            T key = sorted.Get(i);
            int j = i - 1;
            while (j >= 0 && key < sorted.Get(j)) {
                sorted.Set(j + 1, sorted.Get(j));
                --j;
            }
            sorted.Set(j + 1, key);
        }

        int middle = sorted.GetSize() / 2;
        if (sorted.GetSize() % 2 == 1) {
            return static_cast<double>(sorted.Get(middle));
        }
        return (static_cast<double>(sorted.Get(middle - 1)) + static_cast<double>(sorted.Get(middle))) / 2.0;
    }

    T GetWindowMin(std::size_t windowSize) const {
        if (windowSize == 0 || count == 0) throw std::runtime_error("Window is empty");
        std::size_t start = count > windowSize ? count - windowSize : 0;
        T result = values.Get(static_cast<int>(start));
        for (std::size_t index = start + 1; index < count; ++index) {
            T value = values.Get(static_cast<int>(index));
            if (value < result) result = value;
        }
        return result;
    }

    T GetWindowMax(std::size_t windowSize) const {
        if (windowSize == 0 || count == 0) throw std::runtime_error("Window is empty");
        std::size_t start = count > windowSize ? count - windowSize : 0;
        T result = values.Get(static_cast<int>(start));
        for (std::size_t index = start + 1; index < count; ++index) {
            T value = values.Get(static_cast<int>(index));
            if (result < value) result = value;
        }
        return result;
    }

    double GetWindowMean(std::size_t windowSize) const {
        if (windowSize == 0 || count == 0) throw std::runtime_error("Window is empty");
        std::size_t start = count > windowSize ? count - windowSize : 0;
        T windowSum = T();
        for (std::size_t index = start; index < count; ++index) {
            windowSum = windowSum + values.Get(static_cast<int>(index));
        }
        return static_cast<double>(windowSum) / static_cast<double>(count - start);
    }
};
