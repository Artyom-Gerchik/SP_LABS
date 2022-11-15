#include <process.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <time.h>

using namespace std;

#define N 5					//Число философов

#define LEFT (i-1)%N		//Левый сосед философа с номером i - 1
#define RIGHT (i+1)%N		//Правый сосед философа с номером i + 1

#define THINKING 0			//Философ размышляет
#define HUNGRY 1			//Философ получается получить вилки
#define EATING 2			//Философ ест

int philosopher_state[N];   //Состояния каждого философа

struct Philosopher          // Описание философа
{
	int number;
};

CRITICAL_SECTION cs;        //Для критических секций: синхрон. процессов(философов)    
CRITICAL_SECTION cs_forks;  //и синхр. вилок

HANDLE philMutex[N];    //Каждому философу по мьютексу
HANDLE forkMutex[N];    //и каждой вилке

void think(int i)       //Моделирует размышления философа
{
	EnterCriticalSection(&cs);    //Вход в критическую секцию
	cout << "Philosopher		" << i << "      thinking" << endl;
	LeaveCriticalSection(&cs);    //Выход из критической секции
}

void eat(int i)         //Моделирует обед философа
{
	EnterCriticalSection(&cs);    //Вход в критическую секцию
	cout << "Philosopher		" << i << "      eating" << endl;
	LeaveCriticalSection(&cs);    //Выход из критической секции
}

void test_for_dinner(int i)    //Проверка возможности начать философом обед
{
	if (philosopher_state[i] == HUNGRY && philosopher_state[LEFT] != EATING && philosopher_state[RIGHT] != EATING)
	{
		philosopher_state[i] = EATING;
		ReleaseMutex(philMutex[i]);
	}
}

void take_forks(int i)
{
	EnterCriticalSection(&cs_forks);              //Вход в критическую секцию
	philosopher_state[i] = HUNGRY;                //Фиксация наличия голодного философа
	test_for_dinner(i);                           //Попытка получить две вилки
	LeaveCriticalSection(&cs_forks);              //Выход из критической секции
	//cout << "Philosopher number " << i << " taked forks" << endl;
	WaitForSingleObject(philMutex[i], INFINITE);  //Блокировка если вилок не досталось
}

void put_forks(int i)
{
	EnterCriticalSection(&cs_forks);  //Вход в критическую секцию   
	philosopher_state[i] = THINKING;  //Философ перестал есть
	test_for_dinner(LEFT);            //Проверить может ли есть сосед слева
	test_for_dinner(RIGHT);           //Проверить может ли есть сосед справа
	LeaveCriticalSection(&cs_forks);  //Выход из критической секции
	//cout << "Philosopher number " << i << " putted down forks" << endl;
}

DWORD WINAPI philosopher(void* lParam)  //Собственно модель философа
{
	Philosopher phil = *((Philosopher*)lParam);  //Получаем модель философа
	int i = phil.number;

	while (1)    //Моделируем обед
	{
		think(phil.number);     // Думаем
		take_forks(phil.number);// Берем вилки 
		eat(phil.number);       // Едим
		put_forks(phil.number); // Кладём вилки

		Sleep(10);  //Даем время на переключение контекста
	}
}

int main()
{
	Philosopher phil[N];
	for (int i = 0; i < N; i++)  // Нумеруем философов
	{
		phil[i].number = i;
	}

	for (int i = 0; i < N; i++)  //Создаем мьютексы
	{
		philMutex[i] = CreateMutex(NULL, FALSE, NULL);
		forkMutex[i] = CreateMutex(NULL, FALSE, NULL);
	}

	InitializeCriticalSection(&cs);       //Инициализируем
	InitializeCriticalSection(&cs_forks); //критические секции

	DWORD id[N];        //Идентификаторы потоков
	HANDLE hThread[N];  //Потоки

	for (int i = 0; i < N; i++)//Создаем потоки
	{
		hThread[i] = CreateThread(NULL, NULL, &philosopher, &phil[i], NULL, &id[i]);
	}

	Sleep(INFINITE);    //Чтобы потоки успели выполнится с корректными значениями 
	while (1);
}