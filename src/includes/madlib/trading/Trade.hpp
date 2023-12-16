#pragma once

namespace madlib::trading {
    
    // Define a struct for trade event data
    struct Trade {
        double volume;
        double price;
        ms_t timestamp;
    };

}