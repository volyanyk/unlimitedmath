#include "UI.h"
#include <clocale>
#include <sstream>
#include <unistd.h>

// Helper to calculate UTF-8 string length (number of characters, not bytes)
size_t utf8_length(const std::string &str) {
  size_t len = 0;
  for (size_t i = 0; i < str.length(); ++i) {
    if ((str[i] & 0xC0) != 0x80) {
      len++;
    }
  }
  return len;
}

UI::UI() {}

UI::~UI() { cleanup(); }

void UI::init() {
  setlocale(LC_ALL, ""); // Enable system locale for UTF-8 support
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  nodelay(stdscr, TRUE); // Non-blocking input
  start_color();
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
}

void UI::setNonBlocking(bool enable) { nodelay(stdscr, enable); }

void UI::cleanup() { endwin(); }

int UI::getScreenWidth() {
  int w, h;
  getmaxyx(stdscr, h, w);
  (void)h; // Suppress unused warning
  return w;
}

int UI::getScreenHeight() {
  int w, h;
  getmaxyx(stdscr, h, w);
  return h;
}

void UI::loadLanguage(std::string lang) {
  translations.clear();

  // Map full name to ISO code
  std::string code = "en";
  if (lang == "English")
    code = "en";
  else if (lang == "German")
    code = "de";
  else if (lang == "French")
    code = "fr";
  else if (lang == "Spanish")
    code = "es";
  else if (lang == "Italian")
    code = "it";
  else if (lang == "Portuguese")
    code = "pt";
  else if (lang == "Dutch")
    code = "nl";
  else if (lang == "Ukrainian")
    code = "uk";
  else if (lang == "Polish")
    code = "pl";
  else if (lang == "Chinese")
    code = "zh";
  else if (lang == "Japanese")
    code = "ja";
  else if (lang == "Korean")
    code = "ko";

  std::string filename = "lang/" + code + ".json";
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      // Simple JSON extraction: "key": "value"
      size_t colonPos = line.find(':');
      if (colonPos != std::string::npos) {
        size_t keyStart = line.find('"');
        size_t keyEnd = line.find('"', keyStart + 1);
        size_t valStart = line.find('"', colonPos + 1);
        size_t valEnd = line.find('"', valStart + 1);

        if (keyStart != std::string::npos && keyEnd != std::string::npos &&
            valStart != std::string::npos && valEnd != std::string::npos) {
          std::string key = line.substr(keyStart + 1, keyEnd - keyStart - 1);
          std::string value = line.substr(valStart + 1, valEnd - valStart - 1);
          translations[key] = value;
        }
      }
    }
    file.close();
  }
}

std::string UI::translate(std::string key) {
  if (translations.find(key) != translations.end()) {
    return translations[key];
  }
  return key; // Fallback to key
}

void UI::drawCentered(int y, std::string text) {
  int w = getScreenWidth();
  mvprintw(y, (w - utf8_length(text)) / 2, "%s", text.c_str());
}

void UI::drawCenteredX(int y, int x, int w, std::string text) {
  mvprintw(y, x + (w - utf8_length(text)) / 2, "%s", text.c_str());
}

MenuOption UI::showMainMenu() {
  nodelay(stdscr, FALSE); // Blocking for menu
  int choice = 0;
  const int numOptions = 3;
  // Dynamic options based on translation

  while (true) {
    clear();
    drawCentered(5, translate("title"));

    std::string options[] = {translate("start_game"), translate("settings"),
                             translate("exit")};

    for (int i = 0; i < numOptions; ++i) {
      if (i == choice)
        attron(A_REVERSE);
      drawCentered(10 + i * 2, options[i]);
      if (i == choice)
        attroff(A_REVERSE);
    }

    int ch = getch();
    switch (ch) {
    case KEY_UP:
      choice = (choice - 1 + numOptions) % numOptions;
      break;
    case KEY_DOWN:
      choice = (choice + 1) % numOptions;
      break;
    case 10: // Enter
      return static_cast<MenuOption>(choice);
    }
  }
}

