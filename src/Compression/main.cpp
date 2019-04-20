#include <iostream>
using namespace std;
#include <stdlib.h>

#include "VideoCompression.h"


int main(int argc, char* argv[])
{
	cout << endl;
	if ( (argc == 2) && (string(argv[1]).compare("--help") == 0) )
	{
		// HELP
		cout << "--------------------- HELP ---------------------" << endl;
		cout << "________________________________________________" << endl << endl;
		cout << "How to make a videocompression? Use the following expression:" << endl;
		cout << "VideoCompress input.yuv output.y inColor height width GOP=1 maxMad=7 maxP=6 macroblock=16 useDifference=0 blocksize=8 qstart=2 qraise=2" << endl;
		cout << "\t isColor = 1 if you want to compress your video in color, 0 if not." << endl;
		cout << "\t height = the height of your video." << endl;
		cout << "\t width = the width of your video." << endl;
		cout << "\t input.yuv = the video that you want to compress." << endl;
		cout << "\t output.y = the compressed video saved in a compressed file format." << endl;
		cout << "\t GOP = Group Of Pictures (optional)." << endl;
		cout << "\t maxMad = The maximal limit of the MAD calculation (optional)." << endl;
		cout << "\t maxP = Maximal window to search a reference (optional)." << endl;
		cout << "\t macroblock = Size of the macroblock (optional)." << endl;
		cout << "\t useDifference = 1 to use differencial matrix for references, 0 if not (optional)." << endl;
		cout << "\t blocksize = blocksize of the quantization (optional)." << endl;
		cout << "\t qstart = quantization start (optional)." << endl;
		cout << "\t qraise = quantization raise (optional)." << endl;
		cout << "\t restriction: Height and width must be divisible by the blocksize." << endl;
		cout << "\t restriction: Height and width must be divisible by the macroblock." << endl;
		cout << "\t restriction: Macroblock must be divisible by the blocksize." << endl;
		cout << "________________________________________________" << endl << endl;
		exit(0);
	}
	if (argc < 6 || argc > 14)
	{
		cout << "Error: Use \"VideoCompress input.yuv output.y inColor height width GOP=1 maxMad=7 maxP=6 macroblock=16 useDifference=0 blocksize=8 qstart=2 qraise=2\"." << endl;
		cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl; 
		exit(0);
	}
	bool isColor = atoi(argv[3]); 
	int height = atoi(argv[4]);
	int width = atoi(argv[5]);
	
	int GOP = 1;
	int maxMad = 7;
	int maxP = 6;
	int macroblock = 16;
	bool useDifference = false;
	int start = 2;
	int raise = 2;
	int blocksize = 8;
	if (height <= 0 || width <= 0){
		cout << "Error: Height and Width must be > 0." << endl;
		cout << "Error: Use \"VideoCompress input.yuv output.y inColor height width GOP=1 maxMad=7 maxP=6 macroblock=16 useDifference=0 blocksize=8 qstart=2 qraise=2\"." << endl;
		cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl; 
		exit(0);
	}

	if (argc >= 7)
	{
		GOP = atoi(argv[6]);
		if (GOP < 1)
		{
			cout << "Error: GOP >= 1." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl;
			exit(0);
		}
	}
	if (argc >= 8)
	{
		maxMad = atoi(argv[7]);
		if (maxMad < 0)
		{
			cout << "Error: maxMad >= 0." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl;
			exit(0);
		}
	}
	if (argc >= 9)
	{
		maxP = atoi(argv[8]);
		if (maxP < 1)
		{
			cout << "Error: maxP >= 1." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl;
			exit(0);
		}
	}
	if (argc >= 10)
	{
		macroblock = atoi(argv[9]);
		if (macroblock < 1)
		{
			cout << "Error: macroblock >= 1." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl;
			exit(0);
		}
		if ( (height % macroblock != 0) || (width % macroblock != 0) )
		{
			cout << "Error: Height and width must be divisible by the macroblock." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl;
			exit(0);
		}
	}
	if (argc >= 11)
	{
		useDifference = atoi(argv[10]);
	}
	if (argc >= 12)
	{
		blocksize = atoi(argv[11]);
		if (blocksize < 1)
		{
			cout << "Error: blocksize >= 1." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl; 
			exit(0);
		}
		if ( (width % blocksize !=0) || (height % blocksize !=0) )
		{
			cout << "Error: Wrong blocksize." << endl;
			cout << "Error: Height and width must be divisible by the blocksize." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl; 
			exit(0);
		}
	}
	if (argc >= 13)
	{	
		start = atoi(argv[12]);
		if (start < 1)
		{
			cout << "Error: qstart >= 1." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl;
			exit(0);
		}
	}
	if (argc >= 14)
	{
		raise = atoi(argv[13]);
		if (raise < 1)
		{
			cout << "Error: qraise >= 1." << endl;
			cout << "FOR HELP: \"VideoCompress --help\"" << endl << endl; 
			exit(0);
		}
	}

	cout << endl << "Begin VideoCompressing " << argv[1] << "->" << argv[2] <<  " (" << height << "," << width << ")..." << endl;

	VideoCompression *video = new VideoCompression(string(argv[1]), string(argv[2]),isColor, height, width, GOP,maxMad,maxP, macroblock, useDifference,blocksize,start,raise);
	video->startCompression();
	delete video;
	return 0;
}