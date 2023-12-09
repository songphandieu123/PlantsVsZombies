#include <raylib.h>
#include <vector>
#include <iostream>

#define Dead -1
#define OutsideLawn -3000

class Buttons{
    private:
    int X;
    int Y;
    int width = 500;
    int height = 100;

    public:   
    int textCount = 0;
    char text[10] = {};

    int GetWidth(){
        return width;
    }

    int GetHeight(){
        return height;
    }
    int GetX(){
        return X;
    }

    int GetY(){
        return Y;
    }

    Buttons(int X, int Y, char text[10], int textCount, int width = 500, int height = 80){
        this->X = X;
        this->Y = Y;
        this->textCount = textCount;
        for(int i = 0; i < textCount; i++){
            this->text[i] += text[i];
        }
        this->width = width;
        this->height = height;
    }

    void Draw(){
        DrawRectangleRoundedLines(Rectangle{(float)X, (float)Y, (float)width, (float)height}, 0.8, 1, 1, BLACK);
        DrawText(TextFormat("%s" ,text), X + width * (10 - textCount + 1) / 20.0, Y, height, RED);
    }

};

enum Ebuttons{
    E_Play = 1
};


enum Plants{
    E_Sunflower = 1, E_PeaShooter
};

enum Zombies{
    E_Basic = 1, E_ConeHead
};

struct ZombieTrack{
    int X, id;
};

namespace Tracker{
    int EncryptTracker(int codeName, int index){
        return codeName * 100 + index;
    }

    int GetTrackerCodeName(int value){
        return value / 100;
    }

    int GetTrackerIndex(int value){
        return value % 100;
    }
};

namespace Lawn{
    const int X = 400;
    const int Y = 200;
    const int side = 150;

    void DrawLawn(){
        int temporaryX = X;
        int temporaryY = Y;
        for(int column = 0; column < 8; column++){
            for(int row = 0; row < 5; row++){
                DrawRectangleLines(temporaryX, temporaryY, side, side, BROWN);
                temporaryY += side;
            }
        temporaryY = Y;
        temporaryX += side;
        }
    }

    int RowToY(int row){
        return row * side + Y;
    }

    int YToRow(int newY){
        return (newY - Y)/ side;
    }
        
    int ColumnToX(int column){
        return column * side + X;
    }

    int XToColumn(int newX){
        return (newX - X) / side;
    }
    
    bool MouseInLawn(){
        if(GetMouseX() < Lawn::X || GetMouseX() > Lawn::side * 8 + Lawn::X || GetMouseY() < Lawn::Y|| GetMouseY() > Lawn::Y + Lawn::side * 5){
            return false;
        }
        return true;
    }

};

namespace GameSetup{
    void SetZombieTrackPosition(ZombieTrack zombieTrack[5][10]){
        for(int i = 0; i < 5; i++){
            for(int j = 0; j < 10; j++){
                if(zombieTrack[i][j].id){
                    zombieTrack[i][j].X = Lawn::X + Lawn::side * 8 + 50;
                }
                else{
                    break;
                }
            }
        }
    }
        
    template <typename T>
    void SetRow(std::vector<T>& zombies, int start, int stop, int row){
        for(int i = start; i < stop ; i++){
            zombies[i].SetRow(row);
        }
    }

    template <typename T>
    void SetRowMultiple(std::vector<T>& zombies){
        int zombieOnRow = zombies.size() / 5;
        for(unsigned int i = 0; i < zombies.size(); i += zombieOnRow){
            SetRow(zombies, i, i + zombieOnRow, i / zombieOnRow);
        }
    }

    template <typename T>
    void SetZombieTrack(std::vector<T>& zombies, ZombieTrack zombieTrack[5][10], int row, int index, int codeName){
        zombies[index].SetFirstInRow(true);
        zombieTrack[row][zombies[0].GetZombieTrackColumn()].id = Tracker::EncryptTracker(codeName, index);
    }

    template <typename T>
    void SetZombieTrackMultiple(std::vector<T>& zombies, ZombieTrack zombieTrack[5][10], int codeName){
        int zombieOnRow = zombies.size() / 5;
        for(unsigned int i = 0; i < zombies.size(); i += zombieOnRow){
            SetZombieTrack(zombies, zombieTrack, i / zombieOnRow, i, codeName);
        }
    } 

