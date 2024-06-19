#include "RecommenderSystemLoader.h"

#define MIN_VAL 1
#define MAX_VAL 10

using std::istringstream;
using std::string;
using std::stoi;
using std::ifstream;
using std::getline;


// see documentation
UniqueReq RecommenderSystemLoader::create_rs_from_movies_file(
        const std::string &movies_file_path) noexcept(false) {
    ifstream my_file(movies_file_path, std::ios::in);
    if(my_file) {
        string line;
        istringstream buffer;
        RecommenderSystem sys;
        UniqueReq rec_sys = std::make_unique<RecommenderSystem>(sys);
        std::vector<double> vec;
        while(getline(my_file, line)){
            buffer.str(line);
            string tmp;
            string movie_name;
            int year;
            bool first_input = true;
            while(getline(buffer, tmp, ' ')){
                if(first_input){
                    movie_name = tmp.substr(0, tmp.find('-'));
                    tmp.erase(0, tmp.find('-')+1);
                    year = stoi(tmp);
                    first_input = false;
                } else if(stoi(tmp) >= MIN_VAL && stoi(tmp) <= MAX_VAL){
                    vec.push_back(stoi(tmp));
                } else{
                    throw std::out_of_range("numbers aren't between 1 and 10");
                }
            }
            rec_sys->add_movie(movie_name, year, vec);
            vec.clear();
            buffer.clear();
        }
        return rec_sys;
    } else{
        throw std::runtime_error("failed to open file- sysLoader");
    }
}

