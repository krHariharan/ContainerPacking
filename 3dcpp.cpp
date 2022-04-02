/************************

Container Packing with Multi Drop Constraint
Logistics Lab - IIT Madras

Started: Dec 2021

Guide: Prof.Narayanaswamy
Anuj
KR Hariharan
Soham Roy
Shree Vishnu

************************/

#include<bits/stdc++.h>

using namespace std;

const double TEMP_MAX = 1, TEMP_MIN = 0.1, COOLING_RATE = 0.90;

bool DEBUG = false;

random_device rd;
seed_seq sd{rd(), rd(), rd(), rd()};
mt19937 rng(rd());

// Location struct
typedef struct LocType {
	int x, y, z;
} Location;

enum orientation
{
   Height,
   Length,
   Width,
   All,
   Other
};

// Item struct
typedef struct ItemType {
	int sNo;
	int locNo;
	int l;
	int b;
    int h;
    int l1;
	int b1;
    int h1;
    bool packed;
	bool stackable;
	orientation o;
	Location pos={-1, -1, -1};   	
} Item;

// Container struct
class Container{
	int L, B, H;	// dimensions of container
	vector<vector<int>> v;	// height upto which each coordinate is filled
	vector<vector<int>> minh;	// minimum height a consignment must be to be placed in a given coordinate, to prevent being blocked by consingment in front
   	set<pair<int, int>> positions;	// (x, y) of the left rear corner of every available cuboidal space, in lexicographical order
	vector<Item> packedItems;	// details of packed items, in order of packing
public:
	// Constructor to create class objects, given properties [dimensions] of container
	Container(int x, int y, int z){
		L=x;
		B=y;
		H=z;
		
		v.resize(L, vector<int>(B, 0));
		minh.resize(L, vector<int>(B, 0));
		
		positions.insert({0,0});
	}	


	// given a consignment with it's assigned position, "pack" the consingment and update the colume space of container
	void pack(Item I){
		if(I.pos.x==-1)
			return;
		int x=I.pos.x;
		int y=I.pos.y;

		// Updating remaining volume space of container after consignment is packed
		for(int m=x; m<x+I.l1; m++)
			for(int n=y; n<y+I.b1; n++)
				v[m][n]+=I.h1;
		
		// updating minh - the minimum height another consignment needs to be, to be packed behind current consignment without being blocked when unloading
		for(int n=y; n<y+I.b1; n++){
			for(int m=x-1; m>=0; m--){
				// if(v[m][n]>=v[x][n])
				// 	break;
				minh[m][n] = max(v[x][n],minh[m][n]);
			}
		}

		//Updating remaining volume space of container if Item is not stackable
		if(!I.stackable){
			for(int m=x; m<x+I.l1; m++)
				for(int n=y; n<y+I.b1; n++)
					v[m][n]=H;
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
	Location fit(int l, int b, int h){
		int flag = 0, x, y, base;

		Location loc;
		loc.x = -1;
		loc.y = -1;
		loc.z = -1;

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
					if(v[x+m][y+n]!=base || minh[x+m][y+n]>=base+h){
						flag=0;	// flag position 0 if either consignemnt doesn't have the min height, or base is uneven at this area
						break;
					}
				}
				if(flag==0)
					break;	// position doesn't satisfy reqts - skipped
			}
			if(flag==1){	// position chosen
				loc.x = x;
				loc.y = y;
				loc.z = base;
				break;
			}				
		}

