#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_SIZE 100

const char* keywords[] = {
    "int", "float", "char", "double", "return", "if", "else", "for", "while",
    "do", "break", "continue", "switch", "case", "default", "void", "long",
    "short", "unsigned", "signed", "const", "static", "struct", "union",
    "typedef", "enum", "extern", "goto", "sizeof", "volatile", "register", "auto","printf"
};

const char* operators[] = {
    "+", "-", "*", "/", "=", "==", "!=", "<", "<=", ">", ">=", "&&", "||", "%",
    "++", "--"
};

int isKeyword(const char *token) {
    for (int i = 0; i < sizeof(keywords)/sizeof(keywords[0]); i++) {
        if (strcmp(token, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

int isOperator(const char *token) {
    for (int i = 0; i < sizeof(operators)/sizeof(operators[0]); i++) {
        if (strcmp(token, operators[i]) == 0)
            return 1;
    }
    return 0;
}

void classifyToken(FILE *out, const char *token, int tokenNumber) {
    if (strlen(token) == 0)
        return;

    if (isKeyword(token)) {
        fprintf(out, "Token %d: %s - Keyword\n", tokenNumber, token);
    } else if (isOperator(token)) {
        fprintf(out, "Token %d: %s - Operator\n", tokenNumber, token);
    } else if (isdigit(token[0])) {
        fprintf(out, "Token %d: %s - Constant\n", tokenNumber, token);
    } else if (isalpha(token[0]) || token[0] == '_') {
        fprintf(out, "Token %d: %s - Identifier\n", tokenNumber, token);
    } else {
        fprintf(out, "Token %d: %s - Unknown\n", tokenNumber, token);
    }
}

int isDelimiter(char ch) {
    return ch == ' ' || ch == '\n' || ch == ',' || ch == ';' ||
           ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '"';
}

int main() {
    FILE *fin = fopen("input_code.txt", "r");
    FILE *fout = fopen("output_tokens.txt", "w");
    char ch, token[MAX_TOKEN_SIZE];
    int i = 0, tokenCount = 1;

    if (fin == NULL) {
        fin = fopen("input_code.txt", "w");
        fprintf(fin, "// Write your C code here...\n");
        fclose(fin);
        printf("✅ input_code.txt created.\nPlease write your code in it and run the program again.\n");
        return 0;
    }

    while ((ch = fgetc(fin)) != EOF) {
        if (ch == '#') {
            // Skip preprocessor line
            while ((ch = fgetc(fin)) != '\n' && ch != EOF);
            continue;
        }

        if (!isDelimiter(ch)) {
            token[i++] = ch;
        } else {
            if (i > 0) {
                token[i] = '\0';
                classifyToken(fout, token, tokenCount++);
                i = 0;
            }

            // Check for operators (like =, ==, ++)
            if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' ||
                ch == '<' || ch == '>' || ch == '!' || ch == '%') {

                char next = fgetc(fin);
                char op[3] = {ch, '\0', '\0'};

                if (next == '=' || (ch == '+' && next == '+') || (ch == '-' && next == '-')) {
                    op[1] = next;
                } else {
                    ungetc(next, fin);
                }

                classifyToken(fout, op, tokenCount++);
            }
        }
    }

    // Final token
    if (i > 0) {
        token[i] = '\0';
        classifyToken(fout, token, tokenCount++);
    }

    fclose(fin);
    fclose(fout);

    printf("Tokenization complete. Check 'output_tokens.txt'.\n");
    return 0;
}
