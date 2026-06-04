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

#include "docraft/backend/docraft_rendering_backend.h"
#include "docraft/docraft_lib.h"

namespace docraft::backend {
    struct DOCRAFT_LIB DocraftBackendProviders {
        std::shared_ptr<IDocraftRenderingCapabilityProvider> rendering_provider;
        std::shared_ptr<IDocraftResourceCapabilityProvider> resource_provider;
        std::shared_ptr<IDocraftLifecycleCapabilityProvider> lifecycle_provider;
    };

    class DOCRAFT_LIB IDocraftBackendProvidersFactory {
    public:
        virtual ~IDocraftBackendProvidersFactory() = default;

        [[nodiscard]] virtual DocraftBackendProviders create_backend_providers() const = 0;
    };
} // namespace docraft::backend

