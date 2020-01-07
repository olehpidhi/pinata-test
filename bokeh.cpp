#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;

namespace  {

int DELAY_CAPTION = 1500;
char window_name[] = "Bokeh Demo";

}

int main( int argc, char** argv )
{
    const String keys =
        "{help h usage ? |      | print this message   }"
        "{@image1        |      | image path           }"
        "{@image2        |      | mask path            }"
        "{radius r       |      | blur radius          }"
        ;

    CommandLineParser parser(argc, argv, keys);
    parser.about("Bokeh effect demo");
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }

    string imagePath = parser.get<string>(0);
    string maskPath = parser.get<string>(1);

    int radius = parser.get<int>("radius");

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }

    namedWindow( window_name, WINDOW_AUTOSIZE );

    Mat src = imread(imagePath);
    Mat srcMask = imread(maskPath);

    imshow( window_name, src );
    waitKey( DELAY_CAPTION );

    auto back = src - srcMask;

    Mat blurredBack;
    GaussianBlur(back, blurredBack, Size(radius, radius), 0);

    imshow( window_name, blurredBack + srcMask );
    waitKey(0);
    return 0;
}

