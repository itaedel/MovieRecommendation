#include "RSUser.h"
#include "RecommenderSystem.h"


// constructor
RSUser::RSUser(std::string& user, rank_map& rankings,
               RecSys &rs) {
    _username = user;
    _ranks = rankings;
    _rec_sys = rs;
}

// see documentation
sp_movie RSUser::get_recommendation_by_content() const {
    return _rec_sys->recommend_by_content(*this);
}

// see documentation
sp_movie RSUser::get_recommendation_by_cf(int k) const {
    return _rec_sys->recommend_by_cf(*this, k);
}

// see documentation
void RSUser::add_movie_to_rs(const std::string &name, int year,
                             const std::vector<double> &features,
                             double rate) {
    auto mov = _rec_sys->add_movie(name, year, features);
    _ranks[mov] = rate;
}


// see documentation
double RSUser::get_prediction_score_for_movie(const std::string &name,
                                                int year, int k) const {
    //auto movie = std::make_shared<Movie>(Movie(name, year));
    auto movie = _rec_sys->get_movie(name, year);
    return _rec_sys->predict_movie_score(*this, movie , k);
}


// see documentation
std::ostream& operator<<(std::ostream &os, const RSUser& user){
    using std::endl;
    os << "name: " << user.get_name() << endl;
    os << *(user._rec_sys) << endl;
    return os;
}
