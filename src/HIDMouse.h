#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/BatteryService.h"
#include "events/mbed_events.h"
#include "BLE_HID/mouse.h"

static events::EventQueue event_queue(/* event count */ 16 * EVENTS_EVENT_SIZE);


class HIDMouse : public ble::Gap::EventHandler,
                 public SecurityManager::EventHandler,
                 public BLEMouse{


public:
    BLE &_ble;
    HIDMouse(BLE &ble = BLE::Instance());
    void begin();

    void click(uint8_t b = MOUSE_BUTTON_LEFT);
    void move(signed char x, signed char y, signed char wheel = 0);
    void press(uint8_t b = MOUSE_BUTTON_LEFT);
    void release(uint8_t b = MOUSE_BUTTON_LEFT);
    bool isPressed(uint8_t b = MOUSE_BUTTON_LEFT);

    bool isConnected(void);
    void setManufacturerName(const char *manufacturersName2);
    void setDeviceName(const char *device_name2);
    void setBatteryLevel(uint8_t _battery_level);

private:

    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context);
    events::EventQueue &_event_queue = event_queue;

    uint8_t _battery_level;
    const char *manufacturersName;
    const char *device_name;

    UUID _battery_uuid;
    BatteryService _battery_service;
	
    bool batteryService;
    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
    ble::connection_handle_t _handle;
    bool ifconnected;

private:
    void start(void);
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params);
    void start_advertising();
    virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent&);
    virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event);
    virtual void pairingResult(
        ble::connection_handle_t connectionHandle,
        SecurityManager::SecurityCompletionStatus_t result
    );
	int _dispatch_event;
    virtual void pairingRequest(
        ble::connection_handle_t connectionHandle
    );

};
