// GNU Tatermark 13.3

// Only to GNU/Linux Arch linux

// Progrmmer Daniil Dorofeev

// 1999 - 2025 Kolhoz
// 2025 include digital

// Welcome to GNU Products

// Help: @dear_finn - telegram

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale.h>
#include <ncurses.h>
#include <algorithm>
#include <sstream>

std::vector<std::string> text_lines;
int cursor_y = 0;
int cursor_x = 0;
std::string filename = "new_file.txt";
std::string status_message = "";
bool is_dirty = false;
std::string clipboard = "";
int start_line = 0;
int start_col = 0;


void init_editor() {
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_WHITE, COLOR_BLUE);
        init_pair(2, COLOR_BLACK, COLOR_WHITE);
    }
}

void cleanup_editor() {
    endwin();
}


void draw_header() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    

    attron(COLOR_PAIR(1));
    for (int i = 0; i < max_x; ++i) {
        mvaddch(0, i, ' ');
    }
    

    std::string title = (filename.length() > 20 ? filename.substr(0, 17) + "..." : filename);
    mvprintw(0, (max_x - title.length()) / 2, "%s", title.c_str());
    

    mvprintw(0, 0, "GNU tCat 13.3");

    attroff(COLOR_PAIR(1));
}

void draw_statusbar() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    attron(A_BOLD);
    mvprintw(max_y - 3, 0, "%.*s", max_x, status_message.c_str());
    attroff(A_BOLD);
}

void draw_footer() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    attron(COLOR_PAIR(2));
    for (int i = 0; i < max_x; ++i) {
        mvaddch(max_y - 2, i, ' ');
        mvaddch(max_y - 1, i, ' ');
    }
    
    mvprintw(max_y - 2, 0, "^G Справка  ^O Записать  ^W Поиск    ^K Вырезать  ^T Выполнить");
    mvprintw(max_y - 1, 0, "^X Выход    ^R ЧитФайл   ^\\ Замена   ^U Вставить  ^J Выровнять");
    
    attroff(COLOR_PAIR(2));
}

void draw_all() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    clear();
    int visible_rows = max_y - 4;
    int visible_cols = max_x;

    for (int i = 0; i < visible_rows; ++i) {
        if (start_line + i < text_lines.size()) {
            std::string line_to_draw = text_lines[start_line + i];
            if (line_to_draw.length() > start_col) {
                line_to_draw = line_to_draw.substr(start_col);
            } else {
                line_to_draw = "";
            }
            if (line_to_draw.length() > visible_cols) {
                line_to_draw = line_to_draw.substr(0, visible_cols);
            }
            mvprintw(i + 1, 0, "%s", line_to_draw.c_str());
        }
    }
    draw_header();
    draw_statusbar();
    draw_footer();
    move(cursor_y - start_line + 1, cursor_x - start_col);
    refresh();
}


void draw_welcome_screen() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    clear();
    
    std::string welcome = "Welcome to GNU tatermark 13.3!";
    std::string start_msg = "Нажмите любую клавишу для начала работы...";
    
    mvprintw(max_y/2 - 2, (max_x - welcome.length())/2, "%s", welcome.c_str());
    mvprintw(max_y/2, (max_x - start_msg.length())/2, "%s", start_msg.c_str());
    refresh();
    getch();
    clear();
}


void load_file(const std::string& fname) {
    filename = fname;
    std::ifstream file(filename);
    if (file.is_open()) {
        text_lines.clear();
        std::string line;
        while (getline(file, line)) {
            text_lines.push_back(line);
        }
        if (text_lines.empty()) {
            text_lines.push_back("");
        }
        file.close();
        status_message = "[ Прочитано " + std::to_string(text_lines.size()) + " строк ]";
        is_dirty = false;
        start_line = 0;
    } else {
        text_lines.clear();
        text_lines.push_back("");
        status_message = "Ошибка: не удалось открыть файл! Создан новый файл.";
        is_dirty = false;
        start_line = 0;
    }
}

void save_file() {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& line : text_lines) {
            file << line << '\n';
        }
        file.close();
        status_message = "Файл сохранён успешно!";
        is_dirty = false;
    } else {
        status_message = "Ошибка: не удалось сохранить файл!";
    }
}


