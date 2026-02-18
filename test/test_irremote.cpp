#include "IRremote.h"
#include <cassert>
#include <cstdio>

int main() {
    // Receiver
    IRrecv recv(11);
    assert(recv.getPin() == 11);
    assert(!recv.isEnabled());

    recv.enableIRIn();
    assert(recv.isEnabled());

    // No data yet
    decode_results res;
    assert(recv.decode(&res) == 0);

    // Inject codes
    recv.injectCode(NEC, 0xFF00FF00, 32);
    recv.injectCode(SONY, 0x00A90, 12);
    assert(recv.pendingCount() == 2);

    // Decode first
    assert(recv.decode(&res) == 1);
    assert(res.decode_type == NEC);
    assert(res.value == 0xFF00FF00);
    assert(res.bits == 32);
    recv.resume();

    // Decode second
    assert(recv.decode(&res) == 1);
    assert(res.decode_type == SONY);
    assert(res.value == 0x00A90);
    recv.resume();

    // Queue empty
    assert(recv.decode(&res) == 0);

    // Sender
    IRsend sender(3);
    sender.begin();
    assert(sender.isActive());

    sender.sendNEC(0xABCD1234);
    sender.sendSony(0x123, 12);
    sender.sendSamsung(0xDEADBEEF);

    assert(sender.getSentCount() == 3);
    assert(sender.getSentLog()[0].decode_type == NEC);
    assert(sender.getSentLog()[0].value == 0xABCD1234);
    assert(sender.getSentLog()[1].decode_type == SONY);
    assert(sender.getSentLog()[2].decode_type == SAMSUNG);

    // Raw send
    uint16_t raw[] = {9000, 4500, 560, 560};
    sender.sendRaw(raw, 4, 38000);
    assert(sender.getRawCount() == 1);

    sender.clearLog();
    assert(sender.getSentCount() == 0);

    printf("test_irremote: all assertions passed\n");
    return 0;
}
