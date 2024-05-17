#include <iostream>
#include <fstream>

#include "Script.hpp"

int windowWidth = 800;
int windowHeight = 600;
int windowPositionX = 0;
int windowPositionY = 0;

std::string windowTitle = "Visual Script";
Color windowColor = {0, 0, 45, 255};

bool scriptFail = false;
Lexer lexer;


enum ObjectType
{
    NIL,
    RECT,
    CIRCLE,
    LABEL
};


struct Object
{

    std::string name;
    int x;
    int y;
    Color color;
    ObjectType type;
    virtual ~Object() = default;

};

struct RectObject : public Object
{
    int width;
    int height;
};

struct CircleObject : public Object
{
    float radius;
} ;

struct LabelObject : public Object
{
    std::string text;
} ;


std::vector<Object*> objects;


void AddCircle(int x, int y, int radius, Color color)
{

    CircleObject * circle = new CircleObject();
    circle->name = "Circle";
    circle->x = x;
    circle->y = y;
    circle->radius = radius;
    circle->color = color;
    circle->type = ObjectType::CIRCLE;
    objects.push_back(circle);
}

void AddRect(int x, int y, int width, int height, Color color)
{

    RectObject * rect = new RectObject();
    rect->name = "Rect";
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->color = color;
    rect->type = ObjectType::RECT;
    objects.push_back(rect);
}

void AddLabel(int x, int y, std::string text, Color color)
{

    LabelObject * label = new LabelObject();
    label->name = "Label";
    label->x = x;
    label->y = y;
    label->text = text;
    label->color = color;
    label->type = ObjectType::LABEL;
    objects.push_back(label);
}

void ClearObjects()
{
    if (objects.size() == 0)
    {
        return;
    }
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        delete objects[i];
    }
    objects.clear();
}

void RenderObjects()
{

  
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        switch (objects[i]->type)
        {
            case ObjectType::RECT:
            {
                RectObject * rect = (RectObject*)objects[i];
                DrawRectangle(rect->x, rect->y, rect->width, rect->height, rect->color);
                break;
            }
            case ObjectType::CIRCLE:
            {
                CircleObject * circle = (CircleObject*)objects[i];
                DrawCircle(circle->x, circle->y, circle->radius, circle->color);
                break;
            }
            case ObjectType::LABEL:
            {
                LabelObject * label = (LabelObject*)objects[i];
                DrawText(label->text.c_str(), label->x, label->y, 20, label->color);
                break;
            }
            case ObjectType::NIL: 
                break;
        }
    }
}



void LoadScript()
{
char * data = LoadFileText("script.c");
if (data == nullptr)
{
    std::cout << "Error Loading File" << std::endl;
    scriptFail = true;
    return ;
}

Scanner scanner;


std::string fileData = data;

free(data);


if (scanner.Load(fileData))
{
    if (lexer.Parse(scanner.getTokens()))
    {
            try 
            {
                ClearObjects();
                Block * window = lexer.GetBlock("window");
                if (window != nullptr)
                {
                    windowTitle = window->getVariable<std::string>("title");
                    windowWidth = window->getVariable<int>("width");
                    windowHeight = window->getVariable<int>("height");
                    windowPositionX = window->getVariable<int>("x");
                    windowPositionY = window->getVariable<int>("y");
                    windowColor = window->getVariable<Color>("backColor");
                    SetWindowPosition(windowPositionX, windowPositionY);
                    SetWindowSize(windowWidth, windowHeight);
                    SetWindowTitle(windowTitle.c_str());

                }
                {
                    std::vector<Block*> blocks = lexer.GetBlocks("Rectangle");
                    for (unsigned int i = 0; i < blocks.size(); i++)
                    {
                        Block * block = blocks[i];
                        int x = block->getVariable<int>("x");
                        int y = block->getVariable<int>("y");
                        int width = block->getVariable<int>("width");
                        int height = block->getVariable<int>("height");
                        Color color = block->getVariable<Color>("tint");
                        AddRect(x, y, width, height, color);
                        
                    }
                }
                {
                    std::vector<Block*> blocks = lexer.GetBlocks("Circle");
                    for (unsigned int i = 0; i < blocks.size(); i++)
                    {
                        Block * block = blocks[i];
                       int x = block->getVariable<int>("x");
                        int y = block->getVariable<int>("y");
                        float radius = block->getVariable<float>("radius");
                        Color color = block->getVariable<Color>("tint");
                        AddCircle(x, y, radius, color);
                        
                    }
                }

                {
                    std::vector<Block*> blocks = lexer.GetBlocks("Label");
                    for (unsigned int i = 0; i < blocks.size(); i++)
                    {
                        Block * block = blocks[i];
                        int x = block->getVariable<int>("x");
                        int y = block->getVariable<int>("y");
                        std::string text = block->getVariable<std::string>("text");
                        Color color = block->getVariable<Color>("tint");
                        AddLabel(x, y, text, color);
                        
                    }
                }

            
            
                 scriptFail = false;
            } catch (const std::exception& e) 
            {
                std::cerr << e.what() << std::endl;
                 scriptFail = true;
            }
    } else 
    {
        std::cout << "Error in Lexer" << std::endl;
         scriptFail = true;
    }
} else 
{
    std::cout << "Error in scanner" << std::endl;
     scriptFail = true;
}
    
}


int main()
{



long fileTime = GetFileModTime("script.c");
long lastTime = fileTime; 


std::cout<<"File Time: "<<fileTime<<std::endl;



  InitWindow( windowWidth, windowHeight, windowTitle.c_str());


LoadScript();
if (scriptFail)
{
    CloseWindow();
    std::cout << "Error Loading Script" << std::endl;
    return 0;
}


    float timeAccumulator = 0.0f;



    while (!WindowShouldClose())       
    {

        timeAccumulator += GetFrameTime();

        if (timeAccumulator > 2.0f)
        {
            timeAccumulator = 0.0f;
            long fileTime = GetFileModTime("script.c"); 
             if (fileTime != lastTime)
             {
                LoadScript();
                lastTime = fileTime;
                std::cout << "File reloaded ." << std::endl;
             }
        }




     BeginDrawing();

     if (scriptFail)
     {

        ClearBackground(BLACK);

        DrawText("Script Failed", 10, 10, 20, RED);

     } else 
     {
         ClearBackground(windowColor);
         RenderObjects();


     }

       


   

           

        EndDrawing();

    }

    ClearObjects();

    CloseWindow();

 


return 0;

}