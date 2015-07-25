#ifndef API_CLIENT_H_
#define API_CLIENT_H_

#include <api/config.h>

#include <atomic>
#include <deque>
#include <map>
#include <string>
#include <core/net/http/request.h>
#include <core/net/uri.h>

#include <QJsonDocument>

namespace api {

/**
 * Provide a nice way to access the HTTP API.
 *
 * We don't want our scope's code to be mixed together with HTTP and JSON handling.
 */
class Client {
    public:


    /**
    * Info
    */
    struct Info {
        //unsigned int id;
        std::string name;
        std::string type;
        std::string wTeaser;
        std::string wUrl;
        std::string yUrl;
        std::string yID;
    };

    struct Result {
            //unsigned int id;
        std::string name;
        std::string type;
        std::string wTeaser;
        std::string wUrl;
        std::string yUrl;
        std::string yID;
    };

    /**
    * A list of objects.
    */
    typedef std::deque<Info> InfoList;
    typedef std::deque<Result> ResultList;


    /**
    *  results.
    */
    struct InfoRes {
        InfoList infos;
    };
    struct ResultRes {
        ResultList results;
    };

    Client(Config::Ptr config);

    virtual ~Client() = default;

    /**
     * Get the list for a query
     */
    virtual InfoRes infos(const std::string &query);
    virtual ResultRes results(const std::string &query);

    /**
     * Cancel any pending queries (this method can be called from a different thread)
     */
    virtual void cancel();

    virtual Config::Ptr config();

protected:
    void get(const core::net::Uri::Path &path,
             const core::net::Uri::QueryParameters &parameters,
             QJsonDocument &root);
    /**
     * Progress callback that allows the query to cancel pending HTTP requests.
     */
    core::net::http::Request::Progress::Next progress_report(
            const core::net::http::Request::Progress& progress);

    /**
     * Hang onto the configuration information
     */
    Config::Ptr config_;

    /**
     * Thread-safe cancelled flag
     */
    std::atomic<bool> cancelled_;
};

}

#endif // API_CLIENT_H_