std::string get_input_from_statusbar(const std::string& prompt) {
    std::string input = "";
    int ch;
    int max_y, max_x;
    int input_cursor = 0;
    
    while (true) {
        getmaxyx(stdscr, max_y, max_x);
        status_message = prompt + input;
        draw_all();
        move(max_y - 3, prompt.length() + input_cursor);
        refresh();
        
        ch = getch();
        if (ch == 10) { 
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (input_cursor > 0) {
                input.pop_back();
                input_cursor--;
            }
        } else if (ch >= 32 && ch <= 255) {
            input += (char)ch;
            input_cursor++;
        }
    }
    return input;
}



bool confirm_exit() {
    if (!is_dirty) return true;
    status_message = "Файл имеет несохранённые изменения! Сохранить? (y/n)";
    draw_all();
    int ch = getch();
    if (ch == 'y' || ch == 'Y') {
        save_file();
        return true;
    } else if (ch == 'n' || ch == 'N') {
        return true;
    } else {
        status_message = "Отмена.";
        return false;
    }
}

void handle_find() {
    std::string query = get_input_from_statusbar("Поиск: ");
    if (query.empty()) {
        status_message = "";
        return;
    }
    bool found = false;
    for (size_t i = 0; i < text_lines.size(); ++i) {
        size_t pos = text_lines[i].find(query);
        if (pos != std::string::npos) {
            cursor_y = i;
            cursor_x = pos;
            found = true;
            break;
        }
    }
    if (found) {
        status_message = "Найдено!";
    } else {
        status_message = "Не найдено.";
    }
}

void handle_goto_line() {
    std::string line_str = get_input_from_statusbar("Перейти к строке: ");
    if (line_str.empty()) {
        status_message = "";
        return;
    }
    int line_num = atoi(line_str.c_str());
    if (line_num > 0 && line_num <= text_lines.size()) {
        cursor_y = line_num - 1;
        cursor_x = 0;
        status_message = "Переход к строке " + std::to_string(line_num);
    } else {
        status_message = "Неверный номер строки.";
    }
}

void handle_help() {
    clear();
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    int left_margin = (max_x - 80) / 2;
    if (left_margin < 0) left_margin = 0;

    mvprintw(1, left_margin, "GNU Tatermark 13.3 - Помощь по горячим клавишам:");
    mvprintw(3, left_margin, "^G - Справка");
    mvprintw(4, left_margin, "^X - Выход (с подтверждением сохранения)");
    mvprintw(5, left_margin, "^O - Сохранить файл");
    mvprintw(6, left_margin, "^F - Поиск текста");
    mvprintw(7, left_margin, "^K - Вырезать текущую строку");
    mvprintw(8, left_margin, "^U - Вставить строку из буфера");
    mvprintw(9, left_margin, "^R - Вставить содержимое другого файла");
    mvprintw(10, left_margin, "^\\ - Заменить текст");
    mvprintw(11, left_margin, "^C - Показать текущую позицию курсора");
    mvprintw(12, left_margin, "^/ - Перейти к строке");
    mvprintw(14, left_margin, "Нажмите любую клавишу для возврата в редактор...");
    refresh();
    getch();
    status_message = "";
}

void handle_read_file() {
    std::string read_file_name = get_input_from_statusbar("Имя файла для чтения: ");
    if (read_file_name.empty()) {
        status_message = "";
        return;
    }
    std::ifstream file(read_file_name);
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            text_lines.insert(text_lines.begin() + cursor_y + 1, line);
            cursor_y++;
        }
        file.close();
        is_dirty = true;
        status_message = "Файл " + read_file_name + " прочитан.";
    } else {
        status_message = "Ошибка: не удалось прочитать файл " + read_file_name;
    }
}

void handle_replace() {
    std::string search_str = get_input_from_statusbar("Заменить: ");
    if (search_str.empty()) {
        status_message = "";
        return;
    }

    std::string replace_str = get_input_from_statusbar("На что заменить: ");
    bool replaced = false;
    for (auto& line : text_lines) {
        size_t pos = 0;
        while((pos = line.find(search_str, pos)) != std::string::npos) {
            line.replace(pos, search_str.length(), replace_str);
            pos += replace_str.length();
            replaced = true;
        }
    }
    if (replaced) {
        status_message = "Замена выполнена!";
        is_dirty = true;
    } else {
        status_message = "Ничего не найдено для замены.";
    }
}


