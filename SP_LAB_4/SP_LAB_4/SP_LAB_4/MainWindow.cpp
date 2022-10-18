#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <chrono>
#include <string>
#include <random>
#include <thread>

const int OnWriteToFileButtonClicked = 0;
const int OnReadFromFileButtonClicked = 1;

const int OnWriteToFileAsyncButtonClicked = 2;
const int OnReadFromFileAsyncButtonClicked = 3;

struct StateInfo
{
    HWND inputText = {};
    HWND outputText = {};
    HWND arrayInputEdit = {};
    HWND arrayOutputEdit = {};

    HWND writeToFileButton = {};
    HWND readFromFileButton = {};
    HWND writeToFileAsyncButton = {};
    HWND readFromFileAsyncButton = {};

    HWND timeText = {};
    HWND timeEdit = {};
    HWND arraySizeText = {};
    HWND arraySizeInput = {};
    HWND threadsAmountText = {};
    HWND threadsAmountInput = {};
    HWND printText = {};
    HWND printEdit = {};
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
StateInfo* GetAppState(HWND hwnd);
HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked);
LRESULT Initialize(HWND hwnd);
void MergeSort(int* array, int left, int right);
void FillArray(int* array, int size);

void merge(int* a, int low, int mid, int high);
void merge_sort(int* a, int low, int high);
void* merge_sort_thread(void* arg);



