#include "VideoCompression.h"

VideoCompression::VideoCompression(string inputFile, string outputFile, bool color,int height, int width, int GOP,int maxMad,int maxP, int macro, bool difference, int block, int qstart, int qraise) : currentFrame(height, width), previousFrame(height, width)
{
	this->inputFile = inputFile;
	this->outputFile = outputFile;
	this->width = width;
	this->height = height;
	this->GOP = GOP;
	this->maxFrames = 0;
	this->maxMad=maxMad; 
	this->difference=difference; 
	this->block=block; 
	this->qstart=qstart; 
	this->qraise=qraise;
	this->maxP=maxP;
	this->macro=macro;
	this->color=color;
	this->isColorFrame = false;
	this->colorband = Y;
	if (color)
	{
		this->previousFrameU = Matrix<int>(height/2, width/2);
		this->previousFrameV = Matrix<int>(height/2, width/2);
	}
}

VideoCompression::~VideoCompression()
{
	
}

void VideoCompression::setHeight(int height)
{
	this->height = height;
	currentFrame.setHeight(height);
	currentFrame.renew(); // frame moet hermaakt worden aan nieuwe grootte
}

void VideoCompression::setWidth(int width)
{
	this->width = width;
	currentFrame.setWidth(width);
	currentFrame.renew(); // frame moet hermaakt worden aan nieuwe grootte
}

int VideoCompression::openInputFile()
{
	errno_t err;

	cout << "Opening video \'" << inputFile.c_str() << "\'." << endl;
	if ( (err = fopen_s(&fptrIn,inputFile.c_str(), "rb")) != NULL){
		cerr << "VideoDecompression(Open): Can't open file " << inputFile.c_str() << "." << endl;
		return 0;
	}
	cout << "Video \"" << inputFile.c_str() << "\" opened succesfully." << endl;
	return 1;
}

int VideoCompression::openOutputFile()
{
	errno_t err;
	cout << "Opening video \'" << outputFile.c_str() << "\'." << endl;
	if ( (err = fopen_s(&fptrOut,outputFile.c_str(), "wb")) != NULL){
		cerr << "VideoDecompression(Open): Can't open file " << outputFile.c_str() << "." << endl;
		return 0;
	}
	if (color)
	{
		// indien kleur wordt weggeschreven wordt er een extra file aangemaakt met dezelfde naam en .uv achter
		string h = outputFile;
		h.append(".uv");
		cout << "Opening video \'" << h.c_str() << "\'." << endl;
		if ( (err = fopen_s(&fptrOut2,h.c_str(), "wb")) != NULL){
			cerr << "VideoDecompression(Open): Can't open file " << h.c_str() << "." << endl;
			return 0;
		}
	}
	cout << "Video \"" << outputFile.c_str() << "\" opened succesfully." << endl;
	return 1;
}

void VideoCompression::CountFrames()
{
	// berekenen lengte file
	if( fseek(fptrIn, 0, SEEK_END) != 0){
		cerr << "Error: Can't move filepointer to the end of the file." << endl;
		system("pause");
		exit(0);
	}
	int length = ftell(fptrIn); // lengte van de file

	// filepointer vooraan te zetten om vanaf daar te lezen
	if( fseek(fptrIn, 0, SEEK_SET) != 0){
		cerr << "Error: Can't move filepointer to the end of the file." << endl;
		system("pause");
		exit(0);
	}
	maxFrames = length / (this->height * this->width);
	maxFrames = maxFrames-maxFrames/3;
}

