#include <iostream>
#include <string>
#include <vector>
#include <cctype>

enum class TokenType
{
    T_FUNCTION,
    T_INT,
    T_FLOAT,
    T_BOOL,
    T_STRING,
    T_IDENTIFIER,
    T_ASSIGNOP,
    T_EQUALSOP,
    T_PARENL,
    T_PARENR,
    T_COMMA,
    T_BRACEL,
    T_BRACER,
    T_SEMICOLON,
    T_STRINGLIT,
    T_INTLIT,
    T_RETURN,
    T_IF,
    T_ELSE,
    T_FOR,
    T_WHILE,
    T_AND,
    T_OR,
    T_BITWISEAND,
    T_BITWISEOR,
    T_BITWISEXOR,
    T_BITWISENOT,
    T_BITSHIFTLEFT,
    T_BITSHIFTRIGHT,
    T_LT,
    T_GT,
    T_LTE,
    T_GTE,
    T_NEQ,
    T_NOT,
    T_LOGICALAND,
    T_LOGICALOR,
    T_ERROR,
    T_COMMENT,
    T_LBRACKET,
    T_RBRACKET,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_MOD,
    T_FLOATLIT,
    T_BOOLLIT,
    T_BREAK,
    T_CONTINUE,

};

// Token struct to hold token type and value
struct Token
{
    TokenType type;
    std::string value;
};

// Utility function to check if a character is a letter or underscore
bool is_alpha(char c)
{
    return std::isalpha(c) || c == '_';
}

// Utility function to check if a character is a digit
bool is_digit(char c)
{
    return std::isdigit(c);
}

// Utility function to check if a character is part of an identifier
bool is_identifier_char(char c)
{
    return std::isalnum(c) || c == '_';
}

// Utility function to print strings with escape sequences properly handled
std::string handle_special_characters(const std::string &str)
{
    std::string result = "";
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '\\' && i + 1 < str.length())
        {
            switch (str[i + 1])
            {
            case 'n':
                result += '\n';
                i++;
                break;
            case 't':
                result += '\t';
                i++;
                break;
            case '"':
                result += '"';
                i++;
                break;
            case '\\':
                result += '\\';
                i++;
                break;
            default:
                result += str[i];
                break;
            }
        }
        else
        {
            result += str[i];
        }
    }
    return result;
}

// Function to handle escaped characters inside string literals
std::string handle_escape_sequences(const std::string &str)
{
    std::string result = "";
    bool in_escape = false;

    for (size_t i = 0; i < str.length(); i++)
    {
        if (in_escape)
        {
            switch (str[i])
            {
            case '"':
                result += '"';
                break;
            case '\\':
                result += '\\';
                break;
            case 'n':
                result += '\n';
                break;
            case 't':
                result += '\t';
                break;
            default:
                result += str[i];
                break;
            }
            in_escape = false;
        }
        else
        {
            if (str[i] == '\\')
            {
                in_escape = true;
            }
            else
            {
                result += str[i];
            }
        }
    }
    return result;
}

