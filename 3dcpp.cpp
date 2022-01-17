#include<bits/stdc++.h>

using namespace std;

typedef struct LocType {
	int x, y, z;
} Location;

typedef struct ItemType {
	int l;
	int b;
    int h;
    int orientation;
    bool packed;
	Location* pos;   	
} Item;

class Container{
	int L, B, H;
	int **v;
	int **minh;
	set<pair<int, int>> positions;
public:
	Container(int x, int y, int z){
		L=x;
		B=y;
		H=z;
		v = new int*[L];
		minh = new int*[L];
		for(int i=0; i<L; i++){
			v[i] = new int[B];
			minh[i] = new int[B];
			for(int j=0; j<B; j++){
				v[i][j] = 0;
				minh[i][j] = 0;
			}
		}
		positions.insert({0,0});
	}	
	~Container(){
		/*
		for(int i=0; i<L; i++){
			delete[] v[i];
			delete[] minh[i];	
		}
		delete[] v;
		delete[] minh;
		*/
	}
	Location* fit(int l, int b, int h){
		int flag = 0, x, y, base;
		Location * loc = new Location;
		loc->x = -1;
		loc->y = -1;
		loc->z = -1;
		for(auto p: positions){
			x=p.first;
			y=p.second;
			flag = 1;
			base = v[x][y];
			if(base+h>H)
				continue;
			if(x+l>L || y+b>B)
				continue;
			for(int m=0; m<l; m++){				
				for(int n=0; n<b; n++){
					if(v[x+m][y+n]!=base || minh[x+m][y+n]>=h){
						flag=0;
						break;
					}
				}
				if(flag==0)
					break;
			}
			if(flag==1){
				loc->x = x;
				loc->y = y;
				loc->z = base;
				break;
			}				
		}
		if(flag==0 || loc->x<0)
			return NULL;
		
		x=loc->x;
		y=loc->y;
		for(int m=x; m<x+l; m++)
			for(int n=y; n<y+b; n++)
				v[m][n]+=h;
		
		for(int n=y; n<y+b; n++){
			for(int m=x-1; m>=0; m++){
				if(v[m][n]>=v[x][n])
					break;
				minh[m][n] = v[x][n]-v[m][n];
			}
		}		

		if(x+l<L)
			positions.insert({x+l, y});
		if(y+b<B)
			positions.insert({x, y+b});

		return loc;
	}
};


void threedcpp(vector<Item>& Items, int L, int B, int H) {
	cout<<"3dcpp called\n";
	Container C(L, B, H);
	cout<<"Container created";
    for(int i=Items.size()-1; i>=0; i--){
    	Item I = Items[i];
    	vector<Item> Iarr(6);
   		Iarr[0] = {I.l, I.b, I.h, 1, true, NULL};
   		Iarr[1] = {I.l, I.h, I.b, 2, true, NULL};
   		Iarr[2] = {I.b, I.l, I.h, 3, true, NULL};
   		Iarr[3] = {I.b, I.h, I.l, 4, true, NULL};
   		Iarr[4] = {I.h, I.l, I.b, 5, true, NULL};
   		Iarr[5] = {I.h, I.b, I.l, 6, true, NULL};
   		random_shuffle(Iarr.begin(), Iarr.end());
		cout<<"Fitting consignment "<<i<<'\n';
   		for(int j=0; j<6; j++){
       		// if orientation i+1 is allowed for given package then do:
       		Iarr[j].pos = C.fit(Iarr[j].l, Iarr[j].b, Iarr[j].h);
       		if(Iarr[j].pos!=NULL){
       			Items[i] = Iarr[j];
				break;
       		}
   		}
   	}
}

void readcsv(vector<Item>& Is, int& L, int& B, int& H) {
	cout << "Please enter the name of the input file:\n"
                 "Note: Subfolder of the same name must exist in the current directory."
        	<< endl;
	string baseFile;
	cin>>baseFile;
    ifstream file("baseFile");
    string line;

    getline(file, line);
    stringstream containerinfo(line);
    char hashtag; // consume the hashtag
    containerinfo >> hashtag >> L >> B >> H;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string item;

        Item I;
        I.orientation = 0;
        I.packed = false;
		I.pos = NULL;
        
        for (int i = 0; getline(ss, item, ','); ++i) {
            if (i == 10) I.l = stoi(item);
            else if (i == 11) I.b = stoi(item);
            else if (i == 12) I.h = stoi(item);
        }
        Is.push_back(I);
    }
}

void outputRep(vector<Item>& Is, int L, int B, int H){
	double occVol=0.0, totalVol;
	for(int i=Is.size()-1; i>=0; i--){
		if(Is[i].packed){
			cout<<"\nItem "<<i+1;
			cout<<"\tdimensions : "<<Is[i].l<<'x'<<Is[i].b<<'x'<<Is[i].h;
			cout<<"\norientation : "<<Is[i].orientation;
			cout<<"\tlocation : "<<Is[i].pos->x<<'x'<<Is[i].pos->y<<'x'<<Is[i].pos->z;
			occVol += (double)(Is[i].l*Is[i].b*Is[i].h);
		}
		else{
			cout<<"\nItem "<<i+1<<"\tNot Packed";
		}
	}
	totalVol = (double)(L*B*H);
	cout<<"\nVolume Optimization : "<<occVol/totalVol;
}

int main() {
    // receive input data and pass on to 3dcpp fn
    vector<Item> Is;
    int L, B, H;
    readcsv(Is, L, B, H);
	cout<<"Input read\n";
	threedcpp(Is, L, B, H);
	outputRep(Is, L, B, H);
}
