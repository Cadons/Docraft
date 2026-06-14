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

#include "docraft/backend/pdf/docraft_haru_backend_providers_factory.h"

#include "docraft/backend/pdf/docraft_haru_backend.h"

namespace docraft::backend::pdf {
    backend::DocraftCapabilityProviders DocraftHaruCapabilityProvidersFactory::create_capability_providers() const {
        auto backend = std::make_shared<DocraftHaruBackend>();
        return {
            .rendering_provider = backend,
            .resource_provider = backend,
            .lifecycle_provider = backend
        };
    }
} // namespace docraft::backend::pdf

