
/* 
   Name: SANJAY M SHETTY
   ADMISSION NUMBER: 25048_056
   Date: 19-06-2026

   Project Description:
        This project is a simple tool that reads a C program file letter by letter. 
        It looks at the text in order and breaks it down into small, meaningful pieces called tokens. 
        It then groups these tokens into clear categories—like keywords, names, and symbols—based on standard C programming rules.

   Key Functionalities:

    Based on the project requirements from Emertxe, the system performs the following main functions:

        * Keyword Identification: Recognizes and isolates standard C language keywords (such as int, float, if, while, and return).

        * Identifier Tracking: Detects user-defined names used for variables, arrays, and functions.

        * Literal and Constant Extraction: Finds and categorizes different data constants, including integers (decimals), floating-point numbers, characters, and string literals.

        * Tokenized Output Display: Prints out a clean, structured list mapping each isolated text segment (lexeme) to its corresponding token category.

    Note:
    it also checks the syntax error for paranthesis and Literal seuence;

    I used ANSI escape codes to dynamically colorize my terminal text.
    * \033[0;31m (Red): Used to draw high-visibility attention to syntax errors, unmatched brackets, and file loading failures.
    * \033[0;32m (Green): Used to display positive progress metrics, layout borders, and successful parsing status reports.
    * \033[0m (Reset): Appended to the end of every message to revert the terminal text back to your system's default color settings.

    Project Conclusion:
        This Lexical Analyzer project successfully implements a foundational compiler front-end component using pure C.
        the project demonstrates a strong grasp of low-level string processing, standard C grammar parsing, and state-machine design. 
        Ultimately, it provides a highly efficient, single-pass solution for converting raw source code into structured tokens, serving as a critical first step toward understanding full compiler design.

*/

#include <stdio.h>
#include <string.h>
#include "lexer.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ./a.out <.c file>\n");
        return 1;
    }

    initializeLexer(argv[1]);// Calls the external function to open and configure the lexer for the specified file.

    // Syntax tracking variables
    int brace_count = 0;// Integer variable to track the nesting level balance of curly braces {}.
    int paren_count = 0;// Integer variable to track the nesting level balance of parentheses ().
    int syntax_errors = 0;// Integer variable to accumulate the total count of syntax issues encountered.

    Token token;// declaring struct variable
    while (1) {
        token = getNextToken();// Extracts the next token from the file stream via the lexer engine.
        if (token.type == EOF_TOKEN) {// Evaluates if the lexer has reached the absolute end of the input file.
            break;
        }
        // Print token info
        if (token.type == KEYWORD) {// Checks if the current token has been categorized as a C keyword.
            printf("Keyword           : %s\n", token.lexeme);
        } else if (token.type == IDENTIFIER) {// Checks if the current token matches the structural rules of an identifier.
            printf("Identifier        : %s\n", token.lexeme);
        } else if(token.type == CONSTANT){// Checks if the current token represents a literal numeric constant value.
            printf("Constant          : %s\n", token.lexeme);
        } else if (token.type == LITERAL) {// Checks if the current token is a character or string literal value.
            printf("Literal           : %s\n", token.lexeme);
            
            // Check for unclosed quotes handled by the lexer buffer safely
            size_t len = strlen(token.lexeme);// Computes the absolute string length of the extracted literal lexeme.
            if (len > 0 && ((token.lexeme[0] == '"' && token.lexeme[len-1] != '"') || // Evaluates if a string double quote is missing its closing counterpart.
                            (token.lexeme[0] == '\'' && token.lexeme[len-1] != '\''))) {// Evaluates if a character single quote is missing its closing counterpart.
                printf("\033[0;31mSyntax Error      : Unclosed literal sequence [%s]\033[0m\n", token.lexeme);
                syntax_errors++;// Increments the global syntactic error counter tracking metric.
            }
        } else if (token.type == OPERATOR) {// Checks if the current token corresponds to a built-in operator symbol.
            printf("Operator          : %s\n", token.lexeme);
        } else if (token.type == SPECIAL_CHARACTER) {// Checks if the current token matches a syntax punctuation boundary character.
            printf("Special Character : %s\n", token.lexeme);
            
            // Syntax Tracking Logic
            if (token.lexeme[0] == '{') brace_count++;// Increments the tracking counter when an opening curly brace is found.
            else if (token.lexeme[0] == '}') {// Evaluates if the current character matches a closing curly brace symbol.
                brace_count--;// Decrements the tracking counter when a closing curly brace is encountered.
                if (brace_count < 0) {// Checks if a closing brace was found without an active preceding opening brace.
                    printf("\033[0;31mSyntax Error      : Unmatched closing brace '}'\033[0m\n");
                    syntax_errors++;// Increments the cumulative syntax discrepancy tracking count indicator.
                    brace_count = 0; // reset to avoid compounding cascades// Re-aligns the bracket tracker to zero to prevent downstream error cascading.
                }
            }
            else if (token.lexeme[0] == '(') paren_count++;// Increments the matching index tracking counter when an opening parenthesis is discovered.
            else if (token.lexeme[0] == ')') {// Evaluates if the current character matches a closing parenthesis symbol.
                paren_count--;// Decrements the tracking counter when a closing parenthesis is encountered.
                if (paren_count < 0) {// Checks if a closing parenthesis occurs out of order without a matching open pair.
                    printf("\033[0;31mSyntax Error      : Unmatched closing parenthesis ')'\033[0m\n");
                    syntax_errors++;// Increments the total syntax error frequency count tracker variable.
                    paren_count = 0; // Re-aligns the parenthesis tracker counter to zero to protect remaining validation.
                }
            }
        } else {// Fallback branch targeting tokens labeled explicitly with the UNKNOWN enum type.
            printf("Token: %s, Type: Unknown\n", token.lexeme);
        }
    }

    // Post-loop validation checks
    if (brace_count > 0) {// Verifies if any opening curly braces remained unclosed at the end of execution.
        printf("\033[0;31mSyntax Error      : Missing closing brace '}' at end of file\033[0m\n");
        syntax_errors++;// Increments the system structural syntax exception tracker metric.
    }
    if (paren_count > 0) {// Verifies if any opening parenthesis structures remained unclosed at the end of execution.
        printf("\033[0;31mSyntax Error      : Missing closing parenthesis ')'\033[0m\n");
        syntax_errors++;// Increments the system structural syntax exception tracker metric.
    }

    // --- Observation Summary block ---
    printf("\033[0;32m------------------------------\033[0m\n");
    printf("Parsing: %s: \033[0;32mDone\033[0m\n", argv[1]);
    printf("\033[0;32m------------------------------\033[0m\n");
    
    printf("\n=== OBSERVATION REPORT ===\n");
    if (syntax_errors == 0) {// Determines if the targeted application source document verified completely without structural anomalies.
        printf("Status            : \033[0;32mSUCCESS\033[0m\n");
        printf("Details           : No syntax errors detected. All pairs balances matched.\n");
    } else {// Executes if structural parsing generated anomalies during execution.
        printf("Status            : \033[0;31mFAILURE\033[0m\n");
        printf("Details           : Found \033[0;31m%d\033[0m syntax structural discrepancy error(s).\n", syntax_errors);
    }
    printf("==========================\n");
    
    //return (syntax_errors > 0) ? 1 : 0;
    return 0;
}