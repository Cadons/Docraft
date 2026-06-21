//
// Created by Matteo on 21/06/2026.
//
#pragma once

namespace docraft::loom::nodes {
    class DocraftLoomText;
    class DocraftLoomRectangle;
}

namespace docraft::loom::interfaces {
    /**
     * @brief Visitor interface for DocraftLoom nodes.
     * @note This class will be implemented by
     */
    class DocraftLoomIVisitor
    {
    public:
        virtual ~DocraftLoomIVisitor() = default;
        virtual void visit(docraft::loom::nodes::DocraftLoomText*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomRectangle*) = 0;
    };

    /**
     * @brief Accepter interface for DocraftLoom nodes that can accept a DocraftLoomIVisitor.
     * @note This class will be implemented by all DocraftLoom nodes that can be visited by a DocraftLoomIVisitor.
     */
    class DocraftLoomIVisitorNode
    {
    public:
        virtual ~DocraftLoomIVisitorNode() = default;
        virtual void accept(DocraftLoomIVisitor&) = 0;
    };
}
