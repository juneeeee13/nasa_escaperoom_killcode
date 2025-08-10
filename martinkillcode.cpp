/*
 * Escape-Room Terminal (v4-a)
 *
 *  ? Admin password : 9999
 *  ? Hidden timer   : 2 hours (kills program on expiry)      // ? updated
 *  ? Player login   :  user = "Voyager", pass = "201977"
 *  ? Kill-code      :  "000011100001111000011"
 *
 *  Revision:
 *    • Clears screen once more after the Matrix effect so the final
 *      congratulatory message appears on a clean, non-green screen.
 */

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#define CLR_CMD "cls"
#else
#include <sys/ioctl.h>
#include <unistd.h>
#define CLR_CMD "clear"
#endif

using namespace std;

/* ?? helpers ??????????????????????????????????????????????????????????? */
inline void clearScreen() { system(CLR_CMD); }

void terminalSize(int& rows, int& cols) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return;
    }
#else
    winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        cols = w.ws_col;
        rows = w.ws_row;
        return;
    }
#endif
    rows = 30;
    cols = 100;
}

void matrixFlood(chrono::seconds duration = chrono::seconds{5},
                 chrono::milliseconds frameDelay = chrono::milliseconds{15}) {
    srand(static_cast<unsigned>(time(nullptr)));
    int rows, cols;
    terminalSize(rows, cols);

    const auto endTime = chrono::steady_clock::now() + duration;
    constexpr char green[] = "\033[32m";
    constexpr char reset[] = "\033[0m";

    while (chrono::steady_clock::now() < endTime) {
        clearScreen();
        cout << green;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c)
                cout << (rand() & 1);
            cout << '\n';
        }
        cout << reset << flush;
        this_thread::sleep_for(frameDelay);
    }
}

/* ?? main ??????????????????????????????????????????????????????????????? */
int main() {
    using namespace chrono_literals;
    using clock = chrono::steady_clock;

    string input;

    // 1. Admin gate
    while (true) {
        cout << "Enter admin password: ";
        getline(cin, input);
        clearScreen();
        if (input == "9999") break;
        clearScreen();
        cout << "Incorrect. Try again.\n";
    }

    // 2. Hidden two-hour timer
    const auto start = clock::now();
    thread([] {
        this_thread::sleep_for(chrono::hours{2});
        cerr << "\n*** TIME'S UP! You failed to escape in time. ***\n";
        exit(EXIT_FAILURE);
    }).detach();

    // 3. Player login
    string user, pass;
    while (true) {
        clearScreen();
        cout << "Username: ";
        getline(cin, user);
        cout << "Password: ";
        getline(cin, pass);
        clearScreen();
        if (user == "Voyager" && pass == "201977") break;
        cout << "WRONG LOGIN CREDENTIALS, TRY AGAIN\n";
        this_thread::sleep_for(3s);
        clearScreen();
    }

    // 4. Kill-code
    constexpr char killCode[] = "000011100001111000011";
    while (true) {
        cout << "M.A.R.T.I.N Kill Code: ";
        getline(cin, input);
        clearScreen();
        if (input == killCode) break;
        cout << "Incorrect code. Try again.\n";
    }

    // 5. Matrix splash
    matrixFlood();
    clearScreen();

    // 6. Success message
    auto elapsed = chrono::duration_cast<chrono::seconds>(clock::now() - start);
    auto minutes = elapsed.count() / 60;
    auto seconds = elapsed.count() % 60;

    cout << "Congrats! Your team solved the escape room in "
         << minutes << " minute" << (minutes == 1 ? "" : "s") << ' '
         << seconds << " second" << (seconds == 1 ? "" : "s") << ".\n";

    return 0;
}
