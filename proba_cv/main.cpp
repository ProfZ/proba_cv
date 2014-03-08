#include "SearchAlgorithm.hpp"

#define CAMERA_OUTPUT "Camera_Output"
#define DEBUG_WINDOW "Debug_Output"


using namespace std;
using namespace cv;
using namespace tracking;

namespace tracking{
	unsigned char lB,lG,lR,uB,uG,uR;
	Mat frame;
	Sablon sablon;
	
	float cart_y0,cart_x0;
	chrono::steady_clock::duration timeSample;
}

void mouseCallbackHandler(int event,int x,int y,int flags, void *user_data){
	if(event != CV_EVENT_LBUTTONDOWN)
		return;
	unsigned char *data = (unsigned char*)(frame.data);
	
	lB = data[frame.step*y + x*3] - RANGE_THRESH;
	lG = data[frame.step*y + x*3 + 1] - RANGE_THRESH;
	lR = data[frame.step*y + x*3 + 2] - RANGE_THRESH;

	uB = lB + 2*RANGE_THRESH;
	uG = lG + 2*RANGE_THRESH;
	uR = lR + 2*RANGE_THRESH;

	printf("%u,%u,%u\t%u,%u\n",tracking::lB,tracking::lG,tracking::lR,frame.rows,frame.cols);
}
 
int main()
{
	Mat frameBGR;
	namedWindow(CAMERA_OUTPUT,WINDOW_AUTOSIZE);
	namedWindow(DEBUG_WINDOW,WINDOW_AUTOSIZE);

	VideoCapture cap(0);
	if(!cap.isOpened()){
		return -1;
	}
	while(!cap.read(frameBGR)){
		cout << "Connecting camera!" << endl;
	}
	// normalizuje koordinatni pocetak Dekartovog koordinatnog sistema na sam centar slike
	tracking::cart_y0 = frameBGR.rows/2; 
	tracking::cart_x0 = frameBGR.cols/2;

	// pocetne boje za pracenje
	lB = lG = lR = 230;
	uB = uG = uR = 255;
	cvSetMouseCallback(CAMERA_OUTPUT,mouseCallbackHandler);

	//tracking::sablon = tracking::Sablon::_TOP_BOTTOM_S;

	//chrono::steady_clock clock;
	//chrono::steady_clock::time_point tp = clock.now();

	bool start = false;
	while(!start){
		cap.read(frameBGR);
		//tracking::timeSample = tp - clock.now(); //racunamo vreme izmedju frejmova (sluzi za aproksimaciju brzine kretanja markera)
		//tp = clock.now();

		tracking::frame = frameBGR.clone();
		calculateRectangle(frameBGR);
		start = initializeStart();

		imshow(DEBUG_WINDOW,frameBGR);
		imshow(CAMERA_OUTPUT,tracking::frame);
		if(waitKey(10) == 27){
			return 0;
		}
	}

	while(true){
		cap.read(frameBGR);
		//tracking::timeSample = tp - clock.now();
		//tp = clock.now();
		
		tracking::frame = frameBGR.clone();
		calculateRectangle(frameBGR);
		setUpMarkers();
		drawLines();
		imshow(DEBUG_WINDOW,frameBGR);
		imshow(CAMERA_OUTPUT,tracking::frame);
		if(waitKey(10) == 27){
			break;
		}
	}

}