    template <typename T>
    void SetZombieTrackColumn(std::vector<T>& zombies, int zombieTrackColumn){
        for(unsigned int i = 0; i < zombies.size(); i++){
            zombies[i].SetZombieTrackColumn(zombieTrackColumn);
        }
    }
};

namespace Script{
    template <typename T>
    void DeletePlant(std::vector<T> &plant){
        plant.resize(plant.size() - 2);
        plant.resize(plant.size() + 1);
        plant[plant.size() - 1].SetIndex(plant.size() - 1);
        plant.shrink_to_fit();
    }

    template <typename T>
    void ZombieMainScript(std::vector<T> &zombies, int boardTrack[5][9], bool boardTrackAltered, ZombieTrack zombieTrack[5][10]){
        static unsigned int counter = 0;
        static int zombieTrackColumn; 
        static int lastRow;
        if(!counter){
            counter++;
            zombieTrackColumn = zombies[0].GetZombieTrackColumn();
        }
        if(zombies.size() == 1){
            lastRow = zombies[0].GetRow();
        }
        for(int i = 0; i < 5 && zombies.size(); i++){
            if(i != zombies[Tracker::GetTrackerIndex(zombieTrack[i][zombieTrackColumn].id)].GetRow()){ // row with no zombies
                zombieTrack[i][zombieTrackColumn].id = 0;
                zombieTrack[i][zombieTrackColumn].X = 0;
            }
        }
        if(!zombies.size()){
            zombieTrack[lastRow][zombieTrackColumn].id = 0;
            zombieTrack[lastRow][zombieTrackColumn].X = 0;
        }
        for(unsigned int i = 0; i < zombies.size(); i++){
            zombies[i].Draw();
            if(zombies[i].GetImmobolized() && zombies[i].GetFirstInRow()){
                int X = zombies[i].GetX();
                int index = i;
                zombies[i].SetFirstInRow(false);
                // not a problem just yet but if one pops up look here first
                for(unsigned int j = 0; j < zombies.size(); j++){
                    if(zombies[i].GetRow() != zombies[j].GetRow()){
                        continue;
                    }
                    else if(X > zombies[j].GetX()){
                        X = zombies[j].GetX();
                        index = j;
                    }
                }
                zombies[index].SetFirstInRow(true);
                zombieTrack[zombies[i].GetRow()][zombieTrackColumn].id = Tracker::EncryptTracker(zombies[1].GetCodeName(), index);
            }
            if(zombies[i].GetFirstInRow()){
                zombieTrack[zombies[i].GetRow()][zombieTrackColumn].X = zombies[i].GetX();
                zombieTrack[zombies[i].GetRow()][zombieTrackColumn].id = Tracker::EncryptTracker(zombies[0].GetCodeName(), i);
            }
            if(boardTrackAltered){
                zombies[i].SetPlantToAttack(boardTrack);
            }
            if(zombies[i].GetHealth() <= 0){
                if(zombies[i + 1].GetRow() == zombies[i].GetRow()){
                    zombies[i + 1].SetFirstInRow(true); 
                }
                for(unsigned int j = i; j < zombies.size() - 1; j++){
                    zombies[j].SetX(zombies[j + 1].GetX());
                    zombies[j].SetRow(zombies[j + 1].GetRow());                
                    zombies[j].SetStopToSpace(zombies[j + 1].GetStopToSpace());
                    zombies[j].SetImmobolized(zombies[j + 1].GetImmobolized());
                    zombies[j].SetFirstInRow(zombies[j + 1].GetFirstInRow());
                    zombies[j].SetPlantColumn(zombies[j + 1].GetPlantColumn());
                    zombies[j].SetHealth(zombies[j + 1].GetHealth());  
                }
                zombies.resize(zombies.size() - 1);
                zombies.shrink_to_fit();
            }
            if(i == 0 || zombies[i].GetRow() != zombies[i - 1].GetRow()){
                continue;
            }
            else if(zombies[i].GetX() - 20 < zombies[i - 1].GetX()){
                zombies[i].SetStopToSpace(true);
            }
            else{
                zombies[i].SetStopToSpace(false);
            }
        }
    }

