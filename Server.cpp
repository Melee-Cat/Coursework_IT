#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <map>
#include <sstream>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define PORT 7777
#define PRINTNUSERS if (nclients)\
printf ("%d user online \n", nclients);\
else printf ("No users online\n");
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
using namespace std;
SOCKET Connection;
int nclients = 0;
HOSTENT* hst;
string username, password;
//ОПИСАНИЕ ФУНКЦИИ void SendMsg(string msg, SOCKET socket)
/*
Данная функция отправляет сообщение в виде строки msg через указанный сокет socket. Сначала определяется размер сообщения msg с помощью метода size(), затем он отправляется в виде целого числа размером sizeof(int) через функцию send.
После этого само сообщение msg отправляется как массив символов типа char* с помощью функции send, где 
первым аргументом является сокет socket, 
вторым - указатель на начало сообщения msg.c_str(), 
третьим - размер сообщения msg_size.
*/
void SendMsg(string msg, SOCKET socket)//Функция отправки сообщений
{
	int msg_size = msg.size();
	send(socket, (char*)&msg_size, sizeof(int), NULL);
	send(socket, msg.c_str(), msg_size, NULL);
}
//ОПИСАНИЕ ФУНКЦИИ string RecvMsg(SOCKET socket)
/*
Эта функция выполняет прием строковых сообщений от клиента, который подключен к серверу через сокет socket.
Алгоритм работы функции:
Получает размер сообщения в виде целого числа (int) от клиента.
Создает динамический массив символов (char*) размером msg_size + 1, где msg_size - размер сообщения, а "+1" для добавления нулевого символа в конец массива.
Принимает сообщение от клиента через сокет socket и записывает его в массив символов.
Преобразует массив символов в объект типа string и сохраняет его в переменной str_msg.
Удаляет динамически выделенный массив символов и возвращает строку str_msg.
Таким образом, функция позволяет серверу принимать строки от клиента, что может быть полезно, например, для передачи команд или текстовых сообщений между клиентом и сервером.
*/
string RecvMsg(SOCKET socket)//Функция приема строковых сообщений
{
	int msg_size = 0;
	recv(socket, (char*)&msg_size, sizeof(int), NULL);
	char* msg = new char[msg_size + 1];
	msg[msg_size] = '\0';
	recv(socket, msg, msg_size, NULL);
	string str_msg = msg;
	delete[] msg;
	return str_msg;
}
//ОПИСАНИЕ ФУНКЦИИ int RecvInt(SOCKET socket)
/*
Данная функция принимает целочисленное значение (тип int) от клиента, которое было отправлено функцией SendInt на сервере. 
Принимаемое значение сохраняется в переменную value и возвращается функцией в качестве результата.
*/
int RecvInt(SOCKET socket) {//Функция приема целочисленных сообщений
	int value = 0;
	recv(socket, (char*)&value, sizeof(int), NULL);
	return value;
}
//ОПИСАНИЕ ФУНКЦИИ bool checkLogin(string login, string password, string filename)
/*
Данный код отвечает за проверку валидности логина и пароля, хранящихся в файле. Функция принимает три параметра: логин, пароль и имя файла, в котором хранятся логины и пароли.
Сначала функция открывает файл с помощью ifstream и проверяет, удалось ли открыть файл. 
Затем функция читает построчно файл с помощью функции getline() и проверяет, совпадает ли логин и пароль, переданные в функцию, с логином и паролем, извлеченными из строки. 
Если найдено соответствие, функция возвращает true. Если прочтены все строки, и соответствие не найдено, функция возвращает false.
Функция используется для проверки подлинности логина и пароля при авторизации в системе.
*/
bool checkLogin(string login, string password, string filename) {
	ifstream file(filename);
	if (file.is_open()) {
		std::string line;
		while (getline(file, line)) {
			// Извлекаем логин и пароль из текущей строки
			istringstream iss(line);
			string savedLogin, savedPassword;
			iss >> savedLogin >> savedPassword;
			// Проверяем, совпадает ли логин и пароль с заданными
			if (login == savedLogin && password == savedPassword) {
				file.close();
				return true;
			}
		}
		file.close();
	}
	return false;
}
//ОПИСАНИЕ ФУНКЦИИ int Size(string Name)
/*
Функция Size получает название файла в качестве аргумента и возвращает его размер в байтах. 
Она открывает файл с заданным именем, перемещает указатель в конец файла с помощью функции seekg и получает текущую позицию с помощью функции tellg. 
Полученное значение является размером файла в байтах. Затем функция закрывает файл и возвращает размер.
*/
int Size(string Name)
{
	fstream File(Name);//Открытие файла
	int SizeFile;
	File.seekg(0, ios::end);//Перемещение метки в конец файла
	SizeFile = File.tellg();//Получение размера файла
	File.close();
	return SizeFile;
}
//ОПИСАНИЕ ФУНКЦИИ void HandleNumber(int number)
/*
Данная функция отвечает за запись переданного целого числа в файл "output.txt" по адресу "C:\auth". Сначала функция создает объект std::ofstream для записи в файл и проверяет, удалось ли открыть файл.
Если файл не удалось открыть, функция выводит сообщение об ошибке и прекращает свою работу. Если файл открылся успешно, функция записывает в него переданное число и закрывает файл.
*/
void HandleNumber(int number) {
	// Открываем файл output.txt в текстовом режиме для записи
	std::ofstream file("C:\\auth\\output.txt");
	if (!file) {
		std::cerr << "Ошибка открытия файла!" << std::endl;
		return;
	}

	// Записываем число в файл
	file << number;

	// Закрываем файл
	file.close();
}
//ОПИСАНИЕ ФУНКЦИИ void SendFile(SOCKET socket)
/*
Функция SendFile предназначена для отправки файла клиенту через сокет. 
Сначала функция получает имя файла и расширение файла от клиента с помощью вызова функции RecvMsg. 
Затем функция проверяет, существует ли запрошенный файл на сервере. Если файл существует, функция отправляет сообщение клиенту о том, что файл найден с помощью вызова функции SendMsg. 
Далее функция открывает файл и получает его размер, используя функцию Size. Затем функция отправляет размер файла клиенту. 
После этого функция читает файл блоками размером 1024 байт, и каждый блок отправляется клиенту с помощью send. 
В случае, если оставшийся размер файла меньше 1024 байт, он отправляется отдельно. 
Если запрошенный файл не найден на сервере, функция отправляет клиенту сообщение о том, что файл не найден с помощью вызова SendMsg.
*/
void SendFile(SOCKET socket)//Функция для отпарвки файла клиенту
{
	string Name = RecvMsg(socket);
	string Extension = RecvMsg(socket);
	string FullNameFile = "D:\\Send\\" + Name + "." + Extension;
	ifstream File(FullNameFile);
	if (File.is_open())
	{
		SendMsg("FileFound", socket);
		int SizeFile = Size(FullNameFile);//Получение размера файла
		send(socket, (char*)&SizeFile, sizeof(int), NULL);//Отправка размера файла
		ifstream File(FullNameFile, ios::in | ios::binary | ios::ate);//Открываем файл для чтения в бинарном режиме
		File.seekg(0, ios_base::beg);//Перемещение метки в начало файла
		const int SizeBuffer = 1024;//Переменная для хранения размера буфера
		char TrBuff[SizeBuffer];//Массив для хрнения данных из файла
		cout << "Запрошенный файл на передачу: " << Name << endl;
		do {
			if (SizeFile >= 1024) {
				send(socket, (char*)&SizeBuffer, sizeof(int), NULL);//Отправка размера буфера
				File.read(TrBuff, SizeBuffer);//Чтение из файла данных
				send(socket, TrBuff, SizeBuffer, NULL);//Отправка данных клиенту
			}
			else {
				send(socket, (char*)&SizeFile, sizeof(int), NULL);//Отправка размера буфера
				char* TransferBuff = new char[SizeFile + 1];
				TransferBuff[SizeFile] = '\0';
				File.read(TransferBuff, SizeFile);
				send(socket, TransferBuff, SizeFile, NULL);
				delete[] TransferBuff;
			}
			SizeFile = SizeFile - 1024;
		} while (SizeFile > 0);
		File.close();
		cout << "Файл " << Name << " успешно отправлен" << endl;
	}
	else {
		SendMsg("NoFile", socket);
	}
}
//ОПИСАНИЕ ФУНКЦИИ DWORD WINAPI ServiceToClient(LPVOID client_socket)
/*
Данная функция представляет собой обработчик клиентского соединения в отдельном потоке. Она принимает на вход указатель на сокет, созданный при подключении клиента, и работает с этим клиентом до тех пор, пока не будет получен сигнал "Exit".
Функция начинается с чтения значения переменной NFiles из файла "C:\auth\output.txt". Затем она начинает цикл, который будет продолжаться до тех пор, пока не будет получен сигнал "Exit". 
В каждой итерации цикла происходит чтение от клиента названия файла и проверка, не превышено ли количество передаваемых файлов. Если это не так, функция отправляет сообщение об ошибке и завершает работу. 
Если количество передаваемых файлов не превышено, функция отправляет клиенту сообщение "Ok_Files" и начинает передачу файла функцией SendFile.
После завершения цикла функция уменьшает количество подключенных клиентов и завершает работу.
*/
DWORD WINAPI ServiceToClient(LPVOID client_socket)// Эта функция создается в отдельном потоке и обсуживает очередного подключившегося клиента независимо от остальных
{
	SOCKET my_socket;// Создание новой переменной типа SOCKET
	my_socket = ((SOCKET*)client_socket)[0]; // Присваиваем my_socket значение client_socket
	int SRecv = 0;
	std::ifstream file("C:\\auth\\output.txt");
	if (!file) {
		std::cerr << "Ошибка открытия файла!" << std::endl;
		return 1;
	}
	// Считываем переменную NFiles из файла
	int NFiles;
	file >> NFiles;
	// Закрываем файл
	file.close();
	string StrRecv;
	do {
		recv(my_socket, (char*)&SRecv, sizeof(int), 0);
		char* CharRecv = new char[SRecv + 1]; //Выделение динамической памяти под название файла
		CharRecv[SRecv] = '\0';
		recv(my_socket, CharRecv, SRecv, 0);
		StrRecv = CharRecv;
		if (NFiles != 0) {
			SendMsg("Ok_Files", my_socket);
			if (StrRecv == "Recv") {
				SendFile(my_socket);
				NFiles--;
			}
		}
		else {
			SendMsg("Error_Send_Files", my_socket);
			cout << "Ошибка: Клиент превысил количество передаваемых файлов!" << endl;
		}
	} while (StrRecv != "Exit");
	nclients--;
	PRINTNUSERS;
	return 0;
}
//ОПИСАНИЕ int main(int argc, const char* argv[])
/*
Данный код является реализацией серверной части приложения с использованием библиотеки Winsock для работы с сетью. Он отвечает за установку соединения с клиентами, принятие запросов на аутентификацию и обслуживание подключившихся клиентов.
При запуске сервер устанавливает соединение с библиотекой Winsock и создает сокет для приема входящих подключений от клиентов. Затем сервер связывает сокет с локальным адресом, указывая номер порта и тип протокола.
После этого сервер переходит в режим ожидания подключений.
Когда клиент подключается, сервер отправляет ему количество подключенных пользователей и ожидает от него логин и пароль. Для проверки правильности аутентификации сервер использует файл с логинами и паролями.
Если аутентификация прошла успешно, сервер отправляет клиенту сообщение об успешной аутентификации, иначе клиенту отправляется сообщение об ошибке и подключение закрывается.
Если логин и пароль совпадают с логином и паролем администратора, то сервер ждет от клиента номер файла и обрабатывает его.
Если количество подключенных клиентов не превышает 100, сервер создает новый поток для обслуживания клиента. В противном случае подключение закрывается.
После завершения работы с клиентом сервер переходит к ожиданию следующего подключения.
*/
int main(int argc, const char* argv[]) {
	setlocale(LC_ALL, "");
	cout << "SERVER (Afonkin 20PT1)" << endl;
	WSAData wsaData;//Инициализация Библиотеки Сокетов
	WORD DDLVersions = MAKEWORD(2, 1); // Установка версии библиотеки
	WSAStartup(DDLVersions, &wsaData);// 1-ый параметр - версия; 2-ой параметр - ссылка на структуру
	WSAData;
	if (WSAStartup(DDLVersions, &wsaData) != 0)// Проверка подключения библиотеки
	{
		cout << "Error WSAStartup" << endl;
		return 0;
	}
	if ((Connection = socket(AF_INET, SOCK_STREAM, NULL)) < 0)// Если значения сокета установлены
	{
		WSACleanup();//Деиницилизация библиотек иWinsock
		cout << "Error socket" << endl;
		return 0;
	}
	SOCKADDR_IN address;//Связывание сокета с локальным адресом
	int sizeOFaddress = sizeof(address);//Размер адреса
	address.sin_port = htons(PORT);//Записываем номер порта
	address.sin_family = AF_INET;// Записываем идентификатор домена
	address.sin_addr.s_addr = 0;//не забываем о сетевом порядке, IP адрес хоста
	if (bind(Connection, (sockaddr*)&address, sizeof(address)))//вызываем bind для связывания
	{
		closesocket(Connection);//закрываем сокет
		WSACleanup(); //Деиницилизация библиотек иWinsock
		cout << "Error bind" << endl;
		return 0;
	}
	if (listen(Connection, 100))//ожидание подключений.
	{
		closesocket(Connection);//закрываем сокет
		WSACleanup();//Деиницилизация библиотек иWinsock
		cout << "Error listen" << endl;
		return 0;
	}
	cout << "Ожидание подключений" << endl;
	sockaddr_in client_addr;
	SOCKET client_socket{};
	int client_addr_size = sizeof(client_addr);
	while ((client_socket = accept(Connection, (sockaddr*)&client_addr, &client_addr_size)))//цикл извлечения запросов на подключение из очереди
	{
		send(client_socket, (char*)&nclients, sizeof(int), NULL);
		string login = RecvMsg(client_socket);
		string password = RecvMsg(client_socket);
		string filename = "C:\\auth\\logins.txt";
		if (checkLogin(login, password, filename)) {
			cout << "Аутентификация прошла успешно" << endl;
			SendMsg("Ok_login", client_socket);
		}
		else {
			cout << "Аутентификация не удалась" << endl;
			SendMsg("No_login", client_socket);
			nclients--;
			//PRINTNUSERS;
		}
		int NFiles;
		if (login == "admin" && password == "admin") {
			int number = RecvInt(client_socket);
			HandleNumber(number);
		}
		if (nclients < 100) {// Если количество подключеных клиентов меньше 1, то
			nclients++;//увеличиваем счетчик подключившихся клиентов
			hst = gethostbyaddr((char*)&client_addr.sin_addr.s_addr, 4, AF_INET);// Получение хоста
			printf("пользователь: %s [ IP-адрес: %s ]\n", (hst) ? hst->h_name : "",
				inet_ntoa(client_addr.sin_addr));//вывод сведений о клиенте
			PRINTNUSERS;// Вывод сведений о количестве активных клиентов
			DWORD thID;// Вызов нового потока для обслуживания клиента
			LPVOID client_Socket;

			CreateThread(NULL, NULL, ServiceToClient, &client_socket, NULL, &thID);// Создание отдельного потока для выполнения функции ServiceToClient
		}
		else {
			closesocket(client_socket);// закрываем сокет
			continue;
		}
	}
	return 0;
}