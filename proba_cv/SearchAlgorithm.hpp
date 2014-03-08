
#ifndef SEARCH_ALGORITHM_HPP
#define SEARCH_ALGORITHM_HPP

#include "defs.hpp"
#include "Marker.hpp"

bool initializeStart();																	
void compareClossest();													
void sortMarkers();	
void setUpMarkers();							
void drawLines();

void calculateRectangle(cv::Mat &frameBGR);
void searchSphere(tracking::Rect rect);

#endif