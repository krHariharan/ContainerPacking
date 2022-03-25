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

#include <cmath>
#include <iostream>
#include <set>
#include <string>
#include <vector>

enum Orientation { // dimension along which item can be rotated
    ALL,
    HEIGHT,
    LENGTH,
    WIDTH,
    OTHER
};

// Location struct
struct Location {
    int x, y, z;

    double operator-(const Location &other) const { // Euclidean distance
        return std::sqrt(std::pow(x - other.x, 2) +
                         std::pow(y - other.y, 2) +
                         std::pow(z - other.z, 2));
    }
};

inline std::ostream &operator<<(std::ostream &out, const Location &l) {
    out << "(" << l.x << ", " << l.y << ", " << l.z << ")";
    return out;
}

// Item struct
struct Item {
    int l, b, h;
    int l1, b1, h1;
    bool packed;
    bool stackable;
    Location *pos;
    Orientation o;

    int id;
    float weight;
    float src_lat, src_long;
    float dst_lat, dst_long;

    bool is_below(const Item &I) const;
    bool is_behind(const Item &I) const;
    bool is_blocked_by(const Item &I) const;

    void set_dim1(int l, int b, int h);
};

// Container struct
struct Container {
    int L, B, H;                             // dimensions of container
    std::vector<std::vector<int>> v;         // height upto which each coordinate is filled
    std::vector<std::vector<int>> min_h;     // minimum height a consignment must be to be placed in a given coordinate, to prevent being blocked by consignment in front
    std::set<std::pair<int, int>> positions; // (x, y) of the left rear corner of every available cuboidal space, in lexicographical order
    std::vector<Item *> packed_items;        // details of packed items, in order of packing

    // Constructor to create class objects, given properties [dimensions] of container
    Container(int x, int y, int z);

    // given the dimensions of the consignment, in order of orientation,
    // return the (x,y) coordinates at which the the consignment can be packed
    Location *fit(int l, int b, int h);

    double vol_opt();         // Returns volume optimization of current packing of container
    double output_rep(int i); // Same, but also with output text
    void reset();             // reset contents of container, keep only dimensions
    int item_count();         // returns number of items currently packed
    bool check_valid();
};

void threed_cpp(Container &cont, std::vector<Item> &items);
