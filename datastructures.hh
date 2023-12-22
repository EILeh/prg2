// Datastructures.hh
//
// Student name: Elli Lehtimäki
// Student email: elli.i.lehtimaki@tuni.fi
// Student number: 151309919

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <exception>
#include <unordered_map>
#include <map>
#include <set>
#include <stack>
#include <unordered_set>

// Types for IDs
using AffiliationID = std::string;
using PublicationID = unsigned long long int;
using Name = std::string;
using Year = unsigned short int;
using Weight = int;
struct Connection;
// Type for a distance (in arbitrary units)
using Distance = int;

using Path = std::vector<Connection>;
using PathWithDist = std::vector<std::pair<Connection,Distance>>;

// Return values for cases where required thing was not found
AffiliationID const NO_AFFILIATION = "---";
PublicationID const NO_PUBLICATION = -1;
Name const NO_NAME = "!NO_NAME!";
Year const NO_YEAR = -1;
Weight const NO_WEIGHT = -1;

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};


// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

struct Connection
{
    AffiliationID aff1 = NO_AFFILIATION;
    AffiliationID aff2 = NO_AFFILIATION;
    Weight weight = NO_WEIGHT;
    bool operator==(const Connection& c1) const{
        return (aff1==c1.aff1) && (aff2==c1.aff2) && (weight==c1.weight);
    }
};
const Connection NO_CONNECTION{NO_AFFILIATION,NO_AFFILIATION,NO_WEIGHT};


// Return value for cases where Distance is unknown
Distance const NO_DISTANCE = NO_VALUE;

// This exception class is there just so that the user interface can notify
// about operations which are not (yet) implemented
class NotImplemented : public std::exception
{
public:
    NotImplemented() : msg_{} {}
    explicit NotImplemented(std::string const& msg) : msg_{msg + " not implemented"} {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
private:
    std::string msg_;
};

// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: size on vakioaikainen
    unsigned int get_affiliation_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: clear on lineaarinen
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: reserve ja transform ovat lineaarisia
    std::vector<AffiliationID> get_all_affiliations();

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen, lisäys
    // lineaarinen unsorted_map:ssa, push_back on amortisoitu
    bool add_affiliation(AffiliationID id, Name const& name, Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    Name get_affiliation_name(AffiliationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    Coord get_affiliation_coord(AffiliationID id);


    // We recommend you implement the operations below only after implementing
    // the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: for, clear ja transform lineaarisia, lisäys
    // unsorted_map:iin vakioaikainen
    std::vector<AffiliationID> get_affiliations_alphabetically();

    // Estimate of performance: O(n)
    // Short rationale for estimate: for, clear ja transform lineaarisia, lisäys
    // unsorted_map:iin vakioaikainen
    std::vector<AffiliationID> get_affiliations_distance_increasing();

    // Estimate of performance: O(n)
    // Short rationale for estimate: find_if on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    AffiliationID find_affiliation_with_coord(Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    bool change_affiliation_coord(AffiliationID id, Coord newcoord);


    // We recommend you implement the operations below only after implementing
    // the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen, for
    // lineaarinen, insert unordered_map:in vakioaikainen ja tehdään vain kerran
    bool add_publication(PublicationID id, Name const& name, Year year, const
                         std::vector<AffiliationID> & affiliations);

    // Estimate of performance: O(n)
    // Short rationale for estimate: reserve ja transform ovat lineaarisia
    std::vector<PublicationID> all_publications();

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    Name get_publication_name(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    Year get_publication_year(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    std::vector<AffiliationID> get_affiliations(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen,
    // push_back amortisoitu, lisäys unsorted_map:iin vakioaikainen
    bool add_reference(PublicationID id, PublicationID parentid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    std::vector<PublicationID> get_direct_references(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen,
    // push_back amortisoitu, lisäsys unsorted_map:iin vakioaikainen
    bool add_affiliation_to_publication(AffiliationID affiliationid,
                                        PublicationID publicationid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    std::vector<PublicationID> get_publications(AffiliationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen
    PublicationID get_parent(PublicationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen,
    // for lineaarinen, push_back amortisoitu
    std::vector<std::pair<Year, PublicationID>> get_publications_after
        (AffiliationID affiliationid, Year year);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find on keskimäärin vakioaikainen
    // unsorted_map:ssa, kuitenkin huonoimmassa tapauksessa lineaarinen,
    // while lineaarinen, push_back amortisoitu
    std::vector<PublicationID> get_referenced_by_chain(PublicationID id);


    // Non-compulsory operations

    // Estimate of performance: O(nlogn)
    // Short rationale for estimate: find on keskimäärin vakioaikainen,
    // kuitenkin huonoimmassa tapauksessa lineaarinen, for lineaarinen,
    // push_back amortisoitu, insert nlogn ja at logn
    std::vector<PublicationID> get_all_references(PublicationID id);

    // Estimate of performance: O(nlogn)
    // Short rationale for estimate: for lineaarinen, push_back amortisoitu,
    // emplace logn
    std::vector<AffiliationID> get_affiliations_closest_to(Coord xy);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find, erase, remove ja for lineaarisia
    bool remove_affiliation(AffiliationID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: funktio get_referenced_by_chain ja
    // algoritmi find_first_of lineaarisia
    PublicationID get_closest_common_parent(PublicationID id1, PublicationID
                                                                   id2);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find, erase, remove ja for lineaarisia
    bool remove_publication(PublicationID publicationid);

    // PRG 2 functions:

    // Estimate of performance: O(n)
    // Short rationale for estimate: empty ja size vakioaikaisia, for, clear,
    // set_intersection ja find lineaarisia, push_back amortisoitu.
    std::vector<Connection> get_connected_affiliations(AffiliationID id);

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: empty ja size vakioaikaisia, for, clear,
    // set_intersection, transform, reserve, find_if ja find lineaarisia,
    // push_back amortisoitu. Kaksi sisäkkäistä for looppia, josta n^2.
    std::vector<Connection> get_all_connections();

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: empty, pop, top ja size vakioaikaisia, for,
    // reserve, while, insert, find_if ja find lineaarisia, push_back ja erase
    // amortisoitu, erase pahimmillaan nlogn. While loopin sisällä for loop,
    // josta n^2.
    Path get_any_path(AffiliationID source, AffiliationID target);

    // PRG2 optional functions

    // Estimate of performance:
    // Short rationale for estimate:
    Path get_path_with_least_affiliations(AffiliationID source, AffiliationID
                                                                    target);

    // Estimate of performance:
    // Short rationale for estimate:
    Path get_path_of_least_friction(AffiliationID source, AffiliationID target);

    // Estimate of performance:
    // Short rationale for estimate:
    PathWithDist get_shortest_path(AffiliationID source, AffiliationID target);


private:
    struct affiliation_info {
        Name name_;
        Coord xy_;
        std::vector<PublicationID> publications_;
    };

    struct publication_info {
        Name title_;
        Year year_;
        std::vector<AffiliationID> affiliations_;
        std::vector<PublicationID> reference_children_;
        PublicationID referenced_by_parent = NO_PUBLICATION;
    };

    std::unordered_map<AffiliationID, affiliation_info>
        information_of_affiliations_;

    std::unordered_map<PublicationID, publication_info>
        information_of_publications_;

    bool are_names_ordered_ = false;

    bool are_distances_ordered_ = false;

    bool are_connections_correct_ = false;

    std::vector<AffiliationID> names_ordered_;

    std::vector<AffiliationID> distances_ordered_;

    std::vector<Connection> connections_;

};

#endif // DATASTRUCTURES_HH
