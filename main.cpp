#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <map>
#include <algorithm>

using namespace std;

typedef vector<int> vi;
typedef vector<string> vs;
typedef unsigned int ui;
typedef pair<int, int> paii;
typedef map<int, int, greater<int>> maii;

const string PATH = "/Users/stichiboi/Downloads/";
const string INPUT_TAG = "input";
const string OUTPUT_TAG = "output";
const string APPENDIX = ".in";
const string FILE_NUM = "5";

const paii PAII_ERROR_CODE(-1,-1);

maii available;
maii taken;
int maxSlices;
int currSlices;
vi allThePizzas;

//--------------------------------------------------------
template <class t>
int findInVector(vector<t> v, t element){
    //Returns the index at which the element is stored
    int i;
    for(i = 0; i < v.size(); i++){
        if(v[i] == element){
            return i;
        }
    }
    return i;
}

template <class t>
vector<t> flipVector(vector<t> in) {
    //Returns the vector flipped
    vector<t> out;
    for (int i = (int)in.size() - 1; i >= 0; i--) {
        out.push_back(in[i]);
    }
    return out;
}

vector<string> separateLine(string line, char separator = ' ') {
    size_t size = line.size();
    vector<string> words;
    string curr = "";
    char c;
    for (ui i = 0; i < size; i++) {
        c = line[i];
        if (c == separator) {
            words.push_back(curr);
            curr.clear();
        }
        else {
            curr += c;
        }
    }
    words.push_back(curr);
    return words;
}

template <class t>
t stringToInt(string s) {
    //This function is required as sometimes stoi() encounters a runtime error on the judge system
    //Plus, it allows convertion up to long long
    size_t size = s.size();
    t tot = 0;
    t power = (t)pow(10, size - 1);
    for (ui i = 0; i < size; i++, power /= 10) {
        tot += (s[i] - '0') * power;
    }
    return tot;
}
//------------------------------------------------------


bool takeInput(){
    string inputPath = PATH + INPUT_TAG + FILE_NUM + APPENDIX;
    ifstream input(inputPath);
    if (input.is_open()) {
        string line;
        getline(input, line);
        maxSlices = stringToInt<int>(separateLine(line)[0]);
        getline(input, line);
        vs separate = separateLine(line);
        for(int i = 0; i < separate.size(); i++){
            int curr = stringToInt<int>(separate[i]);
            available[curr]++;
            allThePizzas.push_back(curr);
        }
    }
    else{
        cout << "Error opening file: " + inputPath << endl;
        return false;
    }
    return true;
}

void output (vi result) {
    string outputPath = PATH + OUTPUT_TAG + FILE_NUM + APPENDIX;
    ofstream output;
    output.open(outputPath);
    if(output.is_open()){
        output << result.size() << endl;
        for (int i = 0; i < result.size(); i++) {
            output << result[i] << " ";
        }
        output << endl;
        output.close();
    }
}

paii bestSwitch() {
    //Finds 2 pizzas, one taken and one available, that when switched, allow the currSlices count to go below maxSlices
    //The optimal switch will be the one with delta equal to the amount overflowed
    const int originalDelta = currSlices - maxSlices;
    //Initialize at ERROR_CODE so that if no solution was found, the driver will quit
    paii bestDelta = PAII_ERROR_CODE;
    for (auto it = taken.begin(); it != taken.end(); it++) {
        if (it->second > 0) {
            for (auto av = available.begin(); av != available.end(); av++) {
                if (av->second > 0) {
                    int delta_new = it->first - av->first;
                    if (!(delta_new <= 0 || originalDelta > delta_new)) {
                        if(bestDelta == PAII_ERROR_CODE || delta_new < (bestDelta.first - bestDelta.second)){
                            bestDelta = make_pair(it->first, av->first);
                        }
                        break;
                    }
                }
            }
        }
    }
    return bestDelta;
}
int bestOver() {
    //Finds the pizza that allows the current number of slices to overflow the limit by the smallest amount
    //In no pizza can overflow the limit, it will return the biggest available pizza
    const int delta = maxSlices - currSlices;
    //Initialize at error code
    int currentlyBest = -1;
    for (auto it = available.begin(); it != available.end(); it++) {
        //Check if it is available to take
        if(it->second > 0){
            if(it->first >= delta){
                //If value overflows and it is available, set as best
                currentlyBest = it->first;
            }
            else {
                if(currentlyBest == -1){
                    //Set self, even though it does not overflow
                    //Since the map is ordered, it will the closest one to overflow available
                    currentlyBest = it->first;
                }
                //No one else will overflow
                break;
            }
        }
    }
    return currentlyBest;
}

//Helper functions to make code more readable
void addPizza(maii &pizzas, int size) {
    pizzas[size]++;
}
void removePizza(maii &pizzas, int size) {
    pizzas[size]--;
}
bool overflowMaximumLimit(){
    int newPizza = bestOver();
    if (newPizza != -1) {
        removePizza(available, newPizza);
        addPizza(taken, newPizza);
        currSlices += newPizza;
        return true;
    }
    return false;
}
void switchPizzaHelper(paii toSwitch){
    int toLeave = toSwitch.first, toTake = toSwitch.second;
    removePizza(taken, toLeave);
    addPizza(taken, toTake);
    removePizza(available, toTake);
    addPizza(available, toLeave);
}
bool switchPizzas(){
    //Leave a taken pizza and take another one in place
    paii change = bestSwitch();
    //change.first is the pizza to not take, change.second will be the pizza to take
    if (change != PAII_ERROR_CODE) {
        currSlices -= change.first;
        currSlices += change.second;
        switchPizzaHelper(change);
        return true;
    }
    return false;
}

maii driver() {
    maii bestMap;
    int bestSlices = 0;
    currSlices = 0;
    bool executionOver = false;
    while (!executionOver) {
        if (currSlices == maxSlices) {
            //Found the best possible solution
            executionOver = true;
        } else if (currSlices < maxSlices) {
            //Attempt to overflow the maximum limit
            executionOver = !overflowMaximumLimit();
        } else { //currSlices > maxSlices
            //Switch an already taken pizza with another
            executionOver = !switchPizzas();
        }
        if(currSlices <= maxSlices && currSlices > bestSlices){
            bestSlices = currSlices;
            bestMap = taken;
        }
    }
    return bestMap;
}

vi convertFromSlicesToIndeces(maii m){
    vi output;
    for(auto it = m.begin(); it != m.end(); it++){
        if (it->second > 0) {
            int index = findInVector(allThePizzas, it->first);
            for(int i = 0; i < it->second; i++){
                output.push_back(index + i);
            }
        }
    }
    return flipVector(output);
}

int main() {
    if(takeInput()){
        maii bestMap = driver();
        output(convertFromSlicesToIndeces(bestMap));
    }
    return 0;
}
