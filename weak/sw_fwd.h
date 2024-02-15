#pragma once

#include <exception>

class BadWeakPtr : public std::exception {};

template <typename T>
class ControlBlockObject;

template <typename T>
class ControlBlockPtr;

class ControlBlockBase;

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;
