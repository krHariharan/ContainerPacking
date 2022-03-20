#include <bits/stdc++.h>
using namespace std;
// log(0.1 / 1) / log(0.9) is approximately 21 sweeps
const double TEMP_MAX = 1, TEMP_MIN = 0.1, COOLING_RATE = 0.95;

bool DEBUG = false;

random_device rd;
seed_seq sd{rd(), rd(), rd(), rd()};
mt19937 rng(rd());

class Location {
public:
    int x = -1, y = -1, z = -1;

    double operator-(const Location &other) const {
        if(x==-1 || other.x!=-1)
            return -1;
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2) + pow(z - other.z, 2));
    }
};

ostream &operator<<(ostream &out, const Location &l) {
    out << "(" << l.x << ", " << l.y << ", " << l.z << ")";
    return out;
}

class Item {
public:
	int sNo, locNo;
    int l, b, h;
    int o[3];
    bool packed;
    bool stackable;
    Location pos;

    bool is_below(const Item &I) const {
        return pos.z + o[2] <= I.pos.z &&                        // somewhere below, &
               (I.pos.x <= pos.x && pos.x < I.pos.x + I.o[0] || // stacked in
                I.pos.x < pos.x + o[0] && pos.x + o[0] <= I.pos.x + I.o[0]) &&
               (I.pos.y <= pos.y && pos.y < I.pos.y + I.o[1] || // the same column
                I.pos.y < pos.y + o[1] && pos.y + o[1] <= I.pos.y + I.o[1]);
    }

    bool is_behind(const Item &I) const {
        return pos.x + o[0] < I.pos.x &&                        // somewhere behind, &
               (pos.y < I.pos.y && I.pos.y < pos.y + o[1] ||    // overlapping
                I.pos.y < pos.y && pos.y < I.pos.y + I.o[1]) && // in y direction
               pos.z + o[2] < I.pos.z + I.o[2];                        // below
    }

    bool is_blocked_by(const Item &I) const {
        return is_below(I) || is_behind(I);
    }
};

class Container { 
    vector<vector<int>> v;         // current height at (x, y)
    vector<vector<int>> minh;      // minimum allowed item height at (x, y)
    set<pair<int, int>> positions; // current available positions
public:
    int L, B, H;                             // dimensions of the container
    vector<Item> packed;
    vector<pair<double, int>> unloadingCost;
    Container(int x, int y, int z) : L(x), B(y), H(z),
                                     v(x, vector<int>(y, 0)),
                                     minh(x, vector<int>(y, 0)) {
        positions.insert({0, 0});
    }

    Location fit(int l, int b, int h, bool stackable=true) {
        bool pos_is_suitable = false;
        int x, y, base;
        Location loc;

        for (auto &pos : positions) {
            x = pos.first;
            y = pos.second;
            pos_is_suitable = true;
            base = v[x][y];

            // ensure object fits inside container if placed at pos
            if (base + h > H || x + l > L || y + b > B) continue;

            // ensure base is uniform from (x, y) to (x + l, y + b)
            for (int m = 0; m < l; m++) {
                for (int n = 0; n < b; n++) {
                    if (v[x + m][y + n] != base || minh[x + m][y + n] >= base + h) {
                        pos_is_suitable = false;
                        break;
                    }
                }
                if (!pos_is_suitable) break;
            }

            if (pos_is_suitable) {
                loc = {x, y, base};
                break;
            }
        }

        if (loc.x < 0) return loc; // no suitable position found

        // update current height from (x, y) to (x + l, y + b)
        for (int m = loc.x; m < loc.x + l; m++)
            for (int n = loc.y; n < loc.y + b; n++)
                v[m][n] += h;

        // minh ensures that a future item does not hide behind an already placed item
        for (int m = loc.x - 1; m >= 0; m--) {
            for (int n = loc.y; n < loc.y + b; n++) {
                // if (v[m][n] >= v[loc.x][n]) break;
                minh[m][n] = max(v[loc.x][n], minh[m][n]);
            }
        }

        if(!stackable){
            for (int m = loc.x; m < loc.x + l; m++)
                for (int n = loc.y; n < loc.y + b; n++)
                    v[m][n] = H;
        }

        // additional possible positions
        if (loc.x + l < L) positions.insert({loc.x + l, loc.y});
        if (loc.y + b < B) positions.insert({loc.x, loc.y + b});

        return loc;
    }

