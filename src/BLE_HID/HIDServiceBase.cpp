#include "mbed.h"
#include "HIDServiceBase.h"

HIDServiceBase::HIDServiceBase(BLE          &_ble,
                               report_map_t reportMap,
                               uint8_t      reportMapSize,
                               report_t     inputReport,
                               report_t     outputReport,
                               report_t     featureReport,
                               uint8_t      inputReportLength,
                               uint8_t      outputReportLength,
                               uint8_t      featureReportLength) :
    ble(_ble),
    reportMapLength(reportMapSize),

    inputReport(inputReport),
    outputReport(outputReport),
    featureReport(featureReport),

    inputReportLength(inputReportLength),
    outputReportLength(outputReportLength),
    featureReportLength(featureReportLength),

    protocolMode(REPORT_PROTOCOL),

    inputReportReferenceDescriptor(BLE_UUID_DESCRIPTOR_REPORT_REFERENCE,
            (uint8_t *)&inputReportReferenceData, 2, 2),
    outputReportReferenceDescriptor(BLE_UUID_DESCRIPTOR_REPORT_REFERENCE,
            (uint8_t *)&outputReportReferenceData, 2, 2),
    featureReportReferenceDescriptor(BLE_UUID_DESCRIPTOR_REPORT_REFERENCE,
            (uint8_t *)&featureReportReferenceData, 2, 2),

    protocolModeCharacteristic(GattCharacteristic::UUID_PROTOCOL_MODE_CHAR, &protocolMode, 1, 1,
              GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE),

    inputReportCharacteristic(GattCharacteristic::UUID_REPORT_CHAR,
            (uint8_t *)inputReport, inputReportLength, inputReportLength,
              GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
            inputReportDescriptors(), 1),

    outputReportCharacteristic(GattCharacteristic::UUID_REPORT_CHAR,
            (uint8_t *)outputReport, outputReportLength, outputReportLength,
              GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
            outputReportDescriptors(), 1),

    featureReportCharacteristic(GattCharacteristic::UUID_REPORT_CHAR,
            (uint8_t *)featureReport, featureReportLength, featureReportLength,
              GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ
            | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE,
            featureReportDescriptors(), 1),

    /*
     * We need to set reportMap content as const, in order to let the compiler put it into flash
     * instead of RAM. The characteristic is read-only so it won't be written, but
     * GattCharacteristic constructor takes non-const arguments only. Hence the cast.
     */
    reportMapCharacteristic(GattCharacteristic::UUID_REPORT_MAP_CHAR,
            const_cast<uint8_t*>(reportMap), reportMapLength, reportMapLength,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ),

    HIDInformationCharacteristic(GattCharacteristic::UUID_HID_INFORMATION_CHAR, HIDInformation()),
    HIDControlPointCharacteristic(GattCharacteristic::UUID_HID_CONTROL_POINT_CHAR,
            &controlPointCommand, 1, 1,
            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE)

{
    static GattCharacteristic *characteristics[] = {
        &HIDInformationCharacteristic,
        &reportMapCharacteristic,
        &protocolModeCharacteristic,
        &HIDControlPointCharacteristic,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    };

    unsigned int charIndex = 4;
    /*
     * Report characteristics are optional, and depend on the reportMap descriptor
     * Note: at least one should be present, but we don't check that at the moment.
     */
    if (inputReportLength)
        characteristics[charIndex++] = &inputReportCharacteristic;
    if (outputReportLength)
        characteristics[charIndex++] = &outputReportCharacteristic;
    if (featureReportLength)
        characteristics[charIndex++] = &featureReportCharacteristic;

    /* TODO: let children add some more characteristics, namely boot keyboard and mouse (They are
     * mandatory as per HIDS spec.) Ex:
     *
     * addExtraCharacteristics(characteristics, int& charIndex);
     */

    GattService service(GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE,
                        characteristics, charIndex);

    ble.gattServer().addService(service);

    /*
     * Change preferred connection params, in order to optimize the notification frequency. Most
     * OSes seem to respect this, even though they are not required to.
     *
     * Some OSes don't handle reconnection well, at the moment, so we set the maximum possible
     * timeout, 32 seconds
     */

    /* 
    uint16_t minInterval = Gap::MSEC_TO_GAP_DURATION_UNITS(reportTickerDelay / 2);
    if (minInterval < 6)
        minInterval = 6;
    uint16_t maxInterval = minInterval * 2;
    Gap::ConnectionParams_t params = {minInterval, maxInterval, 0, 3200};

    ble.gap().setPreferredConnectionParams(&params);
    */


    //protocolModeCharacteristic.setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    //reportMapCharacteristic.setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    //inputReportCharacteristic.setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    //outputReportCharacteristic.setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
    //featureReportCharacteristic.setReadSecurityRequirement(ble::att_security_requirement_t::UNAUTHENTICATED);
}


GattAttribute** HIDServiceBase::inputReportDescriptors() {
    inputReportReferenceData.ID = 0;
    inputReportReferenceData.type = INPUT_REPORT;

    static GattAttribute * descs[] = {
        &inputReportReferenceDescriptor,
    };
    return descs;
}

GattAttribute** HIDServiceBase::outputReportDescriptors() {
    outputReportReferenceData.ID = 0;
    outputReportReferenceData.type = OUTPUT_REPORT;

    static GattAttribute * descs[] = {
        &outputReportReferenceDescriptor,
    };
    return descs;
}

GattAttribute** HIDServiceBase::featureReportDescriptors() {
    featureReportReferenceData.ID = 0;
    featureReportReferenceData.type = FEATURE_REPORT;

    static GattAttribute * descs[] = {
        &featureReportReferenceDescriptor,
    };
    return descs;
}


HID_information_t* HIDServiceBase::HIDInformation() {
    static HID_information_t info = {HID_VERSION_1_11, 0x00, 0x03};
    //printf("read hid information\n");

    return &info;
}

ble_error_t HIDServiceBase::send(const report_t report) {
    return ble.gattServer().write(inputReportCharacteristic.getValueHandle(),
                                  report,
                                  inputReportLength);
}

ble_error_t HIDServiceBase::read(report_t report) {
    // TODO. For the time being, we'll just have HID input reports...
    //printf("read not implemented\n");
    return BLE_ERROR_NOT_IMPLEMENTED;
}


