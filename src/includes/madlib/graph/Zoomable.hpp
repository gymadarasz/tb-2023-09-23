#pragma once

#include "defs.hpp"

namespace madlib::graph {

    class Zoomable { // TODO: remove Zoomable, it's now YAGNI
    protected:
        const int defaultZoomCenterX = 0, defaultZoomCenterY = 0; // TODO
        const double defaultZoomRatioX = 1.0, defaultZoomRatioY = 1.0;

        Pixel zoomCenter = Pixel(defaultZoomCenterX, defaultZoomCenterY);
        Coord zoomRatio = Coord(defaultZoomRatioX, defaultZoomRatioY);

        bool zoomFixedX;
        bool zoomFixedY;
    public:

        Zoomable(bool zoomFixed = true): zoomCenter(defaultZoomCenterX, defaultZoomCenterY), zoomRatio(defaultZoomRatioX, defaultZoomRatioY), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        Zoomable(int centerX, int centerY, bool zoomFixed = true): zoomCenter(centerX, centerY), zoomRatio(defaultZoomRatioX, defaultZoomRatioY), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        explicit Zoomable(const Pixel& center, bool zoomFixed = true): zoomCenter(center), zoomRatio(defaultZoomRatioX, defaultZoomRatioY), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        Zoomable(double ratioX, double ratioY, bool zoomFixed = true): zoomCenter(defaultZoomCenterX, defaultZoomCenterY), zoomRatio(ratioX, ratioY), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        explicit Zoomable(const Coord& ratio, bool zoomFixed = true): zoomCenter(defaultZoomCenterX, defaultZoomCenterY), zoomRatio(ratio), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        Zoomable(int centerX, int centerY, double ratioX, double ratioY, bool zoomFixed = true): zoomCenter(centerX, centerY), zoomRatio(ratioX, ratioY), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        Zoomable(const Pixel& center, const Coord& ratio, bool zoomFixed = true): zoomCenter(center), zoomRatio(ratio), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        Zoomable(double ratioX, double ratioY, int centerX, int centerY, bool zoomFixed = true): zoomCenter(centerX, centerY), zoomRatio(ratioX, ratioY), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        Zoomable(const Coord& ratio, const Pixel& center, bool zoomFixed = true): zoomCenter(center), zoomRatio(ratio), zoomFixedX(zoomFixed), zoomFixedY(zoomFixed) {}
        
        virtual ~Zoomable() {}

        void setZoomFixedX(bool zoomFixedX) {
            this->zoomFixedX = zoomFixedX;
        }

        bool isZoomFixedX() const {
            return zoomFixedX;
        }

        void setZoomFixedY(bool zoomFixedY) {
            this->zoomFixedY = zoomFixedY;
        }

        bool isZoomFixedY() const {
            return zoomFixedY;
        }

        void setZoomFixed(bool zoomFixed) {
            zoomFixedX = zoomFixed;
            zoomFixedY = zoomFixed;
        }

        Zoomable& setZoomFrom(const Zoomable& other) {
            if (this != &other) {
                setZoomCenter(other.zoomCenter);
                setZoomRatio(other.zoomRatio);
            }
            return *this;
        }

        Pixel getZoomCenter() const {
            return zoomCenter;
        }

        void setZoomCenter(const Pixel& zoomCenter) {
            setZoomCenterX(zoomCenter.x);
            setZoomCenterY(zoomCenter.y);
        }

        void setZoomCenter(int x = 0, int y = 0) {
            setZoomCenterX(x);
            setZoomCenterY(y);
        }

        void setZoomCenterX(int x = 0) {
            if (!zoomFixedX) 
                zoomCenter.x = x;
        }

        void setZoomCenterY(int y = 0) {
            if (!zoomFixedY) 
                zoomCenter.y = y;
        }

        Coord getZoomRatio() const {
            return zoomRatio;
        }

        void setZoomRatio(const Coord& zoomRatio) {
            setZoomRatioX(zoomRatio.x);
            setZoomRatioY(zoomRatio.y);
        }

        void setZoomRatio(double x = 1.0, double y = 1.0) {
            setZoomRatioX(x);
            setZoomRatioY(y);
        }

        void setZoomRatioX(double x) {
            if (!zoomFixedX) zoomRatio.x = x;
        }

        void setZoomRatioY(double y) {
            if (!zoomFixedY) zoomRatio.y = y;
        }

        int applyZoomX(int origoX, int pointX) const {
            int origoXAddCenterX = origoX + zoomCenter.x;
            int pointXSubCenterX = pointX - origoXAddCenterX;
            double pointXSubCenterXMulRatioX = pointXSubCenterX * zoomRatio.x;
            return origoXAddCenterX + (int)pointXSubCenterXMulRatioX;
        }

        int applyZoomY(int origoY, int pointY) const {
            int origoYAddCenterY = origoY + zoomCenter.y;
            int pointYSubCenterY = pointY - origoYAddCenterY;
            double pointYSubCenterYMulRatioY = pointYSubCenterY * zoomRatio.y;
            return origoYAddCenterY + (int)pointYSubCenterYMulRatioY;
        }

        Pixel applyZoom(int origoX, int origoY, int pointX, int pointY) const {
            Pixel result(
                applyZoomX(origoX, pointX),
                applyZoomY(origoY, pointY)
            );

            return result;
        }
        
        Pixel applyZoom(const Pixel& origo, const Pixel& point) const {
            return applyZoom(origo.x, origo.y, point.x, point.y);
        }
    };

}