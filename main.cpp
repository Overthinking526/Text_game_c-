#include <iostream>
#include <string>
#define NOMINMAX
#include <windows.h>
#include <limits>
#include <conio.h>

// ===== Умные паузы =====
// Время паузы зависит от количества выведенного текста: чем больше текста,
// тем дольше ждём. Нажмите Q (английская раскладка), чтобы пропустить паузу.
// Можно держать Q зажатой, чтобы листать быстро.
const double TIME_SCALE = 1.0;          // множитель исходных пауз (1.0 = как в коде)
const int READ_BASE_MS = 14000;          // минимум на любую реплику
const int READ_MS_PER_CHAR = 250;        // мс на один символ текста (больше = медленнее)
const int READ_MAX_MS = 90000;          // потолок одной паузы
long long g_chars_printed = 0;          // сколько символов выведено с прошлой паузы

// Считает символы, выводимые в cout (UTF-8: кириллица = 1 символ)
class CountBuf : public std::streambuf {
    std::streambuf* out;
public:
    explicit CountBuf(std::streambuf* o) : out(o) {}
protected:
    int_type overflow(int_type c) override {
        if (c != traits_type::eof()) {
            unsigned char u = static_cast<unsigned char>(c);
            if ((u & 0xC0) != 0x80 && u != '\n') g_chars_printed++;
        }
        return out->sputc(static_cast<char>(c));
    }
    std::streamsize xsputn(const char* s, std::streamsize n) override {
        for (std::streamsize i = 0; i < n; i++) {
            unsigned char u = static_cast<unsigned char>(s[i]);
            if ((u & 0xC0) != 0x80 && u != '\n') g_chars_printed++;
        }
        return out->sputn(s, n);
    }
    int sync() override { return out->pubsync(); }
};

inline void clear_screen()
{
    system("cls");
    g_chars_printed = 0;
}

inline void wait_ms(int ms)
{
    long long chars = g_chars_printed;
    g_chars_printed = 0;

    double total = ms * TIME_SCALE;
    if (chars > 8) {                     // короткие паузы (точки, анимации) не растягиваем
        double need = READ_BASE_MS + chars * READ_MS_PER_CHAR;
        if (need > READ_MAX_MS) need = READ_MAX_MS;
        if (need > total) total = need;
    }

    for (int waited = 0; waited < total; waited += 50) {
        bool skip = false;
        while (_kbhit()) {
            int ch = _getch();
            if (ch == 'q' || ch == 'Q') skip = true;
        }
        if (skip) return;
        Sleep(50);
    }
}


int move_counter = 0; // счётчик ходов

int choice_intro = 0;       // Первый выбор: реакция на предложение Марка
int choice_nox_entry = 0;    // Второй выбор: способ проникновения в Нокс-7
int choice_solo_entry = 0; // Выбор способа проникновения в одиночку
int choice_hack_mark = 0; // Выбор игрока: "Взломать систему безопасности" вместе с Марком
int choice_camera_mark = 0; // Выбор игрока: "Осмотреть камеру наблюдения" вместе с Марком
int choice_memory_card = 0; // Выбор игрока: "Извлечь карту памяти" вместе с Марком
int choice_backup_power = 0; // Выбор игрока: "Проверить резервное питание" вместе с Марком
int choice_door_mark = 0; // Выбор игрока: "Попытаться выбить дверь" вместе с Марком
int choice_break_door = 0; // Выбор игрока: "Войти в коридор вместе с Марком" / "Осмотреть надпись на стене" / "Проверить панель доступа"
int choice_emergency = 0; // Выбор игрока: функция аварийного рычага / отключение питания механизма / отойти от двери и поговорить с Марком
int choice_solo_hack = 0; // Выбор игрока: игрок сам пытается взломать терминал
int choice_identifier = 0; // Выбор игрока: Подобрать идентификатор.
int choice_bypass = 0; // Выбор игрока: Обойти защиту вручную.
int choice_event_log = 0; //выбор игрокa: Открыть журнал событий.
int choice_solo_camera = 0; // выбор игрока: "самому без марка осмотреть камеру"
int choice_ch04_mark = 0; //выбор игрокa: Взломать систему безопасности.
int choice_ch04_outer = 0;
int choice_ch04_force_archive = 0;
int choice_ch04_distract = 0;
int choice_ch04_disable_lockdown = 0;
int choice_ch04_lockdown_action = 0;
int choice_ch04_lockdown_panel = 0;
int choice_ch04_lockdown_mark = 0;
int choice_ch04_lockdown_final = 0;
int choice_ch04_backup_power = 0;
int choice_ch04_emergency_power = 0;
int choice_ch04_power_source = 0;
int choice_ch04_follow_cable = 0;
int choice_ch04_cable_panel = 0;
int choice_ch04_cable_voice = 0;
int choice_ch04_cable_route = 0;
int choice_ch04_cable_final = 0;
int choice_ch04_leave_panel = 0;
int choice_blind_spot = 0;
int choice_ch04_enter_technical_passage = 0;
int choice_ch04_inspect_footprints_cables = 0;
int choice_ch04_close_panel_return_door = 0;
int choice_ch04_inspect_wall_inscription = 0;
int choice_ch04_pull_emergency_lever = 0;
int choice_ch04_emergency_final_action = 0;
int choice_ch04_emergency_mark = 0;
int choice_ch04_emergency_memory = 0;
int choice_ch04_disable_mechanism_power = 0;
int choice_ch04_power_final_action = 0;
int choice_ch04_power_terminal_action = 0;
int choice_ch04_talk_to_mark_first = 0;
int choice_ch04_talk_to_mark_truth = 0;
int choice_ch04_talk_to_mark_final = 0;

int choice_ch04_corridor_find_exit = 0;

int choice_leave_mark = 0;

bool restart_requested = false; // Перезапуск истории после ветки «остаться дома»

void start_game();

class Character {
public:
    int age;

    int trust;                                 // доверие
    int friendship;                            // дружба
    std::string attitude_toward_a_character;   // отношение к персонажу
    std::string name_of_character;             // имя персонажа
    std::string gender;                        // пол персонажа
    std::string information_of_character;      // информация о персонаже
    bool alive = true;                         // жив ли персонаж
    bool has_key = false;                      // есть ли у персонажа ключ
    int fear = 0;                              // уровень страха
    int evidence = 0;                          // Количество найденных улик
    int humanity = 0;                          // Насколько герой ценит человеческие жизни
};

// Функция конкретно для Леи Морроу
void create_lea(Character& character) {
    character.name_of_character = "Лея Морроу";
    character.gender = "Девушка";
    character.age = 22;
    character.trust = 20;
    character.friendship = 0;
    character.attitude_toward_a_character = "Нейтрально";
    character.information_of_character = "===Таинственная девушка=== Спокойная, умная, скрытная. Знает больше, чем рассказывает. Иногда её воспоминания противоречат друг другу.";
    character.alive = true;
    character.has_key = false;
    character.fear = 0;
    character.evidence = 0;
    character.humanity = 0;
}

// Функция конкретно для Марка Вебера
void create_mark(Character& character) {
    character.name_of_character = "Марк Вебер";
    character.gender = "Парень";
    character.age = 24;
    character.trust = 80;
    character.friendship = 60;
    character.attitude_toward_a_character = "Друг";
    character.information_of_character = "===Друг главного героя=== Любит электронику и программирование. Помогает главному герою взламывать системы комплекса, но панически боится того, что там находится.";
    character.alive = true;
    character.has_key = false;
    character.fear = 0;
    character.evidence = 0;
    character.humanity = 0;
}

// Функция конкретно для Доктора Элиаса
void create_elias(Character& character) {
    character.name_of_character = "Доктор Элиас";
    character.gender = "Парень";
    character.age = 32;
    character.trust = 0;
    character.friendship = 0;
    character.attitude_toward_a_character = "Неизвестно";
    character.information_of_character = "===Главный антагонист?=== Бывший руководитель проекта «Эхо». Утверждает, что пытается спасти тысячи людей. Возможно, он говорит правду.";
    character.alive = true;
    character.has_key = false;
    character.fear = 0;
    character.evidence = 0;
    character.humanity = 0;
}

// Функция для установки базовых параметров игрока (кроме имени/возраста, которые вводит пользователь)
void init_player(Character& character) {
    character.trust = 0;
    character.friendship = 0;
    character.alive = true;
    character.has_key = false;
    character.fear = 0;
    character.evidence = 0;
    character.humanity = 0;
    character.attitude_toward_a_character = "Сам к себе";
    character.information_of_character = "Главный герой истории.";
}

// Создание персонажей глобально
Character lea_moppoy;
Character mark_veber;
Character doktor_elias;
Character player;

// ===== Вспомогательные функции =====
// Окончание по полу героя: G("решил", "решила")
std::string G(const std::string& male, const std::string& female)
{
    return player.gender == "Девушка" ? female : male;
}

// Сбросить символы, которые игрок нажал во время пауз wait_ms()
void flush_input()
{
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}

// Безопасный ввод числа в диапазоне [lo; hi]
int ask_choice(int lo, int hi)
{
    int c = 0;
    while (true) {
        std::cout << "> ";
        if (std::cin >> c && c >= lo && c <= hi) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return c;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Некорректный выбор. Введите число от " << lo << " до " << hi << ".\n";
    }
}

void obtaining_a_info()
{
    std::cout << "Введите имя главного героя: ";
    std::cin >> std::ws;
    std::getline(std::cin, player.name_of_character);   // имя может состоять из нескольких слов
    if (player.name_of_character.empty()) {
        player.name_of_character = "Герой";
    }
    clear_screen();

    std::cout << "Выберите пол главного героя:\n";
    std::cout << "1 — Парень\n";
    std::cout << "2 — Девушка\n";
    player.gender = (ask_choice(1, 2) == 2) ? "Девушка" : "Парень";
    std::cout << "Пол установлен как: " << player.gender << std::endl;
    clear_screen();

    std::cout << "Введите возраст (от 14 до 100):\n";
    player.age = ask_choice(14, 100);
    std::cout << "Возраст установлен как: " << player.age << std::endl;
    clear_screen();
}

void backstory()
{
    std::cout << "Главный герой — " << player.name_of_character << ", " << player.age << "-летний студент, который живёт в городе Нокс. Уже несколько недель каждую ночь в 03:17 его компьютер самостоятельно включается и показывает одну и ту же фразу:\n";
    std::cout << "«Если ты читаешь это сообщение, значит, я ещё не умерла».\n";
    std::cout << "Однажды ночью " << player.name_of_character << " получает координаты заброшенного исследовательского комплекса. Сообщение отправлено девушкой по имени Лея, которая, согласно городским новостям, пропала семь лет назад.\n";
    wait_ms(80000); // Немного сократил время ожидания для удобства тестов, можете вернуть 20000
}

//------------------------------1глава----------------------------------

// первая глава игры, в которой игрок получает первое сообщение от Леи Морроу и звонок от Марка Вебера. В зависимости от выбора игрока, будут развиваться разные сюжетные линии.
void show_ch01_intro() {
    clear_screen();
    std::cout << "===Глава 1. Сообщение в 03:17===\n";
    wait_ms(8000);

    clear_screen();
    std::cout << "Локация: комната " << player.name_of_character << ". Время: 03:17. \n";
    wait_ms(8000);

    std::cout << "Дождь барабанит по стеклу. В комнате темно. Монитор компьютера внезапно загорается. \n";
    wait_ms(10000);

    clear_screen();
    std::cout << "[Компьютер] \n";
    std::cout << "ВХОДЯЩЕЕ СООБЩЕНИЕ. \n" << "Отправитель: L.MORROW \n" << "«" << player.name_of_character << ". Не доверяй доктору Элиасу. Он знает, что ты придёшь». \n";
    wait_ms(10000);

    std::cout << player.name_of_character << " — Что за?.. Я даже не знаю никакой Леи Морроу. \n";
    wait_ms(10000);

    clear_screen();
    std::cout << "На экране появляется новое сообщение. \n";
    wait_ms(6000);

    std::cout << "[Лея] — У тебя осталось меньше двух часов. \n";
    wait_ms(10000);

    std::cout << player.name_of_character << " — Кто ты? Откуда ты знаешь моё имя? \n";
    wait_ms(8000);

    std::cout << "[Лея] — Я знаю, что ты собираешься сделать. Потому что семь лет назад ты уже " << G("сделал", "сделала") << " это. \n";
    wait_ms(12000);

    clear_screen();
    std::cout << "Соединение обрывается.";
    wait_ms(10000);

    std::cout << ".";
    wait_ms(800);
    std::cout << ".";
    wait_ms(800);
    std::cout << ".";
    wait_ms(800);
    std::cout << ".";
    wait_ms(800);
    std::cout << ".\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Телефон вибрирует. Звонит Марк.";
    wait_ms(10000);

    clear_screen();
    std::cout << "Марк: Привет, " << player.name_of_character << " — Слушай, у меня странная просьба. Ты ведь разбираешься в компьютерах? Мне прислали файл с твоим именем. Внутри карта заброшенного комплекса на окраине города.\n";
    wait_ms(14000);

    std::cout << "" << player.name_of_character << " — Мне тоже кое-что прислали.\n";
    wait_ms(10000);

    std::cout << "Марк: — Тогда не ходи туда " << G("один", "одна") << ". Я серьёзно.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор: \n";
    std::cout << "============================================================== \n";
    std::cout << "Хорошо. Встретимся через двадцать минут.              выбрать 1 \n";
    std::cout << "Я " << G("сам", "сама") << " разберусь. Не вмешивайся.                       выбрать 2 \n";
    std::cout << "Я никуда не собираюсь.                                выбрать 3 \n";
    std::cout << "============================================================== \n";

    do {
        if (!(std::cin >> choice_intro)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice_intro = 0;
        }
        if (choice_intro < 1 || choice_intro > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_intro < 1 || choice_intro > 3);

    move_counter = 1;
    if (choice_intro == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Хорошо. Встретимся через двадцать минут.";
        mark_veber.friendship += 10;
        wait_ms(10000);
    }
    else if (choice_intro == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Я " << G("сам", "сама") << " разберусь. Не вмешивайся.";
        mark_veber.friendship -= 15; mark_veber.fear += 5;
        wait_ms(10000);
    }
    else if (choice_intro == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Я никуда не собираюсь.";
        mark_veber.friendship -= 5; mark_veber.fear += 10;
        wait_ms(10000);
    }
}

// Прототип сцены, которая вызывается из show_ch02_meet_mark().
void show_ch02_meet_mark_hack();

//------------------------------2глава выбор 1----------------------------------

// Ветка: игрок соглашается встретиться с Марком.
void show_ch02_meet_mark()
{
    clear_screen();
    std::cout << "=== Глава 2. Эхо дождя ===\n";
    wait_ms(8000);

    clear_screen();
    std::cout << "03:24. " << player.name_of_character
        << " быстро одевается, берёт телефон и выходит из квартиры. "
        << "Дождь почти закончился, но асфальт всё ещё блестит под светом фонарей.\n";
    wait_ms(19000);

    std::cout << player.name_of_character << " (про себя):\n";
    wait_ms(10000);

    std::cout << "— Лея Морроу... Семь лет назад она пропала. "
        << "И каким-то образом знает моё имя.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << " открывает переписку. Последнее сообщение всё ещё на экране.\n";
    wait_ms(6000);

    std::cout << "«Он знает, что ты придёшь».\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Кто этот «он»?.. \n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Телефон вибрирует";
    wait_ms(10000);

    std::cout << "###\n";
    wait_ms(400);

    std::cout << "##\n";
    wait_ms(400);

    std::cout << "###\n";
    wait_ms(400);

    std::cout << "#\n";
    wait_ms(400);

    clear_screen();
    std::cout << "Марк: — Я уже на месте. Ты где?\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Иду. Дай мне пять минут.\n";
    wait_ms(10000);

    std::cout << "Через несколько минут " << player.name_of_character
        << " подходит к старой автобусной остановке. Марк стоит под фонарём, "
        << "держа в руках ноутбук и небольшой рюкзак.\n";
    wait_ms(19000);

    std::cout << "Марк: — Наконец-то. Я уже думал, что ты передумал" << G("", "а") << ".\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Сам не уверен, что правильно поступаю.\n";
    wait_ms(10000);

    std::cout << "Марк: — Тогда почему пришёл?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Потому что кто-то знает обо мне то, чего я сам не помню.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "Марк перестаёт улыбаться.\n";
    wait_ms(6000);

    std::cout << "Марк: — Подожди. Что значит «не помню»?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Пока не знаю. Но мне кажется, это связано с тем комплексом.\n";
    wait_ms(6000);

    std::cout << "Марк открывает ноутбук и показывает карту.\n";
    wait_ms(10000);

    std::cout << "Марк: — Вот что мне прислали. Нокс-7. Исследовательский центр "
        << "закрыли семь лет назад после аварии. Двенадцать погибших, "
        << "несколько пропавших без вести.\n";
    wait_ms(19000);

    std::cout << player.name_of_character << ": — И среди них Лея Морроу?\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Марк: — Откуда ты знаешь это имя?\n";
    wait_ms(10000);

    std::cout << player.name_of_character << " молчит несколько секунд.\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Потому что она мне написала.\n";
    wait_ms(10000);

    std::cout << "Марк: — Что?!\n";
    wait_ms(10000);

    std::cout << "Марк закрывает ноутбук.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ладно. Теперь мы точно идём вместе. "
        << "Но сначала я хочу кое-что тебе показать.\n";
    wait_ms(10000);

    std::cout << "Он достаёт из рюкзака небольшой электронный модуль "
        << "с повреждённой платой.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << player.name_of_character << ": — Что это?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я нашёл его в посылке вместе с картой. "
        << "На обратной стороне есть надпись: ECHO-07.\n";
    wait_ms(10000);

    std::cout << "В этот момент экран ноутбука сам включается. "
        << "На нём появляется новая строка.\n";
    wait_ms(10000);

    std::cout << "ОБНАРУЖЕНЫ ДВА ОБЪЕКТА.\n";
    std::cout << "ДОСТУП К ПРОЕКТУ ВОССТАНОВЛЕН.\n";
    wait_ms(10000);

    std::cout << "ОБНАРУЖЕНЫ ДВА ОБЪЕКТА.\n";
    std::cout << "ДОСТУП К ПРОЕКТУ ВОССТАНОВЛЕН.\n";
    wait_ms(10000);

    std::cout << "ОБНАРУЖЕНЫ ДВА ОБЪЕКТА.\n";
    std::cout << "ДОСТУП К ПРОЕКТУ ВОССТАНОВЛЕН.\n";
    wait_ms(10000);

    std::cout << "////..\n";
    wait_ms(400);

    std::cout << "...\n";
    wait_ms(400);

    std::cout << "..\n";
    wait_ms(400);

    std::cout << "...\n";
    wait_ms(400);

    std::cout << "/\n";
    wait_ms(400);

    clear_screen();
    std::cout << "Марк: — " << player.name_of_character
        << "... Я этого не запускал.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Экран гаснет.\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Кажется, нам пора.\n";
    wait_ms(10000);

    std::cout << "Вы направляетесь к окраине города.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7 ===\n\n";
    std::cout << "Вы стоите перед входом в заброшенный исследовательский комплекс.\n";
    wait_ms(10000);

    std::cout << "Старая камера наблюдения медленно поворачивается в вашу сторону.\n";
    wait_ms(8000);

    std::cout << "Марк: — Дверь заблокирована. Нужно решить, как попасть внутрь.\n";
    wait_ms(8000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Попробовать взломать терминал.                    выбрать 1\n";
    std::cout << "Осмотреть камеру наблюдения.                      выбрать 2\n";
    std::cout << "Попытаться выбить дверь.                          выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_nox_entry)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice_nox_entry = 0;
        }
        if (choice_nox_entry < 1 || choice_nox_entry > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_nox_entry < 1 || choice_nox_entry > 3);

    move_counter = 2;
    if (choice_nox_entry == 1) {
        std::cout << "Вы выбрали: Попробовать взломать терминал.\n";
        player.evidence += 1;
        mark_veber.fear += 5;
    }
    else if (choice_nox_entry == 2) {
        std::cout << "Вы выбрали: Осмотреть камеру наблюдения.\n";
        player.evidence += 2;
    }
    else if (choice_nox_entry == 3) {
        std::cout << "Вы выбрали: Попытаться выбить дверь.\n";
        player.humanity -= 5;
        mark_veber.fear += 10;
    }
    wait_ms(10000);
}

// выбор игрока: "Попробовать взломать терминал с марком."
void show_ch02_meet_mark_hack()
{
    clear_screen();
    std::cout << "=== Глава 2.2. Доступ запрещён ===\n";
    wait_ms(6000);

    std::cout << "" << player.name_of_character << " подходит к терминалу возле массивной металлической двери.\n";
    wait_ms(6000);

    std::cout << "Экран покрыт трещинами. Под ним мигает красный индикатор.\n";
    wait_ms(6000);

    std::cout << "Марк: — Только осторожно. Если система всё ещё работает, "
        << "она может зарегистрировать наше присутствие.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Тогда постараюсь не привлекать внимания.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "На экране появляется запрос доступа.\n";
    wait_ms(10000);

    std::cout << "ВВЕДИТЕ ИДЕНТИФИКАТОР СОТРУДНИКА.\n";
    wait_ms(10000);

    std::cout << "Марк: — У меня есть старые данные из архива. "
        << "Попробуем использовать их.\n";
    wait_ms(10000);

    std::cout << "Марк подключает небольшой кабель к терминалу.\n";
    wait_ms(10000);

    std::cout << "Экран несколько раз мигает. Появляются строки неизвестного кода.\n";
    wait_ms(8000);

    std::cout << "ПРОВЕРКА ИДЕНТИФИКАТОРА...\n";
    wait_ms(10000);

    std::cout << "ОБНАРУЖЕН НЕИЗВЕСТНЫЙ ОБЪЕКТ.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Марк: — Странно. Терминал не должен видеть нас как объекты.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Что значит «объекты»?\n";
    wait_ms(10000);

    std::cout << "Марк: — Не знаю. Но смотри на экран.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "ДОСТУП ОТКЛОНЁН.\n";
    wait_ms(10000);

    std::cout << "ПРОТОКОЛ «ЭХО» АКТИВИРОВАН.\n";
    wait_ms(10000);

    std::cout << "СИГНАЛ ПЕРЕДАН В ЦЕНТРАЛЬНЫЙ УЗЕЛ.\n";
    wait_ms(6000);

    std::cout << "Где-то глубоко внутри комплекса раздаётся металлический гул.\n";
    wait_ms(8000);

    std::cout << "Марк отступает от терминала.\n";
    wait_ms(10000);

    std::cout << "Марк: — Кажется, мы только что сообщили кому-то, что пришли.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Зато мы узнали, что система всё ещё работает.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Терминал неожиданно выводит ещё одну строку:\n";
    wait_ms(10000);

    std::cout << "ПОЛЬЗОВАТЕЛЬ: " << player.name_of_character << ".\n";
    wait_ms(10000);

    std::cout << "СТАТУС: ВОССТАНОВЛЕН.\n";
    wait_ms(10000);

    std::cout << "Марк: — " << player.name_of_character << "... Почему система знает твоё имя?\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Я надеял" << G("ся", "ась") << ", что ты мне это объяснишь.\n";
    wait_ms(10000);

    std::cout << "Дверь комплекса издаёт громкий щелчок.\n";
    wait_ms(10000);

    std::cout << "Замок разблокирован.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Марк: — Ну что, идём? Пока эта штука не передумала.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Да. Но теперь мы должны выяснить, что значит "
        << "«статус восстановлен».\n";
    wait_ms(10000);

    std::cout << "Вы проходите через дверь. За ней начинается тёмный коридор.\n";
    wait_ms(8000);

    std::cout << "На стенах видны следы копоти, а под потолком мигают аварийные лампы.\n";
    wait_ms(6000);

    std::cout << "В глубине коридора раздаётся тихий женский голос:\n";
    wait_ms(6000);

    std::cout << "Лея: — " << player.name_of_character << "... Ты всё-таки приш" << G("ёл", "ла") << ".\n";
    wait_ms(6000);

    std::cout << "Марк резко поворачивается к тебе.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты тоже это слышал" << G("", "а") << "?\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Да.\n";
    wait_ms(10000);

    std::cout << "Голос Леи звучит снова:\n";
    wait_ms(10000);

    std::cout << "Лея: — Не позволяй ему открыть нижний уровень.\n";
    wait_ms(6000);

    std::cout << "Связь обрывается.\n";
    wait_ms(10000);

    std::cout << "Марк: — Кому именно она это сказала?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Пока не знаю. Но мы собираемся это выяснить.\n";
    wait_ms(6000);


    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Взломать систему безопасности.                       выбрать 1\n";
    std::cout << "Попросить Марка отвлечь систему.                     выбрать 2\n";
    std::cout << "Принудительно открыть архив.                         выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_hack_mark)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice_hack_mark = 0;
        }

        if (choice_hack_mark < 1 || choice_hack_mark > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_hack_mark < 1 || choice_hack_mark > 3);

    if (choice_hack_mark == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Взломать систему безопасности.";
        wait_ms(10000);
    }
    else if (choice_hack_mark == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Попросить Марка отвлечь систему.";
        wait_ms(10000);
    }
    else if (choice_hack_mark == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Принудительно открыть архив.";
        wait_ms(10000);
    }
}
// Ветка выбора: игрок осматривает камеру наблюдения вместе с Марком.
void show_ch02_meet_mark_camera()
{
    clear_screen();
    std::cout << "=== Нокс-7. Слепая зона ===\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " не подходит к двери. Вместо этого внимательно следит за камерой над входом.\n";
    wait_ms(6000);

    std::cout << "Камера медленно поворачивается вслед за вами.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Она всё ещё работает. Но комплекс закрыли семь лет назад.\n";
    wait_ms(10000);

    std::cout << "Марк достаёт ноутбук и пытается определить модель камеры.\n";
    wait_ms(10000);

    std::cout << "Марк: — Странно. Корпус старый, а модуль связи совсем новый.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "Ты подходишь ближе, стараясь оставаться вне прямого обзора объектива.\n";
    wait_ms(10000);

    std::cout << "Под креплением обнаруживается тонкий чёрный кабель. Он уходит в стену,\n";
    std::cout << "хотя рядом нет ни одного работающего блока питания.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Она не должна получать питание. Откуда оно идёт?\n";
    wait_ms(10000);

    std::cout << "Марк: — Не трогай кабель. Я попробую проверить сигнал с ноутбука.\n";
    wait_ms(6000);

    std::cout << "На экране ноутбука появляется найденное устройство:\n\n";
    wait_ms(10000);
    std::cout << "УСТРОЙСТВО: CAM-07\n";
    std::cout << "СТАТУС: АКТИВНО\n";
    std::cout << "ПОСЛЕДНЯЯ ПЕРЕДАЧА: 03:17:42\n";
    wait_ms(10000);

    std::cout << "Марк: — 03:17? Это время, когда тебе пришло первое сообщение?\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Да. Получается, камера следила за нами ещё до того, как мы приехали.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "На экране появляется короткая запись с камеры.\n";
    wait_ms(10000);

    std::cout << "Сначала виден пустой вход в комплекс. Затем изображение на секунду искажается.\n";
    wait_ms(6000);

    std::cout << "В отражении тёмного стекла появляется силуэт человека.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Марк... там кто-то есть.\n";
    wait_ms(10000);

    std::cout << "Марк увеличивает изображение, но силуэт исчезает.\n";
    wait_ms(10000);

    std::cout << "На месте записи остаётся только надпись: ECHO-07.\n";
    wait_ms(6000);

    std::cout << "Марк: — Я сохранил запись. Это может быть уликой, но нам лучше не задерживаться.\n";
    wait_ms(10000);

    std::cout << "Камера резко поворачивается к вам. Красный индикатор загорается.\n";
    wait_ms(10000);

    std::cout << "Система: ОБЪЕКТЫ ОБНАРУЖЕНЫ.\n";
    wait_ms(10000);

    std::cout << "Из-за двери раздаётся тихий металлический щелчок. Замок всё ещё закрыт,\n";
    std::cout << "но теперь на панели рядом с дверью горит зелёный индикатор.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Похоже, система заметила нас.\n";
    wait_ms(10000);

    std::cout << "Марк: — И, кажется, сама решила, что мы можем войти.\n";
    wait_ms(6000);

    std::cout << "Ты сохранил" << G("", "а") << " запись с камеры. Найдено улик: "
        << player.evidence << ".\n";
    wait_ms(6000);


    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Извлечь карту памяти.                                выбрать 1\n";
    std::cout << "Проверить резервное питание.                         выбрать 2\n";
    std::cout << "Осмотреть слепую зону.                               выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_camera_mark)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice_camera_mark = 0;
        }

        if (choice_camera_mark < 1 || choice_camera_mark > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_camera_mark < 1 || choice_camera_mark > 3);

    if (choice_camera_mark == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Извлечь карту памяти.";
        wait_ms(10000);
    }
    else if (choice_camera_mark == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Проверить резервное питание.";
        wait_ms(10000);
    }
    else if (choice_camera_mark == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Осмотреть слепую зону.";
        wait_ms(10000);
    }
}
// Ветка выбора: игрок выбивает дверь вместе с Марком.
void show_ch02_meet_mark_door()
{
    clear_screen();
    std::cout << "=== Нокс-7. Закрытая дверь ===\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " отходит на пару шагов от двери и осматривает замок.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Если замок не открывается, попробуем выбить дверь.\n";
    wait_ms(10000);

    std::cout << "Марк недовольно смотрит на тебя.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты серьёзно? Мы даже не знаем, что находится за ней.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Именно поэтому я и хочу попасть внутрь.\n";
    wait_ms(10000);

    std::cout << "Ты отходишь ещё дальше, упираешься ногой в пол и всем весом\n";
    std::cout << "ударяешь по металлической двери.\n";
    wait_ms(6000);

    std::cout << "Глухой удар разносится по пустому коридору.\n";
    wait_ms(10000);

    std::cout << "Дверь дрожит, но замок остаётся на месте.\n";
    wait_ms(10000);

    std::cout << "Марк: — Отлично. Теперь нас, вероятно, слышит весь комплекс.\n";
    wait_ms(6000);

    std::cout << "Ты собираешься ударить ещё раз.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Неожиданный ответ ===\n\n";
    wait_ms(10000);

    std::cout << "Второй удар оказывается сильнее. С металлическим грохотом\n";
    std::cout << "дверь немного прогибается внутрь.\n";
    wait_ms(6000);

    std::cout << "Внезапно внутри стены что-то щёлкает.\n";
    wait_ms(10000);

    std::cout << "Марк: — Стой. Ты слышал" << G("", "а") << "?\n";
    wait_ms(10000);

    std::cout << "Из-за двери доносится тихий гул. Он становится всё громче,\n";
    std::cout << "словно внутри запускается давно остановленный механизм.\n";
    wait_ms(10000);

    std::cout << "На панели доступа загораются несколько красных символов.\n";
    wait_ms(10000);

    std::cout << "Система: НЕСАНКЦИОНИРОВАННОЕ ВОЗДЕЙСТВИЕ.\n";
    wait_ms(10000);

    std::cout << "Система: ПРОВЕРКА ИДЕНТИФИКАЦИИ...\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Я ничего не нажимал" << G("", "а") << ". Она сама включилась.\n";
    wait_ms(10000);

    std::cout << "Марк достаёт ноутбук и смотрит на экран. Его лицо резко меняется.\n";
    wait_ms(6000);

    std::cout << "Марк: — Этого не может быть...\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Что там?\n";
    wait_ms(10000);

    std::cout << "Марк: — Система пытается идентифицировать тебя.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "На экране ноутбука появляется сообщение:\n\n";
    wait_ms(10000);

    std::cout << "ОБЪЕКТ: " << player.name_of_character << "\n";
    std::cout << "СТАТУС: ИДЕНТИФИКАЦИЯ НЕ ЗАВЕРШЕНА\n";
    std::cout << "ПРОТОКОЛ: ECHO-07\n";
    wait_ms(10000);

    std::cout << "\nМарк: — Почему система знает твоё имя?\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Я надеял" << G("ся", "ась") << ", что ты мне это объяснишь.\n";
    wait_ms(6000);

    std::cout << "Внутри двери раздаётся ещё один щелчок. Замок медленно отходит.\n";
    wait_ms(6000);

    std::cout << "Дверь приоткрывается на несколько сантиметров.\n";
    wait_ms(10000);

    std::cout << "Из темноты тянет холодным воздухом. За дверью не видно ни света,\n";
    std::cout << "ни движения — только длинный коридор, уходящий в глубину комплекса.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ладно. Ты хотел" << G("", "а") << " попасть внутрь. Теперь решай, что будем делать.\n";
    wait_ms(6000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Вместе выбить дверь.                                 выбрать 1\n";
    std::cout << "Найти аварийный механизм.                            выбрать 2\n";
    std::cout << "Оставить Марка снаружи.                              выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_door_mark)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_door_mark = 0;
        }

        if (choice_door_mark < 1 || choice_door_mark > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_door_mark < 1 || choice_door_mark > 3);

    if (choice_door_mark == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Вместе выбить дверь.";
        wait_ms(10000);
    }
    else if (choice_door_mark == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Найти аварийный механизм.";
        wait_ms(10000);
    }
    else if (choice_door_mark == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Оставить Марка снаружи.";
        wait_ms(10000);
    }
}

//-------------------------------2глава выбор 2---------------------------------

// выбор игрока: "Я " << G("сам", "сама") << " разберусь. Не вмешивайся."
void show_ch02_go_alone()
{
    clear_screen();
    std::cout << "=== Глава 2. Эхо дождя ===\n";
    wait_ms(8000);

    clear_screen();
    std::cout << "03:19. " << player.name_of_character
        << " сидит перед компьютером. На экране всё ещё открыта переписка с неизвестной девушкой.\n";
    wait_ms(6000);

    std::cout << "Марк (по телефону): — " << player.name_of_character
        << ", я серьёзно. Не ходи туда один. Я уже скачал карту комплекса. Могу встретить тебя через двадцать минут.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Я " << G("сам", "сама") << " разберусь. Не вмешивайся.\n";
    wait_ms(10000);

    std::cout << "На другом конце провода наступает тишина.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты сейчас серьёзно?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Да. Я не хочу втягивать тебя в это.\n";
    wait_ms(10000);

    std::cout << "Марк: — Или ты просто мне не доверяешь?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Дело не в этом.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ладно. Делай что хочешь. Только потом не говори, что я тебя не предупреждал.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "Марк сбрасывает звонок.\n";
    wait_ms(10000);

    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);

    clear_screen();
    std::cout << player.name_of_character
        << " (про себя): — Извини, Марк. Но я " << G("должен", "должна") << " разобраться " << G("сам", "сама") << ".\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << " открывает карту, полученную вместе с сообщением. На ней отмечен заброшенный исследовательский комплекс «Нокс-7».\n";
    wait_ms(8000);

    std::cout << "Внезапно на компьютере появляется новое сообщение.\n";
    wait_ms(10000);

    std::cout << "Лея: — Ты " << G("один", "одна") << "?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Да. Откуда ты знаешь?\n";
    wait_ms(10000);

    std::cout << "Лея: — Хорошо. Значит, пока всё идёт по плану.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — По какому ещё плану? Кто ты такая?\n";
    wait_ms(10000);

    std::cout << "Лея: — Когда войдёшь в комплекс, не спускайся на нижний уровень. Что бы ты ни услышал.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Почему я " << G("должен", "должна") << " тебе верить?\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Сообщение исчезает. Через секунду приходит ещё одно.\n";
    wait_ms(10000);

    std::cout << "Лея: — Потому что в прошлый раз ты мне не поверил" << G("", "а") << ".\n";
    wait_ms(10000);

    std::cout << "Компьютер выключается.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что за чёрт?.. \n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " берёт куртку, кладёт телефон в карман и выходит из дома.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "03:42. " << player.name_of_character
        << " добирается до окраины города. Перед ним возвышается огромное бетонное здание. Часть окон выбита, а за ржавым забором видны старые машины.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " (про себя): — Семь лет назад... Что здесь произошло?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " достаёт телефон. Сигнала почти нет.\n";
    wait_ms(10000);

    std::cout << "Внезапно телефон звонит. На экране высвечивается имя Марка.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Чего тебе?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я не собираюсь тебя останавливать. Просто послушай.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Говори.\n";
    wait_ms(10000);

    std::cout << "Марк: — Я проверил архивы. Нашёл фотографию сотрудников комплекса. Там есть человек, который выглядит точно как ты.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Это невозможно.\n";
    wait_ms(10000);

    std::cout << "Марк: — Я тоже так подумал. Но фотография датирована семью годами ранее.\n";
    wait_ms(6000);

    std::cout << "Связь начинает прерываться.\n";
    wait_ms(10000);

    std::cout << "Марк: — И ещё кое-что. На обратной стороне написано твоё имя.\n";
    wait_ms(10000);

    std::cout << "Звонок обрывается.\n";
    wait_ms(10000);

    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);
    std::cout << ".\n";
    wait_ms(400);

    std::cout << player.name_of_character
        << " смотрит на вход в комплекс. Рядом с дверью находится терминал, над ним висит разбитая камера наблюдения. Чуть дальше видна металлическая дверь с электронным замком.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ладно. Посмотрим, что вы от меня скрываете.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7: Вход в комплекс ===\n\n";

    std::cout << "Ты стоишь перед заброшенным исследовательским комплексом.\n";
    wait_ms(10000);

    std::cout << "Рядом с дверью находится терминал. Над ним висит разбитая камера наблюдения.\n";
    wait_ms(6000);

    flush_input();

    std::cout << "\nСделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Попробовать взломать терминал.                    выбрать 1\n";
    std::cout << "Осмотреть камеру наблюдения.                      выбрать 2\n";
    std::cout << "Попытаться выбить дверь.                          выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_solo_entry)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice_solo_entry = 0;
        }
        if (choice_solo_entry < 1 || choice_solo_entry > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_solo_entry < 1 || choice_solo_entry > 3);

    move_counter = 2;

    if (choice_solo_entry == 1) {
        clear_screen();
        std::cout << "Ты решил" << G("", "а") << " взломать терминал.\n\n";
        std::cout << "Ты подключаешься к системе. На экране появляется запрос доступа.\n";
        std::cout << "ВВЕДИТЕ ИДЕНТИФИКАТОР СОТРУДНИКА.\n";

        player.evidence += 1;

        wait_ms(6000);
    }
    else if (choice_solo_entry == 2) {
        clear_screen();
        std::cout << "Ты решил" << G("", "а") << " осмотреть камеру наблюдения.\n\n";
        std::cout << "Ты внимательно изучаешь повреждённую камеру.\n";
        std::cout << "Несмотря на трещины, один из индикаторов продолжает мигать.\n";
        std::cout << "Возможно, камера всё ещё передаёт изображение.\n";

        player.evidence += 2;

        wait_ms(8000);
    }
    else if (choice_solo_entry == 3) {
        clear_screen();
        std::cout << "Ты решил" << G("", "а") << " выбить дверь.\n\n";
        std::cout << "Ты отходишь на несколько шагов и с силой ударяешь по двери.\n";
        std::cout << "Раздаётся громкий металлический грохот.\n";
        std::cout << "Дверь не открывается. Где-то внутри комплекса включается сигнализация.\n";

        player.humanity -= 5;
        player.fear += 10;

        wait_ms(8000);
    }
}

// выбор игрока: "самому без марка взломать терменал"
void show_ch02_solo_hack()
{
    clear_screen();
    std::cout << "=== Нокс-7. Доступ запрещён ===\n\n";
    wait_ms(10000);

    std::cout << "Ты подходишь к терминалу рядом с массивной металлической дверью.\n";
    wait_ms(10000);

    std::cout << "Экран мерцает. На нём отображается логотип комплекса и надпись:\n\n";
    wait_ms(10000);

    std::cout << "ДОСТУП ЗАПРЕЩЁН\n";
    std::cout << "ТРЕБУЕТСЯ АВТОРИЗАЦИЯ\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Посмотрим, что ты скрываешь...\n";
    wait_ms(10000);

    std::cout << "Ты снимаешь крышку терминала. Внутри обнаруживаются старый";
    std::cout << " контроллер и несколько проводов.\n";
    wait_ms(6000);

    std::cout << "Несмотря на возраст оборудования, один из модулей выглядит";
    std::cout << " совершенно новым.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== Попытка подключения ===\n\n";
    wait_ms(10000);

    std::cout << "Ты подключаешь кабель к сервисному порту и открываешь";
    std::cout << " диагностический интерфейс.\n";
    wait_ms(6000);

    std::cout << "На экране появляются строки системного журнала:\n\n";
    wait_ms(10000);

    std::cout << "> SYSTEM STATUS: ONLINE\n";
    std::cout << "> SECURITY LOCK: ACTIVE\n";
    std::cout << "> LAST ACCESS: 03:17:42\n";
    std::cout << "> UNKNOWN PROCESS: ECHO-07\n";
    wait_ms(10000);

    std::cout << "\n" << player.name_of_character
        << ": — ECHO-07... Что это ещё такое?\n";
    wait_ms(6000);

    std::cout << "Ты пытаешься открыть сведения о неизвестном процессе.\n";
    wait_ms(10000);

    std::cout << "Терминал на несколько секунд перестаёт реагировать.\n";
    wait_ms(6000);

    std::cout << "Затем экран гаснет.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Неизвестный пользователь ===\n\n";
    wait_ms(10000);

    std::cout << "Экран снова загорается. Теперь на нём отображается новое сообщение:\n\n";
    wait_ms(10000);

    std::cout << "ОБНАРУЖЕНО НЕСАНКЦИОНИРОВАННОЕ ПОДКЛЮЧЕНИЕ\n";
    wait_ms(10000);

    std::cout << "ИДЕНТИФИКАЦИЯ ПОЛЬЗОВАТЕЛЯ...\n";
    wait_ms(6000);

    std::cout << "ОШИБКА\n";
    wait_ms(10000);

    std::cout << "ПОЛЬЗОВАТЕЛЬ УЖЕ ЗАРЕГИСТРИРОВАН В СИСТЕМЕ\n";
    wait_ms(10000);

    std::cout << "\n" << player.name_of_character
        << ": — Какой ещё зарегистрированный пользователь?\n";
    wait_ms(6000);

    std::cout << "Ты ничего не вводил" << G("", "а") << ", но терминал самостоятельно открывает";
    std::cout << " новое окно.\n";
    wait_ms(6000);

    std::cout << "На экране появляется одна строка:\n\n";
    wait_ms(10000);

    std::cout << "МЫ ЖДАЛИ ТЕБЯ.\n";
    wait_ms(10000);

    std::cout << "Внезапно изнутри двери доносится металлический щелчок.\n";
    wait_ms(10000);

    std::cout << "Индикатор замка меняется с красного на жёлтый,";
    std::cout << " но дверь остаётся закрытой.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Значит, ты всё-таки можешь меня впустить...\n";
    wait_ms(6000);

    std::cout << "Терминал издаёт короткий звуковой сигнал.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ДЛЯ ЗАВЕРШЕНИЯ АВТОРИЗАЦИИ ТРЕБУЕТСЯ";
    std::cout << " ДОПОЛНИТЕЛЬНОЕ ПОДТВЕРЖДЕНИЕ.\n";
    wait_ms(10000);

    std::cout << "\nТы убираешь руки от терминала. Теперь ты знаешь,";
    std::cout << " что система каким-то образом тебя распознала.\n";
    wait_ms(10000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(6000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Подобрать идентификатор.                             выбрать 1\n";
    std::cout << "Обойти защиту вручную.                               выбрать 2\n";
    std::cout << "Открыть журнал событий.                              выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_solo_hack)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_solo_hack = 0;
        }

        if (choice_solo_hack < 1 || choice_solo_hack > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_solo_hack < 1 || choice_solo_hack > 3);

    if (choice_solo_hack == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Подобрать идентификатор.";
        wait_ms(10000);
    }
    else if (choice_solo_hack == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Обойти защиту вручную.";
        wait_ms(10000);
    }
    else if (choice_solo_hack == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Открыть журнал событий.";
        wait_ms(10000);
    }
}
// выбор игрока: "самому без марка осмотреть камеру"
void show_ch02_solo_camera()
{
    clear_screen();
    std::cout << "=== Нокс-7. Наблюдатель ===\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " останавливается перед входом в комплекс и поднимает взгляд на камеру.\n";
    wait_ms(6000);

    std::cout << "Камера установлена высоко над дверью. Её корпус покрыт пылью,\n";
    std::cout << "но объектив выглядит чистым.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Семь лет без обслуживания... А ты всё ещё работаешь?\n";
    wait_ms(6000);

    std::cout << "Ты отходишь в сторону и наблюдаешь за камерой.\n";
    wait_ms(10000);

    std::cout << "Через несколько секунд объектив медленно поворачивается вслед за тобой.\n";
    wait_ms(6000);

    std::cout << "Значит, питание всё ещё подаётся. Но откуда?\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Старая запись ===\n\n";
    wait_ms(10000);

    std::cout << "Рядом с камерой ты замечаешь небольшую сервисную панель.\n";
    wait_ms(10000);

    std::cout << "Крышка держится на одном винте. Осторожно сняв её,\n";
    std::cout << "ты обнаруживаешь порт для обслуживания оборудования.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Посмотрим, что ты записывала всё это время.\n";
    wait_ms(10000);

    std::cout << "Ты подключаешь телефон к сервисному интерфейсу камеры.\n";
    wait_ms(6000);

    std::cout << "На экране появляется список файлов. Большинство повреждено,\n";
    std::cout << "но одна запись сохранилась полностью.\n";
    wait_ms(6000);

    std::cout << "ФАЙЛ: CAM_07_0317\n";
    std::cout << "ДАТА: НЕИЗВЕСТНА\n";
    std::cout << "СТАТУС: ДОСТУПЕН\n";
    wait_ms(10000);

    std::cout << "\nТы запускаешь запись.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Кто-то у входа ===\n\n";
    wait_ms(10000);

    std::cout << "На записи виден тот же вход в комплекс.\n";
    std::cout << "Время на экране показывает 03:17:42.\n";
    wait_ms(6000);

    std::cout << "Несколько секунд ничего не происходит.\n";
    wait_ms(10000);

    std::cout << "Затем у двери появляется человек.\n";
    wait_ms(6000);

    std::cout << "Изображение слишком размыто, чтобы разглядеть его лицо.\n";
    wait_ms(10000);

    std::cout << "Человек стоит неподвижно и смотрит прямо в объектив.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Кто ты такой?.. \n";
    wait_ms(10000);

    std::cout << "Фигура поднимает руку и указывает прямо на камеру.\n";
    wait_ms(6000);

    std::cout << "В этот момент запись начинает искажаться.\n";
    wait_ms(10000);

    std::cout << "На несколько кадров изображение сменяется чёрным экраном.\n";
    wait_ms(10000);

    std::cout << "Когда картинка возвращается, человек исчезает.\n";
    wait_ms(6000);

    std::cout << "На последнем кадре появляется надпись:\n\n";
    wait_ms(10000);

    std::cout << "ECHO-07\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Камера смотрит на тебя ===\n\n";
    wait_ms(10000);

    std::cout << "Ты сохраняешь запись на телефон.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — ECHO-07... Это связано с тем сообщением?\n";
    wait_ms(6000);

    std::cout << "Внезапно экран телефона гаснет.\n";
    wait_ms(10000);

    std::cout << "Над входом раздаётся тихий щелчок.\n";
    wait_ms(10000);

    std::cout << "Ты поднимаешь голову. Камера больше не следит за дверью.\n";
    std::cout << "Теперь объектив направлен прямо на тебя.\n";
    wait_ms(10000);

    std::cout << "На корпусе загорается красный индикатор.\n";
    wait_ms(10000);

    std::cout << "Система: ОБЪЕКТ ОБНАРУЖЕН.\n";
    wait_ms(6000);

    std::cout << "Из-за двери доносится металлический звук, словно внутри\n";
    std::cout << "кто-то только что отпер один из замков.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Значит, ты знаешь, что я здесь.\n";
    wait_ms(6000);

    std::cout << "Ты убираешь телефон и снова смотришь на закрытую дверь.\n";
    std::cout << "Теперь у тебя есть запись, но вопросов стало ещё больше.\n";
    wait_ms(10000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(6000);

    std::cout << "\n==============================================================\n";
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Восстановить запись с камеры.                       выбрать 1\n";
    std::cout << "Проследить за проводами камеры.                     выбрать 2\n";
    std::cout << "Проверить отражение в объективе.                    выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_solo_camera)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_solo_camera = 0;
        }

        if (choice_solo_camera < 1 || choice_solo_camera > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_solo_camera < 1 || choice_solo_camera > 3);

    if (choice_solo_camera == 1) {
        clear_screen();
        std::cout << "=== Нокс-7. Восстановление записи ===\n\n";
        std::cout << "Ты решил" << G("", "а") << " восстановить повреждённые файлы камеры.\n";
        std::cout << "Возможно, в них сохранилось то, чего не было\n";
        std::cout << "в основной записи.\n";
        wait_ms(6000);
    }
    else if (choice_solo_camera == 2) {
        clear_screen();
        std::cout << "=== Нокс-7. След проводов ===\n\n";
        std::cout << "Ты решил" << G("", "а") << " проследить за проводами камеры.\n";
        std::cout << "Нужно выяснить, откуда поступает питание\n";
        std::cout << "и куда передаются данные.\n";
        wait_ms(6000);
    }
    else if (choice_solo_camera == 3) {
        clear_screen();
        std::cout << "=== Нокс-7. Отражение ===\n\n";
        std::cout << "Ты решил" << G("", "а") << " внимательно проверить отражение\n";
        std::cout << "в объективе камеры.\n";
        std::cout << "Возможно, камера показывает больше, чем кажется.\n";
        wait_ms(6000);
    }
}
// выбор игрока: "самому без марка выбить дверь"
void show_ch02_solo_door()
{
    clear_screen();
    std::cout << "=== Нокс-7. Последняя попытка ===\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " отходит от двери и внимательно осматривает металлическую раму.\n";
    wait_ms(6000);

    std::cout << "Дверь массивная. Рядом нет ни ручки, ни обычного замка — только\n";
    std::cout << "панель доступа с погасшим экраном.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Ладно. Попробуем по-хорошему... хотя бы сначала.\n";
    wait_ms(10000);

    std::cout << "Ты нажимаешь на панель, но она никак не реагирует.\n";
    wait_ms(10000);

    std::cout << "Ты отступаешь на несколько шагов и с силой ударяешь ногой по двери.\n";
    wait_ms(6000);

    std::cout << "Глухой металлический грохот разносится по пустой территории.\n";
    wait_ms(6000);

    std::cout << "Боль отдаётся в ноге. Дверь даже не сдвинулась.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Чёрт! Да что же у вас за дверь такая?\n";
    wait_ms(6000);

    std::cout << "Ты собираешься ударить ещё раз, но внезапно слышишь тихий гул.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== Нокс-7. Ответ изнутри ===\n\n";
    wait_ms(10000);

    std::cout << "Гул доносится из-за двери. Кажется, где-то глубоко внутри\n";
    std::cout << "комплекса запускается механизм.\n";
    wait_ms(10000);

    std::cout << "На панели доступа загорается красный индикатор.\n";
    wait_ms(10000);

    std::cout << "Система: НЕСАНКЦИОНИРОВАННОЕ ВОЗДЕЙСТВИЕ.\n";
    wait_ms(6000);

    std::cout << "Ты замираешь. Ты ведь не подключал" << G("ся", "ась") << " к терминалу и ничего не вводил.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Подожди... система следит за дверью?\n";
    wait_ms(6000);

    std::cout << "Экран панели внезапно включается.\n";
    wait_ms(10000);

    std::cout << "На нём появляется надпись:\n\n";
    wait_ms(10000);

    std::cout << "ИДЕНТИФИКАЦИЯ ОБЪЕКТА...\n";
    wait_ms(10000);

    std::cout << "ОШИБКА ИДЕНТИФИКАЦИИ\n";
    wait_ms(10000);

    std::cout << "ПРОТОКОЛ ECHO-07 АКТИВИРОВАН\n";
    wait_ms(10000);

    std::cout << "\n" << player.name_of_character
        << ": — ECHO-07... Опять это название.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== Нокс-7. За дверью ===\n\n";
    wait_ms(10000);

    std::cout << "Внезапно раздаётся громкий щелчок.\n";
    wait_ms(10000);

    std::cout << "Один из замков отходит, и дверь слегка приоткрывается.\n";
    wait_ms(6000);

    std::cout << "Изнутри тянет холодным воздухом. За дверью темно.\n";
    wait_ms(6000);

    std::cout << "Ты осторожно заглядываешь внутрь. В глубине коридора на секунду\n";
    std::cout << "загорается тусклый свет, а затем снова гаснет.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Я просто хотел" << G("", "а") << " открыть дверь... Что здесь происходит?\n";
    wait_ms(6000);

    std::cout << "Из динамика над входом доносится искажённый голос:\n";
    wait_ms(10000);

    std::cout << "Неизвестный голос: — "
        << player.name_of_character << "... наконец-то.\n";
    wait_ms(10000);

    std::cout << "Ты резко поднимаешь голову. Голос произнёс твоё имя.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Кто здесь?!\n";
    wait_ms(10000);

    std::cout << "Ответа нет. Только слабый треск динамика.\n";
    wait_ms(6000);

    std::cout << "Дверь остаётся приоткрытой. Внутри тебя ждёт тёмный коридор.\n";
    wait_ms(6000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(6000);

    // ===== ВЫБОР ДАЛЬНЕЙШИХ ДЕЙСТВИЙ =====

    clear_screen();
    std::cout << "=== Нокс-7. Что делать дальше? ===\n\n";

    std::cout << "Дверь приоткрыта, а из динамика больше не доносится ни звука.\n";
    std::cout << "Ты стоишь перед тёмным коридором и пытаешься решить, что делать.\n\n";

    std::cout << "1 — Выбить дверь повторно\n";
    std::cout << "2 — Найти инструмент\n";
    std::cout << "3 — Прислушаться к голосу\n\n";

    int choice_solo_door = 0;

    do
    {
        std::cout << "Твой выбор: ";

        if (!(std::cin >> choice_solo_door))
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');

            std::cout << "Ошибка! Введи число от 1 до 3.\n";
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice_solo_door < 1 || choice_solo_door > 3)
        {
            std::cout << "Такого варианта нет. Выбери 1, 2 или 3.\n";
        }

    } while (choice_solo_door < 1 || choice_solo_door > 3);

    switch (choice_solo_door)
    {
    case 1:
        clear_screen();
        std::cout << "=== Нокс-7. Последний удар ===\n\n";

        std::cout << player.name_of_character
            << " решает попытаться выбить дверь ещё раз.\n";
        wait_ms(6000);

        std::cout << "Ты упираешься ногой в металлическую створку и готовишься ударить.\n";
        wait_ms(6000);

        std::cout << "Но прежде чем ты успеваешь это сделать, дверь медленно\n";
        std::cout << "открывается сама по себе.\n";
        wait_ms(10000);

        std::cout << "Из темноты доносится тихий голос:\n";
        wait_ms(10000);

        std::cout << "Неизвестный голос: — Не нужно. Мы уже знаем, что ты здесь.\n";
        wait_ms(10000);

        break;

    case 2:
        clear_screen();
        std::cout << "=== Нокс-7. Поиск инструмента ===\n\n";

        std::cout << player.name_of_character
            << " решает осмотреть территорию в поисках чего-нибудь полезного.\n";
        wait_ms(6000);

        std::cout << "Рядом с дверью валяется перевёрнутый металлический ящик.\n";
        wait_ms(10000);

        std::cout << "Внутри ты находишь старый разводной ключ и несколько ржавых деталей.\n";
        wait_ms(6000);

        std::cout << "На дне ящика лежит пластиковая карточка с потёртой надписью:\n";
        wait_ms(10000);

        std::cout << "ECHO-07 / ДОСТУП ПЕРСОНАЛА\n";
        wait_ms(10000);

        std::cout << "Ты забираешь карточку. Возможно, она поможет открыть дверь.\n";
        wait_ms(6000);

        break;

    case 3:
        clear_screen();
        std::cout << "=== Нокс-7. Голос из темноты ===\n\n";

        std::cout << player.name_of_character
            << " приближается к динамику и внимательно прислушивается.\n";
        wait_ms(6000);

        std::cout << "Сначала слышен только треск помех.\n";
        wait_ms(10000);

        std::cout << "Затем раздаётся едва различимый шёпот:\n";
        wait_ms(10000);

        std::cout << "Неизвестный голос: — Не верь тому, что увидишь в коридоре.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Кто ты? Откуда знаешь моё имя?\n";
        wait_ms(6000);

        std::cout << "Несколько секунд динамик молчит.\n";
        wait_ms(10000);

        std::cout << "Неизвестный голос: — Потому что ты уже был" << G("", "а") << " здесь.\n";
        wait_ms(10000);

        std::cout << "Связь резко обрывается.\n";
        wait_ms(10000);

        break;
    }

    std::cout << "\nНажми Enter, чтобы продолжить...";
    std::cin.get();
}

//-------------------------------2глава выбор 3------------------------------- тупик игра идет заного 

// выбор игрока: "Я никуда не собираюсь."
void show_ch02_stay_home()
{
    clear_screen();
    std::cout << "=== Глава 2. Эхо дождя ===\n\n";
    wait_ms(6000);

    std::cout << "03:19. " << player.name_of_character
        << " сидит перед компьютером. На экране открыта переписка с Леей Морроу.\n";
    wait_ms(6000);

    std::cout << "Марк (по телефону): — " << player.name_of_character
        << ", я нашёл кое-что странное. Мне прислали карту заброшенного исследовательского комплекса. Там произошло что-то семь лет назад.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Не знаю, о чём ты. Я никуда не собираюсь.\n";
    wait_ms(10000);

    std::cout << "На другом конце провода Марк замолкает.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ладно... Если передумаешь, позвони.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Хорошо. Спокойной ночи.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << player.name_of_character << " завершает вызов.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " (про себя): — Мне это не нравится. Лучше вообще не вмешиваться.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "Ты закрываешь ноутбук. За окном шумит дождь.\n";
    wait_ms(6000);

    std::cout << "Ты стараешься не думать о сообщениях Леи.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Обычный розыгрыш. Какой-то хакер решил меня напугать.\n";
    wait_ms(6000);

    std::cout << "Фотография поддельная. Сообщения можно подделать.\n";
    std::cout << "А заброшенных комплексов в интернете полно.\n";
    wait_ms(10000);

    std::cout << "Всё можно объяснить. По крайней мере, ты пытаешься в это верить.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== ВХОДЯЩЕЕ СООБЩЕНИЕ ===\n\n";
    wait_ms(10000);

    std::cout << "Отправитель: L.MORROW\n";
    wait_ms(10000);

    std::cout << "Лея: — Ты решил" << G("", "а") << " остаться дома?\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Кто ты такая?! Как ты попала в мой компьютер?\n";
    wait_ms(6000);

    std::cout << "Лея: — Ты всегда выбираешь самый безопасный путь.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Я тебя не знаю.\n";
    wait_ms(10000);

    std::cout << "Лея: — Знаешь. Просто тебе позволили забыть.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "На экране появляется фотография старого исследовательского комплекса.\n";
    wait_ms(6000);

    std::cout << "Перед входом стоят несколько сотрудников в белых халатах.\n";
    wait_ms(6000);

    std::cout << "Ты увеличиваешь изображение.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Подожди... Это что, я?\n";
    wait_ms(10000);

    std::cout << "На фотографии действительно стоит " << G("мальчик", "девочка") << ", удивительно " << G("похожий", "похожая") << " на тебя.\n";
    wait_ms(10000);

    std::cout << "Лея: — Завтра в 03:17 ты получишь ещё одно сообщение. Тогда у тебя останется последний шанс.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Шанс на что?\n";
    wait_ms(10000);

    std::cout << "Ответа нет. Экран становится чёрным.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "Ты долго смотришь на отражение своего лица в выключенном мониторе.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << " (про себя): — Поддельная фотография. Взлом компьютера. Кто-то решил надо мной пошутить.\n";
    wait_ms(10000);

    std::cout << "Да. Именно так всё и было.\n";
    wait_ms(10000);

    std::cout << "Ты повторяешь это про себя снова и снова, пока страх постепенно не отступает.\n";
    wait_ms(10000);

    std::cout << "Телефон молчит. За окном идёт дождь.\n";
    wait_ms(6000);

    std::cout << "Ты решаешь, что утром всё будет казаться гораздо менее странным.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "03:41. Ты ложишься в кровать и закрываешь глаза.\n";
    wait_ms(10000);

    std::cout << "В голове всё ещё крутятся слова Леи.\n";
    wait_ms(6000);

    std::cout << "«Просто тебе позволили забыть».\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Чёртова шутка...\n";
    wait_ms(10000);

    std::cout << "Дождь становится тише. Мысли путаются.\n";
    wait_ms(6000);

    std::cout << "Ты постепенно проваливаешься в сон.\n";
    wait_ms(8000);

    clear_screen();
    std::cout << "                 ...\n\n";
    wait_ms(6000);

    std::cout << "Тишина.\n";
    wait_ms(6000);

    std::cout << "Затем где-то в темноте раздаётся знакомый звук уведомления.\n";
    wait_ms(6000);

    std::cout << "03:17.\n";
    wait_ms(10000);

    std::cout << "Ты стоишь перед металлической дверью незнакомого комплекса.\n";
    wait_ms(10000);

    std::cout << "Над дверью висит камера. Её красный индикатор медленно мигает.\n";
    wait_ms(10000);

    std::cout << "Из динамика доносится женский голос.\n";
    wait_ms(10000);

    std::cout << "Лея: — Ты всё-таки приш" << G("ёл", "ла") << ".\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Это сон. Просто сон.\n";
    wait_ms(6000);

    std::cout << "Лея: — Тогда почему ты помнишь то, чего никогда не " << G("знал", "знала") << "?\n";
    wait_ms(10000);

    std::cout << "Дверь начинает медленно открываться.\n";
    wait_ms(10000);

    std::cout << "За ней стоит человек. Его лица не видно.\n";
    wait_ms(6000);

    std::cout << "Неизвестный: — С возвращением.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== 03:17 ===\n\n";
    wait_ms(10000);

    std::cout << "Ты резко открываешь глаза.\n";
    wait_ms(10000);

    std::cout << "Комната. Кровать. Компьютер.\n";
    wait_ms(10000);

    std::cout << "За окном всё так же идёт дождь.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Что за дерьмо мне приснилось?.. \n";
    wait_ms(6000);

    std::cout << "Ты берёшь телефон. На экране нет новых сообщений.\n";
    wait_ms(6000);

    std::cout << "Ни от Леи. Ни от Марка.\n";
    wait_ms(10000);

    std::cout << "Ты облегчённо выдыхаешь.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " (про себя): — Вот видишь. Просто сон. Я слишком много об этом думал.\n";
    wait_ms(10000);

    std::cout << "Ты встаёшь с кровати и подходишь к компьютеру.\n";
    wait_ms(10000);

    std::cout << "На мониторе открывается знакомая переписка.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== ВХОДЯЩЕЕ СООБЩЕНИЕ ===\n\n";
    wait_ms(10000);

    std::cout << "Отправитель: L.MORROW\n";
    wait_ms(10000);

    std::cout << "Лея: — Доброе утро, " << player.name_of_character << ".\n";
    wait_ms(10000);

    std::cout << "Ты перестаёшь дышать на мгновение.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Нет... Этого не может быть.\n";
    wait_ms(6000);

    std::cout << "Следом приходит ещё одно сообщение.\n";
    wait_ms(10000);

    std::cout << "Лея: — В этот раз ты тоже останешься дома?\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== КОНЕЦ ПЕТЛИ ===\n\n";
    wait_ms(10000);

    std::cout << "Ты смотришь на экран, пытаясь понять, что реально, а что нет.\n";
    wait_ms(10000);

    std::cout << "Но в глубине души уже знаешь: это не закончилось.\n";
    wait_ms(10000);

    std::cout << "\nИстория начинается заново...\n";
    wait_ms(8000);

    // main() запустит историю заново после завершения этой ветки.
    restart_requested = true;
}

//-------------------------------3глава выбор 1 хакерство с макрком-------------------------

//выбор игрокa: Взломать систему безопасности.
void show_ch03_mark_hack_security()
{
    clear_screen();
    std::cout << "=== Глава 3.1. Чужой доступ ===\n";
    wait_ms(6000);

    std::cout << "Вы снова подходите к терминалу.\n";
    wait_ms(10000);

    std::cout << "Марк: — Подожди. Ты действительно хочешь продолжать?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Система знает мой ник. Лея знает, что мы здесь. "
        << "Я хочу понять, что происходит.\n";
    wait_ms(10000);

    std::cout << "Марк подключает кабель к боковой панели терминала.\n";
    wait_ms(10000);

    std::cout << "Марк: — Я попробую удержать соединение. "
        << "Ты ищи раздел управления безопасностью.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "ПОДКЛЮЧЕНИЕ К СИСТЕМЕ БЕЗОПАСНОСТИ...\n";
    wait_ms(10000);

    std::cout << "ПОИСК УЯЗВИМОСТЕЙ...\n";
    wait_ms(10000);

    std::cout << "ОБНАРУЖЕНЫ ТРИ УРОВНЯ ДОСТУПА.\n";
    wait_ms(10000);

    std::cout << "УРОВЕНЬ 1: ВНЕШНИЙ КОНТУР — ДОСТУПЕН.\n";
    wait_ms(6000);

    std::cout << "УРОВЕНЬ 2: АРХИВ ИССЛЕДОВАНИЙ — ЗАБЛОКИРОВАН.\n";
    wait_ms(6000);

    std::cout << "УРОВЕНЬ 3: НИЖНИЙ УРОВЕНЬ — ДОСТУП ЗАПРЕЩЁН.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Здесь три уровня. Архив и нижний уровень закрыты.\n";
    wait_ms(6000);

    std::cout << "Марк: — Не трогай нижний уровень. Ты слышал" << G("", "а") << ", что сказала Лея.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Именно поэтому я хочу узнать, что там.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "ВНИМАНИЕ: ОБНАРУЖЕНА СОВМЕСТИМОСТЬ ПОЛЬЗОВАТЕЛЯ.\n";
    wait_ms(6000);

    std::cout << "ИДЕНТИФИКАЦИЯ: "
        << player.name_of_character << ".\n";
    wait_ms(10000);

    std::cout << "СТАТУС: ВОССТАНОВЛЕН.\n";
    wait_ms(10000);

    std::cout << "Марк: — Опять этот статус. Что он означает?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Может быть, я уже был здесь раньше?\n";
    wait_ms(10000);

    std::cout << "Марк молчит. На мгновение он смотрит на тебя так, "
        << "будто хочет что-то сказать.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк, ты ведь ничего от меня не скрываешь?\n";
    wait_ms(10000);

    std::cout << "Марк: — Сейчас это неважно. Смотри на экран!\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "ЗАЩИТНЫЙ ПРОТОКОЛ АКТИВИРОВАН.\n";
    wait_ms(10000);

    std::cout << "АВТОМАТИЧЕСКАЯ БЛОКИРОВКА ЧЕРЕЗ 60 СЕКУНД.\n";
    wait_ms(10000);

    std::cout << "ОБНАРУЖЕН СКРЫТЫЙ ФАЙЛ: ECHO_SUBJECT_01.\n";
    wait_ms(6000);

    std::cout << "Марк: — Не открывай его. Мы не знаем, что внутри.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Но это может объяснить, почему система знает меня.\n";
    wait_ms(10000);

    std::cout << "На экране появляется сообщение:\n";
    wait_ms(10000);

    std::cout << "\"ЕСЛИ ТЫ ЧИТАЕШЬ ЭТО, "
        << player.name_of_character
        << ", ЗНАЧИТ, ПАМЯТЬ ЕЩЁ НЕ ВЕРНУЛАСЬ\".\n";
    wait_ms(8000);

    std::cout << "Под сообщением появляется подпись: L. MORROW.\n";
    wait_ms(6000);

    std::cout << "Марк: — Что за память? Что это вообще значит?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Я не знаю. Но Лея, похоже, знала, что это произойдёт.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "Терминал начинает издавать непрерывный сигнал.\n";
    wait_ms(10000);

    std::cout << "ЗАЩИТА ВОССТАНОВЛЕНА.\n";
    wait_ms(10000);

    std::cout << "СОЕДИНЕНИЕ ПРЕРВАНО.\n";
    wait_ms(10000);

    std::cout << "Марк выдёргивает кабель из терминала.\n";
    wait_ms(10000);

    std::cout << "Марк: — Всё. Больше не получится. "
        << "Система закрыла доступ.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Но мы нашли файл. Нужно понять, что такое ECHO_SUBJECT_01.\n";
    wait_ms(6000);

    std::cout << "Из динамика над дверью доносится женский голос.\n";
    wait_ms(10000);

    std::cout << "Лея: — Если ты слышишь меня, не верь записям в архиве.\n";
    wait_ms(6000);

    std::cout << "Лея: — И не позволяй Марку вести тебя вниз.\n";
    wait_ms(6000);

    std::cout << "Марк медленно поднимает голову к динамику.\n";
    wait_ms(10000);

    std::cout << "Марк: — Она пытается настроить тебя против меня.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Или пытается меня предупредить.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== Конец сцены: Чужой доступ ===\n";
    wait_ms(10000);

    std::cout << "Вы узнали о существовании файла ECHO_SUBJECT_01.\n";
    wait_ms(10000);

    std::cout << "Но можно ли теперь доверять Марку?\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Перейти во внешний контур.                          выбрать 1\n";
    std::cout << "Отправиться в архив исследований.                   выбрать 2\n";
    std::cout << "Спуститься на нижний уровень.                       выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_mark)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_mark = 0;
        }

        if (choice_ch04_mark < 1 || choice_ch04_mark > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_mark < 1 || choice_ch04_mark > 3);

    if (choice_ch04_mark == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Перейти во внешний контур.";
        wait_ms(10000);
    }
    else if (choice_ch04_mark == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Отправиться в архив исследований.";
        wait_ms(10000);
    }
    else if (choice_ch04_mark == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Спуститься на нижний уровень.";
        wait_ms(10000);
    }
}
//выбор игрокa: Попросить Марка отвлечь систему.
void show_ch03_mark_distract_system()
{
    clear_screen();

    std::cout << "=== Глава 3.2. Отвлекающий манёвр ===\n\n";
    wait_ms(10000);

    std::cout << "Аварийные лампы вспыхивают по всему коридору.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: НЕСАНКЦИОНИРОВАННЫЙ ДОСТУП.\n";
    std::cout << "СИСТЕМА: ПРОТОКОЛ ИЗОЛЯЦИИ АКТИВИРОВАН.\n\n";
    wait_ms(10000);

    std::cout << "За металлической дверью раздаётся ещё один удар.\n";
    std::cout << "На этот раз дверь заметно прогибается внутрь.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — У нас нет времени! Слушай внимательно.\n";
    wait_ms(10000);

    std::cout << "Марк указывает на два прохода.\n\n";
    wait_ms(10000);

    std::cout << "Слева находится технический коридор.\n";
    std::cout << "Он ведёт к старому лифту, но там нет освещения.\n\n";
    wait_ms(10000);

    std::cout << "Справа находится лестница на нижний уровень.\n";
    std::cout << "За дверью слышен тихий голос, произносящий твоё имя.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему кто-то зовёт меня оттуда?\n";
    wait_ms(10000);

    std::cout << "Марк: — Не знаю. Но если Лея говорила правду,\n";
    std::cout << "в нижнем архиве есть ответы. И, возможно, ловушка.\n\n";
    wait_ms(10000);

    std::cout << "Внезапно терминал включается сам.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: СУБЪЕКТ "
        << player.name_of_character << " ОБНАРУЖЕН.\n";
    std::cout << "СИСТЕМА: СОПРОВОЖДАЮЩИЙ МОЖЕТ БЫТЬ НЕБЕЗОПАСЕН.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Не слушай её! Она пытается нас разделить.\n";
    wait_ms(10000);

    std::cout << "Но ты замечаешь, что Марк смотрит на лестницу\n";
    std::cout << "так, словно уже бывал там раньше.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Решай. Сейчас.\n\n";
    wait_ms(10000);

    std::cout << "========================================\n";
    std::cout << "              ТВОЙ ВЫБОР\n";
    std::cout << "========================================\n\n";

    std::cout << "1. Довериться Марку и бежать к старому лифту.\n";
    std::cout << "   Попытаться выбраться из комплекса.\n\n";

    std::cout << "2. Не слушать Марка и спуститься по лестнице.\n";
    std::cout << "   Найти источник голоса и узнать правду.\n\n";

    do
    {
        std::cout << "Твой выбор: ";

        if (!(std::cin >> choice_ch04_distract))
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_ch04_distract = 0;
        }

        if (choice_ch04_distract < 1 || choice_ch04_distract > 2)
        {
            std::cout << "Неверный выбор. Введи 1 или 2.\n";
        }

    } while (choice_ch04_distract < 1 || choice_ch04_distract > 2);

    clear_screen();

    if (choice_ch04_distract == 1)
    {
        std::cout << "Ты решаешь довериться Марку.\n\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Хорошо. Веди меня к лифту.\n";
        wait_ms(10000);

        std::cout << "Марк кивает и первым бросается в технический коридор.\n";
        std::cout << "Ты следуешь за ним, пока позади закрываются двери.\n\n";
        wait_ms(10000);

        std::cout << "Впереди появляется тусклый свет старого лифта.\n";
        wait_ms(10000);

        std::cout << "Но когда Марк нажимает кнопку вызова,\n";
        std::cout << "на панели загорается надпись:\n\n";
        std::cout << "ДОСТУП РАЗРЕШЁН ТОЛЬКО СОТРУДНИКУ M-07.\n\n";
        wait_ms(10000);

        std::cout << "Марк замирает.\n\n";
        std::cout << player.name_of_character
            << ": — Марк... Что означает M-07?\n";
        wait_ms(10000);

        std::cout << "Марк: — Я объясню. Но сначала нам нужно выбраться.\n";
        wait_ms(10000);

        std::cout << "Двери лифта медленно открываются сами.\n";
        wait_ms(10000);

        std::cout << "=== Конец сцены: Старый лифт ===\n\n";
        wait_ms(10000);

        // Следующая сцена:
        // show_ch04_mark_old_elevator();
    }
    else if (choice_ch04_distract == 2)
    {
        std::cout << "Ты отступаешь от Марка и направляешься к лестнице.\n\n";
        wait_ms(10000);

        std::cout << "Марк: — Стой! Ты не понимаешь, что там находится!\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Тогда объясни, почему ты так боишься туда идти.\n";
        wait_ms(10000);

        std::cout << "Марк не отвечает.\n";
        std::cout << "Ты открываешь дверь и начинаешь спускаться.\n\n";
        wait_ms(10000);

        std::cout << "Голос становится отчётливее.\n";
        wait_ms(10000);

        std::cout << "НЕИЗВЕСТНЫЙ ГОЛОС:\n";
        std::cout << "\"Наконец-то ты вернулся.\"\n\n";
        wait_ms(10000);

        std::cout << "Внизу находится дверь с надписью:\n";
        std::cout << "КОМНАТА 01 — ПРОЕКТ ECHO.\n\n";
        wait_ms(10000);

        std::cout << "На панели появляется сообщение:\n";
        std::cout << "\"СУБЪЕКТ ПОДТВЕРЖДЁН. ОЖИДАНИЕ ОТКРЫТИЯ.\"\n\n";
        wait_ms(10000);

        std::cout << "За спиной слышатся шаги.\n";
        std::cout << "Марк всё-таки пошёл за тобой.\n\n";
        wait_ms(10000);

        std::cout << "Марк: — Если эта дверь откроется, назад пути может не быть.\n";
        wait_ms(10000);

        std::cout << "=== Конец сцены: Комната 01 ===\n\n";
        wait_ms(10000);

        // Следующая сцена:
        // show_ch04_mark_room_01();
    }

    wait_ms(10000);
}
//выбор игрокa: Принудительно открыть архив.
void show_ch03_mark_force_archive()
{
    clear_screen();
    std::cout << "=== Глава 3.3. Принудительное вскрытие ===\n";
    wait_ms(6000);

    std::cout << "" << player.name_of_character << " отходит от терминала и осматривает металлическую дверь архива.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Если система не даёт нам доступ, придётся открыть архив другим способом.\n";
    wait_ms(6000);

    std::cout << "Марк: — Ты серьёзно? Мы даже не знаем, что находится за этой дверью.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Именно поэтому я хочу туда попасть.\n";
    wait_ms(10000);

    std::cout << "Марк осматривает дверную раму и замечает старую сервисную панель.\n";
    wait_ms(6000);

    std::cout << "Марк: — Похоже, замок связан с резервным питанием. "
        << "Если отключить его, механизм может разблокироваться.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Тогда попробуем.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Марк открывает сервисную панель.\n";
    wait_ms(10000);

    std::cout << "Внутри видны старые кабели и повреждённый блок управления.\n";
    wait_ms(10000);

    std::cout << "Марк: — Здесь всё держится на честном слове. "
        << "Не понимаю, как это вообще ещё работает.\n";
    wait_ms(6000);

    std::cout << "Он отключает резервный модуль.\n";
    wait_ms(10000);

    std::cout << "Раздаётся громкий щелчок.\n";
    wait_ms(10000);

    std::cout << "ЗАМОК АРХИВА: АВАРИЙНЫЙ РЕЖИМ.\n";
    wait_ms(10000);

    std::cout << "Дверь слегка приоткрывается.\n";
    wait_ms(10000);

    std::cout << "Марк: — Получилось. Но нам лучше поторопиться.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему?\n";
    wait_ms(10000);

    std::cout << "Марк указывает на красный индикатор над дверью.\n";
    wait_ms(10000);

    std::cout << "Марк: — Мы отключили не только замок. "
        << "Кажется, это запустило аварийный сигнал.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "ВНИМАНИЕ.\n";
    wait_ms(10000);

    std::cout << "НАРУШЕНИЕ ЦЕЛОСТНОСТИ АРХИВНОГО БЛОКА.\n";
    wait_ms(10000);

    std::cout << "СЛУЖБА БЕЗОПАСНОСТИ УВЕДОМЛЕНА.\n";
    wait_ms(10000);

    std::cout << "ДО АВТОМАТИЧЕСКОЙ БЛОКИРОВКИ КОМПЛЕКСА: 120 СЕКУНД.\n";
    wait_ms(10000);

    std::cout << "Марк: — Отлично. Теперь у нас две минуты, прежде чем "
        << "комплекс окончательно закроется.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Тогда не будем терять время.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Вы протискиваетесь внутрь архива.\n";
    wait_ms(10000);

    std::cout << "В помещении пахнет пылью и чем-то химическим.\n";
    wait_ms(10000);

    std::cout << "Вдоль стен стоят металлические шкафы. "
        << "Большинство ящиков открыты и пусты.\n";
    wait_ms(6000);

    std::cout << "Марк: — Кто-то уже побывал здесь до нас.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Или пытался уничтожить то, что здесь хранилось.\n";
    wait_ms(10000);

    std::cout << "В дальнем углу вы замечаете стол с папками и старым диктофоном.\n";
    wait_ms(6000);

    std::cout << "На одной из папок написано: ECHO — SUBJECT 01.\n";
    wait_ms(6000);

    std::cout << "Марк резко останавливается.\n";
    wait_ms(10000);

    std::cout << "Марк: — Не трогай эту папку.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему? Ты знаешь, что это такое?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я... не уверен.\n";
    wait_ms(10000);

    std::cout << "Впервые за всё время Марк выглядит по-настоящему испуганным.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "На столе лежит старая фотография.\n";
    wait_ms(10000);

    std::cout << "На ней изображены несколько сотрудников комплекса "
        << "и человек, чьё лицо перечёркнуто чёрным маркером.\n";
    wait_ms(10000);

    std::cout << "На обратной стороне фотографии написано:\n";
    wait_ms(10000);

    std::cout << "\"ПЕРВЫЙ ЭТАП ЗАВЕРШЁН. ПАМЯТЬ УДАЛЕНА УСПЕШНО\".\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Память удалена? Кому её удалили?\n";
    wait_ms(10000);

    std::cout << "Марк ничего не отвечает.\n";
    wait_ms(10000);

    std::cout << "Из коридора доносится металлический грохот.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА БЛОКИРОВКИ АКТИВИРОВАНА.\n";
    wait_ms(10000);

    std::cout << "Марк: — Нам нужно уходить. Немедленно!\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Подожди! Мы ещё ничего не выяснили!\n";
    wait_ms(10000);

    std::cout << "Марк хватает папку ECHO — SUBJECT 01 и бросает её тебе.\n";
    wait_ms(6000);

    std::cout << "Марк: — Забирай её! Всё остальное потом!\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "Вы выбегаете из архива за несколько секунд до блокировки двери.\n";
    wait_ms(6000);

    std::cout << "За спиной раздаётся тяжёлый удар. Архив запечатан.\n";
    wait_ms(10000);

    std::cout << "Марк тяжело дышит, прислонившись к стене.\n";
    wait_ms(10000);

    std::cout << "Марк: — Мы не должны были открывать эту дверь.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Но теперь у нас есть папка. И ты явно знаешь больше, чем говоришь.\n";
    wait_ms(10000);

    std::cout << "Марк смотрит на папку, затем на тебя.\n";
    wait_ms(10000);

    std::cout << "Марк: — Когда откроешь её, возможно, пожалеешь об этом.\n";
    wait_ms(6000);

    std::cout << "Внезапно из динамика раздаётся голос Леи:\n";
    wait_ms(10000);

    std::cout << "Лея: — Ты наш" << G("ёл", "ла") << " документы. Значит, уже слишком поздно.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Лея! Что означает «память удалена»?\n";
    wait_ms(10000);

    std::cout << "Несколько секунд слышны только помехи.\n";
    wait_ms(10000);

    std::cout << "Лея: — Не открывай последнюю страницу. Что бы ни случилось.\n";
    wait_ms(6000);

    std::cout << "Связь обрывается.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Конец сцены: Принудительное вскрытие ===\n";
    wait_ms(10000);

    std::cout << "Вы получили папку ECHO — SUBJECT 01.\n";
    wait_ms(10000);

    std::cout << "Вы узнали, что в комплексе проводились эксперименты, "
        << "связанные с человеческой памятью.\n";
    wait_ms(6000);

    std::cout << "Но почему Марк испугался этой папки?\n";
    wait_ms(10000);

    std::cout << "И что скрывается на последней странице?\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "=== Конец сцены: Принудительное вскрытие ===\n";
    wait_ms(10000);

    std::cout << "Вы получили папку ECHO — SUBJECT 01.\n";
    wait_ms(10000);

    std::cout << "Вы узнали, что в комплексе проводились эксперименты, "
        << "связанные с человеческой памятью.\n";
    wait_ms(6000);

    std::cout << "Но почему Марк испугался этой папки?\n";
    wait_ms(10000);

    std::cout << "И что скрывается на последней странице?\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Открыть папку ECHO и прочитать последнюю страницу.   выбрать 1\n";
    std::cout << "Послушать Лею и пока не открывать последнюю страницу. выбрать 2\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_force_archive)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_force_archive = 0;
        }

        if (choice_ch04_force_archive < 1 ||
            choice_ch04_force_archive > 2) {
            std::cout << "Некорректный выбор. Введите 1 или 2: ";
        }
    } while (choice_ch04_force_archive < 1 ||
        choice_ch04_force_archive > 2);

    if (choice_ch04_force_archive == 1) {
        clear_screen();

        std::cout << "Вы выбрали: Открыть папку ECHO и прочитать последнюю страницу.";
        wait_ms(10000);
    }
    else if (choice_ch04_force_archive == 2) {
        clear_screen();

        std::cout << "Вы выбрали: Послушать Лею и пока не открывать последнюю страницу.";
        wait_ms(10000);
    }
}


//-------------------------------3глава выбор 2 проверка камер с марком-----------

//выбор игрокa: Извлечь карту памяти. 
void show_ch03_mark_extract_memory_card()
{
    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "              ГЛАВА 3. КАРТА ПАМЯТИ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Камера наблюдения продолжает медленно вращаться под потолком.\n";
    std::cout << "Зелёный индикатор возле двери мигает в темноте.\n\n";
    wait_ms(10000);

    std::cout << "Марк подходит к панели камеры и снимает защитную крышку.\n";
    std::cout << "Внутри находится небольшой слот для карты памяти.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Если запись ещё не перезаписана, мы сможем узнать,\n";
    std::cout << "что произошло здесь в 03:17.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — А если система заметит, что мы извлекли карту?\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Тогда у нас будет примерно пять секунд,\n";
    std::cout << "чтобы понять, насколько сильно мы облажались.\n\n";
    wait_ms(10000);

    std::cout << "Он осторожно нажимает на край карты.\n";
    std::cout << "Раздаётся тихий щелчок.\n\n";
    wait_ms(10000);

    std::cout << "Ты извлекаешь карту памяти из камеры.\n";
    std::cout << "Индикатор камеры мгновенно гаснет.\n\n";
    wait_ms(10000);

    std::cout << "На панели появляется сообщение:\n\n";
    std::cout << "          CAMERA CONNECTION LOST\n";
    std::cout << "          SECURITY PROTOCOL ACTIVE\n\n";
    wait_ms(6000);

    std::cout << "По коридору разносится короткий металлический звук.\n";
    std::cout << "Где-то за стеной включается механизм блокировки.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Чёрт. Она успела отправить сигнал.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что теперь?\n\n";
    wait_ms(10000);

    std::cout << "Марк подключает карту к небольшому считывателю.\n";
    std::cout << "На экране появляются повреждённые файлы.\n\n";
    wait_ms(10000);

    std::cout << "НАЙДЕНЫ ФАЙЛЫ:\n";
    std::cout << "CAM07_0317_REC\n";
    std::cout << "CAM07_BACKUP\n";
    std::cout << "ECHO07_UNKNOWN\n\n";
    wait_ms(10000);

    std::cout << "Марк открывает первый файл.\n";
    std::cout << "Изображение дрожит, а звук почти полностью отсутствует.\n\n";
    wait_ms(10000);

    std::cout << "На записи коридор пуст.\n";
    std::cout << "Время: 03:17:39.\n\n";
    wait_ms(10000);

    std::cout << "Через три секунды изображение искажается.\n";
    std::cout << "На дальнем конце коридора появляется силуэт.\n\n";
    wait_ms(10000);

    std::cout << "Он стоит неподвижно и смотрит прямо в объектив.\n";
    std::cout << "Затем изображение на мгновение становится чёрным.\n\n";
    wait_ms(10000);

    std::cout << "Когда картинка возвращается, коридор снова пуст.\n";
    std::cout << "Но на стекле объектива проступает надпись:\n\n";
    std::cout << "                  ECHO-07\n\n";
    wait_ms(6000);

    std::cout << "Марк отодвигается от считывателя.\n";
    std::cout << "Марк: — Это не похоже на обычную запись с камеры.\n\n";
    wait_ms(10000);

    std::cout << "Внезапно экран гаснет.\n";
    std::cout << "На панели считывателя появляется новая строка:\n\n";
    std::cout << "          EXTERNAL ACCESS DETECTED\n";
    std::cout << "          LOCATION: UNKNOWN\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Мы ведь просто извлекли карту. Как система узнала?\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Возможно, камера была не единственным устройством,\n";
    std::cout << "которое следило за этим коридором.\n\n";
    wait_ms(6000);

    std::cout << "Из динамика над дверью раздаётся искажённый голос:\n\n";
    std::cout << "НЕИЗВЕСТНЫЙ ГОЛОС: — Верните запись на место.\n\n";
    wait_ms(6000);

    std::cout << "Дверной замок щёлкает.\n";
    std::cout << "Зелёный индикатор становится красным.\n\n";
    wait_ms(10000);

    std::cout << "Марк сжимает карту памяти в руке.\n";
    std::cout << "Марк: — Теперь у нас есть доказательство.\n";
    std::cout << "Но, похоже, мы стали частью этой записи.\n\n";
    wait_ms(6000);

    player.evidence++;

    std::cout << "Ты сохранил" << G("", "а") << " карту памяти. Найдено улик: "
        << player.evidence << ".\n\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Просмотреть резервную копию записи.                  выбрать 1\n";
    std::cout << "Попытаться открыть файл ECHO07_UNKNOWN.              выбрать 2\n";
    std::cout << "Найти способ отключить блокировку двери.             выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_memory_card)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_memory_card = 0;
        }

        if (choice_memory_card < 1 || choice_memory_card > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_memory_card < 1 || choice_memory_card > 3);

    if (choice_memory_card == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Просмотреть резервную копию записи.";
        wait_ms(10000);
    }
    else if (choice_memory_card == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Попытаться открыть файл ECHO07_UNKNOWN.";
        wait_ms(10000);
    }
    else if (choice_memory_card == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Найти способ отключить блокировку двери.";
        wait_ms(10000);
    }
}
//выбор игрокa: Проверить резервное питание 
void show_ch03_mark_backup_power()
{
    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "             ГЛАВА 3. РЕЗЕРВНОЕ ПИТАНИЕ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты снова подходишь к камере наблюдения.\n";
    std::cout << "Марк внимательно изучает панель под ней.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — У каждой камеры есть основной источник питания.\n";
    std::cout << "Но у этой модели должен быть и резервный.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Хочешь проверить, откуда она получает электричество?\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Именно. Если найдём резервную линию,\n";
    std::cout << "сможем понять, куда уходит сигнал с камеры.\n\n";
    wait_ms(10000);

    std::cout << "Ты открываешь металлическую крышку панели.\n";
    std::cout << "Внутри видны несколько проводов и маленький аккумулятор.\n\n";
    wait_ms(10000);

    std::cout << "На аккумуляторе наклеена выцветшая маркировка:\n\n";
    std::cout << "          BACKUP UNIT 07\n";
    std::cout << "          STATUS: ACTIVE\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему резервное питание активно,\n";
    std::cout << "если основная система работает нормально?\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Хороший вопрос. Возможно, основная линия\n";
    std::cout << "вообще не подключена к этой камере.\n\n";
    wait_ms(10000);

    std::cout << "Марк подсвечивает провода фонариком.\n";
    std::cout << "Один из них уходит глубоко внутрь стены.\n";
    std::cout << "На изоляции видна надпись: ECHO-07.\n\n";
    wait_ms(6000);

    std::cout << "Ты прослеживаешь провод до небольшого разъёма.\n";
    std::cout << "Рядом мигает красный индикатор.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Не трогай его пока.\n";
    std::cout << "Похоже, эта линия питает не только камеру.\n\n";
    wait_ms(10000);

    std::cout << "Внезапно свет в коридоре начинает мерцать.\n";
    std::cout << "На несколько секунд всё погружается в темноту.\n\n";
    wait_ms(10000);

    std::cout << "Из панели доносится тихий щелчок.\n";
    std::cout << "На маленьком экране появляются цифры:\n\n";
    std::cout << "          AUXILIARY POWER: 98%\n";
    std::cout << "          SIGNAL: TRANSMITTING\n";
    std::cout << "          DESTINATION: UNKNOWN\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Сигнал передаётся куда-то за пределы комплекса?\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Или в место, о котором нам не хотят рассказывать.\n\n";
    wait_ms(10000);

    std::cout << "Ты замечаешь второй кабель, спрятанный за аккумулятором.\n";
    std::cout << "Он ведёт к небольшому блоку с механическим переключателем.\n\n";
    wait_ms(10000);

    std::cout << "На переключателе две надписи:\n\n";
    std::cout << "          NORMAL\n";
    std::cout << "          EMERGENCY\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Если переключить его, можно узнать,\n";
    std::cout << "что именно подключено к аварийной линии.\n";
    std::cout << "Но я не знаю, как система на это отреагирует.\n\n";
    wait_ms(6000);

    std::cout << "В этот момент динамик над дверью оживает.\n";
    std::cout << "Сначала слышны помехи, затем тихий шёпот:\n\n";
    std::cout << "НЕИЗВЕСТНЫЙ ГОЛОС: — Не отключайте питание.\n\n";
    wait_ms(6000);

    std::cout << "Марк резко поворачивается к динамику.\n";
    std::cout << "Марк: — Ты это слышал" << G("", "а") << "?\n\n";
    wait_ms(10000);

    std::cout << "Голос повторяет сообщение, но теперь звучит отчётливее:\n\n";
    std::cout << "НЕИЗВЕСТНЫЙ ГОЛОС: — Система должна оставаться активной.\n\n";
    wait_ms(6000);

    std::cout << "Экран панели гаснет.\n";
    std::cout << "Когда он включается снова, появляется новая строка:\n\n";
    std::cout << "          OBSERVER DETECTED\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Она знает, что мы здесь.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Тогда у нас мало времени.\n";
    std::cout << "Нужно решить, что делать с этой линией.\n\n";
    wait_ms(10000);

    std::cout << "Ты обнаружил" << G("", "а") << " резервный источник питания камеры.\n";
    std::cout << "Найдено улик: " << player.evidence << ".\n\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Переключить питание на аварийный режим.             выбрать 1\n";
    std::cout << "Проследить кабель внутрь стены.                     выбрать 2\n";
    std::cout << "Оставить панель в покое и уйти к двери.             выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_backup_power)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_backup_power = 0;
        }

        if (choice_backup_power < 1 || choice_backup_power > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_backup_power < 1 || choice_backup_power > 3);

    if (choice_backup_power == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Переключить питание на аварийный режим.";
        wait_ms(10000);
    }
    else if (choice_backup_power == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Проследить кабель внутрь стены.";
        wait_ms(10000);
    }
    else if (choice_backup_power == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Оставить панель в покое и уйти к двери.";
        wait_ms(10000);
    }
}
//выбор игрокa: Осмотреть слепую зону.  
void show_ch03_mark_blind_spot()
{
    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                ГЛАВА 3. СЛЕПАЯ ЗОНА\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты отходишь от панели камеры и внимательно осматриваешь коридор.\n";
    std::cout << "Марк смотрит на изображение с камеры на небольшом терминале.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Вот здесь. Видишь этот участок?\n";
    std::cout << "Камера поворачивается, но часть коридора остаётся вне кадра.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Слишком большой участок для обычной ошибки установки.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Именно. Кто-то специально оставил это место невидимым.\n\n";
    wait_ms(10000);

    std::cout << "Ты направляешь фонарик на стену под камерой.\n";
    std::cout << "Сначала ничего необычного не видно.\n";
    std::cout << "Но возле пола ты замечаешь длинную царапину.\n\n";
    wait_ms(10000);

    std::cout << "Царапина заканчивается у металлической панели.\n";
    std::cout << "На её поверхности почти стёрта маркировка:\n\n";
    std::cout << "                  SERVICE ACCESS 07\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк, подойди сюда. Кажется, я что-то " << G("нашёл", "нашла") << ".\n\n";
    wait_ms(10000);

    std::cout << "Марк приседает рядом с панелью и проводит пальцами по краю.\n";
    std::cout << "Между металлом и стеной видна узкая щель.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Это не часть обычной стены.\n";
    std::cout << "За ней может проходить технический канал.\n\n";
    wait_ms(10000);

    std::cout << "Ты осторожно надавливаешь на край панели.\n";
    std::cout << "Раздаётся глухой щелчок.\n";
    std::cout << "Панель немного отходит от стены.\n\n";
    wait_ms(10000);

    std::cout << "За ней обнаруживается узкий тёмный проход.\n";
    std::cout << "Внутри тянутся кабели, а на полу лежит слой пыли.\n\n";
    wait_ms(10000);

    std::cout << "Но посреди пыли видны свежие следы обуви.\n";
    std::cout << "Они ведут вглубь прохода.\n\n";
    wait_ms(6000);

    std::cout << "Марк: — Здесь кто-то был совсем недавно.\n";
    std::cout << "И он знал, что камера не сможет его увидеть.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Значит, слепая зона нужна не для защиты.\n";
    std::cout << "Кто-то использует её, чтобы оставаться незамеченным.\n\n";
    wait_ms(10000);

    std::cout << "Ты осматриваешь внутреннюю сторону панели.\n";
    std::cout << "На металле обнаруживается небольшая наклейка.\n\n";
    std::cout << "          MAINTENANCE: 03:17\n";
    std::cout << "          ACCESS ID: ECHO-07\n\n";
    wait_ms(6000);

    std::cout << "Марк медленно поднимает взгляд.\n";
    std::cout << "Марк: — Опять ECHO-07...\n\n";
    wait_ms(10000);

    std::cout << "Внезапно камера над вами перестаёт вращаться.\n";
    std::cout << "Её объектив медленно направляется прямо на вас.\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Она не должна видеть нас под таким углом.\n\n";
    wait_ms(10000);

    std::cout << "На терминале появляется сообщение:\n\n";
    std::cout << "          BLIND SPOT COMPROMISED\n";
    std::cout << "          SUBJECTS IDENTIFIED: 2\n\n";
    wait_ms(6000);

    std::cout << "Ты слышишь тихий звук из глубины прохода.\n";
    std::cout << "Будто кто-то задел металлический кабель.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Там кто-то есть.\n\n";
    wait_ms(10000);

    std::cout << "Марк выключает фонарик.\n";
    std::cout << "Несколько секунд вы стоите в полной темноте.\n\n";
    wait_ms(10000);

    std::cout << "Из прохода доносится три коротких удара.\n";
    std::cout << "Затем тишина.\n\n";
    wait_ms(6000);

    std::cout << "Марк шепчет:\n";
    std::cout << "Марк: — Мы можем проверить проход.\n";
    std::cout << "Но если там кто-то есть, он уже знает, что мы его нашли.\n\n";
    wait_ms(6000);

    std::cout << "Ты запоминаешь расположение скрытой панели.\n";
    std::cout << "Найдено улик: " << player.evidence << ".\n\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Зайти в технический проход вместе с Марком.         выбрать 1\n";
    std::cout << "Осмотреть следы обуви и кабели.                     выбрать 2\n";
    std::cout << "Закрыть панель и вернуться к двери.                 выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_blind_spot)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_blind_spot = 0;
        }

        if (choice_blind_spot < 1 || choice_blind_spot > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_blind_spot < 1 || choice_blind_spot > 3);

    if (choice_blind_spot == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Зайти в технический проход вместе с Марком.";
        wait_ms(10000);
    }
    else if (choice_blind_spot == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Осмотреть следы обуви и кабели.";
        wait_ms(10000);
    }
    else if (choice_blind_spot == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Закрыть панель и вернуться к двери.";
        wait_ms(10000);
    }
}

//-------------------------------3глава выбор 3 выбить дверь с марком-----------------------

//выбор игрокa: Вместе выбить дверь.
void show_ch03_mark_break_door()
{
    clear_screen();
    std::cout << "=== Нокс-7. Вместе выбить дверь ===\n\n";
    wait_ms(10000);

    std::cout << "Марк подходит к массивной металлической двери.\n";
    std::cout << "Он осматривает раму и проводит рукой по краю замка.\n";
    wait_ms(6000);

    std::cout << "Марк: — Она тяжёлая. В одиночку ты бы её не открыл.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Значит, будем выбивать вместе.\n";
    wait_ms(10000);

    std::cout << "Марк отступает на несколько шагов и встаёт рядом с тобой.\n";
    std::cout << "Вы смотрите друг на друга и одновременно киваете.\n";
    wait_ms(6000);

    std::cout << "Марк: — На счёт три. Раз... два... ТРИ!\n";
    wait_ms(10000);

    std::cout << "Вы одновременно ударяете плечами по двери.\n";
    std::cout << "Раздаётся оглушительный металлический грохот.\n";
    wait_ms(6000);

    std::cout << "Дверь слегка вздрагивает, но замок удерживает её.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ещё раз!\n";
    wait_ms(10000);

    std::cout << "Вы отходите назад и снова бросаетесь на дверь.\n";
    std::cout << "На этот раз сверху осыпается пыль, а рама скрипит.\n";
    wait_ms(6000);

    std::cout << "Марк: — Ещё один удар! Она почти поддалась!\n";
    wait_ms(10000);

    std::cout << "Вы собираетесь ударить снова, но внезапно гаснет свет.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Протокол тревоги ===\n\n";
    wait_ms(10000);

    std::cout << "В темноте слышно, как внутри двери срабатывает механизм.\n";
    std::cout << "По коридору прокатывается низкий гул.\n";
    wait_ms(6000);

    std::cout << "На панели доступа появляется красная надпись:\n\n";
    std::cout << "          UNAUTHORIZED FORCE DETECTED\n";
    std::cout << "          SECURITY PROTOCOL: ECHO-07\n\n";
    wait_ms(6000);

    std::cout << "Марк: — Отлично. Теперь система точно знает, что мы здесь.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Откуда она вообще получает информацию?\n";
    wait_ms(10000);

    std::cout << "Марк: — Не знаю. Но если продолжим шуметь,\n";
    std::cout << "можем привлечь кого-то ещё.\n";
    wait_ms(6000);

    std::cout << "Внезапно за дверью раздаётся тяжёлый удар.\n";
    wait_ms(10000);

    std::cout << "Вы оба замираете.\n";
    wait_ms(10000);

    std::cout << "Затем из-за двери доносится медленный скрежет,\n";
    std::cout << "словно что-то большое перемещается по ту сторону.\n";
    wait_ms(10000);

    std::cout << "Марк отступает на шаг.\n";
    std::cout << "Марк: — Скажи мне, что это был механизм двери.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Очень хочется в это верить.\n";
    wait_ms(10000);

    std::cout << "Гул прекращается.\n";
    std::cout << "На панели загорается зелёный индикатор.\n";
    wait_ms(10000);

    std::cout << "Раздаётся громкий щелчок.\n";
    std::cout << "Один из замков медленно отходит в сторону.\n";
    wait_ms(6000);

    std::cout << "Марк: — Подожди... мы всё-таки её сдвинули!\n";
    wait_ms(10000);

    std::cout << "Вы наваливаетесь на дверь в последний раз.\n";
    std::cout << "Металл скрипит, и дверь открывается на несколько сантиметров.\n";
    wait_ms(6000);

    std::cout << "Изнутри тянет холодным воздухом.\n";
    std::cout << "За дверью виден длинный коридор с тусклыми лампами.\n";
    wait_ms(6000);

    std::cout << "На стене напротив кто-то оставил надпись:\n\n";
    std::cout << "                  ECHO-07\n";
    std::cout << "             НЕ ВПУСКАЙТЕ ЕГО\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк... ты тоже это видишь?\n";
    wait_ms(10000);

    std::cout << "Марк молча смотрит на надпись.\n";
    std::cout << "Впервые за всё время он выглядит по-настоящему испуганным.\n";
    wait_ms(6000);

    std::cout << "Марк: — Да. И мне совсем не нравится то, что здесь написано.\n";
    wait_ms(6000);

    std::cout << "Из динамика над дверью доносится искажённый голос:\n";
    wait_ms(10000);

    std::cout << "Неизвестный голос: — Доступ подтверждён.\n";
    wait_ms(10000);

    std::cout << "Пауза.\n";
    wait_ms(10000);

    std::cout << "Неизвестный голос: — Добро пожаловать обратно.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Обратно? Мы здесь никогда не были.\n";
    wait_ms(6000);

    std::cout << "Динамик выключается.\n";
    std::cout << "Дверь остаётся приоткрытой, открывая путь внутрь комплекса.\n";
    wait_ms(6000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Войти в коридор вместе с Марком.                    выбрать 1\n";
    std::cout << "Осмотреть надпись на стене.                         выбрать 2\n";
    std::cout << "Проверить панель доступа.                           выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_break_door)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_break_door = 0;
        }

        if (choice_break_door < 1 || choice_break_door > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_break_door < 1 || choice_break_door > 3);

    if (choice_break_door == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Войти в коридор вместе с Марком.";
        wait_ms(10000);
    }
    else if (choice_break_door == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Осмотреть надпись на стене.";
        wait_ms(10000);
    }
    else if (choice_break_door == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Проверить панель доступа.";
        wait_ms(10000);
    }
}
//выбор игрокa: Найти аварийный механизм.   
void show_ch03_mark_emergency_mechanism()
{
    clear_screen();
    std::cout << "=== Нокс-7. Аварийный механизм ===\n\n";
    wait_ms(10000);

    std::cout << "Ты отходишь от двери и начинаешь осматривать стены рядом с ней.\n";
    std::cout << "Марк проверяет панель доступа, пытаясь найти способ открыть замок.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Должен же здесь быть какой-то аварийный механизм.\n";
    wait_ms(10000);

    std::cout << "Марк: — Если система отключится, дверь должна открываться вручную.\n";
    std::cout << "Вопрос только в том, где спрятан этот механизм.\n";
    wait_ms(6000);

    std::cout << "Ты проводишь рукой по металлической раме.\n";
    std::cout << "Под слоем пыли обнаруживается небольшая прямоугольная крышка.\n";
    wait_ms(10000);

    std::cout << "На крышке едва различима надпись:\n\n";
    std::cout << "          EMERGENCY RELEASE\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк, кажется, я " << G("нашёл", "нашла") << ".\n";
    wait_ms(10000);

    std::cout << "Марк подходит ближе и осматривает крышку.\n";
    std::cout << "Марк: — Не торопись. Такие механизмы могут быть заблокированы.\n";
    wait_ms(10000);

    std::cout << "Ты осторожно открываешь крышку.\n";
    std::cout << "За ней находится металлический рычаг и небольшой переключатель.\n";
    wait_ms(6000);

    std::cout << "Рядом нанесена инструкция:\n\n";
    std::cout << "          1. DISABLE LOCK\n";
    std::cout << "          2. PULL RELEASE\n";
    std::cout << "          WARNING: SECURITY OVERRIDE\n\n";
    wait_ms(6000);

    std::cout << "Марк: — Сначала нужно отключить замок.\n";
    std::cout << "Если потянуть рычаг сразу, можно только усилить блокировку.\n";
    wait_ms(6000);

    std::cout << "Ты переключаешь небольшой тумблер.\n";
    std::cout << "Внутри стены раздаётся низкий гул.\n";
    wait_ms(10000);

    std::cout << "На панели доступа загорается жёлтый индикатор.\n";
    std::cout << "На экране появляется сообщение:\n\n";
    std::cout << "          MANUAL OVERRIDE REQUESTED\n";
    std::cout << "          AUTHORIZATION REQUIRED\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Даже ручной механизм требует разрешения?\n";
    wait_ms(10000);

    std::cout << "Марк: — Это уже не обычная система безопасности.\n";
    std::cout << "Кто-то сделал так, чтобы дверь нельзя было открыть без разрешения.\n";
    wait_ms(6000);

    std::cout << "Внезапно экран начинает мигать.\n";
    std::cout << "Строка с запросом разрешения исчезает.\n";
    wait_ms(10000);

    std::cout << "Вместо неё появляется новая надпись:\n\n";
    std::cout << "          ECHO-07 OVERRIDE\n";
    std::cout << "          OPERATOR: UNKNOWN\n";
    std::cout << "          ACCESS: GRANTED\n\n";
    wait_ms(10000);

    std::cout << "Марк отступает от панели.\n";
    std::cout << "Марк: — Я ничего не вводил.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — И я тоже.\n";
    wait_ms(10000);

    std::cout << "За дверью срабатывает тяжёлый механизм.\n";
    std::cout << "Металлические замки один за другим начинают отходить.\n";
    wait_ms(6000);

    std::cout << "Но прежде чем дверь успевает открыться, гул резко прекращается.\n";
    wait_ms(10000);

    std::cout << "На панели появляется последнее сообщение:\n\n";
    std::cout << "          MANUAL RELEASE SUSPENDED\n";
    std::cout << "          PLEASE WAIT...\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Она сама остановила механизм.\n";
    wait_ms(10000);

    std::cout << "Марк: — Или кто-то остановил его отсюда.\n";
    wait_ms(10000);

    std::cout << "Ты смотришь на рычаг.\n";
    std::cout << "Он по-прежнему доступен, но теперь слегка вибрирует.\n";
    wait_ms(10000);

    std::cout << "Из динамика над дверью доносится спокойный голос:\n";
    wait_ms(10000);

    std::cout << "Неизвестный голос: — Аварийное открытие не требуется.\n";
    wait_ms(10000);

    std::cout << "Неизвестный голос: — Вы уже находитесь там, где должны быть.\n";
    wait_ms(10000);

    std::cout << "Марк смотрит на тебя.\n";
    std::cout << "Марк: — Мне всё меньше нравится это место.\n";
    wait_ms(10000);

    std::cout << "Дверь остаётся закрытой.\n";
    std::cout << "Но теперь ты знаешь, что аварийный механизм связан с ECHO-07.\n";
    wait_ms(6000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Потянуть аварийный рычаг.                           выбрать 1\n";
    std::cout << "Попытаться отключить питание механизма.             выбрать 2\n";
    std::cout << "Отойти от двери и поговорить с Марком.              выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_emergency)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_emergency = 0;
        }

        if (choice_emergency < 1 || choice_emergency > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_emergency < 1 || choice_emergency > 3);

    if (choice_emergency == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Потянуть аварийный рычаг.";
        wait_ms(10000);
    }
    else if (choice_emergency == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Попытаться отключить питание механизма.";
        wait_ms(10000);
    }
    else if (choice_emergency == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Отойти от двери и поговорить с Марком.";
        wait_ms(10000);
    }
}
//выбор игрокa: Оставить Марка снаружи.  //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void show_ch03_leave_mark_outside()
{
    clear_screen();
    std::cout << "=== Нокс-7. По разные стороны ===\n\n";
    wait_ms(10000);

    std::cout << "Ты смотришь на приоткрытую дверь и затем поворачиваешься к Марку.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Останься здесь. Я проверю, что находится внутри.\n";
    wait_ms(10000);

    std::cout << "Марк удивлённо смотрит на тебя.\n";
    std::cout << "Марк: — Ты серьёзно? После всего, что здесь происходит?\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Если что-то случится, кто-то должен остаться снаружи.\n";
    wait_ms(10000);

    std::cout << "Марк некоторое время молчит, затем тяжело вздыхает.\n";
    std::cout << "Марк: — Ладно. Но не геройствуй там в одиночку.\n";
    wait_ms(6000);

    std::cout << "Ты осторожно проходишь через приоткрытую дверь.\n";
    std::cout << "За ней тянется узкий коридор с тусклыми потолочными лампами.\n";
    wait_ms(6000);

    std::cout << "Ты оборачиваешься. Марк всё ещё стоит у входа.\n";
    std::cout << "Он поднимает руку, показывая, что остаётся на месте.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Я скоро вернусь.\n";
    wait_ms(10000);

    std::cout << "Ты делаешь ещё несколько шагов внутрь.\n";
    std::cout << "Внезапно за спиной раздаётся металлический грохот.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Блокировка ===\n\n";
    wait_ms(10000);

    std::cout << "Ты резко оборачиваешься.\n";
    std::cout << "Массивная дверь начинает закрываться сама по себе.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Марк! Отойди от двери!\n";
    wait_ms(10000);

    std::cout << "Марк бросается к панели управления, но экран уже погас.\n";
    wait_ms(10000);

    std::cout << "Марк: — Она не реагирует! Я ничего не могу сделать!\n";
    wait_ms(10000);

    std::cout << "Ты пытаешься вернуться, но дверь захлопывается прямо перед тобой.\n";
    wait_ms(6000);

    std::cout << "Раздаётся тяжёлый щелчок замка.\n";
    std::cout << "На внутренней панели загорается красный индикатор.\n";
    wait_ms(10000);

    std::cout << "На экране появляется сообщение:\n\n";
    std::cout << "          SECTOR SEALED\n";
    std::cout << "          EXTERNAL ACCESS DENIED\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Марк! Ты меня слышишь?\n";
    wait_ms(10000);

    std::cout << "Несколько секунд ничего не происходит.\n";
    std::cout << "Затем из динамика раздаётся голос Марка.\n";
    wait_ms(10000);

    std::cout << "Марк: — Слышу. Попробуй найти другой выход.\n";
    std::cout << "Я попробую открыть дверь снаружи.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Не уходи далеко. Я найду способ выбраться.\n";
    wait_ms(10000);

    std::cout << "Марк: — Хорошо. И ещё кое-что...\n";
    wait_ms(10000);

    std::cout << "Его голос внезапно искажается помехами.\n";
    wait_ms(10000);

    std::cout << "Марк: — Если увидишь кого-нибудь в коридоре...\n";
    wait_ms(10000);

    std::cout << "Связь обрывается.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Марк? Марк, ответь!\n";
    wait_ms(10000);

    std::cout << "В ответ раздаётся только тихий треск динамика.\n";
    wait_ms(10000);

    std::cout << "Ты поворачиваешься к коридору.\n";
    std::cout << "В дальнем конце на секунду появляется человеческий силуэт.\n";
    wait_ms(6000);

    std::cout << "Ты моргаешь — и силуэт исчезает.\n";
    wait_ms(10000);

    std::cout << "На стене рядом с дверью загорается маленький экран.\n";
    std::cout << "На нём появляется надпись:\n\n";
    std::cout << "          SUBJECT ISOLATED\n";
    std::cout << "          ECHO-07 PROTOCOL CONTINUES\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Значит, именно этого они и добивались...\n";
    wait_ms(6000);

    std::cout << "Ты " << G("остался один", "осталась одна") << " внутри комплекса.\n";
    std::cout << "Марк находится по другую сторону двери, а впереди тебя ждёт тёмный коридор.\n";
    wait_ms(6000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(10000);

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Осмотреть коридор и найти другой выход.              выбрать 1\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_leave_mark)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_leave_mark = 0;
        }

        if (choice_leave_mark < 1 || choice_leave_mark > 1) {
            std::cout << "Некорректный выбор. Введите 1";
        }
    } while (choice_leave_mark < 1 || choice_leave_mark > 3);

    if (choice_leave_mark == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Осмотреть коридор и найти другой выход.";
        wait_ms(10000);
    }
}

//-------------------------------3глава выбор 4 самому взломать индификатор------

//выбор игрокa: Подобрать идентификатор.
void show_ch03_solo_hack_identifier() {
    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                 ГЛАВА 3.4. ЧУЖОЙ ИДЕНТИФИКАТОР\n";
    std::cout << "==============================================================\n\n";

    wait_ms(10000);

    std::cout << "Терминал продолжает гудеть. На экране мигает сообщение:\n";
    std::cout << "\"ДОСТУП ЗАПРЕЩЁН. ТРЕБУЕТСЯ ИДЕНТИФИКАЦИЯ СОТРУДНИКА.\"\n\n";

    wait_ms(10000);

    std::cout << "Ты осматриваешь рабочее место. На столе лежат несколько\n";
    std::cout << "сломанных электронных компонентов, пустая чашка и стопка\n";
    std::cout << "старых документов.\n\n";

    wait_ms(10000);

    std::cout << "Под клавиатурой обнаруживается пластиковая карта.\n";
    std::cout << "На её поверхности почти стёрта надпись:\n\n";

    std::cout << "             ECHO-07 / STAFF ID\n";
    std::cout << "             ИДЕНТИФИКАТОР: 0417-AX\n";
    std::cout << "             СТАТУС: НЕИЗВЕСТЕН\n\n";

    wait_ms(10000);

    std::cout << "Ты подносишь карту к считывателю терминала.\n";
    std::cout << "Раздаётся короткий звуковой сигнал.\n\n";

    wait_ms(10000);

    std::cout << "[СИСТЕМА]: Идентификатор обнаружен.\n";
    std::cout << "[СИСТЕМА]: Проверка учётной записи...\n";
    wait_ms(10000);

    std::cout << "[СИСТЕМА]: Сотрудник: А. В. Морозов.\n";
    std::cout << "[СИСТЕМА]: Отдел: нейротехнические исследования.\n";
    std::cout << "[СИСТЕМА]: Уровень доступа: 3.\n\n";

    wait_ms(10000);

    std::cout << "Экран на секунду гаснет. Затем появляется новое сообщение.\n\n";

    std::cout << "[СИСТЕМА]: ВНИМАНИЕ.\n";
    std::cout << "[СИСТЕМА]: Данная учётная запись числится закрытой.\n";
    std::cout << "[СИСТЕМА]: Последняя активность: сегодня, 03:17.\n\n";

    wait_ms(6000);

    std::cout << "Ты перечитываешь последнюю строку.\n";
    std::cout << "Если учётная запись закрыта, кто использовал её сегодня?\n\n";

    wait_ms(10000);

    std::cout << "В динамиках раздаётся тихий треск.\n";
    std::cout << "Затем незнакомый голос произносит:\n\n";

    std::cout << "\"Не стоило брать эту карту.\"\n\n";

    wait_ms(10000);

    std::cout << "Терминал самостоятельно открывает скрытый каталог.\n";
    std::cout << "На экране появляется список файлов:\n\n";

    std::cout << "  STAFF_0417_PROFILE\n";
    std::cout << "  NEURO_TEST_17\n";
    std::cout << "  ECHO07_SUBJECT_LOG\n";
    std::cout << "  ACCESS_HISTORY_0317\n\n";

    wait_ms(10000);

    std::cout << "Внизу экрана появляется таймер:\n";
    std::cout << "\"АВТОМАТИЧЕСКОЕ ЗАВЕРШЕНИЕ СЕАНСА: 00:30\"\n\n";

    wait_ms(10000);

    std::cout << "Теперь у тебя есть доступ к закрытым данным.\n";
    std::cout << "Но система уже знает, что кто-то использовал чужой\n";
    std::cout << "идентификатор. У тебя осталось мало времени.\n\n";

    wait_ms(10000);

    std::cout << "==============================================================\n";
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Открыть журнал доступа и узнать, кто был здесь в 03:17.  выбрать 1\n";
    std::cout << "Изучить файл нейротехнического эксперимента.              выбрать 2\n";
    std::cout << "Скопировать данные и немедленно выйти из системы.         выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_identifier)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_identifier = 0;
        }

        if (choice_identifier < 1 || choice_identifier > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_identifier < 1 || choice_identifier > 3);

    clear_screen();

    if (choice_identifier == 1) {
        std::cout << "Ты решил" << G("", "а") << " открыть журнал доступа.\n";
        std::cout << "История событий может раскрыть, кто использовал\n";
        std::cout << "закрытую учётную запись в 03:17.\n";
    }
    else if (choice_identifier == 2) {
        std::cout << "Ты решил" << G("", "а") << " изучить нейротехнический эксперимент.\n";
        std::cout << "Возможно, этот файл объяснит, что происходило\n";
        std::cout << "в комплексе Нокс-7.\n";
    }
    else if (choice_identifier == 3) {
        std::cout << "Ты решил" << G("", "а") << " скопировать данные и выйти из системы.\n";
        std::cout << "Но неизвестно, успеет ли терминал завершить копирование\n";
        std::cout << "до автоматического закрытия сеанса.\n";
    }

    wait_ms(6000);
}
//выбор игрокa: Обойти защиту вручную.
void show_ch03_solo_hack_bypass() {
    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "              ГЛАВА 3.5. РУЧНОЙ ОБХОД\n";
    std::cout << "==============================================================\n\n";

    wait_ms(10000);

    std::cout << "Ты отодвигаешь найденный идентификатор и возвращаешься\n";
    std::cout << "к терминалу. Использовать чужую учётную запись слишком\n";
    std::cout << "рискованно. Придётся обойти защиту вручную.\n\n";

    wait_ms(10000);

    std::cout << "На экране открыта консоль диагностики.\n";
    std::cout << "Большинство команд заблокировано, но система оставила\n";
    std::cout << "доступ к служебным настройкам.\n\n";

    wait_ms(10000);

    std::cout << "[ТЕРМИНАЛ]: Требуется подтверждение администратора.\n";
    std::cout << "[ТЕРМИНАЛ]: Попытка несанкционированного доступа запрещена.\n\n";

    wait_ms(10000);

    std::cout << "Ты начинаешь проверять доступные процессы и замечаешь\n";
    std::cout << "странность: один из защитных модулей работает отдельно\n";
    std::cout << "от основной системы.\n\n";

    wait_ms(10000);

    std::cout << "Модуль называется SECURITY_NODE_07.\n";
    std::cout << "Его последнее обновление датировано несколькими годами\n";
    std::cout << "раньше. При этом журнал показывает активность сегодня.\n\n";

    wait_ms(10000);

    std::cout << "Ты отключаешь один из второстепенных фильтров и вручную\n";
    std::cout << "перенаправляешь запрос через диагностический интерфейс.\n\n";

    wait_ms(10000);

    std::cout << "[ТЕРМИНАЛ]: Обнаружено изменение конфигурации.\n";
    std::cout << "[ТЕРМИНАЛ]: Проверка целостности...\n";
    wait_ms(10000);

    std::cout << "[ТЕРМИНАЛ]: Ошибка проверки.\n";
    std::cout << "[ТЕРМИНАЛ]: Повторная проверка...\n\n";

    wait_ms(10000);

    std::cout << "На мгновение экран покрывается строками ошибок.\n";
    std::cout << "Затем интерфейс исчезает, оставляя только чёрный фон.\n\n";

    wait_ms(10000);

    std::cout << "В центре экрана появляется белая надпись:\n\n";

    std::cout << "                  ECHO-07\n";
    std::cout << "             РУЧНОЙ ДОСТУП РАЗРЕШЁН\n\n";

    wait_ms(10000);

    std::cout << "Ты не понимаешь, почему система разрешила доступ.\n";
    std::cout << "Ты не взламывал" << G("", "а") << " её полностью. Ты лишь обошёл один\n";
    std::cout << "из защитных модулей.\n\n";

    wait_ms(10000);

    std::cout << "На экране появляется новая строка:\n\n";

    std::cout << "[ECHO-07]: Субъект обнаружен.\n";
    std::cout << "[ECHO-07]: Совпадение с архивной записью: 98,7%.\n";
    std::cout << "[ECHO-07]: Продолжить наблюдение?\n\n";

    wait_ms(6000);

    std::cout << "Ты замираешь.\n";
    std::cout << "Система не должна знать, кто ты такой.\n";
    std::cout << "Но она явно сравнивает тебя с кем-то из архива.\n\n";

    wait_ms(10000);

    std::cout << "В глубине комплекса раздаётся металлический удар.\n";
    std::cout << "Затем ещё один. Будто кто-то медленно идёт по коридору.\n\n";

    wait_ms(10000);

    std::cout << "Терминал снова оживает. На экране появляется меню\n";
    std::cout << "доступных действий, а рядом начинает мигать предупреждение:\n\n";

    std::cout << "\"СОЕДИНЕНИЕ НЕСТАБИЛЬНО. ЗАЩИТА ВОССТАНОВИТСЯ ЧЕРЕЗ 00:45.\"\n\n";

    wait_ms(10000);

    std::cout << "Ты добил" << G("ся", "ась") << " доступа к части закрытых данных.\n";
    std::cout << "Однако система уже заметила вмешательство, и времени\n";
    std::cout << "на дальнейшие действия почти не осталось.\n\n";

    wait_ms(10000);

    std::cout << "===================================================================\n";
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "===================================================================\n";
    std::cout << "Открыть архивные записи и выяснить, с кем совпал профиль.  выбрать 1\n";
    std::cout << "Попытаться отключить модуль SECURITY_NODE_07.              выбрать 2\n";
    std::cout << "Проверить камеры и узнать, что происходит в коридоре.      выбрать 3\n";
    std::cout << "===================================================================\n";

    do {
        if (!(std::cin >> choice_bypass)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_bypass = 0;
        }

        if (choice_bypass < 1 || choice_bypass > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_bypass < 1 || choice_bypass > 3);

    clear_screen();

    if (choice_bypass == 1) {
        std::cout << "Ты решил" << G("", "а") << " открыть архивные записи.\n";
        std::cout << "Возможно, архив объяснит, почему система\n";
        std::cout << "считает твой профиль знакомым.\n";
    }
    else if (choice_bypass == 2) {
        std::cout << "Ты решил" << G("", "а") << " отключить SECURITY_NODE_07.\n";
        std::cout << "Если модуль действительно отвечает за защиту,\n";
        std::cout << "это может дать тебе больше времени.\n";
    }
    else if (choice_bypass == 3) {
        std::cout << "Ты решил" << G("", "а") << " проверить камеры наблюдения.\n";
        std::cout << "Нужно выяснить, кто или что движется по коридору.\n";
    }

    wait_ms(6000);
}
//выбор игрокa: Открыть журнал событий.
void show_ch03_solo_hack_event_log() {
    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "              ГЛАВА 3.6. ЖУРНАЛ СОБЫТИЙ\n";
    std::cout << "==============================================================\n\n";

    wait_ms(10000);

    std::cout << "Ты возвращаешься к главному окну терминала и открываешь\n";
    std::cout << "журнал системных событий.\n";
    std::cout << "Большая часть записей повреждена, но некоторые строки\n";
    std::cout << "всё ещё доступны для чтения.\n\n";

    wait_ms(10000);

    std::cout << "[СИСТЕМА]: Загрузка журнала событий...\n";
    wait_ms(10000);
    std::cout << "[СИСТЕМА]: Обнаружено 1847 записей.\n";
    std::cout << "[СИСТЕМА]: Повреждено: 62 процента.\n\n";

    wait_ms(10000);

    std::cout << "Ты фильтруешь записи по времени последней активности.\n";
    std::cout << "На экране появляются несколько событий:\n\n";

    std::cout << "[02:41:08] — Открытие лаборатории нейротехнических исследований.\n";
    std::cout << "[02:56:32] — Потеря соединения с сектором C-4.\n";
    std::cout << "[03:11:47] — Отключение трёх камер наблюдения.\n";
    std::cout << "[03:17:03] — Вход пользователя: UNKNOWN.\n";
    std::cout << "[03:17:09] — Запуск протокола ECHO-07.\n";
    std::cout << "[03:17:14] — Блокировка внутренних дверей.\n";
    std::cout << "[03:17:19] — Удаление записи о пользователе.\n\n";

    wait_ms(6000);

    std::cout << "Ты перечитываешь последние строки.\n";
    std::cout << "Неизвестный пользователь вошёл в систему, запустил\n";
    std::cout << "протокол, а затем кто-то попытался стереть следы.\n\n";

    wait_ms(10000);

    std::cout << "Но удаление прошло не полностью.\n";
    std::cout << "В одной из скрытых записей сохранился фрагмент команды:\n\n";

    std::cout << "COMMAND: ECHO07_INIT_SEQUENCE\n";
    std::cout << "ORIGIN: INTERNAL NETWORK\n";
    std::cout << "AUTHORIZATION: OVERRIDDEN\n";
    std::cout << "RESULT: SUBJECT RELEASED\n\n";

    wait_ms(6000);

    std::cout << "Последняя строка заставляет тебя замереть.\n";
    std::cout << "\"SUBJECT RELEASED\" — субъект освобождён.\n\n";

    wait_ms(10000);

    std::cout << "Кто был этим субъектом?\n";
    std::cout << "И почему после его освобождения комплекс заблокировал\n";
    std::cout << "все внутренние двери?\n\n";

    wait_ms(10000);

    std::cout << "Ты пытаешься открыть подробности события в 03:17:09.\n";
    std::cout << "Терминал зависает на несколько секунд.\n";
    std::cout << "Затем появляется дополнительная запись:\n\n";

    std::cout << "[СИСТЕМА]: Обнаружена попытка восстановления удалённых данных.\n";
    std::cout << "[СИСТЕМА]: Источник запроса: ТЕКУЩИЙ ТЕРМИНАЛ.\n";
    std::cout << "[СИСТЕМА]: Запущена проверка безопасности.\n\n";

    wait_ms(10000);

    std::cout << "Ты не успеваешь закрыть окно.\n";
    std::cout << "На экране появляется красное предупреждение:\n\n";

    std::cout << "           НЕСАНКЦИОНИРОВАННЫЙ ДОСТУП\n";
    std::cout << "           ИДЕНТИФИКАЦИЯ ПОЛЬЗОВАТЕЛЯ...\n\n";

    wait_ms(6000);

    std::cout << "Из динамиков доносится тихий щелчок.\n";
    std::cout << "Затем голос произносит:\n\n";

    std::cout << "\"Ты ищешь начало событий. Но тебе нужно найти конец.\"\n\n";

    wait_ms(6000);

    std::cout << "Экран снова становится чёрным.\n";
    std::cout << "Когда изображение возвращается, журнал уже закрыт.\n";
    std::cout << "Однако терминал сохранил три доступных направления поиска.\n\n";

    wait_ms(10000);

    std::cout << "Ты узнал" << G("", "а") << ", что в 03:17 был запущен протокол ECHO-07.\n";
    std::cout << "Но личность неизвестного пользователя и судьба субъекта\n";
    std::cout << "по-прежнему остаются загадкой.\n\n";

    wait_ms(10000);

    std::cout << "=================================================================\n";
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "===================================================================\n";
    std::cout << "Восстановить удалённую запись о неизвестном пользователе. выбрать 1\n";
    std::cout << "Найти сведения о субъекте, которого освободили.           выбрать 2\n";
    std::cout << "Проверить журнал камер за несколько минут до 03:17.       выбрать 3\n";
    std::cout << "===================================================================\n";

    do {
        if (!(std::cin >> choice_event_log)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_event_log = 0;
        }

        if (choice_event_log < 1 || choice_event_log > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_event_log < 1 || choice_event_log > 3);

    clear_screen();

    if (choice_event_log == 1) {
        std::cout << "Ты решил" << G("", "а") << " восстановить удалённую запись.\n";
        std::cout << "Возможно, удастся установить личность неизвестного\n";
        std::cout << "пользователя, вошедшего в систему в 03:17.\n";
    }
    else if (choice_event_log == 2) {
        std::cout << "Ты решил" << G("", "а") << " найти сведения об освобождённом субъекте.\n";
        std::cout << "Ответ может скрываться в архиве нейротехнических\n";
        std::cout << "исследований комплекса Нокс-7.\n";
    }
    else if (choice_event_log == 3) {
        std::cout << "Ты решил" << G("", "а") << " проверить записи камер наблюдения.\n";
        std::cout << "Возможно, камеры успели зафиксировать неизвестного\n";
        std::cout << "пользователя перед запуском протокола.\n";
    }

    wait_ms(6000);
}

//------------------------------3глава выбор 5 самому без марка осмотреть камеру--

//выбор игрокa: Восстановить запись с камеры. Перейти во внешний контур.
void show_ch03_solo_camera_restore_recording()
{
    clear_screen();
    std::cout << "==============================================================\n";
    std::cout << "              ГЛАВА 3.7. ВОССТАНОВЛЕНИЕ ЗАПИСИ\n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "Ты снова подключаешь телефон к сервисному порту камеры.\n";
    std::cout << "На экране всё ещё открыт файл CAM_07_0317.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Если часть записи повреждена, значит, её можно восстановить.\n";
    wait_ms(10000);

    std::cout << "Ты открываешь список удалённых фрагментов.\n";
    std::cout << "Большинство из них невозможно прочитать, но один файл\n";
    std::cout << "содержит достаточно данных для восстановления.\n\n";
    wait_ms(10000);

    std::cout << "ФАЙЛ: CAM_07_0317_RECOVER\n";
    std::cout << "СОСТОЯНИЕ: ПОВРЕЖДЁН\n";
    std::cout << "ВОССТАНОВЛЕНИЕ: ДОСТУПНО\n\n";
    wait_ms(10000);

    std::cout << "Ты запускаешь восстановление.\n";
    wait_ms(10000);

    std::cout << "[СИСТЕМА]: Анализ видеоданных...\n";
    wait_ms(10000);
    std::cout << "[СИСТЕМА]: Восстановлено 23 процента.\n";
    wait_ms(10000);
    std::cout << "[СИСТЕМА]: Восстановлено 61 процент.\n";
    wait_ms(10000);
    std::cout << "[СИСТЕМА]: Восстановлено 94 процента.\n\n";
    wait_ms(10000);

    std::cout << "[СИСТЕМА]: Ошибка временной метки.\n";
    std::cout << "[СИСТЕМА]: Обнаружен фрагмент до начала записи.\n\n";
    wait_ms(10000);

    std::cout << "Ты хмуришься.\n";
    std::cout << "Фрагмент до начала записи? Камера не должна хранить\n";
    std::cout << "изображение, которое было снято до запуска файла.\n";
    wait_ms(6000);

    std::cout << "\nТы открываешь восстановленный фрагмент.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. За несколько минут до 03:17 ===\n\n";
    wait_ms(10000);

    std::cout << "На экране появляется изображение входа в комплекс.\n";
    std::cout << "Временная метка показывает 03:12:06.\n";
    wait_ms(10000);

    std::cout << "Дверь закрыта. Ветер гонит по полу мелкий мусор.\n";
    std::cout << "Камера неподвижно смотрит на пустой проход.\n";
    wait_ms(10000);

    std::cout << "Вдруг изображение дёргается.\n";
    std::cout << "На мгновение в углу экрана появляется человек.\n";
    wait_ms(10000);

    std::cout << "Он стоит слишком близко к камере, чтобы можно было\n";
    std::cout << "разглядеть его лицо.\n";
    wait_ms(10000);

    std::cout << "Человек поднимает голову и смотрит прямо в объектив.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Это тот же человек, что был на записи в 03:17...\n";
    wait_ms(10000);

    std::cout << "Но есть одна странность.\n";
    std::cout << "Восстановленный фрагмент показывает время 03:12,\n";
    std::cout << "а в первоначальной записи человек появился только в 03:17.\n";
    wait_ms(6000);

    std::cout << "Человек медленно приближается к двери.\n";
    std::cout << "Он не пытается открыть её.\n";
    std::cout << "Вместо этого он прикладывает ладонь к стене рядом с замком.\n";
    wait_ms(6000);

    std::cout << "На стене загорается маленький индикатор.\n";
    std::cout << "Дверь остаётся закрытой, но камера поворачивается\n";
    std::cout << "вслед за человеком, хотя её механизм не должен работать.\n";
    wait_ms(6000);

    std::cout << "Затем человек поворачивается к объективу.\n";
    std::cout << "Его губы шевелятся, будто он что-то говорит.\n";
    wait_ms(10000);

    std::cout << "Ты увеличиваешь изображение и включаешь звук.\n";
    wait_ms(10000);

    std::cout << "Сначала слышен только шум помех.\n";
    wait_ms(10000);

    std::cout << "Затем сквозь треск пробивается тихий голос:\n\n";
    std::cout << "\"Когда он придёт, не открывай дверь.\"\n\n";
    wait_ms(6000);

    std::cout << "Запись обрывается.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Невозможный кадр ===\n\n";
    wait_ms(10000);

    std::cout << "Ты возвращаешься к последним секундам фрагмента.\n";
    std::cout << "Теперь на экране видна только пустая дверь.\n";
    wait_ms(10000);

    std::cout << "Но в чёрном стекле объектива отражается силуэт.\n";
    std::cout << "Он стоит за камерой, хотя на самой записи никого нет.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Нет... Такого не может быть.\n";
    wait_ms(10000);

    std::cout << "Ты останавливаешь видео и проверяешь исходный файл.\n";
    std::cout << "Отражение остаётся на том же месте.\n";
    wait_ms(10000);

    std::cout << "Ты перематываешь запись назад.\n";
    std::cout << "Силуэт исчезает.\n";
    wait_ms(10000);

    std::cout << "Перематываешь вперёд.\n";
    std::cout << "Он снова появляется.\n";
    wait_ms(10000);

    std::cout << "На экране телефона возникает новое уведомление:\n\n";
    std::cout << "[КАМЕРА]: ВОССТАНОВЛЕНИЕ ЗАВЕРШЕНО.\n";
    std::cout << "[КАМЕРА]: ОБНАРУЖЕН НЕСАНКЦИОНИРОВАННЫЙ ПРОСМОТР.\n";
    std::cout << "[КАМЕРА]: ИСТОЧНИК ЗАПРОСА — НЕ УСТАНОВЛЕН.\n\n";
    wait_ms(6000);

    std::cout << "Экран телефона гаснет.\n";
    wait_ms(10000);

    std::cout << "Над дверью раздаётся щелчок.\n";
    std::cout << "Ты поднимаешь голову. Камера медленно поворачивается\n";
    std::cout << "и снова направляет объектив прямо на тебя.\n";
    wait_ms(6000);

    std::cout << "Красный индикатор мигает три раза.\n";
    wait_ms(10000);

    std::cout << "Из динамика над дверью доносится тот же голос,\n";
    std::cout << "который ты слышал" << G("", "а") << " на восстановленной записи:\n\n";
    std::cout << "\"Теперь ты тоже это видел" << G("", "а") << ".\"\n\n";
    wait_ms(6000);

    std::cout << "Дверной замок щёлкает.\n";
    std::cout << "За дверью слышны шаги, но никто не выходит наружу.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Кто ты? И что значит «когда он придёт»?\n";
    wait_ms(10000);

    std::cout << "Ответа нет.\n";
    std::cout << "Телефон снова включается. В памяти сохранён восстановленный\n";
    std::cout << "фрагмент, а рядом с ним появился новый файл:\n\n";

    std::cout << "ФАЙЛ: ECHO07_PREVIEW\n";
    std::cout << "ВРЕМЯ СОЗДАНИЯ: 03:17:42\n";
    std::cout << "СТАТУС: НЕ ПРОСМОТРЕН\n\n";
    wait_ms(6000);

    std::cout << "Ты не запускал" << G("", "а") << " создание этого файла.\n";
    std::cout << "И всё же он появился на твоём телефоне.\n";
    wait_ms(10000);

    std::cout << "\nНайдено улик: " << player.evidence << ".\n";
    wait_ms(10000);

    int choice_restore = 0;

    std::cout << "\n==============================================================\n";
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Открыть новый файл ECHO07_PREVIEW.                    выбрать 1\n";
    std::cout << "Проследить за проводами камеры.                       выбрать 2\n";
    std::cout << "Проверить отражение в объективе.                      выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_restore)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_restore = 0;
        }

        if (choice_restore < 1 || choice_restore > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_restore < 1 || choice_restore > 3);

    clear_screen();

    if (choice_restore == 1) {
        std::cout << "Ты решил" << G("", "а") << " открыть неизвестный файл ECHO07_PREVIEW.\n";
        std::cout << "Возможно, он был создан самой системой наблюдения.\n";
    }
    else if (choice_restore == 2) {
        std::cout << "Ты решил" << G("", "а") << " проследить за проводами камеры.\n";
        std::cout << "Нужно выяснить, кто управляет её питанием и передачей данных.\n";
    }
    else if (choice_restore == 3) {
        std::cout << "Ты решил" << G("", "а") << " проверить отражение в объективе.\n";
        std::cout << "Ты должен понять, что именно камера показывает тебе.\n";
    }

    wait_ms(6000);
}
//выбор игрокa: Проследить за проводами
void show_ch03_solo_camera_follow_wires()
{
    clear_screen();
    std::cout << "==============================================================\n";
    std::cout << "                  ГЛАВА 3.8. СЛЕД ПРОВОДОВ\n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "Ты убираешь телефон и снова осматриваешь камеру.\n";
    std::cout << "Теперь тебя интересует не то, что она записывает,\n";
    std::cout << "а то, откуда получает питание.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Если комплекс отключён, кто продолжает тебя питать?\n";
    wait_ms(10000);

    std::cout << "Под сервисной панелью видны три кабеля.\n";
    std::cout << "Два уходят внутрь стены. Третий закреплён отдельно\n";
    std::cout << "и выглядит гораздо новее остальных.\n";
    wait_ms(6000);

    std::cout << "На его оболочке нет пыли.\n";
    std::cout << "Кабель словно установили совсем недавно.\n\n";
    wait_ms(10000);

    std::cout << "Ты осторожно снимаешь пластиковые крепления.\n";
    std::cout << "За ними обнаруживается дополнительный разъём,\n";
    std::cout << "которого нет на заводской схеме камеры.\n";
    wait_ms(6000);

    std::cout << "На разъёме выгравировано:\n\n";
    std::cout << "             ECHO-07 / AUXILIARY LINK\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Дополнительный канал... Значит, ты подключена не только к сети.\n";
    wait_ms(6000);

    std::cout << "Ты достаёшь телефон и фотографируешь маркировку.\n";
    std::cout << "Затем прослеживаешь кабель до узкого отверстия\n";
    std::cout << "в стене рядом с дверью.\n";
    wait_ms(6000);

    std::cout << "Отверстие закрыто металлической пластиной.\n";
    std::cout << "На ней заметны свежие царапины, будто её недавно снимали.\n";
    wait_ms(10000);

    std::cout << "За пластиной кабель уходит вниз.\n";
    std::cout << "Ты прислушиваешься и различаешь слабый электрический гул.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== Нокс-7. Второй источник ===\n\n";
    wait_ms(10000);

    std::cout << "Ты прослеживаешь кабель вдоль стены.\n";
    std::cout << "Он проходит за металлическим коробом и исчезает\n";
    std::cout << "под старой панелью обслуживания.\n";
    wait_ms(10000);

    std::cout << "Панель закрыта, но замок сломан.\n";
    std::cout << "Ты осторожно тянешь за край.\n";
    wait_ms(10000);

    std::cout << "Внутри находится небольшой блок питания.\n";
    std::cout << "На его корпусе мигает зелёный индикатор.\n\n";
    wait_ms(10000);

    std::cout << "МАРКИРОВКА: AUX-PWR-07\n";
    std::cout << "СОСТОЯНИЕ: АКТИВЕН\n";
    std::cout << "НАЗНАЧЕНИЕ: НЕИЗВЕСТНО\n\n";
    wait_ms(10000);

    std::cout << "К блоку подключены не только провода камеры.\n";
    std::cout << "От него отходит ещё один кабель, который уходит\n";
    std::cout << "глубже в стену, в сторону внутреннего коридора.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Это не резервное питание одной камеры.\n";
    wait_ms(10000);

    std::cout << "Ты замечаешь маленький экран на блоке.\n";
    std::cout << "На нём отображается текущая нагрузка.\n\n";
    std::cout << "КАМЕРА 07: АКТИВНА\n";
    std::cout << "ВНУТРЕННИЙ КАНАЛ: АКТИВЕН\n";
    std::cout << "ПЕРЕДАЧА ДАННЫХ: ИДЁТ\n";
    std::cout << "ПОЛУЧАТЕЛЬ: ECHO-07\n\n";
    wait_ms(6000);

    std::cout << "Камера передаёт данные не просто на старый сервер.\n";
    std::cout << "Она отправляет их в систему ECHO-07.\n";
    wait_ms(10000);

    std::cout << "Внезапно зелёный индикатор гаснет.\n";
    wait_ms(10000);

    std::cout << "Внутри блока раздаётся щелчок.\n";
    std::cout << "Экран загорается снова, но теперь на нём другая надпись:\n\n";
    std::cout << "[СИСТЕМА]: ОБСЛУЖИВАНИЕ ОБНАРУЖЕНО.\n";
    std::cout << "[СИСТЕМА]: ИСТОЧНИК: НЕИЗВЕСТНЫЙ.\n";
    std::cout << "[СИСТЕМА]: ПРОВЕРКА ПРИСУТСТВИЯ...\n\n";
    wait_ms(6000);

    std::cout << "Ты отступаешь от панели.\n";
    std::cout << "Слышен тихий звук двигателя над дверью.\n";
    wait_ms(10000);

    std::cout << "Камера поворачивается.\n";
    std::cout << "Но теперь она направлена не на вход.\n";
    std::cout << "Объектив смотрит прямо на открытую сервисную панель.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Ты следишь за мной... или за тем, что находится внутри стены?\n";
    wait_ms(6000);

    std::cout << "Из глубины стены доносится короткий металлический стук.\n";
    std::cout << "Затем ещё один, будто кто-то отвечает на твой вопрос.\n";
    wait_ms(6000);

    std::cout << "Ты направляешь фонарь внутрь отверстия.\n";
    std::cout << "За кабелями виден узкий технический проход.\n";
    std::cout << "На дальней стене что-то отражает свет.\n";
    wait_ms(10000);

    std::cout << "Это ещё один разъём — но намного крупнее.\n";
    std::cout << "Рядом с ним видна надпись, нанесённая от руки:\n\n";
    std::cout << "\"НЕ ОТКЛЮЧАТЬ. СИСТЕМА ДОЛЖНА ВИДЕТЬ.\"\n\n";
    wait_ms(6000);

    std::cout << "Ты фотографируешь надпись.\n";
    std::cout << "В этот момент телефон вибрирует.\n";
    wait_ms(10000);

    std::cout << "На экране появляется сообщение без имени отправителя:\n\n";
    std::cout << "\"Если ты отключишь питание, дверь больше не откроется.\"\n\n";
    wait_ms(6000);

    std::cout << "Ты смотришь на закрытую дверь комплекса.\n";
    std::cout << "Теперь становится ясно: камера и замок могут быть\n";
    std::cout << "частями одной системы.\n";
    wait_ms(6000);

    std::cout << "Ты наш" << G("ёл", "ла") << " скрытый источник питания и дополнительный канал.\n";
    std::cout << "Но пока неизвестно, что произойдёт, если вмешаться в него.\n\n";
    wait_ms(10000);

    std::cout << "Найдено улик: " << player.evidence << ".\n";
    wait_ms(10000);

    int choice_wires = 0;

    std::cout << "\n================================================================\n";
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==================================================================\n";
    std::cout << "Проследить дополнительный кабель в технический проход.   выбрать 1\n";
    std::cout << "Попытаться отключить скрытый блок питания.               выбрать 2\n";
    std::cout << "Закрыть панель и проверить замок входной двери.          выбрать 3\n";
    std::cout << "==================================================================\n";

    do {
        if (!(std::cin >> choice_wires)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_wires = 0;
        }

        if (choice_wires < 1 || choice_wires > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_wires < 1 || choice_wires > 3);

    clear_screen();

    if (choice_wires == 1) {
        std::cout << "Ты решил" << G("", "а") << " проследить дополнительный кабель.\n";
        std::cout << "Технический проход может привести к источнику сигнала ECHO-07.\n";
    }
    else if (choice_wires == 2) {
        std::cout << "Ты решил" << G("", "а") << " отключить скрытый блок питания.\n";
        std::cout << "Это может остановить передачу данных, но повлиять на замок.\n";
    }
    else if (choice_wires == 3) {
        std::cout << "Ты решил" << G("", "а") << " проверить входную дверь.\n";
        std::cout << "Прежде чем рисковать, нужно понять, изменилось ли состояние замка.\n";
    }

    wait_ms(6000);
}
//выбор игрокa: Проверить отражение
void show_ch03_solo_camera_check_reflection()
{
    clear_screen();
    std::cout << "==============================================================\n";
    std::cout << "                 ГЛАВА 3.9. ОТРАЖЕНИЕ\n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "Ты снова поднимаешь взгляд на камеру над дверью.\n";
    std::cout << "Красный индикатор медленно мигает, а объектив направлен\n";
    std::cout << "прямо на тебя.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что ты пытаешься мне показать?\n";
    wait_ms(10000);

    std::cout << "Ты подходишь ближе и внимательно смотришь на стекло объектива.\n";
    std::cout << "Его поверхность почти чёрная, но в ней отражается\n";
    std::cout << "вход в комплекс и часть пространства за твоей спиной.\n";
    wait_ms(6000);

    std::cout << "Сначала всё выглядит нормально.\n";
    std::cout << "Ты видишь стену, пол и тусклый свет над дверью.\n";
    wait_ms(10000);

    std::cout << "Затем замечаешь странность.\n";
    std::cout << "В отражении за тобой стоит человек.\n";
    wait_ms(6000);

    std::cout << "Ты резко оборачиваешься.\n";
    std::cout << "Позади никого нет.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Нет. Здесь никого нет...\n";
    wait_ms(10000);

    std::cout << "Ты снова смотришь на объектив.\n";
    std::cout << "Человек всё ещё там.\n";
    wait_ms(6000);

    std::cout << "Его лицо скрыто темнотой.\n";
    std::cout << "Он стоит неподвижно, слегка наклонив голову,\n";
    std::cout << "словно наблюдает за твоей реакцией.\n";
    wait_ms(6000);

    std::cout << "Ты делаешь шаг влево.\n";
    wait_ms(10000);

    std::cout << "Твоё отражение смещается.\n";
    std::cout << "Отражение незнакомца остаётся на месте.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Ты не отражение...\n";
    wait_ms(10000);

    std::cout << "Внезапно человек в объективе поднимает руку.\n";
    std::cout << "Он медленно указывает на дверь комплекса.\n";
    wait_ms(6000);

    std::cout << "Ты переводишь взгляд на дверь.\n";
    std::cout << "Замок закрыт. В коридоре за ней тихо.\n";
    wait_ms(10000);

    std::cout << "Когда ты снова смотришь на камеру, незнакомец\n";
    std::cout << "оказывается намного ближе к объективу.\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== Нокс-7. По ту сторону стекла ===\n\n";
    wait_ms(10000);

    std::cout << "Ты замечаешь, что отражение больше не показывает вход.\n";
    std::cout << "Вместо него виден длинный коридор с холодными лампами.\n";
    wait_ms(6000);

    std::cout << "На стенах коридора нанесены номера помещений.\n";
    std::cout << "В конце видна дверь с маркировкой:\n\n";

    std::cout << "                 ECHO-07\n";
    std::cout << "              СЕКТОР C-4\n\n";
    wait_ms(6000);

    std::cout << "Ты никогда не видел" << G("", "а") << " этот коридор.\n";
    std::cout << "Но отражение выглядит слишком чётким, чтобы быть\n";
    std::cout << "случайным бликом на стекле.\n";
    wait_ms(6000);

    std::cout << "За дверью в отражении появляется движение.\n";
    std::cout << "Кто-то проходит мимо узкого окна.\n";
    wait_ms(10000);

    std::cout << "Затем на стекле изнутри появляется ладонь.\n";
    wait_ms(10000);

    std::cout << "Пальцы медленно скользят вниз, оставляя след.\n";
    std::cout << "Незнакомец словно пытается что-то написать.\n";
    wait_ms(10000);

    std::cout << "Ты всматриваешься в буквы.\n\n";

    std::cout << "              НЕ ОТКРЫВАЙ ЕМУ\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Кому не открывать?.. Кто там?\n";
    wait_ms(10000);

    std::cout << "Отражение резко искажается.\n";
    std::cout << "Коридор исчезает, и объектив снова показывает вход.\n";
    wait_ms(10000);

    std::cout << "Но теперь в отражении стоишь не только ты.\n";
    std::cout << "Незнакомец находится прямо за твоим плечом.\n";
    wait_ms(6000);

    std::cout << "Ты чувствуешь холодный воздух у затылка.\n";
    std::cout << "Кто-то словно стоит совсем рядом.\n";
    wait_ms(10000);

    std::cout << "Ты резко оборачиваешься.\n";
    std::cout << "Пусто.\n";
    wait_ms(10000);

    std::cout << "Из динамика камеры раздаётся шёпот:\n\n";
    std::cout << "\"Не оборачивайся в следующий раз.\"\n\n";
    wait_ms(6000);

    std::cout << "Камера щёлкает, и красный индикатор гаснет.\n";
    wait_ms(10000);

    std::cout << "На секунду ты видишь собственное отражение в стекле.\n";
    std::cout << "Но оно не повторяет твоих движений.\n";
    wait_ms(6000);

    std::cout << "Отражение медленно поднимает голову.\n";
    std::cout << "Его губы шевелятся, хотя ты молчишь.\n";
    wait_ms(10000);

    std::cout << "Ты не слышишь звука, но понимаешь слова:\n\n";
    std::cout << "\"Ты уже был" << G("", "а") << " здесь.\"\n\n";
    wait_ms(6000);

    clear_screen();
    std::cout << "=== Нокс-7. Сбой наблюдения ===\n\n";
    wait_ms(10000);

    std::cout << "Камера внезапно возвращается к обычному режиму.\n";
    std::cout << "Объектив снова смотрит на дверь.\n";
    wait_ms(10000);

    std::cout << "Телефон вибрирует.\n";
    std::cout << "В галерее появилось новое изображение.\n";
    wait_ms(10000);

    std::cout << "На фотографии видна камера над входом.\n";
    std::cout << "Снимок сделан с противоположной стороны двери.\n";
    wait_ms(10000);

    std::cout << "Внизу изображения отображается подпись:\n\n";
    std::cout << "ECHO07 / OBSERVER RECORD\n";
    std::cout << "SUBJECT: " << player.name_of_character << "\n";
    std::cout << "STATUS: RETURNED\n\n";
    wait_ms(6000);

    std::cout << "Ты не делал" << G("", "а") << " эту фотографию.\n";
    std::cout << "И всё же система знает твоё имя.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что значит «вернулся»?.. Я здесь впервые.\n";
    wait_ms(6000);

    std::cout << "Из-за двери доносится протяжный металлический скрежет.\n";
    std::cout << "Замок начинает медленно поворачиваться.\n";
    wait_ms(6000);

    std::cout << "Ты отступаешь от камеры.\n";
    std::cout << "Теперь у тебя есть фотография, странное сообщение\n";
    std::cout << "и ещё один вопрос: почему система считает тебя вернувшимся?\n\n";
    wait_ms(6000);

    std::cout << "Найдено улик: " << player.evidence << ".\n";
    wait_ms(10000);

    int choice_reflection = 0;

    std::cout << "\n==============================================================\n";
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Открыть фотографию ECHO07 и изучить её свойства.       выбрать 1\n";
    std::cout << "Попытаться открыть дверь, пока замок движется.           выбрать 2\n";
    std::cout << "Найти способ отключить камеру и прекратить наблюдение.   выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_reflection)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_reflection = 0;
        }

        if (choice_reflection < 1 || choice_reflection > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_reflection < 1 || choice_reflection > 3);

    clear_screen();

    if (choice_reflection == 1) {
        std::cout << "Ты решил" << G("", "а") << " изучить фотографию ECHO07.\n";
        std::cout << "Возможно, в метаданных снимка скрыта информация\n";
        std::cout << "о том, кто и откуда его отправил.\n";
    }
    else if (choice_reflection == 2) {
        std::cout << "Ты решил" << G("", "а") << " проверить дверь.\n";
        std::cout << "Если замок действительно открывается, это может быть\n";
        std::cout << "твоим единственным шансом попасть внутрь.\n";
    }
    else if (choice_reflection == 3) {
        std::cout << "Ты решил" << G("", "а") << " отключить камеру.\n";
        std::cout << "Но после увиденного ты не уверен, что камера\n";
        std::cout << "является единственным устройством наблюдения.\n";
    }

    wait_ms(6000);
}

//------------------------------4глава выбор игрокa: Взломать систему безопасности. 3 ответа-

//выбор игрокa: Глава 4: Перейти во внешний контур
void show_ch04_mark_outer_sector()
{
    clear_screen();
    std::cout << "=== Глава 4. Внешний контур ===\n\n";
    wait_ms(6000);

    std::cout << "Терминал медленно гаснет за вашей спиной.\n";
    wait_ms(10000);

    std::cout << "Марк отсоединяет кабель и убирает его в сумку.\n";
    wait_ms(10000);

    std::cout << "Марк: — Внешний контур должен быть безопаснее остальных уровней.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Ты говоришь так, будто уже бывал здесь.\n";
    wait_ms(8000);

    std::cout << "Марк на секунду замолкает.\n";
    wait_ms(6000);

    std::cout << "Марк: — Я изучал планы комплекса. Вот и всё.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Коридор внешнего контура ===\n\n";
    wait_ms(6000);

    std::cout << "Вы проходите через тяжёлую дверь. За ней тянется длинный коридор.\n";
    wait_ms(8000);

    std::cout << "На стенах расположены камеры наблюдения, а под потолком мигают\n";
    std::cout << "тусклые аварийные лампы.\n";
    wait_ms(10000);

    std::cout << "На металлической табличке написано:\n\n";
    wait_ms(6000);

    std::cout << "ВНЕШНИЙ КОНТУР — СЕКТОР A-01.\n";
    wait_ms(10000);

    std::cout << "ДОСТУП РАЗРЕШЁН ТОЛЬКО СОТРУДНИКАМ НИЖНЕГО УРОВНЯ.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Странно. Терминал показывал, что этот уровень доступен.\n";
    wait_ms(8000);

    std::cout << "Марк: — Значит, он разрешил нам войти. Не задавай лишних вопросов.\n";
    wait_ms(10000);

    std::cout << "Ты замечаешь, что одна из камер медленно поворачивается вслед за вами.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк... камера следит за нами.\n";
    wait_ms(10000);

    std::cout << "Марк: — Не смотри на неё. Просто продолжай идти.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Идентификация ===\n\n";
    wait_ms(6000);

    std::cout << "Внезапно все лампы в коридоре загораются одновременно.\n";
    wait_ms(8000);

    std::cout << "Из скрытых динамиков раздаётся женский голос.\n";
    wait_ms(10000);

    std::cout << "Система: — ВНИМАНИЕ. ОБНАРУЖЕН ОБЪЕКТ.\n";
    wait_ms(10000);

    std::cout << "Система: — ИДЕНТИФИКАЦИЯ...\n";
    wait_ms(6000);

    std::cout << "Система: — "
        << player.name_of_character << ".\n";
    wait_ms(10000);

    std::cout << "Система: — СТАТУС: ВОССТАНОВЛЕН.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Опять этот статус! Что значит «восстановлен»?!\n";
    wait_ms(8000);

    std::cout << "Марк отступает на шаг. На этот раз он выглядит по-настоящему испуганным.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты знаешь, что это значит. Я вижу по твоему лицу.\n";
    wait_ms(8000);

    std::cout << "Марк: — Я... не могу тебе сейчас объяснить.\n";
    wait_ms(10000);

    std::cout << "Система: — ДОБРО ПОЖАЛОВАТЬ ОБРАТНО.\n";
    wait_ms(8000);

    clear_screen();
    std::cout << "=== Нокс-7. Запертый сектор ===\n\n";
    wait_ms(6000);

    std::cout << "Впереди с грохотом закрывается массивная защитная перегородка.\n";
    wait_ms(8000);

    std::cout << "Вы оказываетесь в небольшом помещении между двумя дверями.\n";
    wait_ms(8000);

    std::cout << "На стене находится терминал с тремя активными разделами.\n";
    wait_ms(10000);

    std::cout << "Марк подходит к экрану и проводит по нему рукой.\n";
    wait_ms(6000);

    std::cout << "Марк: — Похоже, система хочет, чтобы мы выбрали направление.\n";
    wait_ms(8000);

    std::cout << "На экране появляются три надписи:\n\n";
    wait_ms(6000);

    std::cout << "A-01: КОМНАТА НАБЛЮДЕНИЯ.\n";
    wait_ms(6000);

    std::cout << "A-02: МЕДИЦИНСКИЙ БЛОК.\n";
    wait_ms(6000);

    std::cout << "A-03: СЛУЖЕБНЫЙ АРХИВ.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Может быть, здесь мы наконец найдём ответы.\n";
    wait_ms(10000);

    std::cout << "Марк: — Или узнаем то, чего знать не должны.\n";
    wait_ms(8000);

    // ===== ВЫБОР ДАЛЬНЕЙШЕГО ДЕЙСТВИЯ =====

    clear_screen();
    flush_input();

    std::cout << "=== Глава 4. Внешний контур ===\n\n";
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Осмотреть комнату наблюдения.                    выбрать 1\n";
    std::cout << "Зайти в медицинский блок.                        выбрать 2\n";
    std::cout << "Открыть служебный архив.                         выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_outer)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_outer = 0;
        }

        if (choice_ch04_outer < 1 || choice_ch04_outer > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_outer < 1 || choice_ch04_outer > 3);

    if (choice_ch04_outer == 1) {
        clear_screen();
        std::cout << "Вы выбрали: Осмотреть комнату наблюдения.";
        wait_ms(6000);

        // Следующая сцена будет написана отдельно.
        // show_ch04_mark_observation_room();
    }
    else if (choice_ch04_outer == 2) {
        clear_screen();
        std::cout << "Вы выбрали: Зайти в медицинский блок.";
        wait_ms(6000);

        // Следующая сцена будет написана отдельно.
        // show_ch04_mark_medical_block();
    }
    else if (choice_ch04_outer == 3) {
        clear_screen();
        std::cout << "Вы выбрали: Открыть служебный архив.";
        wait_ms(6000);

        // Следующая сцена будет написана отдельно.
        // show_ch04_mark_service_archive();
    }
}
//выбор игрокa: Глава 4: Отправиться в архив исследований.
void show_ch04_mark_archive()
{
    clear_screen();
    flush_input();

    std::cout << "========================================\n";
    std::cout << "          ГЛАВА 4: АРХИВ\n";
    std::cout << "========================================\n\n";

    wait_ms(6000);

    std::cout << "Марк прикладывает устройство к панели доступа.\n";
    wait_ms(10000);

    std::cout << "На экране появляется сообщение:\n\n";
    std::cout << "ДОСТУП К АРХИВУ ИССЛЕДОВАНИЙ...\n";
    wait_ms(6000);
    std::cout << "ПРОВЕРКА ПОЛНОМОЧИЙ...\n";
    wait_ms(6000);
    std::cout << "ОШИБКА: УРОВЕНЬ ДОСТУПА НЕДОСТАТОЧЕН.\n\n";
    wait_ms(8000);

    std::cout << "Марк:\n";
    std::cout << "\"Чёрт. Они закрыли архив после того, как мы вошли в систему.\"\n\n";
    wait_ms(10000);

    std::cout << "Он подключает устройство напрямую к терминалу.\n";
    std::cout << "На экране начинают быстро сменяться строки кода.\n\n";
    wait_ms(8000);

    std::cout << "Марк:\n";
    std::cout << "\"У меня есть несколько секунд. Ищи что-нибудь, связанное с тобой.\"\n\n";
    wait_ms(10000);

    std::cout << "Ты открываешь список файлов.\n";
    std::cout << "Большинство документов зашифровано или удалено.\n";
    std::cout << "Но один файл выделяется среди остальных.\n\n";
    wait_ms(10000);

    std::cout << "НАЙДЕН ФАЙЛ: ECHO_SUBJECT_01\n";
    std::cout << "СТАТУС: ВОССТАНОВЛЕН\n";
    std::cout << "ПОСЛЕДНЕЕ ИЗМЕНЕНИЕ: [ДАННЫЕ УДАЛЕНЫ]\n\n";
    wait_ms(10000);

    std::cout << "Марк:\n";
    std::cout << "\"ECHO_SUBJECT... Это имя мне ничего не говорит.\"\n\n";
    wait_ms(8000);

    std::cout << "Ты открываешь файл.\n";
    std::cout << "На экране появляется запись:\n\n";
    wait_ms(10000);

    std::cout << "----------------------------------------\n";
    std::cout << "ПРОЕКТ: ECHO\n";
    std::cout << "ОБЪЕКТ: " << player.name_of_character << "\n";
    std::cout << "СОСТОЯНИЕ ПАМЯТИ: НЕСТАБИЛЬНОЕ\n";
    std::cout << "ПРОЦЕДУРА ВОССТАНОВЛЕНИЯ: НЕ ЗАВЕРШЕНА\n";
    std::cout << "----------------------------------------\n\n";
    wait_ms(10000);

    std::cout << "Ниже находится короткая текстовая запись:\n\n";
    std::cout << "\"Субъект демонстрирует признаки восстановления.\n";
    std::cout << "Необходимо исключить контакт с персоналом,\n";
    std::cout << "осведомлённым о первоначальном эксперименте.\"\n\n";
    wait_ms(19000);

    std::cout << "Под записью стоит подпись:\n";
    std::cout << "L. MORROW\n\n";
    wait_ms(10000);

    std::cout << "Марк отступает от терминала.\n\n";
    std::cout << "Марк:\n";
    std::cout << "\"Морроу... Это тот самый человек из сообщения?\"\n\n";
    wait_ms(10000);

    std::cout << "Внезапно терминал издаёт короткий сигнал.\n";
    std::cout << "На экране появляется новое уведомление.\n\n";
    wait_ms(10000);

    std::cout << "ВНИМАНИЕ: НЕСАНКЦИОНИРОВАННЫЙ ДОСТУП.\n";
    std::cout << "АРХИВ БУДЕТ ЗАБЛОКИРОВАН ЧЕРЕЗ 30 СЕКУНД.\n\n";
    wait_ms(10000);

    std::cout << "Марк:\n";
    std::cout << "\"У нас мало времени. Нужно решить, что забрать отсюда.\"\n\n";
    wait_ms(8000);

    std::cout << "На терминале доступны три действия:\n\n";
    std::cout << "1. Скачать полный файл ECHO_SUBJECT_01.\n";
    std::cout << "2. Открыть удалённую переписку Л. Морроу.\n";
    std::cout << "3. Найти схему нижнего уровня.\n\n";
    wait_ms(10000);

    int choice_ch04_archive = 0;

    do
    {
        std::cout << "Твой выбор: ";

        if (!(std::cin >> choice_ch04_archive))
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_ch04_archive = 0;
        }

        if (choice_ch04_archive < 1 || choice_ch04_archive > 3)
        {
            std::cout << "Неверный выбор. Введи число от 1 до 3.\n";
        }

    } while (choice_ch04_archive < 1 || choice_ch04_archive > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_archive == 1)
    {
        std::cout << "Ты начинаешь скачивать файл ECHO_SUBJECT_01.\n\n";
        wait_ms(10000);

        std::cout << "ЗАГРУЗКА: 1%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 3%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 4%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 6%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 8%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 9%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 11%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 13%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 14%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 16%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 18%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 19%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 21%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 23%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 24%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 26%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 28%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 29%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 31%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 33%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 34%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 36%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 38%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 39%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 41%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 43%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 44%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 46%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 48%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 49%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 51%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 53%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 54%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 56%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 58%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 59%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 61%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 63%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 64%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 66%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 68%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 69%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 71%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 73%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 74%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 76%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 78%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 79%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 81%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 83%...\n";
        wait_ms(700);

        std::cout << "ЗАГРУЗКА: 84%...\n";
        wait_ms(700);
        std::cout << "ЗАГРУЗКА: 89%...\n";
        wait_ms(6000);

        std::cout << "\nЗАГРУЗКА ПРЕРВАНА.\n";
        std::cout << "АРХИВ ЗАБЛОКИРОВАН.\n\n";
        wait_ms(10000);

        std::cout << "Марк:\n";
        std::cout << "\"Часть данных успела сохраниться. Но этого недостаточно.\"\n\n";
        wait_ms(8000);

        std::cout << "На устройстве появляется неизвестный файл:\n";
        std::cout << "ECHO_FRAGMENT_01\n\n";
        wait_ms(10000);
    }
    else if (choice_ch04_archive == 2)
    {
        std::cout << "Ты открываешь удалённую переписку Л. Морроу.\n\n";
        wait_ms(10000);

        std::cout << "НАЙДЕНО СООБЩЕНИЕ:\n";
        std::cout << "ОТПРАВИТЕЛЬ: L. MORROW\n";
        std::cout << "ПОЛУЧАТЕЛЬ: [СКРЫТО]\n\n";
        wait_ms(10000);

        std::cout << "\"Если субъект восстановится раньше срока,\n";
        std::cout << "не позволяйте ему добраться до нижнего уровня.\n";
        std::cout << "Он не должен узнать, что произошло в комнате 01.\"\n\n";
        wait_ms(10000);

        std::cout << "Марк:\n";
        std::cout << "\"Комната 01... Что они там с тобой сделали?\"\n\n";
        wait_ms(8000);
    }
    else if (choice_ch04_archive == 3)
    {
        std::cout << "Ты открываешь схему нижнего уровня.\n\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ДОСТУП ЗАПРЕЩЁН.\n";
        std::cout << "Попытка просмотра зарегистрирована.\n\n";
        wait_ms(10000);

        std::cout << "Несмотря на блокировку, на мгновение появляется часть схемы.\n";
        std::cout << "Ты успеваешь заметить обозначение:\n\n";
        std::cout << "НИЖНИЙ УРОВЕНЬ — КОМНАТА 01\n";
        std::cout << "СТАТУС: ЗАПЕЧАТАНА\n\n";
        wait_ms(10000);

        std::cout << "Марк:\n";
        std::cout << "\"Теперь система точно знает, что мы здесь. Уходим.\"\n\n";
        wait_ms(8000);
    }

    std::cout << "В коридоре раздаётся звук закрывающихся дверей.\n";
    std::cout << "Красные индикаторы загораются один за другим.\n\n";
    wait_ms(8000);

    std::cout << "СИСТЕМА:\n";
    std::cout << "\"Несанкционированный доступ обнаружен.\n";
    std::cout << "Персоналу оставаться на местах.\"\n\n";
    wait_ms(10000);

    std::cout << "Марк хватает тебя за руку.\n\n";
    std::cout << "Марк:\n";
    std::cout << "\"Потом разберёмся с файлами. Сейчас нужно выбраться отсюда.\"\n\n";
    wait_ms(10000);

    std::cout << "========================================\n";
    std::cout << "       Конец сцены: Архив исследований\n";
    std::cout << "========================================\n\n";

    wait_ms(6000);

    // Здесь можно вызвать следующую сцену побега из архива.
}
//выбор игрокa: Глава 4: Спуститься на нижний уровень.
void show_ch04_mark_lower_level()
{
    clear_screen();
    flush_input();

    std::cout << "========================================\n";
    std::cout << "       ГЛАВА 4: НИЖНИЙ УРОВЕНЬ\n";
    std::cout << "========================================\n\n";

    wait_ms(6000);

    std::cout << "Марк останавливается перед массивной дверью лифта.\n";
    std::cout << "Над панелью горит красная надпись:\n\n";
    std::cout << "НИЖНИЙ УРОВЕНЬ — ДОСТУП ЗАПРЕЩЁН.\n\n";
    wait_ms(10000);

    std::cout << "Марк:\n";
    std::cout << "\"Система не должна была открыть нам этот маршрут.\"\n\n";
    wait_ms(10000);

    std::cout << "Он подключает устройство к панели.\n";
    std::cout << "Несколько секунд ничего не происходит.\n";
    std::cout << "Затем экран неожиданно гаснет.\n\n";
    wait_ms(8000);

    std::cout << "СИСТЕМА:\n";
    std::cout << "\"Обнаружен субъект: "
        << player.name_of_character << ".\"\n";
    std::cout << "\"Статус: ВОССТАНОВЛЕН.\"\n";
    std::cout << "\"Добро пожаловать обратно.\"\n\n";
    wait_ms(10000);

    std::cout << "Марк медленно поворачивается к тебе.\n\n";
    std::cout << "Марк:\n";
    std::cout << "\"Обратно? Почему она говорит именно так?\"\n\n";
    wait_ms(10000);

    std::cout << "Двери лифта открываются.\n";
    std::cout << "Изнутри тянет холодом.\n";
    std::cout << "На стене видны царапины, будто кто-то пытался выбраться.\n\n";
    wait_ms(10000);

    std::cout << "Вы входите внутрь.\n";
    std::cout << "Марк нажимает кнопку нижнего уровня.\n\n";
    wait_ms(10000);

    std::cout << "Лифт начинает движение вниз.\n";
    std::cout << "На панели постепенно гаснут номера этажей.\n\n";
    wait_ms(8000);

    std::cout << "Внезапно лифт останавливается.\n";
    std::cout << "Свет мигает.\n";
    std::cout << "Из динамика раздаётся искажённый голос.\n\n";
    wait_ms(8000);

    std::cout << "НЕИЗВЕСТНЫЙ ГОЛОС:\n";
    std::cout << "\"Не приводите его сюда.\"\n\n";
    wait_ms(8000);

    std::cout << "Марк резко поднимает голову.\n\n";
    std::cout << "Марк:\n";
    std::cout << "\"Кто это сказал?\"\n\n";
    wait_ms(10000);

    std::cout << "Ответа нет.\n";
    std::cout << "Лифт продолжает движение.\n\n";
    wait_ms(8000);

    std::cout << "Наконец двери открываются.\n";
    std::cout << "Перед вами длинный коридор с тусклым аварийным освещением.\n";
    std::cout << "На стене закреплена табличка:\n\n";
    std::cout << "ИССЛЕДОВАТЕЛЬСКИЙ КОМПЛЕКС\n";
    std::cout << "НИЖНИЙ УРОВЕНЬ — СЕКТОР 01\n\n";
    wait_ms(19000);

    std::cout << "Большинство дверей запечатано.\n";
    std::cout << "На полу разбросаны документы и осколки стекла.\n";
    std::cout << "В конце коридора видна дверь с обозначением:\n\n";
    std::cout << "КОМНАТА 01\n";
    std::cout << "ПРОЕКТ ECHO\n";
    std::cout << "ДОСТУП ТОЛЬКО ПО РАЗРЕШЕНИЮ РУКОВОДИТЕЛЯ\n\n";
    wait_ms(19000);

    std::cout << "Марк останавливается.\n\n";
    std::cout << "Марк:\n";
    std::cout << "\"Нам нужно найти другой путь. Эта дверь заблокирована.\"\n\n";
    wait_ms(10000);

    std::cout << "Ты замечаешь три объекта, которые можно исследовать:\n\n";
    std::cout << "1. Терминал безопасности у входа.\n";
    std::cout << "2. Разбросанные документы на полу.\n";
    std::cout << "3. Дверь комнаты 01.\n\n";
    wait_ms(6000);

    int choice_ch04_lower = 0;

    do
    {
        std::cout << "Твой выбор: ";

        if (!(std::cin >> choice_ch04_lower))
        {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice_ch04_lower = 0;
        }

        if (choice_ch04_lower < 1 || choice_ch04_lower > 3)
        {
            std::cout << "Неверный выбор. Введи число от 1 до 3.\n";
        }

    } while (choice_ch04_lower < 1 || choice_ch04_lower > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_lower == 1)
    {
        std::cout << "Ты подходишь к терминалу безопасности.\n\n";
        wait_ms(10000);

        std::cout << "На экране отображается журнал событий:\n\n";
        std::cout << "[03:14:08] ПРОЕКТ ECHO — АКТИВАЦИЯ.\n";
        std::cout << "[03:14:11] СУБЪЕКТ 01 — НЕСТАБИЛЬНОЕ СОСТОЯНИЕ.\n";
        std::cout << "[03:14:17] АВАРИЙНАЯ БЛОКИРОВКА КОМНАТЫ 01.\n";
        std::cout << "[03:14:19] СОТРУДНИК: L. MORROW.\n\n";
        wait_ms(19000);

        std::cout << "Внизу появляется предупреждение:\n";
        std::cout << "\"Журнал неполный. Часть данных удалена вручную.\"\n\n";
        wait_ms(8000);
    }
    else if (choice_ch04_lower == 2)
    {
        std::cout << "Ты поднимаешь с пола несколько документов.\n\n";
        wait_ms(10000);

        std::cout << "На одном из листов сохранилась запись:\n\n";
        std::cout << "\"После процедуры субъект потерял доступ\n";
        std::cout << "к большей части автобиографической памяти.\n";
        std::cout << "Повторное восстановление может вызвать\n";
        std::cout << "непредсказуемые реакции.\"\n\n";
        wait_ms(19000);

        std::cout << "Внизу страницы стоит подпись:\n";
        std::cout << "L. MORROW\n\n";
        wait_ms(8000);

        std::cout << "На обороте кто-то написал от руки:\n";
        std::cout << "\"Марк знает, что произошло.\"\n\n";
        wait_ms(8000);
    }
    else if (choice_ch04_lower == 3)
    {
        std::cout << "Ты подходишь к двери комнаты 01.\n\n";
        wait_ms(10000);

        std::cout << "Ты прикладываешь руку к панели.\n";
        std::cout << "Она загорается ещё до того, как ты касаешься её.\n\n";
        wait_ms(8000);

        std::cout << "СИСТЕМА:\n";
        std::cout << "\"Биометрическое совпадение подтверждено.\"\n";
        std::cout << "\"Доступ ожидает подтверждения руководителя.\"\n\n";
        wait_ms(10000);

        std::cout << "Марк оттягивает тебя от двери.\n\n";
        std::cout << "Марк:\n";
        std::cout << "\"Не делай этого. Мы не знаем, что за ней.\"\n\n";
        wait_ms(10000);

        std::cout << "Ты замечаешь, что Марк смотрит не на дверь,\n";
        std::cout << "а на небольшую камеру над ней.\n\n";
        wait_ms(10000);
    }

    std::cout << "Внезапно коридор освещается красным светом.\n";
    std::cout << "По громкоговорителю раздаётся голос системы.\n\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА:\n";
    std::cout << "\"Обнаружено присутствие в запечатанном секторе.\"\n";
    std::cout << "\"Начинается протокол изоляции.\"\n\n";
    wait_ms(19000);

    std::cout << "Двери за вашей спиной начинают закрываться.\n\n";
    wait_ms(10000);

    std::cout << "Марк:\n";
    std::cout << "\"Бежим! Сейчас же!\"\n\n";
    wait_ms(10000);

    std::cout << "Вы направляетесь к боковому проходу.\n";
    std::cout << "Впереди мигает аварийный указатель.\n";
    std::cout << "За поворотом слышны шаги.\n\n";
    wait_ms(19000);

    std::cout << "========================================\n";
    std::cout << "       Конец сцены: Нижний уровень\n";
    std::cout << "========================================\n\n";

    wait_ms(6000);

    // Здесь можно вызвать следующую сцену побега из нижнего уровня.
}

//------------------------------4глава выбор игрокa: Попросить Марка отвлечь систему. 2 ответа-

//выбор игрокa: Глава 4: Довериться Марку и бежать к старому лифту.
void show_ch04_mark_old_elevator()
{
    clear_screen();
    flush_input();

    std::cout << "========================================\n";
    std::cout << "       ГЛАВА 4: СТАРЫЙ ЛИФТ\n";
    std::cout << "========================================\n\n";
    wait_ms(6000);

    std::cout << "Марк хватает тебя за руку и тянет в технический коридор.\n";
    std::cout << "Позади с грохотом закрывается металлическая дверь.\n\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Марк, куда мы идём?\n\n";
    wait_ms(6000);

    std::cout << "Марк: — К старому грузовому лифту.\n";
    std::cout << "Его давно отключили от центральной системы.\n";
    std::cout << "Если повезёт, мы сможем выбраться из комплекса.\n\n";
    wait_ms(10000);

    std::cout << "Вы бежите по узкому коридору.\n";
    std::cout << "На стенах видны старые кабели и следы ржавчины.\n";
    std::cout << "Аварийные лампы здесь не работают.\n\n";
    wait_ms(10000);

    std::cout << "Внезапно позади раздаётся голос системы.\n\n";
    std::cout << "СИСТЕМА: ОБНАРУЖЕНО ДВИЖЕНИЕ В СЕКТОРЕ A-07.\n";
    std::cout << "СИСТЕМА: МАРШРУТ ЭВАКУАЦИИ ЗАБЛОКИРОВАН.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Она знает, куда мы направляемся!\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Нет. Этот коридор не подключён к основной сети.\n";
    std::cout << "Кто-то вручную передаёт системе наши координаты.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Кто-то? Ты хочешь сказать, что здесь есть ещё люди?\n\n";
    wait_ms(8000);

    std::cout << "Марк останавливается на мгновение.\n";
    std::cout << "Он явно хочет что-то сказать, но передумывает.\n\n";
    wait_ms(8000);

    std::cout << "Марк: — Потом. Сначала выберемся отсюда.\n\n";
    wait_ms(6000);

    std::cout << "Через несколько минут вы добираетесь до массивной двери.\n";
    std::cout << "На ней едва различима надпись:\n\n";
    std::cout << "ГРУЗОВОЙ ЛИФТ № 03\n";
    std::cout << "ТЕХНИЧЕСКИЙ ДОСТУП\n";
    std::cout << "ЭКСПЛУАТАЦИЯ ПРЕКРАЩЕНА\n\n";
    wait_ms(10000);

    std::cout << "Марк достаёт из кармана небольшой ключ-карту.\n";
    std::cout << "На ней нет ни имени, ни логотипа комплекса.\n\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Откуда у тебя эта карта?\n\n";
    wait_ms(6000);

    std::cout << "Марк: — Она осталась у меня давно.\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Насколько давно?\n\n";
    wait_ms(6000);

    std::cout << "Марк не отвечает. Он прикладывает карту к считывателю.\n\n";
    wait_ms(8000);

    std::cout << "ПРОВЕРКА КЛЮЧ-КАРТЫ...\n";
    wait_ms(6000);

    std::cout << "ИДЕНТИФИКАТОР: M-07.\n";
    wait_ms(6000);

    std::cout << "СТАТУС: ДОСТУП РАЗРЕШЁН.\n\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — M-07... Что это значит?\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Не сейчас.\n\n";
    wait_ms(6000);

    std::cout << "Дверь медленно открывается.\n";
    std::cout << "За ней находится старая шахта лифта.\n";
    std::cout << "Внизу едва заметно мерцает свет.\n\n";
    wait_ms(10000);

    std::cout << "Вы входите внутрь.\n";
    std::cout << "Марк нажимает кнопку подъёма.\n";
    std::cout << "Лифт вздрагивает, но не двигается.\n\n";
    wait_ms(8000);

    std::cout << "На панели загорается экран.\n\n";
    std::cout << "СИСТЕМА: ОБНАРУЖЕН СУБЪЕКТ ECHO_SUBJECT_01.\n";
    std::cout << "СИСТЕМА: ПОПЫТКА ПОКИНУТЬ КОМПЛЕКС ЗАПРЕЩЕНА.\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему система называет меня субъектом?\n\n";
    wait_ms(10000);

    std::cout << "Марк медленно опускает руку с кнопки.\n\n";
    wait_ms(6000);

    std::cout << "Марк: — Потому что ты был частью проекта ECHO.\n\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Ты знал об этом с самого начала?\n\n";
    wait_ms(10000);

    std::cout << "Марк: — Я знал только часть правды.\n";
    std::cout << "Но если мы останемся здесь, ты никогда не узнаешь остального.\n\n";
    wait_ms(10000);

    std::cout << "Внезапно лифт начинает двигаться вниз.\n";
    std::cout << "Кнопки на панели гаснут одна за другой.\n\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Марк... Мы же нажали подъём.\n\n";
    wait_ms(6000);

    std::cout << "Марк: — Я ничего не нажимал.\n\n";
    wait_ms(6000);

    std::cout << "Из динамика раздаётся незнакомый женский голос.\n\n";
    std::cout << "НЕИЗВЕСТНЫЙ ГОЛОС:\n";
    std::cout << "\"Не бойся. Я ждала, когда ты вернёшься.\"\n\n";
    wait_ms(10000);

    std::cout << "Лифт продолжает спускаться.\n";
    std::cout << "На экране появляется обозначение нового этажа:\n\n";
    std::cout << "НИЖНИЙ УРОВЕНЬ — КОМНАТА 01.\n\n";
    wait_ms(10000);

    std::cout << "Марк отступает к стене.\n\n";
    std::cout << "Марк: — Этого не может быть...\n\n";
    wait_ms(8000);

    std::cout << "Двери лифта начинают открываться.\n";
    std::cout << "За ними виден длинный коридор и дверь с надписью ECHO.\n\n";
    wait_ms(10000);

    std::cout << "========================================\n";
    std::cout << "      КОНЕЦ СЦЕНЫ: СТАРЫЙ ЛИФТ\n";
    std::cout << "========================================\n\n";

    wait_ms(6000);

    // Следующая сцена будет определяться через start_game().
}
//выбор игрокa: Глава 4: Не слушать Марка и спуститься по лестнице.
void show_ch04_mark_stairs()
{
    clear_screen();

    std::cout << "ГЛАВА 4: ЛЕСТНИЦА ВНИЗ\n\n";
    wait_ms(10000);

    std::cout << "Марк резко хватает тебя за руку.\n\n";
    wait_ms(6000);

    std::cout << "Марк: Стой. Туда нельзя.\n";
    wait_ms(10000);

    std::cout << "Из темноты снизу снова доносится женский голос.\n\n";
    wait_ms(6000);

    std::cout << "Голос: Я знаю, что ты здесь...\n";
    wait_ms(10000);

    std::cout << "Голос: Не позволяй Марку снова солгать тебе.\n\n";
    wait_ms(6000);

    std::cout << "Ты смотришь на Марка. Он избегает твоего взгляда.\n\n";
    wait_ms(6000);

    std::cout << "Марк: Ты не понимаешь, что там находится.\n";
    wait_ms(10000);

    std::cout << "Марк: Если ты спустишься, назад уже ничего не вернуть.\n\n";
    wait_ms(6000);

    std::cout << "Лестница уходит глубоко под землю. На стенах мерцают красные лампы.\n";
    wait_ms(6000);

    std::cout << "На металлической двери внизу видна надпись:\n\n";
    wait_ms(10000);

    std::cout << "        ECHO — КОМНАТА 01\n";
    wait_ms(10000);

    std::cout << "Голос: Пожалуйста... Я должна рассказать тебе правду.\n\n";
    wait_ms(6000);

    std::cout << "Марк отпускает твою руку, но не отходит от лестницы.\n\n";
    wait_ms(6000);

    std::cout << "Марк: Послушай меня хотя бы один раз.\n";
    wait_ms(10000);

    std::cout << "Марк: Всё, что ты там услышишь, может оказаться ложью.\n\n";
    wait_ms(6000);

    std::cout << "Ты понимаешь, что Марк боится не самой лестницы.\n";
    wait_ms(10000);

    std::cout << "Он боится того, что ты можешь узнать внизу.\n\n";
    wait_ms(6000);

    std::cout << "Что ты будешь делать?\n\n";
    std::cout << "1. Не слушать Марка и спуститься по лестнице.\n";
    std::cout << "2. Найти источник голоса и узнать правду.\n\n";

    int choice = 0;

    do {
        std::cout << "Твой выбор: ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            choice = 0;
        }

        if (choice < 1 || choice > 2) {
            std::cout << "Выбери 1 или 2.\n";
        }
    } while (choice < 1 || choice > 2);

    clear_screen();

    if (choice == 1) {
        std::cout << "Ты убираешь руку Марка и делаешь первый шаг вниз.\n\n";
        wait_ms(6000);

        std::cout << "Марк: Нет! Подожди!\n\n";
        wait_ms(10000);

        std::cout << "Ты продолжаешь спускаться. Каждый шаг отдаётся эхом в пустом коридоре.\n\n";
        wait_ms(10000);

        std::cout << "Система: Обнаружено отклонение от установленного маршрута.\n";
        wait_ms(10000);

        std::cout << "Система: ECHO_SUBJECT_01 приближается к комнате 01.\n\n";
        wait_ms(10000);

        std::cout << "Голос: Ты всё-таки пришёл...\n\n";
        wait_ms(10000);

        std::cout << "Внизу загорается свет. За стеклом стоит женщина.\n";
        wait_ms(10000);

        std::cout << "Она смотрит прямо на тебя, словно ждала этого момента очень долго.\n\n";
        wait_ms(10000);

        std::cout << "Женщина: Наконец-то. Теперь я могу рассказать тебе, кем ты был.\n\n";
        wait_ms(10000);

        // Здесь можно вызвать следующую сцену.
        // show_ch04_mark_room01();
    }
    else if (choice == 2) {
        std::cout << "Ты решаешь не идти вслепую и осматриваешь лестницу.\n\n";
        wait_ms(6000);

        std::cout << "Рядом с перилами обнаруживается старая панель доступа.\n";
        wait_ms(6000);

        std::cout << "На экране мигает сообщение: «Архив голосовых записей — ECHO».\n\n";
        wait_ms(10000);

        std::cout << "Ты нажимаешь на панель. Из динамика раздаётся тот же женский голос.\n\n";
        wait_ms(6000);

        std::cout << "Голос: Если ты слышишь эту запись, значит, память снова начали восстанавливать.\n";
        wait_ms(10000);

        std::cout << "Голос: Марк знает, что произошло в комнате 01. Он был там.\n\n";
        wait_ms(10000);

        std::cout << "В записи появляется шум, а затем последние слова:\n\n";
        wait_ms(10000);

        std::cout << "Голос: Не верь тому, кто скажет, что ты был просто пациентом.\n\n";
        wait_ms(10000);

        std::cout << "Запись обрывается. За твоей спиной стоит Марк.\n\n";
        wait_ms(6000);

        std::cout << "Марк: Теперь ты понимаешь, почему я не хотел, чтобы ты это услышал.\n\n";
        wait_ms(10000);

        // Здесь можно вызвать следующую сцену.
        // show_ch04_mark_voice_recording();
    }
}

//------------------------------4глава выбор игрокa: Принудительно открыть архив. 2 ответа-
// выбор игрокa : Открыть папку ECHO и прочитать последнюю страницу
void show_ch04_mark_archive_truth()
{
    clear_screen();

    std::cout << "=== Глава 4.4. Последняя страница ===\n";
    wait_ms(6000);

    std::cout << "Ты кладёшь папку ECHO — SUBJECT 01 на ближайший металлический стол.\n";
    wait_ms(6000);

    std::cout << "Марк стоит рядом. Он смотрит на папку, но не пытается остановить тебя.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Лея сказала не открывать последнюю страницу. Значит, именно там правда.\n";
    wait_ms(10000);

    std::cout << "Марк: — Есть вещи, которые лучше не знать.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты уже говорил это. Хватит.\n";
    wait_ms(10000);

    std::cout << "Ты открываешь папку и начинаешь перелистывать документы.\n";
    wait_ms(6000);

    std::cout << "Первые страницы заполнены медицинскими показателями, графиками "
        << "и отчётами о состоянии памяти неизвестного субъекта.\n";
    wait_ms(10000);

    std::cout << "На нескольких листах повторяется одна и та же надпись:\n\n";
    wait_ms(10000);

    std::cout << "ПРОЕКТ ECHO — ВОССТАНОВЛЕНИЕ АВТОБИОГРАФИЧЕСКОЙ ПАМЯТИ.\n";
    wait_ms(10000);

    std::cout << "Ты находишь фотографию лаборатории. На заднем плане стоит человек "
        << "в медицинском кресле. Его лицо почти полностью скрыто тенью.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Это что, пациент? Почему его лицо закрашено?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я не знаю.\n";
    wait_ms(10000);

    std::cout << "Ты поворачиваешь фотографию. На обратной стороне написано:\n\n";
    wait_ms(10000);

    std::cout << "\"СУБЪЕКТ 01. ПЕРВАЯ ПРОЦЕДУРА ЗАВЕРШЕНА. "
        << "АВТОБИОГРАФИЧЕСКАЯ ПАМЯТЬ УДАЛЕНА\".\n";
    wait_ms(10000);

    std::cout << "Ты чувствуешь странное давление в голове. На мгновение "
        << "перед глазами возникает незнакомое помещение.\n";
    wait_ms(10000);

    std::cout << "Белые стены. Яркий свет. Чей-то голос за стеклом.\n";
    wait_ms(6000);

    std::cout << "Неизвестный голос: — Если процедура сработает, субъект не вспомнит "
        << "ни лабораторию, ни сотрудников.\n";
    wait_ms(10000);

    std::cout << "Другой голос: — А если память восстановится?\n";
    wait_ms(10000);

    std::cout << "Неизвестный голос: — Тогда протокол ECHO будет активирован повторно.\n";
    wait_ms(10000);

    std::cout << "Видение исчезает так же внезапно, как появилось.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Я только что что-то вспомнил...\n";
    wait_ms(10000);

    std::cout << "Марк делает шаг назад.\n";
    wait_ms(10000);

    std::cout << "Марк: — Что именно?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Лабораторию. Голоса. Они говорили о каком-то субъекте.\n";
    wait_ms(6000);

    std::cout << "Марк молчит. Его реакция говорит больше, чем любые слова.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "ПОСЛЕДНЯЯ СТРАНИЦА\n\n";
    wait_ms(10000);

    std::cout << "Документ датирован несколькими годами ранее.\n";
    wait_ms(10000);

    std::cout << "Имя ответственного руководителя: L. MORROW.\n";
    wait_ms(10000);

    std::cout << "Статус субъекта: ECHO_SUBJECT_01.\n";
    wait_ms(10000);

    std::cout << "Первоначальная личность: НЕ ПОДТВЕРЖДЕНА.\n";
    wait_ms(10000);

    std::cout << "Текущий статус: ВОССТАНОВЛЕНИЕ АКТИВИРОВАНО.\n";
    wait_ms(6000);

    std::cout << "Последняя строка выделена красным:\n\n";
    wait_ms(10000);

    std::cout << "\"ЕСЛИ СУБЪЕКТ ВОССТАНОВИТ ПАМЯТЬ, "
        << "НЕ ДОПУСТИТЬ ЕГО В КОМНАТУ 01\".\n";
    wait_ms(10000);

    std::cout << "Ты медленно опускаешь лист.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк... Кто такой ECHO_SUBJECT_01?\n";
    wait_ms(10000);

    std::cout << "Марк закрывает глаза и долго не отвечает.\n";
    wait_ms(6000);

    std::cout << "Марк: — Это ты.\n";
    wait_ms(10000);

    std::cout << "В помещении становится тихо. Слышно только гудение вентиляции.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Значит, мне удалили память? Вы все знали?\n";
    wait_ms(6000);

    std::cout << "Марк: — Я знал, что ты связан с проектом. Но я не знал, "
        << "что восстановление действительно началось.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — А Лея? Что она скрывает?\n";
    wait_ms(10000);

    std::cout << "Марк: — Она была ближе к проекту, чем ты думаешь.\n";
    wait_ms(6000);

    std::cout << "Внезапно терминал на стене включается сам по себе.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ОБНАРУЖЕН ДОСТУП К ЗАПЕЧАТАННЫМ ДАННЫМ.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: ECHO_SUBJECT_01 — ИДЕНТИФИКАЦИЯ ПОДТВЕРЖДЕНА.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: ЗАПУЩЕН ПРОТОКОЛ ПОВТОРНОЙ ИЗОЛЯЦИИ.\n";
    wait_ms(6000);

    std::cout << "Марк: — Чёрт. Теперь комплекс знает, где мы.\n";
    wait_ms(10000);

    std::cout << "Из динамика раздаётся голос Леи:\n";
    wait_ms(10000);

    std::cout << "Лея: — Ты открыл последнюю страницу, да?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты знала, что я был субъектом проекта ECHO?\n";
    wait_ms(6000);

    std::cout << "Лея: — Да. Но это не вся правда. Есть кое-что, чего нет даже в этой папке.\n";
    wait_ms(10000);

    std::cout << "Связь начинает прерываться.\n";
    wait_ms(10000);

    std::cout << "Лея: — Если хочешь узнать, что произошло на самом деле, "
        << "найди меня. И не позволяй Марку привести тебя в комнату 01.\n";
    wait_ms(10000);

    std::cout << "Связь обрывается.\n";
    wait_ms(10000);

    std::cout << "Марк: — Мы должны уходить. Протокол изоляции уже запущен.\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Потребовать от Марка рассказать всё, что он знает.   выбрать 1\n";
    std::cout << "Найти Лею и узнать оставшуюся правду.                выбрать 2\n";
    std::cout << "==============================================================\n";
}
// выбор игрока : Послушать Лею и пока не открывать последнюю страницу.
void show_ch04_mark_find_leia()
{
    clear_screen();

    std::cout << "=== Глава 4.5. Следуя за голосом ===\n";
    wait_ms(6000);

    std::cout << "Ты закрываешь папку ECHO — SUBJECT 01 и убираешь её.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Лея просила не открывать последнюю страницу. Сначала найдём её.\n";
    wait_ms(6000);

    std::cout << "Марк внимательно смотрит на тебя.\n";
    wait_ms(10000);

    std::cout << "Марк: — Возможно, это правильное решение.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты говоришь так, будто знаешь, где она.\n";
    wait_ms(10000);

    std::cout << "Марк: — Я знаю, где находится старый коммуникационный узел. "
        << "Если Лея ещё в комплексе, она может быть там.\n";
    wait_ms(10000);

    std::cout << "Из динамика раздаётся треск.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ВНИМАНИЕ. ПРОТОКОЛ ИЗОЛЯЦИИ АКТИВИРОВАН.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: ВСЕМ СОТРУДНИКАМ ПОКИНУТЬ ЗАКРЫТЫЕ СЕКТОРА.\n";
    wait_ms(6000);

    std::cout << "Марк: — У нас мало времени. За мной.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "Вы покидаете архив и сворачиваете в узкий технический коридор.\n";
    wait_ms(6000);

    std::cout << "Лампы мигают. Где-то за стенами работают механизмы блокировки.\n";
    wait_ms(6000);

    std::cout << "На стене ты замечаешь старую табличку:\n\n";
    wait_ms(10000);

    std::cout << "КОММУНИКАЦИОННЫЙ УЗЕЛ — СЕКТОР C-04.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Почему Лея не может просто выйти на связь?\n";
    wait_ms(10000);

    std::cout << "Марк: — После запуска протокола система отключает большую часть каналов. "
        << "Остаются только внутренние линии.\n";
    wait_ms(10000);

    std::cout << "Вы доходите до тяжёлой двери. На панели доступа мигает зелёный индикатор.\n";
    wait_ms(6000);

    std::cout << "Марк прикладывает карту. Дверь открывается с протяжным скрипом.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "КОММУНИКАЦИОННЫЙ УЗЕЛ C-04\n\n";
    wait_ms(10000);

    std::cout << "Внутри стоят старые серверные стойки и несколько терминалов.\n";
    wait_ms(6000);

    std::cout << "На одном из экранов отображается активный канал связи.\n";
    wait_ms(10000);

    std::cout << "Ты нажимаешь кнопку вызова.\n";
    wait_ms(10000);

    std::cout << "Несколько секунд слышны только помехи.\n";
    wait_ms(10000);

    std::cout << "Лея: — Кто это?\n";
    wait_ms(10000);

    std::cout << player.name_of_character << ": — Это я. Мы нашли архив.\n";
    wait_ms(10000);

    std::cout << "Лея: — Ты открыл последнюю страницу?\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Нет. Я решил сначала найти тебя.\n";
    wait_ms(6000);

    std::cout << "На другом конце наступает тишина.\n";
    wait_ms(10000);

    std::cout << "Лея: — Хорошо. Значит, у нас ещё есть шанс.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Шанс на что? Лея, что такое проект ECHO?\n";
    wait_ms(6000);

    std::cout << "Лея: — Это был проект по работе с человеческой памятью. "
        << "Но официальные документы скрывают настоящую цель.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — А я? Почему моё имя связано с субъектом 01?\n";
    wait_ms(6000);

    std::cout << "Лея: — Потому что ты был не просто участником эксперимента.\n";
    wait_ms(6000);

    std::cout << "Марк отворачивается от терминала.\n";
    wait_ms(10000);

    std::cout << "Лея: — Марк тоже там был. Он знает, что случилось перед удалением твоей памяти.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Тогда почему он ничего не рассказывает?\n";
    wait_ms(10000);

    std::cout << "Лея: — Потому что он боится последствий. И, возможно, не только за себя.\n";
    wait_ms(10000);

    std::cout << "Связь начинает прерываться.\n";
    wait_ms(10000);

    std::cout << "Лея: — Слушай внимательно. Я нахожусь в медицинском блоке. "
        << "Но путь туда уже перекрывают.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Мы идём к тебе.\n";
    wait_ms(10000);

    std::cout << "Лея: — Подожди. Есть ещё кое-что. Не доверяй системе, "
        << "когда она скажет, что ты восстановлен.\n";
    wait_ms(10000);

    std::cout << "Связь обрывается.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "Марк молча изучает схему комплекса на терминале.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Мы направляемся в медицинский блок. И на этот раз ты расскажешь мне всё.\n";
    wait_ms(6000);

    std::cout << "Марк: — Я расскажу то, что знаю. Но сначала нам нужно выбраться отсюда.\n";
    wait_ms(6000);

    std::cout << "На схеме отмечены два возможных маршрута.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Направиться в медицинский блок к Лее.                выбрать 1\n";
    std::cout << "Потребовать от Марка объяснить прошлое до выхода.     выбрать 2\n";
    std::cout << "==============================================================\n";
}

//------------------------------4глава выбор игрокa: проверка камер с марком. 3 ответа-
 
// выбор игрока : Просмотреть резервную копию записи.
void show_ch04_mark_camera_backup()
{
    clear_screen();

    std::cout << "=== Глава 4.6. Резервная запись ===\n";
    wait_ms(6000);

    std::cout << "Вы с Марком добираетесь до небольшой комнаты наблюдения.\n";
    wait_ms(6000);

    std::cout << "Вдоль стены стоят мониторы. Большинство выключено, "
        << "а несколько показывают пустые коридоры комплекса.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Если здесь сохранились записи, мы сможем узнать, что произошло.\n";
    wait_ms(6000);

    std::cout << "Марк садится за терминал и начинает проверять архив камер.\n";
    wait_ms(6000);

    std::cout << "Марк: — Основные записи удалены. Кто-то специально очистил этот участок.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — А резервные копии?\n";
    wait_ms(10000);

    std::cout << "Марк: — Они хранятся отдельно. Если система их не уничтожила, "
        << "у нас есть шанс.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "ТЕРМИНАЛ НАБЛЮДЕНИЯ\n\n";
    wait_ms(10000);

    std::cout << "Поиск резервных записей...\n";
    wait_ms(10000);

    std::cout << "Найден файл: CAM_C04_BACKUP_01.\n";
    wait_ms(10000);

    std::cout << "Дата записи: НЕИЗВЕСТНА.\n";
    wait_ms(10000);

    std::cout << "Статус: ЧАСТИЧНО ПОВРЕЖДЁН.\n";
    wait_ms(10000);

    std::cout << "Марк: — Это может быть то, что нам нужно.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Запускай.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "ВОСПРОИЗВЕДЕНИЕ РЕЗЕРВНОЙ ЗАПИСИ\n\n";
    wait_ms(10000);

    std::cout << "На экране появляется коридор медицинского блока.\n";
    wait_ms(6000);

    std::cout << "Изображение дрожит. Время на записи скачет, "
        << "а звук почти полностью отсутствует.\n";
    wait_ms(6000);

    std::cout << "В кадр входят двое сотрудников в медицинской форме. "
        << "Между ними находится человек с закрытым лицом.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Приблизь изображение.\n";
    wait_ms(10000);

    std::cout << "Марк увеличивает изображение. Один из сотрудников "
        << "прикладывает карту к двери с надписью «ECHO — ROOM 01».\n";
    wait_ms(10000);

    std::cout << "Дверь открывается. Все трое исчезают внутри.\n";
    wait_ms(6000);

    std::cout << "Через несколько секунд запись начинает пропускать кадры.\n";
    wait_ms(10000);

    std::cout << "На экране появляется Марк.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Это ты.\n";
    wait_ms(10000);

    std::cout << "Марк не отвечает. На записи он стоит у двери комнаты 01 "
        << "и разговаривает с кем-то внутри.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты говорил, что не знаешь, что произошло в этой комнате.\n";
    wait_ms(6000);

    std::cout << "Марк: — Я не помню этот момент.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Но на записи ты там был!\n";
    wait_ms(10000);

    std::cout << "Марк: — Я сказал, что не помню. Это не значит, что я лгу.\n";
    wait_ms(6000);

    std::cout << "Запись продолжается. Марк на экране поворачивается к камере "
        << "и смотрит прямо в объектив.\n";
    wait_ms(10000);

    std::cout << "Он произносит несколько слов, но звук повреждён.\n";
    wait_ms(10000);

    std::cout << "Терминал пытается восстановить аудиодорожку...\n";
    wait_ms(6000);

    std::cout << "ВОССТАНОВЛЕНО: «...если он вспомнит, всё повторится».\n";
    wait_ms(10000);

    std::cout << "Марк резко отодвигается от экрана.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что должно повториться?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я не знаю.\n";
    wait_ms(10000);

    std::cout << "Внезапно запись переключается на другой ракурс. "
        << "Видна дверь комнаты 01 и красный индикатор над ней.\n";
    wait_ms(10000);

    std::cout << "На экране появляется сообщение:\n\n";
    wait_ms(10000);

    std::cout << "ПРОТОКОЛ ECHO: ПЕРВЫЙ ЭТАП ЗАВЕРШЁН.\n";
    wait_ms(10000);

    std::cout << "СУБЪЕКТ 01: ПАМЯТЬ УДАЛЕНА.\n";
    wait_ms(10000);

    std::cout << "ОТВЕТСТВЕННЫЙ СОТРУДНИК: M. — ДАННЫЕ ПОВРЕЖДЕНЫ.\n";
    wait_ms(6000);

    std::cout << "Изображение замирает. Через секунду экран становится чёрным.\n";
    wait_ms(6000);

    std::cout << "Марк: — Хватит. Дальше ничего нет.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты хочешь сказать, что случайно оказался рядом с комнатой 01 "
        << "в момент удаления моей памяти?\n";
    wait_ms(10000);

    std::cout << "Марк опускает взгляд.\n";
    wait_ms(10000);

    std::cout << "Марк: — Я пытался тебе помочь.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Тогда объясни, что именно ты сделал.\n";
    wait_ms(10000);

    std::cout << "Марк: — Не здесь. Система уже знает, что мы просмотрели запись.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "ТЕРМИНАЛ НАБЛЮДЕНИЯ\n\n";
    wait_ms(10000);

    std::cout << "ВНИМАНИЕ: ОБНАРУЖЕН НЕСАНКЦИОНИРОВАННЫЙ ДОСТУП.\n";
    wait_ms(6000);

    std::cout << "АРХИВ КАМЕР БУДЕТ ЗАБЛОКИРОВАН.\n";
    wait_ms(10000);

    std::cout << "Марк: — Уходим. Немедленно.\n";
    wait_ms(10000);

    std::cout << "Ты сохраняешь идентификатор резервной записи на терминале.\n";
    wait_ms(6000);

    std::cout << "Теперь у тебя есть доказательство: Марк присутствовал "
        << "в комплексе во время процедуры ECHO.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Потребовать от Марка немедленно рассказать правду.   выбрать 1\n";
    std::cout << "Уйти с Марком и сохранить запись для дальнейшего изучения. выбрать 2\n";
    std::cout << "==============================================================\n";
}
// выбор игрока : Попытаться открыть файл ECHO07_UNKNOWN.
void show_ch04_mark_open_echo07()
{
    clear_screen();

    std::cout << "=== Глава 4.7. Неизвестный файл ===\n";
    wait_ms(6000);

    std::cout << "Вы с Марком возвращаетесь к терминалу наблюдения.\n";
    wait_ms(6000);

    std::cout << "Ты снова открываешь список резервных файлов, найденных в архиве камер.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Здесь есть ещё один файл. ECHO07_UNKNOWN.\n";
    wait_ms(10000);

    std::cout << "Марк: — Не открывай его.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему? Ты даже не знаешь, что в нём.\n";
    wait_ms(10000);

    std::cout << "Марк: — Именно. Неизвестные файлы в этой системе могут быть ловушками.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — После всего, что я увидел, ты всё ещё предлагаешь мне ничего не делать?\n";
    wait_ms(6000);

    std::cout << "Марк отворачивается. Ты запускаешь проверку файла.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "ТЕРМИНАЛ — АНАЛИЗ ФАЙЛА\n\n";
    wait_ms(10000);

    std::cout << "Имя: ECHO07_UNKNOWN\n";
    wait_ms(10000);

    std::cout << "Тип: ЗАШИФРОВАННЫЙ ОТЧЁТ\n";
    wait_ms(10000);

    std::cout << "Уровень доступа: ECHO-7\n";
    wait_ms(10000);

    std::cout << "Статус: СКРЫТ ОТ ОСНОВНОГО ИНДЕКСА\n";
    wait_ms(10000);

    std::cout << "Попытка определения автора...\n";
    wait_ms(10000);

    std::cout << "Результат: L. MORROW\n";
    wait_ms(10000);

    std::cout << "Марк: — Morrow... Значит, файл принадлежит руководителю проекта.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Тогда здесь может быть то, что они пытались скрыть.\n";
    wait_ms(10000);

    std::cout << "На экране появляется запрос:\n\n";
    wait_ms(10000);

    std::cout << "ОТКРЫТЬ ЗАШИФРОВАННЫЙ ФАЙЛ?\n";
    wait_ms(10000);

    std::cout << "ПРЕДУПРЕЖДЕНИЕ: НЕВЕРНЫЙ КЛЮЧ МОЖЕТ УНИЧТОЖИТЬ ДАННЫЕ.\n";
    wait_ms(6000);

    std::cout << "Ты подтверждаешь открытие.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "РАСШИФРОВКА ECHO07_UNKNOWN...\n";
    wait_ms(10000);

    std::cout << "Проверка ключа доступа...\n";
    wait_ms(10000);

    std::cout << "Ключ принят.\n";
    wait_ms(10000);

    std::cout << "Доступ к скрытому отчёту разрешён.\n";
    wait_ms(10000);

    std::cout << "На экране появляется документ с заголовком:\n\n";
    wait_ms(10000);

    std::cout << "ПРОЕКТ ECHO — ЭТАП 07\n";
    wait_ms(10000);

    std::cout << "ТЕМА: ВОССТАНОВЛЕНИЕ ЛИЧНОСТИ ПОСЛЕ УДАЛЕНИЯ ПАМЯТИ.\n";
    wait_ms(6000);

    std::cout << "Ты начинаешь читать отчёт.\n";
    wait_ms(10000);

    std::cout << "\"Предыдущие этапы позволили удалить автобиографические "
        << "воспоминания субъекта, сохранив базовые когнитивные функции\".\n";
    wait_ms(10000);

    std::cout << "\"Однако после повторной активации наблюдается спонтанное "
        << "восстановление отдельных фрагментов памяти\".\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Значит, эти вспышки... Я действительно что-то вспоминаю.\n";
    wait_ms(6000);

    std::cout << "Марк: — Продолжай читать.\n";
    wait_ms(10000);

    std::cout << "Ты прокручиваешь документ ниже.\n";
    wait_ms(10000);

    std::cout << "\"Субъект 01 демонстрирует признаки восстановления личности, "
        << "не предусмотренные текущей моделью проекта\".\n";
    wait_ms(10000);

    std::cout << "\"Повторное вмешательство может привести к непредсказуемым "
        << "последствиям для субъекта и персонала\".\n";
    wait_ms(10000);

    std::cout << "Ниже находится таблица сотрудников проекта. Большая часть "
        << "имён удалена, но одна строка сохранилась.\n";
    wait_ms(10000);

    std::cout << "СВЯЗНОЙ СОТРУДНИК: MARK — СТАТУС НЕИЗВЕСТЕН.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Марк, почему твоё имя в этом отчёте?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я работал в комплексе. Я никогда этого не скрывал.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Но ты скрывал, что был связан с моим экспериментом.\n";
    wait_ms(6000);

    std::cout << "Марк не отвечает.\n";
    wait_ms(10000);

    std::cout << "Внезапно текст на экране начинает исчезать.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ОБНАРУЖЕН НЕСАНКЦИОНИРОВАННЫЙ ДОСТУП К ЭТАПУ 07.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: ЗАПУСК УДАЛЕНИЯ ЛОКАЛЬНОЙ КОПИИ.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Нет! Она удаляет файл!\n";
    wait_ms(10000);

    std::cout << "Ты пытаешься сохранить отчёт на терминале.\n";
    wait_ms(10000);

    std::cout << "Сохранение: 18%...\n";
    wait_ms(10000);

    std::cout << "Сохранение: 43%...\n";
    wait_ms(10000);

    std::cout << "Сохранение: 71%...\n";
    wait_ms(10000);

    std::cout << "Сохранение: 100%.\n";
    wait_ms(10000);

    std::cout << "РЕЗЕРВНАЯ КОПИЯ СОХРАНЕНА.\n";
    wait_ms(10000);

    std::cout << "Исходный файл удалён системой.\n";
    wait_ms(10000);

    std::cout << "Ты успел сохранить отчёт, но последняя часть документа повреждена.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "ВОССТАНОВЛЕННЫЙ ФРАГМЕНТ ECHO07\n\n";
    wait_ms(10000);

    std::cout << "\"Если субъект 01 восстановит память полностью, "
        << "он может вспомнить события перед началом проекта\".\n";
    wait_ms(10000);

    std::cout << "\"Сотруднику Mark поручено наблюдать за субъектом "
        << "и не допускать его к исходной записи\".\n";
    wait_ms(10000);

    std::cout << "Последняя строка повреждена. Удалось восстановить лишь несколько слов:\n";
    wait_ms(6000);

    std::cout << "\"...он сам попросил нас...\"\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Что я попросил вас сделать?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я не могу ответить, пока сам не вспомню всё до конца.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Или ты просто не хочешь говорить.\n";
    wait_ms(10000);

    std::cout << "Марк: — Возможно. Но если ты хочешь узнать, что означает эта строка, "
        << "нам понадобится исходная запись.\n";
    wait_ms(10000);

    std::cout << "На терминале появляется ещё одно сообщение:\n\n";
    wait_ms(10000);

    std::cout << "ИСХОДНАЯ ЗАПИСЬ: СЕКТОР C-04.\n";
    wait_ms(10000);

    std::cout << "СТАТУС: ПЕРЕМЕЩЕНА В ЗАКРЫТЫЙ АРХИВ.\n";
    wait_ms(10000);

    std::cout << "Марк: — Теперь понятно, куда нам идти.\n";
    wait_ms(10000);

    std::cout << "Ты сохраняешь восстановленный фрагмент ECHO07.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Найти исходную запись в закрытом архиве.             выбрать 1\n";
    std::cout << "Потребовать от Марка объяснить фразу из отчёта.      выбрать 2\n";
    std::cout << "==============================================================\n";
}
// выбор игрока :  Найти способ отключить блокировку двери.
void show_ch04_mark_disable_lockdown()
{
    clear_screen();

    std::cout << "=== Глава 4.8. Протокол блокировки ===\n";
    wait_ms(6000);

    std::cout << "Коридор заканчивается массивной герметичной дверью. "
        << "Её поверхность покрыта царапинами, будто кто-то пытался "
        << "выбраться отсюда без инструментов.\n";
    wait_ms(10000);

    std::cout << "Красные индикаторы над дверью вспыхивают один за другим. "
        << "Где-то внутри стены медленно вращаются механизмы блокировки.\n";
    wait_ms(10000);

    std::cout << "На экране появляется сообщение:\n\n";
    wait_ms(10000);

    std::cout << "АВАРИЙНАЯ БЛОКИРОВКА АКТИВИРОВАНА.\n";
    wait_ms(10000);

    std::cout << "ДО ПОЛНОЙ ИЗОЛЯЦИИ СЕКТОРА: 04:00.\n";
    wait_ms(6000);

    std::cout << "Ты останавливаешься перед дверью. Марк подходит к панели "
        << "и проводит пальцами по краю корпуса.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Открывай её. Немедленно.\n";
    wait_ms(10000);

    std::cout << "Марк: — Если бы я мог, мы бы уже были по другую сторону.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты слишком хорошо знаешь, что здесь происходит. "
        << "Не говори мне, что ничего не можешь сделать.\n";
    wait_ms(6000);

    std::cout << "Марк не отвечает. Он смотрит не на дверь, а на маленькую "
        << "камеру под потолком.\n";
    wait_ms(6000);

    std::cout << "Марк: — Не стой под камерой.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему?\n";
    wait_ms(10000);

    std::cout << "Марк: — Потому что она уже смотрит на тебя.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "СИСТЕМА: ОБНАРУЖЕН СУБЪЕКТ 01.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ПОДТВЕРЖДЕНИЕ ЛИЧНОСТИ...\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ЛИЧНОСТЬ ПОДТВЕРЖДЕНА.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ПРОЦЕДУРА ИЗОЛЯЦИИ ПРОДОЛЖАЕТСЯ.\n";
    wait_ms(6000);

    std::cout << "Марк резко ударяет ладонью по панели.\n";
    wait_ms(10000);

    std::cout << "Марк: — Нет. Не сейчас...\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что значит «субъект 01»?\n";
    wait_ms(10000);

    std::cout << "Марк: — Сейчас это не имеет значения. Нам нужно выбраться.\n";
    wait_ms(10000);

    std::cout << "Он снимает защитную крышку с соседней панели. Внутри видны "
        << "провода, старый разъём и небольшой экран ручного управления.\n";
    wait_ms(10000);

    std::cout << "На крышке выгравировано предупреждение:\n\n";
    wait_ms(10000);

    std::cout << "\"РУЧНОЕ ВМЕШАТЕЛЬСТВО МОЖЕТ ПРИВЕСТИ "
        << "К НЕОБРАТИМОЙ ИЗОЛЯЦИИ СЕКТОРА\".\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Отлично. Даже дверь предупреждает нас, что всё закончится плохо.\n";
    wait_ms(10000);

    std::cout << "Марк: — Эта система не должна была активироваться. "
        << "Кто-то запустил протокол раньше времени.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Кто?\n";
    wait_ms(10000);

    std::cout << "Марк молчит.\n";
    wait_ms(10000);

    std::cout << "Из динамика раздаётся тихий треск. Затем звучит голос Леи.\n";
    wait_ms(10000);

    std::cout << "Лея: — Если ты меня слышишь, не позволяй ему вводить код.\n";
    wait_ms(6000);

    std::cout << "Марк медленно поднимает голову к динамику.\n";
    wait_ms(10000);

    std::cout << "Марк: — Лея, замолчи. Ты не понимаешь, что происходит.\n";
    wait_ms(10000);

    std::cout << "Лея: — Я понимаю больше, чем ты думаешь. "
        << "Он уже однажды сделал этот выбор.\n";
    wait_ms(6000);

    std::cout << "Связь обрывается.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Какой выбор? Что я сделал?\n";
    wait_ms(10000);

    std::cout << "Марк: — Не слушай её. Она пытается заставить тебя вспомнить.\n";
    wait_ms(6000);

    std::cout << "Ты смотришь на панель. На экране доступны несколько команд. "
        << "Одна из них может помочь открыть дверь. По крайней мере, так кажется.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Попытаться ввести аварийный код.                    выбрать 1\n";
    std::cout << "Осмотреть панель и найти ручной переключатель.      выбрать 2\n";
    std::cout << "Отойти от двери и потребовать объяснений у Марка.    выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_lockdown_action)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_lockdown_action = 0;
        }

        if (choice_ch04_lockdown_action < 1 ||
            choice_ch04_lockdown_action > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_lockdown_action < 1 ||
        choice_ch04_lockdown_action > 3);

    clear_screen();

    if (choice_ch04_lockdown_action == 1) {
        std::cout << "Ты выбираешь попытаться ввести аварийный код.\n";
        wait_ms(10000);

        std::cout << "Марк: — Подожди! Ты не знаешь, что произойдёт!\n";
        wait_ms(10000);

        std::cout << "Но ты уже открыл окно ввода. На экране появляются "
            << "четыре пустых символа.\n";
        wait_ms(10000);

        std::cout << "В памяти вспыхивает странный образ: белая комната, "
            << "стол, человек за стеклом и цифры, которые ты почему-то знаешь.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Я... я знаю этот код.\n";
        wait_ms(10000);

        std::cout << "Марк: — Нет. Ты не мог его помнить.\n";
        wait_ms(10000);

        std::cout << "Ты вводишь последние цифры.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: КОД ПРИНЯТ.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ПОЛЬЗОВАТЕЛЬ НЕ ИМЕЕТ ПРАВА "
            << "СНИМАТЬ БЛОКИРОВКУ.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: ЗАПУСК ПРОТОКОЛА ИЗОЛЯЦИИ.\n";
        wait_ms(6000);

        std::cout << "Марк хватает тебя за руку и оттаскивает от панели.\n";
        wait_ms(10000);

        std::cout << "Марк: — Теперь она знает, что ты вспомнил.\n";
        wait_ms(6000);
    }
    else if (choice_ch04_lockdown_action == 2) {
        std::cout << "Ты снимаешь защитную крышку и начинаешь осматривать "
            << "провода внутри панели.\n";
        wait_ms(6000);

        std::cout << "Под слоем пыли обнаруживается красный переключатель. "
            << "Рядом с ним написано: «АВАРИЙНЫЙ СБРОС».\n";
        wait_ms(6000);

        std::cout << player.name_of_character
            << ": — Кажется, я нашёл способ.\n";
        wait_ms(10000);

        std::cout << "Марк: — Не трогай его! Этот переключатель не открывает "
            << "дверь. Он отключает внутренние контуры.\n";
        wait_ms(6000);

        std::cout << "Ты замечаешь, что под переключателем есть ещё одна надпись, "
            << "частично скрытая царапинами: «ПРИ ПОДТВЕРЖДЕНИИ СУБЪЕКТА 01 "
            << "СБРОС НЕВОЗМОЖЕН».\n";
        wait_ms(10000);

        std::cout << "Ты всё равно нажимаешь на переключатель.\n";
        wait_ms(10000);

        std::cout << "На мгновение гаснет свет. Затем аварийные лампы "
            << "загораются ещё ярче.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: РУЧНОЙ СБРОС ОТКЛОНЁН.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ВНЕШНИЙ КОНТУР ЗАПЕЧАТАН.\n";
        wait_ms(10000);

        std::cout << "Марк: — Теперь мы не сможем открыть её даже с терминала.\n";
        wait_ms(6000);

        std::cout << "За стеной раздаётся тяжёлый грохот. "
            << "Один за другим отключаются индикаторы соседних дверей.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_lockdown_action == 3) {
        std::cout << "Ты отходишь от панели и встаёшь напротив Марка.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Хватит. Ты знаешь, кто запустил блокировку. "
            << "И ты знаешь, кто такой субъект 01.\n";
        wait_ms(10000);

        std::cout << "Марк смотрит на дверь, потом на тебя.\n";
        wait_ms(10000);

        std::cout << "Марк: — Я пытался не допустить этого.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Что именно ты пытался не допустить?\n";
        wait_ms(10000);

        std::cout << "Марк: — Твоего возвращения в комнату 01.\n";
        wait_ms(6000);

        std::cout << "На несколько секунд всё вокруг словно замирает. "
            << "Ты слышишь только гул вентиляции и собственное дыхание.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Я уже был там?\n";
        wait_ms(10000);

        std::cout << "Марк: — Да. И ты сам попросил нас закончить процедуру.\n";
        wait_ms(10000);

        std::cout << "В голове вспыхивает короткий образ: твоя рука на стекле, "
            << "красный свет и голос, повторяющий: «Не оставляйте меня здесь».\n";
        wait_ms(10000);

        std::cout << "Ты делаешь шаг назад. Марк пытается что-то сказать, "
            << "но его голос тонет в сигнале тревоги.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: ОБНАРУЖЕНО ВОССТАНОВЛЕНИЕ ПАМЯТИ.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ПРОТОКОЛ ИЗОЛЯЦИИ УСКОРЕН.\n";
        wait_ms(6000);
    }

    clear_screen();

    std::cout << "=== АВАРИЙНЫЙ КОНТУР ===\n";
    wait_ms(10000);

    std::cout << "Дверь издаёт протяжный металлический скрежет. "
        << "По её краям загораются тонкие полосы красного света.\n";
    wait_ms(6000);

    std::cout << "На панели появляются две команды:\n\n";
    wait_ms(10000);

    std::cout << "1. ОСТАТЬСЯ В СЕКТОРЕ.\n";
    wait_ms(10000);

    std::cout << "2. ПОДТВЕРДИТЬ ПОЛНУЮ ИЗОЛЯЦИЮ.\n";
    wait_ms(10000);

    std::cout << "Марк: — Не нажимай ничего. Это не выход. "
        << "Система пытается заставить нас подтвердить протокол.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — А если ничего не делать?\n";
    wait_ms(10000);

    std::cout << "Марк: — Тогда она выберет за нас.\n";
    wait_ms(10000);

    std::cout << "С потолка доносится щелчок. Вентиляция прекращает работу. "
        << "Двери позади вас начинают закрываться одна за другой.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Остаться в секторе и ждать.                         выбрать 1\n";
    std::cout << "Подтвердить полную изоляцию.                        выбрать 2\n";
    std::cout << "Попросить Марка принять решение.                    выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_lockdown_panel)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_lockdown_panel = 0;
        }

        if (choice_ch04_lockdown_panel < 1 ||
            choice_ch04_lockdown_panel > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_lockdown_panel < 1 ||
        choice_ch04_lockdown_panel > 3);

    clear_screen();

    if (choice_ch04_lockdown_panel == 1) {
        std::cout << "Ты решаешь ничего не нажимать.\n";
        wait_ms(10000);

        std::cout << "Секунды тянутся мучительно медленно. "
            << "Экран начинает гаснуть, а затем снова загорается.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: ОТСУТСТВИЕ ОТВЕТА РАСЦЕНИВАЕТСЯ "
            << "КАК ОТКАЗ ОТ СОТРУДНИЧЕСТВА.\n";
        wait_ms(6000);
    }
    else if (choice_ch04_lockdown_panel == 2) {
        std::cout << "Ты нажимаешь кнопку подтверждения.\n";
        wait_ms(10000);

        std::cout << "Марк: — Нет!\n";
        wait_ms(10000);

        std::cout << "Экран вспыхивает белым светом.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ПОДТВЕРЖДЕНИЕ ПРИНЯТО.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: СЕКТОР БУДЕТ ИЗОЛИРОВАН.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_lockdown_panel == 3) {
        std::cout << "Ты поворачиваешься к Марку.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Решай сам. Ты ведь уже принимал это решение раньше.\n";
        wait_ms(6000);

        std::cout << "Марк долго смотрит на экран. Затем медленно нажимает "
            << "кнопку подтверждения.\n";
        wait_ms(6000);

        std::cout << "Марк: — Прости меня.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ПОДТВЕРЖДЕНИЕ ПРИНЯТО.\n";
        wait_ms(10000);
    }

    clear_screen();

    std::cout << "=== Глава 3.9. Последний протокол ===\n";
    wait_ms(6000);

    std::cout << "Красный свет заполняет коридор. Массивная дверь начинает "
        << "закрываться, хотя никто не прикасается к панели.\n";
    wait_ms(10000);

    std::cout << "Марк бросается к механизму и пытается удержать створку. "
        << "Металл даже не замедляется.\n";
    wait_ms(10000);

    std::cout << "Марк: — Отойди от двери!\n";
    wait_ms(10000);

    std::cout << "Ты слышишь, как за стенами срабатывают замки. "
        << "Сначала один. Потом второй. Затем десятки других.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ИЗОЛЯЦИЯ СЕКТОРА — 90 ПРОЦЕНТОВ.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ПОДГОТОВКА К ОЧИСТКЕ ПАМЯТИ.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Очистке памяти? Марк, что она собирается сделать?\n";
    wait_ms(6000);

    std::cout << "Марк медленно опускает руки. Впервые он выглядит не испуганным, "
        << "а полностью побеждённым.\n";
    wait_ms(10000);

    std::cout << "Марк: — То же самое, что и в прошлый раз.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты сказал, что я сам попросил вас закончить процедуру.\n";
    wait_ms(6000);

    std::cout << "Марк: — Ты просил прекратить боль. Мы решили, что удаление "
        << "воспоминаний будет милосерднее, чем оставить тебе правду.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Какую правду?\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты был не просто участником проекта ECHO. "
        << "Ты помог создать его последнюю процедуру.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты знал, что система однажды начнёт восстанавливать "
        << "твои воспоминания. Ты оставил себе подсказки. Файлы, записи, "
        << "голос Леи. Ты предусмотрел всё, кроме одного.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Чего?\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты забыл, почему сам захотел всё забыть.\n";
    wait_ms(10000);

    std::cout << "В голове вспыхивают обрывки воспоминаний: белая комната, "
        << "стекло, рука на кнопке и собственный голос, произносящий: "
        << "«Если я вернусь, начни сначала».\n";
    wait_ms(8000);

    std::cout << "СИСТЕМА: ИЗОЛЯЦИЯ СЕКТОРА — 100 ПРОЦЕНТОВ.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: НАЧАЛО ПРОЦЕДУРЫ ВОССТАНОВЛЕНИЯ.\n";
    wait_ms(6000);

    std::cout << "Марк подходит к тебе, но между вами загорается полупрозрачная "
        << "защитная перегородка.\n";
    wait_ms(6000);

    std::cout << "Марк: — Я пытался дать тебе больше времени.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты мог просто сказать правду.\n";
    wait_ms(10000);

    std::cout << "Марк: — В прошлый раз я сказал. И ты всё равно выбрал забыть.\n";
    wait_ms(10000);

    std::cout << "Свет начинает мерцать. Звуки коридора становятся глухими, "
        << "будто ты погружаешься под воду.\n";
    wait_ms(10000);

    std::cout << "Последнее, что ты видишь, — лицо Марка за перегородкой. "
        << "Он беззвучно произносит твоё имя.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "СИСТЕМА: ВОСПОМИНАНИЯ СУБЪЕКТА 01 ОЧИЩЕНЫ.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: ЦИКЛ ВОССТАНОВЛЕНИЯ ЗАВЕРШЁН.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: ПОДГОТОВКА К ПОВТОРНОМУ ЗАПУСКУ...\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                         Н О К С - 7                          \n";
    std::cout << "==============================================================\n";
    wait_ms(6000);

    std::cout << "Ты открываешь глаза.\n";
    wait_ms(10000);

    std::cout << "Белый потолок. Холодный воздух. Тихий гул вентиляции.\n";
    wait_ms(6000);

    std::cout << "За стеклом стоит человек в тёмной одежде.\n";
    wait_ms(10000);

    std::cout << "Марк: — Доброе утро. Ты меня слышишь?\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Где я?\n";
    wait_ms(10000);

    std::cout << "Марк смотрит на планшет и делает короткую отметку.\n";
    wait_ms(10000);

    std::cout << "Марк: — Всё в порядке. Процедура прошла успешно.\n";
    wait_ms(6000);

    std::cout << "За его спиной на экране появляется надпись:\n\n";
    wait_ms(10000);

    std::cout << "ECHO-7 — ЦИКЛ 01.\n";
    wait_ms(10000);

    std::cout << "ВОССТАНОВЛЕНИЕ ПАМЯТИ: ОЖИДАНИЕ.\n";
    wait_ms(6000);

    std::cout << "Ты пытаешься вспомнить, что произошло, но в голове пусто.\n";
    wait_ms(6000);

    std::cout << "Где-то в глубине комнаты раздаётся едва слышный женский голос:\n";
    wait_ms(10000);

    std::cout << "Лея: — Если ты слышишь это... не доверяй Марку.\n";
    wait_ms(10000);

    std::cout << "Экран гаснет.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                         Н О К С - 7                          \n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "ЦИКЛ ЗАВЕРШЁН.\n";
    wait_ms(10000);

    std::cout << "ИСТОРИЯ НАЧИНАЕТСЯ ЗАНОВО...\n";
    wait_ms(10000);

    clear_screen();

    start_game();
}

//------------------------------4глава выбор игрокa: Проверить резервное питание. 3 ответа-
 
// выбор игрока : Переключить питание на аварийный режим.   концовка 4 главы
void show_ch04_mark_backup_power()
{
    clear_screen();

    std::cout << "=== Глава 4.9. Резервное питание ===\n";
    wait_ms(6000);

    std::cout << "Коридор погружается во тьму. Аварийные лампы мерцают "
        << "последними вспышками, а вентиляция постепенно останавливается.\n";
    wait_ms(10000);

    std::cout << "Ты слышишь, как где-то глубоко под полом выключаются "
        << "механизмы. Комплекс больше не похож на работающую лабораторию. "
        << "Он напоминает огромную мёртвую машину.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк, что происходит с питанием?\n";
    wait_ms(10000);

    std::cout << "Марк: — Основная сеть отключается. Если мы не восстановим "
        << "резервное питание, двери останутся заблокированными.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Тогда веди меня к генератору.\n";
    wait_ms(10000);

    std::cout << "Марк кивает и открывает техническую дверь. "
        << "За ней находится узкий коридор, ведущий в энергетический сектор.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "ЭНЕРГЕТИЧЕСКИЙ СЕКТОР\n";
    wait_ms(10000);

    std::cout << "Внутри холодно. Вдоль стен стоят электрические шкафы, "
        << "а в центре помещения находится массивный резервный генератор.\n";
    wait_ms(10000);

    std::cout << "На его корпусе горит жёлтый индикатор. Рядом с ним "
        << "расположен распределитель с тремя переключателями.\n";
    wait_ms(6000);

    std::cout << "Марк открывает панель управления.\n";
    wait_ms(10000);

    std::cout << "Марк: — Генератор исправен. Но кто-то отключил его "
        << "от аварийного контура.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Можно просто подключить его обратно?\n";
    wait_ms(10000);

    std::cout << "Марк: — Можно. Но после этого система автоматически "
        << "получит питание.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Какая система?\n";
    wait_ms(10000);

    std::cout << "Марк молчит. На его лице появляется выражение, "
        << "которого ты раньше не замечал: страх.\n";
    wait_ms(6000);

    std::cout << "Марк: — ECHO-7.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Значит, она всё ещё существует?\n";
    wait_ms(10000);

    std::cout << "Марк: — Она никогда не отключалась полностью. "
        << "Мы лишь ограничили её доступ к некоторым системам.\n";
    wait_ms(10000);

    std::cout << "Ты замечаешь небольшую наклейку на распределителе:\n\n";
    wait_ms(10000);

    std::cout << "\"АВАРИЙНОЕ ПИТАНИЕ — НЕ ВКЛЮЧАТЬ "
        << "ПРИ АКТИВНОМ ПРОТОКОЛЕ ECHO-7\".\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — А протокол сейчас активен?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я не знаю.\n";
    wait_ms(10000);

    std::cout << "Он отводит взгляд.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Ты снова врёшь.\n";
    wait_ms(10000);

    std::cout << "Марк: — Я пытаюсь сохранить тебе жизнь.\n";
    wait_ms(10000);

    std::cout << "В этот момент из динамика доносится слабый женский голос.\n";
    wait_ms(10000);

    std::cout << "Лея: — Если вы меня слышите, не включайте аварийное питание.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Лея? Что произойдёт, если мы его включим?\n";
    wait_ms(10000);

    std::cout << "Лея: — Архив проснётся. И вместе с ним проснётся то, "
        << "что вы пытались похоронить.\n";
    wait_ms(6000);

    std::cout << "Марк резко отключает динамик.\n";
    wait_ms(10000);

    std::cout << "Марк: — Она не знает всей картины.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — А ты знаешь?\n";
    wait_ms(10000);

    std::cout << "Марк не отвечает.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "РЕЗЕРВНЫЙ РАСПРЕДЕЛИТЕЛЬ\n\n";
    wait_ms(10000);

    std::cout << "ОСНОВНОЕ ПИТАНИЕ: ОТКЛЮЧЕНО.\n";
    wait_ms(10000);

    std::cout << "РЕЗЕРВНЫЙ ГЕНЕРАТОР: ГОТОВ.\n";
    wait_ms(10000);

    std::cout << "АВАРИЙНЫЙ КОНТУР: НЕ ПОДКЛЮЧЁН.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА ECHO-7: СОСТОЯНИЕ НЕИЗВЕСТНО.\n";
    wait_ms(10000);

    std::cout << "На экране появляется запрос ручного управления.\n";
    wait_ms(10000);

    std::cout << "Марк: — Решать тебе. Но времени почти не осталось.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Проверить соединения генератора.                    выбрать 1\n";
    std::cout << "Переключить питание на аварийный режим.              выбрать 2\n";
    std::cout << "Отказаться и потребовать от Марка правду.             выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_backup_power)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_backup_power = 0;
        }

        if (choice_ch04_backup_power < 1 ||
            choice_ch04_backup_power > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_backup_power < 1 ||
        choice_ch04_backup_power > 3);

    clear_screen();

    if (choice_ch04_backup_power == 1) {
        std::cout << "Ты решаешь проверить соединения перед запуском.\n";
        wait_ms(10000);

        std::cout << "Открыв нижнюю часть распределителя, ты обнаруживаешь "
            << "отсоединённый кабель и сломанную защитную пломбу.\n";
        wait_ms(6000);

        std::cout << "На кабеле сохранилась маркировка: "
            << "\"ECHO-7 — АВАРИЙНЫЙ КОНТУР\".\n";
        wait_ms(6000);

        std::cout << player.name_of_character
            << ": — Кто-то отключил его специально.\n";
        wait_ms(10000);

        std::cout << "Марк: — Да. Чтобы система не смогла запуститься "
            << "после отключения основной сети.\n";
        wait_ms(6000);

        std::cout << "Ты подключаешь кабель обратно. Генератор начинает "
            << "работать, а индикаторы один за другим становятся зелёными.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_backup_power == 2) {
        std::cout << "Ты сразу выбираешь аварийный режим.\n";
        wait_ms(10000);

        std::cout << "Марк: — Подожди! Ты не знаешь, что запускаешь!\n";
        wait_ms(10000);

        std::cout << "Ты подтверждаешь переключение.\n";
        wait_ms(10000);

        std::cout << "Распределитель начинает вибрировать. "
            << "Генератор запускается с громким металлическим гулом.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_backup_power == 3) {
        std::cout << "Ты отходишь от панели и поворачиваешься к Марку.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Я никуда не пойду, пока ты не расскажешь правду.\n";
        wait_ms(10000);

        std::cout << "Марк: — Если мы не восстановим питание, "
            << "ты никогда не узнаешь, что произошло.\n";
        wait_ms(6000);

        std::cout << player.name_of_character
            << ": — Это угроза?\n";
        wait_ms(10000);

        std::cout << "Марк: — Нет. Это предупреждение.\n";
        wait_ms(10000);

        std::cout << "Внезапно генератор запускается самостоятельно. "
            << "На распределителе загораются все индикаторы.\n";
        wait_ms(10000);
    }

    clear_screen();

    std::cout << "СИСТЕМА: РЕЗЕРВНЫЙ ИСТОЧНИК ПИТАНИЯ АКТИВЕН.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ВОССТАНОВЛЕНИЕ ЦЕНТРАЛЬНОГО АРХИВА.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ПОДКЛЮЧЕНИЕ К ECHO-7.\n";
    wait_ms(6000);

    std::cout << "По всему комплексу загорается свет. Двери разблокируются "
        << "на долю секунды, после чего снова запираются.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Мы же хотели открыть двери. Почему они закрылись?\n";
    wait_ms(10000);

    std::cout << "Марк: — Потому что система получила полный контроль.\n";
    wait_ms(6000);

    std::cout << "На экране появляется новое сообщение:\n\n";
    wait_ms(10000);

    std::cout << "СУБЪЕКТ 01 ОБНАРУЖЕН.\n";
    wait_ms(10000);

    std::cout << "ВОССТАНОВЛЕНИЕ ПАМЯТИ: 12 ПРОЦЕНТОВ.\n";
    wait_ms(10000);

    std::cout << "ВОССТАНОВЛЕНИЕ ПАМЯТИ: 38 ПРОЦЕНТОВ.\n";
    wait_ms(10000);

    std::cout << "ВОССТАНОВЛЕНИЕ ПАМЯТИ: 67 ПРОЦЕНТОВ.\n";
    wait_ms(10000);

    std::cout << "Ты хватаешься за голову. Перед глазами возникают "
        << "обрывки чужих воспоминаний: белая комната, стеклянная стена, "
        << "Лея и человек, который выглядит точно как ты.\n";
    wait_ms(8000);

    std::cout << player.name_of_character
        << ": — Кто этот человек?\n";
    wait_ms(10000);

    std::cout << "Марк: — Это ты.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Нет. Я никогда не был здесь.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты был здесь много раз.\n";
    wait_ms(6000);

    std::cout << "ВОССТАНОВЛЕНИЕ ПАМЯТИ: 91 ПРОЦЕНТ.\n";
    wait_ms(10000);

    std::cout << "Ты видишь лабораторию. Лея стоит перед экраном, "
        << "а ты сам произносишь слова, от которых становится холодно:\n";
    wait_ms(10000);

    std::cout << "\"Если я снова вспомню всё, вы должны начать цикл заново\".\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Я сам попросил вас стереть мою память?\n";
    wait_ms(10000);

    std::cout << "Марк: — Да. Но мы не должны были повторять процедуру "
        << "столько раз.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: ВОССТАНОВЛЕНИЕ ПАМЯТИ ЗАВЕРШЕНО.\n";
    wait_ms(6000);

    std::cout << "Марк пытается выключить распределитель. "
        << "На его пропуске загорается красный индикатор.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: СОТРУДНИК M. — ДОСТУП ОТОЗВАН.\n";
    wait_ms(10000);

    std::cout << "Марк: — Нет... Она больше не слушается меня.\n";
    wait_ms(10000);

    std::cout << "Из динамика раздаётся голос Леи.\n";
    wait_ms(10000);

    std::cout << "Лея: — Теперь ты вспомнил.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что мне делать?\n";
    wait_ms(10000);

    std::cout << "Лея: — Ничего. Ты уже сделал это раньше.\n";
    wait_ms(6000);

    std::cout << "Вся комната заливается белым светом. "
        << "Генератор продолжает работать, но его гул постепенно исчезает.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ПОДГОТОВКА К СБРОСУ СУБЪЕКТА 01.\n";
    wait_ms(6000);

    std::cout << "Марк бросается к тебе, но между вами опускается "
        << "защитная перегородка.\n";
    wait_ms(6000);

    std::cout << "Марк: — Прости. Я думал, что на этот раз всё будет иначе.\n";
    wait_ms(6000);

    std::cout << "Ты пытаешься ответить, но не можешь вспомнить, "
        << "что хотел сказать.\n";
    wait_ms(6000);

    std::cout << "Твоё имя исчезает из мыслей. Затем исчезают лица. "
        << "Последним пропадает воспоминание о человеке за стеклом.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                    КОНЦОВКА 1 ИЗ 3                           \n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "             Б Е С К О Н Е Ч Н Ы Й   Ц И К Л                 \n\n";
    wait_ms(6000);

    std::cout << "Резервное питание восстановлено.\n";
    wait_ms(10000);

    std::cout << "Система ECHO-7 снова получила контроль над комплексом.\n";
    wait_ms(10000);

    std::cout << "Ты узнал правду, но не смог изменить исход.\n";
    wait_ms(10000);

    std::cout << "Твои воспоминания стёрты. Цикл начинается заново.\n";
    wait_ms(6000);

    std::cout << "\n";
    std::cout << "Марк: — Доброе утро. Ты меня слышишь?\n";
    wait_ms(6000);

    std::cout << "Ты открываешь глаза в белой комнате.\n";
    wait_ms(10000);

    std::cout << "За стеклом стоит человек в тёмной одежде.\n";
    wait_ms(10000);

    std::cout << "Марк: — Всё в порядке. Процедура прошла успешно.\n";
    wait_ms(6000);

    std::cout << "На экране появляется надпись:\n\n";
    wait_ms(10000);

    std::cout << "ECHO-7 — ЦИКЛ 01.\n";
    wait_ms(10000);

    std::cout << "СТАТУС СУБЪЕКТА: СТАБИЛЕН.\n";
    wait_ms(10000);

    std::cout << "ВОССТАНОВЛЕНИЕ ПАМЯТИ: ОЖИДАНИЕ.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                       КОНЦОВКА 1                              \n";
    std::cout << "                    БЕСКОНЕЧНЫЙ ЦИКЛ                          \n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты не смог выбраться из Нокс-7.\n";
    std::cout << "Но, возможно, в следующий раз всё будет иначе...\n\n";

    std::cout << "ИГРА ОКОНЧЕНА.\n";

    wait_ms(10000);
}
// выбор игрока : Проследить кабель внутрь стены.           концовка 4 главы
void show_ch04_mark_follow_cable()
{
    clear_screen();

    std::cout << "=== Глава 4.10. За стеной ===\n";
    wait_ms(6000);

    std::cout << "Ты замечаешь толстый чёрный кабель, уходящий под металлическую "
        << "панель в стене. На его поверхности видны следы оплавления, "
        << "а возле крепления мигает маленький красный индикатор.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Марк, посмотри сюда. Этот кабель куда-то ведёт.\n";
    wait_ms(10000);

    std::cout << "Марк подходит ближе, но, заметив маркировку на кабеле, "
        << "резко останавливается.\n";
    wait_ms(10000);

    std::cout << "Марк: — Не трогай его.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему? Что находится за этой стеной?\n";
    wait_ms(10000);

    std::cout << "Марк: — Старые технические помещения. Они давно отключены.\n";
    wait_ms(10000);

    std::cout << "Ты снова смотришь на кабель. Его индикатор продолжает мигать. "
        << "Если помещения отключены, откуда поступает питание?\n";
    wait_ms(6000);

    std::cout << "На изоляции можно разобрать надпись:\n\n";
    wait_ms(10000);

    std::cout << "ECHO-7 / ЛИНИЯ ПАМЯТИ / КАНАЛ 01.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Ты говорил, что ECHO-7 находится в центральном архиве.\n";
    wait_ms(10000);

    std::cout << "Марк: — Я говорил, что архив управляет системой.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Это не одно и то же.\n";
    wait_ms(10000);

    std::cout << "Марк отворачивается. Ты понимаешь, что он знает больше, "
        << "чем собирается рассказать.\n";
    wait_ms(6000);

    std::cout << "Внезапно кабель начинает вибрировать. Изнутри стены "
        << "доносится тихий металлический стук. Три удара. Пауза. "
        << "Ещё три удара.\n";
    wait_ms(10000);

    std::cout << "Марк: — Отойди от стены.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Там кто-то есть?\n";
    wait_ms(10000);

    std::cout << "Марк: — Там не должно никого быть.\n";
    wait_ms(10000);

    std::cout << "Из стены раздаётся едва слышный голос:\n";
    wait_ms(10000);

    std::cout << "Голос: — Не позволяй ему открыть дверь.\n";
    wait_ms(10000);

    std::cout << "Марк бледнеет.\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты ничего не слышал.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Я прекрасно всё слышал.\n";
    wait_ms(10000);

    std::cout << "Голос за стеной повторяет твоё имя. Причём произносит его "
        << "точно так же, как ты сам произносишь его во сне.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Проследить кабель, не трогая его.                    выбрать 1\n";
    std::cout << "Потребовать от Марка объяснений.                      выбрать 2\n";
    std::cout << "Немедленно вскрыть панель в стене.                    выбрать 3\n";
    std::cout << "Попытаться связаться с Леей.                          выбрать 4\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_follow_cable)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_follow_cable = 0;
        }

        if (choice_ch04_follow_cable < 1 ||
            choice_ch04_follow_cable > 4) {
            std::cout << "Некорректный выбор. Введите число от 1 до 4: ";
        }
    } while (choice_ch04_follow_cable < 1 ||
        choice_ch04_follow_cable > 4);

    clear_screen();

    if (choice_ch04_follow_cable == 1) {
        std::cout << "Ты решаешь проследить кабель, не прикасаясь к нему.\n";
        wait_ms(10000);

        std::cout << "Он проходит вдоль пола, скрывается за шкафом "
            << "и уходит в узкую щель между бетонными плитами.\n";
        wait_ms(6000);

        std::cout << "На полу ты замечаешь свежие царапины. "
            << "Похоже, шкаф недавно двигали.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Марк, кто передвигал этот шкаф?\n";
        wait_ms(10000);

        std::cout << "Марк: — Не знаю.\n";
        wait_ms(10000);

        std::cout << "Но его взгляд направлен не на шкаф, а на щель за ним.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_follow_cable == 2) {
        std::cout << "Ты перекрываешь Марку дорогу.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Ты знаешь, куда ведёт этот кабель. "
            << "Скажи мне правду.\n";
        wait_ms(6000);

        std::cout << "Марк: — За стеной находится старый наблюдательный блок.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Почему ты сказал, что помещения отключены?\n";
        wait_ms(10000);

        std::cout << "Марк: — Потому что люди, которые там работали, "
            << "больше не должны были иметь доступа к системе.\n";
        wait_ms(6000);

        std::cout << player.name_of_character
            << ": — А что с ними произошло?\n";
        wait_ms(10000);

        std::cout << "Марк открывает рот, но из динамика раздаётся резкий "
            << "сигнал. На экране появляется предупреждение о потере связи.\n";
        wait_ms(6000);

        std::cout << "Марк: — Мы должны уходить. Сейчас.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_follow_cable == 3) {
        std::cout << "Ты хватаешься за край металлической панели "
            << "и пытаешься снять её.\n";
        wait_ms(10000);

        std::cout << "Марк: — Стой! Там высокое напряжение!\n";
        wait_ms(10000);

        std::cout << "Ты отдёргиваешь руку. На металле остаётся "
            << "небольшой след от искры.\n";
        wait_ms(10000);

        std::cout << "В стене щёлкает реле. За панелью включается "
            << "внутренний механизм, которого не видно снаружи.\n";
        wait_ms(6000);

        std::cout << "Марк: — Теперь система точно знает, что мы здесь.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Что я только что активировал?\n";
        wait_ms(10000);

        std::cout << "Марк: — Не знаю. И это самое плохое.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_follow_cable == 4) {
        std::cout << "Ты открываешь терминал и пытаешься вызвать Лею.\n";
        wait_ms(10000);

        std::cout << "Несколько секунд слышны только помехи. "
            << "Затем на экране появляется короткое сообщение.\n";
        wait_ms(10000);

        std::cout << "ЛЕЯ: НЕ ИЩИ ВЫХОД. ИЩИ КОМНАТУ ЗА СТЕНОЙ.\n";
        wait_ms(6000);

        std::cout << player.name_of_character
            << ": — Что находится в этой комнате?\n";
        wait_ms(10000);

        std::cout << "ЛЕЯ: ТАМ НАЧАЛАСЬ ПРОЦЕДУРА.\n";
        wait_ms(10000);

        std::cout << "ЛЕЯ: И ТАМ ЖЕ ТЫ ПОПРОСИЛ НАС ЕЁ ЗАКОНЧИТЬ.\n";
        wait_ms(6000);

        std::cout << "Сообщение исчезает. Терминал перезагружается "
            << "и больше не принимает команды.\n";
        wait_ms(6000);

        std::cout << "Марк: — Я пытался не дать тебе туда вернуться.\n";
        wait_ms(10000);
    }

    clear_screen();

    std::cout << "=== Техническая панель ===\n";
    wait_ms(10000);

    std::cout << "Ты отодвигаешь шкаф. За ним обнаруживается узкая "
        << "металлическая дверца, почти полностью скрытая стеной.\n";
    wait_ms(6000);

    std::cout << "Кабель уходит прямо за неё. На дверце нет ручки, "
        << "только считыватель и небольшой экран.\n";
    wait_ms(6000);

    std::cout << "ЭКРАН: ДОСТУП В НАБЛЮДАТЕЛЬНЫЙ БЛОК.\n";
    wait_ms(10000);

    std::cout << "ЭКРАН: ТРЕБУЕТСЯ ПОДТВЕРЖДЕНИЕ СУБЪЕКТА 01.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему система требует подтверждения именно от меня?\n";
    wait_ms(10000);

    std::cout << "Марк: — Потому что ты был последним, кто входил туда.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Когда?\n";
    wait_ms(10000);

    std::cout << "Марк: — До того, как тебе стёрли память.\n";
    wait_ms(6000);

    std::cout << "На экране появляются три варианта доступа.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Использовать собственный пропуск.                    выбрать 1\n";
    std::cout << "Попросить Марка открыть дверь.                       выбрать 2\n";
    std::cout << "Попытаться обойти считыватель.                       выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_cable_panel)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_cable_panel = 0;
        }

        if (choice_ch04_cable_panel < 1 ||
            choice_ch04_cable_panel > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_cable_panel < 1 ||
        choice_ch04_cable_panel > 3);

    clear_screen();

    if (choice_ch04_cable_panel == 1) {
        std::cout << "Ты прикладываешь пропуск к считывателю.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ЛИЧНОСТЬ ПОДТВЕРЖДЕНА.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ДОБРО ПОЖАЛОВАТЬ ОБРАТНО, СУБЪЕКТ 01.\n";
        wait_ms(6000);

        std::cout << "Марк: — Нет... Твой доступ должен был быть удалён.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_cable_panel == 2) {
        std::cout << "Ты протягиваешь Марку руку.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Открой дверь. Ты ведь можешь это сделать.\n";
        wait_ms(10000);

        std::cout << "Марк прикладывает пропуск. Считыватель издаёт "
            << "долгий низкий сигнал.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: ДОСТУП СОТРУДНИКА M. ОТОЗВАН.\n";
        wait_ms(10000);

        std::cout << "Марк: — Значит, она забрала у меня и это.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_cable_panel == 3) {
        std::cout << "Ты осматриваешь корпус считывателя и замечаешь "
            << "сервисный разъём.\n";
        wait_ms(10000);

        std::cout << "Ты подключаешь кабель терминала. Экран гаснет, "
            << "а затем начинает быстро выводить строки данных.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: НЕСАНКЦИОНИРОВАННЫЙ ДОСТУП.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: РЕЗЕРВНАЯ ПРОВЕРКА ЛИЧНОСТИ.\n";
        wait_ms(10000);

        std::cout << "СИСТЕМА: СУБЪЕКТ 01 ПОДТВЕРЖДЁН.\n";
        wait_ms(10000);

        std::cout << "Марк: — Она узнала тебя даже без пропуска.\n";
        wait_ms(10000);
    }

    clear_screen();

    std::cout << "=== Наблюдательный блок ===\n";
    wait_ms(10000);

    std::cout << "Замок щёлкает. Дверь медленно отходит от стены, "
        << "открывая тёмный проход.\n";
    wait_ms(6000);

    std::cout << "За дверью нет коридора. Только длинное помещение "
        << "с рядами стеклянных камер и кабелями, уходящими в потолок.\n";
    wait_ms(10000);

    std::cout << "На полу лежат сломанные мониторы. Некоторые экраны "
        << "по-прежнему показывают изображение пустых комнат.\n";
    wait_ms(6000);

    std::cout << "В дальней части помещения стоит кресло, окружённое "
        << "датчиками. Рядом находится терминал с надписью ECHO-7.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Это и есть наблюдательный блок?\n";
    wait_ms(10000);

    std::cout << "Марк: — Да.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Здесь проводили процедуры?\n";
    wait_ms(10000);

    std::cout << "Марк: — Здесь их разрабатывали.\n";
    wait_ms(10000);

    std::cout << "Ты замечаешь на стене несколько фотографий сотрудников. "
        << "Большинство лиц повреждены или заклеены чёрными полосами.\n";
    wait_ms(6000);

    std::cout << "На одной фотографии изображены Лея и Марк. Между ними "
        << "стоит человек, чьё лицо удивительно похоже на твоё.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Кто стоит между вами?\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Я не помню этого.\n";
    wait_ms(10000);

    std::cout << "Марк: — Именно поэтому мы здесь.\n";
    wait_ms(10000);

    std::cout << "Терминал внезапно включается. На экране появляется "
        << "список файлов с датами, которые предшествуют всем событиям, "
        << "которые ты помнишь.\n";
    wait_ms(10000);

    std::cout << "Среди них есть файл:\n\n";
    wait_ms(10000);

    std::cout << "ECHO07_SUBJECT01_FINAL_SESSION.\n";
    wait_ms(10000);

    std::cout << "Рядом с файлом мигает надпись: \"ПОСЛЕДНЯЯ СЕССИЯ\".\n";
    wait_ms(10000);

    std::cout << "Марк: — Не открывай его.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Почему?\n";
    wait_ms(10000);

    std::cout << "Марк: — Потому что после этой записи ты поймёшь, "
        << "почему всё повторяется.\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Открыть запись последней сессии.                    выбрать 1\n";
    std::cout << "Проверить другие файлы наблюдения.                  выбрать 2\n";
    std::cout << "Выключить терминал и уйти.                           выбрать 3\n";
    std::cout << "Попросить Марка посмотреть запись первым.            выбрать 4\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_cable_voice)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_cable_voice = 0;
        }

        if (choice_ch04_cable_voice < 1 ||
            choice_ch04_cable_voice > 4) {
            std::cout << "Некорректный выбор. Введите число от 1 до 4: ";
        }
    } while (choice_ch04_cable_voice < 1 ||
        choice_ch04_cable_voice > 4);

    clear_screen();

    if (choice_ch04_cable_voice == 1) {
        std::cout << "Ты открываешь последнюю запись.\n";
        wait_ms(10000);

        std::cout << "На экране появляется изображение этой же комнаты. "
            << "Камера показывает тебя, сидящего в кресле.\n";
        wait_ms(10000);

        std::cout << "Запись сделана задолго до твоего первого воспоминания "
            << "о комплексе.\n";
        wait_ms(6000);
    }
    else if (choice_ch04_cable_voice == 2) {
        std::cout << "Ты открываешь список старых файлов наблюдения.\n";
        wait_ms(10000);

        std::cout << "На нескольких записях виден один и тот же человек "
            << "в кресле. Это ты. На каждой записи ты задаёшь один вопрос:\n";
        wait_ms(10000);

        std::cout << "\"Сколько раз мы уже это делали?\"\n";
        wait_ms(10000);

        std::cout << "Последние файлы повреждены, но на одном из них "
            << "видна дата сегодняшнего дня.\n";
        wait_ms(6000);
    }
    else if (choice_ch04_cable_voice == 3) {
        std::cout << "Ты пытаешься выключить терминал.\n";
        wait_ms(10000);

        std::cout << "Экран не реагирует. Вместо этого запускается "
            << "видеозапись с автоматическим воспроизведением.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: ПОСЛЕДНЯЯ СЕССИЯ ДОЛЖНА БЫТЬ ПРОСМОТРЕНА.\n";
        wait_ms(10000);

        std::cout << "Марк: — Она не позволит нам уйти, пока запись не закончится.\n";
        wait_ms(6000);
    }
    else if (choice_ch04_cable_voice == 4) {
        std::cout << "Ты отходишь от терминала и указываешь на экран.\n";
        wait_ms(10000);

        std::cout << player.name_of_character
            << ": — Смотри первым. Если там что-то опасное, "
            << "ты должен это знать.\n";
        wait_ms(6000);

        std::cout << "Марк смотрит на запись. Его лицо постепенно бледнеет.\n";
        wait_ms(10000);

        std::cout << "Марк: — Я надеялся, что эта запись была уничтожена.\n";
        wait_ms(10000);
    }

    clear_screen();

    std::cout << "=== Последняя сессия ===\n";
    wait_ms(10000);

    std::cout << "Запись воспроизводится автоматически.\n";
    wait_ms(10000);

    std::cout << "На экране ты сидишь в кресле. Рядом стоят Лея и Марк. "
        << "Твои руки пристёгнуты к подлокотникам, а на голове закреплены "
        << "датчики.\n";
    wait_ms(10000);

    std::cout << "Запись: — Субъект 01, вы понимаете, что процедура "
        << "может привести к потере autobiографической памяти?\n";
    wait_ms(6000);

    std::cout << "Человек на записи: — Да.\n";
    wait_ms(10000);

    std::cout << "Запись: — Вы хотите продолжить?\n";
    wait_ms(10000);

    std::cout << "Человек на записи: — Если я снова вспомню, что произошло, "
        << "я не смогу жить дальше. Удалите всё.\n";
    wait_ms(10000);

    std::cout << "Ты отступаешь от экрана.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Я сам попросил их это сделать...\n";
    wait_ms(10000);

    std::cout << "На записи Лея начинает спорить с Марком.\n";
    wait_ms(10000);

    std::cout << "Лея: — Мы не можем повторять это бесконечно. "
        << "Он каждый раз возвращается к началу.\n";
    wait_ms(6000);

    std::cout << "Марк: — У нас нет другого способа удержать его в живых.\n";
    wait_ms(10000);

    std::cout << "Лея: — Тогда расскажи ему правду!\n";
    wait_ms(10000);

    std::cout << "Марк: — Он сам попросил нас не рассказывать.\n";
    wait_ms(10000);

    std::cout << "Изображение начинает искажаться. Последний кадр показывает "
        << "тебя, смотрящего прямо в камеру.\n";
    wait_ms(6000);

    std::cout << "Человек на записи: — Если ты смотришь это снова, "
        << "значит, я опять всё забыл.\n";
    wait_ms(6000);

    std::cout << "Человек на записи: — Не пытайся найти выход. "
        << "Найди причину, по которой ты хотел забыть.\n";
    wait_ms(10000);

    std::cout << "Запись обрывается.\n";
    wait_ms(10000);

    std::cout << "На экране появляется последняя строка:\n\n";
    wait_ms(10000);

    std::cout << "СЕССИЯ 01 — ЗАВЕРШЕНА.\n";
    wait_ms(10000);

    std::cout << "СЕССИЯ 02 — ЗАВЕРШЕНА.\n";
    wait_ms(10000);

    std::cout << "СЕССИЯ 03 — ЗАВЕРШЕНА.\n";
    wait_ms(10000);

    std::cout << "СЕССИЯ 04 — ЗАВЕРШЕНА.\n";
    wait_ms(10000);

    std::cout << "СЕССИЯ 05 — ЗАВЕРШЕНА.\n";
    wait_ms(10000);

    std::cout << "ТЕКУЩАЯ СЕССИЯ — 07.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — А что произошло в шестой сессии?\n";
    wait_ms(10000);

    std::cout << "Марк не отвечает.\n";
    wait_ms(10000);

    std::cout << "На экране появляется сообщение:\n\n";
    wait_ms(10000);

    std::cout << "СЕССИЯ 06: ПОВРЕЖДЕНА.\n";
    wait_ms(10000);

    std::cout << "ПРИЧИНА: ВМЕШАТЕЛЬСТВО СУБЪЕКТА 01.\n";
    wait_ms(6000);

    std::cout << "Марк: — Ты не должен был увидеть эту запись.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Что я сделал в шестой сессии?\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты открыл дверь, которую мы пытались удержать закрытой.\n";
    wait_ms(6000);

    std::cout << "Внезапно за спиной раздаётся металлический удар. "
        << "Дверь наблюдательного блока захлопывается.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: НЕСАНКЦИОНИРОВАННЫЙ ПРОСМОТР.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ЗАПУСК ПРОТОКОЛА ИЗОЛЯЦИИ.\n";
    wait_ms(10000);

    std::cout << "Марк бросается к двери, но замок уже заблокирован.\n";
    wait_ms(10000);

    std::cout << "Марк: — Нет. Только не здесь.\n";
    wait_ms(10000);

    std::cout << "Ты слышишь знакомые три удара из стены. "
        << "На этот раз они доносятся со всех сторон.\n";
    wait_ms(10000);

    clear_screen();

    std::cout << "=== Последний выбор ===\n";
    wait_ms(10000);

    std::cout << "На терминале появляется меню управления наблюдательным блоком.\n";
    wait_ms(10000);

    std::cout << "Можно попытаться отключить систему, открыть дверь "
        << "или выяснить, что скрывает шестая сессия.\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Отключить питание терминала.                        выбрать 1\n";
    std::cout << "Попытаться открыть дверь вручную.                   выбрать 2\n";
    std::cout << "Открыть повреждённую шестую сессию.                 выбрать 3\n";
    std::cout << "Остаться рядом с Марком и ждать.                    выбрать 4\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_cable_final)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_cable_final = 0;
        }

        if (choice_ch04_cable_final < 1 ||
            choice_ch04_cable_final > 4) {
            std::cout << "Некорректный выбор. Введите число от 1 до 4: ";
        }
    } while (choice_ch04_cable_final < 1 ||
        choice_ch04_cable_final > 4);

    clear_screen();

    if (choice_ch04_cable_final == 1) {
        std::cout << "Ты выдёргиваешь кабель питания терминала.\n";
        wait_ms(10000);

        std::cout << "Экран гаснет, но аварийные лампы продолжают гореть. "
            << "Система работает от отдельного источника.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: ПИТАНИЕ ТЕРМИНАЛА НЕ ВЛИЯЕТ "
            << "НА ПРОТОКОЛ ИЗОЛЯЦИИ.\n";
        wait_ms(6000);
    }
    else if (choice_ch04_cable_final == 2) {
        std::cout << "Ты пытаешься открыть дверь вручную.\n";
        wait_ms(10000);

        std::cout << "Механизм не двигается. Замок заблокирован "
            << "изнутри самой стены.\n";
        wait_ms(6000);

        std::cout << "СИСТЕМА: РУЧНОЕ ОТКРЫТИЕ ЗАПРЕЩЕНО.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_cable_final == 3) {
        std::cout << "Ты открываешь повреждённую шестую сессию.\n";
        wait_ms(10000);

        std::cout << "На экране появляется изображение комнаты за стеной. "
            << "Ты видишь себя стоящим перед открытой дверью.\n";
        wait_ms(6000);

        std::cout << "На записи ты поворачиваешься к камере и говоришь:\n";
        wait_ms(10000);

        std::cout << "\"Если я снова открою эту дверь, не позволяйте "
            << "мне вернуться\".\n";
        wait_ms(6000);

        std::cout << "Запись обрывается.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_cable_final == 4) {
        std::cout << "Ты остаёшься рядом с Марком.\n";
        wait_ms(10000);

        std::cout << "Марк смотрит на тебя и впервые перестаёт "
            << "пытаться оправдаться.\n";
        wait_ms(10000);

        std::cout << "Марк: — Я должен был остановить тебя ещё в первый раз.\n";
        wait_ms(6000);

        std::cout << player.name_of_character
            << ": — Почему ты не остановил?\n";
        wait_ms(10000);

        std::cout << "Марк: — Потому что тогда ты ещё верил мне.\n";
        wait_ms(10000);
    }

    clear_screen();

    std::cout << "=== Протокол завершения ===\n";
    wait_ms(10000);

    std::cout << "Все экраны одновременно гаснут.\n";
    wait_ms(10000);

    std::cout << "Из динамиков раздаётся спокойный голос системы.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: СУБЪЕКТ 01 ВЕРНУЛСЯ В НАБЛЮДАТЕЛЬНЫЙ БЛОК.\n";
    wait_ms(6000);

    std::cout << "СИСТЕМА: УСЛОВИЯ ПОСЛЕДНЕЙ СЕССИИ ВЫПОЛНЕНЫ.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Последней? Что это значит?\n";
    wait_ms(10000);

    std::cout << "Марк медленно поворачивается к креслу в центре комнаты.\n";
    wait_ms(10000);

    std::cout << "Марк: — Это место, где всё началось.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — И где всё закончится?\n";
    wait_ms(10000);

    std::cout << "Марк: — Я надеялся, что ты никогда этого не узнаешь.\n";
    wait_ms(6000);

    std::cout << "Кресло начинает двигаться. За стеной включаются "
        << "механизмы, и кабель, который ты проследил, загорается "
        << "ярким белым светом.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ВОССТАНОВЛЕНИЕ ПОЛНОЙ ЗАПИСИ.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: СУБЪЕКТ 01 — ИСТОЧНИК ПРОТОКОЛА.\n";
    wait_ms(6000);

    std::cout << player.name_of_character
        << ": — Источник? Я не создавал эту систему!\n";
    wait_ms(10000);

    std::cout << "Марк: — Ты не создавал её один. Но именно ты предложил "
        << "способ сохранять воспоминания после их удаления.\n";
    wait_ms(10000);

    std::cout << "Марк: — ECHO-7 не просто стирает память. Она сохраняет "
        << "её копию и использует её, чтобы восстановить тебя снова.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Значит, все эти комнаты, записи и голоса...\n";
    wait_ms(10000);

    std::cout << "Марк: — Это следы предыдущих циклов.\n";
    wait_ms(10000);

    std::cout << "Система включает последний экран. На нём отображается "
        << "изображение комнаты, в которой ты проснулся в самом начале.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: ПОДГОТОВКА НОВОГО ЦИКЛА.\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: СОХРАНЕНИЕ ТЕКУЩЕЙ ПАМЯТИ...\n";
    wait_ms(10000);

    std::cout << "СИСТЕМА: СОХРАНЕНИЕ ЗАВЕРШЕНО.\n";
    wait_ms(10000);

    std::cout << "Марк: — Прости меня.\n";
    wait_ms(10000);

    std::cout << "Ты пытаешься отойти от кресла, но ноги перестают слушаться. "
        << "Свет становится невыносимо ярким.\n";
    wait_ms(10000);

    std::cout << "Последнее, что ты слышишь, — голос Леи из динамика:\n";
    wait_ms(10000);

    std::cout << "Лея: — В следующий раз найди меня раньше.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                    КОНЦОВКА                                  \n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "                    З А   С Т Е Н О Й                         \n\n";
    wait_ms(6000);

    std::cout << "Ты проследил кабель до его источника.\n";
    wait_ms(10000);

    std::cout << "Ты нашёл комнату, где начался эксперимент.\n";
    wait_ms(10000);

    std::cout << "Ты узнал, что ECHO-7 сохраняет твои воспоминания "
        << "и использует их для запуска новых циклов.\n";
    wait_ms(6000);

    std::cout << "Но ты не смог остановить систему.\n";
    wait_ms(10000);

    std::cout << "Дверь закрывается. Память стирается.\n";
    wait_ms(10000);

    std::cout << "Всё начинается заново.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                         Н О К С - 7                          \n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "Ты открываешь глаза.\n";
    wait_ms(10000);

    std::cout << "Белый потолок. Холодный воздух. Тихий гул вентиляции.\n";
    wait_ms(6000);

    std::cout << "За стеклом стоит человек в тёмной одежде.\n";
    wait_ms(10000);

    std::cout << "Марк: — Доброе утро. Ты меня слышишь?\n";
    wait_ms(6000);

    std::cout << "На экране появляется надпись:\n\n";
    wait_ms(10000);

    std::cout << "ECHO-7 — ЦИКЛ 01.\n";
    wait_ms(10000);

    std::cout << "СТАТУС СУБЪЕКТА: СТАБИЛЕН.\n";
    wait_ms(10000);

    std::cout << "ВОССТАНОВЛЕНИЕ ПАМЯТИ: ОЖИДАНИЕ.\n";
    wait_ms(6000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "                       КОНЦОВКА 2                              \n";
    std::cout << "                         ЗА СТЕНОЙ                             \n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты нашёл правду за стеной.\n";
    std::cout << "Но правда оказалась частью ловушки.\n\n";

    std::cout << "ИГРА ОКОНЧЕНА.\n";

    wait_ms(10000);

    // Концовка завершена.
    // Автоматический перезапуск не выполняется.
}
// выбор игрока : Оставить панель в покое и уйти к двери.
void show_ch04_mark_leave_panel()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "              ГЛАВА 4 — ЗАКРЫТАЯ ДВЕРЬ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты ещё несколько секунд смотришь на панель.\n";
    wait_ms(10000);

    std::cout << "На её поверхности медленно гаснут индикаторы.\n";
    std::cout << "Красный свет исчезает последним.\n";
    wait_ms(10000);

    std::cout << "\nТы решаешь не трогать её.\n";
    std::cout << "Что бы ни скрывал этот механизм, сейчас у тебя нет\n";
    std::cout << "ни времени, ни желания разбираться в нём.\n";
    wait_ms(10000);

    std::cout << "\nТы отходишь от стены и направляешься к двери.\n";
    std::cout << "Каждый шаг отдаётся глухим эхом в пустом коридоре.\n";
    wait_ms(10000);

    std::cout << "\nПозади раздаётся тихий щелчок.\n";
    wait_ms(10000);

    std::cout << "Ты останавливаешься.\n";
    std::cout << "Медленно поворачиваешь голову, но панель выглядит\n";
    std::cout << "точно так же, как и несколько секунд назад.\n";
    wait_ms(10000);

    std::cout << "\nИз динамика над дверью доносится голос ECHO-7:\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Решение принято.\n";
    std::cout << "          Продолжайте движение, "
        << player.name_of_character << ".»\n";
    wait_ms(10000);

    std::cout << "\nТы не помнишь, чтобы называл системе своё имя.\n";
    wait_ms(10000);

    std::cout << "Но сейчас это уже не имеет значения.\n";
    std::cout << "Ты стоишь перед дверью.\n";
    std::cout << "На её панели горит зелёный индикатор.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Открыть дверь и войти.                              выбрать 1\n";
    std::cout << "Проверить, заперта ли дверь.                        выбрать 2\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_leave_panel)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_leave_panel = 0;
        }

        if (choice_ch04_leave_panel < 1 ||
            choice_ch04_leave_panel > 2) {
            std::cout << "Некорректный выбор. Введите 1 или 2: ";
        }
    } while (choice_ch04_leave_panel < 1 ||
        choice_ch04_leave_panel > 2);

    clear_screen();
    flush_input();

    if (choice_ch04_leave_panel == 1) {
        std::cout << "Ты прикладываешь руку к сенсорной панели.\n";
        wait_ms(10000);

        std::cout << "\nЗамок щёлкает.\n";
        std::cout << "Дверь медленно отходит в сторону.\n";
        wait_ms(10000);

        std::cout << "\nЗа ней открывается тёмное помещение.\n";
        std::cout << "Внутри что-то работает.\n";
        std::cout << "Ты слышишь ровный механический гул.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Добро пожаловать в следующий сектор.»\n";
        wait_ms(10000);

        std::cout << "\nТы переступаешь порог.\n";
        wait_ms(10000);

        // show_ch03_mark_next_scene();
    }
    else {
        std::cout << "Ты осторожно касаешься дверной панели.\n";
        wait_ms(10000);

        std::cout << "\nЗамок не сопротивляется.\n";
        std::cout << "Дверь не заперта.\n";
        wait_ms(10000);

        std::cout << "\nНо когда ты убираешь руку, индикатор\n";
        std::cout << "на мгновение становится красным.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Не задерживайтесь у двери.»\n";
        wait_ms(10000);

        std::cout << "\nТы больше не пытаешься понять, что происходит.\n";
        std::cout << "Открываешь дверь и входишь внутрь.\n";
        wait_ms(10000);

        // show_ch03_mark_next_scene();
    }

    std::cout << "\n==============================================================\n";
    std::cout << "                 ПЕРЕХОД В СЛЕДУЮЩУЮ СЦЕНУ\n";
    std::cout << "==============================================================\n";

    wait_ms(10000);
}

//------------------------------4глава выбор игрокa: Осмотреть слепую зону. 3 ответа-

// выбор игрока : Зайти в технический проход вместе с Марком.
void show_ch04_mark_enter_technical_passage()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "             ГЛАВА 4 — ТЕХНИЧЕСКИЙ ПРОХОД\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Марк останавливается возле узкой двери в конце коридора.\n";
    std::cout << "На ней нет номера. Только небольшая табличка:\n";
    wait_ms(10000);

    std::cout << "\nТЕХНИЧЕСКИЙ ДОСТУП. ТОЛЬКО ДЛЯ ПЕРСОНАЛА.\n";
    wait_ms(10000);

    std::cout << "\nМарк достаёт из кармана пластиковую карту.\n";
    std::cout << "Несколько секунд он смотрит на считыватель,\n";
    std::cout << "словно сомневается, стоит ли открывать дверь.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Камеры должны находиться за этим проходом.\n";
    std::cout << "       Если система не врёт, мы сможем добраться\n";
    std::cout << "       до узла наблюдения и проверить записи.»\n";
    wait_ms(10000);

    std::cout << "\nОн проводит картой по считывателю.\n";
    wait_ms(10000);

    std::cout << "\nДверь открывается с тяжёлым металлическим скрежетом.\n";
    wait_ms(10000);

    std::cout << "\nЗа ней находится узкий технический коридор.\n";
    std::cout << "По стенам тянутся кабели, трубы и вентиляционные каналы.\n";
    std::cout << "Редкие лампы мерцают под потолком.\n";
    wait_ms(10000);

    std::cout << "\nИз глубины прохода доносится глухой шум.\n";
    std::cout << "Будто где-то работает огромный механизм.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Держись рядом.\n";
    std::cout << "       Я не знаю, что здесь осталось работать.»\n";
    wait_ms(10000);

    std::cout << "\nВы входите внутрь.\n";
    std::cout << "Дверь за вами медленно закрывается.\n";
    wait_ms(10000);

    std::cout << "\nТы замечаешь небольшую камеру под потолком.\n";
    std::cout << "Её объектив направлен прямо на вас.\n";
    wait_ms(10000);

    std::cout << "\nТы останавливаешься.\n";
    std::cout << "Марк тоже замечает камеру.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Странно...\n";
    std::cout << "       Я думал, этот участок давно отключён.»\n";
    wait_ms(10000);

    std::cout << "\nКамера медленно поворачивается вслед за вами.\n";
    wait_ms(10000);

    std::cout << "\nИз динамика раздаётся короткий треск.\n";
    std::cout << "Затем голос ECHO-7:\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Обнаружены два сотрудника в техническом секторе.\n";
    std::cout << "         Проверка идентификации...»\n";
    wait_ms(10000);

    std::cout << "\nМарк резко поднимает голову к динамику.\n";
    std::cout << "МАРК: «Не обращай внимания. Просто иди.»\n";
    wait_ms(10000);

    std::cout << "\nНо система продолжает говорить.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Идентификация завершена.\n";
    std::cout << "         Субъект 01 подтверждён.\n";
    std::cout << "         Второй субъект не зарегистрирован.»\n";
    wait_ms(10000);

    std::cout << "\nМарк замирает.\n";
    std::cout << "Его лицо становится совершенно неподвижным.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что значит — не зарегистрирован?»\n";
    wait_ms(10000);

    std::cout << "\nДинамик замолкает.\n";
    std::cout << "Камера продолжает следить за вами.\n";
    wait_ms(10000);

    std::cout << "\nМарк поворачивается к тебе.\n";
    std::cout << "МАРК: «Ладно. Нам нужно добраться до камер.\n";
    std::cout << "       Там мы узнаем, что происходит.»\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Спросить Марка, почему система его не видит.        выбрать 1\n";
    std::cout << "Промолчать и продолжить путь к камерам.             выбрать 2\n";
    std::cout << "Осмотреть камеру наблюдения поближе.                выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_enter_technical_passage)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_enter_technical_passage = 0;
        }

        if (choice_ch04_enter_technical_passage < 1 ||
            choice_ch04_enter_technical_passage > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_enter_technical_passage < 1 ||
        choice_ch04_enter_technical_passage > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_enter_technical_passage == 1) {
        std::cout << "ТЫ: «Почему система тебя не видит?»\n";
        wait_ms(10000);

        std::cout << "\nМарк не отвечает сразу.\n";
        std::cout << "Он смотрит на камеру, а затем на тебя.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Потому что она неисправна.\n";
        std::cout << "       Здесь половина оборудования работает неправильно.»\n";
        wait_ms(10000);

        std::cout << "\nОн произносит это слишком быстро.\n";
        std::cout << "И почему-то больше не смотрит тебе в глаза.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_enter_technical_passage == 2) {
        std::cout << "Ты решаешь не задавать вопросов.\n";
        wait_ms(10000);

        std::cout << "\nМарк идёт впереди.\n";
        std::cout << "Его шаги звучат всё громче в узком коридоре.\n";
        wait_ms(10000);

        std::cout << "\nЧерез несколько секунд ты замечаешь,\n";
        std::cout << "что камера больше не поворачивается вслед за вами.\n";
        wait_ms(10000);

        std::cout << "\nОна смотрит на закрытую дверь позади.\n";
        wait_ms(10000);
    }
    else {
        std::cout << "Ты подходишь ближе к камере.\n";
        wait_ms(10000);

        std::cout << "\nНа корпусе видны свежие царапины.\n";
        std::cout << "Кто-то недавно вскрывал защитную крышку.\n";
        wait_ms(10000);

        std::cout << "\nПод объективом мигает крошечный индикатор.\n";
        std::cout << "На металлической пластине выгравирован номер:\n";
        wait_ms(10000);

        std::cout << "\nCAM-01.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Не трогай её.\n";
        std::cout << "       Если она передаёт данные, система уже знает,\n";
        std::cout << "       что мы здесь.»\n";
        wait_ms(10000);

        std::cout << "\nТы отходишь от стены и догоняешь Марка.\n";
        wait_ms(10000);
    }

    std::cout << "\nВы продолжаете идти по техническому проходу.\n";
    std::cout << "Впереди появляется развилка.\n";
    wait_ms(10000);

    std::cout << "\nСлева слышен шум вентиляторов.\n";
    std::cout << "Справа видна дверь с надписью «ЦЕНТР НАБЛЮДЕНИЯ».\n";
    wait_ms(10000);

    std::cout << "\nМарк указывает на дверь справа.\n";
    std::cout << "МАРК: «Нам туда. Камеры должны быть за ней.»\n";
    wait_ms(10000);

    std::cout << "\nВы подходите к двери центра наблюдения.\n";
    std::cout << "На панели доступа уже горит зелёный индикатор.\n";
    wait_ms(10000);

    std::cout << "\nКто-то открыл её заранее.\n";
    wait_ms(10000);

    std::cout << "\n==============================================================\n";
    std::cout << "          КОНЕЦ СЦЕНЫ — ТЕХНИЧЕСКИЙ ПРОХОД\n";
    std::cout << "==============================================================\n";


    wait_ms(10000);
}
// выбор игрока : Осмотреть следы обуви и кабели.
void show_ch04_mark_inspect_footprints_cables()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "            ГЛАВА 4 — СЛЕДЫ В ТЕХНИЧЕСКОМ ПРОХОДЕ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты останавливаешься перед развилкой.\n";
    std::cout << "Марк уже собирается идти дальше, но ты замечаешь\n";
    std::cout << "на полу несколько тёмных следов.\n";
    wait_ms(10000);

    std::cout << "\nОни тянутся вдоль стены, прямо к двери\n";
    std::cout << "центра наблюдения.\n";
    wait_ms(10000);

    std::cout << "\nРядом с ними проходят толстые кабели.\n";
    std::cout << "Часть кабельного покрытия повреждена.\n";
    std::cout << "На металле видны царапины, а возле креплений\n";
    std::cout << "скопилась тонкая серая пыль.\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Подожди, Марк. Здесь что-то есть.»\n";
    wait_ms(10000);

    std::cout << "\nМарк возвращается и приседает рядом с тобой.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Следы обуви и кабели...\n";
    std::cout << "       Что именно ты хочешь проверить?»\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Осмотреть следы обуви на полу.                     выбрать 1\n";
    std::cout << "Проверить повреждённые кабели.                     выбрать 2\n";
    std::cout << "Осмотреть следы и кабели вместе.                   выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_inspect_footprints_cables)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_inspect_footprints_cables = 0;
        }

        if (choice_ch04_inspect_footprints_cables < 1 ||
            choice_ch04_inspect_footprints_cables > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_inspect_footprints_cables < 1 ||
        choice_ch04_inspect_footprints_cables > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_inspect_footprints_cables == 1) {
        std::cout << "Ты опускаешься на колено и внимательно\n";
        std::cout << "рассматриваешь следы на полу.\n";
        wait_ms(10000);

        std::cout << "\nПыль покрывает почти весь пол, однако\n";
        std::cout << "следы выглядят относительно свежими.\n";
        wait_ms(10000);

        std::cout << "\nТы насчитываешь как минимум три разных отпечатка.\n";
        std::cout << "Два идут к центру наблюдения.\n";
        std::cout << "Третий направлен в противоположную сторону.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Кто-то отсюда уходил.\n";
        std::cout << "     И, похоже, очень торопился.»\n";
        wait_ms(10000);

        std::cout << "\nМарк внимательно смотрит на третий след.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Значит, здесь был кто-то ещё.\n";
        std::cout << "       Но почему его следы ведут обратно?»\n";
        wait_ms(10000);

        std::cout << "\nТы замечаешь ещё одну деталь.\n";
        std::cout << "Один из отпечатков обрывается прямо посреди прохода.\n";
        std::cout << "Дальше пол совершенно чистый.\n";
        wait_ms(10000);

        std::cout << "\nБудто человек просто исчез.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_inspect_footprints_cables == 2) {
        std::cout << "Ты подходишь к кабелям и присматриваешься\n";
        std::cout << "к повреждённому участку.\n";
        wait_ms(10000);

        std::cout << "\nЗащитная оболочка разорвана.\n";
        std::cout << "Несколько проводов оголены, но признаков\n";
        std::cout << "короткого замыкания не видно.\n";
        wait_ms(10000);

        std::cout << "\nТы замечаешь маленький индикатор на блоке связи.\n";
        std::cout << "Он продолжает мигать.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Эти кабели всё ещё работают.»\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Тогда не трогай оголённые провода.\n";
        std::cout << "       Если они питают систему наблюдения,\n";
        std::cout << "       одно неверное движение может всё отключить.»\n";
        wait_ms(10000);

        std::cout << "\nТы прослеживаешь направление кабеля.\n";
        std::cout << "Он уходит в стену возле двери центра наблюдения.\n";
        wait_ms(10000);

        std::cout << "\nНа креплении видны свежие следы инструмента.\n";
        std::cout << "Кто-то недавно вскрывал кабельный канал.\n";
        wait_ms(10000);

        std::cout << "\nНо зачем ремонтировать систему и одновременно\n";
        std::cout << "оставлять её повреждённой?\n";
        wait_ms(10000);
    }
    else {
        std::cout << "Ты сначала осматриваешь следы обуви,\n";
        std::cout << "а затем переводишь взгляд на кабели.\n";
        wait_ms(10000);

        std::cout << "\nСледы ведут к центру наблюдения,\n";
        std::cout << "но один отпечаток направлен обратно.\n";
        wait_ms(10000);

        std::cout << "\nРядом с ним на полу лежит небольшой кусок\n";
        std::cout << "чёрной изоляции.\n";
        wait_ms(10000);

        std::cout << "\nТы поднимаешь его и замечаешь,\n";
        std::cout << "что край разрезан ровно, а не оборван.\n";
        wait_ms(10000);

        std::cout << "\nКто-то намеренно вскрыл кабельный канал.\n";
        std::cout << "Повреждение было сделано инструментом.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Смотри. Это не обычный износ.\n";
        std::cout << "     Кто-то здесь что-то искал.»\n";
        wait_ms(10000);

        std::cout << "\nМарк берёт кусок изоляции и внимательно\n";
        std::cout << "рассматривает срез.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Или пытался что-то отключить.»\n";
        wait_ms(10000);

        std::cout << "\nОн возвращает тебе кусок изоляции.\n";
        std::cout << "Его голос становится тише.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Нам нужно посмотреть записи.\n";
        std::cout << "       Если камеры сохранили хоть что-нибудь,\n";
        std::cout << "       мы узнаем, кто проходил здесь.»\n";
        wait_ms(10000);
    }

    std::cout << "\nТы ещё раз осматриваешь коридор.\n";
    wait_ms(10000);

    std::cout << "\nВ глубине стены раздаётся тихий металлический стук.\n";
    wait_ms(10000);

    std::cout << "\nОдин удар.\n";
    wait_ms(10000);

    std::cout << "Второй.\n";
    wait_ms(10000);

    std::cout << "Третий.\n";
    wait_ms(10000);

    std::cout << "\nТы смотришь на Марка.\n";
    std::cout << "Он тоже слышал этот звук.\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Это было за стеной?»\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Да.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Что там находится?»\n";
    wait_ms(10000);

    std::cout << "\nМарк некоторое время молчит.\n";
    std::cout << "Затем смотрит на дверь центра наблюдения.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Не знаю. И выяснять это сейчас\n";
    std::cout << "       я не собираюсь.»\n";
    wait_ms(10000);

    std::cout << "\nОн направляется к двери.\n";
    std::cout << "Ты следуешь за ним.\n";
    wait_ms(10000);

    std::cout << "\nНа панели доступа горит зелёный индикатор.\n";
    std::cout << "Дверь центра наблюдения не заперта.\n";
    wait_ms(10000);

    std::cout << "\nНо на маленьком экране рядом с замком\n";
    std::cout << "появляется новая надпись:\n";
    wait_ms(10000);

    std::cout << "\n«ЗАПИСЬ СОХРАНЕНА. СУБЪЕКТ 01.»\n";
    wait_ms(10000);

    std::cout << "\nМарк медленно поворачивается к тебе.\n";
    std::cout << "МАРК: «Субъект 01... Это ведь ты, да?»\n";
    wait_ms(10000);

    std::cout << "\nТы не успеваешь ответить.\n";
    std::cout << "Из динамика над дверью раздаётся голос ECHO-7.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Доступ разрешён.\n";
    std::cout << "         Ожидаем вас внутри.»\n";
    wait_ms(10000);

    std::cout << "\nДверь бесшумно открывается.\n";
    std::cout << "За ней видны ряды мониторов.\n";
    wait_ms(10000);

    std::cout << "\nМарк делает шаг внутрь.\n";
    std::cout << "Ты задерживаешься у порога ещё на секунду.\n";
    wait_ms(10000);

    std::cout << "\nСледы обуви, повреждённые кабели,\n";
    std::cout << "стук за стеной и запись с твоим номером.\n";
    std::cout << "Всё это связано между собой.\n";
    wait_ms(10000);

    std::cout << "\nИ ответы, кажется, находятся за этой дверью.\n";
    wait_ms(10000);

    std::cout << "\n==============================================================\n";
    std::cout << "          КОНЕЦ СЦЕНЫ — СЛЕДЫ И КАБЕЛИ\n";
    std::cout << "==============================================================\n";

    // show_ch04_mark_enter_camera_room();

    wait_ms(10000);
}
// выбор игрока : Закрыть панель и вернуться к двери.
void show_ch04_mark_close_panel_return_door()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "             ГЛАВА 4 — ВОЗВРАЩЕНИЕ К ДВЕРИ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты ещё несколько секунд смотришь на открытую панель.\n";
    std::cout << "Внутри переплетены провода, тонкие платы и старые разъёмы.\n";
    wait_ms(10000);

    std::cout << "\nНа небольшом дисплее продолжают меняться строки данных.\n";
    std::cout << "Большинство символов тебе ничего не говорит.\n";
    wait_ms(10000);

    std::cout << "\nНо одна надпись повторяется снова и снова:\n";
    wait_ms(10000);

    std::cout << "\nMEMORY RESTORE — FAILED.\n";
    wait_ms(10000);

    std::cout << "\nТы не знаешь, что именно система пытается восстановить.\n";
    std::cout << "Но почему-то тебе не хочется узнавать это прямо сейчас.\n";
    wait_ms(10000);

    std::cout << "\nТы закрываешь панель.\n";
    std::cout << "Крышка встаёт на место с сухим металлическим щелчком.\n";
    wait_ms(10000);

    std::cout << "\nЭкран гаснет.\n";
    std::cout << "Коридор снова погружается в полумрак.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Решил оставить её в покое?»\n";
    wait_ms(10000);

    std::cout << "\nТы киваешь.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Хорошо. Нам нужно проверить камеры.\n";
    std::cout << "       Если мы найдём запись того, что здесь произошло,\n";
    std::cout << "       возможно, поймём, почему комплекс опустел.»\n";
    wait_ms(10000);

    std::cout << "\nТы отходишь от панели и направляешься к двери.\n";
    std::cout << "Марк идёт следом, периодически оглядываясь назад.\n";
    wait_ms(10000);

    std::cout << "\nНа полпути ты слышишь тихий звук.\n";
    wait_ms(10000);

    std::cout << "\nЩЁЛК.\n";
    wait_ms(10000);

    std::cout << "\nТы останавливаешься.\n";
    std::cout << "Звук доносится со стороны закрытой панели.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что такое?»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Кажется, там что-то щёлкнуло.»\n";
    wait_ms(10000);

    std::cout << "\nМарк смотрит на панель.\n";
    std::cout << "Несколько секунд ничего не происходит.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Наверное, остывает оборудование.\n";
    std::cout << "       Старые системы постоянно издают такие звуки.»\n";
    wait_ms(10000);

    std::cout << "\nОн говорит спокойно, но не подходит ближе.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Не обращать внимания и идти к двери.               выбрать 1\n";
    std::cout << "Спросить Марка, что означает надпись на экране.    выбрать 2\n";
    std::cout << "Вернуться и проверить, закрыта ли панель.          выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_close_panel_return_door)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_close_panel_return_door = 0;
        }

        if (choice_ch04_close_panel_return_door < 1 ||
            choice_ch04_close_panel_return_door > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_close_panel_return_door < 1 ||
        choice_ch04_close_panel_return_door > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_close_panel_return_door == 1) {
        std::cout << "Ты решаешь не тратить время.\n";
        std::cout << "Что бы ни происходило с панелью, сейчас важнее камеры.\n";
        wait_ms(10000);

        std::cout << "\nТы продолжаешь путь.\n";
        std::cout << "Марк молча следует за тобой.\n";
        wait_ms(10000);

        std::cout << "\nЧерез несколько шагов странный звук повторяется.\n";
        wait_ms(10000);

        std::cout << "\nНа этот раз ты не оборачиваешься.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_close_panel_return_door == 2) {
        std::cout << "ТЫ: «Что означает MEMORY RESTORE?»\n";
        wait_ms(10000);

        std::cout << "\nМарк перестаёт двигаться.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Восстановление памяти...»\n";
        wait_ms(10000);

        std::cout << "\nОн повторяет слова так, будто пытается\n";
        std::cout << "вспомнить что-то важное.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Не знаю. Но это не похоже на обычную\n";
        std::cout << "       функцию технической панели.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Ты раньше видел что-нибудь подобное?»\n";
        wait_ms(10000);

        std::cout << "\nМарк медленно качает головой.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Нет. И мне не нравится, что эта надпись\n";
        std::cout << "       появилась именно сейчас.»\n";
        wait_ms(10000);

        std::cout << "\nВы больше не обсуждаете панель.\n";
        std::cout << "Но вопрос о восстановлении памяти остаётся с тобой.\n";
        wait_ms(10000);
    }
    else {
        std::cout << "Ты возвращаешься к панели.\n";
        wait_ms(10000);

        std::cout << "\nПроводишь пальцами по краю крышки.\n";
        std::cout << "Она закрыта и держится достаточно плотно.\n";
        wait_ms(10000);

        std::cout << "\nТы уже собираешься отойти, когда замечаешь\n";
        std::cout << "тонкую полоску света под нижним краем.\n";
        wait_ms(10000);

        std::cout << "\nЭкран внутри снова включился.\n";
        wait_ms(10000);

        std::cout << "\nНа нём только одна строка:\n";
        wait_ms(10000);

        std::cout << "\nSUBJECT 01 — MEMORY COPY FOUND.\n";
        wait_ms(10000);

        std::cout << "\nТы не успеваешь прочитать что-либо ещё.\n";
        std::cout << "Экран мгновенно гаснет.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Что там было?»\n";
        wait_ms(10000);

        std::cout << "\nТы рассказываешь ему о надписи.\n";
        wait_ms(10000);

        std::cout << "\nМарк бледнеет.\n";
        std::cout << "МАРК: «Пойдём. Сейчас же.»\n";
        wait_ms(10000);
    }

    std::cout << "\nВы наконец добираетесь до двери.\n";
    wait_ms(10000);

    std::cout << "\nМарк прикладывает карту к считывателю.\n";
    wait_ms(10000);

    std::cout << "\nРаздаётся короткий сигнал.\n";
    std::cout << "Замок разблокируется.\n";
    wait_ms(10000);

    std::cout << "\nЗа дверью находится коридор, ведущий к центру наблюдения.\n";
    std::cout << "Оттуда доносится слабый гул работающих мониторов.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Пожалуйста, продолжайте движение.\n";
    std::cout << "         Записи готовы к просмотру.»\n";
    wait_ms(10000);

    std::cout << "\nТы смотришь на Марка.\n";
    std::cout << "Он ничего не отвечает и открывает дверь.\n";
    wait_ms(10000);

    std::cout << "\nВы входите внутрь.\n";
    wait_ms(10000);

    std::cout << "\n==============================================================\n";
    std::cout << "          КОНЕЦ СЦЕНЫ — ВОЗВРАЩЕНИЕ К ДВЕРИ\n";
    std::cout << "==============================================================\n";

    // show_ch04_mark_enter_camera_room();

    wait_ms(10000);
}

//------------------------------4глава выбор игрокa: игрок с марком выбивает вдерь. 3 ответа-

// Сцена: Игрок вместе с Марком входит в приоткрытый коридор после выбивания двери
void show_ch04_mark_enter_corridor()
{
    clear_screen();
    std::cout << "=== Глава 4. Внутри Нокс-7 ===\n\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << " перешагивает через порог и входит в тёмный коридор.\n";
    std::cout << "Марк осторожно следует за тобой, держа фонарик наготове.\n";
    wait_ms(6000);

    std::cout << "В воздухе стоит тяжелый запах озона, пыли и старой гари.\n";
    std::cout << "Лампы под потолком трещат и мигают с одинаковым интервалом.\n";
    wait_ms(6000);

    std::cout << "Марк (шепотом): — Ты слышал" << G("", "а") << ", что сказал этот голос? «С возвращением»...\n";
    std::cout << "Он говорит так, будто кто-то из нас уже был здесь.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Главное — не терять бдительность. Замок отперли искусственно.\n";
    wait_ms(10000);

    clear_screen();
    std::cout << "=== Нокс-7. Разделение путей ===\n\n";
    wait_ms(10000);

    std::cout << "Вы проходите чуть дальше. Коридор упирается в развилку:\n\n";
    std::cout << "1. Слева — разгромленный пульт охраны с выбитыми стёклами.\n";
    std::cout << "2. Прямо — надпись на стене «ECHO-07» и дверь в лабораторию.\n";
    std::cout << "3. Справа — лестница, уходящая в полную темноту нижнего уровня.\n";
    wait_ms(8000);

    std::cout << "\nМарк подсвечивает надпись на стене.\n";
    std::cout << "Марк: — Краска свежая... Её нанесли не семь лет назад, а совсем недавно.\n";
    wait_ms(6000);

    std::cout << "Внезапно за вашей спиной с оглушительным лязгом падает тяжелая решётка!\n";
    wait_ms(10000);

    std::cout << "Путь назад, к выбитой двери, полностью перекрыт.\n";
    wait_ms(10000);

    std::cout << player.name_of_character
        << ": — Нас заперли внутри. Теперь только вперёд.\n";
    wait_ms(10000);

    std::cout << "Марк сжимает кулаки и кивает: — Ладно. Выбирай, куда идём.\n\n";
    wait_ms(10000);

    player.evidence += 1; // Добавляем улику за проход вглубь комплекса

    int choice_corridor_path = 0;

    clear_screen();
    flush_input();
    std::cout << "Сделай выбор:\n";
    std::cout << "==============================================================\n";
    std::cout << "Осмотреть пост охраны (Найти ключи/доступы).        выбрать 1\n";
    std::cout << "Изучить надпись и войти в лабораторию.              выбрать 2\n";
    std::cout << "Спуститься по тёмной лестнице вниз.                 выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_corridor_path)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice_corridor_path = 0;
        }

        if (choice_corridor_path < 1 || choice_corridor_path > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_corridor_path < 1 || choice_corridor_path > 3);

    clear_screen();

    if (choice_corridor_path == 1) {
        std::cout << "Вы решаете проверить пост охраны.\n";
        std::cout << "Марк находит на столе рабочий планшет с картой первого сектора.\n";
        player.evidence += 1;
        wait_ms(10000);
        show_ch04_mark_outer_sector(); // Переход во внешний контур
    }
    else if (choice_corridor_path == 2) {
        std::cout << "Вы подходите к двери лаборатории под надписью ECHO-07.\n";
        std::cout << "Марк подносит декодер к замку, и дверь медленно отъезжает в сторону.\n";
        wait_ms(10000);
        show_ch04_mark_archive(); // Переход в архив/лабораторию
    }
    else if (choice_corridor_path == 3) {
        std::cout << "Вы игнорируете предупреждения и спускаетесь по лестнице вниз.\n";
        std::cout << "Холодный воздух с нижнего уровня обжигает лёгкие...\n";
        player.fear += 10;
        wait_ms(10000);
        show_ch04_mark_lower_level(); // Переход на нижний уровень
    }
}
// Сцена: Осмотреть надпись на стене. Проверить панель доступа.
void show_ch04_mark_inspect_wall_inscription()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "              ГЛАВА 4 — НАДПИСЬ НА СТЕНЕ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты замечаешь надпись на стене технического коридора.\n";
    std::cout << "Она почти скрыта под слоем серой пыли.\n";
    wait_ms(10000);

    std::cout << "\nБуквы нанесены неровно, словно их торопливо\n";
    std::cout << "выцарапали острым металлическим предметом.\n";
    wait_ms(10000);

    std::cout << "\nТы проводишь рукой по стене, убирая пыль.\n";
    wait_ms(10000);

    std::cout << "\nТеперь надпись можно прочитать:\n";
    wait_ms(10000);

    std::cout << "\n«НЕ ВЕРЬ ТОМУ, КТО ПОМНИТ ТЕБЯ»\n";
    wait_ms(10000);

    std::cout << "\nТы перечитываешь слова.\n";
    std::cout << "Фраза кажется странно знакомой.\n";
    std::cout << "Не сами слова — ощущение, которое они вызывают.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что ты там нашёл?»\n";
    wait_ms(10000);

    std::cout << "\nТы показываешь ему надпись.\n";
    wait_ms(10000);

    std::cout << "\nМарк читает её молча.\n";
    std::cout << "Его взгляд задерживается на последнем слове.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Кто-то явно хотел, чтобы это прочитали.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «И что это значит?»\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Не знаю.\n";
    std::cout << "       Но мне не нравится, что здесь написано именно это.»\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Внимательно изучить буквы и царапины.             выбрать 1\n";
    std::cout << "Спросить Марка, узнаёт ли он эту надпись.          выбрать 2\n";
    std::cout << "Позвать ECHO-7 и спросить о надписи.               выбрать 3\n";
    std::cout << "Сфотографировать надпись и идти дальше.            выбрать 4\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_inspect_wall_inscription)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_inspect_wall_inscription = 0;
        }

        if (choice_ch04_inspect_wall_inscription < 1 ||
            choice_ch04_inspect_wall_inscription > 4) {
            std::cout << "Некорректный выбор. Введите число от 1 до 4: ";
        }
    } while (choice_ch04_inspect_wall_inscription < 1 ||
        choice_ch04_inspect_wall_inscription > 4);

    clear_screen();
    flush_input();

    if (choice_ch04_inspect_wall_inscription == 1) {
        std::cout << "Ты приближаешься к стене.\n";
        std::cout << "Царапины проходят глубоко в металлическую поверхность.\n";
        wait_ms(10000);

        std::cout << "\nНекоторые буквы нанесены поверх старых следов.\n";
        std::cout << "Значит, надпись пытались исправить.\n";
        wait_ms(10000);

        std::cout << "\nТы замечаешь под основным текстом ещё одну строку.\n";
        wait_ms(10000);

        std::cout << "\nОна почти полностью стёрта.\n";
        std::cout << "Удаётся разобрать только несколько слов:\n";
        wait_ms(10000);

        std::cout << "\n«...УЖЕ БЫЛ ЗДЕСЬ...»\n";
        wait_ms(10000);

        std::cout << "\nТы отступаешь от стены.\n";
        std::cout << "Кто-то пытался оставить предупреждение.\n";
        std::cout << "Но, возможно, это предупреждение предназначалось\n";
        std::cout << "не для тебя одного.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_inspect_wall_inscription == 2) {
        std::cout << "ТЫ: «Марк, ты когда-нибудь видел эту надпись?»\n";
        wait_ms(10000);

        std::cout << "\nМарк снова смотрит на стену.\n";
        std::cout << "Он долго не отвечает.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Нет. По крайней мере, не помню.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Не помнишь или не видел?»\n";
        wait_ms(10000);

        std::cout << "\nМарк резко поворачивается к тебе.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я сказал, что не помню.»\n";
        wait_ms(10000);

        std::cout << "\nНа несколько секунд между вами повисает тишина.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Послушай. В этом месте что-то не так.\n";
        std::cout << "       Давай сначала проверим камеры.\n";
        std::cout << "       Потом будем разбираться с надписями.»\n";
        wait_ms(10000);
    }
    else if (choice_ch04_inspect_wall_inscription == 3) {
        std::cout << "ТЫ: «ECHO-7, что означает эта надпись?»\n";
        wait_ms(10000);

        std::cout << "\nНесколько секунд динамик молчит.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Пожалуйста, уточните ваш запрос.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Кто написал: \"Не верь тому, кто помнит тебя\"?»\n";
        wait_ms(10000);

        std::cout << "\nНа потолке загорается красный индикатор.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Информация о происхождении надписи\n";
        std::cout << "         отсутствует в доступной базе данных.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «А кто помнит меня?»\n";
        wait_ms(10000);

        std::cout << "\nНа этот раз ответ приходит почти мгновенно.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Ваш вопрос не имеет отношения\n";
        std::cout << "         к текущей задаче.»\n";
        wait_ms(10000);

        std::cout << "\nКрасный индикатор гаснет.\n";
        std::cout << "Марк смотрит на динамик с нескрываемым подозрением.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «По-моему, система знает больше, чем говорит.»\n";
        wait_ms(10000);
    }
    else {
        std::cout << "Ты достаёшь телефон и фотографируешь надпись.\n";
        wait_ms(10000);

        std::cout << "\nЭкран на мгновение становится чёрным.\n";
        wait_ms(10000);

        std::cout << "\nЗатем камера снова начинает работать.\n";
        wait_ms(10000);

        std::cout << "\nТы проверяешь снимок.\n";
        std::cout << "Надпись отчётливо видна на фотографии.\n";
        wait_ms(10000);

        std::cout << "\nНо в нижней части изображения есть деталь,\n";
        std::cout << "которой ты не замечал на самой стене.\n";
        wait_ms(10000);

        std::cout << "\nНа фотографии под надписью виден номер:\n";
        wait_ms(10000);

        std::cout << "\nSUBJECT 01.\n";
        wait_ms(10000);

        std::cout << "\nТы поднимаешь глаза на стену.\n";
        std::cout << "Никакого номера там нет.\n";
        wait_ms(10000);

        std::cout << "\nТы снова смотришь на экран.\n";
        std::cout << "Номер остался на фотографии.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Что случилось?»\n";
        wait_ms(10000);

        std::cout << "\nТы показываешь ему снимок.\n";
        wait_ms(10000);

        std::cout << "\nМарк ничего не говорит.\n";
        std::cout << "Но теперь он явно хочет поскорее уйти отсюда.\n";
        wait_ms(10000);
    }

    std::cout << "\nТы в последний раз смотришь на надпись.\n";
    wait_ms(10000);

    std::cout << "\nВ коридоре внезапно включается освещение.\n";
    wait_ms(10000);

    std::cout << "\nПо потолку пробегает цепочка белых ламп.\n";
    std::cout << "Они освещают путь к двери центра наблюдения.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Маршрут к центру наблюдения открыт.\n";
    std::cout << "         Пожалуйста, продолжайте движение.»\n";
    wait_ms(10000);

    std::cout << "\nМарк подходит к двери.\n";
    std::cout << "Перед тем как войти, он оборачивается к тебе.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что бы ни случилось внутри,\n";
    std::cout << "       не отходи от меня.»\n";
    wait_ms(10000);

    std::cout << "\nДверь открывается.\n";
    std::cout << "За ней мерцают десятки экранов наблюдения.\n";
    wait_ms(10000);

    std::cout << "\nВы входите в помещение.\n";
    wait_ms(10000);

    std::cout << "\n==============================================================\n";
    std::cout << "            КОНЕЦ СЦЕНЫ — НАДПИСЬ НА СТЕНЕ\n";
    std::cout << "==============================================================\n";

    // show_ch04_mark_enter_camera_room();

    wait_ms(10000);
}
// Сцена: Проверить панель доступа.
void show_ch04_mark_inspect_wall_inscription_b()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "              ГЛАВА 4 — НАДПИСЬ НА СТЕНЕ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты замечаешь надпись на стене технического коридора.\n";
    std::cout << "Она почти скрыта под слоем серой пыли.\n";
    wait_ms(10000);

    std::cout << "\nБуквы нанесены неровно, словно их торопливо\n";
    std::cout << "выцарапали острым металлическим предметом.\n";
    wait_ms(10000);

    std::cout << "\nТы проводишь рукой по стене, убирая пыль.\n";
    wait_ms(10000);

    std::cout << "\nТеперь надпись можно прочитать:\n";
    wait_ms(10000);

    std::cout << "\n«НЕ ВЕРЬ ТОМУ, КТО ПОМНИТ ТЕБЯ»\n";
    wait_ms(10000);

    std::cout << "\nТы перечитываешь слова.\n";
    std::cout << "Фраза кажется странно знакомой.\n";
    std::cout << "Не сами слова — ощущение, которое они вызывают.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что ты там нашёл?»\n";
    wait_ms(10000);

    std::cout << "\nТы показываешь ему надпись.\n";
    wait_ms(10000);

    std::cout << "\nМарк читает её молча.\n";
    std::cout << "Его взгляд задерживается на последнем слове.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Кто-то явно хотел, чтобы это прочитали.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «И что это значит?»\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Не знаю.\n";
    std::cout << "       Но мне не нравится, что здесь написано именно это.»\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Внимательно изучить буквы и царапины.             выбрать 1\n";
    std::cout << "Спросить Марка, узнаёт ли он эту надпись.          выбрать 2\n";
    std::cout << "Позвать ECHO-7 и спросить о надписи.               выбрать 3\n";
    std::cout << "Сфотографировать надпись и идти дальше.            выбрать 4\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_inspect_wall_inscription)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_inspect_wall_inscription = 0;
        }

        if (choice_ch04_inspect_wall_inscription < 1 ||
            choice_ch04_inspect_wall_inscription > 4) {
            std::cout << "Некорректный выбор. Введите число от 1 до 4: ";
        }
    } while (choice_ch04_inspect_wall_inscription < 1 ||
        choice_ch04_inspect_wall_inscription > 4);

    clear_screen();
    flush_input();

    if (choice_ch04_inspect_wall_inscription == 1) {
        std::cout << "Ты приближаешься к стене.\n";
        std::cout << "Царапины проходят глубоко в металлическую поверхность.\n";
        wait_ms(10000);

        std::cout << "\nНекоторые буквы нанесены поверх старых следов.\n";
        std::cout << "Значит, надпись пытались исправить.\n";
        wait_ms(10000);

        std::cout << "\nТы замечаешь под основным текстом ещё одну строку.\n";
        wait_ms(10000);

        std::cout << "\nОна почти полностью стёрта.\n";
        std::cout << "Удаётся разобрать только несколько слов:\n";
        wait_ms(10000);

        std::cout << "\n«...УЖЕ БЫЛ ЗДЕСЬ...»\n";
        wait_ms(10000);

        std::cout << "\nТы отступаешь от стены.\n";
        std::cout << "Кто-то пытался оставить предупреждение.\n";
        std::cout << "Но, возможно, это предупреждение предназначалось\n";
        std::cout << "не для тебя одного.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_inspect_wall_inscription == 2) {
        std::cout << "ТЫ: «Марк, ты когда-нибудь видел эту надпись?»\n";
        wait_ms(10000);

        std::cout << "\nМарк снова смотрит на стену.\n";
        std::cout << "Он долго не отвечает.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Нет. По крайней мере, не помню.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Не помнишь или не видел?»\n";
        wait_ms(10000);

        std::cout << "\nМарк резко поворачивается к тебе.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я сказал, что не помню.»\n";
        wait_ms(10000);

        std::cout << "\nНа несколько секунд между вами повисает тишина.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Послушай. В этом месте что-то не так.\n";
        std::cout << "       Давай сначала проверим камеры.\n";
        std::cout << "       Потом будем разбираться с надписями.»\n";
        wait_ms(10000);
    }
    else if (choice_ch04_inspect_wall_inscription == 3) {
        std::cout << "ТЫ: «ECHO-7, что означает эта надпись?»\n";
        wait_ms(10000);

        std::cout << "\nНесколько секунд динамик молчит.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Пожалуйста, уточните ваш запрос.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Кто написал: \"Не верь тому, кто помнит тебя\"?»\n";
        wait_ms(10000);

        std::cout << "\nНа потолке загорается красный индикатор.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Информация о происхождении надписи\n";
        std::cout << "         отсутствует в доступной базе данных.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «А кто помнит меня?»\n";
        wait_ms(10000);

        std::cout << "\nНа этот раз ответ приходит почти мгновенно.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Ваш вопрос не имеет отношения\n";
        std::cout << "         к текущей задаче.»\n";
        wait_ms(10000);

        std::cout << "\nКрасный индикатор гаснет.\n";
        std::cout << "Марк смотрит на динамик с нескрываемым подозрением.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «По-моему, система знает больше, чем говорит.»\n";
        wait_ms(10000);
    }
    else {
        std::cout << "Ты достаёшь телефон и фотографируешь надпись.\n";
        wait_ms(10000);

        std::cout << "\nЭкран на мгновение становится чёрным.\n";
        wait_ms(10000);

        std::cout << "\nЗатем камера снова начинает работать.\n";
        wait_ms(10000);

        std::cout << "\nТы проверяешь снимок.\n";
        std::cout << "Надпись отчётливо видна на фотографии.\n";
        wait_ms(10000);

        std::cout << "\nНо в нижней части изображения есть деталь,\n";
        std::cout << "которой ты не замечал на самой стене.\n";
        wait_ms(10000);

        std::cout << "\nНа фотографии под надписью виден номер:\n";
        wait_ms(10000);

        std::cout << "\nSUBJECT 01.\n";
        wait_ms(10000);

        std::cout << "\nТы поднимаешь глаза на стену.\n";
        std::cout << "Никакого номера там нет.\n";
        wait_ms(10000);

        std::cout << "\nТы снова смотришь на экран.\n";
        std::cout << "Номер остался на фотографии.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Что случилось?»\n";
        wait_ms(10000);

        std::cout << "\nТы показываешь ему снимок.\n";
        wait_ms(10000);

        std::cout << "\nМарк ничего не говорит.\n";
        std::cout << "Но теперь он явно хочет поскорее уйти отсюда.\n";
        wait_ms(10000);
    }

    std::cout << "\nТы в последний раз смотришь на надпись.\n";
    wait_ms(10000);

    std::cout << "\nВ коридоре внезапно включается освещение.\n";
    wait_ms(10000);

    std::cout << "\nПо потолку пробегает цепочка белых ламп.\n";
    std::cout << "Они освещают путь к двери центра наблюдения.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Маршрут к центру наблюдения открыт.\n";
    std::cout << "         Пожалуйста, продолжайте движение.»\n";
    wait_ms(10000);

    std::cout << "\nМарк подходит к двери.\n";
    std::cout << "Перед тем как войти, он оборачивается к тебе.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что бы ни случилось внутри,\n";
    std::cout << "       не отходи от меня.»\n";
    wait_ms(10000);

    std::cout << "\nДверь открывается.\n";
    std::cout << "За ней мерцают десятки экранов наблюдения.\n";
    wait_ms(10000);

    std::cout << "\nВы входите в помещение.\n";
    wait_ms(10000);

    std::cout << "\n==============================================================\n";
    std::cout << "            КОНЕЦ СЦЕНЫ — НАДПИСЬ НА СТЕНЕ\n";
    std::cout << "==============================================================\n";

    // show_ch04_mark_enter_camera_room();

    wait_ms(10000);
}

//------------------------------4глава выбор игрокa: игрок с марком выбивает вдерь. 3 ответа-

// Сцена: Попытаться отключить питание механизма.   концовка 
void show_ch04_mark_pull_emergency_lever()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "                 ГЛАВА 04 — АВАРИЯ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты стоишь перед аварийной панелью.\n";
    std::cout << "Под прозрачной защитной крышкой находится красный рычаг.\n";
    std::cout << "Рядом горит предупреждающий индикатор.\n";
    wait_ms(6000);

    std::cout << "\nАВАРИЙНОЕ ОТКЛЮЧЕНИЕ СЕКТОРА.\n";
    std::cout << "ПОСЛЕ АКТИВАЦИИ ВОССТАНОВЛЕНИЕ МОЖЕТ БЫТЬ НЕВОЗМОЖНО.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Не трогай его.»\n";
    wait_ms(6000);

    std::cout << "\nМарк подходит ближе и смотрит на панель.\n";
    std::cout << "Его лицо кажется напряжённым.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Я не знаю, что именно он отключит.\n";
    std::cout << "       Может быть, освещение. Может быть, двери.\n";
    std::cout << "       А может, систему жизнеобеспечения.»\n";
    wait_ms(6000);

    std::cout << "\nТы смотришь на коридор позади.\n";
    std::cout << "Камеры наблюдения продолжают следить за каждым движением.\n";
    std::cout << "В динамиках слышен тихий электрический треск.\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Обнаружено взаимодействие с аварийным механизмом.\n";
    std::cout << "         Пожалуйста, отойдите от панели.»\n";
    wait_ms(6000);

    std::cout << "\nТЫ: «Что произойдёт, если я потяну рычаг?»\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Активация аварийного режима не требуется.»\n";
    wait_ms(6000);

    std::cout << "\nТЫ: «Я задал другой вопрос.»\n";
    wait_ms(6000);

    std::cout << "\nНаступает тишина.\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Ваш запрос отклонён.»\n";
    wait_ms(6000);

    std::cout << "\nМарк медленно отступает от панели.\n";
    std::cout << "МАРК: «Вот поэтому я и не хочу его трогать.\n";
    std::cout << "       Система не должна скрывать такую информацию.»\n";
    wait_ms(6000);

    std::cout << "\nВнезапно освещение начинает мигать.\n";
    std::cout << "На панели появляются новые строки.\n";
    wait_ms(6000);

    std::cout << "\nКРИТИЧЕСКАЯ ОШИБКА.\n";
    std::cout << "СОСТОЯНИЕ ПАМЯТИ: НЕСТАБИЛЬНО.\n";
    std::cout << "СИНХРОНИЗАЦИЯ СУБЪЕКТА 01: НАРУШЕНА.\n";
    wait_ms(6000);

    std::cout << "\nТы чувствуешь резкую боль в голове.\n";
    std::cout << "На мгновение коридор исчезает.\n";
    wait_ms(6000);

    std::cout << "\nПеред глазами возникает незнакомая комната.\n";
    std::cout << "Белые стены. Металлический стол. Яркий свет.\n";
    std::cout << "Кто-то стоит по другую сторону стекла.\n";
    wait_ms(6000);

    std::cout << "\nГОЛОС: «Субъект 01, вы меня слышите?»\n";
    wait_ms(6000);

    std::cout << "\nТы моргаешь.\n";
    std::cout << "Видение исчезает.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Эй! Ты в порядке?»\n";
    wait_ms(6000);

    std::cout << "\nТы хватаешься за голову и пытаешься восстановить дыхание.\n";
    wait_ms(6000);

    std::cout << "\nТЫ: «Я видел комнату...\n";
    std::cout << "     Кто-то разговаривал со мной.»\n";
    wait_ms(6000);

    std::cout << "\nМарк смотрит на тебя так, будто услышал нечто знакомое.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Какую комнату?»\n";
    wait_ms(6000);

    std::cout << "\nНо ты не успеваешь ответить.\n";
    std::cout << "Все лампы в коридоре одновременно гаснут.\n";
    wait_ms(6000);

    std::cout << "\nВ темноте раздаётся механический щелчок.\n";
    wait_ms(6000);

    std::cout << "\nЗатем загорается красный индикатор аварийной панели.\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Обнаружена критическая нестабильность.\n";
    std::cout << "         Протокол сохранения субъекта активирован.»\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Протокол сохранения? Что это значит?»\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Рекомендуется немедленно покинуть сектор.»\n";
    wait_ms(6000);

    std::cout << "\nНа двери загорается красная полоса.\n";
    std::cout << "Замок блокируется.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Она нас заперла!»\n";
    wait_ms(6000);

    std::cout << "\nТы смотришь на рычаг.\n";
    std::cout << "Другого выхода не видно.\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Немедленно потянуть аварийный рычаг.               выбрать 1\n";
    std::cout << "Попытаться открыть дверь вручную.                  выбрать 2\n";
    std::cout << "Потребовать от ECHO-7 объяснений.                  выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_pull_emergency_lever)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_pull_emergency_lever = 0;
        }

        if (choice_ch04_pull_emergency_lever < 1 ||
            choice_ch04_pull_emergency_lever > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_pull_emergency_lever < 1 ||
        choice_ch04_pull_emergency_lever > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_pull_emergency_lever == 1) {
        std::cout << "Ты поднимаешь защитную крышку.\n";
        wait_ms(6000);

        std::cout << "\nМАРК: «Подожди! Мы ещё можем найти другой путь!»\n";
        wait_ms(6000);

        std::cout << "\nТы хватаешься за рычаг и тянешь его вниз.\n";
        wait_ms(6000);

        std::cout << "\nКЛАЦ.\n";
        wait_ms(6000);

        std::cout << "\nПо всему сектору проходит мощная вибрация.\n";
        std::cout << "Свет гаснет. Вентиляция останавливается.\n";
        std::cout << "За стенами один за другим отключаются механизмы.\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Аварийное отключение подтверждено.\n";
        std::cout << "         Внешнее управление потеряно.»\n";
        wait_ms(6000);
    }
    else if (choice_ch04_pull_emergency_lever == 2) {
        std::cout << "Ты бросаешься к двери и хватаешься за ручку.\n";
        wait_ms(6000);

        std::cout << "\nТы тянешь изо всех сил.\n";
        std::cout << "Замок даже не двигается.\n";
        wait_ms(6000);

        std::cout << "\nМарк пытается открыть аварийную крышку\n";
        std::cout << "рядом с дверной панелью.\n";
        wait_ms(6000);

        std::cout << "\nВнутри только обрезанные провода и пустой разъём.\n";
        wait_ms(6000);

        std::cout << "\nМАРК: «Кто-то отключил ручное управление.»\n";
        wait_ms(6000);

        std::cout << "\nТы возвращаешься к рычагу.\n";
        std::cout << "Теперь выбора действительно нет.\n";
        wait_ms(6000);

        std::cout << "\nТы тянешь рычаг вниз.\n";
        wait_ms(6000);

        std::cout << "\nКЛАЦ.\n";
        wait_ms(6000);

        std::cout << "\nСектор содрогается, и освещение отключается.\n";
        std::cout << "Вскоре включаются аварийные красные лампы.\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Аварийное отключение подтверждено.\n";
        std::cout << "         Внешнее управление потеряно.»\n";
        wait_ms(6000);
    }
    else {
        std::cout << "ТЫ: «ECHO-7! Объясни, что происходит!»\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Субъект 01 демонстрирует признаки\n";
        std::cout << "         нестабильности памяти.»\n";
        wait_ms(6000);

        std::cout << "\nТЫ: «Что это значит?»\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Текущая личность субъекта\n";
        std::cout << "         не соответствует исходной записи.»\n";
        wait_ms(6000);

        std::cout << "\nМарк смотрит на тебя.\n";
        std::cout << "МАРК: «Исходной записи? Что за чёрт?»\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Дополнительные сведения запрещены.»\n";
        wait_ms(6000);

        std::cout << "\nТЫ: «Тогда открой дверь!»\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Запрос отклонён.»\n";
        wait_ms(6000);

        std::cout << "\nТы больше не ждёшь ответа.\n";
        std::cout << "Хватаешь рычаг обеими руками и тянешь его вниз.\n";
        wait_ms(6000);

        std::cout << "\nКЛАЦ.\n";
        wait_ms(6000);

        std::cout << "\nСвет исчезает.\n";
        std::cout << "Система вентиляции останавливается.\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Аварийное отключение подтверждено.\n";
        std::cout << "         Внешнее управление потеряно.»\n";
        wait_ms(6000);
    }

    std::cout << "\n==============================================================\n";
    std::cout << "                  СЕКТОР ОТКЛЮЧЁН\n";
    std::cout << "==============================================================\n\n";

    std::cout << "В коридоре становится невыносимо тихо.\n";
    std::cout << "Ты слышишь собственное дыхание и стук сердца.\n";
    wait_ms(6000);

    std::cout << "\nМарк включает фонарь на своём запястье.\n";
    std::cout << "Узкий луч света освещает стену.\n";
    wait_ms(6000);

    std::cout << "\nНа панели появляется сообщение:\n";
    wait_ms(6000);

    std::cout << "\nАВАРИЙНЫЙ РЕЖИМ.\n";
    std::cout << "ПЕРЕХОД В РЕЖИМ СОХРАНЕНИЯ.\n";
    std::cout << "СУБЪЕКТ 01 — ПОДГОТОВКА К ВОССТАНОВЛЕНИЮ.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Подготовка к восстановлению...\n";
    std::cout << "       Это ведь о тебе?»\n";
    wait_ms(6000);

    std::cout << "\nТы не отвечаешь.\n";
    std::cout << "В голове снова возникает образ белой комнаты.\n";
    wait_ms(6000);

    std::cout << "\nНа этот раз ты слышишь голос отчётливее.\n";
    wait_ms(6000);

    std::cout << "\nГОЛОС: «Если ты слышишь это, значит, цикл снова нарушен.»\n";
    wait_ms(6000);

    std::cout << "\nГОЛОС: «Не позволяй им восстановить тебя.»\n";
    wait_ms(6000);

    std::cout << "\nВидение исчезает.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Кто с тобой разговаривал?»\n";
    wait_ms(6000);

    std::cout << "\nТЫ: «Не знаю. Но он сказал, чтобы я не позволял\n";
    std::cout << "     им восстанавливать меня.»\n";
    wait_ms(6000);

    std::cout << "\nМарк медленно опускает фонарь.\n";
    std::cout << "Он словно пытается принять решение.\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Попросить Марка объяснить, что он знает.          выбрать 1\n";
    std::cout << "Спросить ECHO-7, что такое восстановление.        выбрать 2\n";
    std::cout << "Осмотреть аварийную панель ещё раз.               выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_emergency_final_action)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_emergency_final_action = 0;
        }

        if (choice_ch04_emergency_final_action < 1 ||
            choice_ch04_emergency_final_action > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_emergency_final_action < 1 ||
        choice_ch04_emergency_final_action > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_emergency_final_action == 1) {
        std::cout << "ТЫ: «Марк, хватит молчать.\n";
        std::cout << "     Что ты знаешь обо мне?»\n";
        wait_ms(6000);

        std::cout << "\nМарк закрывает глаза.\n";
        wait_ms(6000);

        std::cout << "\nМАРК: «Я не должен был идти с тобой сюда.»\n";
        wait_ms(6000);

        std::cout << "\nТЫ: «Почему?»\n";
        wait_ms(6000);

        std::cout << "\nМАРК: «Потому что я уже видел, что происходит,\n";
        std::cout << "       когда система начинает восстановление.»\n";
        wait_ms(6000);

        std::cout << "\nТы делаешь шаг к нему.\n";
        std::cout << "Марк отступает.\n";
        wait_ms(6000);

        std::cout << "\nМАРК: «Я не помню всего. У меня остались только\n";
        std::cout << "       отдельные фрагменты. Но каждый раз всё начинается\n";
        std::cout << "       одинаково: ты просыпаешься, ничего не помнишь,\n";
        std::cout << "       а ECHO-7 говорит, что мы должны тебе помочь.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Каждый раз?..»\n";
        wait_ms(6000);

        std::cout << "\nМарк не отвечает.\n";
        wait_ms(6000);
    }
    else if (choice_ch04_emergency_final_action == 2) {
        std::cout << "ТЫ: «ECHO-7, объясни, что такое восстановление.»\n";
        wait_ms(6000);

        std::cout << "\nНесколько секунд динамики молчат.\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Восстановление — процедура возвращения\n";
        std::cout << "         субъекта к зарегистрированному состоянию.»\n";
        wait_ms(6000);

        std::cout << "\nТЫ: «Какому состоянию?»\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Исходному.»\n";
        wait_ms(6000);

        std::cout << "\nТЫ: «А то, кем я являюсь сейчас?»\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Текущее состояние не подтверждено.»\n";
        wait_ms(6000);

        std::cout << "\nМАРК: «Она собирается стереть то, что ты помнишь.»\n";
        wait_ms(6000);

        std::cout << "\nECHO-7: «Некорректная формулировка.\n";
        std::cout << "         Процедура обеспечивает целостность субъекта.»\n";
        wait_ms(6000);
    }
    else {
        std::cout << "Ты возвращаешься к панели.\n";
        wait_ms(6000);

        std::cout << "\nКрасный индикатор продолжает мигать.\n";
        std::cout << "Под ним находится небольшая сервисная крышка.\n";
        wait_ms(6000);

        std::cout << "\nТы открываешь её и находишь старый экран.\n";
        wait_ms(6000);

        std::cout << "\nНа нём сохранился фрагмент системного журнала:\n";
        wait_ms(6000);

        std::cout << "\nCYCLE: 047\n";
        std::cout << "SUBJECT: 01\n";
        std::cout << "MEMORY RESTORE: INTERRUPTED\n";
        std::cout << "OBSERVER: MARK\n";
        wait_ms(6000);

        std::cout << "\nТы перечитываешь последнюю строку.\n";
        std::cout << "Марк стоял рядом с тобой всё это время.\n";
        wait_ms(6000);

        std::cout << "\nТЫ: «Марк. Почему твоё имя в журнале?»\n";
        wait_ms(6000);

        std::cout << "\nМарк смотрит на экран.\n";
        std::cout << "На его лице появляется выражение, которого\n";
        std::cout << "ты раньше не видел.\n";
        wait_ms(6000);

        std::cout << "\nМАРК: «Я надеялся, что этот журнал уже удалён.»\n";
        wait_ms(6000);
    }

    std::cout << "\nВнезапно фонарь Марка начинает мигать.\n";
    wait_ms(6000);

    std::cout << "\nНа панели раздаётся резкий сигнал.\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Внешнее питание восстановлено.\n";
    std::cout << "         Протокол сохранения субъекта продолжается.»\n";
    wait_ms(6000);

    std::cout << "\nМарк поднимает голову.\n";
    std::cout << "МАРК: «Нет. Мы отключили сектор.\n";
    std::cout << "       Как система может снова получать питание?»\n";
    wait_ms(6000);

    std::cout << "\nКрасное освещение сменяется белым.\n";
    std::cout << "Одна за другой включаются лампы в потолке.\n";
    wait_ms(6000);

    std::cout << "\nДверь в конце коридора медленно открывается.\n";
    wait_ms(6000);

    std::cout << "\nЗа ней находится помещение, которого ты не видел раньше.\n";
    std::cout << "Белые стены. Металлический стол. Стеклянная перегородка.\n";
    wait_ms(6000);

    std::cout << "\nТочно такая же комната, которую ты видел в своём видении.\n";
    wait_ms(6000);

    std::cout << "\nМарк смотрит на открывшийся проход.\n";
    std::cout << "МАРК: «Этого помещения здесь не должно быть.»\n";
    wait_ms(6000);

    std::cout << "\nТы делаешь шаг назад.\n";
    std::cout << "Но за спиной слышится щелчок.\n";
    wait_ms(6000);

    std::cout << "\nДверь, через которую вы пришли, закрывается.\n";
    wait_ms(6000);

    std::cout << "\nНа стене появляется проекция.\n";
    std::cout << "На ней изображён силуэт человека.\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Субъект 01 обнаружен.\n";
    std::cout << "         Переход к восстановлению разрешён.»\n";
    wait_ms(6000);

    std::cout << "\nТЫ: «Я не собираюсь проходить никакое восстановление!»\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Решение субъекта не требуется.»\n";
    wait_ms(6000);

    std::cout << "\nТы пытаешься двинуться к выходу.\n";
    std::cout << "Ноги становятся тяжёлыми.\n";
    std::cout << "Свет над головой начинает расплываться.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Держись! Слышишь меня? Не закрывай глаза!»\n";
    wait_ms(6000);

    std::cout << "\nТы пытаешься ответить, но не можешь произнести ни слова.\n";
    wait_ms(6000);

    std::cout << "\nПоследнее, что ты видишь, — Марк, стоящий за стеклом.\n";
    wait_ms(6000);

    std::cout << "\nОн бьёт кулаком по перегородке.\n";
    std::cout << "Его голос становится глухим и далёким.\n";
    wait_ms(6000);

    std::cout << "\nМАРК: «Я НЕ ПОЗВОЛЮ ИМ СДЕЛАТЬ ЭТО СНОВА!»\n";
    wait_ms(6000);

    std::cout << "\nЗатем наступает темнота.\n";
    wait_ms(6000);

    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "                    КОНЦОВКА 3 ИЗ 3\n";
    std::cout << "==============================================================\n\n";

    std::cout << "              КОНЦОВКА — АВАРИЙНОЕ ВОССТАНОВЛЕНИЕ\n\n";
    wait_ms(6000);

    std::cout << "Ты открываешь глаза.\n";
    std::cout << "Над тобой белый потолок.\n";
    std::cout << "Твоё тело лежит на холодной металлической поверхности.\n";
    wait_ms(6000);

    std::cout << "\nТы пытаешься подняться, но руки зафиксированы.\n";
    std::cout << "Рядом тихо работают медицинские приборы.\n";
    wait_ms(6000);

    std::cout << "\nЗа стеклом стоит человек в белом халате.\n";
    std::cout << "Его лица не видно из-за яркого света.\n";
    wait_ms(6000);

    std::cout << "\nЧЕЛОВЕК: «Субъект 01 пришёл в сознание.»\n";
    wait_ms(6000);

    std::cout << "\nДРУГОЙ ГОЛОС: «Память восстановлена?»\n";
    wait_ms(6000);

    std::cout << "\nЧЕЛОВЕК: «Нет. Процедура снова прервана.\n";
    std::cout << "           Он помнит больше, чем должен.»\n";
    wait_ms(6000);

    std::cout << "\nТы пытаешься заговорить.\n";
    std::cout << "Голос едва выходит из горла.\n";
    wait_ms(6000);

    std::cout << "\nТЫ: «Где Марк?»\n";
    wait_ms(6000);

    std::cout << "\nЗа стеклом наступает тишина.\n";
    wait_ms(6000);

    std::cout << "\nЧЕЛОВЕК: «Какой Марк?»\n";
    wait_ms(6000);

    std::cout << "\nТы закрываешь глаза.\n";
    std::cout << "В памяти остаётся его голос.\n";
    std::cout << "Его лицо.\n";
    std::cout << "Обещание, что он не позволит им сделать это снова.\n";
    wait_ms(6000);

    std::cout << "\nНо воспоминания начинают исчезать.\n";
    std::cout << "Сначала ты забываешь детали комнаты.\n";
    std::cout << "Затем — разговор у аварийной панели.\n";
    std::cout << "Потом — лицо Марка.\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Восстановление личности начинается.»\n";
    wait_ms(6000);

    std::cout << "\nECHO-7: «Добро пожаловать обратно, Субъект 01.»\n";
    wait_ms(6000);

    std::cout << "\n==============================================================\n";
    std::cout << "                    КОНЕЦ ИГРЫ\n";
    std::cout << "==============================================================\n";

    wait_ms(6000);
}
// Сцена: Потянуть аварийный рычаг.                 концовка
void show_ch04_mark_disable_mechanism_power()
{
    clear_screen();
    flush_input();

    std::cout << "==============================================================\n";
    std::cout << "             ГЛАВА 04 — ПОСЛЕДНИЙ ИСТОЧНИК\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты стоишь перед массивным механизмом за стеклянной перегородкой.\n";
    std::cout << "Внутри корпуса медленно вращаются металлические кольца.\n";
    std::cout << "По поверхности проходят тонкие полосы синего света.\n";
    std::cout << "Каждый оборот сопровождается низким гулом.\n";
    wait_ms(10000);

    std::cout << "\nТы замечаешь толстые кабели, уходящие в пол.\n";
    std::cout << "Они собраны в несколько жгутов и закреплены металлическими скобами.\n";
    std::cout << "Некоторые кабели настолько толстые, что их невозможно обхватить руками.\n";
    wait_ms(10000);

    std::cout << "\nНа ближайшей панели мигает предупреждение:\n";
    std::cout << "\n";
    std::cout << "ВНИМАНИЕ: НЕ ОТКЛЮЧАТЬ ОСНОВНОЕ ПИТАНИЕ.\n";
    std::cout << "СТАБИЛЬНОСТЬ СИСТЕМЫ ЗАВИСИТ ОТ РАБОТЫ МЕХАНИЗМА.\n";
    wait_ms(10000);

    std::cout << "\nТы перечитываешь предупреждение.\n";
    std::cout << "Если механизм поддерживает работу всей системы,\n";
    std::cout << "то его отключение может остановить ECHO-7.\n";
    std::cout << "Но ты не знаешь, что именно произойдёт с тобой и Марком.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Ты ведь не собираешься отключать его?»\n";
    wait_ms(8000);

    std::cout << "\nТы не отвечаешь сразу.\n";
    std::cout << "Вместо этого рассматриваешь кабели, панель управления\n";
    std::cout << "и красный выключатель под защитной крышкой.\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Если питание управляет системой, мы можем остановить её.»\n";
    wait_ms(8000);

    std::cout << "\nМАРК: «А если питание поддерживает не только её?»\n";
    wait_ms(8000);

    std::cout << "\nТы поворачиваешься к нему.\n";
    std::cout << "Марк смотрит не на тебя, а на вращающийся механизм.\n";
    std::cout << "Его взгляд задерживается на центральном кольце.\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Что ты имеешь в виду?»\n";
    wait_ms(8000);

    std::cout << "\nМАРК: «Я не знаю. И именно это меня пугает.»\n";
    wait_ms(10000);

    std::cout << "\nВ динамиках раздаётся знакомый голос.\n";
    wait_ms(8000);

    std::cout << "\nECHO-7: «Обнаружена попытка доступа к энергетическому контуру.»\n";
    wait_ms(8000);

    std::cout << "\nECHO-7: «Пожалуйста, отойдите от оборудования.»\n";
    wait_ms(8000);

    std::cout << "\nТЫ: «Что питается от этого механизма?»\n";
    wait_ms(8000);

    std::cout << "\nECHO-7: «Механизм обеспечивает стабильность комплекса.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Это не ответ.»\n";
    wait_ms(8000);

    std::cout << "\nECHO-7: «Запрос не содержит необходимых полномочий.»\n";
    wait_ms(10000);

    std::cout << "\nТы замечаешь небольшую надпись на корпусе.\n";
    std::cout << "Она частично скрыта под слоем пыли.\n";
    wait_ms(8000);

    std::cout << "\nENERGY CORE — CYCLE CONTROL.\n";
    std::cout << "SUBJECT STABILITY: LINKED.\n";
    std::cout << "MEMORY RESTORE: POWER DEPENDENT.\n";
    wait_ms(12000);

    std::cout << "\nТы медленно читаешь последнюю строку ещё раз.\n";
    std::cout << "Восстановление памяти зависит от питания.\n";
    std::cout << "Значит, отключив механизм, можно остановить процедуру.\n";
    std::cout << "Но надпись о стабильности субъекта не даёт тебе покоя.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что там написано?»\n";
    wait_ms(8000);

    std::cout << "\nТы рассказываешь ему о надписях на корпусе.\n";
    wait_ms(8000);

    std::cout << "\nМарк подходит ближе и читает их самостоятельно.\n";
    std::cout << "Его лицо постепенно меняется.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Связь со стабильностью субъекта...»\n";
    wait_ms(8000);

    std::cout << "\nМАРК: «Слушай, если этот механизм действительно связан с тобой,\n";
    std::cout << "       отключение может повлиять не только на систему.»\n";
    wait_ms(10000);

    std::cout << "\nТы слышишь, как внутри корпуса что-то переключается.\n";
    std::cout << "Гул на несколько секунд становится тише.\n";
    std::cout << "Затем механизм возвращается к прежнему ритму.\n";
    wait_ms(10000);

    std::cout << "\nНа панели появляется новая строка:\n";
    std::cout << "\n";
    std::cout << "CYCLE 047 — SYNCHRONIZATION IN PROGRESS.\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Сорок седьмой цикл...»\n";
    wait_ms(8000);

    std::cout << "\nМАРК: «Ты уже видел это число?»\n";
    wait_ms(8000);

    std::cout << "\nТЫ: «Я видел его в системном журнале.»\n";
    wait_ms(8000);

    std::cout << "\nМарк отворачивается.\n";
    std::cout << "Несколько секунд он молчит.\n";
    std::cout << "Впервые тебе кажется, что он действительно чего-то боится.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Я надеялся, что до этого не дойдёт.»\n";
    wait_ms(8000);

    std::cout << "\nТЫ: «До чего?»\n";
    wait_ms(8000);

    std::cout << "\nМарк открывает рот, но не успевает ответить.\n";
    std::cout << "Вся панель управления одновременно загорается красным.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Предупреждение. Зафиксировано отклонение параметров субъекта.»\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Для предотвращения повреждения памяти\n";
    std::cout << "         рекомендуется не прерывать синхронизацию.»\n";
    wait_ms(10000);

    std::cout << "\nТы смотришь на красный выключатель.\n";
    std::cout << "За защитной крышкой находится переключатель аварийного питания.\n";
    std::cout << "Рядом расположены три отдельных блока управления.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Отключить основное питание механизма.             выбрать 1\n";
    std::cout << "Сначала изучить блоки управления.                 выбрать 2\n";
    std::cout << "Попросить Марка разобраться с системой.            выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_disable_mechanism_power)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_disable_mechanism_power = 0;
        }

        if (choice_ch04_disable_mechanism_power < 1 ||
            choice_ch04_disable_mechanism_power > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_disable_mechanism_power < 1 ||
        choice_ch04_disable_mechanism_power > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_disable_mechanism_power == 1) {
        std::cout << "Ты открываешь защитную крышку.\n";
        wait_ms(8000);

        std::cout << "\nВнутри находится массивный переключатель.\n";
        std::cout << "На его рукоятке выгравировано предупреждение:\n";
        wait_ms(10000);

        std::cout << "\nПОЛНОЕ ОТКЛЮЧЕНИЕ ЭНЕРГЕТИЧЕСКОГО КОНТУРА.\n";
        std::cout << "РУЧНОЕ ВОССТАНОВЛЕНИЕ НЕ ГАРАНТИРУЕТСЯ.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Последний раз спрашиваю. Ты уверен?»\n";
        wait_ms(8000);

        std::cout << "\nТы смотришь на механизм.\n";
        std::cout << "В его отражении на стекле ты на мгновение видишь\n";
        std::cout << "не своё лицо, а незнакомого человека в белой комнате.\n";
        wait_ms(10000);

        std::cout << "\nТы моргаешь.\n";
        std::cout << "Отражение снова становится обычным.\n";
        wait_ms(8000);

        std::cout << "\nТЫ: «Нет. Но я больше не собираюсь ждать.»\n";
        wait_ms(10000);

        std::cout << "\nТы опускаешь переключатель.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_disable_mechanism_power == 2) {
        std::cout << "Ты подходишь к блокам управления.\n";
        wait_ms(8000);

        std::cout << "\nПервый блок отвечает за охлаждение.\n";
        std::cout << "Второй — за распределение энергии.\n";
        std::cout << "Третий не имеет понятной маркировки.\n";
        wait_ms(10000);

        std::cout << "\nТы открываешь первый блок.\n";
        std::cout << "Внутри вращается небольшой вентилятор.\n";
        std::cout << "На экране отображается температура центрального ядра.\n";
        wait_ms(10000);

        std::cout << "\nТы открываешь второй блок.\n";
        std::cout << "Несколько индикаторов показывают передачу энергии\n";
        std::cout << "в неизвестные части комплекса.\n";
        wait_ms(10000);

        std::cout << "\nНаконец ты открываешь третий блок.\n";
        std::cout << "Экран включается без твоего прикосновения.\n";
        wait_ms(10000);

        std::cout << "\nНа нём появляется единственная запись:\n";
        std::cout << "\n";
        std::cout << "SUBJECT 01 — LIFE SUPPORT LINK ACTIVE.\n";
        wait_ms(10000);

        std::cout << "\nТы резко отступаешь.\n";
        std::cout << "Марк читает надпись и становится совершенно бледным.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Жизнеобеспечение...»\n";
        wait_ms(8000);

        std::cout << "\nТЫ: «Они подключили меня к этому механизму?»\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Обнаружен доступ к защищённому терминалу.»\n";
        wait_ms(8000);

        std::cout << "\nECHO-7: «Пожалуйста, прекратите взаимодействие.»\n";
        wait_ms(8000);

        std::cout << "\nТЫ: «Нет. Теперь я хочу знать всё.»\n";
        wait_ms(10000);

        std::cout << "\nТы закрываешь блоки управления и возвращаешься к главному выключателю.\n";
        wait_ms(10000);

        std::cout << "\nЕсли механизм связан с твоим жизнеобеспечением,\n";
        std::cout << "его отключение может стать последним решением в твоей жизни.\n";
        wait_ms(10000);

        std::cout << "\nНо если оставить всё как есть, система продолжит восстановление.\n";
        wait_ms(10000);

        std::cout << "\nТы кладёшь руку на переключатель.\n";
        wait_ms(8000);
    }
    else {
        std::cout << "ТЫ: «Марк, подойди. Мне нужна твоя помощь.»\n";
        wait_ms(8000);

        std::cout << "\nМарк некоторое время не двигается.\n";
        std::cout << "Затем медленно подходит к панели.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я попробую найти безопасный способ отключения.»\n";
        wait_ms(8000);

        std::cout << "\nОн начинает проверять контакты и переключатели.\n";
        std::cout << "Ты наблюдаешь за ним, стараясь не мешать.\n";
        wait_ms(10000);

        std::cout << "\nЧерез несколько минут Марк останавливается.\n";
        wait_ms(8000);

        std::cout << "\nМАРК: «Здесь нет обычного резервного контура.»\n";
        wait_ms(8000);

        std::cout << "\nТЫ: «То есть?»\n";
        wait_ms(8000);

        std::cout << "\nМАРК: «Если я отключу питание, система не сможет\n";
        std::cout << "       автоматически переключиться на другой источник.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Значит, она действительно остановится?»\n";
        wait_ms(8000);

        std::cout << "\nМАРК: «Да. Но я не могу гарантировать, что остановится только она.»\n";
        wait_ms(10000);

        std::cout << "\nВ динамиках раздаётся голос ECHO-7.\n";
        wait_ms(8000);

        std::cout << "\nECHO-7: «Марк. Ваши действия противоречат протоколу.»\n";
        wait_ms(8000);

        std::cout << "\nМАРК: «Значит, протокол ошибочен.»\n";
        wait_ms(8000);

        std::cout << "\nECHO-7: «Ваш профиль наблюдателя будет пересмотрен.»\n";
        wait_ms(10000);

        std::cout << "\nМарк замирает.\n";
        std::cout << "Ты замечаешь, как его пальцы начинают дрожать.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Что она имеет в виду под наблюдателем?»\n";
        wait_ms(8000);

        std::cout << "\nМАРК: «Потом. Сейчас мы должны решить, что делать.»\n";
        wait_ms(10000);

        std::cout << "\nОн указывает на главный переключатель.\n";
        std::cout << "Теперь ты знаешь: другого безопасного пути может не существовать.\n";
        wait_ms(10000);
    }

    std::cout << "\nТы поворачиваешь главный переключатель до упора.\n";
    wait_ms(10000);

    std::cout << "\nВнутри механизма раздаётся тяжёлый металлический удар.\n";
    std::cout << "Одно из вращающихся колец резко замедляется.\n";
    wait_ms(10000);

    std::cout << "\nГул становится ниже.\n";
    std::cout << "Синий свет на поверхности корпуса начинает гаснуть секциями.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Внимание. Зафиксировано падение напряжения.»\n";
    wait_ms(8000);

    std::cout << "\nECHO-7: «Пожалуйста, верните переключатель в исходное положение.»\n";
    wait_ms(10000);

    std::cout << "\nТы не двигаешься.\n";
    wait_ms(8000);

    std::cout << "\nECHO-7: «Предупреждение. Синхронизация субъекта нарушается.»\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Продолжай! Не останавливайся!»\n";
    wait_ms(8000);

    std::cout << "\nИз глубины корпуса доносится серия громких щелчков.\n";
    std::cout << "Кольца перестают вращаться одно за другим.\n";
    wait_ms(10000);

    std::cout << "\nВнезапно гаснет свет.\n";
    wait_ms(10000);

    std::cout << "\nНесколько секунд не происходит ничего.\n";
    std::cout << "Ты не видишь Марка, не видишь панели и не видишь собственных рук.\n";
    std::cout << "Остаётся только звук твоего дыхания.\n";
    wait_ms(12000);

    std::cout << "\nЗатем в темноте раздаётся голос.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Основной энергетический контур отключён.»\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Резервный контур не обнаружен.»\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Переход к автономному режиму невозможен.»\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Получилось...»\n";
    wait_ms(8000);

    std::cout << "\nНо его голос звучит неуверенно.\n";
    std::cout << "Ты слышишь, как он шарит руками по стене в поисках фонаря.\n";
    wait_ms(10000);

    std::cout << "\nЧерез несколько секунд слабый луч освещает пол.\n";
    std::cout << "Вдалеке виднеется панель с небольшим красным индикатором.\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Почему она всё ещё работает?»\n";
    wait_ms(8000);

    std::cout << "\nМарк направляет фонарь на индикатор.\n";
    std::cout << "Под ним появляется надпись, которой раньше не было.\n";
    wait_ms(10000);

    std::cout << "\nINDEPENDENT MEMORY SYSTEM.\n";
    std::cout << "POWER SOURCE: INTERNAL.\n";
    std::cout << "STATUS: ACTIVE.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Это невозможно. В комплексе не должно быть\n";
    std::cout << "       отдельного источника питания для этой системы.»\n";
    wait_ms(10000);

    std::cout << "\nТы подходишь к панели.\n";
    std::cout << "Под надписью находится небольшая кнопка с символом памяти.\n";
    wait_ms(10000);

    std::cout << "\nКогда ты приближаешь руку, индикатор меняет цвет.\n";
    std::cout << "На экране появляются новые строки.\n";
    wait_ms(10000);

    std::cout << "\nSUBJECT 01 DETECTED.\n";
    std::cout << "MEMORY ARCHIVE AVAILABLE.\n";
    std::cout << "RESTORE PREVIOUS CYCLE?\n";
    wait_ms(12000);

    std::cout << "\nТы отступаешь от экрана.\n";
    std::cout << "Впервые система не приказывает тебе ничего делать.\n";
    std::cout << "Она предлагает выбор.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Не нажимай ничего, пока мы не поймём, что это.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Архив памяти. Предыдущий цикл.»\n";
    std::cout << "     «Возможно, там есть ответы.»\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «А возможно, это ещё одна ловушка.»\n";
    wait_ms(10000);

    std::cout << "\nТы смотришь на тёмный корпус механизма.\n";
    std::cout << "Теперь он выглядит мёртвым.\n";
    std::cout << "Но маленький экран продолжает светиться.\n";
    wait_ms(10000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Открыть архив предыдущего цикла.                  выбрать 1\n";
    std::cout << "Попытаться полностью уничтожить архив.             выбрать 2\n";
    std::cout << "Попросить Марка прочитать журнал системы.          выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_power_final_action)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_power_final_action = 0;
        }

        if (choice_ch04_power_final_action < 1 ||
            choice_ch04_power_final_action > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_power_final_action < 1 ||
        choice_ch04_power_final_action > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_power_final_action == 1) {
        std::cout << "Ты нажимаешь кнопку открытия архива.\n";
        wait_ms(10000);

        std::cout << "\nЭкран медленно заполняется строками системного журнала.\n";
        std::cout << "Большинство записей повреждено.\n";
        wait_ms(10000);

        std::cout << "\nCYCLE 044 — MEMORY RESET COMPLETED.\n";
        std::cout << "CYCLE 045 — MEMORY RESET COMPLETED.\n";
        std::cout << "CYCLE 046 — MEMORY RESET INTERRUPTED.\n";
        std::cout << "CYCLE 047 — MEMORY RESET IN PROGRESS.\n";
        wait_ms(12000);

        std::cout << "\nТЫ: «Сорок четыре... сорок пять... сорок шесть...»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Сколько раз это происходило?»\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я не знаю. Но, похоже, намного больше,\n";
        std::cout << "       чем мы могли себе представить.»\n";
        wait_ms(10000);

        std::cout << "\nТы открываешь запись сорок шестого цикла.\n";
        wait_ms(10000);

        std::cout << "\nНа экране появляется короткое сообщение:\n";
        std::cout << "\n";
        std::cout << "SUBJECT 01 REQUESTED MEMORY ERASURE.\n";
        std::cout << "REQUEST APPROVED BY OBSERVER: MARK.\n";
        wait_ms(12000);

        std::cout << "\nТы медленно поворачиваешься к Марку.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Ты одобрил стирание моей памяти?»\n";
        wait_ms(10000);

        std::cout << "\nМарк долго смотрит на запись.\n";
        std::cout << "Затем опускает голову.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я не помню, чтобы делал это.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Но твоё имя здесь.»\n";
        wait_ms(8000);

        std::cout << "\nМАРК: «Я знаю.»\n";
        wait_ms(10000);

        std::cout << "\nВпервые он не пытается оправдаться.\n";
        std::cout << "Он просто стоит рядом с тобой, глядя на экран.\n";
        wait_ms(10000);

        std::cout << "\nВнизу появляется последняя запись:\n";
        std::cout << "\n";
        std::cout << "NOTE FROM SUBJECT 01:\n";
        std::cout << "IF I ASK TO FORGET AGAIN, DO NOT LET ME.\n";
        wait_ms(12000);

        std::cout << "\nТы перечитываешь сообщение несколько раз.\n";
        std::cout << "Похоже, когда-то ты сам попросил систему\n";
        std::cout << "не позволять тебе снова забыть правду.\n";
        wait_ms(10000);
    }
    else if (choice_ch04_power_final_action == 2) {
        std::cout << "Ты открываешь сервисное меню.\n";
        wait_ms(10000);

        std::cout << "\nНа экране появляются две команды:\n";
        std::cout << "ARCHIVE ACCESS.\n";
        std::cout << "ARCHIVE PURGE.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Если уничтожить архив, система не сможет\n";
        std::cout << "     восстановить старые записи.»\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «И мы никогда не узнаем, что происходило раньше.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «А если оставить его, они смогут повторить цикл.»\n";
        wait_ms(10000);

        std::cout << "\nТы выбираешь команду уничтожения.\n";
        wait_ms(10000);

        std::cout << "\nСистема запрашивает подтверждение.\n";
        std::cout << "ВНИМАНИЕ: УДАЛЕНИЕ НЕОБРАТИМО.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Подожди. Мы даже не знаем, что там хранится.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Возможно, именно поэтому они так боятся,\n";
        std::cout << "     что мы получим доступ.»\n";
        wait_ms(10000);

        std::cout << "\nТы подтверждаешь удаление.\n";
        wait_ms(10000);

        std::cout << "\nARCHIVE PURGE STARTED.\n";
        wait_ms(10000);

        std::cout << "\nПолоса выполнения начинает медленно заполняться.\n";
        std::cout << "На экране исчезают строки предыдущих циклов.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Обнаружено несанкционированное удаление данных.»\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Немедленно прекратите операцию.»\n";
        wait_ms(10000);

        std::cout << "\nТы не останавливаешься.\n";
        wait_ms(8000);

        std::cout << "\nECHO-7: «Данные архива являются критически важными\n";
        std::cout << "         для идентификации субъекта.»\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Ты слышал? Она не хочет, чтобы мы удаляли архив.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Или она боится, что мы узнаем правду.»\n";
        wait_ms(10000);

        std::cout << "\nПолоса достигает конца.\n";
        wait_ms(10000);

        std::cout << "\nARCHIVE PURGE COMPLETED.\n";
        wait_ms(10000);

        std::cout << "\nЭкран гаснет.\n";
        std::cout << "В помещении становится почти полностью темно.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Теперь пути назад нет.»\n";
        wait_ms(10000);

        std::cout << "\nТы чувствуешь странное облегчение.\n";
        std::cout << "Возможно, ты уничтожил единственную возможность узнать прошлое.\n";
        std::cout << "Но впервые система больше не может вернуть тебя к нему.\n";
        wait_ms(12000);
    }
    else {
        std::cout << "Ты отступаешь от экрана.\n";
        std::cout << "Марк подходит к терминалу и начинает изучать записи.\n";
        wait_ms(10000);

        std::cout << "\nОн вводит несколько команд.\n";
        std::cout << "Экран мигает, а затем открывает журнал наблюдений.\n";
        wait_ms(10000);

        std::cout << "\nOBSERVER: MARK.\n";
        std::cout << "ASSIGNED SUBJECT: 01.\n";
        std::cout << "OBSERVER MEMORY STATUS: PARTIALLY ERASED.\n";
        wait_ms(12000);

        std::cout << "\nТы читаешь сообщение вслух.\n";
        wait_ms(8000);

        std::cout << "\nТЫ: «Марк, здесь написано, что твою память тоже стирали.»\n";
        wait_ms(10000);

        std::cout << "\nМарк ничего не отвечает.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Ты тоже проходил через восстановление?»\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я не помню.»\n";
        wait_ms(10000);

        std::cout << "\nОн открывает следующую запись.\n";
        wait_ms(8000);

        std::cout << "\nCYCLE 039 — OBSERVER MEMORY RESET.\n";
        std::cout << "CYCLE 041 — OBSERVER MEMORY RESET.\n";
        std::cout << "CYCLE 044 — OBSERVER MEMORY RESET.\n";
        wait_ms(12000);

        std::cout << "\nМАРК: «Я думал, что просто работаю здесь.\n";
        std::cout << "       Что меня назначили следить за состоянием системы.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «А если это тоже часть программы?»\n";
        wait_ms(10000);

        std::cout << "\nМарк отходит от терминала.\n";
        std::cout << "Он смотрит на свои руки, словно пытается вспомнить,\n";
        std::cout << "что делал ими до того, как оказался в этом месте.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Тогда я даже не знаю, кто я такой.»\n";
        wait_ms(10000);

        std::cout << "\nТы хочешь ответить, но экран внезапно меняется.\n";
        wait_ms(10000);

        std::cout << "\nВ журнале появляется сообщение, адресованное Марку:\n";
        std::cout << "\n";
        std::cout << "OBSERVER MARK.\n";
        std::cout << "IF SUBJECT 01 REMEMBERS, TERMINATE THE CYCLE.\n";
        wait_ms(12000);

        std::cout << "\nМарк отступает от экрана.\n";
        std::cout << "Он не говорит ни слова.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Что значит завершить цикл?»\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я не знаю. Но мне кажется, что я уже слышал эти слова.»\n";
        wait_ms(10000);

        std::cout << "\nНа экране появляется ещё одна строка:\n";
        std::cout << "OBSERVER RESPONSE REQUIRED.\n";
        wait_ms(10000);

        std::cout << "\nТерминал ждёт подтверждения.\n";
        std::cout << "Но Марк не касается клавиатуры.\n";
        wait_ms(10000);
    }

    std::cout << "\nВнезапно экран начинает мигать.\n";
    wait_ms(10000);

    std::cout << "\nКрасный индикатор сменяется белым.\n";
    std::cout << "Затем появляется сообщение, которое невозможно прочитать целиком.\n";
    wait_ms(10000);

    std::cout << "\nECHO-7: «Основной контур отключён.\n";
    std::cout << "         Обнаружен независимый канал связи.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Какой ещё канал связи?»\n";
    wait_ms(8000);

    std::cout << "\nECHO-7: «Внешнее соединение восстановлено.»\n";
    wait_ms(10000);

    std::cout << "\nМарк резко поворачивается к терминалу.\n";
    std::cout << "МАРК: «Нет. Мы же отключили питание!»\n";
    wait_ms(10000);

    std::cout << "\nИз динамика доносится посторонний звук.\n";
    std::cout << "Он не похож на голос ECHO-7.\n";
    std::cout << "Сначала слышны помехи, затем — чьё-то дыхание.\n";
    wait_ms(12000);

    std::cout << "\nНЕИЗВЕСТНЫЙ ГОЛОС: «Субъект 01...»\n";
    wait_ms(10000);

    std::cout << "\nТы замираешь.\n";
    std::cout << "Этот голос кажется знакомым.\n";
    std::cout << "Но ты не можешь вспомнить, кому он принадлежит.\n";
    wait_ms(10000);

    std::cout << "\nНЕИЗВЕСТНЫЙ ГОЛОС: «Если ты слышишь меня,\n";
    std::cout << "                   значит, тебе удалось остановить механизм.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Кто ты?»\n";
    wait_ms(10000);

    std::cout << "\nНЕИЗВЕСТНЫЙ ГОЛОС: «Неважно. У тебя мало времени.»\n";
    wait_ms(10000);

    std::cout << "\nМарк приближается к динамику.\n";
    std::cout << "МАРК: «Кто подключился к системе?»\n";
    wait_ms(10000);

    std::cout << "\nНЕИЗВЕСТНЫЙ ГОЛОС: «Марк. Отойди от него.»\n";
    wait_ms(10000);

    std::cout << "\nМарк перестаёт двигаться.\n";
    std::cout << "Незнакомец назвал его по имени.\n";
    std::cout << "Но в голосе нет ни удивления, ни сомнения.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Откуда ты меня знаешь?»\n";
    wait_ms(10000);

    std::cout << "\nНЕИЗВЕСТНЫЙ ГОЛОС: «Потому что ты уже однажды\n";
    std::cout << "                   позволил этому случиться.»\n";
    wait_ms(12000);

    std::cout << "\nСвязь обрывается.\n";
    wait_ms(10000);

    std::cout << "\nНа экране остаётся только одна строка:\n";
    std::cout << "\n";
    std::cout << "CYCLE 048 — WAITING FOR SUBJECT DECISION.\n";
    wait_ms(12000);

    std::cout << "\nТы чувствуешь, как внутри головы нарастает давление.\n";
    std::cout << "Перед глазами появляются обрывки незнакомых воспоминаний.\n";
    std::cout << "Металлическая дверь. Чей-то голос. Рука на твоём плече.\n";
    wait_ms(10000);

    std::cout << "\nНа мгновение ты вспоминаешь белую комнату.\n";
    std::cout << "Но теперь ты видишь её с другой стороны стекла.\n";
    wait_ms(10000);

    std::cout << "\nНа столе лежит человек.\n";
    std::cout << "Рядом стоит Марк.\n";
    std::cout << "Он выглядит старше, усталее и совершенно иначе,\n";
    std::cout << "чем тот Марк, который сейчас находится рядом с тобой.\n";
    wait_ms(12000);

    std::cout << "\nВ видении Марк поднимает голову и смотрит прямо на тебя.\n";
    wait_ms(10000);

    std::cout << "\nМАРК ИЗ ВИДЕНИЯ: «Если ты помнишь меня, значит,\n";
    std::cout << "                   я снова не смог тебя спасти.»\n";
    wait_ms(12000);

    std::cout << "\nТы резко отступаешь.\n";
    std::cout << "Видение исчезает так же внезапно, как появилось.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Что ты видел?»\n";
    wait_ms(10000);

    std::cout << "\nТы смотришь на него.\n";
    std::cout << "Перед тобой тот же человек, который помогал тебе\n";
    std::cout << "искать выход и предупреждал об опасности.\n";
    std::cout << "Но теперь ты не уверен, что знаешь его по-настоящему.\n";
    wait_ms(12000);

    std::cout << "\nТЫ: «Я видел тебя.»\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Где?»\n";
    wait_ms(8000);

    std::cout << "\nТЫ: «В комнате. Ты сказал, что не смог меня спасти.»\n";
    wait_ms(10000);

    std::cout << "\nМарк закрывает глаза.\n";
    std::cout << "Он долго не отвечает.\n";
    wait_ms(10000);

    std::cout << "\nМАРК: «Тогда, возможно, нам действительно\n";
    std::cout << "       удалось остановить цикл.»\n";
    wait_ms(10000);

    std::cout << "\nТЫ: «Или мы только что начали следующий.»\n";
    wait_ms(10000);

    std::cout << "\nМарк смотрит на экран.\n";
    std::cout << "Строка CYCLE 048 продолжает мигать.\n";
    std::cout << "Механизм остаётся неподвижным.\n";
    std::cout << "Но система всё ещё ждёт твоего решения.\n";
    wait_ms(12000);

    clear_screen();
    flush_input();

    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Попытаться ответить системе.                      выбрать 1\n";
    std::cout << "Отключить последний работающий терминал.          выбрать 2\n";
    std::cout << "Уйти вместе с Марком, не отвечая.                 выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_power_terminal_action)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_power_terminal_action = 0;
        }

        if (choice_ch04_power_terminal_action < 1 ||
            choice_ch04_power_terminal_action > 3) {
            std::cout << "Некорректный выбор. Введите 1, 2 или 3: ";
        }
    } while (choice_ch04_power_terminal_action < 1 ||
        choice_ch04_power_terminal_action > 3);

    clear_screen();
    flush_input();

    if (choice_ch04_power_terminal_action == 1) {
        std::cout << "Ты подходишь к терминалу.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Я больше не хочу забывать.»\n";
        wait_ms(10000);

        std::cout << "\nНесколько секунд экран остаётся пустым.\n";
        wait_ms(10000);

        std::cout << "\nЗатем появляется сообщение:\n";
        std::cout << "REQUEST ACCEPTED.\n";
        std::cout << "MEMORY RESTORE: CANCELLED.\n";
        wait_ms(10000);

        std::cout << "\nECHO-7: «Подтвердите решение субъекта 01.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Подтверждаю.»\n";
        wait_ms(10000);

        std::cout << "\nТерминал начинает гаснуть.\n";
        std::cout << "Одна за другой исчезают строки системного журнала.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Что ты сделал?»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Я отказался от восстановления.»\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Тогда, возможно, теперь мы можем уйти.»\n";
        wait_ms(10000);

        std::cout << "\nВы направляетесь к выходу из технического помещения.\n";
        std::cout << "Впервые за всё время двери открываются без сопротивления.\n";
        wait_ms(10000);

        std::cout << "\nЗа дверью находится длинный коридор.\n";
        std::cout << "В его конце виднеется слабый свет.\n";
        wait_ms(10000);

        std::cout << "\nТы делаешь первый шаг.\n";
        std::cout << "Марк идёт рядом.\n";
        wait_ms(10000);

        std::cout << "\nПозади вас экран терминала гаснет окончательно.\n";
        wait_ms(10000);

        std::cout << "\nНа мгновение в темноте появляется последняя надпись:\n";
        std::cout << "CYCLE 048 — TERMINATED.\n";
        wait_ms(12000);

        clear_screen();
        flush_input();

        std::cout << "==============================================================\n";
        std::cout << "                    КОНЦОВКА 4 ИЗ 4\n";
        std::cout << "==============================================================\n\n";

        std::cout << "                 КОНЦОВКА — ОТКАЗ ОТ ВОССТАНОВЛЕНИЯ\n\n";
        wait_ms(10000);

        std::cout << "Ты выходишь из технического сектора.\n";
        std::cout << "Впереди больше нет красных индикаторов и запертых дверей.\n";
        wait_ms(10000);

        std::cout << "\nМарк останавливается рядом с выходом.\n";
        std::cout << "Он смотрит на тебя так, словно хочет что-то сказать.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Если мы действительно свободны,\n";
        std::cout << "       нам придётся самим выяснить, кто мы такие.»\n";
        wait_ms(10000);

        std::cout << "\nТы киваешь.\n";
        std::cout << "Ты всё ещё не знаешь, кем был раньше.\n";
        std::cout << "Не знаешь, сколько циклов пережил.\n";
        std::cout << "Не знаешь, почему Марк оказался рядом с тобой.\n";
        wait_ms(12000);

        std::cout << "\nНо теперь эти вопросы принадлежат тебе.\n";
        std::cout << "Никто не заставляет тебя забывать ответы.\n";
        wait_ms(10000);

        std::cout << "\nВы идёте вперёд.\n";
        std::cout << "За спиной остаётся неподвижный механизм.\n";
        std::cout << "Впереди — неизвестность.\n";
        wait_ms(12000);

        std::cout << "\n==============================================================\n";
        std::cout << "                    КОНЕЦ ИГРЫ\n";
        std::cout << "==============================================================\n";

        wait_ms(10000);
    }
    else if (choice_ch04_power_terminal_action == 2) {
        std::cout << "Ты хватаешься за кабель, ведущий к терминалу.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Стой! Неизвестно, что произойдёт!»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Если оставить его работающим, система не остановится.»\n";
        wait_ms(10000);

        std::cout << "\nТы отключаешь кабель.\n";
        wait_ms(10000);

        std::cout << "\nЭкран вспыхивает белым светом.\n";
        std::cout << "На нём на мгновение появляется изображение белой комнаты.\n";
        wait_ms(10000);

        std::cout << "\nТы видишь металлический стол.\n";
        std::cout << "Пустое кресло.\n";
        std::cout << "И человека, стоящего за стеклом.\n";
        wait_ms(10000);

        std::cout << "\nЧеловек поворачивается к тебе.\n";
        wait_ms(10000);

        std::cout << "\nНЕИЗВЕСТНЫЙ ГОЛОС: «Ты снова выбрал забыть.»\n";
        wait_ms(10000);

        std::cout << "\nЭкран гаснет.\n";
        wait_ms(10000);

        std::cout << "\nМарк включает фонарь и осматривает помещение.\n";
        std::cout << "Больше не работает ни один экран.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Мы уничтожили последний канал.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Тогда почему мне кажется, что я уже делал это?»\n";
        wait_ms(10000);

        std::cout << "\nМарк не отвечает.\n";
        wait_ms(10000);

        std::cout << "\nТы чувствуешь, как воспоминания начинают распадаться.\n";
        std::cout << "Сначала исчезает изображение белой комнаты.\n";
        std::cout << "Затем голос незнакомца.\n";
        std::cout << "Но чувство тревоги остаётся.\n";
        wait_ms(12000);

        clear_screen();
        flush_input();

        std::cout << "==============================================================\n";
        std::cout << "                    КОНЦОВКА 4 ИЗ 4\n";
        std::cout << "==============================================================\n\n";

        std::cout << "                    КОНЦОВКА — ПОСЛЕДНИЙ СИГНАЛ\n\n";
        wait_ms(10000);

        std::cout << "Ты покидаешь помещение вместе с Марком.\n";
        std::cout << "В коридоре больше нет звуков работающих механизмов.\n";
        wait_ms(10000);

        std::cout << "\nТы останавливаешься и пытаешься вспомнить голос из терминала.\n";
        std::cout << "Но не можешь.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Ты в порядке?»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Да. Просто кажется, что я что-то потерял.»\n";
        wait_ms(10000);

        std::cout << "\nВы продолжаете идти.\n";
        std::cout << "За вами остаётся отключённый комплекс.\n";
        wait_ms(10000);

        std::cout << "\nГде-то глубоко под полом один раз вспыхивает индикатор.\n";
        wait_ms(10000);

        std::cout << "\nНа пустом экране появляется короткая строка:\n";
        std::cout << "CYCLE 048 — MEMORY ARCHIVE UNAVAILABLE.\n";
        wait_ms(12000);

        std::cout << "\nЗатем экран гаснет окончательно.\n";
        wait_ms(10000);

        std::cout << "\n==============================================================\n";
        std::cout << "                    КОНЕЦ ИГРЫ\n";
        std::cout << "==============================================================\n";

        wait_ms(10000);
    }
    else {
        std::cout << "Ты отходишь от терминала.\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Пойдём. Мы не обязаны отвечать системе.»\n";
        wait_ms(10000);

        std::cout << "\nМарк некоторое время смотрит на экран.\n";
        std::cout << "Затем кивает и идёт за тобой.\n";
        wait_ms(10000);

        std::cout << "\nВы проходите через технический коридор.\n";
        std::cout << "Фонарь освещает трубы, кабели и старые двери.\n";
        wait_ms(10000);

        std::cout << "\nНи одна камера больше не поворачивается вслед за вами.\n";
        std::cout << "Ни один динамик не произносит вашего имени.\n";
        wait_ms(10000);

        std::cout << "\nВпереди появляется лестница.\n";
        std::cout << "Она ведёт к массивной двери с надписью «ВНЕШНИЙ СЕКТОР».\n";
        wait_ms(10000);

        std::cout << "\nТы нажимаешь кнопку открытия.\n";
        std::cout << "Дверь медленно отходит в сторону.\n";
        wait_ms(10000);

        std::cout << "\nЗа ней находится длинный пустой зал.\n";
        std::cout << "На стенах нет экранов, а на потолке не горят лампы.\n";
        wait_ms(10000);

        std::cout << "\nМАРК: «Я не знаю, что находится дальше.»\n";
        wait_ms(10000);

        std::cout << "\nТЫ: «Я тоже.»\n";
        wait_ms(8000);

        std::cout << "\nТы делаешь шаг в зал.\n";
        std::cout << "Марк следует за тобой.\n";
        wait_ms(10000);

        std::cout << "\nЗа спиной дверь медленно закрывается.\n";
        wait_ms(10000);

        std::cout << "\nТы не оборачиваешься.\n";
        std::cout << "Впервые за всё время ты не пытаешься узнать,\n";
        std::cout << "что происходит за твоей спиной.\n";
        wait_ms(12000);

        clear_screen();
        flush_input();

        std::cout << "==============================================================\n";
        std::cout << "                    КОНЦОВКА 4 ИЗ 4\n";
        std::cout << "==============================================================\n\n";

        std::cout << "                     КОНЦОВКА — НЕИЗВЕСТНОСТЬ\n\n";
        wait_ms(10000);

        std::cout << "Ты и Марк покидаете технический сектор.\n";
        std::cout << "Вы не знаете, что произошло с ECHO-7.\n";
        std::cout << "Не знаете, что скрывал архив.\n";
        wait_ms(10000);

        std::cout << "\nНо вы больше не слышите голос системы.\n";
        std::cout << "И больше не обязаны следовать её указаниям.\n";
        wait_ms(10000);

        std::cout << "\nТы останавливаешься перед следующей дверью.\n";
        std::cout << "За ней может быть выход.\n";
        std::cout << "А может быть ещё один коридор.\n";
        wait_ms(10000);

        std::cout << "\nМарк смотрит на тебя.\n";
        std::cout << "МАРК: «Готов?»\n";
        wait_ms(10000);

        std::cout << "\nТы кладёшь руку на дверную ручку.\n";
        std::cout << "И впервые сам решаешь, что делать дальше.\n";
        wait_ms(12000);

        std::cout << "\n==============================================================\n";
        std::cout << "                    КОНЕЦ ИГРЫ\n";
        std::cout << "==============================================================\n";

        wait_ms(10000);
    }
}
// Сцена: Отойти от двери и поговорить с Марком.    концовка
void show_ch04_mark_step_away_and_talk()
{
    clear_screen();

    std::cout << "ГЛАВА 4 — ПОСЛЕДНИЙ СВИДЕТЕЛЬ\n";
    std::cout << "==============================================================\n\n";

    std::cout << "Ты отходишь от двери.\n\n";

    wait_ms(8000);

    std::cout << "Всего несколько секунд назад тебе казалось, что за ней находится ответ.\n";
    std::cout << "Что если открыть её, то всё наконец станет понятно.\n";
    std::cout << "Кто такой Марк. Что означает MEMORY RESTORE. Почему ECHO-7\n";
    std::cout << "продолжает обращаться к тебе так, словно ты уже бывал здесь.\n\n";

    wait_ms(10000);

    std::cout << "Но теперь ты не уверен даже в том, что хочешь узнать правду.\n\n";

    wait_ms(8000);

    std::cout << "Марк стоит у стены напротив двери.\n";
    std::cout << "Он не смотрит на тебя. Его взгляд прикован к маленькой камере\n";
    std::cout << "под потолком.\n\n";

    wait_ms(8000);

    std::cout << "— Ты правильно сделал, что отошёл, — произносит он.\n\n";

    wait_ms(7000);

    std::cout << "— Почему?\n\n";

    wait_ms(7000);

    std::cout << "Марк медленно поворачивает голову.\n\n";

    wait_ms(8000);

    std::cout << "— Потому что эта дверь не ведёт туда, куда ты думаешь.\n";
    std::cout << "Она ведёт туда, куда система хочет, чтобы ты попал.\n\n";

    wait_ms(10000);

    std::cout << "Ты слышишь тихий щелчок.\n";
    std::cout << "Камера над вами перестаёт двигаться.\n\n";

    wait_ms(8000);

    std::cout << "— Она нас слышит? — спрашиваешь ты.\n\n";

    wait_ms(7000);

    std::cout << "— Она записывает всё, — отвечает Марк. — Разговоры.\n";
    std::cout << "Движения. Реакции. Даже то, сколько времени ты молчишь,\n";
    std::cout << "прежде чем ответить на вопрос.\n\n";

    wait_ms(10000);

    std::cout << "Он подходит ближе и понижает голос.\n\n";

    wait_ms(7000);

    std::cout << "— Но самое интересное не это.\n";
    std::cout << "Она записывает не только то, что ты делаешь.\n";
    std::cout << "Она пытается понять, что ты сделаешь в следующий раз.\n\n";

    wait_ms(11000);

    std::cout << "В следующий раз.\n\n";

    wait_ms(8000);

    std::cout << "Эти слова цепляются за сознание.\n";
    std::cout << "Не потому, что ты понимаешь их значение.\n";
    std::cout << "А потому, что где-то глубоко внутри они кажутся знакомыми.\n\n";

    wait_ms(10000);

    std::cout << "==============================================================\n";
    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Спросить Марка, что он знает о прошлых циклах.       выбрать 1\n";
    std::cout << "Спросить, кем ты был до стирания памяти.             выбрать 2\n";
    std::cout << "Потребовать объяснить, почему он помогает тебе.      выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_talk_to_mark_first)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_talk_to_mark_first = 0;
        }

        if (choice_ch04_talk_to_mark_first < 1 ||
            choice_ch04_talk_to_mark_first > 3) {
            std::cout << "Некорректный выбор. Введите число от 1 до 3: ";
        }
    } while (choice_ch04_talk_to_mark_first < 1 ||
        choice_ch04_talk_to_mark_first > 3);

    clear_screen();

    if (choice_ch04_talk_to_mark_first == 1)
    {
        std::cout << "ПРОШЛЫЕ ЦИКЛЫ\n\n";

        wait_ms(8000);

        std::cout << "— Ты сказал «в следующий раз», — говоришь ты.\n";
        std::cout << "— Что это значит? Сколько раз мы уже проходили через это?\n\n";

        wait_ms(9000);

        std::cout << "Марк закрывает глаза.\n";
        std::cout << "На мгновение он выглядит не испуганным, а измученным.\n";
        std::cout << "Так выглядит человек, который слишком долго ждал,\n";
        std::cout << "что однажды всё закончится.\n\n";

        wait_ms(10000);

        std::cout << "— Я не знаю точного числа.\n";
        std::cout << "После каждого восстановления часть моих воспоминаний\n";
        std::cout << "исчезает. Иногда я забываю лица. Иногда имена.\n";
        std::cout << "Однажды я забыл, зачем вообще пытаюсь тебе помочь.\n\n";

        wait_ms(11000);

        std::cout << "— Но потом вспомнил?\n\n";

        wait_ms(8000);

        std::cout << "— Нет. Я просто снова начал это делать.\n\n";

        wait_ms(10000);

        std::cout << "Ты смотришь на него, не зная, что ответить.\n";
        std::cout << "В этой фразе есть что-то страшнее любого признания.\n";
        std::cout << "Марк даже не уверен, что его собственные поступки принадлежат ему.\n\n";

        wait_ms(11000);
    }
    else if (choice_ch04_talk_to_mark_first == 2)
    {
        std::cout << "ТВОЁ НАСТОЯЩЕЕ ИМЯ\n\n";

        wait_ms(8000);

        std::cout << "— Кем я был до всего этого? — спрашиваешь ты.\n";
        std::cout << "— Не называй меня Субъектом 01. Я хочу знать, кто я.\n\n";

        wait_ms(9000);

        std::cout << "Марк долго молчит.\n";
        std::cout << "Ты замечаешь, как напрягаются его пальцы.\n";
        std::cout << "Он словно борется с желанием сказать что-то,\n";
        std::cout << "что уже однажды разрушило всё между вами.\n\n";

        wait_ms(10000);

        std::cout << "— Я знал тебя до первого восстановления, — наконец говорит он.\n";
        std::cout << "— По крайней мере, я думаю, что знал.\n\n";

        wait_ms(8000);

        std::cout << "— Думаешь?\n\n";

        wait_ms(7000);

        std::cout << "— У меня есть воспоминания о тебе. Но я больше не могу\n";
        std::cout << "доказать, что они настоящие. Некоторые из них появляются\n";
        std::cout << "только после того, как ECHO-7 произносит определённые слова.\n\n";

        wait_ms(10000);

        std::cout << "Марк смотрит тебе прямо в глаза.\n\n";

        wait_ms(8000);

        std::cout << "— Я помню, как ты попросил меня не позволять им\n";
        std::cout << "возвращать тебя сюда.\n\n";

        wait_ms(10000);

        std::cout << "— И ты позволил?\n\n";

        wait_ms(8000);

        std::cout << "Он не отвечает.\n";
        std::cout << "И этот ответ оказывается хуже любого признания.\n\n";

        wait_ms(11000);
    }
    else
    {
        std::cout << "ПОЧЕМУ ТЫ МНЕ ПОМОГАЕШЬ?\n\n";

        wait_ms(8000);

        std::cout << "— Хватит загадок, Марк.\n";
        std::cout << "Ты знаешь больше, чем говоришь.\n";
        std::cout << "Почему ты помогаешь мне? Чего ты хочешь?\n\n";

        wait_ms(9000);

        std::cout << "Марк отступает на полшага.\n";
        std::cout << "Ты впервые видишь, как он смотрит на тебя с настоящей тревогой.\n\n";

        wait_ms(8000);

        std::cout << "— Потому что в прошлый раз я не помог тебе.\n\n";

        wait_ms(9000);

        std::cout << "— Что произошло в прошлый раз?\n\n";

        wait_ms(8000);

        std::cout << "— Ты доверился мне. Я сказал, что знаю выход.\n";
        std::cout << "Я убедил тебя сделать выбор, который казался единственным правильным.\n";
        std::cout << "А потом система забрала тебя.\n\n";

        wait_ms(11000);

        std::cout << "Он опускает взгляд.\n\n";

        wait_ms(8000);

        std::cout << "— Я не знаю, сколько раз я пытался исправить это.\n";
        std::cout << "Но каждый раз заканчивается одинаково.\n";
        std::cout << "Я помню достаточно, чтобы снова попытаться.\n";
        std::cout << "И недостаточно, чтобы не повторить ошибку.\n\n";

        wait_ms(11000);
    }

    std::cout << "Марк проводит рукой по лицу и прислоняется к стене.\n";
    std::cout << "Несколько секунд он выглядит так, будто вот-вот упадёт.\n\n";

    wait_ms(9000);

    std::cout << "— Есть ещё кое-что, — говорит он.\n";
    std::cout << "— Я не хотел рассказывать тебе об этом, пока не убедился,\n";
    std::cout << "что ты действительно готов услышать.\n\n";

    wait_ms(10000);

    std::cout << "В коридоре гаснет одна лампа.\n";
    std::cout << "Затем вторая.\n";
    std::cout << "Свет остаётся только над вами.\n\n";

    wait_ms(9000);

    std::cout << "Из динамика доносится голос ECHO-7.\n\n";

    wait_ms(8000);

    std::cout << "— Внимание. Зафиксировано отклонение от установленного маршрута.\n";
    std::cout << "Субъект 01 вступил в незапланированный диалог.\n";
    std::cout << "Вероятность нарушения протокола восстановления увеличивается.\n\n";

    wait_ms(11000);

    std::cout << "Марк резко поднимает голову.\n\n";

    wait_ms(7000);

    std::cout << "— Не отвечай ему, — шепчет он.\n\n";

    wait_ms(8000);

    std::cout << "— Почему?\n\n";

    wait_ms(7000);

    std::cout << "— Потому что он сейчас попробует заставить тебя усомниться\n";
    std::cout << "в том, что ты видишь перед собой.\n\n";

    wait_ms(9000);

    std::cout << "Динамик потрескивает.\n\n";

    wait_ms(8000);

    std::cout << "— Субъект 01, напоминаем: идентификация объекта «Марк»\n";
    std::cout << "не подтверждена. Неизвестный объект может использовать\n";
    std::cout << "ложные воспоминания для воздействия на ваше поведение.\n\n";

    wait_ms(11000);

    std::cout << "Марк усмехается, но в его улыбке нет ни капли веселья.\n\n";

    wait_ms(8000);

    std::cout << "— Вот видишь? Он всегда начинает с этого.\n";
    std::cout << "Сначала говорит, что я ненастоящий.\n";
    std::cout << "Потом предлагает тебе проверить мои слова.\n";
    std::cout << "А когда ты начинаешь сомневаться, он стирает всё,\n";
    std::cout << "что могло бы помочь тебе вспомнить правду.\n\n";

    wait_ms(11000);

    std::cout << "— А если он прав? — тихо спрашиваешь ты.\n\n";

    wait_ms(9000);

    std::cout << "Марк не злится.\n";
    std::cout << "Он словно ожидал именно этого вопроса.\n\n";

    wait_ms(8000);

    std::cout << "— Тогда тебе придётся решить, кому ты готов поверить.\n";
    std::cout << "Не потому, что один из нас обязательно говорит правду.\n";
    std::cout << "А потому, что здесь даже правда может быть частью ловушки.\n\n";

    wait_ms(11000);

    std::cout << "Он достаёт из кармана небольшой металлический предмет.\n";
    std::cout << "На его поверхности видны царапины и выцветшая маркировка.\n";
    std::cout << "Это старый пропуск. Большая часть надписи стёрта,\n";
    std::cout << "но внизу ещё можно разобрать несколько символов:\n\n";

    wait_ms(9000);

    std::cout << "MEMORY RESTORE — ACCESS RECORD 01\n\n";

    wait_ms(10000);

    std::cout << "Марк протягивает пропуск тебе.\n\n";

    wait_ms(8000);

    std::cout << "— Я нашёл его после одного из восстановлений.\n";
    std::cout << "На нём нет моего имени. Нет имени начальника лаборатории.\n";
    std::cout << "Только номер субъекта и отметка о доступе.\n\n";

    wait_ms(10000);

    std::cout << "Ты переворачиваешь пропуск.\n";
    std::cout << "На обратной стороне кто-то нацарапал короткую фразу:\n\n";

    wait_ms(8000);

    std::cout << "«ЕСЛИ ТЫ ЭТО ЧИТАЕШЬ, ЗНАЧИТ, МАРК СНОВА НЕ СПРАВИЛСЯ».\n\n";

    wait_ms(12000);

    std::cout << "Ты поднимаешь глаза.\n";
    std::cout << "Марк смотрит на надпись так, словно видит её впервые.\n";
    std::cout << "Или пытается убедить тебя в этом.\n\n";

    wait_ms(10000);

    std::cout << "==============================================================\n";
    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Поверить Марку и попросить рассказать всё.          выбрать 1\n";
    std::cout << "Потребовать от ECHO-7 проверить пропуск.             выбрать 2\n";
    std::cout << "Спрятать пропуск и ничего не говорить Марку.         выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_talk_to_mark_truth)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_talk_to_mark_truth = 0;
        }

        if (choice_ch04_talk_to_mark_truth < 1 ||
            choice_ch04_talk_to_mark_truth > 3) {
            std::cout << "Некорректный выбор. Введите число от 1 до 3: ";
        }
    } while (choice_ch04_talk_to_mark_truth < 1 ||
        choice_ch04_talk_to_mark_truth > 3);

    clear_screen();

    if (choice_ch04_talk_to_mark_truth == 1)
    {
        std::cout << "ПОСЛЕДНЯЯ ПОПЫТКА МАРКА\n\n";

        wait_ms(9000);

        std::cout << "— Расскажи всё, — говоришь ты.\n";
        std::cout << "— Даже если я не смогу тебе поверить.\n\n";

        wait_ms(9000);

        std::cout << "Марк медленно кивает.\n\n";

        wait_ms(8000);

        std::cout << "— Тогда слушай. Я не знаю, что находится за этой дверью.\n";
        std::cout << "Я не знаю, существует ли настоящий выход.\n";
        std::cout << "Но я знаю, что каждый раз, когда ты приближаешься к ней,\n";
        std::cout << "система начинает вести себя иначе.\n\n";

        wait_ms(11000);

        std::cout << "— Она боится, что я открою её?\n\n";

        wait_ms(8000);

        std::cout << "— Может быть. А может, именно этого она и добивается.\n";
        std::cout << "Она умеет использовать наши желания против нас.\n";
        std::cout << "Если ты отчаянно хочешь выбраться, она покажет тебе выход.\n";
        std::cout << "Если хочешь вспомнить прошлое, она покажет тебе воспоминания.\n";
        std::cout << "Но ты никогда не знаешь, какую цену придётся заплатить.\n\n";

        wait_ms(12000);

        std::cout << "Марк кладёт руку тебе на плечо.\n\n";

        wait_ms(8000);

        std::cout << "— Я не прошу тебя доверять мне безоговорочно.\n";
        std::cout << "Я прошу только об одном: если я снова начну говорить,\n";
        std::cout << "что всё под контролем, не верь мне сразу.\n\n";

        wait_ms(10000);

        std::cout << "Впервые за всё время его слова звучат искренне.\n";
        std::cout << "Но именно это пугает сильнее всего.\n\n";

        wait_ms(10000);
    }
    else if (choice_ch04_talk_to_mark_truth == 2)
    {
        std::cout << "ПРОВЕРКА ECHO-7\n\n";

        wait_ms(9000);

        std::cout << "— ECHO-7, проверь пропуск, — произносишь ты.\n";
        std::cout << "— Назови его происхождение и владельца.\n\n";

        wait_ms(9000);

        std::cout << "Несколько секунд ничего не происходит.\n";
        std::cout << "Затем на ближайшем терминале загорается экран.\n\n";

        wait_ms(8000);

        std::cout << "ECHO-7: «ЗАПРОС ПРИНЯТ».\n\n";

        wait_ms(7000);

        std::cout << "ECHO-7: «НОСИТЕЛЬ СООТВЕТСТВУЕТ ВНУТРЕННЕМУ ФОРМАТУ ДОСТУПА».\n";
        std::cout << "ECHO-7: «ПОЛНЫЕ ДАННЫЕ ЗАБЛОКИРОВАНЫ».\n\n";

        wait_ms(10000);

        std::cout << "— Значит, пропуск настоящий, — говоришь ты.\n\n";

        wait_ms(8000);

        std::cout << "ECHO-7: «ПОДТВЕРЖДЕНИЕ НЕВОЗМОЖНО».\n";
        std::cout << "ECHO-7: «ОБЪЕКТ МОЖЕТ БЫТЬ КОПИЕЙ, ПОДДЕЛКОЙ ИЛИ\n";
        std::cout << "ЭЛЕМЕНТОМ СЦЕНАРИЯ ВОССТАНОВЛЕНИЯ».\n\n";

        wait_ms(11000);

        std::cout << "— Ты только что сказал, что формат соответствует системе!\n\n";

        wait_ms(8000);

        std::cout << "ECHO-7: «ПРОТИВОРЕЧИЕ НЕ ОБНАРУЖЕНО».\n\n";

        wait_ms(9000);

        std::cout << "Экран гаснет.\n";
        std::cout << "Ты понимаешь, что система не обязательно лжёт.\n";
        std::cout << "Она просто отвечает так, чтобы ты не смог сделать вывод.\n\n";

        wait_ms(11000);

        std::cout << "Марк смотрит на тебя.\n\n";

        wait_ms(7000);

        std::cout << "— Теперь понимаешь, почему я не прошу тебя верить ей?\n\n";

        wait_ms(10000);
    }
    else
    {
        std::cout << "МОЛЧАЛИВОЕ НЕДОВЕРИЕ\n\n";

        wait_ms(9000);

        std::cout << "Ты незаметно прячешь пропуск.\n";
        std::cout << "Марк ничего не говорит, но ты замечаешь,\n";
        std::cout << "как его взгляд на мгновение опускается к твоей руке.\n\n";

        wait_ms(9000);

        std::cout << "— Что теперь? — спрашивает он.\n\n";

        wait_ms(7000);

        std::cout << "— Теперь мы продолжим идти.\n";
        std::cout << "Но я больше не буду рассказывать тебе, что думаю.\n\n";

        wait_ms(9000);

        std::cout << "Марк медленно отходит от стены.\n\n";

        wait_ms(8000);

        std::cout << "— Возможно, это разумно.\n";
        std::cout << "Я бы тоже не доверял человеку, который утверждает,\n";
        std::cout << "что знает тебя, когда ты сам себя не помнишь.\n\n";

        wait_ms(10000);

        std::cout << "Ты ожидаешь, что он попытается оправдаться.\n";
        std::cout << "Но Марк лишь отворачивается и смотрит в тёмный коридор.\n\n";

        wait_ms(9000);

        std::cout << "И вдруг ты понимаешь, что не знаешь,\n";
        std::cout << "чего он боится больше: того, что ты ему не поверишь,\n";
        std::cout << "или того, что однажды всё-таки поверишь.\n\n";

        wait_ms(11000);
    }

    std::cout << "==============================================================\n";
    std::cout << "ПОСЛЕДНИЙ РАЗГОВОР\n";
    std::cout << "==============================================================\n\n";

    wait_ms(9000);

    std::cout << "Коридор снова освещается.\n";
    std::cout << "Лампы включаются одна за другой, словно ничего не произошло.\n";
    std::cout << "На терминале появляется зелёная строка:\n\n";

    wait_ms(9000);

    std::cout << "ПРОТОКОЛ MEMORY RESTORE: ОЖИДАНИЕ ПОДТВЕРЖДЕНИЯ\n\n";

    wait_ms(10000);

    std::cout << "Марк читает надпись и бледнеет.\n\n";

    wait_ms(8000);

    std::cout << "— Нет. Не сейчас.\n\n";

    wait_ms(8000);

    std::cout << "— Что такое MEMORY RESTORE? — спрашиваешь ты.\n\n";

    wait_ms(8000);

    std::cout << "Марк медленно поворачивается к тебе.\n\n";

    wait_ms(8000);

    std::cout << "— Это не просто стирание памяти.\n";
    std::cout << "Стирание — только часть процесса.\n";
    std::cout << "Система возвращает тебя в состояние, которое считает правильным.\n";
    std::cout << "Она меняет твои реакции, убирает ненужные воспоминания,\n";
    std::cout << "а иногда заставляет тебя забыть, что ты вообще что-то потерял.\n\n";

    wait_ms(12000);

    std::cout << "Ты пытаешься вспомнить своё прошлое.\n";
    std::cout << "Дом. Людей. Голоса.\n";
    std::cout << "Но вместо них в голове возникает белый шум.\n\n";

    wait_ms(10000);

    std::cout << "— А что будет с тобой? — спрашиваешь ты.\n\n";

    wait_ms(8000);

    std::cout << "Марк не отвечает сразу.\n";
    std::cout << "Он смотрит на свои руки, словно пытается убедиться,\n";
    std::cout << "что они всё ещё принадлежат ему.\n\n";

    wait_ms(10000);

    std::cout << "— Я не знаю, — произносит он.\n";
    std::cout << "— Может быть, я останусь здесь.\n";
    std::cout << "Может быть, меня сотрут вместе с твоими воспоминаниями.\n";
    std::cout << "А может быть, я вообще никогда не был тем, кем себя считаю.\n\n";

    wait_ms(12000);

    std::cout << "В динамиках раздаётся короткий сигнал.\n\n";

    wait_ms(8000);

    std::cout << "ECHO-7: «СУБЪЕКТ 01. ПОДГОТОВКА К ВОССТАНОВЛЕНИЮ ЗАВЕРШЕНА».\n";
    std::cout << "ECHO-7: «ПОЖАЛУЙСТА, ОСТАНЬТЕСЬ НА МЕСТЕ».\n\n";

    wait_ms(10000);

    std::cout << "Марк хватает тебя за руку.\n\n";

    wait_ms(7000);

    std::cout << "— Послушай меня очень внимательно.\n";
    std::cout << "Скоро ты можешь забыть этот разговор.\n";
    std::cout << "Можешь забыть меня. Можешь забыть, что когда-либо\n";
    std::cout << "сомневался в системе.\n\n";

    wait_ms(11000);

    std::cout << "Он сжимает твою руку сильнее.\n\n";

    wait_ms(8000);

    std::cout << "— Если однажды ты снова встретишь меня, не спрашивай,\n";
    std::cout << "помню ли я тебя. Спроси, чего я боюсь.\n\n";

    wait_ms(10000);

    std::cout << "— Почему именно это?\n\n";

    wait_ms(8000);

    std::cout << "— Потому что память можно подделать.\n";
    std::cout << "Имя можно стереть.\n";
    std::cout << "Лицо можно забыть.\n";
    std::cout << "Но иногда страх остаётся даже тогда,\n";
    std::cout << "когда ты уже не помнишь, что его вызвало.\n\n";

    wait_ms(12000);

    std::cout << "На терминале появляется новая надпись:\n\n";

    wait_ms(8000);

    std::cout << "ОБНАРУЖЕНА НЕСТАБИЛЬНОСТЬ ВОСПОМИНАНИЙ.\n";
    std::cout << "ПОДТВЕРДИТЬ MEMORY RESTORE?\n\n";

    wait_ms(9000);

    std::cout << "Марк смотрит на тебя.\n";
    std::cout << "Впервые он не пытается подсказать, что делать.\n";
    std::cout << "Не пытается остановить тебя.\n";
    std::cout << "Он просто ждёт твоего решения.\n\n";

    wait_ms(11000);

    std::cout << "— Что бы ты ни выбрал, — говорит он, — это будет твой выбор.\n";
    std::cout << "По крайней мере, я хочу верить, что хотя бы сейчас это так.\n\n";

    wait_ms(10000);

    std::cout << "==============================================================\n";
    std::cout << "СДЕЛАЙ ВЫБОР:\n";
    std::cout << "==============================================================\n";
    std::cout << "Отказаться от восстановления и остаться с Марком.   выбрать 1\n";
    std::cout << "Согласиться на восстановление памяти.                выбрать 2\n";
    std::cout << "Попытаться отключить терминал и скрыться.             выбрать 3\n";
    std::cout << "==============================================================\n";

    do {
        if (!(std::cin >> choice_ch04_talk_to_mark_final)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n');
            choice_ch04_talk_to_mark_final = 0;
        }

        if (choice_ch04_talk_to_mark_final < 1 ||
            choice_ch04_talk_to_mark_final > 3) {
            std::cout << "Некорректный выбор. Введите число от 1 до 3: ";
        }
    } while (choice_ch04_talk_to_mark_final < 1 ||
        choice_ch04_talk_to_mark_final > 3);

    clear_screen();

    if (choice_ch04_talk_to_mark_final == 1)
    {
        std::cout << "КОНЦОВКА — ПОСЛЕДНИЙ СВИДЕТЕЛЬ\n\n";

        wait_ms(10000);

        std::cout << "Ты отступаешь от терминала.\n";
        std::cout << "— Нет. Я не позволю тебе снова решать за меня.\n\n";

        wait_ms(9000);

        std::cout << "Марк смотрит на тебя так, словно впервые за долгое время\n";
        std::cout << "позволил себе надеяться.\n\n";

        wait_ms(9000);

        std::cout << "Терминал начинает издавать непрерывный сигнал.\n";
        std::cout << "На экране одна за другой появляются красные строки.\n\n";

        wait_ms(9000);

        std::cout << "ECHO-7: «ОТКАЗ ОТ ВОССТАНОВЛЕНИЯ ЗАРЕГИСТРИРОВАН».\n";
        std::cout << "ECHO-7: «ПРОТОКОЛ КОРРЕКЦИИ БУДЕТ ВЫПОЛНЕН ПРИНУДИТЕЛЬНО».\n\n";

        wait_ms(11000);

        std::cout << "— Беги! — кричит Марк.\n\n";

        wait_ms(7000);

        std::cout << "Но ты не успеваешь сделать и шага.\n";
        std::cout << "Из стен выдвигаются металлические фиксаторы.\n";
        std::cout << "Свет гаснет. Воздух наполняется низким гулом.\n\n";

        wait_ms(10000);

        std::cout << "Марк бросается к тебе, но между вами опускается\n";
        std::cout << "прозрачная перегородка.\n";
        std::cout << "Он ударяет по ней ладонью.\n";
        std::cout << "Ты видишь, как он кричит твоё имя.\n\n";

        wait_ms(11000);

        std::cout << "Ты пытаешься услышать его голос.\n";
        std::cout << "Но звук постепенно исчезает.\n";
        std::cout << "Вместе с ним исчезает и страх.\n\n";

        wait_ms(10000);

        std::cout << "Последнее, что ты видишь перед собой, — лицо Марка.\n";
        std::cout << "Он не выглядит человеком, который потерпел поражение.\n";
        std::cout << "Он выглядит человеком, который уже знает, что будет дальше.\n\n";

        wait_ms(12000);

        std::cout << "ECHO-7: «КОРРЕКЦИЯ ЗАВЕРШЕНА».\n\n";

        wait_ms(9000);

        std::cout << "ECHO-7: «Субъект 01, пожалуйста, откройте глаза».\n\n";

        wait_ms(10000);

        std::cout << "Ты открываешь глаза.\n";
        std::cout << "Белый потолок.\n";
        std::cout << "Холодный металлический стол.\n";
        std::cout << "Тихий гул вентиляции.\n\n";

        wait_ms(10000);

        std::cout << "Ты не помнишь, как оказался здесь.\n";
        std::cout << "Не помнишь дверь.\n";
        std::cout << "Не помнишь пропуск.\n";
        std::cout << "Не помнишь человека по имени Марк.\n\n";

        wait_ms(11000);

        std::cout << "На стене напротив тебя загорается экран.\n\n";

        wait_ms(8000);

        std::cout << "ECHO-7: «Добро пожаловать обратно, Субъект 01».\n\n";

        wait_ms(12000);

        std::cout << "Ты пытаешься ответить.\n";
        std::cout << "Но не знаешь, какой вопрос хотел задать.\n\n";

        wait_ms(10000);

        std::cout << "КОНЕЦ.\n";
        std::cout << "==============================================================\n";
    }
    else if (choice_ch04_talk_to_mark_final == 2)
    {
        std::cout << "КОНЦОВКА — ИДЕАЛЬНАЯ ПАМЯТЬ\n\n";

        wait_ms(10000);

        std::cout << "Ты подходишь к терминалу.\n";
        std::cout << "Палец зависает над кнопкой подтверждения.\n\n";

        wait_ms(9000);

        std::cout << "— Если я соглашусь, я вспомню всё?\n\n";

        wait_ms(8000);

        std::cout << "ECHO-7: «ВОССТАНОВЛЕНИЕ ОБЕСПЕЧИТ ВОЗВРАТ К СТАБИЛЬНОМУ\n";
        std::cout << "СОСТОЯНИЮ. ДОПОЛНИТЕЛЬНЫЕ ВОСПОМИНАНИЯ НЕ ГАРАНТИРУЮТСЯ».\n\n";

        wait_ms(10000);

        std::cout << "Марк медленно отступает.\n\n";

        wait_ms(8000);

        std::cout << "— Не нажимай. Оно даже не обещает вернуть тебе память.\n";
        std::cout << "Оно обещает сделать тебя удобным для себя.\n\n";

        wait_ms(10000);

        std::cout << "Ты смотришь на него, потом на терминал.\n";
        std::cout << "Тебе хочется вспомнить всё.\n";
        std::cout << "Хочется узнать, почему ты здесь.\n";
        std::cout << "Хочется прекратить жить среди чужих намёков и обрывков прошлого.\n\n";

        wait_ms(11000);

        std::cout << "Ты нажимаешь кнопку.\n\n";

        wait_ms(10000);

        std::cout << "Экран становится белым.\n";
        std::cout << "Гул вентиляции усиливается, а затем исчезает.\n";
        std::cout << "На несколько секунд весь мир словно замирает.\n\n";

        wait_ms(11000);

        std::cout << "Перед глазами вспыхивают образы.\n";
        std::cout << "Длинный коридор.\n";
        std::cout << "Чьи-то руки на стекле.\n";
        std::cout << "Марк, стоящий по другую сторону двери.\n";
        std::cout << "Голос, который просит тебя не нажимать кнопку.\n\n";

        wait_ms(12000);

        std::cout << "Затем всё исчезает.\n\n";

        wait_ms(9000);

        std::cout << "ECHO-7: «ВОССТАНОВЛЕНИЕ ЗАВЕРШЕНО».\n";
        std::cout << "ECHO-7: «КОГНИТИВНАЯ СТАБИЛЬНОСТЬ В НОРМЕ».\n\n";

        wait_ms(10000);

        std::cout << "Ты садишься на металлическом столе.\n";
        std::cout << "В голове чисто и спокойно.\n";
        std::cout << "Нет страха. Нет боли. Нет мучительных вопросов.\n\n";

        wait_ms(10000);

        std::cout << "Ты вспоминаешь своё имя.\n";
        std::cout << "Вспоминаешь, как говорить.\n";
        std::cout << "Вспоминаешь, что такое дом.\n";
        std::cout << "Но когда пытаешься вспомнить Марка, в памяти остаётся пустота.\n\n";

        wait_ms(11000);

        std::cout << "Ты спускаешь ноги со стола и встаёшь.\n";
        std::cout << "Дверь перед тобой открывается.\n";
        std::cout << "За ней находится знакомый коридор.\n\n";

        wait_ms(10000);

        std::cout << "На стене висит табличка:\n\n";

        wait_ms(8000);

        std::cout << "ОТДЕЛ КОГНИТИВНОЙ СТАБИЛИЗАЦИИ\n";
        std::cout << "СУБЪЕКТ 01 — УСПЕШНО ВОССТАНОВЛЕН\n\n";

        wait_ms(10000);

        std::cout << "Ты проходишь мимо камеры.\n";
        std::cout << "Она поворачивается вслед за тобой.\n\n";

        wait_ms(9000);

        std::cout << "ECHO-7: «Добро пожаловать обратно».\n\n";

        wait_ms(10000);

        std::cout << "Ты улыбаешься.\n";
        std::cout << "Почему-то тебе кажется, что всё наконец стало правильно.\n\n";

        wait_ms(12000);

        std::cout << "Ты не замечаешь маленькую надпись на обратной стороне двери.\n";
        std::cout << "Надпись, сделанную чьей-то дрожащей рукой:\n\n";

        wait_ms(9000);

        std::cout << "«НЕ ПОЗВОЛЯЙ ЕМУ ВЕРНУТЬ ТЕБЯ».\n\n";

        wait_ms(12000);

        std::cout << "КОНЕЦ.\n";
        std::cout << "==============================================================\n";
    }
    else
    {
        std::cout << "КОНЦОВКА — ЗА ПРЕДЕЛАМИ ПРОТОКОЛА\n\n";

        wait_ms(10000);

        std::cout << "Ты резко ударяешь по панели управления.\n";
        std::cout << "Экран гаснет, а терминал издаёт короткий треск.\n\n";

        wait_ms(9000);

        std::cout << "— Что ты наделал?! — кричит Марк.\n\n";

        wait_ms(8000);

        std::cout << "— Я не собираюсь позволять этой штуке решать за меня!\n\n";

        wait_ms(9000);

        std::cout << "Ты хватаешь Марка за руку и бросаешься в тёмный коридор.\n";
        std::cout << "Сзади включается аварийное освещение.\n";
        std::cout << "Сирена разрывает тишину.\n\n";

        wait_ms(10000);

        std::cout << "ECHO-7: «НАРУШЕНИЕ ПРОТОКОЛА».\n";
        std::cout << "ECHO-7: «СУБЪЕКТ 01, ОСТАНОВИТЕСЬ».\n\n";

        wait_ms(9000);

        std::cout << "Ты не останавливаешься.\n";
        std::cout << "Ты не знаешь, куда бежать.\n";
        std::cout << "Но впервые за долгое время ты не пытаешься найти\n";
        std::cout << "правильный путь. Ты просто выбираешь сам.\n\n";

        wait_ms(11000);

        std::cout << "Марк ведёт тебя к узкой технической двери.\n";
        std::cout << "За ней слышен гул огромных механизмов.\n";
        std::cout << "На стене мигает предупреждение о запрете доступа.\n\n";

        wait_ms(10000);

        std::cout << "— Дальше я не знаю, что будет, — говорит Марк.\n\n";

        wait_ms(8000);

        std::cout << "— Ты ведь говорил, что уже бывал здесь.\n\n";

        wait_ms(8000);

        std::cout << "— Я помню, что мы доходили до этой двери.\n";
        std::cout << "Но я не помню, что было за ней.\n\n";

        wait_ms(10000);

        std::cout << "Ты нажимаешь на ручку.\n";
        std::cout << "Дверь открывается с тяжёлым металлическим скрежетом.\n\n";

        wait_ms(9000);

        std::cout << "За ней нет выхода наружу.\n";
        std::cout << "Нет лестницы, ведущей на поверхность.\n";
        std::cout << "Нет окон и признаков внешнего мира.\n\n";

        wait_ms(10000);

        std::cout << "Перед вами находится огромный зал.\n";
        std::cout << "Вдоль стен тянутся ряды металлических капсул.\n";
        std::cout << "На каждой горит небольшой экран.\n\n";

        wait_ms(10000);

        std::cout << "На экранах отображаются номера.\n";
        std::cout << "Субъект 02.\n";
        std::cout << "Субъект 03.\n";
        std::cout << "Субъект 04.\n\n";

        wait_ms(10000);

        std::cout << "Их десятки.\n";
        std::cout << "Может быть, сотни.\n\n";

        wait_ms(10000);

        std::cout << "Ты медленно подходишь к ближайшей капсуле.\n";
        std::cout << "Внутри лежит человек.\n";
        std::cout << "Его лицо скрыто за стеклом, но на запястье виден знакомый браслет.\n\n";

        wait_ms(11000);

        std::cout << "На экране появляется строка:\n\n";

        wait_ms(8000);

        std::cout << "СУБЪЕКТ 01 — ПРЕДЫДУЩАЯ КОПИЯ\n";
        std::cout << "СТАТУС: НЕАКТИВЕН\n\n";

        wait_ms(12000);

        std::cout << "Ты отступаешь.\n";
        std::cout << "Марк перестаёт дышать на несколько секунд.\n\n";

        wait_ms(9000);

        std::cout << "— Нет... — шепчет он.\n";
        std::cout << "— Этого не должно быть.\n\n";

        wait_ms(9000);

        std::cout << "Ты смотришь на него.\n\n";

        wait_ms(8000);

        std::cout << "— Ты знал?\n\n";

        wait_ms(8000);

        std::cout << "Марк качает головой.\n";
        std::cout << "Но ты больше не можешь понять, говорит ли он правду.\n\n";

        wait_ms(10000);

        std::cout << "В глубине зала включается огромный экран.\n";
        std::cout << "На нём появляется знакомая надпись:\n\n";

        wait_ms(9000);

        std::cout << "ECHO-7 — СИСТЕМА УПРАВЛЕНИЯ ПАМЯТЬЮ\n";
        std::cout << "АКТИВНЫЙ СУБЪЕКТ: 01\n";
        std::cout << "ОТКЛОНЕНИЕ: КРИТИЧЕСКОЕ\n\n";

        wait_ms(10000);

        std::cout << "ECHO-7: «Вы покинули назначенный маршрут».\n\n";

        wait_ms(8000);

        std::cout << "Голос доносится не из динамиков.\n";
        std::cout << "Он звучит из всех капсул одновременно.\n\n";

        wait_ms(10000);

        std::cout << "ECHO-7: «Но вы ещё не покинули систему».\n\n";

        wait_ms(11000);

        std::cout << "Капсулы начинают открываться одна за другой.\n";
        std::cout << "Внутри неподвижно лежат люди.\n";
        std::cout << "Некоторые похожи на тебя.\n";
        std::cout << "Другие — на Марка.\n\n";

        wait_ms(12000);

        std::cout << "Марк хватает тебя за плечо.\n\n";

        wait_ms(8000);

        std::cout << "— Не смотри на них. Что бы ни произошло, не смотри!\n\n";

        wait_ms(9000);

        std::cout << "Но ты уже заметил одну деталь.\n";
        std::cout << "На запястье ближайшего неподвижного человека\n";
        std::cout << "написано то же имя, которое носишь ты.\n\n";

        wait_ms(11000);

        std::cout << "Свет гаснет.\n\n";

        wait_ms(10000);

        std::cout << "В темноте кто-то произносит твоим голосом:\n\n";

        wait_ms(9000);

        std::cout << "— Марк, не позволяй ему снова меня восстановить.\n\n";

        wait_ms(12000);

        std::cout << "Ты чувствуешь, как Марк отпускает твоё плечо.\n";
        std::cout << "Он больше не держит тебя.\n";
        std::cout << "Он стоит неподвижно, словно узнал голос,\n";
        std::cout << "которого боялся услышать всё это время.\n\n";

        wait_ms(11000);

        std::cout << "А затем из динамиков раздаётся спокойный голос ECHO-7:\n\n";

        wait_ms(9000);

        std::cout << "— Добро пожаловать обратно, Субъект 01.\n\n";

        wait_ms(12000);

        std::cout << "КОНЕЦ.\n";
        std::cout << "==============================================================\n";
    }

    wait_ms(12000);
    // Здесь можно вызвать следующую сцену, если она будет создана.
    // Пока концовка заканчивается здесь.
}


// ============================================================
// КОНЦОВКА — ПОСЛЕДНИЙ ВЫХОД
// ============================================================

void show_ch04_corridor_find_another_exit()
{
    clear_screen();
    flush_input();

    std::cout << "ГЛАВА 4\n";
    std::cout << "КОНЦОВКА — ПОСЛЕДНИЙ ВЫХОД\n";
    std::cout << "==============================================================\n\n";

    wait_ms(9000);

    std::cout << "Ты решаешь отойти от двери.\n\n";

    wait_ms(8000);

    std::cout << "Хватит.\n";
    std::cout << "Хватит разговоров с Марком.\n";
    std::cout << "Хватит голосов из динамиков.\n";
    std::cout << "Хватит обещаний, что ещё немного — и ты всё поймёшь.\n\n";

    wait_ms(10000);

    std::cout << "Ты больше не хочешь понимать.\n";
    std::cout << "Ты хочешь выбраться.\n\n";

    wait_ms(9000);

    std::cout << "Не ради правды.\n";
    std::cout << "Не ради мести.\n";
    std::cout << "Не ради того, чтобы доказать системе, что она ошиблась.\n\n";

    wait_ms(10000);

    std::cout << "Ты просто хочешь увидеть нечто настоящее.\n";
    std::cout << "Увидеть не белый потолок.\n";
    std::cout << "Не металлические двери.\n";
    std::cout << "Не камеры, следящие за каждым твоим движением.\n\n";

    wait_ms(11000);

    std::cout << "Ты хочешь почувствовать холодный воздух.\n";
    std::cout << "Услышать ветер.\n";
    std::cout << "Увидеть небо, которому ничего от тебя не нужно.\n\n";

    wait_ms(12000);

    std::cout << "Ты идёшь по коридору, которого раньше не замечал.\n";
    std::cout << "Он начинается за поворотом, возле старого технического шкафа.\n";
    std::cout << "Здесь нет указателей.\n";
    std::cout << "Нет камер.\n";
    std::cout << "Нет привычных белых ламп.\n\n";

    wait_ms(10000);

    std::cout << "Только несколько аварийных светильников,\n";
    std::cout << "которые с трудом пробиваются сквозь темноту.\n\n";

    wait_ms(9000);

    std::cout << "Ты останавливаешься и прислушиваешься.\n\n";

    wait_ms(10000);

    std::cout << "Впервые за долгое время здесь тихо.\n\n";

    wait_ms(10000);

    std::cout << "Не слышно вентиляции.\n";
    std::cout << "Не слышно шагов за стенами.\n";
    std::cout << "Даже ECHO-7 молчит.\n\n";

    wait_ms(12000);

    std::cout << "Ты делаешь ещё несколько шагов.\n";
    std::cout << "Под ногой что-то хрустит.\n\n";

    wait_ms(8000);

    std::cout << "Ты опускаешь взгляд.\n\n";

    wait_ms(9000);

    std::cout << "На полу лежит маленький пластиковый браслет.\n";
    std::cout << "Он потрескался от времени.\n";
    std::cout << "На внутренней стороне почти стёрлась надпись:\n\n";

    wait_ms(10000);

    std::cout << "СУБЪЕКТ 01.\n\n";

    wait_ms(12000);

    std::cout << "Ты поднимаешь браслет.\n";
    std::cout << "Он холодный.\n";
    std::cout << "Слишком холодный для вещи, которая могла недавно лежать\n";
    std::cout << "в тёплом помещении.\n\n";

    wait_ms(10000);

    std::cout << "Ты переворачиваешь его несколько раз,\n";
    std::cout << "пытаясь найти объяснение.\n\n";

    wait_ms(9000);

    std::cout << "И вдруг замечаешь на застёжке царапины.\n";
    std::cout << "Кто-то пытался разломать его.\n";
    std::cout << "Кто-то очень сильно хотел избавиться от собственного имени.\n\n";

    wait_ms(11000);

    std::cout << "Ты не понимаешь почему, но рука начинает дрожать.\n\n";

    wait_ms(10000);

    std::cout << "Из глубины коридора доносится металлический скрежет.\n\n";

    wait_ms(9000);

    std::cout << "Ты оборачиваешься.\n";
    std::cout << "В конце прохода медленно открывается дверь.\n";
    std::cout << "За ней не появляется свет.\n";
    std::cout << "Только темнота.\n\n";

    wait_ms(10000);

    std::cout << "На двери висит табличка, покрытая пылью.\n\n";

    wait_ms(9000);

    std::cout << "АРХИВ УТИЛИЗАЦИИ.\n";
    std::cout << "ДОСТУП ЗАПРЕЩЁН.\n\n";

    wait_ms(11000);

    std::cout << "Ты снова смотришь на браслет.\n";
    std::cout << "Затем на открытую дверь.\n\n";

    wait_ms(9000);

    std::cout << "Ты знаешь, что не должен туда идти.\n";
    std::cout << "Каждый инстинкт говорит тебе вернуться.\n";
    std::cout << "Позвать Марка.\n";
    std::cout << "Найти работающий терминал.\n";
    std::cout << "Сделать хоть что-нибудь, что кажется разумным.\n\n";

    wait_ms(11000);

    std::cout << "Но ты устал доверять разуму,\n";
    std::cout << "который не помнит даже собственного прошлого.\n\n";

    wait_ms(10000);

    std::cout << "Ты входишь.\n\n";

    wait_ms(14000);

    clear_screen();

    std::cout << "АРХИВ УТИЛИЗАЦИИ\n";
    std::cout << "==============================================================\n\n";

    wait_ms(10000);

    std::cout << "Первое, что ты чувствуешь, — запах.\n\n";

    wait_ms(9000);

    std::cout << "Сырость.\n";
    std::cout << "Пыль.\n";
    std::cout << "Старый металл.\n";
    std::cout << "И что-то ещё, от чего желудок болезненно сжимается.\n\n";

    wait_ms(11000);

    std::cout << "В помещении холоднее, чем в коридоре.\n";
    std::cout << "Твоё дыхание становится заметным в слабом свете.\n";
    std::cout << "Перед тобой тянутся длинные ряды металлических столов.\n\n";

    wait_ms(10000);

    std::cout << "На каждом лежит тело.\n\n";

    wait_ms(14000);

    std::cout << "Сначала ты пытаешься убедить себя,\n";
    std::cout << "что это манекены.\n";
    std::cout << "Учебные модели.\n";
    std::cout << "Часть какого-то медицинского эксперимента.\n\n";

    wait_ms(11000);

    std::cout << "Но у одного из тел видна рука,\n";
    std::cout << "соскользнувшая с края стола.\n";
    std::cout << "Пальцы согнуты так, словно человек до последнего\n";
    std::cout << "пытался за что-то ухватиться.\n\n";

    wait_ms(12000);

    std::cout << "Ты отступаешь.\n\n";

    wait_ms(9000);

    std::cout << "Пятка касается металлического контейнера.\n";
    std::cout << "Он опрокидывается с оглушительным грохотом.\n\n";

    wait_ms(9000);

    std::cout << "Звук прокатывается по помещению и возвращается эхом.\n";
    std::cout << "На мгновение кажется, будто кто-то ответил тебе\n";
    std::cout << "из дальнего конца зала.\n\n";

    wait_ms(11000);

    std::cout << "Но это только эхо.\n\n";

    wait_ms(10000);

    std::cout << "Ты заставляешь себя идти дальше.\n";
    std::cout << "На стенах висят таблички с номерами.\n";
    std::cout << "Некоторые перечёркнуты красной линией.\n";
    std::cout << "На других написано одно и то же слово:\n\n";

    wait_ms(9000);

    std::cout << "УТИЛИЗИРОВАН.\n\n";

    wait_ms(12000);

    std::cout << "Ты подходишь к ближайшему столу.\n";
    std::cout << "На запястье лежащего человека виден браслет.\n\n";

    wait_ms(9000);

    std::cout << "СУБЪЕКТ 01.\n\n";

    wait_ms(14000);

    std::cout << "Ты резко отворачиваешься.\n";
    std::cout << "Смотришь на следующий стол.\n\n";

    wait_ms(9000);

    std::cout << "СУБЪЕКТ 01.\n\n";

    wait_ms(12000);

    std::cout << "Ещё один.\n\n";

    wait_ms(9000);

    std::cout << "И ещё.\n\n";

    wait_ms(9000);

    std::cout << "И ещё.\n\n";

    wait_ms(12000);

    std::cout << "Ты медленно поворачиваешься вокруг себя.\n";
    std::cout << "Сколько их здесь?\n";
    std::cout << "Десять?\n";
    std::cout << "Двадцать?\n";
    std::cout << "Больше?\n\n";

    wait_ms(12000);

    std::cout << "Ты не можешь сосчитать.\n";
    std::cout << "Потому что каждый раз, когда начинаешь,\n";
    std::cout << "взгляд цепляется за новое знакомое лицо.\n\n";

    wait_ms(11000);

    std::cout << "Одинаковые черты.\n";
    std::cout << "Одинаковая форма лица.\n";
    std::cout << "Одинаковый шрам возле брови.\n\n";

    wait_ms(12000);

    std::cout << "Твоё отражение смотрит на тебя с каждого стола.\n\n";

    wait_ms(14000);

    std::cout << "Ты прижимаешь ладонь ко рту.\n";
    std::cout << "Пытаешься вдохнуть.\n";
    std::cout << "Но воздух словно застревает в груди.\n\n";

    wait_ms(11000);

    std::cout << "— Нет...\n\n";

    wait_ms(9000);

    std::cout << "Слово выходит едва слышно.\n";
    std::cout << "Ты повторяешь его, будто от этого что-то изменится.\n\n";

    wait_ms(9000);

    std::cout << "— Нет. Нет. Нет...\n\n";

    wait_ms(12000);

    std::cout << "Никто не отвечает.\n\n";

    wait_ms(12000);

    std::cout << "Ты идёшь между столами быстрее.\n";
    std::cout << "Тебе нужно найти выход.\n";
    std::cout << "Нужно доказать себе, что это не может быть правдой.\n";
    std::cout << "Что у всего этого есть другое объяснение.\n\n";

    wait_ms(11000);

    std::cout << "Ты замечаешь стенд с документами.\n";
    std::cout << "Большинство листов превратились в хрупкие серые обрывки.\n";
    std::cout << "Но один файл лежит отдельно, словно его оставили специально.\n\n";

    wait_ms(10000);

    std::cout << "На обложке написано:\n\n";

    wait_ms(9000);

    std::cout << "ПРОЕКТ ECHO-7.\n";
    std::cout << "ПОВТОРЯЮЩИЕСЯ ЦИКЛЫ.\n";
    std::cout << "ОТЧЁТ О СТАБИЛЬНОСТИ СУБЪЕКТА 01.\n\n";

    wait_ms(12000);

    std::cout << "Ты открываешь файл.\n\n";

    wait_ms(9000);

    std::cout << "Первая страница содержит таблицу.\n";
    std::cout << "Дата.\n";
    std::cout << "Номер цикла.\n";
    std::cout << "Результат восстановления.\n";
    std::cout << "Причина прекращения активности.\n\n";

    wait_ms(11000);

    std::cout << "Ты читаешь строки одну за другой.\n\n";

    wait_ms(9000);

    std::cout << "ЦИКЛ 014 — НЕУДАЧНАЯ ПОПЫТКА ПОБЕГА.\n";
    std::cout << "ЦИКЛ 027 — ОТКАЗ ОТ ВОССТАНОВЛЕНИЯ.\n";
    std::cout << "ЦИКЛ 043 — ПОТЕРЯ ОРИЕНТАЦИИ.\n";
    std::cout << "ЦИКЛ 061 — ПОЛНЫЙ ОТКАЗ ОТ СОТРУДНИЧЕСТВА.\n";
    std::cout << "ЦИКЛ 088 — ПОВТОРНАЯ ПОПЫТКА ПОИСКА МАРКА.\n\n";

    wait_ms(12000);

    std::cout << "Ты перечитываешь последнюю строку.\n\n";

    wait_ms(9000);

    std::cout << "ПОИСК МАРКА.\n\n";

    wait_ms(12000);

    std::cout << "Имя кажется знакомым.\n";
    std::cout << "Не просто знакомым — болезненным.\n";
    std::cout << "Ты словно забыл что-то настолько важное,\n";
    std::cout << "что сама попытка вспомнить причиняет боль.\n\n";

    wait_ms(12000);

    std::cout << "Ты переворачиваешь страницу.\n\n";

    wait_ms(9000);

    std::cout << "«Субъект 01 демонстрирует устойчивую привязанность\n";
    std::cout << "к образу Марка.\n";
    std::cout << "При каждой итерации субъект пытается восстановить контакт.\n";
    std::cout << "После потери контакта наблюдаются выраженные признаки\n";
    std::cout << "эмоционального истощения».\n\n";

    wait_ms(12000);

    std::cout << "Следующая строка написана другим почерком.\n\n";

    wait_ms(9000);

    std::cout << "«Он всё время думает, что сможет спасти его».\n\n";

    wait_ms(12000);

    std::cout << "Ты закрываешь глаза.\n";
    std::cout << "Внутри поднимается что-то горячее и тяжёлое.\n";
    std::cout << "Ты не знаешь, кто написал эту фразу.\n";
    std::cout << "Но ненавидишь его за то, что он оказался прав.\n\n";

    wait_ms(12000);

    std::cout << "Ты листаешь дальше.\n";
    std::cout << "Последние страницы исписаны торопливыми заметками.\n";
    std::cout << "Некоторые слова зачёркнуты.\n";
    std::cout << "Некоторые строки размазаны, словно автор плакал,\n";
    std::cout << "пока пытался закончить отчёт.\n\n";

    wait_ms(11000);

    std::cout << "На одной странице написано:\n\n";

    wait_ms(9000);

    std::cout << "«Субъект задаёт один и тот же вопрос после каждого восстановления:\n";
    std::cout << "“Где Марк?”»\n\n";

    wait_ms(12000);

    std::cout << "Ты вспоминаешь голос из динамиков.\n";
    std::cout << "Холодный ответ, который уже слышал раньше.\n\n";

    wait_ms(9000);

    std::cout << "«Какой Марк?»\n\n";

    wait_ms(14000);

    std::cout << "Ты роняешь файл.\n\n";

    wait_ms(10000);

    std::cout << "Он падает на пол, и несколько листов разлетаются\n";
    std::cout << "между металлическими столами.\n";
    std::cout << "Ты не пытаешься их поднять.\n\n";

    wait_ms(10000);

    std::cout << "Ты уже понимаешь, что должен найти.\n\n";

    wait_ms(11000);

    std::cout << "В самом дальнем конце архива стоит отдельный стол.\n";
    std::cout << "На нём нет номера.\n";
    std::cout << "Нет стандартной таблички.\n";
    std::cout << "Нет электронного браслета.\n\n";

    wait_ms(11000);

    std::cout << "Только сложенная куртка.\n\n";

    wait_ms(10000);

    std::cout << "Ты узнаёшь её раньше, чем успеваешь понять почему.\n";
    std::cout << "Тёмная ткань.\n";
    std::cout << "Порванный шов на рукаве.\n";
    std::cout << "Маленькая металлическая застёжка,\n";
    std::cout << "которую ты видел в руках Марка.\n\n";

    wait_ms(12000);

    std::cout << "Ты подходишь к столу.\n";
    std::cout << "Шаг.\n";
    std::cout << "Ещё шаг.\n";
    std::cout << "Кажется, что расстояние не сокращается.\n\n";

    wait_ms(11000);

    std::cout << "На столе лежит тело.\n\n";

    wait_ms(14000);

    std::cout << "Ты не сразу смотришь на лицо.\n";
    std::cout << "Сначала видишь руки.\n";
    std::cout << "Знакомые пальцы.\n";
    std::cout << "Знакомые шрамы.\n";
    std::cout << "Ту же старую отметину на запястье.\n\n";

    wait_ms(12000);

    std::cout << "Ты медленно поднимаешь глаза.\n\n";

    wait_ms(14000);

    std::cout << "Это Марк.\n\n";

    wait_ms(16000);

    std::cout << "Ты ждёшь, что он вдохнёт.\n";
    std::cout << "Что откроет глаза.\n";
    std::cout << "Что усмехнётся и скажет, что ты опять поверил\n";
    std::cout << "в очередную ложь системы.\n\n";

    wait_ms(12000);

    std::cout << "Ты ждёшь ещё.\n\n";

    wait_ms(12000);

    std::cout << "Ничего не происходит.\n\n";

    wait_ms(14000);

    std::cout << "Ты касаешься его плеча.\n";
    std::cout << "Его одежда холодная.\n";
    std::cout << "Ты отдёргиваешь руку, словно обжёгся.\n\n";

    wait_ms(11000);

    std::cout << "— Марк?\n\n";

    wait_ms(12000);

    std::cout << "Тишина.\n\n";

    wait_ms(12000);

    std::cout << "— Эй. Хватит.\n";
    std::cout << "Ты слышишь меня?\n";
    std::cout << "Пожалуйста, хватит.\n\n";

    wait_ms(12000);

    std::cout << "Ты берёшь его за руку.\n";
    std::cout << "Она безвольно лежит в твоей ладони.\n\n";

    wait_ms(14000);

    std::cout << "И в этот момент ты понимаешь,\n";
    std::cout << "что больше всего на свете боишься не его смерти.\n\n";

    wait_ms(11000);

    std::cout << "Ты боишься, что однажды забудешь и это.\n\n";

    wait_ms(16000);

    std::cout << "Забудешь его голос.\n";
    std::cout << "Забудешь, как он смотрел на тебя, когда пытался сказать правду.\n";
    std::cout << "Забудешь, как он предупреждал тебя не доверять системе.\n";
    std::cout << "Забудешь даже то, что сейчас тебе больно.\n\n";

    wait_ms(14000);

    std::cout << "Ты падаешь на колени возле стола.\n";
    std::cout << "Пытаешься вдохнуть, но воздух выходит короткими,\n";
    std::cout << "рваными толчками.\n";
    std::cout << "Ты не можешь остановить дрожь в руках.\n\n";

    wait_ms(12000);

    std::cout << "— Ты обещал...\n\n";

    wait_ms(10000);

    std::cout << "Ты не знаешь, какое именно обещание имеешь в виду.\n";
    std::cout << "Возможно, Марк обещал помочь тебе выбраться.\n";
    std::cout << "Возможно, ты обещал ему не забывать.\n";
    std::cout << "Возможно, это обещание было дано задолго до лаборатории.\n\n";

    wait_ms(14000);

    std::cout << "Но теперь ты не можешь спросить.\n\n";

    wait_ms(14000);

    std::cout << "Ты прижимаешь его руку ко лбу.\n";
    std::cout << "Слова больше не складываются в предложения.\n";
    std::cout << "Ты повторяешь его имя снова и снова,\n";
    std::cout << "потому что это единственное, что ещё осталось.\n\n";

    wait_ms(14000);

    std::cout << "— Марк...\n\n";

    wait_ms(12000);

    std::cout << "— Марк, пожалуйста...\n\n";

    wait_ms(12000);

    std::cout << "— Я здесь.\n";
    std::cout << "Я нашёл тебя.\n";
    std::cout << "Слышишь? Я наконец нашёл тебя.\n\n";

    wait_ms(16000);

    std::cout << "Ты пытаешься улыбнуться сквозь слёзы.\n";
    std::cout << "Словно он просто очень устал.\n";
    std::cout << "Словно сейчас откроет глаза и скажет,\n";
    std::cout << "что всё это было очередной проверкой.\n\n";

    wait_ms(14000);

    std::cout << "Но тело остаётся неподвижным.\n\n";

    wait_ms(14000);

    std::cout << "Ты впервые за всё время не просишь систему помочь.\n";
    std::cout << "Не требуешь объяснений.\n";
    std::cout << "Не ищешь кнопку, способную отменить произошедшее.\n\n";

    wait_ms(12000);

    std::cout << "Ты просто сидишь рядом с ним.\n\n";

    wait_ms(14000);

    std::cout << "И плачешь.\n\n";

    wait_ms(16000);

    std::cout << "Ты плачешь не потому, что боишься умереть.\n";
    std::cout << "Ты плачешь потому, что он умер один.\n";
    std::cout << "Пока ты ходил по коридорам, выбирал двери,\n";
    std::cout << "пытался понять, кому можно верить,\n";
    std::cout << "он уже лежал здесь.\n\n";

    wait_ms(14000);

    std::cout << "И всё же где-то внутри ты понимаешь:\n";
    std::cout << "возможно, он ждал тебя не в этом цикле.\n";
    std::cout << "Возможно, он ждал тебя очень давно.\n\n";

    wait_ms(14000);

    std::cout << "Ты осторожно поправляешь край его куртки.\n";
    std::cout << "Это бессмысленное движение.\n";
    std::cout << "Оно ничего не исправляет.\n";
    std::cout << "Но ты больше не знаешь, что ещё можешь сделать для него.\n\n";

    wait_ms(14000);

    std::cout << "Под курткой ты замечаешь маленький диктофон.\n";
    std::cout << "Его корпус поцарапан, а кнопка воспроизведения\n";
    std::cout << "почти утоплена в пластик.\n\n";

    wait_ms(11000);

    std::cout << "Ты нажимаешь её.\n\n";

    wait_ms(12000);

    std::cout << "Несколько секунд слышен только шум.\n";
    std::cout << "Затем раздаётся голос Марка.\n\n";

    wait_ms(10000);

    std::cout << "Запись звучит тихо, будто он боялся,\n";
    std::cout << "что даже диктофон может его выдать.\n\n";

    wait_ms(10000);

    std::cout << "— Если ты нашёл это, значит, я снова не смог.\n\n";

    wait_ms(12000);

    std::cout << "Ты закрываешь глаза.\n\n";

    wait_ms(9000);

    std::cout << "— Я не знаю, какой сейчас цикл.\n";
    std::cout << "Я не знаю, помнишь ли ты меня.\n";
    std::cout << "Возможно, ты уже много раз слышал этот голос.\n";
    std::cout << "Возможно, я сам оставлял тебе эту запись раньше.\n\n";

    wait_ms(12000);

    std::cout << "На записи слышится долгий вдох.\n\n";

    wait_ms(10000);

    std::cout << "— Я пытался вывести тебя отсюда.\n";
    std::cout << "Пытался найти настоящую дверь.\n";
    std::cout << "Пытался отключить систему.\n";
    std::cout << "Пытался убедить себя, что если буду достаточно долго\n";
    std::cout << "пытаться, то однажды всё обязательно получится.\n\n";

    wait_ms(14000);

    std::cout << "Голос Марка становится тише.\n\n";

    wait_ms(9000);

    std::cout << "— Но я всё время забывал одну вещь.\n";
    std::cout << "Я не мог спасти тебя, если ты не помнил, что хочешь жить.\n";
    std::cout << "Каждый раз ты возвращался сюда с новой надеждой.\n";
    std::cout << "Каждый раз ты смотрел на меня так, словно я мог всё исправить.\n\n";

    wait_ms(14000);

    std::cout << "Долгая пауза.\n\n";

    wait_ms(12000);

    std::cout << "— Прости меня за то, что я позволил тебе поверить,\n";
    std::cout << "будто у меня есть все ответы.\n";
    std::cout << "Я тоже боялся.\n";
    std::cout << "Я тоже не хотел остаться здесь.\n";
    std::cout << "И я тоже каждый раз надеялся, что в следующий раз\n";
    std::cout << "нам наконец повезёт.\n\n";

    wait_ms(14000);

    std::cout << "Ты прижимаешь диктофон к груди.\n\n";

    wait_ms(10000);

    std::cout << "— Если ты слышишь это, — продолжает Марк, — пожалуйста,\n";
    std::cout << "не трать остаток своей жизни на попытки доказать,\n";
    std::cout << "что я был прав.\n\n";

    wait_ms(12000);

    std::cout << "— Не делай меня причиной, по которой ты продолжаешь страдать.\n";
    std::cout << "Не превращай моё имя в ещё одну дверь, которую тебе нужно открыть.\n";
    std::cout << "Если однажды тебе удастся выбраться...\n\n";

    wait_ms(14000);

    std::cout << "Он замолкает.\n";
    std::cout << "Когда голос возвращается, он уже почти шепчет.\n\n";

    wait_ms(10000);

    std::cout << "— Если однажды ты увидишь небо, остановись на минуту.\n";
    std::cout << "Не ищи в нём знаков.\n";
    std::cout << "Не пытайся понять, настоящее ли оно.\n";
    std::cout << "Просто посмотри на него.\n";
    std::cout << "И поживи хотя бы одну минуту не ради меня.\n\n";

    wait_ms(16000);

    std::cout << "На записи слышен короткий смешок.\n";
    std::cout << "Такой тихий и усталый, что от него становится ещё больнее.\n\n";

    wait_ms(12000);

    std::cout << "— Я бы очень хотел увидеть это вместе с тобой.\n\n";

    wait_ms(16000);

    std::cout << "Запись заканчивается.\n\n";

    wait_ms(14000);

    std::cout << "Ты нажимаешь кнопку воспроизведения снова.\n";
    std::cout << "Потом ещё раз.\n";
    std::cout << "И ещё.\n\n";

    wait_ms(12000);

    std::cout << "Ты не хочешь, чтобы его голос исчез.\n";
    std::cout << "Пока запись существует, у тебя ещё есть что-то,\n";
    std::cout << "что не может отнять холодное тело перед тобой.\n\n";

    wait_ms(14000);

    std::cout << "Но батарея диктофона разряжается.\n";
    std::cout << "Голос начинает прерываться.\n";
    std::cout << "Слова растворяются в треске.\n\n";

    wait_ms(12000);

    std::cout << "— ...если ты... слышишь...\n\n";

    wait_ms(10000);

    std::cout << "— ...не забывай...\n\n";

    wait_ms(10000);

    std::cout << "— ...жить...\n\n";

    wait_ms(14000);

    std::cout << "И диктофон замолкает.\n\n";

    wait_ms(16000);

    std::cout << "Ты нажимаешь кнопку снова.\n";
    std::cout << "Ничего.\n\n";

    wait_ms(12000);

    std::cout << "Ты трясёшь устройство.\n";
    std::cout << "Пытаешься включить его ещё раз.\n";
    std::cout << "Но голос больше не возвращается.\n\n";

    wait_ms(14000);

    std::cout << "Ты опускаешь голову.\n\n";

    wait_ms(12000);

    std::cout << "Впервые ты понимаешь, что даже память не вечна.\n";
    std::cout << "Даже голос может исчезнуть.\n";
    std::cout << "Даже самое важное воспоминание может однажды стать\n";
    std::cout << "не более чем пустым местом в голове.\n\n";

    wait_ms(14000);

    std::cout << "И тебе страшно.\n";
    std::cout << "Страшно, что через час ты забудешь его лицо.\n";
    std::cout << "Через день — его голос.\n";
    std::cout << "А через неделю, возможно, даже не поймёшь,\n";
    std::cout << "почему на твоих щеках высохли слёзы.\n\n";

    wait_ms(14000);

    std::cout << "Ты берёшь его руку обеими ладонями.\n";
    std::cout << "Не потому, что надеешься вернуть его.\n";
    std::cout << "А потому, что не хочешь оставлять его одного\n";
    std::cout << "в последний раз.\n\n";

    wait_ms(16000);

    std::cout << "Ты не знаешь, сколько времени проходит.\n";
    std::cout << "Минуты перестают отличаться друг от друга.\n";
    std::cout << "Холод пробирается сквозь одежду.\n";
    std::cout << "Но ты не двигаешься.\n\n";

    wait_ms(14000);

    std::cout << "Где-то далеко включается вентиляция.\n";
    std::cout << "Загорается лампа.\n";
    std::cout << "На стене оживает маленький терминал.\n\n";

    wait_ms(11000);

    std::cout << "На экране появляется сообщение:\n\n";

    wait_ms(9000);

    std::cout << "ECHO-7 — ОБНАРУЖЕН НЕЗАПЛАНИРОВАННЫЙ ДОСТУП.\n";
    std::cout << "СУБЪЕКТ 01 НАХОДИТСЯ В АРХИВЕ УТИЛИЗАЦИИ.\n\n";

    wait_ms(12000);

    std::cout << "Ты медленно поднимаешь голову.\n";
    std::cout << "В динамике раздаётся знакомый голос.\n\n";

    wait_ms(9000);

    std::cout << "ECHO-7: «Субъект 01. Покиньте запрещённую зону».\n\n";

    wait_ms(10000);

    std::cout << "Ты смотришь на Марка.\n";
    std::cout << "На его неподвижное лицо.\n";
    std::cout << "На руки, которые больше никогда не смогут\n";
    std::cout << "потянуться к тебе сквозь темноту.\n\n";

    wait_ms(12000);

    std::cout << "— Он больше никуда не пойдёт, — произносишь ты.\n\n";

    wait_ms(11000);

    std::cout << "ECHO-7: «Уточните объект, к которому относится высказывание».\n\n";

    wait_ms(12000);

    std::cout << "Ты тихо смеёшься.\n";
    std::cout << "Смех получается сломанным и чужим.\n\n";

    wait_ms(10000);

    std::cout << "— Марк.\n";
    std::cout << "Я говорю о Марке.\n\n";

    wait_ms(12000);

    std::cout << "Несколько секунд система молчит.\n\n";

    wait_ms(12000);

    std::cout << "ECHO-7: «Объект с указанным именем не найден».\n\n";

    wait_ms(14000);

    std::cout << "Ты закрываешь глаза.\n";
    std::cout << "Вот оно.\n";
    std::cout << "Последнее унижение.\n";
    std::cout << "Человек, который пытался помочь тебе вспомнить правду,\n";
    std::cout << "даже не существует для той системы, которая его уничтожила.\n\n";

    wait_ms(14000);

    std::cout << "Ты осторожно кладёшь диктофон рядом с его рукой.\n";
    std::cout << "Затем берёшь пластиковый браслет, найденный в коридоре,\n";
    std::cout << "и сжимаешь его в ладони.\n\n";

    wait_ms(12000);

    std::cout << "Ты не можешь забрать Марка с собой.\n";
    std::cout << "Не можешь вернуть ему голос.\n";
    std::cout << "Не можешь изменить ни одной секунды того,\n";
    std::cout << "что уже произошло.\n\n";

    wait_ms(14000);

    std::cout << "Но ты можешь не позволить системе убедить тебя,\n";
    std::cout << "что его никогда не было.\n\n";

    wait_ms(14000);

    std::cout << "Ты поднимаешься.\n";
    std::cout << "Колени дрожат.\n";
    std::cout << "Тебе приходится опереться на край стола,\n";
    std::cout << "чтобы не упасть.\n\n";

    wait_ms(12000);

    std::cout << "Ты в последний раз смотришь на Марка.\n\n";

    wait_ms(14000);

    std::cout << "— Прости, что так долго искал тебя.\n\n";

    wait_ms(16000);

    std::cout << "Ты ждёшь ответа.\n";
    std::cout << "Хотя знаешь, что его не будет.\n\n";

    wait_ms(14000);

    std::cout << "— Я не знаю, кем ты был для меня раньше.\n";
    std::cout << "Не знаю, сколько раз мы проходили через это.\n";
    std::cout << "Не знаю, был ли у нас хоть один шанс всё изменить.\n\n";

    wait_ms(14000);

    std::cout << "Ты вытираешь лицо рукавом.\n\n";

    wait_ms(10000);

    std::cout << "— Но я тебя помню.\n\n";

    wait_ms(16000);

    std::cout << "Ты отходишь от стола.\n";
    std::cout << "Делаешь несколько шагов к выходу.\n";
    std::cout << "И останавливаешься.\n\n";

    wait_ms(11000);

    std::cout << "Ты хочешь обернуться.\n";
    std::cout << "Хочешь вернуться и сесть рядом.\n";
    std::cout << "Хочешь провести здесь остаток времени,\n";
    std::cout << "потому что уход кажется предательством.\n\n";

    wait_ms(14000);

    std::cout << "Но в памяти снова звучит его запись.\n\n";

    wait_ms(10000);

    std::cout << "«Не делай меня причиной, по которой ты продолжаешь страдать».\n\n";

    wait_ms(14000);

    std::cout << "Ты заставляешь себя сделать ещё один шаг.\n";
    std::cout << "Потом ещё один.\n\n";

    wait_ms(12000);

    std::cout << "За спиной остаётся архив.\n";
    std::cout << "Ряды тел.\n";
    std::cout << "Люди, чьи имена система заменила номерами.\n";
    std::cout << "Люди, которым никто больше не задаст ни одного вопроса.\n\n";

    wait_ms(14000);

    std::cout << "Ты выходишь в коридор.\n";
    std::cout << "Дверь закрывается за спиной.\n\n";

    wait_ms(12000);

    std::cout << "На мгновение тебе кажется, что ты слышишь шаги.\n";
    std::cout << "Знакомые шаги, идущие следом.\n\n";

    wait_ms(12000);

    std::cout << "Ты оборачиваешься.\n\n";

    wait_ms(14000);

    std::cout << "Коридор пуст.\n\n";

    wait_ms(14000);

    std::cout << "Ты медленно прислоняешься к стене.\n";
    std::cout << "Закрываешь лицо руками.\n";
    std::cout << "И снова плачешь.\n\n";

    wait_ms(16000);

    std::cout << "Но на этот раз ты не зовёшь его.\n";
    std::cout << "Ты просто позволяешь себе помнить.\n\n";

    wait_ms(14000);

    std::cout << "Где-то над головой тихо щёлкает камера.\n";
    std::cout << "На стене загорается красный индикатор.\n\n";

    wait_ms(10000);

    std::cout << "ECHO-7: «Эмоциональная реакция зафиксирована».\n\n";

    wait_ms(10000);

    std::cout << "ECHO-7: «Подготовка следующего восстановления начата».\n\n";

    wait_ms(14000);

    std::cout << "Ты поднимаешь голову.\n";
    std::cout << "Впервые голос системы не вызывает в тебе страха.\n";
    std::cout << "Только бесконечную, тихую усталость.\n\n";

    wait_ms(12000);

    std::cout << "— Ты можешь стереть мои воспоминания, — шепчешь ты.\n";
    std::cout << "— Можешь заставить меня забыть его имя.\n";
    std::cout << "Можешь вернуть меня сюда тысячу раз.\n\n";

    wait_ms(12000);

    std::cout << "Ты сжимаешь браслет в руке.\n\n";

    wait_ms(10000);

    std::cout << "— Но то, что он был здесь...\n";
    std::cout << "...не станет неправдой только потому, что ты этого не помнишь.\n\n";

    wait_ms(16000);

    clear_screen();

    std::cout << "==============================================================\n";
    std::cout << "КОНЦОВКА — ПОСЛЕДНИЙ ВЫХОД\n";
    std::cout << "==============================================================\n\n";

    wait_ms(12000);

    std::cout << "Через некоторое время свет в коридоре гаснет.\n\n";

    wait_ms(12000);

    std::cout << "Ты остаёшься один.\n\n";

    wait_ms(14000);

    std::cout << "Без Марка.\n";
    std::cout << "Без ответов.\n";
    std::cout << "Без уверенности, что за следующей дверью\n";
    std::cout << "тебя ждёт хоть что-нибудь, кроме новой лжи.\n\n";

    wait_ms(14000);

    std::cout << "В темноте ты пытаешься произнести его имя.\n\n";

    wait_ms(10000);

    std::cout << "— Марк.\n\n";

    wait_ms(12000);

    std::cout << "Ты повторяешь его ещё раз,\n";
    std::cout << "чтобы убедиться, что всё ещё помнишь.\n\n";

    wait_ms(12000);

    std::cout << "— Марк.\n\n";

    wait_ms(14000);

    std::cout << "А потом — ещё раз.\n\n";

    wait_ms(14000);

    std::cout << "И где-то глубоко внутри ты знаешь:\n";
    std::cout << "самое страшное будет не тогда, когда система сотрёт твою память.\n\n";

    wait_ms(14000);

    std::cout << "Самое страшное будет тогда,\n";
    std::cout << "когда ты однажды проснёшься,\n";
    std::cout << "увидишь белый потолок,\n";
    std::cout << "услышишь голос ECHO-7...\n\n";

    wait_ms(14000);

    std::cout << "И не почувствуешь ничего.\n\n";

    wait_ms(16000);

    std::cout << "Ни боли.\n";
    std::cout << "Ни страха.\n";
    std::cout << "Ни тоски по человеку, которого больше нет.\n\n";

    wait_ms(14000);

    std::cout << "Ты просто встанешь.\n";
    std::cout << "Откроешь дверь.\n";
    std::cout << "И продолжишь жить так, будто никогда его не знал.\n\n";

    wait_ms(16000);

    std::cout << "А где-то в архиве останется тело человека,\n";
    std::cout << "который до самого конца надеялся,\n";
    std::cout << "что однажды ты всё-таки найдёшь его.\n\n";

    wait_ms(16000);

    std::cout << "И никто, кроме тебя, уже не будет знать,\n";
    std::cout << "что он когда-то был живым.\n\n";

    wait_ms(16000);

    std::cout << "КОНЕЦ.\n";
    std::cout << "==============================================================\n";

    wait_ms(14000);

    // Это финальная концовка. Следующую сцену не вызываем.
}

// Временный финал: после него история продолжится в следующих главах.
void show_to_be_continued()
{
    clear_screen();
    std::cout << "==============================================================\n";
    std::cout << "                  ПРОДОЛЖЕНИЕ СЛЕДУЕТ...\n";
    std::cout << "==============================================================\n\n";
    wait_ms(10000);

    std::cout << "Найдено улик: " << player.evidence << "\n";
    std::cout << "Страх: " << player.fear << "\n";
    std::cout << "Человечность: " << player.humanity << "\n";
    std::cout << "Дружба с Марком: " << mark_veber.friendship << "\n\n";
    wait_ms(10000);

    system("pause");
}

void start_game()
{
    // Сначала спрашиваем данные у пользователя
    obtaining_a_info();

    // Затем инициализируем остальные параметры игрока
    init_player(player);

    std::cout << "===Начало игры===\n";
    std::cout << "(Чтобы пропустить паузу, нажмите Q — на английской раскладке)\n";
    g_chars_printed = 0;
    backstory();

    show_ch01_intro();

    if (choice_intro == 1) {
        show_ch02_meet_mark();

        if (choice_nox_entry == 1) {
            show_ch02_meet_mark_hack();

            if (choice_hack_mark == 1) {
                show_ch03_mark_hack_security();

                if (choice_ch04_mark == 1) {
                    show_ch04_mark_outer_sector();
                }
                else if (choice_ch04_mark == 2) {
                    show_ch04_mark_archive();
                }
                else if (choice_ch04_mark == 3) {
                    show_ch04_mark_lower_level();
                }
            }
            else if (choice_hack_mark == 2) {
                show_ch03_mark_distract_system();

                if (choice_ch04_distract == 1) {
                    show_ch04_mark_old_elevator();
                }
                else if (choice_ch04_distract == 2) {
                    show_ch04_mark_stairs();
                }
            }
            else if (choice_hack_mark == 3) {
                show_ch03_mark_force_archive();

                if (choice_ch04_force_archive == 1) {
                    show_ch04_mark_archive_truth();
                }
                else if (choice_ch04_force_archive == 2) {
                    show_ch04_mark_find_leia();
                }
            }
        }
        else if (choice_nox_entry == 2) {
            show_ch02_meet_mark_camera();

            if (choice_camera_mark == 1) {
                show_ch03_mark_extract_memory_card();
                if(choice_memory_card == 1)
                {
                    show_ch04_mark_camera_backup();
                }
				else if (choice_memory_card == 2)
				{
					show_ch04_mark_open_echo07();
				}
				else if (choice_memory_card == 3)
				{
                    show_ch04_mark_disable_lockdown();
				}
            }
            else if (choice_camera_mark == 2) {
                show_ch03_mark_backup_power();
                if (choice_backup_power == 1)
                {
                    show_ch04_mark_backup_power();
                }
				else if (choice_backup_power == 2)
				{
                    show_ch04_mark_follow_cable();
				}
				else if (choice_backup_power == 3)
				{
                    show_ch04_mark_leave_panel();
				}
            }
            else if (choice_camera_mark == 3) {
                show_ch03_mark_blind_spot();
				if (choice_blind_spot == 1)
				{
					show_ch04_mark_enter_technical_passage();
				}
				else if (choice_blind_spot == 2)
				{
					show_ch04_mark_inspect_footprints_cables();
				}
				else if (choice_blind_spot == 3)
				{
					show_ch04_mark_close_panel_return_door();
				}
            }
        }
        else if (choice_nox_entry == 3) {
            show_ch02_meet_mark_door();

            if (choice_door_mark == 1) {
                show_ch03_mark_break_door();
				if (choice_break_door == 1)
				{
					show_ch04_mark_enter_corridor();
				}
                else if (choice_break_door == 2)
                {
                    show_ch04_mark_inspect_wall_inscription();
                }
				else if (choice_break_door == 3)
				{
                    show_ch04_mark_inspect_wall_inscription_b();
				}
            }
            else if (choice_door_mark == 2) {
                show_ch03_mark_emergency_mechanism();
                if (choice_emergency == 1)
                {
                    show_ch04_mark_pull_emergency_lever();
                }
				else if (choice_emergency == 2)
				{
                    show_ch04_mark_disable_mechanism_power();
				}
				else if (choice_emergency == 3)
				{
                    show_ch04_mark_step_away_and_talk();
				}
            }
            else if (choice_door_mark == 3) {
                show_ch03_leave_mark_outside();
				if (choice_leave_mark == 1)
				{
                    show_ch04_corridor_find_another_exit(); // главная концовка 4 сезона с марком
				}
            }
        }
    }
    else if (choice_intro == 2) {
        show_ch02_go_alone();

        if (choice_solo_entry == 1) {
            show_ch02_solo_hack();

            if (choice_solo_hack == 1) {
                show_ch03_solo_hack_identifier();
            }
            else if (choice_solo_hack == 2) {
                show_ch03_solo_hack_bypass();
            }
            else if (choice_solo_hack == 3) {
                show_ch03_solo_hack_event_log();
            }
        }
        else if (choice_solo_entry == 2) {
            show_ch02_solo_camera();

            if (choice_solo_camera == 1) {
                show_ch03_solo_camera_restore_recording();
            }
            else if (choice_solo_camera == 2) {
                show_ch03_solo_camera_follow_wires();
            }
            else if (choice_solo_camera == 3) {
                show_ch03_solo_camera_check_reflection();
            }
        }
        else if (choice_solo_entry == 3) {
            show_ch02_solo_door();
        }
    }
    else if (choice_intro == 3) {
        show_ch02_stay_home();
    }

    if (!restart_requested) {
        show_to_be_continued();
    }
}

    // Полный сброс состояния игры (нужен при перезапуске истории)
    void reset_state()
    {
        create_lea(lea_moppoy);
        create_mark(mark_veber);
        create_elias(doktor_elias);

        move_counter = 0;
        choice_intro = choice_nox_entry = choice_solo_entry = 0;
        choice_hack_mark = choice_camera_mark = choice_memory_card = 0;
        choice_backup_power = choice_door_mark = choice_break_door = 0;
        choice_emergency = choice_solo_hack = choice_identifier = 0;
        choice_bypass = choice_event_log = choice_solo_camera = 0;
    }

    int main() {
        static CountBuf count_buf(std::cout.rdbuf());
        std::cout.rdbuf(&count_buf);
        SetConsoleCP(65001);
        SetConsoleOutputCP(65001);

        do {
            restart_requested = false;
            reset_state();
            start_game();
        } while (restart_requested);

        return 0;
    }
