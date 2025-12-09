#include <iostream>
using namespace std;
//pass by value
void swap1(int a, int b){
int temp = b;
b = a;
a = temp;
}
//pass by reference using reference variable
void swap2(int &a, int &b){
int temp = b;
b = a;
a = temp;
}
//pass by reference using pointer
void swap3(int *a, int *b){
int temp = *b;
*b = *a;
*a = temp;
}
int main(){
	int a = 10, b = 20;
	swap1(a,b);
	cout<<a << “,”<<b << endl;
	swap2(a,b);
	cout << “a=” << a << “,” << “b=” << b << endl;
	swap3(&a,&b);
	cout << “a=” << a << “,” << “b=” << b << endl;
return 0;
}
