#include "mbed.h"
#include "MMA7660.h"
#include "C12832.h"
#include <cmath>
#include <cstdlib> // pour random

// exercice fini jusqu'au 5.1 approx

MMA7660 MMA(p28, p27);
C12832 lcd(p5, p7, p6, p8, p11, "LCD");
DigitalOut connectionLed(LED1);
AnalogIn pot1(p19);
// pour des raisons obscurs, on peut seulement declarer les membres de struct a l'exterieur car il a besoin de membre statique

int coords_b[2];
int r_b = 1;
struct bille{
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
struct cible{
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
struct Jeu{

    bool CheckCollision(){
        return sqrt(pow(coords_b[0] - coords_c[0], 2.0) + pow(coords_b[1] - coords_c[1], 2)) <= pow(r_b+r_c, 2);
    }

    void Game_Over(){

    }

    void Reset(){

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
        
    while(1) {
        wait(1);
        lcd.cls();
        //DisplayXYZ();
    }

}
