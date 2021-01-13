/*
    _____ _____ _____ __
   |   __|     |  |  |  |      The SOUL language
   |__   |  |  |  |  |  |__    Copyright (c) 2019 - ROLI Ltd.
   |_____|_____|_____|_____|

   The code in this file is provided under the terms of the ISC license:

   Permission to use, copy, modify, and/or distribute this software for any purpose
   with or without fee is hereby granted, provided that the above copyright notice and
   this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

namespace soul
{

/** Utilities for parsing and modifying source code textually. */
struct SourceCodeOperations
{
    SourceCodeOperations();
    ~SourceCodeOperations();

    struct TextModificationOp
    {
        size_t startOffset = 0, length = 0;
        std::string insertedText;
    };

    using ApplyModificationFn = std::function<void(const TextModificationOp&)>;

    struct Comment
    {
        bool valid = false, isStarSlash = false, isDoxygenStyle = false, isReferringBackwards = false;
        std::vector<std::string> lines;
        CodeLocation start, end;

        std::string getText() const;
    };

    struct ModuleDeclaration
    {
        AST::ModuleBase& module;

        CodeLocation startIncludingPreamble,
                     moduleKeyword,
                     openBrace,
                     endOfClosingBrace;

        std::string getType() const;
        std::string getName() const;
        std::string getFullyQualifiedName() const;
        Comment getComment() const;
        std::vector<std::string> getSpecialisationParameters() const;
    };

    void clear();
    bool reload (CompileMessageList&, CodeLocation code, ApplyModificationFn applyModificationFn);

    SourceCodeOperations::Comment getFileSummaryComment() const;
    std::string getFileSummaryTitle() const;
    std::string getFileSummaryBody() const;

    ArrayView<ModuleDeclaration> getAllModules() const      { return allModules; }
    ArrayView<ModuleDeclaration> getProcessors() const      { return processors; }
    ArrayView<ModuleDeclaration> getGraphs() const          { return graphs; }
    ArrayView<ModuleDeclaration> getNamespaces() const      { return namespaces; }

    static Comment parseComment (CodeLocation startOfComment);
    static CodeLocation findStartOfPrecedingComment (CodeLocation location);

    static CodeLocation findEndOfMatchingBrace (CodeLocation openBrace);
    static CodeLocation findEndOfMatchingParen (CodeLocation openParen);

    static std::vector<std::string> parseParenthesisedParameterList (CodeLocation openParen);

    static std::string getStringForType (AST::Expression&);

    void removeProcessor (AST::ProcessorBase&);
    void addProcessor (AST::ProcessorBase&);

    CodeLocation source;

private:
    AST::Allocator allocator;
    pool_ptr<AST::Namespace> topLevelNamespace;
    std::vector<ModuleDeclaration> allModules, processors, graphs, namespaces;
    ApplyModificationFn applyModification;

    bool reparse (CompileMessageList&, CodeLocation);
    void reparse();

    ModuleDeclaration createDecl (AST::ModuleBase&);
    ModuleDeclaration* findDeclaration (AST::ModuleBase&);
    size_t getFileOffset (const CodeLocation&) const;

    void insertText (CodeLocation, std::string newText);
    void replaceText (CodeLocation start, CodeLocation end, std::string newText);
    void deleteText (CodeLocation start, CodeLocation end);
    void recurseFindingModules (soul::AST::ModuleBase&);
};


} // namespace soul
