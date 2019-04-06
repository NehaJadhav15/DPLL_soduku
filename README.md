# DPLL_soduku

solve SAT problem using DPLL. Project of HUST-CS by why.

## Usage 

- Running env: `linix 4.14, gcc 8.2.1`
- running:

    ```shell
    $ gcc main.c
    $ ./a.out
    ```
- Moving in menu: press `wsad` as `↑↓←→`
- press `enter` or `d` to choose one item.
- **SAT Solver:**
  - moving `.cnf` file to `./data` and rename them to `n.cnf`, `n` should be number from `0~9`.
  - Start solver, enter the name of cnf file.
  - waiting for solving!
- **Sudoku game:**
  - During sudoku game, press `1~9` to fill cell, press `0` to clear cell, and press `q` to quit.
  - press `ws` to flush the terminal.
  - enjoy!