// Este projeto foi feito pelo alunos Ana Beatriz da Conceição, Karin Araldi e Lucas Vieira da Silva, da Universidade do Vale do Itajaí (UNIVALI), matéria de Algoritmos e Programação II com o professor Thiago Pereira Felski


#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>
using namespace std;

// 0 Empty
// 1 Strong Wall
// 2 Weak Wall
// 3 Bomberman
// 4 Enemies
// 5 Bomb
// 6 Explosion Area

bool is_place_empty(int pos, char entity_name) {
    if (pos == 0 ||pos == 4 && entity_name =='b' || pos == 5 && entity_name == 'p' || pos == 2 && entity_name == 'b' || pos == 6 && entity_name == 'b' || pos == 4 && entity_name == 'p') {
        return 1;
    }
    return 0;
}

struct Player {
    int x = 0, y = 0;
    bool alive = true;

    void show() {
        cout << char(2);
    }
    void move(char direction, int map[10][20]) {
        switch (direction) {
        case 's': ///Cima
            if (is_place_empty(map[y + 1][x], 'p')) {
                y++;
            }
            break;
        case 'w': ///baixo
            if (is_place_empty(map[y - 1][x], 'p')) {
                y--;
            }
            break;
        case 'a': ///esquerda
            if (is_place_empty(map[y][x - 1], 'p')) {
                x--;
            }
            break;
        case 'd': ///direita
            if (is_place_empty(map[y][x + 1], 'p')) {
                x++;
            }
            break;
        }
    }
};

struct Bomb {
    bool is_placed = false;
    bool is_animating = false;
    int x, y;
    clock_t start, end;
    int explode_cd_time = 3;

    void setup(int px, int py, int map[10][20]) {
        x = px;
        y = py;
        bomb_set_active();
        map[y][x] = 5;
    }

    void bomb_set_active() {
        start = clock();
        is_placed = true;
    }

    void modify_map(int map[10][20], int num) {
        if (is_place_empty(map[y - 1][x], 'b')) {
            map[y - 1][x] = num;
        }
        if (is_place_empty(map[y + 1][x], 'b')) {
            map[y + 1][x] = num;
        }
        if (is_place_empty(map[y][x - 1], 'b')) {
            map[y][x - 1] = num;
        }
        if (is_place_empty(map[y][x + 1], 'b')) {
            map[y][x + 1] = num;
        }
        map[y][x] = num;
    }

    void bomb_animate(int map[10][20]) {
        end = clock();
        if ((end - start) / CLOCKS_PER_SEC == 1) {
            modify_map(map, 0);
            is_animating = false;
        }
    }

    void kill_ent(int map[10][20], int e_x, int e_y) {
        map[e_y][e_x] = 0;
    }

    void bomb_check_coll(int map[10][20], Player& player) {
        if (is_place_empty(map[y][x - 1], 'b') && map[y][x - 1] != 1) {
            kill_ent(map, x - 1, y);
        }
        if (!is_place_empty(map[y][x + 1], 'b') && map[y][x + 1] != 1) {
            kill_ent(map, x + 1, y);
        }
        if (!is_place_empty(map[y - 1][x], 'b') && map[y - 1][x] != 1) {
            kill_ent(map, x, y - 1);
        }
        if (is_place_empty(map[y + 1][x], 'b') && map[y + 1][x] != 1) {
            kill_ent(map, x, y + 1);
        }
        if (is_place_empty(map[y][x], 'b')) {
            kill_ent(map, x, y);
        }
        if (player.x == x - 1 && player.y == y) {
            player.alive = false;
        }
        if (player.x == x + 1 && player.y == y) {
            player.alive = false;
        }
        if (player.x == x && player.y + 1 == y) {
            player.alive = false;
        }
        if (player.x == x && player.y == y - 1) {
            player.alive = false;
        }
    }

    void bomb_explode(int map[10][20], Player& player) {
        bomb_check_coll(map, player);
        modify_map(map, 6);
        is_animating = true;
    }

    void bomb_explode_controler(int map[10][20], Player& player) {
        end = clock();
        if ((end - start) / CLOCKS_PER_SEC == explode_cd_time) {
            is_placed = false;
            start = clock();
            bomb_explode(map, player);
        }
    }
};

