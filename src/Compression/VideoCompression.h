#ifndef VIDEO_COMPRESSION_H
#define VIDEO_COMPRESSION_H

#include "BitStream.h"
#include "Matrix.h"
#include "Compression.h"
#include <iomanip>
using namespace util;

#define MAXP 2
#define MADR 5

class VideoCompression {
	public:
		VideoCompression(string inputFile="", string outputFile="", bool color=false,int height=512, int width=512, int GOP=1,int maxMad=7,int maxP=6, int macro=16, bool difference=false, int block=8, int qstart=2, int qraise=2);
		~VideoCompression();

		// get and set functies op klassevariabelen
		string getInputFile(){return inputFile;}
		void setInputFile(string inputFile){this->inputFile = inputFile;}
		string getOutputFile(){return outputFile;}
		void setOutputFile(string outputFile){this->outputFile = outputFile;}
		int getHeight(){return height;}
		int getWidth(){return width;}
		void setHeight(int height);
		void setWidth(int width);
		int getGOP(){return GOP;}
		void setGOP(int GOP){this->GOP = GOP;}
		BitStreamWriter* getWriter(){return write;}

		// De compressie starten met de huidige gegevens van file en parameters
		void startCompression();

	private:
		// afbeelding gegevens
		string inputFile;	// filenaam die ingelezen is
		string outputFile;	// filenaam voor uit te lezen
		int width;
		int height;
		int maxFrames;	// aantal frames in de file
		FILE *fptrIn;	// inputfile
		FILE *fptrOut;	// .y outputfile
		FILE *fptrOut2;	// .uv outputfile voor kleuren

		// instelbare parameters
		int GOP;		// Group of pictures: aantal frames tussen twee I-frames
		int maxMad;		// maximale MAD om te voldoen aan een referentie
		int maxP;		// maximale offset van zoekruimte voor het vinden van referenties voor P frame
		bool difference;// Keuze of er een bij een referentie ook een verschilmatrix wordt berekend
		int block;		// blocksize voor quantisation van I-frame en verschilmatrices
		int qstart;		// quantisatie startwaarde linksboven in quantisatiematrix
		int qraise;		// getal waarmee de quantisatie waardes lineair stijgen	
		int macro;		// instelbare grootte van de macroblock
		bool color;		// bool die aangeeft of de kleur mee gecompressed moet worden of niet


		
		// hulpvariabelen
		Matrix<int> currentFrame;	// frame van huidige verwerking
		Matrix<int> previousFrame;	// Y frame van vorige verwerking 
		Matrix<int> previousFrameU;	// U frame van vorige verwerking
		Matrix<int> previousFrameV;	// V frame van vorige verwerking

		bool isColorFrame; // bijhouden of de frame naar de color file of naar die Y file moet gaan
		enum { Y, U, V } colorband;

		BitStreamWriter *write;

		int openInputFile();	// Input file openen (geeft 0 terug indien fout)
		int openOutputFile();	// Output file openen (geeft 0 terug indien fout)
		void CountFrames();		// berekenen hoeveel frames een file bevat
		void PrintStatus(int frame);
		void readNextFrameFromFile();
		void readNextWriterFromFile();
		void writeBitstreamToFile(BitStreamWriter *writer);	// Bitstream wegschrijven naar outputfile
		void writeMatrixToFile(Matrix<int> ma);				// matrix wegschrijven naar outputfile
		void makeHeader();	// maakt en schrijft de header weg
		void makeIFrame();	// compressed een I frame en schrijft deze weg
		void makePFrame();	// compressed een P frame en schrijft deze weg	
};

#endif