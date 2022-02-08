#include <bits/stdc++.h>

std::random_device rd;
std::seed_seq sd{rd(), rd(), rd(), rd()};
std::mt19937 rng(sd);

struct Location {
    int x = -1, y = -1, z = -1;
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
    std::vector<Item> Is;
    int L, B, H;

    readcsv(Is, L, B, H, fileName);

    double efficiency = 0.0;

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; i++) {
        // TODO: Simulated Annealing of Is

        threedcpp(Is, L, B, H);
        double tEff = outputRep(Is, L, B, H);

        if (tEff > efficiency) efficiency = tEff;
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
    std::string baseFile;
    std::cin >> baseFile;
    baseFile += "/";

    int totalFiles = 100;
    double time = 0, volOpt = 0;
    for (int i = 1; i <= totalFiles; i++) {
        auto temp = packer(baseFile + std::string(std::to_string(i)) + ".csv");
        time += temp.first;
        volOpt += temp.second;
    }

    std::cout << "\n    Total time taken: " << time << "s    "
              << "Average volume optimization: " << volOpt / totalFiles << "%    for "
              << totalFiles << " files\n";
}
