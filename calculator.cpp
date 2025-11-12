#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_CLASS_NAME "Win98Calc"
#define WINDOW_WIDTH 210
#define WINDOW_HEIGHT 300
#define BUTTON_SIZE 40
#define MARGIN 10
#define DISPLAY_HEIGHT 30

#define IDC_DISPLAY         100
#define IDC_BUTTON_0        101
#define IDC_BUTTON_1        102
#define IDC_BUTTON_2        103
#define IDC_BUTTON_3        104
#define IDC_BUTTON_4        105
#define IDC_BUTTON_5        106
#define IDC_BUTTON_6        107
#define IDC_BUTTON_7        108
#define IDC_BUTTON_8        109
#define IDC_BUTTON_9        110
#define IDC_BUTTON_PLUS     120
#define IDC_BUTTON_MINUS    121
#define IDC_BUTTON_MULTIPLY 122
#define IDC_BUTTON_DIVIDE   123
#define IDC_BUTTON_EQ       124
#define IDC_BUTTON_CLEAR    125
#define IDC_BUTTON_DOT      126

HWND hDisplay;
char display_buffer[64] = "0";
double accumulator = 0.0;
char current_op = 0;
BOOL new_input = TRUE;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateButton(HWND hwndParent, HINSTANCE hInst, const char* text, int x, int y, int id)
{
    return CreateWindow(
        "BUTTON", text,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x, y, BUTTON_SIZE, BUTTON_SIZE,
        hwndParent, (HMENU)id, hInst, NULL
    );
}

void DoubleToString(double value, char* buffer)
{
    sprintf(buffer, "%.10f", value);
    int len = strlen(buffer);
    while (len > 0 && buffer[len - 1] == '0') {
        buffer[len - 1] = '\0';
        len--;
    }
    if (len > 0 && buffer[len - 1] == '.') {
        buffer[len - 1] = '\0';
    }
}

void PerformCalculation()
{
    double current_value = atof(display_buffer);

    if (current_op == 0) {
        accumulator = current_value;
    } else {
        switch (current_op) {
        case '+': accumulator += current_value; break;
        case '-': accumulator -= current_value; break;
        case '*': accumulator *= current_value; break;
        case '/':
            if (current_value == 0) {
                strcpy(display_buffer, "Error");
                accumulator = 0;
                current_op = 0;
                new_input = TRUE;
                return;
            }
            accumulator /= current_value;
            break;
        }
    }

    DoubleToString(accumulator, display_buffer);
    new_input = TRUE;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        hDisplay = CreateWindowEx(
            0,
            "STATIC",
            display_buffer,
            WS_CHILD | WS_VISIBLE | SS_RIGHT | WS_BORDER,
            MARGIN, MARGIN, WINDOW_WIDTH - 2 * MARGIN - 6, DISPLAY_HEIGHT,
            hwnd, (HMENU)IDC_DISPLAY, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );

        int btn_y = MARGIN + DISPLAY_HEIGHT + MARGIN;

        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "C", MARGIN, btn_y, IDC_BUTTON_CLEAR);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "/", MARGIN + BUTTON_SIZE, btn_y, IDC_BUTTON_DIVIDE);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "*", MARGIN + 2 * BUTTON_SIZE, btn_y, IDC_BUTTON_MULTIPLY);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "-", MARGIN + 3 * BUTTON_SIZE, btn_y, IDC_BUTTON_MINUS);

        btn_y += BUTTON_SIZE;
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "7", MARGIN, btn_y, IDC_BUTTON_7);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "8", MARGIN + BUTTON_SIZE, btn_y, IDC_BUTTON_8);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "9", MARGIN + 2 * BUTTON_SIZE, btn_y, IDC_BUTTON_9);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "+", MARGIN + 3 * BUTTON_SIZE, btn_y, IDC_BUTTON_PLUS);

        btn_y += BUTTON_SIZE;
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "4", MARGIN, btn_y, IDC_BUTTON_4);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "5", MARGIN + BUTTON_SIZE, btn_y, IDC_BUTTON_5);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "6", MARGIN + 2 * BUTTON_SIZE, btn_y, IDC_BUTTON_6);

        btn_y += BUTTON_SIZE;
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "1", MARGIN, btn_y, IDC_BUTTON_1);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "2", MARGIN + BUTTON_SIZE, btn_y, IDC_BUTTON_2);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "3", MARGIN + 2 * BUTTON_SIZE, btn_y, IDC_BUTTON_3);
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, "=", MARGIN + 3 * BUTTON_SIZE, btn_y, IDC_BUTTON_EQ);

        btn_y += BUTTON_SIZE;
        CreateWindow(
            "BUTTON", "0",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            MARGIN, btn_y, BUTTON_SIZE * 2, BUTTON_SIZE,
            hwnd, (HMENU)IDC_BUTTON_0, ((LPCREATESTRUCT)lParam)->hInstance, NULL
        );
        CreateButton(hwnd, ((LPCREATESTRUCT)lParam)->hInstance, ".", MARGIN + 2 * BUTTON_SIZE, btn_y, IDC_BUTTON_DOT);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int len = strlen(display_buffer);

        if (wmId >= IDC_BUTTON_0 && wmId <= IDC_BUTTON_9)
        {
            char digit = (char)(wmId - IDC_BUTTON_0 + '0');
            if (new_input || strcmp(display_buffer, "0") == 0 || strcmp(display_buffer, "Error") == 0) {
                display_buffer[0] = digit;
                display_buffer[1] = 0;
                new_input = FALSE;
            } else if (len < 63) {
                display_buffer[len] = digit;
                display_buffer[len + 1] = 0;
            }
        }
        else if (wmId == IDC_BUTTON_DOT) {
            if (new_input || strcmp(display_buffer, "Error") == 0) {
                strcpy(display_buffer, "0.");
                new_input = FALSE;
            } else if (strchr(display_buffer, '.') == NULL && len < 63) {
                display_buffer[len] = '.';
                display_buffer[len + 1] = 0;
            }
        }
        else if (wmId >= IDC_BUTTON_PLUS && wmId <= IDC_BUTTON_DIVIDE) {
            if (current_op != 0 && !new_input) {
                PerformCalculation();
            } else {
                accumulator = atof(display_buffer);
            }

            switch (wmId) {
            case IDC_BUTTON_PLUS: current_op = '+'; break;
            case IDC_BUTTON_MINUS: current_op = '-'; break;
            case IDC_BUTTON_MULTIPLY: current_op = '*'; break;
            case IDC_BUTTON_DIVIDE: current_op = '/'; break;
            }
            new_input = TRUE;
        }
        else if (wmId == IDC_BUTTON_EQ) {
            if (current_op != 0 && !new_input) {
                PerformCalculation();
            }
            current_op = 0;
            new_input = TRUE;
        }
        else if (wmId == IDC_BUTTON_CLEAR) {
            strcpy(display_buffer, "0");
            accumulator = 0;
            current_op = 0;
            new_input = TRUE;
        }

        SetWindowText(hDisplay, display_buffer);
    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClass(&wc)) return 0;

    HWND hwnd = CreateWindow(
        WINDOW_CLASS_NAME,
        "Win98 Calculator",
        WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
