#ifndef VIDEO_DECOMPRESSION_H
#define VIDEO_DECOMPRESSION_H

#include <iomanip>
#include "Decompression.h"
using namespace util;


class VideoDecompression {
	public:
		VideoDecompression(string inputFile="", string outputFile="", bool color=false, bool removeArtefact=false);
		~VideoDecompression();

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

		void startDecompression(); // de core van decompression

	private:
		// file gegevens
		string inputFile;	// filenaam die ingelezen is
		string outputFile;	// filenaam voor uit te lezen
		int width;
		int height;
		int maxFrames;	// aantal frames in de file
		FILE *fptrIn;	// .y inputfile
		FILE *fptrIn2;	// .uv inputfile voor kleuren
		FILE *fptrOut;	// .yuv outputfile

		// compressieparameters afkomstig uit de header
		int GOP;		// Group of pictures: aantal frames tussen twee I-frames
		bool difference;// Keuze of er een bij een referentie ook een verschilmatrix wordt berekend
		int block;		// blocksize voor quantisation van I-frame en verschilmatrices
		int qstart;		// quantisatie startwaarde linksboven in quantisatiematrix
		int qraise;		// getal waarmee de quantisatie waardes lineair stijgen	
		int macro;		// instelbare grootte van de macroblock

		// parameters meegekregen van de gebruiker
		bool color;		// bool die aangeeft of de kleur mee gecompressed moet worden of niet
		bool removeArtefact; // geeft aan of de witte afrondingsfouten moeten worden uitgemiddeld

		// hulp variabelen
		bool isColorFrame; // bijhouden of de frame naar de color file of naar die Y file moet gaan
		enum { Y, U, V } colorband;

		Decompression *decom;
		BitStreamWriter *write;
		BitStreamReader *read;

		Matrix<int> previousFrame;	// Y frame van vorige verwerking 
		Matrix<int> previousFrameU;	// U frame van vorige verwerking
		Matrix<int> previousFrameV;	// V frame van vorige verwerking

		

		int openInputFile();	// inputfile inlezen (geeft 0 terug indien fout)
		int openOutputFile();	// outputfile inlezen (geeft 0 terug indien fout)
		void PrintStatus(int frame);

		void writeMatrixToFile(Matrix<int> ma);
		void fillUV();		// voor grijswaardenvideo moeten de kleuren opgevuld worden
		void readHeader();
		void readIFrame();
		void readPFrame();

};

#endif