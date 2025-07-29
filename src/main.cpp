#include <iostream>
#include <raylib.h>
#include<deque>
#include<raymath.h>
using namespace std;

// Define colors
Color green = {173, 204, 96, 255};
Color darkgreen = {43, 51, 24, 255};
int offset=75;
double lastupdatetime = 0;
bool ElementInDeque(Vector2 element,deque<Vector2> deque)
{
    for (unsigned int i=0;i<deque.size();i++)
    {
        if(Vector2Equals(deque[i],element)){
            return true;
        }
    }
    return false;
}
bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastupdatetime >= interval){
        lastupdatetime = currentTime;
        return true;
    }
    return false;
}
// Define cell size and count
int cellSize = 16; // Smaller cell size to reduce food size
int cellCount = 30; // Increase cell count to make the screen bigger
//snake class definition
class Snake{
    public:
    deque<Vector2> body= {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
    Vector2 direction= {1,0};
    bool addsegment = false;
    void Draw(){
        for(unsigned int i=0; i<body.size();i++) {
             float x= body[i].x;
             float y= body[i].y;
            Rectangle segment = Rectangle{offset+x*cellSize,offset+y*cellSize,(float)cellSize,(float)cellSize} ;
            DrawRectangleRounded(segment,0.5,6,darkgreen);
        }
    }
    void update(){

        body.push_front(Vector2Add(body[0],direction));
        
        if(addsegment == true){
             body.push_front(Vector2Add(body[0],direction));
             addsegment = false;
        }
        else{
             body.pop_back();
        
       
    }
    }
    void reset(){
        body =  {Vector2{6,9},Vector2{5,9},Vector2{4,9}};
        direction = {1,0};
    }

};

// Food class definition
class Food {
public:
    Vector2 position; // Position in terms of grid cells
    Texture2D texture;
    int foodSize = cellSize / 2; // Smaller food size

    // Constructor
    Food(deque<Vector2> snakebody) {
        Image image = LoadImage("graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakebody);
    }

    // Destructor
    ~Food() {
        UnloadTexture(texture); // Properly unload the texture
    }

    // Draw method with a smaller rectangle
    void Draw() {
        // Calculate position to center the smaller food square in the cell
        float drawX = offset + position.x * cellSize + (cellSize - foodSize) / 2;
        float drawY = offset + position.y * cellSize + (cellSize - foodSize) / 2;
        DrawRectangle(drawX, drawY, foodSize, foodSize, BLACK);
    }

   Vector2 GenerateRandomCell() {
    // Ensure the random position is within the boundaries of the rectangle
    float x = GetRandomValue(0, cellCount - 1);
    float y = GetRandomValue(0, cellCount - 1);
    return Vector2{x, y};
}


    Vector2 GenerateRandomPos(deque<Vector2> snakebody) {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakebody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
};

class Game{
public:
Snake snake = Snake();
Food food = Food(snake.body);
bool running = true;
Sound eatsound;
Sound wallsound;
int score = 0;
Game(){
    InitAudioDevice();
    eatsound = LoadSound("Sounds/eat.mp3");
    wallsound = LoadSound("Sounds/wall.mp3");
}
~Game(){
    UnloadSound(eatsound);
    UnloadSound(wallsound);
    CloseAudioDevice();
}
void Draw(){
    food.Draw();
    snake.Draw();
}
void update(){
    if(running){
    snake.update();
    CheckCollisionWithFood();
    CheckCollisionWithEdges();
    CheckCollisionWithTail();
    }

}
void CheckCollisionWithFood(){
    if(Vector2Equals(snake.body[0],food.position)){
     food.position = food.GenerateRandomPos(snake.body);
     snake.addsegment= true;
     score++;
     PlaySound(eatsound);
    }
}
    void CheckCollisionWithEdges()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1){
                  GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1){
                  GameOver();
        }
    }
    void GameOver(){
snake.reset();
food.position = food.GenerateRandomPos(snake.body);
running = false;
score = 0;
PlaySound(wallsound);
    }
    void CheckCollisionWithTail(){
        deque<Vector2> headlessbody = snake.body;
        headlessbody.pop_front();
        if(ElementInDeque(snake.body[0],headlessbody))
        {
            GameOver();
        }
    }
};
// Main function
// Main function
int main() {
    // Initialize the window
    InitWindow(2*offset+cellSize * cellCount, 2*offset+cellSize * cellCount, "retro snake");
    SetTargetFPS(60);

    Game game = Game();

    // Main game loop
    while (!WindowShouldClose()) {
        BeginDrawing();

        if (eventTriggered(0.2)) {
            game.update();
        }

        // Capture current direction to avoid immediate reversal
        Vector2 currentDirection = game.snake.direction;

        // Handle direction change
        if (IsKeyPressed(KEY_UP) && currentDirection.y != 1) {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && currentDirection.y != -1) {
            game.snake.direction = {0, 1};
            game.running = true;

        }
        if (IsKeyPressed(KEY_LEFT) && currentDirection.x != 1) {
            game.snake.direction = {-1, 0};
            game.running = true;

        }
        if (IsKeyPressed(KEY_RIGHT) && currentDirection.x != -1) {
            game.snake.direction = {1, 0};
            game.running = true;

        }

        // Clear the background
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5 ,(float)offset-5,(float) cellCount*cellSize+10,(float)cellCount*cellSize+10}
        , 5, darkgreen);

        // Draw the game elements
        game.Draw();
        DrawText("Retro Snake",offset-5, 20, 40, darkgreen);
        DrawText(TextFormat("%i",game.score),offset-5,offset+cellSize*cellCount+10,40,darkgreen);

        EndDrawing();
    }

    // Close the window
    CloseWindow();

    return 0;
}
