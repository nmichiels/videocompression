#include "VideoDecompression.h"

VideoDecompression::VideoDecompression(string inputFile, string outputFile, bool color, bool removeArtefact)
{
	this->inputFile = inputFile;
	this->outputFile = outputFile;
	this->maxFrames = 0;
	this->color=color;
	this->isColorFrame = false;
	this->colorband = Y;
	this->removeArtefact = removeArtefact;
}


VideoDecompression::~VideoDecompression()
{
	
}


void VideoDecompression::setHeight(int height)
{
	this->height = height;
}

void VideoDecompression::setWidth(int width)
{
	this->width = width;
}

int VideoDecompression::openInputFile()
{
	errno_t err;

	cout << "Opening video \'" << inputFile.c_str() << "\'." << endl;
	if ( (err = fopen_s(&fptrIn,inputFile.c_str(), "rb")) != NULL){
		cerr << "VideoDecompression(Open): Can't open file " << inputFile.c_str() << "." << endl;
		return 0;
	}

	if (color)
	{
		// indien de gebruiker kiest om met kleur te decompressen moet de overeenkomstige .uv file worden ingelezen
		string h = inputFile;
		if ( (err = fopen_s(&fptrIn2,h.append(".uv").c_str(), "rb")) != NULL){
		cerr << "VideoDecompression(Open): Can't open file " << inputFile.c_str() << "." << endl;
		return 0;
	}
	}
	cout << "Video \"" << inputFile.c_str() << "\" opened succesfully." << endl;
	return 1;
}

int VideoDecompression::openOutputFile()
{
	errno_t err;
	cout << "Opening video \'" << outputFile.c_str() << "\'." << endl;
	if ( (err = fopen_s(&fptrOut,outputFile.c_str(), "wb")) != NULL){
		cerr << "VideoDecompression(Open): Can't open file " << outputFile.c_str() << "." << endl;
		return 0;
	}

	cout << "Video \"" << outputFile.c_str() << "\" opened succesfully." << endl;
	return 1;
}

void VideoDecompression::PrintStatus(int frame)
{
	system("cls");
	cout << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "Video Decompressing in progress..." << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "Inputfile  : " << inputFile.c_str() << endl;
	cout << "Outputfile : " << outputFile.c_str() << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << setw(6) << setprecision(2) << fixed << (double(frame)/double(maxFrames))*100 << "% ready..." << endl;
	cout << "Current frame : " << frame << "/" << maxFrames << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << endl;
}

// core van decompression (symmetrisch met startCompression()
void VideoDecompression::startDecompression()
{
	// nodige files inlezen
	if ( openInputFile() == 0)
	{
		cerr << "Error: Couldn't open input file \'" << inputFile.c_str() << endl;
		return;
	}
	if ( openOutputFile() == 0)
	{
		cerr << "Error: Couldn't open output file \'" << outputFile.c_str() << endl;
		return;
	}

	readHeader(); // eerst header inlezen
	decom = new Decompression;
	// Alle frames afgaan en overeenkomstige I of P frame creeeren.
	int cFrames = 0;
	while (cFrames < maxFrames)
	{
		PrintStatus(cFrames);
		this->colorband = Y;
		readIFrame(); // I frame van Y
		if (color)//iframe kleur
		{
			this->isColorFrame = true;
			this->setHeight(this->height/2);
			this->setWidth(this->width/2);
			this->macro = this->macro / 2;
			this->colorband = U;
			readIFrame(); // I frame inlezen van kleuren
			this->colorband = V;
			readIFrame();
			this->macro = this->macro * 2;
			this->setHeight(this->height*2);
			this->setWidth(this->width*2);
			this->isColorFrame = false;
		}
		else
		{
			// UV opvullen met grijswaarden
			fillUV();
		}
		cFrames++;
		for (int i=1; i<this->GOP && cFrames < maxFrames; i++)//beurt aan p-frame
		{
			PrintStatus(cFrames);
			this->colorband = Y;
			readPFrame(); // P frame inlezen
			if (color)//indien kleur
			{
				this->isColorFrame = true;
				this->setHeight(this->height/2);
				this->setWidth(this->width/2);
				this->colorband = U;
				readIFrame(); // kleuren worden ook gedecompressed als I frame
				this->colorband = V;
				readIFrame();
				this->setHeight(this->height*2);
				this->setWidth(this->width*2);
				this->isColorFrame = false;
			}
			else
			{
				// UV opvullen met grijswaarden
				fillUV();
			}
			cFrames++;
		}
	}
	PrintStatus(cFrames);
	fclose(fptrIn);
	fclose(fptrOut);
	if (color)
		fclose(fptrIn2);
	if(this->color)
		cout << "Videodecompressing yuv ready..." << endl;
	else
		cout << "Videodecompressing y ready..." << endl;
	cout << "parameters:" << endl;
	cout << "macroblok-size: " <<this->macro<< endl;
	cout << "difference-matrix references: " <<this->difference<< endl;
	cout << "size: " <<this->height<< "x"<<this->width<< endl;
	cout << "block-size: " <<this->block<< endl;
	cout << "quantisation left-corner, increase: " <<this->qstart<<", "<<this->qraise<< endl;
	cout << "remove artefacts: " <<this->removeArtefact << endl;
	cout << "-----------------------------------------------------------------------" << endl;
}



