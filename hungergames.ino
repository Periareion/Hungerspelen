// Kod skriven av Anton Oresten Sollman (GPT-4 hjälpte mig förstå pointers och annat)
// Del av "Hungerspelen", ett projekt med Manda Rosenberg och Sedra Fares
// Naturvetenskaplig Specialisering NA20
// Maj 2023

// Ett bibliotek som används för att styra motorerna. Se https://learn.sparkfun.com/tutorials/tb6612fng-hookup-guide#introduction
#include <SparkFun_TB6612.h>

// Innehåller Instruction och Pattern structs
#include "pattern.h"

// Varje direction får ett eget nummer så att de kan urskiljas.
// enum betyder enumerate
enum Directions {
  NOTHING, // 0
  FORWARD, // 1
  REVERSE, // 2
  LEFT,    // 3
  RIGHT,   // 4
};

// Skapar en instruktion där svängningsgraden kan justeras
Instruction Turn(
  int turn_direction = RIGHT,
  int duration = 1000,
  double turn_factor = 1.0,
  bool reverse = false
) {
  // reverse_coef allows backwards turning... and... reverse reversing.
  double reverse_coef = reverse ? -1.0 : 1.0;
  switch(turn_direction) {
    case FORWARD:
      return Instruction(reverse_coef*1.0, 0.0, duration);
    case REVERSE:
      return Instruction(reverse_coef*(-1.0), 0.0, duration);
    case LEFT:
      return Instruction(reverse_coef*1.0, -turn_factor, duration);
    case RIGHT:
      return Instruction(reverse_coef*1.0, turn_factor, duration);
    default:
      return Instruction(0.0, 0.0, duration);
  }
}

// Skapar en instruktion
Instruction Drive(
  int drive_direction = FORWARD,
  int duration = 1000,
  bool reverse = false
) {
  return Turn(drive_direction, duration, 0.0, reverse);
}

Instruction Brake(int duration = 1000) {
  return Drive(NOTHING, duration);
}

/* WELCOME TO THE FUN ZONE */

Instruction the_snake[] = {
  Drive(FORWARD, 1000),
  Turn(RIGHT, 1000, 0.4),
  Turn(LEFT, 1000, 0.4),
};

Pattern patterns[] = {
  Pattern(the_snake, 3),
};

// Denna funktion gör att det blir slumpmässigt vilket håll den backar.
// random(2)+3 kommer bli antingen RIGHT (3) eller LEFT (4) enligt "enum Directions"
Instruction go_back() {
  return Turn(random(2)+3, 2000, 1.0, true);
}

/*****   THE NO-NO ZONE   *****/
/***** Danger - Keep Away *****/

// long kan hålla 25 dagar värt av millisekunder
// unsigned long kan hålla 50 dagar, men blir problematisk vid subtraktion
long arena_timespan = 1000L*60*2; // Suffixen 'L' ser till att produkten blir en long (32 bit)

Instruction* instruction_pointer; // Håller koll på nuvarande instruktion
long instruction_start_time; // Tiden då nuvarande instruktion började
long instruction_duration; // Hur länge en instruktion håller på
int instruction_index; // Index av nuvarande instruktion i nuvarande mönstrets instruktion array

Pattern* pattern_pointer; // Håller koll på nuvarande mönster
int pattern_length; // Antal instruktioner i nuvarande pattern
int pattern_index = 0; // Index av nuvarande mönster i mönster-arrayen
int pattern_count = sizeof(patterns) / sizeof(Pattern); // Antalet mönster i mönster-arrayen

// En array som gör det möjligt att konvertera en instruktion till ett mönster 
Instruction single_instruction_array[1];

// Konverterar en enda instruktion till ett mönster med den instruktionen
// Används för att go_back ska kunna vara random
Pattern InstructionToPattern(Instruction instruction) {
  single_instruction_array[0] = instruction;
  Pattern pattern = Pattern(single_instruction_array, 1);
  return pattern;
}

Pattern go_back_pattern;

long setup_time;
long current_time;
long arena_start_time;

const int RED_LED = 10;
const int GREEN_LED = 11;
const int BLUE_LED = 12;

const int ANTENNA_PIN = 13;
int antenna_value;

const int SENSOR_PIN = A0; // infrared sensor pin number
int sensor_value;
int food_counter;

