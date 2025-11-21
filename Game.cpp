#include "Game.h"
#include <unistd.h>

Game::Game()
    : isRunning(true), inMenu(true), inLevelTransition(false),
      isGameOver(false), score(0), level(1), challengesPassed(0),
      timeLeft(1.0f), timeDecay(0.00083f), difficulty(Difficulty::EASY),
      language("English") {
  ui.init(); // Init ncurses first to be safe, though loadLanguage doesn't need
             // it, but good practice
  ui.loadLanguage(language);
}

void Game::run() {
  // ui.init() called in constructor or here? Let's keep it here if we want to
  // separate init. Actually, UI::init() starts ncurses. We might want to load
  // language before that or after. File loading is independent.

  while (isRunning) {
    if (inMenu) {
      MenuOption opt = ui.showMainMenu();
      if (opt == MenuOption::START_GAME) {
        inMenu = false;
        ui.setNonBlocking(true); // Enable non-blocking for game
        reset();
      } else if (opt == MenuOption::SETTINGS) {
        ui.showSettings(difficulty, language);
        mathGen.setDifficulty(difficulty);
      } else if (opt == MenuOption::EXIT) {
        isRunning = false;
      }
    } else if (isGameOver) {
      ui.showGameOver(score, level);
      // Check if user wants to restart or quit is handled inside showGameOver
      // loop or we just reset here if they return Actually showGameOver returns
      // when space is pressed (restart) or Q (quit). For simplicity, let's
      // assume it returns on restart request, or we can check input there.
      // Let's just reset to menu for now.
      inMenu = true;
      isGameOver = false;
    } else if (inLevelTransition) {
      ui.showLevelComplete(level);
      int ch = ui.getInput();
      if (ch == ' ') {
        inLevelTransition = false;
        // Start next level logic
        level++;
        mathGen.startNewLevel(); // Reset unique operands for new level

        // Duration: 20s, 17.5s, 15s... (-2.5s per level)
        float duration = 20.0f - (level - 1) * 2.5f;
        if (duration < 3.0f)
          duration = 3.0f;
        timeDecay = 1.0f / (duration * 60.0f);

        // Reset timer for new level
        timeLeft = 1.0f;
        // Generate new problem? Already generated after last success?
        // We generated a problem after success, but maybe we should regenerate
        // or keep it. Keeping it is fine.
      }
    } else {
      handleInput();
      update();
      ui.drawGame(score, level, challengesPassed, currentProblem, timeLeft,
                  ui.getScreenWidth(), ui.getScreenHeight());
      usleep(16000); // ~60 FPS
    }
  }

  ui.cleanup();
}

void Game::reset() {
  score = 0;
  level = 1;
  challengesPassed = 0;
  timeLeft = 1.0f;
  timeDecay =
      0.00083f; // Initial decay speed for 20s: 1.0 / (20 * 60) ~= 0.000833
  mathGen.setDifficulty(difficulty);
  currentProblem = mathGen.generateProblem(0, challengesPassed);
}

void Game::handleInput() {
  int ch = ui.getInput();
  if (ch != ERR) {
    int chosenOption = -1;
    switch (ch) {
    case KEY_LEFT:
      chosenOption = 0;
      break;
    case KEY_UP:
      chosenOption = 1;
      break;
    case KEY_RIGHT:
      chosenOption = 2;
      break;
    case 'q':
    case 'Q':
      inMenu = true;            // Quit to menu
      ui.setNonBlocking(false); // Disable non-blocking for menu
      return;
    }

    if (chosenOption != -1) {
      // Immediate validation
      if (currentProblem.options[chosenOption] ==
          currentProblem.correctAnswer) {
        // Correct
        score += 10 * level;
        challengesPassed++;

        if (challengesPassed % 10 == 0) {
          inLevelTransition = true;
          // Don't increment level yet, wait for transition
          // But we need to show "Level X Completed" or "Ready for Level X+1"?
          // Let's say "Level X Completed! Press Space for Level X+1"
        } else {
          // Normal problem generation
          currentProblem = mathGen.generateProblem(currentProblem.correctAnswer,
                                                   challengesPassed);
          timeLeft = 1.0f;
        }
      } else {
        // Wrong
        isGameOver = true;
      }
    }
  }
}

void Game::update() {
  timeLeft -= timeDecay;

  if (timeLeft <= 0.0f) {
    // Time out
    isGameOver = true;
  }
}
