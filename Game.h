#ifndef GAME_H
#define GAME_H

#include "MathGenerator.h"
#include "UI.h"

class Game {
public:
  Game();
  void run();

private:
  void reset();
  void update();
  void handleInput();

  UI ui;
  MathGenerator mathGen;

  bool isRunning;
  bool inMenu;
  bool inLevelTransition;
  bool isGameOver;

  int score;
  int level;
  int challengesPassed;

  MathProblem currentProblem;

  // Game State
  float timeLeft;  // 0.0 to 1.0 (normalized)
  float timeDecay; // Amount to decrease per update

  Difficulty difficulty;
  std::string language;
};

#endif // GAME_H
