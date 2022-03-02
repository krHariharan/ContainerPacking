#include <bits/stdc++.h>

// log(0.1 / 1) / log(0.9) is approximately 21 sweeps
const double TEMP_MAX = 1, TEMP_MIN = 0.1, COOLING_RATE = 0.9;

bool DEBUG = false;

std::random_device rd;
std::seed_seq sd{rd(), rd(), rd(), rd()};
std::mt19937 rng(rd());

class Location {
public:
    int x = -1, y = -1, z = -1;

    double operator-(const Location &other) const {
        return std::sqrt(std::pow(x - other.x, 2) +
                         std::pow(y - other.y, 2) +
                         std::pow(z - other.z, 2));
    }
};

std::ostream &operator<<(std::ostream &out, const Location &l) {
    out << "(" << l.x << ", " << l.y << ", " << l.z << ")";
    return out;
}

class Item {
public:
    int l, b, h;
    int l1, b1, h1;
    bool packed;
    Location pos;

    bool is_below(const Item &I) const {
        return pos.z + h <= I.pos.z &&                        // somewhere below, &
               (I.pos.x <= pos.x && pos.x <= I.pos.x + I.l || // stacked in
                I.pos.x <= pos.x + l && pos.x + l <= I.pos.x + I.l) &&
               (I.pos.y <= pos.y && pos.y <= I.pos.y + I.b || // the same column
                I.pos.y <= pos.y + b && pos.y + b <= I.pos.y + I.b);
    }

    bool is_behind(const Item &I) const {
        return pos.x + l < I.pos.x &&                        // somewhere behind, &
               (pos.y < I.pos.y && I.pos.y < pos.y + b ||    // overlapping
                I.pos.y < pos.y && pos.y < I.pos.y + I.b) && // in y direction
               pos.z < I.pos.z + I.h;                        // even slightly below
    }

    bool is_blocked_by(const Item &I) const {
        return is_below(I) || is_behind(I);
    }
};

