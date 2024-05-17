#include "Script.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#define MAX_PATH 256


 bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}
 bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}
 bool isAlphaNumeric(char c)
{
    return isAlpha(c) || isDigit(c);
}




std::string toString(TokenType type)
{
    switch (type)
    {
        case TokenType::BEGIN:
            return "BEGIN";
        case TokenType::END:
            return "END";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER";
        case TokenType::STRING:
            return "STRING";
        case TokenType::REAL:   
            return "REAL";
        case TokenType::INTEGER:
            return "INTEGER";
        case TokenType::LEFT_PAREN:
            return "LPARENT";
        case TokenType::RIGHT_PAREN:
            return "RPARENT";
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::EQUAL:
            return "EQUAL";
        case TokenType::KEYWORD_BOOL:
            return "KEYWORD";
        case TokenType::KEYWORD_INT:
            return "KEYWORD";
        case TokenType::KEYWORD_FLOAT:
            return "KEYWORD";
        case TokenType::KEYWORD_STRING:
            return "KEYWORD";
        case TokenType::KEYWORD_VEC2:
             return "KEYWORD"; 
        case TokenType::KEYWORD_VEC3:
            return "KEYWORD";
        case TokenType::KEYWORD_RECT:
            return "KEYWORD";
        case TokenType::KEYWORD_CLIP:
            return "KEYWORD";
        case TokenType::KEYWORD_COLOR:
            return "KEYWORD";
        case TokenType::TRUE:
            return "TRUE";
        case TokenType::FALSE:
            return "FALSE";
        case TokenType::TEOF:
            return "TEOF";
        case TokenType::NONE:
            return "NONE";
            
            }
            return "UNKNOWN";
}



Scanner::Scanner()
{
    length = 0;
    line = 1;
    pos = 0;

    keywords["bool"] = TokenType::KEYWORD_BOOL;
    keywords["int"] = TokenType::KEYWORD_INT;
    keywords["float"] = TokenType::KEYWORD_FLOAT;
    keywords["string"] = TokenType::KEYWORD_STRING;
    keywords["vec2"] = TokenType::KEYWORD_VEC2;
    keywords["vec3"] = TokenType::KEYWORD_VEC3;
    keywords["color"] = TokenType::KEYWORD_COLOR;
    keywords["rect"] = TokenType::KEYWORD_RECT;
    keywords["clip"] = TokenType::KEYWORD_CLIP;


    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;


    keywords["begin"] = TokenType::BEGIN;
    keywords["end"] = TokenType::END;
    

}

Scanner::~Scanner()
{
}

bool Scanner::Load(const std::string &data)
{
    tokens.clear();
    source = data;
    line =1;
    length = (int)source.size();
    current = 0;
    pos = 0;
    panic=false;
    scanTokens();
    return (tokens.size() > 1 && !panic);
}

const std::vector<Token> &Scanner::getTokens() const
{
    return tokens;
}

void Scanner::add(const Token &t)
{
    tokens.push_back(t);
}

void Scanner::addToken(TokenType type)
{
    Token token = Token(type, source.substr(start, current - start), start, line);
    add(token);
}


char Scanner::advance()
{
    pos++;
    return source[current++];
}
bool Scanner::match(char expected)
{
    if (isEOF())
        return false;
    if (source[current] != expected)
        return false;
    current++;
    return true;
}
char Scanner::peek()
{
    if (isEOF())
        return '\0';
    return source[current];
}
char Scanner::peekNext()
{
    if (current + 1 >= length)
        return '\0';
    return source[current + 1];
}

bool Scanner::isEOF()
{
    return current >= length;
}

std::vector<Token> Scanner::scanTokens()
{
    pos = 0;
    panic=false;
    while (!isEOF() && !panic)
    {
        start = current;
        scanToken();
        if (panic) break;
    }
    Token token = Token(TokenType::TEOF, "EOF", current, line);
    add(token);
    return tokens;
}

void Scanner::scanToken()
{
     char c = advance();
        switch (c)
        {
        case '(':
            addToken(TokenType::LEFT_PAREN);
            break;
        case ')':
            addToken(TokenType::RIGHT_PAREN);
            break;
        case ',':
             addToken(TokenType::COMMA);
            break;
         case '=':
            addToken(TokenType::EQUAL);
            break;
        case '/':
            if (match('/'))
            {
                while (peek() != '\n' && !isEOF())
                    advance();
            } else 
            {
                panic=true;
            }
        
            break;
        case ' ':
        case '\r':
        case '\t':
        {

            break;
        }
        case '\n':
        {
            line++;
            pos = 0;
        }
        break;
        case '"':
            string();
            break;
        default:
            if (isDigit(c))
            {
                number();
            }
            else if (isAlpha(c))
            {
                identifier();
            }
            else
            {
                std::cout << "Unexpected character." << std::endl;
                panic=true;
            }

            break;
        }

}

