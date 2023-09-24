#include <iostream>
#include <string>
#include "src/Files.h"
#include "src/System.h"
#include "src/Vector.h"

using namespace std;

void exec(string cmd) {
    cout << "EXEC:" << cmd << endl;
    string output = System::exec(cmd);
    if (!output.empty()) {
        cout << output << endl;
    }
}

int main(/* int argc, char *argv[] */) {

    cout << "Build in progress.." << endl;

    const string buildPath = "build/";

    vector<string> files = Files::find("./src", ".cpp");

    vector<string> oFiles;
    for (const string& file : files) {
        string path = Files::normalizePath(buildPath + Files::extractPath(file));
        cout << "PATH:" << path << endl;
        if (!Files::exists(path)) {
            cout << "CREATE:" << path << endl;
            Files::createPath(path);
        }
        string oFile = buildPath + Files::normalizePath(Files::replaceExtension(file, "o"));
        if (
            !Files::exists(oFile) ||
            Files::getLastModificationTime(oFile) < Files::getLastModificationTime(file)
        ) {
            exec("g++ -c " + file + " -o " + oFile);
        }
        oFiles.push_back(oFile);
    }

    exec("g++ -o " + buildPath + "main " + Vector::concat(oFiles) + " -lX11");
    exec("./" + buildPath + "/main");

    return 0;
}