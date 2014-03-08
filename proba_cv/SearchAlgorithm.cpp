#include "SearchAlgorithm.hpp"

using namespace tracking;
mark::cvMarkerArray mark::markerArray(MARKER_NUM);
mark::cvMarkerArray mark::markerArrayBuff;
mark::cvMarkerArray mark::testArray;

void searchSphere(Rect rect){
	using namespace cv;

	if((rect.x -= EXPAND) < 0)
		rect.x = 0;
	if((rect.y -= EXPAND) < 0)
		rect.y = 0;
	if((rect.x + (rect.width += 2*EXPAND)) > frame.cols)
		rect.width = frame.cols - rect.x;
	if((rect.y + (rect.height += 2*EXPAND)) > frame.rows)
		rect.height = frame.rows - rect.y;

	vector<Vec3f> circles;

	Mat RoiGauss;

	GaussianBlur(frame(rect),RoiGauss,Size(5,5),0,0);
	//bilateralFilter(frame(rect),RoiGauss,5,100,50);
	cvtColor(RoiGauss,RoiGauss,CV_BGR2GRAY);
	threshold(RoiGauss,RoiGauss,230,255,0);
	erode(RoiGauss,RoiGauss,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));
	dilate(RoiGauss,RoiGauss,getStructuringElement(MORPH_ELLIPSE,Size(5,5)));
	Canny(RoiGauss,RoiGauss,0,255);
	//GaussianBlur(RoiGauss,RoiGauss,Size(5,5),0,0);

	vector<vector<Point>> contours;

	findContours(RoiGauss,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,Point(rect.x,rect.y));
	/*for(size_t i = 0; i < contours.size(); ++i){
		drawContours(frame,contours,i,Scalar(255,0,0),2);
	}*/

	vector<RotatedRect> minEllipse( contours.size() );
	int i,j=0,k;
		for(i=0; i < contours.size(); ++i){
			if( contours[i].size() > 50 ){
				minEllipse[j] = fitEllipse( contours[i] );
				if(abs(minEllipse[j].size.height - minEllipse[j].size.width) < MIN_GRATE*3){
					if(minEllipse[j].size.height >= minEllipse[j].size.width)
						mark::markerArrayBuff.ts_add(mark::cvMarker(minEllipse[j].center,minEllipse[j].size.width/2));
					else
						mark::markerArrayBuff.ts_add(mark::cvMarker(minEllipse[j].center,minEllipse[j].size.height/2));
					ellipse(frame,minEllipse[j],Scalar(255,0,0),2);
					++j;
					++i;
					break;
				}
			}
		}
	for(; i < contours.size(); ++i ){
			if( contours[i].size() > 50 ){
				minEllipse[j] = fitEllipse( contours[i] );
				if(abs(minEllipse[j].size.height - minEllipse[j].size.width) < MIN_GRATE*4){
					for(k=0; k < j; ++k){
						if((pow((minEllipse[k].center.x - minEllipse[j].center.x),2) + pow((minEllipse[k].center.y - minEllipse[j].center.y),2) < MIN_GRATE*MIN_GRATE)){
							break;
						}
					}
					if(k != j) continue;
					if(minEllipse[j].size.height >= minEllipse[j].size.width)
						mark::markerArrayBuff.ts_add(mark::cvMarker(minEllipse[j].center,minEllipse[j].size.width/2));
					else
						mark::markerArrayBuff.ts_add(mark::cvMarker(minEllipse[j].center,minEllipse[j].size.height/2));
					ellipse(frame,minEllipse[j],Scalar(255,0,0),2);
					++j;
				}
			}
		}
	//HoughCircles( RoiGauss, circles, CV_HOUGH_GRADIENT, 1,RoiGauss.rows/2,200,210);

	//if(circles.size()){
	//	for( size_t i = 0; i < circles.size(); i++ )
	//	{
	//		circles[i][0] += rect.x;
	//		circles[i][1] += rect.y;
	//		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	//		int radius = cvRound(circles[i][2]);
	//		// circle center
	//		circle( frame, center, 1, Scalar(0,255,0), -1, 1, 0 );
	//		// circle outline
	//		circle( frame, center, radius, Scalar(0,0,255), 1, 1, 0 );
	//		mark::markerArrayBuff.ts_add(circles[i]);
	//	}
	//}else
	//	mark::testArray.ts_add(mark::cvMarker((rect.x + rect.width/2),(rect.y + rect.height/2),rect.width/2 - EXPAND));
	//debug ROI
	cv::rectangle(frame,rect,cv::Scalar(0,255,255),2);
}

