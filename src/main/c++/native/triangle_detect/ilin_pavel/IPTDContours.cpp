//
// Created by dialight on 17.12.16.
//

#include <triangle_detect/ilin_pavel/IPTDContours.hpp>
#include <utils/MacroEnumString.hpp>
#include <utils/CvUtils.hpp>
#include <opencv/OpenCVWrap.hpp>
#include <utils/DrawUtils.hpp>

#define IPTri2Macro(m) \
    m(SMOOTH, "Сглаживание") \
    m(GRAY, "Серые тона") \
    m(CANNY, "Метод Кенни") \
    m(FIND_CONTOURS, "Поиск контуров") \
    m(FILTER_CONTOURS, "Фильтр контуров по размеру") \
    m(APPROXIMATION, "Аппроксимация контуров") \
    m(FILTER_TRIANGLE_CONTOURS, "Фильтр треугольных контуров") \
    m(TO_TRIANGLES, "Треугольники") \
    m(FINAL, "Итог")
ENUM_STRING(IPTri2Macro, IPTri2Stages)

//3.1415926535897932384626433832795 / 2
#define HALF_PI 1.5707963267948966192313216916398

void IPTDContours::find(Mat &frame, vector<Triangle12i> &result) {

    int stage = props.getSelect("::IPTD.Contours.stage", &IPTri2Stages::all, IPTri2Stages::FINAL);

    CVWrap::gaussianBlur(frame, 7, 1.5);
    if(stage == IPTri2Stages::SMOOTH) return;

    cvtColor(frame, gray, COLOR_BGR2GRAY);
    if(stage == IPTri2Stages::GRAY) {
        frame = gray;
        return;
    }

    CVWrap::canny(gray);
    if(stage == IPTri2Stages::CANNY) {
        frame = gray;
        return;
    }

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    CVWrap::findContours(gray, contours, hierarchy);
    if(stage == IPTri2Stages::FIND_CONTOURS) {
        DrawUtils::showPointContours(frame, contours);
        stringstream ss;
        ss << "contours: " << contours.size();
        cv::putText(frame, ss.str(), Point2i(10, 22), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(0, 0, 255), 1, LineTypes::LINE_AA);
        return;
    }

    vector<vector<Point>> filteredContours;
    CvUtils::filterContoursBySize(contours, filteredContours);
    if(stage == IPTri2Stages::FILTER_CONTOURS) {
        DrawUtils::showPointContours(frame, filteredContours);
        stringstream ss;
        ss << "filtered contours: " << filteredContours.size();
        cv::putText(frame, ss.str(), Point2i(10, 22), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 255), 1, LineTypes::LINE_AA);
        int i = 2;
        for(auto &l : filteredContours) {
            stringstream ss2;
            ss2 << "contour(" << i - 1 << "): " << l.size();
            cv::putText(frame, ss2.str(), Point2i(10, 22 * i++), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 255), 1, LineTypes::LINE_AA);
        }
        return;
    }

    vector<vector<Point>> approximated;
    CVWrap::approxPolyDP(filteredContours, approximated);
    if(stage == IPTri2Stages::APPROXIMATION) {
        DrawUtils::showPointContours(frame, approximated);
        stringstream ss;
        ss << "approximated contours: " << approximated.size();
        cv::putText(frame, ss.str(), Point2i(10, 22), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 255), 1, LineTypes::LINE_AA);
        int i = 2;
        for(auto &l : approximated) {
            stringstream ss2;
            ss2 << "contour(" << i - 1 << "): " << l.size();
            cv::putText(frame, ss2.str(), Point2i(10, 22 * i++), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 255), 1, LineTypes::LINE_AA);
        }
        return;
    }

    vector<vector<Point>> filteredTriangleContours;
    for(auto const& contour : approximated) {
        if(contour.size() != 3) continue;
        filteredTriangleContours.push_back(contour);
    }
    if(stage == IPTri2Stages::FILTER_TRIANGLE_CONTOURS) {
        DrawUtils::showPointContours(frame, filteredTriangleContours);
        stringstream ss;
        ss << "filtered contours: " << filteredTriangleContours.size();
        cv::putText(frame, ss.str(), Point2i(10, 22), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 255), 1, LineTypes::LINE_AA);
        int i = 2;
        for(auto &l : filteredTriangleContours) {
            stringstream ss2;
            ss2 << "contour(" << i - 1 << "): " << l.size();
            cv::putText(frame, ss2.str(), Point2i(10, 22 * i++), FONT_HERSHEY_COMPLEX_SMALL, 0.5, Scalar(0, 0, 255), 1, LineTypes::LINE_AA);
        }
        return;
    }

    vector<Triangle12i> triangles;
    for(auto const& c : filteredTriangleContours) {
        triangles.push_back(Triangle6i(c[0], c[1], c[2]).to12i());
    }
    if(stage == IPTri2Stages::TO_TRIANGLES) {
        DrawUtils::showTriangles(gray, triangles);
        frame = gray;
        return;
    }

    int triangles_amount = (int) triangles.size();
    for (int i = 0; i < triangles_amount; i++) {
        for (int j = i + 1; j < triangles_amount; j++) {
            Triangle12i t1 = triangles[i];
            Triangle12i t2 = triangles[j];
            auto aabb1 = t1.getAABB();
            auto aabb2 = t2.getAABB();
            if(aabb1.contains(aabb2)) {
                result.push_back(t1);
            } else if(aabb2.contains(aabb1)) {
                result.push_back(t2);
            }
        }
    }

}

void IPTDContours::loop(Mat &frame) {
    vector<Triangle12i> triangles;
    find(frame, triangles);
    Triangle12i *big = nullptr;
    int bigArea = 0;
    for(auto &t : triangles) {
        int curArea = t.getAABB().area();
        if(big == nullptr || curArea > bigArea) {
            bigArea = curArea;
            big = &t;
        }
    }
    if(big == nullptr) return;
    if(last.isEmpty()) {
        color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        last = *big;
        edison.transmit();
        return;
    }

    const Point2i &p1 = big->getCenter();
    const Point2i &p2 = last.getCenter();
    Point2i v = p1 - p2;
    double distance = sqrt(v.x*v.x + v.y*v.y);
    if(distance > 100) {
        color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        edison.transmit();
    }else{
        cv::line(frame, p1, p2, color, 2, LineTypes::LINE_AA);
    }
    DrawUtils::showTriangle(frame, *big, color);
    last = *big;
}

