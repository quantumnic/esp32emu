#include <Arduino.h>
#include <esp_vfs.h>

static int my_open(const char* path, int flags, int mode) {
    (void)flags; (void)mode;
    Serial.print("VFS open: ");
    Serial.println(path);
    return 1;
}

static int my_close(int fd) {
    (void)fd;
    Serial.println("VFS close");
    return 0;
}

void setup() {
    Serial.begin(115200);

    esp_vfs_t vfs = {};
    vfs.open = my_open;
    vfs.close = my_close;

    esp_err_t err = esp_vfs_register("/myfs", &vfs, nullptr);
    if (err == ESP_OK) {
        Serial.println("Custom VFS registered at /myfs");
    }

    Serial.print("VFS entries: ");
    Serial.println((int)esp32emu::vfs::count());

    esp_vfs_unregister("/myfs");
    Serial.println("VFS unregistered");
}

void loop() {
    delay(1000);
}