const int offsetA = 1;
const int offsetB = 1;
const int MOTOR_SPEED = 150; // Motorernas grundhastighet
const int STEERING_COEF = 50; // En konstant som påverkar hur skarpa svängarna blir. Multipliceras med Turn-funktionens turn_factor.
const int SENSOR_THRESHOLD = 500; // Det sensor-värde som skiljer på mat och inte mat

// Används vid direkt kontroll av motorerna
int base_speed;
int steer_term;

#define STBY 5
#define AIN1 6
#define AIN2 4
#define PWMA 3
#define BIN1 7
#define BIN2 8
#define PWMB 9

Motor left_motor = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor right_motor = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

void initializeArena() {
  food_counter = 0;
  arena_start_time = millis();
}

void setup() {
  pinMode(ANTENNA_PIN, INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  randomSeed(analogRead(A3));
  Serial.begin(9600);
  Serial.println(arena_timespan);
  setup_time = millis();
  current_time = setup_time;
  initializeArena();
}

void loop() {
  current_time = millis();
  setRGB(LOW, LOW, LOW);
  
  checkTime();
  checkAntenna();
  checkSensor();
  checkInstructions();
}

void setRGB(bool red_state, bool green_state, bool blue_state) {
  digitalWrite(RED_LED, red_state);
  digitalWrite(GREEN_LED, green_state);
  digitalWrite(BLUE_LED, blue_state);
}

void checkTime() {
  if (current_time - arena_start_time > arena_timespan) {
    doTimeOut();
  }
}

void checkAntenna() {
  antenna_value = digitalRead(ANTENNA_PIN);
  if (antenna_value == HIGH) {
    goBack();
  }
}

void checkSensor() {
  sensor_value = analogRead(SENSOR_PIN);
  if (sensor_value > SENSOR_THRESHOLD) {
    eatFood();
  }
}

void doTimeOut() {
  execute(Brake());
  setRGB(HIGH, HIGH, HIGH);
  Serial.print("Time is up! Seconds passed: "); Serial.println((current_time - arena_start_time) / 1000);
  Serial.print("Score: "); Serial.println(food_counter);
  delay(60000);
  initializeArena();
}

void goBack() {
  setRGB(HIGH, LOW, LOW);
  Serial.println("Backin' up...");
  go_back_pattern = InstructionToPattern(go_back());
  setPattern(&go_back_pattern);
}

void eatFood() {
  setRGB(LOW, HIGH, LOW); // Quettazillatron is a herbivore, hence the green light
  food_counter++;
  Serial.print("Mat: "); Serial.println(food_counter);
  execute(Brake());
  delay(2000);
  resumeInstruction();
  delay(1000); // Ser till att maten inte äts igen
}

void execute(Instruction instruction) {
  base_speed = instruction.speed_factor*MOTOR_SPEED;
  steer_term = instruction.speed_factor*STEERING_COEF*instruction.turn_factor;
  left_motor.drive(base_speed - steer_term);
  right_motor.drive(base_speed + steer_term);
  /*Serial.print("Executing ");
  Serial.print(instruction.speed_factor); Serial.print(",");
  Serial.print(instruction.turn_factor); Serial.print(",");
  Serial.println(instruction.duration);*/
}

void beginNextInstruction() {
  instruction_pointer = pattern_pointer -> instruction_array_pointer + instruction_index;
  instruction_duration = instruction_pointer -> duration;
  instruction_start_time = current_time;
  instruction_index++;
}

void setPattern(Pattern* pattern_ptr) {
  pattern_pointer = pattern_ptr;
  pattern_length = pattern_pointer -> pattern_length;
  instruction_index = 0;
  beginNextInstruction();
}

void resumeInstruction() {
  execute(*instruction_pointer);
  instruction_start_time = millis();
}

void checkInstructions() {
  if (pattern_pointer == NULL) {
    pattern_index = (pattern_index + 1 < pattern_count) ? (pattern_index + 1) : 0;
    setPattern(patterns + pattern_index);
  }
  
  if (current_time - instruction_start_time > instruction_duration) {
    if (instruction_index < pattern_length) {
      beginNextInstruction();
    }
    else {
      instruction_pointer = NULL;
      pattern_pointer = NULL;
      return;
    }
  }

  execute(*instruction_pointer);
}
