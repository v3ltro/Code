#pragma once

#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t

class ControlBlockBase {
private:
    int ref_count_ = 1;
    int weak_ref_count_ = 0;

public:
    ControlBlockBase() : ref_count_(1), weak_ref_count_(0) {
    }

    void IncrementRefCount() {
        ++ref_count_;
    }

    void DecrementRefCount() {
        if (--ref_count_ == 0) {
            Destroy();
            if (weak_ref_count_ == 0) {
                delete this;
            }
        }
    }

    void IncrementWeakRefCount() {
        ++weak_ref_count_;
    }

    void DecrementWeakRefCount() {
        if (--weak_ref_count_ == 0 && ref_count_ == 0) {
            delete this;
        }
    }

    int GetRefCount() const {
        return ref_count_;
    }

    virtual bool Exists() = 0;

    int GetWeakRefCount() const {
        return weak_ref_count_;
    }
    virtual ~ControlBlockBase() = default;
    virtual void Destroy() = 0;
};

template <typename T>
class ControlBlockPtr : public ControlBlockBase {
private:
    T* ptr_;

public:
    ControlBlockPtr(T* p) : ptr_(p) {
    }

    void Destroy() override {
        delete ptr_;
        ptr_ = nullptr;
    }

    bool Exists() override {
        return ptr_;
    }
};

template <typename T>
class ControlBlockObject : public ControlBlockBase {
protected:
    bool exists_ = true;
    std::aligned_storage_t<sizeof(T), alignof(T)> object_;

public:
    template <typename... Args>
    ControlBlockObject(Args&&... args) {
        new (static_cast<void*>(&object_)) T(std::forward<Args>(args)...);
    }
    T* GetObject() {
        if (exists_) {
            return static_cast<T*>(static_cast<void*>(&object_));
        }
        return nullptr;
    }
    bool Exists() override {
        return exists_;
    }
    void Destroy() override {
        if (exists_) {
            static_cast<T*>(static_cast<void*>(&object_))->~T();
        }
        exists_ = false;
    }
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr

template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    SharedPtr() : block_(nullptr), ptr_(nullptr){};
    SharedPtr(std::nullptr_t) : block_(nullptr), ptr_(nullptr) {
    }
    template <typename Y>
    explicit SharedPtr(Y* ptr) : block_(new ControlBlockPtr<Y>(ptr)), ptr_(ptr){};
    explicit SharedPtr(T* ptr) : block_(new ControlBlockPtr<T>(ptr)), ptr_(ptr){};
    template <typename Y>
    SharedPtr(ControlBlockObject<Y>* block) : block_(block), ptr_(block->GetObject()){};
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncrementRefCount();
        }
    };
    SharedPtr(const SharedPtr& other) : block_(other.block_), ptr_(other.ptr_) {
        if (block_) {
            block_->IncrementRefCount();
        }
    };
    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) : block_(other.block_), ptr_(other.ptr_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    };
    SharedPtr(SharedPtr&& other) : block_(other.block_), ptr_(other.ptr_) {
        other.block_ = nullptr;
        other.ptr_ = nullptr;
    };
    template <typename Y>
    friend class SharedPtr;
    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : block_(other.block_), ptr_(ptr) {
        if (block_) {
            block_->IncrementRefCount();
        }
    };

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) : block_(other.block_), ptr_(other.ptr_) {
        if (other.block_->GetRefCount() == 0) {
            throw BadWeakPtr();
        }
        if (block_) {
            block_->IncrementRefCount();
        }
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    SharedPtr& operator=(const SharedPtr<T>& other) {
        if (this != &other) {
            Reset();
            block_ = other.block_;
            ptr_ = other.ptr_;
            if (block_) {
                block_->IncrementRefCount();
            }
        }
        return *this;
    };
    SharedPtr& operator=(SharedPtr&& other) {
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

    ~SharedPtr() {
        Reset();
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->DecrementRefCount();
            block_ = nullptr;
            ptr_ = nullptr;
        }
    };
    void Reset(T* ptr) {
        if (ptr == ptr_) {
            return;
        }
        Reset();
        block_ = ptr ? new ControlBlockPtr<T>(ptr) : nullptr;
        ptr_ = ptr;
    };
    template <typename Y>
    void Reset(Y* ptr) {
        if (ptr == ptr_) {
            return;
        }
        Reset();
        block_ = ptr ? new ControlBlockPtr<Y>(ptr) : nullptr;
        ptr_ = ptr;
    };
    void Swap(SharedPtr& other) {
        std::swap(block_, other.block_);
        std::swap(ptr_, other.ptr_);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    };
    T& operator*() const {
        if (ptr_) {
            return *ptr_;
        }
    };
    T* operator->() const {
        return ptr_;
    };
    size_t UseCount() const {
        if (block_) {
            return block_->GetRefCount();
        }
        return 0;
    };
    explicit operator bool() const {
        return ptr_ != nullptr;
    };
    ControlBlockBase* block_;
    T* ptr_;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return left.Get() == right.Get();
};

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    ControlBlockObject<T>* control_block = new ControlBlockObject<T>(std::forward<Args>(args)...);
    return SharedPtr<T>(control_block);
}

// Look for usage examples in tests
template <typename T>
class EnableSharedFromThis {
public:
    SharedPtr<T> SharedFromThis() {
        return SharedPtr<T>(this);
    };
    [[maybe_unused]] SharedPtr<const T> SharedFromThis() const {
        return SharedPtr<const T>(this);
    };

    WeakPtr<T> WeakFromThis() noexcept {
        return WeakPtr<T>(this);
    };
    WeakPtr<const T> WeakFromThis() const noexcept {
        return WeakPtr<const T>(this);
    };
};