class Container {
    int L, B, H;                             // dimensions of the container
    std::vector<std::vector<int>> v;         // current height at (x, y)
    std::vector<std::vector<int>> minh;      // minimum allowed item height at (x, y)
    std::set<std::pair<int, int>> positions; // current available positions

public:
    Container(int x, int y, int z) : L(x), B(y), H(z),
                                     v(x, std::vector<int>(y, 0)),
                                     minh(x, std::vector<int>(y, 0)) {
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

void threed_cpp(std::vector<Item> &items, int L, int B, int H) {
    Container C(L, B, H);

    for (int i = items.size() - 1; i >= 0; i--) {
        Item I = items[i];

        std::vector<int> dim{I.l, I.b, I.h};
        std::sort(dim.begin(), dim.end());

        std::vector<Item> item_rot(6); // rotations of the item
        item_rot[0] = {I.l, I.b, I.h, dim[0], dim[2], dim[1], true};
        item_rot[1] = {I.l, I.b, I.h, dim[1], dim[2], dim[0], true};
        item_rot[2] = {I.l, I.b, I.h, dim[0], dim[1], dim[2], true};
        item_rot[3] = {I.l, I.b, I.h, dim[1], dim[0], dim[2], true};
        item_rot[4] = {I.l, I.b, I.h, dim[2], dim[1], dim[0], true};
        item_rot[5] = {I.l, I.b, I.h, dim[2], dim[0], dim[1], true};

        // random_shuffle(item_rot.begin(), item_rot.end());
        // cout<<"Fitting consignment "<<i<<'\n';
        for (auto &item : item_rot) {
            // if orientation i+1 is allowed for given package then do:
            item.pos = C.fit(item.l1, item.b1, item.h1);
            if (item.pos.x >= 0) {
                items[i] = item;
                // std::cout << items[i].pos << "\n";
                break;
            }
        }
    }
}

void read_csv(std::vector<Item> &Is, int &L, int &B, int &H, std::string filename) {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    std::stringstream container_info(line);

    char hashtag; // consume the hashtag
    container_info >> hashtag >> L >> B >> H;
    getline(file, line);

    std::cout << "(file : " << filename << ")    ";

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string item;

        Item I;
        I.packed = false;

        for (int i = 0; getline(ss, item, ','); ++i) {
            if (i == 10)
                I.l = I.l1 = std::stoi(item.substr(1, item.size() - 2));
            else if (i == 11)
                I.b = I.b1 = std::stoi(item.substr(1, item.size() - 2));
            else if (i == 12)
                I.h = I.h1 = std::stoi(item.substr(1, item.size() - 2));
        }
        Is.push_back(I);
    }
}

double output_rep(std::vector<Item> &Is, int L, int B, int H) {
    double occ_vol = 0.0, total_vol = (double)(L * B * H);
    for (int i = Is.size() - 1; i >= 0; i--) {
        if (Is[i].packed) {
            // std::cout << "\nItem " << i + 1;
            // std::cout << "\tdimensions : " << Is[i].l << 'x' << Is[i].b << 'x' << Is[i].h;
            // std::cout << "\norientation : " << Is[i].l1 << 'x' << Is[i].b1 << 'x' << Is[i].h1;
            // std::cout << "\tlocation : " << Is[i].pos->x << 'x' << Is[i].pos->y << 'x' << Is[i].pos->z;
            occ_vol += (double)(Is[i].l * Is[i].b * Is[i].h);
            // } else {
            // std::cout << "\nItem " << i + 1 << "\tNot Packed";
        }
    }

    return occ_vol / total_vol;
}

// receive input data and pass on to 3dcpp fn
std::pair<double, double> packer(std::string filename) {
    int L, B, H;
    std::vector<Item> Is;

    read_csv(Is, L, B, H, filename);
    std::uniform_int_distribution<> int_dist(0, Is.size() - 1);
    std::uniform_real_distribution<> float_dist(0, 1);

    auto start = std::chrono::steady_clock::now();

    threed_cpp(Is, L, B, H);
    double efficiency = output_rep(Is, L, B, H);

    for (double temp = TEMP_MAX; temp > TEMP_MIN; temp *= COOLING_RATE) {
        // Number of swaps evaluated at each temperature, hardcoded for now
        int swaps_per_temp = 10;

        for (int i = 0; i < swaps_per_temp; i++) {
            int a = int_dist(rng), b = int_dist(rng);

            while (b == a) {
                b = int_dist(rng);
            }

            std::swap(Is[a], Is[b]);

            threed_cpp(Is, L, B, H);
            double new_eff = output_rep(Is, L, B, H);

            // change in energy between new and old state, i.e. cost function
            // = (change in efficiency in %) + (euclidean distance b/w the items / 100)
            double del_E = (efficiency - new_eff) * 100 + (Is[a].pos - Is[b].pos) / 100;

            // probability = exp(-ΔE / T)
            float probability = exp(-del_E / temp);
            float rand_val = float_dist(rng);

            if (DEBUG) {
                std::cout << "delEfficiency : " << (efficiency - new_eff) * 100
                          << "  dist : " << (Is[a].pos - Is[b].pos) / 100
                          << "  del_E : " << del_E
                          << "  probability : " << probability << std::endl;
            }

            if (rand_val > probability) { // probability too low, revert the change
                std::swap(Is[a], Is[b]);
            } else {
                efficiency = new_eff;
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
    std::cout << "Please enter the subfolder in the current directory which contains "
                 "the .csv files:\n";
    std::string subfolder;
    std::cin >> subfolder;
    subfolder += "/";

    std::cout << "Please enter the number of .csv files in " << subfolder << ":\n";
    int total_files;
    std::cin >> total_files;

    double time = 0, vol_opt = 0;
    for (int i = 1; i <= total_files; i++) {
        auto temp = packer(subfolder + std::to_string(i) + ".csv");
        time += temp.first;
        vol_opt += temp.second;
    }

    std::cout << "\n    Total time taken: " << time << "s    "
              << "Average volume optimization: " << vol_opt / total_files << "%    for "
              << total_files << " files\n";
}