void calculateRectangle(cv::Mat &frameBGR){
	static const int max1 = frame.rows - MIN_GRATE, max2 = frame.cols-MIN_GRATE, min2 = MIN_GRATE;

	unsigned char *data,*row,*pixel;
	unsigned char *data2,*row2,*pixel2; //////

	data = (unsigned char*)(frame.data);
	data2 = (unsigned char*)(frameBGR.data);//////
	mark::markerArrayBuff.num = 0; //praznimo buffer za markere
	mark::testArray.num = 0; //pomocni niz

	int i,j,old;

	bool inRow; //da li smo zaceli rect unutar jednog reda
	int inRow_rects_curr=0; //indeks tekuceg recta u redu
	static Rect *inRow_rects = new Rect[RECT_ALLOC_UNIT]; //niz rektova u redu
	int global_rects_end=0,global_rects_curr=0; //prvi oslobodjeni, prvi neupisani
	static Rect *global_rects = new Rect[RECT_ALLOC_UNIT];
	int thread_num = 0;
	static std::thread *threads = new std::thread[RECT_ALLOC_UNIT];

	for(i=MIN_GRATE; i<=max1; i+=MIN_GRATE){
		row = data + frame.step*i;
		row2 = data2 + frameBGR.step*i;/////////
		inRow = false;
		for(j=min2; j<=max2; j+=MIN_GRATE){ //x treba da se mnozi sa 3?
			pixel = row + j*3;
			pixel2 = row2 + j*3;/////////
			pixel2[0] = 0; pixel2[1] = 255; pixel2[2] = 0; ///////////////////

			if(	lB < pixel[0] && pixel[0] <= uB &&
				lG < pixel[1] && pixel[1] <= uG &&
				lR < pixel[2] && pixel[2] <= uR){ //THRESHOLD

				pixel2[0] = 0; pixel2[1] = 0; pixel2[2] = 255;
				if(inRow)
					continue;

				(inRow_rects[inRow_rects_curr]).x=j;
				(inRow_rects[inRow_rects_curr]).y=i;
				(inRow_rects[inRow_rects_curr]).height=1;
				inRow = true;
			}else{
				if(inRow){
					(inRow_rects[inRow_rects_curr]).width= j + - MIN_GRATE + 1 -(inRow_rects[inRow_rects_curr]).x;
					++inRow_rects_curr;
					inRow = false;
				}
			}
		}
		if(inRow){
			(inRow_rects[inRow_rects_curr]).width= j + - MIN_GRATE + 1 -(inRow_rects[inRow_rects_curr]).x;
			++inRow_rects_curr;
		}
		j=0;
		old = 0;
		if(j < inRow_rects_curr){ //Ako ima novih u redu
			tracking::Rect tmp = inRow_rects[0];
			while(old < global_rects_end){ //za svaki stari
				if(tmp.x <= global_rects[old].width + global_rects[old].x){               //ako nije to znaci da se stari nalazi pre tmp
					if(global_rects[old].x <= tmp.x + tmp.width){ //ako nije, tmp je dobar i treba ga upisati
						tmp = tmp | global_rects[old];
						while(j+1<inRow_rects_curr && inRow_rects[j+1].x <= tmp.x + tmp.width){ //odraditi sve za gornji
							tmp = tmp | inRow_rects[j];
							++j;
						}
					}else{
						inRow_rects[global_rects_curr++] = tmp; //upisujemo tmp
						if(++j < inRow_rects_curr){ //ako nema vise u redu threadovacemo ostatak starih
							tmp = inRow_rects[j];
							continue;
						}else{
							for(; old < global_rects_end; ++old){
								if(global_rects[old].area() > ERODE_THRESH)
									threads[thread_num++] = std::thread(searchSphere,global_rects[old]);
							}
							break;
						}
					}
				}else{
					if(global_rects[old].area() > ERODE_THRESH)
						threads[thread_num++] = std::thread(searchSphere,global_rects[old]);
				}
				++old;
			} //prosli smo sve stare, pa upisujemo nove ako ih ima jos
			do{
				inRow_rects[global_rects_curr++] = tmp;
				++j;
				tmp = inRow_rects[j];
			}while(j < inRow_rects_curr);
			Rect *tmpPtr = global_rects;
			global_rects = inRow_rects;
			inRow_rects = tmpPtr;

			global_rects_end = global_rects_curr;
			global_rects_curr = 0;
			inRow_rects_curr = 0;
		}else{//nema novih pa upisujemo stare ako ih ima
			if(old < global_rects_end){
				for(; old < global_rects_end; ++old){
					if(global_rects[old].area() > ERODE_THRESH)
						threads[thread_num++] = std::thread(searchSphere,global_rects[old]);
				}
				global_rects_end = 0;
			}
		}
	}
	//debug std::cout << thread_num << std::endl;
	for(old = 0; old < global_rects_end; ++old){
		if(global_rects[old].area() > ERODE_THRESH)
			threads[thread_num++] = std::thread(searchSphere,global_rects[old]);
	}
	for(i=0; i<thread_num; ++i){
		threads[i].join();
	}
}