void Scanner::skipWhitespace()
{
    for (;;)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
        {

            advance();
            break;
        }
        case '\n':
        {
            line++;
            advance();
            pos = 0;
            break;
        }
        case '/':
            if (peekNext() == '/')
            {
                while (peek() != '\n' && !isEOF())
                    advance();
            }
            else
            {
                std::cout << "Unexpected character." << std::endl;  
                panic=true;
                return;
            }
            break;
        default:
            return;
        }
    }
}




void Scanner::addString()
{
    Token token = Token(TokenType::STRING, source.substr(start+1, (current - start)-2), start, line);
    add(token);
    
}



void Scanner::string()
{
    while (peek() != '"' && !isEOF())
    {
        if (peek() == '\n')
        {
            line++;
            pos = 0;
        }
        advance();
    }
    if (isEOF())
    {
        std::cout << "Error   at:" << pos << "  Line: " << line << " Unclose String" << std::endl;
         panic=true;
    }
    advance();
    addString();
}
void Scanner::number()
{
    bool isFloat = false;
    while (isDigit(peek()))
        advance();

    if (peek() == '.' && isDigit(peekNext()))
    {
        isFloat = true;
        advance();
        while (isDigit(peek()))
            advance();
    }
    addToken(isFloat ? TokenType::REAL : TokenType::INTEGER);
}
void Scanner::identifier()
{
    while (isAlpha(peek()) || isDigit(peek()))
        advance();
    std::string text = source.substr(start, current - start);
    if (keywords.find(text) != keywords.end())
    {
        addToken(keywords[text]);
    }
    else
    {
        addToken(TokenType::IDENTIFIER);
    }
}

//******************************************************************************************
//LEXER
//******************************************************************************************

Lexer::Lexer()
{

}
Lexer::~Lexer()
{
    Clear();
}

bool Lexer::check(TokenType type)
{
    if (isAtEnd())
        return false;
    TokenType is = peek().type;
    return is == type;
}
bool Lexer::match(TokenType t)
{
   
    if (check(t))
    {
        advance();
        return true;
    }
    
    return false;
}


Token Lexer::advance()
{
    if (!isAtEnd())
        position++;
    return previous();
}

bool Lexer::isAtEnd()
{
    return peek().type == TokenType::TEOF;
}

Token Lexer::peek()
{
    return tokens[position];
}

Token Lexer::peekNext(bool doAdvance)
{
    if (position + 1 >= size)
        return Token(TokenType::TEOF, "", 0, 0);
    Token t = tokens[position + 1];

    if (doAdvance)
        position++;

    return t;
}

Token Lexer::previous()
{
    return tokens[position - 1];
}

Token Lexer::consume(TokenType type, const std::string &message)
{
    if (check(type))
        return advance();
    std::cout<<" Line " << (line-1) <<" "<< message<<  " " << std::endl;
    panic=true;
    return Token(TokenType::NONE, "", position, line);
}
void Lexer::error(Token &token, const std::string &message)
{
    if (token.type == TokenType::TEOF)
    {
        std::cout << "[Line " << token.line << "] Error at end: " << message << std::endl;
    }
    else
    {
        std::cout << "[Line " << token.line << "] Error at '" << token.lexeme << "': " << message << std::endl;
    }
    panic=true;
}


