#ifndef PATTERN_H
#define PATTERN_H

struct Instruction {
  double speed_factor; // between -1.0 and 1.0 (positive forward, negative reverse)
  double turn_factor; // between -2.0 and 2.0 (positive right, negative left)
  int duration; // measured in milliseconds
  
  // Default constructor
  Instruction() : speed_factor(0.0), turn_factor(0.0), duration(0) {}

  Instruction(double sf, double tf, int d) : speed_factor(sf), turn_factor(tf), duration(d) {}
};

struct Pattern {
  Instruction* instruction_array_pointer;
  int pattern_length;

  Pattern(Instruction* arr_ptr = NULL, int len = 0) : instruction_array_pointer(arr_ptr), pattern_length(len) {}
};

#endif
