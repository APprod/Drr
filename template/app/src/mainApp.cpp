#include "mainApp.hpp"
std::unique_ptr<IApp> createApp(){
    return std::make_unique<App>();
}
