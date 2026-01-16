# PictoCraft

PictoCraft is a logic-based puzzle game inspired by classic **Picross / Nonogram** games, combined with a visual style heavily influenced by **Minecraft**.  
The goal of the game is to uncover a hidden pixel-art image by correctly filling a grid of square cells using numerical clues placed along the rows and columns.

The game focuses entirely on **logic, deduction, and patience**. There is no randomness involved in solving a puzzle — every solution can be reached through careful reasoning.

---

## Core Gameplay

Each puzzle consists of a grid where:

- Every **row and column** contains a set of numerical clues.
- Each number represents a group of **consecutive filled cells**.
- When multiple numbers are present on the same row or column, each group must be separated by **at least one empty cell**.
- The player must determine which cells should be filled and which should remain empty based on logic alone.

A puzzle is considered complete only when the entire grid is filled correctly without contradictions.

---

## Main Menu and Game Flow

When starting PictoCraft, the player is greeted by a main menu where different gameplay options are available.

### Available Options

- **Load an existing puzzle**  
  The player can manually select a puzzle image from available files and attempt to solve it.

- **Generate a random puzzle**  
  The game automatically generates a unique puzzle each time, following logical construction rules to ensure it is always solvable.

This allows both curated experiences and infinite replayability through procedural generation.

---

## Difficulty Modes

### Creative Mode

Creative Mode is designed for a relaxed experience.

- No limit on mistakes
- No pressure or penalties
- Ideal for learning the mechanics or solving puzzles at your own pace

---

### Survival Mode

Survival Mode introduces consequences for incorrect moves.

- The player is allowed a maximum of **three mistakes**
- After the third mistake, the game ends
- Designed for players looking for a more traditional challenge

---

## Special Game Modes

To keep gameplay fresh and varied, PictoCraft includes several special modes that introduce unique mechanics and challenges.

### Time Mode

In Time Mode, the player must complete the puzzle within a fixed time limit.  
This mode tests both logical thinking and speed under pressure.

![Time Mode](https://github.com/user-attachments/assets/77e2087e-4245-4f7d-bff1-8de4ec4e2362)

---

### Torch Mode

Torch Mode significantly limits visibility.

Only a circular area around the cursor is visible, simulating the effect of holding a torch in darkness.  
Players must rely on memory and careful movement to solve the puzzle.

![Torch Mode](https://github.com/user-attachments/assets/dc1fe658-ada8-4224-9b91-918c3a8596b0)

---

### Spiders Mode

In this mode, spiders periodically appear on the screen and cover the numerical clues with cobwebs.

- Clues may become temporarily unreadable
- The player must remove spiders and clear the webbing to regain visibility
- Adds distraction and time pressure to otherwise calm puzzles

![Spiders Mode](https://github.com/user-attachments/assets/ba611163-ae8c-4ced-b5a2-b4662ac82efe)

---

### DiscoFever Mode

DiscoFever Mode introduces constant color changes to the blocks.

- Cell colors shift continuously
- The puzzle remains logically the same, but visual clarity is reduced
- Designed to overwhelm the senses while maintaining core logic

![DiscoFever Mode](https://github.com/user-attachments/assets/9522d8ce-47a0-46d7-b648-b78363f720ed)

---

### Enderman Mode

An Enderman roams the screen while the player is solving the puzzle.

- If the player keeps the cursor over the Enderman for too long, it becomes aggressive
- Triggering this results in a jumpscare
- Encourages awareness of cursor movement and adds tension

![Enderman Mode](https://github.com/user-attachments/assets/eb726e93-1b62-48e1-a81c-f7fe783bad15)

---

### Alchemy Mode

Alchemy Mode introduces potion effects that influence gameplay.

- Effects can be positive or negative
- Potions may alter visibility, controls, or player perception
- Creates unpredictable situations that force players to adapt

![Alchemy Mode](https://github.com/user-attachments/assets/129b44dc-848d-47da-96f1-43f3f0f0bbb0)

---

## Replayability

By combining traditional nonogram logic with multiple difficulty settings and special modes, PictoCraft offers:

- Different experiences for the same puzzle
- Increased challenge for advanced players
- Strong replay value through randomness and modifiers

Each mode changes how the player interacts with the same core mechanics.

---

## Assets and Credits

PictoCraft uses assets taken directly from **Minecraft**:

- **Visual assets (blocks, UI):** taken from the original *Minecraft* game files
- **Mob animations:** based on the texture pack  
  [Squishy’s Reanimated Mobs](https://www.curseforge.com/minecraft/texture-packs/squishys-reanimated-mobs)
- **Font:**  
  [Monocraft](https://github.com/IdreesInc/Monocraft) by IdreesInc

---

## Disclaimer

PictoCraft is an independent project created for educational and non-commercial purposes.  
Minecraft is a registered trademark of Mojang Studios. This project is not affiliated with or endorsed by Mojang.
