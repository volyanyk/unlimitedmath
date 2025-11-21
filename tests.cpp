#include "MathGenerator.h"
#include <cassert>
#include <iostream>
#include <vector>

void testDifficultyEasy() {
  MathGenerator gen;
  gen.setDifficulty(Difficulty::EASY);
  for (int i = 0; i < 100; ++i) {
    MathProblem p = gen.generateProblem(0, 0);
    // Check if operation is +
    bool hasPlus = p.question.find('+') != std::string::npos;
    assert(hasPlus);
    // Check result
    // We can't easily parse the string back without regex or parsing logic,
    // but we can check if options contain the answer
    bool found = false;
    for (int opt : p.options) {
      if (opt == p.correctAnswer)
        found = true;
    }
    assert(found);
  }
  std::cout << "testDifficultyEasy passed." << std::endl;
}

void testDifficultyMedium() {
  MathGenerator gen;
  gen.setDifficulty(Difficulty::MEDIUM);
  bool seenMinus = false;
  for (int i = 0; i < 100; ++i) {
    MathProblem p = gen.generateProblem(0, 0);
    if (p.question.find('-') != std::string::npos)
      seenMinus = true;
  }
  assert(seenMinus);
  std::cout << "testDifficultyMedium passed." << std::endl;
}

void testUniqueOptions() {
  MathGenerator gen;
  for (int i = 0; i < 50; ++i) {
    MathProblem p = gen.generateProblem(0, 0);
    assert(p.options[0] != p.options[1]);
    assert(p.options[0] != p.options[2]);
    assert(p.options[1] != p.options[2]);
  }
  std::cout << "testUniqueOptions passed." << std::endl;
}

void testEasyScaling() {
  MathGenerator gen;
  gen.setDifficulty(Difficulty::EASY);
  for (int i = 0; i < 20; ++i) {
    MathProblem p = gen.generateProblem(0, i);

    // Check min value constraint (>= 6)
    // We need to parse the question string "a + b" or "a - b" to find b
    // Or we can check options, but options are derived from result.
    // Let's parse.
    size_t opPos = p.question.find_first_of("+-*/");
    if (opPos != std::string::npos) {
      std::string bStr = p.question.substr(opPos + 2); // Skip " + "
      int b = std::stoi(bStr);
      assert(b >= 6);

      if ((i % 10) >= 3) {
        assert(b >= 10);
      }
    }
  }
  std::cout << "testEasyScaling passed." << std::endl;
}

void testUniqueOperands() {
  MathGenerator gen;
  gen.setDifficulty(Difficulty::EASY);
  gen.startNewLevel();

  std::vector<int> usedBs;
  for (int i = 0; i < 10; ++i) {
    MathProblem p = gen.generateProblem(0, i);

    // Parse b
    size_t opPos = p.question.find_first_of("+-*/");
    if (opPos != std::string::npos) {
      // " + " -> op is at pos+1? No. "31 + 5". pos is 2. q[3] is '+'.
      // Wait, find_first_of returns index of first char from set.
      // "31 + 5". ' ' is at 2. '+' is at 3.
      // Actually let's just look for " + " or " - "

      // Simplified parsing: find last space
      size_t lastSpace = p.question.find_last_of(' ');
      if (lastSpace != std::string::npos) {
        std::string bStr = p.question.substr(lastSpace + 1);
        try {
          int b = std::stoi(bStr);

          // Only check for + and - as per requirement (implied by "never appear
          // again" context usually for the main drill) But let's check if it's
          // + or -
          if (p.question.find('+') != std::string::npos ||
              p.question.find('-') != std::string::npos) {
            for (int used : usedBs) {
              assert(used != b); // Should be unique
            }
            usedBs.push_back(b);
          }
        } catch (...) {
        }
      }
    }
  }
  std::cout << "testUniqueOperands passed." << std::endl;
}

int main() {
  testDifficultyEasy();
  testDifficultyMedium();
  testUniqueOptions();
  testEasyScaling();
  testUniqueOperands();
  std::cout << "All tests passed!" << std::endl;
  return 0;
}
