#pragma once

namespace madlib::graph {
    
    class Alignment {
    protected:
        Projector* projector;
        Projector* alignToProjector;
        // bool extends;
    public:
        explicit Alignment(
            Projector* projector,
            Projector* alignToProjector = nullptr //,
            // bool extends = true
        ):
            projector(projector),
            alignToProjector(alignToProjector) //,
            // extends(extends)
        {
            if (!projector) throw ERR_MISSING;
        }

        Projector* getProjector() const {
            return projector;
        }

        Projector* getAlignToProjector() const {
            return alignToProjector;
        }

        // bool isExtends() const {
        //     return extends;
        // }

        bool fitProjectorsCanvas() const {
            if (!alignToProjector) return true;

            Projector::Canvas& projectorCanvas = projector->getCanvas();
            Projector::Canvas& alignToProjectorCanvas = alignToProjector->getCanvas();
            if (!projectorCanvas.stretched) return true;
            if (!alignToProjectorCanvas.stretched) return true;
            if (projectorCanvas.isAlignedWith(alignToProjectorCanvas)) return true;

            double shapesValueMinMin = min(
                projectorCanvas.shapesValueMin,
                alignToProjectorCanvas.shapesValueMin
            );
            projectorCanvas.shapesValueMin = shapesValueMinMin;
            alignToProjectorCanvas.shapesValueMin = shapesValueMinMin;

            double shapesValueMaxMax = max(
                projectorCanvas.shapesValueMax,
                alignToProjectorCanvas.shapesValueMax
            );
            projectorCanvas.shapesValueMax = shapesValueMaxMax;
            alignToProjectorCanvas.shapesValueMax = shapesValueMaxMax;

            double shapesValueDiff = shapesValueMaxMax - shapesValueMinMin;
            projectorCanvas.shapesValueDiff = shapesValueDiff;
            alignToProjectorCanvas.shapesValueDiff = shapesValueDiff;

            int chartHeightMax = max(
                projectorCanvas.chartHeight,
                alignToProjectorCanvas.chartHeight
            );
            projectorCanvas.chartHeight = chartHeightMax;
            alignToProjectorCanvas.chartHeight = chartHeightMax;

            int chartWidthMax = max(
                projectorCanvas.chartWidth,
                alignToProjectorCanvas.chartWidth
            );
            projectorCanvas.chartWidth = chartWidthMax;
            alignToProjectorCanvas.chartWidth = chartWidthMax;

            ms_t chartBeginMin = min(
                projectorCanvas.chartBegin,
                alignToProjectorCanvas.chartBegin
            );
            projectorCanvas.chartBegin = chartBeginMin;
            alignToProjectorCanvas.chartBegin = chartBeginMin;

            ms_t chartEndMax = max(
                projectorCanvas.chartEnd,
                alignToProjectorCanvas.chartEnd
            );
            projectorCanvas.chartEnd = chartEndMax;
            alignToProjectorCanvas.chartEnd = chartEndMax;

            ms_t chartInterval = chartEndMax - chartBeginMin;
            projectorCanvas.chartInterval = chartInterval;
            alignToProjectorCanvas.chartInterval = chartInterval;

            return false;
        }
    };

}