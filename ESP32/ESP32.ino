#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>

// Configuration WiFi
#define WIFI_SSID ".."
#define WIFI_PASSWORD "12131415"
#define FILE_URL "http://192.168.32.215:8000/application.bin"
#define FILE_PATH "/downloaded_file.bin"

// Configuration UART
#define TX_PIN 17  // Pin TX de l'ESP32
#define RX_PIN 16  // Pin RX de l'ESP32
#define BAUD_RATE 115200  // Vitesse de communication UART

void setup() {
  Serial.begin(115200);  // Moniteur série
  Serial2.begin(BAUD_RATE, SERIAL_8N1, RX_PIN, TX_PIN);  // UART avec STM32

  // Initialisation de SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Erreur : Échec de l'initialisation de SPIFFS.");
    return;
  }

  // Connexion au WiFi
  connectToWiFi();

  // Téléchargement du fichier
  if (downloadFileToSPIFFS()) {
    // Transfert du fichier à la STM32 via UART
    sendFileToSTM32();
  } else {
    Serial.println("Échec du téléchargement du fichier.");
  }
}

void loop() {
  // Rien à exécuter en boucle
}

// Fonction pour se connecter au WiFi
void connectToWiFi() {
  Serial.print("Connexion à WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnecté au WiFi !");
}

// Téléchargement du fichier depuis le serveur local
bool downloadFileToSPIFFS() {
  HTTPClient http;
  http.begin(FILE_URL);
  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    File file = SPIFFS.open(FILE_PATH, FILE_WRITE);
    if (!file) {
      Serial.println("Erreur : Impossible d'ouvrir le fichier SPIFFS pour écriture.");
      return false;
    }

    WiFiClient* stream = http.getStreamPtr();
    size_t fileSize = http.getSize();
    size_t written = 0;
    uint8_t buffer[1024];

    Serial.println("Téléchargement en cours...");
    while (http.connected() && (written < fileSize || fileSize == -1)) {
      size_t size = stream->available();
      if (size) {
        int len = stream->readBytes(buffer, min(size, sizeof(buffer)));
        file.write(buffer, len);
        written += len;

        // Afficher la progression
        if (fileSize > 0) {
          int progress = (written * 100) / fileSize;
          Serial.printf("\rProgression : %d%%", progress);
        }
      }
      delay(1);  // Éviter le blocage
    }
    file.close();
    Serial.println("\nTéléchargement terminé !");
    return true;
  } else {
    Serial.printf("Erreur HTTP : %d\n", httpCode);
    return false;
  }
  http.end();
}

// Envoi du fichier à la STM32 via UART
void sendFileToSTM32() {
  File file = SPIFFS.open(FILE_PATH, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  size_t fileSize = file.size();
  size_t totalBytesSent = 0;
  uint8_t buffer[1024];  // Block size
  char stmResponse;

  Serial.println("Waiting for STM32 to send 'g'...");
  while (true) {
    if (Serial2.available() && Serial2.read() == 'g') {
      Serial.println("Received 'g' from STM32. Sending 'r'...");
      Serial2.write('r');  // Send acknowledgment to start the transfer
      break;
    }
    delay(10);
  }

  // Send file size in 2 bytes (low byte and high byte)
  uint8_t lowByte = fileSize & 0xFF;
  uint8_t highByte = (fileSize >> 8) & 0xFF;

  Serial.println("Sending file size to STM32...");
  while (true) {
    if (Serial2.available() && Serial2.read() == 'y') {
      Serial2.write(lowByte);
      break;
    }
    delay(10);
  }

  while (true) {
    if (Serial2.available() && Serial2.read() == 'x') {
      Serial2.write(highByte);
      break;
    }
    delay(10);
  }

  // Send the firmware data block by block
  Serial.println("Starting firmware data transfer...");
  while (totalBytesSent < fileSize) {
    size_t bytesRead = file.read(buffer, sizeof(buffer));

    // Wait for STM32 to send 'y' (ready for block)
    while (true) {
      if (Serial2.available() && Serial2.read() == 'y') {
        for (int i=0 ; i<bytesRead ; i++) {
          Serial2.write(buffer[i]);
        delay(10);
         while (true) {
      if (Serial2.available() && Serial2.read() == 'x') {
        break;
      }
      delay(10);
    }

        }
        break;
      }
      delay(10);
    }

    totalBytesSent += bytesRead;

    // Log progress
    Serial.printf("Block transmitted. Total bytes sent: %d/%d\n", totalBytesSent, fileSize);
  }

  Serial.println("Firmware transfer completed!");
  file.close();
}
