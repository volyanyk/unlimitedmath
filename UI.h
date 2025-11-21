#ifndef UI_H
#define UI_H

#include "MathGenerator.h"
#include <fstream>
#include <map>
#include <ncurses.h>
#include <string>
#include <vector>

enum class MenuOption { START_GAME, SETTINGS, EXIT };

class UI {
public:
  UI();
  ~UI();

  void init();
  void cleanup();
  void setNonBlocking(bool enable);
  void loadLanguage(std::string lang);

  // Menu
  MenuOption showMainMenu();
  void showSettings(Difficulty &currentDiff, std::string &currentLang);
  void showGameOver(int score, int level);
  void showLevelComplete(int level);
  void showLevelUp(int level);

  // Game
  void drawGame(int score, int level, int challengesPassed,
                const MathProblem &problem, float timeLeft, int width,
                int height);
  int getInput(); // Returns key press

  int getScreenWidth();
  int getScreenHeight();

private:
  void drawBorders();
  void drawCentered(int y, std::string text);
  void drawCenteredX(int y, int x, int w, std::string text);
  std::string translate(std::string key);

  std::map<std::string, std::string> translations;
};

#endif // UI_H
