#include <iostream>

using namespace std;

bool CheckAddress(char* ip_)
{
	int points = 0, // количество точек
		numbers = 0; // значение октета

	char* buff; // буфер для одного октета

	buff = new char[3];

	for (int i = 0; ip_[i] != '\0'; i++)
	{ // для строки IP-адреса
		if (ip_[i] <= '9' && ip_[i] >= '0') // если цифра
		{
			if (numbers > 3) return false;
			//если больше трех чисел в октете – ошибка
			buff[numbers++] = ip_[i];
			//скопировать в буфер
		}
		else
			if (ip_[i] == '.') // если точка
			{
				if (atoi(buff) > 255)
					// проверить диапазон октета
					return false;

				if (numbers == 0)
					//если числа нет - ошибка
					return false;

				numbers = 0;
				points++;
				delete[]buff;
				buff = new char[3];
			}
			else return false;
	}
	if (points != 3)
		// если количество точек в IP-адресе не 3 - ошибка
		return false;

	if (numbers == 0 || numbers > 3)
		return false;

	return true;
}

unsigned long CharToLong(char* ip_)
{
	unsigned long out = 0;//число для IP-адреса
	char* buff = new char[3];
	//буфер для хранения одного октета

	for (int i = 0, j = 0, k = 0; ip_[i] != '\0'; i++, j++)
	{
		if (ip_[i] != '.') //если не точка
			buff[j] = ip_[i]; // записать символ в буфер

		if (ip_[i] == '.' || ip_[i + 1] == '\0')
		{
			// если следующий октет или последний
			out <<= 8; //сдвинуть число на 8 бит

			if (atoi(buff) > 255)
				return NULL;
			// еcли октет больше 255 – ошибка

			out += (unsigned long)atoi(buff);
			//преобразовать и добавить
			//к числу IP-адреса

			k++;
			j = -1;
			delete[]buff;
			buff = new char[3];
		}
	}
	return out;
}

bool CheckMask(unsigned long mask)
{
	if (mask == 0)
		return false;

	unsigned long maskTest = 1 << 31;

	for (int i = 0; i < 32; i++)
	{
		if (!(mask & maskTest))
		{
			for (int j = i + 1; j < 32; j++)
			{
				mask <<= 1;
				if (mask & maskTest) 
					return false;
			}
		}
		mask <<= 1;
	}

	return true;
}

void WriteLongToChar(unsigned long ip)
{
	unsigned long oktet[4];
	oktet[0] = ip >> 24;
	oktet[1] = (ip << 8) >> 24;
	oktet[2] = (ip << 16) >> 24;
	oktet[3] = (ip << 24) >> 24;

	cout << oktet[0] << "." << oktet[1] << "." << oktet[2] << "." << oktet[3] << "\n";
}

int main()
{
	setlocale(LC_ALL, "Russian");

	unsigned long ip, mask, host, subnet, broadcast;
	char* ip_, * mask_;
	bool flag = true;
	ip_ = new char[16];
	mask_ = new char[16];

	do
	{
		if (!flag) cout << "\nНеверно введён адрес!\n" << endl;
		cout << "Введите IP - адрес: \n> ";
		cin >> ip_;
	} while (!(flag = CheckAddress(ip_)));
	ip = CharToLong(ip_);
	flag = true;


	do
	{
		if (!flag) 
			cout << "\nНеправильная маска!\n" << endl;
		flag = true;
		do
		{
			if (!flag) 
				cout << "\nНеверно введена маска!\n" << endl;
			cout << "Введите маску подсети: \n> ";
			cin >> mask_;
		} while (!(flag = CheckAddress(mask_)));
		mask = CharToLong(mask_);
	} while (!(flag = CheckMask(mask)));


	subnet = ip & mask;
	host = ip & ~mask;
	broadcast = ip & mask | ~mask;

	cout << "ID подсети:\t\t";
	WriteLongToChar(subnet);

	cout << "ID хоста:\t\t";
	WriteLongToChar(host);

	cout << "Broadcast адрес:\t";
	WriteLongToChar(broadcast);

	return 0;
}