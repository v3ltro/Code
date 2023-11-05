add_executable(
    image_processor
    image_processor.cpp
        "Source/Image.cpp"
        "Headers/Console.h"
        "Source/Console.cpp"
        "Source/Filters.cpp"
        "Headers/Filters.h"
        "Headers/MatrixFilter.h"
        "Source/MatrixFilter.cpp" Headers/Exceptions.h Source/Exceptions.cpp)
