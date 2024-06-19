#ifndef SCHOOL_SOLUTION_USERFACTORY_H
#define SCHOOL_SOLUTION_USERFACTORY_H
#include "RSUser.h"
#include "RecommenderSystem.h"

#include <sstream>
#include <fstream>

typedef std::unique_ptr<RecommenderSystem> UniqueReq;
using std::string;

class RSUsersLoader
{
private:
    static void map_helper(rank_map& rm, sp_movie& sp, string& tmp);
    static std::vector<sp_movie> create_movie_vec(string& line, UniqueReq& rs);
public:
    RSUsersLoader() = delete;
    /**
     *
     * loads users by the given format with their movie's ranks
     * @param users_file_path a path to the file of the users and their
     * movie ranks
     * @param rs RecommendingSystem for the Users
     * @return vector of the users created according to the file
     */
    static std::vector<RSUser> create_users_from_file(
            const std::string& users_file_path, UniqueReq rs) noexcept(false);

};


#endif
