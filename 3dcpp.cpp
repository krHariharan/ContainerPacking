#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Location {
    int x, y, z;
};

struct Item {
    int len, wid, hei;
    bool packed;
    int orientation;
    Location *pos;
};

struct Volume {
    int x, y, z;
    int len, wid, hei;
};

struct Container {
    int len, wid, hei;
};

int L, H, W;

bool ord(const Volume &x, const Volume &y) {
    return (x.x < y.x ||
            x.x == y.x && x.y > y.y ||
            x.x == y.x && x.y == y.y && x.z < y.z);
}

bool checkValidVolume(Volume &s) {
    return (s.len > 0 && s.wid > 0 && s.hei > 0);
}

bool checkfit(Item I, Volume vol) {
    return I.len <= vol.x && I.wid <= vol.y && I.hei <= vol.z;
}

void remOverlap(vector<Volume> &SubV, Volume vol) {
    vector<Volume> nonoverlap(8);
    vector<Volume> nonOLSubV;

    for (int i = 0; i < SubV.size(); i++) {
        Volume selSub = SubV[i];
        //everything to the left
        nonoverlap[3] = {selSub.x, vol.y + vol.wid, selSub.z, selSub.len, selSub.y + selSub.wid - (vol.y + vol.wid), selSub.hei};
        //everything to the right
        nonoverlap[4] = {selSub.x, selSub.y, selSub.z, selSub.len, vol.y - selSub.y, selSub.hei};
        //top
        nonoverlap[5] = {selSub.x, vol.y, vol.z + vol.hei, selSub.len, vol.wid, selSub.z + selSub.hei - (vol.z + vol.hei)};

        bool f = false;
        for (Volume s : nonoverlap) {
            if (checkValidVolume(s)) {
                nonOLSubV.push_back(s);
                f = 1;
            }
        }

        if (!f) {
            nonOLSubV.push_back(selSub);
        }
    }

    sort(nonOLSubV.begin(), nonOLSubV.end(), ord);

    SubV = nonOLSubV;
}

bool putInSubV(Item I, int subVindex, vector<Volume> &SVV) {
    Volume subv = SVV[subVindex];

    if (checkfit(I, subv)) {
        //new subvolumes created
        Volume subv1 = {subv.x, subv.y, subv.z + I.hei, I.len, I.wid, subv.hei - I.hei};
        Volume subv2 = {subv.x, subv.y + I.wid, subv.z, I.len, subv.wid - I.wid, I.hei};
        Volume subv3 = {subv.x + I.hei, subv.y, subv.z, subv.len - I.len, I.wid, subv.hei - I.hei};

        SVV.erase(SVV.begin() + subVindex);
        //insert them into SubV
        SVV.push_back(subv1);
        SVV.push_back(subv2);
        SVV.push_back(subv3);

        sort(SVV.begin(), SVV.end(), ord);

        remOverlap(SVV, subv);

        return true;
    }

    return false;
}

int fitInSubV(Item I, int subVindex, vector<Volume> &SVV) {
    vector<Item> Iarr(6);
    Iarr[0] = {I.len, I.wid, I.hei, false, 1};
    Iarr[1] = {I.len, I.hei, I.wid, false, 2};
    Iarr[2] = {I.wid, I.len, I.hei, false, 3};
    Iarr[3] = {I.wid, I.hei, I.len, false, 4};
    Iarr[4] = {I.hei, I.len, I.wid, false, 5};
    Iarr[5] = {I.hei, I.wid, I.len, false, 6};
    random_shuffle(Iarr.begin(), Iarr.end());

    for (int i = 0; i < 6; i++) {
        // if orientation i+1 is allowed for given package then do:
        if (putInSubV(Iarr[i], subVindex, SVV)) {
            return Iarr[i].orientation;
        }
    }

    return 0;
}

bool pack(Item &I, vector<Volume> &SVV) {
    for (int i = 0; i < SVV.size(); i++) {
        int orientation = fitInSubV(I, i, SVV);
        if (orientation) {
            Location *pos = new Location();
            pos->x = SVV[i].x;
            pos->y = SVV[i].y;
            pos->z = SVV[i].z;
            I.pos = pos;
            return orientation;
        }
    }

    return false;
}

void threedcpp(vector<Item> &I, Container C) {
    vector<Volume> SubVolVec;
    Volume container = {0, 0, 0, C.len, C.wid, C.hei};

    for (int i = I.size() - 1; i >= 0; i++) {
        I[i].orientation = pack(I[i], SubVolVec);
        I[i].packed = (bool)I[i].orientation;
    }
}

void readcsv(vector<Item> &Is, Container C) {
    ifstream file("thpack1/1.csv");
    string line;

    getline(file, line);
    stringstream containerinfo(line);
    char hashtag; // consume the hashtag
    containerinfo >> hashtag >> C.len >> C.wid >> C.hei;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string item;

        Item I;
        I.orientation = 0;
        I.packed = false;

        for (int i = 0; getline(ss, item, ','); ++i) {
            if (i == 10) I.len = stoi(item.substr(1, item.size() - 2));
            else if (i == 11)
                I.wid = stoi(item.substr(1, item.size() - 2));
            else if (i == 12)
                I.hei = stoi(item.substr(1, item.size() - 2));
        }

        Is.push_back(I);
    }
}

void outputRep(vector<Item> &Is, Container C) {
    double occVol = 0.0, totalVol;
    for (int i = Is.size() - 1; i >= 0; i--) {
        if (Is[i].packed) {
            cout << "\nItem " << i + 1;
            cout << "\tdimensions : " << Is[i].len << 'x' << Is[i].wid << 'x' << Is[i].hei;
            cout << "\norientation : " << Is[i].orientation;
            cout << "\tlocation : " << Is[i].pos->x << 'x' << Is[i].pos->y << 'x' << Is[i].pos->z;
            occVol += (double)(Is[i].len * Is[i].wid * Is[i].hei);
        } else {
            cout << "\nItem " << i + 1 << "\tNot Packed";
        }
    }
    totalVol = (double)(C.len * C.wid * C.hei);
    cout << "\nVolume Optimization : " << occVol / totalVol << endl;
}

int main() {
    // receive input data and pass on to 3dcpp fn
    vector<Item> I;
    Container C;
    readcsv(I, C);
    outputRep(I, C);
}
