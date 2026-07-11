#include <stdio.h>
#include <stdlib.h>// Standard library for system utilities, including the exit() function.
#include <string.h>
#include <ctype.h>// Character classification library for checking alphanumeric and space types.
#include "lexer.h"

static const char* keywords[MAX_KEYWORDS] = {// Fixed array of strings defining the standard C language keywords recognized by this lexer.
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

// Dynamic symbol table to remember variables declared in the file
#define MAX_DYNAMIC_IDENTIFIERS 100// Preprocessor directive defining the maximum capacity of the dynamic symbol table.
static char dynamicIdentifiers[MAX_DYNAMIC_IDENTIFIERS][MAX_TOKEN_SIZE]; // 2D character array acting as storage for dynamically tracked variable names.
static int dynamicIdentifierCount = 0;//It keeps track of how many slots are currently filled.but starts from 0 // Integer counter tracking the current number of registered unique identifiers.

static const char* operators = "+-*/%=!<>|&";// String containing all valid individual character operators handled by the lexer.
static const char* specialCharacters = ",;{}()[]";// String containing all valid individual punctuation and grouping boundary characters.

static FILE* file = NULL; // Static pointer tracking the active input stream file structure.
static int expectsVariableDeclaration = 0; // Flag to track if the next word is a variable declaration. mainly for keywords or identifiers // State flag indicating that the next valid string sequence should be parsed as a new variable declaration.

// Helper to track dynamically declared identifiers
// Function to store a newly encountered variable name into the dynamic symbol table.
void registerIdentifier(const char* name) {
    // Avoid duplicates
    for (int i = 0; i < dynamicIdentifierCount; i++) {
        if (strcmp(dynamicIdentifiers[i], name) == 0) return; // Exits the registration function immediately if the identifier is already in the table.
    }
    if (dynamicIdentifierCount < MAX_DYNAMIC_IDENTIFIERS) { // Checks if the symbol table has remaining capacity before performing an insertion.
        strncpy(dynamicIdentifiers[dynamicIdentifierCount++], name, MAX_TOKEN_SIZE - 1); // Copies the new identifier string safely into the next open table slot and increments the count.
    }
}

int isRegisteredIdentifier(const char* name) { // Function to determine if a scanned word is a known valid variable or core function name.
    if (strcmp(name, "main") == 0 || strcmp(name, "printf") == 0) { // Explicitly permits standard baseline environment entries like main or printf.
        return 1; // Returns truthy to confirm validation for baseline environment names.
    }
    // Check dynamically registered variables
    for (int i = 0; i < dynamicIdentifierCount; i++) {
        if (strcmp(dynamicIdentifiers[i], name) == 0) { // Evaluates if the current query matches a previously saved variable name.
            return 1; // Returns truthy to confirm the identifier is registered and valid.
        }
    }
    return 0; // Returns falsy if the string does not match any known valid identifier names.
}

// for coloring:
// \033[: This kicks off the escape sequence.
// 0;31m: Sets the terminal color to Red.
// 0;32m: Sets the terminal color to Green.
// 0m: Resets the terminal color back to your system's default text settings. Always include this at the end of your string, or your entire terminal window will stay green/red forever!

void initializeLexer(const char* filename) { // Sets up the lexer environment by opening the specified target source text file.
    file = fopen(filename, "r"); // Attempts to open the target source document in read-only mode.
    if (file == NULL) {
        printf("\n\033[0;31m------------------------------\033[0m\n");
        //printf("Open : %s: Failure\n", filename);
        printf("Open : %s: \033[0;31mFailure\033[0m\n", filename);
        printf("\033[0;31m------------------------------\033[0m\n\n");
        exit(1); // Hard-terminates the entire application run immediately due to the missing critical file resource.
    }
    printf("\n\033[0;32m------------------------------\033[0m\n");
    //printf("Open : %s: Success\n", filename);
    printf("Open : %s: \033[0;32mSuccess\033[0m\n", filename);
    printf("Parsing: %s: Started\n", filename);
    printf("\033[0;32m------------------------------\033[0m\n\n");
    printf("\n======Lexical analyzer output======\n\n");
    // printf("\033[0;32m------------------------------\033[0m\n\n");
    dynamicIdentifierCount = 0; // Explicitly clears the dynamic symbol tracker back to zero to prepare for a fresh file run.
    expectsVariableDeclaration = 0; // Disables the declaration state tracker flag to guarantee a clean initial state machine baseline.
}

int isKeyword(const char* str) { // Function to determine if a text string matches any registered fixed C language keyword.
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(str, keywords[i]) == 0) { // Compares the scanned text string against the current array keyword entry.
            return 1; // Returns truthy immediately upon finding an exact matching keyword match.
        }
    }
    return 0; // Returns falsy if the string does not correspond to any known language keywords.
}

