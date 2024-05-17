🎮⚙️ Custom Scripting Language 🚀💻



Key highlights of this custom scripting language include:

✨ Dynamic property definition and modification during runtime, enabling real-time adjustments without the burden of tedious recompilations.

✨ Support for various data types, including integers, floats, booleans, strings, as well as custom types like vectors and colors.

✨ The flexibility to define and apply object properties dynamically, allowing developers to make real-time adjustments seamlessly.


```cpp
int main()
{
    std::string text = R"(begin (player)

int     age=123
float   altura=23.0
string  name="luis"

vec3    position=(2.0,3.4,0.0)
vec2    uv=(1.0,0.0)
color  rgba=(255,255,255,255)

end


//todos os robots
begin (robot)

int     age=123
float   altura=23.0
string  name="robot"
bool active=true
vec3    position=(2.0,3.4,0.0)
vec2    uv=(1.0,0.0)
color  rgba=(255,255,255,255)

clip   bound=(0.0,0.0,1.0,1.0)

rect size =(0,0,45,45)


end

)";

Scanner scanner;

if (scanner.Load(text))
{
    // for (auto token : scanner.tokens)
    // {
    //     std::cout << token.lexeme<< " " << std::endl;
    // }   
    Lexer lexer;
    if (lexer.Parse(scanner.getTokens()))
    {

        Block * robot = lexer.GetBlock("robot");

        if (robot)  
        {
             try 
             {
                std::cout << "Name  "<< robot->getVariable<int>("name") << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << e.what() << std::endl;
                }
                        
            
               
            
        }

      
    } else 
    {
        std::cout << "Error in Lexer" << std::endl;
    }
} else 
{
    std::cout << "Error in scanner" << std::endl;
}
}
```
