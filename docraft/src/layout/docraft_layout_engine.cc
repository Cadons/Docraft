#include "layout/docraft_layout_engine.h"

#include <iostream>
#include <ostream>

#include "layout/handler/docraft_basic_layout_handler.h"
#include "layout/handler/docraft_layout_blank_line.h"
#include "layout/handler/docraft_layout_handler.h"
#include "layout/handler/docraft_layout_section_handler.h"
#include "layout/handler/docraft_layout_table_handler.h"
#include "layout/handler/docraft_layout_text_handler.h"
namespace docraft::layout {

    Handlers DocraftLayoutEngine::handlers_;

    void
    DocraftLayoutEngine::
    configure_handlers(const std::shared_ptr<DocraftPDFContext> &context) {
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutSectionHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTextHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutTableHandler>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftLayoutBlankLine>(context));
        handlers_.emplace_back(std::make_unique<handler::DocraftBasicLayoutHandler>(context));
    }

    void DocraftLayoutEngine::layout(std::shared_ptr<model::DocraftNode> node,
                                     const std::shared_ptr<DocraftPDFContext> &context) {
        std::cout << "computing layout...." << std::endl;
        configure_handlers(context);
        for (const auto &handler: handlers_) {
            if (handler->handle(node)) {

                break;
            }
        }
    }
} // docraft
