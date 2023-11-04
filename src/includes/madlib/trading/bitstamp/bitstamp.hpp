#pragma once

#include <iostream>
#include <string>

#include "../../madlib.hpp"
#include "../trading.hpp"

using namespace std;
using namespace madlib;

namespace madlib::trading::bitstamp {
    
    enum BitstampCandlesCsvField { UNIX = 0, DATE, SYMBOL, OPEN, HIGH, LOW, CLOSE, VOLUME_BASE, VOLUME_QUOTED };
    const string datPath = __DIR__ + "/data/";
    const string datFileTpl = datPath + "{symbol}_{year}_{period}.dat";

    vector<Candle> bitstamp_read_candle_history_dat(const string& datFile) {
        return vector_load<Candle>(datFile);
    }

    vector<Candle> bitstamp_parse_candle_history_csv(
        const string& csvFile, const string& datFile, 
        bool readOutFileIfExists = true,
        bool throwIfOutFileExists = true
    ) {
        if (!file_exists(csvFile)) throw ERROR("File not found: " + csvFile);
        if (file_exists(datFile)) {
            if (readOutFileIfExists) return bitstamp_read_candle_history_dat(datFile);
            if (throwIfOutFileExists) throw ERROR("File already exists: " + datFile);
        }
        logger.date().writeln(
            string("Parsing Bitstamp candle history data:\b")
                + "csv input file   : " + csvFile + "\n"
                + "output data file : " + datFile
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
        logger.date().writeln("Writing: [" + datFile + "]");
        vector_save(datFile, candles);
        return candles;
    }

    void bitstamp_parse_candle_history_csv_all(
        const string& symbol, int yearFrom, int yearTo, const string& period = "minute"
    ) {
        const string csvPath = __DIR__ + "/download/cryptodatadownload.com/bitstamp/";
        const string csvFileTpl = csvPath + "/Bitstamp_{symbol}_{year}_{period}.csv";
        const map<string, string> repl = {
            {"{symbol}", symbol},
            {"{period}", period},
        };
        string _datFileTpl = str_replace(datFileTpl, repl);
        string _csvFileTpl = str_replace(datFileTpl, repl);
        for (int year = yearFrom; year <= yearTo; year++) {
            const string csvFile = str_replace(_csvFileTpl, "{year}", to_string(year));
            const string datFile = str_replace(_datFileTpl, "{year}", to_string(year));
            bitstamp_parse_candle_history_csv(csvFile, datFile, false, false);
        }
    }

    class BitstampHistory: public TradeHistory {
    protected:
        
        void initCandles() override {
            if (period != MS_PER_MIN) throw ERROR("Period works only on minutes charts"); // TODO: aggregate to other periods
            int yearFrom = parse<int>(ms_to_datetime(startTime).substr(0, 4));
            int yearTo = parse<int>(ms_to_datetime(endTime).substr(0, 4));
            string _datFileTpl = str_replace(datFileTpl, {
                {"{symbol}", symbol},
                {"{period}", "minute"},
            });
            for (int year = yearFrom; year <= yearTo; year++) {
                const string datFile = str_replace(_datFileTpl, "{year}", to_string(year));
                vector<Candle> yearCandles = bitstamp_read_candle_history_dat(datFile);
                for (const Candle& candle: yearCandles) {
                    if (startTime <= candle.getStart() || endTime >= candle.getEnd())
                        candles.push_back(candle);
                }
            }
        };

    public:
        BitstampHistory(
            const string &symbol, const ms_t startTime, const ms_t endTime, const ms_t period
        ):
            TradeHistory(symbol, startTime, endTime, period) 
        {
            initCandles();
        }

    };
}


