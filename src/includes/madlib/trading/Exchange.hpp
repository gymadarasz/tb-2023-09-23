#pragma once

#include <map>
#include <string>

#include "../time.hpp"

#include "Pair.hpp"
#include "Balance.hpp"

namespace madlib::trading {
    
    class Exchange {
    protected:
        
        map<string, Pair> pairs;
        map<string, Balance> balances;

    public:

        Exchange() {}

        virtual ~Exchange() {}

        virtual vector<string> getPeriods() const {
            throw ERR_UNIMP;
        }

        virtual vector<string> getSymbols() const {
            throw ERR_UNIMP;
        }

        Pair& getPairAt(const string& symbol) {
            if (pairs.count(symbol) == 1) return pairs.at(symbol);
            throw ERROR("No symbol: " + symbol);
        }

        const map<string, Balance>& getBalances() const {
            return balances;
        }

        double getBalanceBase(const string& symbol) {
            return getBalances().at(getPairAt(symbol).getBaseCurrency()).getAmount();
        }

        double getBalanceBase(const Pair& pair) {
            return getBalances().at(pair.getBaseCurrency()).getAmount();
        }

        double getBalanceQuoted(const string& symbol) {
            return getBalances().at(getPairAt(symbol).getQuotedCurrency()).getAmount();
        }

        double getBalanceQuoted(const Pair& pair) {
            return getBalances().at(pair.getQuotedCurrency()).getAmount();
        }

        double getBalanceQuotedFull(const string& symbol) {
            const Pair& pair = getPairAt(symbol);
            return getBalanceQuoted(symbol) + getBalanceBase(symbol) * pair.getPrice();
        }

        double getBalanceQuotedFull(const Pair& pair) {
            return getBalanceQuoted(pair) + getBalanceBase(pair) * pair.getPrice();
        }

        double getBalanceBaseFull(const string& symbol) {
            const Pair& pair = getPairAt(symbol);
            return getBalanceQuoted(symbol) / pair.getPrice() + getBalanceBase(symbol);
        }

        double getBalanceBaseFull(const Pair& pair) {
            return getBalanceQuoted(pair) / pair.getPrice() + getBalanceBase(pair);
        }

        virtual ms_t getCurrentTime() const {
            throw ERR_UNIMP;
        }

        virtual bool marketBuy(const string& /*symbol*/, double /*amount*/, bool = true) {
            throw ERR_UNIMP;
        }

        virtual bool marketSell(const string& /*symbol*/, double /*amount*/, bool = true) {
            throw ERR_UNIMP;
        }

        virtual void limitBuy(const string& /*symbol*/, double /*amount*/, double /*limitPrice*/) {
            throw ERR_UNIMP;
        }

        virtual void limitSell(const string& /*symbol*/, double /*amount*/, double /*limitPrice*/) {
            throw ERR_UNIMP;
        }
    };

}