    pair<double, int> cost(int i, int debug=false){
        int count=0;
        double currCost = 0.0;
        if(unloadingCost[i].second!=-1)
            return unloadingCost[i];
        for(int j=packed.size() - 1; j>=0; j--){
            if(i!=j && packed[i].is_blocked_by(packed[j]) && packed[i].locNo<packed[j].locNo){
                if(packed[j].stackable)
                    currCost+=2*(cost(j, debug).first + (double)(packed[j].l*packed[j].b*packed[j].h)); 
                else
                    currCost+=0.5*(cost(j, debug).first + (double)(packed[j].l*packed[j].b*packed[j].h));
                count+=cost(j).second+1;
                if(debug){
                    if(packed[i].is_behind(packed[j]))
                        cout<<"behind ";
                    if(packed[i].is_below(packed[j]))
                        cout<<"below ";
                    printf("%d: (%d, %d, %d)->(%d, %d, %d) blocked by %d: (%d, %d, %d)->(%d, %d, %d)\n", packed[i].locNo, packed[i].pos.x, packed[i].pos.y, packed[i].pos.z, packed[i].pos.x+ packed[i].o[0], packed[i].pos.y+ packed[i].o[1], packed[i].pos.z+ packed[i].o[2], packed[j].locNo, packed[j].pos.x, packed[j].pos.y, packed[j].pos.z, packed[j].pos.x+ packed[j].o[0], packed[j].pos.y+ packed[j].o[1], packed[j].pos.z+ packed[j].o[2]);
                }
            }
        }
        unloadingCost[i] = {currCost, count};
        return {currCost, count};
    }
};

Container threed_cpp(vector<Item> &items, int L, int B, int H, bool DEBUG=false) {
    Container C(L, B, H);
    for (int i = items.size() - 1; i >= 0; i--) {
        Item I = items[i];
        
        vector<int> dim{I.l, I.b, I.h};
        sort(dim.begin(), dim.end());
        vector<Item> item_rot(6); // rotations of the item
        item_rot[0] = {I.sNo, I.locNo, I.l, I.b, I.h, {dim[0], dim[2], dim[1]}, true, I.stackable};
        item_rot[1] = {I.sNo, I.locNo, I.l, I.b, I.h, {dim[1], dim[2], dim[0]}, true, I.stackable};
        item_rot[2] = {I.sNo, I.locNo, I.l, I.b, I.h, {dim[0], dim[1], dim[2]}, true, I.stackable};
        item_rot[3] = {I.sNo, I.locNo, I.l, I.b, I.h, {dim[1], dim[0], dim[2]}, true, I.stackable};
        item_rot[4] = {I.sNo, I.locNo, I.l, I.b, I.h, {dim[2], dim[1], dim[0]}, true, I.stackable};
        item_rot[5] = {I.sNo, I.locNo, I.l, I.b, I.h, {dim[2], dim[0], dim[1]}, true, I.stackable};
        
        for(int i=0; i<6; i++){
            item_rot[i].pos = items[i].pos = C.fit(item_rot[i].o[0], item_rot[i].o[1], item_rot[i].o[2], item_rot[i].stackable);
            if (item_rot[i].pos.x >= 0) {
                C.packed.push_back(item_rot[i]);
                C.unloadingCost.push_back({-1.0, -1});
                if(DEBUG)
                    cout<<"packing "<<item_rot[i].sNo<<" at "<<item_rot[i].pos<<endl;
                break;
                // cout << items[i].pos << "\n";
            }
            else{
                items[i].pos = {-1, -1, -1};
            }
        }
        

        

        // random_shuffle(item_rot.begin(), item_rot.end());
        // cout<<"Fitting consignment "<<i<<'\n';
    }
    return C;
}

