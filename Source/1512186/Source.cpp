#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"
#include "windows.h"
#include "sstream"
#include "iostream"
#include "fstream"
#include <vector>
#include <queue>
#include <process.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;


int getProtocol(string type); // kiểm tra đó phương thức được lựa chọn  là 1.0 hay 1.1
string getURL(string link);	// lấy URL 
char* convertHostNameToIP(char hostname[]); // chuyển đổi hostname sang địa chỉ IP
void deleteHeader(string &html);	// cắt bỏ phần Header trong tập tin html lấy từ web page
vector<string> findFolderAndFile(string html);	// Trong tập html, xử lý "href" để lấy ra link của folder và file lưu bỏ vào vector
vector<string> getNameFileAndFolder(string html);	// Trong tập html tiếp tục lấy ra tên hiển thị cho folder và file, lưu tên vào vector
bool checkFile(string link);	// kiểm tra với link là của file hay không?
bool checkFolder(string link);
string getNameFile(string link);	// Lấy tên file
string getHost(string link);	// lấy host từ link đã nhập
string getNameFolder(string link);	// Lấy tên folder 

void getFile(string link, string dir, string fileName, int type);
void downloadFile(string link, int type);
void downloadFolder(string link, int type);

int main(int argc, const char *argv[])
{

	cout << "==================CHUONG TRINH DOWNLOAD FILE VA FOLDER TU INTERNET===================" << endl;
	cout << "         1. Phung Thi Hoa      1512186" << endl;
	cout << "-------------------------------------------------------------------------------------" << endl;
	if (argc != 3)
	{
	cout << "Tham so truyen vao khong hop le!" << endl;
	return 0;
	}

	//string link = "http://students.iitk.ac.in/programmingclub/course/lectures/1.%20Introduction%20to%20C%20language%20and%20Linux.pdf";
	//string link = "http://students.iitk.ac.in/programmingclub/course/lectures/";
	//string link = "http://www.sample-videos.com/audio/mp3/wave.mp3";
	//string type = "--http1.0";
	
	
	string link = argv[1];
	string type = argv[2];
	
	int protocol = getProtocol(type);
	string URL = getURL(link);

	WSADATA wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		cout << "Winsock startup failed" << endl;
		return -1;
	}
	// HTTP 1.0
	if (protocol == 0)
	{
		if (link.at(link.length() - 4) == '.' || link.at(link.length() - 5) == '.')
		{
			cout << "Download file using HTTP 1.0 ..." << endl;
			downloadFile(link,protocol);

		}
		else
		{
			cout << "Download folder using HTTP 1.0 ..." << endl;
			downloadFolder(link,protocol);

		}
	}

	// HTTP1.1
	if (protocol == 1)
	{
		if (link.at(link.length() - 4) == '.' || link.at(link.length() - 5) == '.')
		{
			cout << "Download file using HTTP 1.1 ..." << endl;
			downloadFile(link, protocol);
		}
		else
		{
			cout << "Download folder using HTTP 1.1 ..." << endl;
			downloadFolder(link,protocol);
		}
	}

	WSACleanup();
	system("pause");
	return 0;
}

int getProtocol(string type)
{
	if (type.at(type.length() - 1) == '1')
		return 1;
	else if (type.at(type.length() - 1) == '0')
		return 0;
}

string getURL(string link)
{
	size_t temp = link.find("//");
	temp = (temp == string::npos) ? 0 : temp + 2;
	return link.substr(link.find('/', temp) + 1);
}

char* convertHostNameToIP(char hostname[])
{
	hostent* host;
	char* ip;
	WSADATA wsaData;
	SOCKADDR_IN sockAddr;
	if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
		return NULL; //Tra ve NULL neu nhu khong convert duoc
	}

	host = gethostbyname(hostname);
	memcpy(&(sockAddr.sin_addr), host->h_addr, host->h_length);
	ip = inet_ntoa(sockAddr.sin_addr);
	WSACleanup();
	return ip;
}

void deleteHeader(string &html)
{
	int size = html.length();
	string temp = "\r\n\r\n";
	size_t index = html.find(temp);
	index += 4;
	html = html.substr(index);
}

// Xử lý href
vector<string> findFolderAndFile(string html)
{
	int start = 0;
	int end = 0;
	vector<string> list;
	while (start != -1)
	{
		start = html.find("href=", start + 1);
		if (start == -1) // nếu trường hợp hàm find không tìm thấy
			break;

		// hàm find tìm thấy chuỗi "href=" sẽ trả về != -1
		for (int i = start + 5; i < html.length(); ++i)
		{
			if (html.at(i) == '>')
			{
				end = i;
				break;
			}
		}
		list.push_back(html.substr(start + 6, end - (start + 6) + 1 - 2));
	}
	return list;
}

vector<string> getNameFileAndFolder(string html)
{
	int s1 = 0, s2 = 0, e = 0;
	vector<string> name;
	while (s1 != -1)
	{
		s1 = html.find("href=", s1 + 1);
		if (s1 == -1) // nếu trường hợp hàm find không tìm thấy
			break;

		// hàm find tìm thấy chuỗi "href=" sẽ trả về != -1
		for (int i = s1 + 5; i < html.length(); ++i)
		{
			if (html.at(i) == '>')
			{
				s2 = i;
				break;
			}
		}
		string test1;
		test1 = html[s2 + 1];
		for (int i = s2 + 1; i < html.length(); ++i)
		{
			if (html.at(i) == '<')
			{
				e = i;
				break;
			}
		}
		string test2;
		test2 = html[e - 1];
		name.push_back(html.substr(s2 + 1, e - 1 - s2));
	}
	return name;
}

