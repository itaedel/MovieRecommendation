#include "RecommenderSystem.h"
bool mycomp(const sp_movie& sp1, const sp_movie& sp2){
        return (*sp1) < (*sp2);
    }


bool compare_movie_rank(const std::pair<sp_movie, double>& sp1,
                        const std::pair<sp_movie, double>& sp2){
    return sp1.second > sp2.second;
}


// returns movie
sp_movie RecommenderSystem::get_movie(const std::string &name,
                                      int year) const {
    for(const auto& cur: _database){
        if(cur.first->get_name() == name && cur.first->get_year() == year){
            return cur.first;
        }
    }
    return nullptr;
}


// adds movie
sp_movie RecommenderSystem::add_movie(const std::string &name, int year,
                                      const std::vector<double> &features) {
    auto mov = std::make_shared<Movie>(Movie(name, year));
    _database[mov] = features;
    return mov;
}


// multiply two vectors
double vec_times_vec(const std::vector<double>& vec1,
                     const std::vector<double>& vec2){
    double sum = 0;
    for(std::size_t i = 0; i < vec1.size(); i++){
        sum += vec1[i] * vec2[i];
    }
    return sum;
}


//calc norm of vec as specified
double RecommenderSystem::norm_of_vec(const std::vector<double>& vec1,
                   const std::vector<double>& vec2) const{
    double sum = vec_times_vec(vec1, vec2);
    sum /= (sqrt(vec_times_vec(vec1, vec1)) *
            sqrt(vec_times_vec(vec2, vec2)));
    return sum;
}


//for each unranked movie we go over all the ranked movies
// and get the k-best movies to calculate the optimal score of it
// then we will choose in place the highest score unranked movie
double RecommenderSystem::predict_movie_score(const RSUser &user,
                                              const sp_movie &movie, int k){
    rank_map ranked_movies = user.get_ranks(); // ranked
    std::vector<std::pair<sp_movie, double>> k_similar_movies;
    for(const auto& cur: ranked_movies){
        if(cur.second == 0){
            continue;
        }
        k_similar_movies.emplace_back(std::make_pair(
                cur.first,norm_of_vec(
                        _database[movie],_database[cur.first])));
    }
    std::sort(k_similar_movies.begin(), k_similar_movies.end(),
              compare_movie_rank);
    double numerator = 0, denominator = 0;
    double smj = 0, rxj = 0;
    for(int i = 0; i < k; i++){
        smj = norm_of_vec( _database[movie],
                           _database[k_similar_movies[i].first]);//similar
        rxj = ranked_movies[k_similar_movies[i].first]; // ranking
        numerator += smj * rxj;
        denominator += smj;
    }
    return numerator / denominator;
}


// see documentation
sp_movie RecommenderSystem::recommend_by_cf(const RSUser &user, int k) {
    std::vector<sp_movie> unranked_movies; // unranked movies
    rank_map ranked_movies = user.get_ranks(); // ranked movies
    // now making sure we get all unranked movies
    std::vector<std::pair<sp_movie, double>> similar_movies;
    for(const auto& cur: _database){
        if(ranked_movies.count(cur.first) == 0){
            unranked_movies.push_back(cur.first);
        }
    }
    sp_movie best_match;
    double best_score = 0, cur_score = 0;
    for(const auto& cur: unranked_movies) {
        cur_score = predict_movie_score(user, cur, k);
        if(cur_score > best_score){
            best_match = cur;
            best_score = cur_score;
        }
    }
    return best_match;
}


// see documentation
sp_movie RecommenderSystem::recommend_by_content(const RSUser &user) {
    rank_map user_rank = user.get_ranks();
    std::vector<double> pref_vec;
    std::vector<std::string> not_ranked;
    double avg = 0;
    int num_of_ranked = 0;
    for(const auto& cur: user_rank){ // calc avg
        //total_movies++;
        if(cur.second != 0){
            num_of_ranked++;
            avg += cur.second;
        }
    }
    avg /= num_of_ranked;
    for(auto& cur: user_rank){ // norm the rankings
        if(cur.second != 0){
            cur.second -= avg;
        } else{
            not_ranked.push_back(cur.first->get_name());
        }
    }
    for(size_t i = 0; i < (*_database.begin()).second.size(); i++){
        pref_vec.push_back(0);
    }
    for(const auto& cur: user_rank){
        if(cur.second == 0){ continue; }

        for(size_t i = 0; i < pref_vec.size(); i++){
            pref_vec[i] += cur.second * _database[cur.first][i];
        }
    }
    double max_norm = 0, cur_norm;
    sp_movie ret;
    for(const auto& cur: _database){
        if(std::find(not_ranked.begin(), not_ranked.end(),
                     cur.first->get_name()) == not_ranked.end()){
            continue;
        }
        cur_norm = norm_of_vec(pref_vec, cur.second);
        if(cur_norm > max_norm){
            ret = cur.first;
            max_norm = cur_norm;
        }
    }
    return ret;
}


// see documentation
std::ostream& operator<<(std::ostream& os, const RecommenderSystem& rs){
    using std::endl;
    for(const auto& cur: rs._database){
        os << *(cur.first);
    }
    return os;
}