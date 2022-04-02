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

// log(0.1 / 1) / log(0.9) is approximately 21 sweeps
const double TEMP_MAX = 1, TEMP_MIN = 0.1, COOLING_RATE = 0.9;

bool DEBUG = false;

std::random_device rd;
std::seed_seq sd{rd(), rd(), rd(), rd()};
std::mt19937 rng(rd());

// Given the consignments to be packed (in order of destination) and dimensions of container, returns container object with an optimal packing based on decision tree heurisitics
void packer(Container &cont, std::vector<Item> &items) {
    cont.reset();

    for (int i = items.size() - 1; i >= 0; i--) {
        Item item = items[i];

        std::vector<int> dim{item.l, item.b, item.h};
        std::sort(dim.begin(), dim.end());

        std::vector<Item> item_rot(6, item); // rotations of the item
        item_rot[0].set_dim1(dim[0], dim[2], dim[1]);
        item_rot[1].set_dim1(dim[1], dim[2], dim[0]);
        item_rot[2].set_dim1(dim[0], dim[1], dim[2]);
        item_rot[3].set_dim1(dim[1], dim[0], dim[2]);
        item_rot[4].set_dim1(dim[2], dim[1], dim[0]);
        item_rot[5].set_dim1(dim[2], dim[0], dim[1]);

        // random_shuffle(item_rot.begin(), item_rot.end());
        for (auto &item : item_rot) {
            // if orientation i+1 is allowed for given package then do:
            item.pos = cont.fit(item.l1, item.b1, item.h1);
            if (item.pos != nullptr && item.pos->x >= 0) {
                items[i] = item;
                items[i].packed = true;
                cont.packed_items.push_back(&items[i]);
                break;
            }
        }
    }
}

// receive input data and pass on to 3dcpp fn
void threed_cpp(Container &cont, std::vector<Item> &items) {
    std::uniform_int_distribution<> int_dist(0, items.size() - 1);
    std::uniform_real_distribution<> float_dist(0, 1);



    //ensure that items contains only those consignments which belong to that container
    std::vector<Item> items_in_cont;
    for (auto &item : items) {
        //check if item.id is in cont.consignment_ids
        if (std::find(cont.consignment_ids.begin(), cont.consignment_ids.end(), item.id) != cont.consignment_ids.end()) {
            items_in_cont.push_back(item);
        }
    }

    // if no items in container, return
    if (items_in_cont.empty()) {
        return;
    }



    packer(cont, items_in_cont);
    double efficiency = cont.vol_opt();

    for (double temp = TEMP_MAX; temp > TEMP_MIN; temp *= COOLING_RATE) {
        // Number of swaps evaluated at each temperature, hardcoded for now
        int swaps_per_temp = 10;

        for (int i = 0; i < swaps_per_temp; i++) {

            int a = int_dist(rng), b = int_dist(rng);

            while (b == a) {
                b = int_dist(rng);
            }
            std::swap(items_in_cont[a], items_in_cont[b]);

            packer(cont, items_in_cont);
            double new_eff = cont.vol_opt();

            // change in energy between new and old state, i.e. cost function
            // = (change in efficiency in %) + (euclidean distance b/w the items_in_cont / 100)
            double del_E = (efficiency - new_eff) * 100 + (items_in_cont[a].pos - items_in_cont[b].pos) / 100;

            // probability = exp(-ΔE / T)
            float probability = exp(-del_E / temp);
            float rand_val = float_dist(rng);

            if (DEBUG) {
                std::cout << "delEfficiency : " << (efficiency - new_eff) * 100
                          << "  dist : " << (items_in_cont[a].pos - items_in_cont[b].pos) / 100
                          << "  del_E : " << del_E
                          << "  probability : " << probability << std::endl;
            }

            if (rand_val > probability) { // probability too low, revert the change
                std::swap(items_in_cont[a], items_in_cont[b]);
            } else {
                efficiency = new_eff;
            }
        }
    }
}