		return loc;
	}

	// Returns volume optimization of current packing of container
	double volOpt(){
		double occVol=0.0, totalVol;
		for(auto I: packedItems){
			occVol += (double)(I.l*I.b*I.h);
	}
	totalVol = (double)(L*B*H);
	return occVol/totalVol;
	}

	// returns number of items currently packed
	int itemCount(){
		return packedItems.size();
	}

	void convert(int id)
	{
		string colour[]={"Violet","Indigo","Blue","Green","Yellow","Orange","Red","MidnightBlue","Brown",",Crimson"};
		string filename ="Container_"+to_string(id)+".txt";
		ofstream file_out;
		file_out.open(filename, std::ios_base::app);
		file_out<<"{ \n";
		file_out<<"\"container_id\":"<<id<<","<<endl;
		file_out<<"\"container_length\":"<<L<<","<<endl;
		file_out<<"\"container_width\":"<<B<<","<<endl;
		file_out<<"\"container_height\":"<<H<<","<<endl;
		file_out<<"\"volume optimization\":"<<volOpt()*100<<","<<endl;
		file_out<<"\"EBFT_best_orientation\": 1,\n\"EBFT_best_relative_iteration\": 1,"<<endl;
		file_out<<"\"data\" : ["<<endl;
		for(int i=0;i<packedItems.size();i++)
		{
			file_out<<"\"id\":"<<packedItems[i].sNo<<","<<endl;
			file_out<<"\"length\":"<<packedItems[i].l1<<","<<endl;
			file_out<<"\"width\":"<<packedItems[i].b1<<","<<endl;
			file_out<<"\"height\":"<<packedItems[i].h1<<","<<endl;
			file_out<<"\"x\":"<<packedItems[i].pos.x<<","<<endl;
			file_out<<"\"y\":"<<packedItems[i].pos.y<<","<<endl;
			file_out<<"\"z\":"<<packedItems[i].pos.z<<","<<endl;
			file_out<<"\"colour\":"<<"\""<<colour[i%9]<<"\""<<","<<endl;
			file_out<<"\"wall\":"<<(i%4)+1<<","<<endl;
			file_out<<"\"row\":"<<(i/8)+1<<","<<endl;
			file_out<<"\"column\":"<<(i/4)+1<<","<<endl;
			file_out<<"\"priority\":"<<"1"<<endl;
			file_out<<"}";
			if(i==packedItems.size()-1)
			{
				continue;
			}
			file_out<<","<<endl;
		}
		file_out<<"]"<<endl<<"}"<<endl;
	}
};


// Given a partially packed container as input, fn packs remaining consignments using a purely greedy approach and returns the resultant volume optimization
double greedy(Container C, vector<Item>& Items, int starting, bool annealing=false) {
	//cout<<"3dcpp called\n";
	//cout<<"Container created";
    for(int i=starting; i>=0; i--){
    	Item I = Items[i];
		vector<int> dim{I.l, I.b, I.h};
		sort(dim.begin(), dim.end());
    	vector<Item> Iarr(6);
		// all possible orientations - ordered based on heuristic considering protrusion length and stability
   		Iarr[0] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[1], dim[2], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[1] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[2], dim[1], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[2] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[0], dim[2], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[3] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[2], dim[0], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[4] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[0], dim[1], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[5] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[1], dim[0], true, I.stackable, I.o, {-1, -1, -1}};

   		for(int j=0; j<6; j++){
       		// if orientation i+1 is allowed for given package
			if((I.o==Height && Iarr[j].h!=Iarr[j].h1) || (I.o==Length && Iarr[j].l!=Iarr[j].l1) || (I.o==Width && Iarr[j].b!=Iarr[j].b1))
					continue;
       		Iarr[j].pos = C.fit(Iarr[j].l1, Iarr[j].b1, Iarr[j].h1);
       		if(Iarr[j].pos.x!=-1){
				C.pack(Iarr[j]);
				// if(annealing)
				// 	Items[i] = Iarr[j];
				break;
       		}
   		}
   	}
	return C.volOpt();
}

// helper fn for threedcpp used in sorting different container packing options
bool greaterPair(pair<double,Container>& a, pair<double,Container>& b){
	if(a.first>b.first)
		return true;
	else if(b.first>a.first)
		return false;
	else
		return a.second.itemCount() <= b.second.itemCount();	// more larger packages more likely to have been packed in packing with less overall packages
}

// Given the consignments to be packed (in order of destination) and dimensions of container, returns container object with an optimal packing based on decision tree heurisitics
Container threedcpp(vector<Item>& Items, int L, int B, int H, int treeWidth=5) {
	vector<pair<double,Container>> options;	// contains all possible packing options of limited size decision tree in descending order of volOpt
	Container C(L,B,H);
	options.push_back({greedy(C, Items, Items.size()-1), C});	// initializing options with empty container and purely greedy packing volume optimization
	// cout<<"Greedy Optimization: "<<options[0].first<<endl;
    for(int i=Items.size()-1; i>=0; i--){
    	Item I = Items[i];
		vector<int> dim{I.l, I.b, I.h};
		sort(dim.begin(), dim.end());
    	vector<Item> Iarr(6);
		// all possible orientations - unlike the greedy fn, in decision tree fn, order is irrelevant
   		Iarr[0] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[1], dim[2], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[1] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[2], dim[1], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[2] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[0], dim[2], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[3] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[2], dim[0], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[4] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[1], dim[0], true, I.stackable, I.o, {-1, -1, -1}};
   		Iarr[5] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[0], dim[1], true, I.stackable, I.o, {-1, -1, -1}};

		// updating decision tree options
		for(int k=options.size()-1; k>=0; k--){
			// case skipping Item i considered
			C = options[k].second;
			options.push_back({greedy(C, Items, i-1), C});

			// case of packing item I considered
			for(int j=0; j<6; j++){
				// check if orientation i+1 is allowed for given consignment
				if((I.o==Height && Iarr[j].h!=Iarr[j].h1) || (I.o==Length && Iarr[j].l!=Iarr[j].l1) || (I.o==Width && Iarr[j].b!=Iarr[j].b1))
					continue;
				Iarr[j].pos = options[k].second.fit(Iarr[j].l1, Iarr[j].b1, Iarr[j].h1);
				// if orientation i+1 is allowed for given consignment, pack into container and and keep this packing as an option
				if(Iarr[j].pos.x!=-1){
					C = options[k].second;
					C.pack(Iarr[j]);
					options.push_back({greedy(C, Items, i-1), C});
				}
			}

			// old packing that did not consider Item i removed
			options.erase(options.begin()+k);
		}

		// sort options in descending order of volume optimization
		//sort(options.begin(), options.end(), greaterPair);

		// keeping 'treeWidth' most optimal packings
		if(options.size()>treeWidth){
			for(int i=0; i<treeWidth; i++){
				int greatest = i;
				for(int j=i+1; j<options.size(); j++){
					if(greaterPair(options[j], options[greatest])){
						greatest = j;
					}
				}
				swap(options[i], options[greatest]);
			}
			options.resize(treeWidth, {greedy(Container(L,B,H), Items, Items.size()-1), Container(L,B,H)});
		}
   	}
	return options[0].second;
}

