#pragma once

#include "Fees.hpp"
#include "Balance.hpp"
#include "Exchange.hpp"

namespace madlib::trading {
    
    class TestExchange: public Exchange {
    protected:

        vector<string> periods;
        vector<string> symbols;

        ms_t currentTime = 0;
        double currentPrice = 0;

        struct MarketOrderInfos {
            const double price;
            const Fees& fees;
            Balance& baseBalance;
            Balance& quotedBalance;
        };

        MarketOrderInfos getMarketOrderInfos(const string& symbol) {
            const Pair& pair = pairs.at(symbol);
            const Fees& fees = pair.getFees();
            const double price = pair.getPrice();
            return { 
                price, 
                fees, 
                balances.at(pair.getBaseCurrency()), 
                balances.at(pair.getQuotedCurrency()) 
            };
        }

    public:
        // struct Args {
        //     const vector<string>& periods;
        //     const vector<string>& symbols;
        //     const map<string, Pair>& pairs;
        //     const map<string, Balance>& balances;
        // };
        
        TestExchange(
            const vector<string>& periods,
            const vector<string>& symbols,
            const map<string, Pair>& pairs,
            const map<string, Balance>& balances
        ): 
            Exchange(), 
            periods(periods),
            symbols(symbols)
        {
            this->pairs = pairs;
            this->balances = balances;
        }

        virtual ~TestExchange() {}

        // virtual void init(void* = nullptr) override {

        // }

        virtual vector<string> getPeriods() const override {
            return periods;
        }

        virtual vector<string> getSymbols() const override {
            return symbols;
        }

        void setCurrentTime(ms_t currentTime) {
            this->currentTime = currentTime;
        }

        virtual ms_t getCurrentTime() const override {
            return currentTime;
        }

        virtual bool marketBuy(const string& symbol, double amount, bool throws = true) override {
            MarketOrderInfos marketOrderInfos = getMarketOrderInfos(symbol);
            double cost = amount * marketOrderInfos.price;
            double fee = amount * marketOrderInfos.fees.getMarketBuyPc();
            return marketOrderInfos.quotedBalance.decrement(cost, throws)
                && marketOrderInfos.baseBalance.increment(amount - fee, throws);
        }

        virtual bool marketSell(const string& symbol, double amount, bool throws = true) override {
            MarketOrderInfos marketOrderInfos = getMarketOrderInfos(symbol);
            double cost = amount * marketOrderInfos.price;
            double fee = cost * marketOrderInfos.fees.getMarketSellPc();
            return marketOrderInfos.baseBalance.decrement(amount, throws)
                && marketOrderInfos.quotedBalance.increment(cost - fee, throws);
        }

        // TODO:
        // virtual void limitBuy(const string& symbol, double amount, double limitPrice) {
        //     throw ERR_UNIMP;
        // }

        // TODO:
        // virtual void limitSell(const string& symbol, double amount, double limitPrice) {
        //     throw ERR_UNIMP;
        // }
    };

}