    template <typename T>
    void PlantMainScript(std::vector<T>& plants, int boardTrack[5][9], bool& boardTrackAltered, KeyboardKey keyToUse, int &plantDeletedIndex){
        for(unsigned int i = 0; i < plants.size(); i++){
            plants[i].Draw(boardTrack, keyToUse);
            if((IsKeyPressed(KEY_W) && Lawn::MouseInLawn())){
                for(unsigned int j = 0; j < plants.size(); j++){
                    if(plants[j].GetPlantToTerminate()){
                        plants[j].Terminate(boardTrack);
                        break;
                    }
                }
            }
            if(plants[i].GetHealth() <= 0){
                plantDeletedIndex = i;
                plants[i].Terminate(boardTrack);
                plants[i].SetDrawn(plants[plants.size() - 2].GetDrawn());
                plants[i].SetX(plants[plants.size() - 2].GetX());
                plants[i].SetY(plants[plants.size() - 2].GetY());
                plants[i].SetHealth(plants[plants.size() - 2].GetHealth());
                boardTrackAltered = true;
                break;
            }
        }
        if(plants[plants.size() - 1].GetDrawn()){
            plants.resize(plants.size() + 1);
            boardTrackAltered = true;
            plants[plants.size() - 1].SetIndex(plants.size() - 1);
        }
    }
        
    template <typename T>
    void ShooterAdditionalScript(std::vector<T> &shooters, ZombieTrack zombieTrack[5][10], int plantDeletedIndex){
        if(plantDeletedIndex != -1){
            shooters[plantDeletedIndex].SetProjectileX(shooters[shooters.size() - 2].GetProjectileX());
            shooters[plantDeletedIndex].SetTimer(shooters[shooters.size() - 2].GetTimer());
            shooters[plantDeletedIndex].SetStartTimer(shooters[shooters.size() - 2].GetStartTimer());
            shooters[plantDeletedIndex].SetProjectileMoving(shooters[shooters.size() - 2].GetProjectileMoving());
            DeletePlant(shooters);
        }
        for(unsigned int i = 0; i < shooters.size() - 1; i++){
            shooters[i].SetZombieToAttack(zombieTrack);
            shooters[i].UpdateTimer();
            if(!shooters[i].GetProjectileX()){
                shooters[i].SetProjectileX(shooters[i].GetX());
            }
            if(shooters[i].GetZombieX() > Lawn::X && shooters[i].GetZombieX() < Lawn::X + Lawn::side * 8){
                shooters[i].Shoot();
            }
        }
    }

    template <typename T>
    void SunProducerAdditionalScript(std::vector<T> &sunProducer, int plantDeletedIndex){
        if(plantDeletedIndex != -1){
            DeletePlant(sunProducer);
        }
    }
};

class Zombie{
    private:
    int X = Lawn::X + Lawn::side * 8 + 50;
    int Y;
    const int width = 10;
    const int height = Lawn::side - 20;
    const float speed = 0.5;
    int plantColumn = -2;
    bool stopToSpace = false;
    bool immobolized = false;
    bool firstInRow = false;
    int zombieTrackColumn;
    int health;
    int damage;
    int codeName;

    public:
    int GetX(){
        return X;
    }
    
    int GetRow(){
        return Lawn::YToRow(Y);
    }

    int GetPlantColumn(){
        return plantColumn;
    }

    int GetColumn(){
        return Lawn::XToColumn(X);
    }

    bool GetStopToSpace(){
        return stopToSpace;
    }

    int GetDamage(){
        return damage;
    }

    int GetCodeName(){
        return codeName;
    }

    bool GetImmobolized(){
        return immobolized;
    }

    bool GetFirstInRow(){
        return firstInRow;
    }

    int GetZombieTrackColumn(){
        return zombieTrackColumn;
    }

    int GetHealth(){
        return health;
    }

    void SetHealth(int health){
        this->health = health;
    }

    void SetX(int X){
        this->X = X;
    }

    void SetRow(int row){
        Y = Lawn::RowToY(row);
    }

    void SetPlantColumn(int plantColumn){
        this->plantColumn = plantColumn;
    }

    void SetStopToSpace(bool stopToSpace){
        this->stopToSpace = stopToSpace;
    }

    void SetImmobolized(bool immobolized){
        this->immobolized = immobolized; 
    }

    void SetFirstInRow(bool firstInRow){
        this->firstInRow = firstInRow;
    }

