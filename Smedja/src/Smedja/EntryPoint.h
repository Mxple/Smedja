#pragma once

#include "Application.h"
#include "Log.h"

extern Smedja::Application *Smedja::createApplication();

int main(int argc, char **argv) {
    Smedja::Log::init();
    SD_CORE_WARN("Initialized Log!");
    int a = 5;

    auto app = Smedja::createApplication();
    app->run();
    delete app;
}
