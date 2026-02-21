#include "TCA9548A.h"
#include <cassert>
#include <cstdio>

int main() {
    TCA9548A mux(0x71);
    assert(mux.getAddress() == 0x71);
    assert(mux.begin());
    assert(mux.isInitialized());

    // Initially no channel
    assert(mux.getChannel() == -1);
    assert(mux.getChannelMask() == 0);

    // Select channel
    mux.selectChannel(3);
    assert(mux.getChannel() == 3);
    assert(mux.isChannelActive(3));
    assert(!mux.isChannelActive(0));

    // Switch channel
    mux.selectChannel(7);
    assert(mux.getChannel() == 7);
    assert(!mux.isChannelActive(3));

    // Multi-channel mask
    mux.setChannelMask(0x05); // channels 0 and 2
    assert(mux.isChannelActive(0));
    assert(mux.isChannelActive(2));
    assert(!mux.isChannelActive(1));
    assert(mux.getChannel() == -1); // multiple channels active

    // Close all
    mux.closeAll();
    assert(mux.getChannelMask() == 0);
    assert(mux.getChannel() == -1);

    // Default address
    TCA9548A mux2;
    assert(mux2.getAddress() == 0x70);

    printf("test_tca9548a: all assertions passed\n");
    return 0;
}