int threadsAmount = 0;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"MainWindowClass";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    auto pState = new StateInfo;
    if (pState == nullptr)
    {
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"LAB4",
        WS_OVERLAPPED | WS_SYSMENU,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        335,
        500,
        nullptr,
        nullptr,
        hInstance,
        pState
    );

    if (hwnd == nullptr)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    StateInfo* pState;
    if (uMsg == WM_CREATE)
    {
        auto pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pState = reinterpret_cast<StateInfo*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
    }
    else
    {
        pState = GetAppState(hwnd);
    }

    switch (uMsg)
    {
    case WM_CREATE:
        Initialize(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_COMMAND:
    {
        TCHAR stringThreadsAmount[] = TEXT("__");
        GetWindowText(pState->threadsAmountInput, stringThreadsAmount, 3);
        threadsAmount = _wtoi(stringThreadsAmount);

        TCHAR stringToPrint[] = TEXT("_");
        GetWindowText(pState->printEdit, stringToPrint, 2);
        int toPrint = _wtoi(stringToPrint);

        switch (wParam)
        {
        case OnWriteToFileButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = _wtoi(stringArraySize);

            int* array = new int[arraySize];
            FillArray(array, arraySize);

            std::string input = "";

            if (toPrint == 1) {
                std::string input = "";

                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += "\r\n";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayInputEdit, input_w);
            }

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, 0, nullptr);

            dwStart = GetTickCount64();
            WriteFile(hFile, array, arraySize * sizeof(int), &dwDone, nullptr);
            dwEnd = GetTickCount64();

            CloseHandle(hFile);

            input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeEdit, time);

            delete[] array;

            return 0;
        }
        case OnReadFromFileButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = _wtoi(stringArraySize);

            int* array = new int[arraySize];

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_READ, 0, nullptr, OPEN_ALWAYS, 0, nullptr);

            dwStart = GetTickCount();
            ReadFile(hFile, array, arraySize * sizeof(int), &dwDone, nullptr);
            merge_sort(array, 0, arraySize - 1);
            dwEnd = GetTickCount();

            CloseHandle(hFile);

            std::string input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeEdit, time);

            if (toPrint == 1) {
                std::string input = "";

                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += "\r\n";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayOutputEdit, input_w);
            }

            delete[] array;

            return 0;
        }
        case OnWriteToFileAsyncButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = _wtoi(stringArraySize);

            int* array = new int[arraySize];
            FillArray(array, arraySize);

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_FLAG_OVERLAPPED, nullptr);

            std::vector<OVERLAPPED> overlappeds;

            dwStart = GetTickCount();

            for (auto i = 0; i < threadsAmount; i++)
            {
                OVERLAPPED overlapped = {};
                overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                overlapped.Offset += arraySize * sizeof(int) * i / (threadsAmount);

                WriteFile(hFile, array + i * arraySize / (threadsAmount), arraySize * sizeof(int) / (threadsAmount),
                    &dwDone, &overlapped);

                overlappeds.push_back(overlapped);
            }

            for (auto o : overlappeds)
            {
                WaitForSingleObject(o.hEvent, INFINITE);
                CloseHandle(o.hEvent);
            }

            dwEnd = GetTickCount();

            CloseHandle(hFile);

            std::string input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeEdit, time);

            input = "";

            if (toPrint == 1)
            {
                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += "\r\n";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayInputEdit, input_w);
            }

            delete[] array;

            return 0;
        }
        case OnReadFromFileAsyncButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = _wtoi(stringArraySize);

            int* array = new int[arraySize];

            DWORD dwDone, dwStart, dwEnd;
            HANDLE hFile;

            hFile = CreateFile(L"file.dat", GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_FLAG_OVERLAPPED, nullptr);

            std::vector<OVERLAPPED> overlappeds;
  
            dwStart = GetTickCount();

            for (auto i = 0; i < threadsAmount; i++)
            {
                OVERLAPPED overlapped = {};
                overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
                overlapped.Offset += arraySize * sizeof(int) * i / (threadsAmount);
                ReadFile(hFile, array + i * arraySize / (threadsAmount), arraySize * sizeof(int) / (threadsAmount),
                    &dwDone, &overlapped);

                overlappeds.push_back(overlapped);
            }

            std::vector<std::thread> threads;
            
            int index = 0;
            
            for (auto o : overlappeds)
            {
                WaitForSingleObject(o.hEvent, INFINITE);
                CloseHandle(o.hEvent);
                std::thread thread(merge_sort, array, o.Offset / sizeof(int), o.Offset / sizeof(int) + arraySize / (threadsAmount));
                threads.push_back(move(thread));
            }

            for (auto i = 0; i < threads.size(); i++)
            {
                threads[i].join();
            }

            auto i = 0;
           
            while (threadsAmount > 1)
            {
                int left = i * arraySize / threadsAmount;
                int right = left + 2 * arraySize / threadsAmount;
                int mid = (left + right) / 2;

                merge(array, left, mid, right);

                i += 1;
                threadsAmount -= 1;
            }

            dwEnd = GetTickCount();

            CloseHandle(hFile);

            std::string input = std::to_string(dwEnd - dwStart);
            TCHAR* time = new TCHAR[100];
            time[input.length()] = '\0';
            copy(input.begin(), input.end(), time);
            SetWindowText(pState->timeEdit, time);

            if (toPrint == 1)
            {
                input = "";

                for (int i = 0; i < arraySize; i++)
                {
                    input += std::to_string(array[i]);
                    input += "\r\n";
                }

                int size_w = input.length();
                TCHAR* input_w = new TCHAR[size_w + 1];
                input_w[size_w] = '\0';

                copy(input.begin(), input.end(), input_w);
                SetWindowText(pState->arrayOutputEdit, input_w);
            }

            return 0;
        }
        default:
            return 0;
        }
    }

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND CreateButton(HWND hwnd, LPCWSTR buttonText, int x, int y, int width, int height, int OnButtonClicked)
{
    return CreateWindow(
        L"BUTTON",
        buttonText,
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        x,
        y,
        width,
        height,
        hwnd,
        (HMENU)OnButtonClicked,
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        nullptr
    );
}

StateInfo* GetAppState(HWND hwnd)
{
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    auto pState = reinterpret_cast<StateInfo*>(ptr);
    return pState;
}