// Helper function to convert TokenType enum to string
std::string token_type_to_string(TokenType type)
{
    switch (type)
    {
    case TokenType::T_FUNCTION:
        return "T_FUNCTION";
    case TokenType::T_INT:
        return "T_INT";
    case TokenType::T_FLOAT:
        return "T_FLOAT";
    case TokenType::T_BOOL:
        return "T_BOOL";
    case TokenType::T_STRING:
        return "T_STRING";
    case TokenType::T_IDENTIFIER:
        return "T_IDENTIFIER";
    case TokenType::T_ASSIGNOP:
        return "T_ASSIGNOP";
    case TokenType::T_EQUALSOP:
        return "T_EQUALSOP";
    case TokenType::T_PARENL:
        return "T_PARENL";
    case TokenType::T_PARENR:
        return "T_PARENR";
    case TokenType::T_COMMA:
        return "T_COMMA";
    case TokenType::T_BRACEL:
        return "T_BRACEL";
    case TokenType::T_BRACER:
        return "T_BRACER";
    case TokenType::T_SEMICOLON:
        return "T_SEMICOLON";
    case TokenType::T_STRINGLIT:
        return "T_STRINGLIT";
    case TokenType::T_INTLIT:
        return "T_INTLIT";
    case TokenType::T_RETURN:
        return "T_RETURN";
    case TokenType::T_IF:
        return "T_IF";
    case TokenType::T_NOT:
        return "T_NOT";
    case TokenType::T_ELSE:
        return "T_ELSE";
    case TokenType::T_FOR:
        return "T_FOR";
    case TokenType::T_WHILE:
        return "T_WHILE";
    case TokenType::T_AND:
        return "T_AND";
    case TokenType::T_OR:
        return "T_OR";
    case TokenType::T_BITWISEAND:
        return "T_BITWISEAND";
    case TokenType::T_BITWISEOR:
        return "T_BITWISEOR";
    case TokenType::T_BITWISEXOR:
        return "T_BITWISEXOR";
    case TokenType::T_BITWISENOT:
        return "T_BITWISENOT";
    case TokenType::T_BITSHIFTLEFT:
        return "T_BITSHIFTLEFT";
    case TokenType::T_BITSHIFTRIGHT:
        return "T_BITSHIFTRIGHT";
    case TokenType::T_LTE:
        return "T_LTE";
    case TokenType::T_GTE:
        return "T_GTE";
    case TokenType::T_LT:
        return "T_LT";
    case TokenType::T_GT:
        return "T_GT";
    case TokenType::T_NEQ:
        return "T_NEQ";
    case TokenType::T_LOGICALAND:
        return "T_LOGICALAND";
    case TokenType::T_LOGICALOR:
        return "T_LOGICALOR";
    case TokenType::T_LBRACKET:
        return "T_LBRACKET";
    case TokenType::T_RBRACKET:
        return "T_RBRACKET";
    case TokenType::T_COMMENT:
        return "T_COMMENT";
    case TokenType::T_ERROR:
        return "T_ERROR";
    case TokenType::T_PLUS:
        return "T_PLUS";
    case TokenType::T_MINUS:
        return "T_MINUS";
    case TokenType::T_MUL:
        return "T_MUL";
    case TokenType::T_DIV:
        return "T_DIV";
    case TokenType::T_MOD:
        return "T_MOD";
    case TokenType::T_FLOATLIT:
        return "T_FLOATLIT";
    case TokenType::T_BOOLLIT:

        return "T_BOOLLIT";
    case TokenType::T_BREAK:
        return "T_BREAK";
    case TokenType::T_CONTINUE:
        return "T_CONTINUE";

    default:
        return "UNKNOWN";
    }
}

// Print tokens for debugging, showing type names
void print_tokens(const std::vector<Token> &tokens)
{
    for (const auto &token : tokens)
    {
        std::string value = token.value;

        // If the token is a string literal, handle special characters (escape sequences)
        if (token.type == TokenType::T_STRINGLIT)
        {
            value = handle_special_characters(value);
        }

        std::cout << token_type_to_string(token.type)
                  << "(\"" << value << "\"), ";
    }
}