    void SetZombieTrackColumn(int zombieTrackColumn){
        this->zombieTrackColumn = zombieTrackColumn;
    }

    void SetPlantToAttack(int boardTrack[5][9]){
        bool columnSet = false;
        for(int column = GetColumn(); column > -1; column--){
            if(boardTrack[Lawn::YToRow(Y)][column]){
                SetPlantColumn(column);
                columnSet = true;
                break;
            }
        }
        if(!columnSet){
            SetPlantColumn(-2);
        }
    }

    void Draw(){
        if(health > 0){
            DrawRectangle(X, Y + 20, width, height, BROWN);
        }
        if(Lawn::XToColumn(X) != plantColumn && !stopToSpace && !immobolized){
            X -= speed;
        }
    }

    Zombie(int health, int damage, int codeName){
        this->health = health;
        this->damage = damage;
        this->codeName = codeName;
    }
};
class Basic : public Zombie{
    public:
    Basic() : Zombie(200, 2, 1){
    }
};

class ConeHead : public Zombie{
    public:
    ConeHead() : Zombie(500, 2, 2){
    }
};

class Plant{
    private:
    int radius = Lawn::side / 2;
    bool drawn = false;
    int Y = 0;
    int index = 0;
    int codeName;
    Color color;
    int health;

    protected:
    int X = 0;

    public:
    bool GetPlantToTerminate(){
        if(GetMouseX() > X - Lawn::side / 2.0 && GetMouseX() < X + Lawn::side / 2.0 && GetMouseY() > Y - Lawn::side / 2.0 && GetMouseY() < Y + Lawn::side / 2.0){
            return true;
        }
        return false;
    }

    void SetIndex(int index){
        this->index = index;
    }

    void SetDrawn(bool drawn){
        this->drawn = drawn;
    }

    void SetX(int X){
        this->X = X;
    }

    void SetY(int Y){
        this->Y = Y;
    }

    void SetHealth(int health){
        this->health = health;
    }

    bool GetDrawn(){
        return drawn;
    }

    int GetX(){
        return X;
    }

    int GetY(){
        return Y;
    }

    int GetIndex(){
        return index;
    }

    int GetCodeName(){
        return codeName;
    }

    int GetHealth(){
        return health;
    }

    Plant(int health, int codeName, Color color){
        this->codeName = codeName;   
        this->color = color;
        this->health = health;
    }

    void Terminate(int boardTrack[5][9]){
        boardTrack[Lawn::YToRow(Y)][Lawn::XToColumn(X)] = 0;
        drawn = false;
        health = Dead;
    }

    void Draw(int boardTrack[5][9], KeyboardKey keyToUse){
        if(((IsKeyPressed(keyToUse) && Lawn::MouseInLawn() && !boardTrack[Lawn::YToRow(GetMouseY())][Lawn::XToColumn(GetMouseX())]) || drawn) && health > 0){
            if(!drawn){
                X = Lawn::ColumnToX(Lawn::XToColumn(GetMouseX())) + Lawn::side / 2;
                Y = Lawn::RowToY(Lawn::YToRow(GetMouseY())) + Lawn::side / 2;
                drawn = true;
            }
            if(boardTrack[Lawn::YToRow(Y)][Lawn::XToColumn(X)] != Tracker::EncryptTracker(codeName, index)){
                boardTrack[Lawn::YToRow(Y)][Lawn::XToColumn(X)] = Tracker::EncryptTracker(codeName, index);
            }
            DrawCircle(X, Y, radius, color);
            DrawText(TextFormat("%i", index), X, Y, 50, RED);
        }
    }
};

class SunProducer : public Plant{
    public:
    SunProducer(int health, int codeName, Color color) : Plant(health, codeName, color){
    }
};

class Sunflower: public SunProducer{
    public:
    Sunflower() : SunProducer(200, 1, YELLOW){
    }
};

class Attacker : public Plant{
    private:
    int zombieX = 0;
    int damage;

    public:
    int GetZombieX(){
        return zombieX;
    }

    int GetDamage(){
        return damage;
    }

    void SetZombieX(int zombieX){
        this->zombieX = zombieX;
    }

    Attacker(int health, int codeName, Color color, int damage) : Plant(health, codeName, color){
        this->damage = damage;
    }