void read_csv(vector<Item> &Is, int &L, int &B, int &H, string filename) {
    ifstream file(filename);
    string line;
    getline(file, line);
    stringstream container_info(line);

    char hashtag; // consume the hashtag
    container_info >> hashtag >> L >> B >> H;
    getline(file, line);
	int i=1;
    cout << "(file : " << filename << ")    ";

    while (getline(file, line)) {
        stringstream ss(line);
        string item;

        Item I;
		I.sNo = I.locNo = i++;
        I.packed = true;

        for (int i = 0; getline(ss, item, ','); ++i) {
            if (i == 10)
                I.l = I.o[0] = stoi(item.substr(1, item.size() - 2));
            else if (i == 11)
                I.b = I.o[1] = stoi(item.substr(1, item.size() - 2));
            else if (i == 12)
                I.h = I.o[2] = stoi(item.substr(1, item.size() - 2));
            else if(i==15) {
				// cout<<item.substr(1, item.size()-2)<<' ';
				if(stoi(item.substr(1, item.size()-2)) == 1)
					I.stackable = true;
				else
					I.stackable = false;
				//cout<<I.stackable<<endl;
			}
        }
        Is.push_back(I);
        // vector<int> dim{I.l, I.b, I.h};
        // sort(dim.begin(), dim.end());

        // vector<Item> item_rot(6); // rotations of the item
        // item_rot[0] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[2], dim[1], true};
        // item_rot[1] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[2], dim[0], true};
        // item_rot[2] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[1], dim[2], true};
        // item_rot[3] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[0], dim[2], true};
        // item_rot[4] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[1], dim[0], true};
        // item_rot[5] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[0], dim[1], true};
        // for (int i=0; i<6; i++){
        //     Is.push_back(item_rot[i]);
        // }
    }
    cout<<"Package count "<<Is.size()<<endl;
}

pair<double, int> output_rep(Container &C, bool debug=false) {
    double occ_vol = 0.0, total_vol = (double)(C.L * C.B * C.B), vol;
	int count=0;
    for (int i = C.packed.size() - 1; i >= 0; i--) {
        // cout << "\nItem " << i + 1;
        // cout << "\tdimensions : " << Is[i].l << 'x' << Is[i].b << 'x' << Is[i].h;
        // cout << "\norientation : " << Is[i].o[0] << 'x' << Is[i].o[1] << 'x' << Is[i].o[2];
        // cout << "\tlocation : " << Is[i].pos->x << 'x' << Is[i].pos->y << 'x' << Is[i].pos->z;
        vol = (double)(C.packed[i].l * C.packed[i].b * C.packed[i].h);
        occ_vol += vol;
        occ_vol -= C.cost(i, debug).first;
        count += C.cost(i).second;
        // } else {
        // cout << "\nItem " << i + 1 << "\tNot Packed";
    }

    return pair<double, int>({occ_vol / total_vol, count});
}

