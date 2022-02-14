#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

struct Container {
    int len, wid, hei;
};

struct ConsignmentInfo {
    // unique_consignment_id is index of std::map consignments

    std::string date_added = "NULL", delivery_deadline_date = "NULL", goods_type = "NULL";
    int goods_quantity = 0;
    
    double shipment_charges = 0, max_temperature = 0, min_temperature = 0;
    std::string special_handling = "NULL", packaging_type = "NULL";
    int length = 0, width = 0, height = 0, weight = 0;
    
    bool is_floor_only = 0, is_stackable = 0;
    double stack_weight = 0;
    std::string stack_restriction_type = "STACK_TYPE_NONE"; // consignments_stack_restriction_type_enum
    // can be 'STACK_TYPE_NONE','STACK_TYPE_SAME_CARGO_ONLY','STACK_TYPE_SAME_SIZE_CARGO_ONLY'
    
    bool is_rotatable = 1;
    std::string rotation_type = "ROTATE_ANY"; // consignments_rotation_type_enum
    // can be 'ROTATE_ANY','ROTATE_LENGTH_ONLY','ROTATE_WIDTH_ONLY','ROTATE_HEIGHT_ONLY'
    
    std::string color = "Navy";
    std::string pickup_time_window_start = "NULL", pickup_time_window_end = "NULL";
    std::string dropoff_time_window_start = "NULL", dropoff_time_window_end = "NULL";

    int pickup_coordinate_id = 0, dropoff_coordinate_id = 0;
    int pickup_address_id = 0, dropoff_address_id = 0, current_operator_id = 0;
    
    std::string status = "NULL", pickup_timestamp = "NULL", delivered_timestamp = "NULL", box_description = "NULL";
};

std::vector<int> read_txt(std::ifstream &data, Container &con, std::map<int, ConsignmentInfo> &consignments, bool seed_present) {
    std::vector<int> result;

    long long int seed = 0;
    if (seed_present) data >> seed; // irrelevant, COMMENT OUT FOR "thpack8.txt"

    data >> con.len >> con.wid >> con.hei; // container length, width, height

    int n; // number of types of boxes
    data >> n;

    for (int j = 0; j < n; j++) {
        ConsignmentInfo new_consignment;

        int consignment_id;     // box type
        data >> consignment_id; // = j+1

        int ls, ws, hs; // 1 if vertical orientation permissible

        data >> new_consignment.length >> ls >> new_consignment.width >> ws >> new_consignment.height >> hs;


        if(((double) rand() / (RAND_MAX)<=0.8))
            new_consignment.is_stackable = 1;

        // convert ls, ws, hs to is_stackable and stack_restriction_type
        if (ls + ws + hs >= 1) {
            new_consignment.stack_restriction_type = "STACK_TYPE_SAME_CARGO_ONLY";

            if (ls + ws + hs == 1) {
                if (ls == 1) {
                    new_consignment.rotation_type = "ROTATE_LENGTH_ONLY";
                } else if (ws == 1) {
                    new_consignment.rotation_type = "ROTATE_WIDTH_ONLY";
                } else if (hs == 1) {
                    new_consignment.rotation_type = "ROTATE_HEIGHT_ONLY";
                }
            }
        }

        consignments[consignment_id] = new_consignment;

        int m; // number of boxes of type box
        data >> m;

        for (int i = 0; i < m; i++) {
            result.push_back(consignment_id);
        }
    }

    std::random_device rd;
    auto rng = std::default_random_engine{rd()};
    std::shuffle(result.begin(), result.end(), rng);

    return result;
}

void write_csv(Container &con, std::map<int, ConsignmentInfo> &consignments, std::vector<int> &IDs, std::string filename) {
    std::ofstream table(filename);
    table << "# " << con.len << " " << con.wid << " " << con.hei << "\n";
    table << "\"unique_consignment_id\",\"date_added\",\"delivery_deadline_date\",\"goods_type\",\"goods_quantity\",\"shipment_charges\",\"max_temperature\",\"min_temperature\",\"special_handling\",\"packaging_type\",\"length\",\"width\",\"height\",\"weight\",\"is_floor_only\",\"is_stackable\",\"stack_weight\",\"stack_restriction_type\",\"is_rotatable\",\"rotation_type\",\"color\",\"pickup_time_window_start\",\"pickup_time_window_end\",\"dropoff_time_window_start\",\"dropoff_time_window_end\",\"pickup_coordinate_id\",\"dropoff_coordinate_id\",\"pickup_address_id\",\"dropoff_address_id\",\"current_operator_id\",\"status\",\"pickup_timestamp\",\"delivered_timestamp\",\"box_description\"\n";

    for (int id : IDs) {
        table << "\"" << id << "\",\"";
        table << consignments[id].date_added << "\",\"";
        table << consignments[id].delivery_deadline_date << "\",\"";
        table << consignments[id].goods_type << "\",\"";
        table << consignments[id].goods_quantity << "\",\"";
        table << consignments[id].shipment_charges << "\",\"";
        table << consignments[id].max_temperature << "\",\"";
        table << consignments[id].min_temperature << "\",\"";
        table << consignments[id].special_handling << "\",\"";
        table << consignments[id].packaging_type << "\",\"";
        table << consignments[id].length << "\",\"";
        table << consignments[id].width << "\",\"";
        table << consignments[id].height << "\",\"";
        table << consignments[id].weight << "\",\"";
        table << consignments[id].is_floor_only << "\",\"";
        table << consignments[id].is_stackable << "\",\"";
        table << consignments[id].stack_weight << "\",\"";
        table << consignments[id].stack_restriction_type << "\",\"";
        table << consignments[id].is_rotatable << "\",\"";
        table << consignments[id].rotation_type << "\",\"";
        table << consignments[id].color << "\",\"";
        table << consignments[id].pickup_time_window_start << "\",\"";
        table << consignments[id].pickup_time_window_end << "\",\"";
        table << consignments[id].dropoff_time_window_start << "\",\"";
        table << consignments[id].dropoff_time_window_end << "\",\"";
        table << consignments[id].pickup_coordinate_id << "\",\"";
        table << consignments[id].dropoff_coordinate_id << "\",\"";
        table << consignments[id].pickup_address_id << "\",\"";
        table << consignments[id].dropoff_address_id << "\",\"";
        table << consignments[id].current_operator_id << "\",\"";
        table << consignments[id].status << "\",\"";
        table << consignments[id].pickup_timestamp << "\",\"";
        table << consignments[id].delivered_timestamp << "\",\"";
        table << consignments[id].box_description << "\"\n";
    }
}

int main() {
    std::cout << "Please enter the name of the input .txt file, without the extension:\n"
                 "Note: Subfolder of the same name must exist in the current directory."
              << std::endl;
    std::string dataset;
    std::cin >> dataset;
    std::ifstream data("thpack/" + dataset + ".txt");                    // append file extension
    bool seed_present = (dataset[dataset.size() - 1] < '8'); // seed is present from thpacks 1 to 7

    int p_total; // number of test problems
    data >> p_total;

    for (int i = 0; i < p_total; i++) {
        int p;                                       // problem number
        data >> p;                                   // p = i+1
        std::map<int, ConsignmentInfo> consignments; // info about each consignment

        Container con;
        std::vector<int> IDs = read_txt(data, con, consignments, seed_present);
        write_csv(con, consignments, IDs, "thpack/thpack/" + dataset + "_" + std::to_string(p) + ".csv");
    }

    data.close();

    return 0;
}
