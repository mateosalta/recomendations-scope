#include <scope/preview.h>

#include <unity/scopes/ColumnLayout.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/PreviewReply.h>
#include <unity/scopes/Result.h>
#include <unity/scopes/VariantBuilder.h>

#include <iostream>

namespace sc = unity::scopes;

using namespace std;
using namespace scope;

Preview::Preview(const sc::Result &result, const sc::ActionMetadata &metadata) :
    sc::PreviewQueryBase(result, metadata) {
}

void Preview::cancelled() {
}

void Preview::run(sc::PreviewReplyProxy const& reply) {
    sc::Result result = PreviewQueryBase::result();

    // Support three different column layouts
    sc::ColumnLayout layout1col(1), layout2col(2);

    // Single column layout
    layout1col.add_column( { "imageId", "headerId", "actionsId", "summary"});

    // Two column layout
    layout2col.add_column( { "imageId" });
    layout2col.add_column( { "headerId", "actionsId", "summary" });

    // Register the layouts we just created
    reply->register_layout( { layout1col, layout2col });

    // Define the header section
    sc::PreviewWidget w_header("headerId", "header");
    // It has title and a subtitle properties
    w_header.add_attribute_mapping("title", "title");
    w_header.add_attribute_mapping("subtitle", "artist");

    // Define the image section
    sc::PreviewWidget w_art("imageId", "image");
    // It has a single source property, mapped to the result's art property
    w_art.add_attribute_mapping("source", "art");
    w_art.add_attribute_value("zoomable", unity::scopes::Variant(true));

    sc::PreviewWidget description("summary", "text");
        description.add_attribute_mapping("text", "description");
    // Define the actions section
    sc::PreviewWidget w_actions("actionsId", "actions");
    sc::VariantBuilder builder;
    builder.add_tuple({
        {"id", sc::Variant("open")},
        {"label", sc::Variant("Wikipedia")},
        {"uri", result["uri"]}
    });
    w_actions.add_attribute_value("actions", builder.end());

    // Push each of the sections
    reply->push( { w_art, w_header, description, w_actions });
}