void VideoDecompression::readHeader()
{
	write = new BitStreamWriter(12);//12 karakters header
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	write->put(8,(unsigned __int32)getc(fptrIn));
	
	read = new BitStreamReader(*write);
	this->setHeight((int) read->get(16));
	this->setWidth((int) read->get(16));
	this->setGOP((int) read->get(16));
	this->maxFrames = (int) read->get(16);
	qstart=read->get(9); 
	qraise=read->get(9);
	block=read->get(5);
	difference=(bool)read->get(1);; 
	macro=read->get(8);

	if (color)
	{
		this->previousFrameU = Matrix<int>(height/2, width/2);
		this->previousFrameV = Matrix<int>(height/2, width/2);
	}
	delete write;
	delete read;
}

void VideoDecompression::readIFrame()
{
	FILE *in = fptrIn;
	if (this->isColorFrame == true)
		in = fptrIn2;

	write = new BitStreamWriter(4);
	write->put(8,(unsigned __int32)getc(in));
	write->put(8,(unsigned __int32)getc(in));
	write->put(8,(unsigned __int32)getc(in));
	write->put(8,(unsigned __int32)getc(in));
	read = new BitStreamReader(*write);
	int length = read->get(32);
	write = new BitStreamWriter(length);
	int size = length;
	for(int j=0;j<size;j++)//frame inlezen
		write->put(8,(unsigned __int32) getc(in));

	decom->setWriter(write);//decompressie zoals in imagecompressie
	decom->Read();
	decom->toMatrix();
	decom->antiQuantisation();
	decom->antiDct();
	Matrix<int> hulp(height, width);
	hulp = decom->getMatrix();
	if (this->colorband == Y){
		previousFrame=hulp;
	}
	else if (this->colorband == U){
		previousFrameU=hulp;
	}
	else{
		previousFrameV=hulp;
	}
	writeMatrixToFile(hulp);
	decom->deletes();
	delete write;
	delete read;
}

void VideoDecompression::readPFrame()
{
	FILE *in = fptrIn;
	if (this->isColorFrame == true)
		in = fptrIn2;
	// Header van de frame inlezen (deze bevat de lengte van de totale frame)
	write = new BitStreamWriter(4);
	write->put(8,(unsigned __int32)getc(in));
	write->put(8,(unsigned __int32)getc(in));
	write->put(8,(unsigned __int32)getc(in));
	write->put(8,(unsigned __int32)getc(in));
	read = new BitStreamReader(*write);
	int length = read->get(32);
	delete write;
	delete read;

	// De hele frame inlezen
	write = new BitStreamWriter(length);
	int size = length;
	for(int j=0;j<size;j++)
		write->put(8,(unsigned __int32) getc(in));
	read = new BitStreamReader(*write);
	read->get(8);read->get(8);

	int heightB=height/macro;
	int widthB=width/macro;

	Decompression *decom = new Decompression;//alles klaarmaken
	decom->setReader(read);
	Matrix<Matrix<int>*> current(heightB, widthB);
	for(int i=0;i<heightB;i++)
		for(int j=0;j<widthB;j++)
			current[i][j]=new Matrix<int>(macro,macro);

	int countref = 0;
	int countdif = 0;
	int refi, refj;
	decom->Pinitiate(block,qstart, qraise, macro);
	for(int i=0;i<heightB;i++){
		for(int j=0;j<widthB;j++)
		{
			if ((int)(read->get_bit()) == 1)
			{
				// referentie
				refi = read->get(12);
				refj = read->get(12);
				if(difference){//idien compiled met difmatrix: (keuze van gebruiker)
					decom->Read2();
					decom->toMatrix();
					decom->antiQuantisation();
					decom->antiDct();
					*current[i][j]=decom->getMatrix();
					for(int x=0;x<macro;x++)//anders:
						for(int y=0;y<macro;y++)
							(*current[i][j])[x][y] += previousFrame[refi + x][refj + y];
				}else
					for(int x=0;x<macro;x++)
						for(int y=0;y<macro;y++)
							(*current[i][j])[x][y] = previousFrame[refi + x][refj + y];
			}
			else
			{
				// diff
				decom->Read2();
				decom->toMatrix();
				decom->antiQuantisation();
				decom->antiDct();
				*current[i][j]=decom->getMatrix();
				for(int x=0;x<macro;x++)
					for(int y=0;y<macro;y++)
						(*current[i][j])[x][y] += previousFrame[i*macro + x][j*macro + y];
			}
		}
	}

	Matrix<int> h = current.fromBlocks(macro);
	if (this->colorband == Y){
		previousFrame=h;
	}

	if (this->removeArtefact && block<=macro) // witte afrondingsfouten uitmiddelen
	{
		h.removeRoundingError(this->block);
	}
	writeMatrixToFile(h);
	for(int i=0;i<heightB;i++){
		for(int j=0;j<widthB;j++)
		{
			delete current[i][j];
		}
	}
	decom->deletes();
	delete decom;
	delete write;
	delete read;
}


void VideoDecompression::writeMatrixToFile(Matrix<int> ma)
{
	for(int a=0;a<height;a++)
		for(int b=0;b<width;b++)
			if ( ma[a][b] > 255 )
				putc(255,fptrOut);
			else if ( ma[a][b] < 0 )
				putc(0,fptrOut);
			else
				putc(ma[a][b],fptrOut);
	
}

void VideoDecompression::fillUV()
{
	for (int i=0; i< ((width)*(height))/2; i++)
	{
		if (this->colorband = U){
			putc(128,fptrOut);
		}
		else {
			putc(128,fptrOut);
		}
	}
}