void readcsv(vector<Item>& Is, vector<int>& locBoundaries, int& L, int& B, int& H, string fileName) {
    ifstream file(fileName);
    string line;

    getline(file, line);
    stringstream containerinfo(line);
    char hashtag; // consume the hashtag
    containerinfo >> hashtag >> L >> B >> H;
    getline(file, line);

	// uniform_int_distribution<> new_loc(0, 25);
	int sNo=-1, locNo=0;
	locBoundaries.insert(locBoundaries.begin(), 0);
    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        Item I;
		I.sNo = ++sNo;
		I.locNo = locNo;
		// if(!new_loc(rng)){	
		// 	locNo++;
		// 	locBoundaries.push_back(sNo);
		// }
        I.packed = false;
		I.pos = {-1, -1, -1};
        
        for (int i = 0; getline(ss, item, ','); ++i) {
            if (i == 10) I.l = I.l1 = stoi(item.substr(1, item.size()-2));
            else if (i == 11) I.b = I.b1 = stoi(item.substr(1, item.size()-2));
            else if (i == 12) I.h = I.h1 = stoi(item.substr(1, item.size()-2));
			else if(i==15) {
				// cout<<item.substr(1, item.size()-2)<<' ';
				if(stoi(item.substr(1, item.size()-2)) == 1)
					I.stackable = true;
				else
					I.stackable = false;
				//cout<<I.stackable<<endl;
			}
			else if(i==19) {
				// cout<<item.substr(1, item.size()-2)<<' ';
				if(item.substr(1, item.size()-2) == "ROTATE_ANY")
					I.o = All;
				else if(item.substr(1, item.size()-2) == "ROTATE_HEIGHT_ONLY")
					I.o = Height;
				else if(item.substr(1, item.size()-2) == "ROTATE_WIDTH_ONLY")
					I.o = Width;
				else if(item.substr(1, item.size()-2) == "ROTATE_LENGTH_ONLY")
					I.o = Length;
				else
					I.o = Other;
				// cout<<I.o<<endl;
			}
			else if(i==20){
				I.locNo = stoi(item.substr(1, item.size()-2));
				if(I.locNo!=locNo){
					locBoundaries.insert(locBoundaries.begin(), sNo);
					locNo=I.locNo;
				}
			}
        }
		I.pos={-1, -1, -1};
        Is.push_back(I);
    }
	// locBoundaries.push_back(++sNo);
	cout<<locNo<<' '<<locBoundaries.size()<<endl;
	for(auto b: locBoundaries){
		cout<<b<<' ';
	}
	cout<<endl;
}

bool largerItem(Item& a, Item& b){
	if(a.l*a.b*a.h==b.l*b.b*b.h){
		priority_queue<int> aDim, bDim;
		aDim.push(a.l);
		aDim.push(a.b);
		aDim.push(a.h);
		bDim.push(b.l);
		bDim.push(b.b);
		bDim.push(b.h);
		if(aDim.top()!=bDim.top())
			return aDim.top()>bDim.top();
		aDim.pop();
		bDim.pop();
		if(aDim.top()!=bDim.top())
			return aDim.top()>bDim.top();
		return !(a.stackable);	
	}
	return a.l*a.b*a.h > b.l*b.b*b.h;
}

