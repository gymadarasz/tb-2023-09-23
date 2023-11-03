#pragma once

#include <iostream>
#include <string>

#include "../../madlib.hpp"
#include "../trading.hpp"

using namespace std;
using namespace madlib;

namespace madlib::trading::bitstamp {
    
    enum BitstampCandlesCsvField { UNIX = 0, DATE, SYMBOL, OPEN, HIGH, LOW, CLOSE, VOLUME_BASE, VOLUME_QUOTED };

    vector<Candle> bitstamp_read_candle_history_dat(const string& datFile) {
        return vector_load<Candle>(datFile);
    }

    vector<Candle> bitstamp_parse_candle_history_csv(
        const string& csvFile, const string& outFile, 
        bool readOutFileIfExists = true,
        bool throwIfOutFileExists = true
    ) {
        if (!file_exists(csvFile)) throw ERROR("File not found: " + csvFile);
        if (file_exists(outFile)) {
            if (readOutFileIfExists) return bitstamp_read_candle_history_dat(outFile);
            if (throwIfOutFileExists) throw ERROR("File already exists: " + outFile);
        }
        logger.date().writeln(
            string("Parsing Bitstamp candle history data:\b")
                + "csv input file   : " + csvFile + "\n"
                + "output data file : " + outFile
        );
        vector<string> csvData = split("\n", file_get_contents(csvFile));

        vector<Candle> candles;
        const size_t csvDataSize = csvData.size();
        const size_t headerLines = 2;
        
        string firstLine = trim(csvData.at(headerLines));
        vector<string> firstSplits = split(",", firstLine);
        string firstDate = trim(firstSplits.at(BitstampCandlesCsvField::DATE));
        string secondLine = trim(csvData.at(headerLines + 1));
        vector<string> secondSplits = split(",", secondLine);
        string secondDate = trim(secondSplits.at(BitstampCandlesCsvField::DATE));        
        ms_t period = datetime_to_ms(firstDate) - datetime_to_ms(secondDate);

        for (size_t i = csvDataSize - 1; i >= headerLines; i--) {
            string line = trim(csvData.at(i));
            if (line.empty()) continue;
            if (i % 1000 == 0) logger.date().writeln("Parse CSV: [" + line + "]");
            vector<string> fields = split(",", line);
            if (!is_numeric(trim(fields.at(BitstampCandlesCsvField::UNIX)))) break;            
            double open = parse<double>(trim(fields.at(BitstampCandlesCsvField::OPEN)));
            double close = parse<double>(trim(fields.at(BitstampCandlesCsvField::CLOSE)));
            double low = parse<double>(trim(fields.at(BitstampCandlesCsvField::LOW)));
            double high = parse<double>(trim(fields.at(BitstampCandlesCsvField::HIGH)));
            double volume = parse<double>(trim(fields.at(BitstampCandlesCsvField::VOLUME_BASE)));
            ms_t start = datetime_to_ms(trim(fields.at(BitstampCandlesCsvField::DATE)));
            ms_t end = start + period - 1;
            Candle candle(open, close, low, high, volume, start, end);
            candles.push_back(candle);
        }
        logger.date().writeln("Writing: [" + outFile + "]");
        vector_save(outFile, candles);
        return candles;
    }

    void bitstamp_parse_candle_history_csv_all(
        const string& symbol, int yearFrom, int yearTo, const string& period = "minute"
    ) {
        const string csvPath = __DIR__ + "/download/cryptodatadownload.com/bitstamp/";
        const string outPath = __DIR__ + "/data/";
        const string csvFileTpl = csvPath + "/Bitstamp_{symbol}_{year}_{period}.csv";
        const string outFileTpl = outPath + "{symbol}_{year}_{period}.dat";
        for (int year = yearFrom; year <= yearTo; year++) {
            const map<string, string> repl = {
                {"{symbol}", symbol},
                {"{year}", to_string(year)},
                {"{period}", period}
            };
            const string csvFile = str_replace(csvFileTpl, repl);
            const string outFile = str_replace(outFileTpl, repl);
            bitstamp_parse_candle_history_csv(csvFile, outFile, false, false);
        }
    }
}


