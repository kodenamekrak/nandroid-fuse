#include "assets.hpp"

extern uint8_t* _binary_nandroid_daemon_start;
extern uint8_t* _binary_nandroid_daemon_end;

uint8_t DAEMON_BYTES[] = {
    #embed "../build/nandroid-daemon"
};
constexpr std::span<uint8_t> DAEMON_BYTES_SPAN(DAEMON_BYTES, sizeof(DAEMON_BYTES));

namespace nandroid::assets
{
    std::span<uint8_t> get_daemon_bytes()
    {
        return DAEMON_BYTES_SPAN;
    }
}