int isOperator(const char* str) { // Function to verify if the initial index of a string matches a valid mathematical or logical operator.
    return strchr(operators, str[0]) != NULL; // Returns true if the first character of the string can be found inside the pre-defined operators list.
}

int isSpecialCharacter(char ch) { // Function to evaluate if an individual isolated character matches a valid special punctuation element.
    return strchr(specialCharacters, ch) != NULL; // Returns true if the character is located inside the pre-defined structural punctuation list.
}

int isConstant(const char* str) { // Function to determine if a string element matches the base structural start of a literal constant value.
    return isdigit((unsigned char)str[0]) || str[0] == '"' || str[0] == '\''; // Validates true if the string begins with a base digit or a literal quotation character boundary wrapper.
}

int isIdentifier(const char* str) { // Function to validate if a text sequence adheres to standard identifier character naming constraints.
    if (!isalpha((unsigned char)str[0]) && str[0] != '_') return 0; // Declares the sequence invalid immediately if the leading letter fails basic variable syntax naming laws.
    for (int i = 1; str[i] != '\0'; i++) {
        if (!isalnum((unsigned char)str[i]) && str[i] != '_') return 0; // Flags the identifier as false if an invalid punctuation or symbol breaks variable naming laws.
    }
    return isRegisteredIdentifier(str); // defers tracking status query validation matching to look inside the active system variable registry.
}

