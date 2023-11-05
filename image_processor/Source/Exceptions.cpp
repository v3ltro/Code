#include "../Headers/Exceptions.h"

Exception::Exception(const std::string &msg) : msg_(msg){};
const char *Exception::what() const noexcept {
    return msg_.c_str();
}

InputArgumentException::InputArgumentException(const std::string &msg) : Exception(msg){};

FilterNameException::FilterNameException(const std::string &msg) : Exception(msg){};

FilterArgumentException::FilterArgumentException(const std::string &msg) : Exception(msg){};

ImageHeaderError::ImageHeaderError(const std::string &msg) : Exception(msg){};