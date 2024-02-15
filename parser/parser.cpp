#include "parser.h"
#include "error.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("unexpected EOI");
    }
    Token token = tokenizer->GetToken();
    if (std::holds_alternative<ConstantToken>(token)) {
        tokenizer->Next();
        return std::make_shared<Number>(std::get<ConstantToken>(token).value);
    } else if (std::holds_alternative<SymbolToken>(token)) {
        tokenizer->Next();
        return std::make_shared<Symbol>(std::get<SymbolToken>(token).name);
    } else if (std::holds_alternative<BracketToken>(token)) {
        tokenizer->Next();
        return ReadList(tokenizer);
    } else {
        throw SyntaxError("bad token");
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    Token token = tokenizer->GetToken();

    if (std::holds_alternative<BracketToken>(token) &&
        std::get<BracketToken>(token) == BracketToken::CLOSE) {
        tokenizer->Next();
        return nullptr;
    } else {
        std::shared_ptr<Object> first = Read(tokenizer);
        token = tokenizer->GetToken();

        if (std::holds_alternative<DotToken>(token)) {
            tokenizer->Next();
            std::shared_ptr<Object> second = Read(tokenizer);
            token = tokenizer->GetToken();

            if (std::holds_alternative<BracketToken>(token) &&
                std::get<BracketToken>(token) == BracketToken::CLOSE) {
                tokenizer->Next();
                return std::make_shared<Cell>(first, second);
            } else {
                throw SyntaxError("expected closing bracket after dotted pair");
            }
        } else {
            std::shared_ptr<Object> other = ReadList(tokenizer);
            return std::make_shared<Cell>(first, other);
        }
    }
}
