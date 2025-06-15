#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int buzzerPin = 8;
const int buttonPin = 4;

unsigned long startTime;
unsigned long lastBeatTime = 0;
unsigned long gameDuration = 30000;
int score = 0;
int combo = 0;
int maxCombo = 0;
int noteY = -1;
int noteSpeed = 2;
bool gameOver = false;
bool noteActive = false;
bool judged = false;

void showCenteredText(const char* text, int size, int y = 10) {
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, y);
  display.println(text);
  display.display();
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  display.clearDisplay();
  display.display();

  // 카운트다운
  showCenteredText("READY", 2); delay(800);
  for (int i = 3; i > 0; i--) {
    char count[2]; sprintf(count, "%d", i);
    showCenteredText(count, 3); delay(700);
  }
  showCenteredText("START", 2); delay(1000);

  display.clearDisplay();
  display.display();
  startTime = millis();
  Serial.begin(9600);
}

void loop() {
  if (gameOver) return;
  unsigned long now = millis();

  if (now - startTime >= gameDuration) {
    endGame();
    return;
  }

  if (!noteActive && now - lastBeatTime > 1000) {
    noteActive = true;
    noteY = 0;
    judged = false;
    lastBeatTime = now;

    // 랜덤 부저 리듬
    int freq = random(600, 1500);
    int dur = random(60, 180);
    tone(buzzerPin, freq, dur);

    // 노트 떨어지는 속도 랜덤 (픽셀/frame)
    noteSpeed = random(1, 4);
  }

  if (noteActive) {
    display.clearDisplay();

    // 점수 정보
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SCORE:");
    display.print(score);
    display.setCursor(80, 0);
    display.print("COMBO:");
    display.print(combo);

    // 노트 도형
    display.fillRect(60, noteY, 8, 4, SSD1306_WHITE);
    display.display();

    noteY += noteSpeed;
    delay(60);

    if (!judged && noteY >= 24 && noteY <= 30) {
      if (digitalRead(buttonPin) == LOW) {
        showCenteredText("PERFECT", 1);
        tone(buzzerPin, 1500, 100);
        score++;
        combo++;
        if (combo > maxCombo) maxCombo = combo;
        judged = true;
        delay(300);
      }
    }

    if (noteY > 32) {
      if (!judged) {
        showCenteredText("MISS", 1);
        combo = 0;
        delay(300);
      }
      noteActive = false;
    }
  }
}

void endGame() {
  gameOver = true;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("=== GAME OVER ===");
  display.print("SCORE: ");
  display.println(score);
  display.print("MAX COMBO: ");
  display.println(maxCombo);
  display.print("TIME: ");
  display.print((millis() - startTime) / 1000);
  display.println("s");
  display.display();
  tone(buzzerPin, 500, 500);
}
