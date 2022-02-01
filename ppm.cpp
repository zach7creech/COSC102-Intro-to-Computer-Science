/*
* Author: Zachery Creech
* COSC102 Fall 2018
* This program reads input that makes up a .ppm file, either from
* the console or from an input file. The program will invert the
* colors, or flip the picture along the x/y-axis based on user
* input. It then outputs the file to the console or an output
* file specified by the user.
*/

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct Pixel //each pixel has 3 channels, RGB
{
	unsigned int red, green, blue;
};

class Picture //represents the entire input picture
{
	vector<Pixel> pixels; //stores all the pixels of the picture in order of input reading
	unsigned int width, height, maxIntense; //stores width, height, and max intensity as given by input .ppm
public:
	Picture() //constructor initializes width, height, and max intensity to 0
	{
		width = 0;
		height = 0;
		maxIntense = 0;
	}
	const Pixel& get_pixel(int row, int column) const; //accessor; returns const reference to pixel at given row and column
	Pixel& get_pixel(int row, int column); //mutator; returns reference to pixel at given row and column
	void set_pixel(int row, int column, const Pixel &pixel); //mutator; returns nothing, sets a pixel at row and column to a new pixel value
	void invert(); //mutator; inverts colors of picture
	void flip_x(); //mutator; flips picture along x-axis
	void flip_y(); //mutator; flips picture along y-axis
	bool read_input(istream &in); //mutator; reads in input from file or console .ppm and stores it as Picture object; returns true if number of pixels read in matches the given dimensions
	void write_output(ostream &out) const; //accessor; writes Picture object to new file or console
};

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		cout << "Usage: ./lab12 <input file or '-'> <output file or '-'> <manipulator>\n";
		return -1;
	}

	ifstream fin;
	ofstream fout;
	Picture picture;
	string inFile = argv[1];
	string outFile = argv[2];
	char manipulator = '0';

	if(argc > 3) //if command line contains at least 4 arguments, take first character of 4th argument and assign it to manipulator variable
		manipulator = argv[3][0];

	if(inFile == "-") //user wants to input from console
	{
		if(!(picture.read_input(cin))) //tries to read in data
		{
			cout << "Program unable to read input. Total number of pixels read does not match given dimensions.\n";
			return -1;
		}
	}
	else //user specifies input file
	{
		fin.open(inFile);
		if(!(picture.read_input(fin))) //tries to read in data
		{
			cout << "Program unable to read file. Total number of pixels read does not match given dimensions.\n";
			return -1;
		}
		fin.close();
	}
	
	if(manipulator != '0') //if no manipulator is specified in command line, this is skipped
	{
		switch(manipulator)
		{
			case 'I':
				picture.invert();
				break;
			case 'Y':
				picture.flip_y();
				break;
			case 'X':
				picture.flip_x();
				break;
			default:
				break;
		}
	}

	if(outFile == "-") //user wants to ouput to console
	{
		picture.write_output(cout);
		return 0;
	}
	else //user wants to output to file
	{
		fout.open(outFile);
		picture.write_output(fout);
		fout.close();
		return 0;
	}
}

const Pixel& Picture::get_pixel(int row, int column) const //(width * row) + column calculates the index of the pixel in vector<Pixel> pixels at the given row and column 
{
	return pixels.at((width * row) + column);
}

Pixel& Picture::get_pixel(int row, int column)
{
	return pixels.at((width * row) + column);
}

void Picture::set_pixel(int row, int column, const Pixel &pixel)
{
	Pixel &newPixel = get_pixel(row, column);
	newPixel = pixel;
}

void Picture::invert()
{
	for(unsigned int row = 0; row < height; row++) 
		for(unsigned int column = 0; column < width; column++) //inverts every pixel in a row (row is kept same as every column is changed), then moves on to the next row
		{
			Pixel currentPixel = get_pixel(row, column); //gets a pixel to change
			currentPixel.red = maxIntense - currentPixel.red; //inversion is done by subtracting the current intensity of each color channel from the maximum intensity
			currentPixel.green = maxIntense - currentPixel.green;
			currentPixel.blue = maxIntense - currentPixel.blue;
			set_pixel(row, column, currentPixel); //sets pixel to change to the inverted pixel
		}
}

