// Datastructures.cc
//
// Student name: Elli Lehtimäki
// Student email: elli.i.lehtimaki@tuni.fi
// Student number: 151309919

#include "datastructures.hh"

#include <random>

#include <cmath>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)
        (rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Write any initialization you need here
}

Datastructures::~Datastructures()
{
    // Write any cleanup you need here
}

// Palauttaa tiedon unsorted_map:n information_of_affiliations_ koosta.
unsigned int Datastructures::get_affiliation_count()
{
    return information_of_affiliations_.size();
}

// Tyhjentää tietorakenteet.
void Datastructures::clear_all()
{
    information_of_affiliations_.clear();
    information_of_publications_.clear();
    names_ordered_.clear();
    distances_ordered_.clear();
    connections_.clear();
}

// Varaa unsorted_map:n information_of_affiliations_ koon verran tilaa ja
// tallentaa vektoriin tiedot affiliaatioista käyttäen algoritmia transform.
std::vector<AffiliationID> Datastructures::get_all_affiliations()
{
    std::vector<AffiliationID> all_affiliations;

    all_affiliations.reserve(information_of_affiliations_.size());

    std::transform(information_of_affiliations_.begin(),
                   information_of_affiliations_.end(),
                   std::back_inserter(all_affiliations),
                   [](const auto &infor){ return infor.first; });

    return all_affiliations;
}

// Etsii affiliaation annetulla id:llä, lisää uuden affiliaatiot annetuilla
// tiedoilla.
bool Datastructures::add_affiliation(AffiliationID id, const Name &name,
                                     Coord xy)
{
    std::unordered_map<AffiliationID, affiliation_info>::iterator it;
    it = information_of_affiliations_.find(id);

    if (it != information_of_affiliations_.end())
    {
        return false;
    }

    affiliation_info new_affiliation;
    new_affiliation.name_ = name;
    new_affiliation.xy_ = xy;
    information_of_affiliations_[id] = new_affiliation;

    // Lisätään järjestettyihin nimiin ja etäisyyksiin uudet tiedot.
    names_ordered_.push_back(id);
    distances_ordered_.push_back(id);

    // Muutetaan muuttujien arvot false, sillä oikea järjestys voinut muuttua
    // lisäyksen takia.
    are_names_ordered_ = false;
    are_distances_ordered_ = false;
    are_connections_correct_ = false;

    return true;
}

// Etsii affiliaation annetulla id:llä, palauttaa affiliaation nimen, jos id
// löytyi, muuten NO_NAME.
Name Datastructures::get_affiliation_name(AffiliationID id)
{
    std::unordered_map<AffiliationID, affiliation_info>::iterator it;
    it = information_of_affiliations_.find(id);

    if (it == information_of_affiliations_.end())
    {
        return NO_NAME;
    }

    return it->second.name_;
}

// Etsii affiliaation annetulla id:llä, palauttaa affiliaation koordinaatit, jos
// id löytyi, muuten NO_COORD.
Coord Datastructures::get_affiliation_coord(AffiliationID id)
{
    std::unordered_map<AffiliationID, affiliation_info>::iterator it;
    it = information_of_affiliations_.find(id);

    if (it == information_of_affiliations_.end())
    {
        return NO_COORD;
    }

    return it->second.xy_;
}

// Tarkistaa are_names_ordered_ arvon, jos true niin vektorissa names_ordered_
// nimet aakkosjärjestyksessä, muuten aakkostetaan. Tallennetaan map:in nimet
// avaimeksi ja affiliaatioiden id:t arvoksi, transformilla ja back_inserter:llä
// tallennetaan vektoriin names_ordered_. Map jo järjestetty avaimen mukaan.
std::vector<AffiliationID> Datastructures::get_affiliations_alphabetically()
{
    if (are_names_ordered_)
    {
        return names_ordered_;
    }

    std::map<Name, AffiliationID> affiliation_names;

    for (const auto& i : information_of_affiliations_)
    {
        affiliation_names[i.second.name_] = i.first;
    }

    names_ordered_.clear();

    std::transform(affiliation_names.begin(), affiliation_names.end(),
                   std::back_inserter(names_ordered_),
                   [](auto &i){ return i.second;});

    are_names_ordered_ = true;

    return names_ordered_;
}

