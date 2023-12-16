#pragma once

namespace madlib::graph {
    
    class Alignment {
    protected:
        Projector* projector;
        Projector* alignToProjector;
        bool extends;
    public:
        explicit Alignment(
            Projector* projector,
            Projector* alignToProjector = nullptr,
            bool extends = true
        ):
            projector(projector),
            alignToProjector(alignToProjector),
            extends(extends)
        {
            if (!projector) throw ERR_MISSING;
        }

        Projector* getProjector() const {
            return projector;
        }

        Projector* getAlignToProjector() const {
            return alignToProjector;
        }

        bool isExtends() const {
            return extends;
        }
    };

}