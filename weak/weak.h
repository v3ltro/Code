#pragma once

#include "sw_fwd.h"  // Forward declaration
#include "shared.h"

// https://en.cppreference.com/w/cpp/memory/weak_ptr
template <typename T>
class WeakPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    WeakPtr() : block_(nullptr), ptr_(nullptr){};

    WeakPtr(const WeakPtr& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncrementWeakRefCount();
        }
    };
    WeakPtr(WeakPtr&& other) : block_(other.block_), ptr_(other.ptr_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    };
    template <typename Y>
    WeakPtr(const SharedPtr<Y>& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncrementWeakRefCount();
        }
    }
    // Demote `SharedPtr`
    // #2 from https://en.cppreference.com/w/cpp/memory/weak_ptr/weak_ptr
    WeakPtr(const SharedPtr<T>& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncrementWeakRefCount();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    WeakPtr& operator=(const WeakPtr& other) {
        if (this != &other) {
            Reset();
            block_ = other.block_;
            ptr_ = other.ptr_;
            if (block_) {
                block_->IncrementWeakRefCount();
            }
        }
        return *this;
    };
    WeakPtr& operator=(WeakPtr&& other) {
        if (this != &other) {
            Reset();
            block_ = other.block_;
            ptr_ = other.ptr_;
            other.block_ = nullptr;
            other.ptr_ = nullptr;
        }
        return *this;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~WeakPtr() {
        Reset();
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->DecrementWeakRefCount();
            block_ = nullptr;
        }
        ptr_ = nullptr;
    };
    void Swap(WeakPtr& other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    size_t UseCount() const {
        if (block_) {
            return block_->GetRefCount();
        }
        return 0;
    };
    bool Expired() const {
        return (!block_ || !block_->Exists());
    };
    SharedPtr<T> Lock() const {
        if (Expired()) {
            return SharedPtr<T>();
        }
        return SharedPtr<T>(*this);
    }

    friend class SharedPtr<T>;
    ControlBlockBase* block_;
    T* ptr_;
};