void parar_cursor_piscar() {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO     cursorInfo;
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = false; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

void reposicionar_cursor_inicio() {
    short int CX = 0, CY = 0;
    COORD coord;
    coord.X = CX;
    coord.Y = CY;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setup_cursor() {
    parar_cursor_piscar();
    reposicionar_cursor_inicio();
}

void handle_input(Player& player, int map[10][20], Bomb& bomb) {
    char keyboard_input;
    if (_kbhit()) {
        keyboard_input = _getch();
        if (keyboard_input == 'w' || keyboard_input == 'a' || keyboard_input == 's' || keyboard_input == 'd') {
            player.move(keyboard_input, map);
        }
        else if (keyboard_input == 'b' && bomb.is_placed == false && bomb.is_animating == false) {
            bomb.setup(player.x, player.y, map);
        }
    }
}

void move_enemie(int map[10][20],int enemypos[3][2], Player& player, int pos) {
    int y = enemypos[pos][0];
    int x = enemypos[pos][1];
    int nx = 0;
    int ny = 0;
    int r = round(((rand() % 4) + 1));

    if (map[y - 1][x] != 1 && map[y - 1][x] != 2) {
        map[y][x] = 0;
        map[y - 1][x] = 4;
        nx = x;
        ny = y - 1;
    }
    else if (map[y][x + 1] != 1 && map[y][x + 1] != 2) {
        map[y][x] = 0;
        map[y][x + 1] = 4;
        nx = x + 1;
        ny = y;
    }
    else if (map[y][x - 1] != 1 && map[y][x - 1] != 2) {
        map[y][x] = 0;
        map[y][x - 1] = 4;
        nx = x - 1;
        ny = y;
    }
    else if (map[y + 1][x] != 1 && map[y + 1][x] != 2) {
        map[y][x] = 0;
        map[y + 1][x] = 4;
        nx = x;
        ny = y + 1;
    }
        
    if (nx == player.x && ny == player.y) {
        player.alive = false;
    }
    enemypos[pos][0] = ny;
    enemypos[pos][1] = nx;
}
void att_map(int map[10][20], Player& player) {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 20; x++) {
            if (map[y][x] == 1) {
                cout << char(219);
            }
            else if (map[y][x] == 2) {
                cout << char(177);
            }
            else if (y == player.y && x == player.x) {
                player.show();
            }
            else if (map[y][x] == 5) {
                cout << char(190);
            }
            else if (map[y][x] == 6) {
                cout << char(216);
            }
            else if (map[y][x] == 4) {
                cout << char(206);
            }
            else {
                cout << " ";
            }
        }
        cout << "\n";
    }
}
void enemies_controler(int map[10][20], int enemies[3][2], Player& player) {
    for (int i = 0; i < 3; i++) {
        move_enemie(map, enemies, player, i);
    }
}
bool check_end_game(int map[10][20], Player& player) {
    bool is_end = player.alive;
    int num_of_enem = 0;
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 20; x++) {
            if (map[y][x] == 4) {
                num_of_enem++;
                if (player.x == x && player.y == y) {
                    is_end = false;
                }
            }
        }
    }
    if (num_of_enem == 0) {
        is_end = false;
    }
    return is_end;
}

int main() {
    Player player;
    player.x = 1;
    player.y = 1;
    clock_t start, end;
    bool time = false;
    Bomb bomb;
    int map[10][20] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                        1, 3, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1,
                        1, 2, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1,
                        1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 4, 0, 0, 0, 2, 0, 1,
                        1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 2, 1, 1, 1, 0, 2, 0, 1,
                        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 2, 1, 0, 1,
                        1, 0, 2, 2, 4, 0, 2, 2, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1,
                        1, 0, 2, 0, 0, 1, 0, 2, 0, 1, 0, 0, 0, 0, 0, 1, 0, 2, 0, 1,
                        1, 0, 0, 0, 0, 1, 0, 0, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    start = clock();
    int enemies[3][2] = { 6, 4,
                         8, 8,
                         3, 13 };
        while (check_end_game(map, player)) {
            end = clock();
            if ((end - start) / CLOCKS_PER_SEC == 3) {
                enemies_controler(map, enemies, player);
                start = clock();
            }
            if (bomb.is_placed == true) {
                bomb.bomb_explode_controler(map, player);
            }
            else if (bomb.is_animating == true) {
                bomb.bomb_animate(map);
            }
            setup_cursor();
            handle_input(player, map, bomb);
            att_map(map, player);
            cout << "X[" << player.x << "] ";
            cout << "Y[" << player.y << "]";
            cout << player.alive;

        }
}


//char(219) = quadrado
// mapa
// parede fixa = cinza = char(219)
// parede fragil = azul = char(177)
// bomba = preto
// area da bomba = laranja
// inimigo = char(234)
// personagem = char(2)
// movimento do inimigo e do personagem (com somente o personagem podendo passar por cima de bombas
// inimigo morre ao tocar em bomba ou estar na área da explosão
// personagem morre ao tocar em um inimigo ou estar na area de explosão