// Tarkistaa are_distances_ordered_ arvon, jos true niin vektorissa
// distances_ordered_ etäisyydet suuruusjärjestyksessä, muuten laitetaan
// suuruusjärjestykseen. Tallennetaan multimap:in etäisyydet avaimeksi ja
// affiliaatioiden id:t arvoksi, transformilla ja back_inserter:llä
// tallennetaan vektoriin distances_ordered_. Map jo järjestetty avaimen mukaan.
std::vector<AffiliationID> Datastructures::
    get_affiliations_distance_increasing()
{
    if (are_distances_ordered_)
    {
        return distances_ordered_;
    }

    // Käytetään multimap, joka sallii samojen arvojen tallentamisen.
    std::multimap<Distance, AffiliationID> affiliation_distances = {};

    for (const auto& i : information_of_affiliations_)
    {
        Distance distance_x;
        distance_x = i.second.xy_.x;

        Distance distance_y;
        distance_y = i.second.xy_.y;

        Distance distance;
        distance = std::sqrt(distance_x*distance_x +
                             distance_y*distance_y);

        affiliation_distances.emplace(distance, i.first);
    }

    distances_ordered_.clear();

    std::transform(affiliation_distances.begin(), affiliation_distances.end(),
                   std::back_inserter(distances_ordered_),
                   [](auto &i){ return i.second;});

    are_distances_ordered_ = true;

    return distances_ordered_;
}

// Samalla koordinaatilla voi olla vain yksi affiliaatio, joten etsitään
// ensimmäinen, joka vastaa annettua koordinaattia.
AffiliationID Datastructures::find_affiliation_with_coord(Coord xy)
{
    auto it = std::find_if(information_of_affiliations_.begin(),
                           information_of_affiliations_.end(),
                           [&xy](const auto &affiliation)
                           { return affiliation.second.xy_ == xy; });

    if (it != information_of_affiliations_.end())
    {
        return it->first;
    }

    return NO_AFFILIATION;
}

// Etsitään annettu affiliaatio id, jos affiliaatio löytyi, muutetaan sen
// koordinaatti ja palautetaan true, muuten false. Jos muutos onnistui muutetaan
// muuttuja are_distances_ordered_ falseksi, sillä koordinaatti on nyt eri,
// jolloin etäisyyksien järjestys voi olla väärä.
bool Datastructures::change_affiliation_coord(AffiliationID id, Coord xy)
{
    std::unordered_map<AffiliationID, affiliation_info>::iterator it;
    it = information_of_affiliations_.find(id);

    if (it == information_of_affiliations_.end())
    {
        return false;
    }

    it->second.xy_ = xy;

    are_distances_ordered_ = false;

    return true;
}

// Lisätään julkaisu annetulla id:llä ja liitetään ne vektoriin affiliations
// annettuihin affiliaatioihin. Jos julkaisun id:llä löytyy jo julkaisu,
// palautetaan false, sillä id on uniikki.
bool Datastructures::add_publication(PublicationID id, const Name &name,
                                     Year year, const std::vector<AffiliationID>
                                         &affiliations)
{
    std::unordered_map<PublicationID, publication_info>::iterator
        publications_it;
    publications_it = information_of_publications_.find(id);

    if (publications_it != information_of_publications_.end())
    {
        return false;
    }

    publication_info new_publication;
    new_publication.title_ = name;
    new_publication.year_ = year;

    information_of_publications_.insert({id, new_publication});

    std::unordered_map<AffiliationID, affiliation_info>::iterator
        affiliations_it;
    affiliations_it = information_of_affiliations_.begin();

    // Käydään vektori affiliations läpi, lisätään julkaisu jokaiseen
    // affiliaatioon.
    for (const auto& affiliation : affiliations)
    {
        auto affiliations_it = information_of_affiliations_.find(affiliation);

        if (affiliations_it != information_of_affiliations_.end())
        {
            affiliations_it->second.publications_.push_back(id);
        }
    }

    are_connections_correct_ = false;

    return true;
}