bool Lexer::Parse(const std::vector<Token> &tokens)
{
    position = 0;
    line = 1;

    Clear();
    this->tokens = tokens;
    size = (int)tokens.size();
    currentBlock = nullptr;
    pos=0;
    panic = false;

    while (!isAtEnd() && !panic)
    {
        current = peek();
        line = current.line;
        pos = current.position;

        if (current.type == TokenType::BEGIN)
        {
            advance();//consume BEGIN
            consume(TokenType::LEFT_PAREN, "Expect '(' after 'BEGIN'.");
            Token token = consume(TokenType::IDENTIFIER, "Expect identifier after 'BEGIN'.");
            consume(TokenType::RIGHT_PAREN, "Expect ')' to close block .");
            if (panic) break;
            currentBlock = AddBlock(token.lexeme);
            continue;

        } else 
        if (current.type == TokenType::END)
        {
           
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }
           // std::cout<<"End Block: "<<currentBlock->GetName()<<std::endl;
            advance();//consume END
             if (panic) break;
            continue;
           
        }if (current.type == TokenType::KEYWORD_INT)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }
     
            advance();//consume IDENTIFIER
            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after int.");
            consume(TokenType::EQUAL, "Expect '=' after variable name.");
            Token value = consume(TokenType::INTEGER, "Expect integer value.");

            if (panic) break;

         //   std::cout<<"Int "<< name.lexeme<<" " << value.lexeme<<std::endl;

            currentBlock->addVariable(name.lexeme, std::stoi(value.lexeme));

            continue;


        } if (current.type == TokenType::KEYWORD_FLOAT)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }
     
            advance();//consume IDENTIFIER
            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after float.");
            consume(TokenType::EQUAL, "Expect '=' after variable name.");
            Token value = consume(TokenType::REAL, "Expect float value.");

             if (panic) break;

          //  std::cout<<"Float "<< name.lexeme<<" " << value.lexeme<<std::endl;

            currentBlock->addVariable(name.lexeme, std::stof(value.lexeme));

            continue;
        } else if (current.type == TokenType::KEYWORD_STRING)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }
     
            advance();//consume IDENTIFIER
            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after string.");
            consume(TokenType::EQUAL, "Expect '=' after variable name.");
            Token value = consume(TokenType::STRING, "Expect string value.");
             if (panic) break;

           // std::cout<<"String "<< name.lexeme<<" " << value.lexeme<<std::endl;

            currentBlock->addVariable(name.lexeme, value.lexeme);

            continue;
        } else if (current.type == TokenType::KEYWORD_BOOL)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }
     
            advance();//consume IDENTIFIER
           
            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after bool.");
            consume(TokenType::EQUAL, "Expect '=' after variable name.");

            Token value =  peek(); 

            advance();

             if (panic) break;

         //  std::cout<<"Bool "<< name.lexeme<<" " << value.lexeme<<std::endl;

            bool b = value.type == TokenType::TRUE ? true : false;

            currentBlock->addVariable(name.lexeme, b);

            continue;
        } else if (current.type == TokenType::KEYWORD_VEC3)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }

            advance();//consume IDENTIFIER

            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after vec3.");
            consume(TokenType::EQUAL, "Expect '=' after variable name.");
            consume(TokenType::LEFT_PAREN, "Expect '(' after variable name.");
            Token x = consume(TokenType::REAL, "Expect float value.");
            consume(TokenType::COMMA, "Expect ',' after float value.");
            Token y = consume(TokenType::REAL, "Expect float value.");
            consume(TokenType::COMMA, "Expect ',' after float value.");
            Token z = consume(TokenType::REAL, "Expect float value.");
            consume(TokenType::RIGHT_PAREN, "Expect ')' after float value.");
             if (panic) break;

             Vector3 v;
             v.x=std::stof(x.lexeme);
             v.y=std::stof(y.lexeme);
             v.z=std::stof(z.lexeme);

             currentBlock->addVariable(name.lexeme, v);
          //   std::cout<<"Vec3 "<< name.lexeme<<" " << v.x<<" "<<v.y<<" "<<v.z<<std::endl;
            
            continue;

        } else if (current.type == TokenType::KEYWORD_VEC2)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }

            advance();//consume IDENTIFIER

            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after vec2.");
            consume(TokenType::EQUAL, "Expect '=' after variable name.");
            consume(TokenType::LEFT_PAREN, "Expect '(' after variable name.");

            Token x = consume(TokenType::REAL, "Expect float value.");
            consume(TokenType::COMMA, "Expect ',' after float value.");
            Token y = consume(TokenType::REAL, "Expect float value.");

            consume(TokenType::RIGHT_PAREN, "Expect ')' after float value.");
             if (panic) break;

             Vector2 v;
             v.x=std::stof(x.lexeme);
             v.y=std::stof(y.lexeme);

             currentBlock->addVariable(name.lexeme, v);   
           // std::cout<<"Vec2 "<< name.lexeme<<" " << v.x<<" "<<v.y<<std::endl;
            continue;
        }
        else if (current.type == TokenType::KEYWORD_RECT)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }

            advance();//consume IDENTIFIER

            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after rectangle.");

            consume(TokenType::EQUAL, "Expect '=' after variable name.");

            consume(TokenType::LEFT_PAREN, "Expect '(' after variable name.");

            Token x = consume(TokenType::REAL, "Expect float value.");

            consume(TokenType::COMMA, "Expect ',' after float value.");

            Token y = consume(TokenType::REAL, "Expect float value.");

            consume(TokenType::COMMA, "Expect ',' after float value.");

            Token w = consume(TokenType::REAL, "Expect float value.");

            consume(TokenType::COMMA, "Expect ',' after float value.");

            Token h = consume(TokenType::REAL, "Expect float value.");

            consume(TokenType::RIGHT_PAREN, "Expect ')' after float value.");

            if (panic) break;

            Rectangle clip;

            clip.x=std::stof(x.lexeme);
            clip.y=std::stof(y.lexeme);
            clip.width=std::stof(w.lexeme);
            clip.height=std::stof(h.lexeme);

            currentBlock->addVariable(name.lexeme, clip);

          //  std::cout<<"Clip "<< name.lexeme<<" " << x.lexeme<<" "<<y.lexeme<<" "<<w.lexeme<<" "<<h.lexeme<<std::endl;
            continue;
        }  else if (current.type == TokenType::KEYWORD_CLIP)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }

            advance();//consume IDENTIFIER

            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after rect.");

            consume(TokenType::EQUAL, "Expect '=' after variable name.");

            consume(TokenType::LEFT_PAREN, "Expect '(' after variable name.");

            Token x = consume(TokenType::INTEGER, "Expect int value.");

            consume(TokenType::COMMA, "Expect ',' after int value.");

            Token y = consume(TokenType::INTEGER, "Expect int value.");

            consume(TokenType::COMMA, "Expect ',' after int value.");

            Token w = consume(TokenType::INTEGER, "Expect int value.");

            consume(TokenType::COMMA, "INTEGER ',' after int value.");

            Token h = consume(TokenType::INTEGER, "Expect int value.");

            consume(TokenType::RIGHT_PAREN, "Expect ')' after int value.");

            if (panic) break;

            Clip rect;

            rect.x=std::stoi(x.lexeme);
            rect.y=std::stoi(y.lexeme);
            rect.w=std::stoi(w.lexeme);
            rect.h=std::stoi(h.lexeme);

            currentBlock->addVariable(name.lexeme, rect);

         //   std::cout<<"Rect "<< name.lexeme<<" " << x.lexeme<<" "<<y.lexeme<<" "<<w.lexeme<<" "<<h.lexeme<<std::endl;
            continue;

        }  else if (current.type == TokenType::KEYWORD_COLOR)
        {
            if (currentBlock == nullptr)
            {
                std::cout << "Error   at:" << pos << "  Line: " << line << " Missing BEGIN" << std::endl;
                return false;
            }

            advance();//consume IDENTIFIER

            Token name = consume(TokenType::IDENTIFIER, "Expect 'name' after color.");

            consume(TokenType::EQUAL, "Expect '=' after variable name.");

            consume(TokenType::LEFT_PAREN, "Expect '(' after variable name.");

            Token r = consume(TokenType::INTEGER, "Color expect int value.");

            consume(TokenType::COMMA, "Expect ',' after int value.");

            Token g = consume(TokenType::INTEGER, "Expect int value.");

            consume(TokenType::COMMA, "Expect ',' after int value.");

            Token b = consume(TokenType::INTEGER, "Expect int value.");

            consume(TokenType::COMMA, "Expect ',' after int value.");

            Token a = consume(TokenType::INTEGER, "Expect int value.");

            consume(TokenType::RIGHT_PAREN, "Expect ')' after int value.");

            if (panic) break;

            Color color;

            color.r=std::stoi(r.lexeme);
            color.g=std::stoi(g.lexeme);
            color.b=std::stoi(b.lexeme);
            color.a=std::stoi(a.lexeme);

            currentBlock->addVariable(name.lexeme, color);

          //  std::cout<<"Color "<< name.lexeme<<" " << r.lexeme<<" "<<g.lexeme<<" "<<b.lexeme<<" "<<a.lexeme<<std::endl;
            continue;

        }



        else 
        {
            std::cout<<"UNKNOW Current "<< current.lexeme<<" " << toString(current.type)<<std::endl;
            panic = true;
            return false;
        }
     

         advance();
       
    }

    return  (!panic);
}

