//
// Created by Matteo on 29/06/2026.
//

#pragma once
#include <filesystem>
#include <memory>

#include "docraft/backend/pdf/docraft_haru_backend.h"
#include "docraft/backend/pdf/docraft_haru_shared_state.h"
#include "interfaces/docraft_loom_visitor.h"

namespace docraft::loom {
    class DocraftLoomPdfCreator
    {
    public:
        DocraftLoomPdfCreator(std::shared_ptr<interfaces::DocraftLoomIVisitorNode> root_node);
        ~DocraftLoomPdfCreator() = default;
        void create();
        void render(const std::filesystem::path& output_path);

    private:
        std::shared_ptr<interfaces::DocraftLoomIVisitorNode> root_node_; //taken as input
        std::shared_ptr<docraft::backend::pdf::DocraftHaruSharedState> state_;
        std::shared_ptr<docraft::backend::pdf::DocraftHaruBackend> backend_; //used for text measurement and rendering
    };
} // docraft
