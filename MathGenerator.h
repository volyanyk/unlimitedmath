#ifndef MATHGENERATOR_H
#define MATHGENERATOR_H

#include <string>
#include <vector>

enum class Difficulty {
  EASY,   // +
  MEDIUM, // +, -
  HARD,   // +, -, *
  EXPERT, // +, -, *, /
  MASTER  // +, -, *, /, sqrt, cbrt
};

struct MathProblem {
  std::string question;
  int correctAnswer; // For simplicity, we'll stick to integer answers mostly,
                     // or rounded
  std::vector<int> options; // 3 options: Left, Up, Right mapping
  int correctOptionIndex;   // 0 for Left, 1 for Up, 2 for Right
};

class MathGenerator {
public:
  MathGenerator();
  void setDifficulty(Difficulty diff);
  MathProblem generateProblem(int previousResult, int challengesPassed);
  void startNewLevel();

private:
  Difficulty currentDifficulty;
  std::vector<int>
      usedOperands; // Track used 'b' operands for the current level
  int generateRandomNumber(int min, int max);
};

#endif // MATHGENERATOR_H
