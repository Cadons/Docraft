/*
 * Copyright 2026 Matteo Cadoni (https://github.com/cadons)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <memory>
#include <vector>

#if defined(_WIN32) || defined(__CYGWIN__)
#if defined(DOCRAFT_BUILD_SHARED_LIBS)
#define DOCRAFT_LIB __declspec(dllexport)
#elif defined(DOCRAFT_USE_SHARED_LIB)
#define DOCRAFT_LIB __declspec(dllimport)
#else
#define DOCRAFT_LIB
#endif

#elif defined(__GNUC__) && __GNUC__ >= 4
#if defined(DOCRAFT_BUILD_SHARED_LIBS)
#define DOCRAFT_LIB __attribute__((visibility("default")))
#else
#define DOCRAFT_LIB
#endif

#else
#define DOCRAFT_LIB
#endif

#define DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD(TYPE, NAME, MEMBER) \
    [[nodiscard]] const TYPE &NAME() const { return MEMBER; } \
    [[nodiscard]] TYPE &edit_##NAME() { return MEMBER; }

#define DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER(TYPE, NAME, MEMBER) \
    [[nodiscard]] std::shared_ptr<const TYPE> NAME() const { return MEMBER; } \
    [[nodiscard]] std::shared_ptr<TYPE> edit_##NAME() { return MEMBER; }

#define DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER_CUSTOM(TYPE, GETTER_NAME, EDIT_NAME, MEMBER) \
    [[nodiscard]] std::shared_ptr<const TYPE> GETTER_NAME() const { return MEMBER; } \
    [[nodiscard]] std::shared_ptr<TYPE> EDIT_NAME() { return MEMBER; }

#define DOCRAFT_CREATE_GETTER_AND_EDIT_METHOD_SHARED_SMART_POINTER_LAZY_INIT(TYPE, NAME, MEMBER, INIT_EXPR) \
    [[nodiscard]] std::shared_ptr<const TYPE> NAME() const { \
        if (!(MEMBER)) { \
            INIT_EXPR; \
        } \
        return MEMBER; \
    } \
    [[nodiscard]] std::shared_ptr<TYPE> edit_##NAME() { \
        if (!(MEMBER)) { \
            INIT_EXPR; \
        } \
        return MEMBER; \
    }

#define DOCRAFT_CREATE_GETTER_CONST_VECTOR_AND_EDIT_METHOD_SHARED_SMART_POINTER(TYPE, NAME, MEMBER) \
    [[nodiscard]] std::vector<std::shared_ptr<const TYPE>> NAME() const { return {MEMBER.begin(), MEMBER.end()}; } \
    [[nodiscard]] std::vector<std::shared_ptr<TYPE>> &edit_##NAME() { return MEMBER; }