Block *Lexer::GetBlock(const std::string &name)
{
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        if (blocks[i]->GetName() == name)
        {
            return blocks[i];
        }
    }
    return nullptr;
    
}

std::vector<Block *> Lexer::GetBlocks(const std::string &name)
{
    std::vector<Block *> b;
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        if (blocks[i]->GetName() == name)
        {
            b.push_back(blocks[i]);
        }
    }
    return b;
    
}

Block *Lexer::AddBlock(const std::string &name)
{
    Block *block = new Block(name);
    blocks.push_back(block);
   std::cout<<"Added Block: "<<name<<std::endl;
    return block;
}

void Lexer::Clear()
{
  //  std::cout<<"Clearing"<<std::endl;
    for (unsigned int i = 0; i < blocks.size(); i++)
    {

   //     std::cout<<"  Deleting "<<blocks[i]->GetName()<<" Block"<<std::endl;
        delete blocks[i];
    }
    blocks.clear();
    tokens.clear();
}

Block::~Block()
{
  
    for (auto it = variables.begin(); it != variables.end(); it++)
    {
       // std::cout<<"    Deleting "<<it->first<<" Variable"<<std::endl;
        delete it->second;
    }
    variables.clear();
}

void Block::Clear()
{

    for (auto it = variables.begin(); it != variables.end(); it++)
    {
       // std::cout<<"    Deleting "<<it->first<<" Variable"<<std::endl;
        delete it->second;
    }
    variables.clear();
}

