# Hungerspelen
 
På den här sidan finns koden för att kontrollera er robot.

## Skapa egna instruktioner

```ino
// Skapa ett nytt mönster genom att först skapa en array med instruktioner
Instruction example[] = {
  Drive(FORWARD, 1000),        // Kör framåt i 1000 millisekunder  
  Drive(BACKWARD, 1000),       // Kör bakåt i 1000 millisekunder
  Turn(RIGHT, 500, 1.0),       // Sväng höger i 500 millisekunder
  Turn(RIGHT, 500, 1.0, true), // Samma fast baklänges baklänges
  Turn(LEFT, 3000, 0.2),       // Sväng vänster i 3000 millisekunder, med svängningsfaktorn 0.2
  Brake(1000),                 // Stanna i 1000 millisekunder
};

// Rörelsen liknar en orm
Instruction the_snake[] = {
  Turn(RIGHT, 800, 0.4),
  Turn(LEFT, 800, 0.4),
  Turn(RIGHT, 800, 0.4),
  Turn(LEFT, 800, 0.4),
  Turn(RIGHT, 800, 0.4),
  Turn(LEFT, 800, 0.4),
};

// En array med mönster som kommer loopas igenom om och om igen.
// Skapa en "Pattern" genom att ange instruktion-array och hur många instruktioner som ska användas.
Pattern patterns[] = {
  Pattern(example, 6), // 6 
  Pattern(the_snake, 5), // 5 instruktioner kommer köras
};
/* Antal instruktioner behöver anges manuellt på grund av hur C++ hanterar "listor". 
   Det går inte att veta */
```
