#ifndef DECOMPRESSION_H
#define DECOMPRESSION_H 1

#include "BitStream.h"
using namespace util;
#include "Matrix.h"

//bevat functies om te decomprimeren
class Decompression {
	public:
		Decompression(string file="");
		Decompression(BitStreamWriter *writer);
		~Decompression();
		void Read();
		void Read2();
		int Open();
		string GetFile(){return file;}
		void setFile(string file){this->file = file;}
		void setWriter(BitStreamWriter *writer){this->write = writer;}
		void antiDct();
		void antiQuantisation();
		void toMatrix();
		void antiRle();
		int toFile(string str);

		int getHeight(){return height;}
		int getWidth(){return width;}
		int getBlocksize(){return xblock;}
		int getQStart(){return qStart;}
		int getQRaise(){return qRaise;}
		void setHeight(int height){this->height = height;}
		void setWidth(int width){this->width = width;}
		Matrix<int> getMatrix(){return *ma;}
		void setReader(BitStreamReader* read){this->read = read;}
		void deletes();
		void deleteWriter();
		void Pinitiate(int block, int qstart, int qraise, int macro);

	private:
		string file; // filenaam die ingelezen is
		int width;
		int height;
		Matrix<int> *ma;
		//quantisatie
		int qStart;
		int qRaise;
		//bits per diagonaal
		int* bits;
		//blokgrootte
		int xblock;
		int yblock;
		BitStreamWriter *write;
		BitStreamReader *read;
		int *rij;
		int count;
		int blbits;
};

#endif