// Core van de videocompressie
void VideoCompression::startCompression()
{
	// De nodige files openen
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

	// Aantal frames tellen die moeten gecompressed worden
	CountFrames();

	makeHeader(); // eerst header wegschrijven

	// Alle frames afgaan en overeenkomstige I of P frame creeeren.
	int cFrames = 0;
	while (cFrames < maxFrames)
	{
		// I frame maken
		PrintStatus(cFrames);
		this->colorband = Y;
		makeIFrame(); // I frame maken voor Y
		if(!color){
			// kleurwaardens overslaan
			for (int i=0; i< (width)*(height)/2; i++)
				getc(fptrIn);
		}
		else
		{
			this->isColorFrame = true;
			setHeight(this->height/2);
			setWidth(this->width/2);
			this->colorband = U;
			makeIFrame();
			this->colorband = V;
			makeIFrame(); // kleurwaarde wordt met I frame gecompressed
			setHeight(this->height*2);
			setWidth(this->width*2);
			this->isColorFrame = false;
		}
		cFrames++; // aantal frames die gecompressed zijn toegenomen

		// Aantal P frames aanmaken (aangegeven met GOP)
		for (int i=1; i<this->GOP && cFrames < maxFrames; i++)
		{
			PrintStatus(cFrames);
			this->colorband = Y;
			makePFrame(); // P frame voor Y
			if(!color){
				// kleurwaardens overslaan indien er geen kleur noodzakelijk is
				for (int i=0; i< (width)*(height)/2; i++)
					getc(fptrIn);
			}
			else
			{
				this->isColorFrame = true;
				setHeight(this->height/2);
				setWidth(this->width/2);
				this->colorband = U;
				makeIFrame();
				this->colorband = V;
				makeIFrame();	// Ook hier worden de kleurwaardens gecompressed met I frames
				setHeight(this->height*2);
				setWidth(this->width*2);
				this->isColorFrame = false;
			}
			cFrames++; // aantal frames die gecompressed zijn toegenomen
		}
	}
	PrintStatus(cFrames);
	cout << "Videocompressing y ready..." << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	
	fclose(fptrOut);
	fclose(fptrIn);
	if (color)
		fclose(fptrOut2);

}

void VideoCompression::PrintStatus(int frame)
{
	system("cls");
	cout << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "Videocompressing in progress..." << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << "Inputfile  : " << inputFile.c_str() << endl;
	cout << "Outputfile : " << outputFile.c_str() << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << setw(6) << setprecision(2) << fixed << (double(frame)/double(maxFrames))*100 << "% ready..." << endl;
	cout << "Current frame : " << frame << "/" << maxFrames << endl;
	cout << "-----------------------------------------------------------------------" << endl;
	cout << endl;
}

// nieuwe frame uit de file lezen
void VideoCompression::readNextFrameFromFile()
{
	

	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			currentFrame[i][j]=getc(fptrIn);
}


void VideoCompression::makeHeader()
{
	int l = ftell(fptrOut);
	l = ftell(fptrOut);
	BitStreamWriter header(12);
	header.put(16,this->height);
	header.put(16,this->width);
	header.put(16,this->GOP);
	header.put(16,this->maxFrames);
	header.put(9,this->qstart);
	header.put(9,this->qraise);
	header.put(5,this->block);
	header.put(1,this->difference);
	header.put(8,this->macro);
	writeBitstreamToFile(&header);
	l = ftell(fptrOut);
}
void VideoCompression::makeIFrame()
{
	
	readNextFrameFromFile();
	
	// Image compression aanmaken
	Compression com(block, qstart,qraise,width,height);
	com.setMatrix(currentFrame);
	com.dct();
	com.quantisation();
	com.toArray();
	com.rle();
	BitStreamWriter size(4);
	com.getWriter()->flush();
	size.put(32, com.getWriter()->get_position()/8);
	// bitstreamwriter uit imagecompression halen, de grootte ervan als header wegschrijven en vervolgens de writer wegschrijven
	writeBitstreamToFile(&size);
	writeBitstreamToFile(com.getWriter());
	delete [] com.getWriter()->get_buffer();

	if (this->colorband == Y){
		previousFrame=currentFrame;
	}
	else if (this->colorband == U){
		previousFrameU=currentFrame;
	}
	else{
		previousFrameV=currentFrame;
	}

}


