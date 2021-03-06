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
		outp(0x43, 0xB6);	// 10 11 011 0	{ ����� �� �������, ������ �������� � �������� ����, ������������� ��������, �������� ����� }
		kd = BASE_FREQUENCY / frequency[frequencyCounter];	// �� ������� ��� ������� ������ �������
		outp(0x42, kd % 256);	// ������� ������� ���� kd
		kd /= 256;	// �������� kd �� 8 ��� ������
		outp(0x42, kd);	// ������� ������� ���� kd

		outp(0x61, inp(0x61) | 3);	// ���������� ��������� 2 ���� � �������
		delay(durability[frequencyCounter]);	// ���������������� ��������� (������� ��������)
		outp(0x61, inp(0x61) & 0xFC);	// ��������� ������� � ������ �����
	}
}

void divisionCoefficient()
{
	unsigned long j;
	int Channel;
	int ports[] = { 0x40, 0x41, 0x42 };
	int controlWords[] = { 0, 64, 128 };	// 00 00 000 0, 01 00 000 0, 10 00 000 0
	// { ������ � ������ �������, ��������� �������, �������� IRQ0 ����������, �������� ������ }
	// { ������ �� ������ �������, ��������� �������, �������� IRQ0 ����������, �������� ������ }
	// { ������ � ������� �������, ��������� �������, �������� IRQ0 ����������, �������� ������ }
	unsigned kdLow, kdHigh, kd, max;

	printf("\nCoefficient of division:\n");

	for (Channel = 0; Channel < 3; Channel++)
	{
		kd = 0;
		max = 0;

		if (Channel == 2)	// ���� ������ �����
			outp(0x61, inp(0x61) | 3);	// �������� ������ ����� � �������

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
			outp(0x61, inp(0x61) & 0xFC);	// ��������� ������ ����� � �������

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
	// { �������� �� �������� �������, 1 �� ��������� �������� ��������, ��������� ������, 0 �����, 0 }
	// { �������� �� �������� �������, 1 �� ��������� �������� ��������, ��������� ������, 1 �����, 0 }
	// { �������� �� �������� �������, 1 �� ��������� �������� ��������, ��������� ������, 2 �����, 0 }

	for (Channel = 0; Channel < 3; Channel++)
	{
		outp(0x43, controlWords[Channel]);	// ��������� �������� ��������� ���������
		state = inp(ports[Channel]);	// �������� ��������� i-��� ��������

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

			outp(0x43, 0xB4);	// 10 11 010 0 { ����� ������� ������, ������ � ������ 2 ����, ��������� ���������� ���������, �������� ������ }
			outp(0x42, limit % 256);	// �������� ������� ���� �� ������ �����
			limit /= 256;	// �������� �� 8 ��� ������
			outp(0x42, limit);	// �������� ������� ���� �� ������ �����
			outp(0x61, inp(0x61) | 1);	// �������� ������ �����
			break;

		case '2':
			//outp(0x43, 128);	// 10 00 000 0 { ����� ������� ������, ���������� ��������, ��������� IRQ0 ��� �������� = 0, �������� ������ }
			outp(0x43, 216);
			numberLow = inp(0x42);	// ������� ����
			numberHigh = inp(0x42);	// ������� ����
			number = numberHigh * 256 + numberLow;	// ��������� ����� �� ���������

			printf("\nRandom number: %u\n", number);
			break;
		}
	} while (choice != '0');
	outp(0x61, inp(0x61) & 0xFC);	// ��������� ������� � ������ �����
}