// Tokenize function based on the Maximal Munch rule
std::vector<Token> tokenize(const std::string &code)
{
    std::vector<Token> tokens;
    size_t start = 0;

    while (start < code.length())
    {
        if (std::isspace(code[start]))
        {
            start++;
            continue;
        }

        // Handle comments (single-line and multi-line)
        else if (code.substr(start, 2) == "//")
        {
            size_t end = code.find('\n', start);
            //tokens.push_back({TokenType::T_COMMENT, code.substr(start, end - start)});
            start = (end == std::string::npos) ? code.length() : end;
            continue;
        }

        else if (code.substr(start, 2) == "/*")
        {
            size_t end = code.find("*/", start);
            if (end != std::string::npos)
            {
                //tokens.push_back({TokenType::T_COMMENT, code.substr(start, end - start + 2)});
                start = end + 2;
            }
            else
            {
                //tokens.push_back({TokenType::T_ERROR, code.substr(start, 2)});
                start = code.length();
            }
            continue;
        }

        // Match keywords and operators first (longer tokens)
        // Handle mathematical operators: +, -, *, /

        else if (code[start] == '+')
        {
            tokens.push_back({TokenType::T_PLUS, "+"});
            start++;
        }
        else if (code[start] == '-')
        {
            tokens.push_back({TokenType::T_MINUS, "-"});
            start++;
        }
        else if (code[start] == '*')
        {
            tokens.push_back({TokenType::T_MUL, "*"});
            start++;
        }
        else if (code[start] == '/')
        {
            tokens.push_back({TokenType::T_DIV, "/"});
            start++;
        }
        else if (code[start] == '%')
        {
            tokens.push_back({TokenType::T_MOD, "%"});
            start++;
        }
        else if (code[start] == '!')
        {
            tokens.push_back({TokenType::T_NOT, "!"});
            start++;
        }

        // Handle assignment, equality and comparison operators
        else if (code.substr(start, 2) == "==")
        {
            tokens.push_back({TokenType::T_EQUALSOP, "=="});
            start += 2;
        }
        else if (code.substr(start, 2) == "!=")
        {
            tokens.push_back({TokenType::T_NEQ, "!="});
            start += 2;
        }
        else if (code[start] == '=')
        {
            tokens.push_back({TokenType::T_ASSIGNOP, "="});
            start++;
        }
        else if (code.substr(start, 2) == "<=")
        {
            tokens.push_back({TokenType::T_LTE, "<="});
            start += 2;
        }
        else if (code.substr(start, 2) == ">=")
        {
            tokens.push_back({TokenType::T_GTE, ">="});
            start += 2;
        }
        else if (code.substr(start, 2) == "<<")
        {
            tokens.push_back({TokenType::T_BITSHIFTLEFT, "<<"});
            start += 2;
        }
        else if (code.substr(start, 2) == ">>")
        {
            tokens.push_back({TokenType::T_BITSHIFTRIGHT, ">>"});
            start += 2;
        }
        else if (code[start] == '<')
        {
            tokens.push_back({TokenType::T_LT, "<"});
            start++;
        }
        else if (code[start] == '>')
        {
            tokens.push_back({TokenType::T_GT, ">"});
            start++;
        }
        // Handle logical and bitwise operators
        else if (code.substr(start, 2) == "&&")
        {
            tokens.push_back({TokenType::T_LOGICALAND, "&&"});
            start += 2;
        }
        else if (code.substr(start, 2) == "||")
        {
            tokens.push_back({TokenType::T_LOGICALOR, "||"});
            start += 2;
        }
        else if (code[start] == '&')
        {
            tokens.push_back({TokenType::T_BITWISEAND, "&"});
            start++;
        }
        else if (code[start] == '|')
        {
            tokens.push_back({TokenType::T_BITWISEOR, "|"});
            start++;
        }
        else if (code[start] == '^')
        {
            tokens.push_back({TokenType::T_BITWISEXOR, "^"});
            start++;
        }
        else if (code[start] == '~')
        {
            tokens.push_back({TokenType::T_BITWISENOT, "~"});
            start++;
        }

        // Match brackets and parentheses
        else if (code[start] == '(')
        {
            tokens.push_back({TokenType::T_PARENL, "("});
            start++;
        }
        else if (code[start] == ')')
        {
            tokens.push_back({TokenType::T_PARENR, ")"});
            start++;
        }
        else if (code[start] == '{')
        {
            tokens.push_back({TokenType::T_BRACEL, "{"});
            start++;
        }
        else if (code[start] == '}')
        {
            tokens.push_back({TokenType::T_BRACER, "}"});
            start++;
        }
        else if (code[start] == '[')
        {
            tokens.push_back({TokenType::T_LBRACKET, "["});
            start++;
        }
        else if (code[start] == ']')
        {
            tokens.push_back({TokenType::T_RBRACKET, "]"});
            start++;
        }

        // Handle comma and semicolon
        else if (code[start] == ',')
        {
            tokens.push_back({TokenType::T_COMMA, ","});
            start++;
        }
        else if (code[start] == ';')
        {
            tokens.push_back({TokenType::T_SEMICOLON, ";"});
            start++;
        }

        // Match Identifiers & Keywords (including bool literals)
        else if (is_alpha(code[start]))
        {
            size_t end = start;
            while (end < code.size() && is_identifier_char(code[end]))
                end++;
            std::string id = code.substr(start, end - start);

            if (id == "fn")
                tokens.push_back({TokenType::T_FUNCTION, id});
            else if (id == "int")
                tokens.push_back({TokenType::T_INT, id});
            else if (id == "float")
                tokens.push_back({TokenType::T_FLOAT, id});
            else if (id == "bool")
                tokens.push_back({TokenType::T_BOOL, id});
            else if (id == "string")
                tokens.push_back({TokenType::T_STRING, id});
            else if (id == "return")
                tokens.push_back({TokenType::T_RETURN, id});
            else if (id == "if")
                tokens.push_back({TokenType::T_IF, id});
            else if (id == "else")
                tokens.push_back({TokenType::T_ELSE, id});
            else if (id == "for")
                tokens.push_back({TokenType::T_FOR, id});
            else if (id == "while")
                tokens.push_back({TokenType::T_WHILE, id});
            else if (id == "break")
                tokens.push_back({TokenType::T_BREAK, id});
            else if (id == "continue")
                tokens.push_back({TokenType::T_CONTINUE, id});
            else if (id == "true" || id == "false")
                tokens.push_back({TokenType::T_BOOLLIT, id}); // <-- added
            else
                tokens.push_back({TokenType::T_IDENTIFIER, id});

            start = end;
            continue;
        }

        // Match number literals (int or float) OR invalid identifiers like 1var
        else if (is_digit(code[start]))
        {
            size_t end = start;
            bool is_float = false;
            bool valid = true;

            while (end < code.length() && (std::isdigit(code[end]) || code[end] == '.'))
            {
                if (code[end] == '.')
                {
                    if (is_float) // second dot found → invalid
                    {
                        valid = false;
                        break;
                    }
                    is_float = true;
                }
                end++;
            }

            // Check if the number is immediately followed by identifier characters
            if (end < code.length() && is_alpha(code[end]))
            {
                // Example: "1var", "123abc" → invalid identifier
                size_t bad_end = end;
                while (bad_end < code.length() && is_identifier_char(code[bad_end]))
                    bad_end++;

                tokens.push_back({TokenType::T_ERROR, code.substr(start, bad_end - start)});
                start = bad_end;
            }
            else
            {
                std::string number = code.substr(start, end - start);
                if (!valid)
                {
                    tokens.push_back({TokenType::T_ERROR, number});
                }
                else
                {
                    if (is_float)
                        tokens.push_back({TokenType::T_FLOATLIT, number});
                    else
                        tokens.push_back({TokenType::T_INTLIT, number});
                }
                start = end;
            }
        }

        // Match string literals
        else if (code[start] == '"') // Start of string literal
        {
            size_t end = start + 1;
            // Scan the string until we find the closing quote, ignoring escaped quotes
            while (end < code.length())
            {
                if (code[end] == '\\' && end + 1 < code.length() && code[end + 1] == '"')
                {
                    // Skip over escaped quote (i.e., \" should be treated as part of the string)
                    end += 2;
                }
                else if (code[end] == '"')
                {
                    // End of string literal found
                    break;
                }
                else
                {
                    // Continue scanning the string
                    end++;
                }
            }

            if (end < code.length() && code[end] == '"')
            {
                // We have found the closing quote
                std::string str_literal = code.substr(start, end - start + 1);
                // Now handle escape sequences inside the string
                std::string handled_string = handle_escape_sequences(str_literal);
                tokens.push_back({TokenType::T_STRINGLIT, handled_string});
                start = end + 1; // Move start to after the closing quote
            }
            else
            {
                // Error: No matching closing quote found
                tokens.push_back({TokenType::T_ERROR, code.substr(start, 1)});
                start++;
            }
        }

        // If no match is found, report an error
        else
        {
            tokens.push_back({TokenType::T_ERROR, std::string(1, code[start])});
            start++;
        }
    }

    return tokens;
}

int main()
{
    // Sample code to tokenize
    std::string code = R"(
    int main() {
        // This is a comment
        int x = 42;
        float y = 3.14159;
        string str = "Hello\tWorld! \"quoted\"";
        
        /* Multi-line
        comment */
        
        if (x > 0 && x <= MAX) {
            printf("Value: %d\n", x);
        } else if (x == 0 || x >= MAX) {
            printf("Boundary value\n");
        }
        
        for (int i = 0; i < 10; i = i + 1) {
            x = x + i;
            x = x + 1;
            x = x - 1;
        }
        
        while (x > 0) {
            x >> 1;  // Right shift
        }
        
        return 0;
}
    )";

    // Tokenize the code using Maximal Munch
    std::vector<Token> tokens = tokenize(code);

    // Print out the tokens with type names
    print_tokens(tokens);

    return 0;
}