void UI::showSettings(Difficulty &currentDiff, std::string &currentLang) {
  nodelay(stdscr, FALSE);
  int choice = 0;
  const int numOptions = 3; // Diff, Lang, Back

  std::string diffNames[] = {"Easy", "Medium", "Hard", "Expert", "Master"};
  std::string languages[] = {"English", "German",     "French",   "Spanish",
                             "Italian", "Portuguese", "Dutch",    "Ukrainian",
                             "Polish",  "Chinese",    "Japanese", "Korean"};
  // int langIndex = 0; // Simplified for now, just cycling

  while (true) {
    clear();
    drawCentered(5, translate("settings"));

    std::string diffStr =
        translate("difficulty") + ": " + diffNames[(int)currentDiff];
    std::string langStr = translate("language") + ": " + currentLang;
    std::string backStr = translate("back");

    if (choice == 0)
      attron(A_REVERSE);
    drawCentered(10, diffStr);
    if (choice == 0)
      attroff(A_REVERSE);

    if (choice == 1)
      attron(A_REVERSE);
    drawCentered(12, langStr);
    if (choice == 1)
      attroff(A_REVERSE);

    if (choice == 2)
      attron(A_REVERSE);
    drawCentered(14, backStr);
    if (choice == 2)
      attroff(A_REVERSE);

    int ch = getch();
    switch (ch) {
    case KEY_UP:
      choice = (choice - 1 + numOptions) % numOptions;
      break;
    case KEY_DOWN:
      choice = (choice + 1) % numOptions;
      break;
    case KEY_LEFT:
      if (choice == 0) {
        int d = (int)currentDiff - 1;
        if (d < 0)
          d = 4;
        currentDiff = (Difficulty)d;
      } else if (choice == 1) {
        // Cycle languages backwards
        int numLangs = 12;
        int currentLangIdx = 0;
        for (int i = 0; i < numLangs; ++i)
          if (languages[i] == currentLang)
            currentLangIdx = i;
        currentLangIdx = (currentLangIdx - 1 + numLangs) % numLangs;
        currentLang = languages[currentLangIdx];
        loadLanguage(currentLang); // Reload immediately
      }
      break;
    case KEY_RIGHT:
      if (choice == 0) {
        int d = (int)currentDiff + 1;
        if (d > 4)
          d = 0;
        currentDiff = (Difficulty)d;
      } else if (choice == 1) {
        // Cycle languages
        int numLangs = 12;
        int currentLangIdx = 0;
        for (int i = 0; i < numLangs; ++i)
          if (languages[i] == currentLang)
            currentLangIdx = i;
        currentLangIdx = (currentLangIdx + 1) % numLangs;
        currentLang = languages[currentLangIdx];
        loadLanguage(currentLang); // Reload immediately
      }
      break;
    case 10: // Enter
      if (choice == 2)
        return;
      break;
    }
  }
}

void UI::showGameOver(int score, int level) {
  nodelay(stdscr, TRUE); // Non-blocking for animation

  // Crying Dolphin ASCII
  std::string dolphin = "          ,\n"
                        "      __)\\_  \n"
                        "(\\_.-'    a`-.\n"
                        "(/~~````(/~^^`";

  int frame = 0;
  while (true) {
    clear();
    attron(COLOR_PAIR(3));
    drawCentered(5, translate("game_over"));
    attroff(COLOR_PAIR(3));
    drawCentered(7, translate("score") + ": " + std::to_string(score));
    drawCentered(8, translate("level") + ": " + std::to_string(level));
    drawCentered(20, translate("press_space"));

    // Draw dolphin
    int dX = getScreenWidth() / 2 - 10;
    int dY = 12;
    int curY = dY;
    std::stringstream ss(dolphin);
    std::string line;
    while (std::getline(ss, line)) {
      mvprintw(curY++, dX, "%s", line.c_str());
    }

    // Animate tears
    if (frame % 4 == 0)
      mvprintw(dY + 2, dX + 12, ".");
    if (frame % 4 == 1)
      mvprintw(dY + 3, dX + 12, "o");
    if (frame % 4 == 2)
      mvprintw(dY + 4, dX + 12, "O");
    if (frame % 4 == 3)
      mvprintw(dY + 5, dX + 12, ".");

    refresh();
    usleep(200000);
    frame++;

    int ch = getch();
    if (ch == ' ') {
      nodelay(stdscr, FALSE);
      return;
    }
    if (ch == 'q' || ch == 'Q') {
      nodelay(stdscr, FALSE);
      return;
    }
  }
}