int main(int argc, char** argv) {
    init_editor();
    
    if (argc > 1) {
        load_file(argv[1]);
    } else {
        draw_welcome_screen();
        text_lines.push_back("");
        status_message = "[ Новый файл ]";
    }

    int ch;
    while (true) {
        draw_all();
        ch = getch();
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        int visible_rows = max_y - 4;

        if (ch == 24) { 
            if (confirm_exit()) break;
        } else if (ch == 15) { 
            save_file();
        } else if (ch == 6) { 
            handle_find();
        } else if (ch == 11) { 
            if (cursor_y < text_lines.size()) {
                clipboard = text_lines[cursor_y];
                text_lines.erase(text_lines.begin() + cursor_y);
                if (text_lines.empty()) text_lines.push_back("");
                if (cursor_y >= text_lines.size()) cursor_y = text_lines.size() - 1;
                cursor_x = 0;
                is_dirty = true;
                status_message = "Строка вырезана.";
            }
        } else if (ch == 21) { 
            if (!clipboard.empty()) {
                text_lines.insert(text_lines.begin() + cursor_y + 1, clipboard);
                cursor_y++;
                cursor_x = 0;
                is_dirty = true;
                status_message = "Вставлено из буфера.";
            }
        } else if (ch == 7) { 
            handle_help();
        } else if (ch == 18) { 
            handle_read_file();
        } else if (ch == 28) { 
            handle_replace();
        } else if (ch == 10) { 
            status_message = "Выравнивание пока не реализовано.";
        } else if (ch == 3) { 
            status_message = "Строка: " + std::to_string(cursor_y + 1) + " | Столбец: " + std::to_string(cursor_x + 1);
        } else if (ch == 16) { 
            handle_goto_line();
        } else if (ch == KEY_RESIZE) {

            resize_term(0, 0); 
            status_message = "Размер окна изменён.";
        }
        
        switch (ch) {
            case KEY_UP:
                if (cursor_y > 0) cursor_y--;
                if (cursor_x > text_lines[cursor_y].length()) cursor_x = text_lines[cursor_y].length();
                break;
            case KEY_DOWN:
                if (cursor_y < text_lines.size() - 1) cursor_y++;
                if (cursor_x > text_lines[cursor_y].length()) cursor_x = text_lines[cursor_y].length();
                break;
            case KEY_LEFT:
                if (cursor_x > 0) cursor_x--;
                break;
            case KEY_RIGHT:
                if (cursor_x < text_lines[cursor_y].length()) cursor_x++;
                break;
            case KEY_BACKSPACE:
                if (cursor_x > 0) {
                    text_lines[cursor_y].erase(cursor_x - 1, 1);
                    cursor_x--;
                    is_dirty = true;
                } else if (cursor_y > 0) {
                    std::string current_line = text_lines[cursor_y];
                    text_lines.erase(text_lines.begin() + cursor_y);
                    cursor_y--;
                    cursor_x = text_lines[cursor_y].length();
                    text_lines[cursor_y] += current_line;
                    is_dirty = true;
                }
                status_message = "";
                break;
            case 10: 
                {
                    std::string current_line = text_lines[cursor_y];
                    std::string new_line = current_line.substr(cursor_x);
                    text_lines[cursor_y].erase(cursor_x);
                    text_lines.insert(text_lines.begin() + cursor_y + 1, new_line);
                    cursor_y++;
                    cursor_x = 0;
                    is_dirty = true;
                }
                status_message = "";
                break;
            default:
                if (ch >= 32 && ch <= 255) { 
                    text_lines[cursor_y].insert(cursor_x, 1, ch);
                    cursor_x++;
                    is_dirty = true;
                }
                status_message = "";
                break;
        }

        if (cursor_y < start_line) start_line = cursor_y;
        int max_y_cur, max_x_cur;
        getmaxyx(stdscr, max_y_cur, max_x_cur);
        int visible_rows_cur = max_y_cur - 4;
        if (cursor_y >= start_line + visible_rows_cur) start_line = cursor_y - visible_rows_cur + 1;
        if (cursor_x < start_col) start_col = cursor_x;
        if (cursor_x >= start_col + max_x_cur) start_col = cursor_x - max_x_cur + 1;
    }

    cleanup_editor();
    return 0;
}
