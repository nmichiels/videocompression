#include <iostream>
using namespace std;
#include <stdlib.h>

#include "VideoDecompression.h"
//#include "matrix.h"


int main(int argc, char* argv[])
{
	cout << endl;
	if ( (argc == 3) && (string(argv[1]).compare("--help") == 0) )
	{
		// HELP
		cout << "--------------------- HELP ---------------------" << endl;
		cout << "________________________________________________" << endl << endl;
		cout << "How to make a video decompression? Use the following expression: " << endl;
		cout << "VideoDecompress input.y output.yuv inColor removeArtefact" << endl;
		cout << "\t input.y = the compressed video that you want to decompress." << endl;
		cout << "\t output.yuv = the decompressed video saved in a readable *.yuv file." << endl;
		cout << "\t inColor = 1 if you want to decompress in color, 0 if not." << endl;
		cout << "\t removeArtefact = 1 if you want to remove the white noise, 0 if not." << endl;
		cout << "\t \t If you want to decompress in color, you need an extra .uv file with the same name." << endl;
		cout << "________________________________________________" << endl << endl;
		exit(0);
	}
	if (argc < 3 || argc > 5)
	{
		cout << "Error: Use \"VideoDecompress input.y output.yuv inColor=0 removeArtefact=0." << endl;
		cout << "FOR HELP: \"VideoDecompress --help\"" << endl << endl; 
		exit(0);
	}
	
	bool inColor = false;
	bool removeArtefact = false;

	if (argc >= 4)
	{
		inColor = atoi(argv[3]);
	}
	if (argc >= 5)
	{
		removeArtefact = atoi(argv[4]);
	}

	
	VideoDecompression *video = new VideoDecompression(string(argv[1]), string(argv[2]),inColor,removeArtefact);
	video->startDecompression();
	delete video;
	return 0;
}