    void SetZombieToAttack(ZombieTrack zombieTrack[5][10]){
        int temporaryZombieX = zombieTrack[Lawn::YToRow(GetY())][1].X;
        int id = zombieTrack[Lawn::YToRow(GetY())][1].id;
        for(int i = 2; i < 10; i++){
            if(!zombieTrack[Lawn::YToRow(GetY())][i].X){
                break;
            }
            else if(zombieTrack[Lawn::YToRow(GetY())][i].X < temporaryZombieX){
                temporaryZombieX = zombieTrack[Lawn::YToRow(GetY())][i].X;
                id = zombieTrack[Lawn::YToRow(GetY())][i].id;
            }
        }
        zombieX = temporaryZombieX;
        zombieTrack[Lawn::YToRow(GetY())][0].X = temporaryZombieX;
        zombieTrack[Lawn::YToRow(GetY())][0].id = id;
    }
};

class Shooter : public Attacker{
    private:
    int projectileRadius;
    int speed;
    int projectileX = 0;
    Color projectileColor;
    int timer;
    int originalTimer;  
    bool startTimer = false;
    bool projectileMoving = false;

    public:
    int GetProjectileX(){
        return projectileX;
    }

    int GetTimer(){
        return timer;
    }

    int GetStartTimer(){
        return startTimer;
    }

    int GetProjectileMoving(){
        return projectileMoving;
    }
    
    void SetProjectileX(int projectileX){
        this->projectileX = projectileX;
    }

    void SetTimer(int timer){
        this->timer = timer;
    }

    void SetStartTimer(int startTimer){
        this->startTimer = startTimer;
    }

    void SetProjectileMoving(int projectileMoving){
        this->projectileMoving = projectileMoving;
    }

    bool ProjectileHit(){
        if(projectileX + projectileRadius >= GetZombieX() && projectileX - projectileRadius <= GetZombieX() + 10){
            return true;
        }
        return false;
    }

    void Shoot(){
        DrawCircle(projectileX, GetY(), projectileRadius, projectileColor);
        if(!startTimer){
            projectileX = X;
            projectileMoving = false;
        }
        if(projectileX == GetX()){
            projectileMoving = true;
            startTimer = true;
        }
        if(projectileMoving && projectileX != OutsideLawn){
            projectileX += speed;
        }
    }

    void UpdateTimer(){
        if(startTimer){
            timer--;
        }
        if(!timer){
            timer = originalTimer;
            startTimer = false;
        }
    }

    Shooter(int health, int codeName, Color color, int damage, int radius, int speed, int cooldown) : Attacker(health, codeName, color, damage){
        projectileRadius = radius ? radius : 20;
        this->speed = speed ? speed : 10;
        timer = cooldown;
        originalTimer = cooldown;
    }
};

class Peashooter : public Shooter{
    public:
    Peashooter() : Shooter(200, 2, GREEN, 0, 0, 0, 120){
    }
};

