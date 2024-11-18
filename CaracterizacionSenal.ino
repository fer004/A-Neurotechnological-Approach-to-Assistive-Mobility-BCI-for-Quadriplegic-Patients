// --------- Data Señal de entrada
const int Comunicacion = 7; // Pin para enviar señal digital a arduino controlador
float Fs = 200;                 // frecuencia de muestreo (Hz)
float T = 1 / Fs;               // Periodo de muestreo (s)
float pi = 3.1416;

// Variables de estado del filtro
float x1 = 0, y1 = 0, y2 = 0;
float x = 0, y = 0;

// -------- cutoff frecuencias
float fcLPF = 12;               // cutoff, lpf (Hz)
float fcHPF = 8;                // cutoff, hpf (Hz)
float tao1 = 1 / (2 * pi * fcHPF); // tao = RC, hpf
float tao2 = 1 / (2 * pi * fcLPF); // tao = RC, lpf

// Buffer para almacenar las muestras de salida
int bufferSize = 400; // 2 segundos * 200 muestras/segundo
float yBuffer[400] = {0};
int bufferIndex = 0;

// Umbral para enviar el valor lógico
float threshold = 800; // Ajustar según sea necesario

void setup() {
   pinMode(Comunicacion, OUTPUT); // Set Comunicacion pin as output
  Serial.begin(115200);
}

void loop() {
  // Actualización de variables de estado
  x1 = x;
  y2 = y1;
  y1 = y;

  // Cálculo de coeficientes del filtro pasa banda de 1er orden
  float a0 = tao1 * tao2 + (tao1 + tao2) * T + T * T;
  float a1 = -(2 * tao1 * tao2 + (tao1 + tao2) * T);
  float a2 = tao1 * tao2;
  float b0 = tao1 * T;
  float b1 = -tao1 * T;

  // Lectura de la señal de entrada
  x = analogRead(A0);

  // Cálculo de la salida del filtro
  y = -a1 / a0 * y1 - a2 / a0 * y2 + 2.5 * b0 / a0 * x + 2.5 * b1 / a0 * x1;

  // Almacenamiento de la salida en el buffer
  yBuffer[bufferIndex] = y;
  bufferIndex++;

  // Imprimir el valor de y filtrado
  //Serial2.print(y);

  // Si el buffer está lleno, se calcula la potencia promedio
  if (bufferIndex >= bufferSize) {
    float powerSum = 0;
    for (int i = 0; i < bufferSize; i++) {
      powerSum += yBuffer[i] * yBuffer[i]; // Potencia de cada muestra
    }
    float averagePower = powerSum / bufferSize;

    // Imprimir la potencia promedio en el serial
    Serial.print("Potencia promedio: ");
    Serial.print(averagePower);
    Serial.print(" - Valor lógico: ");

    // Comparar con el umbral y enviar el valor lógico
    if (averagePower > threshold) {
      digitalWrite(Comunicacion, HIGH);
      Serial.println(1);
    } else {
      digitalWrite(Comunicacion, LOW);
      Serial.println(0);
    }

    // Reiniciar el índice del buffer para la siguiente ventana
    bufferIndex = 0;
  
  }

  delay(4); // Delay para estabilidad
}