// receive input data and pass on to 3dcpp fn
pair<double, double> packer(string filename) {
    int L, B, H;
    vector<Item> Is;

    read_csv(Is, L, B, H, filename);
    uniform_int_distribution<> int_dist(0, Is.size() - 1);
    uniform_int_distribution<> orientation_dist(0, 2);
    uniform_real_distribution<> float_dist(0, 1);

    auto start = chrono::steady_clock::now();

    Container C = threed_cpp(Is, L, B, H);
    pair<double, int> output = output_rep(C, true);
	double efficiency = output.first, bestEfficiency = output.first;
	int unloadingCostCount = output.second, bestKaCost = output.second;
	if(true){
		cout<<"Naive efficency : "<<efficiency*100<<"%\tunloading cost : "<<unloadingCostCount<<endl;
	}
    for (double temp = TEMP_MAX; temp > TEMP_MIN; temp *= COOLING_RATE) {
        // Number of swaps evaluated at each temperature, hardcoded for now
        int swaps_per_temp = 20;

        for (int i = 0; i < swaps_per_temp; i++) {
            if(i%5<4){
                int itemNo = int_dist(rng);
                int a = int_dist(rng), b = int_dist(rng);

                while (b == a) {
                    b = int_dist(rng);
                }
                
                Item IsA = Is[a], IsB = Is[b];
                swap(Is[a], Is[b]);

                C = threed_cpp(Is, L, B, H);
                output = output_rep(C);
                double new_eff = output.first;

                // change in energy between new and old state, i.e. cost function
                // = (change in efficiency in %) + (euclidean distance b/w the items / 100)
                double adist, bdist;
                if(IsA.pos.x==-1 || Is[a].pos.x==-1)
                    adist = sqrt(pow(L, 2) + pow(B, 2) + pow(H, 2));
                if(Is[a].pos.x==-1 && Is[a].pos.x==-1)
                        adist = 0;
                else
                    adist = Is[a].pos - IsA.pos;
                
                if(IsB.pos.x==-1 || Is[b].pos.x==-1)
                    bdist = sqrt(pow(L, 2) + pow(B, 2) + pow(H, 2));
                if(Is[B].pos.x==-1 && Is[b].pos.x==-1)
                    bdist = 0;
                else
                    bdist = Is[b].pos - IsB.pos;

                double del_E = (efficiency - new_eff) * 100 - (adist + bdist) / 100;

                // probability = exp(-ΔE / T)
                float probability = exp(-del_E / temp);
                float rand_val = float_dist(rng);

                if (DEBUG) {
                    cout << "unloading Cost : " << output.second
                            << " delEfficiency : " << (efficiency - new_eff) * 100
                            << "  dist : " << (Is[a].pos - Is[b].pos) / 100
                            << "  del_E : " << del_E
                            << "  probability : " << probability << endl;
                }

                if (rand_val > probability) { // probability too low, revert the change
                    swap(Is[b], Is[a]);
                } else {
                    efficiency = new_eff;
                    unloadingCostCount = output.second;
                }

                if(new_eff>bestEfficiency){
                    bestEfficiency = new_eff;
                    bestKaCost = output.second;
                }
            }
            else{
                int a = int_dist(rng), b = int_dist(rng);

                while (b == a) {
                    b = int_dist(rng);
                }

                Item IsA = Is[a], IsB = Is[b];
                swap(Is[a], Is[b]);

                C = threed_cpp(Is, L, B, H);
                output = output_rep(C);
                double new_eff = output.first;

                // change in energy between new and old state, i.e. cost function
                // = (change in efficiency in %) + (euclidean distance b/w the items / 100)
                double adist, bdist;
                if(IsA.pos.x==-1 || Is[a].pos.x==-1)
                    adist = sqrt(pow(L, 2) + pow(B, 2) + pow(H, 2));
                if(Is[a].pos.x==-1 && Is[a].pos.x==-1)
                        adist = 0;
                else
                    adist = Is[a].pos - IsA.pos;
                
                if(IsB.pos.x==-1 || Is[b].pos.x==-1)
                    bdist = sqrt(pow(L, 2) + pow(B, 2) + pow(H, 2));
                if(Is[B].pos.x==-1 && Is[b].pos.x==-1)
                    bdist = 0;
                else
                    bdist = Is[b].pos - IsB.pos;

                double del_E = (efficiency - new_eff) * 100 - (adist + bdist) / 100;

                // probability = exp(-ΔE / T)
                float probability = exp(-del_E / temp);
                float rand_val = float_dist(rng);

                if (DEBUG) {
                    cout << "unloading Cost : " << output.second
                            << " delEfficiency : " << (efficiency - new_eff) * 100
                            << "  dist : " << (Is[a].pos - Is[b].pos) / 100
                            << "  del_E : " << del_E
                            << "  probability : " << probability << endl;
                }

                if (rand_val > probability) { // probability too low, revert the change
                    swap(Is[b], Is[a]);
                } else {
                    efficiency = new_eff;
                    unloadingCostCount = output.second;
                }

                if(new_eff>bestEfficiency){
                    bestEfficiency = new_eff;
                    bestKaCost = output.second;
                }
            }
        }
    }

    chrono::duration<double> diff = chrono::steady_clock::now() - start;

    cout << "Time : " << diff.count() << "s\n";
    cout << "Last iter: Volume Optimization : " << efficiency * 100 << "%";
	cout << "Unloading Cost : " << unloadingCostCount << endl;
    cout << "Best iter: Volume Optimization : " << bestEfficiency * 100 << "%";
	cout << "Unloading Cost : " << bestKaCost << endl;
    return {diff.count(), efficiency * 100};
}

int main() {
    cout << fixed << setprecision(3);
    cout << "Please enter the subfolder in the current directory which contains "
                 "the .csv files:\n";
    string subfolder;
    cin >> subfolder;
    subfolder += "_";

    cout << "Please enter the number of .csv files in " << subfolder << ":\n";
    int total_files;
    cin >> total_files;

    double time = 0, vol_opt = 0;
    for (int i = 1; i <= total_files; i++) {
        auto temp = packer(subfolder + to_string(i) + ".csv");
        time += temp.first;
        vol_opt += temp.second;
    }

    cout << "\n    Total time taken: " << time << "s    "
              << "Average volume optimization: " << vol_opt / total_files << "%    for "
              << total_files << " files\n";
}
