#include "tokenizer.h"
#include "error.h"

bool SymbolToken::operator==(const SymbolToken& other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken&) const {
    return true;
}

bool DotToken::operator==(const DotToken&) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken& other) const {
    return value == other.value;
}

void Tokenizer::Read() {
    char_ = in_->get();
}
Tokenizer::Tokenizer(std::istream* in) : in_(in), char_(-1) {
    Next();
}

bool Tokenizer::IsEnd() {
    return !token_.has_value();
}

void Tokenizer::Next() {
    token_.reset();
    while (true) {
        Read();
        if (std::isspace(char_)) {
            continue;
        }
        if (char_ == EOF || !in_->good()) {
            return;
        }
        if (char_ == '(') {
            token_ = BracketToken::OPEN;
            return;
        } else if (char_ == ')') {
            token_ = BracketToken::CLOSE;
            return;
        } else if (char_ == '.') {
            token_ = DotToken{};
            return;
        } else if (char_ == '\'') {
            token_ = QuoteToken{};
            return;
        } else if (char_ == '@') {
            throw SyntaxError("invalid character");
        }
        if (std::isdigit(char_) || char_ == '-' || char_ == '+') {
            std::string number_str;
            number_str += static_cast<char>(char_);
            while (in_->peek() != EOF && std::isdigit(in_->peek())) {
                Read();
                number_str += static_cast<char>(char_);
            }
            try {
                int value = std::stoi(number_str);
                token_ = ConstantToken{value};
                return;
            } catch (std::invalid_argument&) {
                token_ = SymbolToken{number_str};
                return;
            }
        }
        if (std::isalpha(char_) || char_ == '<' || char_ == '>' || char_ == '=' || char_ == '*' ||
            char_ == '/' || char_ == '#' || char_ == '!' || char_ == '?') {
            std::string symbol_str;
            symbol_str += static_cast<char>(char_);
            while (in_->peek() != EOF &&
                   (std::isalnum(in_->peek()) || in_->peek() == '<' || in_->peek() == '>' ||
                    in_->peek() == '=' || in_->peek() == '*' || in_->peek() == '/' ||
                    in_->peek() == '#' || in_->peek() == '!' || in_->peek() == '-' ||
                    in_->peek() == '?')) {
                Read();
                symbol_str += static_cast<char>(char_);
            }
            token_ = SymbolToken{symbol_str};
            return;
        }
        token_ = SymbolToken{std::string(1, static_cast<char>(char_))};
        return;
    }
}

Token Tokenizer::GetToken() {
    if (token_.has_value()) {
        return token_.value();
    }
    return SymbolToken{""};
}