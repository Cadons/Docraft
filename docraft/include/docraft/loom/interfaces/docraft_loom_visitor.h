//
// Created by Matteo on 21/06/2026.
//
#pragma once

namespace docraft::loom::nodes {
    class DocraftLoomText;
    class DocraftLoomRectangle;
    class DocraftLoomParagraph;
    class DocraftLoomVStack;
    class DocraftLoomHStack;
    class DocraftLoomBlankLine;
    class DocraftLoomImage;
    class DocraftLoomLine;
    class DocraftLoomCircle;
    class DocraftLoomTriangle;
    class DocraftLoomPolygon;
    class DocraftLoomList;
    class DocraftLoomTableCell;
    class DocraftLoomTable;
    class DocraftLoomPageNumber;
    class DocraftLoomNewPage;
}

namespace docraft::loom::interfaces {
    /**
     * @brief Visitor interface for DocraftLoom nodes.
     */
    class DocraftLoomIVisitor
    {
    public:
        virtual ~DocraftLoomIVisitor() = default;
        virtual void visit(docraft::loom::nodes::DocraftLoomText*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomRectangle*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomParagraph*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomVStack*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomHStack*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomBlankLine*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomImage*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomLine*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomCircle*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomTriangle*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomPolygon*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomList*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomTableCell*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomTable*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomPageNumber*) = 0;
        virtual void visit(docraft::loom::nodes::DocraftLoomNewPage*) = 0;
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
