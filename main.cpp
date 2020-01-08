#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/ximgproc/run_length_morphology.hpp>
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;


template <typename T>
class RLEMat {
public:
    struct Encoded {
        ushort count = 0;
        T num = 0;
    };

public:
    RLEMat(const Mat &rawData)
    {

        for (int row = 0; row < rawData.rows; ++row) {
            _rowStarts.push_back(static_cast<int>(std::distance(_encoded.begin(), _encoded.end())));

            T lastEntry = rawData.at<T>(row, 0, 0);
            Encoded currentEncoded;
            currentEncoded.num = lastEntry;
            for (int col = 0; col < rawData.cols; ++col) {
                T currentItem = rawData.at<T>(row, col, 0);
                if (currentItem != lastEntry) {
                    _encoded.push_back(currentEncoded);
                    currentEncoded = Encoded();
                    currentEncoded.num = currentItem;
                }
                ++currentEncoded.count;
                lastEntry = currentItem;
            }

            _encoded.push_back(currentEncoded);
        }
    }

    T at(int row, int col) const
    {
        int rowStart = _rowStarts.at(size_t(row));
        int rowEnd = row + 1 == int(_rowStarts.size()) ? int(_encoded.size()) : _rowStarts.at(size_t(row + 1));


        int lookupOperations = 0;

        int accumulator = 0;
        for (auto it = std::next(_encoded.begin(), rowStart);
             it != std::next(_encoded.begin(), rowEnd);
             ++it) {
            ++lookupOperations;
            accumulator += it->count;
            if (accumulator > col) {
                cout << "Lookup operations count:" << lookupOperations << endl;
                return it->num;
            }
        }
        throw std::runtime_error("Indices (" + to_string(row) + ',' + to_string(col) + ") are out of range.");
    }

    size_t size() const noexcept {
        return _encoded.size();
    }

private:
    std::vector<Encoded> _encoded;
    std::vector<int> _rowStarts;
};


namespace  {
int gCurrentFrame = 0;
std::vector<RLEMat<uchar>> rleFrames;
}


static void onClick( int event, int x, int y, int, void* )
{
    if( event != EVENT_LBUTTONDOWN )
        return;
    cout << x << ' ' << y << endl;
    auto & frame = rleFrames.at(size_t(gCurrentFrame));
    cout << "VAL: " << int(frame.at(y, x)) << endl;
}

int main( int argc, char** argv)
{

    namedWindow("image");
    const String keys =
        "{help h usage ? |      | print this message   }"
        "{@video         |      | video path           }"
        ;
    cv::CommandLineParser parser(argc, argv, keys);

    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    string videoPath = parser.get<string>(0);

    vector<Mat> frames;

    {
        VideoCapture cap;
        cap.open(videoPath, CAP_FFMPEG);
        while (true) {
            Mat frame;
            cap >> frame;
            if (frame.empty()) {
                break;
            }
            Mat grayFrame;
            cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
            frames.push_back(grayFrame);
        }
    }


    std::transform(frames.begin(), frames.end(), std::back_inserter(rleFrames), [](const Mat& mat){
        return RLEMat<uchar>(mat);
    });


    int totalEncodingsCount = 0;
    for (auto && rleFrame : rleFrames) {
        totalEncodingsCount += rleFrame.size();
    }

    int encodingByteSize = sizeof (RLEMat<uchar>::Encoded) / sizeof (int);
    cout << "Size of encoded:" << encodingByteSize << endl;

    int compressedMbSize = totalEncodingsCount * encodingByteSize / 1000000;
    cout << "Size of RLE compressed frames:" << compressedMbSize << endl;

    auto sampleFrame = frames[0];
    cout << "Frame size:" << sampleFrame.size[0] << ' ' << sampleFrame.size[1] << endl;
    int uncompressedMbSize = sampleFrame.size[0] * sampleFrame.size[1] * int(frames.size()) * 3 / 1000000;
    cout << "Size of uncompressed frames:"
         << uncompressedMbSize
         << endl;

    cout << "Compression Ratio: " << float(uncompressedMbSize) / float(compressedMbSize) << endl;

    setMouseCallback("image", onClick);
    createTrackbar( "frame", "image", &gCurrentFrame, int(frames.size() - 1));

    for (;;) {

        imshow("image", frames[size_t(gCurrentFrame)]);
        int c = waitKey(0);
        if (c == 'x') {
            break;
        }
    }

    return 0;
}
