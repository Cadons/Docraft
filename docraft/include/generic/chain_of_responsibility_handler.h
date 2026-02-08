#pragma once
#include <memory>

namespace docraft {
    class DocraftCursor;
}

namespace docraft::generic{
    template<class T,class K>
    class DocraftChainOfResponsibilityHandler {
    public:
        virtual ~DocraftChainOfResponsibilityHandler() = default;
        virtual bool handle(const std::shared_ptr<T>& request, K* result, docraft::DocraftCursor& cursor) =0;
    };
}
