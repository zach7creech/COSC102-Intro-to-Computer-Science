/*
* Author: Zachery Creech
* COSC102 Fall 2018
* This program recieves input from a file containing ticket information;
* that information is read then stored in a police database, where they
* are then sorted by a user-provided timeframe. Tickets that fall within
* the user-provided timeframe are either written to an output file or
* printed to the console, which is also decided by the user.
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

struct Date //stores a date as three separate integers corresponding to the month, day, and year
{
	int month;
	int day;
	int year;
};

struct Ticket //stores a ticket that has a unique citation ID, a Date structure, a clocked speed, the speed limit, and the road type
{
	string citation;
	Date date;
	int clockSpeed;
	int limitSpeed;
	char rdType;
};

class Database //a database contains a vector member that stores all read tickets from the input file
{
	vector<Ticket> ticketList;
public:
	void add_ticket(const Ticket &ticket); //adds a read ticket to the vector member
	vector<Ticket> gen_report(Date start, Date end) const; //generates the report given a user timeframe, returns another vector of valid tickets
};

bool Read(Database &d, istream &in);
bool Write(const vector<Ticket> &tickets, ostream &out);
int Encode(const Date &d);
void changeToInt(string date, Date &startOrEnd);
double calculateFine(const Ticket &ticket);

const double i = 5.2252, h = 9.4412, r = 17.1525, n = 12.152;
const string month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

int main(int argc, char* argv[])
{
	ifstream fin;
	ofstream fout;
	Date start, end; //user-provided start and end dates for report generation
	Database database;
	vector<Ticket> validTickets; //where valid tickets are stored

	if(argc < 5) //if user does not provide the minimum number of command line arguments
	{
		cout << "Usage: ./lab11 <input file> <start: year-month-day> <end: year-month-day> <output or '-'>\n";
		return -1;
	}

	changeToInt(argv[2], start); //argv[2] is the user-provided start date, changeToInt() changes that string to int and stores it in a Date structure
	changeToInt(argv[3], end); //argv[3] is the user-provided end date
	
	string inFile = argv[1];
	string outFile = argv[4];
	
	fin.open(inFile);
	
	if(Read(database, fin)) //if successfully read into database, generate a report
	{
		validTickets = database.gen_report(start, end);
		if(outFile != "-") //chooses output file, if user inputs '-' then the output is the console
		{	
			fout.open(outFile);
			if(Write(validTickets, fout)) //if successfully written, close the files and end program
			{
				fin.close();
				fout.close();
				return 0;
			}
			else //else end program and give error
				{
					cout << "Unable to open output file.\n";
					return -1;
				}
		}
		else if(outFile == "-")
		{
			if(Write(validTickets, cout))
			{	
				fin.close();
				return 0;
			}
			else
			{
				cout << "Unable to write tickets to console.\n";
				return -1;
			}
		}
	}
	else
	{
		cout << "The file '" << inFile << "' cannot be read.\n";
		return -1;
	}
}

bool Read(Database &d, istream &in) //reads ALL tickets into the database, stored as Ticket struct in vector ticketList in Database class
{
	string line;

	if(in)
	{	
		while(getline(in, line)) //reads in an entire citation line
		{
			Ticket ticket;
			stringstream ss(line);
			ss >> ticket.citation >> ticket.date.month >> ticket.date.day >> ticket.date.year >> ticket.clockSpeed >> ticket.limitSpeed >> ticket.rdType;
			if(ticket.date.year < 100) //ticket years that are two digits are assumed to be in the 21st century
				ticket.date.year += 2000;
			d.add_ticket(ticket);
		}
		return true;
	}
	else
		return false;
}

bool Write(const vector<Ticket> &tickets, ostream &out) //writes all relevant data to the output stream, which in this case can be a file or the console
{
	Ticket currentTicket;
	double fine;
	if(out)
	{	
		for(unsigned int i = 0; i < tickets.size(); i++) 
		{
			currentTicket = tickets.at(i); //of the valid tickets, declares the current ticket in the vector to be output
			fine = calculateFine(currentTicket);
			out << setw(2) << right << setfill('0') << currentTicket.date.day;
			out << '-' << month[currentTicket.date.month - 1] << '-' << currentTicket.date.year << ' ' << currentTicket.citation;
			out << "    " << '$' << setw(9) << setfill(' ') << right << setprecision(2) << fixed << showpoint << fine << '\n';
		}
		return true;
	}
	else
		return false;
}

int Encode(const Date &d) //changes a XXXX-XX-XX date to a single int to make comparison simpler
{		
	int encodedDate = (d.year * 1000) + (d.month * 100) + d.day;
	return encodedDate;
}

void Database::add_ticket(const Ticket &ticket) //adds a ticket to vector ticketList in class Database
{
	ticketList.push_back(ticket);
}

vector<Ticket> Database::gen_report(Date start, Date end) const //generates a vector that contains tickets which fall within the specified date bounds
{
	vector<Ticket> validTickets;
	int startDate = Encode(start); 
	int endDate = Encode(end);

	for(unsigned int i = 0; i < ticketList.size(); i++)
	{
		int ticketDate = Encode((ticketList.at(i)).date);
		if(ticketDate >= startDate && ticketDate <= endDate) //if current ticket falls within date bounds, add to vector of valid tickets to be returned
			validTickets.push_back(ticketList.at(i));
	}
	return validTickets;
}

void changeToInt(string date, Date &startOrEnd) //converts a XXXX-XX-XX string date from input file into int type to be stored and manipulated through Data structure
{
	vector<int> dateInts; //vector that stores converted integer values; 0 = year, 1 = month, 2 = day
	unsigned int inString = 0; //when a hyphen is encountered the loop is broken; this variable stores where the loop left off so it doesn't start over from 0
	int dateInt; //stores int that is converted from a string from the stringstream
	
	for(int i = 0; i < 3; i++) //0 = year, 1 = month, 2 = day
	{
		string datePiece = "";
		for(unsigned int j = inString; j < date.size(); j++) //parses the entire input string, when it reaches a hyphen it moves to the next date piece and continues where it left off
		{
			if(date[j] == '-') 
			{	
				inString = j + 1;
				stringstream ss(datePiece); //converts datePiece string collected so far into type int
				ss >> dateInt;
				dateInts.push_back(dateInt); //dateInt is put in vector
				break;
			}
			datePiece += date[j];
			if(j == date.size() - 1) //final number (day) is not followed by a hyphen, so this makes sure the final number (day) is pushed back into the dateInts vector
			{
				stringstream ss(datePiece);
				ss >> dateInt;
				dateInts.push_back(dateInt);
			}
		}
	}
	startOrEnd.year = dateInts.at(0); //stores the vector values into the corresponding member of Data structure
	startOrEnd.month = dateInts.at(1);
	startOrEnd.day = dateInts.at(2);
}

double calculateFine(const Ticket &ticket) //calculates the fine
{
	double fine;

	switch(ticket.rdType)
	{
		case 'i': case 'I': //'i' or 'I' = interstate multiplier
			fine = (ticket.clockSpeed - ticket.limitSpeed) * i;
			break;
		case 'h': case 'H': //'h' or 'H' = highway multiplier
			fine = (ticket.clockSpeed - ticket.limitSpeed) * h;
			break;
		case 'r': case 'R': //'r' or 'R' = residential multiplier
			fine = (ticket.clockSpeed - ticket.limitSpeed) * r;
			break;
		default: //unspecified multiplier defaults to 'n' multiplier
			fine = (ticket.clockSpeed - ticket.limitSpeed) * n;
			break;
	}
	if(fine < 0) //fine cannot be negative, if negative round to 0
		fine = 0;
	return fine;
}