string getNameFile(string link)
{
	return link.substr(link.find_last_of('/') + 1);
}

bool checkFile(string link)
{
	if (link.at(link.length() - 4) == '.' || link.at(link.length() - 5) == '.')
		return true;
	return false;

}

bool checkFolder(string link)
{
	if (link[link.length() - 1] == '/')
		return true;
	else
		return false;
}

string getHost(string link)
{
	size_t t = link.find("//");
	t = t == string::npos ? 0 : t + 2;
	return link.substr(t, link.find('/', t) - t);
}

string getNameFolder(string link)
{
	string _link = link;
	_link = _link.erase(_link.length() - 1);
	return _link.substr(_link.find_last_of('/') + 1);
}

void downloadFile(string url, int type)
{
	//Kiem tra URL dua vao phai la file hay khong? 
	if (!checkFile(url))
	{
		return;
	}

	string host = getHost(url);
	string path = getURL(url);
	string fileName = getNameFile(url);
	//Tao ket noi socket
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	char* s = new char[host.length() + 1];
	strcpy(s, host.c_str());
	addr.sin_addr.s_addr = inet_addr(convertHostNameToIP(s));
	addr.sin_port = htons(80);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);

	//Neu khong ket noi duoc
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) {
		return;
	}
	// GET Mothod
	string get_http;
	if (type==0)
		get_http = "GET /" + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
	else if (type==1)
		get_http= "GET /" + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

	send(Connection, get_http.c_str(), strlen(get_http.c_str()), 0);

	cout << fileName << "....";
	int nDataLengh;
	char buffer[10000];
	string web_HTML = "";
	int sum = 0;
	while ((nDataLengh = recv(Connection, buffer, 10000, 0)) > 0) {
		int i = 0;
		sum += nDataLengh;
		cout << ".";
		for (int i = 0; i < nDataLengh; ++i) {
			web_HTML += buffer[i];
		}
	}

	cout << sum << " Bytes....";
	deleteHeader(web_HTML);
	// Write file
	fileName = "1512186_" + fileName;
	ofstream f;
	f.open(fileName, ofstream::binary);
	f << web_HTML;
	f.close();
	cout << "-> Dowload Successful!\n";
}

void downloadFolder(string url, int type)
{
	string host = getHost(url);
	string path = getURL(url);
	string dir = getNameFolder(url);
	dir = "1512186_" + dir;
	CreateDirectoryA(dir.c_str(), NULL);
	// Tạo kết nối socket
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	char* s = new char[host.length() + 1];
	strcpy(s, host.c_str());
	addr.sin_addr.s_addr = inet_addr(convertHostNameToIP(s));
	addr.sin_port = htons(80);
	addr.sin_family = AF_INET;
	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	//Neu khong ket noi duoc
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) {
		cout << "Failed to Connect!\n";
		return;
	}
	// get page html
	string get_http = "GET /" + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
	send(Connection, get_http.c_str(), strlen(get_http.c_str()), 0);
	int nDataLengh;
	char buffer[10000];
	string web_HTML = "";
	int sum = 0;
	while ((nDataLengh = recv(Connection, buffer, 10000, 0)) > 0) {
		int i = 0;
		for (int i = 0; i < nDataLengh; ++i) {
			web_HTML += buffer[i];
		}
	}
	closesocket(Connection);
	deleteHeader(web_HTML);

	vector<string> file;
	file.clear();
	file = findFolderAndFile(web_HTML);

	vector<string> nameFile;
	nameFile.clear();
	nameFile = getNameFileAndFolder(web_HTML);
	

	for (int i = 0; i < file.size(); i++)
	{
		if (checkFolder(file[i]))
		{
			string temp = getNameFolder(file[i]);
			CreateDirectoryA(temp.c_str(), NULL);
		}
		else
		{
			file[i] = url + file[i];
			getFile(file[i], dir, nameFile[i], type);
		}
		
	}

}

void getFile(string url, string dir, string fileName, int type)
{
	//Kiem tra URL dua vao phai la file hay khong? 
	if (!checkFile(url))
	{
		return;
	}

	string host = getHost(url);
	string path = getURL(url);

	//Tao ket noi socket
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	char* s = new char[host.length() + 1];
	strcpy(s, host.c_str());
	addr.sin_addr.s_addr = inet_addr(convertHostNameToIP(s));
	addr.sin_port = htons(80);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);

	//Neu khong ket noi duoc
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) {
		return;
	}
	// GET Mothod
	string get_http;
	if (type == 0)
		get_http = "GET /" + path + " HTTP/1.0\r\nHost: " + host + "\r\n\r\n";
	else if (type == 1)
		get_http = "GET /" + path + " HTTP/1.1\r\nHost: " + host + "\r\nConnection: close\r\n\r\n";

	send(Connection, get_http.c_str(), strlen(get_http.c_str()), 0);

	cout << fileName << "....";
	int nDataLengh;
	char buffer[10000];
	string web_HTML = "";
	int sum = 0;
	while ((nDataLengh = recv(Connection, buffer, 10000, 0)) > 0) {
		int i = 0;
		sum += nDataLengh;
		cout << ".";
		for (int i = 0; i < nDataLengh; ++i) {
			web_HTML += buffer[i];
		}
	}

	cout << sum << " Bytes....";
	deleteHeader(web_HTML);
	// Write file
	fileName = dir + "\\" + fileName;
	ofstream f;
	f.open(fileName, ofstream::binary);
	f << web_HTML;
	f.close();
	cout << "-> Dowload Successful!\n";
}

