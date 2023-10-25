#include "mbed.h"
#include "MMA7660.h"
#include "C12832.h"
#include "mbed2/299/platform/wait_api.h"
#include <cmath>
#include <cstdlib> // pour random

// exercice fini jusqu'au 5.1 approx

MMA7660 MMA(p28, p27);
C12832 lcd(p5, p7, p6, p8, p11, "LCD");
DigitalOut connectionLed(LED1);
AnalogIn pot1(p19);
const int screen[2] = {lcd.width(), lcd.height()};
int offset = 5;
// pour des raisons obscurs, on peut seulement declarer les membres de struct a l'exterieur car il a besoin de membre statique

int coords_b[2];
int r_b = 1;
int speed = 10; // 10 pixels for a full acceleration along one axis
struct Bille{

    void Init(){
        coords_b[0] = screen[0]/2;
        coords_b[1] = screen[1]/2;
    }

    void SetPos(int x, int y){
        coords_b[0] = x;
        coords_b[1] = y;
    }

    void show(){
        lcd.circle(coords_b[0], coords_b[1], r_b, 1);
    }

};


int coords_c[2];
int r_c = 3;
struct Cible{

    void randomInit(){
        coords_c[0] = rand() % screen[0]-2;
        coords_c[1] = rand() % screen[1]-2;
    }

    void SetPos(int x, int y){
        coords_c[0] = x;
        coords_c[1] = y;
    }

    void show(){
        lcd.circle(coords_c[0], coords_c[1], r_c, 1);
    }
};

int temps_de_jeu = 60; // en seconde
int score = 0;
int lvl = 1;
Timer t;
struct Jeu{

    bool CheckCollisionTarget(){
        return sqrt(pow(coords_b[0] - coords_c[0], 2.0) + pow(coords_b[1] - coords_c[1], 2)) <= r_b+r_c;
    }

    void AddScore(int n){
        score += n;
    }

    void Game_Over(){
        lcd.cls();
        lcd.locate(screen[0]/2-offset, screen[1]/2);
        lcd.printf("Game Over");
        wait(4);
        Reset();
    }

    void Reset(){
        score = 0;
        temps_de_jeu = 60;
        t.reset();
    }

    bool IsInBound(int x, int y){
        return (y > 0 && y < screen[1]) && (x > 0 && x < screen[0]);
    }

    void WinRound(){
        lvl++;
        AddScore(temps_de_jeu);
        temps_de_jeu -= 5;
        lcd.cls();
        lcd.locate(screen[0]/2-offset, screen[1]/2);
        lcd.printf("Moving to level %i", lvl);
        lcd.locate(screen[0]/2-offset, screen[1]/2-offset*2);
        lcd.printf("Time to finish: %i, score: %i", temps_de_jeu, score);
        wait(2);
        t.reset();
    }


};

int AnalogIn2Index(float v, int n){return (v*n==n)? n-1 : v*n ;}

float* ReadXYZ(){
    // Tres instable, use ReadN
    float arr[3] = {MMA.x(), MMA.y(), MMA.z()};
    return arr;
}

float* ReadN(int n = 100){
    float m[3];
    for (int i = 0; i < n; i++){
        float* r;
        r = ReadXYZ();
        for (int j = 0; j < 3; j++){
            m[j] += r[j];
        }
    }
    for (int k = 0; k < 3; k++){
        m[k] /= n;
    }
    return m;
}

void DisplayXYZ(){
    float* arr = ReadN(AnalogIn2Index(pot1.read(), 5) * 100);
    lcd.locate(0, 8);
    lcd.printf("%3.2f %3.2f %3.2f", arr[0], arr[1], arr[2]);
}

int main() {  
    if (MMA.testConnection())
        connectionLed = 1;
    
    Jeu j;
    Bille b;
    Cible c;

    b.Init();
    c.randomInit();
    t.start();
        
    while(1) {
        if (t.read() > temps_de_jeu){
            j.Game_Over();
            c.randomInit();
            b.Init();
            lcd.cls();
            continue;
        }
        if (j.CheckCollisionTarget()){
            j.WinRound();
            c.randomInit();
            b.Init();
            lcd.cls();
            continue;
        }

        b.show();
        c.show();
        float* m;
        m = ReadN(AnalogIn2Index(pot1.read(), 5) * 100);
        if (j.IsInBound(coords_b[0] + m[0] * speed * m[2], coords_b[1] + m[1] * speed * m[2])){
            b.SetPos(coords_b[0] + m[0] * speed * m[2], coords_b[1] + m[1] * speed * m[2]);
        }
        wait_ms(5);
        lcd.cls();
        
    }

}
