#pragma once

#include "Mixed.hpp"

namespace madlib::graph {

    class SettingsHolder {
    protected:

        vector<Mixed> settings;

    public:

        SettingsHolder() {}

        virtual ~SettingsHolder() {}

        vector<Mixed> getSettings() {
            return settings;
        }

        const string& getSettingsValueAsString(const string& name) const {
            for (const Mixed& mixed: settings)
                if (mixed.getName() == name) 
                    return mixed.getValueAsString();
            throw ERROR("Settings value is missing: " + name);
        }

        double getSettingsValueAsDouble(const string& name) const {
            return parse<double>(getSettingsValueAsString(name));
        }

        long getSettingsValueAsLong(const string& name) const {
            return parse<long>(getSettingsValueAsString(name));
        }

        bool getSettingsValueAsBool(const string& name) const {
            return parse_bool(getSettingsValueAsString(name));
        }

        virtual bool isValidSettings(const vector<Mixed>&) {
            throw ERR_UNIMP;
        }

        virtual bool setSettings(const vector<Mixed>&) {
            throw ERR_UNIMP;
        }

        void dumpSettings(vector<Mixed>* settings = nullptr) {
            const vector<Mixed>& _settings = settings ? *settings : this->settings;
            string out = "Settings:";
            for (const Mixed& mixed: _settings)
                out += "\n" + mixed.getName() + " = " + mixed.getValueAsString() + " (" + mixed.getTypeName() + ")";
            DBG(out);
        }
    };

}