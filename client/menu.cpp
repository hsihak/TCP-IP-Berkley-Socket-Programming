#include "menu.h"

void menu(void)  //menu function definition which prints out the menu options
{
	cout << "---------------------------------------- " << endl;
	cout << "Please choose from the menu below : " << endl;
	cout << "a) Submit a post" << endl;
	cout << "b) View all posts" << endl;
	cout << "c) Find a post" << endl;
	cout << "d) QUIT \n" << endl;
	cout << "------------------------------------------\n" << endl;

}

char menuinput(void) //menuinput function definition
{
	char option;
	cin >> option;
	return option;

}


void filtermenu(void)
{
	cout << "---------------------------------------- " << endl;
	cout << "Please choose which option you wish to filter by : " << endl;
	cout << "a) Author" << endl;
	cout << "b) Topic" << endl;
	cout << "c) Get back to the previous menu \n" << endl;
	cout << "------------------------------------------\n" << endl;

}
