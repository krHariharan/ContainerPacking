#include<bits/stdc++.h>

using namespace std;

typedef struct LocType {
	int x, y, z;
} Location;

typedef struct ItemType {
	int l;
	int b;
    int h;
    int l1;
	int b1;
    int h1;
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
		if(loc->x<0)
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
	//cout<<"3dcpp called\n";
	Container C(L, B, H);
	//cout<<"Container created";
    for(int i=Items.size()-1; i>=0; i--){
    	Item I = Items[i];
		vector<int> dim{I.l, I.b, I.h};
		sort(dim.begin(), dim.end());
    	vector<Item> Iarr(6);
   		Iarr[0] = {I.l, I.b, I.h, dim[0], dim[2], dim[1], true, NULL};
   		Iarr[1] = {I.l, I.b, I.h, dim[1], dim[2], dim[0], true, NULL};
   		Iarr[2] = {I.l, I.b, I.h, dim[0], dim[1], dim[2], true, NULL};
   		Iarr[3] = {I.l, I.b, I.h, dim[1], dim[0], dim[2], true, NULL};
   		Iarr[4] = {I.l, I.b, I.h, dim[2], dim[1], dim[0], true, NULL};
   		Iarr[5] = {I.l, I.b, I.h, dim[2], dim[0], dim[1], true, NULL};
   		// random_shuffle(Iarr.begin(), Iarr.end());
		//cout<<"Fitting consignment "<<i<<'\n';
   		for(int j=0; j<6; j++){
       		// if orientation i+1 is allowed for given package then do:
       		Iarr[j].pos = C.fit(Iarr[j].l1, Iarr[j].b1, Iarr[j].h1);
       		if(Iarr[j].pos!=NULL){
       			Items[i] = Iarr[j];
				break;
       		}
   		}
   	}
}

void readcsv(vector<Item>& Is, int& L, int& B, int& H, string fileName) {
    ifstream file(fileName);
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
        I.packed = false;
		I.pos = NULL;
        
        for (int i = 0; getline(ss, item, ','); ++i) {
            if (i == 10) I.l = I.l1 = stoi(item.substr(1, item.size()-2));
            else if (i == 11) I.b = I.b1 = stoi(item.substr(1, item.size()-2));
            else if (i == 12) I.h = I.h1 = stoi(item.substr(1, item.size()-2));
        }
        Is.push_back(I);
    }
}

double outputRep(vector<Item>& Is, int L, int B, int H){
	double occVol=0.0, totalVol;
	for(int i=Is.size()-1; i>=0; i--){
		if(Is[i].packed){
			/*
			cout<<"\nItem "<<i+1;
			cout<<"\tdimensions : "<<Is[i].l<<'x'<<Is[i].b<<'x'<<Is[i].h;
			cout<<"\norientation : "<<Is[i].l1<<'x'<<Is[i].b1<<'x'<<Is[i].h1;
			cout<<"\tlocation : "<<Is[i].pos->x<<'x'<<Is[i].pos->y<<'x'<<Is[i].pos->z;
			*/
			occVol += (double)(Is[i].l*Is[i].b*Is[i].h);
		}/*
		else{
			cout<<"\nItem "<<i+1<<"\tNot Packed";
		}*/
	}
	totalVol = (double)(L*B*H);
	cout<<"\nVolume Optimization : "<<occVol/totalVol;
	return occVol/totalVol;
}

double packer(string fileName) {
    // receive input data and pass on to 3dcpp fn
    vector<Item> Is;
    int L, B, H;
    readcsv(Is, L, B, H, fileName);
	//cout<<"Input read\n";
	threedcpp(Is, L, B, H);
	return outputRep(Is, L, B, H);
}

int main(){
	double volOpt=0;
	cout << "Please enter the name of the input .txt file, without the extension:\n"
                 "Note: Subfolder of the same name must exist in the current directory."
        	<< endl;
	string baseFile;
	cin>>baseFile;
	baseFile += "_";
	string fileType = ".csv";
	for(int i=1; i<100; i++){
		volOpt += packer(baseFile+string(to_string(i))+fileType);
	}
	cout<<"\nAverage volume optimization: "<<volOpt/99;
}