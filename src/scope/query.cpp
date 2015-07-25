#include <boost/algorithm/string/trim.hpp>

#include <scope/localization.h>
#include <scope/query.h>

#include <unity/scopes/Annotation.h>
#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/CategoryRenderer.h>
#include <unity/scopes/QueryBase.h>
#include <unity/scopes/SearchReply.h>

#include <iomanip>
#include <sstream>

namespace sc = unity::scopes;
namespace alg = boost::algorithm;

using namespace std;
using namespace api;
using namespace scope;


/**
 * Define the layout for the forecast results
 *
 * The icon size is small, and ask for the card layout
 * itself to be horizontal. I.e. the text will be placed
 * next to the image.
 */

const static string INFOS_TEMPLATE =
    R"(
        {
            "schema-version": 1,
            "template": {
                "category-layout": "grid",
                "card-layout": "vertical-journal",
                "card-size": "large",
                "overlay": true

            },
            "components": {
                "mascot": "icon",
                "title": "title",
                "art" : {
                    "field": "art",
                    "aspect-ratio": 2.5
                },
                "subtitle": "artist",
                "emblem": "youtube"
            }
        }
    )";


const static string RESULTS_TEMPLATE =
    R"(
        {
            "schema-version": 1,
            "template": {
                "category-layout": "grid",
                "card-layout": "vertical-journal",
                "card-size": "medium",
                "overlay": true

            },
            "components": {
                "mascot": "icon",
                "title": "title",
                "art" : {
                    "field": "art",
                    "aspect-ratio": ".95"
                },
                "subtitle": "artist",
                "emblem": "youtube"
            }
        }
    )";

Query::Query(const sc::CannedQuery &query, const sc::SearchMetadata &metadata,
             Config::Ptr config) :
    sc::SearchQueryBase(query, metadata), client_(config) {
}

void Query::cancelled() {
    client_.cancel();
}

