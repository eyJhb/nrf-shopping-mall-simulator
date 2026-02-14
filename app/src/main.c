#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>

#ifndef IBEACON_RSSI
#define IBEACON_RSSI 0xc8
#endif

#define IBEACON_SWITCH_TIME 5 * 1000

struct shopping_mall {
    uint8_t major1, major2;
};

struct shopping_mall shopping_malls[] = {
    {0x00, 0x40}, // shopping mall 1
    {0x00, 0xa8}, // shopping mall 2
    {0x00, 0x8a}, // shopping mall 3
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");
	printk("Starting advertising iBeacon\n");

    for (int i = 0;; i++) {
        // select the correct shopping mall that we want to advertise
        uint8_t major1 = shopping_malls[i % sizeof(shopping_malls)/sizeof(shopping_malls[0])].major1;
        uint8_t major2 = shopping_malls[i % sizeof(shopping_malls)/sizeof(shopping_malls[0])].major2;

        printk("Advertising major1: %x \t major2: %x\n", major1, major2);

        /*
        * iBeacon Shopping Malls
        *
        * UUID:  F7826DA6-4FA2-4E98-8024-BC5B71E0893E
        * Major: taken from `shopping_malls`
        * Minor: 0 
        * RSSI:  -56 dBm
        */
        struct bt_data ad[] = {
            BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
            BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA,
                          0x4c, 0x00, /* Apple */
                          0x02, 0x15, /* iBeacon */
                          0xf7, 0x82, 0x6d, 0xa6, /* UUID[15..12] */
                          0x4f, 0xa2, /* UUID[11..10] */
                          0x4e, 0x98, /* UUID[9..8] */
                          0x80, 0x24, /* UUID[7..6] */
                          0xbc, 0x5b, 0x71, 0xe0, 0x89, 0x3e, /* UUID[5..0] */
                          major1, major2, /* Major STORCENTER */
                          0x00, 0x00, /* Minor */
                          IBEACON_RSSI) /* Calibrated RSSI @ 1m */
        };

        // start advertising
        err = bt_le_adv_start(BT_LE_ADV_NCONN, ad, ARRAY_SIZE(ad), NULL, 0);
        if (err) {
            printk("Advertising failed to start (err %d)\n", err);
            return;
        }
  
        // sleep before next advertising
        k_msleep(IBEACON_SWITCH_TIME);
  
        // stop advertising
        bt_le_adv_stop();
    }

}

int main(void)
{
	int err;

	printk("Starting iBeacon Demo\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
	return 0;
}

