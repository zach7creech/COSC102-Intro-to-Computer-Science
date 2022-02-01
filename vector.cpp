/*
* Author: Zachery Creech
* COSC102 Fall 2018
* This program is a basic version of the <vector> library. Allows user
* to create vectors, resize them, and push_back elements.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdexcept>

using namespace std;

class doublevector {
     static constexpr double DOUBLE_DEFAULT_VALUE = -5.55;
     double *mValues;
     int mNumValues;
public:
     doublevector();
     ~doublevector();
     void resize(int new_size, double initial_value=DOUBLE_DEFAULT_VALUE);
     void push_back(double value);
     double &at(int index);
     const double &at(int index) const;
     unsigned long size() const;
};
void print_all(const doublevector &v)
{
     if (v.size() == 0) {
           cout << "Vector is empty.\n";
     }
     else {
          unsigned long i;
          for (i = 0;i < v.size();i++) {
                cout << "[" << setfill('0') << right << setw(3) << i
                     << "] = " << fixed << setprecision(4) << v.at(i)
                     << '\n';
          }
     }
}
int main()
{
      doublevector v;
      do {
           string command;
           cout << "Enter a command ('quit' to quit): ";
           if (!(cin >> command) || command == "quit") {
                break;
           }
           else if (command == "push_back") {
                double value;
                cin >> value;
                v.push_back(value);
		cout << "Pushed back "
		     << fixed << setprecision(4)
		     << value << '\n';
           }
           else if (command == "resize") {
                string line;
                int new_size;
                double initial;
                cin >> new_size;
                getline(cin, line);
                istringstream sin(line);

                if (sin >> initial)
                     v.resize(new_size, initial);
                else
                     v.resize(new_size);
           }
           else if (command == "size") {
                cout << v.size() << '\n';
           }
           else if (command == "print") {
                print_all(v);
           }
           else if (command == "get") {
                int index;
                cin >> index;
                try {
                     cout << "Value at " << index << " = "
		          << setprecision(4) << fixed
                          << v.at(index) << '\n';;
                }
                catch(out_of_range &err) {
                     cout << err.what() << '\n';
                }
           }
           else if (command == "set") {
		   double d;
		   int index;
                   cin >> index >> d;
                   try {
			v.at(index) = d;
                        cout << "SET: " << index << " = "
			     << setprecision(4) << fixed
                             << v.at(index) << '\n';;
                   }
                   catch(out_of_range &err) {
                        cout << err.what() << '\n';
                   }
           }
           else if (command == "clear") {
                v.resize(0);
           }
           else {
                cout << "Invalid command '" << command << "'\n";
           }
      } while(true);
      cout << endl;
      return 0;
}
//Write your class members below here.
doublevector::doublevector() //constructor; sets appropriate values
{
	mValues = nullptr;
	mNumValues = 0;
}

doublevector::~doublevector() //destructor; deletes memory allocated for mValues if mValues != nullptr and sets mNumValues = 0
{
	if(mValues != nullptr)
	{	
		delete [] mValues;
		mNumValues = 0;
	}
}

double& doublevector::at(int index) //mutator version of at(); returns editable value at given index
{
	if(index >= mNumValues)
	{
		ostringstream sout;
		sout << "Invalid index #" << index;
		throw out_of_range(sout.str());
	}
	else
		return mValues[index];
}
const double& doublevector::at(int index) const //accessor version of at(); returns read-only value at given index
{
	if(index >= mNumValues)
	{
		ostringstream sout;
		sout << "Invalid index #" << index;
		throw out_of_range(sout.str());
	}
	else
		return mValues[index];
}

unsigned long doublevector::size() const
{
	return mNumValues; //returns mNumValues as a long instead of an int
}

void doublevector::resize(int new_size, double initial_value)
{
	if(new_size == 0 && mValues != nullptr) //checks for mValues = nullptr before deleting
	{	
		delete [] mValues; //frees memory from mValues
		mNumValues = 0;
	}
	else if(new_size > mNumValues)
	{
		double *new_mValues = new double[new_size]; //allocates new memory for copied values
		for(int i = 0; i < mNumValues; i++) //copies initial values into new memory
			new_mValues[i] = mValues[i];
		if(mValues != nullptr) //checks for mValues = nullptr before deleting
			delete [] mValues;
		for(int i = mNumValues; i < new_size; i++) //sets the rest of the values to user-chosen initial value
			new_mValues[i] = initial_value;
		mValues = new_mValues; //sets member variable equal to new pointer
		mNumValues = new_size; //sets mNumValues to resized size
	}
	else if(new_size < mNumValues)
	{
		double *new_mValues = new double[new_size]; //allocates new memory for copied values
		for(int i = 0; i < new_size; i++) //copies initial values up to new size
			new_mValues[i] = mValues[i];
		if(mValues != nullptr) //checks for mValues = nullptr before deleting
			delete [] mValues;
		mValues = new_mValues; //sets member variable equal to new pointer
		mNumValues = new_size; //sets mNumValues to resized size
	}
}

void doublevector::push_back(double value)
{
	resize(mNumValues + 1, value); //uses resize(); makes the vector bigger by 1 and adds the new value to the next memory space
}
