#include <bits/stdc++.h>

// log(0.1 / 1) / log(0.9) is approximately 21 sweeps
const double TEMP_MAX = 1, TEMP_MIN = 0.1, COOLING_RATE = 0.9;

bool DEBUG = false;

std::random_device rd;
std::seed_seq sd{rd(), rd(), rd(), rd()};
std::mt19937 rng(rd());

struct Location {
    int x = -1, y = -1, z = -1;

    double operator-(const Location &other) const {
        return std::sqrt(std::pow(x - other.x, 2) +
                         std::pow(y - other.y, 2) +
                         std::pow(z - other.z, 2));
    }
};

struct Item {
    int l, b, h;
    int l1, b1, h1;
    bool packed;
    Location pos;
};

class Container {
    int L, B, H;
    std::vector<std::vector<int>> v;
    std::vector<std::vector<int>> minh;
    std::set<std::pair<int, int>> positions;

public:
    Container(int x, int y, int z) : L(x), B(y), H(z),
                                     v(x, std::vector<int>(y, 0)),
                                     minh(x, std::vector<int>(y, 0)) {
        positions.insert({0, 0});
    }

    Location fit(int l, int b, int h) {
        bool flag = false;
        int x, y, base;
        Location loc;

        for (auto pos : positions) {
            x = pos.first;
            y = pos.second;
            flag = true;
            base = v[x][y];

            if (base + h > H || x + l > L || y + b > B) continue;

            for (int m = 0; m < l; m++) {
                for (int n = 0; n < b; n++) {
                    if (v[x + m][y + n] != base || minh[x + m][y + n] >= h) {
                        flag = false;
                        break;
                    }
                }
                if (!flag) break;
            }

            if (flag) {
                loc = {x, y, base};
                break;
            }
        }

        if (loc.x < 0) return loc;

        for (int m = loc.x; m < loc.x + l; m++)
            for (int n = loc.y; n < loc.y + b; n++)
                v[m][n] += h;

        for (int n = loc.y; n < loc.y + b; n++) {
            for (int m = loc.x - 1; m >= 0; m--) {
                if (v[m][n] >= v[loc.x][n]) break;

                minh[m][n] = v[loc.x][n] - v[m][n];
            }
        }

        if (loc.x + l < L) positions.insert({loc.x + l, loc.y});
        if (loc.y + b < B) positions.insert({loc.x, loc.y + b});

        return loc;
    }
};

void threedcpp(std::vector<Item> &Items, int L, int B, int H) {
    Container C(L, B, H);

    for (int i = Items.size() - 1; i >= 0; i--) {
        Item I = Items[i];

        std::vector<int> dim{I.l, I.b, I.h};
        std::sort(dim.begin(), dim.end());

        std::vector<Item> Iarr(6);
        Iarr[0] = {I.l, I.b, I.h, dim[0], dim[2], dim[1], true};
        Iarr[1] = {I.l, I.b, I.h, dim[1], dim[2], dim[0], true};
        Iarr[2] = {I.l, I.b, I.h, dim[0], dim[1], dim[2], true};
        Iarr[3] = {I.l, I.b, I.h, dim[1], dim[0], dim[2], true};
        Iarr[4] = {I.l, I.b, I.h, dim[2], dim[1], dim[0], true};
        Iarr[5] = {I.l, I.b, I.h, dim[2], dim[0], dim[1], true};

        // random_shuffle(Iarr.begin(), Iarr.end());
        // cout<<"Fitting consignment "<<i<<'\n';
        for (int j = 0; j < 6; j++) {
            // if orientation i+1 is allowed for given package then do:
            Iarr[j].pos = C.fit(Iarr[j].l1, Iarr[j].b1, Iarr[j].h1);
            if (Iarr[j].pos.x >= 0) {
                Items[i] = Iarr[j];
                break;
            }
        }
    }
}