int main(){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1200, 800, "PvZ I guess");
    Buttons Back(50, 50, (char*)"Back", 4);
    Buttons Play(500, 200, (char*)"Play", 4);
    bool backPressed = false;
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(WHITE);
        Play.Draw();
        EndDrawing();
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && GetMouseX() > Play.GetX() && GetMouseX() < Play.GetX() + Play.GetWidth() && GetMouseY() > Play.GetY() && GetMouseY() < 7*(Play.GetY() + Play.GetHeight())){
            switch((GetMouseY() - Play.GetY()) / Play.GetHeight() + 1){
                case E_Play:
                int initializer = 0;
                std::vector<Sunflower> sunflowers(1);
                std::vector<Peashooter> peashooters(1);
                std::vector<Basic> basics(10);
                int boardTrack[5][9] = {0};
                ZombieTrack zombieTrack[5][10] = {0};
                int timer = 0;
                while(!backPressed){
                    BeginDrawing();
                    ClearBackground(WHITE);
                    if(!initializer){
                        GameSetup::SetRowMultiple(basics);
                        GameSetup::SetZombieTrackColumn(basics, 1);
                        GameSetup::SetZombieTrackMultiple(basics, zombieTrack, basics[0].GetCodeName());
                        GameSetup::SetZombieTrackPosition(zombieTrack);
                        initializer++;
                    }
                    Lawn::DrawLawn();
                    bool boardTrackAltered = false;  
                    int plantDeletedIndex = -1;      
                    Script::PlantMainScript(sunflowers, boardTrack, boardTrackAltered, KEY_A, plantDeletedIndex);
                    Script::SunProducerAdditionalScript(sunflowers, plantDeletedIndex);
                    plantDeletedIndex = -1; //pls give each plant type each own full script instead of this finicky shit
                    Script::PlantMainScript(peashooters, boardTrack, boardTrackAltered, KEY_S, plantDeletedIndex);
                    Script::ShooterAdditionalScript(peashooters, zombieTrack, plantDeletedIndex);
                    Script::ZombieMainScript(basics, boardTrack, boardTrackAltered, zombieTrack);
                    for(unsigned int i = 0; i < basics.size(); i++){
                        if(basics[i].GetPlantColumn() == basics[i].GetColumn()){
                            int boardTrackvalue = boardTrack[basics[i].GetRow()][basics[i].GetColumn()];
                            switch(Tracker::GetTrackerCodeName(boardTrackvalue)){
                            case E_Sunflower:
                            sunflowers[Tracker::GetTrackerIndex(boardTrackvalue)].SetHealth(sunflowers[Tracker::GetTrackerIndex(boardTrackvalue)].GetHealth() - basics[0].GetDamage());
                            break;
                            case E_PeaShooter:
                            peashooters[Tracker::GetTrackerIndex(boardTrackvalue)].SetHealth(peashooters[Tracker::GetTrackerIndex(boardTrackvalue)].GetHealth() - basics[0].GetDamage());
                            break;
                            }
                        }
                    }
                    for(unsigned int i = 0; i < peashooters.size() - 1; i++){
                        if(peashooters[i].ProjectileHit()){
                            peashooters[i].SetProjectileX(OutsideLawn);
                            int zombieTrackValue = zombieTrack[Lawn::YToRow(peashooters[i].GetY())][0].id;
                            switch(Tracker::GetTrackerCodeName(zombieTrackValue)){
                                case E_Basic:
                                basics[Tracker::GetTrackerIndex(zombieTrackValue)].SetHealth(basics[Tracker::GetTrackerIndex(zombieTrackValue)].GetHealth() - peashooters[0].GetDamage());
                                break;
                            }
                        }
                    }
                    timer++;
                    DrawText(TextFormat("%f", timer / 60.0), 0, 0, 30, RED);
                    if(IsKeyPressed(KEY_U)){
                        WaitTime(10);
                    }
                    if(IsKeyPressed(KEY_K)){
                        for(int i = 0; i < 5; i++){
                            for(int j = 0; j < 10; j++){
                                std::cout << zombieTrack[i][j].id << " " << zombieTrack[i][j].X;
                            }
                            std::cout << '\n';
                
                        }
                        std::cout << '\n' << '\n';
                        for(unsigned int i = 0; i < peashooters.size(); i++){
                            std::cout << i << " " << peashooters[i].GetZombieX() << '\n';
                        }
                    }
                    if(IsKeyPressed(KEY_M)){
                        for(int i = 0; i < 5; i++){
                            for(int j = 0; j < 8; j++){
                                std::cout << boardTrack[i][j] << " ";
                            }
                            std::cout << '\n';
                        }
                        std::cout << '\n' << '\n';
                    }
                    if(IsKeyDown(KEY_F)){
                        DrawText(TextFormat("%i", GetFPS()), 0, 0, 30, RED);
                    }
                    Back.Draw();
                    if(IsKeyPressed(KEY_P)){
                        for(unsigned int i = 0; i < basics.size(); i++){
                            std::cout << i << " " << basics[i].GetPlantColumn() << '\n';
                        }
                    }
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && GetMouseX() > Back.GetX() && GetMouseX() < Back.GetX() + Back.GetWidth() && GetMouseY() > Back.GetY() && GetMouseY() < Back.GetY() + Back.GetHeight()){
                        backPressed = true;
                    }
                    EndDrawing();
                    if(WindowShouldClose()){
                        CloseWindow();
                        return 0;
                    }
                }
                backPressed = false;
                break;
            }
        }
        if(IsKeyDown(KEY_F)){
            DrawText(TextFormat("%i", GetFPS()), 0, 0, 30, RED);
        }
    }
    CloseWindow();
    return 0;
}