void sortItems(vector<Item>& items, int start, int end){
	if(start>=end)
		return;
	Item pivot = items[start];
	int pivotPoint=end-1;
	for(int i=end-1; i>start; i--){
		if(largerItem(items[i], pivot)){
			swap(items[pivotPoint], items[i]);
			pivotPoint--;
		}
	}
	swap(items[pivotPoint], items[start]);
	sortItems(items, start, pivotPoint-1);
	sortItems(items, pivotPoint+1, end);
}

void optimalItemList(vector<Item>& Is, vector<int>& locBoundaries, int L, int B, int H){
	int top=Is.size();
	for(auto b: locBoundaries){
		sortItems(Is, b, top);
		top = b;
	}
	top = Is.size();
	for(auto b: locBoundaries){
		if(b>=top-1)
			continue;
		uniform_int_distribution<> int_dist(b, top-1);
		uniform_real_distribution<> float_dist(0, 1);
		double efficiency = greedy(Container(L, B, H), Is, Is.size()-1, true);
		 for (double temp = TEMP_MAX; temp > TEMP_MIN; temp *= COOLING_RATE) {
			// Number of swaps evaluated at each temperature, hardcoded for now
			int swaps_per_temp = 3;
			for (int i = 0; i < swaps_per_temp; i++) {
				int a = int_dist(rng), b = int_dist(rng);

				while (b == a) {
					b = int_dist(rng);
				}

				Item IsA = Is[a], IsB = Is[b];
				swap(Is[a], Is[b]);

				
				double new_eff = greedy(Container(L, B, H), Is, Is.size()-1, true);

				// change in energy between new and old state, i.e. cost function
				// = (change in efficiency in %) + (euclidean distance b/w the items / 100)
				double adist, bdist;
				if(IsA.pos.x==-1 || Is[a].pos.x==-1){
					if(IsA.pos.x==-1 && Is[a].pos.x==-1)
						adist = 0;
					else
						adist = sqrt(pow(L, 2) + pow(B, 2) + pow(H, 2));
				}
				else{
					adist = sqrt(pow(IsA.pos.x - Is[a].pos.x, 2) + pow(IsA.pos.y - Is[a].pos.y, 2) + pow(IsA.pos.z - Is[a].pos.z, 2));;
				}
				
				if(IsB.pos.x==-1 || Is[b].pos.x==-1){
					if(IsB.pos.x==-1 && Is[b].pos.x==-1)
						bdist=0;
					else
						bdist = sqrt(pow(L, 2) + pow(B, 2) + pow(H, 2));
				}
				else
					bdist = sqrt(pow(IsB.pos.x - Is[b].pos.x, 2) + pow(IsB.pos.y - Is[b].pos.y, 2) + pow(IsB.pos.z - Is[b].pos.z, 2));

				double del_E = (efficiency - new_eff) * 100;// - (adist + bdist) / sqrt(pow(L, 2) + pow(B, 2) + pow(H, 2));

				// probability = exp(-ΔE / T)
				float probability = exp(-del_E / temp);
				float rand_val = float_dist(rng);

				if (DEBUG) {
					cout	<< " delEfficiency : " << (efficiency - new_eff) * 100
							<< "  dist : " << (adist + bdist) / 100
							<< "  del_E : " << del_E
							<< "  probability : " << probability << endl;
				}

				if (rand_val > probability) { // probability too low, revert the change
					swap(Is[b], Is[a]);
				} else {
					efficiency = new_eff;
				}
			}
		}
		top=b;
	}
}


double outputRep(Container& C, int i){
	double volOpt = C.volOpt();
	cout<<i<<": Volume Optimization : "<<volOpt<<' '<<endl;
	return volOpt;
}

double packer(string fileName, int i) {
    // receive input data and pass on to 3dcpp fn
    vector<Item> Is;
	vector<int> locBoundaries;
    int L, B, H;
    readcsv(Is, locBoundaries, L, B, H, fileName);

	auto start = chrono::steady_clock::now();

	optimalItemList(Is, locBoundaries, L, B, H);
	//cout<<"Input read\n";
	Container C = threedcpp(Is, L, B, H);

	C.convert(i);

	chrono::duration<double> diff = chrono::steady_clock::now() - start;
	cout << "Time : " << diff.count() << "s\n";

	return outputRep(C, i);
}

int main(int argc, char ** argv){
	double volOpt=0;
	string baseFile(argv[1]);
	baseFile += "_";
	string fileType = ".csv";
	for(int i=1; i<=100; i++){
		volOpt += packer(baseFile+string(to_string(i))+fileType, i);
		cout<<"Average volume optimization: "<<volOpt/i<<endl;
	}
}