Token getNextToken() { // Primary scanning core engine function that isolates and extracts the next token from the input file stream.
    Token token = {"", UNKNOWN}; // Instantiates a fresh blank local token structure initialized with an UNKNOWN structural type baseline.
    int ch; // Declares an integer container variable to capture individual character code data read from the file.

    // Skip whitespace-->'', \n, \t, \v, \f, \r
    while ((ch = fgetc(file)) != EOF && isspace((unsigned char)ch));//isspace is a std library in c, check the whitespace charaters // Consumes sequential whitespace bytes from the file stream continuously until a distinct valid asset block emerges.

    if (ch == EOF) { // Evaluates if the extraction sequence has collided with the absolute final limit block of the target document.
        token.type = EOF_TOKEN; // Assigns the structural token category identifier indicating end-of-file has been achieved.
        return token; // Delivers the populated end-of-file token back to the main loop controller structure.
    }

    // Ignore preprocessor directives
    if (ch == '#') { // Detects if the current character marks the beginning of a preprocessor directive line.
        while ((ch = fgetc(file)) != EOF && ch != '\n') { // Iterates continuously across the current line to discard macro content text characters.
            // consume line
        }
        return getNextToken(); // Recursively invokes the scanning routine to acquire the next actual valid syntax element below the macro.
    }

    // String literals
    if (ch == '"') { // Evaluates if the currently scanned byte represents an opening double quote defining a string literal sequence.
        int i = 0; // Registers a local position index counter to manage token lexeme string character insertion layout.
        token.lexeme[i++] = ch; // Stores the opening double quote character directly inside the token buffer array.
        while ((ch = fgetc(file)) != EOF && ch != '"' && i < MAX_TOKEN_SIZE - 2) { // Consumes characters inside the string until a closing quote or buffer overflow boundary appears.
            token.lexeme[i++] = ch; // Incorporates the current string content character code into the building lexeme buffer array.
            if (ch == '\\') { // Evaluates if the current character is an escape backslash requiring special literal transparency management.
                ch = fgetc(file); // Extractions the immediate following character directly to bypass natural boundary processing evaluation.
                if (ch != EOF) { // Confirms the escaped character represents a real byte rather than an abrupt file truncation event.
                    token.lexeme[i++] = ch; // Appends the escaped character code directly into the growing literal token string sequence array.
                }
            }
        }
        if (ch == '"') { // Evaluates if the character loop completed correctly by matching against a valid closing double quote symbol.
            token.lexeme[i++] = '"'; // Locks the explicit matching closing double quote directly into the token string character sequence.
        }
        token.lexeme[i] = '\0'; // Places a null-terminator byte at the final index boundary of the token string buffer array.
        token.type = LITERAL; // Assigns the literal token classification type category onto the current item profile.
        expectsVariableDeclaration = 0; // Reset // Toggles off the context flag since a string literal cannot be a variable declaration type descriptor.
        return token; // Delivers the successfully packaged string literal token back to the parent execution routine.
    }
    
    // Character literals
    if (ch == '\'') { // Evaluates if the currently scanned byte represents an opening single quote defining a character literal sequence.
        int i = 0;
        token.lexeme[i++] = ch; // Stores the opening single quote character directly inside the token buffer array.
        while ((ch = fgetc(file)) != EOF && ch != '\'' && i < MAX_TOKEN_SIZE - 2) { // Consumes characters inside the quotes until a closing quote or buffer limit boundary appears.
            token.lexeme[i++] = ch; // Incorporates the active character constant component directly into the building lexeme buffer array.
            if (ch == '\\') { // Evaluates if the character represents a backslash indicating a special escaped literal symbol configuration.
                ch = fgetc(file); // Extractions the immediate following character directly to bypass natural boundary processing evaluation.
                if (ch != EOF) { // Confirms the escaped character represents a real byte rather than an abrupt file truncation event.
                    token.lexeme[i++] = ch; // Appends the escaped character code directly into the growing literal token string sequence array.
                }
            }
        }
        if (ch == '\'') { // Evaluates if the character loop completed correctly by matching against a valid closing single quote symbol.
            token.lexeme[i++] = '\''; // Locks the explicit matching closing single quote directly into the token string character sequence.
        }
        token.lexeme[i] = '\0'; // Places a null-terminator byte at the final index boundary of the token string buffer array.
        token.type = LITERAL; // Assigns the literal token classification type category onto the current item profile.
        expectsVariableDeclaration = 0; // Reset // Toggles off the context flag since a character literal cannot be a variable declaration type descriptor.
        return token; // Delivers the successfully packaged character literal token back to the parent execution routine.
    }

    // Special characters
    if (isSpecialCharacter(ch)) { // Evaluates if the character isolated from the file stream matches a valid punctuation boundary symbol.
        token.lexeme[0] = ch; // Stores the individual isolated punctuation character directly at the origin of the token string buffer.
        token.lexeme[1] = '\0'; // Assigns the terminal null byte marker directly to secure the single character string limit.
        token.type = SPECIAL_CHARACTER; // Assigns the structural special character classification identifier onto the current item profile.
        
        if (ch == ';' || ch == '}' || ch == ')' || ch == ',') {
            expectsVariableDeclaration = 0; // Nullifies the contextual type tracker flag because a statement boundary reset has occurred.
        }
        return token; // Returns the single-character structural token entity immediately to the calling router process.
    }

    // Operators
    if (strchr(operators, ch)) { // Evaluates if the character isolated from the input file stream is a known operator symbol component.
        int i = 0;
        token.lexeme[i++] = ch; // Stores the initial operator character code directly inside the building token buffer array.
        int next_ch = fgetc(file); // Proactively extracts the subsequent character from the file stream to evaluate potential multi-byte compound operators.
        
        if (next_ch != EOF && i < MAX_TOKEN_SIZE - 1) { // Confirms the extracted character is valid and that buffer capacity allows compound processing.
            int valid_pair = 0; //flag // Registers an internal validation state flag tracking compound multi-byte operator compliance status.
            if ((ch == '=' && next_ch == '=') || // Checks for the logical assignment equality relational multi-byte operator sequence ==.
                (ch == '!' && next_ch == '=') || // Checks for the logical inequality comparative multi-byte operator sequence !=.
                (ch == '<' && next_ch == '=') || // Checks for the less-than-or-equal relational multi-byte operator sequence <=.
                (ch == '>' && next_ch == '=') || // Checks for the greater-than-or-equal relational multi-byte operator sequence >=.
                (ch == '+' && (next_ch == '=' || next_ch == '+')) || // Checks for compound assignment additive += or increment operator sequence ++.
                (ch == '-' && (next_ch == '=' || next_ch == '-')) || // Checks for compound assignment subtractive -= or decrement operator sequence --.
                (ch == '&' && next_ch == '&') || // Checks for the logical intersection conditional conjunction multi-byte operator sequence &&.
                (ch == '|' && next_ch == '|')) { // Checks for the logical union conditional disjunction multi-byte operator sequence ||.
                valid_pair = 1; // Toggles the internal compound validation status state tracking flag to true.
            }
            
            if (valid_pair) { // Checks if the pair was validated as a multi-character operator symbol compound sequence.
                token.lexeme[i++] = next_ch; // Locks the secondary compound character directly into the active token character buffer list.
            } else { // Executes fallback logic if the secondary character does not create a valid compound assignment or condition.
                ungetc(next_ch, file); // Pushes the unused character back into the input file buffer stream for processing in the next iteration.
            }
        }
        token.lexeme[i] = '\0'; // Plugs the mandatory null character termination entry into the completed operator string buffer.
        token.type = OPERATOR; // Assigns the operator token classification type category onto the current item profile.
        expectsVariableDeclaration = 0; // Reset // Resets the type expectation flag because operator placement interrupts immediate continuous type assignments.
        return token; // Delivers the constructed operator token back to the main loop controller routine.
    }

    // Numbers (Constants)
    if (isdigit((unsigned char)ch)) { // Evaluates if the current byte represents the leading character of a numeric constant sequence.
        int i = 0;
        token.lexeme[i++] = ch; // Records the initial valid numeric digit directly inside the building token buffer array.
        while ((ch = fgetc(file)) != EOF && (isdigit((unsigned char)ch) || ch == '.' || ch == 'e' || ch == 'E' || ch == 'f' || ch == 'F') && i < MAX_TOKEN_SIZE - 1) { // Consumes standard numeric characters, floating points, or exponents.
            token.lexeme[i++] = ch; // Incorporates the valid numeric sequence digit components directly into the growing token string buffer.
        }
        if (ch != EOF) { // Checks if the trailing non-numeric termination character represents an active file token byte.
            ungetc(ch, file); // Pushes the trailing non-numeric character back into the input stream for subsequent scanning passes.
        }
        token.lexeme[i] = '\0'; // Places the required null-terminator character at the end of the extracted numeric sequence string.
        token.type = CONSTANT; // Assigns the constant token classification type category onto the current item profile.
        expectsVariableDeclaration = 0; // Reset // Clears the variable type tracking context flag since raw numbers cannot introduce new variable declarations.
        return token; // Delivers the successfully compiled numeric constant token structure back to the caller.
    }

    // Identifiers and Keywords
    if (isalpha((unsigned char)ch) || ch == '_') { // Evaluates if the current character marks the correct alphabetical or underscore start of an identifier or keyword.
        int i = 0;
        token.lexeme[i++] = ch; // Stores the initial valid character symbol directly at the head of the token string sequence array.
        while ((ch = fgetc(file)) != EOF && (isalnum((unsigned char)ch) || ch == '_') && i < MAX_TOKEN_SIZE - 1) { // Consumes valid trailing alphanumeric or underscore character components.
            token.lexeme[i++] = ch; // Captures and appends the alphanumeric character component into the building text word token buffer array.
        }
        if (ch != EOF) { // Verifies if the non-alphanumeric trailing boundary character represents an active data byte.
            ungetc(ch, file); // Pushes the trailing boundary character back into the stream to keep it available for the next iteration.
        }
        token.lexeme[i] = '\0'; // Secures the final boundaries of the extracted word string buffer by appending a null terminator.

        if (isKeyword(token.lexeme)) { // Checks if the text word sequence matches an entry in the preprocessor keyword database array.
            token.type = KEYWORD; // Configures the token structure classification attribute designation to the KEYWORD category profile type.
            // If it's a data type keyword, the next identifier is likely a variable name creation!
            if (strcmp(token.lexeme, "int") == 0 || strcmp(token.lexeme, "float") == 0 ||  // Triggers lookahead declaration scanning state tracking if a core data type definition keyword is matched.
                strcmp(token.lexeme, "char") == 0 || strcmp(token.lexeme, "double") == 0) { // Continues conditional analysis mapping looking explicitly for basic variable allocation primitives.
                expectsVariableDeclaration = 1; // Elevates the tracking flag to note that the following identifier word sequence represents a new variable allocation.
            } else { // Executes if the detected keyword is a structural or control flow command instead of a data type primitive.
                expectsVariableDeclaration = 0; // Lowers the tracking flag since control keywords do not directly initialize data identifiers.
            }
        } else { // Executes fallback processing if the text word sequence does not match any reserved language keywords.
            // It's a word string sequence
            if (expectsVariableDeclaration) { // Evaluates if the internal state machine requires this specific string word to be treated as a new variable allocation.
                // We are actively declaring this string as a variable! Save it.
                registerIdentifier(token.lexeme); // Commits the newly discovered variable word string into the dynamic tracking symbol table.
                token.type = IDENTIFIER; // Formally sets the current token element categorization profile type as a valid IDENTIFIER.
                expectsVariableDeclaration = 0; // Clear flag after registering // Resets the tracking flag down to zero to clear context tracking after successful variable registration.
            } else if (isRegisteredIdentifier(token.lexeme)) { // Checks if the string matches a variable or function name that has already been registered in the table.
                // Known variable or built-in function
                token.type = IDENTIFIER; // Categorizes the token as an IDENTIFIER since it matches a known entry in the variable table.
            } else { // Executes if the text sequence is completely untracked and has no relation to any known keywords or declared variables.
                // Completely random undeclared text string sequence
                token.type = UNKNOWN; // Designates the element with an UNKNOWN type classification because it represents an unmapped text token sequence.
            }
        }
        return token; // Delivers the fully analyzed word string token package structure back to the main router routine.
    }
    
    // Fallback rule
    token.lexeme[0] = ch; // Records the unclassified individual fallback byte directly inside the token string buffer array.
    token.lexeme[1] = '\0'; // Safeguards the character string sequence format boundary by adding a null terminator byte.
    token.type = UNKNOWN; // Explicitly defines the fallback token element with an UNKNOWN structural type profile designation.
    return token; // Returns the fallback token structure back to the parsing pipeline controller loop.
}

void categorizeToken(Token* token) { // Utility function to re-evaluate and assign a category type directly to a token structure pointer reference.
    if (isKeyword(token->lexeme)) token->type = KEYWORD; // Checks for keyword compliance matches and overwrites the category type attribute accordingly.
    else if (isIdentifier(token->lexeme)) token->type = IDENTIFIER; // Checks if the lexeme text sequence matches an identifier profile rule or an entry in the variable registry table.
    else if (token->lexeme[0] == '"' || token->lexeme[0] == '\'') token->type = LITERAL; // Identifies standard quotation characters to classify the object as a text literal element.
    else if (isConstant(token->lexeme)) token->type = CONSTANT; // Determines if the string contents form a structural numeric constant element sequence.
    else if (isOperator(token->lexeme)) token->type = OPERATOR; // Analyzes the target text sequence string characters against the valid mathematical operator database list.
    else if (strlen(token->lexeme) == 1 && isSpecialCharacter(token->lexeme[0])) token->type = SPECIAL_CHARACTER; // Maps isolated individual characters against structural punctuation lists to categorize them as special characters.
    else token->type = UNKNOWN; // Applies the UNKNOWN fallback category classification profile type if all structural mapping rule attempts fail.
}