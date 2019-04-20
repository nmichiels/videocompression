#include "BitStream.h"
using namespace util;
#include <stdio.h>
#include <stdlib.h>
#include <Math.h>


/* BitStreamReader */

BitStreamReader::BitStreamReader(unsigned __int8 *buffer, int size) : BitStream(buffer, size)
{

}

BitStreamReader::BitStreamReader(BitStreamWriter &b)
{
	this->buffer = b.get_buffer();
	this->size = b.get_size();
	this->position = 0;
	this->managed = false;
}

BitStreamReader::~BitStreamReader(){}

unsigned __int8 BitStreamReader::get_bit()
{
	if (position >= (this->size*8)){
		cerr << "Error: Buffer overflow." << endl;
		return 0;
	}
	int cel = position/8;
	// als je de bv de derde bit van een byte wil opvragen wordt dit 00100000
	unsigned __int8 bit = pow((double)2,(double)(7-(position%8)));
	position++;
	if ( ((buffer[cel])&(bit)) > 0)
		return 1;
	else
		return 0;
}

unsigned __int32 BitStreamReader::get(int l)
{
	if (l <= 0)
		return 0;
	if (l > 32)
		l = 32;
	int value = 0;
	for (unsigned int i=pow(double(2),double(l-1)); i>0; i /= 2){
		if (get_bit() > 0){
			value += i;
		}
	}
	return value;
}

void BitStreamReader::flush()
{
	this->position = (position-(position%8)+8)%(size*8);
}



/* BitStreamWriter */

BitStreamWriter::BitStreamWriter(unsigned __int8 *buffer, int size) : BitStream(buffer, size)
{
	//buffer = new unsigned __int8[size];
	for (int i=0; i<(size/8); i++)
	{
		buffer[i] = (unsigned __int8) 0;
	}
}



BitStreamWriter::BitStreamWriter(int s)
{
	buffer = new unsigned __int8[s];
	for (int i=0; i<(s/8); i++)
	{
		buffer[i] = (unsigned __int8) 0;
	}
	this->size = s;
	this->position = 0;
	this->managed = false;
}


BitStreamWriter::~BitStreamWriter()
{
		delete[] buffer;
}


void BitStreamWriter::put_bit(unsigned __int8 value)
{
	if (position >= (this->size*8)){
		cerr << "Error: Buffer overflow." << endl;
		return;
	}
	int cel = position/8;
	// als je de bv de derde bit van een byte wil opvragen wordt dit 00100000
	int bit = pow((double)2,(double)(7-(position%8)));
	if (value == 1)
		buffer[cel] = (buffer[cel])|(bit);
	else
		buffer[cel] = ((buffer[cel])&(~(bit)));
	position++;
}

void BitStreamWriter::put(int length, unsigned __int32 value)
{
	for(;length>32;length--)
	put_bit(0);

	unsigned __int32 hulp = pow(2.0,double(length-1));

	//value = value % (int)pow(double(2),double(length)); // overbodige bits afkappen

	for (unsigned __int32 i=hulp; i>0; i /= 2){
		if (value >= i){
			value -= i;
			put_bit(1);
		}
		else {
			put_bit(0);
		}
	}
}


void BitStreamWriter::flush()
{
	while ((position%8) != 0){
		put_bit(0);
	}
}

void util::write(FILE *f, const BitStreamWriter &b)
{
	const unsigned __int8* a=b.get_buffer();
	int size=b.get_position()/8;
	
	for(int i=0;i<size;i++){
		putc((const char)a[i], f);
	}
}