// Varaa unsorted_map:n information_of_publications_ koon verran tilaa ja
// tallentaa vektoriin tiedot julkaisuista käyttäen algoritmia transform.
std::vector<PublicationID> Datastructures::all_publications()
{
    std::vector<PublicationID> publications;
    publications.reserve(information_of_publications_.size());

    std::transform(information_of_publications_.begin(),
                   information_of_publications_.end(),
                   std::back_inserter(publications),
                   [](const auto &infor){ return infor.first; });

    return publications;
}

// Etsii julkaisun annetulla id:llä, palauttaa julkaisun otsikon, jos id löytyi,
// muuten NO_NAME.
Name Datastructures::get_publication_name(PublicationID id)
{
    std::unordered_map<PublicationID, publication_info>::iterator it;
    it = information_of_publications_.find(id);

    if (it == information_of_publications_.end())
    {
        return NO_NAME;
    }

    return it->second.title_;
}

// Etsii julkaisun annetulla id:llä, palauttaa julkaisun vuoden, jos id löytyi,
// muuten NO_YEAR.
Year Datastructures::get_publication_year(PublicationID id)
{
    std::unordered_map<PublicationID, publication_info>::iterator it;
    it = information_of_publications_.find(id);

    if (it == information_of_publications_.end())
    {
        return NO_YEAR;
    }

    return it->second.year_;
}

// Etsii julkaisun annetulla id:llä ja palauttaa vektorin, joka sisältää
// kyseisen julkaisun affiliaatiot.
std::vector<AffiliationID> Datastructures::get_affiliations(PublicationID id)
{
    std::vector<AffiliationID> affiliations;

    std::unordered_map<PublicationID, publication_info>::iterator pubit;
    pubit = information_of_publications_.find(id);

    if (pubit != information_of_publications_.end())
    {
        return pubit->second.affiliations_;
    }

    return affiliations;
}

// Parametri parentid ottaa viitteekseen parametrin id. Parametrin parentid:n
// vektoriin, joka sisältää viitteet, lisätään id ja id:n viittaukseksi lisätään
// parentid.
bool Datastructures::add_reference(PublicationID id, PublicationID parentid)
{
    std::unordered_map<PublicationID, publication_info>::iterator
        publications_parent_it;
    publications_parent_it = information_of_publications_.find(parentid);

    std::unordered_map<PublicationID, publication_info>::iterator
        publications_it;
    publications_it = information_of_publications_.find(id);

    if ((publications_parent_it == information_of_publications_.end()) ||
        (publications_it == information_of_publications_.end()))
    {
        return false;
    }

    information_of_publications_[parentid].reference_children_.push_back(id);

    information_of_publications_[id].referenced_by_parent = parentid;

    are_connections_correct_ = false;

    return true;
}

// Etsii julkaisun annetulla id:llä ja palauttaa vektorin, joka sisältää sen
// viittaukset. Jos id:llä ei löydy julkaisua, palautetaan tyhjä vektori.
std::vector<PublicationID> Datastructures::get_direct_references
    (PublicationID id)
{
    std::vector<PublicationID> all_publication;

    std::unordered_map<PublicationID, publication_info>::iterator
        publications_it;
    publications_it = information_of_publications_.find(id);

    if (publications_it != information_of_publications_.end())
    {
        return publications_it->second.reference_children_;
    }

    return all_publication;
}

// Liittää annetulla affiliationid:llä affiliaation annettuun publicationid:n
// julkaisuun. Jos jompaa kumpaa tai kumpaakaan ei löydy, palautetaan false.
bool Datastructures::add_affiliation_to_publication(AffiliationID affiliationid,
                                                    PublicationID publicationid)
{
    std::unordered_map<AffiliationID, affiliation_info>::iterator
        affiliations_it;
    affiliations_it = information_of_affiliations_.find(affiliationid);

    std::unordered_map<PublicationID, publication_info>::iterator
        publications_it;
    publications_it = information_of_publications_.find(publicationid);

    if ((affiliations_it == information_of_affiliations_.end()) ||
        (publications_it == information_of_publications_.end()))
    {
        return false;
    }

    information_of_affiliations_[affiliationid].publications_.
        push_back(publicationid);
    information_of_publications_[publicationid].affiliations_.
        push_back(affiliationid);

    are_connections_correct_ = false;

    return true;
}

