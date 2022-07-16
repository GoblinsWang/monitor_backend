#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;

int main()
{
    string str = "helloworld";
    cout << "str len:" << str.length() << endl;

    while (1)
    {
        char c = getchar();
        if (c == '1')
        {
            string str = "helloworld";
            cout << "str len:" << str.length() << endl;

            char *data = new char[str.length()];

            memcpy(data, str.data(), str.length());
            cout << "test data:" << data << endl;

            delete[] data;
        }
        if (c == '2')
        {
            string str = "hello";
            cout << "str len:" << str.length() << endl;

            char *data = new char[str.length()];

            memcpy(data, str.data(), str.length());
            cout << "test data:" << data << endl;
            delete[] data;
        }
    }
}
