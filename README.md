# Unlimited Math

Unlimited Math is a terminal-based arithmetic game designed to challenge your mental math skills with progressively difficult problems. It features multiple difficulty levels, a time-attack mechanic that speeds up as you progress, and support for over a dozen languages. The game runs directly in your console, offering a distraction-free environment to practice addition, subtraction, multiplication, and more.

## Features

*   **Progressive Difficulty:** Problems get harder as you level up, with specific constraints to ensure a challenge (e.g., no single-digit additions in later levels).
*   **Time Attack:** The time limit decreases with each level, demanding faster reflexes and calculation speed.
*   **Multiple Languages:** Support for English, German, French, Spanish, Italian, Portuguese, Dutch, Ukrainian, Polish, Chinese, Japanese, and Korean.
*   **Visual Polish:** Enjoy ASCII art animations for level completion and game over screens.
*   **Scoreboard:** Track your score and current challenge progress directly on the HUD.

## Prerequisites

*   A C++ compiler (GCC/Clang) supporting C++17.
*   `make` build system.
*   `ncurses` library.

### Installing ncurses

*   **macOS:** `brew install ncurses` (usually pre-installed)
*   **Ubuntu/Debian:** `sudo apt-get install libncurses5-dev`
*   **Fedora:** `sudo dnf install ncurses-devel`

## How to Build and Run

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/volyanyk/unlimitedmath.git
    cd unlimitedmath
    ```

2.  **Build the game:**
    ```bash
    make
    ```

3.  **Run the game:**
    ```bash
    ./unlimitedmath
    ```

4.  **Run Tests (Optional):**
    ```bash
    make test
    ```

## Controls

*   **Arrow Keys:**
    *   `Left`: Select the left answer option.
    *   `Up`: Select the middle answer option.
    *   `Right`: Select the right answer option.
*   **Menu Navigation:** Use Up/Down arrows to select options, Left/Right to change settings, and Enter to confirm.
*   **Space:** Press Space to continue after completing a level or on the Game Over screen.
*   **Q:** Quit the game (during gameplay).

## License

This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.