// Etsii affiliaation annetulla id:llä ja palauttaa vektorin, joka sisältää
// kyseisen affiliaation julkaisut.
std::vector<PublicationID> Datastructures::get_publications(AffiliationID id)
{
    std::vector<PublicationID> all_publications;

    std::unordered_map<AffiliationID, affiliation_info>::iterator it;
    it = information_of_affiliations_.find(id);

    if (it != information_of_affiliations_.end())
    {
        return it->second.publications_;
    }
    all_publications.push_back(NO_PUBLICATION);

    return all_publications;
}

// Etsii julkaisun annetulla id:llä ja palauttaa sen vanhemman, jos sellainen
// on, muuten NO_PUBLICATION.
PublicationID Datastructures::get_parent(PublicationID id)
{
    std::unordered_map<PublicationID, publication_info>::iterator it;
    it = information_of_publications_.find(id);

    if (it == information_of_publications_.end())
    {

        return NO_PUBLICATION;
    }

    return it->second.referenced_by_parent;
}

// Etsii affiliaation annetulla id:llä ja laittaa vektoriin kaikki ne
// julkaisuvuodet sekä julkaisun id:t, joihin pätee annettu ehto.
std::vector<std::pair<Year, PublicationID>> Datastructures::
    get_publications_after(AffiliationID affiliationid, Year year)
{
    std::vector<std::pair<Year, PublicationID>> year_and_publication_id;

    std::unordered_map<AffiliationID, affiliation_info>::iterator it;
    it = information_of_affiliations_.find(affiliationid);

    if (it == information_of_affiliations_.end())
    {
        return year_and_publication_id;
    }

    for (unsigned long long i = 0; i<it->second.publications_.size(); i++)
    {
        std::unordered_map<PublicationID, publication_info>::iterator pubit;
        pubit = information_of_publications_.find(it->second.publications_[i]);

        if ((pubit != information_of_publications_.end()) &&
            (pubit->second.year_ >= year))
        {
            year_and_publication_id.
                push_back({pubit->second.year_, pubit->first});
        }
    }

    return year_and_publication_id;
}

// Etsii julkaisun annetulla id:llä, jos julkaisua ei löydy, lisätään vektoriin
// all_publications NO_PUBLICATION. Julkaisun löytyessä lisätään vanhempi
// vektoriin niin kauan, kunnes jonkin julkaisun vanhempi on NO_PUBLICATION.
std::vector<PublicationID> Datastructures::
    get_referenced_by_chain(PublicationID id)
{
    std::vector<PublicationID> all_publications;

    std::unordered_map<PublicationID, publication_info>::iterator it;
    it = information_of_publications_.find(id);

    if (it == information_of_publications_.end())
    {
        all_publications.push_back(NO_PUBLICATION);
        return all_publications;
    }

    while (true)
    {
        it = information_of_publications_.find(id);

        id = it->second.referenced_by_parent;
        if (id == NO_PUBLICATION)
        {
            break;
        }
        all_publications.push_back(id);
    }

    return all_publications;
}

// Hakee rekursiivisesti kaikki suorat ja epäsuorat viittaukset, lisää ne
// vektoriin ja palauttaa vektorin.
std::vector<PublicationID> Datastructures::get_all_references(PublicationID id)
{
    std::unordered_map<PublicationID, publication_info>::iterator it;
    it = information_of_publications_.find(id);

    std::vector<PublicationID> all_references;

    if (it != information_of_publications_.end())
    {
        publication_info pub_info = information_of_publications_.at(id);
        for (auto i : pub_info.reference_children_)
        {
            all_references.push_back(i);
            auto recursive_references = get_all_references(i);
            all_references.insert(all_references.end(),
                                  recursive_references.begin(),
                                  recursive_references.end());
        }
    }

    if (it == information_of_publications_.end())
    {
        all_references.push_back(NO_PUBLICATION);
    }

    return all_references;
}