void readcsv(std::vector<Item> &Is, int &L, int &B, int &H, std::string fileName) {
    std::ifstream file(fileName);
    std::string line;
    std::getline(file, line);
    std::stringstream containerinfo(line);

    char hashtag; // consume the hashtag
    containerinfo >> hashtag >> L >> B >> H;
    getline(file, line);

    std::cout << "(file : " << fileName << ")    ";

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string item;

        Item I;
        I.packed = false;

        for (int i = 0; getline(ss, item, ','); ++i) {
            if (i == 10)
                I.l = I.l1 = stoi(item.substr(1, item.size() - 2));
            else if (i == 11)
                I.b = I.b1 = stoi(item.substr(1, item.size() - 2));
            else if (i == 12)
                I.h = I.h1 = stoi(item.substr(1, item.size() - 2));
        }
        Is.push_back(I);
    }
}

double outputRep(std::vector<Item> &Is, int L, int B, int H) {
    double occVol = 0.0, totalVol = (double)(L * B * H);
    for (int i = Is.size() - 1; i >= 0; i--) {
        if (Is[i].packed) {
            // cout << "\nItem " << i + 1;
            // cout << "\tdimensions : " << Is[i].l << 'x' << Is[i].b << 'x' << Is[i].h;
            // cout << "\norientation : " << Is[i].l1 << 'x' << Is[i].b1 << 'x' << Is[i].h1;
            // cout << "\tlocation : " << Is[i].pos->x << 'x' << Is[i].pos->y << 'x' << Is[i].pos->z;
            occVol += (double)(Is[i].l * Is[i].b * Is[i].h);
            // } else {
            // cout << "\nItem " << i + 1 << "\tNot Packed";
        }
    }

    return occVol / totalVol;
}

// receive input data and pass on to 3dcpp fn
std::pair<double, double> packer(std::string fileName) {
    int L, B, H;
    std::vector<Item> Is;

    readcsv(Is, L, B, H, fileName);

    threedcpp(Is, L, B, H);

    double efficiency = outputRep(Is, L, B, H);

    std::uniform_int_distribution<> intDist(0, Is.size() - 1);
    std::uniform_real_distribution<> floatDist(0, 1);

    auto start = std::chrono::steady_clock::now();
    for (double temp = TEMP_MAX; temp > TEMP_MIN; temp *= COOLING_RATE) {
        // Number of swaps evaluated at each temperature, hardcoded for now
        int swapsPerTemp = 10;

        for (int i = 0; i < swapsPerTemp; i++) {
            int a = intDist(rng), b = intDist(rng);

            while (b == a) {
                b = intDist(rng);
            }

            std::swap(Is[a], Is[b]);

            threedcpp(Is, L, B, H);
            double newEff = outputRep(Is, L, B, H);

            // change in energy between new and old state, i.e. cost function
            // = (change in efficiency in %) + (euclidean distance b/w the items / 100)
            double delE = (efficiency - newEff) * 100 + (Is[a].pos - Is[b].pos) / 100;

            // probability = exp(-ΔE / T)
            float probability = exp(-delE / temp);
            float randVal = floatDist(rng);

            if (DEBUG) {
                std::cout << "delEfficiency : " << (efficiency - newEff) * 100
                          << "  dist : " << (Is[a].pos - Is[b].pos) / 100
                          << "  delE : " << delE
                          << "  probability : " << probability << std::endl;
            }

            if (randVal > probability) { // probability too low, revert the change
                std::swap(Is[a], Is[b]);
            } else {
                efficiency = newEff;
            }
        }
    }
    std::chrono::duration<double> diff = std::chrono::steady_clock::now() - start;

    std::cout << "Time : " << diff.count() << "s    ";
    std::cout << "Volume Optimization : " << efficiency * 100 << "%\n";

    return {diff.count(), efficiency * 100};
}

int main() {
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Please enter the subfolder in the current directory which contains"
                 "the .csv files:\n";
    std::string subfolder;
    std::cin >> subfolder;
    subfolder += "/";

    std::cout << "Please enter the number of .csv files in " << subfolder << ":\n";
    int totalFiles;
    std::cin >> totalFiles;

    double time = 0, volOpt = 0;
    for (int i = 1; i <= totalFiles; i++) {
        auto temp = packer(subfolder + std::to_string(i) + ".csv");
        time += temp.first;
        volOpt += temp.second;
    }

    std::cout << "\n    Total time taken: " << time << "s    "
              << "Average volume optimization: " << volOpt / totalFiles << "%    for "
              << totalFiles << " files\n";
}
