#include <bits/stdc++.h>
using namespace std;
// log(0.1 / 1) / log(0.9) is approximately 21 sweeps
const double TEMP_MAX = 1, TEMP_MIN = 0.1, COOLING_RATE = 0.9;

bool DEBUG = true;

random_device rd;
seed_seq sd{rd(), rd(), rd(), rd()};
mt19937 rng(rd());

class Location {
public:
    int x = -1, y = -1, z = -1;

    double operator-(const Location &other) const {
        return sqrt(pow(x - other.x, 2) +
                         pow(y - other.y, 2) +
                         pow(z - other.z, 2));
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
    int L, B, H;                             // dimensions of the container
    vector<vector<int>> v;         // current height at (x, y)
    vector<vector<int>> minh;      // minimum allowed item height at (x, y)
    set<pair<int, int>> positions; // current available positions
public:
    vector<Item> packed;
    Container(int x, int y, int z) : L(x), B(y), H(z),
                                     v(x, vector<int>(y, 0)),
                                     minh(x, vector<int>(y, 0)) {
        positions.insert({0, 0});
    }

    Location fit(int l, int b, int h) {
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
                    if (v[x + m][y + n] != base || minh[x + m][y + n] >= h) {
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
                if (v[m][n] >= v[loc.x][n]) break;

                minh[m][n] = v[loc.x][n] - v[m][n];
            }
        }

        // additional possible positions
        if (loc.x + l < L) positions.insert({loc.x + l, loc.y});
        if (loc.y + b < B) positions.insert({loc.x, loc.y + b});

        return loc;
    }
};

Container threed_cpp(vector<Item> &items, int L, int B, int H) {
    Container C(L, B, H);
    for (int i = items.size() - 1; i >= 0; i--) {
        Item item = items[i];
        item.pos = C.fit(item.o[0], item.o[1], item.o[2]);
        if (item.pos.x >= 0) {
            C.packed.push_back(item);
            // cout << items[i].pos << "\n";
        }
        // vector<int> dim{I.l, I.b, I.h};
        // sort(dim.begin(), dim.end());

        // vector<Item> item_rot(6); // rotations of the item
        // item_rot[0] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[2], dim[1], true};
        // item_rot[1] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[2], dim[0], true};
        // item_rot[2] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[0], dim[1], dim[2], true};
        // item_rot[3] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[1], dim[0], dim[2], true};
        // item_rot[4] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[1], dim[0], true};
        // item_rot[5] = {I.sNo, I.locNo, I.l, I.b, I.h, dim[2], dim[0], dim[1], true};

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

pair<double, int> output_rep(vector<Item> &Is, int L, int B, int H, bool debug=false) {
    double occ_vol = 0.0, total_vol = (double)(L * B * H), vol;
	int count=0;
    for (int i = Is.size() - 1; i >= 0; i--) {
        if (Is[i].packed) {
            // cout << "\nItem " << i + 1;
            // cout << "\tdimensions : " << Is[i].l << 'x' << Is[i].b << 'x' << Is[i].h;
            // cout << "\norientation : " << Is[i].o[0] << 'x' << Is[i].o[1] << 'x' << Is[i].o[2];
            // cout << "\tlocation : " << Is[i].pos->x << 'x' << Is[i].pos->y << 'x' << Is[i].pos->z;
            vol = (double)(Is[i].l * Is[i].b * Is[i].h);
			occ_vol += vol;
            // } else {
            // cout << "\nItem " << i + 1 << "\tNot Packed";
			for(int j=Is.size() - 1; j>=0; j--){
				if(i!=j && Is[i].is_blocked_by(Is[j]) && Is[i].locNo<Is[j].locNo){
					occ_vol-=4*(double)(Is[j].l * Is[j].b * Is[j].h);
					count++;
					if(debug){
						if(Is[i].is_behind(Is[j]))
							cout<<"behind ";
						if(Is[i].is_below(Is[j]))
							cout<<"below ";
					 	printf("%d: (%d, %d, %d)->(%d, %d, %d) blocked by %d: (%d, %d, %d)->(%d, %d, %d)\n", i, Is[i].pos.x, Is[i].pos.y, Is[i].pos.z, Is[i].pos.x+Is[i].o[0], Is[i].pos.y+Is[i].o[1], Is[i].pos.z+Is[i].o[2], j, Is[j].pos.x, Is[j].pos.y, Is[j].pos.z, Is[j].pos.x+Is[j].o[0], Is[j].pos.y+Is[j].o[1], Is[j].pos.z+Is[j].o[2]);
					}
				}
			}
        }
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
    pair<double, int> output = output_rep(C.packed, L, B, H, DEBUG);
	double efficiency = output.first;
	int unloadingCostCount = output.second;
	if(DEBUG){
		cout<<"Naive efficency : "<<efficiency<<"%\tunloading cost : "<<unloadingCostCount<<endl;
	}

    for (double temp = TEMP_MAX; temp > TEMP_MIN; temp *= COOLING_RATE) {
        // Number of swaps evaluated at each temperature, hardcoded for now
        int swaps_per_temp = 10;

        for (int i = 0; i < swaps_per_temp; i++) {
            int itemNo = int_dist(rng);
            int a = orientation_dist(rng), b = orientation_dist(rng);

            while (b == a) {
                b = orientation_dist(rng);
            }

            swap(Is[itemNo].o[a], Is[itemNo].o[b]);

            C = threed_cpp(Is, L, B, H);
			output = output_rep(C.packed, L, B, H);
            double new_eff = output.first;

            // change in energy between new and old state, i.e. cost function
            // = (change in efficiency in %) + (euclidean distance b/w the items / 100)
            double del_E = (efficiency - new_eff) * 100 + (Is[a].pos - Is[b].pos) / 100;

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
                 swap(Is[itemNo].o[b], Is[itemNo].o[a]);
            } else {
                efficiency = new_eff;
				unloadingCostCount = output.second;
            }
        }
    }

    chrono::duration<double> diff = chrono::steady_clock::now() - start;

    cout << "Time : " << diff.count() << "s    ";
    cout << "Volume Optimization : " << efficiency * 100 << "%\n";
	cout << "Unloading Cost : " << unloadingCostCount << endl;
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