///
/// \brief Query::run
/// \param reply
///
void Query::run(sc::SearchReplyProxy const& reply) {
    //sleep(2);
    try {
        // Start by getting information about the query
        const sc::CannedQuery &query(sc::SearchQueryBase::query());
       // sleep(2);
        // Trim the query string of whitespace
        string query_string = alg::trim_copy(query.query_string());



        ///
        /// \brief infoslist
        ///
        Client::InfoRes infoslist;
        if (query_string.empty()) {
            // If the string is empty, provide a specific one
            infoslist = client_.infos("ratatat");
        } else {
            // otherwise, use the query string

            //////////tenitive sleep hack fixxxxxxxxxxxxxxxxxxxx
            //sleep(2);
            infoslist = client_.infos(query_string);
        }

        // Register a category for infos
        auto infos_cat = reply->register_category("infos", "Information", "logo.png",
            sc::CategoryRenderer(INFOS_TEMPLATE));

        // register_category(arbitrary category id, header title, header icon, template)
        // In this case, since this is the only category used by our scope,
        // it doesn’t need to display a header title, we leave it as a blank string.


        for (const auto &info : infoslist.infos) {
        // Iterate over the trackslist
            sc::CategorisedResult res(infos_cat);


            // We must have a URI
            res.set_uri(info.wUrl);

            // We also need the name
            res.set_title(info.name);

            // Set the rest of the attributes, art, artist, etc
            //res.set_art("http://www.google.com/s2/favicons?domain=" + feed.link);

            ////DEFAULT IMAGE STUFF
            ///



            //TYPE author
            if(info.type == "author") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/contact.svg";
            }
            //

            //TYPE movie
            else if(info.type == "movie") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/stock_video.svg";
            }
            //

            //TYPE game
            else if(info.type == "game") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/media-playback-start.svg.svg";
            }
            //

            //TYPE book
            else if(info.type == "book") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/stock_note.svg";
            }
            //

            //TYPE show
            else if(info.type == "show") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/camcorder.svg";
            }
            //

            //TYPE music
            else if(info.type == "music") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/stock_music.svg";
            }
            //

            else {
                res.set_art("blah");
            }

            if (info.yID.empty()) {

                //TYPE author
                if(info.type == "author") {
                res.set_art("/usr/share/icons/suru/actions/scalable/contact.svg");
                }
                //

                //TYPE movie
                else if(info.type == "movie") {
                res.set_art("/usr/share/icons/suru/actions/scalable/stock_video.svg");
                }
                //

                //TYPE game
                else if(info.type == "game") {
                res.set_art("/usr/share/icons/suru/actions/scalable/media-playback-start.svg");
                }
                //

                //TYPE book
                else if(info.type == "book") {
                res.set_art("/usr/share/icons/suru/actions/scalable/stock_note.svg");
                }
                //

                //TYPE show
                else if(info.type == "show") {
                res.set_art("/usr/share/icons/suru/actions/scalable/camcorder.svg");
                }
                //

                //TYPE music
                else if(info.type == "music") {
                res.set_art("/usr/share/icons/suru/actions/scalable/stock_music.svg");
                }
                //

                else {
                    res.set_art("blah");
                }
            } else {
                res.set_art("http://img.youtube.com/vi/" + info.yID + "/hqdefault.jpg");
                res["youtube"] = "http://www.google.com/s2/favicons?domain=www.youtube.com";
            }
            ///

            res["artist"] = info.type;
            res["description"] = info.wTeaser;

            // Push the result
            if (!reply->push(res)) {
                // If we fail to push, it means the query has been cancelled.
                // So don't continue;
               return;
            }
        }
        ///

        ///
        /// \brief resultslist
        ///
        Client::ResultRes resultslist;
        if (query_string.empty()) {
            // If the string is empty, provide a specific one
            resultslist = client_.results("ratatat");
        } else {
            // otherwise, use the query string
            resultslist = client_.results(query_string);
        }

        // Register a category for infos
        auto results_cat = reply->register_category("results", "Recomendations", "",
            sc::CategoryRenderer(RESULTS_TEMPLATE));
        // register_category(arbitrary category id, header title, header icon, template)
        // In this case, since this is the only category used by our scope,
        // it doesn’t need to display a header title, we leave it as a blank string.

        for (const auto &result : resultslist.results) {
        // Iterate over the trackslist
            sc::CategorisedResult res(results_cat);

            // We must have a URI
            res.set_uri(result.wUrl);
            // We also need the namename
            res.set_title(result.name);

            // Set the rest of the attributes, art, artist, etc
            //res.set_art(track.artwork_url);

            ////DEFAULT IMAGE STUFF
            ///

            //TYPE author
            if(result.type == "author") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/contact.svg";
            }
            //

            //TYPE movie
            else if(result.type == "movie") {
            res["icon"] = "/usr/share/icons/suru/actions/scalable/stock_video.svg";
            }
            //

            //TYPE game
            else if(result.type == "game") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/media-playback-start.svg.svg";
            }
            //

            //TYPE book
            else if(result.type == "book") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/stock_note.svg";
            }
            //

            //TYPE show
            else if(result.type == "show") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/camcorder.svg";
            }
            //

            //TYPE music
            else if(result.type == "music") {
                res["icon"] = "/usr/share/icons/suru/actions/scalable/stock_music.svg";
            }
            //

            else {
                res.set_art("blah");
            }

            ///

            if (result.yID.empty()) {

                //TYPE author
                if(result.type == "author") {
                res.set_art("/usr/share/icons/suru/actions/scalable/contact.svg");
                }
                //

                //TYPE movie
                else if(result.type == "movie") {
                res.set_art("/usr/share/icons/suru/actions/scalable/stock_video.svg");
                }
                //

                //TYPE game
                else if(result.type == "game") {
                res.set_art("/usr/share/icons/suru/actions/scalable/media-playback-start.svg");
                }
                //

                //TYPE book
                else if(result.type == "book") {
                res.set_art("/usr/share/icons/suru/actions/scalable/stock_note.svg");
                }
                //

                //TYPE show
                else if(result.type == "show") {
                res.set_art("/usr/share/icons/suru/actions/scalable/camcorder.svg");
                }
                //

                //TYPE music
                else if(result.type == "music") {
                res.set_art("/usr/share/icons/suru/actions/scalable/stock_music.svg");
                }
                //

                else {
                    res.set_art("blah");
                }


            } else {
                res.set_art("http://img.youtube.com/vi/" + result.yID + "/mqdefault.jpg");
                res["youtube"] = "http://www.google.com/s2/favicons?domain=www.youtube.com";

            }
            ///

            res["artist"] = result.type;
            res["description"] = result.wTeaser;


            // Push the result
            if (!reply->push(res)) {
                //sleep(2);
                // If we fail to push, it means the query has been cancelled.
                // So don't continue;
                return;
            }
        }
        ///



    } catch (domain_error &e) {
        // Handle exceptions being thrown by the client API
        cerr << e.what() << endl;
        reply->error(current_exception());
    }
}

