#include "RSUsersLoader.h"

#define MIN_VAL 1
#define MAX_VAL 10

using std::string;
using std::istringstream;
using std::stoi;


// helper - creates the map of rankings as specified
void RSUsersLoader::map_helper(rank_map& rm, sp_movie& sp, string& tmp){
    string x;
    istringstream buffer(tmp);
    std::getline(buffer,x,'\r');
    istringstream buffer1(x);
    std::getline(buffer1,x,'\n');
    if(x == "NA"){
        rm[sp] = 0;
    } else{
        if(stoi(x) <= MAX_VAL && stoi(x) >= MIN_VAL){
            rm[sp] = stoi(x);
        } else{
            throw std::out_of_range("numbers aren't between 1 and 10");
        }
    }
}


// helper - creates vector of all the movies in line 1 of file
std::vector<sp_movie> RSUsersLoader::create_movie_vec(string& line, UniqueReq&
            rs){
    std::vector<sp_movie> movie_vec;
    string tmp;
    istringstream buffer(line);
    while(std::getline(buffer, tmp, ' ')){//enter the movies to sp vector
        string movie_name = tmp.substr(0, tmp.find('-'));
        tmp.erase(0, tmp.find('-')+1);
        int year = stoi(tmp);
        movie_vec.push_back(rs->get_movie(movie_name, year));
    }
    buffer.clear();
    return movie_vec;
}


// see documentation
std::vector<RSUser> RSUsersLoader::create_users_from_file(
        const std::string& users_file_path, UniqueReq rs) noexcept(false){
    std::ifstream my_file(users_file_path);
    if(my_file){
        auto shared_rec_sys = std::make_shared<RecommenderSystem>(*rs);
        std::vector<RSUser> ret_vec;
        string line;
        istringstream buffer;
        std::getline(my_file, line); // for the line of movies
        std::vector<sp_movie> movie_vec = create_movie_vec(line, rs);
        string tmp;
        rank_map user_map(0, sp_movie_hash, sp_movie_equal);
        while(std::getline(my_file, line)){
            buffer.str(line);
            tmp.clear();
            string username;
            bool first_input = true;
            int i = 0; // iter over movies_vec
            while(std::getline(buffer, tmp, ' ')){
                if(first_input){
                    username = tmp;
                    first_input = false;
                    continue;
                } else{
                    map_helper(user_map, movie_vec[i], tmp);
                }
                i++;
            }
            ret_vec.emplace_back(RSUser(username, user_map, shared_rec_sys));
            user_map.clear();
            buffer.clear();
        }
        return ret_vec;
    }
    throw std::runtime_error("failed to open file-UserLoader");
}