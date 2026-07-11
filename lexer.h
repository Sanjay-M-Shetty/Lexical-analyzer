#ifndef LEXER_H
#define LEXER_H

#define MAX_KEYWORDS 20
#define MAX_TOKEN_SIZE 100
// Type-safe constants replacing macros
// static const int MAX_KEYWORDS = 20;
// static const int MAX_TOKEN_SIZE = 100;

typedef enum {
    KEYWORD,// Represents reserved language words like 'int', 'return', or 'if'.
    OPERATOR,// Represents symbols used for operations like '+', '-', '=', or '&&'.
    SPECIAL_CHARACTER,// Represents syntax punctuation like ';', '{', '}', or ','.
    CONSTANT,// Represents numeric values like integers or floating-point constants.
    LITERAL,// Represents string or character literals enclosed in quotes.
    IDENTIFIER,// Represents user-defined names for variables, functions, or structures.
    UNKNOWN,// Represents invalid or unrecognized character sequences.
    EOF_TOKEN// Represents the end of the input source file.
} TokenType;

typedef struct {
    char lexeme[MAX_TOKEN_SIZE];//array storing the actual text string extracted from the source code.
    TokenType type;//variable storing the classified category of the token
} Token;

void initializeLexer(const char* filename);// Function prototype to open the source file and set up initial lexer states.
Token getNextToken();// Function prototype to scan and return the next token from the input stream.
void categorizeToken(Token* token);// Function prototype to analyze a token's text and update its classified type.
int isKeyword(const char* str);// Helper function prototype to check if a given string matches a C keyword.
int isOperator(const char* str);// Helper function prototype to check if a given string is a valid operator symbol.
int isSpecialCharacter(char ch);// Helper function prototype to check if a single character is a special punctuation symbol.
int isConstant(const char* str);// Helper function prototype to check if a given string represents a valid numeric constant.
int isIdentifier(const char* str);// Helper function prototype to check if a given string matches variable naming rules.

#endif
