#pragma once

#include "ArraySequence.hpp"
#include "LazySequence.hpp"

#include <fstream>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

template<class T>
class IReadOnlyStream {
public:
    virtual ~IReadOnlyStream() = default;
    virtual bool IsEndOfStream() const = 0;
    virtual T Read() = 0;
    virtual std::size_t GetPosition() const = 0;
    virtual bool IsCanSeek() const = 0;
    virtual bool IsCanGoBack() const = 0;
    virtual std::size_t Seek(std::size_t index) = 0;
    virtual void Open() = 0;
    virtual void Close() = 0;
};

template<class T>
class IWriteOnlyStream {
public:
    virtual ~IWriteOnlyStream() = default;
    virtual std::size_t GetPosition() const = 0;
    virtual std::size_t Write(const T& value) = 0;
    virtual void Open() = 0;
    virtual void Close() = 0;
};

template<class T>
class Stream {
private:
    std::shared_ptr<LazySequence<T>> sequence;

public:
    Stream() : sequence(std::make_shared<LazySequence<T>>()) {}
    explicit Stream(LazySequence<T>* seq) : sequence(seq) {
        if (seq == nullptr) throw std::invalid_argument("seq is null");
    }

    static Stream<T> FromArray(const T* items, int count) {
        return Stream<T>(new LazySequence<T>(items, count));
    }

    static Stream<T> FromSequence(const Sequence<T>& items) {
        return Stream<T>(new LazySequence<T>(items));
    }

    static Stream<T> Recurrence(const Sequence<T>& seed,
                                std::function<T(const Sequence<T>&)> recurrence,
                                Cardinal cardinal = Cardinal::Infinity()) {
        return Stream<T>(new LazySequence<T>(seed, recurrence, cardinal));
    }

    T Get(int index) const { return sequence->Get(index); }

    ArraySequence<T> Take(int count) const {
        if (count < 0) throw std::invalid_argument("count < 0");
        ArraySequence<T> result;
        for (int i = 0; i < count; ++i) result.Append(sequence->Get(i));
        return result;
    }

    template<class U>
    Stream<U> Map(U (*mapper)(T)) const {
        return Stream<U>(sequence->template Map<U>(mapper));
    }

    template<class U>
    U Reduce(U (*reducer)(U, T), U initial) const {
        return sequence->template Reduce<U>(reducer, initial);
    }

    Stream<T> Where(bool (*predicate)(T)) const {
        return Stream<T>(sequence->Where(predicate));
    }

    template<class U>
    Stream<std::pair<T, U>> Zip(const Stream<U>& other) const {
        return Stream<std::pair<T, U>>(sequence->template Zip<U>(other.sequence.get()));
    }

    LazySequence<T>& Source() const {
        return *sequence;
    }
};

template<class T>
class ReadOnlyStream : public IReadOnlyStream<T> {
private:
    ArraySequence<T> buffer;
    std::size_t position;
    bool opened;
    bool canSeek;
    std::string filePath;
    std::function<T(const std::string&)> deserializer;

    void LoadText(const std::string& text) {
        std::istringstream input(text);
        std::string token;
        while (input >> token) {
            buffer.Append(deserializer(token));
        }
    }

public:
    ReadOnlyStream()
        : buffer(), position(0), opened(true), canSeek(true), filePath(), deserializer(nullptr) {}

    explicit ReadOnlyStream(const Sequence<T>& source)
        : buffer(), position(0), opened(true), canSeek(true), filePath(), deserializer(nullptr) {
        for (int index = 0; index < source.GetSize(); ++index) {
            buffer.Append(source.Get(index));
        }
    }

    explicit ReadOnlyStream(LazySequence<T>* source, std::size_t count)
        : buffer(), position(0), opened(true), canSeek(true), filePath(), deserializer(nullptr) {
        if (source == nullptr) throw std::invalid_argument("source is null");
        for (std::size_t index = 0; index < count; ++index) {
            buffer.Append(source->Get(static_cast<int>(index)));
        }
    }

    explicit ReadOnlyStream(const Stream<T>& source, std::size_t count)
        : ReadOnlyStream(&source.Source(), count) {}

    ReadOnlyStream(const std::string& text, std::function<T(const std::string&)> parse)
        : buffer(), position(0), opened(true), canSeek(true), filePath(), deserializer(parse) {
        if (!deserializer) throw std::invalid_argument("deserializer is null");
        LoadText(text);
    }