void Picture::flip_x()
{
	for(unsigned int column = 0; column < width; column++) //edits every column of a single row, then moves to the next row
	{	
		int iteration = 1; //for the pixel in the 0 row and column, the pixel that it needs to flip with is the height - 1. Each subsequent pixel down the column will flip with the pixel at height - #of subsequent rows down
		for(unsigned int row = 0; row < (height / 2); row++) //only needs to do the first half of flips, doing it again would return it to original form
		{
			Pixel orig_pixel = get_pixel(row, column); //stores original value of given pixel
			Pixel pixel_to_flip_with = get_pixel(height - iteration, column); //finds and stores the appropriate pixel for original to flip with
			set_pixel(row, column, pixel_to_flip_with); //sets original pixel to value of the pixel to flip with
			set_pixel(height - iteration, column, orig_pixel); //sets pixel to flip with to value of the original pixel
			iteration++; //increments iteration by one, making it possible to correctly choose the next pixel to flip with
		}
	}
}

void Picture::flip_y() //essentially the same as flip_x(); replace every instance of height with width, row with column, and vice versa
{
	for(unsigned int row = 0; row < height; row++)
	{
		int iteration = 1;
		for(unsigned int column = 0; column < (width / 2); column++)
		{
			Pixel orig_pixel = get_pixel(row, column);
			Pixel pixel_to_flip_with = get_pixel(row, width - iteration);
			set_pixel(row, column, pixel_to_flip_with);
			set_pixel(row, width - iteration, orig_pixel);
			iteration++;
		}
	}
}

bool Picture::read_input(istream &in)
{
	string line;
	string doc = "";
	string isP3;
	
	while(getline(in, line)) //reads in the entire .ppm input
	{
		if(line[0] == '#') //ignores all comments
			continue;
		else
			doc += (' ' + line); //each good line is fed into doc string variable that will be put in a stringstream
	}

	doc += " end"; //"end" is appended as a flag to tell the next while loop to stop trying to get pixels from file

	stringstream ss(doc);
	
	ss >> isP3;
	
	if(isP3 != "P3") //first string in the ss wil always contain P3; if not in current file, end the program
		return false;
	else
	{	
		ss >> width >> height >> maxIntense; //next 3 values in order are extracted
		pixels.reserve(width * height);

		while(true)
		{
			Pixel newPixel;
			if(ss >> newPixel.red) //when "end" is reached, this will return false and end the while loop
			{
				ss >> newPixel.green >> newPixel.blue; //red, green, and blue channels are always in this order and always come in threes
				if(newPixel.red > maxIntense) //all of the following if statements error check each channel's intensity and compares it to the given max intensity
				{
					cout << "Red color channel of intensity " << newPixel.red << " has intensity value greater than given max intensity. Value set to max intensity " << maxIntense << ".\n";
					newPixel.red = maxIntense;
				}
				if(newPixel.green > maxIntense)
				{
					cout << "Green color channel of intensity " << newPixel.green << " has intensity value greater than given max intensity. Value set to max intensity " << maxIntense << ".\n";
					newPixel.green = maxIntense;
				}
				if(newPixel.blue > maxIntense)
				{
					cout << "Blue color channel of intensity " << newPixel.blue << " has intensity value greater than given max intensity. Value set to max intensity " << maxIntense << ".\n";
					newPixel.blue = maxIntense;
				}
				pixels.push_back(newPixel);
			}
			else
				break;
		}
	}
	if(pixels.size() == width * height) //checks if given pixel dimensions in document match the number of pixels read in
		return true;
	else
		return false;
}

void Picture::write_output(ostream &out) const //ostream so that it can output to cout or fout
{
	out << "P3\n" << width << ' ' << height << '\n' << maxIntense << '\n'; //puts the 4 things in the correct order at the top of the output
	for(unsigned int i = 0; i < pixels.size(); i++) //for all pixels in the vector
	{
		Pixel pixelOut = pixels.at(i); 
		out << pixelOut.red << ' ' << pixelOut.green << ' ' << pixelOut.blue << '\n'; //outputs the color channel values with spaces between; each pixel is on its own line
	}
}
