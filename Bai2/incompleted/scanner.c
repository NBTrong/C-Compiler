/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  while (currentChar != EOF && charCodes[currentChar] == CHAR_SPACE)
    currentChar = readChar();
}

void skipComment() {
  currentChar = readChar();
  while (currentChar != EOF && charCodes[currentChar] != CHAR_TIMES)
    currentChar = readChar();
  if (currentChar != EOF) {
    currentChar = readChar();
    if(charCodes[currentChar] == CHAR_RPAR) {
      currentChar = readChar();
      return;
    }
    skipComment();
  }
  error(ERR_ENDOFCOMMENT, lineNo, colNo);
}

Token* readIdentKeyword() {
  char keyWord[MAX_IDENT_LEN + 1];
  int lineNoOfStartKeyWord = lineNo;
  int colNoOfStartKeyWord = colNo;
  int keyWordLen = 0;
  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT) {
    keyWord[keyWordLen] = currentChar;
    keyWordLen++;
    currentChar = readChar();
  }
  keyWord[keyWordLen] = '\0';

  // Make token with information of keyWord
  TokenType tokenType = checkKeyword(keyWord);
  if (tokenType == TK_NONE) {
    tokenType = TK_IDENT;
  }
  Token* token = makeToken(tokenType, lineNoOfStartKeyWord, colNoOfStartKeyWord);
  strcpy(token->string, keyWord);
  return token;
}

Token* readNumber(void) {
  char keyWord[MAX_IDENT_LEN + 1];
  int lineNoOfStartKeyWord = lineNo;
  int colNoOfStartKeyWord = colNo;
  int keyWordLen = 0;
  while (charCodes[currentChar] == CHAR_DIGIT) {
    keyWord[keyWordLen] = currentChar;
    keyWordLen++;
    currentChar = readChar();
  }
  keyWord[keyWordLen] = '\0';

  // Make token with information of keyWord
  Token* token = makeToken(TK_NUMBER, lineNoOfStartKeyWord, colNoOfStartKeyWord);
  strcpy(token->string, keyWord);
  return token;
}

// Check currentChar is : or := ?
Token* isSbColonOrSbAssign(void) {
  int lineNoBackup = lineNo;
  int colNoBackup = colNo;
  if (currentChar == ':') {
    currentChar = readChar();
    if (currentChar == '=') {
      currentChar = readChar();
      return makeToken(SB_ASSIGN, lineNoBackup, colNoBackup);
    }
    return makeToken(SB_COLON, lineNoBackup, lineNoBackup);
  }
  return NULL;
}

Token* readConstChar() {
  Token* token = makeToken(TK_CHAR, lineNo, colNo);
  currentChar = readChar();
  token->string[0] = currentChar;
  token->string[1] = '\0';
  currentChar = readChar();
  if(currentChar != '\'') {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
  currentChar = readChar();
  return token;
}

Token* getToken(void) {
  Token *token;
  Token *oldToken;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);
  switch (charCodes[currentChar]) {
    case CHAR_SPACE: skipBlank(); return getToken();

    case CHAR_LETTER: return readIdentKeyword();

    case CHAR_DIGIT: return readNumber();

    case CHAR_PLUS: 
      token = makeToken(SB_PLUS, lineNo, colNo);
      readChar();
      return token;

    case CHAR_SEMICOLON: 
      token = makeToken(SB_SEMICOLON, lineNo, colNo);
      readChar();
      return token;

    case CHAR_LPAR: 
      oldToken = makeToken(SB_LPAR, lineNo, colNo);
      currentChar = readChar();
      if(currentChar != EOF && currentChar == '*'){
        skipComment();
        return getToken();
      }
      return oldToken;

    case CHAR_RPAR:
      token = makeToken(SB_RPAR, lineNo, colNo);
      readChar();
      return token;

    case CHAR_EQ: 
      token = makeToken(SB_EQ, lineNo, colNo);
      readChar();
      return token;

    case CHAR_TIMES: 
      token = makeToken(SB_TIMES, lineNo, colNo);
      readChar();
      return token;

    case CHAR_PERIOD: 
      token = makeToken(SB_PERIOD, lineNo, colNo);
      readChar();
      return token;

    case CHAR_COLON:
      return isSbColonOrSbAssign();

    case CHAR_MINUS: 
      token = makeToken(SB_MINUS, lineNo, colNo);
      readChar();
      return token;

    case CHAR_EXCLAIMATION: 
      token = makeToken(SB_NEQ, lineNo, colNo);
      currentChar = readChar();
      if(currentChar != EOF && currentChar != '='){
        error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
      }
      readChar();
      return token;

    case CHAR_COMMA:
      token = makeToken(SB_COMMA, lineNo, colNo);
      readChar();
      return token;

    case CHAR_SINGLEQUOTE: 
      return readConstChar();

    default:
      token = makeToken(TK_NONE, lineNo, colNo);
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
      readChar(); 
      return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
    case TK_NONE: printf("TK_NONE\n"); break;
    case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
    case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
    case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
    case TK_EOF: printf("TK_EOF\n"); break;

    case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
    case KW_CONST: printf("KW_CONST\n"); break;
    case KW_TYPE: printf("KW_TYPE\n"); break;
    case KW_VAR: printf("KW_VAR\n"); break;
    case KW_INTEGER: printf("KW_INTEGER\n"); break;
    case KW_CHAR: printf("KW_CHAR\n"); break;
    case KW_ARRAY: printf("KW_ARRAY\n"); break;
    case KW_OF: printf("KW_OF\n"); break;
    case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
    case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
    case KW_BEGIN: printf("KW_BEGIN\n"); break;
    case KW_END: printf("KW_END\n"); break;
    case KW_CALL: printf("KW_CALL\n"); break;
    case KW_IF: printf("KW_IF\n"); break;
    case KW_THEN: printf("KW_THEN\n"); break;
    case KW_ELSE: printf("KW_ELSE\n"); break;
    case KW_WHILE: printf("KW_WHILE\n"); break;
    case KW_DO: printf("KW_DO\n"); break;
    case KW_FOR: printf("KW_FOR\n"); break;
    case KW_TO: printf("KW_TO\n"); break;

    case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
    case SB_COLON: printf("SB_COLON\n"); break;
    case SB_PERIOD: printf("SB_PERIOD\n"); break;
    case SB_COMMA: printf("SB_COMMA\n"); break;
    case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
    case SB_EQ: printf("SB_EQ\n"); break;
    case SB_NEQ: printf("SB_NEQ\n"); break;
    case SB_LT: printf("SB_LT\n"); break;
    case SB_LE: printf("SB_LE\n"); break;
    case SB_GT: printf("SB_GT\n"); break;
    case SB_GE: printf("SB_GE\n"); break;
    case SB_PLUS: printf("SB_PLUS\n"); break;
    case SB_MINUS: printf("SB_MINUS\n"); break;
    case SB_TIMES: printf("SB_TIMES\n"); break;
    case SB_SLASH: printf("SB_SLASH\n"); break;
    case SB_LPAR: printf("SB_LPAR\n"); break;
    case SB_RPAR: printf("SB_RPAR\n"); break;
    case SB_LSEL: printf("SB_LSEL\n"); break;
    case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



