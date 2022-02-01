/************************

Container Packing with Multi Drop Constraint
Logistics Lab - IIT Madras

Guide: Prof.Narayanaswamy
Anuj
KR Hariharan
Soham Roy
Shree Vishnu

************************/

#include<bits/stdc++.h>

using namespace std;

// Location struct
typedef struct LocType {
	int x, y, z;
} Location;

// Item struct
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

// Container struct
class Container{
	int L, B, H;	// dimensions of container
	int **v;	// height upto which each coordinate is filled
	int **minh;	// minimum height a consignment must be to be placed in a given coordinate, to prevent being blocked by consingment in front
   	set<pair<int, int>> positions;	// (x, y) of the left rear corner of every available cuboidal space, in lexicographical order
	vector<Item> packedItems;	// details of packed items, in order of packing
public:
	// Constructor to create class objects, given properties [dimensions] of container
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

	// Destructor - will not be needed once moved to 2D Vectors for v and minh
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

	// given a consignment with it's assigned position, "pack" the consingment and update the colume space of container
	void pack(Item I){
		if(I.pos==NULL)
			return;
		int x=I.pos->x;
		int y=I.pos->y;

		// Updating remaining volume space of container after consignment is packed
		for(int m=x; m<x+I.l1; m++)
			for(int n=y; n<y+I.b1; n++)
				v[m][n]+=I.h1;
		
		// updating minh - the minimum height another consignment needs to be, to be packed behind current consignment without being blocked when unloading
		for(int n=y; n<y+I.b1; n++){
			for(int m=x-1; m>=0; m--){
				if(v[m][n]>=v[x][n])
					break;
				minh[m][n] = v[x][n]-v[m][n];
			}
		}

		// packing a consignment breaks up available volume space into smaller sub-cuboids. Coordinates of the same inserted into positions
        if(x+I.l1<L)
			positions.insert({x+I.l1, y});
		if(y+I.b1<B)
			positions.insert({x, y+I.b1});

		// consignment "packed"
		packedItems.push_back(I);
	}

	// Given the dimensions of the consignment, in order of orientation, return the (x,y) coordinates at which the the consignment can be packed
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
				continue;	// height of consignment, if packed at (x,y), exceeds the height of the container
			if(x+l>L || y+b>B)
				continue;
			for(int m=0; m<l; m++){				
				for(int n=0; n<b; n++){
					if(v[x+m][y+n]!=base || minh[x+m][y+n]>=h){
						flag=0;	// flag position 0 if either consignemnt doesn't have the min height, or base is uneven at this area
						break;
					}
				}
				if(flag==0)
					break;	// position doesn't satisfy reqts - skipped
			}
			if(flag==1){	// position chosen
				loc->x = x;
				loc->y = y;
				loc->z = base;
				break;
			}				
		}
		if(loc->x<0)
			return NULL;	// no suitable location found for consignment in given orientation

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

   		for(int j=0; j<6; j++){
       		// if orientation i+1 is allowed for given package then do:
       		Iarr[j].pos = C.fit(Iarr[j].l1, Iarr[j].b1, Iarr[j].h1);
       		if(Iarr[j].pos!=NULL){
       			Items[i] = Iarr[j];
				   C.pack(Items[i]);
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