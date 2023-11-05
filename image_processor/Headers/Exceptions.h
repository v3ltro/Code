#pragma once

#include <exception>
#include <iostream>

class Exception : public std::exception {
public:
    explicit Exception(const std::string& msg);
    const char* what() const noexcept override;

private:
    std::string msg_;
};

class InputArgumentException : public Exception {
public:
    explicit InputArgumentException(const std::string& msg);
};

class FilterNameException : public Exception {
public:
    explicit FilterNameException(const std::string& msg);
};

class FilterArgumentException : public Exception {
public:
    explicit FilterArgumentException(const std::string& msg);
};

class ImageHeaderError : public Exception {
public:
    explicit ImageHeaderError(const std::string& msg);
};