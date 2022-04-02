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

#include "packer.hpp"

#include <bits/stdc++.h>

double invoker(std::vector<Container> conts, std::vector<Item> items);
void read_txt(std::vector<Container> &conts, std::vector<Item> &items, std::string filename);
void write_txt(std::vector<Item> &items, std::string in_filename, std::string out_filename);

int main(int argc, char **argv) {
    // receive input data and pass on
    std::vector<Container> conts;
    std::vector<Item> items;
    read_txt(conts, items, argv[1]);

    double avg = invoker(conts, items);
    std::cout << "\nAverage volume optimization: " << avg << "\n";

    write_txt(items, argv[1], argv[2]);
}

// invokes the main algorithm function- threed_cpp() for each container
double invoker(std::vector<Container> conts, std::vector<Item> items) {
    double total_vol_opt = 0;
    for (int i = 0; i < conts.size(); ++i) {
        threed_cpp(conts[i], items);
        total_vol_opt += conts[i].output_rep(i);
    }

    return total_vol_opt / conts.size();
}

void read_txt(std::vector<Container> &conts, std::vector<Item> &items, std::string filename) {
    std::ifstream file(filename);
    std::string line;

    bool in_vehicles_section = false;
    bool in_consignment_section = false;
    bool in_vehicles_packing_section = false;

    while (getline(file, line)) {
        // check if line contains "Vehicles Section"
        if (line.find("Vehicles Section") != std::string::npos) {
            in_vehicles_section = true; // in_vehicles_section set to true if line contains "Vehicles Section"
            continue;
        }

        if (line.find("Vehicles Route Section") != std::string::npos) {
            in_vehicles_section = false; // end of vehicles section
        }

        // check if line contains "Consignment Section"
        if (line.find("Consignments Section") != std::string::npos) {
            in_consignment_section = true;
            continue;
        }

        if (line.find("Consignments Packing Section") != std::string::npos) {
            in_consignment_section = false; // end of consignments section
        }

        //check if line contains "Vehicles Packing Section"
        if (line.find("Vehicles Packing Section") != std::string::npos) {
            in_vehicles_packing_section = true; // in_vehicles_section set to true if line contains "Vehicles Section"
            continue;
        }

        if (line.find("Consignments Section") != std::string::npos) {
            in_vehicles_packing_section = false; // end of vehicles section
        }

        if (in_vehicles_section) {
            std::stringstream ss(line);

            std::string temp;
            for (int j = 0; j < 7; ++j)
                ss >> temp; // consume unnecessary data

            double l, b, h;
            ss >> l >> b >> h;
            conts.push_back(Container(l * 1000, b * 1000, h * 1000)); // casts to int
        } 



        else if(in_vehicles_packing_section) {
            std::stringstream ss(line);

            std::string temp;
            int id, count;
            std::vector<int> consignment_ids;

            //push back consignment ids to conts
            ss >> id;
            ss >> count;
            for (int j = 0; j < count; ++j) {
                ss >> temp;
                consignment_ids.push_back(std::stoi(temp));
            }
            //match id to container id and push consignment_ids back to container
            for (int j = 0; j < conts.size(); ++j) {
                if (conts[j].id == id) {
                    conts[j].consignment_ids = consignment_ids;
                }
            }


        }

        
        
        else if (in_consignment_section) {
            std::stringstream ss(line);
            std::string temp;
            Item item;

            ss >> item.id;       // id (int)
            ss >> temp;          // status (std::string)
            ss >> item.src_lat;  // src_lat is the latitude of the source of the item (float)
            ss >> item.src_long; // src_long is the longitude of the source of the item (float)
            ss >> item.dst_lat;  // dst_lat is the latitude of the destination of the item (float)
            ss >> item.dst_long; // dst_long is the longitude of the destination of the item (float)
            ss >> item.weight;   // weight is the weight of the item (int)
            ss >> item.l;        // l is the length of the item (int)
            ss >> item.b;        // b is the breadth of the item (int)
            ss >> item.h;        // h is the height of the item (int)

            int bool_val;
            ss >> bool_val;
            item.stackable = bool_val; // store boolean value of Stackable

            ss >> bool_val;
            item.o = bool_val ? ALL : HEIGHT; // store axis along which item can be rotated

            items.push_back(item); // add the item to the std::vector of items
        }
    }

    file.close();
}