LRESULT Initialize(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    auto pState = GetAppState(hwnd);

    pState->inputText = CreateWindow(L"STATIC", L"INPUT", WS_VISIBLE | WS_CHILD, 10, 10, 60, 20, hwnd, nullptr, nullptr, nullptr);
    pState->arrayInputEdit = CreateWindow(L"EDIT", 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL | WS_BORDER, 10, 30, 150, 300, hwnd, nullptr, nullptr, nullptr);

    pState->outputText = CreateWindow(L"STATIC", L"OUTPUT", WS_VISIBLE | WS_CHILD, 160, 10, 60, 20, hwnd, nullptr, nullptr, nullptr);
    pState->arrayOutputEdit = CreateWindow(L"EDIT", 0, WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL | WS_BORDER, 160, 30, 150, 300, hwnd, nullptr, nullptr, nullptr);

    pState->timeText = CreateWindow(L"STATIC", L"Time (ms):", WS_VISIBLE | WS_CHILD, 10, 340, 80, 20, hwnd, nullptr, nullptr, nullptr);
    pState->timeEdit = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 340, 80, 20, hwnd, nullptr, nullptr, nullptr);

    pState->arraySizeText = CreateWindow(L"STATIC", L"Array size:", WS_VISIBLE | WS_CHILD, 10, 365, 80, 20, hwnd, nullptr, nullptr, nullptr);
    pState->arraySizeInput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 365, 80, 20, hwnd, nullptr, nullptr, nullptr);

    pState->threadsAmountText = CreateWindow(L"STATIC", L"Threads amount:", WS_VISIBLE | WS_CHILD, 10, 390, 120, 20, hwnd, nullptr, nullptr, nullptr);
    pState->threadsAmountInput = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 390, 80, 20, hwnd, nullptr, nullptr, nullptr);

    pState->printText = CreateWindow(L"STATIC", L"To print:", WS_VISIBLE | WS_CHILD, 10, 415, 60, 20, hwnd, nullptr, nullptr, nullptr);
    pState->printEdit = CreateWindow(L"EDIT", L"0", WS_VISIBLE | WS_CHILD | WS_BORDER, 150, 415, 80, 20, hwnd, nullptr, nullptr, nullptr);

    pState->writeToFileButton = CreateButton(hwnd, L"Write", 240, 340, 70, 20, OnWriteToFileButtonClicked);
    pState->readFromFileButton = CreateButton(hwnd, L"Read", 240, 365, 70, 20, OnReadFromFileButtonClicked);

    pState->writeToFileAsyncButton = CreateButton(hwnd, L"WriteA", 240, 390, 70, 20, OnWriteToFileAsyncButtonClicked);
    pState->readFromFileAsyncButton = CreateButton(hwnd, L"ReadA", 240, 415, 70, 20, OnReadFromFileAsyncButtonClicked);

    return 0;
}

void FillArray(int* array, int size)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(-2147483648, 2147483647);

    for (int i = 0; i < size; i++)
    {
        array[i] = distr(gen);
    }
}

// merge function for merging two parts
void merge(int* a, int low, int mid, int high)
{

    // n1 is size of left side and n2 is size of right side
    int n1 = mid - low + 1;
    int n2 = high - mid;

    int* left = (int*)malloc(n1 * sizeof(int));
    int* right = (int*)malloc(n2 * sizeof(int));

    int i;
    int j;

    // storing values in left part
    for (i = 0; i < n1; i++) {
        left[i] = a[i + low];
    }

    // storing values in right part
    for (j = 0; j < n2; j++) {
        right[j] = a[j + mid + 1];
    }

    int k = low;

    i = j = 0;

    // merge left and right in ascending order
    while (i < n1 && j < n2)
    {
        if (left[i] <= right[j])
            a[k++] = left[i++];
        else
            a[k++] = right[j++];
    }

    // insert remaining values from left
    while (i < n1)
        a[k++] = left[i++];

    // insert remaining values from right
    while (j < n2)
        a[k++] = right[j++];

    free(left);
    free(right);
}

// merge sort function
void merge_sort(int* a, int low, int high)
{

    // calculating mid point of array
    int mid = low + (high - low) / 2;

    if (low < high)
    {
        // call 1st half
        merge_sort(a, low, mid);

        // call 2nd half
        merge_sort(a, mid + 1, high);

        // merge 1st and 2nd halves
        merge(a, low, mid, high);
    }
}

// thread function
//void* merge_sort_thread(void* arg)
//{
//    TASK* task = (TASK*)arg;
//    int low;
//    int high;
//
//    // calculating low and high
//    low = task->low;
//    high = task->high;
//
//    // evaluating mid point
//    int mid = low + (high - low) / 2;
//
//    if (low < high)
//    {
//        merge_sort(task->a, low, mid);
//        merge_sort(task->a, mid + 1, high);
//        merge(task->a, low, mid, high);
//    }
//    task->busy = 0;
//    return 0;
//}