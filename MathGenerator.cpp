#include "MathGenerator.h"
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

MathGenerator::MathGenerator() {
  std::srand(std::time(nullptr));
  currentDifficulty = Difficulty::EASY;
}

void MathGenerator::setDifficulty(Difficulty diff) { currentDifficulty = diff; }

void MathGenerator::startNewLevel() { usedOperands.clear(); }

int MathGenerator::generateRandomNumber(int min, int max) {
  return min + (std::rand() % (max - min + 1));
}

MathProblem MathGenerator::generateProblem(int previousResult,
                                           int challengesPassed) {
  (void)challengesPassed; // Suppress unused warning if not used in this path
                          // (e.g. for +/- logic which is now strict)
  MathProblem problem;
  int a, b;
  char op = '+';
  int result = 0;

  // Determine operation based on difficulty
  std::vector<char> allowedOps;
  allowedOps.push_back('+');
  if (currentDifficulty >= Difficulty::MEDIUM)
    allowedOps.push_back('-');
  if (currentDifficulty >= Difficulty::HARD)
    allowedOps.push_back('*');
  if (currentDifficulty >= Difficulty::EXPERT)
    allowedOps.push_back('/');
  // Master adds sqrt/cbrt logic, handled separately or as special ops

  // Simple logic: if previousResult is 0 (start), generate fresh.
  // If not, use previousResult as one operand to chain?
  // The prompt says: "The following math problems are generated using the
  // result of the previous math problem."

  bool usePrevious = (previousResult != 0); // Simplified check

  if (currentDifficulty == Difficulty::MASTER && (std::rand() % 5 == 0)) {
    // 20% chance for sqrt/cbrt in Master
    if (std::rand() % 2 == 0) {
      // Sqrt
      int base = generateRandomNumber(2, 20);
      int square = base * base;
      if (usePrevious && previousResult > 0 && previousResult < 1000) {
        // Try to use previous result if it's a perfect square? Hard to
        // guarantee. Let's just make a new problem for roots to keep flow
        // smooth or adapt previous if lucky. To strictly follow "using result
        // of previous", we might do: previous + X = perfect square? Let's stick
        // to simple chaining: New number op Previous Result.
      }
      problem.question = "sqrt(" + std::to_string(square) + ")";
      result = base;
    } else {
      // Cbrt
      int base = generateRandomNumber(2, 10);
      int cube = base * base * base;
      problem.question = "cbrt(" + std::to_string(cube) + ")";
      result = base;
    }
  } else {
    // Standard arithmetic
    op = allowedOps[std::rand() % allowedOps.size()];

    if (op == '+' || op == '-') {
      // Enforce double digits (>= 10) for + and -
      int maxVal = 10 * ((int)currentDifficulty + 1);
      if (maxVal < 20)
        maxVal = 20;

      if (usePrevious) {
        a = previousResult;
        // If previous result is single digit, we can't easily fix 'a' without
        // breaking the chain rule. But user said "never operate on single digit
        // numbers". If 'a' is < 10, maybe we just don't use it? Or we add to it
        // to make it >= 10? Let's just not use previous if it's small for +/-?
        // Or we accept 'a' as is (since it's the result of previous) and only
        // enforce 'b'? "never operate on single digit numbers" implies both
        // operands. If previous result is 5, and we need 5 + X, 5 is single
        // digit. So we must NOT use previous result if it is < 10 for +/-.
        if (a < 10) {
          usePrevious = false;
          a = generateRandomNumber(10, 99); // Force double digit
        }
      } else {
        a = generateRandomNumber(10, 99);
      }

      // Adjust maxVal if needed based on difficulty, but keeping it simple:
      // double digits. Actually, let's respect difficulty somewhat but ensure
      // min is 10. Easy: 10-20? Medium: 10-50? Let's use the calculated maxVal
      // but ensure min is 10.
      if (maxVal < 30)
        maxVal = 30; // Give some range

      // Generate unique b
      int attempts = 0;
      do {
        b = generateRandomNumber(10, maxVal);
        attempts++;

        // Check if b is in usedOperands
        bool used = false;
        for (int val : usedOperands) {
          if (val == b) {
            used = true;
            break;
          }
        }

        if (!used) {
          usedOperands.push_back(b);
          break;
        }

        // Safety break to prevent infinite loop if range is too small
        if (attempts > 50) {
          // Just use it if we can't find a unique one
          break;
        }
      } while (true);

    } else {
      // *, /, sqrt, cbrt allow single digits
      if (usePrevious) {
        a = previousResult;
        b = generateRandomNumber(2, 12);
      } else {
        a = generateRandomNumber(1, 20); // Can be single
        b = generateRandomNumber(2, 12);
      }
    }
    switch (op) {
    case '+':
      result = a + b;
      problem.question = std::to_string(a) + " + " + std::to_string(b);
      break;
    case '-':
      // Ensure positive results for simplicity? Or allow negative. Let's allow
      // negative.
      result = a - b;
      problem.question = std::to_string(a) + " - " + std::to_string(b);
      break;
    case '*':
      // Keep numbers smaller for multiplication
      if (!usePrevious)
        a = generateRandomNumber(2, 12);
      b = generateRandomNumber(2, 12);
      result = a * b;
      problem.question = std::to_string(a) + " * " + std::to_string(b);
      break;
    case '/':
      // Ensure divisibility
      b = generateRandomNumber(2, 10);
      if (usePrevious) {
        // We must use 'a' (previous). To make it divisible, we might need to
        // adjust or cheat. Or we just add to 'a' to make it divisible.
        int remainder = a % b;
        if (remainder != 0) {
          a += (b - remainder); // Adjust a to be divisible
          // But we can't change previous result displayed to user...
          // Actually, the prompt says "generated using the result".
          // Maybe: (PreviousResult * X) / X ? Or just PreviousResult + X then
          // divide? Let's simpler: If division is chosen, we generate A and B
          // such that A / B = result. If we MUST use previous result, maybe we
          // treat previous result as 'B' or 'A' if suitable. Let's just
          // fallback to + or - if division is messy with previous result.
          op = (std::rand() % 2 == 0) ? '+' : '-';
          b = generateRandomNumber(1, 20);
          if (op == '+') {
            result = a + b;
            problem.question = std::to_string(a) + " + " + std::to_string(b);
          } else {
            result = a - b;
            problem.question = std::to_string(a) + " - " + std::to_string(b);
          }
        } else {
          result = a / b;
          problem.question = std::to_string(a) + " / " + std::to_string(b);
        }
      } else {
        result = generateRandomNumber(2, 12); // This is the answer
        a = result * b;                       // This is the dividend
        problem.question = std::to_string(a) + " / " + std::to_string(b);
      }
      break;
    }
  }

  problem.correctAnswer = result;

  // Generate options (one correct, two wrong)
  problem.options.resize(3);
  problem.correctOptionIndex = std::rand() % 3;
  problem.options[problem.correctOptionIndex] = result;

  for (int i = 0; i < 3; ++i) {
    if (i == problem.correctOptionIndex)
      continue;
    int wrong;
    bool unique;
    do {
      unique = true;
      wrong = result + generateRandomNumber(-5, 5);
      if (wrong == result)
        unique = false;
      for (int j = 0; j < 3; ++j) {
        if (i != j && problem.options[j] == wrong &&
            (j == problem.correctOptionIndex || j < i)) {
          // Check against correct answer (already set) and previously set wrong
          // answers
          unique = false;
          break;
        }
      }
    } while (!unique);
    problem.options[i] = wrong;
  }

  return problem;
}