// Vähentää jokaisesta affiliaation koordinaateista annetun koordinaatin ja
// tallentaa multimap:iin etäisyyden ja affiliaation id:n. Multimap järjestää
// affiliaatiot koordinaattien mukaan kasvavaan järjestykseen. Affiliaatioiden
// id:t laitetaan vektoriin, joka palautetaan.
std::vector<AffiliationID> Datastructures::get_affiliations_closest_to(Coord xy)
{
    std::vector<AffiliationID> closest;
    closest.reserve(3);

    std::multimap<Distance, AffiliationID> affiliation_distances = {};

    int counter = 0;

    Distance distance_given_x;
    distance_given_x = xy.x;

    Distance distance_given_y;
    distance_given_y = xy.y;

    for (const auto& i : information_of_affiliations_)
    {
        Distance distance_x;
        distance_x = i.second.xy_.x - distance_given_x;

        Distance distance_y;
        distance_y = i.second.xy_.y - distance_given_y;

        Distance distance;
        distance = std::sqrt(distance_x*distance_x +
                             distance_y*distance_y);

        affiliation_distances.emplace(distance, i.first);
    }

    for (const auto& i : affiliation_distances)
    {
        if (counter < 3)
        {
            closest.push_back(i.second);
            counter++;
        }
        else
        {
            break;
        }
    }

    return closest;
}

// Poistaa annettua id:tä vastaavan affiliaation. Käy läpi kaikki tietorakenteet
// ja poistaa affiliaation jokaisesta tietorakenteesta.
bool Datastructures::remove_affiliation(AffiliationID id)
{
    std::unordered_map<AffiliationID, affiliation_info>::iterator it;
    it = information_of_affiliations_.find(id);

    if (it != information_of_affiliations_.end())
    {
        information_of_affiliations_.erase(it);
        names_ordered_.erase(std::remove(names_ordered_.begin(),
                                         names_ordered_.end(), id),
                             names_ordered_.end());
        distances_ordered_.erase(std::remove(distances_ordered_.begin(),
                                             distances_ordered_.end(), id),
                                 distances_ordered_.end());
        for (auto &i : information_of_publications_)
        {
            i.second.affiliations_.erase(std::remove
                                         (i.second.affiliations_.begin(),
                                          i.second.affiliations_.end(), id),
                                         i.second.affiliations_.end());
        }
        return true;
    }
    are_connections_correct_ = false;
    return false;
}

// Etsii lähimmän vanhemman hakemalla ensin molempien julkaisujen koko
// vanhempiketjun. Etsitään ensimmäinen vanhempi, joka löytyy molemmista
// vektoreista, tallennetaan se muuttujaan ja palautetaan muuttuja.
PublicationID Datastructures::get_closest_common_parent(PublicationID id1,
                                                        PublicationID id2)
{
    std::vector<PublicationID> id1_publications;
    std::vector<PublicationID> id2_publications;

    PublicationID closest_common_parent = NO_PUBLICATION;

    id1_publications = get_referenced_by_chain(id1);
    id2_publications = get_referenced_by_chain(id2);

    auto it = std::find_first_of(id1_publications.begin(),
                                 id1_publications.end(),
                                 id2_publications.begin(),
                                 id2_publications.end());

    if (it != id1_publications.end())
    {
        closest_common_parent = *it;
    }

    return closest_common_parent;
}

// Poistaa annettua id:tä vastaavan julkaisun. Käy läpi kaikki tietorakenteet
// ja poistaa julkaisun jokaisesta tietorakenteesta.
bool Datastructures::remove_publication(PublicationID publicationid)
{
    std::unordered_map<PublicationID, publication_info>::iterator it;
    it = information_of_publications_.find(publicationid);

    if (it != information_of_publications_.end())
    {
        information_of_publications_.erase(it);
        for (auto &i : information_of_affiliations_)
        {
            i.second.publications_.erase(std::remove
                                         (i.second.publications_.begin(),
                                          i.second.publications_.end(),
                                          publicationid),
                                         i.second.publications_.end());
        }
        for (auto &i : information_of_publications_)
        {
            i.second.reference_children_.erase(std::remove
                                               (i.second.reference_children_.
                                                           begin(),
                                                i.second.reference_children_.
                                                           end(),
                                                publicationid),
                                               i.second.reference_children_.
                                               end());
            if (i.second.referenced_by_parent == publicationid)
            {
                i.second.referenced_by_parent = NO_PUBLICATION;
            }
        }
        return true;
    }

    are_connections_correct_ = false;

    return false;
}