void UI::showLevelComplete(int level) {
  // Moving Cat Animation
  std::string cat1 =
      "      |\\      _,,,---,,_\nZZZzz /,`.-'`'    -.  ;-;;,_\n     |,4-  ) "
      ")-,_. ,\\ (  `'-'\n    '---''(_/--'  `-'\\_)";
  // Simple cat ASCII

  int w = getScreenWidth();
  int h = getScreenHeight();
  int catY = h / 2;

  nodelay(stdscr, TRUE); // Non-blocking to allow animation loop

  for (int x = -20; x < w; x += 2) {
    clear();
    drawCentered(h / 2 - 5, translate("level") + " " + std::to_string(level) +
                                " " + "Completed!");
    drawCentered(h / 2 + 5, translate("press_space"));

    // Draw cat at x
    int curY = catY;
    std::string line;
    std::stringstream ss(cat1);
    while (std::getline(ss, line)) {
      if (x >= 0 && x < w)
        mvprintw(curY++, x, "%s", line.c_str());
      else
        curY++;
    }

    refresh();
    usleep(100000); // 100ms

    // Check for space
    int ch = getch();
    if (ch == ' ') {
      nodelay(stdscr, FALSE); // Restore blocking
      return;
    }
  }

  // If animation finishes, wait for space
  nodelay(stdscr, FALSE);
  while (true) {
    int ch = getch();
    if (ch == ' ')
      return;
  }
}

void UI::showLevelUp(int level) {
  // Simple animation
  for (int i = 0; i < 5; ++i) {
    clear();
    attron(COLOR_PAIR(2));
    if (i % 2 == 0)
      drawCentered(10, "*** " + translate("level_up") + " ***");
    else
      drawCentered(10, "    " + translate("level_up") + "    ");
    attroff(COLOR_PAIR(2));
    drawCentered(12, translate("welcome_level") + " " + std::to_string(level));
    refresh();
    usleep(200000);
  }
}

void UI::drawGame(int score, int level, int challengesPassed,
                  const MathProblem &problem, float timeLeft, int width,
                  int height) {
  (void)height;
  clear();

  // Narrow UI Logic
  int gameWidth = 80;
  if (width < 80)
    gameWidth = width;
  int startX = (width - gameWidth) / 2;

  // HUD
  std::string scoreStr = translate("score") + ": " + std::to_string(score);
  std::string levelStr = translate("level") + ": " + std::to_string(level);
  std::string challengeStr =
      "Challenge: " + std::to_string((challengesPassed % 10) + 1) + "/10";

  mvprintw(1, startX + 2, "%s", scoreStr.c_str());

  int levelLen = utf8_length(levelStr);
  int levelX = startX + gameWidth - 2 - levelLen;
  if (levelX < startX + 2)
    levelX = startX + 2; // Safety clamp
  mvprintw(1, levelX, "%s", levelStr.c_str());

  // Draw challenge counter centered
  drawCenteredX(1, startX, gameWidth, challengeStr);

  // Progress Bar
  int barWidth = gameWidth - 4;
  int filledWidth = (int)(barWidth * timeLeft);

  mvprintw(2, startX + 2, "[");
  attron(COLOR_PAIR(2));
  if (timeLeft < 0.3f)
    attron(COLOR_PAIR(3));

  for (int i = 0; i < barWidth; ++i) {
    if (i < filledWidth)
      addch('=');
    else
      addch(' ');
  }
  attroff(COLOR_PAIR(2));
  attroff(COLOR_PAIR(3));
  mvprintw(2, startX + gameWidth - 2, "]");

  // Problem
  drawCenteredX(5, startX, gameWidth,
                translate("problem") + ": " + problem.question);

  // Lanes
  int laneWidth = gameWidth / 3;
  int lane1X = startX + laneWidth / 2;
  int lane2X = startX + gameWidth / 2;
  int lane3X = startX + gameWidth - laneWidth / 2;

  // Draw Options
  mvprintw(10, lane1X - 2, "%d", problem.options[0]); // Left
  mvprintw(10, lane2X - 2, "%d", problem.options[1]); // Up (Middle)
  mvprintw(10, lane3X - 2, "%d", problem.options[2]); // Right

  // Draw arrows
  mvprintw(12, lane1X - 2, "^");
  mvprintw(13, lane1X - 2, "|");
  mvprintw(14, lane1X - 4, "LEFT");

  mvprintw(12, lane2X - 2, "^");
  mvprintw(13, lane2X - 2, "|");
  mvprintw(14, lane2X - 3, "UP");

  mvprintw(12, lane3X - 2, "^");
  mvprintw(13, lane3X - 2, "|");
  mvprintw(14, lane3X - 4, "RIGHT");

  refresh();
}

int UI::getInput() { return getch(); }
