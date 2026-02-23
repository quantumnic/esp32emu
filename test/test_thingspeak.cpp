// test_thingspeak: ThingSpeak IoT cloud client mock tests
#include "ThingSpeak.h"
#include <cassert>
#include <cstdio>

int main() {
    ThingSpeak.reset();
    assert(!ThingSpeak.isBegun());

    // Begin
    int dummy_client = 0;
    ThingSpeak.begin(&dummy_client);
    assert(ThingSpeak.isBegun());

    // Set fields
    assert(ThingSpeak.setField(1, 25.5f) == TS_OK_SUCCESS);
    assert(ThingSpeak.setField(2, "hello") == TS_OK_SUCCESS);
    assert(ThingSpeak.setField(3, 42) == TS_OK_SUCCESS);

    // Invalid field number
    assert(ThingSpeak.setField(0, 1) == TS_ERR_INVALID_FIELD_NUM);
    assert(ThingSpeak.setField(9, 1) == TS_ERR_INVALID_FIELD_NUM);

    // Write fields
    assert(ThingSpeak.writeFields(12345, "MYAPIKEY") == TS_OK_SUCCESS);
    assert(ThingSpeak.getWriteCount() == 1);
    assert(ThingSpeak.getChannel() == 12345);
    assert(ThingSpeak.getWriteKey() == "MYAPIKEY");

    // Last write fields should have 3 entries
    auto& lw = ThingSpeak.getLastWriteFields();
    assert(lw.size() == 3);
    assert(lw.at(3) == "42");

    // Fields cleared after write
    ThingSpeak.writeFields(12345, "MYAPIKEY");
    assert(ThingSpeak.getWriteCount() == 2);
    assert(ThingSpeak.getLastWriteFields().empty());

    // Write single field
    ThingSpeak.writeField(99999, 1, 3.14f, "KEY2");
    assert(ThingSpeak.getWriteCount() == 3);

    // Read with injected values
    ThingSpeak.injectReadValue(1, "98.6");
    ThingSpeak.injectReadValue(2, "hello world");

    float val = ThingSpeak.readFloatField(12345, 1, "READKEY");
    assert(val > 98.5f && val < 98.7f);
    assert(ThingSpeak.getReadCount() == 1);

    std::string sval = ThingSpeak.readStringField(12345, 2);
    assert(sval == "hello world");

    long lval = ThingSpeak.readLongField(12345, 1);
    assert(lval == 98);

    // Inject error status
    ThingSpeak.injectWriteStatus(TS_ERR_CONNECT_FAILED);
    assert(ThingSpeak.writeField(12345, 1, 0, "KEY") == TS_ERR_CONNECT_FAILED);

    // Latitude/longitude/status setters
    assert(ThingSpeak.setLatitude(47.3f) == TS_OK_SUCCESS);
    assert(ThingSpeak.setLongitude(8.5f) == TS_OK_SUCCESS);
    assert(ThingSpeak.setElevation(408.0f) == TS_OK_SUCCESS);
    assert(ThingSpeak.setStatus("online") == TS_OK_SUCCESS);

    printf("test_thingspeak: all assertions passed\n");
    return 0;
}