    ReadOnlyStream(const char* path, std::function<T(const std::string&)> parse)
        : buffer(), position(0), opened(false), canSeek(true), filePath(path ? path : ""), deserializer(parse) {
        if (filePath.empty()) throw std::invalid_argument("path is empty");
        if (!deserializer) throw std::invalid_argument("deserializer is null");
    }

    void Open() override {
        if (opened) return;
        std::ifstream input(filePath);
        if (!input) throw std::runtime_error("Cannot open stream");

        buffer = ArraySequence<T>();
        std::string token;
        while (input >> token) {
            buffer.Append(deserializer(token));
        }
        position = 0;
        opened = true;
    }

    void Close() override {
        opened = false;
    }

    bool IsEndOfStream() const override {
        return position >= static_cast<std::size_t>(buffer.GetSize());
    }

    T Read() override {
        if (!opened) throw std::runtime_error("Stream is closed");
        if (IsEndOfStream()) throw std::out_of_range("EndOfStream");
        return buffer.Get(static_cast<int>(position++));
    }

    std::size_t GetPosition() const override {
        return position;
    }

    bool IsCanSeek() const override {
        return canSeek;
    }

    bool IsCanGoBack() const override {
        return canSeek;
    }

    std::size_t Seek(std::size_t index) override {
        if (!canSeek) throw std::runtime_error("Seek is not supported");
        if (index > static_cast<std::size_t>(buffer.GetSize())) throw std::out_of_range("IndexOutOfRange");
        position = index;
        return position;
    }
};

template<class T>
class WriteOnlyStream : public IWriteOnlyStream<T> {
private:
    Sequence<T>* targetSequence;
    std::size_t position;
    bool opened;
    std::string filePath;
    std::function<std::string(const T&)> serializer;
    std::ofstream output;

public:
    explicit WriteOnlyStream(Sequence<T>* target)
        : targetSequence(target), position(0), opened(true), filePath(), serializer(nullptr), output() {
        if (targetSequence == nullptr) throw std::invalid_argument("target is null");
    }

    WriteOnlyStream(const char* path, std::function<std::string(const T&)> serialize)
        : targetSequence(nullptr), position(0), opened(false), filePath(path ? path : ""), serializer(serialize), output() {
        if (filePath.empty()) throw std::invalid_argument("path is empty");
        if (!serializer) throw std::invalid_argument("serializer is null");
    }

    void Open() override {
        if (opened) return;
        output.open(filePath);
        if (!output) throw std::runtime_error("Cannot open stream");
        opened = true;
    }

    void Close() override {
        if (output.is_open()) output.close();
        opened = false;
    }

    std::size_t GetPosition() const override {
        return position;
    }

    std::size_t Write(const T& value) override {
        if (!opened) throw std::runtime_error("Stream is closed");
        if (targetSequence) {
            targetSequence->Append(value);
        } else {
            output << serializer(value) << '\n';
            if (!output) throw std::runtime_error("Cannot write stream");
        }
        ++position;
        return position;
    }
};

template<class T>
class ReadWriteStream : public IReadOnlyStream<T>, public IWriteOnlyStream<T> {
private:
    ArraySequence<T> storage;
    std::size_t readPosition;
    bool opened;

public:
    ReadWriteStream() : storage(), readPosition(0), opened(true) {}

    void Open() override { opened = true; }
    void Close() override { opened = false; }

    bool IsEndOfStream() const override {
        return readPosition >= static_cast<std::size_t>(storage.GetSize());
    }

    T Read() override {
        if (!opened) throw std::runtime_error("Stream is closed");
        if (IsEndOfStream()) throw std::out_of_range("EndOfStream");
        return storage.Get(static_cast<int>(readPosition++));
    }

    std::size_t Write(const T& value) override {
        if (!opened) throw std::runtime_error("Stream is closed");
        storage.Append(value);
        return static_cast<std::size_t>(storage.GetSize());
    }

    std::size_t GetPosition() const override {
        return readPosition;
    }

    bool IsCanSeek() const override { return true; }
    bool IsCanGoBack() const override { return true; }

    std::size_t Seek(std::size_t index) override {
        if (index > static_cast<std::size_t>(storage.GetSize())) throw std::out_of_range("IndexOutOfRange");
        readPosition = index;
        return readPosition;
    }

    int GetSize() const {
        return storage.GetSize();
    }
};