void VideoCompression::makePFrame()
{
	int heightB=height/macro;
	int widthB=width/macro;
	readNextFrameFromFile();
	
	Matrix<int> refi(heightB, widthB);
	refi.negatives();
	Matrix<int> refj(heightB, widthB);
	refj.negatives();
	//verschilmatrix
	Matrix<double> mad(height,width);
	//reeds gevonden waardes, om niets dubbel te berekenen
	Matrix<Matrix<int>*> dif(heightB,widthB);
	for(int i=0;i<heightB;i++){
		for(int j=0;j<widthB;j++)
		{
			dif[i][j]=new Matrix<int>(macro,macro);
			dif[i][j]->zeros();
		}
	}
	//omzetten current en previous naar macronlokken
	Matrix<Matrix<int>*> current=currentFrame.toBlocks(macro);
	//alle macroblokken afgaan:
	int oi,oj,offset=maxP;//offsets tov huidig
	int besti,bestj;//minste MAD
	double best;
	int ci,cj;
	//
	for(int i=0;i<heightB;i++){
		for(int j=0;j<widthB;j++)
		{//redelijk hardcoded...
			mad.negatives();
			best = this->maxMad + 1;
			offset=maxP;
			ci=i*macro;cj=j*macro;
			do{
				offset/=2;
				oi=ci;oj=cj;//1
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				besti=bestj=0;best=mad[oi][oj];}
				//2
				oj=min(cj+offset,width-macro-1);
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}}
				//3
				oj=max(cj-offset,0);
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}
				}//4
				oi=min(ci+offset,height-macro-1);
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}
				}//5
				oj=cj;
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}
				}//6
				oj=min(cj+offset,width-macro-1);
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}
				}//7
				oi=max(ci-offset,0);
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}
				}//8
				oj=cj;
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}
				}//9
				oj=max(cj-offset,0);
				if(mad[oi][oj]<0){
				mad[oi][oj]=current[i][j]->calcMad(previousFrame,oi,oj);
				if(mad[oi][oj]<best)	{	best=mad[oi][oj];besti=oi;bestj=oj;	}
				}
				ci=besti;cj=bestj;
			}while(offset>1 && best!=0);
			if(best<=this->maxMad && best > 0){
				refi[i][j]=ci;
				refj[i][j]=cj;
				for(int y=0;y<macro;y++)
					for(int z=0;z<macro;z++)
						(*dif[i][j])[y][z]=(*current[i][j])[y][z]-previousFrame[ci+y][cj+z];
			}else
				for(int y=0;y<macro;y++)
					for(int z=0;z<macro;z++)
						(*dif[i][j])[y][z]=(*current[i][j])[y][z]-previousFrame[i*macro+y][j*macro+z];
		}
	
	}
	BitStreamWriter *write = new BitStreamWriter(8*height*width+5*(heightB+widthB)+6);//een maxhoeveelheid...
	write->put(8,3);write->put(8,1);
	Compression *com = new Compression;
	com->setWriter(write);
	com->setHeight(macro);
	com->setWidth(macro);
	com->setBlocksize(block);
	com->setQStart(qstart);
	com->setQRaise(qraise);
	for(int i=0;i<heightB;i++){
		for(int j=0;j<widthB;j++){
			if(refi[i][j]>=0 && refj[i][j]>=0)  // referentie wegschrijven
			{
				write->put_bit(1);
				int testi = refi[i][j];
				int testj = refj[i][j];
				write->put(12,refi[i][j]);
				write->put(12,refj[i][j]);
				if(this->difference) // indien nodig ook bij de referentie de verschilmatrix wegschrijven
				{
					com->setMatrix(*dif[i][j]);
					com->dct();
					com->quantisation();
					com->toArray();
					com->rleb();
					
				}
			}	
			else					// verschilmatrix wegschrijven
			{
				write->put_bit(0);
				com->setMatrix(*dif[i][j]);
				com->dct();
				com->quantisation();
				com->toArray();
				com->rleb();
			}
		}
	}
	// header wegschrijven en vervolgens de writer die dynamisch gecreeërd is.
	BitStreamWriter header(4);
	write->flush();
	header.put(32, write->get_position()/8);	
	
	writeBitstreamToFile(&header);	
	writeBitstreamToFile(write);

	// enkel de Y color banden gelijkstellen aan vorige frame
	if (this->colorband == Y){
		previousFrame=currentFrame;
	}

	// dealloceren van geheugen
	delete com;
	delete [] write->get_buffer();
	for(int i=0;i<heightB;i++){
		for(int j=0;j<widthB;j++)
		{
			delete dif[i][j];
			delete current[i][j];
		}
	}
	

}

void VideoCompression::writeBitstreamToFile(BitStreamWriter *writer)
{
	if (this->isColorFrame == true)
		util::write(fptrOut2,*writer);
	else
		util::write(fptrOut,*writer);
}

void VideoCompression::writeMatrixToFile(Matrix<int> ma)
{
	FILE *out = fptrOut;
	if (this->isColorFrame == true)
		out = fptrOut2;

	for(int a=0;a<height;a++)
		for(int b=0;b<width;b++)
			if ( ma[a][b] > 255 )
				putc(255,out);
			else if ( ma[a][b] < 0 )
				putc(0,out);
			else
				putc(ma[a][b],out);
}