// Etsii kaikki annetun affiliaation yhteydet, lisää ne vektoriin ilman
// duplikaatteja.
std::vector<Connection> Datastructures::get_connected_affiliations
    (AffiliationID id)
{
    std::unordered_map<AffiliationID, affiliation_info>::iterator it_begin;
    it_begin = information_of_affiliations_.find(id);
    std::vector<Connection> connected_affiliations;
    std::set<PublicationID> id1_publications;

    // Jos annettua affiliaatiota ei ole olemassa tai sillä ei ole julkaisuja,
    // palautetaan tyhjä vektori.
    if ((it_begin == information_of_affiliations_.end()) ||
        (it_begin->second.publications_.empty()))
    {
        return connected_affiliations;
    }

    std::set<PublicationID> given_aff_pub
        (it_begin->second.publications_.begin(),
                                          it_begin->second.publications_.end());

    for (auto &i : information_of_affiliations_)
    {
        std::set<PublicationID> current_aff_pub(i.second.publications_.begin(),
                                                i.second.publications_.end());
        int weight = 0;
        AffiliationID connect_to = i.first;

        id1_publications.clear();

        // Tallennetaan kaikki yhteiset julkaisut.
        std::set_intersection(given_aff_pub.begin(),
                              given_aff_pub.end(),
                              current_aff_pub.begin(),
                              current_aff_pub.end(),
                              std::inserter(id1_publications,
                                            id1_publications.begin()));

        if ((!id1_publications.empty()) && (connect_to != id))
        {
            weight = id1_publications.size();
            connected_affiliations.push_back({id, connect_to, weight});
        }
    }

    return connected_affiliations;
}

// Hakee kaikkien affiliaatioiden yhteydet toisiinsa ja tallentaa sen
// tietorakenteeseen. Jos muuttuja are_connections_correct_ on true, on kaikki
// yhteydet jo haettu, joten sitä ei tarvitse tehdä uudestaan, vaan palautetaan
// vektori, joka sisältää kaikki yhteydet.
std::vector<Connection> Datastructures::get_all_connections()
{
    if (are_connections_correct_)
    {
        return connections_;
    }

    std::map<AffiliationID, std::set<AffiliationID>> aff_map;
    std::set<AffiliationID> aff_set;

    std::transform(information_of_affiliations_.begin(),
                   information_of_affiliations_.end(),
                   std::inserter(aff_set, aff_set.begin()),
                   [](const auto& pair) { return pair.first; });


    connections_.clear();
    connections_.reserve(information_of_affiliations_.size());

    for (const auto &j : aff_set)
    {
        AffiliationID id = j;

        std::unordered_map<AffiliationID, affiliation_info>::iterator it_begin;
        it_begin = information_of_affiliations_.find(id);

        std::set<PublicationID> given_aff_pub
            (it_begin->second.publications_.begin(),
             it_begin->second.publications_.end());

        std::unordered_map<AffiliationID, int> un_map;
        std::set<PublicationID> id1_publications;

        for (auto &i : information_of_affiliations_)
        {
            std::set<PublicationID> current_aff_pub(i.second.publications_.
                                                    begin(),
                                                    i.second.publications_.
                                                    end());
            int weight = 0;
            AffiliationID connect_to = i.first;

            id1_publications.clear();

            std::set_intersection(given_aff_pub.begin(),
                                  given_aff_pub.end(),
                                  current_aff_pub.begin(),
                                  current_aff_pub.end(),
                                  std::inserter(id1_publications,
                                                id1_publications.begin()));

            if ((!id1_publications.empty()) && (connect_to != id))
            {
                // Etsitään duplikaattiarvot.
                auto duplicate_check = std::find_if(connections_.begin(),
                                                    connections_.end(),
                                                    [id, connect_to]
                                                    (const Connection& conn)
                                                    {
                                                        return (conn.aff1 == id
                                                && conn.aff2 == connect_to) ||
                                                    (conn.aff1 == connect_to &&
                                                            conn.aff2 == id);
                                                    });

                // Jos duplikaatteja ei ole, voidaan uusi yhteys lisätä.
                if (duplicate_check == connections_.end())
                {
                    weight = id1_publications.size();
                    connections_.push_back({id, connect_to, weight});
                }
            }
        }
    }

    are_connections_correct_ = true;

    return connections_;
}

