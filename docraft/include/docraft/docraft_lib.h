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
