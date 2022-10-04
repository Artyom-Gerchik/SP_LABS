#ifndef UNICODE
#define UNICODE
#endif 

#include <Windows.h>
#include <chrono>
#include <string>
#include <random>
#include <thread>

#define pthread_t DWORD
#define pthread_create(THREAD_ID_PTR, ATTR, ROUTINE, PARAMS) CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ROUTINE,(void*)PARAMS,0,THREAD_ID_PTR)
#define sleep(ms) Sleep(ms)

using namespace std::chrono_literals;



const int OnSortButtonClicked = 1;

struct StateInfo
{
    HWND inputText = {};
    HWND outputText = {};
    HWND arrayInputEdit = {};
    HWND arrayOutputEdit = {};
    HWND sortButton = {};
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

struct TASK
{
    int low;
    int high;
    int busy;
    int* a;
};

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
        L"Multithreading sort",
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
        switch (wParam)
        {
        case OnSortButtonClicked:
        {
            TCHAR stringArraySize[] = TEXT("__________");
            GetWindowText(pState->arraySizeInput, stringArraySize, 11);
            int arraySize = _wtoi(stringArraySize);

            TCHAR stringThreadsAmount[] = TEXT("__");
            GetWindowText(pState->threadsAmountInput, stringThreadsAmount, 3);
            threadsAmount = _wtoi(stringThreadsAmount);

            TCHAR stringToPrint[] = TEXT("_");
            GetWindowText(pState->printEdit, stringToPrint, 2);
            int toPrint = _wtoi(stringToPrint);

            int* array = new int[arraySize];
            FillArray(array, arraySize);


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



            if (threadsAmount > 0) {

                auto sizeOfArrayPart = arraySize / threadsAmount;
                pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * threadsAmount);
                TASK* tasklist = (TASK*)malloc(sizeof(TASK) * threadsAmount);

                TASK* task;
                int low = 0;


                for (int i = 0; i < threadsAmount; i++, low += sizeOfArrayPart)
                {
                    task = &tasklist[i];
                    task->low = low;
                    task->high = low + sizeOfArrayPart - 1;
                    if (i == (threadsAmount - 1))
                        task->high = arraySize - 1;
                }

                auto start = std::chrono::steady_clock::now();

                // create the threads
                for (int i = 0; i < threadsAmount; i++)
                {
                    task = &tasklist[i];
                    task->a = array;
                    task->busy = 1;
                    pthread_create(&threads[i], 0, merge_sort_thread, task);
                }

                // wait for all threads
                auto start_for_complete_threads = std::chrono::steady_clock::now();
                for (int i = 0; i < threadsAmount; i++)
                    while (tasklist[i].busy)
                        sleep(50);

                auto end_for_complete_threads = std::chrono::steady_clock::now();
                auto elapsed_ms_for_complete_threads = std::chrono::duration_cast<std::chrono::milliseconds>(end_for_complete_threads - start_for_complete_threads);

                TASK* taskm = &tasklist[0];
                for (int i = 1; i < threadsAmount; i++)
                {
                    TASK* task = &tasklist[i];
                    merge(taskm->a, taskm->low, task->low - 1, task->high);
                }

                auto end = std::chrono::steady_clock::now();
                auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                elapsed_ms -= elapsed_ms_for_complete_threads;

                std::string tmp = std::to_string(elapsed_ms.count());
                TCHAR* time = new TCHAR[100];
                time[tmp.length()] = '\0';
                copy(tmp.begin(), tmp.end(), time);

                SetWindowText(pState->timeEdit, time);

                free(tasklist);
                free(threads);
            }
            else{
                auto start = std::chrono::steady_clock::now();
                merge_sort(array, 0, arraySize - 1);
                auto end = std::chrono::steady_clock::now();
                auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                std::string tmp = std::to_string(elapsed_ms.count());
                TCHAR* time = new TCHAR[100];
                time[tmp.length()] = '\0';
                copy(tmp.begin(), tmp.end(), time);

                SetWindowText(pState->timeEdit, time);
            }

            if (toPrint == 1) {
                std::string output = "";

                for (int i = 0; i < arraySize; i++)
                {
                    output += std::to_string(array[i]);
                    output += "\r\n";
                }

                int size_w = output.length();
                TCHAR* output_w = new TCHAR[size_w + 1];
                output_w[size_w] = '\0';

                copy(output.begin(), output.end(), output_w);
                SetWindowText(pState->arrayOutputEdit, output_w);
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

    pState->sortButton = CreateButton(hwnd, L"Sort", 240, 340, 70, 95, OnSortButtonClicked);

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
void* merge_sort_thread(void* arg)
{
    TASK* task = (TASK*)arg;
    int low;
    int high;

    // calculating low and high
    low = task->low;
    high = task->high;

    // evaluating mid point
    int mid = low + (high - low) / 2;

    if (low < high)
    {
        merge_sort(task->a, low, mid);
        merge_sort(task->a, mid + 1, high);
        merge(task->a, low, mid, high);
    }
    task->busy = 0;
    return 0;
}
