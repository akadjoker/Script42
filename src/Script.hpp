#pragma once



#include <unordered_map>
#include <string>
#include <vector>
#include <stdexcept>
#include <raylib.h>
#include <raymath.h>


enum TokenType
{

    LEFT_PAREN, //(0
    RIGHT_PAREN,//)1

    COMMA,//,2
    EQUAL,//=4
 

    // Literals.
    IDENTIFIER,//5
    STRING,//6
    REAL,//7
    INTEGER,//8

    


    // Keywords.
    BEGIN,
    END,
    TRUE,
    FALSE,
    KEYWORD_BOOL,
    KEYWORD_COLOR,
    KEYWORD_FLOAT,
    KEYWORD_INT,
    KEYWORD_STRING,
    KEYWORD_VEC2,
    KEYWORD_VEC3,
    KEYWORD_CLIP,
    KEYWORD_RECT,
    




    
    TEOF,
    NONE
};






struct Clip
{
    int x, y, w, h;
};

class BaseValue 
{
public:
    virtual ~BaseValue() = default;
    virtual const std::type_info& getType() const = 0;
};

template <typename T>
class Variable : public BaseValue 
{
private:
    T data;

public:
    Variable(const T& data) : data(data) {}
    const T& getData() const { return data; }
    void setData(const T& data) { this->data = data; }
    const std::type_info& getType() const override { return typeid(T); }
};

class Block
{
    public:
        Block(const std::string &name) : name(name) {}

        ~Block();


         template <typename T>
         void addVariable(const std::string& name, const T& value)
         {
            auto it = variables.find(name);
            if (it != variables.end()) 
            {
                delete it->second;
            }
          variables[name] = new Variable<T>(value);
        }

        template <typename T>
        T getVariable(const std::string& name) const 
        {
            auto it = variables.find(name);
            if (it != variables.end() && it->second->getType() == typeid(T)) 
            {
                return static_cast<Variable<T>*>(it->second)->getData();
            }
            throw std::runtime_error("Value not found or incorrect type for key: " + name);
        }

        void Clear();



        
        const std::string &GetName() const { return name; }

    private:
               
        std::unordered_map<std::string, BaseValue*> variables;
        std::string name;

};

struct Token
{


    TokenType type;
    std::string lexeme;
    int position;
    int line;

    Token() = default;

    Token(TokenType type, const std::string &lexeme, int position, int line)
        : type(type), lexeme(lexeme), position(position), line(line)
    {
    }

    Token(TokenType type, const std::string &lexeme)
        : type(type), lexeme(lexeme), position(0), line(0)
    {
    }
};



class Scanner 
{
    public:
        Scanner() ;
        ~Scanner() ;


 
    bool Load(const std::string &data);

    const std::vector<Token> &getTokens() const;
        

    


    private:
    int start{0};
    int current{0};
    int line;
    int pos{0};
    int length;
    bool panic;
    std::string source;
    std::unordered_map<std::string, TokenType> keywords;
    std::vector<Token> tokens;

 

    void addToken(TokenType type);

    void add(const Token &t);
    void skipWhitespace();
    char advance();
    bool match(char expected);
    char peek();
    char peekNext();
    bool isEOF();
    void scanToken();
    void number();
    void string();
    void identifier();
    void addString();
    void addNumber(bool real);
    std::vector<Token> scanTokens();
    

};




class Lexer 
{
    public:
        Lexer() ;
        ~Lexer() ;

    bool Parse(const std::vector<Token> &tokens);

    Block* GetBlock(const std::string &name);

    std::vector<Block*> GetBlocks( const std::string &name) ;

    Block *AddBlock(const std::string &name);

    void Clear();

    private:
        std::vector <Block*> blocks;
        std::vector<Token> tokens;

        bool check(TokenType type);
        bool match(TokenType t);



    Token previous();

    Token consume(TokenType type, const std::string &message);
    void  error(Token &token, const std::string &message);


    Token peek();
    Token peekNext(bool doAdvance=false);
    bool  isAtEnd();
    Token advance();
    int position;
    int line;
    int pos;
    int size;
    bool panic;
    Token current;
    Block *currentBlock;




};

