#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>

#define LIMIT ( 65536 - 1 )
#define BASE_FREQUENCY 1193180
#define COUNT 9
#define TIME_RUN 65536

int counter;
int index;
int frequency[COUNT] = {392, 329, 329, 329, 329, 329, 392, 349, 329};

void soundGeneration();
void divisionCoefficient();
void statusWord();
void randomNumber();

void main()
{
	char choice;
	do
	{
		system("cls");
		printf("1) Sound generation\n2) Coefficient of division\n3) Status word\n4) Random number\n0) Exit\n");
		rewind(stdin);
		printf("\nYour choise: ");
		scanf("%c", &choice);

		switch (choice) {
		case '1':
			soundGeneration();
			break;
		case '2':
			divisionCoefficient();
			break;
		case '3':
			statusWord();
			break;
		case '4':
			randomNumber();
			break;
		}
	} while (choice != '0');

	outp(0x43, 0xB4);
	outp(0x42, LIMIT % 256);
	outp(0x42, LIMIT / 256);
}

void soundGeneration()
{
	int frequency[COUNT] = {392, 329, 329, 329, 329, 329, 392, 349, 329};
	int durability[COUNT] = {400, 400, 400, 400, 400, 400, 400, 400, 400};

	int frequencyCounter, kd;

	for (frequencyCounter = 0; frequencyCounter < COUNT; frequencyCounter++)
	{
		outp(0x43, 0xB6);	// 10 11 011 0	{ Вывод на динамик, чтение старшего и младнего байт, Прямоугольные импульсы, двоичный режим }
		kd = BASE_FREQUENCY / frequency[frequencyCounter];	// Во сколько раз частота меньше базовой
		outp(0x42, kd % 256);	// Передаём младший байт kd
		kd /= 256;	// Сдвигаем kd на 8 бит вправо
		outp(0x42, kd);	// Передаём старший байт kd

		outp(0x61, inp(0x61) | 3);	// Выставляем последние 2 бита в единицу
		delay(durability[frequencyCounter]);	// Приостанавливаем программу (динамик работает)
		outp(0x61, inp(0x61) & 0xFC);	// Отключаем динамик и второй канал
	}
}

void divisionCoefficient()
{
	unsigned long j;
	int Channel;
	int ports[] = { 0x40, 0x41, 0x42 };
	int controlWords[] = { 0, 64, 128 };	// 00 00 000 0, 01 00 000 0, 10 00 000 0
	// { Работа с первым каналом, блокируем счетчик, вызываем IRQ0 прерывание, двоичный формат }
	// { Работа со вторым каналом, блокируем счетчик, вызываем IRQ0 прерывание, двоичный формат }
	// { Работа с третьим каналом, блокируем счетчик, вызываем IRQ0 прерывание, двоичный формат }
	unsigned kdLow, kdHigh, kd, max;

	printf("\nCoefficient of division:\n");

	for (Channel = 0; Channel < 3; Channel++)
	{
		kd = 0;
		max = 0;

		if (Channel == 2)	// Если второй канал
			outp(0x61, inp(0x61) | 3);	// Включаем второй канал и динамик

		for (j = 0; j < TIME_RUN; j++)
		{
			outp(0x43, controlWords[Channel]);
			kdLow = inp(ports[Channel]);
			kdHigh = inp(ports[Channel]);
			kd = kdHigh * 256 + kdLow;

			if (kd > max)
				max = kd;
		}
		if (Channel == 2)
			outp(0x61, inp(0x61) & 0xFC);	// Выключаем второй канал и динамик

		printf("\nChannel %d: %4X\n", Channel, max);
		system("pause");
	}
}

void statusWord()
{
	printf("\nStatus word:\n");
	char stateWord[8];
	int Channel, state;
	int ports[] = { 0x40, 0x41, 0x42 };
	int controlWords[] = { 226, 228, 232 };	// 11 1 0 001 0, 11 1 0 010 0, 11 1 0 100 0
	// { Значения из регистра каналов, 1 не фиксирует значение счетчика, состояние канала, 0 канал, 0 }
	// { Значения из регистра каналов, 1 не фиксирует значение счетчика, состояние канала, 1 канал, 0 }
	// { Значения из регистра каналов, 1 не фиксирует значение счетчика, состояние канала, 2 канал, 0 }

	for (Channel = 0; Channel < 3; Channel++)
	{
		outp(0x43, controlWords[Channel]);	// Требуется получить состояния регистров
		state = inp(ports[Channel]);	// Получаем состояние i-ого регистра

		for (int i = 7; i >= 0; i--)
		{
			stateWord[i] = state % 2 + '0';
			state /= 2;
		}
		printf("\nChannel %d: ", Channel);
		for (i = 0; i < 8; i++)
			printf("%c", stateWord[i]);
		printf("\n");
		system("pause");
	}
}

void randomNumber()
{
	char choice;
	unsigned int limit = TIME_RUN - 1, numberLow, numberHigh, number;

	do
	{
		printf("\n1) Set a limit\n2) Get a number\n0) Exit\n");
		rewind(stdin);
		printf("\nYour choise: ");
		scanf("%c", &choice);

		switch (choice) {
		case '1':
			do
			{
				printf("\nEnter a limit in range [1...65635]:\n");
				rewind(stdin);
			} while (!scanf("%d", &limit) || limit < 1);

			outp(0x43, 0xB4);	// 10 11 010 0 { Выбор второго канала, чтение и запись 2 байт, установка генератора импульсов, двоичный формат }
			outp(0x42, limit % 256);	// Помещаем младший байт во второй канал
			limit /= 256;	// Сдвигаем на 8 бит вправо
			outp(0x42, limit);	// Помещаем старший байт во второй канал
			outp(0x61, inp(0x61) | 1);	// Включаем второй канал
			break;

		case '2':
			//outp(0x43, 128);	// 10 00 000 0 { Выбор второго канала, блокировка счетчика, генерация IRQ0 при счетчике = 0, двоичный формат }
			outp(0x43, 216);
			numberLow = inp(0x42);	// Младший байт
			numberHigh = inp(0x42);	// Старший байт
			number = numberHigh * 256 + numberLow;	// Формируем число из диапазона

			printf("\nRandom number: %u\n", number);
			break;
		}
	} while (choice != '0');
	outp(0x61, inp(0x61) & 0xFC);	// Отключаем динамик и второй канал
}