// Hakee minkä tahansa yhteyden annettujen affiliaatioiden välillä
// dfs-algoritmin avulla.
Path Datastructures::get_any_path(AffiliationID source, AffiliationID target)
{
    std::vector<Connection> conn_aff;

    if (connections_.empty() || !are_connections_correct_)
    {
        get_all_connections();
    }

    std::vector<Connection> connection_between_source_and_target;
    std::unordered_set<AffiliationID> visited;
    std::unordered_map<AffiliationID, AffiliationID> previous;
    std::stack<AffiliationID> stack;

    visited.insert(source);
    previous.insert({source, NO_AFFILIATION});
    connection_between_source_and_target.reserve(connections_.size());

    stack.push(source);
    bool isPathFound = false;

    while (!stack.empty())
    {
        AffiliationID current = stack.top();
        stack.pop();

        for (const auto& connection : connections_)
        {
            if ((connection.aff1 == current && visited.find(connection.aff2) ==
                                                   visited.end()) ||
                (connection.aff2 == current && visited.find(connection.aff1) ==
                                                   visited.end()))
            {
                AffiliationID next_affiliation = (connection.aff1 == current) ?
                                            connection.aff2 : connection.aff1;
                stack.push(next_affiliation);

                auto it_v = visited.find(current);
                if (it_v == visited.end())
                {
                    visited.insert(current);
                }

                auto it = previous.find(next_affiliation);
                if (it != previous.end())
                {
                    previous.erase(it);
                }
                previous.insert({next_affiliation, current});

                if (next_affiliation == target)
                {
                    isPathFound = true;
                    break;
                }
            }
        }
    }

    if (!isPathFound)
    {
        return connection_between_source_and_target;
    }

    AffiliationID previous_affiliation = target;
    AffiliationID current_affiliation = previous.at(target);

    while (current_affiliation != NO_AFFILIATION)
    {
        auto duplicateCheck = std::find_if(connections_.begin(),
                                           connections_.end(),
                                    [current_affiliation, previous_affiliation]
                                           (const Connection& conn)
                                    {
                                    return (conn.aff1 == current_affiliation &&
                                        conn.aff2 == previous_affiliation) ||
                                        (conn.aff1 == previous_affiliation &&
                                            conn.aff2 == current_affiliation);
                                    });

        connection_between_source_and_target.push_back({current_affiliation,
                                                        previous_affiliation,
                                                    duplicateCheck->weight});
        previous_affiliation = current_affiliation;
        current_affiliation = previous.at(current_affiliation);
    }

    std::reverse(connection_between_source_and_target.begin(),
                 connection_between_source_and_target.end());

    return connection_between_source_and_target;
}

Path Datastructures::get_path_with_least_affiliations(AffiliationID /*source*/, AffiliationID /*target*/)
{
    // Replace the line below with your implementation
    throw NotImplemented("get_path_with_least_affiliations()");
}

Path Datastructures::get_path_of_least_friction(AffiliationID /*source*/, AffiliationID /*target*/)
{
    // Replace the line below with your implementation
    throw NotImplemented("get_path_of_least_friction()");
}

PathWithDist Datastructures::get_shortest_path(AffiliationID /*source*/, AffiliationID /*target*/)
{
    // Replace the line below with your implementation
    throw NotImplemented("get_shortest_path()");
}



