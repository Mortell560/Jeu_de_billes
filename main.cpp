#include "mbed.h"
#include "MMA7660.h"

MMA7660 MMA(p28, p27);

DigitalOut connectionLed(LED1);
PwmOut Zaxis_p(LED2);
PwmOut Zaxis_n(LED3);

int main() {  
    if (MMA.testConnection())
        connectionLed = 1;
        
    while(1) {
        Zaxis_p = MMA.z();
        Zaxis_n = -MMA.z();
    }

}
