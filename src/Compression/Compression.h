#ifndef COMPRESSION_H
#define COMPRESSION_H 1

#include "BitStream.h"
#include "Matrix.h"
using namespace util;
struct send{
	int current;
	int bits;
	int nr;
};
//bevat alle functionaliteit om een image te comprimeren (dct, quantisatie, rle)
class Compression {
	public:
		Compression(string file="", int width=512, int height=512);
		Compression(int block, int qraise, int qstart,int width, int height);
		~Compression();
		int Read();
		string GetFile(){return file;}
		void setFile(string file){this->file = file;}
		void dct();
		void quantisation();
		void toArray();
		//rle+opslaan
		void rle();
		void rle2();
		void rleb();
		//maxima bepalen per diagonaal
		int* maxi();
		int toFile(string fileName);
		Matrix<int> getMatrix(){return ma;}
		void setMatrix(Matrix<int> mat){ma = mat;}
		int getHeight(){return height;}
		int getWidth(){return width;}
		void setHeight(int height){this->height = height;}
		void setWidth(int width){this->width = width;}
		void setWriter(BitStreamWriter* writer){write=writer;}
		BitStreamWriter* getWriter(){return write;}
		int getLengthFile();
		void renewMatrix();

		void setQStart(int start){qStart = start;}
		void setQRaise(int raise){qRaise = raise;}
		void setBlocksize(int blocksize){xblock = blocksize;yblock=blocksize;}

	private:
		string file; // filenaam die ingelezen is
		//breedte, hoogte image
		int width;
		int height;
		Matrix<int> ma;
		//quantisatieparamters
		int qStart;
		int qRaise;
		//blokgrootte
		int xblock;
		int yblock;
		//aantal bits per diagonaal
		int *bits;
		BitStreamWriter *write;
		//rij om rle op toe te passen
		int *rij;
		//aantal diagonalen
		int count;
};

#endif