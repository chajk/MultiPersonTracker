#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>

using namespace std;
using namespace cv;

vector<string> trackerTypes = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT" };

// create tracker by name
Ptr<Tracker> createTrackerByName(string trackerType)
{
	Ptr<Tracker> tracker;
	if (trackerType == trackerTypes[0])
		tracker = TrackerBoosting::create();
	else if (trackerType == trackerTypes[1])
		tracker = TrackerMIL::create();
	else if (trackerType == trackerTypes[2])
		tracker = TrackerKCF::create();
	else if (trackerType == trackerTypes[3])
		tracker = TrackerTLD::create();
	else if (trackerType == trackerTypes[4])
		tracker = TrackerMedianFlow::create();
	else if (trackerType == trackerTypes[5])
		tracker = TrackerGOTURN::create();
	else if (trackerType == trackerTypes[6])
		tracker = TrackerMOSSE::create();
	else if (trackerType == trackerTypes[7])
		tracker = TrackerCSRT::create();
	else {
		cout << "Incorrect tracker name" << endl;
		cout << "Available trackers are: " << endl;
		for (vector<string>::iterator it = trackerTypes.begin(); it != trackerTypes.end(); ++it)
			std::cout << " " << *it << endl;
	}
	return tracker;
}

// Fill the vector with random colors
void getRandomColors(vector<Scalar> &colors, int numColors)
{
	RNG rng(0);
	for (int i = 0; i < numColors; i++)
		colors.push_back(Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));
}


int main()
{
	cout << "Default tracking algoritm is CSRT" << endl;
	cout << "Available tracking algorithms are:" << endl;

	for (vector<string>::iterator it = trackerTypes.begin(); it != trackerTypes.end(); ++it)
		std::cout << " " << *it << endl;

	// Set tracker type. Change this to try different trackers.
	string trackerType = "CSRT";

	// set default values for tracking algorithm and video
	string videoPath = "person2_640_480.mp4";

	// Initialize MultiTracker with tracking algo
	vector<Rect> bboxes;

	// create a video capture object to read videos
	cv::VideoCapture cap(videoPath);
	Mat frame;
	Mat edgeFrame;
	
	// quit if unabke to read video file
	if (!cap.isOpened())
	{
		cout << "Error opening video file " << videoPath << endl;
		return -1;
	}

	// read first frame
	cap >> frame;

	// draw bounding boxes over objects
    // selectROI's default behaviour is to draw box starting from the center
    // when fromCenter is set to false, you can draw box starting from top left corner
	bool showCrosshair = true;
	bool fromCenter = false;
	cout << "\n==========================================================\n";
	cout << "OpenCV says press c to cancel objects selection process" << endl;
	cout << "It doesn't work. Press Escape to exit selection process" << endl;
	cout << "\n==========================================================\n";
	cv::selectROIs("TOPOS:MultiTracker", frame, bboxes, showCrosshair, fromCenter);


	// quit if there are no objects to track
	if (bboxes.size() < 1)
		return 0;
	
	vector<Scalar> colors;
	getRandomColors(colors, bboxes.size());

	// Create multitracker
	Ptr<MultiTracker> multiTracker = cv::MultiTracker::create();

	// initialize multitracker
	for (int i = 0; i < bboxes.size(); i++)
		multiTracker->add(createTrackerByName(trackerType), frame, Rect2d(bboxes[i]));

	// process video and track objects
	cout << "\n==========================================================\n";
	cout << "Started tracking, press ESC to quit." << endl;

	while (cap.isOpened())
	{
		// get frame from the video
		cap >> frame;
		
		// stop the program if reached end of video
		if (frame.empty()) 
			break;

		GaussianBlur(frame, frame, Size(3, 3), 0, 0, BORDER_DEFAULT);
	    
		cvtColor(frame, frame, CV_RGB2GRAY);

		Sobel(frame, frame, CV_8U, 1, 0);


		//update the tracking result with new frame
		multiTracker->update(frame);

		// draw tracked objects
		for (unsigned i = 0; i < multiTracker->getObjects().size(); i++)
		{
			rectangle(frame, multiTracker->getObjects()[i], colors[i], 2, 1);
		}

		// show frame
		imshow("TOPOS:MultiTracker", frame);

		// quit on x button
		if (waitKey(1) == 27) 
			break;
	}


/*

	if (!capture.isOpened())
	{
		printf("AVI file can not open.\n");
		return 0;
	}

	namedWindow("W");

	while (1) {
		capture >> img;
		if (img.empty()) 
			break;

		Sobel(img, img, img.depth(), 1, 0);

		imshow("W", img);

		if (waitKey(10) > 0)
			break;
	}
*/
	return 0;
}