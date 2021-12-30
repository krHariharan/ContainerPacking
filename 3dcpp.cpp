#include<bits/stdc++.h>

using namespace std;

typedef struct ItemType {
    int len;
    int wid;
    int hei;
    bool packed;
    int orientation;
} Item;

typedef struct VolType {
    int x;
    int y;
    int z;
    int len;
    int wid;
    int hei;
} Volume;

typedef struct ConType {
    int len;
    int wid;
    int hei;
} Container;

int L;
int H;
int W;

bool ord(const Volume &x, const Volume &y) {
    if(x.x<y.x || x.x==y.x&&x.y>y.y || x.x==y.x&&x.y==y.y&&x.z<y.z) return true;
    return false;
}

void push(vector<Volume> & SubV, Volume vol){
    for()
}

bool checkValidVolume(Volume& s) {
    if(s.len>0 && s.wid>0 && s.hei > 0) return true;
    return false;
}

bool checkfit(Item I, Volume vol){
    return I.len<=vol.x && I.wid<=vol.y && I.hei<=vol.z;
}

void remOverlap(vector<Volume> & SubV, Volume vol) {
    vector<Volume> nonoverlap(8);
    vector<Volume> nonOLSubV;

    for(int i=0; i<SubV.size(); i++) {
        Volume selSub = SubV[i];
        //everything to the left
        nonoverlap[3] = {selSub.x, vol.y+vol.wid, selSub.z, selSub.len, selSub.y+selSub.wid-(vol.y+vol.wid), selSub.hei};
        //everything to the right
        nonoverlap[4] = {selSub.x, selSub.y, selSub.z, selSub.len,  vol.y-selSub.y, selSub.hei};
        //top
        nonoverlap[5] = {selSub.x, vol.y, vol.z+vol.hei, selSub.len,  vol.wid, selSub.z+selSub.hei-(vol.z+vol.hei)};

        int f = 0;
        for(Volume s: nonoverlap) {
            if(checkValidVolume(s)) {
                nonOLSubV.push_back(s);
                f=1;
            }
        }
        if(!f) {
            nonOLSubV.push_back(selSub);
        }
        
    }

    sort(nonOLSubV.begin(), nonOLSubV.end(), ord);

    SubV = nonOLSubV;
}



void threedcpp(vector<Item>& I, Container C) {
    vector<Volume>  SubVolVec;
    Items bi = I[i];
    Volume container = {0, 0, 0, C.len, C.wid, C.hei};

    for(int i=I.size()-1; i>=0; i++){
	    I[i].orientation = pack(I[i], SubVolVec);
        I[i].packed = (bool)I[i].orientation;
    }
}

bool pack(Item I, vector<Volume>& SVV){
	for(int i=0; i<SVV.size(); i++){
        int orientation = fitInSubV(Item I, i, SVV);
		if(orientation){
			return orientation;
		}
	}
	return false;
}

int fitInSubV(Item I, int subVindex, vector<Volume>& SVV){
    vector<Item> Iarr(6);
    Iarr[0] = {I.len, I.wid, I.hei, 1, false};
    Iarr[1] = {I.len, I.hei, I.wid, 2, false};
    Iarr[2] = {I.wid, I.len, I.hei, 3, false};
    Iarr[3] = {I.wid, I.hei, I.len, 4, false};
    Iarr[4] = {I.hei, I.len, I.wid, 5, false};
    Iarr[5] = {I.hei, I.wid, I.len, 6, false};
    random_shuffle(Iarr.begin(), Iarr.end());
    for(int i=0; i<6; i++){
        // if orientation i+1 is allowed for given package then do:
        if(putInSubV(Iarr[i], subVindex, SVV))
            return Iarr[i].orientation;
    }
    return 0;
}

bool putInSubV(Item I, int subVindex, vector<Volume>& SVV){
    Volume subv = SVV[subVindex];

    if(checkfit(I, subv)){
        //new subvolumes created
        Volume subv1 = {subv.x, subv.y, subv.z+bi.hei, bi.len, bi.wid, subv.hei-bi.hei};
        Volume subv2 = {subv.x, subv.y+bi.wid, subv.z, bi.len, subv.wid-bi.wid, bi.hei};
        Volume subv3 = {subv.x+bi.hei, subv.y, subv.z, subv.len-bi.len, bi.wid, subv.hei-bi.hei};

        SVV.erase(subVindex);
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

void readcsv(vector<Item>& Is, Container C) {
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
            else if (i == 11) I.wid = stoi(item.substr(1, item.size() - 2));
            else if (i == 12) I.hei = stoi(item.substr(1, item.size() - 2));
        }

        Is.push_back(I);
    }
}

int main() {
    // receive input data and pass on to 3dcpp fn
    vector<Item> I;
    Container C;
    readcsv(I, C);
}
