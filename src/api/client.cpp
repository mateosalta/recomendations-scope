#include <api/client.h>

#include <core/net/error.h>
#include <core/net/http/client.h>
#include <core/net/http/content_type.h>
#include <core/net/http/response.h>
#include <QVariantMap>
#include <iostream>
//#include <unistd.h>

namespace http = core::net::http;
namespace net = core::net;

using namespace api;
using namespace std;

Client::Client(Config::Ptr config) :
    config_(config), cancelled_(false) {
}

void Client::get(
        const net::Uri::Path &path,
                 const net::Uri::QueryParameters &parameters, QJsonDocument &root) {
    // Create a new HTTP client
    auto client = http::make_client();

    // Start building the request configuration
    http::Request::Configuration configuration;

    // Build the URI from its components
    net::Uri uri = net::make_uri(config_->apiroot, path, parameters);
    configuration.uri = client->uri_to_string(uri);

    // Give out a user agent string
    configuration.header.add("User-Agent", config_->user_agent);

    // Build a HTTP request object from our configuration
    auto request = client->head(configuration);
//sleep(2);
    try {
        // Synchronously make the HTTP request
        // We bind the cancellable callback to #progress_report
        auto response = request->execute(
                    bind(&Client::progress_report, this, placeholders::_1));

        // Check that we got a sensible HTTP status code
        if (response.status != http::Status::ok) {
            throw domain_error(response.body);
        }
        // Parse the JSON from the response
        root = QJsonDocument::fromJson(response.body.c_str());
    } catch (net::Error &) {
    }
}

///
/// \brief Client::infos
/// \param query
/// \return
///
Client::InfoRes Client::infos(const string& query) {
   // sleep(2);
    QJsonDocument root;

    // Build a URI and get the contents.
    // The fist parameter forms the path part of the URI.
    // The second parameter forms the CGI parameters.
    get( { "api", "similar"}, { { "k", "147022-TasteKid-3H5727D0" }, { "q", query }, { "info", "1"} }, root);
    // https://www.tastekid.com/api/similar?k=147022-TasteKid-3H5727D0&q=ratatat

    // My “list of info” object (as seen in the corresponding header file)
    InfoRes result;

    QVariantMap variant = root.toVariant().toMap();
    QVariantMap Similar = variant["Similar"].toMap();
    for (const QVariant &i : Similar["Info"].toList()) {
        QVariantMap item = i.toMap();
        //cout << item["Name"].toString().toStdString();
        // We add each result to our list
        result.infos.emplace_back(
            Info {
                item["Name"].toString().toStdString(),
                item["Type"].toString().toStdString(),
                item["wTeaser"].toString().toStdString(),
                item["wUrl"].toString().toStdString(),
                item["yUrl"].toString().toStdString(),
                item["yID"].toString().toStdString(),
            }

        );
    }
    return result;
}
///

///
/// \brief Client::results
/// \param query
/// \return
///
Client::ResultRes Client::results(const string& query) {
    QJsonDocument root;

    // Build a URI and get the contents.
    // The fist parameter forms the path part of the URI.
    // The second parameter forms the CGI parameters.
    get( { "api", "similar"}, { { "k", "147022-TasteKid-3H5727D0" }, { "q", query }, { "info", "1" } }, root);
    // https://www.tastekid.com/api/similar?k=147022-TasteKid-3H5727D0&q=ratatat

    // My “list of results" object (as seen in the corresponding header file)
    ResultRes result;

    QVariantMap variant = root.toVariant().toMap();
    QVariantMap Similar = variant["Similar"].toMap();
    for (const QVariant &i : Similar["Results"].toList()) {
        QVariantMap item = i.toMap();
        result.results.emplace_back(
            Result {
                item["Name"].toString().toStdString(),
                item["Type"].toString().toStdString(),
                item["wTeaser"].toString().toStdString(),
                item["wUrl"].toString().toStdString(),
                item["yUrl"].toString().toStdString(),
                item["yID"].toString().toStdString(),
                    });}


    return result;
}
///

http::Request::Progress::Next Client::progress_report(
        const http::Request::Progress&) {

    return cancelled_ ?
                http::Request::Progress::Next::abort_operation :
                http::Request::Progress::Next::continue_operation;
}

void Client::cancel() {
    cancelled_ = true;
}

Config::Ptr Client::config() {
    return config_;
}

