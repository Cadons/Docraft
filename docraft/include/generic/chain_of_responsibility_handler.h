#pragma once
#include <memory>

namespace docraft::generic{
    template<typename T>
    class DocraftChainOfResponsibilityHandler {
    public:
        virtual ~DocraftChainOfResponsibilityHandler() = default;
        virtual bool handle(std::shared_ptr<T> request) =0;
    };
}