void write_txt(std::vector<Item> &items, std::string in_filename, std::string out_filename) {
    std::ifstream file(in_filename);
    std::ofstream out(out_filename);
    std::string line;

    bool in_consignment_section = false, in_packing_section = false;
    int id_counter = 0;

    while (std::getline(file, line)) {
        if (line.find("Consignments Section") != std::string::npos) {
            in_consignment_section = true;
            out << line << "\n";
            continue;
        }

        if (line.find("Consignments Packing Section") != std::string::npos) {
            in_consignment_section = false; // end of consignments section
            in_packing_section = true;
            out << line << "\n";
            continue;
        }

        if (in_consignment_section) {
            std::stringstream in(line);

            int id;
            in >> id;

            std::string temp;
            in >> temp;             // consume allocation status
            std::getline(in, temp); // get rest of the line

            out << id << " Allocated " << temp << "\n";
        } else if (in_packing_section) {
            out << items[id_counter].id << " ";

            if (items[id_counter].packed) {
                out << "Packed" << items[id_counter].pos->x << " " << items[id_counter].pos->y
                    << " " << items[id_counter].pos->z << "\n";
            } else {
                out << "Unpacked\n";
            }
        } else {
            out << line << "\n";
        }
    }

    file.close();
    out.close();
}

bool Item::is_below(const Item &item) const {
    return pos->z + h <= item.pos->z &&                                // somewhere below,
           (item.pos->x <= pos->x && pos->x <= item.pos->x + item.l || // stacked in
            item.pos->x <= pos->x + l && pos->x + l <= item.pos->x + item.l) &&
           (item.pos->y <= pos->y && pos->y <= item.pos->y + item.b || // the same column
            item.pos->y <= pos->y + b && pos->y + b <= item.pos->y + item.b);
}

bool Item::is_behind(const Item &item) const {
    return pos->x + l < item.pos->x &&                              // somewhere behind,
           pos->z < item.pos->z + item.h &&                         // even slightly below,
           (pos->y < item.pos->y && item.pos->y < pos->y + b ||     // overlapping
            item.pos->y < pos->y && pos->y < item.pos->y + item.b); // in y direction
}

bool Item::is_blocked_by(const Item &item) const {
    return is_below(item) || is_behind(item);
}

void Item::set_dim1(int l, int b, int h) {
    this->l1 = l;
    this->b1 = b;
    this->h1 = h;
}

Container::Container(int x, int y, int z) : L(x), B(y), H(z),
                                            v(x, std::vector<int>(y)),
                                            min_h(x, std::vector<int>(y)) {
    positions.insert({0, 0});
}

Location *Container::fit(int l, int b, int h) {
    Location *loc = new Location{-1, -1, -1};
    bool pos_valid;

    for (auto p : positions) {
        int x = p.first, y = p.second;
        int base = v[x][y];
        pos_valid = true;

        if (base + h > H) {
            continue; // height of consignment, if packed at (x,y), exceeds the height of the container
        }
        if (x + l > L || y + b > B) {
            continue;
        }

        for (int m = 0; m < l; m++) {
            for (int n = 0; n < b; n++) {
                if (v[x + m][y + n] != base || min_h[x + m][y + n] >= h) {
                    pos_valid = false; // flag position 0 if either consignemnt doesn't have the min height, or base is uneven at this area
                    break;
                }
            }

            if (!pos_valid) {
                break; // position doesn't satisfy reqts - skipped
            }
        }

        if (pos_valid) { // position chosen
            *loc = {x, y, base};
            break;
        }
    }
    if (loc->x < 0) {
        return nullptr; // no suitable location found for consignment in given orientation
    }

    // update current height from (x, y) to (x + l, y + b)
    for (int m = loc->x; m < loc->x + l; m++) {
        for (int n = loc->y; n < loc->y + b; n++) {
            v[m][n] += h;
        }
    }

    // minh ensures that a future item does not hide behind an already placed item
    for (int m = loc->x - 1; m >= 0; m--) {
        for (int n = loc->y; n < loc->y + b; n++) {
            if (v[m][n] >= v[loc->x][n]) {
                break;
            }

            min_h[m][n] = v[loc->x][n] - v[m][n];
        }
    }

    // additional possible positions
    if (loc->x + l < L) {
        positions.insert({loc->x + l, loc->y});
    }

    if (loc->y + b < B) {
        positions.insert({loc->x, loc->y + b});
    }

    return loc;
}

double Container::vol_opt() {
    double occ_vol = 0;

    for (auto &item : packed_items) {
        occ_vol += ((double)item->l * item->b * item->h);
    }

    return occ_vol / ((double)L * B * H);
}

double Container::output_rep(int i) {
    double vol_opt = Container::vol_opt();
    std::cout << i << ":   Volume Optimization " << vol_opt << "   Validity Check "
              << Container::check_valid() << "   Item Count " << Container::item_count() << "\n";

    return vol_opt;
}

void Container::reset() {
    positions.clear();
    positions.insert({0, 0});

    for (int i = 0; i < L; i++) {
        for (int j = 0; j < B; j++) {
            v[i][j] = 0;
            min_h[i][j] = 0;
        }
    }

    packed_items.clear();
}

int Container::item_count() {
    return packed_items.size();
}

bool Container::check_valid() {
    for (auto &item : packed_items) {
        if (item->o == HEIGHT && item->h != item->h1) {
            return false;
        }
    }

    return true;
}