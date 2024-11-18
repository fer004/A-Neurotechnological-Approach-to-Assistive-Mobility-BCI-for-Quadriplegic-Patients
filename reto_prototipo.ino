#include <MD_MAX72xx.h>

uint8_t buf[8] = { 0 }; // buf reporte teclado
#define HARDWARE_TYPE MD_MAX72XX::DR0CR0RR1_HW
#define NUM_OF_MATRIX 1
// Definir pines 
#define SIGNAL_PIN 2 //Pin que recibe señal enviada del arduino de caracterizacion 
#define CLK_PIN   5
#define DATA_PIN  7
#define CS_PIN    6

#define DELAY_ANIMATION 3000
#define KEY_PRESS_DELAY 1000

int dir = 0;
bool animationEnabled = true;
bool animationPaused = false;

uint8_t flecha1[] = { 
  0b00010000,  // Derecha
  0b00110000,  
  0b01111111,  
  0b11111111,  
  0b11111111,  
  0b01111111,  
  0b00110000,  
  0b00010000     
};
 
uint8_t flecha2[] = { 
0b00111100,  //    Arriba
0b00111100,  //    
0b00111100,  //    
0b00111100,  //   
0b11111111,  // 
0b01111110,  //  
0b00111100,  //   
0b00011000   // 
};

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, NUM_OF_MATRIX);

void setup() {
  pinMode(SIGNAL_PIN, INPUT); // Definir Pin de señal como input
  Serial.begin(9600);
  
  // Inicializar Matriz
  mx.begin();

  // Test de matriz
  mx.control(MD_MAX72XX::TEST, true);
  delay(2000);

  // Terminar test de matriz
  mx.control(MD_MAX72XX::TEST, false);


  mx.control(MD_MAX72XX::INTENSITY, 5);
}

// Recibe configuración de matriz para trazado de flechas y cambio de dibujo
void drawRows(const byte fig[]) {
  for (int i = 0; i < 8; i++) {
    mx.setRow(0, i, fig[i]);
  }
}

// Animacion con flechas
void animateArrow() {
  if (animationEnabled && !animationPaused) {
    static int currentArrow = 0;
    uint8_t* arrows[] = {flecha1, flecha2}; 
    // Clear the screen
    mx.clear();

    // Dibujar grafico actual
    drawRows(arrows[currentArrow]);
    dir = currentArrow; // Direccion corresponde a flecha actual
    delay(DELAY_ANIMATION);

    // Avanca a siguiente grafico
    currentArrow = (currentArrow + 1) % 2;
  }
}

void releaseKey() {
  buf[0] = 0;
  buf[2] = 0;
  Serial.write(buf, 8); // Suelta tecla
}

void loop() {
  int switchState = digitalRead(SIGNAL_PIN); // Lectura de señal recibida
  //Serial.println(switchState);
  //Serial.println(digitalRead(SIGNAL_PIN));
  // Revisar si la señal es positiva, o sea se detecto actividad motora 
  if (switchState == 1) {
    // Enviar tecla acorde a dirección
    switch (dir) {
      case 0: // flecha1 (Derecha)
        Serial.println("Arriba");
        buf[2] = 38; // Keycode flecha arriba
        break;
    }
    Serial.write(buf, 8); // enviar tecla
    releaseKey(); // Soltar tecla despues de enviar
    animationPaused = true; // Pausar animacion
  } else {
    // Reiniciar estado si señal recibida es baja
    animationPaused = false;
    releaseKey(); // Redundancia de liberacion de tecla
  }

  // Animar flechas
  animateArrow();  
}