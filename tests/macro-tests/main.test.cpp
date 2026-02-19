//
// Created by nexie on 17.02.2026.
//

// #define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    Catch::Session session;

    auto& cfg = session.configData();
    // cfg.showSuccessfulTests = true;
    cfg.verbosity = Catch::Verbosity::High;

    // cfg.showSuccessfulTests = true;
    // cfg.shouldDebugBreak = true;
    cfg.showInvisibles = true;
    // cfg.filenamesAsTags = true;
    // cfg.allowZeroTests = true;

    return session.run(argc, argv);
}