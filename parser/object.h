#pragma once

#include <memory>
#include <string>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
};

class Number : public Object {
private:
    int val_;

public:
    explicit Number(int val) : val_(val){};

    int GetValue() const {
        return val_;
    };
};

class Symbol : public Object {
private:
    std::string name_;

public:
    explicit Symbol(const std::string name) : name_(name){};
    const std::string& GetName() const {
        return name_;
    };
};

class Cell : public Object {
private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;

public:
    Cell(const std::shared_ptr<Object>& first, const std::shared_ptr<Object>& second)
        : first_(first), second_(second){};

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    };
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    };
};

///////////////////////////////////////////////////////////////////////////////

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
};

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return As<T>(obj) != nullptr;
};
