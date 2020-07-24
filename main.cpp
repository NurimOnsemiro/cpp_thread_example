#include <iostream>
#include <vector>
#include <future>
#include <fstream>
#include <sstream>

#include "hourmeter.h"

using namespace std;

int numThreads = 16;
bool useThread = false;
long long jobSize = 100;

long long doSomething(long long jobSize){
    long long data {0};
    for(long long i=0;i<jobSize;i++){
        data += i;
    }
    return data;
}

void setupEnvInfo(){
    ifstream inFile("./env_info.config");
    if(!inFile.is_open()){
        cout <<"env_info.config file does not exist" << endl;
        exit(0);
    }

    while(!inFile.eof()){
        string line;
        getline(inFile, line);
        istringstream ss(line);
        string key, value;
        getline(ss, key, '=');
        getline(ss, value, '=');

        if(key == "jobSize"){
            jobSize = std::stoll(value);
        }
        else if(key == "numThreads"){
            numThreads = std::stoi(value);
        }
        else if(key == "useThread"){
            useThread = std::stoi(value);
        }
    }
    inFile.close();

    cout << "jobSize : " << jobSize << endl;
    cout << "numThreads : " << numThreads << endl;
    cout << "useThread : " << useThread << endl;
}

int main()
{
    setupEnvInfo();

    HourMeter hm;
    vector<long long> resultArr;
    resultArr.resize(numThreads + 1);

    hm.startMeasure();

    if(useThread){
        future<long long> thr[numThreads];
        for(int i=0;i<numThreads;i++){
            thr[i] = async(launch::async, doSomething, jobSize);
        }

        for(int i=0;i<numThreads;i++){
            resultArr[i] = thr[i].get();
        }
    }
    else {
        for(int i=0;i<numThreads;i++){
            resultArr[i] = doSomething(jobSize);
        }
    }

    hm.endMeasure();

    for(int i=0;i<numThreads;i++){
        cout << "result " << i << " : " << resultArr[i] << endl;
    }

    return 0;
}
