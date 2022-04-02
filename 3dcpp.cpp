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

const int TREE_WIDTH = 2;

// helper fn for threed_cpp used in sorting different container packing options
bool greater_pair(std::pair<double, Container> &a, std::pair<double, Container> &b);

// given a consignment with it's assigned position, "pack" the consignment and update the colume space of container
void pack_item(Container &C, Item I);

// Given a partially packed container as input, fn packs remaining consignments using a purely greedy approach and returns the resultant volume optimization
double greedy(Container C, std::vector<Item> &Items, int starting);

// Given the consignments to be packed (in order of destination) and dimensions of container, returns container object with an optimal packing based on decision tree heurisitics
void threed_cpp(Container &C, std::vector<Item> &Items) {
    std::vector<std::pair<double, Container>> options;          // contains all possible packing options of limited size decision tree in descending order of vol_opt
    options.push_back({greedy(C, Items, Items.size() - 1), C}); // initializing options with empty container and purely greedy packing volume optimization
                                                                // cout<<"Greedy Optimization: "<<options[0].first<<endl;
    for (int i = Items.size() - 1; i >= 0; i--) {
        Item I = Items[i];

        std::vector<int> dim{I.l, I.b, I.h};
        std::sort(dim.begin(), dim.end());

        // all possible orientations - unlike the greedy fn, in decision tree fn, order is irrelevant
        std::vector<Item> Iarr(6);
        Iarr[0] = {I.l, I.b, I.h, dim[0], dim[2], dim[1], true, I.stackable, Location(), I.o};
        Iarr[1] = {I.l, I.b, I.h, dim[1], dim[2], dim[0], true, I.stackable, Location(), I.o};
        Iarr[2] = {I.l, I.b, I.h, dim[0], dim[1], dim[2], true, I.stackable, Location(), I.o};
        Iarr[3] = {I.l, I.b, I.h, dim[1], dim[0], dim[2], true, I.stackable, Location(), I.o};
        Iarr[4] = {I.l, I.b, I.h, dim[2], dim[1], dim[0], true, I.stackable, Location(), I.o};
        Iarr[5] = {I.l, I.b, I.h, dim[2], dim[0], dim[1], true, I.stackable, Location(), I.o};

        // updating decision tree options
        for (int k = options.size() - 1; k >= 0; k--) {
            // case skipping Item i considered
            C = options[k].second;
            options.push_back({greedy(C, Items, i - 1), C});

            // case of packing item I considered
            for (int j = 0; j < 6; j++) {
                // check if orientation i+1 is allowed for given consignment
                if ((I.o == HEIGHT && Iarr[j].h != Iarr[j].h1) || (I.o == LENGTH && Iarr[j].l != Iarr[j].l1) || (I.o == WIDTH && Iarr[j].b != Iarr[j].b1))
                    continue;
                Iarr[j].pos = options[k].second.fit(Iarr[j].l1, Iarr[j].b1, Iarr[j].h1);
                // if orientation i+1 is allowed for given consignment, pack into container and and keep this packing as an option
                if (Iarr[j].pos.x != -1) {
                    C = options[k].second;
                    pack_item(C, Iarr[j]);
                    options.push_back({greedy(C, Items, i - 1), C});
                }
            }

            // old packing that did not consider Item i removed
            options.erase(options.begin() + k);
        }

        // sort options in descending order of volume optimization
        std::sort(options.begin(), options.end(), greater_pair);

        // keeping 'TREE_WIDTH' most optimal packings
        if (options.size() > TREE_WIDTH) {
            options.resize(TREE_WIDTH, {greedy(Container(C.L, C.B, C.H), Items, Items.size() - 1), Container(C.L, C.B, C.H)});
        }
    }
}

double greedy(Container C, std::vector<Item> &Items, int starting) {
    // cout << "Container created";
    for (int i = starting; i >= 0; i--) {
        Item I = Items[i];

        std::vector<int> dim{I.l, I.b, I.h};
        std::sort(dim.begin(), dim.end());

        // all possible orientations - ordered based on heuristic considering protrusion length and stability
        std::vector<Item> Iarr(6);
        Iarr[0] = {I.l, I.b, I.h, dim[0], dim[2], dim[1], true, I.stackable, Location(), I.o};
        Iarr[1] = {I.l, I.b, I.h, dim[1], dim[2], dim[0], true, I.stackable, Location(), I.o};
        Iarr[2] = {I.l, I.b, I.h, dim[0], dim[1], dim[2], true, I.stackable, Location(), I.o};
        Iarr[3] = {I.l, I.b, I.h, dim[1], dim[0], dim[2], true, I.stackable, Location(), I.o};
        Iarr[4] = {I.l, I.b, I.h, dim[2], dim[1], dim[0], true, I.stackable, Location(), I.o};
        Iarr[5] = {I.l, I.b, I.h, dim[2], dim[0], dim[1], true, I.stackable, Location(), I.o};

        for (int j = 0; j < 6; j++) {
            // if orientation i+1 is allowed for given package
            if ((I.o == HEIGHT && Iarr[j].h != Iarr[j].h1) || (I.o == LENGTH && Iarr[j].l != Iarr[j].l1) || (I.o == WIDTH && Iarr[j].b != Iarr[j].b1)) {
                continue;
            }

            Iarr[j].pos = C.fit(Iarr[j].l1, Iarr[j].b1, Iarr[j].h1);
            if (Iarr[j].pos.x != -1) {
                pack_item(C, Iarr[j]);
                break;
            }
        }
    }

    return C.vol_opt();
}

void pack_item(Container &C, Item I) {
    if (I.pos.x == -1) {
        return;
    }

    // Updating remaining volume space of container after consignment is packed
    for (int m = I.pos.x; m < I.pos.x + I.l1; m++) {
        for (int n = I.pos.y; n < I.pos.y + I.b1; n++) {
            C.v[m][n] += I.h1;
        }
    }

    // updating min_h - the minimum height another consignment needs to be, to be packed behind current consignment without being blocked when unloading
    for (int n = I.pos.y; n < I.pos.y + I.b1; n++) {
        for (int m = I.pos.x - 1; m >= 0; m--) {
            if (C.v[m][n] >= C.v[I.pos.x][n]) {
                break;
            }

            C.min_h[m][n] = C.v[I.pos.x][n] - C.v[m][n];
        }
    }

    // Updating remaining volume space of container if Item is not stackable
    if (!I.stackable) {
        for (int m = I.pos.x; m < I.pos.x + I.l1; m++) {
            for (int n = I.pos.y; n < I.pos.y + I.b1; n++) {
                C.v[m][n] = C.H;
            }
        }
    }

    // packing a consignment breaks up available volume space into smaller sub-cuboids. Coordinates of the same inserted into positions
    if (I.pos.x + I.l1 < C.L) {
        C.positions.insert({I.pos.x + I.l1, I.pos.y});
    }
    if (I.pos.y + I.b1 < C.B) {
        C.positions.insert({I.pos.x, I.pos.y + I.b1});
    }

    // consignment "packed"
    I.packed = true;
    C.packed_items.push_back(I);
}

bool greater_pair(std::pair<double, Container> &a, std::pair<double, Container> &b) {
    if (a.first > b.first) {
        return true;
    } else if (b.first > a.first) {
        return false;
    } else {
        return a.second.item_count() <= b.second.item_count(); // more larger packages more likely to have been packed in packing with less overall packages
    }
}
